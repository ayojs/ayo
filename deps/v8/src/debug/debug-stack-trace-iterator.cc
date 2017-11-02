// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/debug/debug-stack-trace-iterator.h"

#include "src/debug/debug-evaluate.h"
#include "src/debug/debug-scope-iterator.h"
#include "src/debug/debug.h"
#include "src/debug/liveedit.h"
#include "src/frames-inl.h"
#include "src/isolate.h"

namespace v8 {

std::unique_ptr<debug::StackTraceIterator> debug::StackTraceIterator::Create(
    v8::Isolate* isolate, int index) {
  return std::unique_ptr<debug::StackTraceIterator>(
      new internal::DebugStackTraceIterator(
          reinterpret_cast<internal::Isolate*>(isolate), index));
}

namespace internal {

DebugStackTraceIterator::DebugStackTraceIterator(Isolate* isolate, int index)
    : isolate_(isolate),
      iterator_(isolate, isolate->debug()->break_frame_id()),
      is_top_frame_(true) {
  if (iterator_.done()) return;
  std::vector<FrameSummary> frames;
  frames.reserve(FLAG_max_inlining_levels + 1);
  iterator_.frame()->Summarize(&frames);
  inlined_frame_index_ = static_cast<int>(frames.size());
  Advance();
  for (; !Done() && index > 0; --index) Advance();
}

DebugStackTraceIterator::~DebugStackTraceIterator() {}

bool DebugStackTraceIterator::Done() const { return iterator_.done(); }

void DebugStackTraceIterator::Advance() {
  while (true) {
    --inlined_frame_index_;
    for (; inlined_frame_index_ >= 0; --inlined_frame_index_) {
      // Omit functions from native and extension scripts.
      if (FrameSummary::Get(iterator_.frame(), inlined_frame_index_)
              .is_subject_to_debugging()) {
        break;
      }
      is_top_frame_ = false;
    }
    if (inlined_frame_index_ >= 0) {
      frame_inspector_.reset(new FrameInspector(
          iterator_.frame(), inlined_frame_index_, isolate_));
      break;
    }
    is_top_frame_ = false;
    frame_inspector_.reset();
    iterator_.Advance();
    if (iterator_.done()) break;
    std::vector<FrameSummary> frames;
    frames.reserve(FLAG_max_inlining_levels + 1);
    iterator_.frame()->Summarize(&frames);
    inlined_frame_index_ = static_cast<int>(frames.size());
  }
}

int DebugStackTraceIterator::GetContextId() const {
  DCHECK(!Done());
  Handle<Object> context = frame_inspector_->GetContext();
  if (context->IsContext()) {
    Object* value =
        Context::cast(*context)->native_context()->debug_context_id();
    if (value->IsSmi()) return Smi::ToInt(value);
  }
  return 0;
}

v8::Local<v8::Value> DebugStackTraceIterator::GetReceiver() const {
  DCHECK(!Done());
  Handle<Object> value = frame_inspector_->GetReceiver();
  if (value.is_null() || (value->IsSmi() || !value->IsTheHole(isolate_))) {
    return Utils::ToLocal(value);
  }
  return v8::Undefined(reinterpret_cast<v8::Isolate*>(isolate_));
}

v8::Local<v8::Value> DebugStackTraceIterator::GetReturnValue() const {
  DCHECK(!Done());
  if (frame_inspector_->IsWasm()) return v8::Local<v8::Value>();
  bool is_optimized = iterator_.frame()->is_optimized();
  if (is_optimized || !is_top_frame_ ||
      !isolate_->debug()->IsBreakAtReturn(iterator_.javascript_frame())) {
    return v8::Local<v8::Value>();
  }
  return Utils::ToLocal(isolate_->debug()->return_value_handle());
}

v8::Local<v8::String> DebugStackTraceIterator::GetFunctionName() const {
  DCHECK(!Done());
  return Utils::ToLocal(frame_inspector_->GetFunctionName());
}

v8::Local<v8::debug::Script> DebugStackTraceIterator::GetScript() const {
  DCHECK(!Done());
  Handle<Object> value = frame_inspector_->GetScript();
  if (!value->IsScript()) return v8::Local<v8::debug::Script>();
  return ToApiHandle<debug::Script>(Handle<Script>::cast(value));
}

debug::Location DebugStackTraceIterator::GetSourceLocation() const {
  DCHECK(!Done());
  v8::Local<v8::debug::Script> script = GetScript();
  if (script.IsEmpty()) return v8::debug::Location();
  return script->GetSourceLocation(frame_inspector_->GetSourcePosition());
}

v8::Local<v8::Function> DebugStackTraceIterator::GetFunction() const {
  DCHECK(!Done());
  if (!frame_inspector_->IsJavaScript()) return v8::Local<v8::Function>();
  return Utils::ToLocal(frame_inspector_->GetFunction());
}

std::unique_ptr<v8::debug::ScopeIterator>
DebugStackTraceIterator::GetScopeIterator() const {
  DCHECK(!Done());
  StandardFrame* frame = iterator_.frame();
  if (frame->is_wasm_interpreter_entry()) {
    return std::unique_ptr<v8::debug::ScopeIterator>(new DebugWasmScopeIterator(
        isolate_, iterator_.frame(), inlined_frame_index_));
  }
  return std::unique_ptr<v8::debug::ScopeIterator>(
      new DebugScopeIterator(isolate_, frame_inspector_.get()));
}

bool DebugStackTraceIterator::Restart() {
  DCHECK(!Done());
  if (iterator_.is_wasm()) return false;
  return !LiveEdit::RestartFrame(iterator_.javascript_frame());
}

v8::MaybeLocal<v8::Value> DebugStackTraceIterator::Evaluate(
    v8::Local<v8::String> source, bool throw_on_side_effect) {
  DCHECK(!Done());
  Handle<Object> value;
  if (!DebugEvaluate::Local(isolate_, iterator_.frame()->id(),
                            inlined_frame_index_, Utils::OpenHandle(*source),
                            throw_on_side_effect)
           .ToHandle(&value)) {
    isolate_->OptionalRescheduleException(false);
    return v8::MaybeLocal<v8::Value>();
  }
  return Utils::ToLocal(value);
}
}  // namespace internal
}  // namespace v8
