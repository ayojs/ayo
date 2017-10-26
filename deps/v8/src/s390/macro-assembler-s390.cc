// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>  // For assert
#include <limits.h>  // For LONG_MIN, LONG_MAX.

#if V8_TARGET_ARCH_S390

#include "src/base/bits.h"
#include "src/base/division-by-constant.h"
#include "src/bootstrapper.h"
#include "src/codegen.h"
#include "src/debug/debug.h"
#include "src/external-reference-table.h"
#include "src/frames-inl.h"
#include "src/register-configuration.h"
#include "src/runtime/runtime.h"

#include "src/s390/macro-assembler-s390.h"

namespace v8 {
namespace internal {

MacroAssembler::MacroAssembler(Isolate* isolate, void* buffer, int size,
                               CodeObjectRequired create_code_object)
    : TurboAssembler(isolate, buffer, size, create_code_object) {}

void TurboAssembler::PushCallerSaved(SaveFPRegsMode fp_mode,
                                     Register exclusion1, Register exclusion2,
                                     Register exclusion3) {
  RegList exclusions = 0;
  if (!exclusion1.is(no_reg)) {
    exclusions |= exclusion1.bit();
    if (!exclusion2.is(no_reg)) {
      exclusions |= exclusion2.bit();
      if (!exclusion3.is(no_reg)) {
        exclusions |= exclusion3.bit();
      }
    }
  }

  MultiPush(kJSCallerSaved & ~exclusions);

  if (fp_mode == kSaveFPRegs) {
    MultiPushDoubles(kCallerSavedDoubles);
  }
}

void TurboAssembler::PopCallerSaved(SaveFPRegsMode fp_mode, Register exclusion1,
                                    Register exclusion2, Register exclusion3) {
  if (fp_mode == kSaveFPRegs) {
    MultiPopDoubles(kCallerSavedDoubles);
  }

  RegList exclusions = 0;
  if (!exclusion1.is(no_reg)) {
    exclusions |= exclusion1.bit();
    if (!exclusion2.is(no_reg)) {
      exclusions |= exclusion2.bit();
      if (!exclusion3.is(no_reg)) {
        exclusions |= exclusion3.bit();
      }
    }
  }

  MultiPop(kJSCallerSaved & ~exclusions);
}

void TurboAssembler::Jump(Register target) { b(target); }

void MacroAssembler::JumpToJSEntry(Register target) {
  Move(ip, target);
  Jump(ip);
}

void TurboAssembler::Jump(intptr_t target, RelocInfo::Mode rmode,
                          Condition cond, CRegister) {
  Label skip;

  if (cond != al) b(NegateCondition(cond), &skip);

  DCHECK(rmode == RelocInfo::CODE_TARGET || rmode == RelocInfo::RUNTIME_ENTRY);

  mov(ip, Operand(target, rmode));
  b(ip);

  bind(&skip);
}

void TurboAssembler::Jump(Address target, RelocInfo::Mode rmode, Condition cond,
                          CRegister cr) {
  DCHECK(!RelocInfo::IsCodeTarget(rmode));
  Jump(reinterpret_cast<intptr_t>(target), rmode, cond, cr);
}

void TurboAssembler::Jump(Handle<Code> code, RelocInfo::Mode rmode,
                          Condition cond) {
  DCHECK(RelocInfo::IsCodeTarget(rmode));
  jump(code, rmode, cond);
}

int TurboAssembler::CallSize(Register target) { return 2; }  // BASR

void TurboAssembler::Call(Register target) {
  Label start;
  bind(&start);

  // Branch to target via indirect branch
  basr(r14, target);

  DCHECK_EQ(CallSize(target), SizeOfCodeGeneratedSince(&start));
}

void MacroAssembler::CallJSEntry(Register target) {
  DCHECK(target.is(ip));
  Call(target);
}

int TurboAssembler::CallSize(Address target, RelocInfo::Mode rmode,
                             Condition cond) {
  // S390 Assembler::move sequence is IILF / IIHF
  int size;
#if V8_TARGET_ARCH_S390X
  size = 14;  // IILF + IIHF + BASR
#else
  size = 8;  // IILF + BASR
#endif
  return size;
}

int MacroAssembler::CallSizeNotPredictableCodeSize(Address target,
                                                   RelocInfo::Mode rmode,
                                                   Condition cond) {
  // S390 Assembler::move sequence is IILF / IIHF
  int size;
#if V8_TARGET_ARCH_S390X
  size = 14;  // IILF + IIHF + BASR
#else
  size = 8;  // IILF + BASR
#endif
  return size;
}

void TurboAssembler::Call(Address target, RelocInfo::Mode rmode,
                          Condition cond) {
  DCHECK(cond == al);

#ifdef DEBUG
  // Check the expected size before generating code to ensure we assume the same
  // constant pool availability (e.g., whether constant pool is full or not).
  int expected_size = CallSize(target, rmode, cond);
  Label start;
  bind(&start);
#endif

  mov(ip, Operand(reinterpret_cast<intptr_t>(target), rmode));
  basr(r14, ip);

  DCHECK_EQ(expected_size, SizeOfCodeGeneratedSince(&start));
}

int TurboAssembler::CallSize(Handle<Code> code, RelocInfo::Mode rmode,
                             Condition cond) {
  return 6;  // BRASL
}

void TurboAssembler::Call(Handle<Code> code, RelocInfo::Mode rmode,
                          Condition cond) {
  DCHECK(RelocInfo::IsCodeTarget(rmode) && cond == al);

#ifdef DEBUG
  // Check the expected size before generating code to ensure we assume the same
  // constant pool availability (e.g., whether constant pool is full or not).
  int expected_size = CallSize(code, rmode, cond);
  Label start;
  bind(&start);
#endif
  call(code, rmode);
  DCHECK_EQ(expected_size, SizeOfCodeGeneratedSince(&start));
}

void TurboAssembler::Drop(int count) {
  if (count > 0) {
    int total = count * kPointerSize;
    if (is_uint12(total)) {
      la(sp, MemOperand(sp, total));
    } else if (is_int20(total)) {
      lay(sp, MemOperand(sp, total));
    } else {
      AddP(sp, Operand(total));
    }
  }
}

void TurboAssembler::Drop(Register count, Register scratch) {
  ShiftLeftP(scratch, count, Operand(kPointerSizeLog2));
  AddP(sp, sp, scratch);
}

void TurboAssembler::Call(Label* target) { b(r14, target); }

void TurboAssembler::Push(Handle<HeapObject> handle) {
  mov(r0, Operand(handle));
  push(r0);
}

void TurboAssembler::Push(Smi* smi) {
  mov(r0, Operand(smi));
  push(r0);
}

void TurboAssembler::Move(Register dst, Handle<HeapObject> value) {
  mov(dst, Operand(value));
}

void TurboAssembler::Move(Register dst, Register src, Condition cond) {
  if (!dst.is(src)) {
    LoadRR(dst, src);
  }
}

void TurboAssembler::Move(DoubleRegister dst, DoubleRegister src) {
  if (!dst.is(src)) {
    ldr(dst, src);
  }
}

void TurboAssembler::MultiPush(RegList regs, Register location) {
  int16_t num_to_push = base::bits::CountPopulation(regs);
  int16_t stack_offset = num_to_push * kPointerSize;

  SubP(location, location, Operand(stack_offset));
  for (int16_t i = Register::kNumRegisters - 1; i >= 0; i--) {
    if ((regs & (1 << i)) != 0) {
      stack_offset -= kPointerSize;
      StoreP(ToRegister(i), MemOperand(location, stack_offset));
    }
  }
}

void TurboAssembler::MultiPop(RegList regs, Register location) {
  int16_t stack_offset = 0;

  for (int16_t i = 0; i < Register::kNumRegisters; i++) {
    if ((regs & (1 << i)) != 0) {
      LoadP(ToRegister(i), MemOperand(location, stack_offset));
      stack_offset += kPointerSize;
    }
  }
  AddP(location, location, Operand(stack_offset));
}

void TurboAssembler::MultiPushDoubles(RegList dregs, Register location) {
  int16_t num_to_push = base::bits::CountPopulation(dregs);
  int16_t stack_offset = num_to_push * kDoubleSize;

  SubP(location, location, Operand(stack_offset));
  for (int16_t i = DoubleRegister::kNumRegisters - 1; i >= 0; i--) {
    if ((dregs & (1 << i)) != 0) {
      DoubleRegister dreg = DoubleRegister::from_code(i);
      stack_offset -= kDoubleSize;
      StoreDouble(dreg, MemOperand(location, stack_offset));
    }
  }
}

void TurboAssembler::MultiPopDoubles(RegList dregs, Register location) {
  int16_t stack_offset = 0;

  for (int16_t i = 0; i < DoubleRegister::kNumRegisters; i++) {
    if ((dregs & (1 << i)) != 0) {
      DoubleRegister dreg = DoubleRegister::from_code(i);
      LoadDouble(dreg, MemOperand(location, stack_offset));
      stack_offset += kDoubleSize;
    }
  }
  AddP(location, location, Operand(stack_offset));
}

void TurboAssembler::LoadRoot(Register destination, Heap::RootListIndex index,
                              Condition) {
  LoadP(destination, MemOperand(kRootRegister, index << kPointerSizeLog2), r0);
}

void MacroAssembler::InNewSpace(Register object, Register scratch,
                                Condition cond, Label* branch) {
  DCHECK(cond == eq || cond == ne);
  CheckPageFlag(object, scratch, MemoryChunk::kIsInNewSpaceMask, cond, branch);
}

void MacroAssembler::RecordWriteField(
    Register object, int offset, Register value, Register dst,
    LinkRegisterStatus lr_status, SaveFPRegsMode save_fp,
    RememberedSetAction remembered_set_action, SmiCheck smi_check,
    PointersToHereCheck pointers_to_here_check_for_value) {
  // First, check if a write barrier is even needed. The tests below
  // catch stores of Smis.
  Label done;

  // Skip barrier if writing a smi.
  if (smi_check == INLINE_SMI_CHECK) {
    JumpIfSmi(value, &done);
  }

  // Although the object register is tagged, the offset is relative to the start
  // of the object, so so offset must be a multiple of kPointerSize.
  DCHECK(IsAligned(offset, kPointerSize));

  lay(dst, MemOperand(object, offset - kHeapObjectTag));
  if (emit_debug_code()) {
    Label ok;
    AndP(r0, dst, Operand(kPointerSize - 1));
    beq(&ok, Label::kNear);
    stop("Unaligned cell in write barrier");
    bind(&ok);
  }

  RecordWrite(object, dst, value, lr_status, save_fp, remembered_set_action,
              OMIT_SMI_CHECK, pointers_to_here_check_for_value);

  bind(&done);

  // Clobber clobbered input registers when running with the debug-code flag
  // turned on to provoke errors.
  if (emit_debug_code()) {
    mov(value, Operand(bit_cast<intptr_t>(kZapValue + 4)));
    mov(dst, Operand(bit_cast<intptr_t>(kZapValue + 8)));
  }
}

// Will clobber 4 registers: object, map, dst, ip.  The
// register 'object' contains a heap object pointer.
void MacroAssembler::RecordWriteForMap(Register object, Register map,
                                       Register dst,
                                       LinkRegisterStatus lr_status,
                                       SaveFPRegsMode fp_mode) {
  if (emit_debug_code()) {
    LoadP(dst, FieldMemOperand(map, HeapObject::kMapOffset));
    CmpP(dst, Operand(isolate()->factory()->meta_map()));
    Check(eq, kWrongAddressOrValuePassedToRecordWrite);
  }

  if (!FLAG_incremental_marking) {
    return;
  }

  if (emit_debug_code()) {
    CmpP(map, FieldMemOperand(object, HeapObject::kMapOffset));
    Check(eq, kWrongAddressOrValuePassedToRecordWrite);
  }

  Label done;

  // A single check of the map's pages interesting flag suffices, since it is
  // only set during incremental collection, and then it's also guaranteed that
  // the from object's page's interesting flag is also set.  This optimization
  // relies on the fact that maps can never be in new space.
  CheckPageFlag(map,
                map,  // Used as scratch.
                MemoryChunk::kPointersToHereAreInterestingMask, eq, &done);

  lay(dst, MemOperand(object, HeapObject::kMapOffset - kHeapObjectTag));
  if (emit_debug_code()) {
    Label ok;
    AndP(r0, dst, Operand(kPointerSize - 1));
    beq(&ok, Label::kNear);
    stop("Unaligned cell in write barrier");
    bind(&ok);
  }

  // Record the actual write.
  if (lr_status == kLRHasNotBeenSaved) {
    push(r14);
  }
  RecordWriteStub stub(isolate(), object, map, dst, OMIT_REMEMBERED_SET,
                       fp_mode);
  CallStub(&stub);
  if (lr_status == kLRHasNotBeenSaved) {
    pop(r14);
  }

  bind(&done);

  // Count number of write barriers in generated code.
  isolate()->counters()->write_barriers_static()->Increment();
  IncrementCounter(isolate()->counters()->write_barriers_dynamic(), 1, ip, dst);

  // Clobber clobbered registers when running with the debug-code flag
  // turned on to provoke errors.
  if (emit_debug_code()) {
    mov(dst, Operand(bit_cast<intptr_t>(kZapValue + 12)));
    mov(map, Operand(bit_cast<intptr_t>(kZapValue + 16)));
  }
}

// Will clobber 4 registers: object, address, scratch, ip.  The
// register 'object' contains a heap object pointer.  The heap object
// tag is shifted away.
void MacroAssembler::RecordWrite(
    Register object, Register address, Register value,
    LinkRegisterStatus lr_status, SaveFPRegsMode fp_mode,
    RememberedSetAction remembered_set_action, SmiCheck smi_check,
    PointersToHereCheck pointers_to_here_check_for_value) {
  DCHECK(!object.is(value));
  if (emit_debug_code()) {
    CmpP(value, MemOperand(address));
    Check(eq, kWrongAddressOrValuePassedToRecordWrite);
  }

  if (remembered_set_action == OMIT_REMEMBERED_SET &&
      !FLAG_incremental_marking) {
    return;
  }
  // First, check if a write barrier is even needed. The tests below
  // catch stores of smis and stores into the young generation.
  Label done;

  if (smi_check == INLINE_SMI_CHECK) {
    JumpIfSmi(value, &done);
  }

  if (pointers_to_here_check_for_value != kPointersToHereAreAlwaysInteresting) {
    CheckPageFlag(value,
                  value,  // Used as scratch.
                  MemoryChunk::kPointersToHereAreInterestingMask, eq, &done);
  }
  CheckPageFlag(object,
                value,  // Used as scratch.
                MemoryChunk::kPointersFromHereAreInterestingMask, eq, &done);

  // Record the actual write.
  if (lr_status == kLRHasNotBeenSaved) {
    push(r14);
  }
  RecordWriteStub stub(isolate(), object, value, address, remembered_set_action,
                       fp_mode);
  CallStub(&stub);
  if (lr_status == kLRHasNotBeenSaved) {
    pop(r14);
  }

  bind(&done);

  // Count number of write barriers in generated code.
  isolate()->counters()->write_barriers_static()->Increment();
  IncrementCounter(isolate()->counters()->write_barriers_dynamic(), 1, ip,
                   value);

  // Clobber clobbered registers when running with the debug-code flag
  // turned on to provoke errors.
  if (emit_debug_code()) {
    mov(address, Operand(bit_cast<intptr_t>(kZapValue + 12)));
    mov(value, Operand(bit_cast<intptr_t>(kZapValue + 16)));
  }
}

void MacroAssembler::RememberedSetHelper(Register object,  // For debug tests.
                                         Register address, Register scratch,
                                         SaveFPRegsMode fp_mode,
                                         RememberedSetFinalAction and_then) {
  Label done;
  if (emit_debug_code()) {
    Label ok;
    JumpIfNotInNewSpace(object, scratch, &ok);
    stop("Remembered set pointer is in new space");
    bind(&ok);
  }
  // Load store buffer top.
  ExternalReference store_buffer =
      ExternalReference::store_buffer_top(isolate());
  mov(ip, Operand(store_buffer));
  LoadP(scratch, MemOperand(ip));
  // Store pointer to buffer and increment buffer top.
  StoreP(address, MemOperand(scratch));
  AddP(scratch, Operand(kPointerSize));
  // Write back new top of buffer.
  StoreP(scratch, MemOperand(ip));
  // Call stub on end of buffer.
  // Check for end of buffer.
  AndP(scratch, Operand(StoreBuffer::kStoreBufferMask));

  if (and_then == kFallThroughAtEnd) {
    bne(&done, Label::kNear);
  } else {
    DCHECK(and_then == kReturnAtEnd);
    bne(&done, Label::kNear);
  }
  push(r14);
  StoreBufferOverflowStub store_buffer_overflow(isolate(), fp_mode);
  CallStub(&store_buffer_overflow);
  pop(r14);
  bind(&done);
  if (and_then == kReturnAtEnd) {
    Ret();
  }
}

void TurboAssembler::PushCommonFrame(Register marker_reg) {
  int fp_delta = 0;
  CleanseP(r14);
  if (marker_reg.is_valid()) {
    Push(r14, fp, marker_reg);
    fp_delta = 1;
  } else {
    Push(r14, fp);
    fp_delta = 0;
  }
  la(fp, MemOperand(sp, fp_delta * kPointerSize));
}

void TurboAssembler::PopCommonFrame(Register marker_reg) {
  if (marker_reg.is_valid()) {
    Pop(r14, fp, marker_reg);
  } else {
    Pop(r14, fp);
  }
}

void TurboAssembler::PushStandardFrame(Register function_reg) {
  int fp_delta = 0;
  CleanseP(r14);
  if (function_reg.is_valid()) {
    Push(r14, fp, cp, function_reg);
    fp_delta = 2;
  } else {
    Push(r14, fp, cp);
    fp_delta = 1;
  }
  la(fp, MemOperand(sp, fp_delta * kPointerSize));
}

void TurboAssembler::RestoreFrameStateForTailCall() {
  // if (FLAG_enable_embedded_constant_pool) {
  //   LoadP(kConstantPoolRegister,
  //         MemOperand(fp, StandardFrameConstants::kConstantPoolOffset));
  //   set_constant_pool_available(false);
  // }
  DCHECK(!FLAG_enable_embedded_constant_pool);
  LoadP(r14, MemOperand(fp, StandardFrameConstants::kCallerPCOffset));
  LoadP(fp, MemOperand(fp, StandardFrameConstants::kCallerFPOffset));
}

const RegList MacroAssembler::kSafepointSavedRegisters = Register::kAllocatable;
const int MacroAssembler::kNumSafepointSavedRegisters =
    Register::kNumAllocatable;

// Push and pop all registers that can hold pointers.
void MacroAssembler::PushSafepointRegisters() {
  // Safepoints expect a block of kNumSafepointRegisters values on the
  // stack, so adjust the stack for unsaved registers.
  const int num_unsaved = kNumSafepointRegisters - kNumSafepointSavedRegisters;
  DCHECK(num_unsaved >= 0);
  if (num_unsaved > 0) {
    lay(sp, MemOperand(sp, -(num_unsaved * kPointerSize)));
  }
  MultiPush(kSafepointSavedRegisters);
}

void MacroAssembler::PopSafepointRegisters() {
  const int num_unsaved = kNumSafepointRegisters - kNumSafepointSavedRegisters;
  MultiPop(kSafepointSavedRegisters);
  if (num_unsaved > 0) {
    la(sp, MemOperand(sp, num_unsaved * kPointerSize));
  }
}

int MacroAssembler::SafepointRegisterStackIndex(int reg_code) {
  // The registers are pushed starting with the highest encoding,
  // which means that lowest encodings are closest to the stack pointer.
  RegList regs = kSafepointSavedRegisters;
  int index = 0;

  DCHECK(reg_code >= 0 && reg_code < kNumRegisters);

  for (int16_t i = 0; i < reg_code; i++) {
    if ((regs & (1 << i)) != 0) {
      index++;
    }
  }

  return index;
}

void TurboAssembler::CanonicalizeNaN(const DoubleRegister dst,
                                     const DoubleRegister src) {
  // Turn potential sNaN into qNaN
  if (!dst.is(src)) ldr(dst, src);
  lzdr(kDoubleRegZero);
  sdbr(dst, kDoubleRegZero);
}

void TurboAssembler::ConvertIntToDouble(DoubleRegister dst, Register src) {
  cdfbr(dst, src);
}

void TurboAssembler::ConvertUnsignedIntToDouble(DoubleRegister dst,
                                                Register src) {
  if (CpuFeatures::IsSupported(FLOATING_POINT_EXT)) {
    cdlfbr(Condition(5), Condition(0), dst, src);
  } else {
    // zero-extend src
    llgfr(src, src);
    // convert to double
    cdgbr(dst, src);
  }
}

void TurboAssembler::ConvertIntToFloat(DoubleRegister dst, Register src) {
  cefbr(Condition(4), dst, src);
}

void TurboAssembler::ConvertUnsignedIntToFloat(DoubleRegister dst,
                                               Register src) {
  celfbr(Condition(4), Condition(0), dst, src);
}

void TurboAssembler::ConvertInt64ToFloat(DoubleRegister double_dst,
                                         Register src) {
  cegbr(double_dst, src);
}

void TurboAssembler::ConvertInt64ToDouble(DoubleRegister double_dst,
                                          Register src) {
  cdgbr(double_dst, src);
}

void TurboAssembler::ConvertUnsignedInt64ToFloat(DoubleRegister double_dst,
                                                 Register src) {
  celgbr(Condition(0), Condition(0), double_dst, src);
}

void TurboAssembler::ConvertUnsignedInt64ToDouble(DoubleRegister double_dst,
                                                  Register src) {
  cdlgbr(Condition(0), Condition(0), double_dst, src);
}

void TurboAssembler::ConvertFloat32ToInt64(const Register dst,
                                           const DoubleRegister double_input,
                                           FPRoundingMode rounding_mode) {
  Condition m = Condition(0);
  switch (rounding_mode) {
    case kRoundToZero:
      m = Condition(5);
      break;
    case kRoundToNearest:
      UNIMPLEMENTED();
      break;
    case kRoundToPlusInf:
      m = Condition(6);
      break;
    case kRoundToMinusInf:
      m = Condition(7);
      break;
    default:
      UNIMPLEMENTED();
      break;
  }
  cgebr(m, dst, double_input);
}

void TurboAssembler::ConvertDoubleToInt64(const Register dst,
                                          const DoubleRegister double_input,
                                          FPRoundingMode rounding_mode) {
  Condition m = Condition(0);
  switch (rounding_mode) {
    case kRoundToZero:
      m = Condition(5);
      break;
    case kRoundToNearest:
      UNIMPLEMENTED();
      break;
    case kRoundToPlusInf:
      m = Condition(6);
      break;
    case kRoundToMinusInf:
      m = Condition(7);
      break;
    default:
      UNIMPLEMENTED();
      break;
  }
  cgdbr(m, dst, double_input);
}

void TurboAssembler::ConvertDoubleToInt32(const Register dst,
                                          const DoubleRegister double_input,
                                          FPRoundingMode rounding_mode) {
  Condition m = Condition(0);
  switch (rounding_mode) {
    case kRoundToZero:
      m = Condition(5);
      break;
    case kRoundToNearest:
      m = Condition(4);
      break;
    case kRoundToPlusInf:
      m = Condition(6);
      break;
    case kRoundToMinusInf:
      m = Condition(7);
      break;
    default:
      UNIMPLEMENTED();
      break;
  }
  cfdbr(m, dst, double_input);
}

void TurboAssembler::ConvertFloat32ToInt32(const Register result,
                                           const DoubleRegister double_input,
                                           FPRoundingMode rounding_mode) {
  Condition m = Condition(0);
  switch (rounding_mode) {
    case kRoundToZero:
      m = Condition(5);
      break;
    case kRoundToNearest:
      m = Condition(4);
      break;
    case kRoundToPlusInf:
      m = Condition(6);
      break;
    case kRoundToMinusInf:
      m = Condition(7);
      break;
    default:
      UNIMPLEMENTED();
      break;
  }
  cfebr(m, result, double_input);
}

void TurboAssembler::ConvertFloat32ToUnsignedInt32(
    const Register result, const DoubleRegister double_input,
    FPRoundingMode rounding_mode) {
  Condition m = Condition(0);
  switch (rounding_mode) {
    case kRoundToZero:
      m = Condition(5);
      break;
    case kRoundToNearest:
      UNIMPLEMENTED();
      break;
    case kRoundToPlusInf:
      m = Condition(6);
      break;
    case kRoundToMinusInf:
      m = Condition(7);
      break;
    default:
      UNIMPLEMENTED();
      break;
  }
  clfebr(m, Condition(0), result, double_input);
}

void TurboAssembler::ConvertFloat32ToUnsignedInt64(
    const Register result, const DoubleRegister double_input,
    FPRoundingMode rounding_mode) {
  Condition m = Condition(0);
  switch (rounding_mode) {
    case kRoundToZero:
      m = Condition(5);
      break;
    case kRoundToNearest:
      UNIMPLEMENTED();
      break;
    case kRoundToPlusInf:
      m = Condition(6);
      break;
    case kRoundToMinusInf:
      m = Condition(7);
      break;
    default:
      UNIMPLEMENTED();
      break;
  }
  clgebr(m, Condition(0), result, double_input);
}

void TurboAssembler::ConvertDoubleToUnsignedInt64(
    const Register dst, const DoubleRegister double_input,
    FPRoundingMode rounding_mode) {
  Condition m = Condition(0);
  switch (rounding_mode) {
    case kRoundToZero:
      m = Condition(5);
      break;
    case kRoundToNearest:
      UNIMPLEMENTED();
      break;
    case kRoundToPlusInf:
      m = Condition(6);
      break;
    case kRoundToMinusInf:
      m = Condition(7);
      break;
    default:
      UNIMPLEMENTED();
      break;
  }
  clgdbr(m, Condition(0), dst, double_input);
}

void TurboAssembler::ConvertDoubleToUnsignedInt32(
    const Register dst, const DoubleRegister double_input,
    FPRoundingMode rounding_mode) {
  Condition m = Condition(0);
  switch (rounding_mode) {
    case kRoundToZero:
      m = Condition(5);
      break;
    case kRoundToNearest:
      UNIMPLEMENTED();
      break;
    case kRoundToPlusInf:
      m = Condition(6);
      break;
    case kRoundToMinusInf:
      m = Condition(7);
      break;
    default:
      UNIMPLEMENTED();
      break;
  }
  clfdbr(m, Condition(0), dst, double_input);
}

#if !V8_TARGET_ARCH_S390X
void TurboAssembler::ShiftLeftPair(Register dst_low, Register dst_high,
                                   Register src_low, Register src_high,
                                   Register scratch, Register shift) {
  LoadRR(r0, src_high);
  LoadRR(r1, src_low);
  sldl(r0, shift, Operand::Zero());
  LoadRR(dst_high, r0);
  LoadRR(dst_low, r1);
}

void TurboAssembler::ShiftLeftPair(Register dst_low, Register dst_high,
                                   Register src_low, Register src_high,
                                   uint32_t shift) {
  LoadRR(r0, src_high);
  LoadRR(r1, src_low);
  sldl(r0, r0, Operand(shift));
  LoadRR(dst_high, r0);
  LoadRR(dst_low, r1);
}

void TurboAssembler::ShiftRightPair(Register dst_low, Register dst_high,
                                    Register src_low, Register src_high,
                                    Register scratch, Register shift) {
  LoadRR(r0, src_high);
  LoadRR(r1, src_low);
  srdl(r0, shift, Operand::Zero());
  LoadRR(dst_high, r0);
  LoadRR(dst_low, r1);
}

void TurboAssembler::ShiftRightPair(Register dst_low, Register dst_high,
                                    Register src_low, Register src_high,
                                    uint32_t shift) {
  LoadRR(r0, src_high);
  LoadRR(r1, src_low);
  srdl(r0, r0, Operand(shift));
  LoadRR(dst_high, r0);
  LoadRR(dst_low, r1);
}

void TurboAssembler::ShiftRightArithPair(Register dst_low, Register dst_high,
                                         Register src_low, Register src_high,
                                         Register scratch, Register shift) {
  LoadRR(r0, src_high);
  LoadRR(r1, src_low);
  srda(r0, shift, Operand::Zero());
  LoadRR(dst_high, r0);
  LoadRR(dst_low, r1);
}

void TurboAssembler::ShiftRightArithPair(Register dst_low, Register dst_high,
                                         Register src_low, Register src_high,
                                         uint32_t shift) {
  LoadRR(r0, src_high);
  LoadRR(r1, src_low);
  srda(r0, r0, Operand(shift));
  LoadRR(dst_high, r0);
  LoadRR(dst_low, r1);
}
#endif

void TurboAssembler::MovDoubleToInt64(Register dst, DoubleRegister src) {
  lgdr(dst, src);
}

void TurboAssembler::MovInt64ToDouble(DoubleRegister dst, Register src) {
  ldgr(dst, src);
}

void TurboAssembler::StubPrologue(StackFrame::Type type, Register base,
                                  int prologue_offset) {
  {
    ConstantPoolUnavailableScope constant_pool_unavailable(this);
    Load(r1, Operand(StackFrame::TypeToMarker(type)));
    PushCommonFrame(r1);
  }
}

void TurboAssembler::Prologue(Register base, int prologue_offset) {
  DCHECK(!base.is(no_reg));
  PushStandardFrame(r3);
}

void TurboAssembler::EnterFrame(StackFrame::Type type,
                                bool load_constant_pool_pointer_reg) {
  // We create a stack frame with:
  //    Return Addr <-- old sp
  //    Old FP      <-- new fp
  //    CP
  //    type
  //    CodeObject  <-- new sp

  Load(ip, Operand(StackFrame::TypeToMarker(type)));
  PushCommonFrame(ip);

  if (type == StackFrame::INTERNAL) {
    mov(r0, Operand(CodeObject()));
    push(r0);
  }
}

int TurboAssembler::LeaveFrame(StackFrame::Type type, int stack_adjustment) {
  // Drop the execution stack down to the frame pointer and restore
  // the caller frame pointer, return address and constant pool pointer.
  LoadP(r14, MemOperand(fp, StandardFrameConstants::kCallerPCOffset));
  if (is_int20(StandardFrameConstants::kCallerSPOffset + stack_adjustment)) {
    lay(r1, MemOperand(fp, StandardFrameConstants::kCallerSPOffset +
                               stack_adjustment));
  } else {
    AddP(r1, fp,
         Operand(StandardFrameConstants::kCallerSPOffset + stack_adjustment));
  }
  LoadP(fp, MemOperand(fp, StandardFrameConstants::kCallerFPOffset));
  LoadRR(sp, r1);
  int frame_ends = pc_offset();
  return frame_ends;
}

void MacroAssembler::EnterBuiltinFrame(Register context, Register target,
                                       Register argc) {
  CleanseP(r14);
  Push(r14, fp, context, target);
  la(fp, MemOperand(sp, 2 * kPointerSize));
  Push(argc);
}

void MacroAssembler::LeaveBuiltinFrame(Register context, Register target,
                                       Register argc) {
  Pop(argc);
  Pop(r14, fp, context, target);
}

// ExitFrame layout (probably wrongish.. needs updating)
//
//  SP -> previousSP
//        LK reserved
//        code
//        sp_on_exit (for debug?)
// oldSP->prev SP
//        LK
//        <parameters on stack>

// Prior to calling EnterExitFrame, we've got a bunch of parameters
// on the stack that we need to wrap a real frame around.. so first
// we reserve a slot for LK and push the previous SP which is captured
// in the fp register (r11)
// Then - we buy a new frame

// r14
// oldFP <- newFP
// SP
// Code
// Floats
// gaps
// Args
// ABIRes <- newSP
void MacroAssembler::EnterExitFrame(bool save_doubles, int stack_space,
                                    StackFrame::Type frame_type) {
  DCHECK(frame_type == StackFrame::EXIT ||
         frame_type == StackFrame::BUILTIN_EXIT);
  // Set up the frame structure on the stack.
  DCHECK_EQ(2 * kPointerSize, ExitFrameConstants::kCallerSPDisplacement);
  DCHECK_EQ(1 * kPointerSize, ExitFrameConstants::kCallerPCOffset);
  DCHECK_EQ(0 * kPointerSize, ExitFrameConstants::kCallerFPOffset);
  DCHECK(stack_space > 0);

  // This is an opportunity to build a frame to wrap
  // all of the pushes that have happened inside of V8
  // since we were called from C code
  CleanseP(r14);
  Load(r1, Operand(StackFrame::TypeToMarker(frame_type)));
  PushCommonFrame(r1);
  // Reserve room for saved entry sp and code object.
  lay(sp, MemOperand(fp, -ExitFrameConstants::kFixedFrameSizeFromFp));

  if (emit_debug_code()) {
    StoreP(MemOperand(fp, ExitFrameConstants::kSPOffset), Operand::Zero(), r1);
  }
  mov(r1, Operand(CodeObject()));
  StoreP(r1, MemOperand(fp, ExitFrameConstants::kCodeOffset));

  // Save the frame pointer and the context in top.
  mov(r1, Operand(ExternalReference(IsolateAddressId::kCEntryFPAddress,
                                    isolate())));
  StoreP(fp, MemOperand(r1));
  mov(r1,
      Operand(ExternalReference(IsolateAddressId::kContextAddress, isolate())));
  StoreP(cp, MemOperand(r1));

  // Optionally save all volatile double registers.
  if (save_doubles) {
    MultiPushDoubles(kCallerSavedDoubles);
    // Note that d0 will be accessible at
    //   fp - ExitFrameConstants::kFrameSize -
    //   kNumCallerSavedDoubles * kDoubleSize,
    // since the sp slot and code slot were pushed after the fp.
  }

  lay(sp, MemOperand(sp, -stack_space * kPointerSize));

  // Allocate and align the frame preparing for calling the runtime
  // function.
  const int frame_alignment = TurboAssembler::ActivationFrameAlignment();
  if (frame_alignment > 0) {
    DCHECK(frame_alignment == 8);
    ClearRightImm(sp, sp, Operand(3));  // equivalent to &= -8
  }

  lay(sp, MemOperand(sp, -kNumRequiredStackFrameSlots * kPointerSize));
  StoreP(MemOperand(sp), Operand::Zero(), r0);
  // Set the exit frame sp value to point just before the return address
  // location.
  lay(r1, MemOperand(sp, kStackFrameSPSlot * kPointerSize));
  StoreP(r1, MemOperand(fp, ExitFrameConstants::kSPOffset));
}

int TurboAssembler::ActivationFrameAlignment() {
#if !defined(USE_SIMULATOR)
  // Running on the real platform. Use the alignment as mandated by the local
  // environment.
  // Note: This will break if we ever start generating snapshots on one S390
  // platform for another S390 platform with a different alignment.
  return base::OS::ActivationFrameAlignment();
#else  // Simulated
  // If we are using the simulator then we should always align to the expected
  // alignment. As the simulator is used to generate snapshots we do not know
  // if the target platform will need alignment, so this is controlled from a
  // flag.
  return FLAG_sim_stack_alignment;
#endif
}

void MacroAssembler::LeaveExitFrame(bool save_doubles, Register argument_count,
                                    bool restore_context,
                                    bool argument_count_is_length) {
  // Optionally restore all double registers.
  if (save_doubles) {
    // Calculate the stack location of the saved doubles and restore them.
    const int kNumRegs = kNumCallerSavedDoubles;
    lay(r5, MemOperand(fp, -(ExitFrameConstants::kFixedFrameSizeFromFp +
                             kNumRegs * kDoubleSize)));
    MultiPopDoubles(kCallerSavedDoubles, r5);
  }

  // Clear top frame.
  mov(ip, Operand(ExternalReference(IsolateAddressId::kCEntryFPAddress,
                                    isolate())));
  StoreP(MemOperand(ip), Operand(0, kRelocInfo_NONEPTR), r0);

  // Restore current context from top and clear it in debug mode.
  if (restore_context) {
    mov(ip, Operand(ExternalReference(IsolateAddressId::kContextAddress,
                                      isolate())));
    LoadP(cp, MemOperand(ip));
  }
#ifdef DEBUG
  mov(ip,
      Operand(ExternalReference(IsolateAddressId::kContextAddress, isolate())));
  StoreP(MemOperand(ip), Operand(0, kRelocInfo_NONEPTR), r0);
#endif

  // Tear down the exit frame, pop the arguments, and return.
  LeaveFrame(StackFrame::EXIT);

  if (argument_count.is_valid()) {
    if (!argument_count_is_length) {
      ShiftLeftP(argument_count, argument_count, Operand(kPointerSizeLog2));
    }
    la(sp, MemOperand(sp, argument_count));
  }
}

void TurboAssembler::MovFromFloatResult(const DoubleRegister dst) {
  Move(dst, d0);
}

void TurboAssembler::MovFromFloatParameter(const DoubleRegister dst) {
  Move(dst, d0);
}

void TurboAssembler::PrepareForTailCall(const ParameterCount& callee_args_count,
                                        Register caller_args_count_reg,
                                        Register scratch0, Register scratch1) {
#if DEBUG
  if (callee_args_count.is_reg()) {
    DCHECK(!AreAliased(callee_args_count.reg(), caller_args_count_reg, scratch0,
                       scratch1));
  } else {
    DCHECK(!AreAliased(caller_args_count_reg, scratch0, scratch1));
  }
#endif

  // Calculate the end of destination area where we will put the arguments
  // after we drop current frame. We AddP kPointerSize to count the receiver
  // argument which is not included into formal parameters count.
  Register dst_reg = scratch0;
  ShiftLeftP(dst_reg, caller_args_count_reg, Operand(kPointerSizeLog2));
  AddP(dst_reg, fp, dst_reg);
  AddP(dst_reg, dst_reg,
       Operand(StandardFrameConstants::kCallerSPOffset + kPointerSize));

  Register src_reg = caller_args_count_reg;
  // Calculate the end of source area. +kPointerSize is for the receiver.
  if (callee_args_count.is_reg()) {
    ShiftLeftP(src_reg, callee_args_count.reg(), Operand(kPointerSizeLog2));
    AddP(src_reg, sp, src_reg);
    AddP(src_reg, src_reg, Operand(kPointerSize));
  } else {
    mov(src_reg, Operand((callee_args_count.immediate() + 1) * kPointerSize));
    AddP(src_reg, src_reg, sp);
  }

  if (FLAG_debug_code) {
    CmpLogicalP(src_reg, dst_reg);
    Check(lt, kStackAccessBelowStackPointer);
  }

  // Restore caller's frame pointer and return address now as they will be
  // overwritten by the copying loop.
  RestoreFrameStateForTailCall();

  // Now copy callee arguments to the caller frame going backwards to avoid
  // callee arguments corruption (source and destination areas could overlap).

  // Both src_reg and dst_reg are pointing to the word after the one to copy,
  // so they must be pre-decremented in the loop.
  Register tmp_reg = scratch1;
  Label loop;
  if (callee_args_count.is_reg()) {
    AddP(tmp_reg, callee_args_count.reg(), Operand(1));  // +1 for receiver
  } else {
    mov(tmp_reg, Operand(callee_args_count.immediate() + 1));
  }
  LoadRR(r1, tmp_reg);
  bind(&loop);
  LoadP(tmp_reg, MemOperand(src_reg, -kPointerSize));
  StoreP(tmp_reg, MemOperand(dst_reg, -kPointerSize));
  lay(src_reg, MemOperand(src_reg, -kPointerSize));
  lay(dst_reg, MemOperand(dst_reg, -kPointerSize));
  BranchOnCount(r1, &loop);

  // Leave current frame.
  LoadRR(sp, dst_reg);
}

void MacroAssembler::InvokePrologue(const ParameterCount& expected,
                                    const ParameterCount& actual, Label* done,
                                    bool* definitely_mismatches,
                                    InvokeFlag flag) {
  bool definitely_matches = false;
  *definitely_mismatches = false;
  Label regular_invoke;

  // Check whether the expected and actual arguments count match. If not,
  // setup registers according to contract with ArgumentsAdaptorTrampoline:
  //  r2: actual arguments count
  //  r3: function (passed through to callee)
  //  r4: expected arguments count

  // The code below is made a lot easier because the calling code already sets
  // up actual and expected registers according to the contract if values are
  // passed in registers.

  // ARM has some sanity checks as per below, considering add them for S390
  //  DCHECK(actual.is_immediate() || actual.reg().is(r2));
  //  DCHECK(expected.is_immediate() || expected.reg().is(r4));

  if (expected.is_immediate()) {
    DCHECK(actual.is_immediate());
    mov(r2, Operand(actual.immediate()));
    if (expected.immediate() == actual.immediate()) {
      definitely_matches = true;
    } else {
      const int sentinel = SharedFunctionInfo::kDontAdaptArgumentsSentinel;
      if (expected.immediate() == sentinel) {
        // Don't worry about adapting arguments for builtins that
        // don't want that done. Skip adaption code by making it look
        // like we have a match between expected and actual number of
        // arguments.
        definitely_matches = true;
      } else {
        *definitely_mismatches = true;
        mov(r4, Operand(expected.immediate()));
      }
    }
  } else {
    if (actual.is_immediate()) {
      mov(r2, Operand(actual.immediate()));
      CmpPH(expected.reg(), Operand(actual.immediate()));
      beq(&regular_invoke);
    } else {
      CmpP(expected.reg(), actual.reg());
      beq(&regular_invoke);
    }
  }

  if (!definitely_matches) {
    Handle<Code> adaptor = BUILTIN_CODE(isolate(), ArgumentsAdaptorTrampoline);
    if (flag == CALL_FUNCTION) {
      Call(adaptor);
      if (!*definitely_mismatches) {
        b(done);
      }
    } else {
      Jump(adaptor, RelocInfo::CODE_TARGET);
    }
    bind(&regular_invoke);
  }
}

void MacroAssembler::CheckDebugHook(Register fun, Register new_target,
                                    const ParameterCount& expected,
                                    const ParameterCount& actual) {
  Label skip_hook;
  ExternalReference debug_hook_avtive =
      ExternalReference::debug_hook_on_function_call_address(isolate());
  mov(r6, Operand(debug_hook_avtive));
  LoadB(r6, MemOperand(r6));
  CmpP(r6, Operand::Zero());
  beq(&skip_hook);
  {
    FrameScope frame(this,
                     has_frame() ? StackFrame::NONE : StackFrame::INTERNAL);
    if (expected.is_reg()) {
      SmiTag(expected.reg());
      Push(expected.reg());
    }
    if (actual.is_reg()) {
      SmiTag(actual.reg());
      Push(actual.reg());
    }
    if (new_target.is_valid()) {
      Push(new_target);
    }
    Push(fun, fun);
    CallRuntime(Runtime::kDebugOnFunctionCall);
    Pop(fun);
    if (new_target.is_valid()) {
      Pop(new_target);
    }
    if (actual.is_reg()) {
      Pop(actual.reg());
      SmiUntag(actual.reg());
    }
    if (expected.is_reg()) {
      Pop(expected.reg());
      SmiUntag(expected.reg());
    }
  }
  bind(&skip_hook);
}

void MacroAssembler::InvokeFunctionCode(Register function, Register new_target,
                                        const ParameterCount& expected,
                                        const ParameterCount& actual,
                                        InvokeFlag flag) {
  // You can't call a function without a valid frame.
  DCHECK(flag == JUMP_FUNCTION || has_frame());

  DCHECK(function.is(r3));
  DCHECK_IMPLIES(new_target.is_valid(), new_target.is(r5));

  // On function call, call into the debugger if necessary.
  CheckDebugHook(function, new_target, expected, actual);

  // Clear the new.target register if not given.
  if (!new_target.is_valid()) {
    LoadRoot(r5, Heap::kUndefinedValueRootIndex);
  }

  Label done;
  bool definitely_mismatches = false;
  InvokePrologue(expected, actual, &done, &definitely_mismatches, flag);
  if (!definitely_mismatches) {
    // We call indirectly through the code field in the function to
    // allow recompilation to take effect without changing any of the
    // call sites.
    Register code = ip;
    LoadP(code, FieldMemOperand(function, JSFunction::kCodeOffset));
    AddP(code, code, Operand(Code::kHeaderSize - kHeapObjectTag));
    if (flag == CALL_FUNCTION) {
      CallJSEntry(code);
    } else {
      DCHECK(flag == JUMP_FUNCTION);
      JumpToJSEntry(code);
    }

    // Continue here if InvokePrologue does handle the invocation due to
    // mismatched parameter counts.
    bind(&done);
  }
}

void MacroAssembler::InvokeFunction(Register fun, Register new_target,
                                    const ParameterCount& actual,
                                    InvokeFlag flag) {
  // You can't call a function without a valid frame.
  DCHECK(flag == JUMP_FUNCTION || has_frame());

  // Contract with called JS functions requires that function is passed in r3.
  DCHECK(fun.is(r3));

  Register expected_reg = r4;
  Register temp_reg = r6;
  LoadP(temp_reg, FieldMemOperand(r3, JSFunction::kSharedFunctionInfoOffset));
  LoadP(cp, FieldMemOperand(r3, JSFunction::kContextOffset));
  LoadW(expected_reg,
        FieldMemOperand(temp_reg,
                        SharedFunctionInfo::kFormalParameterCountOffset));

  ParameterCount expected(expected_reg);
  InvokeFunctionCode(fun, new_target, expected, actual, flag);
}

void MacroAssembler::InvokeFunction(Register function,
                                    const ParameterCount& expected,
                                    const ParameterCount& actual,
                                    InvokeFlag flag) {
  // You can't call a function without a valid frame.
  DCHECK(flag == JUMP_FUNCTION || has_frame());

  // Contract with called JS functions requires that function is passed in r3.
  DCHECK(function.is(r3));

  // Get the function and setup the context.
  LoadP(cp, FieldMemOperand(r3, JSFunction::kContextOffset));

  InvokeFunctionCode(r3, no_reg, expected, actual, flag);
}

void MacroAssembler::InvokeFunction(Handle<JSFunction> function,
                                    const ParameterCount& expected,
                                    const ParameterCount& actual,
                                    InvokeFlag flag) {
  Move(r3, function);
  InvokeFunction(r3, expected, actual, flag);
}

void MacroAssembler::MaybeDropFrames() {
  // Check whether we need to drop frames to restart a function on the stack.
  ExternalReference restart_fp =
      ExternalReference::debug_restart_fp_address(isolate());
  mov(r3, Operand(restart_fp));
  LoadP(r3, MemOperand(r3));
  CmpP(r3, Operand::Zero());
  Jump(BUILTIN_CODE(isolate(), FrameDropperTrampoline), RelocInfo::CODE_TARGET,
       ne);
}

void MacroAssembler::PushStackHandler() {
  // Adjust this code if not the case.
  STATIC_ASSERT(StackHandlerConstants::kSize == 1 * kPointerSize);
  STATIC_ASSERT(StackHandlerConstants::kNextOffset == 0 * kPointerSize);

  // Link the current handler as the next handler.
  mov(r7,
      Operand(ExternalReference(IsolateAddressId::kHandlerAddress, isolate())));

  // Buy the full stack frame for 5 slots.
  lay(sp, MemOperand(sp, -StackHandlerConstants::kSize));

  // Copy the old handler into the next handler slot.
  mvc(MemOperand(sp, StackHandlerConstants::kNextOffset), MemOperand(r7),
      kPointerSize);
  // Set this new handler as the current one.
  StoreP(sp, MemOperand(r7));
}

void MacroAssembler::PopStackHandler() {
  STATIC_ASSERT(StackHandlerConstants::kSize == 1 * kPointerSize);
  STATIC_ASSERT(StackHandlerConstants::kNextOffset == 0);

  // Pop the Next Handler into r3 and store it into Handler Address reference.
  Pop(r3);
  mov(ip,
      Operand(ExternalReference(IsolateAddressId::kHandlerAddress, isolate())));

  StoreP(r3, MemOperand(ip));
}

void MacroAssembler::Allocate(int object_size, Register result,
                              Register scratch1, Register scratch2,
                              Label* gc_required, AllocationFlags flags) {
  DCHECK(object_size <= kMaxRegularHeapObjectSize);
  if (!FLAG_inline_new) {
    if (emit_debug_code()) {
      // Trash the registers to simulate an allocation failure.
      LoadImmP(result, Operand(0x7091));
      LoadImmP(scratch1, Operand(0x7191));
      LoadImmP(scratch2, Operand(0x7291));
    }
    b(gc_required);
    return;
  }

  DCHECK(!AreAliased(result, scratch1, scratch2, ip));

  // Make object size into bytes.
  if ((flags & SIZE_IN_WORDS) != 0) {
    object_size *= kPointerSize;
  }
  DCHECK_EQ(0, static_cast<int>(object_size & kObjectAlignmentMask));

  // Check relative positions of allocation top and limit addresses.
  ExternalReference allocation_top =
      AllocationUtils::GetAllocationTopReference(isolate(), flags);
  ExternalReference allocation_limit =
      AllocationUtils::GetAllocationLimitReference(isolate(), flags);

  intptr_t top = reinterpret_cast<intptr_t>(allocation_top.address());
  intptr_t limit = reinterpret_cast<intptr_t>(allocation_limit.address());
  DCHECK((limit - top) == kPointerSize);

  // Set up allocation top address register.
  Register top_address = scratch1;
  Register result_end = scratch2;
  mov(top_address, Operand(allocation_top));

  if ((flags & RESULT_CONTAINS_TOP) == 0) {
    // Load allocation top into result and allocation limit into ip.
    LoadP(result, MemOperand(top_address));
  } else {
    if (emit_debug_code()) {
      // Assert that result actually contains top on entry.
      CmpP(result, MemOperand(top_address));
      Check(eq, kUnexpectedAllocationTop);
    }
  }

  if ((flags & DOUBLE_ALIGNMENT) != 0) {
// Align the next allocation. Storing the filler map without checking top is
// safe in new-space because the limit of the heap is aligned there.
#if V8_TARGET_ARCH_S390X
    STATIC_ASSERT(kPointerAlignment == kDoubleAlignment);
#else
    STATIC_ASSERT(kPointerAlignment * 2 == kDoubleAlignment);
    AndP(result_end, result, Operand(kDoubleAlignmentMask));
    Label aligned;
    beq(&aligned, Label::kNear);
    if ((flags & PRETENURE) != 0) {
      CmpLogicalP(result, MemOperand(top_address, limit - top));
      bge(gc_required);
    }
    mov(result_end, Operand(isolate()->factory()->one_pointer_filler_map()));
    StoreW(result_end, MemOperand(result));
    AddP(result, result, Operand(kDoubleSize / 2));
    bind(&aligned);
#endif
  }

  AddP(result_end, result, Operand(object_size));

  // Compare with allocation limit.
  CmpLogicalP(result_end, MemOperand(top_address, limit - top));
  bge(gc_required);

  StoreP(result_end, MemOperand(top_address));

  if (CpuFeatures::IsSupported(GENERAL_INSTR_EXT)) {
    // Prefetch the allocation_top's next cache line in advance to
    // help alleviate potential cache misses.
    // Mode 2 - Prefetch the data into a cache line for store access.
    pfd(static_cast<Condition>(2), MemOperand(result, 256));
  }

  // Tag object.
  la(result, MemOperand(result, kHeapObjectTag));
}

void MacroAssembler::CompareObjectType(Register object, Register map,
                                       Register type_reg, InstanceType type) {
  const Register temp = type_reg.is(no_reg) ? r0 : type_reg;

  LoadP(map, FieldMemOperand(object, HeapObject::kMapOffset));
  CompareInstanceType(map, temp, type);
}

void MacroAssembler::CompareInstanceType(Register map, Register type_reg,
                                         InstanceType type) {
  STATIC_ASSERT(Map::kInstanceTypeOffset < 4096);
  STATIC_ASSERT(LAST_TYPE < 256);
  LoadlB(type_reg, FieldMemOperand(map, Map::kInstanceTypeOffset));
  CmpP(type_reg, Operand(type));
}

void MacroAssembler::CompareRoot(Register obj, Heap::RootListIndex index) {
  CmpP(obj, MemOperand(kRootRegister, index << kPointerSizeLog2));
}

void MacroAssembler::SmiToDouble(DoubleRegister value, Register smi) {
  SmiUntag(ip, smi);
  ConvertIntToDouble(value, ip);
}

void MacroAssembler::CompareMap(Register obj, Register scratch, Handle<Map> map,
                                Label* early_success) {
  LoadP(scratch, FieldMemOperand(obj, HeapObject::kMapOffset));
  CompareMap(obj, map, early_success);
}

void MacroAssembler::CompareMap(Register obj_map, Handle<Map> map,
                                Label* early_success) {
  mov(r0, Operand(map));
  CmpP(r0, FieldMemOperand(obj_map, HeapObject::kMapOffset));
}

void MacroAssembler::CheckMap(Register obj, Register scratch, Handle<Map> map,
                              Label* fail, SmiCheckType smi_check_type) {
  if (smi_check_type == DO_SMI_CHECK) {
    JumpIfSmi(obj, fail);
  }

  Label success;
  CompareMap(obj, scratch, map, &success);
  bne(fail);
  bind(&success);
}

void MacroAssembler::CheckMap(Register obj, Register scratch,
                              Heap::RootListIndex index, Label* fail,
                              SmiCheckType smi_check_type) {
  if (smi_check_type == DO_SMI_CHECK) {
    JumpIfSmi(obj, fail);
  }
  LoadP(scratch, FieldMemOperand(obj, HeapObject::kMapOffset));
  CompareRoot(scratch, index);
  bne(fail);
}

void MacroAssembler::GetWeakValue(Register value, Handle<WeakCell> cell) {
  mov(value, Operand(cell));
  LoadP(value, FieldMemOperand(value, WeakCell::kValueOffset));
}

void MacroAssembler::LoadWeakValue(Register value, Handle<WeakCell> cell,
                                   Label* miss) {
  GetWeakValue(value, cell);
  JumpIfSmi(value, miss);
}

void MacroAssembler::GetMapConstructor(Register result, Register map,
                                       Register temp, Register temp2) {
  Label done, loop;
  LoadP(result, FieldMemOperand(map, Map::kConstructorOrBackPointerOffset));
  bind(&loop);
  JumpIfSmi(result, &done);
  CompareObjectType(result, temp, temp2, MAP_TYPE);
  bne(&done);
  LoadP(result, FieldMemOperand(result, Map::kConstructorOrBackPointerOffset));
  b(&loop);
  bind(&done);
}

void MacroAssembler::CallStub(CodeStub* stub, Condition cond) {
  DCHECK(AllowThisStubCall(stub));  // Stub calls are not allowed in some stubs.
  Call(stub->GetCode(), RelocInfo::CODE_TARGET, cond);
}

void TurboAssembler::CallStubDelayed(CodeStub* stub) {
  DCHECK(AllowThisStubCall(stub));  // Stub calls are not allowed in some stubs.
  call(stub);
}

void MacroAssembler::TailCallStub(CodeStub* stub, Condition cond) {
  Jump(stub->GetCode(), RelocInfo::CODE_TARGET, cond);
}

bool TurboAssembler::AllowThisStubCall(CodeStub* stub) {
  return has_frame_ || !stub->SometimesSetsUpAFrame();
}

void MacroAssembler::TryDoubleToInt32Exact(Register result,
                                           DoubleRegister double_input,
                                           Register scratch,
                                           DoubleRegister double_scratch) {
  Label done;
  DCHECK(!double_input.is(double_scratch));

  ConvertDoubleToInt64(result, double_input);

  TestIfInt32(result);
  bne(&done);

  // convert back and compare
  cdfbr(double_scratch, result);
  cdbr(double_scratch, double_input);
  bind(&done);
}

void TurboAssembler::TruncateDoubleToIDelayed(Zone* zone, Register result,
                                              DoubleRegister double_input) {
  Label done;

  TryInlineTruncateDoubleToI(result, double_input, &done);

  // If we fell through then inline version didn't succeed - call stub instead.
  push(r14);
  // Put input on stack.
  lay(sp, MemOperand(sp, -kDoubleSize));
  StoreDouble(double_input, MemOperand(sp));

  CallStubDelayed(new (zone) DoubleToIStub(nullptr, sp, result, 0, true, true));

  la(sp, MemOperand(sp, kDoubleSize));
  pop(r14);

  bind(&done);
}

void TurboAssembler::TryInlineTruncateDoubleToI(Register result,
                                                DoubleRegister double_input,
                                                Label* done) {
  ConvertDoubleToInt64(result, double_input);

  // Test for overflow
  TestIfInt32(result);
  beq(done);
}

void TurboAssembler::CallRuntimeDelayed(Zone* zone, Runtime::FunctionId fid,
                                        SaveFPRegsMode save_doubles) {
  const Runtime::Function* f = Runtime::FunctionForId(fid);
  mov(r2, Operand(f->nargs));
  mov(r3, Operand(ExternalReference(f, isolate())));
  CallStubDelayed(new (zone) CEntryStub(nullptr,
#if V8_TARGET_ARCH_S390X
                                        f->result_size,
#else
                                        1,
#endif
                                        save_doubles));
}

void MacroAssembler::CallRuntime(const Runtime::Function* f, int num_arguments,
                                 SaveFPRegsMode save_doubles) {
  // All parameters are on the stack.  r2 has the return value after call.

  // If the expected number of arguments of the runtime function is
  // constant, we check that the actual number of arguments match the
  // expectation.
  CHECK(f->nargs < 0 || f->nargs == num_arguments);

  // TODO(1236192): Most runtime routines don't need the number of
  // arguments passed in because it is constant. At some point we
  // should remove this need and make the runtime routine entry code
  // smarter.
  mov(r2, Operand(num_arguments));
  mov(r3, Operand(ExternalReference(f, isolate())));
  CEntryStub stub(isolate(),
#if V8_TARGET_ARCH_S390X
                  f->result_size,
#else
                  1,
#endif
                  save_doubles);
  CallStub(&stub);
}

void MacroAssembler::TailCallRuntime(Runtime::FunctionId fid) {
  const Runtime::Function* function = Runtime::FunctionForId(fid);
  DCHECK_EQ(1, function->result_size);
  if (function->nargs >= 0) {
    mov(r2, Operand(function->nargs));
  }
  JumpToExternalReference(ExternalReference(fid, isolate()));
}

void MacroAssembler::JumpToExternalReference(const ExternalReference& builtin,
                                             bool builtin_exit_frame) {
  mov(r3, Operand(builtin));
  CEntryStub stub(isolate(), 1, kDontSaveFPRegs, kArgvOnStack,
                  builtin_exit_frame);
  Jump(stub.GetCode(), RelocInfo::CODE_TARGET);
}

void MacroAssembler::SetCounter(StatsCounter* counter, int value,
                                Register scratch1, Register scratch2) {
  if (FLAG_native_code_counters && counter->Enabled()) {
    mov(scratch1, Operand(value));
    mov(scratch2, Operand(ExternalReference(counter)));
    StoreW(scratch1, MemOperand(scratch2));
  }
}

void MacroAssembler::IncrementCounter(StatsCounter* counter, int value,
                                      Register scratch1, Register scratch2) {
  DCHECK(value > 0 && is_int8(value));
  if (FLAG_native_code_counters && counter->Enabled()) {
    mov(scratch1, Operand(ExternalReference(counter)));
    // @TODO(john.yan): can be optimized by asi()
    LoadW(scratch2, MemOperand(scratch1));
    AddP(scratch2, Operand(value));
    StoreW(scratch2, MemOperand(scratch1));
  }
}

void MacroAssembler::DecrementCounter(StatsCounter* counter, int value,
                                      Register scratch1, Register scratch2) {
  DCHECK(value > 0 && is_int8(value));
  if (FLAG_native_code_counters && counter->Enabled()) {
    mov(scratch1, Operand(ExternalReference(counter)));
    // @TODO(john.yan): can be optimized by asi()
    LoadW(scratch2, MemOperand(scratch1));
    AddP(scratch2, Operand(-value));
    StoreW(scratch2, MemOperand(scratch1));
  }
}

void TurboAssembler::Assert(Condition cond, BailoutReason reason,
                            CRegister cr) {
  if (emit_debug_code()) Check(cond, reason, cr);
}

void TurboAssembler::Check(Condition cond, BailoutReason reason, CRegister cr) {
  Label L;
  b(cond, &L);
  Abort(reason);
  // will not return here
  bind(&L);
}

void TurboAssembler::Abort(BailoutReason reason) {
  Label abort_start;
  bind(&abort_start);
#ifdef DEBUG
  const char* msg = GetBailoutReason(reason);
  if (msg != NULL) {
    RecordComment("Abort message: ");
    RecordComment(msg);
  }

  if (FLAG_trap_on_abort) {
    stop(msg);
    return;
  }
#endif

  LoadSmiLiteral(r3, Smi::FromInt(static_cast<int>(reason)));

  // Disable stub call restrictions to always allow calls to abort.
  if (!has_frame_) {
    // We don't actually want to generate a pile of code for this, so just
    // claim there is a stack frame, without generating one.
    FrameScope scope(this, StackFrame::NONE);
    Call(BUILTIN_CODE(isolate(), Abort), RelocInfo::CODE_TARGET);
  } else {
    Call(BUILTIN_CODE(isolate(), Abort), RelocInfo::CODE_TARGET);
  }
  // will not return here
}

void MacroAssembler::LoadNativeContextSlot(int index, Register dst) {
  LoadP(dst, NativeContextMemOperand());
  LoadP(dst, ContextMemOperand(dst, index));
}

void MacroAssembler::LoadGlobalFunctionInitialMap(Register function,
                                                  Register map,
                                                  Register scratch) {
  // Load the initial map. The global functions all have initial maps.
  LoadP(map,
        FieldMemOperand(function, JSFunction::kPrototypeOrInitialMapOffset));
  if (emit_debug_code()) {
    Label ok, fail;
    CheckMap(map, scratch, Heap::kMetaMapRootIndex, &fail, DO_SMI_CHECK);
    b(&ok);
    bind(&fail);
    Abort(kGlobalFunctionsMustHaveInitialMap);
    bind(&ok);
  }
}

void MacroAssembler::UntagAndJumpIfSmi(Register dst, Register src,
                                       Label* smi_case) {
  STATIC_ASSERT(kSmiTag == 0);
  STATIC_ASSERT(kSmiTagSize == 1);
  // this won't work if src == dst
  DCHECK(src.code() != dst.code());
  SmiUntag(dst, src);
  TestIfSmi(src);
  beq(smi_case);
}

void MacroAssembler::JumpIfEitherSmi(Register reg1, Register reg2,
                                     Label* on_either_smi) {
  STATIC_ASSERT(kSmiTag == 0);
  JumpIfSmi(reg1, on_either_smi);
  JumpIfSmi(reg2, on_either_smi);
}

void MacroAssembler::AssertNotSmi(Register object) {
  if (emit_debug_code()) {
    STATIC_ASSERT(kSmiTag == 0);
    TestIfSmi(object);
    Check(ne, kOperandIsASmi, cr0);
  }
}

void MacroAssembler::AssertSmi(Register object) {
  if (emit_debug_code()) {
    STATIC_ASSERT(kSmiTag == 0);
    TestIfSmi(object);
    Check(eq, kOperandIsNotSmi, cr0);
  }
}

void MacroAssembler::AssertFixedArray(Register object) {
  if (emit_debug_code()) {
    STATIC_ASSERT(kSmiTag == 0);
    TestIfSmi(object);
    Check(ne, kOperandIsASmiAndNotAFixedArray, cr0);
    push(object);
    CompareObjectType(object, object, object, FIXED_ARRAY_TYPE);
    pop(object);
    Check(eq, kOperandIsNotAFixedArray);
  }
}

void MacroAssembler::AssertFunction(Register object) {
  if (emit_debug_code()) {
    STATIC_ASSERT(kSmiTag == 0);
    TestIfSmi(object);
    Check(ne, kOperandIsASmiAndNotAFunction, cr0);
    push(object);
    CompareObjectType(object, object, object, JS_FUNCTION_TYPE);
    pop(object);
    Check(eq, kOperandIsNotAFunction);
  }
}

void MacroAssembler::AssertBoundFunction(Register object) {
  if (emit_debug_code()) {
    STATIC_ASSERT(kSmiTag == 0);
    TestIfSmi(object);
    Check(ne, kOperandIsASmiAndNotABoundFunction, cr0);
    push(object);
    CompareObjectType(object, object, object, JS_BOUND_FUNCTION_TYPE);
    pop(object);
    Check(eq, kOperandIsNotABoundFunction);
  }
}

void MacroAssembler::AssertGeneratorObject(Register object) {
  if (!emit_debug_code()) return;
  TestIfSmi(object);
  Check(ne, kOperandIsASmiAndNotAGeneratorObject, cr0);

  // Load map
  Register map = object;
  push(object);
  LoadP(map, FieldMemOperand(object, HeapObject::kMapOffset));

  // Check if JSGeneratorObject
  Label do_check;
  Register instance_type = object;
  CompareInstanceType(map, instance_type, JS_GENERATOR_OBJECT_TYPE);
  beq(&do_check);

  // Check if JSAsyncGeneratorObject (See MacroAssembler::CompareInstanceType)
  CmpP(instance_type, Operand(JS_ASYNC_GENERATOR_OBJECT_TYPE));

  bind(&do_check);
  // Restore generator object to register and perform assertion
  pop(object);
  Check(eq, kOperandIsNotAGeneratorObject);
}

void MacroAssembler::AssertUndefinedOrAllocationSite(Register object,
                                                     Register scratch) {
  if (emit_debug_code()) {
    Label done_checking;
    AssertNotSmi(object);
    CompareRoot(object, Heap::kUndefinedValueRootIndex);
    beq(&done_checking, Label::kNear);
    LoadP(scratch, FieldMemOperand(object, HeapObject::kMapOffset));
    CompareRoot(scratch, Heap::kAllocationSiteMapRootIndex);
    Assert(eq, kExpectedUndefinedOrCell);
    bind(&done_checking);
  }
}

void MacroAssembler::JumpIfNonSmisNotBothSequentialOneByteStrings(
    Register first, Register second, Register scratch1, Register scratch2,
    Label* failure) {
  // Test that both first and second are sequential one-byte strings.
  // Assume that they are non-smis.
  LoadP(scratch1, FieldMemOperand(first, HeapObject::kMapOffset));
  LoadP(scratch2, FieldMemOperand(second, HeapObject::kMapOffset));
  LoadlB(scratch1, FieldMemOperand(scratch1, Map::kInstanceTypeOffset));
  LoadlB(scratch2, FieldMemOperand(scratch2, Map::kInstanceTypeOffset));

  JumpIfBothInstanceTypesAreNotSequentialOneByte(scratch1, scratch2, scratch1,
                                                 scratch2, failure);
}

void MacroAssembler::JumpIfNotUniqueNameInstanceType(Register reg,
                                                     Label* not_unique_name) {
  STATIC_ASSERT(kInternalizedTag == 0 && kStringTag == 0);
  Label succeed;
  AndP(r0, reg, Operand(kIsNotStringMask | kIsNotInternalizedMask));
  beq(&succeed, Label::kNear);
  CmpP(reg, Operand(SYMBOL_TYPE));
  bne(not_unique_name);

  bind(&succeed);
}

void MacroAssembler::AllocateJSValue(Register result, Register constructor,
                                     Register value, Register scratch1,
                                     Register scratch2, Label* gc_required) {
  DCHECK(!result.is(constructor));
  DCHECK(!result.is(scratch1));
  DCHECK(!result.is(scratch2));
  DCHECK(!result.is(value));

  // Allocate JSValue in new space.
  Allocate(JSValue::kSize, result, scratch1, scratch2, gc_required,
           NO_ALLOCATION_FLAGS);

  // Initialize the JSValue.
  LoadGlobalFunctionInitialMap(constructor, scratch1, scratch2);
  StoreP(scratch1, FieldMemOperand(result, HeapObject::kMapOffset), r0);
  LoadRoot(scratch1, Heap::kEmptyFixedArrayRootIndex);
  StoreP(scratch1, FieldMemOperand(result, JSObject::kPropertiesOrHashOffset),
         r0);
  StoreP(scratch1, FieldMemOperand(result, JSObject::kElementsOffset), r0);
  StoreP(value, FieldMemOperand(result, JSValue::kValueOffset), r0);
  STATIC_ASSERT(JSValue::kSize == 4 * kPointerSize);
}

void MacroAssembler::JumpIfBothInstanceTypesAreNotSequentialOneByte(
    Register first, Register second, Register scratch1, Register scratch2,
    Label* failure) {
  const int kFlatOneByteStringMask =
      kIsNotStringMask | kStringEncodingMask | kStringRepresentationMask;
  const int kFlatOneByteStringTag =
      kStringTag | kOneByteStringTag | kSeqStringTag;
  if (!scratch1.is(first)) LoadRR(scratch1, first);
  if (!scratch2.is(second)) LoadRR(scratch2, second);
  nilf(scratch1, Operand(kFlatOneByteStringMask));
  CmpP(scratch1, Operand(kFlatOneByteStringTag));
  bne(failure);
  nilf(scratch2, Operand(kFlatOneByteStringMask));
  CmpP(scratch2, Operand(kFlatOneByteStringTag));
  bne(failure);
}

static const int kRegisterPassedArguments = 5;

int TurboAssembler::CalculateStackPassedWords(int num_reg_arguments,
                                              int num_double_arguments) {
  int stack_passed_words = 0;
  if (num_double_arguments > DoubleRegister::kNumRegisters) {
    stack_passed_words +=
        2 * (num_double_arguments - DoubleRegister::kNumRegisters);
  }
  // Up to five simple arguments are passed in registers r2..r6
  if (num_reg_arguments > kRegisterPassedArguments) {
    stack_passed_words += num_reg_arguments - kRegisterPassedArguments;
  }
  return stack_passed_words;
}

void TurboAssembler::PrepareCallCFunction(int num_reg_arguments,
                                          int num_double_arguments,
                                          Register scratch) {
  int frame_alignment = ActivationFrameAlignment();
  int stack_passed_arguments =
      CalculateStackPassedWords(num_reg_arguments, num_double_arguments);
  int stack_space = kNumRequiredStackFrameSlots;
  if (frame_alignment > kPointerSize) {
    // Make stack end at alignment and make room for stack arguments
    // -- preserving original value of sp.
    LoadRR(scratch, sp);
    lay(sp, MemOperand(sp, -(stack_passed_arguments + 1) * kPointerSize));
    DCHECK(base::bits::IsPowerOfTwo(frame_alignment));
    ClearRightImm(sp, sp, Operand(WhichPowerOf2(frame_alignment)));
    StoreP(scratch, MemOperand(sp, (stack_passed_arguments)*kPointerSize));
  } else {
    stack_space += stack_passed_arguments;
  }
  lay(sp, MemOperand(sp, -(stack_space)*kPointerSize));
}

void TurboAssembler::PrepareCallCFunction(int num_reg_arguments,
                                          Register scratch) {
  PrepareCallCFunction(num_reg_arguments, 0, scratch);
}

void TurboAssembler::MovToFloatParameter(DoubleRegister src) { Move(d0, src); }

void TurboAssembler::MovToFloatResult(DoubleRegister src) { Move(d0, src); }

void TurboAssembler::MovToFloatParameters(DoubleRegister src1,
                                          DoubleRegister src2) {
  if (src2.is(d0)) {
    DCHECK(!src1.is(d2));
    Move(d2, src2);
    Move(d0, src1);
  } else {
    Move(d0, src1);
    Move(d2, src2);
  }
}

void TurboAssembler::CallCFunction(ExternalReference function,
                                   int num_reg_arguments,
                                   int num_double_arguments) {
  mov(ip, Operand(function));
  CallCFunctionHelper(ip, num_reg_arguments, num_double_arguments);
}

void TurboAssembler::CallCFunction(Register function, int num_reg_arguments,
                                   int num_double_arguments) {
  CallCFunctionHelper(function, num_reg_arguments, num_double_arguments);
}

void TurboAssembler::CallCFunction(ExternalReference function,
                                   int num_arguments) {
  CallCFunction(function, num_arguments, 0);
}

void TurboAssembler::CallCFunction(Register function, int num_arguments) {
  CallCFunction(function, num_arguments, 0);
}

void TurboAssembler::CallCFunctionHelper(Register function,
                                         int num_reg_arguments,
                                         int num_double_arguments) {
  DCHECK_LE(num_reg_arguments + num_double_arguments, kMaxCParameters);
  DCHECK(has_frame());

  // Just call directly. The function called cannot cause a GC, or
  // allow preemption, so the return address in the link register
  // stays correct.
  Register dest = function;
  if (ABI_CALL_VIA_IP) {
    Move(ip, function);
    dest = ip;
  }

  Call(dest);

  int stack_passed_arguments =
      CalculateStackPassedWords(num_reg_arguments, num_double_arguments);
  int stack_space = kNumRequiredStackFrameSlots + stack_passed_arguments;
  if (ActivationFrameAlignment() > kPointerSize) {
    // Load the original stack pointer (pre-alignment) from the stack
    LoadP(sp, MemOperand(sp, stack_space * kPointerSize));
  } else {
    la(sp, MemOperand(sp, stack_space * kPointerSize));
  }
}

void TurboAssembler::CheckPageFlag(
    Register object,
    Register scratch,  // scratch may be same register as object
    int mask, Condition cc, Label* condition_met) {
  DCHECK(cc == ne || cc == eq);
  ClearRightImm(scratch, object, Operand(kPageSizeBits));

  if (base::bits::IsPowerOfTwo(mask)) {
    // If it's a power of two, we can use Test-Under-Mask Memory-Imm form
    // which allows testing of a single byte in memory.
    int32_t byte_offset = 4;
    uint32_t shifted_mask = mask;
    // Determine the byte offset to be tested
    if (mask <= 0x80) {
      byte_offset = kPointerSize - 1;
    } else if (mask < 0x8000) {
      byte_offset = kPointerSize - 2;
      shifted_mask = mask >> 8;
    } else if (mask < 0x800000) {
      byte_offset = kPointerSize - 3;
      shifted_mask = mask >> 16;
    } else {
      byte_offset = kPointerSize - 4;
      shifted_mask = mask >> 24;
    }
#if V8_TARGET_LITTLE_ENDIAN
    // Reverse the byte_offset if emulating on little endian platform
    byte_offset = kPointerSize - byte_offset - 1;
#endif
    tm(MemOperand(scratch, MemoryChunk::kFlagsOffset + byte_offset),
       Operand(shifted_mask));
  } else {
    LoadP(scratch, MemOperand(scratch, MemoryChunk::kFlagsOffset));
    AndP(r0, scratch, Operand(mask));
  }
  // Should be okay to remove rc

  if (cc == ne) {
    bne(condition_met);
  }
  if (cc == eq) {
    beq(condition_met);
  }
}

void MacroAssembler::JumpIfBlack(Register object, Register scratch0,
                                 Register scratch1, Label* on_black) {
  HasColor(object, scratch0, scratch1, on_black, 1, 1);  // kBlackBitPattern.
  DCHECK(strcmp(Marking::kBlackBitPattern, "11") == 0);
}

void MacroAssembler::HasColor(Register object, Register bitmap_scratch,
                              Register mask_scratch, Label* has_color,
                              int first_bit, int second_bit) {
  DCHECK(!AreAliased(object, bitmap_scratch, mask_scratch, no_reg));

  GetMarkBits(object, bitmap_scratch, mask_scratch);

  Label other_color, word_boundary;
  LoadlW(ip, MemOperand(bitmap_scratch, MemoryChunk::kHeaderSize));
  // Test the first bit
  AndP(r0, ip, mask_scratch /*, SetRC*/);  // Should be okay to remove rc
  b(first_bit == 1 ? eq : ne, &other_color, Label::kNear);
  // Shift left 1
  // May need to load the next cell
  sll(mask_scratch, Operand(1) /*, SetRC*/);
  LoadAndTest32(mask_scratch, mask_scratch);
  beq(&word_boundary, Label::kNear);
  // Test the second bit
  AndP(r0, ip, mask_scratch /*, SetRC*/);  // Should be okay to remove rc
  b(second_bit == 1 ? ne : eq, has_color);
  b(&other_color, Label::kNear);

  bind(&word_boundary);
  LoadlW(ip, MemOperand(bitmap_scratch, MemoryChunk::kHeaderSize + kIntSize));
  AndP(r0, ip, Operand(1));
  b(second_bit == 1 ? ne : eq, has_color);
  bind(&other_color);
}

void MacroAssembler::GetMarkBits(Register addr_reg, Register bitmap_reg,
                                 Register mask_reg) {
  DCHECK(!AreAliased(addr_reg, bitmap_reg, mask_reg, no_reg));
  LoadRR(bitmap_reg, addr_reg);
  nilf(bitmap_reg, Operand(~Page::kPageAlignmentMask));
  const int kLowBits = kPointerSizeLog2 + Bitmap::kBitsPerCellLog2;
  ExtractBitRange(mask_reg, addr_reg, kLowBits - 1, kPointerSizeLog2);
  ExtractBitRange(ip, addr_reg, kPageSizeBits - 1, kLowBits);
  ShiftLeftP(ip, ip, Operand(Bitmap::kBytesPerCellLog2));
  AddP(bitmap_reg, ip);
  LoadRR(ip, mask_reg);  // Have to do some funky reg shuffling as
                         // 31-bit shift left clobbers on s390.
  LoadImmP(mask_reg, Operand(1));
  ShiftLeftP(mask_reg, mask_reg, ip);
}

void MacroAssembler::JumpIfWhite(Register value, Register bitmap_scratch,
                                 Register mask_scratch, Register load_scratch,
                                 Label* value_is_white) {
  DCHECK(!AreAliased(value, bitmap_scratch, mask_scratch, ip));
  GetMarkBits(value, bitmap_scratch, mask_scratch);

  // If the value is black or grey we don't need to do anything.
  DCHECK(strcmp(Marking::kWhiteBitPattern, "00") == 0);
  DCHECK(strcmp(Marking::kBlackBitPattern, "11") == 0);
  DCHECK(strcmp(Marking::kGreyBitPattern, "10") == 0);
  DCHECK(strcmp(Marking::kImpossibleBitPattern, "01") == 0);

  // Since both black and grey have a 1 in the first position and white does
  // not have a 1 there we only need to check one bit.
  LoadlW(load_scratch, MemOperand(bitmap_scratch, MemoryChunk::kHeaderSize));
  LoadRR(r0, load_scratch);
  AndP(r0, mask_scratch);
  beq(value_is_white);
}

void MacroAssembler::LoadInstanceDescriptors(Register map,
                                             Register descriptors) {
  LoadP(descriptors, FieldMemOperand(map, Map::kDescriptorsOffset));
}

void MacroAssembler::LoadAccessor(Register dst, Register holder,
                                  int accessor_index,
                                  AccessorComponent accessor) {
  LoadP(dst, FieldMemOperand(holder, HeapObject::kMapOffset));
  LoadInstanceDescriptors(dst, dst);
  LoadP(dst,
        FieldMemOperand(dst, DescriptorArray::GetValueOffset(accessor_index)));
  const int getterOffset = AccessorPair::kGetterOffset;
  const int setterOffset = AccessorPair::kSetterOffset;
  int offset = ((accessor == ACCESSOR_GETTER) ? getterOffset : setterOffset);
  LoadP(dst, FieldMemOperand(dst, offset));
}

////////////////////////////////////////////////////////////////////////////////
//
// New MacroAssembler Interfaces added for S390
//
////////////////////////////////////////////////////////////////////////////////
// Primarily used for loading constants
// This should really move to be in macro-assembler as it
// is really a pseudo instruction
// Some usages of this intend for a FIXED_SEQUENCE to be used
// @TODO - break this dependency so we can optimize mov() in general
// and only use the generic version when we require a fixed sequence
void MacroAssembler::LoadRepresentation(Register dst, const MemOperand& mem,
                                        Representation r, Register scratch) {
  DCHECK(!r.IsDouble());
  if (r.IsInteger8()) {
    LoadB(dst, mem);
  } else if (r.IsUInteger8()) {
    LoadlB(dst, mem);
  } else if (r.IsInteger16()) {
    LoadHalfWordP(dst, mem, scratch);
  } else if (r.IsUInteger16()) {
    LoadHalfWordP(dst, mem, scratch);
#if V8_TARGET_ARCH_S390X
  } else if (r.IsInteger32()) {
    LoadW(dst, mem, scratch);
#endif
  } else {
    LoadP(dst, mem, scratch);
  }
}

void MacroAssembler::StoreRepresentation(Register src, const MemOperand& mem,
                                         Representation r, Register scratch) {
  DCHECK(!r.IsDouble());
  if (r.IsInteger8() || r.IsUInteger8()) {
    StoreByte(src, mem, scratch);
  } else if (r.IsInteger16() || r.IsUInteger16()) {
    StoreHalfWord(src, mem, scratch);
#if V8_TARGET_ARCH_S390X
  } else if (r.IsInteger32()) {
    StoreW(src, mem, scratch);
#endif
  } else {
    if (r.IsHeapObject()) {
      AssertNotSmi(src);
    } else if (r.IsSmi()) {
      AssertSmi(src);
    }
    StoreP(src, mem, scratch);
  }
}

Register GetRegisterThatIsNotOneOf(Register reg1, Register reg2, Register reg3,
                                   Register reg4, Register reg5,
                                   Register reg6) {
  RegList regs = 0;
  if (reg1.is_valid()) regs |= reg1.bit();
  if (reg2.is_valid()) regs |= reg2.bit();
  if (reg3.is_valid()) regs |= reg3.bit();
  if (reg4.is_valid()) regs |= reg4.bit();
  if (reg5.is_valid()) regs |= reg5.bit();
  if (reg6.is_valid()) regs |= reg6.bit();

  const RegisterConfiguration* config = RegisterConfiguration::Default();
  for (int i = 0; i < config->num_allocatable_general_registers(); ++i) {
    int code = config->GetAllocatableGeneralCode(i);
    Register candidate = Register::from_code(code);
    if (regs & candidate.bit()) continue;
    return candidate;
  }
  UNREACHABLE();
}

void TurboAssembler::mov(Register dst, const Operand& src) {
#if V8_TARGET_ARCH_S390X
  int64_t value;
#else
  int value;
#endif
  if (src.is_heap_object_request()) {
    RequestHeapObject(src.heap_object_request());
    value = 0;
  } else {
    value = src.immediate();
  }

  if (src.rmode() != kRelocInfo_NONEPTR) {
    // some form of relocation needed
    RecordRelocInfo(src.rmode(), value);
  }

#if V8_TARGET_ARCH_S390X
  int32_t hi_32 = static_cast<int64_t>(value) >> 32;
  int32_t lo_32 = static_cast<int32_t>(value);

  iihf(dst, Operand(hi_32));
  iilf(dst, Operand(lo_32));
#else
  iilf(dst, Operand(value));
#endif
}

void TurboAssembler::Mul32(Register dst, const MemOperand& src1) {
  if (is_uint12(src1.offset())) {
    ms(dst, src1);
  } else if (is_int20(src1.offset())) {
    msy(dst, src1);
  } else {
    UNIMPLEMENTED();
  }
}

void TurboAssembler::Mul32(Register dst, Register src1) { msr(dst, src1); }

void TurboAssembler::Mul32(Register dst, const Operand& src1) {
  msfi(dst, src1);
}

#define Generate_MulHigh32(instr) \
  {                               \
    lgfr(dst, src1);              \
    instr(dst, src2);             \
    srlg(dst, dst, Operand(32));  \
  }

void TurboAssembler::MulHigh32(Register dst, Register src1,
                               const MemOperand& src2) {
  Generate_MulHigh32(msgf);
}

void TurboAssembler::MulHigh32(Register dst, Register src1, Register src2) {
  if (dst.is(src2)) {
    std::swap(src1, src2);
  }
  Generate_MulHigh32(msgfr);
}

void TurboAssembler::MulHigh32(Register dst, Register src1,
                               const Operand& src2) {
  Generate_MulHigh32(msgfi);
}

#undef Generate_MulHigh32

#define Generate_MulHighU32(instr) \
  {                                \
    lr(r1, src1);                  \
    instr(r0, src2);               \
    LoadlW(dst, r0);               \
  }

void TurboAssembler::MulHighU32(Register dst, Register src1,
                                const MemOperand& src2) {
  Generate_MulHighU32(ml);
}

void TurboAssembler::MulHighU32(Register dst, Register src1, Register src2) {
  Generate_MulHighU32(mlr);
}

void TurboAssembler::MulHighU32(Register dst, Register src1,
                                const Operand& src2) {
  USE(dst);
  USE(src1);
  USE(src2);
  UNREACHABLE();
}

#undef Generate_MulHighU32

#define Generate_Mul32WithOverflowIfCCUnequal(instr) \
  {                                                  \
    lgfr(dst, src1);                                 \
    instr(dst, src2);                                \
    cgfr(dst, dst);                                  \
  }

void TurboAssembler::Mul32WithOverflowIfCCUnequal(Register dst, Register src1,
                                                  const MemOperand& src2) {
  Register result = dst;
  if (src2.rx().is(dst) || src2.rb().is(dst)) dst = r0;
  Generate_Mul32WithOverflowIfCCUnequal(msgf);
  if (!result.is(dst)) llgfr(result, dst);
}

void TurboAssembler::Mul32WithOverflowIfCCUnequal(Register dst, Register src1,
                                                  Register src2) {
  if (dst.is(src2)) {
    std::swap(src1, src2);
  }
  Generate_Mul32WithOverflowIfCCUnequal(msgfr);
}

void TurboAssembler::Mul32WithOverflowIfCCUnequal(Register dst, Register src1,
                                                  const Operand& src2) {
  Generate_Mul32WithOverflowIfCCUnequal(msgfi);
}

#undef Generate_Mul32WithOverflowIfCCUnequal

void TurboAssembler::Mul64(Register dst, const MemOperand& src1) {
  if (is_int20(src1.offset())) {
    msg(dst, src1);
  } else {
    UNIMPLEMENTED();
  }
}

void TurboAssembler::Mul64(Register dst, Register src1) { msgr(dst, src1); }

void TurboAssembler::Mul64(Register dst, const Operand& src1) {
  msgfi(dst, src1);
}

void TurboAssembler::Mul(Register dst, Register src1, Register src2) {
  if (CpuFeatures::IsSupported(MISC_INSTR_EXT2)) {
    MulPWithCondition(dst, src1, src2);
  } else {
    if (dst.is(src2)) {
      MulP(dst, src1);
    } else if (dst.is(src1)) {
      MulP(dst, src2);
    } else {
      Move(dst, src1);
      MulP(dst, src2);
    }
  }
}

void TurboAssembler::DivP(Register dividend, Register divider) {
  // have to make sure the src and dst are reg pairs
  DCHECK(dividend.code() % 2 == 0);
#if V8_TARGET_ARCH_S390X
  dsgr(dividend, divider);
#else
  dr(dividend, divider);
#endif
}

#define Generate_Div32(instr) \
  {                           \
    lgfr(r1, src1);           \
    instr(r0, src2);          \
    LoadlW(dst, r1);          \
  }

void TurboAssembler::Div32(Register dst, Register src1,
                           const MemOperand& src2) {
  Generate_Div32(dsgf);
}

void TurboAssembler::Div32(Register dst, Register src1, Register src2) {
  Generate_Div32(dsgfr);
}

#undef Generate_Div32

#define Generate_DivU32(instr) \
  {                            \
    lr(r0, src1);              \
    srdl(r0, Operand(32));     \
    instr(r0, src2);           \
    LoadlW(dst, r1);           \
  }

void TurboAssembler::DivU32(Register dst, Register src1,
                            const MemOperand& src2) {
  Generate_DivU32(dl);
}

void TurboAssembler::DivU32(Register dst, Register src1, Register src2) {
  Generate_DivU32(dlr);
}

#undef Generate_DivU32

#define Generate_Div64(instr) \
  {                           \
    lgr(r1, src1);            \
    instr(r0, src2);          \
    lgr(dst, r1);             \
  }

void TurboAssembler::Div64(Register dst, Register src1,
                           const MemOperand& src2) {
  Generate_Div64(dsg);
}

void TurboAssembler::Div64(Register dst, Register src1, Register src2) {
  Generate_Div64(dsgr);
}

#undef Generate_Div64

#define Generate_DivU64(instr) \
  {                            \
    lgr(r1, src1);             \
    lghi(r0, Operand::Zero()); \
    instr(r0, src2);           \
    lgr(dst, r1);              \
  }

void TurboAssembler::DivU64(Register dst, Register src1,
                            const MemOperand& src2) {
  Generate_DivU64(dlg);
}

void TurboAssembler::DivU64(Register dst, Register src1, Register src2) {
  Generate_DivU64(dlgr);
}

#undef Generate_DivU64

#define Generate_Mod32(instr) \
  {                           \
    lgfr(r1, src1);           \
    instr(r0, src2);          \
    LoadlW(dst, r0);          \
  }

void TurboAssembler::Mod32(Register dst, Register src1,
                           const MemOperand& src2) {
  Generate_Mod32(dsgf);
}

void TurboAssembler::Mod32(Register dst, Register src1, Register src2) {
  Generate_Mod32(dsgfr);
}

#undef Generate_Mod32

#define Generate_ModU32(instr) \
  {                            \
    lr(r0, src1);              \
    srdl(r0, Operand(32));     \
    instr(r0, src2);           \
    LoadlW(dst, r0);           \
  }

void TurboAssembler::ModU32(Register dst, Register src1,
                            const MemOperand& src2) {
  Generate_ModU32(dl);
}

void TurboAssembler::ModU32(Register dst, Register src1, Register src2) {
  Generate_ModU32(dlr);
}

#undef Generate_ModU32

#define Generate_Mod64(instr) \
  {                           \
    lgr(r1, src1);            \
    instr(r0, src2);          \
    lgr(dst, r0);             \
  }

void TurboAssembler::Mod64(Register dst, Register src1,
                           const MemOperand& src2) {
  Generate_Mod64(dsg);
}

void TurboAssembler::Mod64(Register dst, Register src1, Register src2) {
  Generate_Mod64(dsgr);
}

#undef Generate_Mod64

#define Generate_ModU64(instr) \
  {                            \
    lgr(r1, src1);             \
    lghi(r0, Operand::Zero()); \
    instr(r0, src2);           \
    lgr(dst, r0);              \
  }

void TurboAssembler::ModU64(Register dst, Register src1,
                            const MemOperand& src2) {
  Generate_ModU64(dlg);
}

void TurboAssembler::ModU64(Register dst, Register src1, Register src2) {
  Generate_ModU64(dlgr);
}

#undef Generate_ModU64

void TurboAssembler::MulP(Register dst, const Operand& opnd) {
#if V8_TARGET_ARCH_S390X
  msgfi(dst, opnd);
#else
  msfi(dst, opnd);
#endif
}

void TurboAssembler::MulP(Register dst, Register src) {
#if V8_TARGET_ARCH_S390X
  msgr(dst, src);
#else
  msr(dst, src);
#endif
}

void TurboAssembler::MulPWithCondition(Register dst, Register src1,
                                       Register src2) {
  CHECK(CpuFeatures::IsSupported(MISC_INSTR_EXT2));
#if V8_TARGET_ARCH_S390X
  msgrkc(dst, src1, src2);
#else
  msrkc(dst, src1, src2);
#endif
}

void TurboAssembler::MulP(Register dst, const MemOperand& opnd) {
#if V8_TARGET_ARCH_S390X
  if (is_uint16(opnd.offset())) {
    ms(dst, opnd);
  } else if (is_int20(opnd.offset())) {
    msy(dst, opnd);
  } else {
    UNIMPLEMENTED();
  }
#else
  if (is_int20(opnd.offset())) {
    msg(dst, opnd);
  } else {
    UNIMPLEMENTED();
  }
#endif
}

void TurboAssembler::Sqrt(DoubleRegister result, DoubleRegister input) {
  sqdbr(result, input);
}
void TurboAssembler::Sqrt(DoubleRegister result, const MemOperand& input) {
  if (is_uint12(input.offset())) {
    sqdb(result, input);
  } else {
    ldy(result, input);
    sqdbr(result, result);
  }
}
//----------------------------------------------------------------------------
//  Add Instructions
//----------------------------------------------------------------------------

// Add 32-bit (Register dst = Register dst + Immediate opnd)
void TurboAssembler::Add32(Register dst, const Operand& opnd) {
  if (is_int16(opnd.immediate()))
    ahi(dst, opnd);
  else
    afi(dst, opnd);
}

// Add 32-bit (Register dst = Register dst + Immediate opnd)
void TurboAssembler::Add32_RI(Register dst, const Operand& opnd) {
  // Just a wrapper for above
  Add32(dst, opnd);
}

// Add Pointer Size (Register dst = Register dst + Immediate opnd)
void TurboAssembler::AddP(Register dst, const Operand& opnd) {
#if V8_TARGET_ARCH_S390X
  if (is_int16(opnd.immediate()))
    aghi(dst, opnd);
  else
    agfi(dst, opnd);
#else
  Add32(dst, opnd);
#endif
}

// Add 32-bit (Register dst = Register src + Immediate opnd)
void TurboAssembler::Add32(Register dst, Register src, const Operand& opnd) {
  if (!dst.is(src)) {
    if (CpuFeatures::IsSupported(DISTINCT_OPS) && is_int16(opnd.immediate())) {
      ahik(dst, src, opnd);
      return;
    }
    lr(dst, src);
  }
  Add32(dst, opnd);
}

// Add 32-bit (Register dst = Register src + Immediate opnd)
void TurboAssembler::Add32_RRI(Register dst, Register src,
                               const Operand& opnd) {
  // Just a wrapper for above
  Add32(dst, src, opnd);
}

// Add Pointer Size (Register dst = Register src + Immediate opnd)
void TurboAssembler::AddP(Register dst, Register src, const Operand& opnd) {
  if (!dst.is(src)) {
    if (CpuFeatures::IsSupported(DISTINCT_OPS) && is_int16(opnd.immediate())) {
      AddPImm_RRI(dst, src, opnd);
      return;
    }
    LoadRR(dst, src);
  }
  AddP(dst, opnd);
}

// Add 32-bit (Register dst = Register dst + Register src)
void TurboAssembler::Add32(Register dst, Register src) { ar(dst, src); }

// Add Pointer Size (Register dst = Register dst + Register src)
void TurboAssembler::AddP(Register dst, Register src) { AddRR(dst, src); }

// Add Pointer Size with src extension
//     (Register dst(ptr) = Register dst (ptr) + Register src (32 | 32->64))
// src is treated as a 32-bit signed integer, which is sign extended to
// 64-bit if necessary.
void TurboAssembler::AddP_ExtendSrc(Register dst, Register src) {
#if V8_TARGET_ARCH_S390X
  agfr(dst, src);
#else
  ar(dst, src);
#endif
}

// Add 32-bit (Register dst = Register src1 + Register src2)
void TurboAssembler::Add32(Register dst, Register src1, Register src2) {
  if (!dst.is(src1) && !dst.is(src2)) {
    // We prefer to generate AR/AGR, over the non clobbering ARK/AGRK
    // as AR is a smaller instruction
    if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
      ark(dst, src1, src2);
      return;
    } else {
      lr(dst, src1);
    }
  } else if (dst.is(src2)) {
    src2 = src1;
  }
  ar(dst, src2);
}

// Add Pointer Size (Register dst = Register src1 + Register src2)
void TurboAssembler::AddP(Register dst, Register src1, Register src2) {
  if (!dst.is(src1) && !dst.is(src2)) {
    // We prefer to generate AR/AGR, over the non clobbering ARK/AGRK
    // as AR is a smaller instruction
    if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
      AddP_RRR(dst, src1, src2);
      return;
    } else {
      LoadRR(dst, src1);
    }
  } else if (dst.is(src2)) {
    src2 = src1;
  }
  AddRR(dst, src2);
}

