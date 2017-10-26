// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_ARM64_MACRO_ASSEMBLER_ARM64_H_
#define V8_ARM64_MACRO_ASSEMBLER_ARM64_H_

#include <vector>

#include "src/arm64/assembler-arm64.h"
#include "src/bailout-reason.h"
#include "src/base/bits.h"
#include "src/globals.h"

// Simulator specific helpers.
#if USE_SIMULATOR
  // TODO(all): If possible automatically prepend an indicator like
  // UNIMPLEMENTED or LOCATION.
  #define ASM_UNIMPLEMENTED(message)                                         \
  __ Debug(message, __LINE__, NO_PARAM)
  #define ASM_UNIMPLEMENTED_BREAK(message)                                   \
  __ Debug(message, __LINE__,                                                \
           FLAG_ignore_asm_unimplemented_break ? NO_PARAM : BREAK)
#if DEBUG
#define ASM_LOCATION(message) __ Debug("LOCATION: " message, __LINE__, NO_PARAM)
#define ASM_LOCATION_IN_ASSEMBLER(message) \
  Debug("LOCATION: " message, __LINE__, NO_PARAM)
#else
#define ASM_LOCATION(message)
#define ASM_LOCATION_IN_ASSEMBLER(message)
#endif
#else
#define ASM_UNIMPLEMENTED(message)
#define ASM_UNIMPLEMENTED_BREAK(message)
#define ASM_LOCATION(message)
#define ASM_LOCATION_IN_ASSEMBLER(message)
#endif


namespace v8 {
namespace internal {

// Give alias names to registers for calling conventions.
#define kReturnRegister0 x0
#define kReturnRegister1 x1
#define kReturnRegister2 x2
#define kJSFunctionRegister x1
#define kContextRegister cp
#define kAllocateSizeRegister x1
#define kInterpreterAccumulatorRegister x0
#define kInterpreterBytecodeOffsetRegister x19
#define kInterpreterBytecodeArrayRegister x20
#define kInterpreterDispatchTableRegister x21
#define kJavaScriptCallArgCountRegister x0
#define kJavaScriptCallNewTargetRegister x3
#define kRuntimeCallFunctionRegister x1
#define kRuntimeCallArgCountRegister x0

#define LS_MACRO_LIST(V)                                     \
  V(Ldrb, Register&, rt, LDRB_w)                             \
  V(Strb, Register&, rt, STRB_w)                             \
  V(Ldrsb, Register&, rt, rt.Is64Bits() ? LDRSB_x : LDRSB_w) \
  V(Ldrh, Register&, rt, LDRH_w)                             \
  V(Strh, Register&, rt, STRH_w)                             \
  V(Ldrsh, Register&, rt, rt.Is64Bits() ? LDRSH_x : LDRSH_w) \
  V(Ldr, CPURegister&, rt, LoadOpFor(rt))                    \
  V(Str, CPURegister&, rt, StoreOpFor(rt))                   \
  V(Ldrsw, Register&, rt, LDRSW_x)

#define LSPAIR_MACRO_LIST(V)                             \
  V(Ldp, CPURegister&, rt, rt2, LoadPairOpFor(rt, rt2))  \
  V(Stp, CPURegister&, rt, rt2, StorePairOpFor(rt, rt2)) \
  V(Ldpsw, CPURegister&, rt, rt2, LDPSW_x)

#define LDA_STL_MACRO_LIST(V) \
  V(Ldarb, ldarb)             \
  V(Ldarh, ldarh)             \
  V(Ldar, ldar)               \
  V(Ldaxrb, ldaxrb)           \
  V(Ldaxrh, ldaxrh)           \
  V(Ldaxr, ldaxr)             \
  V(Stlrb, stlrb)             \
  V(Stlrh, stlrh)             \
  V(Stlr, stlr)

#define STLX_MACRO_LIST(V) \
  V(Stlxrb, stlxrb)        \
  V(Stlxrh, stlxrh)        \
  V(Stlxr, stlxr)

// ----------------------------------------------------------------------------
// Static helper functions

// Generate a MemOperand for loading a field from an object.
inline MemOperand FieldMemOperand(Register object, int offset);
inline MemOperand UntagSmiFieldMemOperand(Register object, int offset);

// Generate a MemOperand for loading a SMI from memory.
inline MemOperand UntagSmiMemOperand(Register object, int offset);


// ----------------------------------------------------------------------------
// MacroAssembler

enum BranchType {
  // Copies of architectural conditions.
  // The associated conditions can be used in place of those, the code will
  // take care of reinterpreting them with the correct type.
  integer_eq = eq,
  integer_ne = ne,
  integer_hs = hs,
  integer_lo = lo,
  integer_mi = mi,
  integer_pl = pl,
  integer_vs = vs,
  integer_vc = vc,
  integer_hi = hi,
  integer_ls = ls,
  integer_ge = ge,
  integer_lt = lt,
  integer_gt = gt,
  integer_le = le,
  integer_al = al,
  integer_nv = nv,

  // These two are *different* from the architectural codes al and nv.
  // 'always' is used to generate unconditional branches.
  // 'never' is used to not generate a branch (generally as the inverse
  // branch type of 'always).
  always, never,
  // cbz and cbnz
  reg_zero, reg_not_zero,
  // tbz and tbnz
  reg_bit_clear, reg_bit_set,

  // Aliases.
  kBranchTypeFirstCondition = eq,
  kBranchTypeLastCondition = nv,
  kBranchTypeFirstUsingReg = reg_zero,
  kBranchTypeFirstUsingBit = reg_bit_clear
};

inline BranchType InvertBranchType(BranchType type) {
  if (kBranchTypeFirstCondition <= type && type <= kBranchTypeLastCondition) {
    return static_cast<BranchType>(
        NegateCondition(static_cast<Condition>(type)));
  } else {
    return static_cast<BranchType>(type ^ 1);
  }
}

enum RememberedSetAction { EMIT_REMEMBERED_SET, OMIT_REMEMBERED_SET };
enum SmiCheck { INLINE_SMI_CHECK, OMIT_SMI_CHECK };
enum PointersToHereCheck {
  kPointersToHereMaybeInteresting,
  kPointersToHereAreAlwaysInteresting
};
enum LinkRegisterStatus { kLRHasNotBeenSaved, kLRHasBeenSaved };
enum TargetAddressStorageMode {
  CAN_INLINE_TARGET_ADDRESS,
  NEVER_INLINE_TARGET_ADDRESS
};
enum UntagMode { kNotSpeculativeUntag, kSpeculativeUntag };
enum ArrayHasHoles { kArrayCantHaveHoles, kArrayCanHaveHoles };
enum CopyHint { kCopyUnknown, kCopyShort, kCopyLong };
enum DiscardMoveMode { kDontDiscardForSameWReg, kDiscardForSameWReg };
enum SeqStringSetCharCheckIndexType { kIndexIsSmi, kIndexIsInteger32 };

// The macro assembler supports moving automatically pre-shifted immediates for
// arithmetic and logical instructions, and then applying a post shift in the
// instruction to undo the modification, in order to reduce the code emitted for
// an operation. For example:
//
//  Add(x0, x0, 0x1f7de) => movz x16, 0xfbef; add x0, x0, x16, lsl #1.
//
// This optimisation can be only partially applied when the stack pointer is an
// operand or destination, so this enumeration is used to control the shift.
enum PreShiftImmMode {
  kNoShift,          // Don't pre-shift.
  kLimitShiftForSP,  // Limit pre-shift for add/sub extend use.
  kAnyShift          // Allow any pre-shift.
};

class TurboAssembler : public Assembler {
 public:
  TurboAssembler(Isolate* isolate, void* buffer, int buffer_size,
                 CodeObjectRequired create_code_object)
      : Assembler(isolate, buffer, buffer_size),
        isolate_(isolate),
#if DEBUG
        allow_macro_instructions_(true),
#endif
        tmp_list_(DefaultTmpList()),
        fptmp_list_(DefaultFPTmpList()),
        sp_(jssp),
        use_real_aborts_(true) {
    if (create_code_object == CodeObjectRequired::kYes) {
      code_object_ =
          Handle<HeapObject>::New(isolate->heap()->undefined_value(), isolate);
    }
  }

  // The Abort method should call a V8 runtime function, but the CallRuntime
  // mechanism depends on CEntryStub. If use_real_aborts is false, Abort will
  // use a simpler abort mechanism that doesn't depend on CEntryStub.
  //
  // The purpose of this is to allow Aborts to be compiled whilst CEntryStub is
  // being generated.
  bool use_real_aborts() const { return use_real_aborts_; }

  class NoUseRealAbortsScope {
   public:
    explicit NoUseRealAbortsScope(TurboAssembler* tasm)
        : saved_(tasm->use_real_aborts_), tasm_(tasm) {
      tasm_->use_real_aborts_ = false;
    }
    ~NoUseRealAbortsScope() { tasm_->use_real_aborts_ = saved_; }

   private:
    bool saved_;
    TurboAssembler* tasm_;
  };

  void set_has_frame(bool value) { has_frame_ = value; }
  bool has_frame() const { return has_frame_; }

  Isolate* isolate() const { return isolate_; }

  Handle<HeapObject> CodeObject() {
    DCHECK(!code_object_.is_null());
    return code_object_;
  }

#if DEBUG
  void set_allow_macro_instructions(bool value) {
    allow_macro_instructions_ = value;
  }
  bool allow_macro_instructions() const { return allow_macro_instructions_; }
#endif

  // Set the current stack pointer, but don't generate any code.
  inline void SetStackPointer(const Register& stack_pointer) {
    DCHECK(!TmpList()->IncludesAliasOf(stack_pointer));
    sp_ = stack_pointer;
  }

  // Activation support.
  void EnterFrame(StackFrame::Type type);
  void EnterFrame(StackFrame::Type type, bool load_constant_pool_pointer_reg) {
    // Out-of-line constant pool not implemented on arm64.
    UNREACHABLE();
  }
  void LeaveFrame(StackFrame::Type type);

  inline void InitializeRootRegister();

  void Mov(const Register& rd, const Operand& operand,
           DiscardMoveMode discard_mode = kDontDiscardForSameWReg);
  void Mov(const Register& rd, uint64_t imm);
  inline void Mov(const Register& rd, const Register& rm);
  void Mov(const VRegister& vd, int vd_index, const VRegister& vn,
           int vn_index) {
    DCHECK(allow_macro_instructions());
    mov(vd, vd_index, vn, vn_index);
  }
  void Mov(const VRegister& vd, const VRegister& vn, int index) {
    DCHECK(allow_macro_instructions());
    mov(vd, vn, index);
  }
  void Mov(const VRegister& vd, int vd_index, const Register& rn) {
    DCHECK(allow_macro_instructions());
    mov(vd, vd_index, rn);
  }
  void Mov(const Register& rd, const VRegister& vn, int vn_index) {
    DCHECK(allow_macro_instructions());
    mov(rd, vn, vn_index);
  }

