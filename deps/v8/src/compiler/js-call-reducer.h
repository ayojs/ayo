// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_JS_CALL_REDUCER_H_
#define V8_COMPILER_JS_CALL_REDUCER_H_

#include "src/base/flags.h"
#include "src/compiler/graph-reducer.h"
#include "src/deoptimize-reason.h"

namespace v8 {
namespace internal {

// Forward declarations.
class CompilationDependencies;
class Factory;

namespace compiler {

// Forward declarations.
class CallFrequency;
class CommonOperatorBuilder;
class JSGraph;
class JSOperatorBuilder;
class SimplifiedOperatorBuilder;
class VectorSlotPair;

// Performs strength reduction on {JSConstruct} and {JSCall} nodes,
// which might allow inlining or other optimizations to be performed afterwards.
class JSCallReducer final : public AdvancedReducer {
 public:
  // Flags that control the mode of operation.
  enum Flag { kNoFlags = 0u, kBailoutOnUninitialized = 1u << 0 };
  typedef base::Flags<Flag> Flags;

  JSCallReducer(Editor* editor, JSGraph* jsgraph, Flags flags,
                Handle<Context> native_context,
                CompilationDependencies* dependencies)
      : AdvancedReducer(editor),
        jsgraph_(jsgraph),
        flags_(flags),
        native_context_(native_context),
        dependencies_(dependencies) {}

  const char* reducer_name() const override { return "JSCallReducer"; }

  Reduction Reduce(Node* node) final;

  // Processes the waitlist gathered while the reducer was running,
  // and does a final attempt to reduce the nodes in the waitlist.
  void Finalize() final;

 private:
  Reduction ReduceArrayConstructor(Node* node);
  Reduction ReduceBooleanConstructor(Node* node);
  Reduction ReduceCallApiFunction(
      Node* node, Handle<FunctionTemplateInfo> function_template_info);
  Reduction ReduceNumberConstructor(Node* node);
  Reduction ReduceFunctionPrototypeApply(Node* node);
  Reduction ReduceFunctionPrototypeCall(Node* node);
  Reduction ReduceFunctionPrototypeHasInstance(Node* node);
  Reduction ReduceObjectGetPrototype(Node* node, Node* object);
  Reduction ReduceObjectGetPrototypeOf(Node* node);
  Reduction ReduceObjectPrototypeGetProto(Node* node);
  Reduction ReduceObjectPrototypeHasOwnProperty(Node* node);
  Reduction ReduceObjectPrototypeIsPrototypeOf(Node* node);
  Reduction ReduceReflectApply(Node* node);
  Reduction ReduceReflectConstruct(Node* node);
  Reduction ReduceReflectGetPrototypeOf(Node* node);
  Reduction ReduceArrayForEach(Handle<JSFunction> function, Node* node);
  Reduction ReduceArrayMap(Handle<JSFunction> function, Node* node);
  Reduction ReduceCallOrConstructWithArrayLikeOrSpread(
      Node* node, int arity, CallFrequency const& frequency,
      VectorSlotPair const& feedback);
  Reduction ReduceJSConstruct(Node* node);
  Reduction ReduceJSConstructWithArrayLike(Node* node);
  Reduction ReduceJSConstructWithSpread(Node* node);
  Reduction ReduceJSCall(Node* node);
  Reduction ReduceJSCallWithArrayLike(Node* node);
  Reduction ReduceJSCallWithSpread(Node* node);
  Reduction ReduceReturnReceiver(Node* node);

  Reduction ReduceSoftDeoptimize(Node* node, DeoptimizeReason reason);

  Graph* graph() const;
  JSGraph* jsgraph() const { return jsgraph_; }
  Isolate* isolate() const;
  Factory* factory() const;
  Handle<Context> native_context() const { return native_context_; }
  Handle<JSGlobalProxy> global_proxy() const;
  CommonOperatorBuilder* common() const;
  JSOperatorBuilder* javascript() const;
  SimplifiedOperatorBuilder* simplified() const;
  Flags flags() const { return flags_; }
  CompilationDependencies* dependencies() const { return dependencies_; }

  JSGraph* const jsgraph_;
  Flags const flags_;
  Handle<Context> const native_context_;
  CompilationDependencies* const dependencies_;
  std::set<Node*> waitlist_;
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_JS_CALL_REDUCER_H_