// Add Pointer Size with src extension
//      (Register dst (ptr) = Register dst (ptr) + Register src1 (ptr) +
//                            Register src2 (32 | 32->64))
// src is treated as a 32-bit signed integer, which is sign extended to
// 64-bit if necessary.
void TurboAssembler::AddP_ExtendSrc(Register dst, Register src1,
                                    Register src2) {
#if V8_TARGET_ARCH_S390X
  if (dst.is(src2)) {
    // The source we need to sign extend is the same as result.
    lgfr(dst, src2);
    agr(dst, src1);
  } else {
    if (!dst.is(src1)) LoadRR(dst, src1);
    agfr(dst, src2);
  }
#else
  AddP(dst, src1, src2);
#endif
}

// Add 32-bit (Register-Memory)
void TurboAssembler::Add32(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
  if (is_uint12(opnd.offset()))
    a(dst, opnd);
  else
    ay(dst, opnd);
}

// Add Pointer Size (Register-Memory)
void TurboAssembler::AddP(Register dst, const MemOperand& opnd) {
#if V8_TARGET_ARCH_S390X
  DCHECK(is_int20(opnd.offset()));
  ag(dst, opnd);
#else
  Add32(dst, opnd);
#endif
}

// Add Pointer Size with src extension
//      (Register dst (ptr) = Register dst (ptr) + Mem opnd (32 | 32->64))
// src is treated as a 32-bit signed integer, which is sign extended to
// 64-bit if necessary.
void TurboAssembler::AddP_ExtendSrc(Register dst, const MemOperand& opnd) {
#if V8_TARGET_ARCH_S390X
  DCHECK(is_int20(opnd.offset()));
  agf(dst, opnd);
#else
  Add32(dst, opnd);
#endif
}

