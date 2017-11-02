#include "inspector_agent.h"

#include "inspector_io.h"
#include "node_internals.h"
#include "v8-inspector.h"
#include "v8-platform.h"

#include "libplatform/libplatform.h"

#include <string.h>
#include <sstream>
#include <unordered_map>
#include <vector>

#ifdef __POSIX__
#include <limits.h>
#include <unistd.h>  // setuid, getuid
#endif  // __POSIX__

namespace node {
namespace inspector {
namespace {

using node::FatalError;

using v8::Array;
using v8::Context;
using v8::Function;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::Value;

using v8_inspector::StringBuffer;
using v8_inspector::StringView;
using v8_inspector::V8Inspector;
using v8_inspector::V8InspectorClient;

static uv_sem_t start_io_thread_semaphore;
static uv_async_t start_io_thread_async;

class StartIoTask : public v8::Task {
 public:
  explicit StartIoTask(Agent* agent) : agent(agent) {}

  void Run() override {
    agent->StartIoThread(false);
  }

 private:
  Agent* agent;
};

std::unique_ptr<StringBuffer> ToProtocolString(Isolate* isolate,
                                               Local<Value> value) {
  TwoByteValue buffer(isolate, value);
  return StringBuffer::create(StringView(*buffer, buffer.length()));
}

// Called on the main thread.
void StartIoThreadAsyncCallback(uv_async_t* handle) {
  static_cast<Agent*>(handle->data)->StartIoThread(false);
}

void StartIoInterrupt(Isolate* isolate, void* agent) {
  static_cast<Agent*>(agent)->StartIoThread(false);
}


#ifdef __POSIX__
static void StartIoThreadWakeup(int signo) {
  uv_sem_post(&start_io_thread_semaphore);
}

inline void* StartIoThreadMain(void* unused) {
  for (;;) {
    uv_sem_wait(&start_io_thread_semaphore);
    Agent* agent = static_cast<Agent*>(start_io_thread_async.data);
    if (agent != nullptr)
      agent->RequestIoThreadStart();
  }
  return nullptr;
}

static int StartDebugSignalHandler() {
  // Start a watchdog thread for calling v8::Debug::DebugBreak() because
  // it's not safe to call directly from the signal handler, it can
  // deadlock with the thread it interrupts.
  CHECK_EQ(0, uv_sem_init(&start_io_thread_semaphore, 0));
  pthread_attr_t attr;
  CHECK_EQ(0, pthread_attr_init(&attr));
  // Don't shrink the thread's stack on FreeBSD.  Said platform decided to
  // follow the pthreads specification to the letter rather than in spirit:
  // https://lists.freebsd.org/pipermail/freebsd-current/2014-March/048885.html
#ifndef __FreeBSD__
  CHECK_EQ(0, pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN));
#endif  // __FreeBSD__
  CHECK_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
  sigset_t sigmask;
  // Mask all signals.
  sigfillset(&sigmask);
  CHECK_EQ(0, pthread_sigmask(SIG_SETMASK, &sigmask, &sigmask));
  pthread_t thread;
  const int err = pthread_create(&thread, &attr,
                                 StartIoThreadMain, nullptr);
  // Restore original mask
  CHECK_EQ(0, pthread_sigmask(SIG_SETMASK, &sigmask, nullptr));
  CHECK_EQ(0, pthread_attr_destroy(&attr));
  if (err != 0) {
    fprintf(stderr, "node[%d]: pthread_create: %s\n", getpid(), strerror(err));
    fflush(stderr);
    // Leave SIGUSR1 blocked.  We don't install a signal handler,
    // receiving the signal would terminate the process.
    return -err;
  }
  RegisterSignalHandler(SIGUSR1, StartIoThreadWakeup);
  // Unblock SIGUSR1.  A pending SIGUSR1 signal will now be delivered.
  sigemptyset(&sigmask);
  sigaddset(&sigmask, SIGUSR1);
  CHECK_EQ(0, pthread_sigmask(SIG_UNBLOCK, &sigmask, nullptr));
  return 0;
}
#endif  // __POSIX__


#ifdef _WIN32
DWORD WINAPI StartIoThreadProc(void* arg) {
  Agent* agent = static_cast<Agent*>(start_io_thread_async.data);
  if (agent != nullptr)
    agent->RequestIoThreadStart();
  return 0;
}

static int GetDebugSignalHandlerMappingName(DWORD pid, wchar_t* buf,
                                            size_t buf_len) {
  return _snwprintf(buf, buf_len, L"node-debug-handler-%u", pid);
}

static int StartDebugSignalHandler() {
  wchar_t mapping_name[32];
  HANDLE mapping_handle;
  DWORD pid;
  LPTHREAD_START_ROUTINE* handler;

  pid = GetCurrentProcessId();

  if (GetDebugSignalHandlerMappingName(pid,
                                       mapping_name,
                                       arraysize(mapping_name)) < 0) {
    return -1;
  }

  mapping_handle = CreateFileMappingW(INVALID_HANDLE_VALUE,
                                      nullptr,
                                      PAGE_READWRITE,
                                      0,
                                      sizeof *handler,
                                      mapping_name);
  if (mapping_handle == nullptr) {
    return -1;
  }

  handler = reinterpret_cast<LPTHREAD_START_ROUTINE*>(
      MapViewOfFile(mapping_handle,
                    FILE_MAP_ALL_ACCESS,
                    0,
                    0,
                    sizeof *handler));
  if (handler == nullptr) {
    CloseHandle(mapping_handle);
    return -1;
  }

  *handler = StartIoThreadProc;

  UnmapViewOfFile(static_cast<void*>(handler));

  return 0;
}
#endif  // _WIN32


// Used in NodeInspectorClient::currentTimeMS() below.
const int NANOS_PER_MSEC = 1000000;
const int CONTEXT_GROUP_ID = 1;

class ChannelImpl final : public v8_inspector::V8Inspector::Channel {
 public:
  explicit ChannelImpl(V8Inspector* inspector,
                       InspectorSessionDelegate* delegate)
                       : delegate_(delegate) {
    session_ = inspector->connect(1, this, StringView());
  }

