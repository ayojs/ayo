// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/code-assembler.h"

#include <ostream>

#include "src/code-factory.h"
#include "src/compiler/graph.h"
#include "src/compiler/instruction-selector.h"
#include "src/compiler/linkage.h"
#include "src/compiler/node-matchers.h"
#include "src/compiler/pipeline.h"
#include "src/compiler/raw-machine-assembler.h"
#include "src/compiler/schedule.h"
#include "src/frames.h"
#include "src/interface-descriptors.h"
#include "src/interpreter/bytecodes.h"
#include "src/machine-type.h"
#include "src/macro-assembler.h"
#include "src/objects-inl.h"
#include "src/utils.h"
#include "src/zone/zone.h"

#define REPEAT_1_TO_2(V, T) V(T) V(T, T)
#define REPEAT_1_TO_3(V, T) REPEAT_1_TO_2(V, T) V(T, T, T)
#define REPEAT_1_TO_4(V, T) REPEAT_1_TO_3(V, T) V(T, T, T, T)
#define REPEAT_1_TO_5(V, T) REPEAT_1_TO_4(V, T) V(T, T, T, T, T)
#define REPEAT_1_TO_6(V, T) REPEAT_1_TO_5(V, T) V(T, T, T, T, T, T)
#define REPEAT_1_TO_7(V, T) REPEAT_1_TO_6(V, T) V(T, T, T, T, T, T, T)
#define REPEAT_1_TO_8(V, T) REPEAT_1_TO_7(V, T) V(T, T, T, T, T, T, T, T)
#define REPEAT_1_TO_9(V, T) REPEAT_1_TO_8(V, T) V(T, T, T, T, T, T, T, T, T)
#define REPEAT_1_TO_10(V, T) REPEAT_1_TO_9(V, T) V(T, T, T, T, T, T, T, T, T, T)
#define REPEAT_1_TO_11(V, T) \
  REPEAT_1_TO_10(V, T) V(T, T, T, T, T, T, T, T, T, T, T)
#define REPEAT_1_TO_12(V, T) \
  REPEAT_1_TO_11(V, T) V(T, T, T, T, T, T, T, T, T, T, T, T)

namespace v8 {
namespace internal {
namespace compiler {

CodeAssemblerState::CodeAssemblerState(
    Isolate* isolate, Zone* zone, const CallInterfaceDescriptor& descriptor,
    Code::Flags flags, const char* name, size_t result_size)
    : CodeAssemblerState(
          isolate, zone,
          Linkage::GetStubCallDescriptor(
              isolate, zone, descriptor, descriptor.GetStackParameterCount(),
              CallDescriptor::kNoFlags, Operator::kNoProperties,
              MachineType::AnyTagged(), result_size),
          flags, name) {}

CodeAssemblerState::CodeAssemblerState(Isolate* isolate, Zone* zone,
                                       int parameter_count, Code::Flags flags,
                                       const char* name)
    : CodeAssemblerState(isolate, zone,
                         Linkage::GetJSCallDescriptor(
                             zone, false, parameter_count,
                             Code::ExtractKindFromFlags(flags) == Code::BUILTIN
                                 ? CallDescriptor::kPushArgumentCount
                                 : CallDescriptor::kNoFlags),
                         flags, name) {}

CodeAssemblerState::CodeAssemblerState(Isolate* isolate, Zone* zone,
                                       CallDescriptor* call_descriptor,
                                       Code::Flags flags, const char* name)
    : raw_assembler_(new RawMachineAssembler(
          isolate, new (zone) Graph(zone), call_descriptor,
          MachineType::PointerRepresentation(),
          InstructionSelector::SupportedMachineOperatorFlags(),
          InstructionSelector::AlignmentRequirements())),
      flags_(flags),
      name_(name),
      code_generated_(false),
      variables_(zone) {}

CodeAssemblerState::~CodeAssemblerState() {}

int CodeAssemblerState::parameter_count() const {
  return static_cast<int>(raw_assembler_->call_descriptor()->ParameterCount());
}

CodeAssembler::~CodeAssembler() {}

#if DEBUG
void CodeAssemblerState::PrintCurrentBlock(std::ostream& os) {
  raw_assembler_->PrintCurrentBlock(os);
}
#endif

void CodeAssemblerState::SetInitialDebugInformation(const char* msg,
                                                    const char* file,
                                                    int line) {
#if DEBUG
  AssemblerDebugInfo debug_info = {msg, file, line};
  raw_assembler_->SetInitialDebugInformation(debug_info);
#endif  // DEBUG
}

class BreakOnNodeDecorator final : public GraphDecorator {
 public:
  explicit BreakOnNodeDecorator(NodeId node_id) : node_id_(node_id) {}

  void Decorate(Node* node) final {
    if (node->id() == node_id_) {
      base::OS::DebugBreak();
    }
  }

