// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_HEAP_ARRAY_BUFFER_TRACKER_INL_H_
#define V8_HEAP_ARRAY_BUFFER_TRACKER_INL_H_

#include "src/conversions-inl.h"
#include "src/heap/array-buffer-tracker.h"
#include "src/heap/heap.h"
#include "src/objects.h"

namespace v8 {
namespace internal {

void ArrayBufferTracker::RegisterNew(Heap* heap, JSArrayBuffer* buffer) {
  void* data = buffer->backing_store();
  if (!data) return;

  size_t length = buffer->allocation_length();
  Page* page = Page::FromAddress(buffer->address());
  {
    base::LockGuard<base::RecursiveMutex> guard(page->mutex());
    LocalArrayBufferTracker* tracker = page->local_tracker();
    if (tracker == nullptr) {
      page->AllocateLocalTracker();
      tracker = page->local_tracker();
    }
    DCHECK_NOT_NULL(tracker);
    tracker->Add(buffer, length);
  }
  // We may go over the limit of externally allocated memory here. We call the
  // api function to trigger a GC in this case.
  reinterpret_cast<v8::Isolate*>(heap->isolate())
      ->AdjustAmountOfExternalAllocatedMemory(length);
}

void ArrayBufferTracker::Unregister(Heap* heap, JSArrayBuffer* buffer) {
  void* data = buffer->backing_store();
  if (!data) return;

  Page* page = Page::FromAddress(buffer->address());
  size_t length = buffer->allocation_length();
  {
    base::LockGuard<base::RecursiveMutex> guard(page->mutex());
    LocalArrayBufferTracker* tracker = page->local_tracker();
    DCHECK_NOT_NULL(tracker);
    tracker->Remove(buffer, length);
  }
  heap->update_external_memory(-static_cast<intptr_t>(length));
}

template <typename Callback>
void LocalArrayBufferTracker::Free(Callback should_free) {
  size_t freed_memory = 0;
  size_t retained_size = 0;
  for (TrackingData::iterator it = array_buffers_.begin();
       it != array_buffers_.end();) {
    JSArrayBuffer* buffer = reinterpret_cast<JSArrayBuffer*>(*it);
    const size_t length = buffer->allocation_length();
    if (should_free(buffer)) {
      freed_memory += length;
      buffer->FreeBackingStore();
      it = array_buffers_.erase(it);
    } else {
      retained_size += length;
      ++it;
    }
  }
  retained_size_ = retained_size;
  if (freed_memory > 0) {
    heap_->update_external_memory_concurrently_freed(
        static_cast<intptr_t>(freed_memory));
  }
}

template <typename MarkingState>
void ArrayBufferTracker::FreeDead(Page* page, MarkingState* marking_state) {
  // Callers need to ensure having the page lock.
  LocalArrayBufferTracker* tracker = page->local_tracker();
  if (tracker == nullptr) return;
  tracker->Free([marking_state](JSArrayBuffer* buffer) {
    return marking_state->IsWhite(buffer);
  });
  if (tracker->IsEmpty()) {
    page->ReleaseLocalTracker();
  }
}

void LocalArrayBufferTracker::Add(JSArrayBuffer* buffer, size_t length) {
  DCHECK_GE(retained_size_ + length, retained_size_);
  retained_size_ += length;
  auto ret = array_buffers_.insert(buffer);
  USE(ret);
  // Check that we indeed inserted a new value and did not overwrite an existing
  // one (which would be a bug).
  DCHECK(ret.second);
}

void LocalArrayBufferTracker::Remove(JSArrayBuffer* buffer, size_t length) {
  DCHECK_GE(retained_size_, retained_size_ - length);
  retained_size_ -= length;
  TrackingData::iterator it = array_buffers_.find(buffer);
  // Check that we indeed find a key to remove.
  DCHECK(it != array_buffers_.end());
  array_buffers_.erase(it);
}

}  // namespace internal
}  // namespace v8

#endif  // V8_HEAP_ARRAY_BUFFER_TRACKER_INL_H_
