// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unordered_map>

#include "src/assembler-inl.h"
#include "src/assert-scope.h"
#include "src/compiler/wasm-compiler.h"
#include "src/debug/debug-scopes.h"
#include "src/debug/debug.h"
#include "src/factory.h"
#include "src/frames-inl.h"
#include "src/identity-map.h"
#include "src/isolate.h"
#include "src/wasm/module-decoder.h"
#include "src/wasm/wasm-interpreter.h"
#include "src/wasm/wasm-limits.h"
#include "src/wasm/wasm-module.h"
#include "src/wasm/wasm-objects.h"
#include "src/zone/accounting-allocator.h"

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wheader-hygiene"
#endif

using namespace v8::internal;
using namespace v8::internal::wasm;

#if __clang__
#pragma clang diagnostic pop
#endif

namespace {

template <bool internal, typename... Args>
Handle<String> PrintFToOneByteString(Isolate* isolate, const char* format,
                                     Args... args) {
  // Maximum length of a formatted value name ("param#%d", "local#%d",
  // "global#%d").
  constexpr int kMaxStrLen = 18;
  EmbeddedVector<char, kMaxStrLen> value;
  int len = SNPrintF(value, format, args...);
  CHECK(len > 0 && len < value.length());
  Vector<uint8_t> name = Vector<uint8_t>::cast(value.SubVector(0, len));
  return internal
             ? isolate->factory()->InternalizeOneByteString(name)
             : isolate->factory()->NewStringFromOneByte(name).ToHandleChecked();
}

Handle<Object> WasmValueToValueObject(Isolate* isolate, WasmValue value) {
  switch (value.type()) {
    case kWasmI32:
      if (Smi::IsValid(value.to<int32_t>()))
        return handle(Smi::FromInt(value.to<int32_t>()), isolate);
      return PrintFToOneByteString<false>(isolate, "%d", value.to<int32_t>());
    case kWasmI64:
      if (Smi::IsValid(value.to<int64_t>()))
        return handle(Smi::FromIntptr(value.to<int64_t>()), isolate);
      return PrintFToOneByteString<false>(isolate, "%" PRId64,
                                          value.to<int64_t>());
    case kWasmF32:
      return isolate->factory()->NewNumber(value.to<float>());
    case kWasmF64:
      return isolate->factory()->NewNumber(value.to<double>());
    default:
      UNIMPLEMENTED();
      return isolate->factory()->undefined_value();
  }
}

MaybeHandle<String> GetLocalName(Isolate* isolate,
                                 Handle<WasmDebugInfo> debug_info,
                                 int func_index, int local_index) {
  DCHECK_LE(0, func_index);
  DCHECK_LE(0, local_index);
  if (!debug_info->has_locals_names()) {
    Handle<WasmCompiledModule> compiled_module(
        debug_info->wasm_instance()->compiled_module(), isolate);
    Handle<FixedArray> locals_names =
        wasm::DecodeLocalNames(isolate, compiled_module);
    debug_info->set_locals_names(*locals_names);
  }

  Handle<FixedArray> locals_names(debug_info->locals_names(), isolate);
  if (func_index >= locals_names->length() ||
      locals_names->get(func_index)->IsUndefined(isolate)) {
    return {};
  }

  Handle<FixedArray> func_locals_names(
      FixedArray::cast(locals_names->get(func_index)), isolate);
  if (local_index >= func_locals_names->length() ||
      func_locals_names->get(local_index)->IsUndefined(isolate)) {
    return {};
  }
  return handle(String::cast(func_locals_names->get(local_index)));
}

// Forward declaration.
class InterpreterHandle;
InterpreterHandle* GetInterpreterHandle(WasmDebugInfo* debug_info);

class InterpreterHandle {
  MOVE_ONLY_NO_DEFAULT_CONSTRUCTOR(InterpreterHandle);
  Isolate* isolate_;
  const WasmModule* module_;
  WasmInterpreter interpreter_;
  StepAction next_step_action_ = StepNone;
  int last_step_stack_depth_ = 0;
  std::unordered_map<Address, uint32_t> activations_;

  uint32_t StartActivation(Address frame_pointer) {
    WasmInterpreter::Thread* thread = interpreter_.GetThread(0);
    uint32_t activation_id = thread->StartActivation();
    DCHECK_EQ(0, activations_.count(frame_pointer));
    activations_.insert(std::make_pair(frame_pointer, activation_id));
    return activation_id;
  }

  void FinishActivation(Address frame_pointer, uint32_t activation_id) {
    WasmInterpreter::Thread* thread = interpreter_.GetThread(0);
    thread->FinishActivation(activation_id);
    DCHECK_EQ(1, activations_.count(frame_pointer));
    activations_.erase(frame_pointer);
  }

