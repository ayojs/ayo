// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_ARM64_MACRO_ASSEMBLER_ARM64_INL_H_
#define V8_ARM64_MACRO_ASSEMBLER_ARM64_INL_H_

#include <ctype.h>

#include "src/globals.h"

#include "src/arm64/assembler-arm64-inl.h"
#include "src/arm64/assembler-arm64.h"
#include "src/arm64/instrument-arm64.h"
#include "src/base/bits.h"
#include "src/macro-assembler.h"

namespace v8 {
namespace internal {


MemOperand FieldMemOperand(Register object, int offset) {
  return MemOperand(object, offset - kHeapObjectTag);
}


MemOperand UntagSmiFieldMemOperand(Register object, int offset) {
  return UntagSmiMemOperand(object, offset - kHeapObjectTag);
}


MemOperand UntagSmiMemOperand(Register object, int offset) {
  // Assumes that Smis are shifted by 32 bits and little endianness.
  STATIC_ASSERT(kSmiShift == 32);
  return MemOperand(object, offset + (kSmiShift / kBitsPerByte));
}

void TurboAssembler::And(const Register& rd, const Register& rn,
                         const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  LogicalMacro(rd, rn, operand, AND);
}

void TurboAssembler::Ands(const Register& rd, const Register& rn,
                          const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  LogicalMacro(rd, rn, operand, ANDS);
}

void TurboAssembler::Tst(const Register& rn, const Operand& operand) {
  DCHECK(allow_macro_instructions());
  LogicalMacro(AppropriateZeroRegFor(rn), rn, operand, ANDS);
}

void TurboAssembler::Bic(const Register& rd, const Register& rn,
                         const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  LogicalMacro(rd, rn, operand, BIC);
}


void MacroAssembler::Bics(const Register& rd,
                          const Register& rn,
                          const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  LogicalMacro(rd, rn, operand, BICS);
}

void TurboAssembler::Orr(const Register& rd, const Register& rn,
                         const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  LogicalMacro(rd, rn, operand, ORR);
}

void TurboAssembler::Orn(const Register& rd, const Register& rn,
                         const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  LogicalMacro(rd, rn, operand, ORN);
}

void TurboAssembler::Eor(const Register& rd, const Register& rn,
                         const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  LogicalMacro(rd, rn, operand, EOR);
}

void TurboAssembler::Eon(const Register& rd, const Register& rn,
                         const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  LogicalMacro(rd, rn, operand, EON);
}

void TurboAssembler::Ccmp(const Register& rn, const Operand& operand,
                          StatusFlags nzcv, Condition cond) {
  DCHECK(allow_macro_instructions());
  if (operand.IsImmediate() && (operand.ImmediateValue() < 0)) {
    ConditionalCompareMacro(rn, -operand.ImmediateValue(), nzcv, cond, CCMN);
  } else {
    ConditionalCompareMacro(rn, operand, nzcv, cond, CCMP);
  }
}


void MacroAssembler::Ccmn(const Register& rn,
                          const Operand& operand,
                          StatusFlags nzcv,
                          Condition cond) {
  DCHECK(allow_macro_instructions());
  if (operand.IsImmediate() && (operand.ImmediateValue() < 0)) {
    ConditionalCompareMacro(rn, -operand.ImmediateValue(), nzcv, cond, CCMP);
  } else {
    ConditionalCompareMacro(rn, operand, nzcv, cond, CCMN);
  }
}

void TurboAssembler::Add(const Register& rd, const Register& rn,
                         const Operand& operand) {
  DCHECK(allow_macro_instructions());
  if (operand.IsImmediate() && (operand.ImmediateValue() < 0) &&
      IsImmAddSub(-operand.ImmediateValue())) {
    AddSubMacro(rd, rn, -operand.ImmediateValue(), LeaveFlags, SUB);
  } else {
    AddSubMacro(rd, rn, operand, LeaveFlags, ADD);
  }
}

void TurboAssembler::Adds(const Register& rd, const Register& rn,
                          const Operand& operand) {
  DCHECK(allow_macro_instructions());
  if (operand.IsImmediate() && (operand.ImmediateValue() < 0) &&
      IsImmAddSub(-operand.ImmediateValue())) {
    AddSubMacro(rd, rn, -operand.ImmediateValue(), SetFlags, SUB);
  } else {
    AddSubMacro(rd, rn, operand, SetFlags, ADD);
  }
}

void TurboAssembler::Sub(const Register& rd, const Register& rn,
                         const Operand& operand) {
  DCHECK(allow_macro_instructions());
  if (operand.IsImmediate() && (operand.ImmediateValue() < 0) &&
      IsImmAddSub(-operand.ImmediateValue())) {
    AddSubMacro(rd, rn, -operand.ImmediateValue(), LeaveFlags, ADD);
  } else {
    AddSubMacro(rd, rn, operand, LeaveFlags, SUB);
  }
}

void TurboAssembler::Subs(const Register& rd, const Register& rn,
                          const Operand& operand) {
  DCHECK(allow_macro_instructions());
  if (operand.IsImmediate() && (operand.ImmediateValue() < 0) &&
      IsImmAddSub(-operand.ImmediateValue())) {
    AddSubMacro(rd, rn, -operand.ImmediateValue(), SetFlags, ADD);
  } else {
    AddSubMacro(rd, rn, operand, SetFlags, SUB);
  }
}

void TurboAssembler::Cmn(const Register& rn, const Operand& operand) {
  DCHECK(allow_macro_instructions());
  Adds(AppropriateZeroRegFor(rn), rn, operand);
}

void TurboAssembler::Cmp(const Register& rn, const Operand& operand) {
  DCHECK(allow_macro_instructions());
  Subs(AppropriateZeroRegFor(rn), rn, operand);
}

void TurboAssembler::Neg(const Register& rd, const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  if (operand.IsImmediate()) {
    Mov(rd, -operand.ImmediateValue());
  } else {
    Sub(rd, AppropriateZeroRegFor(rd), operand);
  }
}

void TurboAssembler::Negs(const Register& rd, const Operand& operand) {
  DCHECK(allow_macro_instructions());
  Subs(rd, AppropriateZeroRegFor(rd), operand);
}

void TurboAssembler::Adc(const Register& rd, const Register& rn,
                         const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  AddSubWithCarryMacro(rd, rn, operand, LeaveFlags, ADC);
}


void MacroAssembler::Adcs(const Register& rd,
                          const Register& rn,
                          const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  AddSubWithCarryMacro(rd, rn, operand, SetFlags, ADC);
}


void MacroAssembler::Sbc(const Register& rd,
                         const Register& rn,
                         const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  AddSubWithCarryMacro(rd, rn, operand, LeaveFlags, SBC);
}


void MacroAssembler::Sbcs(const Register& rd,
                          const Register& rn,
                          const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  AddSubWithCarryMacro(rd, rn, operand, SetFlags, SBC);
}


void MacroAssembler::Ngc(const Register& rd,
                         const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  Register zr = AppropriateZeroRegFor(rd);
  Sbc(rd, zr, operand);
}


void MacroAssembler::Ngcs(const Register& rd,
                          const Operand& operand) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  Register zr = AppropriateZeroRegFor(rd);
  Sbcs(rd, zr, operand);
}

void TurboAssembler::Mvn(const Register& rd, uint64_t imm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  Mov(rd, ~imm);
}

#define DEFINE_FUNCTION(FN, REGTYPE, REG, OP)                          \
  void TurboAssembler::FN(const REGTYPE REG, const MemOperand& addr) { \
    DCHECK(allow_macro_instructions());                                \
    LoadStoreMacro(REG, addr, OP);                                     \
  }
LS_MACRO_LIST(DEFINE_FUNCTION)
#undef DEFINE_FUNCTION

#define DEFINE_FUNCTION(FN, REGTYPE, REG, REG2, OP)              \
  void TurboAssembler::FN(const REGTYPE REG, const REGTYPE REG2, \
                          const MemOperand& addr) {              \
    DCHECK(allow_macro_instructions());                          \
    LoadStorePairMacro(REG, REG2, addr, OP);                     \
  }
LSPAIR_MACRO_LIST(DEFINE_FUNCTION)
#undef DEFINE_FUNCTION

#define DECLARE_FUNCTION(FN, OP)                                    \
  void TurboAssembler::FN(const Register& rt, const Register& rn) { \
    DCHECK(allow_macro_instructions());                             \
    OP(rt, rn);                                                     \
  }
LDA_STL_MACRO_LIST(DECLARE_FUNCTION)
#undef DECLARE_FUNCTION

#define DECLARE_FUNCTION(FN, OP)                                  \
  void MacroAssembler::FN(const Register& rs, const Register& rt, \
                          const Register& rn) {                   \
    DCHECK(allow_macro_instructions());                           \
    OP(rs, rt, rn);                                               \
  }
STLX_MACRO_LIST(DECLARE_FUNCTION)
#undef DECLARE_FUNCTION

void TurboAssembler::Asr(const Register& rd, const Register& rn,
                         unsigned shift) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  asr(rd, rn, shift);
}

