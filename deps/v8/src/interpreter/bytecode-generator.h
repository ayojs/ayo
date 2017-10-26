// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_INTERPRETER_BYTECODE_GENERATOR_H_
#define V8_INTERPRETER_BYTECODE_GENERATOR_H_

#include "src/ast/ast.h"
#include "src/interpreter/bytecode-array-builder.h"
#include "src/interpreter/bytecode-label.h"
#include "src/interpreter/bytecode-register.h"
#include "src/interpreter/bytecodes.h"

namespace v8 {
namespace internal {

class AstNodeSourceRanges;
class AstStringConstants;
class CompilationInfo;
enum class SourceRangeKind;

namespace interpreter {

class GlobalDeclarationsBuilder;
class LoopBuilder;
class BlockCoverageBuilder;
class BytecodeJumpTable;

class BytecodeGenerator final : public AstVisitor<BytecodeGenerator> {
 public:
  explicit BytecodeGenerator(CompilationInfo* info);

  void GenerateBytecode(uintptr_t stack_limit);
  Handle<BytecodeArray> FinalizeBytecode(Isolate* isolate);

#define DECLARE_VISIT(type) void Visit##type(type* node);
  AST_NODE_LIST(DECLARE_VISIT)
#undef DECLARE_VISIT

  // Visiting function for declarations list and statements are overridden.
  void VisitDeclarations(Declaration::List* declarations);
  void VisitStatements(ZoneList<Statement*>* statments);

 private:
  class ContextScope;
  class ControlScope;
  class ControlScopeForBreakable;
  class ControlScopeForIteration;
  class ControlScopeForTopLevel;
  class ControlScopeForTryCatch;
  class ControlScopeForTryFinally;
  class CurrentScope;
  class ExpressionResultScope;
  class EffectResultScope;
  class GlobalDeclarationsBuilder;
  class RegisterAllocationScope;
  class TestResultScope;
  class ValueResultScope;

  using ToBooleanMode = BytecodeArrayBuilder::ToBooleanMode;

  enum class TestFallthrough { kThen, kElse, kNone };
  enum class TypeHint { kAny, kBoolean };

  void GenerateBytecodeBody();
  void AllocateDeferredConstants(Isolate* isolate);

  DEFINE_AST_VISITOR_SUBCLASS_MEMBERS();

  // Dispatched from VisitBinaryOperation.
  void VisitArithmeticExpression(BinaryOperation* binop);
  void VisitCommaExpression(BinaryOperation* binop);
  void VisitLogicalOrExpression(BinaryOperation* binop);
  void VisitLogicalAndExpression(BinaryOperation* binop);

  // Dispatched from VisitUnaryOperation.
  void VisitVoid(UnaryOperation* expr);
  void VisitTypeOf(UnaryOperation* expr);
  void VisitNot(UnaryOperation* expr);
  void VisitDelete(UnaryOperation* expr);

  // Visits a typeof expression for the value on which to perform the typeof.
  void VisitForTypeOfValue(Expression* expr);

  // Used by flow control routines to evaluate loop condition.
  void VisitCondition(Expression* expr);

  // Visit the arguments expressions in |args| and store them in |args_regs|,
  // growing |args_regs| for each argument visited.
  void VisitArguments(ZoneList<Expression*>* args, RegisterList* arg_regs);

  // Visit a keyed super property load. The optional
  // |opt_receiver_out| register will have the receiver stored to it
  // if it's a valid register. The loaded value is placed in the
  // accumulator.
  void VisitKeyedSuperPropertyLoad(Property* property,
                                   Register opt_receiver_out);

  // Visit a named super property load. The optional
  // |opt_receiver_out| register will have the receiver stored to it
  // if it's a valid register. The loaded value is placed in the
  // accumulator.
  void VisitNamedSuperPropertyLoad(Property* property,
                                   Register opt_receiver_out);

  void VisitPropertyLoad(Register obj, Property* expr);
  void VisitPropertyLoadForRegister(Register obj, Property* expr,
                                    Register destination);