  std::pair<uint32_t, uint32_t> GetActivationFrameRange(
      WasmInterpreter::Thread* thread, Address frame_pointer) {
    DCHECK_EQ(1, activations_.count(frame_pointer));
    uint32_t activation_id = activations_.find(frame_pointer)->second;
    uint32_t num_activations = static_cast<uint32_t>(activations_.size() - 1);
    uint32_t frame_base = thread->ActivationFrameBase(activation_id);
    uint32_t frame_limit = activation_id == num_activations
                               ? thread->GetFrameCount()
                               : thread->ActivationFrameBase(activation_id + 1);
    DCHECK_LE(frame_base, frame_limit);
    DCHECK_LE(frame_limit, thread->GetFrameCount());
    return {frame_base, frame_limit};
  }

  static Vector<const byte> GetBytes(WasmDebugInfo* debug_info) {
    // Return raw pointer into heap. The WasmInterpreter will make its own copy
    // of this data anyway, and there is no heap allocation in-between.
    SeqOneByteString* bytes_str =
        debug_info->wasm_instance()->compiled_module()->module_bytes();
    return {bytes_str->GetChars(), static_cast<size_t>(bytes_str->length())};
  }

  static uint32_t GetMemSize(WasmDebugInfo* debug_info) {
    DisallowHeapAllocation no_gc;
    WasmCompiledModule* compiled_module =
        debug_info->wasm_instance()->compiled_module();
    return compiled_module->has_embedded_mem_size()
               ? compiled_module->embedded_mem_size()
               : 0;
  }

  static byte* GetMemStart(WasmDebugInfo* debug_info) {
    DisallowHeapAllocation no_gc;
    WasmCompiledModule* compiled_module =
        debug_info->wasm_instance()->compiled_module();
    return reinterpret_cast<byte*>(compiled_module->has_embedded_mem_start()
                                       ? compiled_module->embedded_mem_start()
                                       : 0);
  }

  static byte* GetGlobalsStart(WasmDebugInfo* debug_info) {
    DisallowHeapAllocation no_gc;
    WasmCompiledModule* compiled_module =
        debug_info->wasm_instance()->compiled_module();
    return reinterpret_cast<byte*>(compiled_module->has_globals_start()
                                       ? compiled_module->globals_start()
                                       : 0);
  }

 public:
  InterpreterHandle(Isolate* isolate, WasmDebugInfo* debug_info)
      : isolate_(isolate),
        module_(debug_info->wasm_instance()->compiled_module()->module()),
        interpreter_(isolate, module_, GetBytes(debug_info),
                     GetGlobalsStart(debug_info), GetMemStart(debug_info),
                     GetMemSize(debug_info)) {}

  ~InterpreterHandle() { DCHECK_EQ(0, activations_.size()); }

  WasmInterpreter* interpreter() { return &interpreter_; }
  const WasmModule* module() const { return module_; }

  void PrepareStep(StepAction step_action) {
    next_step_action_ = step_action;
    last_step_stack_depth_ = CurrentStackDepth();
  }

  void ClearStepping() { next_step_action_ = StepNone; }

  int CurrentStackDepth() {
    DCHECK_EQ(1, interpreter()->GetThreadCount());
    return interpreter()->GetThread(0)->GetFrameCount();
  }