 private:
  NodeId node_id_;
};

void CodeAssembler::BreakOnNode(int node_id) {
  Graph* graph = raw_assembler()->graph();
  Zone* zone = graph->zone();
  GraphDecorator* decorator =
      new (zone) BreakOnNodeDecorator(static_cast<NodeId>(node_id));
  graph->AddDecorator(decorator);
}

void CodeAssembler::RegisterCallGenerationCallbacks(
    const CodeAssemblerCallback& call_prologue,
    const CodeAssemblerCallback& call_epilogue) {
  // The callback can be registered only once.
  DCHECK(!state_->call_prologue_);
  DCHECK(!state_->call_epilogue_);
  state_->call_prologue_ = call_prologue;
  state_->call_epilogue_ = call_epilogue;
}

void CodeAssembler::UnregisterCallGenerationCallbacks() {
  state_->call_prologue_ = nullptr;
  state_->call_epilogue_ = nullptr;
}

void CodeAssembler::CallPrologue() {
  if (state_->call_prologue_) {
    state_->call_prologue_();
  }
}

void CodeAssembler::CallEpilogue() {
  if (state_->call_epilogue_) {
    state_->call_epilogue_();
  }
}

// static
Handle<Code> CodeAssembler::GenerateCode(CodeAssemblerState* state) {
  DCHECK(!state->code_generated_);

  RawMachineAssembler* rasm = state->raw_assembler_.get();
  Schedule* schedule = rasm->Export();

  JumpOptimizationInfo jump_opt;
  bool should_optimize_jumps =
      rasm->isolate()->serializer_enabled() && FLAG_turbo_rewrite_far_jumps;

  Handle<Code> code = Pipeline::GenerateCodeForCodeStub(
      rasm->isolate(), rasm->call_descriptor(), rasm->graph(), schedule,
      state->flags_, state->name_, should_optimize_jumps ? &jump_opt : nullptr);

  if (jump_opt.is_optimizable()) {
    jump_opt.set_optimizing();

    // Regenerate machine code
    code = Pipeline::GenerateCodeForCodeStub(
        rasm->isolate(), rasm->call_descriptor(), rasm->graph(), schedule,
        state->flags_, state->name_, &jump_opt);
  }

  state->code_generated_ = true;
  return code;
}

bool CodeAssembler::Is64() const { return raw_assembler()->machine()->Is64(); }

bool CodeAssembler::IsFloat64RoundUpSupported() const {
  return raw_assembler()->machine()->Float64RoundUp().IsSupported();
}

bool CodeAssembler::IsFloat64RoundDownSupported() const {
  return raw_assembler()->machine()->Float64RoundDown().IsSupported();
}

bool CodeAssembler::IsFloat64RoundTiesEvenSupported() const {
  return raw_assembler()->machine()->Float64RoundTiesEven().IsSupported();
}

bool CodeAssembler::IsFloat64RoundTruncateSupported() const {
  return raw_assembler()->machine()->Float64RoundTruncate().IsSupported();
}

bool CodeAssembler::IsInt32AbsWithOverflowSupported() const {
  return raw_assembler()->machine()->Int32AbsWithOverflow().IsSupported();
}

bool CodeAssembler::IsInt64AbsWithOverflowSupported() const {
  return raw_assembler()->machine()->Int64AbsWithOverflow().IsSupported();
}

bool CodeAssembler::IsIntPtrAbsWithOverflowSupported() const {
  return Is64() ? IsInt64AbsWithOverflowSupported()
                : IsInt32AbsWithOverflowSupported();
}

TNode<Int32T> CodeAssembler::Int32Constant(int32_t value) {
  return UncheckedCast<Int32T>(raw_assembler()->Int32Constant(value));
}

TNode<Int64T> CodeAssembler::Int64Constant(int64_t value) {
  return UncheckedCast<Int64T>(raw_assembler()->Int64Constant(value));
}

TNode<IntPtrT> CodeAssembler::IntPtrConstant(intptr_t value) {
  return UncheckedCast<IntPtrT>(raw_assembler()->IntPtrConstant(value));
}

TNode<Object> CodeAssembler::NumberConstant(double value) {
  return UncheckedCast<Object>(raw_assembler()->NumberConstant(value));
}

TNode<Smi> CodeAssembler::SmiConstant(Smi* value) {
  return UncheckedCast<Smi>(
      BitcastWordToTaggedSigned(IntPtrConstant(bit_cast<intptr_t>(value))));
}

TNode<Smi> CodeAssembler::SmiConstant(int value) {
  return SmiConstant(Smi::FromInt(value));
}

TNode<HeapObject> CodeAssembler::UntypedHeapConstant(
    Handle<HeapObject> object) {
  return UncheckedCast<HeapObject>(raw_assembler()->HeapConstant(object));
}

TNode<String> CodeAssembler::StringConstant(const char* str) {
  return UncheckedCast<String>(
      HeapConstant(factory()->NewStringFromAsciiChecked(str, TENURED)));
}

TNode<Oddball> CodeAssembler::BooleanConstant(bool value) {
  return UncheckedCast<Oddball>(raw_assembler()->BooleanConstant(value));
}

TNode<ExternalReference> CodeAssembler::ExternalConstant(
    ExternalReference address) {
  return UncheckedCast<ExternalReference>(
      raw_assembler()->ExternalConstant(address));
}

TNode<Float64T> CodeAssembler::Float64Constant(double value) {
  return UncheckedCast<Float64T>(raw_assembler()->Float64Constant(value));
}

TNode<HeapNumber> CodeAssembler::NaNConstant() {
  return UncheckedCast<HeapNumber>(LoadRoot(Heap::kNanValueRootIndex));
}

bool CodeAssembler::ToInt32Constant(Node* node, int32_t& out_value) {
  Int64Matcher m(node);
  if (m.HasValue() &&
      m.IsInRange(std::numeric_limits<int32_t>::min(),
                  std::numeric_limits<int32_t>::max())) {
    out_value = static_cast<int32_t>(m.Value());
    return true;
  }

  return false;
}

bool CodeAssembler::ToInt64Constant(Node* node, int64_t& out_value) {
  Int64Matcher m(node);
  if (m.HasValue()) out_value = m.Value();
  return m.HasValue();
}

bool CodeAssembler::ToSmiConstant(Node* node, Smi*& out_value) {
  if (node->opcode() == IrOpcode::kBitcastWordToTaggedSigned) {
    node = node->InputAt(0);
  } else {
    return false;
  }
  IntPtrMatcher m(node);
  if (m.HasValue()) {
    out_value = Smi::cast(bit_cast<Object*>(m.Value()));
    return true;
  }
  return false;
}

bool CodeAssembler::ToIntPtrConstant(Node* node, intptr_t& out_value) {
  if (node->opcode() == IrOpcode::kBitcastWordToTaggedSigned ||
      node->opcode() == IrOpcode::kBitcastWordToTagged) {
    node = node->InputAt(0);
  }
  IntPtrMatcher m(node);
  if (m.HasValue()) out_value = m.Value();
  return m.HasValue();
}

Node* CodeAssembler::Parameter(int value) {
  return raw_assembler()->Parameter(value);
}

TNode<Context> CodeAssembler::GetJSContextParameter() {
  CallDescriptor* desc = raw_assembler()->call_descriptor();
  DCHECK(desc->IsJSFunctionCall());
  return CAST(Parameter(Linkage::GetJSCallContextParamIndex(
      static_cast<int>(desc->JSParameterCount()))));
}

void CodeAssembler::Return(SloppyTNode<Object> value) {
  return raw_assembler()->Return(value);
}

void CodeAssembler::Return(SloppyTNode<Object> value1,
                           SloppyTNode<Object> value2) {
  return raw_assembler()->Return(value1, value2);
}

void CodeAssembler::Return(SloppyTNode<Object> value1,
                           SloppyTNode<Object> value2,
                           SloppyTNode<Object> value3) {
  return raw_assembler()->Return(value1, value2, value3);
}

void CodeAssembler::PopAndReturn(Node* pop, Node* value) {
  return raw_assembler()->PopAndReturn(pop, value);
}

void CodeAssembler::ReturnIf(Node* condition, Node* value) {
  Label if_return(this), if_continue(this);
  Branch(condition, &if_return, &if_continue);
  Bind(&if_return);
  Return(value);
  Bind(&if_continue);
}

void CodeAssembler::DebugAbort(Node* message) {
  raw_assembler()->DebugAbort(message);
}

void CodeAssembler::DebugBreak() { raw_assembler()->DebugBreak(); }

void CodeAssembler::Unreachable() {
  DebugBreak();
  raw_assembler()->Unreachable();
}

void CodeAssembler::Comment(const char* format, ...) {
  if (!FLAG_code_comments) return;
  char buffer[4 * KB];
  StringBuilder builder(buffer, arraysize(buffer));
  va_list arguments;
  va_start(arguments, format);
  builder.AddFormattedList(format, arguments);
  va_end(arguments);

  // Copy the string before recording it in the assembler to avoid
  // issues when the stack allocated buffer goes out of scope.
  const int prefix_len = 2;
  int length = builder.position() + 1;
  char* copy = reinterpret_cast<char*>(malloc(length + prefix_len));
  MemCopy(copy + prefix_len, builder.Finalize(), length);
  copy[0] = ';';
  copy[1] = ' ';
  raw_assembler()->Comment(copy);
}

void CodeAssembler::Bind(Label* label) { return label->Bind(); }

#if DEBUG
void CodeAssembler::Bind(Label* label, AssemblerDebugInfo debug_info) {
  return label->Bind(debug_info);
}
#endif  // DEBUG

Node* CodeAssembler::LoadFramePointer() {
  return raw_assembler()->LoadFramePointer();
}

Node* CodeAssembler::LoadParentFramePointer() {
  return raw_assembler()->LoadParentFramePointer();
}

Node* CodeAssembler::LoadStackPointer() {
  return raw_assembler()->LoadStackPointer();
}

#define DEFINE_CODE_ASSEMBLER_BINARY_OP(name, ResType, Arg1Type, Arg2Type) \
  TNode<ResType> CodeAssembler::name(SloppyTNode<Arg1Type> a,              \
                                     SloppyTNode<Arg2Type> b) {            \
    return UncheckedCast<ResType>(raw_assembler()->name(a, b));            \
  }
CODE_ASSEMBLER_BINARY_OP_LIST(DEFINE_CODE_ASSEMBLER_BINARY_OP)
#undef DEFINE_CODE_ASSEMBLER_BINARY_OP

TNode<WordT> CodeAssembler::IntPtrAdd(SloppyTNode<WordT> left,
                                      SloppyTNode<WordT> right) {
  intptr_t left_constant;
  bool is_left_constant = ToIntPtrConstant(left, left_constant);
  intptr_t right_constant;
  bool is_right_constant = ToIntPtrConstant(right, right_constant);
  if (is_left_constant) {
    if (is_right_constant) {
      return IntPtrConstant(left_constant + right_constant);
    }
    if (left_constant == 0) {
      return right;
    }
  } else if (is_right_constant) {
    if (right_constant == 0) {
      return left;
    }
  }
  return UncheckedCast<WordT>(raw_assembler()->IntPtrAdd(left, right));
}

TNode<WordT> CodeAssembler::IntPtrSub(SloppyTNode<WordT> left,
                                      SloppyTNode<WordT> right) {
  intptr_t left_constant;
  bool is_left_constant = ToIntPtrConstant(left, left_constant);
  intptr_t right_constant;
  bool is_right_constant = ToIntPtrConstant(right, right_constant);
  if (is_left_constant) {
    if (is_right_constant) {
      return IntPtrConstant(left_constant - right_constant);
    }
  } else if (is_right_constant) {
    if (right_constant == 0) {
      return left;
    }
  }
  return UncheckedCast<IntPtrT>(raw_assembler()->IntPtrSub(left, right));
}

TNode<WordT> CodeAssembler::WordShl(SloppyTNode<WordT> value, int shift) {
  return (shift != 0) ? WordShl(value, IntPtrConstant(shift)) : value;
}

TNode<WordT> CodeAssembler::WordShr(SloppyTNode<WordT> value, int shift) {
  return (shift != 0) ? WordShr(value, IntPtrConstant(shift)) : value;
}

TNode<Word32T> CodeAssembler::Word32Shr(SloppyTNode<Word32T> value, int shift) {
  return (shift != 0) ? Word32Shr(value, Int32Constant(shift)) : value;
}

TNode<UintPtrT> CodeAssembler::ChangeUint32ToWord(SloppyTNode<Word32T> value) {
  if (raw_assembler()->machine()->Is64()) {
    return UncheckedCast<UintPtrT>(
        raw_assembler()->ChangeUint32ToUint64(value));
  }
  return UncheckedCast<UintPtrT>(value);
}

TNode<IntPtrT> CodeAssembler::ChangeInt32ToIntPtr(SloppyTNode<Word32T> value) {
  if (raw_assembler()->machine()->Is64()) {
    return UncheckedCast<IntPtrT>(raw_assembler()->ChangeInt32ToInt64(value));
  }
  return UncheckedCast<IntPtrT>(value);
}

Node* CodeAssembler::ChangeFloat64ToUintPtr(Node* value) {
  if (raw_assembler()->machine()->Is64()) {
    return raw_assembler()->ChangeFloat64ToUint64(value);
  }
  return raw_assembler()->ChangeFloat64ToUint32(value);
}

Node* CodeAssembler::RoundIntPtrToFloat64(Node* value) {
  if (raw_assembler()->machine()->Is64()) {
    return raw_assembler()->RoundInt64ToFloat64(value);
  }
  return raw_assembler()->ChangeInt32ToFloat64(value);
}

#define DEFINE_CODE_ASSEMBLER_UNARY_OP(name, ResType, ArgType) \
  TNode<ResType> CodeAssembler::name(SloppyTNode<ArgType> a) { \
    return UncheckedCast<ResType>(raw_assembler()->name(a));   \
  }
CODE_ASSEMBLER_UNARY_OP_LIST(DEFINE_CODE_ASSEMBLER_UNARY_OP)
#undef DEFINE_CODE_ASSEMBLER_UNARY_OP

Node* CodeAssembler::Load(MachineType rep, Node* base) {
  return raw_assembler()->Load(rep, base);
}

Node* CodeAssembler::Load(MachineType rep, Node* base, Node* offset) {
  return raw_assembler()->Load(rep, base, offset);
}

Node* CodeAssembler::AtomicLoad(MachineType rep, Node* base, Node* offset) {
  return raw_assembler()->AtomicLoad(rep, base, offset);
}

TNode<Object> CodeAssembler::LoadRoot(Heap::RootListIndex root_index) {
  if (isolate()->heap()->RootCanBeTreatedAsConstant(root_index)) {
    Handle<Object> root = isolate()->heap()->root_handle(root_index);
    if (root->IsSmi()) {
      return SmiConstant(Smi::cast(*root));
    } else {
      return HeapConstant(Handle<HeapObject>::cast(root));
    }
  }

  Node* roots_array_start =
      ExternalConstant(ExternalReference::roots_array_start(isolate()));
  return UncheckedCast<Object>(Load(MachineType::AnyTagged(), roots_array_start,
                                    IntPtrConstant(root_index * kPointerSize)));
}

Node* CodeAssembler::Store(Node* base, Node* value) {
  return raw_assembler()->Store(MachineRepresentation::kTagged, base, value,
                                kFullWriteBarrier);
}

Node* CodeAssembler::Store(Node* base, Node* offset, Node* value) {
  return raw_assembler()->Store(MachineRepresentation::kTagged, base, offset,
                                value, kFullWriteBarrier);
}

Node* CodeAssembler::StoreWithMapWriteBarrier(Node* base, Node* offset,
                                              Node* value) {
  return raw_assembler()->Store(MachineRepresentation::kTagged, base, offset,
                                value, kMapWriteBarrier);
}

Node* CodeAssembler::StoreNoWriteBarrier(MachineRepresentation rep, Node* base,
                                         Node* value) {
  return raw_assembler()->Store(rep, base, value, kNoWriteBarrier);
}

Node* CodeAssembler::StoreNoWriteBarrier(MachineRepresentation rep, Node* base,
                                         Node* offset, Node* value) {
  return raw_assembler()->Store(rep, base, offset, value, kNoWriteBarrier);
}

Node* CodeAssembler::AtomicStore(MachineRepresentation rep, Node* base,
                                 Node* offset, Node* value) {
  return raw_assembler()->AtomicStore(rep, base, offset, value);
}

#define ATOMIC_FUNCTION(name)                                        \
  Node* CodeAssembler::Atomic##name(MachineType type, Node* base,    \
                                    Node* offset, Node* value) {     \
    return raw_assembler()->Atomic##name(type, base, offset, value); \
  }
ATOMIC_FUNCTION(Exchange);
ATOMIC_FUNCTION(Add);
ATOMIC_FUNCTION(Sub);
ATOMIC_FUNCTION(And);
ATOMIC_FUNCTION(Or);
ATOMIC_FUNCTION(Xor);
#undef ATOMIC_FUNCTION

Node* CodeAssembler::AtomicCompareExchange(MachineType type, Node* base,
                                           Node* offset, Node* old_value,
                                           Node* new_value) {
  return raw_assembler()->AtomicCompareExchange(type, base, offset, old_value,
                                                new_value);
}

Node* CodeAssembler::StoreRoot(Heap::RootListIndex root_index, Node* value) {
  DCHECK(Heap::RootCanBeWrittenAfterInitialization(root_index));
  Node* roots_array_start =
      ExternalConstant(ExternalReference::roots_array_start(isolate()));
  return StoreNoWriteBarrier(MachineRepresentation::kTagged, roots_array_start,
                             IntPtrConstant(root_index * kPointerSize), value);
}

Node* CodeAssembler::Retain(Node* value) {
  return raw_assembler()->Retain(value);
}

Node* CodeAssembler::Projection(int index, Node* value) {
  return raw_assembler()->Projection(index, value);
}

void CodeAssembler::GotoIfException(Node* node, Label* if_exception,
                                    Variable* exception_var) {
  DCHECK(!node->op()->HasProperty(Operator::kNoThrow));

  if (if_exception == nullptr) {
    // If no handler is supplied, don't add continuations
    return;
  }

  Label success(this), exception(this, Label::kDeferred);
  success.MergeVariables();
  exception.MergeVariables();

  raw_assembler()->Continuations(node, success.label_, exception.label_);

  Bind(&exception);
  const Operator* op = raw_assembler()->common()->IfException();
  Node* exception_value = raw_assembler()->AddNode(op, node, node);
  if (exception_var != nullptr) {
    exception_var->Bind(exception_value);
  }
  Goto(if_exception);

  Bind(&success);
}

template <class... TArgs>
TNode<Object> CodeAssembler::CallRuntimeImpl(Runtime::FunctionId function,
                                             SloppyTNode<Object> context,
                                             TArgs... args) {
  int argc = static_cast<int>(sizeof...(args));
  CallDescriptor* desc = Linkage::GetRuntimeCallDescriptor(
      zone(), function, argc, Operator::kNoProperties,
      CallDescriptor::kNoFlags);
  int return_count = static_cast<int>(desc->ReturnCount());

  Node* centry =
      HeapConstant(CodeFactory::RuntimeCEntry(isolate(), return_count));
  Node* ref = ExternalConstant(ExternalReference(function, isolate()));
  Node* arity = Int32Constant(argc);

  Node* nodes[] = {centry, args..., ref, arity, context};

  CallPrologue();
  Node* return_value = raw_assembler()->CallN(desc, arraysize(nodes), nodes);
  CallEpilogue();
  return UncheckedCast<Object>(return_value);
}

// Instantiate CallRuntime() for argument counts used by CSA-generated code
#define INSTANTIATE(...)                                                   \
  template V8_EXPORT_PRIVATE TNode<Object> CodeAssembler::CallRuntimeImpl( \
      Runtime::FunctionId, __VA_ARGS__);
REPEAT_1_TO_7(INSTANTIATE, SloppyTNode<Object>)
#undef INSTANTIATE

template <class... TArgs>
TNode<Object> CodeAssembler::TailCallRuntimeImpl(Runtime::FunctionId function,
                                                 SloppyTNode<Object> context,
                                                 TArgs... args) {
  int argc = static_cast<int>(sizeof...(args));
  CallDescriptor* desc = Linkage::GetRuntimeCallDescriptor(
      zone(), function, argc, Operator::kNoProperties,
      CallDescriptor::kSupportsTailCalls);
  int return_count = static_cast<int>(desc->ReturnCount());

  Node* centry =
      HeapConstant(CodeFactory::RuntimeCEntry(isolate(), return_count));
  Node* ref = ExternalConstant(ExternalReference(function, isolate()));
  Node* arity = Int32Constant(argc);

  Node* nodes[] = {centry, args..., ref, arity, context};

  return UncheckedCast<Object>(
      raw_assembler()->TailCallN(desc, arraysize(nodes), nodes));
}

// Instantiate TailCallRuntime() for argument counts used by CSA-generated code
#define INSTANTIATE(...)                                                       \
  template V8_EXPORT_PRIVATE TNode<Object> CodeAssembler::TailCallRuntimeImpl( \
      Runtime::FunctionId, __VA_ARGS__);
REPEAT_1_TO_7(INSTANTIATE, SloppyTNode<Object>)
#undef INSTANTIATE

template <class... TArgs>
Node* CodeAssembler::CallStubR(const CallInterfaceDescriptor& descriptor,
                               size_t result_size, Node* target, Node* context,
                               TArgs... args) {
  Node* nodes[] = {target, args..., context};
  return CallStubN(descriptor, result_size, arraysize(nodes), nodes);
}

// Instantiate CallStubR() for argument counts used by CSA-generated code.
#define INSTANTIATE(...)                                     \
  template V8_EXPORT_PRIVATE Node* CodeAssembler::CallStubR( \
      const CallInterfaceDescriptor& descriptor, size_t, Node*, __VA_ARGS__);
REPEAT_1_TO_11(INSTANTIATE, Node*)
#undef INSTANTIATE

Node* CodeAssembler::CallStubN(const CallInterfaceDescriptor& descriptor,
                               size_t result_size, int input_count,
                               Node* const* inputs) {
  // 2 is for target and context.
  DCHECK_LE(2, input_count);
  int argc = input_count - 2;
  DCHECK_LE(descriptor.GetParameterCount(), argc);
  // Extra arguments not mentioned in the descriptor are passed on the stack.
  int stack_parameter_count = argc - descriptor.GetRegisterParameterCount();
  DCHECK_LE(descriptor.GetStackParameterCount(), stack_parameter_count);
  CallDescriptor* desc = Linkage::GetStubCallDescriptor(
      isolate(), zone(), descriptor, stack_parameter_count,
      CallDescriptor::kNoFlags, Operator::kNoProperties,
      MachineType::AnyTagged(), result_size);

  CallPrologue();
  Node* return_value = raw_assembler()->CallN(desc, input_count, inputs);
  CallEpilogue();
  return return_value;
}

template <class... TArgs>
Node* CodeAssembler::TailCallStubImpl(const CallInterfaceDescriptor& descriptor,
                                      Node* target, Node* context,
                                      TArgs... args) {
  DCHECK_EQ(descriptor.GetParameterCount(), sizeof...(args));
  size_t result_size = 1;
  CallDescriptor* desc = Linkage::GetStubCallDescriptor(
      isolate(), zone(), descriptor, descriptor.GetStackParameterCount(),
      CallDescriptor::kSupportsTailCalls, Operator::kNoProperties,
      MachineType::AnyTagged(), result_size);

  Node* nodes[] = {target, args..., context};
  CHECK_EQ(descriptor.GetParameterCount() + 2, arraysize(nodes));
  return raw_assembler()->TailCallN(desc, arraysize(nodes), nodes);
}

// Instantiate TailCallStub() for argument counts used by CSA-generated code
#define INSTANTIATE(...)                                            \
  template V8_EXPORT_PRIVATE Node* CodeAssembler::TailCallStubImpl( \
      const CallInterfaceDescriptor& descriptor, Node*, __VA_ARGS__);
REPEAT_1_TO_12(INSTANTIATE, Node*)
#undef INSTANTIATE

template <class... TArgs>
Node* CodeAssembler::TailCallStubThenBytecodeDispatch(
    const CallInterfaceDescriptor& descriptor, Node* target, Node* context,
    TArgs... args) {
  DCHECK_LE(descriptor.GetParameterCount(), sizeof...(args));
  // Extra arguments not mentioned in the descriptor are passed on the stack.
  int stack_parameter_count =
      sizeof...(args) - descriptor.GetRegisterParameterCount();
  DCHECK_LE(descriptor.GetStackParameterCount(), stack_parameter_count);
  CallDescriptor* desc = Linkage::GetStubCallDescriptor(
      isolate(), zone(), descriptor, stack_parameter_count,
      CallDescriptor::kSupportsTailCalls, Operator::kNoProperties,
      MachineType::AnyTagged(), 0);

  Node* nodes[] = {target, args..., context};
  return raw_assembler()->TailCallN(desc, arraysize(nodes), nodes);
}

// Instantiate TailCallJSAndBytecodeDispatch() for argument counts used by
// CSA-generated code
#define INSTANTIATE(...)                           \
  template V8_EXPORT_PRIVATE Node*                 \
  CodeAssembler::TailCallStubThenBytecodeDispatch( \
      const CallInterfaceDescriptor&, Node*, Node*, Node*, __VA_ARGS__);
REPEAT_1_TO_7(INSTANTIATE, Node*)
#undef INSTANTIATE

template <class... TArgs>
Node* CodeAssembler::TailCallBytecodeDispatch(
    const CallInterfaceDescriptor& descriptor, Node* target, TArgs... args) {
  DCHECK_EQ(descriptor.GetParameterCount(), sizeof...(args));
  CallDescriptor* desc = Linkage::GetBytecodeDispatchCallDescriptor(
      isolate(), zone(), descriptor, descriptor.GetStackParameterCount());

  Node* nodes[] = {target, args...};
  CHECK_EQ(descriptor.GetParameterCount() + 1, arraysize(nodes));
  return raw_assembler()->TailCallN(desc, arraysize(nodes), nodes);
}

// Instantiate TailCallBytecodeDispatch() for argument counts used by
// CSA-generated code
template V8_EXPORT_PRIVATE Node* CodeAssembler::TailCallBytecodeDispatch(
    const CallInterfaceDescriptor& descriptor, Node* target, Node*, Node*,
    Node*, Node*);

Node* CodeAssembler::CallCFunctionN(Signature<MachineType>* signature,
                                    int input_count, Node* const* inputs) {
  CallDescriptor* desc = Linkage::GetSimplifiedCDescriptor(zone(), signature);
  return raw_assembler()->CallN(desc, input_count, inputs);
}

Node* CodeAssembler::CallCFunction1(MachineType return_type,
                                    MachineType arg0_type, Node* function,
                                    Node* arg0) {
  return raw_assembler()->CallCFunction1(return_type, arg0_type, function,
                                         arg0);
}

Node* CodeAssembler::CallCFunction1WithCallerSavedRegisters(
    MachineType return_type, MachineType arg0_type, Node* function,
    Node* arg0) {
  DCHECK(return_type.LessThanOrEqualPointerSize());
  return raw_assembler()->CallCFunction1WithCallerSavedRegisters(
      return_type, arg0_type, function, arg0);
}

Node* CodeAssembler::CallCFunction2(MachineType return_type,
                                    MachineType arg0_type,
                                    MachineType arg1_type, Node* function,
                                    Node* arg0, Node* arg1) {
  return raw_assembler()->CallCFunction2(return_type, arg0_type, arg1_type,
                                         function, arg0, arg1);
}

Node* CodeAssembler::CallCFunction3(MachineType return_type,
                                    MachineType arg0_type,
                                    MachineType arg1_type,
                                    MachineType arg2_type, Node* function,
                                    Node* arg0, Node* arg1, Node* arg2) {
  return raw_assembler()->CallCFunction3(return_type, arg0_type, arg1_type,
                                         arg2_type, function, arg0, arg1, arg2);
}

Node* CodeAssembler::CallCFunction3WithCallerSavedRegisters(
    MachineType return_type, MachineType arg0_type, MachineType arg1_type,
    MachineType arg2_type, Node* function, Node* arg0, Node* arg1, Node* arg2) {
  DCHECK(return_type.LessThanOrEqualPointerSize());
  return raw_assembler()->CallCFunction3WithCallerSavedRegisters(
      return_type, arg0_type, arg1_type, arg2_type, function, arg0, arg1, arg2);
}

Node* CodeAssembler::CallCFunction6(
    MachineType return_type, MachineType arg0_type, MachineType arg1_type,
    MachineType arg2_type, MachineType arg3_type, MachineType arg4_type,
    MachineType arg5_type, Node* function, Node* arg0, Node* arg1, Node* arg2,
    Node* arg3, Node* arg4, Node* arg5) {
  return raw_assembler()->CallCFunction6(
      return_type, arg0_type, arg1_type, arg2_type, arg3_type, arg4_type,
      arg5_type, function, arg0, arg1, arg2, arg3, arg4, arg5);
}

Node* CodeAssembler::CallCFunction9(
    MachineType return_type, MachineType arg0_type, MachineType arg1_type,
    MachineType arg2_type, MachineType arg3_type, MachineType arg4_type,
    MachineType arg5_type, MachineType arg6_type, MachineType arg7_type,
    MachineType arg8_type, Node* function, Node* arg0, Node* arg1, Node* arg2,
    Node* arg3, Node* arg4, Node* arg5, Node* arg6, Node* arg7, Node* arg8) {
  return raw_assembler()->CallCFunction9(
      return_type, arg0_type, arg1_type, arg2_type, arg3_type, arg4_type,
      arg5_type, arg6_type, arg7_type, arg8_type, function, arg0, arg1, arg2,
      arg3, arg4, arg5, arg6, arg7, arg8);
}

void CodeAssembler::Goto(Label* label) {
  label->MergeVariables();
  raw_assembler()->Goto(label->label_);
}

void CodeAssembler::GotoIf(SloppyTNode<IntegralT> condition,
                           Label* true_label) {
  Label false_label(this);
  Branch(condition, true_label, &false_label);
  Bind(&false_label);
}

void CodeAssembler::GotoIfNot(SloppyTNode<IntegralT> condition,
                              Label* false_label) {
  Label true_label(this);
  Branch(condition, &true_label, false_label);
  Bind(&true_label);
}

void CodeAssembler::Branch(SloppyTNode<IntegralT> condition, Label* true_label,
                           Label* false_label) {
  true_label->MergeVariables();
  false_label->MergeVariables();
  return raw_assembler()->Branch(condition, true_label->label_,
                                 false_label->label_);
}

void CodeAssembler::Switch(Node* index, Label* default_label,
                           const int32_t* case_values, Label** case_labels,
                           size_t case_count) {
  RawMachineLabel** labels =
      new (zone()->New(sizeof(RawMachineLabel*) * case_count))
          RawMachineLabel*[case_count];
  for (size_t i = 0; i < case_count; ++i) {
    labels[i] = case_labels[i]->label_;
    case_labels[i]->MergeVariables();
    default_label->MergeVariables();
  }
  return raw_assembler()->Switch(index, default_label->label_, case_values,
                                 labels, case_count);
}

bool CodeAssembler::UnalignedLoadSupported(MachineRepresentation rep) const {
  return raw_assembler()->machine()->UnalignedLoadSupported(rep);
}
bool CodeAssembler::UnalignedStoreSupported(MachineRepresentation rep) const {
  return raw_assembler()->machine()->UnalignedStoreSupported(rep);
}

// RawMachineAssembler delegate helpers:
Isolate* CodeAssembler::isolate() const { return raw_assembler()->isolate(); }

Factory* CodeAssembler::factory() const { return isolate()->factory(); }

Zone* CodeAssembler::zone() const { return raw_assembler()->zone(); }

RawMachineAssembler* CodeAssembler::raw_assembler() const {
  return state_->raw_assembler_.get();
}

// The core implementation of Variable is stored through an indirection so
// that it can outlive the often block-scoped Variable declarations. This is
// needed to ensure that variable binding and merging through phis can
// properly be verified.
class CodeAssemblerVariable::Impl : public ZoneObject {
 public:
  explicit Impl(MachineRepresentation rep)
      :
#if DEBUG
        debug_info_(AssemblerDebugInfo(nullptr, nullptr, -1)),
#endif
        value_(nullptr),
        rep_(rep) {
  }

#if DEBUG
  AssemblerDebugInfo debug_info() const { return debug_info_; }
  void set_debug_info(AssemblerDebugInfo debug_info) {
    debug_info_ = debug_info;
  }

