// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Platform-specific code for FreeBSD goes here. For the POSIX-compatible
// parts, the implementation is in platform-posix.cc.

#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ucontext.h>

#include <sys/fcntl.h>  // open
#include <sys/mman.h>   // mmap & munmap
#include <sys/stat.h>   // open
#include <unistd.h>     // getpagesize
// If you don't have execinfo.h then you need devel/libexecinfo from ports.
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <strings.h>    // index

#include <cmath>

#undef MAP_TYPE

#include "src/base/macros.h"
#include "src/base/platform/platform-posix-time.h"
#include "src/base/platform/platform-posix.h"
#include "src/base/platform/platform.h"

namespace v8 {
namespace base {

TimezoneCache* OS::CreateTimezoneCache() {
  return new PosixDefaultTimezoneCache();
}

void* OS::Allocate(const size_t requested, size_t* allocated,
                   OS::MemoryPermission access, void* hint) {
  const size_t msize = RoundUp(requested, getpagesize());
  int prot = GetProtectionFromMemoryPermission(access);
  void* mbase = mmap(hint, msize, prot, MAP_PRIVATE | MAP_ANON, -1, 0);

  if (mbase == MAP_FAILED) return NULL;
  *allocated = msize;
  return mbase;
}


static unsigned StringToLong(char* buffer) {
  return static_cast<unsigned>(strtol(buffer, NULL, 16));  // NOLINT
}


std::vector<OS::SharedLibraryAddress> OS::GetSharedLibraryAddresses() {
  std::vector<SharedLibraryAddress> result;
  static const int MAP_LENGTH = 1024;
  int fd = open("/proc/self/maps", O_RDONLY);
  if (fd < 0) return result;
  while (true) {
    char addr_buffer[11];
    addr_buffer[0] = '0';
    addr_buffer[1] = 'x';
    addr_buffer[10] = 0;
    ssize_t bytes_read = read(fd, addr_buffer + 2, 8);
    if (bytes_read < 8) break;
    unsigned start = StringToLong(addr_buffer);
    bytes_read = read(fd, addr_buffer + 2, 1);
    if (bytes_read < 1) break;
    if (addr_buffer[2] != '-') break;
    bytes_read = read(fd, addr_buffer + 2, 8);
    if (bytes_read < 8) break;
    unsigned end = StringToLong(addr_buffer);
    char buffer[MAP_LENGTH];
    bytes_read = -1;
    do {
      bytes_read++;
      if (bytes_read >= MAP_LENGTH - 1)
        break;
      bytes_read = read(fd, buffer + bytes_read, 1);
      if (bytes_read < 1) break;
    } while (buffer[bytes_read] != '\n');
    buffer[bytes_read] = 0;
    // Ignore mappings that are not executable.
    if (buffer[3] != 'x') continue;
    char* start_of_path = index(buffer, '/');
    // There may be no filename in this line.  Skip to next.
    if (start_of_path == NULL) continue;
    buffer[bytes_read] = 0;
    result.push_back(SharedLibraryAddress(start_of_path, start, end));
  }
  close(fd);
  return result;
}


void OS::SignalCodeMovingGC() {
}



// Constants used for mmap.
static const int kMmapFd = -1;
static const int kMmapFdOffset = 0;


VirtualMemory::VirtualMemory() : address_(NULL), size_(0) { }

VirtualMemory::VirtualMemory(size_t size, void* hint)
    : address_(ReserveRegion(size, hint)), size_(size) {}

VirtualMemory::VirtualMemory(size_t size, size_t alignment, void* hint)
    : address_(NULL), size_(0) {
  hint = AlignedAddress(hint, alignment);
  DCHECK((alignment % OS::AllocateAlignment()) == 0);
  size_t request_size = RoundUp(size + alignment,
                                static_cast<intptr_t>(OS::AllocateAlignment()));
  void* reservation = mmap(hint, request_size, PROT_NONE,
                           MAP_PRIVATE | MAP_ANON, kMmapFd, kMmapFdOffset);
  if (reservation == MAP_FAILED) return;

  uint8_t* base = static_cast<uint8_t*>(reservation);
  uint8_t* aligned_base = RoundUp(base, alignment);
  DCHECK_LE(base, aligned_base);

  // Unmap extra memory reserved before and after the desired block.
  if (aligned_base != base) {
    size_t prefix_size = static_cast<size_t>(aligned_base - base);
    OS::Free(base, prefix_size);
    request_size -= prefix_size;
  }

  size_t aligned_size = RoundUp(size, OS::AllocateAlignment());
  DCHECK_LE(aligned_size, request_size);

  if (aligned_size != request_size) {
    size_t suffix_size = request_size - aligned_size;
    OS::Free(aligned_base + aligned_size, suffix_size);
    request_size -= suffix_size;
  }

  DCHECK(aligned_size == request_size);

  address_ = static_cast<void*>(aligned_base);
  size_ = aligned_size;
}


VirtualMemory::~VirtualMemory() {
  if (IsReserved()) {
    bool result = ReleaseRegion(address(), size());
    DCHECK(result);
    USE(result);
  }
}

void VirtualMemory::Reset() {
  address_ = NULL;
  size_ = 0;
}


bool VirtualMemory::Commit(void* address, size_t size, bool is_executable) {
  return CommitRegion(address, size, is_executable);
}


bool VirtualMemory::Uncommit(void* address, size_t size) {
  return UncommitRegion(address, size);
}


bool VirtualMemory::Guard(void* address) {
  OS::Guard(address, OS::CommitPageSize());
  return true;
}

void* VirtualMemory::ReserveRegion(size_t size, void* hint) {
  void* result = mmap(hint, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, kMmapFd,
                      kMmapFdOffset);

  if (result == MAP_FAILED) return NULL;

  return result;
}


bool VirtualMemory::CommitRegion(void* base, size_t size, bool is_executable) {
  int prot = PROT_READ | PROT_WRITE | (is_executable ? PROT_EXEC : 0);
  if (MAP_FAILED == mmap(base,
                         size,
                         prot,
                         MAP_PRIVATE | MAP_ANON | MAP_FIXED,
                         kMmapFd,
                         kMmapFdOffset)) {
    return false;
  }
  return true;
}


bool VirtualMemory::UncommitRegion(void* base, size_t size) {
  return mmap(base,
              size,
              PROT_NONE,
              MAP_PRIVATE | MAP_ANON | MAP_FIXED,
              kMmapFd,
              kMmapFdOffset) != MAP_FAILED;
}

bool VirtualMemory::ReleasePartialRegion(void* base, size_t size,
                                         void* free_start, size_t free_size) {
  return munmap(free_start, free_size) == 0;
}

bool VirtualMemory::ReleaseRegion(void* base, size_t size) {
  return munmap(base, size) == 0;
}


bool VirtualMemory::HasLazyCommits() {
  // TODO(alph): implement for the platform.
  return false;
}

}  // namespace base
}  // namespace v8
