// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/heap/concurrent-marking.h"

#include <stack>
#include <unordered_map>

#include "src/heap/heap-inl.h"
#include "src/heap/heap.h"
#include "src/heap/mark-compact-inl.h"
#include "src/heap/mark-compact.h"
#include "src/heap/marking.h"
#include "src/heap/objects-visiting-inl.h"
#include "src/heap/objects-visiting.h"
#include "src/heap/worklist.h"
#include "src/isolate.h"
#include "src/utils-inl.h"
#include "src/utils.h"
#include "src/v8.h"

namespace v8 {
namespace internal {

class ConcurrentMarkingState final
    : public MarkingStateBase<ConcurrentMarkingState, AccessMode::ATOMIC> {
 public:
  explicit ConcurrentMarkingState(LiveBytesMap* live_bytes)
      : live_bytes_(live_bytes) {}

  Bitmap* bitmap(const MemoryChunk* chunk) {
    return Bitmap::FromAddress(chunk->address() + MemoryChunk::kHeaderSize);
  }

  void IncrementLiveBytes(MemoryChunk* chunk, intptr_t by) {
    (*live_bytes_)[chunk] += by;
  }

  // The live_bytes and SetLiveBytes methods of the marking state are
  // not used by the concurrent marker.

 private:
  LiveBytesMap* live_bytes_;
};

// Helper class for storing in-object slot addresses and values.
class SlotSnapshot {
 public:
  SlotSnapshot() : number_of_slots_(0) {}
  int number_of_slots() const { return number_of_slots_; }
  Object** slot(int i) const { return snapshot_[i].first; }
  Object* value(int i) const { return snapshot_[i].second; }
  void clear() { number_of_slots_ = 0; }
  void add(Object** slot, Object* value) {
    snapshot_[number_of_slots_].first = slot;
    snapshot_[number_of_slots_].second = value;
    ++number_of_slots_;
  }