  // This is required for compatibility with architecture independent code.
  // Remove if not needed.
  void Move(Register dst, Register src);
  void Move(Register dst, Handle<HeapObject> x);
  void Move(Register dst, Smi* src);

// NEON by element instructions.
#define NEON_BYELEMENT_MACRO_LIST(V) \
  V(fmla, Fmla)                      \
  V(fmls, Fmls)                      \
  V(fmul, Fmul)                      \
  V(fmulx, Fmulx)                    \
  V(mul, Mul)                        \
  V(mla, Mla)                        \
  V(mls, Mls)                        \
  V(sqdmulh, Sqdmulh)                \
  V(sqrdmulh, Sqrdmulh)              \
  V(sqdmull, Sqdmull)                \
  V(sqdmull2, Sqdmull2)              \
  V(sqdmlal, Sqdmlal)                \
  V(sqdmlal2, Sqdmlal2)              \
  V(sqdmlsl, Sqdmlsl)                \
  V(sqdmlsl2, Sqdmlsl2)              \
  V(smull, Smull)                    \
  V(smull2, Smull2)                  \
  V(smlal, Smlal)                    \
  V(smlal2, Smlal2)                  \
  V(smlsl, Smlsl)                    \
  V(smlsl2, Smlsl2)                  \
  V(umull, Umull)                    \
  V(umull2, Umull2)                  \
  V(umlal, Umlal)                    \
  V(umlal2, Umlal2)                  \
  V(umlsl, Umlsl)                    \
  V(umlsl2, Umlsl2)

#define DEFINE_MACRO_ASM_FUNC(ASM, MASM)                                   \
  void MASM(const VRegister& vd, const VRegister& vn, const VRegister& vm, \
            int vm_index) {                                                \
    DCHECK(allow_macro_instructions());                                    \
    ASM(vd, vn, vm, vm_index);                                             \
  }
  NEON_BYELEMENT_MACRO_LIST(DEFINE_MACRO_ASM_FUNC)
#undef DEFINE_MACRO_ASM_FUNC

// NEON 2 vector register instructions.
#define NEON_2VREG_MACRO_LIST(V) \
  V(abs, Abs)                    \
  V(addp, Addp)                  \
  V(addv, Addv)                  \
  V(cls, Cls)                    \
  V(clz, Clz)                    \
  V(cnt, Cnt)                    \
  V(faddp, Faddp)                \
  V(fcvtas, Fcvtas)              \
  V(fcvtau, Fcvtau)              \
  V(fcvtms, Fcvtms)              \
  V(fcvtmu, Fcvtmu)              \
  V(fcvtns, Fcvtns)              \
  V(fcvtnu, Fcvtnu)              \
  V(fcvtps, Fcvtps)              \
  V(fcvtpu, Fcvtpu)              \
  V(fmaxnmp, Fmaxnmp)            \
  V(fmaxnmv, Fmaxnmv)            \
  V(fmaxp, Fmaxp)                \
  V(fmaxv, Fmaxv)                \
  V(fminnmp, Fminnmp)            \
  V(fminnmv, Fminnmv)            \
  V(fminp, Fminp)                \
  V(fminv, Fminv)                \
  V(fneg, Fneg)                  \
  V(frecpe, Frecpe)              \
  V(frecpx, Frecpx)              \
  V(frinta, Frinta)              \
  V(frinti, Frinti)              \
  V(frintm, Frintm)              \
  V(frintn, Frintn)              \
  V(frintp, Frintp)              \
  V(frintx, Frintx)              \
  V(frintz, Frintz)              \
  V(frsqrte, Frsqrte)            \
  V(fsqrt, Fsqrt)                \
  V(mov, Mov)                    \
  V(mvn, Mvn)                    \
  V(neg, Neg)                    \
  V(not_, Not)                   \
  V(rbit, Rbit)                  \
  V(rev16, Rev16)                \
  V(rev32, Rev32)                \
  V(rev64, Rev64)                \
  V(sadalp, Sadalp)              \
  V(saddlp, Saddlp)              \
  V(saddlv, Saddlv)              \
  V(smaxv, Smaxv)                \
  V(sminv, Sminv)                \
  V(sqabs, Sqabs)                \
  V(sqneg, Sqneg)                \
  V(sqxtn2, Sqxtn2)              \
  V(sqxtn, Sqxtn)                \
  V(sqxtun2, Sqxtun2)            \
  V(sqxtun, Sqxtun)              \
  V(suqadd, Suqadd)              \
  V(sxtl2, Sxtl2)                \
  V(sxtl, Sxtl)                  \
  V(uadalp, Uadalp)              \
  V(uaddlp, Uaddlp)              \
  V(uaddlv, Uaddlv)              \
  V(umaxv, Umaxv)                \
  V(uminv, Uminv)                \
  V(uqxtn2, Uqxtn2)              \
  V(uqxtn, Uqxtn)                \
  V(urecpe, Urecpe)              \
  V(ursqrte, Ursqrte)            \
  V(usqadd, Usqadd)              \
  V(uxtl2, Uxtl2)                \
  V(uxtl, Uxtl)                  \
  V(xtn2, Xtn2)                  \
  V(xtn, Xtn)

#define DEFINE_MACRO_ASM_FUNC(ASM, MASM)                \
  void MASM(const VRegister& vd, const VRegister& vn) { \
    DCHECK(allow_macro_instructions());                 \
    ASM(vd, vn);                                        \
  }
  NEON_2VREG_MACRO_LIST(DEFINE_MACRO_ASM_FUNC)
#undef DEFINE_MACRO_ASM_FUNC
#undef NEON_2VREG_MACRO_LIST

// NEON 2 vector register with immediate instructions.
#define NEON_2VREG_FPIMM_MACRO_LIST(V) \
  V(fcmeq, Fcmeq)                      \
  V(fcmge, Fcmge)                      \
  V(fcmgt, Fcmgt)                      \
  V(fcmle, Fcmle)                      \
  V(fcmlt, Fcmlt)

#define DEFINE_MACRO_ASM_FUNC(ASM, MASM)                            \
  void MASM(const VRegister& vd, const VRegister& vn, double imm) { \
    DCHECK(allow_macro_instructions());                             \
    ASM(vd, vn, imm);                                               \
  }
  NEON_2VREG_FPIMM_MACRO_LIST(DEFINE_MACRO_ASM_FUNC)
#undef DEFINE_MACRO_ASM_FUNC

// NEON 3 vector register instructions.
#define NEON_3VREG_MACRO_LIST(V) \
  V(add, Add)                    \
  V(addhn2, Addhn2)              \
  V(addhn, Addhn)                \
  V(addp, Addp)                  \
  V(and_, And)                   \
  V(bic, Bic)                    \
  V(bif, Bif)                    \
  V(bit, Bit)                    \
  V(bsl, Bsl)                    \
  V(cmeq, Cmeq)                  \
  V(cmge, Cmge)                  \
  V(cmgt, Cmgt)                  \
  V(cmhi, Cmhi)                  \
  V(cmhs, Cmhs)                  \
  V(cmtst, Cmtst)                \
  V(eor, Eor)                    \
  V(fabd, Fabd)                  \
  V(facge, Facge)                \
  V(facgt, Facgt)                \
  V(faddp, Faddp)                \
  V(fcmeq, Fcmeq)                \
  V(fcmge, Fcmge)                \
  V(fcmgt, Fcmgt)                \
  V(fmaxnmp, Fmaxnmp)            \
  V(fmaxp, Fmaxp)                \
  V(fminnmp, Fminnmp)            \
  V(fminp, Fminp)                \
  V(fmla, Fmla)                  \
  V(fmls, Fmls)                  \
  V(fmulx, Fmulx)                \
  V(frecps, Frecps)              \
  V(frsqrts, Frsqrts)            \
  V(mla, Mla)                    \
  V(mls, Mls)                    \
  V(mul, Mul)                    \
  V(orn, Orn)                    \
  V(orr, Orr)                    \
  V(pmull2, Pmull2)              \
  V(pmull, Pmull)                \
  V(pmul, Pmul)                  \
  V(raddhn2, Raddhn2)            \
  V(raddhn, Raddhn)              \
  V(rsubhn2, Rsubhn2)            \
  V(rsubhn, Rsubhn)              \
  V(sabal2, Sabal2)              \
  V(sabal, Sabal)                \
  V(saba, Saba)                  \
  V(sabdl2, Sabdl2)              \
  V(sabdl, Sabdl)                \
  V(sabd, Sabd)                  \
  V(saddl2, Saddl2)              \
  V(saddl, Saddl)                \
  V(saddw2, Saddw2)              \
  V(saddw, Saddw)                \
  V(shadd, Shadd)                \
  V(shsub, Shsub)                \
  V(smaxp, Smaxp)                \
  V(smax, Smax)                  \
  V(sminp, Sminp)                \
  V(smin, Smin)                  \
  V(smlal2, Smlal2)              \
  V(smlal, Smlal)                \
  V(smlsl2, Smlsl2)              \
  V(smlsl, Smlsl)                \
  V(smull2, Smull2)              \
  V(smull, Smull)                \
  V(sqadd, Sqadd)                \
  V(sqdmlal2, Sqdmlal2)          \
  V(sqdmlal, Sqdmlal)            \
  V(sqdmlsl2, Sqdmlsl2)          \
  V(sqdmlsl, Sqdmlsl)            \
  V(sqdmulh, Sqdmulh)            \
  V(sqdmull2, Sqdmull2)          \
  V(sqdmull, Sqdmull)            \
  V(sqrdmulh, Sqrdmulh)          \
  V(sqrshl, Sqrshl)              \
  V(sqshl, Sqshl)                \
  V(sqsub, Sqsub)                \
  V(srhadd, Srhadd)              \
  V(srshl, Srshl)                \
  V(sshl, Sshl)                  \
  V(ssubl2, Ssubl2)              \
  V(ssubl, Ssubl)                \
  V(ssubw2, Ssubw2)              \
  V(ssubw, Ssubw)                \
  V(subhn2, Subhn2)              \
  V(subhn, Subhn)                \
  V(sub, Sub)                    \
  V(trn1, Trn1)                  \
  V(trn2, Trn2)                  \
  V(uabal2, Uabal2)              \
  V(uabal, Uabal)                \
  V(uaba, Uaba)                  \
  V(uabdl2, Uabdl2)              \
  V(uabdl, Uabdl)                \
  V(uabd, Uabd)                  \
  V(uaddl2, Uaddl2)              \
  V(uaddl, Uaddl)                \
  V(uaddw2, Uaddw2)              \
  V(uaddw, Uaddw)                \
  V(uhadd, Uhadd)                \
  V(uhsub, Uhsub)                \
  V(umaxp, Umaxp)                \
  V(umax, Umax)                  \
  V(uminp, Uminp)                \
  V(umin, Umin)                  \
  V(umlal2, Umlal2)              \
  V(umlal, Umlal)                \
  V(umlsl2, Umlsl2)              \
  V(umlsl, Umlsl)                \
  V(umull2, Umull2)              \
  V(umull, Umull)                \
  V(uqadd, Uqadd)                \
  V(uqrshl, Uqrshl)              \
  V(uqshl, Uqshl)                \
  V(uqsub, Uqsub)                \
  V(urhadd, Urhadd)              \
  V(urshl, Urshl)                \
  V(ushl, Ushl)                  \
  V(usubl2, Usubl2)              \
  V(usubl, Usubl)                \
  V(usubw2, Usubw2)              \
  V(usubw, Usubw)                \
  V(uzp1, Uzp1)                  \
  V(uzp2, Uzp2)                  \
  V(zip1, Zip1)                  \
  V(zip2, Zip2)

#define DEFINE_MACRO_ASM_FUNC(ASM, MASM)                                     \
  void MASM(const VRegister& vd, const VRegister& vn, const VRegister& vm) { \
    DCHECK(allow_macro_instructions());                                      \
    ASM(vd, vn, vm);                                                         \
  }
  NEON_3VREG_MACRO_LIST(DEFINE_MACRO_ASM_FUNC)
#undef DEFINE_MACRO_ASM_FUNC

  void Bic(const VRegister& vd, const int imm8, const int left_shift = 0) {
    DCHECK(allow_macro_instructions());
    bic(vd, imm8, left_shift);
  }

  // This is required for compatibility in architecture independent code.
  inline void jmp(Label* L);

  void B(Label* label, BranchType type, Register reg = NoReg, int bit = -1);
  inline void B(Label* label);
  inline void B(Condition cond, Label* label);
  void B(Label* label, Condition cond);

  void Tbnz(const Register& rt, unsigned bit_pos, Label* label);
  void Tbz(const Register& rt, unsigned bit_pos, Label* label);

  void Cbnz(const Register& rt, Label* label);
  void Cbz(const Register& rt, Label* label);

  bool AllowThisStubCall(CodeStub* stub);
  void CallStubDelayed(CodeStub* stub);
  void CallRuntimeDelayed(Zone* zone, Runtime::FunctionId fid,
                          SaveFPRegsMode save_doubles = kDontSaveFPRegs);

  // Removes current frame and its arguments from the stack preserving
  // the arguments and a return address pushed to the stack for the next call.
  // Both |callee_args_count| and |caller_args_count_reg| do not include
  // receiver. |callee_args_count| is not modified, |caller_args_count_reg|
  // is trashed.
  void PrepareForTailCall(const ParameterCount& callee_args_count,
                          Register caller_args_count_reg, Register scratch0,
                          Register scratch1);

  inline void SmiUntag(Register dst, Register src);
  inline void SmiUntag(Register smi);

  // Calls Abort(msg) if the condition cond is not satisfied.
  // Use --debug_code to enable.
  void Assert(Condition cond, BailoutReason reason);