void TurboAssembler::Asr(const Register& rd, const Register& rn,
                         const Register& rm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  asrv(rd, rn, rm);
}

void TurboAssembler::B(Label* label) {
  b(label);
  CheckVeneerPool(false, false);
}

void TurboAssembler::B(Condition cond, Label* label) {
  DCHECK(allow_macro_instructions());
  B(label, cond);
}

void TurboAssembler::Bfi(const Register& rd, const Register& rn, unsigned lsb,
                         unsigned width) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  bfi(rd, rn, lsb, width);
}


void MacroAssembler::Bfxil(const Register& rd,
                           const Register& rn,
                           unsigned lsb,
                           unsigned width) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  bfxil(rd, rn, lsb, width);
}

void TurboAssembler::Bind(Label* label) {
  DCHECK(allow_macro_instructions());
  bind(label);
}

void TurboAssembler::Bl(Label* label) {
  DCHECK(allow_macro_instructions());
  bl(label);
}

void TurboAssembler::Blr(const Register& xn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!xn.IsZero());
  blr(xn);
}

void TurboAssembler::Br(const Register& xn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!xn.IsZero());
  br(xn);
}

void TurboAssembler::Brk(int code) {
  DCHECK(allow_macro_instructions());
  brk(code);
}


void MacroAssembler::Cinc(const Register& rd,
                          const Register& rn,
                          Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  DCHECK((cond != al) && (cond != nv));
  cinc(rd, rn, cond);
}