 private:
  static const int kMaxSnapshotSize = JSObject::kMaxInstanceSize / kPointerSize;
  int number_of_slots_;
  std::pair<Object**, Object*> snapshot_[kMaxSnapshotSize];
  DISALLOW_COPY_AND_ASSIGN(SlotSnapshot);
};

class ConcurrentMarkingVisitor final
    : public HeapVisitor<int, ConcurrentMarkingVisitor> {
 public:
  using BaseClass = HeapVisitor<int, ConcurrentMarkingVisitor>;

  explicit ConcurrentMarkingVisitor(ConcurrentMarking::MarkingWorklist* shared,
                                    ConcurrentMarking::MarkingWorklist* bailout,
                                    LiveBytesMap* live_bytes,
                                    WeakObjects* weak_objects, int task_id)
      : shared_(shared, task_id),
        bailout_(bailout, task_id),
        weak_objects_(weak_objects),
        marking_state_(live_bytes),
        task_id_(task_id) {}

  bool ShouldVisit(HeapObject* object) {
    return marking_state_.GreyToBlack(object);
  }

  void VisitPointers(HeapObject* host, Object** start, Object** end) override {
    for (Object** slot = start; slot < end; slot++) {
      Object* object = base::AsAtomicPointer::Relaxed_Load(slot);
      if (!object->IsHeapObject()) continue;
      MarkObject(HeapObject::cast(object));
      MarkCompactCollector::RecordSlot(host, slot, object);
    }
  }

  void VisitPointersInSnapshot(HeapObject* host, const SlotSnapshot& snapshot) {
    for (int i = 0; i < snapshot.number_of_slots(); i++) {
      Object** slot = snapshot.slot(i);
      Object* object = snapshot.value(i);
      if (!object->IsHeapObject()) continue;
      MarkObject(HeapObject::cast(object));
      MarkCompactCollector::RecordSlot(host, slot, object);
    }
  }

  // ===========================================================================
  // JS object =================================================================
  // ===========================================================================

  int VisitJSObject(Map* map, JSObject* object) {
    int size = JSObject::BodyDescriptor::SizeOf(map, object);
    const SlotSnapshot& snapshot = MakeSlotSnapshot(map, object, size);
    if (!ShouldVisit(object)) return 0;
    VisitPointersInSnapshot(object, snapshot);
    return size;
  }

  int VisitJSObjectFast(Map* map, JSObject* object) {
    return VisitJSObject(map, object);
  }

  int VisitJSApiObject(Map* map, JSObject* object) {
    if (marking_state_.IsGrey(object)) {
      int size = JSObject::BodyDescriptor::SizeOf(map, object);
      VisitMapPointer(object, object->map_slot());
      // It is OK to iterate body of JS API object here because they do not have
      // unboxed double fields.
      DCHECK_IMPLIES(FLAG_unbox_double_fields, map->HasFastPointerLayout());
      JSObject::BodyDescriptor::IterateBody(object, size, this);
      // The main thread will do wrapper tracing in Blink.
      bailout_.Push(object);
    }
    return 0;
  }

  // ===========================================================================
  // Fixed array object ========================================================
  // ===========================================================================

  int VisitFixedArray(Map* map, FixedArray* object) {
    int length = object->synchronized_length();
    int size = FixedArray::SizeFor(length);
    if (!ShouldVisit(object)) return 0;
    VisitMapPointer(object, object->map_slot());
    FixedArray::BodyDescriptor::IterateBody(object, size, this);
    return size;
  }

  // ===========================================================================
  // Code object ===============================================================
  // ===========================================================================

  int VisitCode(Map* map, Code* object) {
    bailout_.Push(object);
    return 0;
  }

  // ===========================================================================
  // Objects with weak fields and/or side-effectiful visitation.
  // ===========================================================================

  int VisitBytecodeArray(Map* map, BytecodeArray* object) {
    if (!ShouldVisit(object)) return 0;
    int size = BytecodeArray::BodyDescriptorWeak::SizeOf(map, object);
    VisitMapPointer(object, object->map_slot());
    BytecodeArray::BodyDescriptorWeak::IterateBody(object, size, this);
    object->MakeOlder();
    return size;
  }

  int VisitAllocationSite(Map* map, AllocationSite* object) {
    if (!ShouldVisit(object)) return 0;
    int size = AllocationSite::BodyDescriptorWeak::SizeOf(map, object);
    VisitMapPointer(object, object->map_slot());
    AllocationSite::BodyDescriptorWeak::IterateBody(object, size, this);
    return size;
  }

  int VisitJSFunction(Map* map, JSFunction* object) {
    if (!ShouldVisit(object)) return 0;
    int size = JSFunction::BodyDescriptorWeak::SizeOf(map, object);
    VisitMapPointer(object, object->map_slot());
    JSFunction::BodyDescriptorWeak::IterateBody(object, size, this);
    return size;
  }

  int VisitMap(Map* meta_map, Map* map) {
    if (marking_state_.IsGrey(map)) {
      // Maps have ad-hoc weakness for descriptor arrays. They also clear the
      // code-cache. Conservatively visit strong fields skipping the
      // descriptor array field and the code cache field.
      VisitMapPointer(map, map->map_slot());
      VisitPointer(map, HeapObject::RawField(map, Map::kPrototypeOffset));
      VisitPointer(
          map, HeapObject::RawField(map, Map::kConstructorOrBackPointerOffset));
      VisitPointer(map, HeapObject::RawField(
                            map, Map::kTransitionsOrPrototypeInfoOffset));
      VisitPointer(map, HeapObject::RawField(map, Map::kDependentCodeOffset));
      VisitPointer(map, HeapObject::RawField(map, Map::kWeakCellCacheOffset));
      bailout_.Push(map);
    }
    return 0;
  }

  int VisitNativeContext(Map* map, Context* object) {
    if (marking_state_.IsGrey(object)) {
      int size = Context::BodyDescriptorWeak::SizeOf(map, object);
      VisitMapPointer(object, object->map_slot());
      Context::BodyDescriptorWeak::IterateBody(object, size, this);
      // TODO(ulan): implement proper weakness for normalized map cache
      // and remove this bailout.
      bailout_.Push(object);
    }
    return 0;
  }

  int VisitTransitionArray(Map* map, TransitionArray* array) {
    if (marking_state_.IsGrey(array)) {
      // TODO(ulan): process transition arrays.
      bailout_.Push(array);
    }
    return 0;
  }

  int VisitWeakCell(Map* map, WeakCell* object) {
    if (!ShouldVisit(object)) return 0;
    VisitMapPointer(object, object->map_slot());
    if (!object->cleared()) {
      HeapObject* value = HeapObject::cast(object->value());
      if (marking_state_.IsBlackOrGrey(value)) {
        // Weak cells with live values are directly processed here to reduce
        // the processing time of weak cells during the main GC pause.
        Object** slot = HeapObject::RawField(object, WeakCell::kValueOffset);
        MarkCompactCollector::RecordSlot(object, slot, value);
      } else {
        // If we do not know about liveness of values of weak cells, we have to
        // process them when we know the liveness of the whole transitive
        // closure.
        weak_objects_->weak_cells.Push(task_id_, object);
      }
    }
    return WeakCell::BodyDescriptor::SizeOf(map, object);
  }

  int VisitJSWeakCollection(Map* map, JSWeakCollection* object) {
    // TODO(ulan): implement iteration of strong fields.
    bailout_.Push(object);
    return 0;
  }

  void MarkObject(HeapObject* object) {
#ifdef THREAD_SANITIZER
    // Perform a dummy acquire load to tell TSAN that there is no data race
    // in mark-bit initialization. See MemoryChunk::Initialize for the
    // corresponding release store.
    MemoryChunk* chunk = MemoryChunk::FromAddress(object->address());
    CHECK_NOT_NULL(chunk->synchronized_heap());
#endif
    if (marking_state_.WhiteToGrey(object)) {
      shared_.Push(object);
    }
  }

 private:
  // Helper class for collecting in-object slot addresses and values.
  class SlotSnapshottingVisitor final : public ObjectVisitor {
   public:
    explicit SlotSnapshottingVisitor(SlotSnapshot* slot_snapshot)
        : slot_snapshot_(slot_snapshot) {
      slot_snapshot_->clear();
    }

    void VisitPointers(HeapObject* host, Object** start,
                       Object** end) override {
      for (Object** p = start; p < end; p++) {
        Object* object = reinterpret_cast<Object*>(
            base::Relaxed_Load(reinterpret_cast<const base::AtomicWord*>(p)));
        slot_snapshot_->add(p, object);
      }
    }

   private:
    SlotSnapshot* slot_snapshot_;
  };

  const SlotSnapshot& MakeSlotSnapshot(Map* map, HeapObject* object, int size) {
    // TODO(ulan): Iterate only the existing fields and skip slack at the end
    // of the object.
    SlotSnapshottingVisitor visitor(&slot_snapshot_);
    visitor.VisitPointer(object,
                         reinterpret_cast<Object**>(object->map_slot()));
    JSObject::BodyDescriptor::IterateBody(object, size, &visitor);
    return slot_snapshot_;
  }
  ConcurrentMarking::MarkingWorklist::View shared_;
  ConcurrentMarking::MarkingWorklist::View bailout_;
  WeakObjects* weak_objects_;
  ConcurrentMarkingState marking_state_;
  int task_id_;
  SlotSnapshot slot_snapshot_;
};

class ConcurrentMarking::Task : public CancelableTask {
 public:
  Task(Isolate* isolate, ConcurrentMarking* concurrent_marking,
       TaskState* task_state, int task_id)
      : CancelableTask(isolate),
        concurrent_marking_(concurrent_marking),
        task_state_(task_state),
        task_id_(task_id) {}

