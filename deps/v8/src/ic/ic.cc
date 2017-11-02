// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/ic/ic.h"

#include "src/accessors.h"
#include "src/api-arguments-inl.h"
#include "src/api.h"
#include "src/arguments.h"
#include "src/base/bits.h"
#include "src/codegen.h"
#include "src/conversions.h"
#include "src/execution.h"
#include "src/field-type.h"
#include "src/frames-inl.h"
#include "src/ic/call-optimization.h"
#include "src/ic/handler-compiler.h"
#include "src/ic/handler-configuration-inl.h"
#include "src/ic/ic-inl.h"
#include "src/ic/ic-stats.h"
#include "src/ic/stub-cache.h"
#include "src/isolate-inl.h"
#include "src/macro-assembler.h"
#include "src/prototype.h"
#include "src/runtime-profiler.h"
#include "src/runtime/runtime-utils.h"
#include "src/runtime/runtime.h"
#include "src/tracing/trace-event.h"
#include "src/tracing/tracing-category-observer.h"

namespace v8 {
namespace internal {

char IC::TransitionMarkFromState(IC::State state) {
  switch (state) {
    case UNINITIALIZED:
      return '0';
    case PREMONOMORPHIC:
      return '.';
    case MONOMORPHIC:
      return '1';
    case RECOMPUTE_HANDLER:
      return '^';
    case POLYMORPHIC:
      return 'P';
    case MEGAMORPHIC:
      return 'N';
    case GENERIC:
      return 'G';
  }
  UNREACHABLE();
}


const char* GetTransitionMarkModifier(KeyedAccessStoreMode mode) {
  if (mode == STORE_NO_TRANSITION_HANDLE_COW) return ".COW";
  if (mode == STORE_NO_TRANSITION_IGNORE_OUT_OF_BOUNDS) {
    return ".IGNORE_OOB";
  }
  if (IsGrowStoreMode(mode)) return ".GROW";
  return "";
}

#define TRACE_GENERIC_IC(reason) set_slow_stub_reason(reason);

void IC::TraceIC(const char* type, Handle<Object> name) {
  if (FLAG_ic_stats) {
    if (AddressIsDeoptimizedCode()) return;
    State new_state = nexus()->StateFromFeedback();
    TraceIC(type, name, state(), new_state);
  }
}

Address IC::GetAbstractPC(int* line, int* column) const {
  JavaScriptFrameIterator it(isolate());

  JavaScriptFrame* frame = it.frame();
  DCHECK(!frame->is_builtin());
  int position = frame->position();

  Object* maybe_script = frame->function()->shared()->script();
  if (maybe_script->IsScript()) {
    Handle<Script> script(Script::cast(maybe_script), isolate());
    Script::PositionInfo info;
    Script::GetPositionInfo(script, position, &info, Script::WITH_OFFSET);
    *line = info.line + 1;
    *column = info.column + 1;
  } else {
    *line = position;
    *column = -1;
  }

  if (frame->is_interpreted()) {
    InterpretedFrame* iframe = static_cast<InterpretedFrame*>(frame);
    Address bytecode_start =
        reinterpret_cast<Address>(iframe->GetBytecodeArray()) - kHeapObjectTag +
        BytecodeArray::kHeaderSize;
    return bytecode_start + iframe->GetBytecodeOffset();
  }

  return frame->pc();
}

void IC::TraceIC(const char* type, Handle<Object> name, State old_state,
                 State new_state) {
  if (V8_LIKELY(!FLAG_ic_stats)) return;

  Map* map = nullptr;
  if (!receiver_map().is_null()) {
    map = *receiver_map();
  }

  const char* modifier = "";
  if (IsKeyedStoreIC()) {
    KeyedAccessStoreMode mode =
        casted_nexus<KeyedStoreICNexus>()->GetKeyedAccessStoreMode();
    modifier = GetTransitionMarkModifier(mode);
  }

  if (!(FLAG_ic_stats &
        v8::tracing::TracingCategoryObserver::ENABLED_BY_TRACING)) {
    int line;
    int column;
    Address pc = GetAbstractPC(&line, &column);
    LOG(isolate(), ICEvent(type, is_keyed(), pc, line, column, map, *name,
                           TransitionMarkFromState(old_state),
                           TransitionMarkFromState(new_state), modifier,
                           slow_stub_reason_));
    return;
  }

  ICStats::instance()->Begin();
  ICInfo& ic_info = ICStats::instance()->Current();
  ic_info.type = is_keyed() ? "Keyed" : "";
  ic_info.type += type;

  Object* maybe_function =
      Memory::Object_at(fp_ + JavaScriptFrameConstants::kFunctionOffset);
  DCHECK(maybe_function->IsJSFunction());
  JSFunction* function = JSFunction::cast(maybe_function);
  int code_offset = 0;
  if (function->IsInterpreted()) {
    code_offset = InterpretedFrame::GetBytecodeOffset(fp());
  } else {
    code_offset =
        static_cast<int>(pc() - function->code()->instruction_start());
  }
  JavaScriptFrame::CollectFunctionAndOffsetForICStats(
      function, function->abstract_code(), code_offset);

  // Reserve enough space for IC transition state, the longest length is 17.
  ic_info.state.reserve(17);
  ic_info.state = "(";
  ic_info.state += TransitionMarkFromState(old_state);
  ic_info.state += "->";
  ic_info.state += TransitionMarkFromState(new_state);
  ic_info.state += modifier;
  ic_info.state += ")";
  ic_info.map = reinterpret_cast<void*>(map);
  if (map != nullptr) {
    ic_info.is_dictionary_map = map->is_dictionary_map();
    ic_info.number_of_own_descriptors = map->NumberOfOwnDescriptors();
    ic_info.instance_type = std::to_string(map->instance_type());
  }
  // TODO(lpy) Add name as key field in ICStats.
  ICStats::instance()->End();
}


#define TRACE_IC(type, name) TraceIC(type, name)

IC::IC(FrameDepth depth, Isolate* isolate, FeedbackNexus* nexus)
    : isolate_(isolate),
      vector_set_(false),
      kind_(FeedbackSlotKind::kInvalid),
      target_maps_set_(false),
      slow_stub_reason_(nullptr),
      nexus_(nexus) {
  // To improve the performance of the (much used) IC code, we unfold a few
  // levels of the stack frame iteration code. This yields a ~35% speedup when
  // running DeltaBlue and a ~25% speedup of gbemu with the '--nouse-ic' flag.
  const Address entry = Isolate::c_entry_fp(isolate->thread_local_top());
  Address* constant_pool = NULL;
  if (FLAG_enable_embedded_constant_pool) {
    constant_pool = reinterpret_cast<Address*>(
        entry + ExitFrameConstants::kConstantPoolOffset);
  }
  Address* pc_address =
      reinterpret_cast<Address*>(entry + ExitFrameConstants::kCallerPCOffset);
  Address fp = Memory::Address_at(entry + ExitFrameConstants::kCallerFPOffset);
  // If there's another JavaScript frame on the stack we need to look one frame
  // further down the stack to find the frame pointer and the return address
  // stack slot.
  if (depth == EXTRA_CALL_FRAME) {
    if (FLAG_enable_embedded_constant_pool) {
      constant_pool = reinterpret_cast<Address*>(
          fp + StandardFrameConstants::kConstantPoolOffset);
    }
    const int kCallerPCOffset = StandardFrameConstants::kCallerPCOffset;
    pc_address = reinterpret_cast<Address*>(fp + kCallerPCOffset);
    fp = Memory::Address_at(fp + StandardFrameConstants::kCallerFPOffset);
  }
#ifdef DEBUG
  StackFrameIterator it(isolate);
  for (int i = 0; i < depth + 1; i++) it.Advance();
  StackFrame* frame = it.frame();
  DCHECK(fp == frame->fp() && pc_address == frame->pc_address());
#endif
  // For interpreted functions, some bytecode handlers construct a
  // frame. We have to skip the constructed frame to find the interpreted
  // function's frame. Check if the there is an additional frame, and if there
  // is skip this frame. However, the pc should not be updated. The call to
  // ICs happen from bytecode handlers.
  intptr_t frame_marker =
      Memory::intptr_at(fp + TypedFrameConstants::kFrameTypeOffset);
  if (frame_marker == StackFrame::TypeToMarker(StackFrame::STUB)) {
    fp = Memory::Address_at(fp + TypedFrameConstants::kCallerFPOffset);
  }
  fp_ = fp;
  if (FLAG_enable_embedded_constant_pool) {
    constant_pool_address_ = constant_pool;
  }
  pc_address_ = StackFrame::ResolveReturnAddressLocation(pc_address);
  DCHECK_NOT_NULL(nexus);
  kind_ = nexus->kind();
  state_ = nexus->StateFromFeedback();
  extra_ic_state_ = kNoExtraICState;
  old_state_ = state_;
}

// The ICs that don't pass slot and vector through the stack have to
// save/restore them in the dispatcher.
bool IC::ShouldPushPopSlotAndVector(Code::Kind kind) {
  if (kind == Code::LOAD_IC || kind == Code::LOAD_GLOBAL_IC ||
      kind == Code::KEYED_LOAD_IC) {
    return true;
  }
  if (kind == Code::STORE_IC || kind == Code::KEYED_STORE_IC) {
    return !StoreWithVectorDescriptor::kPassLastArgsOnStack;
  }
  return false;
}

JSFunction* IC::GetHostFunction() const {
  // Compute the JavaScript frame for the frame pointer of this IC
  // structure. We need this to be able to find the function
  // corresponding to the frame.
  StackFrameIterator it(isolate());
  while (it.frame()->fp() != this->fp()) it.Advance();
  JavaScriptFrame* frame = JavaScriptFrame::cast(it.frame());
  // Find the function on the stack and both the active code for the
  // function and the original code.
  return frame->function();
}

static void LookupForRead(LookupIterator* it) {
  for (; it->IsFound(); it->Next()) {
    switch (it->state()) {
      case LookupIterator::NOT_FOUND:
      case LookupIterator::TRANSITION:
        UNREACHABLE();
      case LookupIterator::JSPROXY:
        return;
      case LookupIterator::INTERCEPTOR: {
        // If there is a getter, return; otherwise loop to perform the lookup.
        Handle<JSObject> holder = it->GetHolder<JSObject>();
        if (!holder->GetNamedInterceptor()->getter()->IsUndefined(
                it->isolate())) {
          return;
        }
        break;
      }
      case LookupIterator::ACCESS_CHECK:
        // ICs know how to perform access checks on global proxies.
        if (it->GetHolder<JSObject>()->IsJSGlobalProxy() && it->HasAccess()) {
          break;
        }
        return;
      case LookupIterator::ACCESSOR:
      case LookupIterator::INTEGER_INDEXED_EXOTIC:
      case LookupIterator::DATA:
        return;
    }
  }
}

bool IC::ShouldRecomputeHandler(Handle<String> name) {
  if (!RecomputeHandlerForName(name)) return false;

  maybe_handler_ = nexus()->FindHandlerForMap(receiver_map());

  // This is a contextual access, always just update the handler and stay
  // monomorphic.
  if (IsLoadGlobalIC()) return true;

  // The current map wasn't handled yet. There's no reason to stay monomorphic,
  // *unless* we're moving from a deprecated map to its replacement, or
  // to a more general elements kind.
  // TODO(verwaest): Check if the current map is actually what the old map
  // would transition to.
  if (maybe_handler_.is_null()) {
    if (!receiver_map()->IsJSObjectMap()) return false;
    Map* first_map = FirstTargetMap();
    if (first_map == NULL) return false;
    Handle<Map> old_map(first_map);
    if (old_map->is_deprecated()) return true;
    return IsMoreGeneralElementsKindTransition(old_map->elements_kind(),
                                               receiver_map()->elements_kind());
  }

  return true;
}

bool IC::RecomputeHandlerForName(Handle<Object> name) {
  if (is_keyed()) {
    // Determine whether the failure is due to a name failure.
    if (!name->IsName()) return false;
    Name* stub_name = nexus()->FindFirstName();
    if (*name != stub_name) return false;
  }

  return true;
}


void IC::UpdateState(Handle<Object> receiver, Handle<Object> name) {
  update_receiver_map(receiver);
  if (!name->IsString()) return;
  if (state() != MONOMORPHIC && state() != POLYMORPHIC) return;
  if (receiver->IsNullOrUndefined(isolate())) return;

  // Remove the target from the code cache if it became invalid
  // because of changes in the prototype chain to avoid hitting it
  // again.
  if (ShouldRecomputeHandler(Handle<String>::cast(name))) {
    MarkRecomputeHandler(name);
  }
}


MaybeHandle<Object> IC::TypeError(MessageTemplate::Template index,
                                  Handle<Object> object, Handle<Object> key) {
  HandleScope scope(isolate());
  THROW_NEW_ERROR(isolate(), NewTypeError(index, key, object), Object);
}


MaybeHandle<Object> IC::ReferenceError(Handle<Name> name) {
  HandleScope scope(isolate());
  THROW_NEW_ERROR(
      isolate(), NewReferenceError(MessageTemplate::kNotDefined, name), Object);
}


// static
void IC::OnFeedbackChanged(Isolate* isolate, FeedbackVector* vector,
                           JSFunction* host_function) {
  if (FLAG_trace_opt_verbose) {
    // TODO(leszeks): The host function is only needed for this print, we could
    // remove it as a parameter if we're of with removing this trace (or only
    // tracing the feedback vector, not the function name).
    if (vector->profiler_ticks() != 0) {
      PrintF("[resetting ticks for ");
      host_function->PrintName();
      PrintF(" due from %d due to IC change]\n", vector->profiler_ticks());
    }
  }
  vector->set_profiler_ticks(0);
  isolate->runtime_profiler()->NotifyICChanged();
  // TODO(2029): When an optimized function is patched, it would
  // be nice to propagate the corresponding type information to its
  // unoptimized version for the benefit of later inlining.
}

static bool MigrateDeprecated(Handle<Object> object) {
  if (!object->IsJSObject()) return false;
  Handle<JSObject> receiver = Handle<JSObject>::cast(object);
  if (!receiver->map()->is_deprecated()) return false;
  JSObject::MigrateInstance(Handle<JSObject>::cast(object));
  return true;
}

void IC::ConfigureVectorState(IC::State new_state, Handle<Object> key) {
  if (new_state == PREMONOMORPHIC) {
    nexus()->ConfigurePremonomorphic();
  } else if (new_state == MEGAMORPHIC) {
    DCHECK_IMPLIES(!is_keyed(), key->IsName());
    nexus()->ConfigureMegamorphic(key->IsName() ? PROPERTY : ELEMENT);
  } else {
    UNREACHABLE();
  }

  vector_set_ = true;
  OnFeedbackChanged(isolate(), *vector(), GetHostFunction());
}

void IC::ConfigureVectorState(Handle<Name> name, Handle<Map> map,
                              Handle<Object> handler) {
  if (IsLoadGlobalIC()) {
    LoadGlobalICNexus* nexus = casted_nexus<LoadGlobalICNexus>();
    nexus->ConfigureHandlerMode(handler);
  } else {
    // Non-keyed ICs don't track the name explicitly.
    if (!is_keyed()) name = Handle<Name>::null();
    nexus()->ConfigureMonomorphic(name, map, handler);
  }

  vector_set_ = true;
  OnFeedbackChanged(isolate(), *vector(), GetHostFunction());
}

void IC::ConfigureVectorState(Handle<Name> name, MapHandles const& maps,
                              List<Handle<Object>>* handlers) {
  DCHECK(!IsLoadGlobalIC());
  // Non-keyed ICs don't track the name explicitly.
  if (!is_keyed()) name = Handle<Name>::null();
  nexus()->ConfigurePolymorphic(name, maps, handlers);

  vector_set_ = true;
  OnFeedbackChanged(isolate(), *vector(), GetHostFunction());
}

MaybeHandle<Object> LoadIC::Load(Handle<Object> object, Handle<Name> name) {
  // If the object is undefined or null it's illegal to try to get any
  // of its properties; throw a TypeError in that case.
  if (object->IsNullOrUndefined(isolate())) {
    if (FLAG_use_ic && state() != UNINITIALIZED && state() != PREMONOMORPHIC) {
      // Ensure the IC state progresses.
      TRACE_HANDLER_STATS(isolate(), LoadIC_NonReceiver);
      update_receiver_map(object);
      PatchCache(name, slow_stub());
      TRACE_IC("LoadIC", name);
    }

    if (*name == isolate()->heap()->iterator_symbol()) {
      return Runtime::ThrowIteratorError(isolate(), object);
    }
    return TypeError(MessageTemplate::kNonObjectPropertyLoad, object, name);
  }

  bool use_ic = MigrateDeprecated(object) ? false : FLAG_use_ic;

  if (state() != UNINITIALIZED) {
    JSObject::MakePrototypesFast(object, kStartAtReceiver, isolate());
    update_receiver_map(object);
  }
  // Named lookup in the object.
  LookupIterator it(object, name);
  LookupForRead(&it);

  if (it.IsFound() || !ShouldThrowReferenceError()) {
    // Update inline cache and stub cache.
    if (use_ic) UpdateCaches(&it);

    // Get the property.
    Handle<Object> result;

    ASSIGN_RETURN_ON_EXCEPTION(isolate(), result, Object::GetProperty(&it),
                               Object);
    if (it.IsFound()) {
      return result;
    } else if (!ShouldThrowReferenceError()) {
      LOG(isolate(), SuspectReadEvent(*name, *object));
      return result;
    }
  }
  return ReferenceError(name);
}

MaybeHandle<Object> LoadGlobalIC::Load(Handle<Name> name) {
  Handle<JSGlobalObject> global = isolate()->global_object();

  if (name->IsString()) {
    // Look up in script context table.
    Handle<String> str_name = Handle<String>::cast(name);
    Handle<ScriptContextTable> script_contexts(
        global->native_context()->script_context_table());

    ScriptContextTable::LookupResult lookup_result;
    if (ScriptContextTable::Lookup(script_contexts, str_name, &lookup_result)) {
      Handle<Object> result =
          FixedArray::get(*ScriptContextTable::GetContext(
                              script_contexts, lookup_result.context_index),
                          lookup_result.slot_index, isolate());
      if (result->IsTheHole(isolate())) {
        // Do not install stubs and stay pre-monomorphic for
        // uninitialized accesses.
        return ReferenceError(name);
      }

      if (FLAG_use_ic && LoadScriptContextFieldStub::Accepted(&lookup_result)) {
        TRACE_HANDLER_STATS(isolate(), LoadIC_LoadScriptContextFieldStub);
        LoadScriptContextFieldStub stub(isolate(), &lookup_result);
        PatchCache(name, stub.GetCode());
        TRACE_IC("LoadGlobalIC", name);
      }
      return result;
    }
  }
  return LoadIC::Load(global, name);
}

static bool AddOneReceiverMapIfMissing(MapHandles* receiver_maps,
                                       Handle<Map> new_receiver_map) {
  DCHECK(!new_receiver_map.is_null());
  for (Handle<Map> map : *receiver_maps) {
    if (!map.is_null() && map.is_identical_to(new_receiver_map)) {
      return false;
    }
  }
  receiver_maps->push_back(new_receiver_map);
  return true;
}

bool IC::UpdatePolymorphicIC(Handle<Name> name, Handle<Object> handler) {
  DCHECK(IsHandler(*handler));
  if (is_keyed() && state() != RECOMPUTE_HANDLER) return false;
  Handle<Map> map = receiver_map();
  MapHandles maps;
  List<Handle<Object>> handlers;

  TargetMaps(&maps);
  int number_of_maps = static_cast<int>(maps.size());
  int deprecated_maps = 0;
  int handler_to_overwrite = -1;

  for (int i = 0; i < number_of_maps; i++) {
    Handle<Map> current_map = maps.at(i);
    if (current_map->is_deprecated()) {
      // Filter out deprecated maps to ensure their instances get migrated.
      ++deprecated_maps;
    } else if (map.is_identical_to(current_map)) {
      // If the receiver type is already in the polymorphic IC, this indicates
      // there was a prototoype chain failure. In that case, just overwrite the
      // handler.
      handler_to_overwrite = i;
    } else if (handler_to_overwrite == -1 &&
               IsTransitionOfMonomorphicTarget(*current_map, *map)) {
      handler_to_overwrite = i;
    }
  }

  int number_of_valid_maps =
      number_of_maps - deprecated_maps - (handler_to_overwrite != -1);

  if (number_of_valid_maps >= kMaxPolymorphicMapCount) return false;
  if (number_of_maps == 0 && state() != MONOMORPHIC && state() != POLYMORPHIC) {
    return false;
  }
  if (!nexus()->FindHandlers(&handlers, static_cast<int>(maps.size()))) {
    return false;
  }

  number_of_valid_maps++;
  if (number_of_valid_maps > 1 && is_keyed()) return false;
  if (number_of_valid_maps == 1) {
    ConfigureVectorState(name, receiver_map(), handler);
  } else {
    if (handler_to_overwrite >= 0) {
      handlers.Set(handler_to_overwrite, handler);
      if (!map.is_identical_to(maps.at(handler_to_overwrite))) {
        maps[handler_to_overwrite] = map;
      }
    } else {
      maps.push_back(map);
      handlers.Add(handler);
    }

    ConfigureVectorState(name, maps, &handlers);
  }

  return true;
}

void IC::UpdateMonomorphicIC(Handle<Object> handler, Handle<Name> name) {
  DCHECK(IsHandler(*handler));
  ConfigureVectorState(name, receiver_map(), handler);
}


void IC::CopyICToMegamorphicCache(Handle<Name> name) {
  MapHandles maps;
  List<Handle<Object>> handlers;
  TargetMaps(&maps);
  if (!nexus()->FindHandlers(&handlers, static_cast<int>(maps.size()))) return;
  for (int i = 0; i < static_cast<int>(maps.size()); i++) {
    UpdateMegamorphicCache(*maps.at(i), *name, *handlers.at(i));
  }
}


bool IC::IsTransitionOfMonomorphicTarget(Map* source_map, Map* target_map) {
  if (source_map == nullptr) return true;
  if (target_map == nullptr) return false;
  if (source_map->is_abandoned_prototype_map()) return false;
  ElementsKind target_elements_kind = target_map->elements_kind();
  bool more_general_transition = IsMoreGeneralElementsKindTransition(
      source_map->elements_kind(), target_elements_kind);
  Map* transitioned_map = nullptr;
  if (more_general_transition) {
    MapHandles map_list;
    map_list.push_back(handle(target_map));
    transitioned_map = source_map->FindElementsKindTransitionedMap(map_list);
  }
  return transitioned_map == target_map;
}

void IC::PatchCache(Handle<Name> name, Handle<Object> handler) {
  DCHECK(IsHandler(*handler));
  // Currently only load and store ICs support non-code handlers.
  DCHECK(IsAnyLoad() || IsAnyStore());
  switch (state()) {
    case UNINITIALIZED:
    case PREMONOMORPHIC:
      UpdateMonomorphicIC(handler, name);
      break;
    case RECOMPUTE_HANDLER:
    case MONOMORPHIC:
      if (IsLoadGlobalIC()) {
        UpdateMonomorphicIC(handler, name);
        break;
      }
    // Fall through.
    case POLYMORPHIC:
      if (!is_keyed() || state() == RECOMPUTE_HANDLER) {
        if (UpdatePolymorphicIC(name, handler)) break;
        // For keyed stubs, we can't know whether old handlers were for the
        // same key.
        CopyICToMegamorphicCache(name);
      }
      ConfigureVectorState(MEGAMORPHIC, name);
    // Fall through.
    case MEGAMORPHIC:
      UpdateMegamorphicCache(*receiver_map(), *name, *handler);
      // Indicate that we've handled this case.
      vector_set_ = true;
      break;
    case GENERIC:
      UNREACHABLE();
      break;
  }
}

Handle<Smi> LoadIC::SimpleFieldLoad(Isolate* isolate, FieldIndex index) {
  TRACE_HANDLER_STATS(isolate, LoadIC_LoadFieldDH);
  return LoadHandler::LoadField(isolate, index);
}

namespace {

template <bool fill_array = true>
int InitPrototypeChecks(Isolate* isolate, Handle<Map> receiver_map,
                        Handle<JSReceiver> holder, Handle<Name> name,
                        Handle<FixedArray> array, int first_index) {
  if (!holder.is_null() && holder->map() == *receiver_map) return 0;

  HandleScope scope(isolate);
  int checks_count = 0;

  if (receiver_map->IsPrimitiveMap() || receiver_map->IsJSGlobalProxyMap()) {
    // The validity cell check for primitive and global proxy receivers does
    // not guarantee that certain native context ever had access to other
    // native context. However, a handler created for one native context could
    // be used in other native context through the megamorphic stub cache.
    // So we record the original native context to which this handler
    // corresponds.
    if (fill_array) {
      Handle<Context> native_context = isolate->native_context();
      array->set(first_index + checks_count, native_context->self_weak_cell());
    }
    checks_count++;

  } else if (receiver_map->IsJSGlobalObjectMap()) {
    // If we are creating a handler for [Load/Store]GlobalIC then we need to
    // check that the property did not appear in the global object.
    if (fill_array) {
      Handle<JSGlobalObject> global = isolate->global_object();
      Handle<PropertyCell> cell = JSGlobalObject::EnsureEmptyPropertyCell(
          global, name, PropertyCellType::kInvalidated);
      DCHECK(cell->value()->IsTheHole(isolate));
      Handle<WeakCell> weak_cell = isolate->factory()->NewWeakCell(cell);
      array->set(first_index + checks_count, *weak_cell);
    }
    checks_count++;
  }

  // Create/count entries for each global or dictionary prototype appeared in
  // the prototype chain contains from receiver till holder.
  PrototypeIterator::WhereToEnd end = name->IsPrivate()
                                          ? PrototypeIterator::END_AT_NON_HIDDEN
                                          : PrototypeIterator::END_AT_NULL;
  for (PrototypeIterator iter(receiver_map, end); !iter.IsAtEnd();
       iter.Advance()) {
    Handle<JSReceiver> current =
        PrototypeIterator::GetCurrent<JSReceiver>(iter);
    if (holder.is_identical_to(current)) break;
    Handle<Map> current_map(current->map(), isolate);

    if (current_map->IsJSGlobalObjectMap()) {
      if (fill_array) {
        Handle<JSGlobalObject> global = Handle<JSGlobalObject>::cast(current);
        Handle<PropertyCell> cell = JSGlobalObject::EnsureEmptyPropertyCell(
            global, name, PropertyCellType::kInvalidated);
        DCHECK(cell->value()->IsTheHole(isolate));
        Handle<WeakCell> weak_cell = isolate->factory()->NewWeakCell(cell);
        array->set(first_index + checks_count, *weak_cell);
      }
      checks_count++;

    } else if (current_map->is_dictionary_map()) {
      DCHECK(!current_map->IsJSGlobalProxyMap());  // Proxy maps are fast.
      if (fill_array) {
        DCHECK_EQ(NameDictionary::kNotFound,
                  current->property_dictionary()->FindEntry(name));
        Handle<WeakCell> weak_cell =
            Map::GetOrCreatePrototypeWeakCell(current, isolate);
        array->set(first_index + checks_count, *weak_cell);
      }
      checks_count++;
    }
  }
  return checks_count;
}

// Returns 0 if the validity cell check is enough to ensure that the
// prototype chain from |receiver_map| till |holder| did not change.
// If the |holder| is an empty handle then the full prototype chain is
// checked.
// Returns -1 if the handler has to be compiled or the number of prototype
// checks otherwise.
int GetPrototypeCheckCount(Isolate* isolate, Handle<Map> receiver_map,
                           Handle<JSReceiver> holder, Handle<Name> name) {
  return InitPrototypeChecks<false>(isolate, receiver_map, holder, name,
                                    Handle<FixedArray>(), 0);
}

enum class HolderCellRequest {
  kGlobalPropertyCell,
  kHolder,
};

Handle<WeakCell> HolderCell(Isolate* isolate, Handle<JSReceiver> holder,
                            Handle<Name> name, HolderCellRequest request) {
  if (request == HolderCellRequest::kGlobalPropertyCell) {
    DCHECK(holder->IsJSGlobalObject());
    Handle<JSGlobalObject> global = Handle<JSGlobalObject>::cast(holder);
    GlobalDictionary* dict = global->global_dictionary();
    int number = dict->FindEntry(name);
    DCHECK_NE(NameDictionary::kNotFound, number);
    Handle<PropertyCell> cell(dict->CellAt(number), isolate);
    return isolate->factory()->NewWeakCell(cell);
  }
  return Map::GetOrCreatePrototypeWeakCell(holder, isolate);
}

}  // namespace

Handle<Object> LoadIC::LoadFromPrototype(Handle<Map> receiver_map,
                                         Handle<JSReceiver> holder,
                                         Handle<Name> name,
                                         Handle<Smi> smi_handler) {
  int checks_count =
      GetPrototypeCheckCount(isolate(), receiver_map, holder, name);
  DCHECK_LE(0, checks_count);

  if (receiver_map->IsPrimitiveMap() ||
      receiver_map->is_access_check_needed()) {
    DCHECK(!receiver_map->is_dictionary_map());
    DCHECK_LE(1, checks_count);  // For native context.
    smi_handler =
        LoadHandler::EnableAccessCheckOnReceiver(isolate(), smi_handler);
  } else if (receiver_map->is_dictionary_map() &&
             !receiver_map->IsJSGlobalObjectMap()) {
    smi_handler = LoadHandler::EnableLookupOnReceiver(isolate(), smi_handler);
  }

  Handle<Cell> validity_cell =
      Map::GetOrCreatePrototypeChainValidityCell(receiver_map, isolate());
  DCHECK(!validity_cell.is_null());

  // LoadIC dispatcher expects PropertyCell as a "holder" in case of kGlobal
  // handler kind.
  HolderCellRequest request =
      LoadHandler::GetHandlerKind(*smi_handler) == LoadHandler::kGlobal
          ? HolderCellRequest::kGlobalPropertyCell
          : HolderCellRequest::kHolder;

  Handle<WeakCell> holder_cell = HolderCell(isolate(), holder, name, request);

  if (checks_count == 0) {
    return isolate()->factory()->NewTuple3(holder_cell, smi_handler,
                                           validity_cell);
  }
  Handle<FixedArray> handler_array(isolate()->factory()->NewFixedArray(
      LoadHandler::kFirstPrototypeIndex + checks_count, TENURED));
  handler_array->set(LoadHandler::kSmiHandlerIndex, *smi_handler);
  handler_array->set(LoadHandler::kValidityCellIndex, *validity_cell);
  handler_array->set(LoadHandler::kHolderCellIndex, *holder_cell);
  InitPrototypeChecks(isolate(), receiver_map, holder, name, handler_array,
                      LoadHandler::kFirstPrototypeIndex);
  return handler_array;
}

Handle<Object> LoadIC::LoadFullChain(Handle<Map> receiver_map,
                                     Handle<Object> holder, Handle<Name> name,
                                     Handle<Smi> smi_handler) {
  Handle<JSReceiver> end;  // null handle
  int checks_count = GetPrototypeCheckCount(isolate(), receiver_map, end, name);
  DCHECK_LE(0, checks_count);

  if (receiver_map->IsPrimitiveMap() ||
      receiver_map->is_access_check_needed()) {
    DCHECK(!receiver_map->is_dictionary_map());
    DCHECK_LE(1, checks_count);  // For native context.
    smi_handler =
        LoadHandler::EnableAccessCheckOnReceiver(isolate(), smi_handler);
  } else if (receiver_map->is_dictionary_map() &&
             !receiver_map->IsJSGlobalObjectMap()) {
    smi_handler = LoadHandler::EnableLookupOnReceiver(isolate(), smi_handler);
  }

  Handle<Object> validity_cell =
      Map::GetOrCreatePrototypeChainValidityCell(receiver_map, isolate());
  if (validity_cell.is_null()) {
    DCHECK_EQ(0, checks_count);
    // Lookup on receiver isn't supported in case of a simple smi handler.
    if (!LoadHandler::LookupOnReceiverBits::decode(smi_handler->value())) {
      return smi_handler;
    }
    validity_cell = handle(Smi::kZero, isolate());
  }

  Factory* factory = isolate()->factory();
  if (checks_count == 0) {
    return factory->NewTuple3(holder, smi_handler, validity_cell);
  }
  Handle<FixedArray> handler_array(factory->NewFixedArray(
      LoadHandler::kFirstPrototypeIndex + checks_count, TENURED));
  handler_array->set(LoadHandler::kSmiHandlerIndex, *smi_handler);
  handler_array->set(LoadHandler::kValidityCellIndex, *validity_cell);
  handler_array->set(LoadHandler::kHolderCellIndex, *holder);
  InitPrototypeChecks(isolate(), receiver_map, end, name, handler_array,
                      LoadHandler::kFirstPrototypeIndex);
  return handler_array;
}

void LoadIC::UpdateCaches(LookupIterator* lookup) {
  if (state() == UNINITIALIZED && !IsLoadGlobalIC()) {
    // This is the first time we execute this inline cache. Set the target to
    // the pre monomorphic stub to delay setting the monomorphic state.
    TRACE_HANDLER_STATS(isolate(), LoadIC_Premonomorphic);
    ConfigureVectorState(PREMONOMORPHIC, Handle<Object>());
    TRACE_IC("LoadIC", lookup->name());
    return;
  }

  Handle<Object> code;
  if (lookup->state() == LookupIterator::ACCESS_CHECK) {
    code = slow_stub();
  } else if (!lookup->IsFound()) {
    TRACE_HANDLER_STATS(isolate(), LoadIC_LoadNonexistentDH);
    Handle<Smi> smi_handler = LoadHandler::LoadNonExistent(isolate());
    code = LoadFullChain(receiver_map(), isolate()->factory()->null_value(),
                         lookup->name(), smi_handler);
  } else {
    if (IsLoadGlobalIC()) {
      if (lookup->TryLookupCachedProperty()) {
        DCHECK_EQ(LookupIterator::DATA, lookup->state());
      }
      if (lookup->state() == LookupIterator::DATA &&
          lookup->GetReceiver().is_identical_to(lookup->GetHolder<Object>())) {
        DCHECK(lookup->GetReceiver()->IsJSGlobalObject());
        // Now update the cell in the feedback vector.
        LoadGlobalICNexus* nexus = casted_nexus<LoadGlobalICNexus>();
        nexus->ConfigurePropertyCellMode(lookup->GetPropertyCell());
        TRACE_IC("LoadGlobalIC", lookup->name());
        return;
      }
    }
    code = ComputeHandler(lookup);
  }

  PatchCache(lookup->name(), code);
  TRACE_IC("LoadIC", lookup->name());
}

StubCache* IC::stub_cache() {
  if (IsAnyLoad()) {
    return isolate()->load_stub_cache();
  } else {
    DCHECK(IsAnyStore());
    return isolate()->store_stub_cache();
  }
}

void IC::UpdateMegamorphicCache(Map* map, Name* name, Object* handler) {
  stub_cache()->Set(name, map, handler);
}

void IC::TraceHandlerCacheHitStats(LookupIterator* lookup) {
  DCHECK_EQ(LookupIterator::ACCESSOR, lookup->state());
  if (V8_LIKELY(!FLAG_runtime_stats)) return;
  if (IsAnyLoad()) {
    TRACE_HANDLER_STATS(isolate(), LoadIC_HandlerCacheHit_Accessor);
  } else {
    DCHECK(IsAnyStore());
    TRACE_HANDLER_STATS(isolate(), StoreIC_HandlerCacheHit_Accessor);
  }
}

Handle<Object> IC::ComputeHandler(LookupIterator* lookup) {
  // Try to find a globally shared handler stub.
  Handle<Object> shared_handler = GetMapIndependentHandler(lookup);
  if (!shared_handler.is_null()) {
    DCHECK(IC::IsHandler(*shared_handler));
    return shared_handler;
  }

  Handle<Code> handler = PropertyHandlerCompiler::Find(
      lookup->name(), receiver_map(), handler_kind());
  // Use the cached value if it exists, and if it is different from the
  // handler that just missed.
  if (!handler.is_null()) {
    Handle<Object> current_handler;
    if (maybe_handler_.ToHandle(&current_handler)) {
      if (!current_handler.is_identical_to(handler)) {
        TraceHandlerCacheHitStats(lookup);
        return handler;
      }
    } else {
      // maybe_handler_ is only populated for MONOMORPHIC and POLYMORPHIC ICs.
      // In MEGAMORPHIC case, check if the handler in the megamorphic stub
      // cache (which just missed) is different from the cached handler.
      if (state() == MEGAMORPHIC && lookup->GetReceiver()->IsHeapObject()) {
        Map* map = Handle<HeapObject>::cast(lookup->GetReceiver())->map();
        Object* megamorphic_cached_handler =
            stub_cache()->Get(*lookup->name(), map);
        if (megamorphic_cached_handler != *handler) {
          TraceHandlerCacheHitStats(lookup);
          return handler;
        }
      } else {
        TraceHandlerCacheHitStats(lookup);
        return handler;
      }
    }
  }

  handler = CompileHandler(lookup);
  Map::UpdateCodeCache(receiver_map(), lookup->name(), handler);
  return handler;
}

Handle<Object> LoadIC::GetMapIndependentHandler(LookupIterator* lookup) {
  Handle<Object> receiver = lookup->GetReceiver();
  if (receiver->IsString() &&
      *lookup->name() == isolate()->heap()->length_string()) {
    FieldIndex index = FieldIndex::ForInObjectOffset(String::kLengthOffset);
    return SimpleFieldLoad(isolate(), index);
  }

  if (receiver->IsStringWrapper() &&
      *lookup->name() == isolate()->heap()->length_string()) {
    TRACE_HANDLER_STATS(isolate(), LoadIC_StringLengthStub);
    StringLengthStub string_length_stub(isolate());
    return string_length_stub.GetCode();
  }

  // Use specialized code for getting prototype of functions.
  if (receiver->IsJSFunction() &&
      *lookup->name() == isolate()->heap()->prototype_string() &&
      receiver->IsConstructor() &&
      !Handle<JSFunction>::cast(receiver)
           ->map()
           ->has_non_instance_prototype()) {
    Handle<Code> stub;
    TRACE_HANDLER_STATS(isolate(), LoadIC_FunctionPrototypeStub);
    return BUILTIN_CODE(isolate(), LoadIC_FunctionPrototype);
  }

  Handle<Map> map = receiver_map();
  Handle<JSObject> holder;
  bool receiver_is_holder;
  if (lookup->state() != LookupIterator::JSPROXY) {
    holder = lookup->GetHolder<JSObject>();
    receiver_is_holder = receiver.is_identical_to(holder);
  }

  switch (lookup->state()) {
    case LookupIterator::INTERCEPTOR: {
      Handle<Smi> smi_handler = LoadHandler::LoadInterceptor(isolate());

      if (holder->GetNamedInterceptor()->non_masking()) {
        Handle<Object> holder_ref = isolate()->factory()->null_value();
        if (!receiver_is_holder || IsLoadGlobalIC()) {
          holder_ref = Map::GetOrCreatePrototypeWeakCell(holder, isolate());
        }
        TRACE_HANDLER_STATS(isolate(), LoadIC_LoadNonMaskingInterceptorDH);
        return LoadFullChain(map, holder_ref, lookup->name(), smi_handler);
      }

      if (receiver_is_holder) {
        DCHECK(map->has_named_interceptor());
        TRACE_HANDLER_STATS(isolate(), LoadIC_LoadInterceptorDH);
        return smi_handler;
      }

      TRACE_HANDLER_STATS(isolate(), LoadIC_LoadInterceptorFromPrototypeDH);
      return LoadFromPrototype(map, holder, lookup->name(), smi_handler);
    }

    case LookupIterator::ACCESSOR: {
      // Use simple field loads for some well-known callback properties.
      // The method will only return true for absolute truths based on the
      // receiver maps.
      int object_offset;
      if (Accessors::IsJSObjectFieldAccessor(map, lookup->name(),
                                             &object_offset)) {
        FieldIndex index = FieldIndex::ForInObjectOffset(object_offset, *map);
        return SimpleFieldLoad(isolate(), index);
      }
      if (holder->IsJSModuleNamespace()) {
        Handle<ObjectHashTable> exports(
            Handle<JSModuleNamespace>::cast(holder)->module()->exports(),
            isolate());
        int entry = exports->FindEntry(isolate(), lookup->name(),
                                       Smi::ToInt(lookup->name()->GetHash()));
        // We found the accessor, so the entry must exist.
        DCHECK(entry != ObjectHashTable::kNotFound);
        int index = ObjectHashTable::EntryToValueIndex(entry);
        return LoadHandler::LoadModuleExport(isolate(), index);
      }

      Handle<Object> accessors = lookup->GetAccessors();
      if (accessors->IsAccessorPair()) {
        if (lookup->TryLookupCachedProperty()) {
          DCHECK_EQ(LookupIterator::DATA, lookup->state());
          return ComputeHandler(lookup);
        }

        // When debugging we need to go the slow path to flood the accessor.
        if (GetHostFunction()->shared()->HasBreakInfo()) {
          TRACE_HANDLER_STATS(isolate(), LoadIC_SlowStub);
          return slow_stub();
        }

        Handle<Object> getter(AccessorPair::cast(*accessors)->getter(),
                              isolate());
        if (!getter->IsJSFunction() && !getter->IsFunctionTemplateInfo()) {
          TRACE_HANDLER_STATS(isolate(), LoadIC_SlowStub);
          return slow_stub();
        }

        CallOptimization call_optimization(getter);
        if (call_optimization.is_simple_api_call()) {
          if (!call_optimization.IsCompatibleReceiverMap(map, holder) ||
              !holder->HasFastProperties()) {
            TRACE_HANDLER_STATS(isolate(), LoadIC_SlowStub);
            return slow_stub();
          }
          break;
        }

        // FunctionTemplate isn't yet supported as smi-handler.
        if (getter->IsFunctionTemplateInfo()) {
          if (!holder->HasFastProperties()) {
            TRACE_HANDLER_STATS(isolate(), LoadIC_SlowStub);
            return slow_stub();
          }
          break;
        }

        Handle<Smi> smi_handler;
        if (holder->HasFastProperties()) {
          smi_handler =
              LoadHandler::LoadAccessor(isolate(), lookup->GetAccessorIndex());

          if (receiver_is_holder) {
            TRACE_HANDLER_STATS(isolate(), LoadIC_LoadAccessorDH);
            return smi_handler;
          }
          TRACE_HANDLER_STATS(isolate(), LoadIC_LoadAccessorFromPrototypeDH);
        } else if (holder->IsJSGlobalObject()) {
          TRACE_HANDLER_STATS(isolate(), LoadIC_LoadGlobalFromPrototypeDH);
          smi_handler = LoadHandler::LoadGlobal(isolate());
        } else {
          smi_handler = LoadHandler::LoadNormal(isolate());

          if (receiver_is_holder) {
            TRACE_HANDLER_STATS(isolate(), LoadIC_LoadNormalDH);
            return smi_handler;
          }
          TRACE_HANDLER_STATS(isolate(), LoadIC_LoadNormalFromPrototypeDH);
        }

        return LoadFromPrototype(map, holder, lookup->name(), smi_handler);
      }

      Handle<AccessorInfo> info = Handle<AccessorInfo>::cast(accessors);

      if (v8::ToCData<Address>(info->getter()) == nullptr ||
          !AccessorInfo::IsCompatibleReceiverMap(isolate(), info, map) ||
          !holder->HasFastProperties() ||
          (info->is_sloppy() && !receiver->IsJSReceiver())) {
        TRACE_HANDLER_STATS(isolate(), LoadIC_SlowStub);
        return slow_stub();
      }

      Handle<Smi> smi_handler =
          LoadHandler::LoadApiGetter(isolate(), lookup->GetAccessorIndex());
      TRACE_HANDLER_STATS(isolate(), LoadIC_LoadApiGetterDH);
      if (receiver_is_holder) return smi_handler;
      TRACE_HANDLER_STATS(isolate(), LoadIC_LoadApiGetterFromPrototypeDH);
      return LoadFromPrototype(map, holder, lookup->name(), smi_handler);
    }

    case LookupIterator::DATA: {
      DCHECK_EQ(kData, lookup->property_details().kind());
      Handle<Smi> smi_handler;
      if (lookup->is_dictionary_holder()) {
        smi_handler = LoadHandler::LoadNormal(isolate());
        if (receiver_is_holder) {
          if (holder->IsJSGlobalObject()) {
            // TODO(verwaest): This is a workaround for code that leaks the
            // global object.
            TRACE_HANDLER_STATS(isolate(), LoadIC_LoadGlobalDH);
            smi_handler = LoadHandler::LoadGlobal(isolate());
            return LoadFromPrototype(map, holder, lookup->name(), smi_handler);
          }
          DCHECK(!holder->IsJSGlobalObject());
          TRACE_HANDLER_STATS(isolate(), LoadIC_LoadNormalDH);
          return smi_handler;
        }

        if (holder->IsJSGlobalObject()) {
          TRACE_HANDLER_STATS(isolate(), LoadIC_LoadGlobalFromPrototypeDH);
          smi_handler = LoadHandler::LoadGlobal(isolate());
        } else {
          TRACE_HANDLER_STATS(isolate(), LoadIC_LoadNormalFromPrototypeDH);
        }

      } else if (lookup->property_details().location() == kField) {
        FieldIndex field = lookup->GetFieldIndex();
        smi_handler = SimpleFieldLoad(isolate(), field);
        if (receiver_is_holder) return smi_handler;
        TRACE_HANDLER_STATS(isolate(), LoadIC_LoadFieldFromPrototypeDH);
      } else {
        DCHECK_EQ(kDescriptor, lookup->property_details().location());
        smi_handler =
            LoadHandler::LoadConstant(isolate(), lookup->GetConstantIndex());
        TRACE_HANDLER_STATS(isolate(), LoadIC_LoadConstantDH);
        if (receiver_is_holder) return smi_handler;
        TRACE_HANDLER_STATS(isolate(), LoadIC_LoadConstantFromPrototypeDH);
      }
      return LoadFromPrototype(map, holder, lookup->name(), smi_handler);
    }
    case LookupIterator::INTEGER_INDEXED_EXOTIC:
      TRACE_HANDLER_STATS(isolate(), LoadIC_LoadIntegerIndexedExoticDH);
      return LoadHandler::LoadNonExistent(isolate());
    case LookupIterator::JSPROXY: {
      Handle<JSProxy> holder_proxy = lookup->GetHolder<JSProxy>();
      bool receiver_is_holder_proxy = receiver.is_identical_to(holder_proxy);
      Handle<Smi> smi_handler = LoadHandler::LoadProxy(isolate());
      if (receiver_is_holder_proxy) {
        return smi_handler;
      }
      return LoadFromPrototype(map, holder_proxy, lookup->name(), smi_handler);
    }
    case LookupIterator::ACCESS_CHECK:
    case LookupIterator::NOT_FOUND:
    case LookupIterator::TRANSITION:
      UNREACHABLE();
  }

  return Handle<Code>::null();
}

Handle<Code> LoadIC::CompileHandler(LookupIterator* lookup) {
  DCHECK_EQ(LookupIterator::ACCESSOR, lookup->state());
  Handle<JSObject> holder = lookup->GetHolder<JSObject>();
  Handle<Map> map = receiver_map();

  Handle<Object> accessors = lookup->GetAccessors();
  DCHECK(accessors->IsAccessorPair());
  DCHECK(holder->HasFastProperties());
  DCHECK(!GetHostFunction()->shared()->HasBreakInfo());
  Handle<Object> getter(Handle<AccessorPair>::cast(accessors)->getter(),
                        isolate());
  CallOptimization call_optimization(getter);
  NamedLoadHandlerCompiler compiler(isolate(), map, holder);
  DCHECK(call_optimization.is_simple_api_call());
  TRACE_HANDLER_STATS(isolate(), LoadIC_LoadCallback);
  int index = lookup->GetAccessorIndex();
  Handle<Code> code = compiler.CompileLoadCallback(
      lookup->name(), call_optimization, index, slow_stub());
  return code;
}


static Handle<Object> TryConvertKey(Handle<Object> key, Isolate* isolate) {
  // This helper implements a few common fast cases for converting
  // non-smi keys of keyed loads/stores to a smi or a string.
  if (key->IsHeapNumber()) {
    double value = Handle<HeapNumber>::cast(key)->value();
    if (std::isnan(value)) {
      key = isolate->factory()->NaN_string();
    } else {
      int int_value = FastD2I(value);
      if (value == int_value && Smi::IsValid(int_value)) {
        key = handle(Smi::FromInt(int_value), isolate);
      }
    }
  } else if (key->IsUndefined(isolate)) {
    key = isolate->factory()->undefined_string();
  } else if (key->IsString()) {
    key = isolate->factory()->InternalizeString(Handle<String>::cast(key));
  }
  return key;
}

void KeyedLoadIC::UpdateLoadElement(Handle<HeapObject> receiver) {
  Handle<Map> receiver_map(receiver->map(), isolate());
  DCHECK(receiver_map->instance_type() != JS_VALUE_TYPE &&
         receiver_map->instance_type() != JS_PROXY_TYPE);  // Checked by caller.
  MapHandles target_receiver_maps;
  TargetMaps(&target_receiver_maps);

  if (target_receiver_maps.empty()) {
    Handle<Object> handler = LoadElementHandler(receiver_map);
    return ConfigureVectorState(Handle<Name>(), receiver_map, handler);
  }

  for (Handle<Map> map : target_receiver_maps) {
    if (map.is_null()) continue;
    if (map->instance_type() == JS_VALUE_TYPE) {
      TRACE_GENERIC_IC("JSValue");
      return;
    }
    if (map->instance_type() == JS_PROXY_TYPE) {
      TRACE_GENERIC_IC("JSProxy");
      return;
    }
  }

  // The first time a receiver is seen that is a transitioned version of the
  // previous monomorphic receiver type, assume the new ElementsKind is the
  // monomorphic type. This benefits global arrays that only transition
  // once, and all call sites accessing them are faster if they remain
  // monomorphic. If this optimistic assumption is not true, the IC will
  // miss again and it will become polymorphic and support both the
  // untransitioned and transitioned maps.
  if (state() == MONOMORPHIC && !receiver->IsString() &&
      IsMoreGeneralElementsKindTransition(
          target_receiver_maps.at(0)->elements_kind(),
          Handle<JSObject>::cast(receiver)->GetElementsKind())) {
    Handle<Object> handler = LoadElementHandler(receiver_map);
    return ConfigureVectorState(Handle<Name>(), receiver_map, handler);
  }

  DCHECK(state() != GENERIC);

  // Determine the list of receiver maps that this call site has seen,
  // adding the map that was just encountered.
  if (!AddOneReceiverMapIfMissing(&target_receiver_maps, receiver_map)) {
    // If the miss wasn't due to an unseen map, a polymorphic stub
    // won't help, use the generic stub.
    TRACE_GENERIC_IC("same map added twice");
    return;
  }

  // If the maximum number of receiver maps has been exceeded, use the generic
  // version of the IC.
  if (target_receiver_maps.size() > kMaxKeyedPolymorphism) {
    TRACE_GENERIC_IC("max polymorph exceeded");
    return;
  }

  List<Handle<Object>> handlers(static_cast<int>(target_receiver_maps.size()));
  LoadElementPolymorphicHandlers(&target_receiver_maps, &handlers);
  DCHECK_LE(1, target_receiver_maps.size());
  if (target_receiver_maps.size() == 1) {
    ConfigureVectorState(Handle<Name>(), target_receiver_maps[0],
                         handlers.at(0));
  } else {
    ConfigureVectorState(Handle<Name>(), target_receiver_maps, &handlers);
  }
}

Handle<Object> KeyedLoadIC::LoadElementHandler(Handle<Map> receiver_map) {
  if (receiver_map->has_indexed_interceptor() &&
      !receiver_map->GetIndexedInterceptor()->getter()->IsUndefined(
          isolate()) &&
      !receiver_map->GetIndexedInterceptor()->non_masking()) {
    TRACE_HANDLER_STATS(isolate(), KeyedLoadIC_LoadIndexedInterceptorStub);
    return LoadIndexedInterceptorStub(isolate()).GetCode();
  }
  if (receiver_map->IsStringMap()) {
    TRACE_HANDLER_STATS(isolate(), KeyedLoadIC_LoadIndexedStringStub);
    return BUILTIN_CODE(isolate(), KeyedLoadIC_IndexedString);
  }
  InstanceType instance_type = receiver_map->instance_type();
  if (instance_type < FIRST_JS_RECEIVER_TYPE) {
    TRACE_HANDLER_STATS(isolate(), KeyedLoadIC_SlowStub);
    return BUILTIN_CODE(isolate(), KeyedLoadIC_Slow);
  }

  ElementsKind elements_kind = receiver_map->elements_kind();
  if (IsSloppyArgumentsElementsKind(elements_kind)) {
    TRACE_HANDLER_STATS(isolate(), KeyedLoadIC_KeyedLoadSloppyArgumentsStub);
    return KeyedLoadSloppyArgumentsStub(isolate()).GetCode();
  }
  bool is_js_array = instance_type == JS_ARRAY_TYPE;
  if (elements_kind == DICTIONARY_ELEMENTS) {
    TRACE_HANDLER_STATS(isolate(), KeyedLoadIC_LoadElementDH);
    return LoadHandler::LoadElement(isolate(), elements_kind, false,
                                    is_js_array);
  }
  DCHECK(IsFastElementsKind(elements_kind) ||
         IsFixedTypedArrayElementsKind(elements_kind));
  // TODO(jkummerow): Use IsHoleyOrDictionaryElementsKind(elements_kind).
  bool convert_hole_to_undefined =
      is_js_array && elements_kind == HOLEY_ELEMENTS &&
      *receiver_map ==
          isolate()->raw_native_context()->GetInitialJSArrayMap(elements_kind);
  TRACE_HANDLER_STATS(isolate(), KeyedLoadIC_LoadElementDH);
  return LoadHandler::LoadElement(isolate(), elements_kind,
                                  convert_hole_to_undefined, is_js_array);
}

void KeyedLoadIC::LoadElementPolymorphicHandlers(
    MapHandles* receiver_maps, List<Handle<Object>>* handlers) {
  // Filter out deprecated maps to ensure their instances get migrated.
  receiver_maps->erase(
      std::remove_if(
          receiver_maps->begin(), receiver_maps->end(),
          [](const Handle<Map>& map) { return map->is_deprecated(); }),
      receiver_maps->end());

  for (Handle<Map> receiver_map : *receiver_maps) {
    // Mark all stable receiver maps that have elements kind transition map
    // among receiver_maps as unstable because the optimizing compilers may
    // generate an elements kind transition for this kind of receivers.
    if (receiver_map->is_stable()) {
      Map* tmap = receiver_map->FindElementsKindTransitionedMap(*receiver_maps);
      if (tmap != nullptr) {
        receiver_map->NotifyLeafMapLayoutChange();
      }
    }
    handlers->Add(LoadElementHandler(receiver_map));
  }
}

MaybeHandle<Object> KeyedLoadIC::Load(Handle<Object> object,
                                      Handle<Object> key) {
  if (MigrateDeprecated(object)) {
    Handle<Object> result;
    ASSIGN_RETURN_ON_EXCEPTION(
        isolate(), result, Runtime::GetObjectProperty(isolate(), object, key),
        Object);
    return result;
  }

  Handle<Object> load_handle;

  // Check for non-string values that can be converted into an
  // internalized string directly or is representable as a smi.
  key = TryConvertKey(key, isolate());

  uint32_t index;
  if ((key->IsInternalizedString() &&
       !String::cast(*key)->AsArrayIndex(&index)) ||
      key->IsSymbol()) {
    ASSIGN_RETURN_ON_EXCEPTION(isolate(), load_handle,
                               LoadIC::Load(object, Handle<Name>::cast(key)),
                               Object);
  } else if (FLAG_use_ic && !object->IsAccessCheckNeeded() &&
             !object->IsJSValue()) {
    if ((object->IsJSObject() && key->IsSmi()) ||
        (object->IsString() && key->IsNumber())) {
      UpdateLoadElement(Handle<HeapObject>::cast(object));
      if (is_vector_set()) {
        TRACE_IC("LoadIC", key);
      }
    }
  }

  if (!is_vector_set()) {
    ConfigureVectorState(MEGAMORPHIC, key);
    TRACE_IC("LoadIC", key);
  }

  if (!load_handle.is_null()) return load_handle;

  Handle<Object> result;
  ASSIGN_RETURN_ON_EXCEPTION(isolate(), result,
                             Runtime::GetObjectProperty(isolate(), object, key),
                             Object);
  return result;
}


bool StoreIC::LookupForWrite(LookupIterator* it, Handle<Object> value,
                             JSReceiver::StoreFromKeyed store_mode) {
  // Disable ICs for non-JSObjects for now.
  Handle<Object> object = it->GetReceiver();
  if (!object->IsJSObject()) return false;
  Handle<JSObject> receiver = Handle<JSObject>::cast(object);
  DCHECK(!receiver->map()->is_deprecated());

  for (; it->IsFound(); it->Next()) {
    switch (it->state()) {
      case LookupIterator::NOT_FOUND:
      case LookupIterator::TRANSITION:
        UNREACHABLE();
      case LookupIterator::JSPROXY:
        return false;
      case LookupIterator::INTERCEPTOR: {
        Handle<JSObject> holder = it->GetHolder<JSObject>();
        InterceptorInfo* info = holder->GetNamedInterceptor();
        if (it->HolderIsReceiverOrHiddenPrototype()) {
          return !info->non_masking() && receiver.is_identical_to(holder) &&
                 !info->setter()->IsUndefined(it->isolate());
        } else if (!info->getter()->IsUndefined(it->isolate()) ||
                   !info->query()->IsUndefined(it->isolate())) {
          return false;
        }
        break;
      }
      case LookupIterator::ACCESS_CHECK:
        if (it->GetHolder<JSObject>()->IsAccessCheckNeeded()) return false;
        break;
      case LookupIterator::ACCESSOR:
        return !it->IsReadOnly();
      case LookupIterator::INTEGER_INDEXED_EXOTIC:
        return false;
      case LookupIterator::DATA: {
        if (it->IsReadOnly()) return false;
        Handle<JSObject> holder = it->GetHolder<JSObject>();
        if (receiver.is_identical_to(holder)) {
          it->PrepareForDataProperty(value);
          // The previous receiver map might just have been deprecated,
          // so reload it.
          update_receiver_map(receiver);
          return true;
        }

        // Receiver != holder.
        if (receiver->IsJSGlobalProxy()) {
          PrototypeIterator iter(it->isolate(), receiver);
          return it->GetHolder<Object>().is_identical_to(
              PrototypeIterator::GetCurrent(iter));
        }

        if (it->HolderIsReceiverOrHiddenPrototype()) return false;

        if (it->ExtendingNonExtensible(receiver)) return false;
        created_new_transition_ = it->PrepareTransitionToDataProperty(
            receiver, value, NONE, store_mode);
        return it->IsCacheableTransition();
      }
    }
  }

  receiver = it->GetStoreTarget();
  if (it->ExtendingNonExtensible(receiver)) return false;
  created_new_transition_ =
      it->PrepareTransitionToDataProperty(receiver, value, NONE, store_mode);
  return it->IsCacheableTransition();
}

MaybeHandle<Object> StoreGlobalIC::Store(Handle<Object> object,
                                         Handle<Name> name,
                                         Handle<Object> value) {
  DCHECK(object->IsJSGlobalObject());
  DCHECK(name->IsString());

  // Look up in script context table.
  Handle<String> str_name = Handle<String>::cast(name);
  Handle<JSGlobalObject> global = Handle<JSGlobalObject>::cast(object);
  Handle<ScriptContextTable> script_contexts(
      global->native_context()->script_context_table());

  ScriptContextTable::LookupResult lookup_result;
  if (ScriptContextTable::Lookup(script_contexts, str_name, &lookup_result)) {
    Handle<Context> script_context = ScriptContextTable::GetContext(
        script_contexts, lookup_result.context_index);
    if (lookup_result.mode == CONST) {
      return TypeError(MessageTemplate::kConstAssign, object, name);
    }

    Handle<Object> previous_value =
        FixedArray::get(*script_context, lookup_result.slot_index, isolate());

    if (previous_value->IsTheHole(isolate())) {
      // Do not install stubs and stay pre-monomorphic for
      // uninitialized accesses.
      return ReferenceError(name);
    }

    if (FLAG_use_ic && StoreScriptContextFieldStub::Accepted(&lookup_result)) {
      TRACE_HANDLER_STATS(isolate(), StoreIC_StoreScriptContextFieldStub);
      StoreScriptContextFieldStub stub(isolate(), &lookup_result);
      PatchCache(name, stub.GetCode());
    }

    script_context->set(lookup_result.slot_index, *value);
    return value;
  }

  return StoreIC::Store(object, name, value);
}

MaybeHandle<Object> StoreIC::Store(Handle<Object> object, Handle<Name> name,
                                   Handle<Object> value,
                                   JSReceiver::StoreFromKeyed store_mode) {
  // TODO(verwaest): Let SetProperty do the migration, since storing a property
  // might deprecate the current map again, if value does not fit.
  if (MigrateDeprecated(object) || object->IsJSProxy()) {
    Handle<Object> result;
    ASSIGN_RETURN_ON_EXCEPTION(
        isolate(), result,
        Object::SetProperty(object, name, value, language_mode()), Object);
    return result;
  }

  // If the object is undefined or null it's illegal to try to set any
  // properties on it; throw a TypeError in that case.
  if (object->IsNullOrUndefined(isolate())) {
    if (FLAG_use_ic && state() != UNINITIALIZED && state() != PREMONOMORPHIC) {
      // Ensure the IC state progresses.
      TRACE_HANDLER_STATS(isolate(), StoreIC_NonReceiver);
      update_receiver_map(object);
      PatchCache(name, slow_stub());
      TRACE_IC("StoreIC", name);
    }
    return TypeError(MessageTemplate::kNonObjectPropertyStore, object, name);
  }

  if (state() != UNINITIALIZED) {
    JSObject::MakePrototypesFast(object, kStartAtPrototype, isolate());
  }
  MaybeHandle<Object> cached_handler;
  Handle<Map> transition_map;
  if (object->IsJSReceiver()) {
    name = isolate()->factory()->InternalizeName(name);
    TransitionsAccessor transitions(receiver_map());
    Object* maybe_handler = transitions.SearchHandler(*name, &transition_map);
    if (maybe_handler != nullptr) {
      cached_handler = MaybeHandle<Object>(maybe_handler, isolate());
    }
  }

  LookupIterator it = LookupIterator::ForTransitionHandler(
      isolate(), object, name, value, cached_handler, transition_map);
  if (FLAG_use_ic) UpdateCaches(&it, value, store_mode, cached_handler);

  MAYBE_RETURN_NULL(
      Object::SetProperty(&it, value, language_mode(), store_mode));
  return value;
}

void StoreIC::UpdateCaches(LookupIterator* lookup, Handle<Object> value,
                           JSReceiver::StoreFromKeyed store_mode,
                           MaybeHandle<Object> cached_handler) {
  if (state() == UNINITIALIZED) {
    // This is the first time we execute this inline cache. Set the target to
    // the pre monomorphic stub to delay setting the monomorphic state.
    TRACE_HANDLER_STATS(isolate(), StoreIC_Premonomorphic);
    ConfigureVectorState(PREMONOMORPHIC, Handle<Object>());
    TRACE_IC("StoreIC", lookup->name());
    return;
  }

  Handle<Object> handler;
  if (!cached_handler.is_null()) {
    handler = cached_handler.ToHandleChecked();
  } else if (LookupForWrite(lookup, value, store_mode)) {
    if (created_new_transition_) {
      // The first time a transition is performed, there's a good chance that
      // it won't be taken again, so don't bother creating a handler.
      TRACE_IC("StoreIC", lookup->name());
      return;
    }
    handler = ComputeHandler(lookup);
  } else {
    TRACE_GENERIC_IC("LookupForWrite said 'false'");
    handler = slow_stub();
  }

  PatchCache(lookup->name(), handler);
  TRACE_IC("StoreIC", lookup->name());
}

Handle<Object> StoreIC::StoreTransition(Handle<Map> receiver_map,
                                        Handle<JSObject> holder,
                                        Handle<Map> transition,
                                        Handle<Name> name) {
  Handle<Object> smi_handler;
  if (transition->is_dictionary_map()) {
    smi_handler = StoreHandler::StoreNormal(isolate());
  } else {
    int descriptor = transition->LastAdded();
    Handle<DescriptorArray> descriptors(transition->instance_descriptors());
    PropertyDetails details = descriptors->GetDetails(descriptor);
    Representation representation = details.representation();
    DCHECK(!representation.IsNone());

    // Declarative handlers don't support access checks.
    DCHECK(!transition->is_access_check_needed());

    DCHECK_EQ(kData, details.kind());
    if (details.location() == kDescriptor) {
      smi_handler = StoreHandler::TransitionToConstant(isolate(), descriptor);

    } else {
      DCHECK_EQ(kField, details.location());
      bool extend_storage =
          Map::cast(transition->GetBackPointer())->unused_property_fields() ==
          0;

      FieldIndex index = FieldIndex::ForDescriptor(*transition, descriptor);
      smi_handler = StoreHandler::TransitionToField(
          isolate(), descriptor, index, representation, extend_storage);
    }
  }
  // |holder| is either a receiver if the property is non-existent or
  // one of the prototypes.
  DCHECK(!holder.is_null());
  bool is_nonexistent = holder->map() == transition->GetBackPointer();
  if (is_nonexistent) holder = Handle<JSObject>::null();

  int checks_count =
      GetPrototypeCheckCount(isolate(), receiver_map, holder, name);

  DCHECK_LE(0, checks_count);
  DCHECK(!receiver_map->IsJSGlobalObjectMap());

  Handle<Object> validity_cell =
      Map::GetOrCreatePrototypeChainValidityCell(receiver_map, isolate());
  if (validity_cell.is_null()) {
    DCHECK_EQ(0, checks_count);
    validity_cell = handle(Smi::kZero, isolate());
  }

  Handle<WeakCell> transition_cell = Map::WeakCellForMap(transition);

  Factory* factory = isolate()->factory();
  if (checks_count == 0) {
    return factory->NewTuple3(transition_cell, smi_handler, validity_cell);
  }
  Handle<FixedArray> handler_array(factory->NewFixedArray(
      StoreHandler::kFirstPrototypeIndex + checks_count, TENURED));
  handler_array->set(StoreHandler::kSmiHandlerIndex, *smi_handler);
  handler_array->set(StoreHandler::kValidityCellIndex, *validity_cell);
  handler_array->set(StoreHandler::kTransitionCellIndex, *transition_cell);
  InitPrototypeChecks(isolate(), receiver_map, holder, name, handler_array,
                      StoreHandler::kFirstPrototypeIndex);
  return handler_array;
}

namespace {

Handle<Object> StoreGlobal(Isolate* isolate, Handle<PropertyCell> cell) {
  return isolate->factory()->NewWeakCell(cell);
}

}  // namespace

Handle<Object> StoreIC::GetMapIndependentHandler(LookupIterator* lookup) {
  DCHECK_NE(LookupIterator::JSPROXY, lookup->state());

  // This is currently guaranteed by checks in StoreIC::Store.
  Handle<JSObject> receiver = Handle<JSObject>::cast(lookup->GetReceiver());
  Handle<JSObject> holder = lookup->GetHolder<JSObject>();
  DCHECK(!receiver->IsAccessCheckNeeded() || lookup->name()->IsPrivate());

  switch (lookup->state()) {
    case LookupIterator::TRANSITION: {
      auto store_target = lookup->GetStoreTarget();
      if (store_target->IsJSGlobalObject()) {
        TRACE_HANDLER_STATS(isolate(), StoreIC_StoreGlobalTransitionDH);
        return StoreGlobal(isolate(), lookup->transition_cell());
      }
      // Currently not handled by CompileStoreTransition.
      if (!holder->HasFastProperties()) {
        TRACE_GENERIC_IC("transition from slow");
        TRACE_HANDLER_STATS(isolate(), StoreIC_SlowStub);
        return slow_stub();
      }

      DCHECK(lookup->IsCacheableTransition());
      Handle<Map> transition = lookup->transition_map();
      TRACE_HANDLER_STATS(isolate(), StoreIC_StoreTransitionDH);
      Handle<Object> handler =
          StoreTransition(receiver_map(), holder, transition, lookup->name());
      TransitionsAccessor(receiver_map())
          .UpdateHandler(*lookup->name(), *handler);
      return handler;
    }

    case LookupIterator::INTERCEPTOR: {
      DCHECK(!holder->GetNamedInterceptor()->setter()->IsUndefined(isolate()));
      TRACE_HANDLER_STATS(isolate(), StoreIC_StoreInterceptorStub);
      StoreInterceptorStub stub(isolate());
      return stub.GetCode();
    }

    case LookupIterator::ACCESSOR: {
      if (!holder->HasFastProperties()) {
        TRACE_GENERIC_IC("accessor on slow map");
        TRACE_HANDLER_STATS(isolate(), StoreIC_SlowStub);
        return slow_stub();
      }
      Handle<Object> accessors = lookup->GetAccessors();
      if (accessors->IsAccessorInfo()) {
        Handle<AccessorInfo> info = Handle<AccessorInfo>::cast(accessors);
        if (v8::ToCData<Address>(info->setter()) == nullptr) {
          TRACE_GENERIC_IC("setter == nullptr");
          TRACE_HANDLER_STATS(isolate(), StoreIC_SlowStub);
          return slow_stub();
        }
        if (AccessorInfo::cast(*accessors)->is_special_data_property() &&
            !lookup->HolderIsReceiverOrHiddenPrototype()) {
          TRACE_GENERIC_IC("special data property in prototype chain");
          TRACE_HANDLER_STATS(isolate(), StoreIC_SlowStub);
          return slow_stub();
        }
        if (!AccessorInfo::IsCompatibleReceiverMap(isolate(), info,
                                                   receiver_map())) {
          TRACE_GENERIC_IC("incompatible receiver type");
          TRACE_HANDLER_STATS(isolate(), StoreIC_SlowStub);
          return slow_stub();
        }
        break;  // Custom-compiled handler.
      } else if (accessors->IsAccessorPair()) {
        Handle<Object> setter(Handle<AccessorPair>::cast(accessors)->setter(),
                              isolate());
        if (!setter->IsJSFunction() && !setter->IsFunctionTemplateInfo()) {
          TRACE_GENERIC_IC("setter not a function");
          TRACE_HANDLER_STATS(isolate(), StoreIC_SlowStub);
          return slow_stub();
        }
        CallOptimization call_optimization(setter);
        if (call_optimization.is_simple_api_call()) {
          if (call_optimization.IsCompatibleReceiver(receiver, holder)) {
            break;  // Custom-compiled handler.
          }
          TRACE_GENERIC_IC("incompatible receiver");
          TRACE_HANDLER_STATS(isolate(), StoreIC_SlowStub);
          return slow_stub();
        }
        break;  // Custom-compiled handler.
      }
      TRACE_HANDLER_STATS(isolate(), StoreIC_SlowStub);
      return slow_stub();
    }

    case LookupIterator::DATA: {
      DCHECK_EQ(kData, lookup->property_details().kind());
      if (lookup->is_dictionary_holder()) {
        if (holder->IsJSGlobalObject()) {
          TRACE_HANDLER_STATS(isolate(), StoreIC_StoreGlobalDH);
          return StoreGlobal(isolate(), lookup->GetPropertyCell());
        }
        TRACE_HANDLER_STATS(isolate(), StoreIC_StoreNormalDH);
        DCHECK(holder.is_identical_to(receiver));
        return StoreHandler::StoreNormal(isolate());
      }

      // -------------- Fields --------------
      if (lookup->property_details().location() == kField) {
        TRACE_HANDLER_STATS(isolate(), StoreIC_StoreFieldDH);
        int descriptor = lookup->GetFieldDescriptorIndex();
        FieldIndex index = lookup->GetFieldIndex();
        PropertyConstness constness = lookup->constness();
        if (constness == kConst && IsStoreOwnICKind(nexus()->kind())) {
          // StoreOwnICs are used for initializing object literals therefore
          // we must store the value unconditionally even to kConst fields.
          constness = kMutable;
        }
        return StoreHandler::StoreField(isolate(), descriptor, index, constness,
                                        lookup->representation());
      }

      // -------------- Constant properties --------------
      DCHECK_EQ(kDescriptor, lookup->property_details().location());
      TRACE_GENERIC_IC("constant property");
      TRACE_HANDLER_STATS(isolate(), StoreIC_SlowStub);
      return slow_stub();
    }

    case LookupIterator::INTEGER_INDEXED_EXOTIC:
    case LookupIterator::ACCESS_CHECK:
    case LookupIterator::JSPROXY:
    case LookupIterator::NOT_FOUND:
      UNREACHABLE();
  }
  return Handle<Code>::null();
}

Handle<Code> StoreIC::CompileHandler(LookupIterator* lookup) {
  DCHECK_EQ(LookupIterator::ACCESSOR, lookup->state());

  // This is currently guaranteed by checks in StoreIC::Store.
  Handle<JSObject> receiver = Handle<JSObject>::cast(lookup->GetReceiver());
  Handle<JSObject> holder = lookup->GetHolder<JSObject>();
  DCHECK(!receiver->IsAccessCheckNeeded() || lookup->name()->IsPrivate());

  DCHECK(holder->HasFastProperties());
  Handle<Object> accessors = lookup->GetAccessors();

  if (accessors->IsAccessorInfo()) {
    Handle<AccessorInfo> info = Handle<AccessorInfo>::cast(accessors);
    DCHECK(v8::ToCData<Address>(info->setter()) != 0);
    DCHECK(!AccessorInfo::cast(*accessors)->is_special_data_property() ||
           lookup->HolderIsReceiverOrHiddenPrototype());
    DCHECK(
        AccessorInfo::IsCompatibleReceiverMap(isolate(), info, receiver_map()));
    TRACE_HANDLER_STATS(isolate(), StoreIC_StoreCallback);
    NamedStoreHandlerCompiler compiler(isolate(), receiver_map(), holder);
    // TODO(ishell): don't hard-code language mode into the handler because
    // this handler can be re-used through megamorphic stub cache for wrong
    // language mode.
    // Better pass vector/slot to Runtime::kStoreCallbackProperty and
    // let it decode the language mode from the IC kind.
    Handle<Code> code = compiler.CompileStoreCallback(receiver, lookup->name(),
                                                      info, language_mode());
    return code;
  }

  DCHECK(accessors->IsAccessorPair());
  Handle<Object> setter(Handle<AccessorPair>::cast(accessors)->setter(),
                        isolate());
  DCHECK(setter->IsJSFunction() || setter->IsFunctionTemplateInfo());
  CallOptimization call_optimization(setter);
  NamedStoreHandlerCompiler compiler(isolate(), receiver_map(), holder);
  if (call_optimization.is_simple_api_call()) {
    DCHECK(call_optimization.IsCompatibleReceiver(receiver, holder));
    TRACE_HANDLER_STATS(isolate(), StoreIC_StoreCallback);
    Handle<Code> code = compiler.CompileStoreCallback(
        receiver, lookup->name(), call_optimization, lookup->GetAccessorIndex(),
        slow_stub());
    return code;
  }
  TRACE_HANDLER_STATS(isolate(), StoreIC_StoreViaSetter);
  int expected_arguments =
      JSFunction::cast(*setter)->shared()->internal_formal_parameter_count();
  return compiler.CompileStoreViaSetter(
      receiver, lookup->name(), lookup->GetAccessorIndex(), expected_arguments);
}

void KeyedStoreIC::UpdateStoreElement(Handle<Map> receiver_map,
                                      KeyedAccessStoreMode store_mode) {
  MapHandles target_receiver_maps;
  TargetMaps(&target_receiver_maps);
  if (target_receiver_maps.empty()) {
    Handle<Map> monomorphic_map =
        ComputeTransitionedMap(receiver_map, store_mode);
    store_mode = GetNonTransitioningStoreMode(store_mode);
    Handle<Object> handler = StoreElementHandler(monomorphic_map, store_mode);
    return ConfigureVectorState(Handle<Name>(), monomorphic_map, handler);
  }

  for (Handle<Map> map : target_receiver_maps) {
    if (!map.is_null() && map->instance_type() == JS_VALUE_TYPE) {
      TRACE_GENERIC_IC("JSValue");
      return;
    }
  }

  // There are several special cases where an IC that is MONOMORPHIC can still
  // transition to a different GetNonTransitioningStoreMode IC that handles a
  // superset of the original IC. Handle those here if the receiver map hasn't
  // changed or it has transitioned to a more general kind.
  KeyedAccessStoreMode old_store_mode = GetKeyedAccessStoreMode();
  Handle<Map> previous_receiver_map = target_receiver_maps.at(0);
  if (state() == MONOMORPHIC) {
    Handle<Map> transitioned_receiver_map = receiver_map;
    if (IsTransitionStoreMode(store_mode)) {
      transitioned_receiver_map =
          ComputeTransitionedMap(receiver_map, store_mode);
    }
    if ((receiver_map.is_identical_to(previous_receiver_map) &&
         IsTransitionStoreMode(store_mode)) ||
        IsTransitionOfMonomorphicTarget(*previous_receiver_map,
                                        *transitioned_receiver_map)) {
      // If the "old" and "new" maps are in the same elements map family, or
      // if they at least come from the same origin for a transitioning store,
      // stay MONOMORPHIC and use the map for the most generic ElementsKind.
      store_mode = GetNonTransitioningStoreMode(store_mode);
      Handle<Object> handler =
          StoreElementHandler(transitioned_receiver_map, store_mode);
      ConfigureVectorState(Handle<Name>(), transitioned_receiver_map, handler);
      return;
    }
    if (receiver_map.is_identical_to(previous_receiver_map) &&
        old_store_mode == STANDARD_STORE &&
        (store_mode == STORE_AND_GROW_NO_TRANSITION ||
         store_mode == STORE_NO_TRANSITION_IGNORE_OUT_OF_BOUNDS ||
         store_mode == STORE_NO_TRANSITION_HANDLE_COW)) {
      // A "normal" IC that handles stores can switch to a version that can
      // grow at the end of the array, handle OOB accesses or copy COW arrays
      // and still stay MONOMORPHIC.
      Handle<Object> handler = StoreElementHandler(receiver_map, store_mode);
      return ConfigureVectorState(Handle<Name>(), receiver_map, handler);
    }
  }

  DCHECK(state() != GENERIC);

  bool map_added =
      AddOneReceiverMapIfMissing(&target_receiver_maps, receiver_map);

  if (IsTransitionStoreMode(store_mode)) {
    Handle<Map> transitioned_receiver_map =
        ComputeTransitionedMap(receiver_map, store_mode);
    map_added |= AddOneReceiverMapIfMissing(&target_receiver_maps,
                                            transitioned_receiver_map);
  }

  if (!map_added) {
    // If the miss wasn't due to an unseen map, a polymorphic stub
    // won't help, use the megamorphic stub which can handle everything.
    TRACE_GENERIC_IC("same map added twice");
    return;
  }

  // If the maximum number of receiver maps has been exceeded, use the
  // megamorphic version of the IC.
  if (target_receiver_maps.size() > kMaxKeyedPolymorphism) return;

  // Make sure all polymorphic handlers have the same store mode, otherwise the
  // megamorphic stub must be used.
  store_mode = GetNonTransitioningStoreMode(store_mode);
  if (old_store_mode != STANDARD_STORE) {
    if (store_mode == STANDARD_STORE) {
      store_mode = old_store_mode;
    } else if (store_mode != old_store_mode) {
      TRACE_GENERIC_IC("store mode mismatch");
      return;
    }
  }

  // If the store mode isn't the standard mode, make sure that all polymorphic
  // receivers are either external arrays, or all "normal" arrays. Otherwise,
  // use the megamorphic stub.
  if (store_mode != STANDARD_STORE) {
    size_t external_arrays = 0;
    for (Handle<Map> map : target_receiver_maps) {
      if (map->has_fixed_typed_array_elements()) {
        external_arrays++;
      }
    }
    if (external_arrays != 0 &&
        external_arrays != target_receiver_maps.size()) {
      TRACE_GENERIC_IC("unsupported combination of external and normal arrays");
      return;
    }
  }

  List<Handle<Object>> handlers(static_cast<int>(target_receiver_maps.size()));
  StoreElementPolymorphicHandlers(&target_receiver_maps, &handlers, store_mode);
  if (target_receiver_maps.size() == 0) {
    ConfigureVectorState(PREMONOMORPHIC, Handle<Name>());
  } else if (target_receiver_maps.size() == 1) {
    ConfigureVectorState(Handle<Name>(), target_receiver_maps[0],
                         handlers.at(0));
  } else {
    ConfigureVectorState(Handle<Name>(), target_receiver_maps, &handlers);
  }
}


Handle<Map> KeyedStoreIC::ComputeTransitionedMap(
    Handle<Map> map, KeyedAccessStoreMode store_mode) {
  switch (store_mode) {
    case STORE_TRANSITION_TO_OBJECT:
    case STORE_AND_GROW_TRANSITION_TO_OBJECT: {
      ElementsKind kind = IsHoleyElementsKind(map->elements_kind())
                              ? HOLEY_ELEMENTS
                              : PACKED_ELEMENTS;
      return Map::TransitionElementsTo(map, kind);
    }
    case STORE_TRANSITION_TO_DOUBLE:
    case STORE_AND_GROW_TRANSITION_TO_DOUBLE: {
      ElementsKind kind = IsHoleyElementsKind(map->elements_kind())
                              ? HOLEY_DOUBLE_ELEMENTS
                              : PACKED_DOUBLE_ELEMENTS;
      return Map::TransitionElementsTo(map, kind);
    }
    case STORE_NO_TRANSITION_IGNORE_OUT_OF_BOUNDS:
      DCHECK(map->has_fixed_typed_array_elements());
    // Fall through
    case STORE_NO_TRANSITION_HANDLE_COW:
    case STANDARD_STORE:
    case STORE_AND_GROW_NO_TRANSITION:
      return map;
  }
  UNREACHABLE();
}

Handle<Object> KeyedStoreIC::StoreElementHandler(
    Handle<Map> receiver_map, KeyedAccessStoreMode store_mode) {
  DCHECK(store_mode == STANDARD_STORE ||
         store_mode == STORE_AND_GROW_NO_TRANSITION ||
         store_mode == STORE_NO_TRANSITION_IGNORE_OUT_OF_BOUNDS ||
         store_mode == STORE_NO_TRANSITION_HANDLE_COW);
  DCHECK(!receiver_map->DictionaryElementsInPrototypeChainOnly());

  ElementsKind elements_kind = receiver_map->elements_kind();
  bool is_jsarray = receiver_map->instance_type() == JS_ARRAY_TYPE;
  Handle<Code> stub;
  if (receiver_map->has_sloppy_arguments_elements()) {
    TRACE_HANDLER_STATS(isolate(), KeyedStoreIC_KeyedStoreSloppyArgumentsStub);
    stub = KeyedStoreSloppyArgumentsStub(isolate(), store_mode).GetCode();
  } else if (receiver_map->has_fast_elements() ||
             receiver_map->has_fixed_typed_array_elements()) {
    TRACE_HANDLER_STATS(isolate(), KeyedStoreIC_StoreFastElementStub);
    stub =
        StoreFastElementStub(isolate(), is_jsarray, elements_kind, store_mode)
            .GetCode();
  } else {
    TRACE_HANDLER_STATS(isolate(), KeyedStoreIC_StoreElementStub);
    DCHECK_EQ(DICTIONARY_ELEMENTS, elements_kind);
    stub = StoreSlowElementStub(isolate(), store_mode).GetCode();
  }
  Handle<Object> validity_cell =
      Map::GetOrCreatePrototypeChainValidityCell(receiver_map, isolate());
  if (validity_cell.is_null()) return stub;
  return isolate()->factory()->NewTuple2(validity_cell, stub);
}

void KeyedStoreIC::StoreElementPolymorphicHandlers(
    MapHandles* receiver_maps, List<Handle<Object>>* handlers,
    KeyedAccessStoreMode store_mode) {
  DCHECK(store_mode == STANDARD_STORE ||
         store_mode == STORE_AND_GROW_NO_TRANSITION ||
         store_mode == STORE_NO_TRANSITION_IGNORE_OUT_OF_BOUNDS ||
         store_mode == STORE_NO_TRANSITION_HANDLE_COW);

  // Filter out deprecated maps to ensure their instances get migrated.
  receiver_maps->erase(
      std::remove_if(
          receiver_maps->begin(), receiver_maps->end(),
          [](const Handle<Map>& map) { return map->is_deprecated(); }),
      receiver_maps->end());

  for (Handle<Map> receiver_map : *receiver_maps) {
    Handle<Object> handler;
    Handle<Map> transitioned_map;

    if (receiver_map->instance_type() < FIRST_JS_RECEIVER_TYPE ||
        receiver_map->DictionaryElementsInPrototypeChainOnly()) {
      // TODO(mvstanton): Consider embedding store_mode in the state of the slow
      // keyed store ic for uniformity.
      TRACE_HANDLER_STATS(isolate(), KeyedStoreIC_SlowStub);
      handler = BUILTIN_CODE(isolate(), KeyedStoreIC_Slow);

    } else {
      {
        Map* tmap =
            receiver_map->FindElementsKindTransitionedMap(*receiver_maps);
        if (tmap != nullptr) {
          if (receiver_map->is_stable()) {
            receiver_map->NotifyLeafMapLayoutChange();
          }
          transitioned_map = handle(tmap);
        }
      }

      // TODO(mvstanton): The code below is doing pessimistic elements
      // transitions. I would like to stop doing that and rely on Allocation
      // Site Tracking to do a better job of ensuring the data types are what
      // they need to be. Not all the elements are in place yet, pessimistic
      // elements transitions are still important for performance.
      if (!transitioned_map.is_null()) {
        bool is_js_array = receiver_map->instance_type() == JS_ARRAY_TYPE;
        ElementsKind elements_kind = receiver_map->elements_kind();
        TRACE_HANDLER_STATS(isolate(),
                            KeyedStoreIC_ElementsTransitionAndStoreStub);
        Handle<Code> stub =
            ElementsTransitionAndStoreStub(isolate(), elements_kind,
                                           transitioned_map->elements_kind(),
                                           is_js_array, store_mode)
                .GetCode();
        Handle<Object> validity_cell =
            Map::GetOrCreatePrototypeChainValidityCell(receiver_map, isolate());
        if (validity_cell.is_null()) {
          validity_cell = handle(Smi::kZero, isolate());
        }
        Handle<WeakCell> transition = Map::WeakCellForMap(transitioned_map);
        handler =
            isolate()->factory()->NewTuple3(transition, stub, validity_cell);
      } else {
        handler = StoreElementHandler(receiver_map, store_mode);
      }
    }
    DCHECK(!handler.is_null());
    handlers->Add(handler);
  }
}

bool IsOutOfBoundsAccess(Handle<JSObject> receiver, uint32_t index) {
  uint32_t length = 0;
  if (receiver->IsJSArray()) {
    JSArray::cast(*receiver)->length()->ToArrayLength(&length);
  } else {
    length = static_cast<uint32_t>(receiver->elements()->length());
  }
  return index >= length;
}


static KeyedAccessStoreMode GetStoreMode(Handle<JSObject> receiver,
                                         uint32_t index, Handle<Object> value) {
  bool oob_access = IsOutOfBoundsAccess(receiver, index);
  // Don't consider this a growing store if the store would send the receiver to
  // dictionary mode.
  bool allow_growth = receiver->IsJSArray() && oob_access &&
                      !receiver->WouldConvertToSlowElements(index);
  if (allow_growth) {
    // Handle growing array in stub if necessary.
    if (receiver->HasSmiElements()) {
      if (value->IsHeapNumber()) {
        return STORE_AND_GROW_TRANSITION_TO_DOUBLE;
      }
      if (value->IsHeapObject()) {
        return STORE_AND_GROW_TRANSITION_TO_OBJECT;
      }
    } else if (receiver->HasDoubleElements()) {
      if (!value->IsSmi() && !value->IsHeapNumber()) {
        return STORE_AND_GROW_TRANSITION_TO_OBJECT;
      }
    }
    return STORE_AND_GROW_NO_TRANSITION;
  } else {
    // Handle only in-bounds elements accesses.
    if (receiver->HasSmiElements()) {
      if (value->IsHeapNumber()) {
        return STORE_TRANSITION_TO_DOUBLE;
      } else if (value->IsHeapObject()) {
        return STORE_TRANSITION_TO_OBJECT;
      }
    } else if (receiver->HasDoubleElements()) {
      if (!value->IsSmi() && !value->IsHeapNumber()) {
        return STORE_TRANSITION_TO_OBJECT;
      }
    }
    if (!FLAG_trace_external_array_abuse &&
        receiver->map()->has_fixed_typed_array_elements() && oob_access) {
      return STORE_NO_TRANSITION_IGNORE_OUT_OF_BOUNDS;
    }
    Heap* heap = receiver->GetHeap();
    if (receiver->elements()->map() == heap->fixed_cow_array_map()) {
      return STORE_NO_TRANSITION_HANDLE_COW;
    } else {
      return STANDARD_STORE;
    }
  }
}


MaybeHandle<Object> KeyedStoreIC::Store(Handle<Object> object,
                                        Handle<Object> key,
                                        Handle<Object> value) {
  // TODO(verwaest): Let SetProperty do the migration, since storing a property
  // might deprecate the current map again, if value does not fit.
  if (MigrateDeprecated(object)) {
    Handle<Object> result;
    ASSIGN_RETURN_ON_EXCEPTION(
        isolate(), result, Runtime::SetObjectProperty(isolate(), object, key,
                                                      value, language_mode()),
        Object);
    return result;
  }

  // Check for non-string values that can be converted into an
  // internalized string directly or is representable as a smi.
  key = TryConvertKey(key, isolate());

  Handle<Object> store_handle;

  uint32_t index;
  if ((key->IsInternalizedString() &&
       !String::cast(*key)->AsArrayIndex(&index)) ||
      key->IsSymbol()) {
    ASSIGN_RETURN_ON_EXCEPTION(
        isolate(), store_handle,
        StoreIC::Store(object, Handle<Name>::cast(key), value,
                       JSReceiver::MAY_BE_STORE_FROM_KEYED),
        Object);
    if (!is_vector_set()) {
      ConfigureVectorState(MEGAMORPHIC, key);
      TRACE_GENERIC_IC("unhandled internalized string key");
      TRACE_IC("StoreIC", key);
    }
    return store_handle;
  }

  if (state() != UNINITIALIZED) {
    JSObject::MakePrototypesFast(object, kStartAtPrototype, isolate());
  }

  bool use_ic = FLAG_use_ic && !object->IsStringWrapper() &&
                !object->IsAccessCheckNeeded() && !object->IsJSGlobalProxy();
  if (use_ic && !object->IsSmi()) {
    // Don't use ICs for maps of the objects in Array's prototype chain. We
    // expect to be able to trap element sets to objects with those maps in
    // the runtime to enable optimization of element hole access.
    Handle<HeapObject> heap_object = Handle<HeapObject>::cast(object);
    if (heap_object->map()->IsMapInArrayPrototypeChain()) {
      TRACE_GENERIC_IC("map in array prototype");
      use_ic = false;
    }
  }

  Handle<Map> old_receiver_map;
  bool is_arguments = false;
  bool key_is_valid_index = false;
  KeyedAccessStoreMode store_mode = STANDARD_STORE;
  if (use_ic && object->IsJSObject()) {
    Handle<JSObject> receiver = Handle<JSObject>::cast(object);
    old_receiver_map = handle(receiver->map(), isolate());
    is_arguments = receiver->IsJSArgumentsObject();
    if (!is_arguments) {
      key_is_valid_index = key->IsSmi() && Smi::ToInt(*key) >= 0;
      if (key_is_valid_index) {
        uint32_t index = static_cast<uint32_t>(Smi::ToInt(*key));
        store_mode = GetStoreMode(receiver, index, value);
      }
    }
  }

  DCHECK(store_handle.is_null());
  ASSIGN_RETURN_ON_EXCEPTION(isolate(), store_handle,
                             Runtime::SetObjectProperty(isolate(), object, key,
                                                        value, language_mode()),
                             Object);

  if (use_ic) {
    if (!old_receiver_map.is_null()) {
      if (is_arguments) {
        TRACE_GENERIC_IC("arguments receiver");
      } else if (key_is_valid_index) {
        if (old_receiver_map->is_abandoned_prototype_map()) {
          TRACE_GENERIC_IC("receiver with prototype map");
        } else if (!old_receiver_map
                        ->DictionaryElementsInPrototypeChainOnly()) {
          // We should go generic if receiver isn't a dictionary, but our
          // prototype chain does have dictionary elements. This ensures that
          // other non-dictionary receivers in the polymorphic case benefit
          // from fast path keyed stores.
          UpdateStoreElement(old_receiver_map, store_mode);
        } else {
          TRACE_GENERIC_IC("dictionary or proxy prototype");
        }
      } else {
        TRACE_GENERIC_IC("non-smi-like key");
      }
    } else {
      TRACE_GENERIC_IC("non-JSObject receiver");
    }
  }

  if (!is_vector_set()) {
    ConfigureVectorState(MEGAMORPHIC, key);
  }
  TRACE_IC("StoreIC", key);

  return store_handle;
}


#undef TRACE_IC


// ----------------------------------------------------------------------------
// Static IC stub generators.
//

// Used from ic-<arch>.cc.
RUNTIME_FUNCTION(Runtime_LoadIC_Miss) {
  HandleScope scope(isolate);
  DCHECK_EQ(4, args.length());
  // Runtime functions don't follow the IC's calling convention.
  Handle<Object> receiver = args.at(0);
  Handle<Name> key = args.at<Name>(1);
  Handle<Smi> slot = args.at<Smi>(2);
  Handle<FeedbackVector> vector = args.at<FeedbackVector>(3);
  FeedbackSlot vector_slot = vector->ToSlot(slot->value());
  // A monomorphic or polymorphic KeyedLoadIC with a string key can call the
  // LoadIC miss handler if the handler misses. Since the vector Nexus is
  // set up outside the IC, handle that here.
  FeedbackSlotKind kind = vector->GetKind(vector_slot);
  if (IsLoadICKind(kind)) {
    LoadICNexus nexus(vector, vector_slot);
    LoadIC ic(isolate, &nexus);
    ic.UpdateState(receiver, key);
    RETURN_RESULT_OR_FAILURE(isolate, ic.Load(receiver, key));

  } else if (IsLoadGlobalICKind(kind)) {
    DCHECK_EQ(isolate->native_context()->global_proxy(), *receiver);
    receiver = isolate->global_object();
    LoadGlobalICNexus nexus(vector, vector_slot);
    LoadGlobalIC ic(isolate, &nexus);
    ic.UpdateState(receiver, key);
    RETURN_RESULT_OR_FAILURE(isolate, ic.Load(key));

  } else {
    DCHECK(IsKeyedLoadICKind(kind));
    KeyedLoadICNexus nexus(vector, vector_slot);
    KeyedLoadIC ic(isolate, &nexus);
    ic.UpdateState(receiver, key);
    RETURN_RESULT_OR_FAILURE(isolate, ic.Load(receiver, key));
  }
}

// Used from ic-<arch>.cc.
RUNTIME_FUNCTION(Runtime_LoadGlobalIC_Miss) {
  HandleScope scope(isolate);
  DCHECK_EQ(3, args.length());
  // Runtime functions don't follow the IC's calling convention.
  Handle<JSGlobalObject> global = isolate->global_object();
  Handle<String> name = args.at<String>(0);
  Handle<Smi> slot = args.at<Smi>(1);
  Handle<FeedbackVector> vector = args.at<FeedbackVector>(2);
  FeedbackSlot vector_slot = vector->ToSlot(slot->value());

  LoadGlobalICNexus nexus(vector, vector_slot);
  LoadGlobalIC ic(isolate, &nexus);
  ic.UpdateState(global, name);

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result, ic.Load(name));
  return *result;
}

RUNTIME_FUNCTION(Runtime_LoadGlobalIC_Slow) {
  HandleScope scope(isolate);
  DCHECK_EQ(3, args.length());
  CONVERT_ARG_HANDLE_CHECKED(String, name, 0);

  Handle<Context> native_context = isolate->native_context();
  Handle<ScriptContextTable> script_contexts(
      native_context->script_context_table());

  ScriptContextTable::LookupResult lookup_result;
  if (ScriptContextTable::Lookup(script_contexts, name, &lookup_result)) {
    Handle<Context> script_context = ScriptContextTable::GetContext(
        script_contexts, lookup_result.context_index);
    Handle<Object> result =
        FixedArray::get(*script_context, lookup_result.slot_index, isolate);
    if (*result == isolate->heap()->the_hole_value()) {
      THROW_NEW_ERROR_RETURN_FAILURE(
          isolate, NewReferenceError(MessageTemplate::kNotDefined, name));
    }
    return *result;
  }

  Handle<JSGlobalObject> global(native_context->global_object(), isolate);
  Handle<Object> result;
  bool is_found = false;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      Runtime::GetObjectProperty(isolate, global, name, &is_found));
  if (!is_found) {
    Handle<Smi> slot = args.at<Smi>(1);
    Handle<FeedbackVector> vector = args.at<FeedbackVector>(2);
    FeedbackSlot vector_slot = vector->ToSlot(slot->value());
    FeedbackSlotKind kind = vector->GetKind(vector_slot);
    // It is actually a LoadGlobalICs here but the predicate handles this case
    // properly.
    if (LoadIC::ShouldThrowReferenceError(kind)) {
      THROW_NEW_ERROR_RETURN_FAILURE(
          isolate, NewReferenceError(MessageTemplate::kNotDefined, name));
    }
  }
  return *result;
}