void MacroAssembler::Cinv(const Register& rd,
                          const Register& rn,
                          Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  DCHECK((cond != al) && (cond != nv));
  cinv(rd, rn, cond);
}

void TurboAssembler::Cls(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  cls(rd, rn);
}

void TurboAssembler::Clz(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  clz(rd, rn);
}

void TurboAssembler::Cneg(const Register& rd, const Register& rn,
                          Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  DCHECK((cond != al) && (cond != nv));
  cneg(rd, rn, cond);
}


// Conditionally zero the destination register. Only X registers are supported
// due to the truncation side-effect when used on W registers.
void MacroAssembler::CzeroX(const Register& rd,
                            Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsSP() && rd.Is64Bits());
  DCHECK((cond != al) && (cond != nv));
  csel(rd, xzr, rd, cond);
}


// Conditionally move a value into the destination register. Only X registers
// are supported due to the truncation side-effect when used on W registers.
void MacroAssembler::CmovX(const Register& rd,
                           const Register& rn,
                           Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsSP());
  DCHECK(rd.Is64Bits() && rn.Is64Bits());
  DCHECK((cond != al) && (cond != nv));
  if (!rd.is(rn)) {
    csel(rd, rn, rd, cond);
  }
}

void TurboAssembler::Cset(const Register& rd, Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  DCHECK((cond != al) && (cond != nv));
  cset(rd, cond);
}


void MacroAssembler::Csetm(const Register& rd, Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  DCHECK((cond != al) && (cond != nv));
  csetm(rd, cond);
}

void TurboAssembler::Csinc(const Register& rd, const Register& rn,
                           const Register& rm, Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  DCHECK((cond != al) && (cond != nv));
  csinc(rd, rn, rm, cond);
}


void MacroAssembler::Csinv(const Register& rd,
                           const Register& rn,
                           const Register& rm,
                           Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  DCHECK((cond != al) && (cond != nv));
  csinv(rd, rn, rm, cond);
}


void MacroAssembler::Csneg(const Register& rd,
                           const Register& rn,
                           const Register& rm,
                           Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  DCHECK((cond != al) && (cond != nv));
  csneg(rd, rn, rm, cond);
}


void MacroAssembler::Dmb(BarrierDomain domain, BarrierType type) {
  DCHECK(allow_macro_instructions());
  dmb(domain, type);
}


void MacroAssembler::Dsb(BarrierDomain domain, BarrierType type) {
  DCHECK(allow_macro_instructions());
  dsb(domain, type);
}

void TurboAssembler::Debug(const char* message, uint32_t code, Instr params) {
  DCHECK(allow_macro_instructions());
  debug(message, code, params);
}


void MacroAssembler::Extr(const Register& rd,
                          const Register& rn,
                          const Register& rm,
                          unsigned lsb) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  extr(rd, rn, rm, lsb);
}

void TurboAssembler::Fabs(const VRegister& fd, const VRegister& fn) {
  DCHECK(allow_macro_instructions());
  fabs(fd, fn);
}

void TurboAssembler::Fadd(const VRegister& fd, const VRegister& fn,
                          const VRegister& fm) {
  DCHECK(allow_macro_instructions());
  fadd(fd, fn, fm);
}

void TurboAssembler::Fccmp(const VRegister& fn, const VRegister& fm,
                           StatusFlags nzcv, Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK((cond != al) && (cond != nv));
  fccmp(fn, fm, nzcv, cond);
}

void TurboAssembler::Fcmp(const VRegister& fn, const VRegister& fm) {
  DCHECK(allow_macro_instructions());
  fcmp(fn, fm);
}

void TurboAssembler::Fcmp(const VRegister& fn, double value) {
  DCHECK(allow_macro_instructions());
  if (value != 0.0) {
    UseScratchRegisterScope temps(this);
    VRegister tmp = temps.AcquireSameSizeAs(fn);
    Fmov(tmp, value);
    fcmp(fn, tmp);
  } else {
    fcmp(fn, value);
  }
}

void MacroAssembler::Fcsel(const VRegister& fd, const VRegister& fn,
                           const VRegister& fm, Condition cond) {
  DCHECK(allow_macro_instructions());
  DCHECK((cond != al) && (cond != nv));
  fcsel(fd, fn, fm, cond);
}

void TurboAssembler::Fcvt(const VRegister& fd, const VRegister& fn) {
  DCHECK(allow_macro_instructions());
  fcvt(fd, fn);
}

void TurboAssembler::Fcvtas(const Register& rd, const VRegister& fn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  fcvtas(rd, fn);
}

void TurboAssembler::Fcvtau(const Register& rd, const VRegister& fn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  fcvtau(rd, fn);
}

void TurboAssembler::Fcvtms(const Register& rd, const VRegister& fn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  fcvtms(rd, fn);
}

void TurboAssembler::Fcvtmu(const Register& rd, const VRegister& fn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  fcvtmu(rd, fn);
}

void TurboAssembler::Fcvtns(const Register& rd, const VRegister& fn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  fcvtns(rd, fn);
}

