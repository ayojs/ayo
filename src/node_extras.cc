#include "node.h"
#include "util.h"
#include "util-inl.h"
#include "v8.h"

namespace node {

using v8::Context;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::Value;

namespace v8_extras {

static void Init(Local<Object> target,
                 Local<Value> unused,
                 Local<Context> context,
                 void* priv) {
  Isolate* isolate = context->GetIsolate();
  Local<Object> binding = context->GetExtrasBindingObject();
  CHECK(target->Set(context, FIXED_ONE_BYTE_STRING(isolate, "binding"),
                    binding).FromMaybe(false));
}

}  // namespace v8_extras

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(extras, node::v8_extras::Init)
