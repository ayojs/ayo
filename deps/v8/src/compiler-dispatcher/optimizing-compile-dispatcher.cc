// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler-dispatcher/optimizing-compile-dispatcher.h"

#include "src/base/atomicops.h"
#include "src/compilation-info.h"
#include "src/compiler.h"
#include "src/isolate.h"
#include "src/objects-inl.h"
#include "src/tracing/trace-event.h"
#include "src/v8.h"

namespace v8 {
namespace internal {

namespace {

void DisposeCompilationJob(CompilationJob* job, bool restore_function_code) {
  if (restore_function_code) {
    Handle<JSFunction> function = job->compilation_info()->closure();
    function->ReplaceCode(function->shared()->code());
    if (function->IsInOptimizationQueue()) {
      function->ClearOptimizationMarker();
    }
    // TODO(mvstanton): We can't call ensureliterals here due to allocation,
    // but we probably shouldn't call ReplaceCode either, as this
    // sometimes runs on the worker thread!
    // JSFunction::EnsureLiterals(function);
  }
  delete job;
}

}  // namespace

class OptimizingCompileDispatcher::CompileTask : public v8::Task {
 public:
  explicit CompileTask(Isolate* isolate,
                       OptimizingCompileDispatcher* dispatcher)
      : isolate_(isolate), dispatcher_(dispatcher) {
    base::LockGuard<base::Mutex> lock_guard(&dispatcher_->ref_count_mutex_);
    ++dispatcher_->ref_count_;
  }

  virtual ~CompileTask() {}

 private:
  // v8::Task overrides.
  void Run() override {
    DisallowHeapAllocation no_allocation;
    DisallowHandleAllocation no_handles;
    DisallowHandleDereference no_deref;

    {
      TimerEventScope<TimerEventRecompileConcurrent> timer(isolate_);

      TRACE_EVENT0(TRACE_DISABLED_BY_DEFAULT("v8.compile"),
                   "V8.RecompileConcurrent");

      if (dispatcher_->recompilation_delay_ != 0) {
        base::OS::Sleep(base::TimeDelta::FromMilliseconds(
            dispatcher_->recompilation_delay_));
      }

      dispatcher_->CompileNext(dispatcher_->NextInput(true));
    }
    {
      base::LockGuard<base::Mutex> lock_guard(&dispatcher_->ref_count_mutex_);
      if (--dispatcher_->ref_count_ == 0) {
        dispatcher_->ref_count_zero_.NotifyOne();
      }
    }
  }

  Isolate* isolate_;
  OptimizingCompileDispatcher* dispatcher_;

