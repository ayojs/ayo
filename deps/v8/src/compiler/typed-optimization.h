// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_TYPED_OPTIMIZATION_H_
#define V8_COMPILER_TYPED_OPTIMIZATION_H_

#include "src/base/compiler-specific.h"
#include "src/compiler/graph-reducer.h"
#include "src/globals.h"

namespace v8 {
namespace internal {

// Forward declarations.
class CompilationDependencies;
class Factory;
class Isolate;

namespace compiler {

// Forward declarations.
class JSGraph;
class SimplifiedOperatorBuilder;
class TypeCache;

class V8_EXPORT_PRIVATE TypedOptimization final
    : public NON_EXPORTED_BASE(AdvancedReducer) {
 public:
  TypedOptimization(Editor* editor, CompilationDependencies* dependencies,
                    JSGraph* jsgraph);
  ~TypedOptimization();

  const char* reducer_name() const override { return "TypedOptimization"; }

  Reduction Reduce(Node* node) final;

 private:
  Reduction ReduceCheckHeapObject(Node* node);
  Reduction ReduceCheckMaps(Node* node);
  Reduction ReduceCheckNumber(Node* node);
  Reduction ReduceCheckString(Node* node);
  Reduction ReduceCheckSeqString(Node* node);
  Reduction ReduceLoadField(Node* node);
  Reduction ReduceNumberFloor(Node* node);
  Reduction ReduceNumberRoundop(Node* node);
  Reduction ReduceNumberToUint8Clamped(Node* node);
  Reduction ReducePhi(Node* node);
  Reduction ReduceReferenceEqual(Node* node);
  Reduction ReduceSelect(Node* node);
  Reduction ReduceSpeculativeToNumber(Node* node);
  Reduction ReduceCheckNotTaggedHole(Node* node);

  CompilationDependencies* dependencies() const { return dependencies_; }
  Factory* factory() const;
  Graph* graph() const;
  Isolate* isolate() const;
  JSGraph* jsgraph() const { return jsgraph_; }
  SimplifiedOperatorBuilder* simplified() const;

  CompilationDependencies* const dependencies_;
  JSGraph* const jsgraph_;
  Type* const true_type_;
  Type* const false_type_;
  TypeCache const& type_cache_;

  DISALLOW_COPY_AND_ASSIGN(TypedOptimization);
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_TYPED_OPTIMIZATION_H_