// Used from ic-<arch>.cc
RUNTIME_FUNCTION(Runtime_KeyedLoadIC_Miss) {
  HandleScope scope(isolate);
  DCHECK_EQ(4, args.length());
  // Runtime functions don't follow the IC's calling convention.
  Handle<Object> receiver = args.at(0);
  Handle<Object> key = args.at(1);
  Handle<Smi> slot = args.at<Smi>(2);
  Handle<FeedbackVector> vector = args.at<FeedbackVector>(3);
  FeedbackSlot vector_slot = vector->ToSlot(slot->value());
  KeyedLoadICNexus nexus(vector, vector_slot);
  KeyedLoadIC ic(isolate, &nexus);
  ic.UpdateState(receiver, key);
  RETURN_RESULT_OR_FAILURE(isolate, ic.Load(receiver, key));
}

// Used from ic-<arch>.cc.
RUNTIME_FUNCTION(Runtime_StoreIC_Miss) {
  HandleScope scope(isolate);
  DCHECK_EQ(5, args.length());
  // Runtime functions don't follow the IC's calling convention.
  Handle<Object> value = args.at(0);
  Handle<Smi> slot = args.at<Smi>(1);
  Handle<FeedbackVector> vector = args.at<FeedbackVector>(2);
  Handle<Object> receiver = args.at(3);
  Handle<Name> key = args.at<Name>(4);
  FeedbackSlot vector_slot = vector->ToSlot(slot->value());
  FeedbackSlotKind kind = vector->GetKind(vector_slot);
  if (IsStoreICKind(kind) || IsStoreOwnICKind(kind)) {
    StoreICNexus nexus(vector, vector_slot);
    StoreIC ic(isolate, &nexus);
    ic.UpdateState(receiver, key);
    RETURN_RESULT_OR_FAILURE(isolate, ic.Store(receiver, key, value));
  } else if (IsStoreGlobalICKind(kind)) {
    StoreICNexus nexus(vector, vector_slot);
    StoreGlobalIC ic(isolate, &nexus);
    ic.UpdateState(receiver, key);
    RETURN_RESULT_OR_FAILURE(isolate, ic.Store(receiver, key, value));
  } else {
    DCHECK(IsKeyedStoreICKind(kind));
    KeyedStoreICNexus nexus(vector, vector_slot);
    KeyedStoreIC ic(isolate, &nexus);
    ic.UpdateState(receiver, key);
    RETURN_RESULT_OR_FAILURE(isolate, ic.Store(receiver, key, value));
  }
}

