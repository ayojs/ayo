// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/heap/array-buffer-tracker.h"
#include "src/heap/array-buffer-tracker-inl.h"
#include "src/heap/heap.h"
#include "src/heap/spaces.h"

namespace v8 {
namespace internal {

LocalArrayBufferTracker::~LocalArrayBufferTracker() {
  CHECK(array_buffers_.empty());
}

template <typename Callback>
void LocalArrayBufferTracker::Process(Callback callback) {
  JSArrayBuffer* new_buffer = nullptr;
  JSArrayBuffer* old_buffer = nullptr;
  size_t freed_memory = 0;
  size_t retained_size = 0;
  for (TrackingData::iterator it = array_buffers_.begin();
       it != array_buffers_.end();) {
    old_buffer = reinterpret_cast<JSArrayBuffer*>(*it);
    const size_t length = old_buffer->allocation_length();
    const CallbackResult result = callback(old_buffer, &new_buffer);
    if (result == kKeepEntry) {
      retained_size += length;
      ++it;
    } else if (result == kUpdateEntry) {
      DCHECK_NOT_NULL(new_buffer);
      Page* target_page = Page::FromAddress(new_buffer->address());
      {
        base::LockGuard<base::RecursiveMutex> guard(target_page->mutex());
        LocalArrayBufferTracker* tracker = target_page->local_tracker();
        if (tracker == nullptr) {
          target_page->AllocateLocalTracker();
          tracker = target_page->local_tracker();
        }
        DCHECK_NOT_NULL(tracker);
        DCHECK_EQ(length, new_buffer->allocation_length());
        tracker->Add(new_buffer, length);
      }
      it = array_buffers_.erase(it);
    } else if (result == kRemoveEntry) {
      freed_memory += length;
      old_buffer->FreeBackingStore();
      it = array_buffers_.erase(it);
    } else {
      UNREACHABLE();
    }
  }
  retained_size_ = retained_size;
  if (freed_memory > 0) {
    heap_->update_external_memory_concurrently_freed(
        static_cast<intptr_t>(freed_memory));
  }
}

void ArrayBufferTracker::FreeDeadInNewSpace(Heap* heap) {
  DCHECK_EQ(heap->gc_state(), Heap::HeapState::SCAVENGE);
  for (Page* page : PageRange(heap->new_space()->FromSpaceStart(),
                              heap->new_space()->FromSpaceEnd())) {
    bool empty = ProcessBuffers(page, kUpdateForwardedRemoveOthers);
    CHECK(empty);
  }
  heap->account_external_memory_concurrently_freed();
}

size_t ArrayBufferTracker::RetainedInNewSpace(Heap* heap) {
  size_t retained_size = 0;
  for (Page* page : PageRange(heap->new_space()->ToSpaceStart(),
                              heap->new_space()->ToSpaceEnd())) {
    LocalArrayBufferTracker* tracker = page->local_tracker();
    if (tracker == nullptr) continue;
    retained_size += tracker->retained_size();
  }
  return retained_size;
}

void ArrayBufferTracker::FreeAll(Page* page) {
  LocalArrayBufferTracker* tracker = page->local_tracker();
  if (tracker == nullptr) return;
  tracker->Free([](JSArrayBuffer* buffer) { return true; });
  if (tracker->IsEmpty()) {
    page->ReleaseLocalTracker();
  }
}

bool ArrayBufferTracker::ProcessBuffers(Page* page, ProcessingMode mode) {
  LocalArrayBufferTracker* tracker = page->local_tracker();
  if (tracker == nullptr) return true;

  DCHECK(page->SweepingDone());
  tracker->Process(
      [mode](JSArrayBuffer* old_buffer, JSArrayBuffer** new_buffer) {
        MapWord map_word = old_buffer->map_word();
        if (map_word.IsForwardingAddress()) {
          *new_buffer = JSArrayBuffer::cast(map_word.ToForwardingAddress());
          return LocalArrayBufferTracker::kUpdateEntry;
        }
        return mode == kUpdateForwardedKeepOthers
                   ? LocalArrayBufferTracker::kKeepEntry
                   : LocalArrayBufferTracker::kRemoveEntry;
      });
  return tracker->IsEmpty();
}

bool ArrayBufferTracker::IsTracked(JSArrayBuffer* buffer) {
  Page* page = Page::FromAddress(buffer->address());
  {
    base::LockGuard<base::RecursiveMutex> guard(page->mutex());
    LocalArrayBufferTracker* tracker = page->local_tracker();
    if (tracker == nullptr) return false;
    return tracker->IsTracked(buffer);
  }
}

}  // namespace internal
}  // namespace v8