  void BuildVariableLoad(Variable* variable, FeedbackSlot slot,
                         HoleCheckMode hole_check_mode,
                         TypeofMode typeof_mode = NOT_INSIDE_TYPEOF);
  void BuildVariableLoadForAccumulatorValue(
      Variable* variable, FeedbackSlot slot, HoleCheckMode hole_check_mode,
      TypeofMode typeof_mode = NOT_INSIDE_TYPEOF);
  void BuildVariableAssignment(
      Variable* variable, Token::Value op, FeedbackSlot slot,
      HoleCheckMode hole_check_mode,
      LookupHoistingMode lookup_hoisting_mode = LookupHoistingMode::kNormal);
  void BuildLiteralCompareNil(Token::Value compare_op, NilValue nil);
  void BuildReturn(int source_position = kNoSourcePosition);
  void BuildAsyncReturn(int source_position = kNoSourcePosition);
  void BuildAsyncGeneratorReturn();
  void BuildReThrow();
  void BuildAbort(BailoutReason bailout_reason);
  void BuildHoleCheckForVariableAssignment(Variable* variable, Token::Value op);
  void BuildThrowIfHole(Variable* variable);

  // Build jump to targets[value], where
  // start_index <= value < start_index + size.
  void BuildIndexedJump(Register value, size_t start_index, size_t size,
                        ZoneVector<BytecodeLabel>& targets);

  void BuildNewLocalActivationContext();
  void BuildLocalActivationContextInitialization();
  void BuildNewLocalBlockContext(Scope* scope);
  void BuildNewLocalCatchContext(Scope* scope);
  void BuildNewLocalWithContext(Scope* scope);

  void BuildGeneratorPrologue();
  void BuildSuspendPoint(int suspend_id);

  void BuildAwait(int suspend_id);

  void BuildGetIterator(Expression* iterable, IteratorType hint,
                        FeedbackSlot load_slot, FeedbackSlot call_slot,
                        FeedbackSlot async_load_slot,
                        FeedbackSlot async_call_slot);

  void AllocateTopLevelRegisters();
  void VisitArgumentsObject(Variable* variable);
  void VisitRestArgumentsArray(Variable* rest);
  void VisitCallSuper(Call* call);
  void VisitClassLiteralProperties(ClassLiteral* expr, Register constructor,
                                   Register prototype);
  void BuildClassLiteralNameProperty(ClassLiteral* expr, Register constructor);
  void BuildClassLiteral(ClassLiteral* expr);
  void VisitNewTargetVariable(Variable* variable);
  void VisitThisFunctionVariable(Variable* variable);
  void BuildGeneratorObjectVariableInitialization();
  void VisitBlockDeclarationsAndStatements(Block* stmt);
  void VisitFunctionClosureForContext();
  void VisitSetHomeObject(Register value, Register home_object,
                          LiteralProperty* property, int slot_number = 0);
  void VisitObjectLiteralAccessor(Register home_object,
                                  ObjectLiteralProperty* property,
                                  Register value_out);
  void VisitForInAssignment(Expression* expr, FeedbackSlot slot);
  void VisitModuleNamespaceImports();

  // Builds a logical OR/AND within a test context by rewiring the jumps based
  // on the expression values.
  void BuildLogicalTest(Token::Value token, Expression* left,
                        Expression* right);

  // Visit the header/body of a loop iteration.
  void VisitIterationHeader(IterationStatement* stmt,
                            LoopBuilder* loop_builder);
  void VisitIterationHeader(int first_suspend_id, int suspend_count,
                            LoopBuilder* loop_builder);
  void VisitIterationBody(IterationStatement* stmt, LoopBuilder* loop_builder);

  // Visit a statement and switch scopes, the context is in the accumulator.
  void VisitInScope(Statement* stmt, Scope* scope);

  void BuildPushUndefinedIntoRegisterList(RegisterList* reg_list);

  void BuildLoadPropertyKey(LiteralProperty* property, Register out_reg);

  int AllocateBlockCoverageSlotIfEnabled(AstNode* node, SourceRangeKind kind);
  void BuildIncrementBlockCoverageCounterIfEnabled(AstNode* node,
                                                   SourceRangeKind kind);
  void BuildIncrementBlockCoverageCounterIfEnabled(int coverage_array_slot);

  void BuildTest(ToBooleanMode mode, BytecodeLabels* then_labels,
                 BytecodeLabels* else_labels, TestFallthrough fallthrough);

