#ifndef SRC_NODE_WORKER_H_
#define SRC_NODE_WORKER_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "node_messaging.h"
#include <unordered_map>

namespace node {
namespace worker {

// Generated in C++ in the child, handled in JS on the parent side
#define MESSAGE_FLAG_OUT_OF_MEMORY 3
#define MESSAGE_FLAG_ERROR_MESSAGE 4
#define MESSAGE_FLAG_COULD_NOT_SERIALIZE_ERR 5

// A worker thread.
class Worker : public AsyncWrap {
 public:
  Worker(Environment* env,
         v8::Local<v8::Object> wrap,
         const v8::ResourceConstraints& resource_constraints);
  ~Worker();

  // Run the worker. This is only called from the worker thread.
  void Run();
  // Forcibly exit the thread with a specified exit code.
  void Exit(int code);
  void JoinThread();

  size_t self_size() const override;
  bool stopped() const;

  static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void StartThread(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void StopThread(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetMessagePort(const v8::FunctionCallbackInfo<v8::Value>& args);

  void OnThreadStopped();

  void FatalError(const char* location, const char* message);
  void OnErrorMessage(v8::Local<v8::Message> message,
                      v8::Local<v8::Value> error);

  static Worker* ForIsolate(v8::Isolate* isolate);

 private:
  void AfterGC(bool was_low_memory_notification);
  void OnOOM();

  uv_loop_t loop_;
  IsolateData* isolate_data_ = nullptr;
  v8::Isolate* isolate_ = nullptr;
  Environment* env_ = nullptr;
  v8::ArrayBuffer::Allocator* array_buffer_allocator_ = nullptr;
  uv_thread_t tid_;
  double max_old_space_size_;

  // This mutex protects access to all variables listed below it.
  mutable Mutex mutex_;
  // This only protect stopped_. If both locks are acquired, this needs to
  // be the latter one.
  mutable Mutex stopped_mutex_;
  bool stopped_ = true;
  bool thread_joined_ = true;
  int exit_code_ = 0;
  double thread_id_ = -1;

  std::unique_ptr<MessagePortData> child_port_data_;

  // The child port is always kept alive by the child Environment's persistent
  // handle to it.
  MessagePort* child_port_ = nullptr;
  // This is always kept alive because the JS object associated with the Worker
  // instance refers to it via its MessagePort property.
  MessagePort* parent_port_ = nullptr;

  static Mutex by_isolate_mutex_;
  static std::unordered_map<v8::Isolate*, Worker*> by_isolate_;
};

}  // namespace worker
}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS


#endif  // SRC_NODE_WORKER_H_