  void AssertSmi(Register object, BailoutReason reason = kOperandIsNotASmi);

  // Like Assert(), but always enabled.
  void Check(Condition cond, BailoutReason reason);

  inline void Debug(const char* message, uint32_t code, Instr params = BREAK);

  // Print a message to stderr and abort execution.
  void Abort(BailoutReason reason);

  // If emit_debug_code() is true, emit a run-time check to ensure that
  // StackPointer() does not point below the system stack pointer.
  //
  // Whilst it is architecturally legal for StackPointer() to point below csp,
  // it can be evidence of a potential bug because the ABI forbids accesses
  // below csp.
  //
  // If StackPointer() is the system stack pointer (csp), then csp will be
  // dereferenced to cause the processor (or simulator) to abort if it is not
  // properly aligned.
  //
  // If emit_debug_code() is false, this emits no code.
  void AssertStackConsistency();

  // Remaining instructions are simple pass-through calls to the assembler.
  inline void Asr(const Register& rd, const Register& rn, unsigned shift);
  inline void Asr(const Register& rd, const Register& rn, const Register& rm);

  // Try to move an immediate into the destination register in a single
  // instruction. Returns true for success, and updates the contents of dst.
  // Returns false, otherwise.
  bool TryOneInstrMoveImmediate(const Register& dst, int64_t imm);

  inline void Bind(Label* label);

  static unsigned CountClearHalfWords(uint64_t imm, unsigned reg_size);

  CPURegList* TmpList() { return &tmp_list_; }
  CPURegList* FPTmpList() { return &fptmp_list_; }

  static CPURegList DefaultTmpList();
  static CPURegList DefaultFPTmpList();

  // Return the current stack pointer, as set by SetStackPointer.
  inline const Register& StackPointer() const { return sp_; }

  // Move macros.
  inline void Mvn(const Register& rd, uint64_t imm);
  void Mvn(const Register& rd, const Operand& operand);
  static bool IsImmMovn(uint64_t imm, unsigned reg_size);
  static bool IsImmMovz(uint64_t imm, unsigned reg_size);

  void LogicalMacro(const Register& rd, const Register& rn,
                    const Operand& operand, LogicalOp op);
  void AddSubMacro(const Register& rd, const Register& rn,
                   const Operand& operand, FlagsUpdate S, AddSubOp op);
  inline void Orr(const Register& rd, const Register& rn,
                  const Operand& operand);
  void Orr(const VRegister& vd, const int imm8, const int left_shift = 0) {
    DCHECK(allow_macro_instructions());
    orr(vd, imm8, left_shift);
  }
  inline void Orn(const Register& rd, const Register& rn,
                  const Operand& operand);
  inline void Eor(const Register& rd, const Register& rn,
                  const Operand& operand);
  inline void Eon(const Register& rd, const Register& rn,
                  const Operand& operand);
  inline void And(const Register& rd, const Register& rn,
                  const Operand& operand);
  inline void Ands(const Register& rd, const Register& rn,
                   const Operand& operand);
  inline void Tst(const Register& rn, const Operand& operand);
  inline void Bic(const Register& rd, const Register& rn,
                  const Operand& operand);
  inline void Blr(const Register& xn);
  inline void Cmp(const Register& rn, const Operand& operand);
  inline void Subs(const Register& rd, const Register& rn,
                   const Operand& operand);

  // Emits a runtime assert that the CSP is aligned.
  void AssertCspAligned();

  // Load a literal from the inline constant pool.
  inline void Ldr(const CPURegister& rt, const Operand& imm);
  // Helper function for double immediate.
  inline void Ldr(const CPURegister& rt, double imm);

  // Claim or drop stack space without actually accessing memory.
  //
  // In debug mode, both of these will write invalid data into the claimed or
  // dropped space.
  //
  // If the current stack pointer (according to StackPointer()) is csp, then it
  // must be aligned to 16 bytes and the size claimed or dropped must be a
  // multiple of 16 bytes.
  //
  // Note that unit_size must be specified in bytes. For variants which take a
  // Register count, the unit size must be a power of two.
  inline void Claim(int64_t count, uint64_t unit_size = kXRegSize);
  inline void Claim(const Register& count, uint64_t unit_size = kXRegSize);
  inline void Drop(int64_t count, uint64_t unit_size = kXRegSize);
  inline void Drop(const Register& count, uint64_t unit_size = kXRegSize);

  // Re-synchronizes the system stack pointer (csp) with the current stack
  // pointer (according to StackPointer()).
  //
  // This method asserts that StackPointer() is not csp, since the call does
  // not make sense in that context.
  inline void SyncSystemStackPointer();

  // Push the system stack pointer (csp) down to allow the same to be done to
  // the current stack pointer (according to StackPointer()). This must be
  // called _before_ accessing the memory.
  //
  // This is necessary when pushing or otherwise adding things to the stack, to
  // satisfy the AAPCS64 constraint that the memory below the system stack
  // pointer is not accessed.  The amount pushed will be increased as necessary
  // to ensure csp remains aligned to 16 bytes.
  //
  // This method asserts that StackPointer() is not csp, since the call does
  // not make sense in that context.
  inline void BumpSystemStackPointer(const Operand& space);

  // Add and sub macros.
  inline void Add(const Register& rd, const Register& rn,
                  const Operand& operand);
  inline void Adds(const Register& rd, const Register& rn,
                   const Operand& operand);
  inline void Sub(const Register& rd, const Register& rn,
                  const Operand& operand);

  // Abort execution if argument is not a positive or zero integer, enabled via
  // --debug-code.
  void AssertPositiveOrZero(Register value);

#define DECLARE_FUNCTION(FN, REGTYPE, REG, OP) \
  inline void FN(const REGTYPE REG, const MemOperand& addr);
  LS_MACRO_LIST(DECLARE_FUNCTION)
#undef DECLARE_FUNCTION

  // Push or pop up to 4 registers of the same width to or from the stack,
  // using the current stack pointer as set by SetStackPointer.
  //
  // If an argument register is 'NoReg', all further arguments are also assumed
  // to be 'NoReg', and are thus not pushed or popped.
  //
  // Arguments are ordered such that "Push(a, b);" is functionally equivalent
  // to "Push(a); Push(b);".
  //
  // It is valid to push the same register more than once, and there is no
  // restriction on the order in which registers are specified.
  //
  // It is not valid to pop into the same register more than once in one
  // operation, not even into the zero register.
  //
  // If the current stack pointer (as set by SetStackPointer) is csp, then it
  // must be aligned to 16 bytes on entry and the total size of the specified
  // registers must also be a multiple of 16 bytes.
  //
  // Even if the current stack pointer is not the system stack pointer (csp),
  // Push (and derived methods) will still modify the system stack pointer in
  // order to comply with ABI rules about accessing memory below the system
  // stack pointer.
  //
  // Other than the registers passed into Pop, the stack pointer and (possibly)
  // the system stack pointer, these methods do not modify any other registers.
  void Push(const CPURegister& src0, const CPURegister& src1 = NoReg,
            const CPURegister& src2 = NoReg, const CPURegister& src3 = NoReg);
  void Push(const CPURegister& src0, const CPURegister& src1,
            const CPURegister& src2, const CPURegister& src3,
            const CPURegister& src4, const CPURegister& src5 = NoReg,
            const CPURegister& src6 = NoReg, const CPURegister& src7 = NoReg);
  void Pop(const CPURegister& dst0, const CPURegister& dst1 = NoReg,
           const CPURegister& dst2 = NoReg, const CPURegister& dst3 = NoReg);
  void Pop(const CPURegister& dst0, const CPURegister& dst1,
           const CPURegister& dst2, const CPURegister& dst3,
           const CPURegister& dst4, const CPURegister& dst5 = NoReg,
           const CPURegister& dst6 = NoReg, const CPURegister& dst7 = NoReg);
  void Push(const Register& src0, const VRegister& src1);

  // This is a convenience method for pushing a single Handle<Object>.
  inline void Push(Handle<HeapObject> object);
  inline void Push(Smi* smi);

  // Aliases of Push and Pop, required for V8 compatibility.
  inline void push(Register src) { Push(src); }
  inline void pop(Register dst) { Pop(dst); }

  // Alternative forms of Push and Pop, taking a RegList or CPURegList that
  // specifies the registers that are to be pushed or popped. Higher-numbered
  // registers are associated with higher memory addresses (as in the A32 push
  // and pop instructions).
  //
  // (Push|Pop)SizeRegList allow you to specify the register size as a
  // parameter. Only kXRegSizeInBits, kWRegSizeInBits, kDRegSizeInBits and
  // kSRegSizeInBits are supported.
  //
  // Otherwise, (Push|Pop)(CPU|X|W|D|S)RegList is preferred.
  void PushCPURegList(CPURegList registers);
  void PopCPURegList(CPURegList registers);

  void PushCallerSaved(SaveFPRegsMode fp_mode, Register exclusion1 = no_reg,
                       Register exclusion2 = no_reg,
                       Register exclusion3 = no_reg);
  void PopCallerSaved(SaveFPRegsMode fp_mode, Register exclusion1 = no_reg,
                      Register exclusion2 = no_reg,
                      Register exclusion3 = no_reg);

  // Move an immediate into register dst, and return an Operand object for use
  // with a subsequent instruction that accepts a shift. The value moved into
  // dst is not necessarily equal to imm; it may have had a shifting operation
  // applied to it that will be subsequently undone by the shift applied in the
  // Operand.
  Operand MoveImmediateForShiftedOp(const Register& dst, int64_t imm,
                                    PreShiftImmMode mode);

  void CheckPageFlagSet(const Register& object, const Register& scratch,
                        int mask, Label* if_any_set);

  void CheckPageFlagClear(const Register& object, const Register& scratch,
                          int mask, Label* if_all_clear);

  // Perform necessary maintenance operations before a push or after a pop.
  //
  // Note that size is specified in bytes.
  void PushPreamble(Operand total_size);
  void PopPostamble(Operand total_size);

  void PushPreamble(int count, int size);
  void PopPostamble(int count, int size);

  // Test the bits of register defined by bit_pattern, and branch if ANY of
  // those bits are set. May corrupt the status flags.
  inline void TestAndBranchIfAnySet(const Register& reg,
                                    const uint64_t bit_pattern, Label* label);

  // Test the bits of register defined by bit_pattern, and branch if ALL of
  // those bits are clear (ie. not set.) May corrupt the status flags.
  inline void TestAndBranchIfAllClear(const Register& reg,
                                      const uint64_t bit_pattern, Label* label);

  inline void Brk(int code);

  inline void JumpIfSmi(Register value, Label* smi_label,
                        Label* not_smi_label = NULL);

  inline void Fmov(VRegister fd, VRegister fn);
  inline void Fmov(VRegister fd, Register rn);
  // Provide explicit double and float interfaces for FP immediate moves, rather
  // than relying on implicit C++ casts. This allows signalling NaNs to be
  // preserved when the immediate matches the format of fd. Most systems convert
  // signalling NaNs to quiet NaNs when converting between float and double.
  inline void Fmov(VRegister fd, double imm);
  inline void Fmov(VRegister fd, float imm);
  // Provide a template to allow other types to be converted automatically.
  template <typename T>
  void Fmov(VRegister fd, T imm) {
    DCHECK(allow_macro_instructions());
    Fmov(fd, static_cast<double>(imm));
  }
  inline void Fmov(Register rd, VRegister fn);

  void Movi(const VRegister& vd, uint64_t imm, Shift shift = LSL,
            int shift_amount = 0);
  void Movi(const VRegister& vd, uint64_t hi, uint64_t lo);

  void Jump(Register target);
  void Jump(Address target, RelocInfo::Mode rmode, Condition cond = al);
  void Jump(Handle<Code> code, RelocInfo::Mode rmode, Condition cond = al);
  void Jump(intptr_t target, RelocInfo::Mode rmode, Condition cond = al);

  void Call(Register target);
  void Call(Label* target);
  void Call(Address target, RelocInfo::Mode rmode);
  void Call(Handle<Code> code, RelocInfo::Mode rmode = RelocInfo::CODE_TARGET);

  void CallForDeoptimization(Address target, RelocInfo::Mode rmode) {
    Call(target, rmode);
  }

