// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if V8_TARGET_ARCH_MIPS

#include "src/api-arguments.h"
#include "src/base/bits.h"
#include "src/bootstrapper.h"
#include "src/code-stubs.h"
#include "src/codegen.h"
#include "src/frame-constants.h"
#include "src/frames.h"
#include "src/ic/handler-compiler.h"
#include "src/ic/ic.h"
#include "src/ic/stub-cache.h"
#include "src/isolate.h"
#include "src/regexp/jsregexp.h"
#include "src/regexp/regexp-macro-assembler.h"
#include "src/runtime/runtime.h"

#include "src/mips/code-stubs-mips.h"  // Cannot be the first include.

namespace v8 {
namespace internal {

#define __ ACCESS_MASM(masm)

void ArrayNArgumentsConstructorStub::Generate(MacroAssembler* masm) {
  __ sll(t9, a0, kPointerSizeLog2);
  __ Addu(t9, sp, t9);
  __ sw(a1, MemOperand(t9, 0));
  __ Push(a1);
  __ Push(a2);
  __ Addu(a0, a0, Operand(3));
  __ TailCallRuntime(Runtime::kNewArray);
}


void DoubleToIStub::Generate(MacroAssembler* masm) {
  Label out_of_range, only_low, negate, done;
  Register input_reg = source();
  Register result_reg = destination();

  int double_offset = offset();
  // Account for saved regs if input is sp.
  if (input_reg.is(sp)) double_offset += 3 * kPointerSize;

  Register scratch =
      GetRegisterThatIsNotOneOf(input_reg, result_reg);
  Register scratch2 =
      GetRegisterThatIsNotOneOf(input_reg, result_reg, scratch);
  Register scratch3 =
      GetRegisterThatIsNotOneOf(input_reg, result_reg, scratch, scratch2);
  DoubleRegister double_scratch = kLithiumScratchDouble;

  __ Push(scratch, scratch2, scratch3);

  if (!skip_fastpath()) {
    // Load double input.
    __ Ldc1(double_scratch, MemOperand(input_reg, double_offset));

    // Clear cumulative exception flags and save the FCSR.
    __ cfc1(scratch2, FCSR);
    __ ctc1(zero_reg, FCSR);

    // Try a conversion to a signed integer.
    __ Trunc_w_d(double_scratch, double_scratch);
    // Move the converted value into the result register.
    __ mfc1(scratch3, double_scratch);

    // Retrieve and restore the FCSR.
    __ cfc1(scratch, FCSR);
    __ ctc1(scratch2, FCSR);

    // Check for overflow and NaNs.
    __ And(
        scratch, scratch,
        kFCSROverflowFlagMask | kFCSRUnderflowFlagMask
           | kFCSRInvalidOpFlagMask);
    // If we had no exceptions then set result_reg and we are done.
    Label error;
    __ Branch(&error, ne, scratch, Operand(zero_reg));
    __ Move(result_reg, scratch3);
    __ Branch(&done);
    __ bind(&error);
  }

  // Load the double value and perform a manual truncation.
  Register input_high = scratch2;
  Register input_low = scratch3;

  __ lw(input_low,
      MemOperand(input_reg, double_offset + Register::kMantissaOffset));
  __ lw(input_high,
      MemOperand(input_reg, double_offset + Register::kExponentOffset));

  Label normal_exponent, restore_sign;
  // Extract the biased exponent in result.
  __ Ext(result_reg,
         input_high,
         HeapNumber::kExponentShift,
         HeapNumber::kExponentBits);

  // Check for Infinity and NaNs, which should return 0.
  __ Subu(scratch, result_reg, HeapNumber::kExponentMask);
  __ Movz(result_reg, zero_reg, scratch);
  __ Branch(&done, eq, scratch, Operand(zero_reg));

  // Express exponent as delta to (number of mantissa bits + 31).
  __ Subu(result_reg,
          result_reg,
          Operand(HeapNumber::kExponentBias + HeapNumber::kMantissaBits + 31));

  // If the delta is strictly positive, all bits would be shifted away,
  // which means that we can return 0.
  __ Branch(&normal_exponent, le, result_reg, Operand(zero_reg));
  __ mov(result_reg, zero_reg);
  __ Branch(&done);

  __ bind(&normal_exponent);
  const int kShiftBase = HeapNumber::kNonMantissaBitsInTopWord - 1;
  // Calculate shift.
  __ Addu(scratch, result_reg, Operand(kShiftBase + HeapNumber::kMantissaBits));

  // Save the sign.
  Register sign = result_reg;
  result_reg = no_reg;
  __ And(sign, input_high, Operand(HeapNumber::kSignMask));

  // On ARM shifts > 31 bits are valid and will result in zero. On MIPS we need
  // to check for this specific case.
  Label high_shift_needed, high_shift_done;
  __ Branch(&high_shift_needed, lt, scratch, Operand(32));
  __ mov(input_high, zero_reg);
  __ Branch(&high_shift_done);
  __ bind(&high_shift_needed);

  // Set the implicit 1 before the mantissa part in input_high.
  __ Or(input_high,
        input_high,
        Operand(1 << HeapNumber::kMantissaBitsInTopWord));
  // Shift the mantissa bits to the correct position.
  // We don't need to clear non-mantissa bits as they will be shifted away.
  // If they weren't, it would mean that the answer is in the 32bit range.
  __ sllv(input_high, input_high, scratch);

  __ bind(&high_shift_done);

  // Replace the shifted bits with bits from the lower mantissa word.
  Label pos_shift, shift_done;
  __ li(at, 32);
  __ subu(scratch, at, scratch);
  __ Branch(&pos_shift, ge, scratch, Operand(zero_reg));

  // Negate scratch.
  __ Subu(scratch, zero_reg, scratch);
  __ sllv(input_low, input_low, scratch);
  __ Branch(&shift_done);

  __ bind(&pos_shift);
  __ srlv(input_low, input_low, scratch);

  __ bind(&shift_done);
  __ Or(input_high, input_high, Operand(input_low));
  // Restore sign if necessary.
  __ mov(scratch, sign);
  result_reg = sign;
  sign = no_reg;
  __ Subu(result_reg, zero_reg, input_high);
  __ Movz(result_reg, input_high, scratch);

  __ bind(&done);

  __ Pop(scratch, scratch2, scratch3);
  __ Ret();
}


void StoreRegistersStateStub::Generate(MacroAssembler* masm) {
  __ mov(t9, ra);
  __ pop(ra);
  __ PushSafepointRegisters();
  __ Jump(t9);
}


void RestoreRegistersStateStub::Generate(MacroAssembler* masm) {
  __ mov(t9, ra);
  __ pop(ra);
  __ PopSafepointRegisters();
  __ Jump(t9);
}


void StoreBufferOverflowStub::Generate(MacroAssembler* masm) {
  // We don't allow a GC during a store buffer overflow so there is no need to
  // store the registers in any particular way, but we do have to store and
  // restore them.
  __ MultiPush(kJSCallerSaved | ra.bit());
  if (save_doubles()) {
    __ MultiPushFPU(kCallerSavedFPU);
  }
  const int argument_count = 1;
  const int fp_argument_count = 0;
  const Register scratch = a1;

  AllowExternalCallThatCantCauseGC scope(masm);
  __ PrepareCallCFunction(argument_count, fp_argument_count, scratch);
  __ li(a0, Operand(ExternalReference::isolate_address(isolate())));
  __ CallCFunction(
      ExternalReference::store_buffer_overflow_function(isolate()),
      argument_count);
  if (save_doubles()) {
    __ MultiPopFPU(kCallerSavedFPU);
  }

  __ MultiPop(kJSCallerSaved | ra.bit());
  __ Ret();
}


void MathPowStub::Generate(MacroAssembler* masm) {
  const Register exponent = MathPowTaggedDescriptor::exponent();
  DCHECK(exponent.is(a2));
  const DoubleRegister double_base = f2;
  const DoubleRegister double_exponent = f4;
  const DoubleRegister double_result = f0;
  const DoubleRegister double_scratch = f6;
  const FPURegister single_scratch = f8;
  const Register scratch = t5;
  const Register scratch2 = t3;

  Label call_runtime, done, int_exponent;
  if (exponent_type() == TAGGED) {
    // Base is already in double_base.
    __ UntagAndJumpIfSmi(scratch, exponent, &int_exponent);

    __ Ldc1(double_exponent,
            FieldMemOperand(exponent, HeapNumber::kValueOffset));
  }

  if (exponent_type() != INTEGER) {
    Label int_exponent_convert;
    // Detect integer exponents stored as double.
    __ EmitFPUTruncate(kRoundToMinusInf,
                       scratch,
                       double_exponent,
                       at,
                       double_scratch,
                       scratch2,
                       kCheckForInexactConversion);
    // scratch2 == 0 means there was no conversion error.
    __ Branch(&int_exponent_convert, eq, scratch2, Operand(zero_reg));

    __ push(ra);
    {
      AllowExternalCallThatCantCauseGC scope(masm);
      __ PrepareCallCFunction(0, 2, scratch2);
      __ MovToFloatParameters(double_base, double_exponent);
      __ CallCFunction(
          ExternalReference::power_double_double_function(isolate()),
          0, 2);
    }
    __ pop(ra);
    __ MovFromFloatResult(double_result);
    __ jmp(&done);

    __ bind(&int_exponent_convert);
  }

  // Calculate power with integer exponent.
  __ bind(&int_exponent);

  // Get two copies of exponent in the registers scratch and exponent.
  if (exponent_type() == INTEGER) {
    __ mov(scratch, exponent);
  } else {
    // Exponent has previously been stored into scratch as untagged integer.
    __ mov(exponent, scratch);
  }

  __ mov_d(double_scratch, double_base);  // Back up base.
  __ Move(double_result, 1.0);

  // Get absolute value of exponent.
  Label positive_exponent, bail_out;
  __ Branch(&positive_exponent, ge, scratch, Operand(zero_reg));
  __ Subu(scratch, zero_reg, scratch);
  // Check when Subu overflows and we get negative result
  // (happens only when input is MIN_INT).
  __ Branch(&bail_out, gt, zero_reg, Operand(scratch));
  __ bind(&positive_exponent);
  __ Assert(ge, kUnexpectedNegativeValue, scratch, Operand(zero_reg));

  Label while_true, no_carry, loop_end;
  __ bind(&while_true);

  __ And(scratch2, scratch, 1);

  __ Branch(&no_carry, eq, scratch2, Operand(zero_reg));
  __ mul_d(double_result, double_result, double_scratch);
  __ bind(&no_carry);

  __ sra(scratch, scratch, 1);

  __ Branch(&loop_end, eq, scratch, Operand(zero_reg));
  __ mul_d(double_scratch, double_scratch, double_scratch);

  __ Branch(&while_true);

  __ bind(&loop_end);

  __ Branch(&done, ge, exponent, Operand(zero_reg));
  __ Move(double_scratch, 1.0);
  __ div_d(double_result, double_scratch, double_result);
  // Test whether result is zero.  Bail out to check for subnormal result.
  // Due to subnormals, x^-y == (1/x)^y does not hold in all cases.
  __ BranchF(&done, NULL, ne, double_result, kDoubleRegZero);

  // double_exponent may not contain the exponent value if the input was a
  // smi.  We set it with exponent value before bailing out.
  __ bind(&bail_out);
  __ mtc1(exponent, single_scratch);
  __ cvt_d_w(double_exponent, single_scratch);

  // Returning or bailing out.
  __ push(ra);
  {
    AllowExternalCallThatCantCauseGC scope(masm);
    __ PrepareCallCFunction(0, 2, scratch);
    __ MovToFloatParameters(double_base, double_exponent);
    __ CallCFunction(ExternalReference::power_double_double_function(isolate()),
                     0, 2);
  }
  __ pop(ra);
  __ MovFromFloatResult(double_result);

  __ bind(&done);
  __ Ret();
}

bool CEntryStub::NeedsImmovableCode() {
  return true;
}


void CodeStub::GenerateStubsAheadOfTime(Isolate* isolate) {
  CEntryStub::GenerateAheadOfTime(isolate);
  StoreBufferOverflowStub::GenerateFixedRegStubsAheadOfTime(isolate);
  CommonArrayConstructorStub::GenerateStubsAheadOfTime(isolate);
  StoreRegistersStateStub::GenerateAheadOfTime(isolate);
  RestoreRegistersStateStub::GenerateAheadOfTime(isolate);
  StoreFastElementStub::GenerateAheadOfTime(isolate);
}


void StoreRegistersStateStub::GenerateAheadOfTime(Isolate* isolate) {
  StoreRegistersStateStub stub(isolate);
  stub.GetCode();
}


void RestoreRegistersStateStub::GenerateAheadOfTime(Isolate* isolate) {
  RestoreRegistersStateStub stub(isolate);
  stub.GetCode();
}


void CodeStub::GenerateFPStubs(Isolate* isolate) {
  // Generate if not already in cache.
  SaveFPRegsMode mode = kSaveFPRegs;
  CEntryStub(isolate, 1, mode).GetCode();
  StoreBufferOverflowStub(isolate, mode).GetCode();
}


void CEntryStub::GenerateAheadOfTime(Isolate* isolate) {
  CEntryStub stub(isolate, 1, kDontSaveFPRegs);
  stub.GetCode();
  CEntryStub save_doubles(isolate, 1, kSaveFPRegs);
  save_doubles.GetCode();
}


void CEntryStub::Generate(MacroAssembler* masm) {
  // Called from JavaScript; parameters are on stack as if calling JS function
  // a0: number of arguments including receiver
  // a1: pointer to builtin function
  // fp: frame pointer    (restored after C call)
  // sp: stack pointer    (restored as callee's sp after C call)
  // cp: current context  (C callee-saved)
  //
  // If argv_in_register():
  // a2: pointer to the first argument

  ProfileEntryHookStub::MaybeCallEntryHook(masm);

  if (argv_in_register()) {
    // Move argv into the correct register.
    __ mov(s1, a2);
  } else {
    // Compute the argv pointer in a callee-saved register.
    __ Lsa(s1, sp, a0, kPointerSizeLog2);
    __ Subu(s1, s1, kPointerSize);
  }

  // Enter the exit frame that transitions from JavaScript to C++.
  FrameScope scope(masm, StackFrame::MANUAL);
  __ EnterExitFrame(save_doubles(), 0, is_builtin_exit()
                                           ? StackFrame::BUILTIN_EXIT
                                           : StackFrame::EXIT);

  // s0: number of arguments  including receiver (C callee-saved)
  // s1: pointer to first argument (C callee-saved)
  // s2: pointer to builtin function (C callee-saved)

  // Prepare arguments for C routine.
  // a0 = argc
  __ mov(s0, a0);
  __ mov(s2, a1);

  // We are calling compiled C/C++ code. a0 and a1 hold our two arguments. We
  // also need to reserve the 4 argument slots on the stack.

  __ AssertStackIsAligned();

  int frame_alignment = MacroAssembler::ActivationFrameAlignment();
  int frame_alignment_mask = frame_alignment - 1;
  int result_stack_size;
  if (result_size() <= 2) {
    // a0 = argc, a1 = argv, a2 = isolate
    __ li(a2, Operand(ExternalReference::isolate_address(isolate())));
    __ mov(a1, s1);
    result_stack_size = 0;
  } else {
    DCHECK_EQ(3, result_size());
    // Allocate additional space for the result.
    result_stack_size =
        ((result_size() * kPointerSize) + frame_alignment_mask) &
        ~frame_alignment_mask;
    __ Subu(sp, sp, Operand(result_stack_size));

    // a0 = hidden result argument, a1 = argc, a2 = argv, a3 = isolate.
    __ li(a3, Operand(ExternalReference::isolate_address(isolate())));
    __ mov(a2, s1);
    __ mov(a1, a0);
    __ mov(a0, sp);
  }

  // To let the GC traverse the return address of the exit frames, we need to
  // know where the return address is. The CEntryStub is unmovable, so
  // we can store the address on the stack to be able to find it again and
  // we never have to restore it, because it will not change.
  { Assembler::BlockTrampolinePoolScope block_trampoline_pool(masm);
    int kNumInstructionsToJump = 4;
    Label find_ra;
    // Adjust the value in ra to point to the correct return location, 2nd
    // instruction past the real call into C code (the jalr(t9)), and push it.
    // This is the return address of the exit frame.
    if (kArchVariant >= kMips32r6) {
      __ addiupc(ra, kNumInstructionsToJump + 1);
    } else {
      // This branch-and-link sequence is needed to find the current PC on mips
      // before r6, saved to the ra register.
      __ bal(&find_ra);  // bal exposes branch delay slot.
      __ Addu(ra, ra, kNumInstructionsToJump * Instruction::kInstrSize);
    }
    __ bind(&find_ra);

    // This spot was reserved in EnterExitFrame.
    __ sw(ra, MemOperand(sp, result_stack_size));
    // Stack space reservation moved to the branch delay slot below.
    // Stack is still aligned.

    // Call the C routine.
    __ mov(t9, s2);  // Function pointer to t9 to conform to ABI for PIC.
    __ jalr(t9);
    // Set up sp in the delay slot.
    __ addiu(sp, sp, -kCArgsSlotsSize);
    // Make sure the stored 'ra' points to this position.
    DCHECK_EQ(kNumInstructionsToJump,
              masm->InstructionsGeneratedSince(&find_ra));
  }
  if (result_size() > 2) {
    DCHECK_EQ(3, result_size());
    // Read result values stored on stack.
    __ lw(a0, MemOperand(v0, 2 * kPointerSize));
    __ lw(v1, MemOperand(v0, 1 * kPointerSize));
    __ lw(v0, MemOperand(v0, 0 * kPointerSize));
  }
  // Result returned in v0, v1:v0 or a0:v1:v0 - do not destroy these registers!

  // Check result for exception sentinel.
  Label exception_returned;
  __ LoadRoot(t0, Heap::kExceptionRootIndex);
  __ Branch(&exception_returned, eq, t0, Operand(v0));

  // Check that there is no pending exception, otherwise we
  // should have returned the exception sentinel.
  if (FLAG_debug_code) {
    Label okay;
    ExternalReference pending_exception_address(
        IsolateAddressId::kPendingExceptionAddress, isolate());
    __ li(a2, Operand(pending_exception_address));
    __ lw(a2, MemOperand(a2));
    __ LoadRoot(t0, Heap::kTheHoleValueRootIndex);
    // Cannot use check here as it attempts to generate call into runtime.
    __ Branch(&okay, eq, t0, Operand(a2));
    __ stop("Unexpected pending exception");
    __ bind(&okay);
  }

  // Exit C frame and return.
  // v0:v1: result
  // sp: stack pointer
  // fp: frame pointer
  Register argc;
  if (argv_in_register()) {
    // We don't want to pop arguments so set argc to no_reg.
    argc = no_reg;
  } else {
    // s0: still holds argc (callee-saved).
    argc = s0;
  }
  __ LeaveExitFrame(save_doubles(), argc, true, EMIT_RETURN);

  // Handling of exception.
  __ bind(&exception_returned);

  ExternalReference pending_handler_context_address(
      IsolateAddressId::kPendingHandlerContextAddress, isolate());
  ExternalReference pending_handler_code_address(
      IsolateAddressId::kPendingHandlerCodeAddress, isolate());
  ExternalReference pending_handler_offset_address(
      IsolateAddressId::kPendingHandlerOffsetAddress, isolate());
  ExternalReference pending_handler_fp_address(
      IsolateAddressId::kPendingHandlerFPAddress, isolate());
  ExternalReference pending_handler_sp_address(
      IsolateAddressId::kPendingHandlerSPAddress, isolate());

  // Ask the runtime for help to determine the handler. This will set v0 to
  // contain the current pending exception, don't clobber it.
  ExternalReference find_handler(Runtime::kUnwindAndFindExceptionHandler,
                                 isolate());
  {
    FrameScope scope(masm, StackFrame::MANUAL);
    __ PrepareCallCFunction(3, 0, a0);
    __ mov(a0, zero_reg);
    __ mov(a1, zero_reg);
    __ li(a2, Operand(ExternalReference::isolate_address(isolate())));
    __ CallCFunction(find_handler, 3);
  }

  // Retrieve the handler context, SP and FP.
  __ li(cp, Operand(pending_handler_context_address));
  __ lw(cp, MemOperand(cp));
  __ li(sp, Operand(pending_handler_sp_address));
  __ lw(sp, MemOperand(sp));
  __ li(fp, Operand(pending_handler_fp_address));
  __ lw(fp, MemOperand(fp));

  // If the handler is a JS frame, restore the context to the frame. Note that
  // the context will be set to (cp == 0) for non-JS frames.
  Label zero;
  __ Branch(&zero, eq, cp, Operand(zero_reg));
  __ sw(cp, MemOperand(fp, StandardFrameConstants::kContextOffset));
  __ bind(&zero);

  // Compute the handler entry address and jump to it.
  __ li(a1, Operand(pending_handler_code_address));
  __ lw(a1, MemOperand(a1));
  __ li(a2, Operand(pending_handler_offset_address));
  __ lw(a2, MemOperand(a2));
  __ Addu(t9, a1, a2);
  __ Jump(t9, Code::kHeaderSize - kHeapObjectTag);
}


void JSEntryStub::Generate(MacroAssembler* masm) {
  Label invoke, handler_entry, exit;
  Isolate* isolate = masm->isolate();

  // Registers:
  // a0: entry address
  // a1: function
  // a2: receiver
  // a3: argc
  //
  // Stack:
  // 4 args slots
  // args

  ProfileEntryHookStub::MaybeCallEntryHook(masm);

  // Save callee saved registers on the stack.
  __ MultiPush(kCalleeSaved | ra.bit());

  // Save callee-saved FPU registers.
  __ MultiPushFPU(kCalleeSavedFPU);
  // Set up the reserved register for 0.0.
  __ Move(kDoubleRegZero, 0.0);


  // Load argv in s0 register.
  int offset_to_argv = (kNumCalleeSaved + 1) * kPointerSize;
  offset_to_argv += kNumCalleeSavedFPU * kDoubleSize;

  __ InitializeRootRegister();
  __ lw(s0, MemOperand(sp, offset_to_argv + kCArgsSlotsSize));

  // We build an EntryFrame.
  __ li(t3, Operand(-1));  // Push a bad frame pointer to fail if it is used.
  StackFrame::Type marker = type();
  __ li(t2, Operand(StackFrame::TypeToMarker(marker)));
  __ li(t1, Operand(StackFrame::TypeToMarker(marker)));
  __ li(t0, Operand(ExternalReference(IsolateAddressId::kCEntryFPAddress,
                                      isolate)));
  __ lw(t0, MemOperand(t0));
  __ Push(t3, t2, t1, t0);
  // Set up frame pointer for the frame to be pushed.
  __ addiu(fp, sp, -EntryFrameConstants::kCallerFPOffset);

  // Registers:
  // a0: entry_address
  // a1: function
  // a2: receiver_pointer
  // a3: argc
  // s0: argv
  //
  // Stack:
  // caller fp          |
  // function slot      | entry frame
  // context slot       |
  // bad fp (0xff...f)  |
  // callee saved registers + ra
  // 4 args slots
  // args

  // If this is the outermost JS call, set js_entry_sp value.
  Label non_outermost_js;
  ExternalReference js_entry_sp(IsolateAddressId::kJSEntrySPAddress, isolate);
  __ li(t1, Operand(ExternalReference(js_entry_sp)));
  __ lw(t2, MemOperand(t1));
  __ Branch(&non_outermost_js, ne, t2, Operand(zero_reg));
  __ sw(fp, MemOperand(t1));
  __ li(t0, Operand(StackFrame::OUTERMOST_JSENTRY_FRAME));
  Label cont;
  __ b(&cont);
  __ nop();   // Branch delay slot nop.
  __ bind(&non_outermost_js);
  __ li(t0, Operand(StackFrame::INNER_JSENTRY_FRAME));
  __ bind(&cont);
  __ push(t0);

  // Jump to a faked try block that does the invoke, with a faked catch
  // block that sets the pending exception.
  __ jmp(&invoke);
  __ bind(&handler_entry);
  handler_offset_ = handler_entry.pos();
  // Caught exception: Store result (exception) in the pending exception
  // field in the JSEnv and return a failure sentinel.  Coming in here the
  // fp will be invalid because the PushStackHandler below sets it to 0 to
  // signal the existence of the JSEntry frame.
  __ li(t0, Operand(ExternalReference(
                IsolateAddressId::kPendingExceptionAddress, isolate)));
  __ sw(v0, MemOperand(t0));  // We come back from 'invoke'. result is in v0.
  __ LoadRoot(v0, Heap::kExceptionRootIndex);
  __ b(&exit);  // b exposes branch delay slot.
  __ nop();   // Branch delay slot nop.

  // Invoke: Link this frame into the handler chain.
  __ bind(&invoke);
  __ PushStackHandler();
  // If an exception not caught by another handler occurs, this handler
  // returns control to the code after the bal(&invoke) above, which
  // restores all kCalleeSaved registers (including cp and fp) to their
  // saved values before returning a failure to C.

  // Invoke the function by calling through JS entry trampoline builtin.
  // Notice that we cannot store a reference to the trampoline code directly in
  // this stub, because runtime stubs are not traversed when doing GC.

  // Registers:
  // a0: entry_address
  // a1: function
  // a2: receiver_pointer
  // a3: argc
  // s0: argv
  //
  // Stack:
  // handler frame
  // entry frame
  // callee saved registers + ra
  // 4 args slots
  // args

  if (type() == StackFrame::CONSTRUCT_ENTRY) {
    __ Call(BUILTIN_CODE(isolate, JSConstructEntryTrampoline),
            RelocInfo::CODE_TARGET);
  } else {
    __ Call(BUILTIN_CODE(isolate, JSEntryTrampoline), RelocInfo::CODE_TARGET);
  }

  // Unlink this frame from the handler chain.
  __ PopStackHandler();

  __ bind(&exit);  // v0 holds result
  // Check if the current stack frame is marked as the outermost JS frame.
  Label non_outermost_js_2;
  __ pop(t1);
  __ Branch(&non_outermost_js_2, ne, t1,
            Operand(StackFrame::OUTERMOST_JSENTRY_FRAME));
  __ li(t1, Operand(ExternalReference(js_entry_sp)));
  __ sw(zero_reg, MemOperand(t1));
  __ bind(&non_outermost_js_2);

  // Restore the top frame descriptors from the stack.
  __ pop(t1);
  __ li(t0, Operand(ExternalReference(IsolateAddressId::kCEntryFPAddress,
                                      isolate)));
  __ sw(t1, MemOperand(t0));

  // Reset the stack to the callee saved registers.
  __ addiu(sp, sp, -EntryFrameConstants::kCallerFPOffset);

  // Restore callee-saved fpu registers.
  __ MultiPopFPU(kCalleeSavedFPU);

  // Restore callee saved registers from the stack.
  __ MultiPop(kCalleeSaved | ra.bit());
  // Return.
  __ Jump(ra);
}

void StringHelper::GenerateFlatOneByteStringEquals(
    MacroAssembler* masm, Register left, Register right, Register scratch1,
    Register scratch2, Register scratch3) {
  Register length = scratch1;

  // Compare lengths.
  Label strings_not_equal, check_zero_length;
  __ lw(length, FieldMemOperand(left, String::kLengthOffset));
  __ lw(scratch2, FieldMemOperand(right, String::kLengthOffset));
  __ Branch(&check_zero_length, eq, length, Operand(scratch2));
  __ bind(&strings_not_equal);
  DCHECK(is_int16(NOT_EQUAL));
  __ Ret(USE_DELAY_SLOT);
  __ li(v0, Operand(Smi::FromInt(NOT_EQUAL)));

  // Check if the length is zero.
  Label compare_chars;
  __ bind(&check_zero_length);
  STATIC_ASSERT(kSmiTag == 0);
  __ Branch(&compare_chars, ne, length, Operand(zero_reg));
  DCHECK(is_int16(EQUAL));
  __ Ret(USE_DELAY_SLOT);
  __ li(v0, Operand(Smi::FromInt(EQUAL)));

  // Compare characters.
  __ bind(&compare_chars);

  GenerateOneByteCharsCompareLoop(masm, left, right, length, scratch2, scratch3,
                                  v0, &strings_not_equal);

  // Characters are equal.
  __ Ret(USE_DELAY_SLOT);
  __ li(v0, Operand(Smi::FromInt(EQUAL)));
}


void StringHelper::GenerateCompareFlatOneByteStrings(
    MacroAssembler* masm, Register left, Register right, Register scratch1,
    Register scratch2, Register scratch3, Register scratch4) {
  Label result_not_equal, compare_lengths;
  // Find minimum length and length difference.
  __ lw(scratch1, FieldMemOperand(left, String::kLengthOffset));
  __ lw(scratch2, FieldMemOperand(right, String::kLengthOffset));
  __ Subu(scratch3, scratch1, Operand(scratch2));
  Register length_delta = scratch3;
  __ slt(scratch4, scratch2, scratch1);
  __ Movn(scratch1, scratch2, scratch4);
  Register min_length = scratch1;
  STATIC_ASSERT(kSmiTag == 0);
  __ Branch(&compare_lengths, eq, min_length, Operand(zero_reg));

  // Compare loop.
  GenerateOneByteCharsCompareLoop(masm, left, right, min_length, scratch2,
                                  scratch4, v0, &result_not_equal);

  // Compare lengths - strings up to min-length are equal.
  __ bind(&compare_lengths);
  DCHECK(Smi::FromInt(EQUAL) == static_cast<Smi*>(0));
  // Use length_delta as result if it's zero.
  __ mov(scratch2, length_delta);
  __ mov(scratch4, zero_reg);
  __ mov(v0, zero_reg);

  __ bind(&result_not_equal);
  // Conditionally update the result based either on length_delta or
  // the last comparion performed in the loop above.
  Label ret;
  __ Branch(&ret, eq, scratch2, Operand(scratch4));
  __ li(v0, Operand(Smi::FromInt(GREATER)));
  __ Branch(&ret, gt, scratch2, Operand(scratch4));
  __ li(v0, Operand(Smi::FromInt(LESS)));
  __ bind(&ret);
  __ Ret();
}


void StringHelper::GenerateOneByteCharsCompareLoop(
    MacroAssembler* masm, Register left, Register right, Register length,
    Register scratch1, Register scratch2, Register scratch3,
    Label* chars_not_equal) {
  // Change index to run from -length to -1 by adding length to string
  // start. This means that loop ends when index reaches zero, which
  // doesn't need an additional compare.
  __ SmiUntag(length);
  __ Addu(scratch1, length,
          Operand(SeqOneByteString::kHeaderSize - kHeapObjectTag));
  __ Addu(left, left, Operand(scratch1));
  __ Addu(right, right, Operand(scratch1));
  __ Subu(length, zero_reg, length);
  Register index = length;  // index = -length;


  // Compare loop.
  Label loop;
  __ bind(&loop);
  __ Addu(scratch3, left, index);
  __ lbu(scratch1, MemOperand(scratch3));
  __ Addu(scratch3, right, index);
  __ lbu(scratch2, MemOperand(scratch3));
  __ Branch(chars_not_equal, ne, scratch1, Operand(scratch2));
  __ Addu(index, index, 1);
  __ Branch(&loop, ne, index, Operand(zero_reg));
}


void DirectCEntryStub::Generate(MacroAssembler* masm) {
  // Make place for arguments to fit C calling convention. Most of the callers
  // of DirectCEntryStub::GenerateCall are using EnterExitFrame/LeaveExitFrame
  // so they handle stack restoring and we don't have to do that here.
  // Any caller of DirectCEntryStub::GenerateCall must take care of dropping
  // kCArgsSlotsSize stack space after the call.
  __ Subu(sp, sp, Operand(kCArgsSlotsSize));
  // Place the return address on the stack, making the call
  // GC safe. The RegExp backend also relies on this.
  __ sw(ra, MemOperand(sp, kCArgsSlotsSize));
  __ Call(t9);  // Call the C++ function.
  __ lw(t9, MemOperand(sp, kCArgsSlotsSize));

  if (FLAG_debug_code && FLAG_enable_slow_asserts) {
    // In case of an error the return address may point to a memory area
    // filled with kZapValue by the GC.
    // Dereference the address and check for this.
    __ lw(t0, MemOperand(t9));
    __ Assert(ne, kReceivedInvalidReturnAddress, t0,
        Operand(reinterpret_cast<uint32_t>(kZapValue)));
  }
  __ Jump(t9);
}


void DirectCEntryStub::GenerateCall(MacroAssembler* masm,
                                    Register target) {
  intptr_t loc =
      reinterpret_cast<intptr_t>(GetCode().location());
  __ Move(t9, target);
  __ li(at, Operand(loc, RelocInfo::CODE_TARGET), CONSTANT_SIZE);
  __ Call(at);
}


void NameDictionaryLookupStub::GenerateNegativeLookup(MacroAssembler* masm,
                                                      Label* miss,
                                                      Label* done,
                                                      Register receiver,
                                                      Register properties,
                                                      Handle<Name> name,
                                                      Register scratch0) {
  DCHECK(name->IsUniqueName());
  // If names of slots in range from 1 to kProbes - 1 for the hash value are
  // not equal to the name and kProbes-th slot is not used (its name is the
  // undefined value), it guarantees the hash table doesn't contain the
  // property. It's true even if some slots represent deleted properties
  // (their names are the hole value).
  for (int i = 0; i < kInlinedProbes; i++) {
    // scratch0 points to properties hash.
    // Compute the masked index: (hash + i + i * i) & mask.
    Register index = scratch0;
    // Capacity is smi 2^n.
    __ lw(index, FieldMemOperand(properties, kCapacityOffset));
    __ Subu(index, index, Operand(1));
    __ And(index, index, Operand(
        Smi::FromInt(name->Hash() + NameDictionary::GetProbeOffset(i))));

    // Scale the index by multiplying by the entry size.
    STATIC_ASSERT(NameDictionary::kEntrySize == 3);
    __ Lsa(index, index, index, 1);

    Register entity_name = scratch0;
    // Having undefined at this place means the name is not contained.
    STATIC_ASSERT(kSmiTagSize == 1);
    Register tmp = properties;
    __ Lsa(tmp, properties, index, 1);
    __ lw(entity_name, FieldMemOperand(tmp, kElementsStartOffset));

    DCHECK(!tmp.is(entity_name));
    __ LoadRoot(tmp, Heap::kUndefinedValueRootIndex);
    __ Branch(done, eq, entity_name, Operand(tmp));

    // Load the hole ready for use below:
    __ LoadRoot(tmp, Heap::kTheHoleValueRootIndex);

    // Stop if found the property.
    __ Branch(miss, eq, entity_name, Operand(Handle<Name>(name)));

    Label good;
    __ Branch(&good, eq, entity_name, Operand(tmp));

    // Check if the entry name is not a unique name.
    __ lw(entity_name, FieldMemOperand(entity_name, HeapObject::kMapOffset));
    __ lbu(entity_name,
           FieldMemOperand(entity_name, Map::kInstanceTypeOffset));
    __ JumpIfNotUniqueNameInstanceType(entity_name, miss);
    __ bind(&good);

    // Restore the properties.
    __ lw(properties,
          FieldMemOperand(receiver, JSObject::kPropertiesOrHashOffset));
  }

  const int spill_mask =
      (ra.bit() | t2.bit() | t1.bit() | t0.bit() | a3.bit() |
       a2.bit() | a1.bit() | a0.bit() | v0.bit());

  __ MultiPush(spill_mask);
  __ lw(a0, FieldMemOperand(receiver, JSObject::kPropertiesOrHashOffset));
  __ li(a1, Operand(Handle<Name>(name)));
  NameDictionaryLookupStub stub(masm->isolate(), NEGATIVE_LOOKUP);
  __ CallStub(&stub);
  __ mov(at, v0);
  __ MultiPop(spill_mask);

  __ Branch(done, eq, at, Operand(zero_reg));
  __ Branch(miss, ne, at, Operand(zero_reg));
}

void NameDictionaryLookupStub::Generate(MacroAssembler* masm) {
  // This stub overrides SometimesSetsUpAFrame() to return false.  That means
  // we cannot call anything that could cause a GC from this stub.
  // Registers:
  //  result: NameDictionary to probe
  //  a1: key
  //  dictionary: NameDictionary to probe.
  //  index: will hold an index of entry if lookup is successful.
  //         might alias with result_.
  // Returns:
  //  result_ is zero if lookup failed, non zero otherwise.

  Register result = v0;
  Register dictionary = a0;
  Register key = a1;
  Register index = a2;
  Register mask = a3;
  Register hash = t0;
  Register undefined = t1;
  Register entry_key = t2;

  Label in_dictionary, maybe_in_dictionary, not_in_dictionary;

  __ lw(mask, FieldMemOperand(dictionary, kCapacityOffset));
  __ sra(mask, mask, kSmiTagSize);
  __ Subu(mask, mask, Operand(1));

  __ lw(hash, FieldMemOperand(key, Name::kHashFieldOffset));

  __ LoadRoot(undefined, Heap::kUndefinedValueRootIndex);

  for (int i = kInlinedProbes; i < kTotalProbes; i++) {
    // Compute the masked index: (hash + i + i * i) & mask.
    // Capacity is smi 2^n.
    if (i > 0) {
      // Add the probe offset (i + i * i) left shifted to avoid right shifting
      // the hash in a separate instruction. The value hash + i + i * i is right
      // shifted in the following and instruction.
      DCHECK(NameDictionary::GetProbeOffset(i) <
             1 << (32 - Name::kHashFieldOffset));
      __ Addu(index, hash, Operand(
          NameDictionary::GetProbeOffset(i) << Name::kHashShift));
    } else {
      __ mov(index, hash);
    }
    __ srl(index, index, Name::kHashShift);
    __ And(index, mask, index);

    // Scale the index by multiplying by the entry size.
    STATIC_ASSERT(NameDictionary::kEntrySize == 3);
    // index *= 3.
    __ Lsa(index, index, index, 1);

    STATIC_ASSERT(kSmiTagSize == 1);
    __ Lsa(index, dictionary, index, 2);
    __ lw(entry_key, FieldMemOperand(index, kElementsStartOffset));

    // Having undefined at this place means the name is not contained.
    __ Branch(&not_in_dictionary, eq, entry_key, Operand(undefined));

    // Stop if found the property.
    __ Branch(&in_dictionary, eq, entry_key, Operand(key));

    if (i != kTotalProbes - 1 && mode() == NEGATIVE_LOOKUP) {
      // Check if the entry name is not a unique name.
      __ lw(entry_key, FieldMemOperand(entry_key, HeapObject::kMapOffset));
      __ lbu(entry_key,
             FieldMemOperand(entry_key, Map::kInstanceTypeOffset));
      __ JumpIfNotUniqueNameInstanceType(entry_key, &maybe_in_dictionary);
    }
  }

  __ bind(&maybe_in_dictionary);
  // If we are doing negative lookup then probing failure should be
  // treated as a lookup success. For positive lookup probing failure
  // should be treated as lookup failure.
  if (mode() == POSITIVE_LOOKUP) {
    __ Ret(USE_DELAY_SLOT);
    __ mov(result, zero_reg);
  }

  __ bind(&in_dictionary);
  __ Ret(USE_DELAY_SLOT);
  __ li(result, 1);

  __ bind(&not_in_dictionary);
  __ Ret(USE_DELAY_SLOT);
  __ mov(result, zero_reg);
}


void StoreBufferOverflowStub::GenerateFixedRegStubsAheadOfTime(
    Isolate* isolate) {
  StoreBufferOverflowStub stub1(isolate, kDontSaveFPRegs);
  stub1.GetCode();
  // Hydrogen code stubs need stub2 at snapshot time.
  StoreBufferOverflowStub stub2(isolate, kSaveFPRegs);
  stub2.GetCode();
}


// Takes the input in 3 registers: address_ value_ and object_.  A pointer to
// the value has just been written into the object, now this stub makes sure
// we keep the GC informed.  The word in the object where the value has been
// written is in the address register.
void RecordWriteStub::Generate(MacroAssembler* masm) {
  Label skip_to_incremental_noncompacting;
  Label skip_to_incremental_compacting;

  // The first two branch+nop instructions are generated with labels so as to
  // get the offset fixed up correctly by the bind(Label*) call.  We patch it
  // back and forth between a "bne zero_reg, zero_reg, ..." (a nop in this
  // position) and the "beq zero_reg, zero_reg, ..." when we start and stop
  // incremental heap marking.
  // See RecordWriteStub::Patch for details.
  __ beq(zero_reg, zero_reg, &skip_to_incremental_noncompacting);
  __ nop();
  __ beq(zero_reg, zero_reg, &skip_to_incremental_compacting);
  __ nop();

  if (remembered_set_action() == EMIT_REMEMBERED_SET) {
    __ RememberedSetHelper(object(),
                           address(),
                           value(),
                           save_fp_regs_mode(),
                           MacroAssembler::kReturnAtEnd);
  }
  __ Ret();

  __ bind(&skip_to_incremental_noncompacting);
  GenerateIncremental(masm, INCREMENTAL);

  __ bind(&skip_to_incremental_compacting);
  GenerateIncremental(masm, INCREMENTAL_COMPACTION);

  // Initial mode of the stub is expected to be STORE_BUFFER_ONLY.
  // Will be checked in IncrementalMarking::ActivateGeneratedStub.

  PatchBranchIntoNop(masm, 0);
  PatchBranchIntoNop(masm, 2 * Assembler::kInstrSize);
}


void RecordWriteStub::GenerateIncremental(MacroAssembler* masm, Mode mode) {
  regs_.Save(masm);

  if (remembered_set_action() == EMIT_REMEMBERED_SET) {
    Label dont_need_remembered_set;

    __ lw(regs_.scratch0(), MemOperand(regs_.address(), 0));
    __ JumpIfNotInNewSpace(regs_.scratch0(),  // Value.
                           regs_.scratch0(),
                           &dont_need_remembered_set);

    __ JumpIfInNewSpace(regs_.object(), regs_.scratch0(),
                        &dont_need_remembered_set);

    // First notify the incremental marker if necessary, then update the
    // remembered set.
    CheckNeedsToInformIncrementalMarker(
        masm, kUpdateRememberedSetOnNoNeedToInformIncrementalMarker, mode);
    InformIncrementalMarker(masm);
    regs_.Restore(masm);
    __ RememberedSetHelper(object(),
                           address(),
                           value(),
                           save_fp_regs_mode(),
                           MacroAssembler::kReturnAtEnd);

    __ bind(&dont_need_remembered_set);
  }

  CheckNeedsToInformIncrementalMarker(
      masm, kReturnOnNoNeedToInformIncrementalMarker, mode);
  InformIncrementalMarker(masm);
  regs_.Restore(masm);
  __ Ret();
}


void RecordWriteStub::InformIncrementalMarker(MacroAssembler* masm) {
  regs_.SaveCallerSaveRegisters(masm, save_fp_regs_mode());
  int argument_count = 3;
  __ PrepareCallCFunction(argument_count, regs_.scratch0());
  Register address =
      a0.is(regs_.address()) ? regs_.scratch0() : regs_.address();
  DCHECK(!address.is(regs_.object()));
  DCHECK(!address.is(a0));
  __ Move(address, regs_.address());
  __ Move(a0, regs_.object());
  __ Move(a1, address);
  __ li(a2, Operand(ExternalReference::isolate_address(isolate())));

  AllowExternalCallThatCantCauseGC scope(masm);
  __ CallCFunction(
      ExternalReference::incremental_marking_record_write_function(isolate()),
      argument_count);
  regs_.RestoreCallerSaveRegisters(masm, save_fp_regs_mode());
}


void RecordWriteStub::CheckNeedsToInformIncrementalMarker(
    MacroAssembler* masm,
    OnNoNeedToInformIncrementalMarker on_no_need,
    Mode mode) {
  Label need_incremental;
  Label need_incremental_pop_scratch;

#ifndef V8_CONCURRENT_MARKING
  Label on_black;
  // Let's look at the color of the object:  If it is not black we don't have
  // to inform the incremental marker.
  __ JumpIfBlack(regs_.object(), regs_.scratch0(), regs_.scratch1(), &on_black);

  regs_.Restore(masm);
  if (on_no_need == kUpdateRememberedSetOnNoNeedToInformIncrementalMarker) {
    __ RememberedSetHelper(object(),
                           address(),
                           value(),
                           save_fp_regs_mode(),
                           MacroAssembler::kReturnAtEnd);
  } else {
    __ Ret();
  }

  __ bind(&on_black);
#endif

  // Get the value from the slot.
  __ lw(regs_.scratch0(), MemOperand(regs_.address(), 0));

  if (mode == INCREMENTAL_COMPACTION) {
    Label ensure_not_white;

    __ CheckPageFlag(regs_.scratch0(),  // Contains value.
                     regs_.scratch1(),  // Scratch.
                     MemoryChunk::kEvacuationCandidateMask,
                     eq,
                     &ensure_not_white);

    __ CheckPageFlag(regs_.object(),
                     regs_.scratch1(),  // Scratch.
                     MemoryChunk::kSkipEvacuationSlotsRecordingMask,
                     eq,
                     &need_incremental);

    __ bind(&ensure_not_white);
  }

  // We need extra registers for this, so we push the object and the address
  // register temporarily.
  __ Push(regs_.object(), regs_.address());
  __ JumpIfWhite(regs_.scratch0(),  // The value.
                 regs_.scratch1(),  // Scratch.
                 regs_.object(),    // Scratch.
                 regs_.address(),   // Scratch.
                 &need_incremental_pop_scratch);
  __ Pop(regs_.object(), regs_.address());

  regs_.Restore(masm);
  if (on_no_need == kUpdateRememberedSetOnNoNeedToInformIncrementalMarker) {
    __ RememberedSetHelper(object(),
                           address(),
                           value(),
                           save_fp_regs_mode(),
                           MacroAssembler::kReturnAtEnd);
  } else {
    __ Ret();
  }

  __ bind(&need_incremental_pop_scratch);
  __ Pop(regs_.object(), regs_.address());

  __ bind(&need_incremental);

  // Fall through when we need to inform the incremental marker.
}

void ProfileEntryHookStub::MaybeCallEntryHookDelayed(TurboAssembler* tasm,
                                                     Zone* zone) {
  if (tasm->isolate()->function_entry_hook() != NULL) {
    tasm->push(ra);
    tasm->CallStubDelayed(new (zone) ProfileEntryHookStub(nullptr));
    tasm->pop(ra);
  }
}

void ProfileEntryHookStub::MaybeCallEntryHook(MacroAssembler* masm) {
  if (masm->isolate()->function_entry_hook() != NULL) {
    ProfileEntryHookStub stub(masm->isolate());
    __ push(ra);
    __ CallStub(&stub);
    __ pop(ra);
  }
}


void ProfileEntryHookStub::Generate(MacroAssembler* masm) {
  // The entry hook is a "push ra" instruction, followed by a call.
  // Note: on MIPS "push" is 2 instruction
  const int32_t kReturnAddressDistanceFromFunctionStart =
      Assembler::kCallTargetAddressOffset + (2 * Assembler::kInstrSize);

  // This should contain all kJSCallerSaved registers.
  const RegList kSavedRegs =
     kJSCallerSaved |  // Caller saved registers.
     s5.bit();         // Saved stack pointer.

  // We also save ra, so the count here is one higher than the mask indicates.
  const int32_t kNumSavedRegs = kNumJSCallerSaved + 2;

  // Save all caller-save registers as this may be called from anywhere.
  __ MultiPush(kSavedRegs | ra.bit());

  // Compute the function's address for the first argument.
  __ Subu(a0, ra, Operand(kReturnAddressDistanceFromFunctionStart));

  // The caller's return address is above the saved temporaries.
  // Grab that for the second argument to the hook.
  __ Addu(a1, sp, Operand(kNumSavedRegs * kPointerSize));

  // Align the stack if necessary.
  int frame_alignment = masm->ActivationFrameAlignment();
  if (frame_alignment > kPointerSize) {
    __ mov(s5, sp);
    DCHECK(base::bits::IsPowerOfTwo(frame_alignment));
    __ And(sp, sp, Operand(-frame_alignment));
  }
  __ Subu(sp, sp, kCArgsSlotsSize);
#if defined(V8_HOST_ARCH_MIPS)
  int32_t entry_hook =
      reinterpret_cast<int32_t>(isolate()->function_entry_hook());
  __ li(t9, Operand(entry_hook));
#else
  // Under the simulator we need to indirect the entry hook through a
  // trampoline function at a known address.
  // It additionally takes an isolate as a third parameter.
  __ li(a2, Operand(ExternalReference::isolate_address(isolate())));

  ApiFunction dispatcher(FUNCTION_ADDR(EntryHookTrampoline));
  __ li(t9, Operand(ExternalReference(&dispatcher,
                                      ExternalReference::BUILTIN_CALL,
                                      isolate())));
#endif
  // Call C function through t9 to conform ABI for PIC.
  __ Call(t9);

  // Restore the stack pointer if needed.
  if (frame_alignment > kPointerSize) {
    __ mov(sp, s5);
  } else {
    __ Addu(sp, sp, kCArgsSlotsSize);
  }

  // Also pop ra to get Ret(0).
  __ MultiPop(kSavedRegs | ra.bit());
  __ Ret();
}


template<class T>
static void CreateArrayDispatch(MacroAssembler* masm,
                                AllocationSiteOverrideMode mode) {
  if (mode == DISABLE_ALLOCATION_SITES) {
    T stub(masm->isolate(), GetInitialFastElementsKind(), mode);
    __ TailCallStub(&stub);
  } else if (mode == DONT_OVERRIDE) {
    int last_index =
        GetSequenceIndexFromFastElementsKind(TERMINAL_FAST_ELEMENTS_KIND);
    for (int i = 0; i <= last_index; ++i) {
      ElementsKind kind = GetFastElementsKindFromSequenceIndex(i);
      T stub(masm->isolate(), kind);
      __ TailCallStub(&stub, eq, a3, Operand(kind));
    }

    // If we reached this point there is a problem.
    __ Abort(kUnexpectedElementsKindInArrayConstructor);
  } else {
    UNREACHABLE();
  }
}


static void CreateArrayDispatchOneArgument(MacroAssembler* masm,
                                           AllocationSiteOverrideMode mode) {
  // a2 - allocation site (if mode != DISABLE_ALLOCATION_SITES)
  // a3 - kind (if mode != DISABLE_ALLOCATION_SITES)
  // a0 - number of arguments
  // a1 - constructor?
  // sp[0] - last argument
    STATIC_ASSERT(PACKED_SMI_ELEMENTS == 0);
    STATIC_ASSERT(HOLEY_SMI_ELEMENTS == 1);
    STATIC_ASSERT(PACKED_ELEMENTS == 2);
    STATIC_ASSERT(HOLEY_ELEMENTS == 3);
    STATIC_ASSERT(PACKED_DOUBLE_ELEMENTS == 4);
    STATIC_ASSERT(HOLEY_DOUBLE_ELEMENTS == 5);

  if (mode == DISABLE_ALLOCATION_SITES) {
    ElementsKind initial = GetInitialFastElementsKind();
    ElementsKind holey_initial = GetHoleyElementsKind(initial);

    ArraySingleArgumentConstructorStub stub_holey(masm->isolate(),
                                                  holey_initial,
                                                  DISABLE_ALLOCATION_SITES);
    __ TailCallStub(&stub_holey);
  } else if (mode == DONT_OVERRIDE) {
    // is the low bit set? If so, we are holey and that is good.
    Label normal_sequence;
    __ And(at, a3, Operand(1));
    __ Branch(&normal_sequence, ne, at, Operand(zero_reg));

    // We are going to create a holey array, but our kind is non-holey.
    // Fix kind and retry (only if we have an allocation site in the slot).
    __ Addu(a3, a3, Operand(1));

    if (FLAG_debug_code) {
      __ lw(t1, FieldMemOperand(a2, 0));
      __ LoadRoot(at, Heap::kAllocationSiteMapRootIndex);
      __ Assert(eq, kExpectedAllocationSite, t1, Operand(at));
    }

    // Save the resulting elements kind in type info. We can't just store a3
    // in the AllocationSite::transition_info field because elements kind is
    // restricted to a portion of the field...upper bits need to be left alone.
    STATIC_ASSERT(AllocationSite::ElementsKindBits::kShift == 0);
    __ lw(t0, FieldMemOperand(
                  a2, AllocationSite::kTransitionInfoOrBoilerplateOffset));
    __ Addu(t0, t0, Operand(Smi::FromInt(kFastElementsKindPackedToHoley)));
    __ sw(t0, FieldMemOperand(
                  a2, AllocationSite::kTransitionInfoOrBoilerplateOffset));

    __ bind(&normal_sequence);
    int last_index =
        GetSequenceIndexFromFastElementsKind(TERMINAL_FAST_ELEMENTS_KIND);
    for (int i = 0; i <= last_index; ++i) {
      ElementsKind kind = GetFastElementsKindFromSequenceIndex(i);
      ArraySingleArgumentConstructorStub stub(masm->isolate(), kind);
      __ TailCallStub(&stub, eq, a3, Operand(kind));
    }

    // If we reached this point there is a problem.
    __ Abort(kUnexpectedElementsKindInArrayConstructor);
  } else {
    UNREACHABLE();
  }
}


template<class T>
static void ArrayConstructorStubAheadOfTimeHelper(Isolate* isolate) {
  int to_index =
      GetSequenceIndexFromFastElementsKind(TERMINAL_FAST_ELEMENTS_KIND);
  for (int i = 0; i <= to_index; ++i) {
    ElementsKind kind = GetFastElementsKindFromSequenceIndex(i);
    T stub(isolate, kind);
    stub.GetCode();
    if (AllocationSite::ShouldTrack(kind)) {
      T stub1(isolate, kind, DISABLE_ALLOCATION_SITES);
      stub1.GetCode();
    }
  }
}

void CommonArrayConstructorStub::GenerateStubsAheadOfTime(Isolate* isolate) {
  ArrayConstructorStubAheadOfTimeHelper<ArrayNoArgumentConstructorStub>(
      isolate);
  ArrayConstructorStubAheadOfTimeHelper<ArraySingleArgumentConstructorStub>(
      isolate);
  ArrayNArgumentsConstructorStub stub(isolate);
  stub.GetCode();
  ElementsKind kinds[2] = {PACKED_ELEMENTS, HOLEY_ELEMENTS};
  for (int i = 0; i < 2; i++) {
    // For internal arrays we only need a few things.
    InternalArrayNoArgumentConstructorStub stubh1(isolate, kinds[i]);
    stubh1.GetCode();
    InternalArraySingleArgumentConstructorStub stubh2(isolate, kinds[i]);
    stubh2.GetCode();
  }
}


void ArrayConstructorStub::GenerateDispatchToArrayStub(
    MacroAssembler* masm,
    AllocationSiteOverrideMode mode) {
  Label not_zero_case, not_one_case;
  __ And(at, a0, a0);
  __ Branch(&not_zero_case, ne, at, Operand(zero_reg));
  CreateArrayDispatch<ArrayNoArgumentConstructorStub>(masm, mode);

  __ bind(&not_zero_case);
  __ Branch(&not_one_case, gt, a0, Operand(1));
  CreateArrayDispatchOneArgument(masm, mode);

  __ bind(&not_one_case);
  ArrayNArgumentsConstructorStub stub(masm->isolate());
  __ TailCallStub(&stub);
}


void ArrayConstructorStub::Generate(MacroAssembler* masm) {
  // ----------- S t a t e -------------
  //  -- a0 : argc (only if argument_count() is ANY or MORE_THAN_ONE)
  //  -- a1 : constructor
  //  -- a2 : AllocationSite or undefined
  //  -- a3 : Original constructor
  //  -- sp[0] : last argument
  // -----------------------------------

  if (FLAG_debug_code) {
    // The array construct code is only set for the global and natives
    // builtin Array functions which always have maps.

    // Initial map for the builtin Array function should be a map.
    __ lw(t0, FieldMemOperand(a1, JSFunction::kPrototypeOrInitialMapOffset));
    // Will both indicate a NULL and a Smi.
    __ SmiTst(t0, at);
    __ Assert(ne, kUnexpectedInitialMapForArrayFunction,
        at, Operand(zero_reg));
    __ GetObjectType(t0, t0, t1);
    __ Assert(eq, kUnexpectedInitialMapForArrayFunction,
        t1, Operand(MAP_TYPE));

    // We should either have undefined in a2 or a valid AllocationSite
    __ AssertUndefinedOrAllocationSite(a2, t0);
  }

  // Enter the context of the Array function.
  __ lw(cp, FieldMemOperand(a1, JSFunction::kContextOffset));

  Label subclassing;
  __ Branch(&subclassing, ne, a1, Operand(a3));

  Label no_info;
  // Get the elements kind and case on that.
  __ LoadRoot(at, Heap::kUndefinedValueRootIndex);
  __ Branch(&no_info, eq, a2, Operand(at));

  __ lw(a3, FieldMemOperand(
                a2, AllocationSite::kTransitionInfoOrBoilerplateOffset));
  __ SmiUntag(a3);
  STATIC_ASSERT(AllocationSite::ElementsKindBits::kShift == 0);
  __ And(a3, a3, Operand(AllocationSite::ElementsKindBits::kMask));
  GenerateDispatchToArrayStub(masm, DONT_OVERRIDE);

  __ bind(&no_info);
  GenerateDispatchToArrayStub(masm, DISABLE_ALLOCATION_SITES);

  // Subclassing.
  __ bind(&subclassing);
  __ Lsa(at, sp, a0, kPointerSizeLog2);
  __ sw(a1, MemOperand(at));
  __ li(at, Operand(3));
  __ addu(a0, a0, at);
  __ Push(a3, a2);
  __ JumpToExternalReference(ExternalReference(Runtime::kNewArray, isolate()));
}


void InternalArrayConstructorStub::GenerateCase(
    MacroAssembler* masm, ElementsKind kind) {

  InternalArrayNoArgumentConstructorStub stub0(isolate(), kind);
  __ TailCallStub(&stub0, lo, a0, Operand(1));

  ArrayNArgumentsConstructorStub stubN(isolate());
  __ TailCallStub(&stubN, hi, a0, Operand(1));

  if (IsFastPackedElementsKind(kind)) {
    // We might need to create a holey array
    // look at the first argument.
    __ lw(at, MemOperand(sp, 0));

    InternalArraySingleArgumentConstructorStub
        stub1_holey(isolate(), GetHoleyElementsKind(kind));
    __ TailCallStub(&stub1_holey, ne, at, Operand(zero_reg));
  }

  InternalArraySingleArgumentConstructorStub stub1(isolate(), kind);
  __ TailCallStub(&stub1);
}


void InternalArrayConstructorStub::Generate(MacroAssembler* masm) {
  // ----------- S t a t e -------------
  //  -- a0 : argc
  //  -- a1 : constructor
  //  -- sp[0] : return address
  //  -- sp[4] : last argument
  // -----------------------------------

  if (FLAG_debug_code) {
    // The array construct code is only set for the global and natives
    // builtin Array functions which always have maps.

    // Initial map for the builtin Array function should be a map.
    __ lw(a3, FieldMemOperand(a1, JSFunction::kPrototypeOrInitialMapOffset));
    // Will both indicate a NULL and a Smi.
    __ SmiTst(a3, at);
    __ Assert(ne, kUnexpectedInitialMapForArrayFunction,
        at, Operand(zero_reg));
    __ GetObjectType(a3, a3, t0);
    __ Assert(eq, kUnexpectedInitialMapForArrayFunction,
        t0, Operand(MAP_TYPE));
  }

  // Figure out the right elements kind.
  __ lw(a3, FieldMemOperand(a1, JSFunction::kPrototypeOrInitialMapOffset));

  // Load the map's "bit field 2" into a3. We only need the first byte,
  // but the following bit field extraction takes care of that anyway.
  __ lbu(a3, FieldMemOperand(a3, Map::kBitField2Offset));
  // Retrieve elements_kind from bit field 2.
  __ DecodeField<Map::ElementsKindBits>(a3);

  if (FLAG_debug_code) {
    Label done;
    __ Branch(&done, eq, a3, Operand(PACKED_ELEMENTS));
    __ Assert(eq, kInvalidElementsKindForInternalArrayOrInternalPackedArray, a3,
              Operand(HOLEY_ELEMENTS));
    __ bind(&done);
  }

  Label fast_elements_case;
  __ Branch(&fast_elements_case, eq, a3, Operand(PACKED_ELEMENTS));
  GenerateCase(masm, HOLEY_ELEMENTS);

  __ bind(&fast_elements_case);
  GenerateCase(masm, PACKED_ELEMENTS);
}

static int AddressOffset(ExternalReference ref0, ExternalReference ref1) {
  return ref0.address() - ref1.address();
}


// Calls an API function.  Allocates HandleScope, extracts returned value
// from handle and propagates exceptions.  Restores context.  stack_space
// - space to be unwound on exit (includes the call JS arguments space and
// the additional space allocated for the fast call).
static void CallApiFunctionAndReturn(
    MacroAssembler* masm, Register function_address,
    ExternalReference thunk_ref, int stack_space, int32_t stack_space_offset,
    MemOperand return_value_operand, MemOperand* context_restore_operand) {
  Isolate* isolate = masm->isolate();
  ExternalReference next_address =
      ExternalReference::handle_scope_next_address(isolate);
  const int kNextOffset = 0;
  const int kLimitOffset = AddressOffset(
      ExternalReference::handle_scope_limit_address(isolate), next_address);
  const int kLevelOffset = AddressOffset(
      ExternalReference::handle_scope_level_address(isolate), next_address);

  DCHECK(function_address.is(a1) || function_address.is(a2));

  Label profiler_disabled;
  Label end_profiler_check;
  __ li(t9, Operand(ExternalReference::is_profiling_address(isolate)));
  __ lb(t9, MemOperand(t9, 0));
  __ Branch(&profiler_disabled, eq, t9, Operand(zero_reg));

  // Additional parameter is the address of the actual callback.
  __ li(t9, Operand(thunk_ref));
  __ jmp(&end_profiler_check);

  __ bind(&profiler_disabled);
  __ mov(t9, function_address);
  __ bind(&end_profiler_check);

  // Allocate HandleScope in callee-save registers.
  __ li(s3, Operand(next_address));
  __ lw(s0, MemOperand(s3, kNextOffset));
  __ lw(s1, MemOperand(s3, kLimitOffset));
  __ lw(s2, MemOperand(s3, kLevelOffset));
  __ Addu(s2, s2, Operand(1));
  __ sw(s2, MemOperand(s3, kLevelOffset));

  if (FLAG_log_timer_events) {
    FrameScope frame(masm, StackFrame::MANUAL);
    __ PushSafepointRegisters();
    __ PrepareCallCFunction(1, a0);
    __ li(a0, Operand(ExternalReference::isolate_address(isolate)));
    __ CallCFunction(ExternalReference::log_enter_external_function(isolate),
                     1);
    __ PopSafepointRegisters();
  }

  // Native call returns to the DirectCEntry stub which redirects to the
  // return address pushed on stack (could have moved after GC).
  // DirectCEntry stub itself is generated early and never moves.
  DirectCEntryStub stub(isolate);
  stub.GenerateCall(masm, t9);

  if (FLAG_log_timer_events) {
    FrameScope frame(masm, StackFrame::MANUAL);
    __ PushSafepointRegisters();
    __ PrepareCallCFunction(1, a0);
    __ li(a0, Operand(ExternalReference::isolate_address(isolate)));
    __ CallCFunction(ExternalReference::log_leave_external_function(isolate),
                     1);
    __ PopSafepointRegisters();
  }

  Label promote_scheduled_exception;
  Label delete_allocated_handles;
  Label leave_exit_frame;
  Label return_value_loaded;

  // Load value from ReturnValue.
  __ lw(v0, return_value_operand);
  __ bind(&return_value_loaded);

  // No more valid handles (the result handle was the last one). Restore
  // previous handle scope.
  __ sw(s0, MemOperand(s3, kNextOffset));
  if (__ emit_debug_code()) {
    __ lw(a1, MemOperand(s3, kLevelOffset));
    __ Check(eq, kUnexpectedLevelAfterReturnFromApiCall, a1, Operand(s2));
  }
  __ Subu(s2, s2, Operand(1));
  __ sw(s2, MemOperand(s3, kLevelOffset));
  __ lw(at, MemOperand(s3, kLimitOffset));
  __ Branch(&delete_allocated_handles, ne, s1, Operand(at));

  // Leave the API exit frame.
  __ bind(&leave_exit_frame);

  bool restore_context = context_restore_operand != NULL;
  if (restore_context) {
    __ lw(cp, *context_restore_operand);
  }
  if (stack_space_offset != kInvalidStackOffset) {
    // ExitFrame contains four MIPS argument slots after DirectCEntryStub call
    // so this must be accounted for.
    __ lw(s0, MemOperand(sp, stack_space_offset + kCArgsSlotsSize));
  } else {
    __ li(s0, Operand(stack_space));
  }
  __ LeaveExitFrame(false, s0, !restore_context, NO_EMIT_RETURN,
                    stack_space_offset != kInvalidStackOffset);

  // Check if the function scheduled an exception.
  __ LoadRoot(t0, Heap::kTheHoleValueRootIndex);
  __ li(at, Operand(ExternalReference::scheduled_exception_address(isolate)));
  __ lw(t1, MemOperand(at));
  __ Branch(&promote_scheduled_exception, ne, t0, Operand(t1));

  __ Ret();

  // Re-throw by promoting a scheduled exception.
  __ bind(&promote_scheduled_exception);
  __ TailCallRuntime(Runtime::kPromoteScheduledException);

  // HandleScope limit has changed. Delete allocated extensions.
  __ bind(&delete_allocated_handles);
  __ sw(s1, MemOperand(s3, kLimitOffset));
  __ mov(s0, v0);
  __ mov(a0, v0);
  __ PrepareCallCFunction(1, s1);
  __ li(a0, Operand(ExternalReference::isolate_address(isolate)));
  __ CallCFunction(ExternalReference::delete_handle_scope_extensions(isolate),
                   1);
  __ mov(v0, s0);
  __ jmp(&leave_exit_frame);
}

void CallApiCallbackStub::Generate(MacroAssembler* masm) {
  // ----------- S t a t e -------------
  //  -- a0                  : callee
  //  -- t0                  : call_data
  //  -- a2                  : holder
  //  -- a1                  : api_function_address
  //  -- cp                  : context
  //  --
  //  -- sp[0]               : last argument
  //  -- ...
  //  -- sp[(argc - 1)* 4]   : first argument
  //  -- sp[argc * 4]        : receiver
  //  -- sp[(argc + 1)* 4]   : accessor_holder
  // -----------------------------------

  Register callee = a0;
  Register call_data = t0;
  Register holder = a2;
  Register api_function_address = a1;
  Register context = cp;

  typedef FunctionCallbackArguments FCA;

  STATIC_ASSERT(FCA::kArgsLength == 8);
  STATIC_ASSERT(FCA::kNewTargetIndex == 7);
  STATIC_ASSERT(FCA::kContextSaveIndex == 6);
  STATIC_ASSERT(FCA::kCalleeIndex == 5);
  STATIC_ASSERT(FCA::kDataIndex == 4);
  STATIC_ASSERT(FCA::kReturnValueOffset == 3);
  STATIC_ASSERT(FCA::kReturnValueDefaultValueIndex == 2);
  STATIC_ASSERT(FCA::kIsolateIndex == 1);
  STATIC_ASSERT(FCA::kHolderIndex == 0);

  // new target
  __ PushRoot(Heap::kUndefinedValueRootIndex);

  // Save context, callee and call data.
  __ Push(context, callee, call_data);

  Register scratch = call_data;
  __ LoadRoot(scratch, Heap::kUndefinedValueRootIndex);
  // Push return value and default return value.
  __ Push(scratch, scratch);
  __ li(scratch, Operand(ExternalReference::isolate_address(masm->isolate())));
  // Push isolate and holder.
  __ Push(scratch, holder);

  // Enter a new context
  if (is_lazy()) {
    // ----------- S t a t e -------------------------------------
    //  -- sp[0]                                 : holder
    //  -- ...
    //  -- sp[(FCA::kArgsLength - 1) * 4]        : new_target
    //  -- sp[FCA::kArgsLength * 4]              : last argument
    //  -- ...
    //  -- sp[(FCA::kArgsLength + argc - 1) * 4] : first argument
    //  -- sp[(FCA::kArgsLength + argc) * 4]     : receiver
    //  -- sp[(FCA::kArgsLength + argc + 1) * 4] : accessor_holder
    // -----------------------------------------------------------

    // Load context from accessor_holder
    Register accessor_holder = context;
    Register scratch2 = callee;
    __ lw(accessor_holder,
          MemOperand(sp, (FCA::kArgsLength + 1 + argc()) * kPointerSize));
    // Look for the constructor if |accessor_holder| is not a function.
    Label skip_looking_for_constructor;
    __ lw(scratch, FieldMemOperand(accessor_holder, HeapObject::kMapOffset));
    __ lbu(scratch2, FieldMemOperand(scratch, Map::kBitFieldOffset));
    __ And(scratch2, scratch2, Operand(1 << Map::kIsConstructor));
    __ Branch(&skip_looking_for_constructor, ne, scratch2, Operand(zero_reg));
    __ GetMapConstructor(context, scratch, scratch, scratch2);
    __ bind(&skip_looking_for_constructor);
    __ lw(context, FieldMemOperand(context, JSFunction::kContextOffset));
  } else {
    // Load context from callee.
    __ lw(context, FieldMemOperand(callee, JSFunction::kContextOffset));
  }

  // Prepare arguments.
  __ mov(scratch, sp);

  // Allocate the v8::Arguments structure in the arguments' space since
  // it's not controlled by GC.
  const int kApiStackSpace = 3;

  FrameScope frame_scope(masm, StackFrame::MANUAL);
  __ EnterExitFrame(false, kApiStackSpace);

  DCHECK(!api_function_address.is(a0) && !scratch.is(a0));
  // a0 = FunctionCallbackInfo&
  // Arguments is after the return address.
  __ Addu(a0, sp, Operand(1 * kPointerSize));
  // FunctionCallbackInfo::implicit_args_
  __ sw(scratch, MemOperand(a0, 0 * kPointerSize));
  // FunctionCallbackInfo::values_
  __ Addu(at, scratch, Operand((FCA::kArgsLength - 1 + argc()) * kPointerSize));
  __ sw(at, MemOperand(a0, 1 * kPointerSize));
  // FunctionCallbackInfo::length_ = argc
  __ li(at, Operand(argc()));
  __ sw(at, MemOperand(a0, 2 * kPointerSize));

  ExternalReference thunk_ref =
      ExternalReference::invoke_function_callback(masm->isolate());

  AllowExternalCallThatCantCauseGC scope(masm);
  MemOperand context_restore_operand(
      fp, (2 + FCA::kContextSaveIndex) * kPointerSize);
  // Stores return the first js argument.
  int return_value_offset = 0;
  if (is_store()) {
    return_value_offset = 2 + FCA::kArgsLength;
  } else {
    return_value_offset = 2 + FCA::kReturnValueOffset;
  }
  MemOperand return_value_operand(fp, return_value_offset * kPointerSize);
  const int stack_space = argc() + FCA::kArgsLength + 2;
  // TODO(adamk): Why are we clobbering this immediately?
  const int32_t stack_space_offset = kInvalidStackOffset;
  CallApiFunctionAndReturn(masm, api_function_address, thunk_ref, stack_space,
                           stack_space_offset, return_value_operand,
                           &context_restore_operand);
}


void CallApiGetterStub::Generate(MacroAssembler* masm) {
  // Build v8::PropertyCallbackInfo::args_ array on the stack and push property
  // name below the exit frame to make GC aware of them.
  STATIC_ASSERT(PropertyCallbackArguments::kShouldThrowOnErrorIndex == 0);
  STATIC_ASSERT(PropertyCallbackArguments::kHolderIndex == 1);
  STATIC_ASSERT(PropertyCallbackArguments::kIsolateIndex == 2);
  STATIC_ASSERT(PropertyCallbackArguments::kReturnValueDefaultValueIndex == 3);
  STATIC_ASSERT(PropertyCallbackArguments::kReturnValueOffset == 4);
  STATIC_ASSERT(PropertyCallbackArguments::kDataIndex == 5);
  STATIC_ASSERT(PropertyCallbackArguments::kThisIndex == 6);
  STATIC_ASSERT(PropertyCallbackArguments::kArgsLength == 7);

  Register receiver = ApiGetterDescriptor::ReceiverRegister();
  Register holder = ApiGetterDescriptor::HolderRegister();
  Register callback = ApiGetterDescriptor::CallbackRegister();
  Register scratch = t0;
  DCHECK(!AreAliased(receiver, holder, callback, scratch));

  Register api_function_address = a2;

  // Here and below +1 is for name() pushed after the args_ array.
  typedef PropertyCallbackArguments PCA;
  __ Subu(sp, sp, (PCA::kArgsLength + 1) * kPointerSize);
  __ sw(receiver, MemOperand(sp, (PCA::kThisIndex + 1) * kPointerSize));
  __ lw(scratch, FieldMemOperand(callback, AccessorInfo::kDataOffset));
  __ sw(scratch, MemOperand(sp, (PCA::kDataIndex + 1) * kPointerSize));
  __ LoadRoot(scratch, Heap::kUndefinedValueRootIndex);
  __ sw(scratch, MemOperand(sp, (PCA::kReturnValueOffset + 1) * kPointerSize));
  __ sw(scratch, MemOperand(sp, (PCA::kReturnValueDefaultValueIndex + 1) *
                                    kPointerSize));
  __ li(scratch, Operand(ExternalReference::isolate_address(isolate())));
  __ sw(scratch, MemOperand(sp, (PCA::kIsolateIndex + 1) * kPointerSize));
  __ sw(holder, MemOperand(sp, (PCA::kHolderIndex + 1) * kPointerSize));
  // should_throw_on_error -> false
  DCHECK(Smi::kZero == nullptr);
  __ sw(zero_reg,
        MemOperand(sp, (PCA::kShouldThrowOnErrorIndex + 1) * kPointerSize));
  __ lw(scratch, FieldMemOperand(callback, AccessorInfo::kNameOffset));
  __ sw(scratch, MemOperand(sp, 0 * kPointerSize));

  // v8::PropertyCallbackInfo::args_ array and name handle.
  const int kStackUnwindSpace = PropertyCallbackArguments::kArgsLength + 1;

  // Load address of v8::PropertyAccessorInfo::args_ array and name handle.
  __ mov(a0, sp);                              // a0 = Handle<Name>
  __ Addu(a1, a0, Operand(1 * kPointerSize));  // a1 = v8::PCI::args_

  const int kApiStackSpace = 1;
  FrameScope frame_scope(masm, StackFrame::MANUAL);
  __ EnterExitFrame(false, kApiStackSpace);

  // Create v8::PropertyCallbackInfo object on the stack and initialize
  // it's args_ field.
  __ sw(a1, MemOperand(sp, 1 * kPointerSize));
  __ Addu(a1, sp, Operand(1 * kPointerSize));  // a1 = v8::PropertyCallbackInfo&

  ExternalReference thunk_ref =
      ExternalReference::invoke_accessor_getter_callback(isolate());

  __ lw(scratch, FieldMemOperand(callback, AccessorInfo::kJsGetterOffset));
  __ lw(api_function_address,
        FieldMemOperand(scratch, Foreign::kForeignAddressOffset));

  // +3 is to skip prolog, return address and name handle.
  MemOperand return_value_operand(
      fp, (PropertyCallbackArguments::kReturnValueOffset + 3) * kPointerSize);
  CallApiFunctionAndReturn(masm, api_function_address, thunk_ref,
                           kStackUnwindSpace, kInvalidStackOffset,
                           return_value_operand, NULL);
}

#undef __

}  // namespace internal
}  // namespace v8

#endif  // V8_TARGET_ARCH_MIPS
