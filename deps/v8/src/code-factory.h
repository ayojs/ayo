// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_CODE_FACTORY_H_
#define V8_CODE_FACTORY_H_

#include "src/allocation.h"
#include "src/assembler.h"
#include "src/callable.h"
#include "src/codegen.h"
#include "src/globals.h"
#include "src/interface-descriptors.h"
#include "src/parsing/token.h"

namespace v8 {
namespace internal {

class V8_EXPORT_PRIVATE CodeFactory final {
 public:
  // CEntryStub has var-args semantics (all the arguments are passed on the
  // stack and the arguments count is passed via register) which currently
  // can't be expressed in CallInterfaceDescriptor. Therefore only the code
  // is exported here.
  static Handle<Code> RuntimeCEntry(Isolate* isolate, int result_size = 1);

  // Initial states for ICs.
  static Callable LoadICProtoArray(Isolate* isolate, bool throw_if_nonexistent);
  static Callable LoadGlobalIC(Isolate* isolate, TypeofMode typeof_mode);
  static Callable LoadGlobalICInOptimizedCode(Isolate* isolate,
                                              TypeofMode typeof_mode);
  static Callable StoreGlobalIC(Isolate* isolate, LanguageMode mode);
  static Callable StoreGlobalICInOptimizedCode(Isolate* isolate,
                                               LanguageMode mode);
  static Callable StoreIC(Isolate* isolate, LanguageMode mode);
  static Callable StoreICInOptimizedCode(Isolate* isolate, LanguageMode mode);
  static Callable StoreIC_Uninitialized(Isolate* isolate, LanguageMode mode);
  static Callable StoreOwnIC(Isolate* isolate);
  static Callable StoreOwnICInOptimizedCode(Isolate* isolate);
  static Callable KeyedStoreIC(Isolate* isolate, LanguageMode mode);
  static Callable KeyedStoreICInOptimizedCode(Isolate* isolate,
                                              LanguageMode mode);
  static Callable KeyedStoreIC_Megamorphic(Isolate* isolate, LanguageMode mode);

  static Callable ResumeGenerator(Isolate* isolate);

  static Callable FrameDropperTrampoline(Isolate* isolate);
  static Callable HandleDebuggerStatement(Isolate* isolate);

  static Callable BinaryOperation(Isolate* isolate, Token::Value op);

  static Callable ApiGetter(Isolate* isolate);

  // Code stubs. Add methods here as needed to reduce dependency on
  // code-stubs.h.
  static Callable GetProperty(Isolate* isolate);

  static Callable NonPrimitiveToPrimitive(
      Isolate* isolate, ToPrimitiveHint hint = ToPrimitiveHint::kDefault);
  static Callable OrdinaryToPrimitive(Isolate* isolate,
                                      OrdinaryToPrimitiveHint hint);
  static Callable NumberToString(Isolate* isolate);

  static Callable StringAdd(Isolate* isolate,
                            StringAddFlags flags = STRING_ADD_CHECK_NONE,
                            PretenureFlag pretenure_flag = NOT_TENURED);
  static Callable StringCompare(Isolate* isolate, Token::Value token);
  static Callable SubString(Isolate* isolate);

  static Callable FastCloneShallowArray(Isolate* isolate,
                                        AllocationSiteMode allocation_mode);

  static Callable FastNewFunctionContext(Isolate* isolate,
                                         ScopeType scope_type);

  static Callable AllocateHeapNumber(Isolate* isolate);

  static Callable ArgumentAdaptor(Isolate* isolate);
  static Callable Call(Isolate* isolate,
                       ConvertReceiverMode mode = ConvertReceiverMode::kAny);
  static Callable CallWithArrayLike(Isolate* isolate);
  static Callable CallWithSpread(Isolate* isolate);
  static Callable CallFunction(
      Isolate* isolate, ConvertReceiverMode mode = ConvertReceiverMode::kAny);
  static Callable CallVarargs(Isolate* isolate);
  static Callable CallForwardVarargs(Isolate* isolate);
  static Callable CallFunctionForwardVarargs(Isolate* isolate);
  static Callable Construct(Isolate* isolate);
  static Callable ConstructWithSpread(Isolate* isolate);
  static Callable ConstructFunction(Isolate* isolate);
  static Callable ConstructVarargs(Isolate* isolate);
  static Callable ConstructForwardVarargs(Isolate* isolate);
  static Callable ConstructFunctionForwardVarargs(Isolate* isolate);

  static Callable InterpreterPushArgsThenCall(Isolate* isolate,
                                              ConvertReceiverMode receiver_mode,
                                              InterpreterPushArgsMode mode);
  static Callable InterpreterPushArgsThenConstruct(
      Isolate* isolate, InterpreterPushArgsMode mode);
  static Callable InterpreterCEntry(Isolate* isolate, int result_size = 1);
  static Callable InterpreterOnStackReplacement(Isolate* isolate);

  static Callable ArrayConstructor(Isolate* isolate);
  static Callable ArrayPop(Isolate* isolate);
  static Callable ArrayPush(Isolate* isolate);
  static Callable ArrayShift(Isolate* isolate);
  static Callable FunctionPrototypeBind(Isolate* isolate);
  static Callable TransitionElementsKind(Isolate* isolate, ElementsKind from,
                                         ElementsKind to, bool is_jsarray);
};

}  // namespace internal
}  // namespace v8

#endif  // V8_CODE_FACTORY_H_