// Add 32-bit (Memory - Immediate)
void TurboAssembler::Add32(const MemOperand& opnd, const Operand& imm) {
  DCHECK(is_int8(imm.immediate()));
  DCHECK(is_int20(opnd.offset()));
  DCHECK(CpuFeatures::IsSupported(GENERAL_INSTR_EXT));
  asi(opnd, imm);
}

// Add Pointer-sized (Memory - Immediate)
void TurboAssembler::AddP(const MemOperand& opnd, const Operand& imm) {
  DCHECK(is_int8(imm.immediate()));
  DCHECK(is_int20(opnd.offset()));
  DCHECK(CpuFeatures::IsSupported(GENERAL_INSTR_EXT));
#if V8_TARGET_ARCH_S390X
  agsi(opnd, imm);
#else
  asi(opnd, imm);
#endif
}

//----------------------------------------------------------------------------
//  Add Logical Instructions
//----------------------------------------------------------------------------

// Add Logical With Carry 32-bit (Register dst = Register src1 + Register src2)
void TurboAssembler::AddLogicalWithCarry32(Register dst, Register src1,
                                           Register src2) {
  if (!dst.is(src2) && !dst.is(src1)) {
    lr(dst, src1);
    alcr(dst, src2);
  } else if (!dst.is(src2)) {
    // dst == src1
    DCHECK(dst.is(src1));
    alcr(dst, src2);
  } else {
    // dst == src2
    DCHECK(dst.is(src2));
    alcr(dst, src1);
  }
}