// Used from ic-<arch>.cc.
RUNTIME_FUNCTION(Runtime_KeyedStoreIC_Miss) {
  HandleScope scope(isolate);
  DCHECK_EQ(5, args.length());
  // Runtime functions don't follow the IC's calling convention.
  Handle<Object> value = args.at(0);
  Handle<Smi> slot = args.at<Smi>(1);
  Handle<FeedbackVector> vector = args.at<FeedbackVector>(2);
  Handle<Object> receiver = args.at(3);
  Handle<Object> key = args.at(4);
  FeedbackSlot vector_slot = vector->ToSlot(slot->value());
  KeyedStoreICNexus nexus(vector, vector_slot);
  KeyedStoreIC ic(isolate, &nexus);
  ic.UpdateState(receiver, key);
  RETURN_RESULT_OR_FAILURE(isolate, ic.Store(receiver, key, value));
}


RUNTIME_FUNCTION(Runtime_KeyedStoreIC_Slow) {
  HandleScope scope(isolate);
  DCHECK_EQ(5, args.length());
  // Runtime functions don't follow the IC's calling convention.
  Handle<Object> value = args.at(0);
  Handle<Smi> slot = args.at<Smi>(1);
  Handle<FeedbackVector> vector = args.at<FeedbackVector>(2);
  Handle<Object> object = args.at(3);
  Handle<Object> key = args.at(4);
  FeedbackSlot vector_slot = vector->ToSlot(slot->value());
  LanguageMode language_mode = vector->GetLanguageMode(vector_slot);
  RETURN_RESULT_OR_FAILURE(
      isolate,
      Runtime::SetObjectProperty(isolate, object, key, value, language_mode));
}


