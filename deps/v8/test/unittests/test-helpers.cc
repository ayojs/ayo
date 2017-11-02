// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "test/unittests/test-helpers.h"

#include "include/v8.h"
#include "src/api.h"
#include "src/handles.h"
#include "src/isolate.h"
#include "src/objects-inl.h"
#include "src/objects.h"

namespace v8 {
namespace internal {
namespace test {

Handle<Object> RunJS(v8::Isolate* isolate, const char* script) {
  return Utils::OpenHandle(
      *v8::Script::Compile(
           isolate->GetCurrentContext(),
           v8::String::NewFromUtf8(isolate, script, v8::NewStringType::kNormal)
               .ToLocalChecked())
           .ToLocalChecked()
           ->Run(isolate->GetCurrentContext())
           .ToLocalChecked());
}

Handle<String> CreateSource(Isolate* isolate,
                            ExternalOneByteString::Resource* maybe_resource) {
  static const char test_script[] = "(x) { x*x; }";
  if (maybe_resource) {
    return isolate->factory()
        ->NewExternalStringFromOneByte(maybe_resource)
        .ToHandleChecked();
  }
  return isolate->factory()->NewStringFromAsciiChecked(test_script);
}

Handle<SharedFunctionInfo> CreateSharedFunctionInfo(
    Isolate* isolate,
    v8::String::ExternalOneByteStringResource* maybe_resource) {
  HandleScope scope(isolate);
  Handle<String> source = CreateSource(isolate, maybe_resource);
  Handle<Script> script = isolate->factory()->NewScript(source);
  Handle<FixedArray> infos = isolate->factory()->NewFixedArray(3);
  script->set_shared_function_infos(*infos);
  Handle<SharedFunctionInfo> shared = isolate->factory()->NewSharedFunctionInfo(
      isolate->factory()->NewStringFromAsciiChecked("f"),
      BUILTIN_CODE(isolate, CompileLazy), false);
  shared->set_end_position(source->length());
  shared->set_outer_scope_info(ScopeInfo::Empty(isolate));
  shared->set_function_literal_id(1);
  SharedFunctionInfo::SetScript(shared, script);
  return scope.CloseAndEscape(shared);
}

}  // namespace test
}  // namespace internal
}  // namespace v8