// Add Logical 32-bit (Register dst = Register src1 + Register src2)
void TurboAssembler::AddLogical32(Register dst, Register src1, Register src2) {
  if (!dst.is(src2) && !dst.is(src1)) {
    lr(dst, src1);
    alr(dst, src2);
  } else if (!dst.is(src2)) {
    // dst == src1
    DCHECK(dst.is(src1));
    alr(dst, src2);
  } else {
    // dst == src2
    DCHECK(dst.is(src2));
    alr(dst, src1);
  }
}

// Add Logical 32-bit (Register dst = Register dst + Immediate opnd)
void TurboAssembler::AddLogical(Register dst, const Operand& imm) {
  alfi(dst, imm);
}

// Add Logical Pointer Size (Register dst = Register dst + Immediate opnd)
void TurboAssembler::AddLogicalP(Register dst, const Operand& imm) {
#ifdef V8_TARGET_ARCH_S390X
  algfi(dst, imm);
#else
  AddLogical(dst, imm);
#endif
}

// Add Logical 32-bit (Register-Memory)
void TurboAssembler::AddLogical(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
  if (is_uint12(opnd.offset()))
    al_z(dst, opnd);
  else
    aly(dst, opnd);
}

// Add Logical Pointer Size (Register-Memory)
void TurboAssembler::AddLogicalP(Register dst, const MemOperand& opnd) {
#if V8_TARGET_ARCH_S390X
  DCHECK(is_int20(opnd.offset()));
  alg(dst, opnd);
#else
  AddLogical(dst, opnd);
#endif
}

