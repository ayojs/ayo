#include "node_worker.h"
#include "node_internals.h"
#include "node_buffer.h"
#include "node_perf.h"
#include "util.h"
#include "util-inl.h"
#include "async-wrap.h"
#include "async-wrap-inl.h"

using v8::ArrayBuffer;
using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::HeapStatistics;
using v8::Integer;
using v8::Isolate;
using v8::Local;
using v8::Locker;
using v8::MaybeLocal;
using v8::Number;
using v8::Object;
using v8::SealHandleScope;
using v8::String;
using v8::Value;

namespace node {
namespace worker {

namespace {

double next_thread_id = 1;
Mutex next_thread_id_mutex;

struct ChildListener : public MessagePort::FlaggedMessageListener {
  explicit ChildListener(Environment* e) : env(e) {}
  ~ChildListener() {}

  void HandleMessage(int32_t oob) override {
    // The child context only understands stopping messages right now.
    CHECK_EQ(oob, MESSAGE_FLAG_STOP_THREAD_ORDER);
    uv_stop(env->event_loop());
  }

  Environment* env;  // The child's Environment.
};

struct ParentListener : public MessagePort::FlaggedMessageListener {
  explicit ParentListener(Worker* worker) : w(worker) {}
  ~ParentListener() {}

  void HandleMessage(int32_t oob) override {
    // The child context only understands stop state indicators right now.
    CHECK_EQ(oob, MESSAGE_FLAG_THREAD_STOPPED);
    w->OnThreadStopped();
  }