RUNTIME_FUNCTION(Runtime_ElementsTransitionAndStoreIC_Miss) {
  HandleScope scope(isolate);
  DCHECK_EQ(6, args.length());
  // Runtime functions don't follow the IC's calling convention.
  Handle<Object> object = args.at(0);
  Handle<Object> key = args.at(1);
  Handle<Object> value = args.at(2);
  Handle<Map> map = args.at<Map>(3);
  Handle<Smi> slot = args.at<Smi>(4);
  Handle<FeedbackVector> vector = args.at<FeedbackVector>(5);
  FeedbackSlot vector_slot = vector->ToSlot(slot->value());
  LanguageMode language_mode = vector->GetLanguageMode(vector_slot);
  if (object->IsJSObject()) {
    JSObject::TransitionElementsKind(Handle<JSObject>::cast(object),
                                     map->elements_kind());
  }
  RETURN_RESULT_OR_FAILURE(
      isolate,
      Runtime::SetObjectProperty(isolate, object, key, value, language_mode));
}


RUNTIME_FUNCTION(Runtime_Unreachable) {
  UNREACHABLE();
  CHECK(false);
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_StoreCallbackProperty) {
  Handle<JSObject> receiver = args.at<JSObject>(0);
  Handle<JSObject> holder = args.at<JSObject>(1);
  Handle<HeapObject> callback_or_cell = args.at<HeapObject>(2);
  Handle<Name> name = args.at<Name>(3);
  Handle<Object> value = args.at(4);
  CONVERT_LANGUAGE_MODE_ARG_CHECKED(language_mode, 5);
  HandleScope scope(isolate);

  if (V8_UNLIKELY(FLAG_runtime_stats)) {
    RETURN_RESULT_OR_FAILURE(
        isolate, Runtime::SetObjectProperty(isolate, receiver, name, value,
                                            language_mode));
  }

  Handle<AccessorInfo> callback(
      callback_or_cell->IsWeakCell()
          ? AccessorInfo::cast(WeakCell::cast(*callback_or_cell)->value())
          : AccessorInfo::cast(*callback_or_cell));

  DCHECK(callback->IsCompatibleReceiver(*receiver));

  Address setter_address = v8::ToCData<Address>(callback->setter());
  v8::AccessorNameSetterCallback fun =
      FUNCTION_CAST<v8::AccessorNameSetterCallback>(setter_address);
  DCHECK(fun != NULL);

  Object::ShouldThrow should_throw =
      is_sloppy(language_mode) ? Object::DONT_THROW : Object::THROW_ON_ERROR;
  PropertyCallbackArguments custom_args(isolate, callback->data(), *receiver,
                                        *holder, should_throw);
  custom_args.Call(fun, name, value);
  RETURN_FAILURE_IF_SCHEDULED_EXCEPTION(isolate);
  return *value;
}