  // Returns true if exited regularly, false if a trap/exception occurred and
  // was not handled inside this activation. In the latter case, a pending
  // exception will have been set on the isolate.
  bool Execute(Handle<WasmInstanceObject> instance_object,
               Address frame_pointer, uint32_t func_index,
               uint8_t* arg_buffer) {
    DCHECK_GE(module()->functions.size(), func_index);
    FunctionSig* sig = module()->functions[func_index].sig;
    DCHECK_GE(kMaxInt, sig->parameter_count());
    int num_params = static_cast<int>(sig->parameter_count());
    ScopedVector<WasmValue> wasm_args(num_params);
    uint8_t* arg_buf_ptr = arg_buffer;
    for (int i = 0; i < num_params; ++i) {
      uint32_t param_size = 1 << ElementSizeLog2Of(sig->GetParam(i));
#define CASE_ARG_TYPE(type, ctype)                                    \
  case type:                                                          \
    DCHECK_EQ(param_size, sizeof(ctype));                             \
    wasm_args[i] = WasmValue(ReadUnalignedValue<ctype>(arg_buf_ptr)); \
    break;
      switch (sig->GetParam(i)) {
        CASE_ARG_TYPE(kWasmI32, uint32_t)
        CASE_ARG_TYPE(kWasmI64, uint64_t)
        CASE_ARG_TYPE(kWasmF32, float)
        CASE_ARG_TYPE(kWasmF64, double)
#undef CASE_ARG_TYPE
        default:
          UNREACHABLE();
      }
      arg_buf_ptr += param_size;
    }

    uint32_t activation_id = StartActivation(frame_pointer);

    WasmInterpreter::HeapObjectsScope heap_objects_scope(&interpreter_,
                                                         instance_object);
    WasmInterpreter::Thread* thread = interpreter_.GetThread(0);
    thread->InitFrame(&module()->functions[func_index], wasm_args.start());
    bool finished = false;
    while (!finished) {
      // TODO(clemensh): Add occasional StackChecks.
      WasmInterpreter::State state = ContinueExecution(thread);
      switch (state) {
        case WasmInterpreter::State::PAUSED:
          NotifyDebugEventListeners(thread);
          break;
        case WasmInterpreter::State::FINISHED:
          // Perfect, just break the switch and exit the loop.
          finished = true;
          break;
        case WasmInterpreter::State::TRAPPED: {
          int message_id =
              WasmOpcodes::TrapReasonToMessageId(thread->GetTrapReason());
          Handle<Object> exception = isolate_->factory()->NewWasmRuntimeError(
              static_cast<MessageTemplate::Template>(message_id));
          isolate_->Throw(*exception);
          // Handle this exception. Return without trying to read back the
          // return value.
          auto result = thread->HandleException(isolate_);
          return result == WasmInterpreter::Thread::HANDLED;
        } break;
        case WasmInterpreter::State::STOPPED:
          // An exception happened, and the current activation was unwound.
          DCHECK_EQ(thread->ActivationFrameBase(activation_id),
                    thread->GetFrameCount());
          return false;
        // RUNNING should never occur here.
        case WasmInterpreter::State::RUNNING:
        default:
          UNREACHABLE();
      }
    }

    // Copy back the return value
    DCHECK_GE(kV8MaxWasmFunctionReturns, sig->return_count());
    // TODO(wasm): Handle multi-value returns.
    DCHECK_EQ(1, kV8MaxWasmFunctionReturns);
    if (sig->return_count()) {
      WasmValue ret_val = thread->GetReturnValue(0);
#define CASE_RET_TYPE(type, ctype)                                       \
  case type:                                                             \
    DCHECK_EQ(1 << ElementSizeLog2Of(sig->GetReturn(0)), sizeof(ctype)); \
    WriteUnalignedValue<ctype>(arg_buffer, ret_val.to<ctype>());         \
    break;
      switch (sig->GetReturn(0)) {
        CASE_RET_TYPE(kWasmI32, uint32_t)
        CASE_RET_TYPE(kWasmI64, uint64_t)
        CASE_RET_TYPE(kWasmF32, float)
        CASE_RET_TYPE(kWasmF64, double)
#undef CASE_RET_TYPE
        default:
          UNREACHABLE();
      }
    }

    FinishActivation(frame_pointer, activation_id);

    return true;
  }

  WasmInterpreter::State ContinueExecution(WasmInterpreter::Thread* thread) {
    switch (next_step_action_) {
      case StepNone:
        return thread->Run();
      case StepIn:
        return thread->Step();
      case StepOut:
        thread->AddBreakFlags(WasmInterpreter::BreakFlag::AfterReturn);
        return thread->Run();
      case StepNext: {
        int stack_depth = thread->GetFrameCount();
        if (stack_depth == last_step_stack_depth_) return thread->Step();
        thread->AddBreakFlags(stack_depth > last_step_stack_depth_
                                  ? WasmInterpreter::BreakFlag::AfterReturn
                                  : WasmInterpreter::BreakFlag::AfterCall);
        return thread->Run();
      }
      default:
        UNREACHABLE();
    }
  }

  Handle<WasmInstanceObject> GetInstanceObject() {
    StackTraceFrameIterator it(isolate_);
    WasmInterpreterEntryFrame* frame =
        WasmInterpreterEntryFrame::cast(it.frame());
    Handle<WasmInstanceObject> instance_obj(frame->wasm_instance(), isolate_);
    DCHECK_EQ(this, GetInterpreterHandle(instance_obj->debug_info()));
    return instance_obj;
  }