void TurboAssembler::Fcvtnu(const Register& rd, const VRegister& fn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  fcvtnu(rd, fn);
}

void TurboAssembler::Fcvtzs(const Register& rd, const VRegister& fn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  fcvtzs(rd, fn);
}
void TurboAssembler::Fcvtzu(const Register& rd, const VRegister& fn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  fcvtzu(rd, fn);
}

void TurboAssembler::Fdiv(const VRegister& fd, const VRegister& fn,
                          const VRegister& fm) {
  DCHECK(allow_macro_instructions());
  fdiv(fd, fn, fm);
}

void MacroAssembler::Fmadd(const VRegister& fd, const VRegister& fn,
                           const VRegister& fm, const VRegister& fa) {
  DCHECK(allow_macro_instructions());
  fmadd(fd, fn, fm, fa);
}

void TurboAssembler::Fmax(const VRegister& fd, const VRegister& fn,
                          const VRegister& fm) {
  DCHECK(allow_macro_instructions());
  fmax(fd, fn, fm);
}

void MacroAssembler::Fmaxnm(const VRegister& fd, const VRegister& fn,
                            const VRegister& fm) {
  DCHECK(allow_macro_instructions());
  fmaxnm(fd, fn, fm);
}

void TurboAssembler::Fmin(const VRegister& fd, const VRegister& fn,
                          const VRegister& fm) {
  DCHECK(allow_macro_instructions());
  fmin(fd, fn, fm);
}

void MacroAssembler::Fminnm(const VRegister& fd, const VRegister& fn,
                            const VRegister& fm) {
  DCHECK(allow_macro_instructions());
  fminnm(fd, fn, fm);
}

void TurboAssembler::Fmov(VRegister fd, VRegister fn) {
  DCHECK(allow_macro_instructions());
  // Only emit an instruction if fd and fn are different, and they are both D
  // registers. fmov(s0, s0) is not a no-op because it clears the top word of
  // d0. Technically, fmov(d0, d0) is not a no-op either because it clears the
  // top of q0, but VRegister does not currently support Q registers.
  if (!fd.Is(fn) || !fd.Is64Bits()) {
    fmov(fd, fn);
  }
}

void TurboAssembler::Fmov(VRegister fd, Register rn) {
  DCHECK(allow_macro_instructions());
  fmov(fd, rn);
}

void TurboAssembler::Fmov(VRegister vd, double imm) {
  DCHECK(allow_macro_instructions());

  if (vd.Is1S() || vd.Is2S() || vd.Is4S()) {
    Fmov(vd, static_cast<float>(imm));
    return;
  }

  DCHECK(vd.Is1D() || vd.Is2D());
  if (IsImmFP64(imm)) {
    fmov(vd, imm);
  } else {
    uint64_t bits = bit_cast<uint64_t>(imm);
    if (vd.IsScalar()) {
      if (bits == 0) {
        fmov(vd, xzr);
      } else {
        Ldr(vd, imm);
      }
    } else {
      // TODO(all): consider NEON support for load literal.
      Movi(vd, bits);
    }
  }
}

void TurboAssembler::Fmov(VRegister vd, float imm) {
  DCHECK(allow_macro_instructions());
  if (vd.Is1D() || vd.Is2D()) {
    Fmov(vd, static_cast<double>(imm));
    return;
  }

  DCHECK(vd.Is1S() || vd.Is2S() || vd.Is4S());
  if (IsImmFP32(imm)) {
    fmov(vd, imm);
  } else {
    uint32_t bits = bit_cast<uint32_t>(imm);
    if (vd.IsScalar()) {
      if (bits == 0) {
        fmov(vd, wzr);
      } else {
        UseScratchRegisterScope temps(this);
        Register tmp = temps.AcquireW();
        // TODO(all): Use Assembler::ldr(const VRegister& ft, float imm).
        Mov(tmp, bit_cast<uint32_t>(imm));
        Fmov(vd, tmp);
      }
    } else {
      // TODO(all): consider NEON support for load literal.
      Movi(vd, bits);
    }
  }
}

void TurboAssembler::Fmov(Register rd, VRegister fn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  fmov(rd, fn);
}

void MacroAssembler::Fmsub(const VRegister& fd, const VRegister& fn,
                           const VRegister& fm, const VRegister& fa) {
  DCHECK(allow_macro_instructions());
  fmsub(fd, fn, fm, fa);
}

void TurboAssembler::Fmul(const VRegister& fd, const VRegister& fn,
                          const VRegister& fm) {
  DCHECK(allow_macro_instructions());
  fmul(fd, fn, fm);
}

void MacroAssembler::Fnmadd(const VRegister& fd, const VRegister& fn,
                            const VRegister& fm, const VRegister& fa) {
  DCHECK(allow_macro_instructions());
  fnmadd(fd, fn, fm, fa);
}

void MacroAssembler::Fnmsub(const VRegister& fd, const VRegister& fn,
                            const VRegister& fm, const VRegister& fa) {
  DCHECK(allow_macro_instructions());
  fnmsub(fd, fn, fm, fa);
}

void TurboAssembler::Fsub(const VRegister& fd, const VRegister& fn,
                          const VRegister& fm) {
  DCHECK(allow_macro_instructions());
  fsub(fd, fn, fm);
}


