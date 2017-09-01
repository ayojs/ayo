#include "node_messaging.h"
#include "node_internals.h"
#include "node_buffer.h"
#include "util.h"
#include "util-inl.h"
#include "async-wrap.h"
#include "async-wrap-inl.h"

using v8::Array;
using v8::ArrayBuffer;
using v8::ArrayBufferCreationMode;
using v8::Context;
using v8::EscapableHandleScope;
using v8::Exception;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Int32;
using v8::Integer;
using v8::Isolate;
using v8::Just;
using v8::Local;
using v8::Maybe;
using v8::MaybeLocal;
using v8::Nothing;
using v8::Object;
using v8::SharedArrayBuffer;
using v8::String;
using v8::Value;
using v8::ValueDeserializer;
using v8::ValueSerializer;

namespace node {
namespace worker {

Message::Message(void* buf, size_t length, int32_t flag)
    : flag_(flag),
      main_message_buf_(uv_buf_init(static_cast<char*>(buf), length)) {}

Message::Message(int32_t flags) : Message(nullptr, 0, flags) {}

Message::~Message() {
  for (const uv_buf_t& buf : array_buffer_contents_)
    free(buf.base);
  free(main_message_buf_.base);
}

Message::Message(Message&& other) : Message() {
  *this = std::move(other);
}

Message& Message::operator=(Message&& other) {
  this->~Message();
  flag_ = other.flag_;
  main_message_buf_ = other.main_message_buf_;
  other.main_message_buf_ = uv_buf_init(nullptr, 0);
  array_buffer_contents_ = std::move(other.array_buffer_contents_);
  shared_array_buffers_ = std::move(other.shared_array_buffers_);
  message_ports_ = std::move(other.message_ports_);
  return *this;
}

namespace {

// This is used to transfer objects like other `MessagePort`s and
// `SharedArrayBuffer`s.
class DeserializerDelegate : public ValueDeserializer::Delegate {
 public:
  DeserializerDelegate(Message* m,
                       Environment* env,
                       const std::vector<MessagePort*>& message_ports)
    : env_(env), msg_(m), message_ports_(message_ports) {}

  MaybeLocal<Object> ReadHostObject(Isolate* isolate) override {
    // Currently, only MessagePort hosts objects are supported, so identifying
    // by the index in the message's MessagePort array is sufficient.
    uint32_t id;
    if (!deserializer->ReadUint32(&id))
      return MaybeLocal<Object>();
    CHECK_LE(id, message_ports_.size());
    return message_ports_[id]->object(isolate);
  };

  ValueDeserializer* deserializer = nullptr;
 private:
  Environment* env_;
  Message* msg_;
  const std::vector<MessagePort*>& message_ports_;
};

}  // anonymous namespace

MaybeLocal<Value> Message::Deserialize(Environment* env,
                                       Local<Context> context) {
  EscapableHandleScope handle_scope(env->isolate());
  Context::Scope context_scope(context);
  if (flag_ != MESSAGE_FLAG_NONE &&
      flag_ < MESSAGE_FLAG_CUSTOM_OFFSET) {
    // This is for messages generated in C++ with the expectation that they
    // are handled in JS, e.g. serialized error messages from workers.
    CHECK(array_buffer_contents_.empty());
    CHECK(shared_array_buffers_.empty());
    CHECK(message_ports_.empty());
    char* buf = main_message_buf_.base;
    main_message_buf_.base = nullptr;
    return handle_scope.Escape(
        Buffer::New(env, buf, main_message_buf_.len).FromMaybe(Local<Value>()));
  }

  // Create all necessary MessagePort handles.
  std::vector<MessagePort*> ports(message_ports_.size());
  for (uint32_t i = 0; i < message_ports_.size(); ++i) {
    ports[i] = MessagePort::New(env,
                                context,
                                nullptr,
                                std::move(message_ports_[i]));
    if (ports[i] == nullptr)
      return MaybeLocal<Value>();
  }
  message_ports_.clear();

  DeserializerDelegate delegate(this, env, ports);
  ValueDeserializer deserializer(
      env->isolate(),
      reinterpret_cast<const uint8_t*>(main_message_buf_.base),
      main_message_buf_.len,
      &delegate);
  delegate.deserializer = &deserializer;

  // Attach all transfered (Shared)ArrayBuffers to their new Isolate.
  for (uint32_t i = 0; i < array_buffer_contents_.size(); ++i) {
    Local<ArrayBuffer> ab =
        ArrayBuffer::New(env->isolate(),
                         array_buffer_contents_[i].base,
                         array_buffer_contents_[i].len,
                         ArrayBufferCreationMode::kInternalized);
    array_buffer_contents_[i].base = nullptr;
    deserializer.TransferArrayBuffer(i, ab);
  }
  array_buffer_contents_.clear();

  for (uint32_t i = 0; i < shared_array_buffers_.size(); ++i) {
    Local<SharedArrayBuffer> sab;
    if (!shared_array_buffers_[i]->GetSharedArrayBuffer(env, context)
            .ToLocal(&sab))
      return MaybeLocal<Value>();
    deserializer.TransferSharedArrayBuffer(i, sab);
  }
  shared_array_buffers_.clear();

  if (deserializer.ReadHeader(context).IsNothing())
    return MaybeLocal<Value>();
  return handle_scope.Escape(
      deserializer.ReadValue(context).FromMaybe(Local<Value>()));
}

void Message::AddSharedArrayBuffer(
    SharedArrayBufferMetadataReference reference) {
  shared_array_buffers_.push_back(reference);
}

void Message::AddMessagePort(std::unique_ptr<MessagePortData>&& data) {
  message_ports_.emplace_back(std::move(data));
}

namespace {

class SerializerDelegate : public ValueSerializer::Delegate {
 public:
  SerializerDelegate(Environment* env, Local<Context> context, Message* m)
      : env_(env), context_(context), msg_(m) {}