  // For every Call variant, there is a matching CallSize function that returns
  // the size (in bytes) of the call sequence.
  static int CallSize(Register target);
  static int CallSize(Label* target);
  static int CallSize(Address target, RelocInfo::Mode rmode);
  static int CallSize(Handle<Code> code,
                      RelocInfo::Mode rmode = RelocInfo::CODE_TARGET);

  // Calls a C function.
  // The called function is not allowed to trigger a
  // garbage collection, since that might move the code and invalidate the
  // return address (unless this is somehow accounted for by the called
  // function).
  void CallCFunction(ExternalReference function, int num_reg_arguments);
  void CallCFunction(ExternalReference function, int num_reg_arguments,
                     int num_double_arguments);
  void CallCFunction(Register function, int num_reg_arguments,
                     int num_double_arguments);

  // Performs a truncating conversion of a floating point number as used by
  // the JS bitwise operations. See ECMA-262 9.5: ToInt32.
  // Exits with 'result' holding the answer.
  void TruncateDoubleToIDelayed(Zone* zone, Register result,
                                DoubleRegister double_input);

  inline void Mul(const Register& rd, const Register& rn, const Register& rm);

  inline void Fcvtzs(const Register& rd, const VRegister& fn);
  void Fcvtzs(const VRegister& vd, const VRegister& vn, int fbits = 0) {
    DCHECK(allow_macro_instructions());
    fcvtzs(vd, vn, fbits);
  }

  inline void Fcvtzu(const Register& rd, const VRegister& fn);
  void Fcvtzu(const VRegister& vd, const VRegister& vn, int fbits = 0) {
    DCHECK(allow_macro_instructions());
    fcvtzu(vd, vn, fbits);
  }

  inline void Madd(const Register& rd, const Register& rn, const Register& rm,
                   const Register& ra);
  inline void Mneg(const Register& rd, const Register& rn, const Register& rm);
  inline void Sdiv(const Register& rd, const Register& rn, const Register& rm);
  inline void Udiv(const Register& rd, const Register& rn, const Register& rm);
  inline void Msub(const Register& rd, const Register& rn, const Register& rm,
                   const Register& ra);

  inline void Lsl(const Register& rd, const Register& rn, unsigned shift);
  inline void Lsl(const Register& rd, const Register& rn, const Register& rm);
  inline void Umull(const Register& rd, const Register& rn, const Register& rm);
  inline void Smull(const Register& rd, const Register& rn, const Register& rm);

  inline void Sxtb(const Register& rd, const Register& rn);
  inline void Sxth(const Register& rd, const Register& rn);
  inline void Sxtw(const Register& rd, const Register& rn);
  inline void Ubfiz(const Register& rd, const Register& rn, unsigned lsb,
                    unsigned width);
  inline void Ubfx(const Register& rd, const Register& rn, unsigned lsb,
                   unsigned width);
  inline void Lsr(const Register& rd, const Register& rn, unsigned shift);
  inline void Lsr(const Register& rd, const Register& rn, const Register& rm);
  inline void Ror(const Register& rd, const Register& rs, unsigned shift);
  inline void Ror(const Register& rd, const Register& rn, const Register& rm);
  inline void Cmn(const Register& rn, const Operand& operand);
  inline void Fadd(const VRegister& fd, const VRegister& fn,
                   const VRegister& fm);
  inline void Fcmp(const VRegister& fn, const VRegister& fm);
  inline void Fcmp(const VRegister& fn, double value);
  inline void Fabs(const VRegister& fd, const VRegister& fn);
  inline void Fmul(const VRegister& fd, const VRegister& fn,
                   const VRegister& fm);
  inline void Fsub(const VRegister& fd, const VRegister& fn,
                   const VRegister& fm);
  inline void Fdiv(const VRegister& fd, const VRegister& fn,
                   const VRegister& fm);
  inline void Fmax(const VRegister& fd, const VRegister& fn,
                   const VRegister& fm);
  inline void Fmin(const VRegister& fd, const VRegister& fn,
                   const VRegister& fm);
  inline void Rbit(const Register& rd, const Register& rn);

  enum AdrHint {
    // The target must be within the immediate range of adr.
    kAdrNear,
    // The target may be outside of the immediate range of adr. Additional
    // instructions may be emitted.
    kAdrFar
  };
  void Adr(const Register& rd, Label* label, AdrHint = kAdrNear);

  // Add/sub with carry macros.
  inline void Adc(const Register& rd, const Register& rn,
                  const Operand& operand);

  // Conditional macros.
  inline void Ccmp(const Register& rn, const Operand& operand, StatusFlags nzcv,
                   Condition cond);

  inline void Clz(const Register& rd, const Register& rn);

  // Poke 'src' onto the stack. The offset is in bytes.
  //
  // If the current stack pointer (according to StackPointer()) is csp, then
  // csp must be aligned to 16 bytes.
  void Poke(const CPURegister& src, const Operand& offset);

  // Poke 'src1' and 'src2' onto the stack. The values written will be adjacent
  // with 'src2' at a higher address than 'src1'. The offset is in bytes.
  //
  // If the current stack pointer (according to StackPointer()) is csp, then
  // csp must be aligned to 16 bytes.
  void PokePair(const CPURegister& src1, const CPURegister& src2, int offset);

  inline void Sbfx(const Register& rd, const Register& rn, unsigned lsb,
                   unsigned width);

  inline void Bfi(const Register& rd, const Register& rn, unsigned lsb,
                  unsigned width);

  inline void Scvtf(const VRegister& fd, const Register& rn,
                    unsigned fbits = 0);
  void Scvtf(const VRegister& vd, const VRegister& vn, int fbits = 0) {
    DCHECK(allow_macro_instructions());
    scvtf(vd, vn, fbits);
  }
  inline void Ucvtf(const VRegister& fd, const Register& rn,
                    unsigned fbits = 0);
  void Ucvtf(const VRegister& vd, const VRegister& vn, int fbits = 0) {
    DCHECK(allow_macro_instructions());
    ucvtf(vd, vn, fbits);
  }

  void AssertFPCRState(Register fpcr = NoReg);
  void CanonicalizeNaN(const VRegister& dst, const VRegister& src);
  void CanonicalizeNaN(const VRegister& reg) { CanonicalizeNaN(reg, reg); }

  inline void Cset(const Register& rd, Condition cond);
  inline void Fccmp(const VRegister& fn, const VRegister& fm, StatusFlags nzcv,
                    Condition cond);
  inline void Csinc(const Register& rd, const Register& rn, const Register& rm,
                    Condition cond);

  inline void Fcvt(const VRegister& fd, const VRegister& fn);

  int ActivationFrameAlignment();

  void Ins(const VRegister& vd, int vd_index, const VRegister& vn,
           int vn_index) {
    DCHECK(allow_macro_instructions());
    ins(vd, vd_index, vn, vn_index);
  }
  void Ins(const VRegister& vd, int vd_index, const Register& rn) {
    DCHECK(allow_macro_instructions());
    ins(vd, vd_index, rn);
  }

  inline void Bl(Label* label);
  inline void Br(const Register& xn);

  inline void Uxtb(const Register& rd, const Register& rn);
  inline void Uxth(const Register& rd, const Register& rn);
  inline void Uxtw(const Register& rd, const Register& rn);

  void Dup(const VRegister& vd, const VRegister& vn, int index) {
    DCHECK(allow_macro_instructions());
    dup(vd, vn, index);
  }
  void Dup(const VRegister& vd, const Register& rn) {
    DCHECK(allow_macro_instructions());
    dup(vd, rn);
  }

#define DECLARE_FUNCTION(FN, REGTYPE, REG, REG2, OP) \
  inline void FN(const REGTYPE REG, const REGTYPE REG2, const MemOperand& addr);
  LSPAIR_MACRO_LIST(DECLARE_FUNCTION)
#undef DECLARE_FUNCTION

#define NEON_2VREG_SHIFT_MACRO_LIST(V) \
  V(rshrn, Rshrn)                      \
  V(rshrn2, Rshrn2)                    \
  V(shl, Shl)                          \
  V(shll, Shll)                        \
  V(shll2, Shll2)                      \
  V(shrn, Shrn)                        \
  V(shrn2, Shrn2)                      \
  V(sli, Sli)                          \
  V(sqrshrn, Sqrshrn)                  \
  V(sqrshrn2, Sqrshrn2)                \
  V(sqrshrun, Sqrshrun)                \
  V(sqrshrun2, Sqrshrun2)              \
  V(sqshl, Sqshl)                      \
  V(sqshlu, Sqshlu)                    \
  V(sqshrn, Sqshrn)                    \
  V(sqshrn2, Sqshrn2)                  \
  V(sqshrun, Sqshrun)                  \
  V(sqshrun2, Sqshrun2)                \
  V(sri, Sri)                          \
  V(srshr, Srshr)                      \
  V(srsra, Srsra)                      \
  V(sshll, Sshll)                      \
  V(sshll2, Sshll2)                    \
  V(sshr, Sshr)                        \
  V(ssra, Ssra)                        \
  V(uqrshrn, Uqrshrn)                  \
  V(uqrshrn2, Uqrshrn2)                \
  V(uqshl, Uqshl)                      \
  V(uqshrn, Uqshrn)                    \
  V(uqshrn2, Uqshrn2)                  \
  V(urshr, Urshr)                      \
  V(ursra, Ursra)                      \
  V(ushll, Ushll)                      \
  V(ushll2, Ushll2)                    \
  V(ushr, Ushr)                        \
  V(usra, Usra)

#define DEFINE_MACRO_ASM_FUNC(ASM, MASM)                           \
  void MASM(const VRegister& vd, const VRegister& vn, int shift) { \
    DCHECK(allow_macro_instructions());                            \
    ASM(vd, vn, shift);                                            \
  }
  NEON_2VREG_SHIFT_MACRO_LIST(DEFINE_MACRO_ASM_FUNC)
#undef DEFINE_MACRO_ASM_FUNC

  void Umov(const Register& rd, const VRegister& vn, int vn_index) {
    DCHECK(allow_macro_instructions());
    umov(rd, vn, vn_index);
  }
  void Tbl(const VRegister& vd, const VRegister& vn, const VRegister& vm) {
    DCHECK(allow_macro_instructions());
    tbl(vd, vn, vm);
  }
  void Tbl(const VRegister& vd, const VRegister& vn, const VRegister& vn2,
           const VRegister& vm) {
    DCHECK(allow_macro_instructions());
    tbl(vd, vn, vn2, vm);
  }
  void Tbl(const VRegister& vd, const VRegister& vn, const VRegister& vn2,
           const VRegister& vn3, const VRegister& vm) {
    DCHECK(allow_macro_instructions());
    tbl(vd, vn, vn2, vn3, vm);
  }
  void Tbl(const VRegister& vd, const VRegister& vn, const VRegister& vn2,
           const VRegister& vn3, const VRegister& vn4, const VRegister& vm) {
    DCHECK(allow_macro_instructions());
    tbl(vd, vn, vn2, vn3, vn4, vm);
  }
  void Ext(const VRegister& vd, const VRegister& vn, const VRegister& vm,
           int index) {
    DCHECK(allow_macro_instructions());
    ext(vd, vn, vm, index);
  }

  void Smov(const Register& rd, const VRegister& vn, int vn_index) {
    DCHECK(allow_macro_instructions());
    smov(rd, vn, vn_index);
  }

// Load-acquire/store-release macros.
#define DECLARE_FUNCTION(FN, OP) \
  inline void FN(const Register& rt, const Register& rn);
  LDA_STL_MACRO_LIST(DECLARE_FUNCTION)
#undef DECLARE_FUNCTION

  // Load an object from the root table.
  void LoadRoot(CPURegister destination, Heap::RootListIndex index);

  inline void Ret(const Register& xn = lr);

  // Perform a conversion from a double to a signed int64. If the input fits in
  // range of the 64-bit result, execution branches to done. Otherwise,
  // execution falls through, and the sign of the result can be used to
  // determine if overflow was towards positive or negative infinity.
  //
  // On successful conversion, the least significant 32 bits of the result are
  // equivalent to the ECMA-262 operation "ToInt32".
  //
  // Only public for the test code in test-code-stubs-arm64.cc.
  void TryConvertDoubleToInt64(Register result, DoubleRegister input,
                               Label* done);

  inline void Mrs(const Register& rt, SystemRegister sysreg);