  virtual ~Task() {}

 private:
  // v8::internal::CancelableTask overrides.
  void RunInternal() override {
    concurrent_marking_->Run(task_id_, task_state_);
  }

  ConcurrentMarking* concurrent_marking_;
  TaskState* task_state_;
  int task_id_;
  DISALLOW_COPY_AND_ASSIGN(Task);
};

ConcurrentMarking::ConcurrentMarking(Heap* heap, MarkingWorklist* shared,
                                     MarkingWorklist* bailout,
                                     WeakObjects* weak_objects)
    : heap_(heap),
      shared_(shared),
      bailout_(bailout),
      weak_objects_(weak_objects),
      pending_task_count_(0) {
// The runtime flag should be set only if the compile time flag was set.
#ifndef V8_CONCURRENT_MARKING
  CHECK(!FLAG_concurrent_marking);
#endif
  for (int i = 0; i <= kTasks; i++) {
    is_pending_[i] = false;
  }
}

void ConcurrentMarking::Run(int task_id, TaskState* task_state) {
  size_t kBytesUntilInterruptCheck = 64 * KB;
  int kObjectsUntilInterrupCheck = 1000;
  LiveBytesMap* live_bytes = nullptr;
  {
    base::LockGuard<base::Mutex> guard(&task_state->lock);
    live_bytes = &task_state->live_bytes;
  }
  ConcurrentMarkingVisitor visitor(shared_, bailout_, live_bytes, weak_objects_,
                                   task_id);
  double time_ms;
  size_t total_bytes_marked = 0;
  if (FLAG_trace_concurrent_marking) {
    heap_->isolate()->PrintWithTimestamp(
        "Starting concurrent marking task %d\n", task_id);
  }
  {
    TimedScope scope(&time_ms);
    bool done = false;
    while (!done) {
      base::LockGuard<base::Mutex> guard(&task_state->lock);
      size_t bytes_marked = 0;
      int objects_processed = 0;
      while (bytes_marked < kBytesUntilInterruptCheck &&
             objects_processed < kObjectsUntilInterrupCheck) {
        HeapObject* object;
        if (!shared_->Pop(task_id, &object)) {
          done = true;
          break;
        }
        objects_processed++;
        Address new_space_top = heap_->new_space()->original_top();
        Address new_space_limit = heap_->new_space()->original_limit();
        Address addr = object->address();
        if (new_space_top <= addr && addr < new_space_limit) {
          bailout_->Push(task_id, object);
        } else {
          Map* map = object->synchronized_map();
          bytes_marked += visitor.Visit(map, object);
        }
      }
      total_bytes_marked += bytes_marked;
      if (task_state->interrupt_request.Value()) {
        task_state->interrupt_condition.Wait(&task_state->lock);
      }
    }
    {
      // Take the lock to synchronize with worklist update after
      // young generation GC.
      base::LockGuard<base::Mutex> guard(&task_state->lock);
      bailout_->FlushToGlobal(task_id);
    }
    weak_objects_->weak_cells.FlushToGlobal(task_id);
    weak_objects_->transition_arrays.FlushToGlobal(task_id);
    {
      base::LockGuard<base::Mutex> guard(&pending_lock_);
      is_pending_[task_id] = false;
      --pending_task_count_;
      pending_condition_.NotifyAll();
    }
  }
  if (FLAG_trace_concurrent_marking) {
    heap_->isolate()->PrintWithTimestamp(
        "Task %d concurrently marked %dKB in %.2fms\n", task_id,
        static_cast<int>(total_bytes_marked / KB), time_ms);
  }
}

void ConcurrentMarking::ScheduleTasks() {
  if (!FLAG_concurrent_marking) return;
  base::LockGuard<base::Mutex> guard(&pending_lock_);
  if (pending_task_count_ < kTasks) {
    // Task id 0 is for the main thread.
    for (int i = 1; i <= kTasks; i++) {
      if (!is_pending_[i]) {
        if (FLAG_trace_concurrent_marking) {
          heap_->isolate()->PrintWithTimestamp(
              "Scheduling concurrent marking task %d\n", i);
        }
        task_state_[i].interrupt_request.SetValue(false);
        is_pending_[i] = true;
        ++pending_task_count_;
        V8::GetCurrentPlatform()->CallOnBackgroundThread(
            new Task(heap_->isolate(), this, &task_state_[i], i),
            v8::Platform::kShortRunningTask);
      }
    }
  }
}

void ConcurrentMarking::RescheduleTasksIfNeeded() {
  if (!FLAG_concurrent_marking) return;
  {
    base::LockGuard<base::Mutex> guard(&pending_lock_);
    if (pending_task_count_ > 0) return;
  }
  if (!shared_->IsGlobalPoolEmpty()) {
    ScheduleTasks();
  }
}

void ConcurrentMarking::EnsureCompleted() {
  if (!FLAG_concurrent_marking) return;
  base::LockGuard<base::Mutex> guard(&pending_lock_);
  while (pending_task_count_ > 0) {
    pending_condition_.Wait(&pending_lock_);
  }
}

void ConcurrentMarking::FlushLiveBytes(
    MajorNonAtomicMarkingState* marking_state) {
  DCHECK_EQ(pending_task_count_, 0);
  for (int i = 1; i <= kTasks; i++) {
    LiveBytesMap& live_bytes = task_state_[i].live_bytes;
    for (auto pair : live_bytes) {
      // ClearLiveness sets the live bytes to zero.
      // Pages with zero live bytes might be already unmapped.
      if (pair.second != 0) {
        marking_state->IncrementLiveBytes(pair.first, pair.second);
      }
    }
    live_bytes.clear();
  }
}

void ConcurrentMarking::ClearLiveness(MemoryChunk* chunk) {
  for (int i = 1; i <= kTasks; i++) {
    if (task_state_[i].live_bytes.count(chunk)) {
      task_state_[i].live_bytes[chunk] = 0;
    }
  }
}

ConcurrentMarking::PauseScope::PauseScope(ConcurrentMarking* concurrent_marking)
    : concurrent_marking_(concurrent_marking) {
  if (!FLAG_concurrent_marking) return;
  // Request task_state for all tasks.
  for (int i = 1; i <= kTasks; i++) {
    concurrent_marking_->task_state_[i].interrupt_request.SetValue(true);
  }
  // Now take a lock to ensure that the tasks are waiting.
  for (int i = 1; i <= kTasks; i++) {
    concurrent_marking_->task_state_[i].lock.Lock();
  }
}

ConcurrentMarking::PauseScope::~PauseScope() {
  if (!FLAG_concurrent_marking) return;
  for (int i = kTasks; i >= 1; i--) {
    concurrent_marking_->task_state_[i].interrupt_request.SetValue(false);
    concurrent_marking_->task_state_[i].interrupt_condition.NotifyAll();
    concurrent_marking_->task_state_[i].lock.Unlock();
  }
}

}  // namespace internal
}  // namespace v8