  void NotifyDebugEventListeners(WasmInterpreter::Thread* thread) {
    // Enter the debugger.
    DebugScope debug_scope(isolate_->debug());
    if (debug_scope.failed()) return;

    // Postpone interrupt during breakpoint processing.
    PostponeInterruptsScope postpone(isolate_);

    // Check whether we hit a breakpoint.
    if (isolate_->debug()->break_points_active()) {
      Handle<WasmCompiledModule> compiled_module(
          GetInstanceObject()->compiled_module(), isolate_);
      int position = GetTopPosition(compiled_module);
      Handle<FixedArray> breakpoints;
      if (compiled_module->CheckBreakPoints(position).ToHandle(&breakpoints)) {
        // We hit one or several breakpoints. Clear stepping, notify the
        // listeners and return.
        ClearStepping();
        isolate_->debug()->OnDebugBreak(breakpoints);
        return;
      }
    }

    // We did not hit a breakpoint, so maybe this pause is related to stepping.
    bool hit_step = false;
    switch (next_step_action_) {
      case StepNone:
        break;
      case StepIn:
        hit_step = true;
        break;
      case StepOut:
        hit_step = thread->GetFrameCount() < last_step_stack_depth_;
        break;
      case StepNext: {
        hit_step = thread->GetFrameCount() == last_step_stack_depth_;
        break;
      }
      default:
        UNREACHABLE();
    }
    if (!hit_step) return;
    ClearStepping();
    isolate_->debug()->OnDebugBreak(isolate_->factory()->empty_fixed_array());
  }

  int GetTopPosition(Handle<WasmCompiledModule> compiled_module) {
    DCHECK_EQ(1, interpreter()->GetThreadCount());
    WasmInterpreter::Thread* thread = interpreter()->GetThread(0);
    DCHECK_LT(0, thread->GetFrameCount());

    auto frame = thread->GetFrame(thread->GetFrameCount() - 1);
    return compiled_module->GetFunctionOffset(frame->function()->func_index) +
           frame->pc();
  }

  std::vector<std::pair<uint32_t, int>> GetInterpretedStack(
      Address frame_pointer) {
    DCHECK_EQ(1, interpreter()->GetThreadCount());
    WasmInterpreter::Thread* thread = interpreter()->GetThread(0);

    std::pair<uint32_t, uint32_t> frame_range =
        GetActivationFrameRange(thread, frame_pointer);

    std::vector<std::pair<uint32_t, int>> stack;
    stack.reserve(frame_range.second - frame_range.first);
    for (uint32_t fp = frame_range.first; fp < frame_range.second; ++fp) {
      auto frame = thread->GetFrame(fp);
      stack.emplace_back(frame->function()->func_index, frame->pc());
    }
    return stack;
  }

  std::unique_ptr<wasm::InterpretedFrame> GetInterpretedFrame(
      Address frame_pointer, int idx) {
    DCHECK_EQ(1, interpreter()->GetThreadCount());
    WasmInterpreter::Thread* thread = interpreter()->GetThread(0);

    std::pair<uint32_t, uint32_t> frame_range =
        GetActivationFrameRange(thread, frame_pointer);
    DCHECK_LE(0, idx);
    DCHECK_GT(frame_range.second - frame_range.first, idx);

    return thread->GetFrame(frame_range.first + idx);
  }

  void Unwind(Address frame_pointer) {
    // Find the current activation.
    DCHECK_EQ(1, activations_.count(frame_pointer));
    // Activations must be properly stacked:
    DCHECK_EQ(activations_.size() - 1, activations_[frame_pointer]);
    uint32_t activation_id = static_cast<uint32_t>(activations_.size() - 1);

    // Unwind the frames of the current activation if not already unwound.
    WasmInterpreter::Thread* thread = interpreter()->GetThread(0);
    if (static_cast<uint32_t>(thread->GetFrameCount()) >
        thread->ActivationFrameBase(activation_id)) {
      using ExceptionResult = WasmInterpreter::Thread::ExceptionHandlingResult;
      ExceptionResult result = thread->HandleException(isolate_);
      // TODO(wasm): Handle exceptions caught in wasm land.
      CHECK_EQ(ExceptionResult::UNWOUND, result);
    }

    FinishActivation(frame_pointer, activation_id);
  }

  uint64_t NumInterpretedCalls() {
    DCHECK_EQ(1, interpreter()->GetThreadCount());
    return interpreter()->GetThread(0)->NumInterpretedCalls();
  }

  void UpdateMemory(JSArrayBuffer* new_memory) {
    byte* mem_start = reinterpret_cast<byte*>(new_memory->backing_store());
    uint32_t mem_size;
    CHECK(new_memory->byte_length()->ToUint32(&mem_size));
    interpreter()->UpdateMemory(mem_start, mem_size);
  }