  Worker* w;
};

}  // anonymous namespace

Mutex Worker::by_isolate_mutex_;
std::unordered_map<v8::Isolate*, Worker*> Worker::by_isolate_;

Worker* Worker::ForIsolate(Isolate* isolate) {
  Mutex::ScopedLock lock(by_isolate_mutex_);
  return by_isolate_[isolate];
}

void Worker::AfterGC(bool was_low_memory_notification) {
  if (max_old_space_size_ == 0) return;
  HeapStatistics heap_stats;
  isolate_->GetHeapStatistics(&heap_stats);
  double ratio = static_cast<double>(heap_stats.used_heap_size())
      / max_old_space_size_;
  if (ratio >= 0.9) {
    if (!was_low_memory_notification) {
      isolate_->LowMemoryNotification();
      AfterGC(true);
    } else {
      isolate_->TerminateExecution();
      OnOOM();
    }
  }
}

void Worker::OnOOM() {
  // Called by V8 when there is no memory left.
  {
    Mutex::ScopedLock lock(mutex_);
    uv_stop(&loop_);
    Mutex::ScopedLock stopped_lock(stopped_mutex_);
    if (stopped_) return;
    stopped_ = true;
  }
  if (child_port_->IsSiblingClosed()) return;
  child_port_->Send(Message(nullptr, 0, MESSAGE_FLAG_OUT_OF_MEMORY));
}

void Worker::OnErrorMessage(Local<v8::Message> message, Local<Value> error) {
  // An uncaught exception happened. Call into JS to do some fancy
  // serialization, then send the error report to the parent Environment.
  {
    Mutex::ScopedLock lock(mutex_);
    uv_stop(&loop_);
    Mutex::ScopedLock stopped_lock(stopped_mutex_);
    if (stopped_) return;
    stopped_ = true;
  }
  if (child_port_->IsSiblingClosed()) return;
  Local<Function> serializer = env_->serialize_worker_error_function();
  if (serializer.IsEmpty()) {
    ReportException(env_, error, message);
    ABORT();
  }
  MaybeLocal<Value> buf = serializer->Call(env_->context(),
                                           Undefined(env_->isolate()),
                                           1, &error);
  if (buf.IsEmpty()) {
    child_port_->Send(Message(MESSAGE_FLAG_COULD_NOT_SERIALIZE_ERR));
    return;
  }

  CHECK(buf.ToLocalChecked()->IsUint8Array());
  Local<Object> data = buf.ToLocalChecked().As<Object>();
  size_t length = Buffer::Length(data);
  void* copy = Malloc(length);
  memcpy(copy, Buffer::Data(data), length);

  child_port_->Send(Message(copy, length, MESSAGE_FLAG_ERROR_MESSAGE));
  env_->async_hooks()->clear_id_stack();
}

Worker::Worker(Environment* env,
               Local<Object> wrap,
               const v8::ResourceConstraints& resource_constraints)
    : AsyncWrap(env, wrap, AsyncWrap::PROVIDER_WORKER) {
  MakeWeak<Worker>(this);

  // Generate a new thread id.
  {
    Mutex::ScopedLock next_thread_id_lock(next_thread_id_mutex);
    thread_id_ = next_thread_id++;
  }
  wrap->Set(env->context(), env->thread_id_string(),
            Number::New(env->isolate(), thread_id_)).FromJust();

  // Set up everything that needs to be set up in the parent environment.
  std::unique_ptr<MessagePort::FlaggedMessageListener> listener
      { new ParentListener(this) };
  parent_port_ = MessagePort::New(env, env->context(), std::move(listener));
  if (parent_port_ == nullptr) {
    // This can happen e.g. because execution is terminating.
    return;
  }
  parent_port_->MarkAsPrivileged();
  parent_port_->DoNotCloseWhenSiblingCloses();

  child_port_data_.reset(new MessagePortData(nullptr));
  MessagePort::Entangle(parent_port_, child_port_data_.get());


  object()->Set(env->context(),
                env->message_port_string(),
                parent_port_->object()).FromJust();

  array_buffer_allocator_ = ArrayBuffer::Allocator::NewDefaultAllocator();

  Isolate::CreateParams params;
  params.array_buffer_allocator = array_buffer_allocator_;
  params.constraints = resource_constraints;

  isolate_ = Isolate::New(params);
  CHECK_NE(isolate_, nullptr);
  CHECK_EQ(uv_loop_init(&loop_), 0);

  {
    Mutex::ScopedLock lock(by_isolate_mutex_);
    by_isolate_[isolate_] = this;
  }

  max_old_space_size_ = resource_constraints.max_old_space_size() * 1024 * 1024;
  isolate_->AddGCEpilogueCallback([](Isolate* isolate, v8::GCType type,
                                     v8::GCCallbackFlags flags, void* data) {
    static_cast<Worker*>(data)->AfterGC(false);
  }, static_cast<void*>(this));
  isolate_->AddMessageListener(
    [](Local<v8::Message> message, Local<Value> data) {
    Worker::ForIsolate(Isolate::GetCurrent())
        ->OnErrorMessage(message, data);
  });

  {
    // Enter an environment capable of executing code in the child Isolate
    // (and only in it).
    Locker locker(isolate_);
    Isolate::Scope isolate_scope(isolate_);
    HandleScope handle_scope(isolate_);
    Local<Context> context = Context::New(isolate_);
    Context::Scope context_scope(context);

    isolate_data_ = CreateIsolateData(isolate_, &loop_);
    CHECK_NE(isolate_data_, nullptr);

    env_ = CreateEnvironment(isolate_data_, context, 0, nullptr, 0, nullptr);
    CHECK_NE(env_, nullptr);
    env_->set_abort_on_uncaught_exception(false);
    env_->set_worker_context(this);

    env_->set_thread_id(thread_id_);
  }

  // The new isolate won't be bothered on this thread again.
  isolate_->DiscardThreadSpecificMetadata();
}

bool Worker::stopped() const {
  Mutex::ScopedLock stopped_lock(stopped_mutex_);
  return stopped_;
}

void Worker::Run() {
  {
    Locker locker(isolate_);
    Isolate::Scope isolate_scope(isolate_);
    SealHandleScope outer_seal(isolate_);

    {
      HandleScope handle_scope(isolate_);
      Context::Scope context_scope(env_->context());

      {
        HandleScope handle_scope(isolate_);
        Mutex::ScopedLock lock(mutex_);
        // Set up the message channel for receiving messages in the child.
        std::unique_ptr<MessagePort::FlaggedMessageListener> child_listener
            { new ChildListener(env_) };
        child_port_ = MessagePort::New(env_,
                                       env_->context(),
                                       std::move(child_listener),
                                       std::move(child_port_data_));
        CHECK_NE(child_port_, nullptr);
        child_port_->MarkAsPrivileged();
        env_->set_message_port(child_port_->object(isolate_));
      }

      {
        HandleScope handle_scope(isolate_);
        Environment::AsyncCallbackScope callback_scope(env_);
        Environment::AsyncHooks::ExecScope exec_scope(env_, 1, 0);
        // This loads the Node bootstrapping code.
        LoadEnvironment(env_);
      }

      {
        SealHandleScope seal(isolate_);
        bool more;
        PERFORMANCE_MARK(env_, LOOP_START);
        do {
          if (stopped()) break;
          uv_run(&loop_, UV_RUN_DEFAULT);
          if (stopped()) break;

          EmitBeforeExit(env_);

          // Emit `beforeExit` if the loop became alive either after emitting
          // event, or after running some callbacks.
          more = uv_loop_alive(&loop_);
        } while (more == true);
        PERFORMANCE_MARK(env_, LOOP_EXIT);
      }
    }

    {
      int exit_code;
      bool is_stopped = stopped();
      if (!is_stopped)
        exit_code = EmitExit(env_);
      Mutex::ScopedLock lock(mutex_);
      if (exit_code_ == 0 && !is_stopped)
        exit_code_ = exit_code;
    }

    env_->set_can_call_into_js(false);

    // Grab the parent-to-child channel and render is unusable.
    MessagePort* child_port;
    {
      Mutex::ScopedLock lock(mutex_);
      child_port = child_port_;
      child_port_ = nullptr;
    }

    child_port->Close();
    env_->stop_sub_worker_contexts();
    env_->RunCleanup();
    RunAtExit(env_);

    {
      Mutex::ScopedLock stopped_lock(stopped_mutex_);
      stopped_ = true;
    }
  }

  isolate_->DiscardThreadSpecificMetadata();

  {
    Mutex::ScopedLock lock(mutex_);
    CHECK_NE(parent_port_, nullptr);
    parent_port_->AddToQueue(Message(MESSAGE_FLAG_THREAD_STOPPED));
  }
}

void Worker::JoinThread() {
  if (thread_joined_) return;
  CHECK_EQ(uv_thread_join(&tid_), 0);
  thread_joined_ = true;

  env()->remove_sub_worker_context(this);
}

void Worker::OnThreadStopped() {
  Mutex::ScopedLock lock(mutex_);
  {
    Mutex::ScopedLock stopped_lock(stopped_mutex_);
    CHECK(stopped_);
  }
  CHECK_EQ(child_port_, nullptr);
  parent_port_->Close();
  parent_port_ = nullptr;

  // It's okay to join the thread while holding the mutex because
  // OnThreadStopped means it's no longer doing any work that might grab it.
  JoinThread();

  {
    HandleScope handle_scope(env()->isolate());
    Context::Scope context_scope(env()->context());

    // Reset the parent port as we're closing it now anyway.
    object()->Set(env()->context(),
                  env()->message_port_string(),
                  Undefined(env()->isolate())).FromJust();

    Local<Value> code = Integer::New(env()->isolate(), exit_code_);
    MakeCallback(env()->onexit_string(), 1, &code);
  }
}

Worker::~Worker() {
  Mutex::ScopedLock lock(mutex_);
  JoinThread();

  CHECK(stopped_);
  CHECK(thread_joined_);
  CHECK_EQ(child_port_, nullptr);

  if (isolate_ != nullptr) {
    {
      Locker locker(isolate_);
      Isolate::Scope isolate_scope(isolate_);
      HandleScope handle_scope(isolate_);
      env_->RunCleanup();
      FreeEnvironment(env_);

      delete isolate_data_;
      // Need to run the loop one more time to close the platform's uv_async_t
      uv_run(&loop_, UV_RUN_ONCE);
      CHECK_EQ(uv_loop_close(&loop_), 0);
    }
    isolate_->Dispose();
  }

  {
    Mutex::ScopedLock lock(by_isolate_mutex_);
    by_isolate_.erase(isolate_);
  }

  delete array_buffer_allocator_;
}

void Worker::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  v8::ResourceConstraints rc;
  CHECK_EQ(args.Length(), 1);
  CHECK(args[0]->IsObject());
  Local<Object> options = args[0].As<Object>();
  int64_t max_semi_space_size;
  int64_t max_old_space_size;