void MacroAssembler::Hint(SystemHint code) {
  DCHECK(allow_macro_instructions());
  hint(code);
}


void MacroAssembler::Hlt(int code) {
  DCHECK(allow_macro_instructions());
  hlt(code);
}


void MacroAssembler::Isb() {
  DCHECK(allow_macro_instructions());
  isb();
}

void TurboAssembler::Ldr(const CPURegister& rt, const Operand& operand) {
  DCHECK(allow_macro_instructions());
  ldr(rt, operand);
}

void TurboAssembler::Ldr(const CPURegister& rt, double imm) {
  DCHECK(allow_macro_instructions());
  DCHECK(rt.Is64Bits());
  ldr(rt, Immediate(bit_cast<uint64_t>(imm)));
}

void TurboAssembler::Lsl(const Register& rd, const Register& rn,
                         unsigned shift) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  lsl(rd, rn, shift);
}

void TurboAssembler::Lsl(const Register& rd, const Register& rn,
                         const Register& rm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  lslv(rd, rn, rm);
}

void TurboAssembler::Lsr(const Register& rd, const Register& rn,
                         unsigned shift) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  lsr(rd, rn, shift);
}

void TurboAssembler::Lsr(const Register& rd, const Register& rn,
                         const Register& rm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  lsrv(rd, rn, rm);
}

void TurboAssembler::Madd(const Register& rd, const Register& rn,
                          const Register& rm, const Register& ra) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  madd(rd, rn, rm, ra);
}

void TurboAssembler::Mneg(const Register& rd, const Register& rn,
                          const Register& rm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  mneg(rd, rn, rm);
}

void TurboAssembler::Mov(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  // Emit a register move only if the registers are distinct, or if they are
  // not X registers. Note that mov(w0, w0) is not a no-op because it clears
  // the top word of x0.
  if (!rd.Is(rn) || !rd.Is64Bits()) {
    Assembler::mov(rd, rn);
  }
}


void MacroAssembler::Movk(const Register& rd, uint64_t imm, int shift) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  movk(rd, imm, shift);
}

void TurboAssembler::Mrs(const Register& rt, SystemRegister sysreg) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rt.IsZero());
  mrs(rt, sysreg);
}


void MacroAssembler::Msr(SystemRegister sysreg, const Register& rt) {
  DCHECK(allow_macro_instructions());
  msr(sysreg, rt);
}

void TurboAssembler::Msub(const Register& rd, const Register& rn,
                          const Register& rm, const Register& ra) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  msub(rd, rn, rm, ra);
}

void TurboAssembler::Mul(const Register& rd, const Register& rn,
                         const Register& rm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  mul(rd, rn, rm);
}

void TurboAssembler::Rbit(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  rbit(rd, rn);
}

void TurboAssembler::Ret(const Register& xn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!xn.IsZero());
  ret(xn);
  CheckVeneerPool(false, false);
}


void MacroAssembler::Rev(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  rev(rd, rn);
}

void TurboAssembler::Rev16(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  rev16(rd, rn);
}

void TurboAssembler::Rev32(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  rev32(rd, rn);
}

void TurboAssembler::Ror(const Register& rd, const Register& rs,
                         unsigned shift) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  ror(rd, rs, shift);
}

void TurboAssembler::Ror(const Register& rd, const Register& rn,
                         const Register& rm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  rorv(rd, rn, rm);
}


void MacroAssembler::Sbfiz(const Register& rd,
                           const Register& rn,
                           unsigned lsb,
                           unsigned width) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  sbfiz(rd, rn, lsb, width);
}

void TurboAssembler::Sbfx(const Register& rd, const Register& rn, unsigned lsb,
                          unsigned width) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  sbfx(rd, rn, lsb, width);
}

void TurboAssembler::Scvtf(const VRegister& fd, const Register& rn,
                           unsigned fbits) {
  DCHECK(allow_macro_instructions());
  scvtf(fd, rn, fbits);
}

void TurboAssembler::Sdiv(const Register& rd, const Register& rn,
                          const Register& rm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  sdiv(rd, rn, rm);
}


void MacroAssembler::Smaddl(const Register& rd,
                            const Register& rn,
                            const Register& rm,
                            const Register& ra) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  smaddl(rd, rn, rm, ra);
}


void MacroAssembler::Smsubl(const Register& rd,
                            const Register& rn,
                            const Register& rm,
                            const Register& ra) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  smsubl(rd, rn, rm, ra);
}

void TurboAssembler::Smull(const Register& rd, const Register& rn,
                           const Register& rm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  smull(rd, rn, rm);
}


void MacroAssembler::Smulh(const Register& rd,
                           const Register& rn,
                           const Register& rm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  smulh(rd, rn, rm);
}

void TurboAssembler::Umull(const Register& rd, const Register& rn,
                           const Register& rm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  umaddl(rd, rn, rm, xzr);
}

void TurboAssembler::Sxtb(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  sxtb(rd, rn);
}

void TurboAssembler::Sxth(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  sxth(rd, rn);
}

void TurboAssembler::Sxtw(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  sxtw(rd, rn);
}

