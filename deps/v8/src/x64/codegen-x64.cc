// Copyright 2012 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/x64/codegen-x64.h"

#if V8_TARGET_ARCH_X64

#include "src/codegen.h"
#include "src/macro-assembler.h"
#include "src/x64/assembler-x64-inl.h"

namespace v8 {
namespace internal {

#define __ masm.


UnaryMathFunctionWithIsolate CreateSqrtFunction(Isolate* isolate) {
  size_t actual_size;
  // Allocate buffer in executable space.
  byte* buffer = static_cast<byte*>(base::OS::Allocate(
      1 * KB, &actual_size, true, isolate->heap()->GetRandomMmapAddr()));
  if (buffer == nullptr) return nullptr;

  MacroAssembler masm(isolate, buffer, static_cast<int>(actual_size),
                      CodeObjectRequired::kNo);
  // xmm0: raw double input.
  // Move double input into registers.
  __ Sqrtsd(xmm0, xmm0);
  __ Ret();

  CodeDesc desc;
  masm.GetCode(isolate, &desc);
  DCHECK(!RelocInfo::RequiresRelocation(isolate, desc));

  Assembler::FlushICache(isolate, buffer, actual_size);
  base::OS::ProtectCode(buffer, actual_size);
  return FUNCTION_CAST<UnaryMathFunctionWithIsolate>(buffer);
}

#undef __

// -------------------------------------------------------------------------
// Code generators

#define __ ACCESS_MASM(masm)

void StringCharLoadGenerator::Generate(MacroAssembler* masm,
                                       Register string,
                                       Register index,
                                       Register result,
                                       Label* call_runtime) {
  Label indirect_string_loaded;
  __ bind(&indirect_string_loaded);

  // Fetch the instance type of the receiver into result register.
  __ movp(result, FieldOperand(string, HeapObject::kMapOffset));
  __ movzxbl(result, FieldOperand(result, Map::kInstanceTypeOffset));

  // We need special handling for indirect strings.
  Label check_sequential;
  __ testb(result, Immediate(kIsIndirectStringMask));
  __ j(zero, &check_sequential, Label::kNear);

  // Dispatch on the indirect string shape: slice or cons.
  Label cons_string, thin_string;
  __ andl(result, Immediate(kStringRepresentationMask));
  __ cmpl(result, Immediate(kConsStringTag));
  __ j(equal, &cons_string, Label::kNear);
  __ cmpl(result, Immediate(kThinStringTag));
  __ j(equal, &thin_string, Label::kNear);

  // Handle slices.
  __ SmiToInteger32(result, FieldOperand(string, SlicedString::kOffsetOffset));
  __ addp(index, result);
  __ movp(string, FieldOperand(string, SlicedString::kParentOffset));
  __ jmp(&indirect_string_loaded);

  // Handle thin strings.
  __ bind(&thin_string);
  __ movp(string, FieldOperand(string, ThinString::kActualOffset));
  __ jmp(&indirect_string_loaded);

  // Handle cons strings.
  // Check whether the right hand side is the empty string (i.e. if
  // this is really a flat string in a cons string). If that is not
  // the case we would rather go to the runtime system now to flatten
  // the string.
  __ bind(&cons_string);
  __ CompareRoot(FieldOperand(string, ConsString::kSecondOffset),
                 Heap::kempty_stringRootIndex);
  __ j(not_equal, call_runtime);
  __ movp(string, FieldOperand(string, ConsString::kFirstOffset));
  __ jmp(&indirect_string_loaded);

  // Distinguish sequential and external strings. Only these two string
  // representations can reach here (slices and flat cons strings have been
  // reduced to the underlying sequential or external string).
  Label seq_string;
  __ bind(&check_sequential);
  STATIC_ASSERT(kSeqStringTag == 0);
  __ testb(result, Immediate(kStringRepresentationMask));
  __ j(zero, &seq_string, Label::kNear);

  // Handle external strings.
  Label one_byte_external, done;
  if (FLAG_debug_code) {
    // Assert that we do not have a cons or slice (indirect strings) here.
    // Sequential strings have already been ruled out.
    __ testb(result, Immediate(kIsIndirectStringMask));
    __ Assert(zero, kExternalStringExpectedButNotFound);
  }
  // Rule out short external strings.
  STATIC_ASSERT(kShortExternalStringTag != 0);
  __ testb(result, Immediate(kShortExternalStringTag));
  __ j(not_zero, call_runtime);
  // Check encoding.
  STATIC_ASSERT(kTwoByteStringTag == 0);
  __ testb(result, Immediate(kStringEncodingMask));
  __ movp(result, FieldOperand(string, ExternalString::kResourceDataOffset));
  __ j(not_equal, &one_byte_external, Label::kNear);
  // Two-byte string.
  __ movzxwl(result, Operand(result, index, times_2, 0));
  __ jmp(&done, Label::kNear);
  __ bind(&one_byte_external);
  // One-byte string.
  __ movzxbl(result, Operand(result, index, times_1, 0));
  __ jmp(&done, Label::kNear);

  // Dispatch on the encoding: one-byte or two-byte.
  Label one_byte;
  __ bind(&seq_string);
  STATIC_ASSERT((kStringEncodingMask & kOneByteStringTag) != 0);
  STATIC_ASSERT((kStringEncodingMask & kTwoByteStringTag) == 0);
  __ testb(result, Immediate(kStringEncodingMask));
  __ j(not_zero, &one_byte, Label::kNear);

  // Two-byte string.
  // Load the two-byte character code into the result register.
  STATIC_ASSERT(kSmiTag == 0 && kSmiTagSize == 1);
  __ movzxwl(result, FieldOperand(string,
                                  index,
                                  times_2,
                                  SeqTwoByteString::kHeaderSize));
  __ jmp(&done, Label::kNear);

  // One-byte string.
  // Load the byte into the result register.
  __ bind(&one_byte);
  __ movzxbl(result, FieldOperand(string,
                                  index,
                                  times_1,
                                  SeqOneByteString::kHeaderSize));
  __ bind(&done);
}

#undef __

Operand StackArgumentsAccessor::GetArgumentOperand(int index) {
  DCHECK(index >= 0);
  int receiver = (receiver_mode_ == ARGUMENTS_CONTAIN_RECEIVER) ? 1 : 0;
  int displacement_to_last_argument = base_reg_.is(rsp) ?
      kPCOnStackSize : kFPOnStackSize + kPCOnStackSize;
  displacement_to_last_argument += extra_displacement_to_last_argument_;
  if (argument_count_reg_.is(no_reg)) {
    // argument[0] is at base_reg_ + displacement_to_last_argument +
    // (argument_count_immediate_ + receiver - 1) * kPointerSize.
    DCHECK(argument_count_immediate_ + receiver > 0);
    return Operand(base_reg_, displacement_to_last_argument +
        (argument_count_immediate_ + receiver - 1 - index) * kPointerSize);
  } else {
    // argument[0] is at base_reg_ + displacement_to_last_argument +
    // argument_count_reg_ * times_pointer_size + (receiver - 1) * kPointerSize.
    return Operand(base_reg_, argument_count_reg_, times_pointer_size,
        displacement_to_last_argument + (receiver - 1 - index) * kPointerSize);
  }
}


}  // namespace internal
}  // namespace v8

#endif  // V8_TARGET_ARCH_X64
