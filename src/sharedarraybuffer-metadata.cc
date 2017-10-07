#include "sharedarraybuffer-metadata.h"
#include "base-object.h"
#include "base-object-inl.h"

using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Local;
using v8::Maybe;
using v8::MaybeLocal;
using v8::Nothing;
using v8::Object;
using v8::SharedArrayBuffer;
using v8::Value;

namespace node {
namespace worker {

namespace {

// Yield a JS constructor for SABLifetimePartner objects in the form of a
// standard API object, that has a single field for containing the raw
// SABLiftimePartner* pointer.
Local<Function> GetSABLifetimePartnerConstructor(
    Environment* env, Local<Context> context) {
  Local<FunctionTemplate> templ;
  templ = env->sab_lifetimepartner_constructor_template();
  if (!templ.IsEmpty())
    return templ->GetFunction(context).ToLocalChecked();

  {
    Local<FunctionTemplate> m = env->NewFunctionTemplate(
        [](const FunctionCallbackInfo<Value>& info) {
          CHECK(info.IsConstructCall());
        });
    m->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(),
                                          "SABLifetimePartner"));
    m->InstanceTemplate()->SetInternalFieldCount(1);

    env->set_sab_lifetimepartner_constructor_template(m);
  }

  return GetSABLifetimePartnerConstructor(env, context);
}

class SABLifetimePartner : public BaseObject {
 public:
  SABLifetimePartner(Environment* env,
                     Local<Object> obj,
                     SharedArrayBufferMetadataReference r)
    : BaseObject(env, obj),
      reference(r) {
    MakeWeak<SABLifetimePartner>(this);
  }

  SharedArrayBufferMetadataReference reference;
};

}  // anonymous namespace

SharedArrayBufferMetadataReference
SharedArrayBufferMetadata::ForIncomingSharedArrayBuffer(
    Environment* env, Local<Context> context, Local<SharedArrayBuffer> source) {
  Local<Value> lifetime_partner;

  if (!source->GetPrivate(context,
                          env->sab_lifetimepartner_symbol())
                              .ToLocal(&lifetime_partner)) {
    return nullptr;
  }

  if (lifetime_partner->IsObject() &&
      env->sab_lifetimepartner_constructor_template()
         ->HasInstance(lifetime_partner)) {
    if (!source->IsExternal()) {
      env->ThrowError("Found internalized SharedArrayBuffer with "
                      "lifetime partner object");
      return nullptr;
    }

    SABLifetimePartner* partner =
        Unwrap<SABLifetimePartner>(lifetime_partner.As<Object>());
    CHECK_NE(partner, nullptr);
    return partner->reference;
  }

  if (source->IsExternal()) {
    // If this is an external SharedArrayBuffer but we do not see a lifetime
    // partner object, we did not externalize it. In that case, there is no
    // way to serialize it.
    env->ThrowError("Cannot serialize externalized SharedArrayBuffer");
    return nullptr;
  }

  SharedArrayBuffer::Contents contents = source->Externalize();
  SharedArrayBufferMetadataReference r(new SharedArrayBufferMetadata(
      contents.Data(), contents.ByteLength()));
  if (r->AssignToSharedArrayBuffer(env, context, source).IsNothing())
    return nullptr;
  return r;
}

Maybe<bool> SharedArrayBufferMetadata::AssignToSharedArrayBuffer(
    Environment* env, Local<Context> context,
    Local<SharedArrayBuffer> target) {
  Local<Function> ctor = GetSABLifetimePartnerConstructor(env, context);
  Local<Object> obj;
  if (!ctor->NewInstance(context).ToLocal(&obj))
    return Nothing<bool>();

  new SABLifetimePartner(env, obj, shared_from_this());
  return target->SetPrivate(context,
                            env->sab_lifetimepartner_symbol(),
                            obj);
}

SharedArrayBufferMetadata::SharedArrayBufferMetadata(void* data, size_t size)
  : data(data), size(size) { }

SharedArrayBufferMetadata::~SharedArrayBufferMetadata() {
  free(data);
}

MaybeLocal<SharedArrayBuffer> SharedArrayBufferMetadata::GetSharedArrayBuffer(
    Environment* env, Local<Context> context) {
  Local<SharedArrayBuffer> obj =
      SharedArrayBuffer::New(env->isolate(), data, size);

  if (AssignToSharedArrayBuffer(env, context, obj).IsNothing())
    return MaybeLocal<SharedArrayBuffer>();

  return obj;
}

}  // namespace worker
}  // namespace node