  Handle<JSObject> GetGlobalScopeObject(wasm::InterpretedFrame* frame,
                                        Handle<WasmDebugInfo> debug_info) {
    Isolate* isolate = debug_info->GetIsolate();
    Handle<WasmInstanceObject> instance(debug_info->wasm_instance(), isolate);

    // TODO(clemensh): Add globals to the global scope.
    Handle<JSObject> global_scope_object =
        isolate_->factory()->NewJSObjectWithNullProto();
    if (instance->has_memory_buffer()) {
      Handle<String> name = isolate_->factory()->InternalizeOneByteString(
          STATIC_CHAR_VECTOR("memory"));
      Handle<JSArrayBuffer> memory_buffer(instance->memory_buffer(), isolate_);
      uint32_t byte_length;
      CHECK(memory_buffer->byte_length()->ToUint32(&byte_length));
      Handle<JSTypedArray> uint8_array = isolate_->factory()->NewJSTypedArray(
          kExternalUint8Array, memory_buffer, 0, byte_length);
      JSObject::SetOwnPropertyIgnoreAttributes(global_scope_object, name,
                                               uint8_array, NONE)
          .Assert();
    }
    return global_scope_object;
  }

  Handle<JSObject> GetLocalScopeObject(wasm::InterpretedFrame* frame,
                                       Handle<WasmDebugInfo> debug_info) {
    Isolate* isolate = debug_info->GetIsolate();
    Handle<WasmInstanceObject> instance(debug_info->wasm_instance(), isolate);

    Handle<JSObject> local_scope_object =
        isolate_->factory()->NewJSObjectWithNullProto();
    // Fill parameters and locals.
    int num_params = frame->GetParameterCount();
    int num_locals = frame->GetLocalCount();
    DCHECK_LE(num_params, num_locals);
    if (num_locals > 0) {
      Handle<JSObject> locals_obj =
          isolate_->factory()->NewJSObjectWithNullProto();
      Handle<String> locals_name =
          isolate_->factory()->InternalizeOneByteString(
              STATIC_CHAR_VECTOR("locals"));
      JSObject::SetOwnPropertyIgnoreAttributes(local_scope_object, locals_name,
                                               locals_obj, NONE)
          .Assert();
      for (int i = 0; i < num_locals; ++i) {
        MaybeHandle<String> name =
            GetLocalName(isolate, debug_info, frame->function()->func_index, i);
        if (name.is_null()) {
          // Parameters should come before locals in alphabetical ordering, so
          // we name them "args" here.
          const char* label = i < num_params ? "arg#%d" : "local#%d";
          name = PrintFToOneByteString<true>(isolate_, label, i);
        }
        WasmValue value = frame->GetLocalValue(i);
        Handle<Object> value_obj = WasmValueToValueObject(isolate_, value);
        JSObject::SetOwnPropertyIgnoreAttributes(
            locals_obj, name.ToHandleChecked(), value_obj, NONE)
            .Assert();
      }
    }

    // Fill stack values.
    int stack_count = frame->GetStackHeight();
    // Use an object without prototype instead of an Array, for nicer displaying
    // in DevTools. For Arrays, the length field and prototype is displayed,
    // which does not make too much sense here.
    Handle<JSObject> stack_obj =
        isolate_->factory()->NewJSObjectWithNullProto();
    Handle<String> stack_name = isolate_->factory()->InternalizeOneByteString(
        STATIC_CHAR_VECTOR("stack"));
    JSObject::SetOwnPropertyIgnoreAttributes(local_scope_object, stack_name,
                                             stack_obj, NONE)
        .Assert();
    for (int i = 0; i < stack_count; ++i) {
      WasmValue value = frame->GetStackValue(i);
      Handle<Object> value_obj = WasmValueToValueObject(isolate_, value);
      JSObject::SetOwnElementIgnoreAttributes(
          stack_obj, static_cast<uint32_t>(i), value_obj, NONE)
          .Assert();
    }
    return local_scope_object;
  }

