// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_ISOLATE_INL_H_
#define V8_ISOLATE_INL_H_

#include "src/isolate.h"
#include "src/objects-inl.h"

namespace v8 {
namespace internal {


void Isolate::set_context(Context* context) {
  DCHECK(context == NULL || context->IsContext());
  thread_local_top_.context_ = context;
}

Handle<Context> Isolate::native_context() {
  return handle(context()->native_context(), this);
}

Context* Isolate::raw_native_context() { return context()->native_context(); }

Object* Isolate::pending_exception() {
  DCHECK(has_pending_exception());
  DCHECK(!thread_local_top_.pending_exception_->IsException(this));
  return thread_local_top_.pending_exception_;
}


void Isolate::set_pending_exception(Object* exception_obj) {
  DCHECK(!exception_obj->IsException(this));
  thread_local_top_.pending_exception_ = exception_obj;
}


void Isolate::clear_pending_exception() {
  DCHECK(!thread_local_top_.pending_exception_->IsException(this));
  thread_local_top_.pending_exception_ = heap_.the_hole_value();
}


bool Isolate::has_pending_exception() {
  DCHECK(!thread_local_top_.pending_exception_->IsException(this));
  return !thread_local_top_.pending_exception_->IsTheHole(this);
}

Object* Isolate::get_wasm_caught_exception() const {
  return thread_local_top_.wasm_caught_exception_;
}

void Isolate::set_wasm_caught_exception(Object* exception_obj) {
  thread_local_top_.wasm_caught_exception_ = exception_obj;
}

void Isolate::clear_wasm_caught_exception() {
  thread_local_top_.wasm_caught_exception_ = nullptr;
}

void Isolate::clear_pending_message() {
  thread_local_top_.pending_message_obj_ = heap_.the_hole_value();
}


Object* Isolate::scheduled_exception() {
  DCHECK(has_scheduled_exception());
  DCHECK(!thread_local_top_.scheduled_exception_->IsException(this));
  return thread_local_top_.scheduled_exception_;
}


bool Isolate::has_scheduled_exception() {
  DCHECK(!thread_local_top_.scheduled_exception_->IsException(this));
  return thread_local_top_.scheduled_exception_ != heap_.the_hole_value();
}


void Isolate::clear_scheduled_exception() {
  DCHECK(!thread_local_top_.scheduled_exception_->IsException(this));
  thread_local_top_.scheduled_exception_ = heap_.the_hole_value();
}


bool Isolate::is_catchable_by_javascript(Object* exception) {
  return exception != heap()->termination_exception();
}

bool Isolate::is_catchable_by_wasm(Object* exception) {
  if (!is_catchable_by_javascript(exception) || !exception->IsJSError())
    return false;
  HandleScope scope(this);
  Handle<Object> exception_handle(exception, this);
  return JSReceiver::HasProperty(Handle<JSReceiver>::cast(exception_handle),
                                 factory()->WasmExceptionTag_string())
      .IsJust();
}

void Isolate::FireBeforeCallEnteredCallback() {
  for (int i = 0; i < before_call_entered_callbacks_.length(); i++) {
    before_call_entered_callbacks_.at(i)(reinterpret_cast<v8::Isolate*>(this));
  }
}

Handle<JSGlobalObject> Isolate::global_object() {
  return handle(context()->global_object(), this);
}

Handle<JSObject> Isolate::global_proxy() {
  return handle(context()->global_proxy(), this);
}


Isolate::ExceptionScope::ExceptionScope(Isolate* isolate)
    : isolate_(isolate),
      pending_exception_(isolate_->pending_exception(), isolate_) {}


Isolate::ExceptionScope::~ExceptionScope() {
  isolate_->set_pending_exception(*pending_exception_);
}

#define NATIVE_CONTEXT_FIELD_ACCESSOR(index, type, name)     \
  Handle<type> Isolate::name() {                             \
    return Handle<type>(raw_native_context()->name(), this); \
  }                                                          \
  bool Isolate::is_##name(type* value) {                     \
    return raw_native_context()->is_##name(value);           \
  }
NATIVE_CONTEXT_FIELDS(NATIVE_CONTEXT_FIELD_ACCESSOR)
#undef NATIVE_CONTEXT_FIELD_ACCESSOR

bool Isolate::IsArraySpeciesLookupChainIntact() {
  // Note: It would be nice to have debug checks to make sure that the
  // species protector is accurate, but this would be hard to do for most of
  // what the protector stands for:
  // - You'd need to traverse the heap to check that no Array instance has
  //   a constructor property
  // - To check that Array[Symbol.species] == Array, JS code has to execute,
  //   but JS cannot be invoked in callstack overflow situations
  // All that could be checked reliably is that
  // Array.prototype.constructor == Array. Given that limitation, no check is
  // done here. In place, there are mjsunit tests harmony/array-species* which
  // ensure that behavior is correct in various invalid protector cases.

  PropertyCell* species_cell = heap()->species_protector();
  return species_cell->value()->IsSmi() &&
         Smi::ToInt(species_cell->value()) == kProtectorValid;
}

bool Isolate::IsStringLengthOverflowIntact() {
  Cell* string_length_cell = heap()->string_length_protector();
  return string_length_cell->value() == Smi::FromInt(kProtectorValid);
}

bool Isolate::IsFastArrayIterationIntact() {
  Cell* fast_iteration_cell = heap()->fast_array_iteration_protector();
  return fast_iteration_cell->value() == Smi::FromInt(kProtectorValid);
}

bool Isolate::IsArrayBufferNeuteringIntact() {
  PropertyCell* buffer_neutering = heap()->array_buffer_neutering_protector();
  return buffer_neutering->value() == Smi::FromInt(kProtectorValid);
}

bool Isolate::IsArrayIteratorLookupChainIntact() {
  PropertyCell* array_iterator_cell = heap()->array_iterator_protector();
  return array_iterator_cell->value() == Smi::FromInt(kProtectorValid);
}

}  // namespace internal
}  // namespace v8

#endif  // V8_ISOLATE_INL_H_
