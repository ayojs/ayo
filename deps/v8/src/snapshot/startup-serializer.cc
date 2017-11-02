// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/snapshot/startup-serializer.h"

#include "src/objects-inl.h"
#include "src/v8threads.h"

namespace v8 {
namespace internal {

StartupSerializer::StartupSerializer(
    Isolate* isolate,
    v8::SnapshotCreator::FunctionCodeHandling function_code_handling)
    : Serializer(isolate),
      clear_function_code_(function_code_handling ==
                           v8::SnapshotCreator::FunctionCodeHandling::kClear),
      serializing_builtins_(false),
      can_be_rehashed_(true) {
  InitializeCodeAddressMap();
}

StartupSerializer::~StartupSerializer() {
  RestoreExternalReferenceRedirectors(accessor_infos_);
  OutputStatistics("StartupSerializer");
}

void StartupSerializer::SerializeObject(HeapObject* obj, HowToCode how_to_code,
                                        WhereToPoint where_to_point, int skip) {
  DCHECK(!obj->IsJSFunction());

  if (clear_function_code_) {
    if (obj->IsCode()) {
      Code* code = Code::cast(obj);
      // If the function code is compiled (either as native code or bytecode),
      // replace it with lazy-compile builtin. Only exception is when we are
      // serializing the canonical interpreter-entry-trampoline builtin.
      if (code->kind() == Code::FUNCTION ||
          (!serializing_builtins_ &&
           code->is_interpreter_trampoline_builtin())) {
        obj = isolate()->builtins()->builtin(Builtins::kCompileLazy);
      }
    } else if (obj->IsBytecodeArray()) {
      obj = isolate()->heap()->undefined_value();
    }
  }

  if (SerializeHotObject(obj, how_to_code, where_to_point, skip)) return;

  int root_index = root_index_map_.Lookup(obj);
  // We can only encode roots as such if it has already been serialized.
  // That applies to root indices below the wave front.
  if (root_index != RootIndexMap::kInvalidRootIndex) {
    if (root_has_been_serialized_.test(root_index)) {
      PutRoot(root_index, obj, how_to_code, where_to_point, skip);
      return;
    }
  }

  if (SerializeBackReference(obj, how_to_code, where_to_point, skip)) return;

  FlushSkip(skip);

  if (isolate_->external_reference_redirector() && obj->IsAccessorInfo()) {
    // Wipe external reference redirects in the accessor info.
    AccessorInfo* info = AccessorInfo::cast(obj);
    Address original_address = Foreign::cast(info->getter())->foreign_address();
    Foreign::cast(info->js_getter())->set_foreign_address(original_address);
    accessor_infos_.push_back(info);
  } else if (obj->IsScript() && Script::cast(obj)->IsUserJavaScript()) {
    Script::cast(obj)->set_context_data(
        isolate_->heap()->uninitialized_symbol());
  }

  if (obj->IsHashTable()) CheckRehashability(obj);

  // Object has not yet been serialized.  Serialize it here.
  ObjectSerializer object_serializer(this, obj, &sink_, how_to_code,
                                     where_to_point);
  object_serializer.Serialize();
}

void StartupSerializer::SerializeWeakReferencesAndDeferred() {
  // This comes right after serialization of the partial snapshot, where we
  // add entries to the partial snapshot cache of the startup snapshot. Add
  // one entry with 'undefined' to terminate the partial snapshot cache.
  Object* undefined = isolate()->heap()->undefined_value();
  VisitRootPointer(Root::kPartialSnapshotCache, &undefined);
  isolate()->heap()->IterateWeakRoots(this, VISIT_ALL);
  SerializeDeferredObjects();
  Pad();
}

int StartupSerializer::PartialSnapshotCacheIndex(HeapObject* heap_object) {
  int index;
  if (!partial_cache_index_map_.LookupOrInsert(heap_object, &index)) {
    // This object is not part of the partial snapshot cache yet. Add it to the
    // startup snapshot so we can refer to it via partial snapshot index from
    // the partial snapshot.
    VisitRootPointer(Root::kPartialSnapshotCache,
                     reinterpret_cast<Object**>(&heap_object));
  }
  return index;
}

void StartupSerializer::Synchronize(VisitorSynchronization::SyncTag tag) {
  // We expect the builtins tag after builtins have been serialized.
  DCHECK(!serializing_builtins_ || tag == VisitorSynchronization::kBuiltins);
  serializing_builtins_ = (tag == VisitorSynchronization::kHandleScope);
  sink_.Put(kSynchronize, "Synchronize");
}

void StartupSerializer::SerializeStrongReferences() {
  Isolate* isolate = this->isolate();
  // No active threads.
  CHECK_NULL(isolate->thread_manager()->FirstThreadStateInUse());
  // No active or weak handles.
  CHECK(isolate->handle_scope_implementer()->blocks()->is_empty());
  CHECK_EQ(0, isolate->global_handles()->global_handles_count());
  CHECK_EQ(0, isolate->eternal_handles()->NumberOfHandles());
  // First visit immortal immovables to make sure they end up in the first page.
  serializing_immortal_immovables_roots_ = true;
  isolate->heap()->IterateStrongRoots(this, VISIT_ONLY_STRONG_ROOT_LIST);
  // Check that immortal immovable roots are allocated on the first page.
  CHECK(HasNotExceededFirstPageOfEachSpace());
  serializing_immortal_immovables_roots_ = false;
  // Visit the rest of the strong roots.
  // Clear the stack limits to make the snapshot reproducible.
  // Reset it again afterwards.
  isolate->heap()->ClearStackLimits();
  isolate->heap()->IterateSmiRoots(this);
  isolate->heap()->SetStackLimits();

  isolate->heap()->IterateStrongRoots(this,
                                      VISIT_ONLY_STRONG_FOR_SERIALIZATION);
}

void StartupSerializer::VisitRootPointers(Root root, Object** start,
                                          Object** end) {
  if (start == isolate()->heap()->roots_array_start()) {
    // Serializing the root list needs special handling:
    // - The first pass over the root list only serializes immortal immovables.
    // - The second pass over the root list serializes the rest.
    // - Only root list elements that have been fully serialized can be
    //   referenced via as root by using kRootArray bytecodes.
    int skip = 0;
    for (Object** current = start; current < end; current++) {
      int root_index = static_cast<int>(current - start);
      if (RootShouldBeSkipped(root_index)) {
        skip += kPointerSize;
        continue;
      } else {
        if ((*current)->IsSmi()) {
          FlushSkip(skip);
          PutSmi(Smi::cast(*current));
        } else {
          SerializeObject(HeapObject::cast(*current), kPlain, kStartOfObject,
                          skip);
        }
        root_has_been_serialized_.set(root_index);
        skip = 0;
      }
    }
    FlushSkip(skip);
  } else {
    Serializer::VisitRootPointers(root, start, end);
  }
}

bool StartupSerializer::RootShouldBeSkipped(int root_index) {
  if (root_index == Heap::kStackLimitRootIndex ||
      root_index == Heap::kRealStackLimitRootIndex) {
    return true;
  }
  return Heap::RootIsImmortalImmovable(root_index) !=
         serializing_immortal_immovables_roots_;
}

void StartupSerializer::CheckRehashability(HeapObject* table) {
  DCHECK(table->IsHashTable());
  if (!can_be_rehashed_) return;
  // We can only correctly rehash if the four hash tables below are the only
  // ones that we deserialize.
  if (table->IsUnseededNumberDictionary()) return;
  if (table == isolate_->heap()->empty_ordered_hash_table()) return;
  if (table == isolate_->heap()->empty_slow_element_dictionary()) return;
  if (table == isolate_->heap()->empty_property_dictionary()) return;
  if (table == isolate_->heap()->weak_object_to_code_table()) return;
  if (table == isolate_->heap()->string_table()) return;
  can_be_rehashed_ = false;
}

bool StartupSerializer::MustBeDeferred(HeapObject* object) {
  if (root_has_been_serialized_.test(Heap::kFreeSpaceMapRootIndex) &&
      root_has_been_serialized_.test(Heap::kOnePointerFillerMapRootIndex) &&
      root_has_been_serialized_.test(Heap::kTwoPointerFillerMapRootIndex)) {
    // All required root objects are serialized, so any aligned objects can
    // be saved without problems.
    return false;
  }
  // Just defer everything except of Map objects until all required roots are
  // serialized. Some objects may have special alignment requirements, that may
  // not be fulfilled during deserialization until few first root objects are
  // serialized. But we must serialize Map objects since deserializer checks
  // that these root objects are indeed Maps.
  return !object->IsMap();
}

}  // namespace internal
}  // namespace v8