//----------------------------------------------------------------------------
//  Subtract Instructions
//----------------------------------------------------------------------------

// Subtract Logical With Carry 32-bit (Register dst = Register src1 - Register
// src2)
void TurboAssembler::SubLogicalWithBorrow32(Register dst, Register src1,
                                            Register src2) {
  if (!dst.is(src2) && !dst.is(src1)) {
    lr(dst, src1);
    slbr(dst, src2);
  } else if (!dst.is(src2)) {
    // dst == src1
    DCHECK(dst.is(src1));
    slbr(dst, src2);
  } else {
    // dst == src2
    DCHECK(dst.is(src2));
    lr(r0, dst);
    SubLogicalWithBorrow32(dst, src1, r0);
  }
}

// Subtract Logical 32-bit (Register dst = Register src1 - Register src2)
void TurboAssembler::SubLogical32(Register dst, Register src1, Register src2) {
  if (!dst.is(src2) && !dst.is(src1)) {
    lr(dst, src1);
    slr(dst, src2);
  } else if (!dst.is(src2)) {
    // dst == src1
    DCHECK(dst.is(src1));
    slr(dst, src2);
  } else {
    // dst == src2
    DCHECK(dst.is(src2));
    lr(r0, dst);
    SubLogical32(dst, src1, r0);
  }
}

