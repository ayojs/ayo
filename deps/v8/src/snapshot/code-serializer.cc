// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/snapshot/code-serializer.h"

#include <memory>

#include "src/code-stubs.h"
#include "src/counters.h"
#include "src/log.h"
#include "src/macro-assembler.h"
#include "src/objects-inl.h"
#include "src/snapshot/object-deserializer.h"
#include "src/snapshot/snapshot.h"
#include "src/version.h"
#include "src/visitors.h"
#include "src/wasm/wasm-module.h"
#include "src/wasm/wasm-objects.h"

namespace v8 {
namespace internal {

ScriptData* CodeSerializer::Serialize(Isolate* isolate,
                                      Handle<SharedFunctionInfo> info,
                                      Handle<String> source) {
  base::ElapsedTimer timer;
  if (FLAG_profile_deserialization) timer.Start();
  if (FLAG_trace_serializer) {
    PrintF("[Serializing from");
    Object* script = info->script();
    if (script->IsScript()) Script::cast(script)->name()->ShortPrint();
    PrintF("]\n");
  }

  // Serialize code object.
  CodeSerializer cs(isolate, SerializedCodeData::SourceHash(source));
  DisallowHeapAllocation no_gc;
  cs.reference_map()->AddAttachedReference(*source);
  ScriptData* ret = cs.Serialize(info);

  if (FLAG_profile_deserialization) {
    double ms = timer.Elapsed().InMillisecondsF();
    int length = ret->length();
    PrintF("[Serializing to %d bytes took %0.3f ms]\n", length, ms);
  }

  return ret;
}

ScriptData* CodeSerializer::Serialize(Handle<HeapObject> obj) {
  DisallowHeapAllocation no_gc;

  VisitRootPointer(Root::kHandleScope, Handle<Object>::cast(obj).location());
  SerializeDeferredObjects();
  Pad();

  SerializedCodeData data(sink()->data(), this);

  return data.GetScriptData();
}

void CodeSerializer::SerializeObject(HeapObject* obj, HowToCode how_to_code,
                                     WhereToPoint where_to_point, int skip) {
  if (SerializeHotObject(obj, how_to_code, where_to_point, skip)) return;

  int root_index = root_index_map_.Lookup(obj);
  if (root_index != RootIndexMap::kInvalidRootIndex) {
    PutRoot(root_index, obj, how_to_code, where_to_point, skip);
    return;
  }

  if (SerializeBackReference(obj, how_to_code, where_to_point, skip)) return;

  FlushSkip(skip);

  if (obj->IsCode()) {
    Code* code_object = Code::cast(obj);
    switch (code_object->kind()) {
      case Code::OPTIMIZED_FUNCTION:  // No optimized code compiled yet.
      case Code::HANDLER:             // No handlers patched in yet.
      case Code::REGEXP:              // No regexp literals initialized yet.
      case Code::NUMBER_OF_KINDS:     // Pseudo enum value.
      case Code::BYTECODE_HANDLER:    // No direct references to handlers.
        CHECK(false);
      case Code::BUILTIN:
        SerializeBuiltin(code_object->builtin_index(), how_to_code,
                         where_to_point);
        return;
      case Code::STUB:
#define IC_KIND_CASE(KIND) case Code::KIND:
        IC_KIND_LIST(IC_KIND_CASE)
#undef IC_KIND_CASE
        if (code_object->builtin_index() == -1) {
          SerializeCodeStub(code_object, how_to_code, where_to_point);
        } else {
          SerializeBuiltin(code_object->builtin_index(), how_to_code,
                           where_to_point);
        }
        return;
      case Code::FUNCTION:
        DCHECK(code_object->has_reloc_info_for_serialization());
        SerializeGeneric(code_object, how_to_code, where_to_point);
        return;
      default:
        return SerializeCodeObject(code_object, how_to_code, where_to_point);
    }
    UNREACHABLE();
  }

  if (ElideObject(obj)) {
    return SerializeObject(isolate()->heap()->undefined_value(), how_to_code,
                           where_to_point, skip);
  }

  if (obj->IsScript()) {
    // Wrapper object is a context-dependent JSValue. Reset it here.
    Script::cast(obj)->set_wrapper(isolate()->heap()->undefined_value());
  }

  // Past this point we should not see any (context-specific) maps anymore.
  CHECK(!obj->IsMap());
  // There should be no references to the global object embedded.
  CHECK(!obj->IsJSGlobalProxy() && !obj->IsJSGlobalObject());
  // There should be no hash table embedded. They would require rehashing.
  CHECK(!obj->IsHashTable());
  // We expect no instantiated function objects or contexts.
  CHECK(!obj->IsJSFunction() && !obj->IsContext());

  SerializeGeneric(obj, how_to_code, where_to_point);
}

void CodeSerializer::SerializeGeneric(HeapObject* heap_object,
                                      HowToCode how_to_code,
                                      WhereToPoint where_to_point) {
  // Object has not yet been serialized.  Serialize it here.
  ObjectSerializer serializer(this, heap_object, &sink_, how_to_code,
                              where_to_point);
  serializer.Serialize();
}

void CodeSerializer::SerializeBuiltin(int builtin_index, HowToCode how_to_code,
                                      WhereToPoint where_to_point) {
  DCHECK((how_to_code == kPlain && where_to_point == kStartOfObject) ||
         (how_to_code == kFromCode && where_to_point == kInnerPointer));
  DCHECK_LT(builtin_index, Builtins::builtin_count);
  DCHECK_LE(0, builtin_index);

  if (FLAG_trace_serializer) {
    PrintF(" Encoding builtin: %s\n",
           isolate()->builtins()->name(builtin_index));
  }

  sink_.Put(kBuiltin + how_to_code + where_to_point, "Builtin");
  sink_.PutInt(builtin_index, "builtin_index");
}

void CodeSerializer::SerializeCodeStub(Code* code_stub, HowToCode how_to_code,
                                       WhereToPoint where_to_point) {
  // We only arrive here if we have not encountered this code stub before.
  DCHECK(!reference_map()->Lookup(code_stub).is_valid());
  uint32_t stub_key = code_stub->stub_key();
  DCHECK(CodeStub::MajorKeyFromKey(stub_key) != CodeStub::NoCache);
  DCHECK(!CodeStub::GetCode(isolate(), stub_key).is_null());
  stub_keys_.push_back(stub_key);

  SerializerReference reference =
      reference_map()->AddAttachedReference(code_stub);
  if (FLAG_trace_serializer) {
    PrintF(" Encoding code stub %s as attached reference %d\n",
           CodeStub::MajorName(CodeStub::MajorKeyFromKey(stub_key)),
           reference.attached_reference_index());
  }
  PutAttachedReference(reference, how_to_code, where_to_point);
}

MaybeHandle<SharedFunctionInfo> CodeSerializer::Deserialize(
    Isolate* isolate, ScriptData* cached_data, Handle<String> source) {
  base::ElapsedTimer timer;
  if (FLAG_profile_deserialization) timer.Start();

  HandleScope scope(isolate);

  SerializedCodeData::SanityCheckResult sanity_check_result =
      SerializedCodeData::CHECK_SUCCESS;
  const SerializedCodeData scd = SerializedCodeData::FromCachedData(
      isolate, cached_data, SerializedCodeData::SourceHash(source),
      &sanity_check_result);
  if (sanity_check_result != SerializedCodeData::CHECK_SUCCESS) {
    if (FLAG_profile_deserialization) PrintF("[Cached code failed check]\n");
    DCHECK(cached_data->rejected());
    source->GetIsolate()->counters()->code_cache_reject_reason()->AddSample(
        sanity_check_result);
    return MaybeHandle<SharedFunctionInfo>();
  }

  // Deserialize.
  MaybeHandle<SharedFunctionInfo> maybe_result =
      ObjectDeserializer::DeserializeSharedFunctionInfo(isolate, &scd, source);

  Handle<SharedFunctionInfo> result;
  if (!maybe_result.ToHandle(&result)) {
    // Deserializing may fail if the reservations cannot be fulfilled.
    if (FLAG_profile_deserialization) PrintF("[Deserializing failed]\n");
    return MaybeHandle<SharedFunctionInfo>();
  }

  if (FLAG_profile_deserialization) {
    double ms = timer.Elapsed().InMillisecondsF();
    int length = cached_data->length();
    PrintF("[Deserializing from %d bytes took %0.3f ms]\n", length, ms);
  }
  result->set_deserialized(true);

  if (isolate->logger()->is_logging_code_events() || isolate->is_profiling()) {
    String* name = isolate->heap()->empty_string();
    if (result->script()->IsScript()) {
      Script* script = Script::cast(result->script());
      if (script->name()->IsString()) name = String::cast(script->name());
    }
    PROFILE(isolate, CodeCreateEvent(CodeEventListener::SCRIPT_TAG,
                                     result->abstract_code(), *result, name));
  }
  return scope.CloseAndEscape(result);
}

WasmCompiledModuleSerializer::WasmCompiledModuleSerializer(
    Isolate* isolate, uint32_t source_hash, Handle<Context> native_context,
    Handle<SeqOneByteString> module_bytes)
    : CodeSerializer(isolate, source_hash) {
  reference_map()->AddAttachedReference(*isolate->native_context());
  reference_map()->AddAttachedReference(*module_bytes);
}

std::unique_ptr<ScriptData> WasmCompiledModuleSerializer::SerializeWasmModule(
    Isolate* isolate, Handle<FixedArray> input) {
  Handle<WasmCompiledModule> compiled_module =
      Handle<WasmCompiledModule>::cast(input);
  WasmCompiledModuleSerializer wasm_cs(isolate, 0, isolate->native_context(),
                                       handle(compiled_module->module_bytes()));
  ScriptData* data = wasm_cs.Serialize(compiled_module);
  return std::unique_ptr<ScriptData>(data);
}

MaybeHandle<FixedArray> WasmCompiledModuleSerializer::DeserializeWasmModule(
    Isolate* isolate, ScriptData* data, Vector<const byte> wire_bytes) {
  MaybeHandle<FixedArray> nothing;
  if (!wasm::IsWasmCodegenAllowed(isolate, isolate->native_context())) {
    return nothing;
  }
  SerializedCodeData::SanityCheckResult sanity_check_result =
      SerializedCodeData::CHECK_SUCCESS;

  const SerializedCodeData scd = SerializedCodeData::FromCachedData(
      isolate, data, 0, &sanity_check_result);

  if (sanity_check_result != SerializedCodeData::CHECK_SUCCESS) {
    return nothing;
  }

  MaybeHandle<WasmCompiledModule> maybe_result =
      ObjectDeserializer::DeserializeWasmCompiledModule(isolate, &scd,
                                                        wire_bytes);

  Handle<WasmCompiledModule> result;
  if (!maybe_result.ToHandle(&result)) return nothing;

  WasmCompiledModule::ReinitializeAfterDeserialization(isolate, result);
  DCHECK(WasmCompiledModule::IsWasmCompiledModule(*result));
  return result;
}

void WasmCompiledModuleSerializer::SerializeCodeObject(
    Code* code_object, HowToCode how_to_code, WhereToPoint where_to_point) {
  Code::Kind kind = code_object->kind();
  switch (kind) {
    case Code::WASM_FUNCTION:
    case Code::JS_TO_WASM_FUNCTION:
      // Just serialize the code_object.
      SerializeGeneric(code_object, how_to_code, where_to_point);
      break;
    case Code::WASM_INTERPRETER_ENTRY:
    case Code::WASM_TO_JS_FUNCTION:
      // Serialize the illegal builtin instead. On instantiation of a
      // deserialized module, these will be replaced again.
      SerializeBuiltin(Builtins::kIllegal, how_to_code, where_to_point);
      break;
    default:
      UNREACHABLE();
  }
}

bool WasmCompiledModuleSerializer::ElideObject(Object* obj) {
  return obj->IsWeakCell() || obj->IsForeign() || obj->IsBreakPointInfo();
}

class Checksum {
 public:
  explicit Checksum(Vector<const byte> payload) {
#ifdef MEMORY_SANITIZER
    // Computing the checksum includes padding bytes for objects like strings.
    // Mark every object as initialized in the code serializer.
    MSAN_MEMORY_IS_INITIALIZED(payload.start(), payload.length());
#endif  // MEMORY_SANITIZER
    // Fletcher's checksum. Modified to reduce 64-bit sums to 32-bit.
    uintptr_t a = 1;
    uintptr_t b = 0;
    const uintptr_t* cur = reinterpret_cast<const uintptr_t*>(payload.start());
    DCHECK(IsAligned(payload.length(), kIntptrSize));
    const uintptr_t* end = cur + payload.length() / kIntptrSize;
    while (cur < end) {
      // Unsigned overflow expected and intended.
      a += *cur++;
      b += a;
    }
#if V8_HOST_ARCH_64_BIT
    a ^= a >> 32;
    b ^= b >> 32;
#endif  // V8_HOST_ARCH_64_BIT
    a_ = static_cast<uint32_t>(a);
    b_ = static_cast<uint32_t>(b);
  }