  virtual ~ChannelImpl() {}

  void dispatchProtocolMessage(const StringView& message) {
    session_->dispatchProtocolMessage(message);
  }

  bool waitForFrontendMessage() {
    return delegate_->WaitForFrontendMessageWhilePaused();
  }

  void schedulePauseOnNextStatement(const std::string& reason) {
    std::unique_ptr<StringBuffer> buffer = Utf8ToStringView(reason);
    session_->schedulePauseOnNextStatement(buffer->string(), buffer->string());
  }

  InspectorSessionDelegate* delegate() {
    return delegate_;
  }

 private:
  void sendResponse(
      int callId,
      std::unique_ptr<v8_inspector::StringBuffer> message) override {
    sendMessageToFrontend(message->string());
  }

  void sendNotification(
      std::unique_ptr<v8_inspector::StringBuffer> message) override {
    sendMessageToFrontend(message->string());
  }

  void flushProtocolNotifications() override { }

  void sendMessageToFrontend(const StringView& message) {
    delegate_->SendMessageToFrontend(message);
  }

  InspectorSessionDelegate* const delegate_;
  std::unique_ptr<v8_inspector::V8InspectorSession> session_;
};

class InspectorTimer {
 public:
  InspectorTimer(uv_loop_t* loop,
                 double interval_s,
                 V8InspectorClient::TimerCallback callback,
                 void* data) : timer_(),
                               callback_(callback),
                               data_(data) {
    uv_timer_init(loop, &timer_);
    int64_t interval_ms = 1000 * interval_s;
    uv_timer_start(&timer_, OnTimer, interval_ms, interval_ms);
  }

  InspectorTimer(const InspectorTimer&) = delete;

  void Stop() {
    uv_timer_stop(&timer_);
    uv_close(reinterpret_cast<uv_handle_t*>(&timer_), TimerClosedCb);
  }

 private:
  static void OnTimer(uv_timer_t* uvtimer) {
    InspectorTimer* timer = node::ContainerOf(&InspectorTimer::timer_, uvtimer);
    timer->callback_(timer->data_);
  }

  static void TimerClosedCb(uv_handle_t* uvtimer) {
    InspectorTimer* timer =
        node::ContainerOf(&InspectorTimer::timer_,
                          reinterpret_cast<uv_timer_t*>(uvtimer));
    delete timer;
  }

  ~InspectorTimer() {}

  uv_timer_t timer_;
  V8InspectorClient::TimerCallback callback_;
  void* data_;
};

class InspectorTimerHandle {
 public:
  InspectorTimerHandle(uv_loop_t* loop, double interval_s,
                       V8InspectorClient::TimerCallback callback, void* data) {
    timer_ = new InspectorTimer(loop, interval_s, callback, data);
  }