  // Visitors for obtaining expression result in the accumulator, in a
  // register, or just getting the effect. Some visitors return a TypeHint which
  // specifies the type of the result of the visited expression.
  TypeHint VisitForAccumulatorValue(Expression* expr);
  void VisitForAccumulatorValueOrTheHole(Expression* expr);
  MUST_USE_RESULT Register VisitForRegisterValue(Expression* expr);
  INLINE(void VisitForRegisterValue(Expression* expr, Register destination));
  void VisitAndPushIntoRegisterList(Expression* expr, RegisterList* reg_list);
  void VisitForEffect(Expression* expr);
  void VisitForTest(Expression* expr, BytecodeLabels* then_labels,
                    BytecodeLabels* else_labels, TestFallthrough fallthrough);

  void VisitInSameTestExecutionScope(Expression* expr);

  Register GetRegisterForLocalVariable(Variable* variable);

  // Returns the runtime function id for a store to super for the function's
  // language mode.
  inline Runtime::FunctionId StoreToSuperRuntimeId();
  inline Runtime::FunctionId StoreKeyedToSuperRuntimeId();

  static constexpr ToBooleanMode ToBooleanModeFromTypeHint(TypeHint type_hint) {
    return type_hint == TypeHint::kBoolean ? ToBooleanMode::kAlreadyBoolean
                                           : ToBooleanMode::kConvertToBoolean;
  }

  inline Register generator_object() const;

  inline BytecodeArrayBuilder* builder() const { return builder_; }
  inline Zone* zone() const { return zone_; }
  inline DeclarationScope* closure_scope() const { return closure_scope_; }
  inline CompilationInfo* info() const { return info_; }
  inline const AstStringConstants* ast_string_constants() const {
    return ast_string_constants_;
  }

  inline Scope* current_scope() const { return current_scope_; }
  inline void set_current_scope(Scope* scope) { current_scope_ = scope; }

  inline ControlScope* execution_control() const { return execution_control_; }
  inline void set_execution_control(ControlScope* scope) {
    execution_control_ = scope;
  }
  inline ContextScope* execution_context() const { return execution_context_; }
  inline void set_execution_context(ContextScope* context) {
    execution_context_ = context;
  }
  inline void set_execution_result(ExpressionResultScope* execution_result) {
    execution_result_ = execution_result;
  }
  ExpressionResultScope* execution_result() const { return execution_result_; }
  BytecodeRegisterAllocator* register_allocator() const {
    return builder()->register_allocator();
  }

  GlobalDeclarationsBuilder* globals_builder() {
    DCHECK_NOT_NULL(globals_builder_);
    return globals_builder_;
  }
  inline LanguageMode language_mode() const;
  inline FunctionKind function_kind() const;
  int feedback_index(FeedbackSlot slot) const;

  inline HandlerTable::CatchPrediction catch_prediction() const {
    return catch_prediction_;
  }
  inline void set_catch_prediction(HandlerTable::CatchPrediction value) {
    catch_prediction_ = value;
  }

  Zone* zone_;
  BytecodeArrayBuilder* builder_;
  CompilationInfo* info_;
  const AstStringConstants* ast_string_constants_;
  DeclarationScope* closure_scope_;
  Scope* current_scope_;

  GlobalDeclarationsBuilder* globals_builder_;
  BlockCoverageBuilder* block_coverage_builder_;
  ZoneVector<GlobalDeclarationsBuilder*> global_declarations_;
  ZoneVector<std::pair<FunctionLiteral*, size_t>> function_literals_;
  ZoneVector<std::pair<NativeFunctionLiteral*, size_t>>
      native_function_literals_;
  ZoneVector<std::pair<ObjectLiteral*, size_t>> object_literals_;
  ZoneVector<std::pair<ArrayLiteral*, size_t>> array_literals_;

  ControlScope* execution_control_;
  ContextScope* execution_context_;
  ExpressionResultScope* execution_result_;

  Register incoming_new_target_or_generator_;

  BytecodeJumpTable* generator_jump_table_;
  Register generator_state_;
  int loop_depth_;

  HandlerTable::CatchPrediction catch_prediction_;
};

}  // namespace interpreter
}  // namespace internal
}  // namespace v8

#endif  // V8_INTERPRETER_BYTECODE_GENERATOR_H_