void TurboAssembler::Ubfiz(const Register& rd, const Register& rn, unsigned lsb,
                           unsigned width) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  ubfiz(rd, rn, lsb, width);
}

void TurboAssembler::Ubfx(const Register& rd, const Register& rn, unsigned lsb,
                          unsigned width) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  ubfx(rd, rn, lsb, width);
}

void TurboAssembler::Ucvtf(const VRegister& fd, const Register& rn,
                           unsigned fbits) {
  DCHECK(allow_macro_instructions());
  ucvtf(fd, rn, fbits);
}

void TurboAssembler::Udiv(const Register& rd, const Register& rn,
                          const Register& rm) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  udiv(rd, rn, rm);
}


void MacroAssembler::Umaddl(const Register& rd,
                            const Register& rn,
                            const Register& rm,
                            const Register& ra) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  umaddl(rd, rn, rm, ra);
}


void MacroAssembler::Umsubl(const Register& rd,
                            const Register& rn,
                            const Register& rm,
                            const Register& ra) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  umsubl(rd, rn, rm, ra);
}

void TurboAssembler::Uxtb(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  uxtb(rd, rn);
}

void TurboAssembler::Uxth(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  uxth(rd, rn);
}

void TurboAssembler::Uxtw(const Register& rd, const Register& rn) {
  DCHECK(allow_macro_instructions());
  DCHECK(!rd.IsZero());
  uxtw(rd, rn);
}

void MacroAssembler::AlignAndSetCSPForFrame() {
  int sp_alignment = ActivationFrameAlignment();
  // AAPCS64 mandates at least 16-byte alignment.
  DCHECK(sp_alignment >= 16);
  DCHECK(base::bits::IsPowerOfTwo(sp_alignment));
  Bic(csp, StackPointer(), sp_alignment - 1);
  SetStackPointer(csp);
}

void TurboAssembler::BumpSystemStackPointer(const Operand& space) {
  DCHECK(!csp.Is(StackPointer()));
  if (!TmpList()->IsEmpty()) {
    Sub(csp, StackPointer(), space);
  } else {
    // TODO(jbramley): Several callers rely on this not using scratch
    // registers, so we use the assembler directly here. However, this means
    // that large immediate values of 'space' cannot be handled cleanly. (Only
    // 24-bits immediates or values of 'space' that can be encoded in one
    // instruction are accepted.) Once we implement our flexible scratch
    // register idea, we could greatly simplify this function.
    InstructionAccurateScope scope(this);
    DCHECK(space.IsImmediate());
    // Align to 16 bytes.
    uint64_t imm = RoundUp(space.ImmediateValue(), 0x10);
    DCHECK(is_uint24(imm));

    Register source = StackPointer();
    if (CpuFeatures::IsSupported(ALWAYS_ALIGN_CSP)) {
      bic(csp, source, 0xf);
      source = csp;
    }
    if (!is_uint12(imm)) {
      int64_t imm_top_12_bits = imm >> 12;
      sub(csp, source, imm_top_12_bits << 12);
      source = csp;
      imm -= imm_top_12_bits << 12;
    }
    if (imm > 0) {
      sub(csp, source, imm);
    }
  }
  AssertStackConsistency();
}

void TurboAssembler::SyncSystemStackPointer() {
  DCHECK(emit_debug_code());
  DCHECK(!csp.Is(StackPointer()));
  { InstructionAccurateScope scope(this);
    mov(csp, StackPointer());
  }
  AssertStackConsistency();
}

void TurboAssembler::InitializeRootRegister() {
  ExternalReference roots_array_start =
      ExternalReference::roots_array_start(isolate());
  Mov(root, Operand(roots_array_start));
}


void MacroAssembler::SmiTag(Register dst, Register src) {
  STATIC_ASSERT(kXRegSizeInBits ==
                static_cast<unsigned>(kSmiShift + kSmiValueSize));
  DCHECK(dst.Is64Bits() && src.Is64Bits());
  Lsl(dst, src, kSmiShift);
}


void MacroAssembler::SmiTag(Register smi) { SmiTag(smi, smi); }

void TurboAssembler::SmiUntag(Register dst, Register src) {
  STATIC_ASSERT(kXRegSizeInBits ==
                static_cast<unsigned>(kSmiShift + kSmiValueSize));
  DCHECK(dst.Is64Bits() && src.Is64Bits());
  if (FLAG_enable_slow_asserts) {
    AssertSmi(src);
  }
  Asr(dst, src, kSmiShift);
}

void TurboAssembler::SmiUntag(Register smi) { SmiUntag(smi, smi); }

void MacroAssembler::SmiUntagToDouble(VRegister dst, Register src,
                                      UntagMode mode) {
  DCHECK(dst.Is64Bits() && src.Is64Bits());
  if (FLAG_enable_slow_asserts && (mode == kNotSpeculativeUntag)) {
    AssertSmi(src);
  }
  Scvtf(dst, src, kSmiShift);
}

void MacroAssembler::SmiUntagToFloat(VRegister dst, Register src,
                                     UntagMode mode) {
  DCHECK(dst.Is32Bits() && src.Is64Bits());
  if (FLAG_enable_slow_asserts && (mode == kNotSpeculativeUntag)) {
    AssertSmi(src);
  }
  Scvtf(dst, src, kSmiShift);
}