/**
 * Loads a property with an interceptor performing post interceptor
 * lookup if interceptor failed.
 */
RUNTIME_FUNCTION(Runtime_LoadPropertyWithInterceptor) {
  HandleScope scope(isolate);
  DCHECK_EQ(5, args.length());
  Handle<Name> name = args.at<Name>(0);
  Handle<Object> receiver = args.at(1);
  Handle<JSObject> holder = args.at<JSObject>(2);

  if (!receiver->IsJSReceiver()) {
    ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
        isolate, receiver, Object::ConvertReceiver(isolate, receiver));
  }

  InterceptorInfo* interceptor = holder->GetNamedInterceptor();
  PropertyCallbackArguments arguments(isolate, interceptor->data(), *receiver,
                                      *holder, Object::DONT_THROW);

  v8::GenericNamedPropertyGetterCallback getter =
      v8::ToCData<v8::GenericNamedPropertyGetterCallback>(
          interceptor->getter());
  Handle<Object> result = arguments.Call(getter, name);

  RETURN_FAILURE_IF_SCHEDULED_EXCEPTION(isolate);

  if (!result.is_null()) return *result;

  LookupIterator it(receiver, name, holder);
  // Skip any lookup work until we hit the (possibly non-masking) interceptor.
  while (it.state() != LookupIterator::INTERCEPTOR ||
         !it.GetHolder<JSObject>().is_identical_to(holder)) {
    DCHECK(it.state() != LookupIterator::ACCESS_CHECK || it.HasAccess());
    it.Next();
  }
  // Skip past the interceptor.
  it.Next();
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result, Object::GetProperty(&it));

  if (it.IsFound()) return *result;

  Handle<Smi> slot = args.at<Smi>(3);
  Handle<FeedbackVector> vector = args.at<FeedbackVector>(4);
  FeedbackSlot vector_slot = vector->ToSlot(slot->value());
  FeedbackSlotKind slot_kind = vector->GetKind(vector_slot);
  // It could actually be any kind of load IC slot here but the predicate
  // handles all the cases properly.
  if (!LoadIC::ShouldThrowReferenceError(slot_kind)) {
    return isolate->heap()->undefined_value();
  }

  // Throw a reference error.
  THROW_NEW_ERROR_RETURN_FAILURE(
      isolate, NewReferenceError(MessageTemplate::kNotDefined, it.name()));
}