// Subtract 32-bit (Register dst = Register dst - Immediate opnd)
void TurboAssembler::Sub32(Register dst, const Operand& imm) {
  Add32(dst, Operand(-(imm.immediate())));
}

// Subtract Pointer Size (Register dst = Register dst - Immediate opnd)
void TurboAssembler::SubP(Register dst, const Operand& imm) {
  AddP(dst, Operand(-(imm.immediate())));
}

// Subtract 32-bit (Register dst = Register src - Immediate opnd)
void TurboAssembler::Sub32(Register dst, Register src, const Operand& imm) {
  Add32(dst, src, Operand(-(imm.immediate())));
}

// Subtract Pointer Sized (Register dst = Register src - Immediate opnd)
void TurboAssembler::SubP(Register dst, Register src, const Operand& imm) {
  AddP(dst, src, Operand(-(imm.immediate())));
}

// Subtract 32-bit (Register dst = Register dst - Register src)
void TurboAssembler::Sub32(Register dst, Register src) { sr(dst, src); }

// Subtract Pointer Size (Register dst = Register dst - Register src)
void TurboAssembler::SubP(Register dst, Register src) { SubRR(dst, src); }

// Subtract Pointer Size with src extension
//     (Register dst(ptr) = Register dst (ptr) - Register src (32 | 32->64))
// src is treated as a 32-bit signed integer, which is sign extended to
// 64-bit if necessary.
void TurboAssembler::SubP_ExtendSrc(Register dst, Register src) {
#if V8_TARGET_ARCH_S390X
  sgfr(dst, src);
#else
  sr(dst, src);
#endif
}

// Subtract 32-bit (Register = Register - Register)
void TurboAssembler::Sub32(Register dst, Register src1, Register src2) {
  // Use non-clobbering version if possible
  if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    srk(dst, src1, src2);
    return;
  }
  if (!dst.is(src1) && !dst.is(src2)) lr(dst, src1);
  // In scenario where we have dst = src - dst, we need to swap and negate
  if (!dst.is(src1) && dst.is(src2)) {
    Label done;
    lcr(dst, dst);  // dst = -dst
    b(overflow, &done);
    ar(dst, src1);  // dst = dst + src
    bind(&done);
  } else {
    sr(dst, src2);
  }
}

// Subtract Pointer Sized (Register = Register - Register)
void TurboAssembler::SubP(Register dst, Register src1, Register src2) {
  // Use non-clobbering version if possible
  if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    SubP_RRR(dst, src1, src2);
    return;
  }
  if (!dst.is(src1) && !dst.is(src2)) LoadRR(dst, src1);
  // In scenario where we have dst = src - dst, we need to swap and negate
  if (!dst.is(src1) && dst.is(src2)) {
    Label done;
    LoadComplementRR(dst, dst);  // dst = -dst
    b(overflow, &done);
    AddP(dst, src1);  // dst = dst + src
    bind(&done);
  } else {
    SubP(dst, src2);
  }
}

// Subtract Pointer Size with src extension
//     (Register dst(ptr) = Register dst (ptr) - Register src (32 | 32->64))
// src is treated as a 32-bit signed integer, which is sign extended to
// 64-bit if necessary.
void TurboAssembler::SubP_ExtendSrc(Register dst, Register src1,
                                    Register src2) {
#if V8_TARGET_ARCH_S390X
  if (!dst.is(src1) && !dst.is(src2)) LoadRR(dst, src1);

  // In scenario where we have dst = src - dst, we need to swap and negate
  if (!dst.is(src1) && dst.is(src2)) {
    lgfr(dst, dst);              // Sign extend this operand first.
    LoadComplementRR(dst, dst);  // dst = -dst
    AddP(dst, src1);             // dst = -dst + src
  } else {
    sgfr(dst, src2);
  }
#else
  SubP(dst, src1, src2);
#endif
}

// Subtract 32-bit (Register-Memory)
void TurboAssembler::Sub32(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
  if (is_uint12(opnd.offset()))
    s(dst, opnd);
  else
    sy(dst, opnd);
}

// Subtract Pointer Sized (Register - Memory)
void TurboAssembler::SubP(Register dst, const MemOperand& opnd) {
#if V8_TARGET_ARCH_S390X
  sg(dst, opnd);
#else
  Sub32(dst, opnd);
#endif
}

void TurboAssembler::MovIntToFloat(DoubleRegister dst, Register src) {
  sllg(r0, src, Operand(32));
  ldgr(dst, r0);
}

void TurboAssembler::MovFloatToInt(Register dst, DoubleRegister src) {
  lgdr(dst, src);
  srlg(dst, dst, Operand(32));
}

void TurboAssembler::SubP_ExtendSrc(Register dst, const MemOperand& opnd) {
#if V8_TARGET_ARCH_S390X
  DCHECK(is_int20(opnd.offset()));
  sgf(dst, opnd);
#else
  Sub32(dst, opnd);
#endif
}

//----------------------------------------------------------------------------
//  Subtract Logical Instructions
//----------------------------------------------------------------------------

// Subtract Logical 32-bit (Register - Memory)
void TurboAssembler::SubLogical(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
  if (is_uint12(opnd.offset()))
    sl(dst, opnd);
  else
    sly(dst, opnd);
}

// Subtract Logical Pointer Sized (Register - Memory)
void TurboAssembler::SubLogicalP(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
#if V8_TARGET_ARCH_S390X
  slgf(dst, opnd);
#else
  SubLogical(dst, opnd);
#endif
}

// Subtract Logical Pointer Size with src extension
//      (Register dst (ptr) = Register dst (ptr) - Mem opnd (32 | 32->64))
// src is treated as a 32-bit signed integer, which is sign extended to
// 64-bit if necessary.
void TurboAssembler::SubLogicalP_ExtendSrc(Register dst,
                                           const MemOperand& opnd) {
#if V8_TARGET_ARCH_S390X
  DCHECK(is_int20(opnd.offset()));
  slgf(dst, opnd);
#else
  SubLogical(dst, opnd);
#endif
}

//----------------------------------------------------------------------------
//  Bitwise Operations
//----------------------------------------------------------------------------

// AND 32-bit - dst = dst & src
void TurboAssembler::And(Register dst, Register src) { nr(dst, src); }

// AND Pointer Size - dst = dst & src
void TurboAssembler::AndP(Register dst, Register src) { AndRR(dst, src); }

// Non-clobbering AND 32-bit - dst = src1 & src1
void TurboAssembler::And(Register dst, Register src1, Register src2) {
  if (!dst.is(src1) && !dst.is(src2)) {
    // We prefer to generate XR/XGR, over the non clobbering XRK/XRK
    // as XR is a smaller instruction
    if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
      nrk(dst, src1, src2);
      return;
    } else {
      lr(dst, src1);
    }
  } else if (dst.is(src2)) {
    src2 = src1;
  }
  And(dst, src2);
}

// Non-clobbering AND pointer size - dst = src1 & src1
void TurboAssembler::AndP(Register dst, Register src1, Register src2) {
  if (!dst.is(src1) && !dst.is(src2)) {
    // We prefer to generate XR/XGR, over the non clobbering XRK/XRK
    // as XR is a smaller instruction
    if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
      AndP_RRR(dst, src1, src2);
      return;
    } else {
      LoadRR(dst, src1);
    }
  } else if (dst.is(src2)) {
    src2 = src1;
  }
  AndP(dst, src2);
}

// AND 32-bit (Reg - Mem)
void TurboAssembler::And(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
  if (is_uint12(opnd.offset()))
    n(dst, opnd);
  else
    ny(dst, opnd);
}

// AND Pointer Size (Reg - Mem)
void TurboAssembler::AndP(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
#if V8_TARGET_ARCH_S390X
  ng(dst, opnd);
#else
  And(dst, opnd);
#endif
}

// AND 32-bit - dst = dst & imm
void TurboAssembler::And(Register dst, const Operand& opnd) { nilf(dst, opnd); }

// AND Pointer Size - dst = dst & imm
void TurboAssembler::AndP(Register dst, const Operand& opnd) {
#if V8_TARGET_ARCH_S390X
  intptr_t value = opnd.immediate();
  if (value >> 32 != -1) {
    // this may not work b/c condition code won't be set correctly
    nihf(dst, Operand(value >> 32));
  }
  nilf(dst, Operand(value & 0xFFFFFFFF));
#else
  And(dst, opnd);
#endif
}

// AND 32-bit - dst = src & imm
void TurboAssembler::And(Register dst, Register src, const Operand& opnd) {
  if (!dst.is(src)) lr(dst, src);
  nilf(dst, opnd);
}

// AND Pointer Size - dst = src & imm
void TurboAssembler::AndP(Register dst, Register src, const Operand& opnd) {
  // Try to exploit RISBG first
  intptr_t value = opnd.immediate();
  if (CpuFeatures::IsSupported(GENERAL_INSTR_EXT)) {
    intptr_t shifted_value = value;
    int trailing_zeros = 0;

    // We start checking how many trailing zeros are left at the end.
    while ((0 != shifted_value) && (0 == (shifted_value & 1))) {
      trailing_zeros++;
      shifted_value >>= 1;
    }

    // If temp (value with right-most set of zeros shifted out) is 1 less
    // than power of 2, we have consecutive bits of 1.
    // Special case: If shift_value is zero, we cannot use RISBG, as it requires
    //               selection of at least 1 bit.
    if ((0 != shifted_value) && base::bits::IsPowerOfTwo(shifted_value + 1)) {
      int startBit =
          base::bits::CountLeadingZeros64(shifted_value) - trailing_zeros;
      int endBit = 63 - trailing_zeros;
      // Start: startBit, End: endBit, Shift = 0, true = zero unselected bits.
      risbg(dst, src, Operand(startBit), Operand(endBit), Operand::Zero(),
            true);
      return;
    } else if (-1 == shifted_value) {
      // A Special case in which all top bits up to MSB are 1's.  In this case,
      // we can set startBit to be 0.
      int endBit = 63 - trailing_zeros;
      risbg(dst, src, Operand::Zero(), Operand(endBit), Operand::Zero(), true);
      return;
    }
  }

  // If we are &'ing zero, we can just whack the dst register and skip copy
  if (!dst.is(src) && (0 != value)) LoadRR(dst, src);
  AndP(dst, opnd);
}

// OR 32-bit - dst = dst & src
void TurboAssembler::Or(Register dst, Register src) { or_z(dst, src); }

// OR Pointer Size - dst = dst & src
void TurboAssembler::OrP(Register dst, Register src) { OrRR(dst, src); }

// Non-clobbering OR 32-bit - dst = src1 & src1
void TurboAssembler::Or(Register dst, Register src1, Register src2) {
  if (!dst.is(src1) && !dst.is(src2)) {
    // We prefer to generate XR/XGR, over the non clobbering XRK/XRK
    // as XR is a smaller instruction
    if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
      ork(dst, src1, src2);
      return;
    } else {
      lr(dst, src1);
    }
  } else if (dst.is(src2)) {
    src2 = src1;
  }
  Or(dst, src2);
}

// Non-clobbering OR pointer size - dst = src1 & src1
void TurboAssembler::OrP(Register dst, Register src1, Register src2) {
  if (!dst.is(src1) && !dst.is(src2)) {
    // We prefer to generate XR/XGR, over the non clobbering XRK/XRK
    // as XR is a smaller instruction
    if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
      OrP_RRR(dst, src1, src2);
      return;
    } else {
      LoadRR(dst, src1);
    }
  } else if (dst.is(src2)) {
    src2 = src1;
  }
  OrP(dst, src2);
}

// OR 32-bit (Reg - Mem)
void TurboAssembler::Or(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
  if (is_uint12(opnd.offset()))
    o(dst, opnd);
  else
    oy(dst, opnd);
}

// OR Pointer Size (Reg - Mem)
void TurboAssembler::OrP(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
#if V8_TARGET_ARCH_S390X
  og(dst, opnd);
#else
  Or(dst, opnd);
#endif
}

// OR 32-bit - dst = dst & imm
void TurboAssembler::Or(Register dst, const Operand& opnd) { oilf(dst, opnd); }

// OR Pointer Size - dst = dst & imm
void TurboAssembler::OrP(Register dst, const Operand& opnd) {
#if V8_TARGET_ARCH_S390X
  intptr_t value = opnd.immediate();
  if (value >> 32 != 0) {
    // this may not work b/c condition code won't be set correctly
    oihf(dst, Operand(value >> 32));
  }
  oilf(dst, Operand(value & 0xFFFFFFFF));
#else
  Or(dst, opnd);
#endif
}