void MacroAssembler::SmiTagAndPush(Register src) {
  STATIC_ASSERT((static_cast<unsigned>(kSmiShift) == kWRegSizeInBits) &&
                (static_cast<unsigned>(kSmiValueSize) == kWRegSizeInBits) &&
                (kSmiTag == 0));
  Push(src.W(), wzr);
}


void MacroAssembler::SmiTagAndPush(Register src1, Register src2) {
  STATIC_ASSERT((static_cast<unsigned>(kSmiShift) == kWRegSizeInBits) &&
                (static_cast<unsigned>(kSmiValueSize) == kWRegSizeInBits) &&
                (kSmiTag == 0));
  Push(src1.W(), wzr, src2.W(), wzr);
}

void TurboAssembler::JumpIfSmi(Register value, Label* smi_label,
                               Label* not_smi_label) {
  STATIC_ASSERT((kSmiTagSize == 1) && (kSmiTag == 0));
  // Check if the tag bit is set.
  if (smi_label) {
    Tbz(value, 0, smi_label);
    if (not_smi_label) {
      B(not_smi_label);
    }
  } else {
    DCHECK(not_smi_label);
    Tbnz(value, 0, not_smi_label);
  }
}


void MacroAssembler::JumpIfNotSmi(Register value, Label* not_smi_label) {
  JumpIfSmi(value, NULL, not_smi_label);
}


void MacroAssembler::JumpIfBothSmi(Register value1,
                                   Register value2,
                                   Label* both_smi_label,
                                   Label* not_smi_label) {
  STATIC_ASSERT((kSmiTagSize == 1) && (kSmiTag == 0));
  UseScratchRegisterScope temps(this);
  Register tmp = temps.AcquireX();
  // Check if both tag bits are clear.
  Orr(tmp, value1, value2);
  JumpIfSmi(tmp, both_smi_label, not_smi_label);
}


void MacroAssembler::JumpIfEitherSmi(Register value1,
                                     Register value2,
                                     Label* either_smi_label,
                                     Label* not_smi_label) {
  STATIC_ASSERT((kSmiTagSize == 1) && (kSmiTag == 0));
  UseScratchRegisterScope temps(this);
  Register tmp = temps.AcquireX();
  // Check if either tag bit is clear.
  And(tmp, value1, value2);
  JumpIfSmi(tmp, either_smi_label, not_smi_label);
}


void MacroAssembler::JumpIfEitherNotSmi(Register value1,
                                        Register value2,
                                        Label* not_smi_label) {
  JumpIfBothSmi(value1, value2, NULL, not_smi_label);
}


void MacroAssembler::JumpIfBothNotSmi(Register value1,
                                      Register value2,
                                      Label* not_smi_label) {
  JumpIfEitherSmi(value1, value2, NULL, not_smi_label);
}


void MacroAssembler::ObjectTag(Register tagged_obj, Register obj) {
  STATIC_ASSERT(kHeapObjectTag == 1);
  if (emit_debug_code()) {
    Label ok;
    Tbz(obj, 0, &ok);
    Abort(kObjectTagged);
    Bind(&ok);
  }
  Orr(tagged_obj, obj, kHeapObjectTag);
}


void MacroAssembler::ObjectUntag(Register untagged_obj, Register obj) {
  STATIC_ASSERT(kHeapObjectTag == 1);
  if (emit_debug_code()) {
    Label ok;
    Tbnz(obj, 0, &ok);
    Abort(kObjectNotTagged);
    Bind(&ok);
  }
  Bic(untagged_obj, obj, kHeapObjectTag);
}

void TurboAssembler::jmp(Label* L) { B(L); }

void TurboAssembler::Push(Handle<HeapObject> handle) {
  UseScratchRegisterScope temps(this);
  Register tmp = temps.AcquireX();
  Mov(tmp, Operand(handle));
  Push(tmp);
}

void TurboAssembler::Push(Smi* smi) {
  UseScratchRegisterScope temps(this);
  Register tmp = temps.AcquireX();
  Mov(tmp, Operand(smi));
  Push(tmp);
}

void TurboAssembler::Claim(int64_t count, uint64_t unit_size) {
  DCHECK(count >= 0);
  uint64_t size = count * unit_size;

  if (size == 0) {
    return;
  }

  if (csp.Is(StackPointer())) {
    DCHECK(size % 16 == 0);
  } else {
    BumpSystemStackPointer(size);
  }

  Sub(StackPointer(), StackPointer(), size);
}

void TurboAssembler::Claim(const Register& count, uint64_t unit_size) {
  if (unit_size == 0) return;
  DCHECK(base::bits::IsPowerOfTwo(unit_size));

  const int shift = CountTrailingZeros(unit_size, kXRegSizeInBits);
  const Operand size(count, LSL, shift);

  if (size.IsZero()) {
    return;
  }

  AssertPositiveOrZero(count);
  if (!csp.Is(StackPointer())) {
    BumpSystemStackPointer(size);
  }

  Sub(StackPointer(), StackPointer(), size);
}


