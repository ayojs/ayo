// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_WASM_SIGNATURE_MAP_H_
#define V8_WASM_SIGNATURE_MAP_H_

#include <map>

#include "src/signature.h"
#include "src/wasm/wasm-opcodes.h"

namespace v8 {
namespace internal {
namespace wasm {

// A signature map canonicalizes signatures into a range of indices so that
// two different {FunctionSig} instances with the same contents map to the
// same index.
class V8_EXPORT_PRIVATE SignatureMap {
 public:
  // Allow default construction and move construction (because we have vectors
  // of objects containing SignatureMaps), but disallow copy or assign. It's
  // too easy to get security bugs by accidentally updating a copy of the map.
  SignatureMap();
  SignatureMap(SignatureMap&&) = default;

  // Gets the index for a signature, assigning a new index if necessary.
  uint32_t FindOrInsert(FunctionSig* sig);

  // Gets the index for a signature, returning {-1} if not found.
  int32_t Find(FunctionSig* sig) const;

 private:
  // TODO(wasm): use a hashmap instead of an ordered map?
  struct CompareFunctionSigs {
    bool operator()(FunctionSig* a, FunctionSig* b) const;
  };
  uint32_t next_ = 0;
  // TODO(wasm): performance-critical, replace with a reader-writer lock
  std::unique_ptr<base::Mutex> mutex_;
  std::map<FunctionSig*, uint32_t, CompareFunctionSigs> map_;

  DISALLOW_COPY_AND_ASSIGN(SignatureMap);
};

}  // namespace wasm
}  // namespace internal
}  // namespace v8

#endif  // V8_WASM_SIGNATURE_MAP_H_
