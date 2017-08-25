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

#include "node_stat_watcher.h"
#include "async-wrap.h"
#include "async-wrap-inl.h"
#include "env.h"
#include "env-inl.h"
#include "util.h"
#include "util-inl.h"

#include <string.h>
#include <stdlib.h>

namespace node {

using v8::Context;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Integer;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;


void StatWatcher::Initialize(Environment* env, Local<Object> target) {
  HandleScope scope(env->isolate());

  Local<FunctionTemplate> t = env->NewFunctionTemplate(StatWatcher::New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  Local<String> statWatcherString =
      FIXED_ONE_BYTE_STRING(env->isolate(), "StatWatcher");
  t->SetClassName(statWatcherString);

  AsyncWrap::AddWrapMethods(env, t);
  env->SetProtoMethod(t, "start", StatWatcher::Start);
  env->SetProtoMethod(t, "stop", StatWatcher::Stop);

  target->Set(statWatcherString, t->GetFunction());
}


static void Delete(uv_handle_t* handle) {
  delete reinterpret_cast<uv_fs_poll_t*>(handle);
}


StatWatcher::StatWatcher(Environment* env, Local<Object> wrap)
    : AsyncWrap(env, wrap, AsyncWrap::PROVIDER_STATWATCHER),
      watcher_(new uv_fs_poll_t) {
  MakeWeak<StatWatcher>(this);
  Wrap(wrap, this);
  uv_fs_poll_init(env->event_loop(), watcher_);
  watcher_->data = static_cast<void*>(this);
}


StatWatcher::~StatWatcher() {
  Stop();
  uv_close(reinterpret_cast<uv_handle_t*>(watcher_), Delete);
}


void StatWatcher::Callback(uv_fs_poll_t* handle,
                           int status,
                           const uv_stat_t* prev,
                           const uv_stat_t* curr) {
  StatWatcher* wrap = static_cast<StatWatcher*>(handle->data);
  CHECK_EQ(wrap->watcher_, handle);
  Environment* env = wrap->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  FillStatsArray(env->fs_stats_field_array(), curr);
  FillStatsArray(env->fs_stats_field_array() + 14, prev);
  Local<Value> arg = Integer::New(env->isolate(), status);
  wrap->MakeCallback(env->onchange_string(), 1, &arg);
}


void StatWatcher::New(const FunctionCallbackInfo<Value>& args) {
  CHECK(args.IsConstructCall());
  Environment* env = Environment::GetCurrent(args);
  new StatWatcher(env, args.This());
}


void StatWatcher::Start(const FunctionCallbackInfo<Value>& args) {
  CHECK_EQ(args.Length(), 3);

  StatWatcher* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());
  node::Utf8Value path(args.GetIsolate(), args[0]);
  const bool persistent = args[1]->BooleanValue();
  const uint32_t interval = args[2]->Uint32Value();

  if (!persistent)
    uv_unref(reinterpret_cast<uv_handle_t*>(wrap->watcher_));
  uv_fs_poll_start(wrap->watcher_, Callback, *path, interval);
  wrap->ClearWeak();
}


void StatWatcher::Stop(const FunctionCallbackInfo<Value>& args) {
  StatWatcher* wrap;
  ASSIGN_OR_RETURN_UNWRAP(&wrap, args.Holder());
  Environment* env = wrap->env();
  Context::Scope context_scope(env->context());
  wrap->MakeCallback(env->onstop_string(), 0, nullptr);
  wrap->Stop();
}


void StatWatcher::Stop() {
  if (!uv_is_active(reinterpret_cast<uv_handle_t*>(watcher_)))
    return;
  uv_fs_poll_stop(watcher_);
  MakeWeak<StatWatcher>(this);
}


}  // namespace node