RUNTIME_FUNCTION(Runtime_StorePropertyWithInterceptor) {
  HandleScope scope(isolate);
  DCHECK_EQ(5, args.length());
  // Runtime functions don't follow the IC's calling convention.
  Handle<Object> value = args.at(0);
  Handle<Smi> slot = args.at<Smi>(1);
  Handle<FeedbackVector> vector = args.at<FeedbackVector>(2);
  Handle<JSObject> receiver = args.at<JSObject>(3);
  Handle<Name> name = args.at<Name>(4);
  FeedbackSlot vector_slot = vector->ToSlot(slot->value());
  LanguageMode language_mode = vector->GetLanguageMode(vector_slot);

  DCHECK(receiver->HasNamedInterceptor());
  InterceptorInfo* interceptor = receiver->GetNamedInterceptor();
  DCHECK(!interceptor->non_masking());
  PropertyCallbackArguments arguments(isolate, interceptor->data(), *receiver,
                                      *receiver, Object::DONT_THROW);

  v8::GenericNamedPropertySetterCallback setter =
      v8::ToCData<v8::GenericNamedPropertySetterCallback>(
          interceptor->setter());
  Handle<Object> result = arguments.Call(setter, name, value);
  RETURN_FAILURE_IF_SCHEDULED_EXCEPTION(isolate);
  if (!result.is_null()) return *value;

  LookupIterator it(receiver, name, receiver);
  // Skip past any access check on the receiver.
  if (it.state() == LookupIterator::ACCESS_CHECK) {
    DCHECK(it.HasAccess());
    it.Next();
  }
  // Skip past the interceptor on the receiver.
  DCHECK_EQ(LookupIterator::INTERCEPTOR, it.state());
  it.Next();

  MAYBE_RETURN(Object::SetProperty(&it, value, language_mode,
                                   JSReceiver::CERTAINLY_NOT_STORE_FROM_KEYED),
               isolate->heap()->exception());
  return *value;
}


RUNTIME_FUNCTION(Runtime_LoadElementWithInterceptor) {
  // TODO(verwaest): This should probably get the holder and receiver as input.
  HandleScope scope(isolate);
  Handle<JSObject> receiver = args.at<JSObject>(0);
  DCHECK(args.smi_at(1) >= 0);
  uint32_t index = args.smi_at(1);

  InterceptorInfo* interceptor = receiver->GetIndexedInterceptor();
  PropertyCallbackArguments arguments(isolate, interceptor->data(), *receiver,
                                      *receiver, Object::DONT_THROW);

  v8::IndexedPropertyGetterCallback getter =
      v8::ToCData<v8::IndexedPropertyGetterCallback>(interceptor->getter());
  Handle<Object> result = arguments.Call(getter, index);

  RETURN_FAILURE_IF_SCHEDULED_EXCEPTION(isolate);

  if (result.is_null()) {
    LookupIterator it(isolate, receiver, index, receiver);
    DCHECK_EQ(LookupIterator::INTERCEPTOR, it.state());
    it.Next();
    ASSIGN_RETURN_FAILURE_ON_EXCEPTION(isolate, result,
                                       Object::GetProperty(&it));
  }

  return *result;
}
}  // namespace internal
}  // namespace v8