  DISALLOW_COPY_AND_ASSIGN(CompileTask);
};

OptimizingCompileDispatcher::~OptimizingCompileDispatcher() {
#ifdef DEBUG
  {
    base::LockGuard<base::Mutex> lock_guard(&ref_count_mutex_);
    DCHECK_EQ(0, ref_count_);
  }
#endif
  DCHECK_EQ(0, input_queue_length_);
  DeleteArray(input_queue_);
}

CompilationJob* OptimizingCompileDispatcher::NextInput(bool check_if_flushing) {
  base::LockGuard<base::Mutex> access_input_queue_(&input_queue_mutex_);
  if (input_queue_length_ == 0) return NULL;
  CompilationJob* job = input_queue_[InputQueueIndex(0)];
  DCHECK_NOT_NULL(job);
  input_queue_shift_ = InputQueueIndex(1);
  input_queue_length_--;
  if (check_if_flushing) {
    if (static_cast<ModeFlag>(base::Acquire_Load(&mode_)) == FLUSH) {
      AllowHandleDereference allow_handle_dereference;
      DisposeCompilationJob(job, true);
      return NULL;
    }
  }
  return job;
}

void OptimizingCompileDispatcher::CompileNext(CompilationJob* job) {
  if (!job) return;

  // The function may have already been optimized by OSR.  Simply continue.
  CompilationJob::Status status = job->ExecuteJob();
  USE(status);  // Prevent an unused-variable error.

  // The function may have already been optimized by OSR.  Simply continue.
  // Use a mutex to make sure that functions marked for install
  // are always also queued.
  base::LockGuard<base::Mutex> access_output_queue_(&output_queue_mutex_);
  output_queue_.push(job);
  isolate_->stack_guard()->RequestInstallCode();
}

void OptimizingCompileDispatcher::FlushOutputQueue(bool restore_function_code) {
  for (;;) {
    CompilationJob* job = NULL;
    {
      base::LockGuard<base::Mutex> access_output_queue_(&output_queue_mutex_);
      if (output_queue_.empty()) return;
      job = output_queue_.front();
      output_queue_.pop();
    }

    DisposeCompilationJob(job, restore_function_code);
  }
}

void OptimizingCompileDispatcher::Flush(BlockingBehavior blocking_behavior) {
  if (blocking_behavior == BlockingBehavior::kDontBlock) {
    if (FLAG_block_concurrent_recompilation) Unblock();
    base::LockGuard<base::Mutex> access_input_queue_(&input_queue_mutex_);
    while (input_queue_length_ > 0) {
      CompilationJob* job = input_queue_[InputQueueIndex(0)];
      DCHECK_NOT_NULL(job);
      input_queue_shift_ = InputQueueIndex(1);
      input_queue_length_--;
      DisposeCompilationJob(job, true);
    }
    FlushOutputQueue(true);
    if (FLAG_trace_concurrent_recompilation) {
      PrintF("  ** Flushed concurrent recompilation queues (not blocking).\n");
    }
    return;
  }
  base::Release_Store(&mode_, static_cast<base::AtomicWord>(FLUSH));
  if (FLAG_block_concurrent_recompilation) Unblock();
  {
    base::LockGuard<base::Mutex> lock_guard(&ref_count_mutex_);
    while (ref_count_ > 0) ref_count_zero_.Wait(&ref_count_mutex_);
    base::Release_Store(&mode_, static_cast<base::AtomicWord>(COMPILE));
  }
  FlushOutputQueue(true);
  if (FLAG_trace_concurrent_recompilation) {
    PrintF("  ** Flushed concurrent recompilation queues.\n");
  }
}

void OptimizingCompileDispatcher::Stop() {
  base::Release_Store(&mode_, static_cast<base::AtomicWord>(FLUSH));
  if (FLAG_block_concurrent_recompilation) Unblock();
  {
    base::LockGuard<base::Mutex> lock_guard(&ref_count_mutex_);
    while (ref_count_ > 0) ref_count_zero_.Wait(&ref_count_mutex_);
    base::Release_Store(&mode_, static_cast<base::AtomicWord>(COMPILE));
  }

  if (recompilation_delay_ != 0) {
    // At this point the optimizing compiler thread's event loop has stopped.
    // There is no need for a mutex when reading input_queue_length_.
    while (input_queue_length_ > 0) CompileNext(NextInput());
    InstallOptimizedFunctions();
  } else {
    FlushOutputQueue(false);
  }
}

void OptimizingCompileDispatcher::InstallOptimizedFunctions() {
  HandleScope handle_scope(isolate_);

  for (;;) {
    CompilationJob* job = NULL;
    {
      base::LockGuard<base::Mutex> access_output_queue_(&output_queue_mutex_);
      if (output_queue_.empty()) return;
      job = output_queue_.front();
      output_queue_.pop();
    }
    CompilationInfo* info = job->compilation_info();
    Handle<JSFunction> function(*info->closure());
    if (function->HasOptimizedCode()) {
      if (FLAG_trace_concurrent_recompilation) {
        PrintF("  ** Aborting compilation for ");
        function->ShortPrint();
        PrintF(" as it has already been optimized.\n");
      }
      DisposeCompilationJob(job, false);
    } else {
      Compiler::FinalizeCompilationJob(job);
    }
  }
}

void OptimizingCompileDispatcher::QueueForOptimization(CompilationJob* job) {
  DCHECK(IsQueueAvailable());
  {
    // Add job to the back of the input queue.
    base::LockGuard<base::Mutex> access_input_queue(&input_queue_mutex_);
    DCHECK_LT(input_queue_length_, input_queue_capacity_);
    input_queue_[InputQueueIndex(input_queue_length_)] = job;
    input_queue_length_++;
  }
  if (FLAG_block_concurrent_recompilation) {
    blocked_jobs_++;
  } else {
    V8::GetCurrentPlatform()->CallOnBackgroundThread(
        new CompileTask(isolate_, this), v8::Platform::kShortRunningTask);
  }
}

void OptimizingCompileDispatcher::Unblock() {
  while (blocked_jobs_ > 0) {
    V8::GetCurrentPlatform()->CallOnBackgroundThread(
        new CompileTask(isolate_, this), v8::Platform::kShortRunningTask);
    blocked_jobs_--;
  }
}

}  // namespace internal
}  // namespace v8