  Handle<JSArray> GetScopeDetails(Address frame_pointer, int frame_index,
                                  Handle<WasmDebugInfo> debug_info) {
    auto frame = GetInterpretedFrame(frame_pointer, frame_index);
    Isolate* isolate = debug_info->GetIsolate();
    Handle<WasmInstanceObject> instance(debug_info->wasm_instance(), isolate);

    Handle<FixedArray> global_scope =
        isolate_->factory()->NewFixedArray(ScopeIterator::kScopeDetailsSize);
    global_scope->set(ScopeIterator::kScopeDetailsTypeIndex,
                      Smi::FromInt(ScopeIterator::ScopeTypeGlobal));
    Handle<JSObject> global_scope_object =
        GetGlobalScopeObject(frame.get(), debug_info);
    global_scope->set(ScopeIterator::kScopeDetailsObjectIndex,
                      *global_scope_object);

    Handle<FixedArray> local_scope =
        isolate_->factory()->NewFixedArray(ScopeIterator::kScopeDetailsSize);
    local_scope->set(ScopeIterator::kScopeDetailsTypeIndex,
                     Smi::FromInt(ScopeIterator::ScopeTypeLocal));
    Handle<JSObject> local_scope_object =
        GetLocalScopeObject(frame.get(), debug_info);
    local_scope->set(ScopeIterator::kScopeDetailsObjectIndex,
                     *local_scope_object);

    Handle<JSArray> global_jsarr =
        isolate_->factory()->NewJSArrayWithElements(global_scope);
    Handle<JSArray> local_jsarr =
        isolate_->factory()->NewJSArrayWithElements(local_scope);
    Handle<FixedArray> all_scopes = isolate_->factory()->NewFixedArray(2);
    all_scopes->set(0, *global_jsarr);
    all_scopes->set(1, *local_jsarr);
    return isolate_->factory()->NewJSArrayWithElements(all_scopes);
  }
};

InterpreterHandle* GetOrCreateInterpreterHandle(
    Isolate* isolate, Handle<WasmDebugInfo> debug_info) {
  Handle<Object> handle(debug_info->get(WasmDebugInfo::kInterpreterHandleIndex),
                        isolate);
  if (handle->IsUndefined(isolate)) {
    InterpreterHandle* cpp_handle = new InterpreterHandle(isolate, *debug_info);
    handle = Managed<InterpreterHandle>::New(isolate, cpp_handle);
    debug_info->set(WasmDebugInfo::kInterpreterHandleIndex, *handle);
  }

  return Handle<Managed<InterpreterHandle>>::cast(handle)->get();
}

InterpreterHandle* GetInterpreterHandle(WasmDebugInfo* debug_info) {
  Object* handle_obj = debug_info->get(WasmDebugInfo::kInterpreterHandleIndex);
  DCHECK(!handle_obj->IsUndefined(debug_info->GetIsolate()));
  return Managed<InterpreterHandle>::cast(handle_obj)->get();
}

InterpreterHandle* GetInterpreterHandleOrNull(WasmDebugInfo* debug_info) {
  Object* handle_obj = debug_info->get(WasmDebugInfo::kInterpreterHandleIndex);
  if (handle_obj->IsUndefined(debug_info->GetIsolate())) return nullptr;
  return Managed<InterpreterHandle>::cast(handle_obj)->get();
}

int GetNumFunctions(WasmInstanceObject* instance) {
  size_t num_functions =
      instance->compiled_module()->module()->functions.size();
  DCHECK_GE(kMaxInt, num_functions);
  return static_cast<int>(num_functions);
}

Handle<FixedArray> GetOrCreateInterpretedFunctions(
    Isolate* isolate, Handle<WasmDebugInfo> debug_info) {
  Handle<Object> obj(debug_info->get(WasmDebugInfo::kInterpretedFunctionsIndex),
                     isolate);
  if (!obj->IsUndefined(isolate)) return Handle<FixedArray>::cast(obj);

  Handle<FixedArray> new_arr = isolate->factory()->NewFixedArray(
      GetNumFunctions(debug_info->wasm_instance()));
  debug_info->set(WasmDebugInfo::kInterpretedFunctionsIndex, *new_arr);
  return new_arr;
}

using CodeRelocationMap = IdentityMap<Handle<Code>, FreeStoreAllocationPolicy>;

void RedirectCallsitesInCode(Code* code, CodeRelocationMap& map) {
  DisallowHeapAllocation no_gc;
  for (RelocIterator it(code, RelocInfo::kCodeTargetMask); !it.done();
       it.next()) {
    DCHECK(RelocInfo::IsCodeTarget(it.rinfo()->rmode()));
    Code* target = Code::GetCodeFromTargetAddress(it.rinfo()->target_address());
    Handle<Code>* new_target = map.Find(target);
    if (!new_target) continue;
    it.rinfo()->set_target_address(code->GetIsolate(),
                                   (*new_target)->instruction_start());
  }
}

void RedirectCallsitesInInstance(Isolate* isolate, WasmInstanceObject* instance,
                                 CodeRelocationMap& map) {
  DisallowHeapAllocation no_gc;
  // Redirect all calls in wasm functions.
  FixedArray* code_table = instance->compiled_module()->ptr_to_code_table();
  for (int i = 0, e = GetNumFunctions(instance); i < e; ++i) {
    RedirectCallsitesInCode(Code::cast(code_table->get(i)), map);
  }
  // TODO(6668): Find instances that imported our code and also patch those.

  // Redirect all calls in exported functions.
  FixedArray* weak_exported_functions =
      instance->compiled_module()->ptr_to_weak_exported_functions();
  for (int i = 0, e = weak_exported_functions->length(); i != e; ++i) {
    WeakCell* weak_function = WeakCell::cast(weak_exported_functions->get(i));
    if (weak_function->cleared()) continue;
    Code* code = JSFunction::cast(weak_function->value())->code();
    RedirectCallsitesInCode(code, map);
  }
}

}  // namespace