  max_semi_space_size =
      options->Get(env->context(),
                   env->max_semi_space_size_string())
                       .ToLocalChecked()->IntegerValue(env->context())
                           .FromJust();
  max_old_space_size =
      options->Get(env->context(),
                   env->max_old_space_size_string())
                       .ToLocalChecked()->IntegerValue(env->context())
                           .FromJust();

  rc.set_max_semi_space_size(max_semi_space_size);
  rc.set_max_old_space_size(max_old_space_size * 1.1);

  new Worker(env, args.This(), rc);
}

void Worker::StartThread(const FunctionCallbackInfo<Value>& args) {
  Worker* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.This());
  Mutex::ScopedLock lock(w->mutex_);

  w->env()->add_sub_worker_context(w);
  w->stopped_ = false;
  CHECK_EQ(uv_thread_create(&w->tid_, [](void* arg) {
    static_cast<Worker*>(arg)->Run();
  }, static_cast<void*>(w)), 0);
  w->thread_joined_ = false;
}

void Worker::StopThread(const FunctionCallbackInfo<Value>& args) {
  Worker* w;
  ASSIGN_OR_RETURN_UNWRAP(&w, args.This());

  w->Exit(1);
  w->JoinThread();
}

void Worker::Exit(int code) {
  Mutex::ScopedLock lock(mutex_);
  Mutex::ScopedLock stopped_lock(stopped_mutex_);
  if (!stopped_) {
    CHECK_NE(env_, nullptr);
    stopped_ = true;
    exit_code_ = code;
    parent_port_->Send(Message(MESSAGE_FLAG_STOP_THREAD_ORDER));
    isolate_->TerminateExecution();
  }
}

