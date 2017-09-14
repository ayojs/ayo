#ifndef SRC_NODE_PLATFORM_H_
#define SRC_NODE_PLATFORM_H_

#include <queue>
#include <vector>
#include <unordered_map>

#include "libplatform/libplatform.h"
#include "node_mutex.h"
#include "uv.h"

namespace node {

class NodePlatform;
class IsolateData;

template <class T>
class TaskQueue {
 public:
  TaskQueue();
  ~TaskQueue() {}

  void Push(T* task);
  T* Pop();
  T* BlockingPop();
  void NotifyOfCompletion();
  void BlockingDrain();
  void Stop();

 private:
  Mutex lock_;
  ConditionVariable tasks_available_;
  ConditionVariable tasks_drained_;
  int outstanding_tasks_;
  bool stopped_;
  std::queue<T*> task_queue_;
};

class PerIsolatePlatformData {
 public:
  PerIsolatePlatformData(v8::Isolate* isolate, uv_loop_t* loop);
  ~PerIsolatePlatformData();

  void CallOnForegroundThread(v8::Task* task);
  void CallDelayedOnForegroundThread(v8::Task* task, double delay_in_seconds);

  void Shutdown();

  void ref();
  int unref();

  void FlushForegroundTasksInternal();
 private:
  static void FlushTasks(uv_async_t* handle);

  int ref_count_ = 1;
  v8::Isolate* isolate_;
  uv_loop_t* const loop_;
  uv_async_t flush_tasks_;
  TaskQueue<v8::Task> foreground_tasks_;
  TaskQueue<std::pair<v8::Task*, double>> foreground_delayed_tasks_;
};

class NodePlatform : public v8::Platform {
 public:
  NodePlatform(int thread_pool_size, v8::TracingController* tracing_controller);
  virtual ~NodePlatform() {}

  void DrainBackgroundTasks();
  void Shutdown();

  // v8::Platform implementation.
  size_t NumberOfAvailableBackgroundThreads() override;
  void CallOnBackgroundThread(v8::Task* task,
                              ExpectedRuntime expected_runtime) override;
  void CallOnForegroundThread(v8::Isolate* isolate, v8::Task* task) override;
  void CallDelayedOnForegroundThread(v8::Isolate* isolate, v8::Task* task,
                                     double delay_in_seconds) override;
  bool IdleTasksEnabled(v8::Isolate* isolate) override;
  double MonotonicallyIncreasingTime() override;
  v8::TracingController* GetTracingController() override;

  void FlushForegroundTasks(v8::Isolate* isolate);

  void RegisterIsolate(IsolateData* isolate_data, uv_loop_t* loop);
  void UnregisterIsolate(IsolateData* isolate_data);

  static NodePlatform* platform;

 private:
  PerIsolatePlatformData* ForIsolate(v8::Isolate* isolate);

  Mutex lock_;
  std::unordered_map<v8::Isolate*, PerIsolatePlatformData*> per_isolate_;
  TaskQueue<v8::Task> background_tasks_;
  std::vector<std::unique_ptr<uv_thread_t>> threads_;

  std::unique_ptr<v8::TracingController> tracing_controller_;
};

}  // namespace node

#endif  // SRC_NODE_PLATFORM_H_