  void ThrowDataCloneError(Local<String> message) override {
    env_->isolate()->ThrowException(Exception::Error(message));
  }

  Maybe<bool> WriteHostObject(Isolate* isolate, Local<Object> object) override {
    if (env_->message_port_constructor_template()->HasInstance(object)) {
      return WriteMessagePort(Unwrap<MessagePort>(object));
    }

    env_->ThrowError("Cannot serialize unknown type of host object");
    return Nothing<bool>();
  }

  Maybe<uint32_t> GetSharedArrayBufferId(
      Isolate* isolate, Local<SharedArrayBuffer> shared_array_buffer) override {
    uint32_t i;
    for (i = 0; i < seen_shared_array_buffers_.size(); ++i) {
      if (seen_shared_array_buffers_[i] == shared_array_buffer)
        return Just(i);
    }

    SharedArrayBufferMetadataReference reference(
        SharedArrayBufferMetadata::ForIncomingSharedArrayBuffer(env_,
                                                          context_,
                                                          shared_array_buffer));
    if (!reference) {
      return Nothing<uint32_t>();
    }
    seen_shared_array_buffers_.push_back(shared_array_buffer);
    msg_->AddSharedArrayBuffer(reference);
    return Just(i);
  }

  void Finish() {
    for (MessagePort* port : ports_) {
      port->Close();
      msg_->AddMessagePort(port->Detach());
    }
  }

  ValueSerializer* serializer = nullptr;

 private:
  Maybe<bool> WriteMessagePort(MessagePort* port) {
    for (uint32_t i = 0; i < ports_.size(); i++) {
      if (ports_[i] == port) {
        serializer->WriteUint32(i);
        return Just(true);
      }
    }

    env_->ThrowError("MessagePort was not listed in transferList");
    return Nothing<bool>();
  }

  Environment* env_;
  Local<Context> context_;
  Message* msg_;
  std::vector<Local<SharedArrayBuffer>> seen_shared_array_buffers_;
  std::vector<MessagePort*> ports_;