  // Generates function prologue code.
  void Prologue();

  void Cmgt(const VRegister& vd, const VRegister& vn, int imm) {
    DCHECK(allow_macro_instructions());
    cmgt(vd, vn, imm);
  }
  void Cmge(const VRegister& vd, const VRegister& vn, int imm) {
    DCHECK(allow_macro_instructions());
    cmge(vd, vn, imm);
  }
  void Cmeq(const VRegister& vd, const VRegister& vn, int imm) {
    DCHECK(allow_macro_instructions());
    cmeq(vd, vn, imm);
  }

  inline void Neg(const Register& rd, const Operand& operand);
  inline void Negs(const Register& rd, const Operand& operand);

  // Compute rd = abs(rm).
  // This function clobbers the condition flags. On output the overflow flag is
  // set iff the negation overflowed.
  //
  // If rm is the minimum representable value, the result is not representable.
  // Handlers for each case can be specified using the relevant labels.
  void Abs(const Register& rd, const Register& rm,
           Label* is_not_representable = NULL, Label* is_representable = NULL);

  inline void Cls(const Register& rd, const Register& rn);
  inline void Cneg(const Register& rd, const Register& rn, Condition cond);
  inline void Rev16(const Register& rd, const Register& rn);
  inline void Rev32(const Register& rd, const Register& rn);
  inline void Fcvtns(const Register& rd, const VRegister& fn);
  inline void Fcvtnu(const Register& rd, const VRegister& fn);
  inline void Fcvtms(const Register& rd, const VRegister& fn);
  inline void Fcvtmu(const Register& rd, const VRegister& fn);
  inline void Fcvtas(const Register& rd, const VRegister& fn);
  inline void Fcvtau(const Register& rd, const VRegister& fn);

 protected:
  // The actual Push and Pop implementations. These don't generate any code
  // other than that required for the push or pop. This allows
  // (Push|Pop)CPURegList to bundle together run-time assertions for a large
  // block of registers.
  //
  // Note that size is per register, and is specified in bytes.
  void PushHelper(int count, int size, const CPURegister& src0,
                  const CPURegister& src1, const CPURegister& src2,
                  const CPURegister& src3);
  void PopHelper(int count, int size, const CPURegister& dst0,
                 const CPURegister& dst1, const CPURegister& dst2,
                 const CPURegister& dst3);

  void ConditionalCompareMacro(const Register& rn, const Operand& operand,
                               StatusFlags nzcv, Condition cond,
                               ConditionalCompareOp op);

  void AddSubWithCarryMacro(const Register& rd, const Register& rn,
                            const Operand& operand, FlagsUpdate S,
                            AddSubWithCarryOp op);

  // Call Printf. On a native build, a simple call will be generated, but if the
  // simulator is being used then a suitable pseudo-instruction is used. The
  // arguments and stack (csp) must be prepared by the caller as for a normal
  // AAPCS64 call to 'printf'.
  //
  // The 'args' argument should point to an array of variable arguments in their
  // proper PCS registers (and in calling order). The argument registers can
  // have mixed types. The format string (x0) should not be included.
  void CallPrintf(int arg_count = 0, const CPURegister* args = NULL);

 private:
  bool has_frame_ = false;
  Isolate* const isolate_;
#if DEBUG
  // Tell whether any of the macro instruction can be used. When false the
  // MacroAssembler will assert if a method which can emit a variable number
  // of instructions is called.
  bool allow_macro_instructions_;
#endif
  // This handle will be patched with the code object on installation.
  Handle<HeapObject> code_object_;

  // Scratch registers available for use by the MacroAssembler.
  CPURegList tmp_list_;
  CPURegList fptmp_list_;

  // The register to use as a stack pointer for stack operations.
  Register sp_;

  bool use_real_aborts_;

  // Helps resolve branching to labels potentially out of range.
  // If the label is not bound, it registers the information necessary to later
  // be able to emit a veneer for this branch if necessary.
  // If the label is bound, it returns true if the label (or the previous link
  // in the label chain) is out of range. In that case the caller is responsible
  // for generating appropriate code.
  // Otherwise it returns false.
  // This function also checks wether veneers need to be emitted.
  bool NeedExtraInstructionsOrRegisterBranch(Label* label,
                                             ImmBranchType branch_type);

  void Movi16bitHelper(const VRegister& vd, uint64_t imm);
  void Movi32bitHelper(const VRegister& vd, uint64_t imm);
  void Movi64bitHelper(const VRegister& vd, uint64_t imm);

  void LoadStoreMacro(const CPURegister& rt, const MemOperand& addr,
                      LoadStoreOp op);

  void LoadStorePairMacro(const CPURegister& rt, const CPURegister& rt2,
                          const MemOperand& addr, LoadStorePairOp op);
};

class MacroAssembler : public TurboAssembler {
 public:
  MacroAssembler(Isolate* isolate, byte* buffer, unsigned buffer_size,
                 CodeObjectRequired create_code_object);

  // Instruction set functions ------------------------------------------------
  // Logical macros.
  inline void Bics(const Register& rd, const Register& rn,
                   const Operand& operand);

  inline void Adcs(const Register& rd, const Register& rn,
                   const Operand& operand);
  inline void Sbc(const Register& rd, const Register& rn,
                  const Operand& operand);
  inline void Sbcs(const Register& rd, const Register& rn,
                   const Operand& operand);
  inline void Ngc(const Register& rd, const Operand& operand);
  inline void Ngcs(const Register& rd, const Operand& operand);

  inline void Ccmn(const Register& rn, const Operand& operand, StatusFlags nzcv,
                   Condition cond);
  void Csel(const Register& rd, const Register& rn, const Operand& operand,
            Condition cond);

#define DECLARE_FUNCTION(FN, OP) \
  inline void FN(const Register& rs, const Register& rt, const Register& rn);
  STLX_MACRO_LIST(DECLARE_FUNCTION)
#undef DECLARE_FUNCTION

  // Branch type inversion relies on these relations.
  STATIC_ASSERT((reg_zero == (reg_not_zero ^ 1)) &&
                (reg_bit_clear == (reg_bit_set ^ 1)) &&
                (always == (never ^ 1)));

  inline void Bfxil(const Register& rd, const Register& rn, unsigned lsb,
                    unsigned width);
  inline void Cinc(const Register& rd, const Register& rn, Condition cond);
  inline void Cinv(const Register& rd, const Register& rn, Condition cond);
  inline void CzeroX(const Register& rd, Condition cond);
  inline void CmovX(const Register& rd, const Register& rn, Condition cond);
  inline void Csetm(const Register& rd, Condition cond);
  inline void Csinv(const Register& rd, const Register& rn, const Register& rm,
                    Condition cond);
  inline void Csneg(const Register& rd, const Register& rn, const Register& rm,
                    Condition cond);
  inline void Dmb(BarrierDomain domain, BarrierType type);
  inline void Dsb(BarrierDomain domain, BarrierType type);
  inline void Extr(const Register& rd, const Register& rn, const Register& rm,
                   unsigned lsb);
  inline void Fcsel(const VRegister& fd, const VRegister& fn,
                    const VRegister& fm, Condition cond);
  void Fcvtl(const VRegister& vd, const VRegister& vn) {
    DCHECK(allow_macro_instructions());
    fcvtl(vd, vn);
  }
  void Fcvtl2(const VRegister& vd, const VRegister& vn) {
    DCHECK(allow_macro_instructions());
    fcvtl2(vd, vn);
  }
  void Fcvtn(const VRegister& vd, const VRegister& vn) {
    DCHECK(allow_macro_instructions());
    fcvtn(vd, vn);
  }
  void Fcvtn2(const VRegister& vd, const VRegister& vn) {
    DCHECK(allow_macro_instructions());
    fcvtn2(vd, vn);
  }
  void Fcvtxn(const VRegister& vd, const VRegister& vn) {
    DCHECK(allow_macro_instructions());
    fcvtxn(vd, vn);
  }
  void Fcvtxn2(const VRegister& vd, const VRegister& vn) {
    DCHECK(allow_macro_instructions());
    fcvtxn2(vd, vn);
  }
  inline void Fmadd(const VRegister& fd, const VRegister& fn,
                    const VRegister& fm, const VRegister& fa);
  inline void Fmaxnm(const VRegister& fd, const VRegister& fn,
                     const VRegister& fm);
  inline void Fminnm(const VRegister& fd, const VRegister& fn,
                     const VRegister& fm);
  inline void Fmsub(const VRegister& fd, const VRegister& fn,
                    const VRegister& fm, const VRegister& fa);
  inline void Fnmadd(const VRegister& fd, const VRegister& fn,
                     const VRegister& fm, const VRegister& fa);
  inline void Fnmsub(const VRegister& fd, const VRegister& fn,
                     const VRegister& fm, const VRegister& fa);
  inline void Hint(SystemHint code);
  inline void Hlt(int code);
  inline void Isb();
  inline void Ldnp(const CPURegister& rt, const CPURegister& rt2,
                   const MemOperand& src);
  inline void Movk(const Register& rd, uint64_t imm, int shift = -1);
  inline void Msr(SystemRegister sysreg, const Register& rt);
  inline void Nop() { nop(); }
  void Mvni(const VRegister& vd, const int imm8, Shift shift = LSL,
            const int shift_amount = 0) {
    DCHECK(allow_macro_instructions());
    mvni(vd, imm8, shift, shift_amount);
  }
  inline void Rev(const Register& rd, const Register& rn);
  inline void Sbfiz(const Register& rd, const Register& rn, unsigned lsb,
                    unsigned width);
  inline void Smaddl(const Register& rd, const Register& rn, const Register& rm,
                     const Register& ra);
  inline void Smsubl(const Register& rd, const Register& rn, const Register& rm,
                     const Register& ra);
  inline void Smulh(const Register& rd, const Register& rn, const Register& rm);
  inline void Stnp(const CPURegister& rt, const CPURegister& rt2,
                   const MemOperand& dst);
  inline void Umaddl(const Register& rd, const Register& rn, const Register& rm,
                     const Register& ra);
  inline void Umsubl(const Register& rd, const Register& rn, const Register& rm,
                     const Register& ra);

  void Cmle(const VRegister& vd, const VRegister& vn, int imm) {
    DCHECK(allow_macro_instructions());
    cmle(vd, vn, imm);
  }
  void Cmlt(const VRegister& vd, const VRegister& vn, int imm) {
    DCHECK(allow_macro_instructions());
    cmlt(vd, vn, imm);
  }