Handle<WasmDebugInfo> WasmDebugInfo::New(Handle<WasmInstanceObject> instance) {
  DCHECK(!instance->has_debug_info());
  Factory* factory = instance->GetIsolate()->factory();
  Handle<FixedArray> arr = factory->NewFixedArray(kFieldCount, TENURED);
  arr->set(kInstanceIndex, *instance);
  Handle<WasmDebugInfo> debug_info = Handle<WasmDebugInfo>::cast(arr);
  instance->set_debug_info(*debug_info);
  return debug_info;
}

WasmInterpreter* WasmDebugInfo::SetupForTesting(
    Handle<WasmInstanceObject> instance_obj) {
  Handle<WasmDebugInfo> debug_info = WasmDebugInfo::New(instance_obj);
  Isolate* isolate = instance_obj->GetIsolate();
  InterpreterHandle* cpp_handle = new InterpreterHandle(isolate, *debug_info);
  Handle<Object> handle = Managed<InterpreterHandle>::New(isolate, cpp_handle);
  debug_info->set(kInterpreterHandleIndex, *handle);
  return cpp_handle->interpreter();
}

bool WasmDebugInfo::IsWasmDebugInfo(Object* object) {
  if (!object->IsFixedArray()) return false;
  FixedArray* arr = FixedArray::cast(object);
  if (arr->length() != kFieldCount) return false;
  if (!arr->get(kInstanceIndex)->IsWasmInstanceObject()) return false;
  Isolate* isolate = arr->GetIsolate();
  if (!arr->get(kInterpreterHandleIndex)->IsUndefined(isolate) &&
      !arr->get(kInterpreterHandleIndex)->IsForeign())
    return false;
  return true;
}

WasmDebugInfo* WasmDebugInfo::cast(Object* object) {
  DCHECK(IsWasmDebugInfo(object));
  return reinterpret_cast<WasmDebugInfo*>(object);
}

WasmInstanceObject* WasmDebugInfo::wasm_instance() {
  return WasmInstanceObject::cast(get(kInstanceIndex));
}

void WasmDebugInfo::SetBreakpoint(Handle<WasmDebugInfo> debug_info,
                                  int func_index, int offset) {
  Isolate* isolate = debug_info->GetIsolate();
  InterpreterHandle* handle = GetOrCreateInterpreterHandle(isolate, debug_info);
  RedirectToInterpreter(debug_info, Vector<int>(&func_index, 1));
  const WasmFunction* func = &handle->module()->functions[func_index];
  handle->interpreter()->SetBreakpoint(func, offset, true);
}

void WasmDebugInfo::RedirectToInterpreter(Handle<WasmDebugInfo> debug_info,
                                          Vector<int> func_indexes) {
  Isolate* isolate = debug_info->GetIsolate();
  // Ensure that the interpreter is instantiated.
  GetOrCreateInterpreterHandle(isolate, debug_info);
  Handle<FixedArray> interpreted_functions =
      GetOrCreateInterpretedFunctions(isolate, debug_info);
  Handle<WasmInstanceObject> instance(debug_info->wasm_instance(), isolate);
  Handle<FixedArray> code_table = instance->compiled_module()->code_table();
  CodeRelocationMap code_to_relocate(isolate->heap());
  for (int func_index : func_indexes) {
    DCHECK_LE(0, func_index);
    DCHECK_GT(debug_info->wasm_instance()->module()->functions.size(),
              func_index);
    if (!interpreted_functions->get(func_index)->IsUndefined(isolate)) continue;

    Handle<Code> new_code = compiler::CompileWasmInterpreterEntry(
        isolate, func_index,
        instance->compiled_module()->module()->functions[func_index].sig,
        instance);

    Code* old_code = Code::cast(code_table->get(func_index));
    interpreted_functions->set(func_index, *new_code);
    DCHECK_NULL(code_to_relocate.Find(old_code));
    code_to_relocate.Set(old_code, new_code);
  }
  RedirectCallsitesInInstance(isolate, *instance, code_to_relocate);
}

void WasmDebugInfo::PrepareStep(StepAction step_action) {
  GetInterpreterHandle(this)->PrepareStep(step_action);
}

bool WasmDebugInfo::RunInterpreter(Address frame_pointer, int func_index,
                                   uint8_t* arg_buffer) {
  DCHECK_LE(0, func_index);
  Handle<WasmInstanceObject> instance(wasm_instance());
  return GetInterpreterHandle(this)->Execute(
      instance, frame_pointer, static_cast<uint32_t>(func_index), arg_buffer);
}