  friend class worker::Message;
};

}  // anynomous namespace

Maybe<bool> Message::Serialize(Environment* env,
                               Local<Context> context,
                               Local<Value> input,
                               Local<Value> transfer_list_v) {
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(context);
  CHECK_EQ(main_message_buf_.base, nullptr);
  SerializerDelegate delegate(env, context, this);
  ValueSerializer serializer(env->isolate(), &delegate);
  delegate.serializer = &serializer;
  std::vector<Local<ArrayBuffer>> array_buffers;

  if (transfer_list_v->IsArray()) {
    Local<Array> transfer_list = transfer_list_v.As<Array>();
    uint32_t length = transfer_list->Length();
    for (uint32_t i = 0; i < length; ++i) {
      Local<Value> entry;
      if (!transfer_list->Get(context, i).ToLocal(&entry))
        return Nothing<bool>();
      // Currently, we support ArrayBuffers and MessagePorts.
      if (entry->IsArrayBuffer()) {
        Local<ArrayBuffer> ab = entry.As<ArrayBuffer>();
        if (!ab->IsNeuterable() || ab->IsExternal())
          continue;
        uint32_t id = array_buffers.size();
        array_buffers.push_back(ab);
        serializer.TransferArrayBuffer(id, ab);
        continue;
      } else if (env->message_port_constructor_template()
                    ->HasInstance(entry)) {
        MessagePort* port = Unwrap<MessagePort>(entry.As<Object>());
        CHECK_NE(port, nullptr);
        delegate.ports_.push_back(port);
        continue;
      }

      env->ThrowError("Found invalid object in transferList");
      return Nothing<bool>();
    }
  }

  serializer.WriteHeader();
  if (serializer.WriteValue(context, input).IsNothing()) {
    return Nothing<bool>();
  }

  for (Local<ArrayBuffer> ab : array_buffers) {
    ArrayBuffer::Contents contents = ab->Externalize();
    ab->Neuter();
    array_buffer_contents_.push_back(
        uv_buf_init(static_cast<char*>(contents.Data()),
                    contents.ByteLength()));
  }

  delegate.Finish();
  std::pair<uint8_t*, size_t> data = serializer.Release();
  main_message_buf_.base = reinterpret_cast<char*>(data.first);
  main_message_buf_.len = data.second;
  return Just(true);
}

MessagePortData::MessagePortData(MessagePort* owner) : owner_(owner) { }

MessagePortData::~MessagePortData() {
  CHECK_EQ(owner_, nullptr);
  Disentangle();
  incoming_messages_.clear();
}

void MessagePortData::AddToQueue(Message&& message) {
  // This function will be called by other threads.
  Mutex::ScopedLock lock(mutex_);
  incoming_messages_.emplace_back(std::move(message));

  if (owner_ != nullptr)
    owner_->TriggerAsync();
}

bool MessagePortData::IsSiblingClosed() const {
  Mutex::ScopedLock lock(mutex_);
  return sibling_ == nullptr;
}

void MessagePortData::Entangle(MessagePortData* a, MessagePortData* b) {
  CHECK_EQ(a->sibling_, nullptr);
  CHECK_EQ(b->sibling_, nullptr);
  a->sibling_ = b;
  b->sibling_ = a;
  a->sibling_mutex_ = b->sibling_mutex_;
}

void MessagePortData::PingOwnerAfterDisentanglement() {
  Mutex::ScopedLock lock(mutex_);
  if (owner_ != nullptr)
    owner_->TriggerAsync();
}

void MessagePortData::Disentangle() {
  // Grab a copy of the sibling mutex, then replace it.
  std::shared_ptr<Mutex> sibling_mutex = sibling_mutex_;
  Mutex::ScopedLock sibling_lock(*sibling_mutex);
  sibling_mutex_ = std::make_shared<Mutex>();

  MessagePortData* sibling = sibling_;
  if (sibling_ != nullptr) {
    sibling_->sibling_ = nullptr;
    sibling_ = nullptr;
  }
  PingOwnerAfterDisentanglement();
  if (sibling != nullptr) {
    sibling->PingOwnerAfterDisentanglement();
  }
}

MessagePort::~MessagePort() {
  if (data_)
    data_->owner_ = nullptr;
}

MessagePort::MessagePort(Environment* env,
                         Local<Context> context,
                         Local<Object> wrap,
                         std::unique_ptr<FlaggedMessageListener>&& listener)
  : HandleWrap(env,
               wrap,
               reinterpret_cast<uv_handle_t*>(new uv_async_t()),
               AsyncWrap::PROVIDER_MESSAGEPORT),
    data_(new MessagePortData(this)),
    fm_listener_(std::move(listener)) {
  auto onmessage = [](uv_async_t* handle) {
    // Called when data has been put into the queue.
    MessagePort* channel = static_cast<MessagePort*>(handle->data);
    channel->OnMessage();
  };
  CHECK_EQ(uv_async_init(env->event_loop(),
                         async(),
                         onmessage), 0);
  async()->data = static_cast<void*>(this);

  Local<Value> fn;
  if (!wrap->Get(context, env->oninit_string()).ToLocal(&fn))
    return;

  if (fn->IsFunction()) {
    Local<Function> init = fn.As<Function>();
    (void)init->Call(context, wrap, 0, nullptr);
  }
}

void MessagePort::AddToQueue(Message&& message) {
  data_->AddToQueue(std::move(message));
}

uv_async_t* MessagePort::async() {
  return reinterpret_cast<uv_async_t*>(GetHandle());
}

void MessagePort::TriggerAsync() {
  CHECK_EQ(uv_async_send(async()), 0);
}

void MessagePort::New(const FunctionCallbackInfo<Value>& args) {
  // This is only ever called from MessagePort::New(env, listener).
  Environment* env = Environment::GetCurrent(args);
  Local<Context> context = args.This()->CreationContext();
  Context::Scope context_scope(context);

  new MessagePort(env, context, args.This(), nullptr);
}

MessagePort* MessagePort::New(
    Environment* env,
    Local<Context> context,
    std::unique_ptr<FlaggedMessageListener> listener,
    std::unique_ptr<MessagePortData> data) {
  Context::Scope context_scope(context);
  Local<Function> ctor;
  if (!GetMessagePortConstructor(env, context).ToLocal(&ctor))
    return nullptr;
  MessagePort* port = nullptr;

  // Construct a new instance, then assign the listener instance and possibly
  // the MessagePortData to it.
  Local<Object> instance;
  if (!ctor->NewInstance(context).ToLocal(&instance))
    return nullptr;
  ASSIGN_OR_RETURN_UNWRAP(&port, instance, nullptr);
  if (data) {
    port->Detach();
    port->data_ = std::move(data);
    port->data_->owner_ = port;
    if (port->data_->started_)
      port->TriggerAsync();
  }
  port->fm_listener_ = std::move(listener);
  return port;
}

void MessagePort::OnMessage() {
  // data_ can only ever be modified by the owner thread, so no need to lock.
  // However, the message port may be transferred while it is processing
  // messages, so we need to check that it still owns its data field on every
  // iteration.
  while (data_) {
    Message received;
    {
      // Get the head of the message queue.
      Mutex::ScopedLock lock(data_->mutex_);
      if (!data_->started_) break;
      if (data_->incoming_messages_.empty()) break;
      received = std::move(data_->incoming_messages_.front());
      data_->incoming_messages_.pop_front();
    }

    {
      if (received.flag_ != MESSAGE_FLAG_NONE &&
          received.flag_ <= MESSAGE_FLAG_MAX_HANDLED_INTERNALLY) {
        // This means the message was generated in C++ and is expected to be
        // handled in C++.
        CHECK(fm_listener_);
        fm_listener_->HandleMessage(received.flag_);
        if (received.flag_ == MESSAGE_FLAG_STOP_THREAD_ORDER) {
          // Break out of the loop to exit as soon as possible. There is not
          // going to be any more JS execution on this thread anyway.
          {
            Mutex::ScopedLock lock(data_->mutex_);
            data_->started_ = false;
          }
          return;
        }
        continue;
      }
    }

    if (!env()->can_call_into_js()) {
      // In this case there is nothing to do but to drain the current queue.
      continue;
    }

    {
      // Call the JS .onmessage() callback.
      HandleScope handle_scope(env()->isolate());
      Local<Context> context = object(env()->isolate())->CreationContext();
      Context::Scope context_scope(context);
      Local<Value> args[] = {
        received.Deserialize(env(), context).FromMaybe(Local<Value>()),
        Integer::New(env()->isolate(), received.flag_)
      };

      if (args[0].IsEmpty() ||
          MakeCallback(env()->onmessage_string(),
                       is_privileged_ ? arraysize(args) : 1,
                       args)
              .IsEmpty()) {
        // Re-schedule OnMessage() execution in case of failure.
        if (data_)
          TriggerAsync();
        return;
      }
    }
  }

  if (data_ && data_->IsSiblingClosed() && close_when_sibling_closes_) {
    Close();
  }
}

bool MessagePort::IsSiblingClosed() const {
  CHECK(data_);
  return data_->IsSiblingClosed();
}

void MessagePort::DoNotCloseWhenSiblingCloses() {
  close_when_sibling_closes_ = false;
}

void MessagePort::OnClose() {
  if (data_) {
    data_->owner_ = nullptr;
    data_->Disentangle();
  }
  data_.reset();
  delete async();
  (void)MakeCallback(env()->onclose_string(), 0, nullptr);
}

std::unique_ptr<MessagePortData> MessagePort::Detach() {
  CHECK(IsTransferable());
  Mutex::ScopedLock lock(data_->mutex_);
  data_->owner_ = nullptr;
  return std::move(data_);
}

bool MessagePort::IsTransferable() const {
  return !fm_listener_;
}

void MessagePort::MarkAsPrivileged() {
  is_privileged_ = true;
}

void MessagePort::Send(Message&& message) {
  Mutex::ScopedLock lock(*data_->sibling_mutex_);
  if (data_->sibling_ == nullptr) return;
  data_->sibling_->AddToQueue(std::move(message));
}

void MessagePort::Send(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Local<Context> context = object(env->isolate())->CreationContext();
  if (args.Length() == 0) return;
  int32_t flag = MESSAGE_FLAG_NONE;
  if (args[2]->IsInt32() && is_privileged_) {
    flag = args[2].As<Int32>()->Value();
  }
  Message msg(flag);
  if (msg.Serialize(env, context, args[0], args[1])
          .IsNothing())
    return;
  Send(std::move(msg));
}

void MessagePort::PostMessage(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  MessagePort* port;
  ASSIGN_OR_RETURN_UNWRAP(&port, args.This());
  if (!port->data_) {
    env->ThrowError("Cannot send data on closed MessagePort");
    return;
  }
  port->Send(args);
}

void MessagePort::Start() {
  Mutex::ScopedLock lock(data_->mutex_);
  data_->started_ = true;
  if (!data_->incoming_messages_.empty())
    TriggerAsync();
}

void MessagePort::Stop() {
  Mutex::ScopedLock lock(data_->mutex_);
  data_->started_ = false;
}

void MessagePort::StartBinding(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  MessagePort* port;
  ASSIGN_OR_RETURN_UNWRAP(&port, args.This());
  if (!port->data_) {
    env->ThrowError("Cannot start listening on a closed MessagePort");
    return;
  }
  port->Start();
}

void MessagePort::StopBinding(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  MessagePort* port;
  ASSIGN_OR_RETURN_UNWRAP(&port, args.This());
  if (!port->data_) {
    env->ThrowError("Cannot stop listening on a closed MessagePort");
    return;
  }
  port->Stop();
}

size_t MessagePort::self_size() const {
  Mutex::ScopedLock lock(data_->mutex_);
  size_t sz = sizeof(*this) + sizeof(*data_);
  for (const Message& msg : data_->incoming_messages_)
    sz += sizeof(msg) + msg.main_message_buf_.len;
  return sz;
}

void MessagePort::Entangle(MessagePort* a, MessagePort* b) {
  Entangle(a, b->data_.get());
}

void MessagePort::Entangle(MessagePort* a, MessagePortData* b) {
  MessagePortData::Entangle(a->data_.get(), b);
}

MaybeLocal<Function> GetMessagePortConstructor(
    Environment* env, Local<Context> context) {
  // Factor generating the MessagePort JS constructor into its own piece
  // of code, because it is needed early on in the child environment setup.
  Local<FunctionTemplate> templ = env->message_port_constructor_template();
  if (!templ.IsEmpty())
    return templ->GetFunction(context);

  {
    Local<FunctionTemplate> m = env->NewFunctionTemplate(MessagePort::New);
    m->SetClassName(env->message_port_constructor_string());
    m->InstanceTemplate()->SetInternalFieldCount(1);

    AsyncWrap::AddWrapMethods(env, m);

    env->SetProtoMethod(m, "postMessage", MessagePort::PostMessage);
    env->SetProtoMethod(m, "start", MessagePort::StartBinding);
    env->SetProtoMethod(m, "stop", MessagePort::StopBinding);
    env->SetProtoMethod(m, "close", HandleWrap::Close);
    env->SetProtoMethod(m, "unref", HandleWrap::Unref);
    env->SetProtoMethod(m, "ref", HandleWrap::Ref);
    env->SetProtoMethod(m, "hasRef", HandleWrap::HasRef);

    env->set_message_port_constructor_template(m);
  }

  return GetMessagePortConstructor(env, context);
}

namespace {

static void MessageChannel(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  if (!args.IsConstructCall()) {
    env->ThrowError("MessageChannel() requires calling with new");
    return;
  }

  Local<Context> context = args.This()->CreationContext();
  Context::Scope context_scope(context);

  MessagePort* port1 = MessagePort::New(env, context);
  MessagePort* port2 = MessagePort::New(env, context);
  MessagePort::Entangle(port1, port2);

  args.This()->Set(env->context(), env->port1_string(), port1->object())
      .FromJust();
  args.This()->Set(env->context(), env->port2_string(), port2->object())
      .FromJust();
}

static void InitMessaging(Local<Object> target,
                          Local<Value> unused,
                          Local<Context> context,
                          void* priv) {
  Environment* env = Environment::GetCurrent(context);

  {
    Local<String> message_channel_string =
        FIXED_ONE_BYTE_STRING(env->isolate(), "MessageChannel");
    Local<FunctionTemplate> templ = env->NewFunctionTemplate(MessageChannel);
    templ->SetClassName(message_channel_string);
    target->Set(env->context(),
                message_channel_string,
                templ->GetFunction(context).ToLocalChecked()).FromJust();
  }

  target->Set(context,
              env->message_port_constructor_string(),
              GetMessagePortConstructor(env, context).ToLocalChecked())
                  .FromJust();
}

}  // anonymous namespace

}  // namespace worker
}  // namespace node

NODE_MODULE_CONTEXT_AWARE_INTERNAL(messaging, node::worker::InitMessaging)