size_t Worker::self_size() const {
  return sizeof(*this);
}

namespace {

void SetSerializeWorkerErrorFunction(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK(args[0]->IsFunction());
  env->set_serialize_worker_error_function(args[0].As<Function>());
}

void GetEnvMessagePort(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Local<Object> port = env->message_port();
  if (!port.IsEmpty()) {
    CHECK_EQ(port->CreationContext()->GetIsolate(), args.GetIsolate());
    args.GetReturnValue().Set(port);
  }
}

static void InitWorker(Local<Object> target,
                       Local<Value> unused,
                       Local<Context> context,
                       void* priv) {
  Environment* env = Environment::GetCurrent(context);

  {
    Local<FunctionTemplate> w = env->NewFunctionTemplate(Worker::New);

    w->InstanceTemplate()->SetInternalFieldCount(1);

    AsyncWrap::AddWrapMethods(env, w);
    env->SetProtoMethod(w, "startThread", Worker::StartThread);
    env->SetProtoMethod(w, "stopThread", Worker::StopThread);

    Local<String> workerString =
        FIXED_ONE_BYTE_STRING(env->isolate(), "Worker");
    w->SetClassName(workerString);
    target->Set(workerString, w->GetFunction());
  }

  env->SetMethod(target, "getEnvMessagePort", GetEnvMessagePort);
  env->SetMethod(target, "setSerializeWorkerErrorFunction",
                 SetSerializeWorkerErrorFunction);

  NODE_DEFINE_CONSTANT(target, MESSAGE_FLAG_NONE);
  NODE_DEFINE_CONSTANT(target, MESSAGE_FLAG_COULD_NOT_SERIALIZE_ERR);
  NODE_DEFINE_CONSTANT(target, MESSAGE_FLAG_OUT_OF_MEMORY);
  NODE_DEFINE_CONSTANT(target, MESSAGE_FLAG_ERROR_MESSAGE);
  NODE_DEFINE_CONSTANT(target, MESSAGE_FLAG_CUSTOM_OFFSET);
}

}  // anonymous namespace

}  // namespace worker
}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(worker, node::worker::InitWorker)