  void Ld1(const VRegister& vt, const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld1(vt, src);
  }
  void Ld1(const VRegister& vt, const VRegister& vt2, const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld1(vt, vt2, src);
  }
  void Ld1(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld1(vt, vt2, vt3, src);
  }
  void Ld1(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           const VRegister& vt4, const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld1(vt, vt2, vt3, vt4, src);
  }
  void Ld1(const VRegister& vt, int lane, const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld1(vt, lane, src);
  }
  void Ld1r(const VRegister& vt, const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld1r(vt, src);
  }
  void Ld2(const VRegister& vt, const VRegister& vt2, const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld2(vt, vt2, src);
  }
  void Ld2(const VRegister& vt, const VRegister& vt2, int lane,
           const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld2(vt, vt2, lane, src);
  }
  void Ld2r(const VRegister& vt, const VRegister& vt2, const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld2r(vt, vt2, src);
  }
  void Ld3(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld3(vt, vt2, vt3, src);
  }
  void Ld3(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           int lane, const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld3(vt, vt2, vt3, lane, src);
  }
  void Ld3r(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
            const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld3r(vt, vt2, vt3, src);
  }
  void Ld4(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           const VRegister& vt4, const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld4(vt, vt2, vt3, vt4, src);
  }
  void Ld4(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           const VRegister& vt4, int lane, const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld4(vt, vt2, vt3, vt4, lane, src);
  }
  void Ld4r(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
            const VRegister& vt4, const MemOperand& src) {
    DCHECK(allow_macro_instructions());
    ld4r(vt, vt2, vt3, vt4, src);
  }
  void St1(const VRegister& vt, const MemOperand& dst) {
    DCHECK(allow_macro_instructions());
    st1(vt, dst);
  }
  void St1(const VRegister& vt, const VRegister& vt2, const MemOperand& dst) {
    DCHECK(allow_macro_instructions());
    st1(vt, vt2, dst);
  }
  void St1(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           const MemOperand& dst) {
    DCHECK(allow_macro_instructions());
    st1(vt, vt2, vt3, dst);
  }
  void St1(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           const VRegister& vt4, const MemOperand& dst) {
    DCHECK(allow_macro_instructions());
    st1(vt, vt2, vt3, vt4, dst);
  }
  void St1(const VRegister& vt, int lane, const MemOperand& dst) {
    DCHECK(allow_macro_instructions());
    st1(vt, lane, dst);
  }
  void St2(const VRegister& vt, const VRegister& vt2, const MemOperand& dst) {
    DCHECK(allow_macro_instructions());
    st2(vt, vt2, dst);
  }
  void St3(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           const MemOperand& dst) {
    DCHECK(allow_macro_instructions());
    st3(vt, vt2, vt3, dst);
  }
  void St4(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           const VRegister& vt4, const MemOperand& dst) {
    DCHECK(allow_macro_instructions());
    st4(vt, vt2, vt3, vt4, dst);
  }
  void St2(const VRegister& vt, const VRegister& vt2, int lane,
           const MemOperand& dst) {
    DCHECK(allow_macro_instructions());
    st2(vt, vt2, lane, dst);
  }
  void St3(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           int lane, const MemOperand& dst) {
    DCHECK(allow_macro_instructions());
    st3(vt, vt2, vt3, lane, dst);
  }
  void St4(const VRegister& vt, const VRegister& vt2, const VRegister& vt3,
           const VRegister& vt4, int lane, const MemOperand& dst) {
    DCHECK(allow_macro_instructions());
    st4(vt, vt2, vt3, vt4, lane, dst);
  }
  void Tbx(const VRegister& vd, const VRegister& vn, const VRegister& vm) {
    DCHECK(allow_macro_instructions());
    tbx(vd, vn, vm);
  }
  void Tbx(const VRegister& vd, const VRegister& vn, const VRegister& vn2,
           const VRegister& vm) {
    DCHECK(allow_macro_instructions());
    tbx(vd, vn, vn2, vm);
  }
  void Tbx(const VRegister& vd, const VRegister& vn, const VRegister& vn2,
           const VRegister& vn3, const VRegister& vm) {
    DCHECK(allow_macro_instructions());
    tbx(vd, vn, vn2, vn3, vm);
  }
  void Tbx(const VRegister& vd, const VRegister& vn, const VRegister& vn2,
           const VRegister& vn3, const VRegister& vn4, const VRegister& vm) {
    DCHECK(allow_macro_instructions());
    tbx(vd, vn, vn2, vn3, vn4, vm);
  }

  void LoadObject(Register result, Handle<Object> object);

  inline void PushSizeRegList(RegList registers, unsigned reg_size,
      CPURegister::RegisterType type = CPURegister::kRegister) {
    PushCPURegList(CPURegList(type, reg_size, registers));
  }
  inline void PopSizeRegList(RegList registers, unsigned reg_size,
      CPURegister::RegisterType type = CPURegister::kRegister) {
    PopCPURegList(CPURegList(type, reg_size, registers));
  }
  inline void PushXRegList(RegList regs) {
    PushSizeRegList(regs, kXRegSizeInBits);
  }
  inline void PopXRegList(RegList regs) {
    PopSizeRegList(regs, kXRegSizeInBits);
  }
  inline void PushWRegList(RegList regs) {
    PushSizeRegList(regs, kWRegSizeInBits);
  }
  inline void PopWRegList(RegList regs) {
    PopSizeRegList(regs, kWRegSizeInBits);
  }
  inline void PushDRegList(RegList regs) {
    PushSizeRegList(regs, kDRegSizeInBits, CPURegister::kVRegister);
  }
  inline void PopDRegList(RegList regs) {
    PopSizeRegList(regs, kDRegSizeInBits, CPURegister::kVRegister);
  }
  inline void PushSRegList(RegList regs) {
    PushSizeRegList(regs, kSRegSizeInBits, CPURegister::kVRegister);
  }
  inline void PopSRegList(RegList regs) {
    PopSizeRegList(regs, kSRegSizeInBits, CPURegister::kVRegister);
  }

  // Push the specified register 'count' times.
  void PushMultipleTimes(CPURegister src, Register count);
  void PushMultipleTimes(CPURegister src, int count);

  // Sometimes callers need to push or pop multiple registers in a way that is
  // difficult to structure efficiently for fixed Push or Pop calls. This scope
  // allows push requests to be queued up, then flushed at once. The
  // MacroAssembler will try to generate the most efficient sequence required.
  //
  // Unlike the other Push and Pop macros, PushPopQueue can handle mixed sets of
  // register sizes and types.
  class PushPopQueue {
   public:
    explicit PushPopQueue(MacroAssembler* masm) : masm_(masm), size_(0) { }

    ~PushPopQueue() {
      DCHECK(queued_.empty());
    }

    void Queue(const CPURegister& rt) {
      size_ += rt.SizeInBytes();
      queued_.push_back(rt);
    }

    enum PreambleDirective {
      WITH_PREAMBLE,
      SKIP_PREAMBLE
    };
    void PushQueued(PreambleDirective preamble_directive = WITH_PREAMBLE);
    void PopQueued();

   private:
    MacroAssembler* masm_;
    int size_;
    std::vector<CPURegister> queued_;
  };

  // Peek at a value on the stack, and put it in 'dst'. The offset is in bytes.
  //
  // If the current stack pointer (according to StackPointer()) is csp, then
  // csp must be aligned to 16 bytes.
  void Peek(const CPURegister& dst, const Operand& offset);

  // Peek at two values on the stack, and put them in 'dst1' and 'dst2'. The
  // values peeked will be adjacent, with the value in 'dst2' being from a
  // higher address than 'dst1'. The offset is in bytes.
  //
  // If the current stack pointer (according to StackPointer()) is csp, then
  // csp must be aligned to 16 bytes.
  void PeekPair(const CPURegister& dst1, const CPURegister& dst2, int offset);

  // Emit code that loads |parameter_index|'th parameter from the stack to
  // the register according to the CallInterfaceDescriptor definition.
  // |sp_to_caller_sp_offset_in_words| specifies the number of words pushed
  // below the caller's sp.
  template <class Descriptor>
  void LoadParameterFromStack(
      Register reg, typename Descriptor::ParameterIndices parameter_index,
      int sp_to_ra_offset_in_words = 0) {
    DCHECK(Descriptor::kPassLastArgsOnStack);
    UNIMPLEMENTED();
  }

  // Variants of Claim and Drop, where the 'count' parameter is a SMI held in a
  // register.
  inline void ClaimBySMI(const Register& count_smi,
                         uint64_t unit_size = kXRegSize);
  inline void DropBySMI(const Register& count_smi,
                        uint64_t unit_size = kXRegSize);

  // Compare a register with an operand, and branch to label depending on the
  // condition. May corrupt the status flags.
  inline void CompareAndBranch(const Register& lhs,
                               const Operand& rhs,
                               Condition cond,
                               Label* label);

  // Insert one or more instructions into the instruction stream that encode
  // some caller-defined data. The instructions used will be executable with no
  // side effects.
  inline void InlineData(uint64_t data);

  // Insert an instrumentation enable marker into the instruction stream.
  inline void EnableInstrumentation();

  // Insert an instrumentation disable marker into the instruction stream.
  inline void DisableInstrumentation();

  // Insert an instrumentation event marker into the instruction stream. These
  // will be picked up by the instrumentation system to annotate an instruction
  // profile. The argument marker_name must be a printable two character string;
  // it will be encoded in the event marker.
  inline void AnnotateInstrumentation(const char* marker_name);

  // Preserve the callee-saved registers (as defined by AAPCS64).
  //
  // Higher-numbered registers are pushed before lower-numbered registers, and
  // thus get higher addresses.
  // Floating-point registers are pushed before general-purpose registers, and
  // thus get higher addresses.
  //
  // Note that registers are not checked for invalid values. Use this method
  // only if you know that the GC won't try to examine the values on the stack.
  //
  // This method must not be called unless the current stack pointer (as set by
  // SetStackPointer) is the system stack pointer (csp), and is aligned to
  // ActivationFrameAlignment().
  void PushCalleeSavedRegisters();

  // Restore the callee-saved registers (as defined by AAPCS64).
  //
  // Higher-numbered registers are popped after lower-numbered registers, and
  // thus come from higher addresses.
  // Floating-point registers are popped after general-purpose registers, and
  // thus come from higher addresses.
  //
  // This method must not be called unless the current stack pointer (as set by
  // SetStackPointer) is the system stack pointer (csp), and is aligned to
  // ActivationFrameAlignment().
  void PopCalleeSavedRegisters();

  // Align csp for a frame, as per ActivationFrameAlignment, and make it the
  // current stack pointer.
  inline void AlignAndSetCSPForFrame();

  // Helpers ------------------------------------------------------------------

  static int SafepointRegisterStackIndex(int reg_code);

  void LoadInstanceDescriptors(Register map,
                               Register descriptors);
  void LoadAccessor(Register dst, Register holder, int accessor_index,
                    AccessorComponent accessor);

  template<typename Field>
  void DecodeField(Register dst, Register src) {
    static const int shift = Field::kShift;
    static const int setbits = CountSetBits(Field::kMask, 32);
    Ubfx(dst, src, shift, setbits);
  }

  template<typename Field>
  void DecodeField(Register reg) {
    DecodeField<Field>(reg, reg);
  }

  // ---- SMI and Number Utilities ----

  inline void SmiTag(Register dst, Register src);
  inline void SmiTag(Register smi);
  inline void SmiUntagToDouble(VRegister dst, Register src,
                               UntagMode mode = kNotSpeculativeUntag);
  inline void SmiUntagToFloat(VRegister dst, Register src,
                              UntagMode mode = kNotSpeculativeUntag);

  // Tag and push in one step.
  inline void SmiTagAndPush(Register src);
  inline void SmiTagAndPush(Register src1, Register src2);

  inline void JumpIfNotSmi(Register value, Label* not_smi_label);
  inline void JumpIfBothSmi(Register value1,
                            Register value2,
                            Label* both_smi_label,
                            Label* not_smi_label = NULL);
  inline void JumpIfEitherSmi(Register value1,
                              Register value2,
                              Label* either_smi_label,
                              Label* not_smi_label = NULL);
  inline void JumpIfEitherNotSmi(Register value1,
                                 Register value2,
                                 Label* not_smi_label);
  inline void JumpIfBothNotSmi(Register value1,
                               Register value2,
                               Label* not_smi_label);

  // Abort execution if argument is a smi, enabled via --debug-code.
  void AssertNotSmi(Register object, BailoutReason reason = kOperandIsASmi);

  inline void ObjectTag(Register tagged_obj, Register obj);
  inline void ObjectUntag(Register untagged_obj, Register obj);

  // Abort execution if argument is not a FixedArray, enabled via --debug-code.
  void AssertFixedArray(Register object);

  // Abort execution if argument is not a JSFunction, enabled via --debug-code.
  void AssertFunction(Register object);

  // Abort execution if argument is not a JSGeneratorObject (or subclass),
  // enabled via --debug-code.
  void AssertGeneratorObject(Register object);

  // Abort execution if argument is not a JSBoundFunction,
  // enabled via --debug-code.
  void AssertBoundFunction(Register object);

  // Abort execution if argument is not undefined or an AllocationSite, enabled
  // via --debug-code.
  void AssertUndefinedOrAllocationSite(Register object, Register scratch);

  void JumpIfHeapNumber(Register object, Label* on_heap_number,
                        SmiCheckType smi_check_type = DONT_DO_SMI_CHECK);
  void JumpIfNotHeapNumber(Register object, Label* on_not_heap_number,
                           SmiCheckType smi_check_type = DONT_DO_SMI_CHECK);

  // Try to represent a double as a signed 64-bit int.
  // This succeeds if the result compares equal to the input, so inputs of -0.0
  // are represented as 0 and handled as a success.
  //
  // On output the Z flag is set if the operation was successful.
  void TryRepresentDoubleAsInt64(Register as_int, VRegister value,
                                 VRegister scratch_d,
                                 Label* on_successful_conversion = NULL,
                                 Label* on_failed_conversion = NULL) {
    DCHECK(as_int.Is64Bits());
    TryRepresentDoubleAsInt(as_int, value, scratch_d, on_successful_conversion,
                            on_failed_conversion);
  }