// OR 32-bit - dst = src & imm
void TurboAssembler::Or(Register dst, Register src, const Operand& opnd) {
  if (!dst.is(src)) lr(dst, src);
  oilf(dst, opnd);
}

// OR Pointer Size - dst = src & imm
void TurboAssembler::OrP(Register dst, Register src, const Operand& opnd) {
  if (!dst.is(src)) LoadRR(dst, src);
  OrP(dst, opnd);
}

// XOR 32-bit - dst = dst & src
void TurboAssembler::Xor(Register dst, Register src) { xr(dst, src); }

// XOR Pointer Size - dst = dst & src
void TurboAssembler::XorP(Register dst, Register src) { XorRR(dst, src); }

// Non-clobbering XOR 32-bit - dst = src1 & src1
void TurboAssembler::Xor(Register dst, Register src1, Register src2) {
  if (!dst.is(src1) && !dst.is(src2)) {
    // We prefer to generate XR/XGR, over the non clobbering XRK/XRK
    // as XR is a smaller instruction
    if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
      xrk(dst, src1, src2);
      return;
    } else {
      lr(dst, src1);
    }
  } else if (dst.is(src2)) {
    src2 = src1;
  }
  Xor(dst, src2);
}

// Non-clobbering XOR pointer size - dst = src1 & src1
void TurboAssembler::XorP(Register dst, Register src1, Register src2) {
  if (!dst.is(src1) && !dst.is(src2)) {
    // We prefer to generate XR/XGR, over the non clobbering XRK/XRK
    // as XR is a smaller instruction
    if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
      XorP_RRR(dst, src1, src2);
      return;
    } else {
      LoadRR(dst, src1);
    }
  } else if (dst.is(src2)) {
    src2 = src1;
  }
  XorP(dst, src2);
}

// XOR 32-bit (Reg - Mem)
void TurboAssembler::Xor(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
  if (is_uint12(opnd.offset()))
    x(dst, opnd);
  else
    xy(dst, opnd);
}

// XOR Pointer Size (Reg - Mem)
void TurboAssembler::XorP(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
#if V8_TARGET_ARCH_S390X
  xg(dst, opnd);
#else
  Xor(dst, opnd);
#endif
}

// XOR 32-bit - dst = dst & imm
void TurboAssembler::Xor(Register dst, const Operand& opnd) { xilf(dst, opnd); }

// XOR Pointer Size - dst = dst & imm
void TurboAssembler::XorP(Register dst, const Operand& opnd) {
#if V8_TARGET_ARCH_S390X
  intptr_t value = opnd.immediate();
  xihf(dst, Operand(value >> 32));
  xilf(dst, Operand(value & 0xFFFFFFFF));
#else
  Xor(dst, opnd);
#endif
}

// XOR 32-bit - dst = src & imm
void TurboAssembler::Xor(Register dst, Register src, const Operand& opnd) {
  if (!dst.is(src)) lr(dst, src);
  xilf(dst, opnd);
}

// XOR Pointer Size - dst = src & imm
void TurboAssembler::XorP(Register dst, Register src, const Operand& opnd) {
  if (!dst.is(src)) LoadRR(dst, src);
  XorP(dst, opnd);
}

void TurboAssembler::Not32(Register dst, Register src) {
  if (!src.is(no_reg) && !src.is(dst)) lr(dst, src);
  xilf(dst, Operand(0xFFFFFFFF));
}

void TurboAssembler::Not64(Register dst, Register src) {
  if (!src.is(no_reg) && !src.is(dst)) lgr(dst, src);
  xihf(dst, Operand(0xFFFFFFFF));
  xilf(dst, Operand(0xFFFFFFFF));
}

void TurboAssembler::NotP(Register dst, Register src) {
#if V8_TARGET_ARCH_S390X
  Not64(dst, src);
#else
  Not32(dst, src);
#endif
}

// works the same as mov
void TurboAssembler::Load(Register dst, const Operand& opnd) {
  intptr_t value = opnd.immediate();
  if (is_int16(value)) {
#if V8_TARGET_ARCH_S390X
    lghi(dst, opnd);
#else
    lhi(dst, opnd);
#endif
  } else if (is_int32(value)) {
#if V8_TARGET_ARCH_S390X
    lgfi(dst, opnd);
#else
    iilf(dst, opnd);
#endif
  } else if (is_uint32(value)) {
#if V8_TARGET_ARCH_S390X
    llilf(dst, opnd);
#else
    iilf(dst, opnd);
#endif
  } else {
    int32_t hi_32 = static_cast<int64_t>(value) >> 32;
    int32_t lo_32 = static_cast<int32_t>(value);

    iihf(dst, Operand(hi_32));
    iilf(dst, Operand(lo_32));
  }
}

void TurboAssembler::Load(Register dst, const MemOperand& opnd) {
  DCHECK(is_int20(opnd.offset()));
#if V8_TARGET_ARCH_S390X
  lgf(dst, opnd);  // 64<-32
#else
  if (is_uint12(opnd.offset())) {
    l(dst, opnd);
  } else {
    ly(dst, opnd);
  }
#endif
}

void TurboAssembler::LoadPositiveP(Register result, Register input) {
#if V8_TARGET_ARCH_S390X
  lpgr(result, input);
#else
  lpr(result, input);
#endif
}

void TurboAssembler::LoadPositive32(Register result, Register input) {
  lpr(result, input);
  lgfr(result, result);
}

//-----------------------------------------------------------------------------
//  Compare Helpers
//-----------------------------------------------------------------------------

// Compare 32-bit Register vs Register
void TurboAssembler::Cmp32(Register src1, Register src2) { cr_z(src1, src2); }

// Compare Pointer Sized Register vs Register
void TurboAssembler::CmpP(Register src1, Register src2) {
#if V8_TARGET_ARCH_S390X
  cgr(src1, src2);
#else
  Cmp32(src1, src2);
#endif
}

// Compare 32-bit Register vs Immediate
// This helper will set up proper relocation entries if required.
void TurboAssembler::Cmp32(Register dst, const Operand& opnd) {
  if (opnd.rmode() == kRelocInfo_NONEPTR) {
    intptr_t value = opnd.immediate();
    if (is_int16(value))
      chi(dst, opnd);
    else
      cfi(dst, opnd);
  } else {
    // Need to generate relocation record here
    RecordRelocInfo(opnd.rmode(), opnd.immediate());
    cfi(dst, opnd);
  }
}

// Compare Pointer Sized  Register vs Immediate
// This helper will set up proper relocation entries if required.
void TurboAssembler::CmpP(Register dst, const Operand& opnd) {
#if V8_TARGET_ARCH_S390X
  if (opnd.rmode() == kRelocInfo_NONEPTR) {
    cgfi(dst, opnd);
  } else {
    mov(r0, opnd);  // Need to generate 64-bit relocation
    cgr(dst, r0);
  }
#else
  Cmp32(dst, opnd);
#endif
}

// Compare 32-bit Register vs Memory
void TurboAssembler::Cmp32(Register dst, const MemOperand& opnd) {
  // make sure offset is within 20 bit range
  DCHECK(is_int20(opnd.offset()));
  if (is_uint12(opnd.offset()))
    c(dst, opnd);
  else
    cy(dst, opnd);
}

// Compare Pointer Size Register vs Memory
void TurboAssembler::CmpP(Register dst, const MemOperand& opnd) {
  // make sure offset is within 20 bit range
  DCHECK(is_int20(opnd.offset()));
#if V8_TARGET_ARCH_S390X
  cg(dst, opnd);
#else
  Cmp32(dst, opnd);
#endif
}

//-----------------------------------------------------------------------------
// Compare Logical Helpers
//-----------------------------------------------------------------------------

// Compare Logical 32-bit Register vs Register
void TurboAssembler::CmpLogical32(Register dst, Register src) { clr(dst, src); }

// Compare Logical Pointer Sized Register vs Register
void TurboAssembler::CmpLogicalP(Register dst, Register src) {
#ifdef V8_TARGET_ARCH_S390X
  clgr(dst, src);
#else
  CmpLogical32(dst, src);
#endif
}

// Compare Logical 32-bit Register vs Immediate
void TurboAssembler::CmpLogical32(Register dst, const Operand& opnd) {
  clfi(dst, opnd);
}

// Compare Logical Pointer Sized Register vs Immediate
void TurboAssembler::CmpLogicalP(Register dst, const Operand& opnd) {
#if V8_TARGET_ARCH_S390X
  DCHECK(static_cast<uint32_t>(opnd.immediate() >> 32) == 0);
  clgfi(dst, opnd);
#else
  CmpLogical32(dst, opnd);
#endif
}

// Compare Logical 32-bit Register vs Memory
void TurboAssembler::CmpLogical32(Register dst, const MemOperand& opnd) {
  // make sure offset is within 20 bit range
  DCHECK(is_int20(opnd.offset()));
  if (is_uint12(opnd.offset()))
    cl(dst, opnd);
  else
    cly(dst, opnd);
}

// Compare Logical Pointer Sized Register vs Memory
void TurboAssembler::CmpLogicalP(Register dst, const MemOperand& opnd) {
  // make sure offset is within 20 bit range
  DCHECK(is_int20(opnd.offset()));
#if V8_TARGET_ARCH_S390X
  clg(dst, opnd);
#else
  CmpLogical32(dst, opnd);
#endif
}

// Compare Logical Byte (Mem - Imm)
void TurboAssembler::CmpLogicalByte(const MemOperand& mem, const Operand& imm) {
  DCHECK(is_uint8(imm.immediate()));
  if (is_uint12(mem.offset()))
    cli(mem, imm);
  else
    cliy(mem, imm);
}

void TurboAssembler::Branch(Condition c, const Operand& opnd) {
  intptr_t value = opnd.immediate();
  if (is_int16(value))
    brc(c, opnd);
  else
    brcl(c, opnd);
}

// Branch On Count.  Decrement R1, and branch if R1 != 0.
void TurboAssembler::BranchOnCount(Register r1, Label* l) {
  int32_t offset = branch_offset(l);
  if (is_int16(offset)) {
#if V8_TARGET_ARCH_S390X
    brctg(r1, Operand(offset));
#else
    brct(r1, Operand(offset));
#endif
  } else {
    AddP(r1, Operand(-1));
    Branch(ne, Operand(offset));
  }
}

void TurboAssembler::LoadIntLiteral(Register dst, int value) {
  Load(dst, Operand(value));
}

void TurboAssembler::LoadSmiLiteral(Register dst, Smi* smi) {
  intptr_t value = reinterpret_cast<intptr_t>(smi);
#if V8_TARGET_ARCH_S390X
  DCHECK((value & 0xffffffff) == 0);
  // The smi value is loaded in upper 32-bits.  Lower 32-bit are zeros.
  llihf(dst, Operand(value >> 32));
#else
  llilf(dst, Operand(value));
#endif
}

void TurboAssembler::LoadDoubleLiteral(DoubleRegister result, uint64_t value,
                                       Register scratch) {
  uint32_t hi_32 = value >> 32;
  uint32_t lo_32 = static_cast<uint32_t>(value);

  // Load the 64-bit value into a GPR, then transfer it to FPR via LDGR
  if (value == 0) {
    lzdr(result);
  } else if (lo_32 == 0) {
    llihf(scratch, Operand(hi_32));
    ldgr(result, scratch);
  } else {
    iihf(scratch, Operand(hi_32));
    iilf(scratch, Operand(lo_32));
    ldgr(result, scratch);
  }
}

void TurboAssembler::LoadDoubleLiteral(DoubleRegister result, double value,
                                       Register scratch) {
  uint64_t int_val = bit_cast<uint64_t, double>(value);
  LoadDoubleLiteral(result, int_val, scratch);
}

void TurboAssembler::LoadFloat32Literal(DoubleRegister result, float value,
                                        Register scratch) {
  uint64_t int_val = static_cast<uint64_t>(bit_cast<uint32_t, float>(value))
                     << 32;
  LoadDoubleLiteral(result, int_val, scratch);
}

void TurboAssembler::CmpSmiLiteral(Register src1, Smi* smi, Register scratch) {
#if V8_TARGET_ARCH_S390X
  if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    cih(src1, Operand(reinterpret_cast<intptr_t>(smi) >> 32));
  } else {
    LoadSmiLiteral(scratch, smi);
    cgr(src1, scratch);
  }
#else
  // CFI takes 32-bit immediate.
  cfi(src1, Operand(smi));
#endif
}

void TurboAssembler::CmpLogicalSmiLiteral(Register src1, Smi* smi,
                                          Register scratch) {
#if V8_TARGET_ARCH_S390X
  if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    clih(src1, Operand(reinterpret_cast<intptr_t>(smi) >> 32));
  } else {
    LoadSmiLiteral(scratch, smi);
    clgr(src1, scratch);
  }
#else
  // CLFI takes 32-bit immediate
  clfi(src1, Operand(smi));
#endif
}

void TurboAssembler::AddSmiLiteral(Register dst, Register src, Smi* smi,
                                   Register scratch) {
#if V8_TARGET_ARCH_S390X
  if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    if (!dst.is(src)) LoadRR(dst, src);
    aih(dst, Operand(reinterpret_cast<intptr_t>(smi) >> 32));
  } else {
    LoadSmiLiteral(scratch, smi);
    AddP(dst, src, scratch);
  }
#else
  AddP(dst, src, Operand(reinterpret_cast<intptr_t>(smi)));
#endif
}

void TurboAssembler::SubSmiLiteral(Register dst, Register src, Smi* smi,
                                   Register scratch) {
#if V8_TARGET_ARCH_S390X
  if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    if (!dst.is(src)) LoadRR(dst, src);
    aih(dst, Operand((-reinterpret_cast<intptr_t>(smi)) >> 32));
  } else {
    LoadSmiLiteral(scratch, smi);
    SubP(dst, src, scratch);
  }
#else
  AddP(dst, src, Operand(-(reinterpret_cast<intptr_t>(smi))));
#endif
}

void TurboAssembler::AndSmiLiteral(Register dst, Register src, Smi* smi) {
  if (!dst.is(src)) LoadRR(dst, src);
#if V8_TARGET_ARCH_S390X
  DCHECK((reinterpret_cast<intptr_t>(smi) & 0xffffffff) == 0);
  int value = static_cast<int>(reinterpret_cast<intptr_t>(smi) >> 32);
  nihf(dst, Operand(value));
#else
  nilf(dst, Operand(reinterpret_cast<int>(smi)));
#endif
}

// Load a "pointer" sized value from the memory location
void TurboAssembler::LoadP(Register dst, const MemOperand& mem,
                           Register scratch) {
  int offset = mem.offset();

  if (!scratch.is(no_reg) && !is_int20(offset)) {
    /* cannot use d-form */
    LoadIntLiteral(scratch, offset);
#if V8_TARGET_ARCH_S390X
    lg(dst, MemOperand(mem.rb(), scratch));
#else
    l(dst, MemOperand(mem.rb(), scratch));
#endif
  } else {
#if V8_TARGET_ARCH_S390X
    lg(dst, mem);
#else
    if (is_uint12(offset)) {
      l(dst, mem);
    } else {
      ly(dst, mem);
    }
#endif
  }
}

// Store a "pointer" sized value to the memory location
void TurboAssembler::StoreP(Register src, const MemOperand& mem,
                            Register scratch) {
  if (!is_int20(mem.offset())) {
    DCHECK(!scratch.is(no_reg));
    DCHECK(!scratch.is(r0));
    LoadIntLiteral(scratch, mem.offset());
#if V8_TARGET_ARCH_S390X
    stg(src, MemOperand(mem.rb(), scratch));
#else
    st(src, MemOperand(mem.rb(), scratch));
#endif
  } else {
#if V8_TARGET_ARCH_S390X
    stg(src, mem);
#else
    // StoreW will try to generate ST if offset fits, otherwise
    // it'll generate STY.
    StoreW(src, mem);
#endif
  }
}

// Store a "pointer" sized constant to the memory location
void TurboAssembler::StoreP(const MemOperand& mem, const Operand& opnd,
                            Register scratch) {
  // Relocations not supported
  DCHECK(opnd.rmode() == kRelocInfo_NONEPTR);

  // Try to use MVGHI/MVHI
  if (CpuFeatures::IsSupported(GENERAL_INSTR_EXT) && is_uint12(mem.offset()) &&
      mem.getIndexRegister().is(r0) && is_int16(opnd.immediate())) {
#if V8_TARGET_ARCH_S390X
    mvghi(mem, opnd);
#else
    mvhi(mem, opnd);
#endif
  } else {
    LoadImmP(scratch, opnd);
    StoreP(scratch, mem);
  }
}

void TurboAssembler::LoadMultipleP(Register dst1, Register dst2,
                                   const MemOperand& mem) {
#if V8_TARGET_ARCH_S390X
  DCHECK(is_int20(mem.offset()));
  lmg(dst1, dst2, mem);
#else
  if (is_uint12(mem.offset())) {
    lm(dst1, dst2, mem);
  } else {
    DCHECK(is_int20(mem.offset()));
    lmy(dst1, dst2, mem);
  }
#endif
}

void TurboAssembler::StoreMultipleP(Register src1, Register src2,
                                    const MemOperand& mem) {
#if V8_TARGET_ARCH_S390X
  DCHECK(is_int20(mem.offset()));
  stmg(src1, src2, mem);
#else
  if (is_uint12(mem.offset())) {
    stm(src1, src2, mem);
  } else {
    DCHECK(is_int20(mem.offset()));
    stmy(src1, src2, mem);
  }
#endif
}

void TurboAssembler::LoadMultipleW(Register dst1, Register dst2,
                                   const MemOperand& mem) {
  if (is_uint12(mem.offset())) {
    lm(dst1, dst2, mem);
  } else {
    DCHECK(is_int20(mem.offset()));
    lmy(dst1, dst2, mem);
  }
}

void TurboAssembler::StoreMultipleW(Register src1, Register src2,
                                    const MemOperand& mem) {
  if (is_uint12(mem.offset())) {
    stm(src1, src2, mem);
  } else {
    DCHECK(is_int20(mem.offset()));
    stmy(src1, src2, mem);
  }
}

// Load 32-bits and sign extend if necessary.
void TurboAssembler::LoadW(Register dst, Register src) {
#if V8_TARGET_ARCH_S390X
  lgfr(dst, src);
#else
  if (!dst.is(src)) lr(dst, src);
#endif
}

// Load 32-bits and sign extend if necessary.
void TurboAssembler::LoadW(Register dst, const MemOperand& mem,
                           Register scratch) {
  int offset = mem.offset();

  if (!is_int20(offset)) {
    DCHECK(!scratch.is(no_reg));
    LoadIntLiteral(scratch, offset);
#if V8_TARGET_ARCH_S390X
    lgf(dst, MemOperand(mem.rb(), scratch));
#else
    l(dst, MemOperand(mem.rb(), scratch));
#endif
  } else {
#if V8_TARGET_ARCH_S390X
    lgf(dst, mem);
#else
    if (is_uint12(offset)) {
      l(dst, mem);
    } else {
      ly(dst, mem);
    }
#endif
  }
}

