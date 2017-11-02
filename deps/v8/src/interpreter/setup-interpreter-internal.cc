// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/interpreter/setup-interpreter.h"

#include "src/handles-inl.h"
#include "src/interpreter/bytecodes.h"
#include "src/interpreter/interpreter-generator.h"
#include "src/interpreter/interpreter.h"
#include "src/objects-inl.h"

namespace v8 {
namespace internal {
namespace interpreter {

namespace {
void PrintBuiltinSize(Bytecode bytecode, OperandScale operand_scale,
                      Handle<Code> code) {
  PrintF(stdout, "Ignition Handler, %s, %d\n",
         Bytecodes::ToString(bytecode, operand_scale).c_str(),
         code->instruction_size());
}
}  // namespace

// static
void SetupInterpreter::InstallBytecodeHandlers(Interpreter* interpreter) {
  DCHECK(!interpreter->IsDispatchTableInitialized());
  HandleScope scope(interpreter->isolate_);
  // Canonicalize handles, so that we can share constant pool entries pointing
  // to code targets without dereferencing their handles.
  CanonicalHandleScope canonical(interpreter->isolate_);
  Address* dispatch_table = interpreter->dispatch_table_;

  // Generate bytecode handlers for all bytecodes and scales.
  const OperandScale kOperandScales[] = {
#define VALUE(Name, _) OperandScale::k##Name,
      OPERAND_SCALE_LIST(VALUE)
#undef VALUE
  };

  for (OperandScale operand_scale : kOperandScales) {
#define GENERATE_CODE(Name, ...)                                \
  InstallBytecodeHandler(interpreter->isolate_, dispatch_table, \
                         Bytecode::k##Name, operand_scale);
    BYTECODE_LIST(GENERATE_CODE)
#undef GENERATE_CODE
  }

  // Fill unused entries will the illegal bytecode handler.
  size_t illegal_index = Interpreter::GetDispatchTableIndex(
      Bytecode::kIllegal, OperandScale::kSingle);
  for (size_t index = 0; index < Interpreter::kDispatchTableSize; ++index) {
    if (dispatch_table[index] == nullptr) {
      dispatch_table[index] = dispatch_table[illegal_index];
    }
  }

  // Initialization should have been successful.
  DCHECK(interpreter->IsDispatchTableInitialized());
}

// static
bool SetupInterpreter::ReuseExistingHandler(Address* dispatch_table,
                                            Bytecode bytecode,
                                            OperandScale operand_scale) {
  size_t index = Interpreter::GetDispatchTableIndex(bytecode, operand_scale);
  switch (bytecode) {
    case Bytecode::kLdaImmutableContextSlot:
      STATIC_ASSERT(static_cast<int>(Bytecode::kLdaContextSlot) <
                    static_cast<int>(Bytecode::kLdaImmutableContextSlot));
      dispatch_table[index] = dispatch_table[Interpreter::GetDispatchTableIndex(
          Bytecode::kLdaContextSlot, operand_scale)];
      return true;
    case Bytecode::kLdaImmutableCurrentContextSlot:
      STATIC_ASSERT(
          static_cast<int>(Bytecode::kLdaCurrentContextSlot) <
          static_cast<int>(Bytecode::kLdaImmutableCurrentContextSlot));
      dispatch_table[index] = dispatch_table[Interpreter::GetDispatchTableIndex(
          Bytecode::kLdaCurrentContextSlot, operand_scale)];
      return true;
    default:
      return false;
  }
  return false;
}

// static
void SetupInterpreter::InstallBytecodeHandler(Isolate* isolate,
                                              Address* dispatch_table,
                                              Bytecode bytecode,
                                              OperandScale operand_scale) {
  if (!Bytecodes::BytecodeHasHandler(bytecode, operand_scale)) return;
  if (ReuseExistingHandler(dispatch_table, bytecode, operand_scale)) return;

  size_t index = Interpreter::GetDispatchTableIndex(bytecode, operand_scale);
  Handle<Code> code = GenerateBytecodeHandler(isolate, bytecode, operand_scale);
  dispatch_table[index] = code->entry();

  if (FLAG_print_builtin_size) PrintBuiltinSize(bytecode, operand_scale, code);
}

}  // namespace interpreter
}  // namespace internal
}  // namespace v8