  // ---- String Utilities ----

  // Checks if both instance types are sequential one-byte strings and jumps to
  // label if either is not.
  void JumpIfBothInstanceTypesAreNotSequentialOneByte(
      Register first_object_instance_type, Register second_object_instance_type,
      Register scratch1, Register scratch2, Label* failure);

  void JumpIfNotUniqueNameInstanceType(Register type, Label* not_unique_name);

  // ---- Calling / Jumping helpers ----

  void CallStub(CodeStub* stub);
  void TailCallStub(CodeStub* stub);

  void CallRuntime(const Runtime::Function* f,
                   int num_arguments,
                   SaveFPRegsMode save_doubles = kDontSaveFPRegs);

  // Convenience function: Same as above, but takes the fid instead.
  void CallRuntime(Runtime::FunctionId fid, int num_arguments,
                   SaveFPRegsMode save_doubles = kDontSaveFPRegs) {
    CallRuntime(Runtime::FunctionForId(fid), num_arguments, save_doubles);
  }

  // Convenience function: Same as above, but takes the fid instead.
  void CallRuntime(Runtime::FunctionId fid,
                   SaveFPRegsMode save_doubles = kDontSaveFPRegs) {
    const Runtime::Function* function = Runtime::FunctionForId(fid);
    CallRuntime(function, function->nargs, save_doubles);
  }

  void TailCallRuntime(Runtime::FunctionId fid);

  // Jump to a runtime routine.
  void JumpToExternalReference(const ExternalReference& builtin,
                               bool builtin_exit_frame = false);

  // Registers used through the invocation chain are hard-coded.
  // We force passing the parameters to ensure the contracts are correctly
  // honoured by the caller.
  // 'function' must be x1.
  // 'actual' must use an immediate or x0.
  // 'expected' must use an immediate or x2.
  // 'call_kind' must be x5.
  void InvokePrologue(const ParameterCount& expected,
                      const ParameterCount& actual, Label* done,
                      InvokeFlag flag, bool* definitely_mismatches);

  // On function call, call into the debugger if necessary.
  void CheckDebugHook(Register fun, Register new_target,
                      const ParameterCount& expected,
                      const ParameterCount& actual);
  void InvokeFunctionCode(Register function, Register new_target,
                          const ParameterCount& expected,
                          const ParameterCount& actual, InvokeFlag flag);
  // Invoke the JavaScript function in the given register.
  // Changes the current context to the context in the function before invoking.
  void InvokeFunction(Register function, Register new_target,
                      const ParameterCount& actual, InvokeFlag flag);
  void InvokeFunction(Register function, const ParameterCount& expected,
                      const ParameterCount& actual, InvokeFlag flag);
  void InvokeFunction(Handle<JSFunction> function,
                      const ParameterCount& expected,
                      const ParameterCount& actual, InvokeFlag flag);

  // ---- Code generation helpers ----

  // Frame restart support
  void MaybeDropFrames();

  // Exception handling

  // Push a new stack handler and link into stack handler chain.
  void PushStackHandler();

  // Unlink the stack handler on top of the stack from the stack handler chain.
  // Must preserve the result register.
  void PopStackHandler();


  // ---------------------------------------------------------------------------
  // Allocation support

  // Allocate an object in new space or old space. The object_size is
  // specified either in bytes or in words if the allocation flag SIZE_IN_WORDS
  // is passed. The allocated object is returned in result.
  //
  // If the new space is exhausted control continues at the gc_required label.
  // In this case, the result and scratch registers may still be clobbered.
  void Allocate(int object_size,
                Register result,
                Register scratch1,
                Register scratch2,
                Label* gc_required,
                AllocationFlags flags);

  // Allocate and initialize a JSValue wrapper with the specified {constructor}
  // and {value}.
  void AllocateJSValue(Register result, Register constructor, Register value,
                       Register scratch1, Register scratch2,
                       Label* gc_required);

  // ---------------------------------------------------------------------------
  // Support functions.

  // Machine code version of Map::GetConstructor().
  // |temp| holds |result|'s map when done, and |temp2| its instance type.
  void GetMapConstructor(Register result, Register map, Register temp,
                         Register temp2);

  // Compare object type for heap object.  heap_object contains a non-Smi
  // whose object type should be compared with the given type.  This both
  // sets the flags and leaves the object type in the type_reg register.
  // It leaves the map in the map register (unless the type_reg and map register
  // are the same register).  It leaves the heap object in the heap_object
  // register unless the heap_object register is the same register as one of the
  // other registers.
  void CompareObjectType(Register heap_object,
                         Register map,
                         Register type_reg,
                         InstanceType type);


  // Compare object type for heap object, and branch if equal (or not.)
  // heap_object contains a non-Smi whose object type should be compared with
  // the given type.  This both sets the flags and leaves the object type in
  // the type_reg register. It leaves the map in the map register (unless the
  // type_reg and map register are the same register).  It leaves the heap
  // object in the heap_object register unless the heap_object register is the
  // same register as one of the other registers.
  void JumpIfObjectType(Register object,
                        Register map,
                        Register type_reg,
                        InstanceType type,
                        Label* if_cond_pass,
                        Condition cond = eq);

  void JumpIfNotObjectType(Register object,
                           Register map,
                           Register type_reg,
                           InstanceType type,
                           Label* if_not_object);

  // Compare instance type in a map.  map contains a valid map object whose
  // object type should be compared with the given type.  This both
  // sets the flags and leaves the object type in the type_reg register.
  void CompareInstanceType(Register map,
                           Register type_reg,
                           InstanceType type);

  // Compare an object's map with the specified map. Condition flags are set
  // with result of map compare.
  void CompareObjectMap(Register obj, Heap::RootListIndex index);

  // Compare an object's map with the specified map. Condition flags are set
  // with result of map compare.
  void CompareObjectMap(Register obj, Register scratch, Handle<Map> map);

  // As above, but the map of the object is already loaded into the register
  // which is preserved by the code generated.
  void CompareMap(Register obj_map,
                  Handle<Map> map);

  // Check if the map of an object is equal to a specified map and branch to
  // label if not. Skip the smi check if not required (object is known to be a
  // heap object). If mode is ALLOW_ELEMENT_TRANSITION_MAPS, then also match
  // against maps that are ElementsKind transition maps of the specified map.
  void CheckMap(Register obj,
                Register scratch,
                Handle<Map> map,
                Label* fail,
                SmiCheckType smi_check_type);


  void CheckMap(Register obj,
                Register scratch,
                Heap::RootListIndex index,
                Label* fail,
                SmiCheckType smi_check_type);

  // As above, but the map of the object is already loaded into obj_map, and is
  // preserved.
  void CheckMap(Register obj_map,
                Handle<Map> map,
                Label* fail,
                SmiCheckType smi_check_type);

  void GetWeakValue(Register value, Handle<WeakCell> cell);

  // Load the value of the weak cell in the value register. Branch to the given
  // miss label if the weak cell was cleared.
  void LoadWeakValue(Register value, Handle<WeakCell> cell, Label* miss);

  // Load the elements kind field from a map, and return it in the result
  // register.
  void LoadElementsKindFromMap(Register result, Register map);

  // Load the value from the root list and push it onto the stack.
  void PushRoot(Heap::RootListIndex index);

  // Compare the object in a register to a value from the root list.
  void CompareRoot(const Register& obj, Heap::RootListIndex index);

  // Compare the object in a register to a value and jump if they are equal.
  void JumpIfRoot(const Register& obj,
                  Heap::RootListIndex index,
                  Label* if_equal);

  // Compare the object in a register to a value and jump if they are not equal.
  void JumpIfNotRoot(const Register& obj,
                     Heap::RootListIndex index,
                     Label* if_not_equal);

  // Compare the contents of a register with an operand, and branch to true,
  // false or fall through, depending on condition.
  void CompareAndSplit(const Register& lhs,
                       const Operand& rhs,
                       Condition cond,
                       Label* if_true,
                       Label* if_false,
                       Label* fall_through);

  // Test the bits of register defined by bit_pattern, and branch to
  // if_any_set, if_all_clear or fall_through accordingly.
  void TestAndSplit(const Register& reg,
                    uint64_t bit_pattern,
                    Label* if_all_clear,
                    Label* if_any_set,
                    Label* fall_through);

  // ---------------------------------------------------------------------------
  // Frames.

  void EnterBuiltinFrame(Register context, Register target, Register argc);
  void LeaveBuiltinFrame(Register context, Register target, Register argc);

  // The stack pointer has to switch between csp and jssp when setting up and
  // destroying the exit frame. Hence preserving/restoring the registers is
  // slightly more complicated than simple push/pop operations.
  void ExitFramePreserveFPRegs();
  void ExitFrameRestoreFPRegs();

  // Enter exit frame. Exit frames are used when calling C code from generated
  // (JavaScript) code.
  //
  // The stack pointer must be jssp on entry, and will be set to csp by this
  // function. The frame pointer is also configured, but the only other
  // registers modified by this function are the provided scratch register, and
  // jssp.
  //
  // The 'extra_space' argument can be used to allocate some space in the exit
  // frame that will be ignored by the GC. This space will be reserved in the
  // bottom of the frame immediately above the return address slot.
  //
  // Set up a stack frame and registers as follows:
  //         fp[8]: CallerPC (lr)
  //   fp -> fp[0]: CallerFP (old fp)
  //         fp[-8]: SPOffset (new csp)
  //         fp[-16]: CodeObject()
  //         fp[-16 - fp-size]: Saved doubles, if saved_doubles is true.
  //         csp[8]: Memory reserved for the caller if extra_space != 0.
  //                 Alignment padding, if necessary.
  //  csp -> csp[0]: Space reserved for the return address.
  //
  // This function also stores the new frame information in the top frame, so
  // that the new frame becomes the current frame.
  void EnterExitFrame(bool save_doubles, const Register& scratch,
                      int extra_space = 0,
                      StackFrame::Type frame_type = StackFrame::EXIT);

  // Leave the current exit frame, after a C function has returned to generated
  // (JavaScript) code.
  //
  // This effectively unwinds the operation of EnterExitFrame:
  //  * Preserved doubles are restored (if restore_doubles is true).
  //  * The frame information is removed from the top frame.
  //  * The exit frame is dropped.
  //  * The stack pointer is reset to jssp.
  //
  // The stack pointer must be csp on entry.
  void LeaveExitFrame(bool save_doubles,
                      const Register& scratch,
                      bool restore_context);

  // Load the global object from the current context.
  void LoadGlobalObject(Register dst) {
    LoadNativeContextSlot(Context::EXTENSION_INDEX, dst);
  }

  // Load the global proxy from the current context.
  void LoadGlobalProxy(Register dst) {
    LoadNativeContextSlot(Context::GLOBAL_PROXY_INDEX, dst);
  }

  // ---------------------------------------------------------------------------
  // StatsCounter support

  void IncrementCounter(StatsCounter* counter, int value, Register scratch1,
                        Register scratch2);
  void DecrementCounter(StatsCounter* counter, int value, Register scratch1,
                        Register scratch2);

  // ---------------------------------------------------------------------------
  // Garbage collector support (GC).

  enum RememberedSetFinalAction {
    kReturnAtEnd,
    kFallThroughAtEnd
  };

  // Record in the remembered set the fact that we have a pointer to new space
  // at the address pointed to by the addr register. Only works if addr is not
  // in new space.
  void RememberedSetHelper(Register object,  // Used for debug code.
                           Register addr,
                           Register scratch1,
                           SaveFPRegsMode save_fp,
                           RememberedSetFinalAction and_then);

  // Push and pop the registers that can hold pointers, as defined by the
  // RegList constant kSafepointSavedRegisters.
  void PushSafepointRegisters();
  void PopSafepointRegisters();

  void CheckPageFlag(const Register& object, const Register& scratch, int mask,
                     Condition cc, Label* condition_met);

  // Check if object is in new space and jump accordingly.
  // Register 'object' is preserved.
  void JumpIfNotInNewSpace(Register object,
                           Label* branch) {
    InNewSpace(object, ne, branch);
  }

  void JumpIfInNewSpace(Register object,
                        Label* branch) {
    InNewSpace(object, eq, branch);
  }