std::vector<std::pair<uint32_t, int>> WasmDebugInfo::GetInterpretedStack(
    Address frame_pointer) {
  return GetInterpreterHandle(this)->GetInterpretedStack(frame_pointer);
}

std::unique_ptr<wasm::InterpretedFrame> WasmDebugInfo::GetInterpretedFrame(
    Address frame_pointer, int idx) {
  return GetInterpreterHandle(this)->GetInterpretedFrame(frame_pointer, idx);
}

void WasmDebugInfo::Unwind(Address frame_pointer) {
  return GetInterpreterHandle(this)->Unwind(frame_pointer);
}

uint64_t WasmDebugInfo::NumInterpretedCalls() {
  auto handle = GetInterpreterHandleOrNull(this);
  return handle ? handle->NumInterpretedCalls() : 0;
}

void WasmDebugInfo::UpdateMemory(JSArrayBuffer* new_memory) {
  InterpreterHandle* interp_handle = GetInterpreterHandleOrNull(this);
  if (!interp_handle) return;
  interp_handle->UpdateMemory(new_memory);
}

// static
Handle<JSObject> WasmDebugInfo::GetScopeDetails(
    Handle<WasmDebugInfo> debug_info, Address frame_pointer, int frame_index) {
  InterpreterHandle* interp_handle = GetInterpreterHandle(*debug_info);
  return interp_handle->GetScopeDetails(frame_pointer, frame_index, debug_info);
}

// static
Handle<JSObject> WasmDebugInfo::GetGlobalScopeObject(
    Handle<WasmDebugInfo> debug_info, Address frame_pointer, int frame_index) {
  InterpreterHandle* interp_handle = GetInterpreterHandle(*debug_info);
  auto frame = interp_handle->GetInterpretedFrame(frame_pointer, frame_index);
  return interp_handle->GetGlobalScopeObject(frame.get(), debug_info);
}

// static
Handle<JSObject> WasmDebugInfo::GetLocalScopeObject(
    Handle<WasmDebugInfo> debug_info, Address frame_pointer, int frame_index) {
  InterpreterHandle* interp_handle = GetInterpreterHandle(*debug_info);
  auto frame = interp_handle->GetInterpretedFrame(frame_pointer, frame_index);
  return interp_handle->GetLocalScopeObject(frame.get(), debug_info);
}

// static
Handle<JSFunction> WasmDebugInfo::GetCWasmEntry(
    Handle<WasmDebugInfo> debug_info, FunctionSig* sig) {
  Isolate* isolate = debug_info->GetIsolate();
  DCHECK_EQ(debug_info->has_c_wasm_entries(),
            debug_info->has_c_wasm_entry_map());
  if (!debug_info->has_c_wasm_entries()) {
    auto entries = isolate->factory()->NewFixedArray(4, TENURED);
    debug_info->set_c_wasm_entries(*entries);
    auto managed_map =
        Managed<wasm::SignatureMap>::New(isolate, new wasm::SignatureMap());
    debug_info->set_c_wasm_entry_map(*managed_map);
  }
  Handle<FixedArray> entries(debug_info->c_wasm_entries(), isolate);
  wasm::SignatureMap* map = debug_info->c_wasm_entry_map()->get();
  int32_t index = map->Find(sig);
  if (index == -1) {
    index = static_cast<int32_t>(map->FindOrInsert(sig));
    if (index == entries->length()) {
      entries = isolate->factory()->CopyFixedArrayAndGrow(
          entries, entries->length(), TENURED);
      debug_info->set_c_wasm_entries(*entries);
    }
    DCHECK(entries->get(index)->IsUndefined(isolate));
    Handle<Code> new_entry_code = compiler::CompileCWasmEntry(isolate, sig);
    Handle<String> name = isolate->factory()->InternalizeOneByteString(
        STATIC_CHAR_VECTOR("c-wasm-entry"));
    Handle<SharedFunctionInfo> shared =
        isolate->factory()->NewSharedFunctionInfo(name, new_entry_code, false);
    shared->set_internal_formal_parameter_count(
        compiler::CWasmEntryParameters::kNumParameters);
    Handle<JSFunction> new_entry = isolate->factory()->NewFunction(
        isolate->sloppy_function_map(), name, new_entry_code);
    new_entry->set_context(
        *debug_info->wasm_instance()->compiled_module()->native_context());
    new_entry->set_shared(*shared);
    entries->set(index, *new_entry);
  }
  return handle(JSFunction::cast(entries->get(index)));
}