  bool Check(uint32_t a, uint32_t b) const { return a == a_ && b == b_; }

  uint32_t a() const { return a_; }
  uint32_t b() const { return b_; }

 private:
  uint32_t a_;
  uint32_t b_;

  DISALLOW_COPY_AND_ASSIGN(Checksum);
};

SerializedCodeData::SerializedCodeData(const std::vector<byte>* payload,
                                       const CodeSerializer* cs) {
  DisallowHeapAllocation no_gc;
  const std::vector<uint32_t>* stub_keys = cs->stub_keys();

  std::vector<Reservation> reservations;
  cs->EncodeReservations(&reservations);

  // Calculate sizes.
  uint32_t reservation_size =
      static_cast<uint32_t>(reservations.size()) * kUInt32Size;
  uint32_t num_stub_keys = static_cast<uint32_t>(stub_keys->size());
  uint32_t stub_keys_size = num_stub_keys * kUInt32Size;
  uint32_t payload_offset = kHeaderSize + reservation_size + stub_keys_size;
  uint32_t padded_payload_offset = POINTER_SIZE_ALIGN(payload_offset);
  uint32_t size =
      padded_payload_offset + static_cast<uint32_t>(payload->size());

  // Allocate backing store and create result data.
  AllocateData(size);

  // Set header values.
  SetMagicNumber(cs->isolate());
  SetHeaderValue(kVersionHashOffset, Version::Hash());
  SetHeaderValue(kSourceHashOffset, cs->source_hash());
  SetHeaderValue(kCpuFeaturesOffset,
                 static_cast<uint32_t>(CpuFeatures::SupportedFeatures()));
  SetHeaderValue(kFlagHashOffset, FlagList::Hash());
  SetHeaderValue(kNumReservationsOffset,
                 static_cast<uint32_t>(reservations.size()));
  SetHeaderValue(kNumCodeStubKeysOffset, num_stub_keys);
  SetHeaderValue(kPayloadLengthOffset, static_cast<uint32_t>(payload->size()));

  // Zero out any padding in the header.
  memset(data_ + kUnalignedHeaderSize, 0, kHeaderSize - kUnalignedHeaderSize);

  // Copy reservation chunk sizes.
  CopyBytes(data_ + kHeaderSize,
            reinterpret_cast<const byte*>(reservations.data()),
            reservation_size);

  // Copy code stub keys.
  CopyBytes(data_ + kHeaderSize + reservation_size,
            reinterpret_cast<const byte*>(stub_keys->data()), stub_keys_size);

  // Zero out any padding before the payload.
  memset(data_ + payload_offset, 0, padded_payload_offset - payload_offset);

  // Copy serialized data.
  CopyBytes(data_ + padded_payload_offset, payload->data(),
            static_cast<size_t>(payload->size()));

  Checksum checksum(DataWithoutHeader());
  SetHeaderValue(kChecksum1Offset, checksum.a());
  SetHeaderValue(kChecksum2Offset, checksum.b());
}

SerializedCodeData::SanityCheckResult SerializedCodeData::SanityCheck(
    Isolate* isolate, uint32_t expected_source_hash) const {
  if (this->size_ < kHeaderSize) return INVALID_HEADER;
  uint32_t magic_number = GetMagicNumber();
  if (magic_number != ComputeMagicNumber(isolate)) return MAGIC_NUMBER_MISMATCH;
  uint32_t version_hash = GetHeaderValue(kVersionHashOffset);
  uint32_t source_hash = GetHeaderValue(kSourceHashOffset);
  uint32_t cpu_features = GetHeaderValue(kCpuFeaturesOffset);
  uint32_t flags_hash = GetHeaderValue(kFlagHashOffset);
  uint32_t payload_length = GetHeaderValue(kPayloadLengthOffset);
  uint32_t c1 = GetHeaderValue(kChecksum1Offset);
  uint32_t c2 = GetHeaderValue(kChecksum2Offset);
  if (version_hash != Version::Hash()) return VERSION_MISMATCH;
  if (source_hash != expected_source_hash) return SOURCE_MISMATCH;
  if (cpu_features != static_cast<uint32_t>(CpuFeatures::SupportedFeatures())) {
    return CPU_FEATURES_MISMATCH;
  }
  if (flags_hash != FlagList::Hash()) return FLAGS_MISMATCH;
  uint32_t max_payload_length =
      this->size_ -
      POINTER_SIZE_ALIGN(kHeaderSize +
                         GetHeaderValue(kNumReservationsOffset) * kInt32Size +
                         GetHeaderValue(kNumCodeStubKeysOffset) * kInt32Size);
  if (payload_length > max_payload_length) return LENGTH_MISMATCH;
  if (!Checksum(DataWithoutHeader()).Check(c1, c2)) return CHECKSUM_MISMATCH;
  return CHECK_SUCCESS;
}

uint32_t SerializedCodeData::SourceHash(Handle<String> source) {
  return source->length();
}

// Return ScriptData object and relinquish ownership over it to the caller.
ScriptData* SerializedCodeData::GetScriptData() {
  DCHECK(owns_data_);
  ScriptData* result = new ScriptData(data_, size_);
  result->AcquireDataOwnership();
  owns_data_ = false;
  data_ = NULL;
  return result;
}

Vector<const SerializedData::Reservation> SerializedCodeData::Reservations()
    const {
  return Vector<const Reservation>(
      reinterpret_cast<const Reservation*>(data_ + kHeaderSize),
      GetHeaderValue(kNumReservationsOffset));
}

Vector<const byte> SerializedCodeData::Payload() const {
  int reservations_size = GetHeaderValue(kNumReservationsOffset) * kInt32Size;
  int code_stubs_size = GetHeaderValue(kNumCodeStubKeysOffset) * kInt32Size;
  int payload_offset = kHeaderSize + reservations_size + code_stubs_size;
  int padded_payload_offset = POINTER_SIZE_ALIGN(payload_offset);
  const byte* payload = data_ + padded_payload_offset;
  DCHECK(IsAligned(reinterpret_cast<intptr_t>(payload), kPointerAlignment));
  int length = GetHeaderValue(kPayloadLengthOffset);
  DCHECK_EQ(data_ + size_, payload + length);
  return Vector<const byte>(payload, length);
}

Vector<const uint32_t> SerializedCodeData::CodeStubKeys() const {
  int reservations_size = GetHeaderValue(kNumReservationsOffset) * kInt32Size;
  const byte* start = data_ + kHeaderSize + reservations_size;
  return Vector<const uint32_t>(reinterpret_cast<const uint32_t*>(start),
                                GetHeaderValue(kNumCodeStubKeysOffset));
}

SerializedCodeData::SerializedCodeData(ScriptData* data)
    : SerializedData(const_cast<byte*>(data->data()), data->length()) {}

SerializedCodeData SerializedCodeData::FromCachedData(
    Isolate* isolate, ScriptData* cached_data, uint32_t expected_source_hash,
    SanityCheckResult* rejection_result) {
  DisallowHeapAllocation no_gc;
  SerializedCodeData scd(cached_data);
  *rejection_result = scd.SanityCheck(isolate, expected_source_hash);
  if (*rejection_result != CHECK_SUCCESS) {
    cached_data->Reject();
    return SerializedCodeData(nullptr, 0);
  }
  return scd;
}

}  // namespace internal
}  // namespace v8
