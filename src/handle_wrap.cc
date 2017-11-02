// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "handle_wrap.h"
#include "async-wrap-inl.h"
#include "env-inl.h"
#include "util-inl.h"
#include "node.h"

namespace node {

using v8::Context;
using v8::FunctionCallbackInfo;
using v8::HandleScope;
using v8::Local;
using v8::Object;
using v8::Value;


void HandleWrap::Ref(const FunctionCallbackInfo<Value>& args) {
  HandleWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  if (IsAlive(wrap))
    uv_ref(wrap->GetHandle());
}


void HandleWrap::Unref(const FunctionCallbackInfo<Value>& args) {
  HandleWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  if (IsAlive(wrap))
    uv_unref(wrap->GetHandle());
}


void HandleWrap::HasRef(const FunctionCallbackInfo<Value>& args) {
  HandleWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());
  args.GetReturnValue().Set(HasRef(wrap));
}


void HandleWrap::Close(const FunctionCallbackInfo<Value>& args) {
  HandleWrap* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());

  wrap->Close(args[0]);
}

void HandleWrap::Close(v8::Local<v8::Value> close_callback) {
  if (state_ != kInitialized)
    return;

  CHECK_EQ(false, persistent().IsEmpty());
  uv_close(handle_, OnClose);
  state_ = kClosing;

  if (!close_callback.IsEmpty() && close_callback->IsFunction()) {
    object()->Set(env()->context(), env()->_onclose_string(), close_callback)
        .FromJust();
    state_ = kClosingWithCallback;
  }
}


void HandleWrap::MarkAsInitialized() {
  env()->handle_wrap_queue()->PushBack(this);
  state_ = kInitialized;
}


void HandleWrap::MarkAsUninitialized() {
  handle_wrap_queue_.Remove();
  state_ = kClosed;
}


HandleWrap::HandleWrap(Environment* env,
                       Local<Object> object,
                       uv_handle_t* handle,
                       AsyncWrap::ProviderType provider)
    : AsyncWrap(env, object, provider),
      state_(kInitialized),
      handle_(handle) {
  handle_->data = this;
  HandleScope scope(env->isolate());
  Wrap(object, this);
  env->handle_wrap_queue()->PushBack(this);
}


HandleWrap::~HandleWrap() {
}


void HandleWrap::OnClose(uv_handle_t* handle) {
  HandleWrap* wrap = static_cast<HandleWrap*>(handle->data);
  Environment* env = wrap->env();
  HandleScope scope(env->isolate());
  Context::Scope context_scope(env->context());

  // The wrap object should still be there.
  CHECK_EQ(wrap->persistent().IsEmpty(), false);
  CHECK(wrap->state_ >= kClosing && wrap->state_ <= kClosingWithCallback);

  const bool have_close_callback = (wrap->state_ == kClosingWithCallback);
  wrap->state_ = kClosed;

  wrap->OnClose();

  if (have_close_callback)
    wrap->MakeCallback(env->_onclose_string(), 0, nullptr);

  ClearWrap(wrap->object());
  wrap->persistent().Reset();
  delete wrap;
}


}  // namespace node