  InspectorTimerHandle(const InspectorTimerHandle&) = delete;

  ~InspectorTimerHandle() {
    CHECK_NE(timer_, nullptr);
    timer_->Stop();
    timer_ = nullptr;
  }
 private:
  InspectorTimer* timer_;
};
}  // namespace

class NodeInspectorClient : public V8InspectorClient {
 public:
  NodeInspectorClient(node::Environment* env, node::NodePlatform* platform)
      : env_(env), platform_(platform), terminated_(false),
        running_nested_loop_(false) {
    client_ = V8Inspector::create(env->isolate(), this);
    contextCreated(env->context(), "Node.js Main Context");
  }

  void runMessageLoopOnPause(int context_group_id) override {
    CHECK_NE(channel_, nullptr);
    if (running_nested_loop_)
      return;
    terminated_ = false;
    running_nested_loop_ = true;
    while (!terminated_ && channel_->waitForFrontendMessage()) {
      platform_->FlushForegroundTasks(env_->isolate());
    }
    terminated_ = false;
    running_nested_loop_ = false;
  }

  double currentTimeMS() override {
    return uv_hrtime() * 1.0 / NANOS_PER_MSEC;
  }

  void maxAsyncCallStackDepthChanged(int depth) override {
    if (depth == 0) {
      env_->inspector_agent()->DisableAsyncHook();
    } else {
      env_->inspector_agent()->EnableAsyncHook();
    }
  }

  void contextCreated(Local<Context> context, const std::string& name) {
    std::unique_ptr<StringBuffer> name_buffer = Utf8ToStringView(name);
    v8_inspector::V8ContextInfo info(context, CONTEXT_GROUP_ID,
                                     name_buffer->string());
    client_->contextCreated(info);
  }

  void contextDestroyed(Local<Context> context) {
    client_->contextDestroyed(context);
  }

  void quitMessageLoopOnPause() override {
    terminated_ = true;
  }

  void connectFrontend(InspectorSessionDelegate* delegate) {
    CHECK_EQ(channel_, nullptr);
    channel_ = std::unique_ptr<ChannelImpl>(
        new ChannelImpl(client_.get(), delegate));
  }

  void disconnectFrontend() {
    quitMessageLoopOnPause();
    channel_.reset();
  }

  void dispatchMessageFromFrontend(const StringView& message) {
    CHECK_NE(channel_, nullptr);
    channel_->dispatchProtocolMessage(message);
  }

  Local<Context> ensureDefaultContextInGroup(int contextGroupId) override {
    return env_->context();
  }

  void installAdditionalCommandLineAPI(Local<Context> context,
                                       Local<Object> target) override {
    Local<Object> console_api = env_->inspector_console_api_object();

    Local<Array> properties =
        console_api->GetOwnPropertyNames(context).ToLocalChecked();
    for (uint32_t i = 0; i < properties->Length(); ++i) {
      Local<Value> key = properties->Get(context, i).ToLocalChecked();
      target->Set(context,
                  key,
                  console_api->Get(context, key).ToLocalChecked()).FromJust();
    }
  }

  void FatalException(Local<Value> error, Local<v8::Message> message) {
    Local<Context> context = env_->context();

    int script_id = message->GetScriptOrigin().ScriptID()->Value();

    Local<v8::StackTrace> stack_trace = message->GetStackTrace();

    if (!stack_trace.IsEmpty() &&
        stack_trace->GetFrameCount() > 0 &&
        script_id == stack_trace->GetFrame(0)->GetScriptId()) {
      script_id = 0;
    }

    const uint8_t DETAILS[] = "Uncaught";

    Isolate* isolate = context->GetIsolate();

    client_->exceptionThrown(
        context,
        StringView(DETAILS, sizeof(DETAILS) - 1),
        error,
        ToProtocolString(isolate, message->Get())->string(),
        ToProtocolString(isolate, message->GetScriptResourceName())->string(),
        message->GetLineNumber(context).FromMaybe(0),
        message->GetStartColumn(context).FromMaybe(0),
        client_->createStackTrace(stack_trace),
        script_id);
  }

  ChannelImpl* channel() {
    return channel_.get();
  }

  void startRepeatingTimer(double interval_s,
                           TimerCallback callback,
                           void* data) override {
    timers_.emplace(std::piecewise_construct, std::make_tuple(data),
                    std::make_tuple(env_->event_loop(), interval_s, callback,
                                    data));
  }

