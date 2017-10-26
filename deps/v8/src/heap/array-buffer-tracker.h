// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_HEAP_ARRAY_BUFFER_TRACKER_H_
#define V8_HEAP_ARRAY_BUFFER_TRACKER_H_

#include <unordered_set>

#include "src/allocation.h"
#include "src/base/platform/mutex.h"
#include "src/globals.h"

namespace v8 {
namespace internal {

class Heap;
class JSArrayBuffer;
class MarkingState;
class Page;

class ArrayBufferTracker : public AllStatic {
 public:
  enum ProcessingMode {
    kUpdateForwardedRemoveOthers,
    kUpdateForwardedKeepOthers,
  };

  // The following methods are used to track raw C++ pointers to externally
  // allocated memory used as backing store in live array buffers.

  // Register/unregister a new JSArrayBuffer |buffer| for tracking. Guards all
  // access to the tracker by taking the page lock for the corresponding page.
  inline static void RegisterNew(Heap* heap, JSArrayBuffer* buffer);
  inline static void Unregister(Heap* heap, JSArrayBuffer* buffer);

  // Frees all backing store pointers for dead JSArrayBuffers in new space.
  // Does not take any locks and can only be called during Scavenge.
  static void FreeDeadInNewSpace(Heap* heap);

  // Number of array buffer bytes retained from new space.
  static size_t RetainedInNewSpace(Heap* heap);

  // Frees all backing store pointers for dead JSArrayBuffer on a given page.
  // Requires marking information to be present. Requires the page lock to be
  // taken by the caller.
  template <typename MarkingState>
  static void FreeDead(Page* page, MarkingState* marking_state);

  // Frees all remaining, live or dead, array buffers on a page. Only useful
  // during tear down.
  static void FreeAll(Page* page);

  // Processes all array buffers on a given page. |mode| specifies the action
  // to perform on the buffers. Returns whether the tracker is empty or not.
  static bool ProcessBuffers(Page* page, ProcessingMode mode);

  // Returns whether a buffer is currently tracked.
  static bool IsTracked(JSArrayBuffer* buffer);
};

// LocalArrayBufferTracker tracks internalized array buffers.
//
// Never use directly but instead always call through |ArrayBufferTracker|.
class LocalArrayBufferTracker {
 public:
  enum CallbackResult { kKeepEntry, kUpdateEntry, kRemoveEntry };
  enum FreeMode { kFreeDead, kFreeAll };

  explicit LocalArrayBufferTracker(Heap* heap)
      : heap_(heap), retained_size_(0) {}
  ~LocalArrayBufferTracker();

  inline void Add(JSArrayBuffer* buffer, size_t length);
  inline void Remove(JSArrayBuffer* buffer, size_t length);

  // Frees up array buffers.
  //
  // Sample usage:
  // Free([](HeapObject* array_buffer) {
  //    if (should_free_internal(array_buffer)) return true;
  //    return false;
  // });
  template <typename Callback>
  void Free(Callback should_free);

  // Processes buffers one by one. The CallbackResult of the callback decides
  // what action to take on the buffer.
  //
  // Callback should be of type:
  //   CallbackResult fn(JSArrayBuffer* buffer, JSArrayBuffer** new_buffer);
  template <typename Callback>
  void Process(Callback callback);

  bool IsEmpty() const { return array_buffers_.empty(); }

  bool IsTracked(JSArrayBuffer* buffer) const {
    return array_buffers_.find(buffer) != array_buffers_.end();
  }

  size_t retained_size() const { return retained_size_; }

 private:
  typedef std::unordered_set<JSArrayBuffer*> TrackingData;

  Heap* heap_;
  // The set contains raw heap pointers which are removed by the GC upon
  // processing the tracker through its owning page.
  TrackingData array_buffers_;
  // Retained size of array buffers for this tracker in bytes.
  size_t retained_size_;
};

}  // namespace internal
}  // namespace v8
#endif  // V8_HEAP_ARRAY_BUFFER_TRACKER_H_