  AssemblerDebugInfo debug_info_;
#endif  // DEBUG
  Node* value_;
  MachineRepresentation rep_;
};

CodeAssemblerVariable::CodeAssemblerVariable(CodeAssembler* assembler,
                                             MachineRepresentation rep)
    : impl_(new (assembler->zone()) Impl(rep)), state_(assembler->state()) {
  state_->variables_.insert(impl_);
}

CodeAssemblerVariable::CodeAssemblerVariable(CodeAssembler* assembler,
                                             MachineRepresentation rep,
                                             Node* initial_value)
    : CodeAssemblerVariable(assembler, rep) {
  Bind(initial_value);
}

#if DEBUG
CodeAssemblerVariable::CodeAssemblerVariable(CodeAssembler* assembler,
                                             AssemblerDebugInfo debug_info,
                                             MachineRepresentation rep)
    : impl_(new (assembler->zone()) Impl(rep)), state_(assembler->state()) {
  impl_->set_debug_info(debug_info);
  state_->variables_.insert(impl_);
}

CodeAssemblerVariable::CodeAssemblerVariable(CodeAssembler* assembler,
                                             AssemblerDebugInfo debug_info,
                                             MachineRepresentation rep,
                                             Node* initial_value)
    : CodeAssemblerVariable(assembler, debug_info, rep) {
  impl_->set_debug_info(debug_info);
  Bind(initial_value);
}
#endif  // DEBUG

CodeAssemblerVariable::~CodeAssemblerVariable() {
  state_->variables_.erase(impl_);
}

void CodeAssemblerVariable::Bind(Node* value) { impl_->value_ = value; }

Node* CodeAssemblerVariable::value() const {
#if DEBUG
  if (!IsBound()) {
    std::stringstream str;
    str << "#Use of unbound variable:"
        << "#\n    Variable:      " << *this;
    if (state_) {
      str << "#\n    Current Block: ";
      state_->PrintCurrentBlock(str);
    }
    FATAL(str.str().c_str());
  }
#endif  // DEBUG
  return impl_->value_;
}

MachineRepresentation CodeAssemblerVariable::rep() const { return impl_->rep_; }

bool CodeAssemblerVariable::IsBound() const { return impl_->value_ != nullptr; }

std::ostream& operator<<(std::ostream& os,
                         const CodeAssemblerVariable::Impl& impl) {
#if DEBUG
  AssemblerDebugInfo info = impl.debug_info();
  if (info.name) os << "V" << info;
#endif  // DEBUG
  return os;
}

std::ostream& operator<<(std::ostream& os,
                         const CodeAssemblerVariable& variable) {
  os << *variable.impl_;
  return os;
}

CodeAssemblerLabel::CodeAssemblerLabel(CodeAssembler* assembler,
                                       size_t vars_count,
                                       CodeAssemblerVariable* const* vars,
                                       CodeAssemblerLabel::Type type)
    : bound_(false),
      merge_count_(0),
      state_(assembler->state()),
      label_(nullptr) {
  void* buffer = assembler->zone()->New(sizeof(RawMachineLabel));
  label_ = new (buffer)
      RawMachineLabel(type == kDeferred ? RawMachineLabel::kDeferred
                                        : RawMachineLabel::kNonDeferred);
  for (size_t i = 0; i < vars_count; ++i) {
    variable_phis_[vars[i]->impl_] = nullptr;
  }
}

CodeAssemblerLabel::~CodeAssemblerLabel() { label_->~RawMachineLabel(); }

void CodeAssemblerLabel::MergeVariables() {
  ++merge_count_;
  for (CodeAssemblerVariable::Impl* var : state_->variables_) {
    size_t count = 0;
    Node* node = var->value_;
    if (node != nullptr) {
      auto i = variable_merges_.find(var);
      if (i != variable_merges_.end()) {
        i->second.push_back(node);
        count = i->second.size();
      } else {
        count = 1;
        variable_merges_[var] = std::vector<Node*>(1, node);
      }
    }
    // If the following asserts, then you've jumped to a label without a bound
    // variable along that path that expects to merge its value into a phi.
    DCHECK(variable_phis_.find(var) == variable_phis_.end() ||
           count == merge_count_);
    USE(count);

    // If the label is already bound, we already know the set of variables to
    // merge and phi nodes have already been created.
    if (bound_) {
      auto phi = variable_phis_.find(var);
      if (phi != variable_phis_.end()) {
        DCHECK_NOT_NULL(phi->second);
        state_->raw_assembler_->AppendPhiInput(phi->second, node);
      } else {
        auto i = variable_merges_.find(var);
        if (i != variable_merges_.end()) {
          // If the following assert fires, then you've declared a variable that
          // has the same bound value along all paths up until the point you
          // bound this label, but then later merged a path with a new value for
          // the variable after the label bind (it's not possible to add phis to
          // the bound label after the fact, just make sure to list the variable
          // in the label's constructor's list of merged variables).
#if DEBUG
          if (find_if(i->second.begin(), i->second.end(),
                      [node](Node* e) -> bool { return node != e; }) !=
              i->second.end()) {
            std::stringstream str;
            str << "Unmerged variable found when jumping to block. \n"
                << "#    Variable:      " << *var;
            if (bound_) {
              str << "\n#    Target block:  " << *label_->block();
            }
            str << "\n#    Current Block: ";
            state_->PrintCurrentBlock(str);
            FATAL(str.str().c_str());
          }
#endif  // DEBUG
        }
      }
    }
  }
}

#if DEBUG
void CodeAssemblerLabel::Bind(AssemblerDebugInfo debug_info) {
  if (bound_) {
    std::stringstream str;
    str << "Cannot bind the same label twice:"
        << "\n#    current:  " << debug_info
        << "\n#    previous: " << *label_->block();
    FATAL(str.str().c_str());
  }
  state_->raw_assembler_->Bind(label_, debug_info);
  UpdateVariablesAfterBind();
}
#endif  // DEBUG

void CodeAssemblerLabel::Bind() {
  DCHECK(!bound_);
  state_->raw_assembler_->Bind(label_);
  UpdateVariablesAfterBind();
}

void CodeAssemblerLabel::UpdateVariablesAfterBind() {
  // Make sure that all variables that have changed along any path up to this
  // point are marked as merge variables.
  for (auto var : state_->variables_) {
    Node* shared_value = nullptr;
    auto i = variable_merges_.find(var);
    if (i != variable_merges_.end()) {
      for (auto value : i->second) {
        DCHECK(value != nullptr);
        if (value != shared_value) {
          if (shared_value == nullptr) {
            shared_value = value;
          } else {
            variable_phis_[var] = nullptr;
          }
        }
      }
    }
  }

  for (auto var : variable_phis_) {
    CodeAssemblerVariable::Impl* var_impl = var.first;
    auto i = variable_merges_.find(var_impl);
#if DEBUG
    bool not_found = i == variable_merges_.end();
    if (not_found || i->second.size() != merge_count_) {
      std::stringstream str;
      str << "A variable that has been marked as beeing merged at the label"
          << "\n# doesn't have a bound value along all of the paths that "
          << "\n# have been merged into the label up to this point."
          << "\n#"
          << "\n# This can happen in the following cases:"
          << "\n# - By explicitly marking it so in the label constructor"
          << "\n# - By having seen different bound values at branches"
          << "\n#"
          << "\n# Merge count:     expected=" << merge_count_
          << " vs. found=" << (not_found ? 0 : i->second.size())
          << "\n# Variable:      " << *var_impl
          << "\n# Current Block: " << *label_->block();
      FATAL(str.str().c_str());
    }
#endif  // DEBUG
    Node* phi = state_->raw_assembler_->Phi(
        var.first->rep_, static_cast<int>(merge_count_), &(i->second[0]));
    variable_phis_[var_impl] = phi;
  }

  // Bind all variables to a merge phi, the common value along all paths or
  // null.
  for (auto var : state_->variables_) {
    auto i = variable_phis_.find(var);
    if (i != variable_phis_.end()) {
      var->value_ = i->second;
    } else {
      auto j = variable_merges_.find(var);
      if (j != variable_merges_.end() && j->second.size() == merge_count_) {
        var->value_ = j->second.back();
      } else {
        var->value_ = nullptr;
      }
    }
  }

  bound_ = true;
}

}  // namespace compiler

Smi* CheckObjectType(Object* value, Smi* type, String* location) {
#ifdef DEBUG
  const char* expected;
  switch (static_cast<ObjectType>(type->value())) {
#define TYPE_CASE(Name)                            \
  case ObjectType::k##Name:                        \
    if (value->Is##Name()) return Smi::FromInt(0); \
    expected = #Name;                              \
    break;
#define TYPE_STRUCT_CASE(NAME, Name, name)         \
  case ObjectType::k##Name:                        \
    if (value->Is##Name()) return Smi::FromInt(0); \
    expected = #Name;                              \
    break;

    TYPE_CASE(Object)
    OBJECT_TYPE_LIST(TYPE_CASE)
    HEAP_OBJECT_TYPE_LIST(TYPE_CASE)
    STRUCT_LIST(TYPE_STRUCT_CASE)
#undef TYPE_CASE
#undef TYPE_STRUCT_CASE
  }
  std::stringstream value_description;
  value->Print(value_description);
  V8_Fatal(__FILE__, __LINE__,
           "Type cast failed in %s\n"
           "  Expected %s but found %s",
           location->ToAsciiArray(), expected, value_description.str().c_str());
#else
  UNREACHABLE();
#endif
}

}  // namespace internal
}  // namespace v8