  void cancelTimer(void* data) override {
    timers_.erase(data);
  }

  // Async stack traces instrumentation.
  void AsyncTaskScheduled(const StringView& task_name, void* task,
                          bool recurring) {
    client_->asyncTaskScheduled(task_name, task, recurring);
  }

  void AsyncTaskCanceled(void* task) {
    client_->asyncTaskCanceled(task);
  }

  void AsyncTaskStarted(void* task) {
    client_->asyncTaskStarted(task);
  }

  void AsyncTaskFinished(void* task) {
    client_->asyncTaskFinished(task);
  }

  void AllAsyncTasksCanceled() {
    client_->allAsyncTasksCanceled();
  }

 private:
  node::Environment* env_;
  node::NodePlatform* platform_;
  bool terminated_;
  bool running_nested_loop_;
  std::unique_ptr<V8Inspector> client_;
  std::unique_ptr<ChannelImpl> channel_;
  std::unordered_map<void*, InspectorTimerHandle> timers_;
};

Agent::Agent(Environment* env) : parent_env_(env),
                                 client_(nullptr),
                                 platform_(nullptr),
                                 enabled_(false),
                                 next_context_number_(1),
                                 pending_enable_async_hook_(false),
                                 pending_disable_async_hook_(false) {}

// Destructor needs to be defined here in implementation file as the header
// does not have full definition of some classes.
Agent::~Agent() {
}

bool Agent::Start(node::NodePlatform* platform, const char* path,
                  const DebugOptions& options) {
  path_ = path == nullptr ? "" : path;
  debug_options_ = options;
  client_ =
      std::unique_ptr<NodeInspectorClient>(
          new NodeInspectorClient(parent_env_, platform));
  platform_ = platform;
  CHECK_EQ(0, uv_async_init(uv_default_loop(),
                            &start_io_thread_async,
                            StartIoThreadAsyncCallback));
  start_io_thread_async.data = this;
  uv_unref(reinterpret_cast<uv_handle_t*>(&start_io_thread_async));

  // Ignore failure, SIGUSR1 won't work, but that should not block node start.
  StartDebugSignalHandler();
  if (options.inspector_enabled()) {
    // This will return false if listen failed on the inspector port.
    return StartIoThread(options.wait_for_connect());
  }
  return true;
}

bool Agent::StartIoThread(bool wait_for_connect) {
  if (io_ != nullptr)
    return true;

  CHECK_NE(client_, nullptr);

  enabled_ = true;
  io_ = std::unique_ptr<InspectorIo>(
      new InspectorIo(parent_env_, platform_, path_, debug_options_,
                      wait_for_connect));
  if (!io_->Start()) {
    client_.reset();
    return false;
  }

  v8::Isolate* isolate = parent_env_->isolate();
  HandleScope handle_scope(isolate);
  auto context = parent_env_->context();

  // Send message to enable debug in workers
  Local<Object> process_object = parent_env_->process_object();
  Local<Value> emit_fn =
      process_object->Get(context, FIXED_ONE_BYTE_STRING(isolate, "emit"))
          .ToLocalChecked();
  // In case the thread started early during the startup
  if (!emit_fn->IsFunction())
    return true;

  Local<Object> message = Object::New(isolate);
  message->Set(context, FIXED_ONE_BYTE_STRING(isolate, "cmd"),
               FIXED_ONE_BYTE_STRING(isolate, "NODE_DEBUG_ENABLED")).FromJust();
  Local<Value> argv[] = {
    FIXED_ONE_BYTE_STRING(isolate, "internalMessage"),
    message
  };
  MakeCallback(parent_env_->isolate(), process_object, emit_fn.As<Function>(),
               arraysize(argv), argv, {0, 0});

  return true;
}

void Agent::Stop() {
  if (io_ != nullptr) {
    io_->Stop();
    io_.reset();
    enabled_ = false;
  }
}

void Agent::Connect(InspectorSessionDelegate* delegate) {
  enabled_ = true;
  client_->connectFrontend(delegate);
}

bool Agent::IsConnected() {
  return io_ && io_->IsConnected();
}

void Agent::WaitForDisconnect() {
  CHECK_NE(client_, nullptr);
  client_->contextDestroyed(parent_env_->context());
  if (io_ != nullptr) {
    io_->WaitForDisconnect();
  }
}

void Agent::FatalException(Local<Value> error, Local<v8::Message> message) {
  if (!IsStarted())
    return;
  client_->FatalException(error, message);
  WaitForDisconnect();
}

void Agent::Dispatch(const StringView& message) {
  CHECK_NE(client_, nullptr);
  client_->dispatchMessageFromFrontend(message);
}

void Agent::Disconnect() {
  CHECK_NE(client_, nullptr);
  client_->disconnectFrontend();
}

void Agent::RunMessageLoop() {
  CHECK_NE(client_, nullptr);
  client_->runMessageLoopOnPause(CONTEXT_GROUP_ID);
}

InspectorSessionDelegate* Agent::delegate() {
  CHECK_NE(client_, nullptr);
  ChannelImpl* channel = client_->channel();
  if (channel == nullptr)
    return nullptr;
  return channel->delegate();
}

void Agent::PauseOnNextJavascriptStatement(const std::string& reason) {
  ChannelImpl* channel = client_->channel();
  if (channel != nullptr)
    channel->schedulePauseOnNextStatement(reason);
}

void Agent::RegisterAsyncHook(Isolate* isolate,
                              v8::Local<v8::Function> enable_function,
                              v8::Local<v8::Function> disable_function) {
  enable_async_hook_function_.Reset(isolate, enable_function);
  disable_async_hook_function_.Reset(isolate, disable_function);
  if (pending_enable_async_hook_) {
    CHECK(!pending_disable_async_hook_);
    pending_enable_async_hook_ = false;
    EnableAsyncHook();
  } else if (pending_disable_async_hook_) {
    CHECK(!pending_enable_async_hook_);
    pending_disable_async_hook_ = false;
    DisableAsyncHook();
  }
}

void Agent::EnableAsyncHook() {
  if (!enable_async_hook_function_.IsEmpty()) {
    Isolate* isolate = parent_env_->isolate();
    ToggleAsyncHook(isolate, enable_async_hook_function_.Get(isolate));
  } else if (pending_disable_async_hook_) {
    CHECK(!pending_enable_async_hook_);
    pending_disable_async_hook_ = false;
  } else {
    pending_enable_async_hook_ = true;
  }
}

void Agent::DisableAsyncHook() {
  if (!disable_async_hook_function_.IsEmpty()) {
    Isolate* isolate = parent_env_->isolate();
    ToggleAsyncHook(isolate, disable_async_hook_function_.Get(isolate));
  } else if (pending_enable_async_hook_) {
    CHECK(!pending_disable_async_hook_);
    pending_enable_async_hook_ = false;
  } else {
    pending_disable_async_hook_ = true;
  }
}

void Agent::ToggleAsyncHook(Isolate* isolate, Local<Function> fn) {
  HandleScope handle_scope(isolate);
  auto context = parent_env_->context();
  auto result = fn->Call(context, Undefined(isolate), 0, nullptr);
  if (result.IsEmpty()) {
    FatalError(
        "node::inspector::Agent::ToggleAsyncHook",
        "Cannot toggle Inspector's AsyncHook, please report this.");
  }
}

void Agent::AsyncTaskScheduled(const StringView& task_name, void* task,
                               bool recurring) {
  client_->AsyncTaskScheduled(task_name, task, recurring);
}

void Agent::AsyncTaskCanceled(void* task) {
  client_->AsyncTaskCanceled(task);
}

void Agent::AsyncTaskStarted(void* task) {
  client_->AsyncTaskStarted(task);
}

void Agent::AsyncTaskFinished(void* task) {
  client_->AsyncTaskFinished(task);
}

void Agent::AllAsyncTasksCanceled() {
  client_->AllAsyncTasksCanceled();
}

void Agent::RequestIoThreadStart() {
  // We need to attempt to interrupt V8 flow (in case Node is running
  // continuous JS code) and to wake up libuv thread (in case Node is waiting
  // for IO events)
  uv_async_send(&start_io_thread_async);
  v8::Isolate* isolate = parent_env_->isolate();
  platform_->CallOnForegroundThread(isolate, new StartIoTask(this));
  isolate->RequestInterrupt(StartIoInterrupt, this);
  uv_async_send(&start_io_thread_async);
}

void Agent::ContextCreated(Local<Context> context) {
  if (client_ == nullptr)  // This happens for a main context
    return;
  std::ostringstream name;
  name << "VM Context " << next_context_number_++;
  client_->contextCreated(context, name.str());
}

bool Agent::IsWaitingForConnect() {
  return debug_options_.wait_for_connect();
}

}  // namespace inspector
}  // namespace node