void MacroAssembler::ClaimBySMI(const Register& count_smi, uint64_t unit_size) {
  DCHECK(unit_size == 0 || base::bits::IsPowerOfTwo(unit_size));
  const int shift = CountTrailingZeros(unit_size, kXRegSizeInBits) - kSmiShift;
  const Operand size(count_smi,
                     (shift >= 0) ? (LSL) : (LSR),
                     (shift >= 0) ? (shift) : (-shift));

  if (size.IsZero()) {
    return;
  }

  if (!csp.Is(StackPointer())) {
    BumpSystemStackPointer(size);
  }

  Sub(StackPointer(), StackPointer(), size);
}

void TurboAssembler::Drop(int64_t count, uint64_t unit_size) {
  DCHECK(count >= 0);
  uint64_t size = count * unit_size;

  if (size == 0) {
    return;
  }

  Add(StackPointer(), StackPointer(), size);

  if (csp.Is(StackPointer())) {
    DCHECK(size % 16 == 0);
  } else if (emit_debug_code()) {
    // It is safe to leave csp where it is when unwinding the JavaScript stack,
    // but if we keep it matching StackPointer, the simulator can detect memory
    // accesses in the now-free part of the stack.
    SyncSystemStackPointer();
  }
}

void TurboAssembler::Drop(const Register& count, uint64_t unit_size) {
  if (unit_size == 0) return;
  DCHECK(base::bits::IsPowerOfTwo(unit_size));

  const int shift = CountTrailingZeros(unit_size, kXRegSizeInBits);
  const Operand size(count, LSL, shift);

  if (size.IsZero()) {
    return;
  }

  AssertPositiveOrZero(count);
  Add(StackPointer(), StackPointer(), size);

  if (!csp.Is(StackPointer()) && emit_debug_code()) {
    // It is safe to leave csp where it is when unwinding the JavaScript stack,
    // but if we keep it matching StackPointer, the simulator can detect memory
    // accesses in the now-free part of the stack.
    SyncSystemStackPointer();
  }
}


void MacroAssembler::DropBySMI(const Register& count_smi, uint64_t unit_size) {
  DCHECK(unit_size == 0 || base::bits::IsPowerOfTwo(unit_size));
  const int shift = CountTrailingZeros(unit_size, kXRegSizeInBits) - kSmiShift;
  const Operand size(count_smi,
                     (shift >= 0) ? (LSL) : (LSR),
                     (shift >= 0) ? (shift) : (-shift));

  if (size.IsZero()) {
    return;
  }

  Add(StackPointer(), StackPointer(), size);

  if (!csp.Is(StackPointer()) && emit_debug_code()) {
    // It is safe to leave csp where it is when unwinding the JavaScript stack,
    // but if we keep it matching StackPointer, the simulator can detect memory
    // accesses in the now-free part of the stack.
    SyncSystemStackPointer();
  }
}


void MacroAssembler::CompareAndBranch(const Register& lhs,
                                      const Operand& rhs,
                                      Condition cond,
                                      Label* label) {
  if (rhs.IsImmediate() && (rhs.ImmediateValue() == 0) &&
      ((cond == eq) || (cond == ne))) {
    if (cond == eq) {
      Cbz(lhs, label);
    } else {
      Cbnz(lhs, label);
    }
  } else {
    Cmp(lhs, rhs);
    B(cond, label);
  }
}

void TurboAssembler::TestAndBranchIfAnySet(const Register& reg,
                                           const uint64_t bit_pattern,
                                           Label* label) {
  int bits = reg.SizeInBits();
  DCHECK(CountSetBits(bit_pattern, bits) > 0);
  if (CountSetBits(bit_pattern, bits) == 1) {
    Tbnz(reg, MaskToBit(bit_pattern), label);
  } else {
    Tst(reg, bit_pattern);
    B(ne, label);
  }
}

void TurboAssembler::TestAndBranchIfAllClear(const Register& reg,
                                             const uint64_t bit_pattern,
                                             Label* label) {
  int bits = reg.SizeInBits();
  DCHECK(CountSetBits(bit_pattern, bits) > 0);
  if (CountSetBits(bit_pattern, bits) == 1) {
    Tbz(reg, MaskToBit(bit_pattern), label);
  } else {
    Tst(reg, bit_pattern);
    B(eq, label);
  }
}


void MacroAssembler::InlineData(uint64_t data) {
  DCHECK(is_uint16(data));
  InstructionAccurateScope scope(this, 1);
  movz(xzr, data);
}


void MacroAssembler::EnableInstrumentation() {
  InstructionAccurateScope scope(this, 1);
  movn(xzr, InstrumentStateEnable);
}


void MacroAssembler::DisableInstrumentation() {
  InstructionAccurateScope scope(this, 1);
  movn(xzr, InstrumentStateDisable);
}


void MacroAssembler::AnnotateInstrumentation(const char* marker_name) {
  DCHECK(strlen(marker_name) == 2);

  // We allow only printable characters in the marker names. Unprintable
  // characters are reserved for controlling features of the instrumentation.
  DCHECK(isprint(marker_name[0]) && isprint(marker_name[1]));

  InstructionAccurateScope scope(this, 1);
  movn(xzr, (marker_name[1] << 8) | marker_name[0]);
}

}  // namespace internal
}  // namespace v8

#endif  // V8_ARM64_MACRO_ASSEMBLER_ARM64_INL_H_