// Load 32-bits and zero extend if necessary.
void TurboAssembler::LoadlW(Register dst, Register src) {
#if V8_TARGET_ARCH_S390X
  llgfr(dst, src);
#else
  if (!dst.is(src)) lr(dst, src);
#endif
}

// Variable length depending on whether offset fits into immediate field
// MemOperand of RX or RXY format
void TurboAssembler::LoadlW(Register dst, const MemOperand& mem,
                            Register scratch) {
  Register base = mem.rb();
  int offset = mem.offset();

#if V8_TARGET_ARCH_S390X
  if (is_int20(offset)) {
    llgf(dst, mem);
  } else if (!scratch.is(no_reg)) {
    // Materialize offset into scratch register.
    LoadIntLiteral(scratch, offset);
    llgf(dst, MemOperand(base, scratch));
  } else {
    DCHECK(false);
  }
#else
  bool use_RXform = false;
  bool use_RXYform = false;
  if (is_uint12(offset)) {
    // RX-format supports unsigned 12-bits offset.
    use_RXform = true;
  } else if (is_int20(offset)) {
    // RXY-format supports signed 20-bits offset.
    use_RXYform = true;
  } else if (!scratch.is(no_reg)) {
    // Materialize offset into scratch register.
    LoadIntLiteral(scratch, offset);
  } else {
    DCHECK(false);
  }

  if (use_RXform) {
    l(dst, mem);
  } else if (use_RXYform) {
    ly(dst, mem);
  } else {
    ly(dst, MemOperand(base, scratch));
  }
#endif
}

void TurboAssembler::LoadLogicalHalfWordP(Register dst, const MemOperand& mem) {
#if V8_TARGET_ARCH_S390X
  llgh(dst, mem);
#else
  llh(dst, mem);
#endif
}

void TurboAssembler::LoadLogicalHalfWordP(Register dst, Register src) {
#if V8_TARGET_ARCH_S390X
  llghr(dst, src);
#else
  llhr(dst, src);
#endif
}

void TurboAssembler::LoadB(Register dst, const MemOperand& mem) {
#if V8_TARGET_ARCH_S390X
  lgb(dst, mem);
#else
  lb(dst, mem);
#endif
}

void TurboAssembler::LoadB(Register dst, Register src) {
#if V8_TARGET_ARCH_S390X
  lgbr(dst, src);
#else
  lbr(dst, src);
#endif
}

void TurboAssembler::LoadlB(Register dst, const MemOperand& mem) {
#if V8_TARGET_ARCH_S390X
  llgc(dst, mem);
#else
  llc(dst, mem);
#endif
}

void TurboAssembler::LoadlB(Register dst, Register src) {
#if V8_TARGET_ARCH_S390X
  llgcr(dst, src);
#else
  llcr(dst, src);
#endif
}

void TurboAssembler::LoadLogicalReversedWordP(Register dst,
                                              const MemOperand& mem) {
  lrv(dst, mem);
  LoadlW(dst, dst);
}

void TurboAssembler::LoadLogicalReversedHalfWordP(Register dst,
                                                  const MemOperand& mem) {
  lrvh(dst, mem);
  LoadLogicalHalfWordP(dst, dst);
}


// Load And Test (Reg <- Reg)
void TurboAssembler::LoadAndTest32(Register dst, Register src) {
  ltr(dst, src);
}

// Load And Test
//     (Register dst(ptr) = Register src (32 | 32->64))
// src is treated as a 32-bit signed integer, which is sign extended to
// 64-bit if necessary.
void TurboAssembler::LoadAndTestP_ExtendSrc(Register dst, Register src) {
#if V8_TARGET_ARCH_S390X
  ltgfr(dst, src);
#else
  ltr(dst, src);
#endif
}

// Load And Test Pointer Sized (Reg <- Reg)
void TurboAssembler::LoadAndTestP(Register dst, Register src) {
#if V8_TARGET_ARCH_S390X
  ltgr(dst, src);
#else
  ltr(dst, src);
#endif
}

// Load And Test 32-bit (Reg <- Mem)
void TurboAssembler::LoadAndTest32(Register dst, const MemOperand& mem) {
  lt_z(dst, mem);
}

// Load And Test Pointer Sized (Reg <- Mem)
void TurboAssembler::LoadAndTestP(Register dst, const MemOperand& mem) {
#if V8_TARGET_ARCH_S390X
  ltg(dst, mem);
#else
  lt_z(dst, mem);
#endif
}

// Load On Condition Pointer Sized (Reg <- Reg)
void TurboAssembler::LoadOnConditionP(Condition cond, Register dst,
                                      Register src) {
#if V8_TARGET_ARCH_S390X
  locgr(cond, dst, src);
#else
  locr(cond, dst, src);
#endif
}

// Load Double Precision (64-bit) Floating Point number from memory
void TurboAssembler::LoadDouble(DoubleRegister dst, const MemOperand& mem) {
  // for 32bit and 64bit we all use 64bit floating point regs
  if (is_uint12(mem.offset())) {
    ld(dst, mem);
  } else {
    ldy(dst, mem);
  }
}

// Load Single Precision (32-bit) Floating Point number from memory
void TurboAssembler::LoadFloat32(DoubleRegister dst, const MemOperand& mem) {
  if (is_uint12(mem.offset())) {
    le_z(dst, mem);
  } else {
    DCHECK(is_int20(mem.offset()));
    ley(dst, mem);
  }
}

// Load Single Precision (32-bit) Floating Point number from memory,
// and convert to Double Precision (64-bit)
void TurboAssembler::LoadFloat32ConvertToDouble(DoubleRegister dst,
                                                const MemOperand& mem) {
  LoadFloat32(dst, mem);
  ldebr(dst, dst);
}

// Store Double Precision (64-bit) Floating Point number to memory
void TurboAssembler::StoreDouble(DoubleRegister dst, const MemOperand& mem) {
  if (is_uint12(mem.offset())) {
    std(dst, mem);
  } else {
    stdy(dst, mem);
  }
}

// Store Single Precision (32-bit) Floating Point number to memory
void TurboAssembler::StoreFloat32(DoubleRegister src, const MemOperand& mem) {
  if (is_uint12(mem.offset())) {
    ste(src, mem);
  } else {
    stey(src, mem);
  }
}

// Convert Double precision (64-bit) to Single Precision (32-bit)
// and store resulting Float32 to memory
void TurboAssembler::StoreDoubleAsFloat32(DoubleRegister src,
                                          const MemOperand& mem,
                                          DoubleRegister scratch) {
  ledbr(scratch, src);
  StoreFloat32(scratch, mem);
}

void TurboAssembler::AddFloat32(DoubleRegister dst, const MemOperand& opnd,
                                DoubleRegister scratch) {
  if (is_uint12(opnd.offset())) {
    aeb(dst, opnd);
  } else {
    ley(scratch, opnd);
    aebr(dst, scratch);
  }
}

void TurboAssembler::AddFloat64(DoubleRegister dst, const MemOperand& opnd,
                                DoubleRegister scratch) {
  if (is_uint12(opnd.offset())) {
    adb(dst, opnd);
  } else {
    ldy(scratch, opnd);
    adbr(dst, scratch);
  }
}

void TurboAssembler::SubFloat32(DoubleRegister dst, const MemOperand& opnd,
                                DoubleRegister scratch) {
  if (is_uint12(opnd.offset())) {
    seb(dst, opnd);
  } else {
    ley(scratch, opnd);
    sebr(dst, scratch);
  }
}

void TurboAssembler::SubFloat64(DoubleRegister dst, const MemOperand& opnd,
                                DoubleRegister scratch) {
  if (is_uint12(opnd.offset())) {
    sdb(dst, opnd);
  } else {
    ldy(scratch, opnd);
    sdbr(dst, scratch);
  }
}

void TurboAssembler::MulFloat32(DoubleRegister dst, const MemOperand& opnd,
                                DoubleRegister scratch) {
  if (is_uint12(opnd.offset())) {
    meeb(dst, opnd);
  } else {
    ley(scratch, opnd);
    meebr(dst, scratch);
  }
}

void TurboAssembler::MulFloat64(DoubleRegister dst, const MemOperand& opnd,
                                DoubleRegister scratch) {
  if (is_uint12(opnd.offset())) {
    mdb(dst, opnd);
  } else {
    ldy(scratch, opnd);
    mdbr(dst, scratch);
  }
}

void TurboAssembler::DivFloat32(DoubleRegister dst, const MemOperand& opnd,
                                DoubleRegister scratch) {
  if (is_uint12(opnd.offset())) {
    deb(dst, opnd);
  } else {
    ley(scratch, opnd);
    debr(dst, scratch);
  }
}

void TurboAssembler::DivFloat64(DoubleRegister dst, const MemOperand& opnd,
                                DoubleRegister scratch) {
  if (is_uint12(opnd.offset())) {
    ddb(dst, opnd);
  } else {
    ldy(scratch, opnd);
    ddbr(dst, scratch);
  }
}

void TurboAssembler::LoadFloat32ToDouble(DoubleRegister dst,
                                         const MemOperand& opnd,
                                         DoubleRegister scratch) {
  if (is_uint12(opnd.offset())) {
    ldeb(dst, opnd);
  } else {
    ley(scratch, opnd);
    ldebr(dst, scratch);
  }
}

// Variable length depending on whether offset fits into immediate field
// MemOperand of RX or RXY format
void TurboAssembler::StoreW(Register src, const MemOperand& mem,
                            Register scratch) {
  Register base = mem.rb();
  int offset = mem.offset();

  bool use_RXform = false;
  bool use_RXYform = false;

  if (is_uint12(offset)) {
    // RX-format supports unsigned 12-bits offset.
    use_RXform = true;
  } else if (is_int20(offset)) {
    // RXY-format supports signed 20-bits offset.
    use_RXYform = true;
  } else if (!scratch.is(no_reg)) {
    // Materialize offset into scratch register.
    LoadIntLiteral(scratch, offset);
  } else {
    // scratch is no_reg
    DCHECK(false);
  }

  if (use_RXform) {
    st(src, mem);
  } else if (use_RXYform) {
    sty(src, mem);
  } else {
    StoreW(src, MemOperand(base, scratch));
  }
}

// Loads 16-bits half-word value from memory and sign extends to pointer
// sized register
void TurboAssembler::LoadHalfWordP(Register dst, const MemOperand& mem,
                                   Register scratch) {
  Register base = mem.rb();
  int offset = mem.offset();

  if (!is_int20(offset)) {
    DCHECK(!scratch.is(no_reg));
    LoadIntLiteral(scratch, offset);
#if V8_TARGET_ARCH_S390X
    lgh(dst, MemOperand(base, scratch));
#else
    lh(dst, MemOperand(base, scratch));
#endif
  } else {
#if V8_TARGET_ARCH_S390X
    lgh(dst, mem);
#else
    if (is_uint12(offset)) {
      lh(dst, mem);
    } else {
      lhy(dst, mem);
    }
#endif
  }
}

// Variable length depending on whether offset fits into immediate field
// MemOperand current only supports d-form
void TurboAssembler::StoreHalfWord(Register src, const MemOperand& mem,
                                   Register scratch) {
  Register base = mem.rb();
  int offset = mem.offset();

  if (is_uint12(offset)) {
    sth(src, mem);
  } else if (is_int20(offset)) {
    sthy(src, mem);
  } else {
    DCHECK(!scratch.is(no_reg));
    LoadIntLiteral(scratch, offset);
    sth(src, MemOperand(base, scratch));
  }
}

// Variable length depending on whether offset fits into immediate field
// MemOperand current only supports d-form
void TurboAssembler::StoreByte(Register src, const MemOperand& mem,
                               Register scratch) {
  Register base = mem.rb();
  int offset = mem.offset();

  if (is_uint12(offset)) {
    stc(src, mem);
  } else if (is_int20(offset)) {
    stcy(src, mem);
  } else {
    DCHECK(!scratch.is(no_reg));
    LoadIntLiteral(scratch, offset);
    stc(src, MemOperand(base, scratch));
  }
}

// Shift left logical for 32-bit integer types.
void TurboAssembler::ShiftLeft(Register dst, Register src, const Operand& val) {
  if (dst.is(src)) {
    sll(dst, val);
  } else if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    sllk(dst, src, val);
  } else {
    lr(dst, src);
    sll(dst, val);
  }
}

// Shift left logical for 32-bit integer types.
void TurboAssembler::ShiftLeft(Register dst, Register src, Register val) {
  if (dst.is(src)) {
    sll(dst, val);
  } else if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    sllk(dst, src, val);
  } else {
    DCHECK(!dst.is(val));  // The lr/sll path clobbers val.
    lr(dst, src);
    sll(dst, val);
  }
}

// Shift right logical for 32-bit integer types.
void TurboAssembler::ShiftRight(Register dst, Register src,
                                const Operand& val) {
  if (dst.is(src)) {
    srl(dst, val);
  } else if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    srlk(dst, src, val);
  } else {
    lr(dst, src);
    srl(dst, val);
  }
}

// Shift right logical for 32-bit integer types.
void TurboAssembler::ShiftRight(Register dst, Register src, Register val) {
  if (dst.is(src)) {
    srl(dst, val);
  } else if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    srlk(dst, src, val);
  } else {
    DCHECK(!dst.is(val));  // The lr/srl path clobbers val.
    lr(dst, src);
    srl(dst, val);
  }
}

// Shift left arithmetic for 32-bit integer types.
void TurboAssembler::ShiftLeftArith(Register dst, Register src,
                                    const Operand& val) {
  if (dst.is(src)) {
    sla(dst, val);
  } else if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    slak(dst, src, val);
  } else {
    lr(dst, src);
    sla(dst, val);
  }
}

// Shift left arithmetic for 32-bit integer types.
void TurboAssembler::ShiftLeftArith(Register dst, Register src, Register val) {
  if (dst.is(src)) {
    sla(dst, val);
  } else if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    slak(dst, src, val);
  } else {
    DCHECK(!dst.is(val));  // The lr/sla path clobbers val.
    lr(dst, src);
    sla(dst, val);
  }
}

// Shift right arithmetic for 32-bit integer types.
void TurboAssembler::ShiftRightArith(Register dst, Register src,
                                     const Operand& val) {
  if (dst.is(src)) {
    sra(dst, val);
  } else if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    srak(dst, src, val);
  } else {
    lr(dst, src);
    sra(dst, val);
  }
}

// Shift right arithmetic for 32-bit integer types.
void TurboAssembler::ShiftRightArith(Register dst, Register src, Register val) {
  if (dst.is(src)) {
    sra(dst, val);
  } else if (CpuFeatures::IsSupported(DISTINCT_OPS)) {
    srak(dst, src, val);
  } else {
    DCHECK(!dst.is(val));  // The lr/sra path clobbers val.
    lr(dst, src);
    sra(dst, val);
  }
}

// Clear right most # of bits
void TurboAssembler::ClearRightImm(Register dst, Register src,
                                   const Operand& val) {
  int numBitsToClear = val.immediate() % (kPointerSize * 8);

  // Try to use RISBG if possible
  if (CpuFeatures::IsSupported(GENERAL_INSTR_EXT)) {
    int endBit = 63 - numBitsToClear;
    risbg(dst, src, Operand::Zero(), Operand(endBit), Operand::Zero(), true);
    return;
  }

  uint64_t hexMask = ~((1L << numBitsToClear) - 1);

  // S390 AND instr clobbers source.  Make a copy if necessary
  if (!dst.is(src)) LoadRR(dst, src);

  if (numBitsToClear <= 16) {
    nill(dst, Operand(static_cast<uint16_t>(hexMask)));
  } else if (numBitsToClear <= 32) {
    nilf(dst, Operand(static_cast<uint32_t>(hexMask)));
  } else if (numBitsToClear <= 64) {
    nilf(dst, Operand(static_cast<intptr_t>(0)));
    nihf(dst, Operand(hexMask >> 32));
  }
}

void TurboAssembler::Popcnt32(Register dst, Register src) {
  DCHECK(!src.is(r0));
  DCHECK(!dst.is(r0));

  popcnt(dst, src);
  ShiftRight(r0, dst, Operand(16));
  ar(dst, r0);
  ShiftRight(r0, dst, Operand(8));
  ar(dst, r0);
  llgcr(dst, dst);
}

#ifdef V8_TARGET_ARCH_S390X
void TurboAssembler::Popcnt64(Register dst, Register src) {
  DCHECK(!src.is(r0));
  DCHECK(!dst.is(r0));

  popcnt(dst, src);
  ShiftRightP(r0, dst, Operand(32));
  AddP(dst, r0);
  ShiftRightP(r0, dst, Operand(16));
  AddP(dst, r0);
  ShiftRightP(r0, dst, Operand(8));
  AddP(dst, r0);
  LoadlB(dst, dst);
}
#endif

#ifdef DEBUG
bool AreAliased(Register reg1, Register reg2, Register reg3, Register reg4,
                Register reg5, Register reg6, Register reg7, Register reg8,
                Register reg9, Register reg10) {
  int n_of_valid_regs = reg1.is_valid() + reg2.is_valid() + reg3.is_valid() +
                        reg4.is_valid() + reg5.is_valid() + reg6.is_valid() +
                        reg7.is_valid() + reg8.is_valid() + reg9.is_valid() +
                        reg10.is_valid();

  RegList regs = 0;
  if (reg1.is_valid()) regs |= reg1.bit();
  if (reg2.is_valid()) regs |= reg2.bit();
  if (reg3.is_valid()) regs |= reg3.bit();
  if (reg4.is_valid()) regs |= reg4.bit();
  if (reg5.is_valid()) regs |= reg5.bit();
  if (reg6.is_valid()) regs |= reg6.bit();
  if (reg7.is_valid()) regs |= reg7.bit();
  if (reg8.is_valid()) regs |= reg8.bit();
  if (reg9.is_valid()) regs |= reg9.bit();
  if (reg10.is_valid()) regs |= reg10.bit();
  int n_of_non_aliasing_regs = NumRegs(regs);

  return n_of_valid_regs != n_of_non_aliasing_regs;
}
#endif

CodePatcher::CodePatcher(Isolate* isolate, byte* address, int size,
                         FlushICache flush_cache)
    : address_(address),
      size_(size),
      masm_(isolate, address, size_ + Assembler::kGap, CodeObjectRequired::kNo),
      flush_cache_(flush_cache) {
  // Create a new macro assembler pointing to the address of the code to patch.
  // The size is adjusted with kGap on order for the assembler to generate size
  // bytes of instructions without failing with buffer size constraints.
  DCHECK(masm_.reloc_info_writer.pos() == address_ + size_ + Assembler::kGap);
}

CodePatcher::~CodePatcher() {
  // Indicate that code has changed.
  if (flush_cache_ == FLUSH) {
    Assembler::FlushICache(masm_.isolate(), address_, size_);
  }

  // Check that the code was patched as expected.
  DCHECK(masm_.pc_ == address_ + size_);
  DCHECK(masm_.reloc_info_writer.pos() == address_ + size_ + Assembler::kGap);
}

}  // namespace internal
}  // namespace v8

#endif  // V8_TARGET_ARCH_S390