  // Notify the garbage collector that we wrote a pointer into an object.
  // |object| is the object being stored into, |value| is the object being
  // stored.  value and scratch registers are clobbered by the operation.
  // The offset is the offset from the start of the object, not the offset from
  // the tagged HeapObject pointer.  For use with FieldMemOperand(reg, off).
  void RecordWriteField(
      Register object,
      int offset,
      Register value,
      Register scratch,
      LinkRegisterStatus lr_status,
      SaveFPRegsMode save_fp,
      RememberedSetAction remembered_set_action = EMIT_REMEMBERED_SET,
      SmiCheck smi_check = INLINE_SMI_CHECK,
      PointersToHereCheck pointers_to_here_check_for_value =
          kPointersToHereMaybeInteresting);

  // As above, but the offset has the tag presubtracted. For use with
  // MemOperand(reg, off).
  inline void RecordWriteContextSlot(
      Register context,
      int offset,
      Register value,
      Register scratch,
      LinkRegisterStatus lr_status,
      SaveFPRegsMode save_fp,
      RememberedSetAction remembered_set_action = EMIT_REMEMBERED_SET,
      SmiCheck smi_check = INLINE_SMI_CHECK,
      PointersToHereCheck pointers_to_here_check_for_value =
          kPointersToHereMaybeInteresting) {
    RecordWriteField(context,
                     offset + kHeapObjectTag,
                     value,
                     scratch,
                     lr_status,
                     save_fp,
                     remembered_set_action,
                     smi_check,
                     pointers_to_here_check_for_value);
  }

  void RecordWriteForMap(
      Register object,
      Register map,
      Register dst,
      LinkRegisterStatus lr_status,
      SaveFPRegsMode save_fp);

  // For a given |object| notify the garbage collector that the slot |address|
  // has been written.  |value| is the object being stored. The value and
  // address registers are clobbered by the operation.
  void RecordWrite(
      Register object,
      Register address,
      Register value,
      LinkRegisterStatus lr_status,
      SaveFPRegsMode save_fp,
      RememberedSetAction remembered_set_action = EMIT_REMEMBERED_SET,
      SmiCheck smi_check = INLINE_SMI_CHECK,
      PointersToHereCheck pointers_to_here_check_for_value =
          kPointersToHereMaybeInteresting);

  // Checks the color of an object.  If the object is white we jump to the
  // incremental marker.
  void JumpIfWhite(Register value, Register scratch1, Register scratch2,
                   Register scratch3, Register scratch4, Label* value_is_white);

  // Helper for finding the mark bits for an address.
  // Note that the behaviour slightly differs from other architectures.
  // On exit:
  //  - addr_reg is unchanged.
  //  - The bitmap register points at the word with the mark bits.
  //  - The shift register contains the index of the first color bit for this
  //    object in the bitmap.
  inline void GetMarkBits(Register addr_reg,
                          Register bitmap_reg,
                          Register shift_reg);

  // Check if an object has a given incremental marking color.
  void HasColor(Register object,
                Register scratch0,
                Register scratch1,
                Label* has_color,
                int first_bit,
                int second_bit);

  void JumpIfBlack(Register object,
                   Register scratch0,
                   Register scratch1,
                   Label* on_black);


  // ---------------------------------------------------------------------------
  // Debugging.

  void AssertRegisterIsRoot(
      Register reg,
      Heap::RootListIndex index,
      BailoutReason reason = kRegisterDidNotMatchExpectedRoot);

  // Abort if the specified register contains the invalid color bit pattern.
  // The pattern must be in bits [1:0] of 'reg' register.
  //
  // If emit_debug_code() is false, this emits no code.
  void AssertHasValidColor(const Register& reg);

  void LoadNativeContextSlot(int index, Register dst);

  // Load the initial map from the global function. The registers function and
  // map can be the same, function is then overwritten.
  void LoadGlobalFunctionInitialMap(Register function,
                                    Register map,
                                    Register scratch);

  // Like printf, but print at run-time from generated code.
  //
  // The caller must ensure that arguments for floating-point placeholders
  // (such as %e, %f or %g) are VRegisters, and that arguments for integer
  // placeholders are Registers.
  //
  // At the moment it is only possible to print the value of csp if it is the
  // current stack pointer. Otherwise, the MacroAssembler will automatically
  // update csp on every push (using BumpSystemStackPointer), so determining its
  // value is difficult.
  //
  // Format placeholders that refer to more than one argument, or to a specific
  // argument, are not supported. This includes formats like "%1$d" or "%.*d".
  //
  // This function automatically preserves caller-saved registers so that
  // calling code can use Printf at any point without having to worry about
  // corruption. The preservation mechanism generates a lot of code. If this is
  // a problem, preserve the important registers manually and then call
  // PrintfNoPreserve. Callee-saved registers are not used by Printf, and are
  // implicitly preserved.
  void Printf(const char * format,
              CPURegister arg0 = NoCPUReg,
              CPURegister arg1 = NoCPUReg,
              CPURegister arg2 = NoCPUReg,
              CPURegister arg3 = NoCPUReg);

  // Like Printf, but don't preserve any caller-saved registers, not even 'lr'.
  //
  // The return code from the system printf call will be returned in x0.
  void PrintfNoPreserve(const char * format,
                        const CPURegister& arg0 = NoCPUReg,
                        const CPURegister& arg1 = NoCPUReg,
                        const CPURegister& arg2 = NoCPUReg,
                        const CPURegister& arg3 = NoCPUReg);

  // Return true if the sequence is a young sequence geneated by
  // EmitFrameSetupForCodeAgePatching. Otherwise, this method asserts that the
  // sequence is a code age sequence (emitted by EmitCodeAgeSequence).
  static bool IsYoungSequence(Isolate* isolate, byte* sequence);

 private:
  // Helper for implementing JumpIfNotInNewSpace and JumpIfInNewSpace.
  void InNewSpace(Register object,
                  Condition cond,  // eq for new space, ne otherwise.
                  Label* branch);

  // Try to represent a double as an int so that integer fast-paths may be
  // used. Not every valid integer value is guaranteed to be caught.
  // It supports both 32-bit and 64-bit integers depending whether 'as_int'
  // is a W or X register.
  //
  // This does not distinguish between +0 and -0, so if this distinction is
  // important it must be checked separately.
  //
  // On output the Z flag is set if the operation was successful.
  void TryRepresentDoubleAsInt(Register as_int, VRegister value,
                               VRegister scratch_d,
                               Label* on_successful_conversion = NULL,
                               Label* on_failed_conversion = NULL);

 public:
  // Far branches resolving.
  //
  // The various classes of branch instructions with immediate offsets have
  // different ranges. While the Assembler will fail to assemble a branch
  // exceeding its range, the MacroAssembler offers a mechanism to resolve
  // branches to too distant targets, either by tweaking the generated code to
  // use branch instructions with wider ranges or generating veneers.
  //
  // Currently branches to distant targets are resolved using unconditional
  // branch isntructions with a range of +-128MB. If that becomes too little
  // (!), the mechanism can be extended to generate special veneers for really
  // far targets.
};


// Use this scope when you need a one-to-one mapping between methods and
// instructions. This scope prevents the MacroAssembler from being called and
// literal pools from being emitted. It also asserts the number of instructions
// emitted is what you specified when creating the scope.
class InstructionAccurateScope BASE_EMBEDDED {
 public:
  explicit InstructionAccurateScope(TurboAssembler* tasm, size_t count = 0)
      : tasm_(tasm)
#ifdef DEBUG
        ,
        size_(count * kInstructionSize)
#endif
  {
    // Before blocking the const pool, see if it needs to be emitted.
    tasm_->CheckConstPool(false, true);
    tasm_->CheckVeneerPool(false, true);

    tasm_->StartBlockPools();
#ifdef DEBUG
    if (count != 0) {
      tasm_->bind(&start_);
    }
    previous_allow_macro_instructions_ = tasm_->allow_macro_instructions();
    tasm_->set_allow_macro_instructions(false);
#endif
  }

  ~InstructionAccurateScope() {
    tasm_->EndBlockPools();
#ifdef DEBUG
    if (start_.is_bound()) {
      DCHECK(tasm_->SizeOfCodeGeneratedSince(&start_) == size_);
    }
    tasm_->set_allow_macro_instructions(previous_allow_macro_instructions_);
#endif
  }

 private:
  TurboAssembler* tasm_;
#ifdef DEBUG
  size_t size_;
  Label start_;
  bool previous_allow_macro_instructions_;
#endif
};

// This scope utility allows scratch registers to be managed safely. The
// TurboAssembler's TmpList() (and FPTmpList()) is used as a pool of scratch
// registers. These registers can be allocated on demand, and will be returned
// at the end of the scope.
//
// When the scope ends, the MacroAssembler's lists will be restored to their
// original state, even if the lists were modified by some other means. Note
// that this scope can be nested but the destructors need to run in the opposite
// order as the constructors. We do not have assertions for this.
class UseScratchRegisterScope {
 public:
  explicit UseScratchRegisterScope(TurboAssembler* tasm)
      : available_(tasm->TmpList()),
        availablefp_(tasm->FPTmpList()),
        old_available_(available_->list()),
        old_availablefp_(availablefp_->list()) {
    DCHECK_EQ(available_->type(), CPURegister::kRegister);
    DCHECK_EQ(availablefp_->type(), CPURegister::kVRegister);
  }

  ~UseScratchRegisterScope();

  // Take a register from the appropriate temps list. It will be returned
  // automatically when the scope ends.
  Register AcquireW() { return AcquireNextAvailable(available_).W(); }
  Register AcquireX() { return AcquireNextAvailable(available_).X(); }
  VRegister AcquireS() { return AcquireNextAvailable(availablefp_).S(); }
  VRegister AcquireD() { return AcquireNextAvailable(availablefp_).D(); }
  VRegister AcquireV(VectorFormat format) {
    return VRegister::Create(AcquireNextAvailable(availablefp_).code(), format);
  }

  Register AcquireSameSizeAs(const Register& reg);
  VRegister AcquireSameSizeAs(const VRegister& reg);

 private:
  static CPURegister AcquireNextAvailable(CPURegList* available);

  // Available scratch registers.
  CPURegList* available_;     // kRegister
  CPURegList* availablefp_;   // kVRegister

  // The state of the available lists at the start of this scope.
  RegList old_available_;     // kRegister
  RegList old_availablefp_;   // kVRegister
};

MemOperand ContextMemOperand(Register context, int index = 0);
MemOperand NativeContextMemOperand();

// Encode and decode information about patchable inline SMI checks.
class InlineSmiCheckInfo {
 public:
  explicit InlineSmiCheckInfo(Address info);

  bool HasSmiCheck() const {
    return smi_check_ != NULL;
  }

  const Register& SmiRegister() const {
    return reg_;
  }

  Instruction* SmiCheck() const {
    return smi_check_;
  }

  int SmiCheckDelta() const { return smi_check_delta_; }

  // Use MacroAssembler::InlineData to emit information about patchable inline
  // SMI checks. The caller may specify 'reg' as NoReg and an unbound 'site' to
  // indicate that there is no inline SMI check. Note that 'reg' cannot be csp.
  //
  // The generated patch information can be read using the InlineSMICheckInfo
  // class.
  static void Emit(MacroAssembler* masm, const Register& reg,
                   const Label* smi_check);

  // Emit information to indicate that there is no inline SMI check.
  static void EmitNotInlined(MacroAssembler* masm) {
    Label unbound;
    Emit(masm, NoReg, &unbound);
  }

 private:
  Register reg_;
  int smi_check_delta_;
  Instruction* smi_check_;

  // Fields in the data encoded by InlineData.

  // A width of 5 (Rd_width) for the SMI register preclues the use of csp,
  // since kSPRegInternalCode is 63. However, csp should never hold a SMI or be
  // used in a patchable check. The Emit() method checks this.
  //
  // Note that the total size of the fields is restricted by the underlying
  // storage size handled by the BitField class, which is a uint32_t.
  class RegisterBits : public BitField<unsigned, 0, 5> {};
  class DeltaBits : public BitField<uint32_t, 5, 32-5> {};
};

}  // namespace internal
}  // namespace v8

#define ACCESS_MASM(masm) masm->

#endif  // V8_ARM64_MACRO_ASSEMBLER_ARM64_H_
