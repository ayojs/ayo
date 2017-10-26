// Copyright 2012 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <iostream>  // NOLINT(readability/streams)

#include "src/v8.h"

#include "src/base/utils/random-number-generator.h"
#include "src/disassembler.h"
#include "src/factory.h"
#include "src/macro-assembler.h"
#include "src/mips/macro-assembler-mips.h"
#include "src/mips/simulator-mips.h"

#include "test/cctest/cctest.h"


using namespace v8::internal;


// Define these function prototypes to match JSEntryFunction in execution.cc.
typedef Object* (*F1)(int x, int p1, int p2, int p3, int p4);
typedef Object* (*F2)(int x, int y, int p2, int p3, int p4);
typedef Object* (*F3)(void* p, int p1, int p2, int p3, int p4);
typedef Object* (*F4)(void* p0, void* p1, int p2, int p3, int p4);

#define __ assm.

TEST(MIPS0) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  // Addition.
  __ addu(v0, a0, a1);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F2 f = FUNCTION_CAST<F2>(code->entry());
  int res = reinterpret_cast<int>(
      CALL_GENERATED_CODE(isolate, f, 0xab0, 0xc, 0, 0, 0));
  CHECK_EQ(static_cast<int32_t>(0xabc), res);
}


TEST(MIPS1) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  Label L, C;

  __ mov(a1, a0);
  __ li(v0, 0);
  __ b(&C);
  __ nop();

  __ bind(&L);
  __ addu(v0, v0, a1);
  __ addiu(a1, a1, -1);

  __ bind(&C);
  __ xori(v1, a1, 0);
  __ Branch(&L, ne, v1, Operand(0));
  __ nop();

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F1 f = FUNCTION_CAST<F1>(code->entry());
  int res = reinterpret_cast<int>(
      CALL_GENERATED_CODE(isolate, f, 50, 0, 0, 0, 0));
  CHECK_EQ(1275, res);
}


TEST(MIPS2) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  Label exit, error;

  // ----- Test all instructions.

  // Test lui, ori, and addiu, used in the li pseudo-instruction.
  // This way we can then safely load registers with chosen values.

  __ ori(t0, zero_reg, 0);
  __ lui(t0, 0x1234);
  __ ori(t0, t0, 0);
  __ ori(t0, t0, 0x0f0f);
  __ ori(t0, t0, 0xf0f0);
  __ addiu(t1, t0, 1);
  __ addiu(t2, t1, -0x10);

  // Load values in temporary registers.
  __ li(t0, 0x00000004);
  __ li(t1, 0x00001234);
  __ li(t2, 0x12345678);
  __ li(t3, 0x7fffffff);
  __ li(t4, 0xfffffffc);
  __ li(t5, 0xffffedcc);
  __ li(t6, 0xedcba988);
  __ li(t7, 0x80000000);

  // SPECIAL class.
  __ srl(v0, t2, 8);    // 0x00123456
  __ sll(v0, v0, 11);   // 0x91a2b000
  __ sra(v0, v0, 3);    // 0xf2345600
  __ srav(v0, v0, t0);  // 0xff234560
  __ sllv(v0, v0, t0);  // 0xf2345600
  __ srlv(v0, v0, t0);  // 0x0f234560
  __ Branch(&error, ne, v0, Operand(0x0f234560));
  __ nop();

  __ addu(v0, t0, t1);   // 0x00001238
  __ subu(v0, v0, t0);  // 0x00001234
  __ Branch(&error, ne, v0, Operand(0x00001234));
  __ nop();
  __ addu(v1, t3, t0);
  __ Branch(&error, ne, v1, Operand(0x80000003));
  __ nop();
  __ subu(v1, t7, t0);  // 0x7ffffffc
  __ Branch(&error, ne, v1, Operand(0x7ffffffc));
  __ nop();

  __ and_(v0, t1, t2);  // 0x00001230
  __ or_(v0, v0, t1);   // 0x00001234
  __ xor_(v0, v0, t2);  // 0x1234444c
  __ nor(v0, v0, t2);   // 0xedcba987
  __ Branch(&error, ne, v0, Operand(0xedcba983));
  __ nop();

  __ slt(v0, t7, t3);
  __ Branch(&error, ne, v0, Operand(0x1));
  __ nop();
  __ sltu(v0, t7, t3);
  __ Branch(&error, ne, v0, Operand(zero_reg));
  __ nop();
  // End of SPECIAL class.

  __ addiu(v0, zero_reg, 0x7421);  // 0x00007421
  __ addiu(v0, v0, -0x1);  // 0x00007420
  __ addiu(v0, v0, -0x20);  // 0x00007400
  __ Branch(&error, ne, v0, Operand(0x00007400));
  __ nop();
  __ addiu(v1, t3, 0x1);  // 0x80000000
  __ Branch(&error, ne, v1, Operand(0x80000000));
  __ nop();

  __ slti(v0, t1, 0x00002000);  // 0x1
  __ slti(v0, v0, 0xffff8000);  // 0x0
  __ Branch(&error, ne, v0, Operand(zero_reg));
  __ nop();
  __ sltiu(v0, t1, 0x00002000);  // 0x1
  __ sltiu(v0, v0, 0x00008000);  // 0x1
  __ Branch(&error, ne, v0, Operand(0x1));
  __ nop();

  __ andi(v0, t1, 0xf0f0);  // 0x00001030
  __ ori(v0, v0, 0x8a00);  // 0x00009a30
  __ xori(v0, v0, 0x83cc);  // 0x000019fc
  __ Branch(&error, ne, v0, Operand(0x000019fc));
  __ nop();
  __ lui(v1, 0x8123);  // 0x81230000
  __ Branch(&error, ne, v1, Operand(0x81230000));
  __ nop();

  // Bit twiddling instructions & conditional moves.
  // Uses t0-t7 as set above.
  __ Clz(v0, t0);       // 29
  __ Clz(v1, t1);       // 19
  __ addu(v0, v0, v1);  // 48
  __ Clz(v1, t2);       // 3
  __ addu(v0, v0, v1);  // 51
  __ Clz(v1, t7);       // 0
  __ addu(v0, v0, v1);  // 51
  __ Branch(&error, ne, v0, Operand(51));
  __ Movn(a0, t3, t0);  // Move a0<-t3 (t0 is NOT 0).
  __ Ins(a0, t1, 12, 8);  // 0x7ff34fff
  __ Branch(&error, ne, a0, Operand(0x7ff34fff));
  __ Movz(a0, t6, t7);    // a0 not updated (t7 is NOT 0).
  __ Ext(a1, a0, 8, 12);  // 0x34f
  __ Branch(&error, ne, a1, Operand(0x34f));
  __ Movz(a0, t6, v1);    // a0<-t6, v0 is 0, from 8 instr back.
  __ Branch(&error, ne, a0, Operand(t6));

  // Everything was correctly executed. Load the expected result.
  __ li(v0, 0x31415926);
  __ b(&exit);
  __ nop();

  __ bind(&error);
  // Got an error. Return a wrong result.
  __ li(v0, 666);

  __ bind(&exit);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F2 f = FUNCTION_CAST<F2>(code->entry());
  int res = reinterpret_cast<int>(
      CALL_GENERATED_CODE(isolate, f, 0xab0, 0xc, 0, 0, 0));
  CHECK_EQ(static_cast<int32_t>(0x31415926), res);
}


TEST(MIPS3) {
  // Test floating point instructions.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
    double g;
    double h;
    double i;
    float fa;
    float fb;
    float fc;
    float fd;
    float fe;
    float ff;
    float fg;
  } T;
  T t;

  // Create a function that accepts &t, and loads, manipulates, and stores
  // the doubles t.a ... t.f.
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  Label L, C;

  // Double precision floating point instructions.
  __ Ldc1(f4, MemOperand(a0, offsetof(T, a)));
  __ Ldc1(f6, MemOperand(a0, offsetof(T, b)));
  __ add_d(f8, f4, f6);
  __ Sdc1(f8, MemOperand(a0, offsetof(T, c)));  // c = a + b.

  __ mov_d(f10, f8);  // c
  __ neg_d(f12, f6);  // -b
  __ sub_d(f10, f10, f12);
  __ Sdc1(f10, MemOperand(a0, offsetof(T, d)));  // d = c - (-b).

  __ Sdc1(f4, MemOperand(a0, offsetof(T, b)));  // b = a.

  __ li(t0, 120);
  __ mtc1(t0, f14);
  __ cvt_d_w(f14, f14);   // f14 = 120.0.
  __ mul_d(f10, f10, f14);
  __ Sdc1(f10, MemOperand(a0, offsetof(T, e)));  // e = d * 120 = 1.8066e16.

  __ div_d(f12, f10, f4);
  __ Sdc1(f12, MemOperand(a0, offsetof(T, f)));  // f = e / a = 120.44.

  __ sqrt_d(f14, f12);
  __ Sdc1(f14, MemOperand(a0, offsetof(T, g)));
  // g = sqrt(f) = 10.97451593465515908537

  if (IsMipsArchVariant(kMips32r2)) {
    __ Ldc1(f4, MemOperand(a0, offsetof(T, h)));
    __ Ldc1(f6, MemOperand(a0, offsetof(T, i)));
    __ madd_d(f14, f6, f4, f6);
    __ Sdc1(f14, MemOperand(a0, offsetof(T, h)));
  }

  // Single precision floating point instructions.
  __ lwc1(f4, MemOperand(a0, offsetof(T, fa)) );
  __ lwc1(f6, MemOperand(a0, offsetof(T, fb)) );
  __ add_s(f8, f4, f6);
  __ swc1(f8, MemOperand(a0, offsetof(T, fc)) );  // fc = fa + fb.

  __ neg_s(f10, f6);  // -fb
  __ sub_s(f10, f8, f10);
  __ swc1(f10, MemOperand(a0, offsetof(T, fd)) );  // fd = fc - (-fb).

  __ swc1(f4, MemOperand(a0, offsetof(T, fb)) );   // fb = fa.

  __ li(t0, 120);
  __ mtc1(t0, f14);
  __ cvt_s_w(f14, f14);   // f14 = 120.0.
  __ mul_s(f10, f10, f14);
  __ swc1(f10, MemOperand(a0, offsetof(T, fe)) );  // fe = fd * 120

  __ div_s(f12, f10, f4);
  __ swc1(f12, MemOperand(a0, offsetof(T, ff)) );  // ff = fe / fa

  __ sqrt_s(f14, f12);
  __ swc1(f14, MemOperand(a0, offsetof(T, fg)) );

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  // Double test values.
  t.a = 1.5e14;
  t.b = 2.75e11;
  t.c = 0.0;
  t.d = 0.0;
  t.e = 0.0;
  t.f = 0.0;
  t.h = 1.5;
  t.i = 2.75;
  // Single test values.
  t.fa = 1.5e6;
  t.fb = 2.75e4;
  t.fc = 0.0;
  t.fd = 0.0;
  t.fe = 0.0;
  t.ff = 0.0;
  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
  USE(dummy);
  // Expected double results.
  CHECK_EQ(1.5e14, t.a);
  CHECK_EQ(1.5e14, t.b);
  CHECK_EQ(1.50275e14, t.c);
  CHECK_EQ(1.50550e14, t.d);
  CHECK_EQ(1.8066e16, t.e);
  CHECK_EQ(120.44, t.f);
  CHECK_EQ(10.97451593465515908537, t.g);
  if (IsMipsArchVariant(kMips32r2)) {
    CHECK_EQ(6.875, t.h);
  }
  // Expected single results.
  CHECK_EQ(1.5e6, t.fa);
  CHECK_EQ(1.5e6, t.fb);
  CHECK_EQ(1.5275e06, t.fc);
  CHECK_EQ(1.5550e06, t.fd);
  CHECK_EQ(1.866e08, t.fe);
  CHECK_EQ(124.40000152587890625, t.ff);
  CHECK_EQ(11.1534748077392578125, t.fg);
}


TEST(MIPS4) {
  // Exchange between GP anf FP registers is done through memory
  // on FPXX compiled binaries and architectures that do not support
  // MTHC1 and MTFC1. If this is the case, skipping this test.
  if (IsFpxxMode() &&
      (IsMipsArchVariant(kMips32r1) || IsMipsArchVariant(kLoongson))) {
    return;
  }

  // Test moves between floating point and integer registers.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
    double a;
    double b;
    double c;
  } T;
  T t;

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  Label L, C;

  __ Ldc1(f4, MemOperand(a0, offsetof(T, a)));
  __ Ldc1(f6, MemOperand(a0, offsetof(T, b)));

  // Swap f4 and f6, by using four integer registers, t0-t3.
  if (IsFp32Mode()) {
    __ mfc1(t0, f4);
    __ mfc1(t1, f5);
    __ mfc1(t2, f6);
    __ mfc1(t3, f7);

    __ mtc1(t0, f6);
    __ mtc1(t1, f7);
    __ mtc1(t2, f4);
    __ mtc1(t3, f5);
  } else {
    CHECK(!IsMipsArchVariant(kMips32r1) && !IsMipsArchVariant(kLoongson));
    DCHECK(IsFp64Mode() || IsFpxxMode());
    __ mfc1(t0, f4);
    __ mfhc1(t1, f4);
    __ mfc1(t2, f6);
    __ mfhc1(t3, f6);

    __ mtc1(t0, f6);
    __ mthc1(t1, f6);
    __ mtc1(t2, f4);
    __ mthc1(t3, f4);
  }

  // Store the swapped f4 and f5 back to memory.
  __ Sdc1(f4, MemOperand(a0, offsetof(T, a)));
  __ Sdc1(f6, MemOperand(a0, offsetof(T, c)));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  t.a = 1.5e22;
  t.b = 2.75e11;
  t.c = 17.17;
  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
  USE(dummy);

  CHECK_EQ(2.75e11, t.a);
  CHECK_EQ(2.75e11, t.b);
  CHECK_EQ(1.5e22, t.c);
}


TEST(MIPS5) {
  // Test conversions between doubles and integers.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
    double a;
    double b;
    int i;
    int j;
  } T;
  T t;

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  Label L, C;

  // Load all structure elements to registers.
  __ Ldc1(f4, MemOperand(a0, offsetof(T, a)));
  __ Ldc1(f6, MemOperand(a0, offsetof(T, b)));
  __ lw(t0, MemOperand(a0, offsetof(T, i)) );
  __ lw(t1, MemOperand(a0, offsetof(T, j)) );

  // Convert double in f4 to int in element i.
  __ cvt_w_d(f8, f4);
  __ mfc1(t2, f8);
  __ sw(t2, MemOperand(a0, offsetof(T, i)) );

  // Convert double in f6 to int in element j.
  __ cvt_w_d(f10, f6);
  __ mfc1(t3, f10);
  __ sw(t3, MemOperand(a0, offsetof(T, j)) );

  // Convert int in original i (t0) to double in a.
  __ mtc1(t0, f12);
  __ cvt_d_w(f0, f12);
  __ Sdc1(f0, MemOperand(a0, offsetof(T, a)));

  // Convert int in original j (t1) to double in b.
  __ mtc1(t1, f14);
  __ cvt_d_w(f2, f14);
  __ Sdc1(f2, MemOperand(a0, offsetof(T, b)));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  t.a = 1.5e4;
  t.b = 2.75e8;
  t.i = 12345678;
  t.j = -100000;
  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
  USE(dummy);

  CHECK_EQ(12345678.0, t.a);
  CHECK_EQ(-100000.0, t.b);
  CHECK_EQ(15000, t.i);
  CHECK_EQ(275000000, t.j);
}


TEST(MIPS6) {
  // Test simple memory loads and stores.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
    uint32_t ui;
    int32_t si;
    int32_t r1;
    int32_t r2;
    int32_t r3;
    int32_t r4;
    int32_t r5;
    int32_t r6;
  } T;
  T t;

  Assembler assm(isolate, NULL, 0);
  Label L, C;

  // Basic word load/store.
  __ lw(t0, MemOperand(a0, offsetof(T, ui)) );
  __ sw(t0, MemOperand(a0, offsetof(T, r1)) );

  // lh with positive data.
  __ lh(t1, MemOperand(a0, offsetof(T, ui)) );
  __ sw(t1, MemOperand(a0, offsetof(T, r2)) );

  // lh with negative data.
  __ lh(t2, MemOperand(a0, offsetof(T, si)) );
  __ sw(t2, MemOperand(a0, offsetof(T, r3)) );

  // lhu with negative data.
  __ lhu(t3, MemOperand(a0, offsetof(T, si)) );
  __ sw(t3, MemOperand(a0, offsetof(T, r4)) );

  // lb with negative data.
  __ lb(t4, MemOperand(a0, offsetof(T, si)) );
  __ sw(t4, MemOperand(a0, offsetof(T, r5)) );

  // sh writes only 1/2 of word.
  __ lui(t5, 0x3333);
  __ ori(t5, t5, 0x3333);
  __ sw(t5, MemOperand(a0, offsetof(T, r6)) );
  __ lhu(t5, MemOperand(a0, offsetof(T, si)) );
  __ sh(t5, MemOperand(a0, offsetof(T, r6)) );

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  t.ui = 0x11223344;
  t.si = 0x99aabbcc;
  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
  USE(dummy);

  CHECK_EQ(static_cast<int32_t>(0x11223344), t.r1);
#if __BYTE_ORDER == __LITTLE_ENDIAN
  CHECK_EQ(static_cast<int32_t>(0x3344), t.r2);
  CHECK_EQ(static_cast<int32_t>(0xffffbbcc), t.r3);
  CHECK_EQ(static_cast<int32_t>(0x0000bbcc), t.r4);
  CHECK_EQ(static_cast<int32_t>(0xffffffcc), t.r5);
  CHECK_EQ(static_cast<int32_t>(0x3333bbcc), t.r6);
#elif __BYTE_ORDER == __BIG_ENDIAN
  CHECK_EQ(static_cast<int32_t>(0x1122), t.r2);
  CHECK_EQ(static_cast<int32_t>(0xffff99aa), t.r3);
  CHECK_EQ(static_cast<int32_t>(0x000099aa), t.r4);
  CHECK_EQ(static_cast<int32_t>(0xffffff99), t.r5);
  CHECK_EQ(static_cast<int32_t>(0x99aa3333), t.r6);
#else
#error Unknown endianness
#endif
}


TEST(MIPS7) {
  // Test floating point compare and branch instructions.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;
    int32_t result;
  } T;
  T t;

  // Create a function that accepts &t, and loads, manipulates, and stores
  // the doubles t.a ... t.f.
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  Label neither_is_nan, less_than, outa_here;

  __ Ldc1(f4, MemOperand(a0, offsetof(T, a)));
  __ Ldc1(f6, MemOperand(a0, offsetof(T, b)));
  if (!IsMipsArchVariant(kMips32r6)) {
  __ c(UN, D, f4, f6);
  __ bc1f(&neither_is_nan);
  } else {
    __ cmp(UN, L, f2, f4, f6);
    __ bc1eqz(&neither_is_nan, f2);
  }
  __ nop();
  __ sw(zero_reg, MemOperand(a0, offsetof(T, result)) );
  __ Branch(&outa_here);

  __ bind(&neither_is_nan);

  if (IsMipsArchVariant(kLoongson)) {
    __ c(OLT, D, f6, f4);
    __ bc1t(&less_than);
  } else if (IsMipsArchVariant(kMips32r6)) {
    __ cmp(OLT, L, f2, f6, f4);
    __ bc1nez(&less_than, f2);
  } else {
    __ c(OLT, D, f6, f4, 2);
    __ bc1t(&less_than, 2);
  }

  __ nop();
  __ sw(zero_reg, MemOperand(a0, offsetof(T, result)) );
  __ Branch(&outa_here);

  __ bind(&less_than);
  __ Addu(t0, zero_reg, Operand(1));
  __ sw(t0, MemOperand(a0, offsetof(T, result)) );  // Set true.


  // This test-case should have additional tests.

  __ bind(&outa_here);

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  t.a = 1.5e14;
  t.b = 2.75e11;
  t.c = 2.0;
  t.d = -4.0;
  t.e = 0.0;
  t.f = 0.0;
  t.result = 0;
  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
  USE(dummy);
  CHECK_EQ(1.5e14, t.a);
  CHECK_EQ(2.75e11, t.b);
  CHECK_EQ(1, t.result);
}


TEST(MIPS8) {
  // Test ROTR and ROTRV instructions.
  if (IsMipsArchVariant(kMips32r2)) {
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);

    typedef struct {
      int32_t input;
      int32_t result_rotr_4;
      int32_t result_rotr_8;
      int32_t result_rotr_12;
      int32_t result_rotr_16;
      int32_t result_rotr_20;
      int32_t result_rotr_24;
      int32_t result_rotr_28;
      int32_t result_rotrv_4;
      int32_t result_rotrv_8;
      int32_t result_rotrv_12;
      int32_t result_rotrv_16;
      int32_t result_rotrv_20;
      int32_t result_rotrv_24;
      int32_t result_rotrv_28;
    } T;
    T t;

    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);

    // Basic word load.
    __ lw(t0, MemOperand(a0, offsetof(T, input)) );

    // ROTR instruction (called through the Ror macro).
    __ Ror(t1, t0, 0x0004);
    __ Ror(t2, t0, 0x0008);
    __ Ror(t3, t0, 0x000c);
    __ Ror(t4, t0, 0x0010);
    __ Ror(t5, t0, 0x0014);
    __ Ror(t6, t0, 0x0018);
    __ Ror(t7, t0, 0x001c);

    // Basic word store.
    __ sw(t1, MemOperand(a0, offsetof(T, result_rotr_4)) );
    __ sw(t2, MemOperand(a0, offsetof(T, result_rotr_8)) );
    __ sw(t3, MemOperand(a0, offsetof(T, result_rotr_12)) );
    __ sw(t4, MemOperand(a0, offsetof(T, result_rotr_16)) );
    __ sw(t5, MemOperand(a0, offsetof(T, result_rotr_20)) );
    __ sw(t6, MemOperand(a0, offsetof(T, result_rotr_24)) );
    __ sw(t7, MemOperand(a0, offsetof(T, result_rotr_28)) );

    // ROTRV instruction (called through the Ror macro).
    __ li(t7, 0x0004);
    __ Ror(t1, t0, t7);
    __ li(t7, 0x0008);
    __ Ror(t2, t0, t7);
    __ li(t7, 0x000C);
    __ Ror(t3, t0, t7);
    __ li(t7, 0x0010);
    __ Ror(t4, t0, t7);
    __ li(t7, 0x0014);
    __ Ror(t5, t0, t7);
    __ li(t7, 0x0018);
    __ Ror(t6, t0, t7);
    __ li(t7, 0x001C);
    __ Ror(t7, t0, t7);

    // Basic word store.
    __ sw(t1, MemOperand(a0, offsetof(T, result_rotrv_4)) );
    __ sw(t2, MemOperand(a0, offsetof(T, result_rotrv_8)) );
    __ sw(t3, MemOperand(a0, offsetof(T, result_rotrv_12)) );
    __ sw(t4, MemOperand(a0, offsetof(T, result_rotrv_16)) );
    __ sw(t5, MemOperand(a0, offsetof(T, result_rotrv_20)) );
    __ sw(t6, MemOperand(a0, offsetof(T, result_rotrv_24)) );
    __ sw(t7, MemOperand(a0, offsetof(T, result_rotrv_28)) );

    __ jr(ra);
    __ nop();

    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());
    t.input = 0x12345678;
    Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0x0, 0, 0, 0);
    USE(dummy);
    CHECK_EQ(static_cast<int32_t>(0x81234567), t.result_rotr_4);
    CHECK_EQ(static_cast<int32_t>(0x78123456), t.result_rotr_8);
    CHECK_EQ(static_cast<int32_t>(0x67812345), t.result_rotr_12);
    CHECK_EQ(static_cast<int32_t>(0x56781234), t.result_rotr_16);
    CHECK_EQ(static_cast<int32_t>(0x45678123), t.result_rotr_20);
    CHECK_EQ(static_cast<int32_t>(0x34567812), t.result_rotr_24);
    CHECK_EQ(static_cast<int32_t>(0x23456781), t.result_rotr_28);

    CHECK_EQ(static_cast<int32_t>(0x81234567), t.result_rotrv_4);
    CHECK_EQ(static_cast<int32_t>(0x78123456), t.result_rotrv_8);
    CHECK_EQ(static_cast<int32_t>(0x67812345), t.result_rotrv_12);
    CHECK_EQ(static_cast<int32_t>(0x56781234), t.result_rotrv_16);
    CHECK_EQ(static_cast<int32_t>(0x45678123), t.result_rotrv_20);
    CHECK_EQ(static_cast<int32_t>(0x34567812), t.result_rotrv_24);
    CHECK_EQ(static_cast<int32_t>(0x23456781), t.result_rotrv_28);
  }
}


TEST(MIPS9) {
  // Test BRANCH improvements.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  Label exit, exit2, exit3;

  __ Branch(&exit, ge, a0, Operand(zero_reg));
  __ Branch(&exit2, ge, a0, Operand(0x00001FFF));
  __ Branch(&exit3, ge, a0, Operand(0x0001FFFF));

  __ bind(&exit);
  __ bind(&exit2);
  __ bind(&exit3);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
}


TEST(MIPS10) {
  // Test conversions between doubles and words.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
    double a;
    double b;
    int32_t dbl_mant;
    int32_t dbl_exp;
    int32_t word;
    int32_t b_word;
  } T;
  T t;

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  Label L, C;

  if (IsMipsArchVariant(kMips32r1) || IsMipsArchVariant(kLoongson)) return;

  // Load all structure elements to registers.
  // (f0, f1) = a (fp32), f0 = a (fp64)
  __ Ldc1(f0, MemOperand(a0, offsetof(T, a)));

  __ mfc1(t0, f0);   // t0 = f0(31..0)
  __ mfhc1(t1, f0);  // t1 = sign_extend(f0(63..32))
  __ sw(t0, MemOperand(a0, offsetof(T, dbl_mant)));  // dbl_mant = t0
  __ sw(t1, MemOperand(a0, offsetof(T, dbl_exp)));   // dbl_exp = t1

  // Convert double in f0 to word, save hi/lo parts.
  __ cvt_w_d(f0, f0);  // a_word = (word)a
  __ mfc1(t0, f0);  // f0 has a 32-bits word. t0 = a_word
  __ sw(t0, MemOperand(a0, offsetof(T, word)));  // word = a_word

  // Convert the b word to double b.
  __ lw(t0, MemOperand(a0, offsetof(T, b_word)));
  __ mtc1(t0, f8);  // f8 has a 32-bits word.
  __ cvt_d_w(f10, f8);
  __ Sdc1(f10, MemOperand(a0, offsetof(T, b)));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  t.a = 2.147483646e+09;       // 0x7FFFFFFE -> 0xFF80000041DFFFFF as double.
  t.b_word = 0x0ff00ff0;       // 0x0FF00FF0 -> 0x as double.
  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
  USE(dummy);
  CHECK_EQ(static_cast<int32_t>(0x41DFFFFF), t.dbl_exp);
  CHECK_EQ(static_cast<int32_t>(0xFF800000), t.dbl_mant);
  CHECK_EQ(static_cast<int32_t>(0x7FFFFFFE), t.word);
  // 0x0FF00FF0 -> 2.6739096+e08
  CHECK_EQ(2.6739096e08, t.b);
}


TEST(MIPS11) {
  // Do not run test on MIPS32r6, as these instructions are removed.
  if (IsMipsArchVariant(kMips32r6)) return;
  // Test LWL, LWR, SWL and SWR instructions.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
    int32_t reg_init;
    int32_t mem_init;
    int32_t lwl_0;
    int32_t lwl_1;
    int32_t lwl_2;
    int32_t lwl_3;
    int32_t lwr_0;
    int32_t lwr_1;
    int32_t lwr_2;
    int32_t lwr_3;
    int32_t swl_0;
    int32_t swl_1;
    int32_t swl_2;
    int32_t swl_3;
    int32_t swr_0;
    int32_t swr_1;
    int32_t swr_2;
    int32_t swr_3;
  } T;
  T t;

  Assembler assm(isolate, NULL, 0);

  // Test all combinations of LWL and vAddr.
  __ lw(t0, MemOperand(a0, offsetof(T, reg_init)) );
  __ lwl(t0, MemOperand(a0, offsetof(T, mem_init)) );
  __ sw(t0, MemOperand(a0, offsetof(T, lwl_0)) );

  __ lw(t1, MemOperand(a0, offsetof(T, reg_init)) );
  __ lwl(t1, MemOperand(a0, offsetof(T, mem_init) + 1) );
  __ sw(t1, MemOperand(a0, offsetof(T, lwl_1)) );

  __ lw(t2, MemOperand(a0, offsetof(T, reg_init)) );
  __ lwl(t2, MemOperand(a0, offsetof(T, mem_init) + 2) );
  __ sw(t2, MemOperand(a0, offsetof(T, lwl_2)) );

  __ lw(t3, MemOperand(a0, offsetof(T, reg_init)) );
  __ lwl(t3, MemOperand(a0, offsetof(T, mem_init) + 3) );
  __ sw(t3, MemOperand(a0, offsetof(T, lwl_3)) );

  // Test all combinations of LWR and vAddr.
  __ lw(t0, MemOperand(a0, offsetof(T, reg_init)) );
  __ lwr(t0, MemOperand(a0, offsetof(T, mem_init)) );
  __ sw(t0, MemOperand(a0, offsetof(T, lwr_0)) );

  __ lw(t1, MemOperand(a0, offsetof(T, reg_init)) );
  __ lwr(t1, MemOperand(a0, offsetof(T, mem_init) + 1) );
  __ sw(t1, MemOperand(a0, offsetof(T, lwr_1)) );

  __ lw(t2, MemOperand(a0, offsetof(T, reg_init)) );
  __ lwr(t2, MemOperand(a0, offsetof(T, mem_init) + 2) );
  __ sw(t2, MemOperand(a0, offsetof(T, lwr_2)) );

  __ lw(t3, MemOperand(a0, offsetof(T, reg_init)) );
  __ lwr(t3, MemOperand(a0, offsetof(T, mem_init) + 3) );
  __ sw(t3, MemOperand(a0, offsetof(T, lwr_3)) );

  // Test all combinations of SWL and vAddr.
  __ lw(t0, MemOperand(a0, offsetof(T, mem_init)) );
  __ sw(t0, MemOperand(a0, offsetof(T, swl_0)) );
  __ lw(t0, MemOperand(a0, offsetof(T, reg_init)) );
  __ swl(t0, MemOperand(a0, offsetof(T, swl_0)) );

  __ lw(t1, MemOperand(a0, offsetof(T, mem_init)) );
  __ sw(t1, MemOperand(a0, offsetof(T, swl_1)) );
  __ lw(t1, MemOperand(a0, offsetof(T, reg_init)) );
  __ swl(t1, MemOperand(a0, offsetof(T, swl_1) + 1) );

  __ lw(t2, MemOperand(a0, offsetof(T, mem_init)) );
  __ sw(t2, MemOperand(a0, offsetof(T, swl_2)) );
  __ lw(t2, MemOperand(a0, offsetof(T, reg_init)) );
  __ swl(t2, MemOperand(a0, offsetof(T, swl_2) + 2) );

  __ lw(t3, MemOperand(a0, offsetof(T, mem_init)) );
  __ sw(t3, MemOperand(a0, offsetof(T, swl_3)) );
  __ lw(t3, MemOperand(a0, offsetof(T, reg_init)) );
  __ swl(t3, MemOperand(a0, offsetof(T, swl_3) + 3) );

  // Test all combinations of SWR and vAddr.
  __ lw(t0, MemOperand(a0, offsetof(T, mem_init)) );
  __ sw(t0, MemOperand(a0, offsetof(T, swr_0)) );
  __ lw(t0, MemOperand(a0, offsetof(T, reg_init)) );
  __ swr(t0, MemOperand(a0, offsetof(T, swr_0)) );

  __ lw(t1, MemOperand(a0, offsetof(T, mem_init)) );
  __ sw(t1, MemOperand(a0, offsetof(T, swr_1)) );
  __ lw(t1, MemOperand(a0, offsetof(T, reg_init)) );
  __ swr(t1, MemOperand(a0, offsetof(T, swr_1) + 1) );

  __ lw(t2, MemOperand(a0, offsetof(T, mem_init)) );
  __ sw(t2, MemOperand(a0, offsetof(T, swr_2)) );
  __ lw(t2, MemOperand(a0, offsetof(T, reg_init)) );
  __ swr(t2, MemOperand(a0, offsetof(T, swr_2) + 2) );

  __ lw(t3, MemOperand(a0, offsetof(T, mem_init)) );
  __ sw(t3, MemOperand(a0, offsetof(T, swr_3)) );
  __ lw(t3, MemOperand(a0, offsetof(T, reg_init)) );
  __ swr(t3, MemOperand(a0, offsetof(T, swr_3) + 3) );

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  t.reg_init = 0xaabbccdd;
  t.mem_init = 0x11223344;

  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
  USE(dummy);

#if __BYTE_ORDER == __LITTLE_ENDIAN
  CHECK_EQ(static_cast<int32_t>(0x44bbccdd), t.lwl_0);
  CHECK_EQ(static_cast<int32_t>(0x3344ccdd), t.lwl_1);
  CHECK_EQ(static_cast<int32_t>(0x223344dd), t.lwl_2);
  CHECK_EQ(static_cast<int32_t>(0x11223344), t.lwl_3);

  CHECK_EQ(static_cast<int32_t>(0x11223344), t.lwr_0);
  CHECK_EQ(static_cast<int32_t>(0xaa112233), t.lwr_1);
  CHECK_EQ(static_cast<int32_t>(0xaabb1122), t.lwr_2);
  CHECK_EQ(static_cast<int32_t>(0xaabbcc11), t.lwr_3);

  CHECK_EQ(static_cast<int32_t>(0x112233aa), t.swl_0);
  CHECK_EQ(static_cast<int32_t>(0x1122aabb), t.swl_1);
  CHECK_EQ(static_cast<int32_t>(0x11aabbcc), t.swl_2);
  CHECK_EQ(static_cast<int32_t>(0xaabbccdd), t.swl_3);

  CHECK_EQ(static_cast<int32_t>(0xaabbccdd), t.swr_0);
  CHECK_EQ(static_cast<int32_t>(0xbbccdd44), t.swr_1);
  CHECK_EQ(static_cast<int32_t>(0xccdd3344), t.swr_2);
  CHECK_EQ(static_cast<int32_t>(0xdd223344), t.swr_3);
#elif __BYTE_ORDER == __BIG_ENDIAN
  CHECK_EQ(static_cast<int32_t>(0x11223344), t.lwl_0);
  CHECK_EQ(static_cast<int32_t>(0x223344dd), t.lwl_1);
  CHECK_EQ(static_cast<int32_t>(0x3344ccdd), t.lwl_2);
  CHECK_EQ(static_cast<int32_t>(0x44bbccdd), t.lwl_3);

  CHECK_EQ(static_cast<int32_t>(0xaabbcc11), t.lwr_0);
  CHECK_EQ(static_cast<int32_t>(0xaabb1122), t.lwr_1);
  CHECK_EQ(static_cast<int32_t>(0xaa112233), t.lwr_2);
  CHECK_EQ(static_cast<int32_t>(0x11223344), t.lwr_3);

  CHECK_EQ(static_cast<int32_t>(0xaabbccdd), t.swl_0);
  CHECK_EQ(static_cast<int32_t>(0x11aabbcc), t.swl_1);
  CHECK_EQ(static_cast<int32_t>(0x1122aabb), t.swl_2);
  CHECK_EQ(static_cast<int32_t>(0x112233aa), t.swl_3);

  CHECK_EQ(static_cast<int32_t>(0xdd223344), t.swr_0);
  CHECK_EQ(static_cast<int32_t>(0xccdd3344), t.swr_1);
  CHECK_EQ(static_cast<int32_t>(0xbbccdd44), t.swr_2);
  CHECK_EQ(static_cast<int32_t>(0xaabbccdd), t.swr_3);
#else
#error Unknown endianness
#endif
}


TEST(MIPS12) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
      int32_t  x;
      int32_t  y;
      int32_t  y1;
      int32_t  y2;
      int32_t  y3;
      int32_t  y4;
  } T;
  T t;

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  __ mov(t6, fp);  // Save frame pointer.
  __ mov(fp, a0);  // Access struct T by fp.
  __ lw(t0, MemOperand(a0, offsetof(T, y)) );
  __ lw(t3, MemOperand(a0, offsetof(T, y4)) );

  __ addu(t1, t0, t3);
  __ subu(t4, t0, t3);
  __ nop();
  __ push(t0);  // These instructions disappear after opt.
  __ Pop();
  __ addu(t0, t0, t0);
  __ nop();
  __ Pop();     // These instructions disappear after opt.
  __ push(t3);
  __ nop();
  __ push(t3);  // These instructions disappear after opt.
  __ pop(t3);
  __ nop();
  __ push(t3);
  __ pop(t4);
  __ nop();
  __ sw(t0, MemOperand(fp, offsetof(T, y)) );
  __ lw(t0, MemOperand(fp, offsetof(T, y)) );
  __ nop();
  __ sw(t0, MemOperand(fp, offsetof(T, y)) );
  __ lw(t1, MemOperand(fp, offsetof(T, y)) );
  __ nop();
  __ push(t1);
  __ lw(t1, MemOperand(fp, offsetof(T, y)) );
  __ pop(t1);
  __ nop();
  __ push(t1);
  __ lw(t2, MemOperand(fp, offsetof(T, y)) );
  __ pop(t1);
  __ nop();
  __ push(t1);
  __ lw(t2, MemOperand(fp, offsetof(T, y)) );
  __ pop(t2);
  __ nop();
  __ push(t2);
  __ lw(t2, MemOperand(fp, offsetof(T, y)) );
  __ pop(t1);
  __ nop();
  __ push(t1);
  __ lw(t2, MemOperand(fp, offsetof(T, y)) );
  __ pop(t3);
  __ nop();

  __ mov(fp, t6);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  t.x = 1;
  t.y = 2;
  t.y1 = 3;
  t.y2 = 4;
  t.y3 = 0XBABA;
  t.y4 = 0xDEDA;

  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
  USE(dummy);

  CHECK_EQ(3, t.y1);
}


TEST(MIPS13) {
  // Test Cvt_d_uw and Trunc_uw_d macros.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
    double cvt_big_out;
    double cvt_small_out;
    uint32_t trunc_big_out;
    uint32_t trunc_small_out;
    uint32_t cvt_big_in;
    uint32_t cvt_small_in;
  } T;
  T t;

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  __ sw(t0, MemOperand(a0, offsetof(T, cvt_small_in)));
  __ Cvt_d_uw(f10, t0, f4);
  __ Sdc1(f10, MemOperand(a0, offsetof(T, cvt_small_out)));

  __ Trunc_uw_d(f10, f10, f4);
  __ swc1(f10, MemOperand(a0, offsetof(T, trunc_small_out)));

  __ sw(t0, MemOperand(a0, offsetof(T, cvt_big_in)));
  __ Cvt_d_uw(f8, t0, f4);
  __ Sdc1(f8, MemOperand(a0, offsetof(T, cvt_big_out)));

  __ Trunc_uw_d(f8, f8, f4);
  __ swc1(f8, MemOperand(a0, offsetof(T, trunc_big_out)));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());

  t.cvt_big_in = 0xFFFFFFFF;
  t.cvt_small_in  = 333;

  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
  USE(dummy);

  CHECK_EQ(t.cvt_big_out, static_cast<double>(t.cvt_big_in));
  CHECK_EQ(t.cvt_small_out, static_cast<double>(t.cvt_small_in));

  CHECK_EQ(static_cast<int>(t.trunc_big_out), static_cast<int>(t.cvt_big_in));
  CHECK_EQ(static_cast<int>(t.trunc_small_out),
           static_cast<int>(t.cvt_small_in));
}


TEST(MIPS14) {
  // Test round, floor, ceil, trunc, cvt.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

#define ROUND_STRUCT_ELEMENT(x) \
  uint32_t x##_isNaN2008; \
  int32_t x##_up_out; \
  int32_t x##_down_out; \
  int32_t neg_##x##_up_out; \
  int32_t neg_##x##_down_out; \
  uint32_t x##_err1_out; \
  uint32_t x##_err2_out; \
  uint32_t x##_err3_out; \
  uint32_t x##_err4_out; \
  int32_t x##_invalid_result;

  typedef struct {
    double round_up_in;
    double round_down_in;
    double neg_round_up_in;
    double neg_round_down_in;
    double err1_in;
    double err2_in;
    double err3_in;
    double err4_in;

    ROUND_STRUCT_ELEMENT(round)
    ROUND_STRUCT_ELEMENT(floor)
    ROUND_STRUCT_ELEMENT(ceil)
    ROUND_STRUCT_ELEMENT(trunc)
    ROUND_STRUCT_ELEMENT(cvt)
  } T;
  T t;

#undef ROUND_STRUCT_ELEMENT

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  // Save FCSR.
  __ cfc1(a1, FCSR);
  // Disable FPU exceptions.
  __ ctc1(zero_reg, FCSR);
#define RUN_ROUND_TEST(x)                                       \
  __ cfc1(t0, FCSR);                                            \
  __ sw(t0, MemOperand(a0, offsetof(T, x##_isNaN2008)));        \
  __ Ldc1(f0, MemOperand(a0, offsetof(T, round_up_in)));        \
  __ x##_w_d(f0, f0);                                           \
  __ swc1(f0, MemOperand(a0, offsetof(T, x##_up_out)));         \
                                                                \
  __ Ldc1(f0, MemOperand(a0, offsetof(T, round_down_in)));      \
  __ x##_w_d(f0, f0);                                           \
  __ swc1(f0, MemOperand(a0, offsetof(T, x##_down_out)));       \
                                                                \
  __ Ldc1(f0, MemOperand(a0, offsetof(T, neg_round_up_in)));    \
  __ x##_w_d(f0, f0);                                           \
  __ swc1(f0, MemOperand(a0, offsetof(T, neg_##x##_up_out)));   \
                                                                \
  __ Ldc1(f0, MemOperand(a0, offsetof(T, neg_round_down_in)));  \
  __ x##_w_d(f0, f0);                                           \
  __ swc1(f0, MemOperand(a0, offsetof(T, neg_##x##_down_out))); \
                                                                \
  __ Ldc1(f0, MemOperand(a0, offsetof(T, err1_in)));            \
  __ ctc1(zero_reg, FCSR);                                      \
  __ x##_w_d(f0, f0);                                           \
  __ cfc1(a2, FCSR);                                            \
  __ sw(a2, MemOperand(a0, offsetof(T, x##_err1_out)));         \
                                                                \
  __ Ldc1(f0, MemOperand(a0, offsetof(T, err2_in)));            \
  __ ctc1(zero_reg, FCSR);                                      \
  __ x##_w_d(f0, f0);                                           \
  __ cfc1(a2, FCSR);                                            \
  __ sw(a2, MemOperand(a0, offsetof(T, x##_err2_out)));         \
                                                                \
  __ Ldc1(f0, MemOperand(a0, offsetof(T, err3_in)));            \
  __ ctc1(zero_reg, FCSR);                                      \
  __ x##_w_d(f0, f0);                                           \
  __ cfc1(a2, FCSR);                                            \
  __ sw(a2, MemOperand(a0, offsetof(T, x##_err3_out)));         \
                                                                \
  __ Ldc1(f0, MemOperand(a0, offsetof(T, err4_in)));            \
  __ ctc1(zero_reg, FCSR);                                      \
  __ x##_w_d(f0, f0);                                           \
  __ cfc1(a2, FCSR);                                            \
  __ sw(a2, MemOperand(a0, offsetof(T, x##_err4_out)));         \
  __ swc1(f0, MemOperand(a0, offsetof(T, x##_invalid_result)));

  RUN_ROUND_TEST(round)
  RUN_ROUND_TEST(floor)
  RUN_ROUND_TEST(ceil)
  RUN_ROUND_TEST(trunc)
  RUN_ROUND_TEST(cvt)

  // Restore FCSR.
  __ ctc1(a1, FCSR);

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());

  t.round_up_in = 123.51;
  t.round_down_in = 123.49;
  t.neg_round_up_in = -123.5;
  t.neg_round_down_in = -123.49;
  t.err1_in = 123.51;
  t.err2_in = 1;
  t.err3_in = static_cast<double>(1) + 0xFFFFFFFF;
  t.err4_in = NAN;

  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
  USE(dummy);

#define GET_FPU_ERR(x) (static_cast<int>(x & kFCSRFlagMask))
#define CHECK_NAN2008(x) (x & kFCSRNaN2008FlagMask)
#define CHECK_ROUND_RESULT(type)                                  \
  CHECK(GET_FPU_ERR(t.type##_err1_out) & kFCSRInexactFlagMask);   \
  CHECK_EQ(0, GET_FPU_ERR(t.type##_err2_out));                    \
  CHECK(GET_FPU_ERR(t.type##_err3_out) & kFCSRInvalidOpFlagMask); \
  CHECK(GET_FPU_ERR(t.type##_err4_out) & kFCSRInvalidOpFlagMask); \
  if (CHECK_NAN2008(t.type##_isNaN2008) && kArchVariant == kMips32r6) {\
    CHECK_EQ(static_cast<int32_t>(0), t.type##_invalid_result);\
  } else {\
    CHECK_EQ(static_cast<int32_t>(kFPUInvalidResult), t.type##_invalid_result);\
  }


  CHECK_ROUND_RESULT(round);
  CHECK_ROUND_RESULT(floor);
  CHECK_ROUND_RESULT(ceil);
  CHECK_ROUND_RESULT(cvt);
}


TEST(MIPS15) {
  // Test chaining of label usages within instructions (issue 1644).
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  Assembler assm(isolate, NULL, 0);

  Label target;
  __ beq(v0, v1, &target);
  __ nop();
  __ bne(v0, v1, &target);
  __ nop();
  __ bind(&target);
  __ nop();
}


// ----------------------mips32r6 specific tests----------------------
TEST(seleqz_selnez) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);

    typedef struct test {
      int a;
      int b;
      int c;
      int d;
      double e;
      double f;
      double g;
      double h;
      float i;
      float j;
      float k;
      float l;
    } Test;

    Test test;
    // Integer part of test.
    __ addiu(t1, zero_reg, 1);                      // t1 = 1
    __ seleqz(t3, t1, zero_reg);                    // t3 = 1
    __ sw(t3, MemOperand(a0, offsetof(Test, a)));  // a = 1
    __ seleqz(t2, t1, t1);                          // t2 = 0
    __ sw(t2, MemOperand(a0, offsetof(Test, b)));  // b = 0
    __ selnez(t3, t1, zero_reg);                    // t3 = 1;
    __ sw(t3, MemOperand(a0, offsetof(Test, c)));  // c = 0
    __ selnez(t3, t1, t1);                          // t3 = 1
    __ sw(t3, MemOperand(a0, offsetof(Test, d)));  // d = 1
    // Floating point part of test.
    __ Ldc1(f0, MemOperand(a0, offsetof(Test, e)));   // src
    __ Ldc1(f2, MemOperand(a0, offsetof(Test, f)));   // test
    __ lwc1(f8, MemOperand(a0, offsetof(Test, i)) );  // src
    __ lwc1(f10, MemOperand(a0, offsetof(Test, j)) );  // test
    __ seleqz_d(f4, f0, f2);
    __ selnez_d(f6, f0, f2);
    __ seleqz_s(f12, f8, f10);
    __ selnez_s(f14, f8, f10);
    __ Sdc1(f4, MemOperand(a0, offsetof(Test, g)));    // src
    __ Sdc1(f6, MemOperand(a0, offsetof(Test, h)));    // src
    __ swc1(f12, MemOperand(a0, offsetof(Test, k)) );  // src
    __ swc1(f14, MemOperand(a0, offsetof(Test, l)) );  // src
    __ jr(ra);
    __ nop();
    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());

    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));

    CHECK_EQ(1, test.a);
    CHECK_EQ(0, test.b);
    CHECK_EQ(0, test.c);
    CHECK_EQ(1, test.d);

    const int test_size = 3;
    const int input_size = 5;

    double inputs_D[input_size] = {0.0, 65.2, -70.32,
      18446744073709551621.0, -18446744073709551621.0};
    double outputs_D[input_size] = {0.0, 65.2, -70.32,
      18446744073709551621.0, -18446744073709551621.0};
    double tests_D[test_size*2] = {2.8, 2.9, -2.8, -2.9,
      18446744073709551616.0, 18446744073709555712.0};
    float inputs_S[input_size] = {0.0, 65.2, -70.32,
      18446744073709551621.0, -18446744073709551621.0};
    float outputs_S[input_size] = {0.0, 65.2, -70.32,
      18446744073709551621.0, -18446744073709551621.0};
    float tests_S[test_size*2] = {2.9, 2.8, -2.9, -2.8,
      18446744073709551616.0, 18446746272732807168.0};
    for (int j=0; j < test_size; j+=2) {
      for (int i=0; i < input_size; i++) {
        test.e = inputs_D[i];
        test.f = tests_D[j];
        test.i = inputs_S[i];
        test.j = tests_S[j];
        (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
        CHECK_EQ(outputs_D[i], test.g);
        CHECK_EQ(0, test.h);
        CHECK_EQ(outputs_S[i], test.k);
        CHECK_EQ(0, test.l);

        test.f = tests_D[j+1];
        test.j = tests_S[j+1];
        (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
        CHECK_EQ(0, test.g);
        CHECK_EQ(outputs_D[i], test.h);
        CHECK_EQ(0, test.k);
        CHECK_EQ(outputs_S[i], test.l);
      }
    }
  }
}


TEST(min_max) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, nullptr, 0,
                        v8::internal::CodeObjectRequired::kYes);

    struct TestFloat {
      double a;
      double b;
      double c;
      double d;
      float e;
      float f;
      float g;
      float h;
    };

    TestFloat test;
    const double dnan = std::numeric_limits<double>::quiet_NaN();
    const double dinf = std::numeric_limits<double>::infinity();
    const double dminf = -std::numeric_limits<double>::infinity();
    const float fnan = std::numeric_limits<float>::quiet_NaN();
    const float finf = std::numeric_limits<float>::infinity();
    const float fminf = std::numeric_limits<float>::infinity();
    const int kTableLength = 13;
    double inputsa[kTableLength] = {2.0,  3.0,  dnan, 3.0,   -0.0, 0.0, dinf,
                                    dnan, 42.0, dinf, dminf, dinf, dnan};
    double inputsb[kTableLength] = {3.0,  2.0,  3.0,  dnan, 0.0,   -0.0, dnan,
                                    dinf, dinf, 42.0, dinf, dminf, dnan};
    double outputsdmin[kTableLength] = {2.0,   2.0,   3.0,  3.0,  -0.0,
                                        -0.0,  dinf,  dinf, 42.0, 42.0,
                                        dminf, dminf, dnan};
    double outputsdmax[kTableLength] = {3.0,  3.0,  3.0,  3.0,  0.0,  0.0, dinf,
                                        dinf, dinf, dinf, dinf, dinf, dnan};

    float inputse[kTableLength] = {2.0,  3.0,  fnan, 3.0,   -0.0, 0.0, finf,
                                   fnan, 42.0, finf, fminf, finf, fnan};
    float inputsf[kTableLength] = {3.0,  2.0,  3.0,  fnan, 0.0,   -0.0, fnan,
                                   finf, finf, 42.0, finf, fminf, fnan};
    float outputsfmin[kTableLength] = {2.0,   2.0,   3.0,  3.0,  -0.0,
                                       -0.0,  finf,  finf, 42.0, 42.0,
                                       fminf, fminf, fnan};
    float outputsfmax[kTableLength] = {3.0,  3.0,  3.0,  3.0,  0.0,  0.0, finf,
                                       finf, finf, finf, finf, finf, fnan};

    __ Ldc1(f4, MemOperand(a0, offsetof(TestFloat, a)));
    __ Ldc1(f8, MemOperand(a0, offsetof(TestFloat, b)));
    __ lwc1(f2, MemOperand(a0, offsetof(TestFloat, e)));
    __ lwc1(f6, MemOperand(a0, offsetof(TestFloat, f)));
    __ min_d(f10, f4, f8);
    __ max_d(f12, f4, f8);
    __ min_s(f14, f2, f6);
    __ max_s(f16, f2, f6);
    __ Sdc1(f10, MemOperand(a0, offsetof(TestFloat, c)));
    __ Sdc1(f12, MemOperand(a0, offsetof(TestFloat, d)));
    __ swc1(f14, MemOperand(a0, offsetof(TestFloat, g)));
    __ swc1(f16, MemOperand(a0, offsetof(TestFloat, h)));
    __ jr(ra);
    __ nop();

    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());
    for (int i = 0; i < kTableLength; i++) {
      test.a = inputsa[i];
      test.b = inputsb[i];
      test.e = inputse[i];
      test.f = inputsf[i];

      CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0);

      CHECK_EQ(0, memcmp(&test.c, &outputsdmin[i], sizeof(test.c)));
      CHECK_EQ(0, memcmp(&test.d, &outputsdmax[i], sizeof(test.d)));
      CHECK_EQ(0, memcmp(&test.g, &outputsfmin[i], sizeof(test.g)));
      CHECK_EQ(0, memcmp(&test.h, &outputsfmax[i], sizeof(test.h)));
    }
  }
}


TEST(rint_d)  {
  if (IsMipsArchVariant(kMips32r6)) {
    const int kTableLength = 30;
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);

    typedef struct test_float {
      double a;
      double b;
      int fcsr;
    }TestFloat;

    TestFloat test;
    double inputs[kTableLength] = {18446744073709551617.0,
      4503599627370496.0, -4503599627370496.0,
      1.26782468584154733584017312973E30, 1.44860108245951772690707170478E147,
      1.7976931348623157E+308, 6.27463370218383111104242366943E-307,
      309485009821345068724781056.89,
      2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
      -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
      37778931862957161709568.0, 37778931862957161709569.0,
      37778931862957161709580.0, 37778931862957161709581.0,
      37778931862957161709582.0, 37778931862957161709583.0,
      37778931862957161709584.0, 37778931862957161709585.0,
      37778931862957161709586.0, 37778931862957161709587.0};
    double outputs_RN[kTableLength] = {18446744073709551617.0,
      4503599627370496.0, -4503599627370496.0,
      1.26782468584154733584017312973E30, 1.44860108245951772690707170478E147,
      1.7976931348623157E308, 0,
      309485009821345068724781057.0,
      2.0, 3.0, 2.0, 3.0, 4.0, 4.0,
      -2.0, -3.0, -2.0, -3.0, -4.0, -4.0,
      37778931862957161709568.0, 37778931862957161709569.0,
      37778931862957161709580.0, 37778931862957161709581.0,
      37778931862957161709582.0, 37778931862957161709583.0,
      37778931862957161709584.0, 37778931862957161709585.0,
      37778931862957161709586.0, 37778931862957161709587.0};
    double outputs_RZ[kTableLength] = {18446744073709551617.0,
      4503599627370496.0, -4503599627370496.0,
      1.26782468584154733584017312973E30, 1.44860108245951772690707170478E147,
      1.7976931348623157E308, 0,
      309485009821345068724781057.0,
      2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
      -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
      37778931862957161709568.0, 37778931862957161709569.0,
      37778931862957161709580.0, 37778931862957161709581.0,
      37778931862957161709582.0, 37778931862957161709583.0,
      37778931862957161709584.0, 37778931862957161709585.0,
      37778931862957161709586.0, 37778931862957161709587.0};
    double outputs_RP[kTableLength] = {18446744073709551617.0,
      4503599627370496.0, -4503599627370496.0,
      1.26782468584154733584017312973E30, 1.44860108245951772690707170478E147,
      1.7976931348623157E308, 1,
      309485009821345068724781057.0,
      3.0, 3.0, 3.0, 4.0, 4.0, 4.0,
      -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
      37778931862957161709568.0, 37778931862957161709569.0,
      37778931862957161709580.0, 37778931862957161709581.0,
      37778931862957161709582.0, 37778931862957161709583.0,
      37778931862957161709584.0, 37778931862957161709585.0,
      37778931862957161709586.0, 37778931862957161709587.0};
    double outputs_RM[kTableLength] = {18446744073709551617.0,
      4503599627370496.0, -4503599627370496.0,
      1.26782468584154733584017312973E30, 1.44860108245951772690707170478E147,
      1.7976931348623157E308, 0,
      309485009821345068724781057.0,
      2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
      -3.0, -3.0, -3.0, -4.0, -4.0, -4.0,
      37778931862957161709568.0, 37778931862957161709569.0,
      37778931862957161709580.0, 37778931862957161709581.0,
      37778931862957161709582.0, 37778931862957161709583.0,
      37778931862957161709584.0, 37778931862957161709585.0,
      37778931862957161709586.0, 37778931862957161709587.0};
    int fcsr_inputs[4] =
      {kRoundToNearest, kRoundToZero, kRoundToPlusInf, kRoundToMinusInf};
    double* outputs[4] = {outputs_RN, outputs_RZ, outputs_RP, outputs_RM};
    __ Ldc1(f4, MemOperand(a0, offsetof(TestFloat, a)));
    __ lw(t0, MemOperand(a0, offsetof(TestFloat, fcsr)) );
    __ cfc1(t1, FCSR);
    __ ctc1(t0, FCSR);
    __ rint_d(f8, f4);
    __ Sdc1(f8, MemOperand(a0, offsetof(TestFloat, b)));
    __ ctc1(t1, FCSR);
    __ jr(ra);
    __ nop();

    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());

    for (int j = 0; j < 4; j++) {
      test.fcsr = fcsr_inputs[j];
      for (int i = 0; i < kTableLength; i++) {
        test.a = inputs[i];
        (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
        CHECK_EQ(test.b, outputs[j][i]);
      }
    }
  }
}


TEST(sel) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);

    typedef struct test {
      double dd;
      double ds;
      double dt;
      float fd;
      float fs;
      float ft;
    } Test;

    Test test;
    __ Ldc1(f0, MemOperand(a0, offsetof(Test, dd)));   // test
    __ Ldc1(f2, MemOperand(a0, offsetof(Test, ds)));   // src1
    __ Ldc1(f4, MemOperand(a0, offsetof(Test, dt)));   // src2
    __ lwc1(f6, MemOperand(a0, offsetof(Test, fd)) );  // test
    __ lwc1(f8, MemOperand(a0, offsetof(Test, fs)) );  // src1
    __ lwc1(f10, MemOperand(a0, offsetof(Test, ft)) );  // src2
    __ sel_d(f0, f2, f4);
    __ sel_s(f6, f8, f10);
    __ Sdc1(f0, MemOperand(a0, offsetof(Test, dd)));
    __ swc1(f6, MemOperand(a0, offsetof(Test, fd)) );
    __ jr(ra);
    __ nop();
    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());

    const int test_size = 3;
    const int input_size = 5;

    double inputs_dt[input_size] = {0.0, 65.2, -70.32,
      18446744073709551621.0, -18446744073709551621.0};
    double inputs_ds[input_size] = {0.1, 69.88, -91.325,
      18446744073709551625.0, -18446744073709551625.0};
    float inputs_ft[input_size] = {0.0, 65.2, -70.32,
      18446744073709551621.0, -18446744073709551621.0};
    float inputs_fs[input_size] = {0.1, 69.88, -91.325,
      18446744073709551625.0, -18446744073709551625.0};
    double tests_D[test_size*2] = {2.8, 2.9, -2.8, -2.9,
      18446744073709551616.0, 18446744073709555712.0};
    float tests_S[test_size*2] = {2.9, 2.8, -2.9, -2.8,
      18446744073709551616.0, 18446746272732807168.0};
    for (int j=0; j < test_size; j+=2) {
      for (int i=0; i < input_size; i++) {
        test.dt = inputs_dt[i];
        test.dd = tests_D[j];
        test.ds = inputs_ds[i];
        test.ft = inputs_ft[i];
        test.fd = tests_S[j];
        test.fs = inputs_fs[i];
        (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
        CHECK_EQ(test.dd, inputs_ds[i]);
        CHECK_EQ(test.fd, inputs_fs[i]);

        test.dd = tests_D[j+1];
        test.fd = tests_S[j+1];
        (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
        CHECK_EQ(test.dd, inputs_dt[i]);
        CHECK_EQ(test.fd, inputs_ft[i]);
      }
    }
  }
}


TEST(rint_s)  {
  if (IsMipsArchVariant(kMips32r6)) {
    const int kTableLength = 30;
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);

    typedef struct test_float {
      float a;
      float b;
      int fcsr;
    }TestFloat;

    TestFloat test;
    float inputs[kTableLength] = {18446744073709551617.0,
      4503599627370496.0, -4503599627370496.0,
      1.26782468584154733584017312973E30, 1.44860108245951772690707170478E37,
      1.7976931348623157E+38, 6.27463370218383111104242366943E-37,
      309485009821345068724781056.89,
      2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
      -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
      37778931862957161709568.0, 37778931862957161709569.0,
      37778931862957161709580.0, 37778931862957161709581.0,
      37778931862957161709582.0, 37778931862957161709583.0,
      37778931862957161709584.0, 37778931862957161709585.0,
      37778931862957161709586.0, 37778931862957161709587.0};
    float outputs_RN[kTableLength] = {18446744073709551617.0,
      4503599627370496.0, -4503599627370496.0,
      1.26782468584154733584017312973E30, 1.44860108245951772690707170478E37,
      1.7976931348623157E38, 0,
      309485009821345068724781057.0,
      2.0, 3.0, 2.0, 3.0, 4.0, 4.0,
      -2.0, -3.0, -2.0, -3.0, -4.0, -4.0,
      37778931862957161709568.0, 37778931862957161709569.0,
      37778931862957161709580.0, 37778931862957161709581.0,
      37778931862957161709582.0, 37778931862957161709583.0,
      37778931862957161709584.0, 37778931862957161709585.0,
      37778931862957161709586.0, 37778931862957161709587.0};
    float outputs_RZ[kTableLength] = {18446744073709551617.0,
      4503599627370496.0, -4503599627370496.0,
      1.26782468584154733584017312973E30, 1.44860108245951772690707170478E37,
      1.7976931348623157E38, 0,
      309485009821345068724781057.0,
      2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
      -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
      37778931862957161709568.0, 37778931862957161709569.0,
      37778931862957161709580.0, 37778931862957161709581.0,
      37778931862957161709582.0, 37778931862957161709583.0,
      37778931862957161709584.0, 37778931862957161709585.0,
      37778931862957161709586.0, 37778931862957161709587.0};
    float outputs_RP[kTableLength] = {18446744073709551617.0,
      4503599627370496.0, -4503599627370496.0,
      1.26782468584154733584017312973E30, 1.44860108245951772690707170478E37,
      1.7976931348623157E38, 1,
      309485009821345068724781057.0,
      3.0, 3.0, 3.0, 4.0, 4.0, 4.0,
      -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
      37778931862957161709568.0, 37778931862957161709569.0,
      37778931862957161709580.0, 37778931862957161709581.0,
      37778931862957161709582.0, 37778931862957161709583.0,
      37778931862957161709584.0, 37778931862957161709585.0,
      37778931862957161709586.0, 37778931862957161709587.0};
    float outputs_RM[kTableLength] = {18446744073709551617.0,
      4503599627370496.0, -4503599627370496.0,
      1.26782468584154733584017312973E30, 1.44860108245951772690707170478E37,
      1.7976931348623157E38, 0,
      309485009821345068724781057.0,
      2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
      -3.0, -3.0, -3.0, -4.0, -4.0, -4.0,
      37778931862957161709568.0, 37778931862957161709569.0,
      37778931862957161709580.0, 37778931862957161709581.0,
      37778931862957161709582.0, 37778931862957161709583.0,
      37778931862957161709584.0, 37778931862957161709585.0,
      37778931862957161709586.0, 37778931862957161709587.0};
    int fcsr_inputs[4] =
      {kRoundToNearest, kRoundToZero, kRoundToPlusInf, kRoundToMinusInf};
    float* outputs[4] = {outputs_RN, outputs_RZ, outputs_RP, outputs_RM};
    __ lwc1(f4, MemOperand(a0, offsetof(TestFloat, a)) );
    __ lw(t0, MemOperand(a0, offsetof(TestFloat, fcsr)) );
    __ cfc1(t1, FCSR);
    __ ctc1(t0, FCSR);
    __ rint_s(f8, f4);
    __ swc1(f8, MemOperand(a0, offsetof(TestFloat, b)) );
    __ ctc1(t1, FCSR);
    __ jr(ra);
    __ nop();

    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());

    for (int j = 0; j < 4; j++) {
      test.fcsr = fcsr_inputs[j];
      for (int i = 0; i < kTableLength; i++) {
        test.a = inputs[i];
        (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
        CHECK_EQ(test.b, outputs[j][i]);
      }
    }
  }
}


TEST(Cvt_d_uw) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0,
                      v8::internal::CodeObjectRequired::kYes);

  typedef struct test_struct {
    unsigned input;
    uint64_t output;
  } TestStruct;

  unsigned inputs[] = {
    0x0, 0xffffffff, 0x80000000, 0x7fffffff
  };

  uint64_t outputs[] = {
    0x0, 0x41efffffffe00000,
    0x41e0000000000000, 0x41dfffffffc00000
  };

  int kTableLength = sizeof(inputs)/sizeof(inputs[0]);

  TestStruct test;

  __ lw(t1, MemOperand(a0, offsetof(TestStruct, input)));
  __ Cvt_d_uw(f4, t1, f6);
  __ Sdc1(f4, MemOperand(a0, offsetof(TestStruct, output)));
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  for (int i = 0; i < kTableLength; i++) {
    test.input = inputs[i];
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    // Check outputs
    CHECK_EQ(test.output, outputs[i]);
  }
}


TEST(mina_maxa) {
  if (IsMipsArchVariant(kMips32r6)) {
    const int kTableLength = 23;
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, nullptr, 0,
                        v8::internal::CodeObjectRequired::kYes);
    const double dnan = std::numeric_limits<double>::quiet_NaN();
    const double dinf = std::numeric_limits<double>::infinity();
    const double dminf = -std::numeric_limits<double>::infinity();
    const float fnan = std::numeric_limits<float>::quiet_NaN();
    const float finf = std::numeric_limits<float>::infinity();
    const float fminf = std::numeric_limits<float>::infinity();

    struct TestFloat {
      double a;
      double b;
      double resd;
      double resd1;
      float c;
      float d;
      float resf;
      float resf1;
    };

    TestFloat test;
    double inputsa[kTableLength] = {
        5.3,  4.8, 6.1,  9.8, 9.8,  9.8,  -10.0, -8.9, -9.8,  -10.0, -8.9, -9.8,
        dnan, 3.0, -0.0, 0.0, dinf, dnan, 42.0,  dinf, dminf, dinf,  dnan};
    double inputsb[kTableLength] = {
        4.8, 5.3,  6.1, -10.0, -8.9, -9.8, 9.8,  9.8,  9.8,  -9.8,  -11.2, -9.8,
        3.0, dnan, 0.0, -0.0,  dnan, dinf, dinf, 42.0, dinf, dminf, dnan};
    double resd[kTableLength] = {
        4.8, 4.8, 6.1,  9.8,  -8.9, -9.8, 9.8,  -8.9, -9.8,  -9.8,  -8.9, -9.8,
        3.0, 3.0, -0.0, -0.0, dinf, dinf, 42.0, 42.0, dminf, dminf, dnan};
    double resd1[kTableLength] = {
        5.3, 5.3, 6.1, -10.0, 9.8,  9.8,  -10.0, 9.8,  9.8,  -10.0, -11.2, -9.8,
        3.0, 3.0, 0.0, 0.0,   dinf, dinf, dinf,  dinf, dinf, dinf,  dnan};
    float inputsc[kTableLength] = {
        5.3,  4.8, 6.1,  9.8, 9.8,  9.8,  -10.0, -8.9, -9.8,  -10.0, -8.9, -9.8,
        fnan, 3.0, -0.0, 0.0, finf, fnan, 42.0,  finf, fminf, finf,  fnan};
    float inputsd[kTableLength] = {4.8,  5.3,  6.1,  -10.0, -8.9,  -9.8,
                                   9.8,  9.8,  9.8,  -9.8,  -11.2, -9.8,
                                   3.0,  fnan, -0.0, 0.0,   fnan,  finf,
                                   finf, 42.0, finf, fminf, fnan};
    float resf[kTableLength] = {
        4.8, 4.8, 6.1,  9.8,  -8.9, -9.8, 9.8,  -8.9, -9.8,  -9.8,  -8.9, -9.8,
        3.0, 3.0, -0.0, -0.0, finf, finf, 42.0, 42.0, fminf, fminf, fnan};
    float resf1[kTableLength] = {
        5.3, 5.3, 6.1, -10.0, 9.8,  9.8,  -10.0, 9.8,  9.8,  -10.0, -11.2, -9.8,
        3.0, 3.0, 0.0, 0.0,   finf, finf, finf,  finf, finf, finf,  fnan};

    __ Ldc1(f2, MemOperand(a0, offsetof(TestFloat, a)));
    __ Ldc1(f4, MemOperand(a0, offsetof(TestFloat, b)));
    __ lwc1(f8, MemOperand(a0, offsetof(TestFloat, c)) );
    __ lwc1(f10, MemOperand(a0, offsetof(TestFloat, d)) );
    __ mina_d(f6, f2, f4);
    __ mina_s(f12, f8, f10);
    __ maxa_d(f14, f2, f4);
    __ maxa_s(f16, f8, f10);
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, resf)) );
    __ Sdc1(f6, MemOperand(a0, offsetof(TestFloat, resd)));
    __ swc1(f16, MemOperand(a0, offsetof(TestFloat, resf1)) );
    __ Sdc1(f14, MemOperand(a0, offsetof(TestFloat, resd1)));
    __ jr(ra);
    __ nop();

    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());
    for (int i = 0; i < kTableLength; i++) {
      test.a = inputsa[i];
      test.b = inputsb[i];
      test.c = inputsc[i];
      test.d = inputsd[i];
      (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
      if (i < kTableLength - 1) {
        CHECK_EQ(test.resd, resd[i]);
        CHECK_EQ(test.resf, resf[i]);
        CHECK_EQ(test.resd1, resd1[i]);
        CHECK_EQ(test.resf1, resf1[i]);
      } else {
        CHECK(std::isnan(test.resd));
        CHECK(std::isnan(test.resf));
        CHECK(std::isnan(test.resd1));
        CHECK(std::isnan(test.resf1));
      }
    }
  }
}


// ----------------------mips32r2 specific tests----------------------
TEST(trunc_l) {
  if (IsMipsArchVariant(kMips32r2) && IsFp64Mode()) {
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);
    const double dFPU64InvalidResult = static_cast<double>(kFPU64InvalidResult);
    typedef struct test_float {
      uint32_t isNaN2008;
      double a;
      float b;
      int64_t c;  // a trunc result
      int64_t d;  // b trunc result
    }Test;
    const int kTableLength = 15;
    double inputs_D[kTableLength] = {
        2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
        -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
        2147483648.0,
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::infinity()
        };
    float inputs_S[kTableLength] = {
        2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
        -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
        2147483648.0,
        std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::infinity()
        };
    double outputs[kTableLength] = {
        2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
        -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
        2147483648.0, dFPU64InvalidResult,
        dFPU64InvalidResult};
    double outputsNaN2008[kTableLength] = {
        2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
        -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
        2147483648.0,
        0,
        dFPU64InvalidResult};

    __ cfc1(t1, FCSR);
    __ sw(t1, MemOperand(a0, offsetof(Test, isNaN2008)));
    __ Ldc1(f4, MemOperand(a0, offsetof(Test, a)));
    __ lwc1(f6, MemOperand(a0, offsetof(Test, b)) );
    __ trunc_l_d(f8, f4);
    __ trunc_l_s(f10, f6);
    __ Sdc1(f8, MemOperand(a0, offsetof(Test, c)));
    __ Sdc1(f10, MemOperand(a0, offsetof(Test, d)));
    __ jr(ra);
    __ nop();
    Test test;
    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());
    for (int i = 0; i < kTableLength; i++) {
      test.a = inputs_D[i];
      test.b = inputs_S[i];
      (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
      if ((test.isNaN2008 & kFCSRNaN2008FlagMask) &&
              kArchVariant == kMips32r6) {
        CHECK_EQ(test.c, outputsNaN2008[i]);
      } else {
        CHECK_EQ(test.c, outputs[i]);
      }
      CHECK_EQ(test.d, test.c);
    }
  }
}


TEST(movz_movn) {
  if (IsMipsArchVariant(kMips32r2)) {
    const int kTableLength = 4;
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);

    typedef struct test_float {
      int32_t rt;
      double a;
      double b;
      double bold;
      double b1;
      double bold1;
      float c;
      float d;
      float dold;
      float d1;
      float dold1;
    }TestFloat;

    TestFloat test;
    double inputs_D[kTableLength] = {
      5.3, -5.3, 5.3, -2.9
    };
    double inputs_S[kTableLength] = {
      4.8, 4.8, -4.8, -0.29
    };

    float outputs_S[kTableLength] = {
      4.8, 4.8, -4.8, -0.29
    };
    double outputs_D[kTableLength] = {
      5.3, -5.3, 5.3, -2.9
    };

    __ Ldc1(f2, MemOperand(a0, offsetof(TestFloat, a)));
    __ lwc1(f6, MemOperand(a0, offsetof(TestFloat, c)) );
    __ lw(t0, MemOperand(a0, offsetof(TestFloat, rt)) );
    __ Move(f12, 0.0);
    __ Move(f10, 0.0);
    __ Move(f16, 0.0);
    __ Move(f14, 0.0);
    __ Sdc1(f12, MemOperand(a0, offsetof(TestFloat, bold)));
    __ swc1(f10, MemOperand(a0, offsetof(TestFloat, dold)) );
    __ Sdc1(f16, MemOperand(a0, offsetof(TestFloat, bold1)));
    __ swc1(f14, MemOperand(a0, offsetof(TestFloat, dold1)) );
    __ movz_s(f10, f6, t0);
    __ movz_d(f12, f2, t0);
    __ movn_s(f14, f6, t0);
    __ movn_d(f16, f2, t0);
    __ swc1(f10, MemOperand(a0, offsetof(TestFloat, d)) );
    __ Sdc1(f12, MemOperand(a0, offsetof(TestFloat, b)));
    __ swc1(f14, MemOperand(a0, offsetof(TestFloat, d1)) );
    __ Sdc1(f16, MemOperand(a0, offsetof(TestFloat, b1)));
    __ jr(ra);
    __ nop();

    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());
    for (int i = 0; i < kTableLength; i++) {
      test.a = inputs_D[i];
      test.c = inputs_S[i];

      test.rt = 1;
      (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
      CHECK_EQ(test.b, test.bold);
      CHECK_EQ(test.d, test.dold);
      CHECK_EQ(test.b1, outputs_D[i]);
      CHECK_EQ(test.d1, outputs_S[i]);

      test.rt = 0;
      (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
      CHECK_EQ(test.b, outputs_D[i]);
      CHECK_EQ(test.d, outputs_S[i]);
      CHECK_EQ(test.b1, test.bold1);
      CHECK_EQ(test.d1, test.dold1);
    }
  }
}


TEST(movt_movd) {
  if (IsMipsArchVariant(kMips32r2)) {
    const int kTableLength = 4;
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();

    typedef struct test_float {
      double srcd;
      double dstd;
      double dstdold;
      double dstd1;
      double dstdold1;
      float srcf;
      float dstf;
      float dstfold;
      float dstf1;
      float dstfold1;
      int32_t cc;
      int32_t fcsr;
    }TestFloat;

    TestFloat test;
    double inputs_D[kTableLength] = {
      5.3, -5.3, 20.8, -2.9
    };
    double inputs_S[kTableLength] = {
      4.88, 4.8, -4.8, -0.29
    };

    float outputs_S[kTableLength] = {
      4.88, 4.8, -4.8, -0.29
    };
    double outputs_D[kTableLength] = {
      5.3, -5.3, 20.8, -2.9
    };
    int condition_flags[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    for (int i = 0; i < kTableLength; i++) {
      test.srcd = inputs_D[i];
      test.srcf = inputs_S[i];

      for (int j = 0; j< 8; j++) {
        test.cc = condition_flags[j];
        if (test.cc == 0) {
          test.fcsr = 1 << 23;
        } else {
          test.fcsr = 1 << (24+condition_flags[j]);
        }
        HandleScope scope(isolate);
        MacroAssembler assm(isolate, NULL, 0,
                            v8::internal::CodeObjectRequired::kYes);
        __ Ldc1(f2, MemOperand(a0, offsetof(TestFloat, srcd)));
        __ lwc1(f4, MemOperand(a0, offsetof(TestFloat, srcf)) );
        __ lw(t1, MemOperand(a0, offsetof(TestFloat, fcsr)) );
        __ cfc1(t0, FCSR);
        __ ctc1(t1, FCSR);
        __ li(t2, 0x0);
        __ mtc1(t2, f12);
        __ mtc1(t2, f10);
        __ Sdc1(f10, MemOperand(a0, offsetof(TestFloat, dstdold)));
        __ swc1(f12, MemOperand(a0, offsetof(TestFloat, dstfold)) );
        __ movt_s(f12, f4, test.cc);
        __ movt_d(f10, f2, test.cc);
        __ swc1(f12, MemOperand(a0, offsetof(TestFloat, dstf)) );
        __ Sdc1(f10, MemOperand(a0, offsetof(TestFloat, dstd)));
        __ Sdc1(f10, MemOperand(a0, offsetof(TestFloat, dstdold1)));
        __ swc1(f12, MemOperand(a0, offsetof(TestFloat, dstfold1)) );
        __ movf_s(f12, f4, test.cc);
        __ movf_d(f10, f2, test.cc);
        __ swc1(f12, MemOperand(a0, offsetof(TestFloat, dstf1)) );
        __ Sdc1(f10, MemOperand(a0, offsetof(TestFloat, dstd1)));
        __ ctc1(t0, FCSR);
        __ jr(ra);
        __ nop();

        CodeDesc desc;
        assm.GetCode(isolate, &desc);
        Handle<Code> code = isolate->factory()->NewCode(
            desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
        F3 f = FUNCTION_CAST<F3>(code->entry());

        (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
        CHECK_EQ(test.dstf, outputs_S[i]);
        CHECK_EQ(test.dstd, outputs_D[i]);
        CHECK_EQ(test.dstf1, test.dstfold1);
        CHECK_EQ(test.dstd1, test.dstdold1);
        test.fcsr = 0;
        (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
        CHECK_EQ(test.dstf, test.dstfold);
        CHECK_EQ(test.dstd, test.dstdold);
        CHECK_EQ(test.dstf1, outputs_S[i]);
        CHECK_EQ(test.dstd1, outputs_D[i]);
      }
    }
  }
}


// ----------------------tests for all archs--------------------------
TEST(cvt_w_d) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    double a;
    int32_t b;
    int32_t fcsr;
  }Test;
  const int kTableLength = 24;
  double inputs[kTableLength] = {
      2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
      -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
      2147483637.0, 2147483638.0, 2147483639.0,
      2147483640.0, 2147483641.0, 2147483642.0,
      2147483643.0, 2147483644.0, 2147483645.0,
      2147483646.0, 2147483647.0, 2147483653.0
      };
  double outputs_RN[kTableLength] = {
      2.0, 3.0, 2.0, 3.0, 4.0, 4.0,
      -2.0, -3.0, -2.0, -3.0, -4.0, -4.0,
      2147483637.0, 2147483638.0, 2147483639.0,
      2147483640.0, 2147483641.0, 2147483642.0,
      2147483643.0, 2147483644.0, 2147483645.0,
      2147483646.0, 2147483647.0, kFPUInvalidResult};
  double outputs_RZ[kTableLength] = {
      2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
      -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
      2147483637.0, 2147483638.0, 2147483639.0,
      2147483640.0, 2147483641.0, 2147483642.0,
      2147483643.0, 2147483644.0, 2147483645.0,
      2147483646.0, 2147483647.0, kFPUInvalidResult};
  double outputs_RP[kTableLength] = {
      3.0, 3.0, 3.0, 4.0, 4.0, 4.0,
      -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
      2147483637.0, 2147483638.0, 2147483639.0,
      2147483640.0, 2147483641.0, 2147483642.0,
      2147483643.0, 2147483644.0, 2147483645.0,
      2147483646.0, 2147483647.0, kFPUInvalidResult};
  double outputs_RM[kTableLength] = {
      2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
      -3.0, -3.0, -3.0, -4.0, -4.0, -4.0,
      2147483637.0, 2147483638.0, 2147483639.0,
      2147483640.0, 2147483641.0, 2147483642.0,
      2147483643.0, 2147483644.0, 2147483645.0,
      2147483646.0, 2147483647.0, kFPUInvalidResult};
  int fcsr_inputs[4] =
      {kRoundToNearest, kRoundToZero, kRoundToPlusInf, kRoundToMinusInf};
  double* outputs[4] = {outputs_RN, outputs_RZ, outputs_RP, outputs_RM};
  __ Ldc1(f4, MemOperand(a0, offsetof(Test, a)));
  __ lw(t0, MemOperand(a0, offsetof(Test, fcsr)) );
  __ cfc1(t1, FCSR);
  __ ctc1(t0, FCSR);
  __ cvt_w_d(f8, f4);
  __ swc1(f8, MemOperand(a0, offsetof(Test, b)) );
  __ ctc1(t1, FCSR);
  __ jr(ra);
  __ nop();
  Test test;
  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  for (int j = 0; j < 4; j++) {
    test.fcsr = fcsr_inputs[j];
    for (int i = 0; i < kTableLength; i++) {
      test.a = inputs[i];
      (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
      CHECK_EQ(test.b, outputs[j][i]);
    }
  }
}


TEST(trunc_w) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    uint32_t isNaN2008;
    double a;
    float b;
    int32_t c;  // a trunc result
    int32_t d;  // b trunc result
  }Test;
  const int kTableLength = 15;
  double inputs_D[kTableLength] = {
      2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
      -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
      2147483648.0,
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::infinity()
      };
  float inputs_S[kTableLength] = {
      2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
      -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
      2147483648.0,
      std::numeric_limits<float>::quiet_NaN(),
      std::numeric_limits<float>::infinity()
      };
  double outputs[kTableLength] = {
      2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
      -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
      kFPUInvalidResult, kFPUInvalidResult,
      kFPUInvalidResult};
  double outputsNaN2008[kTableLength] = {
      2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
      -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
      kFPUInvalidResult,
      0,
      kFPUInvalidResult};

  __ cfc1(t1, FCSR);
  __ sw(t1, MemOperand(a0, offsetof(Test, isNaN2008)));
  __ Ldc1(f4, MemOperand(a0, offsetof(Test, a)));
  __ lwc1(f6, MemOperand(a0, offsetof(Test, b)) );
  __ trunc_w_d(f8, f4);
  __ trunc_w_s(f10, f6);
  __ swc1(f8, MemOperand(a0, offsetof(Test, c)) );
  __ swc1(f10, MemOperand(a0, offsetof(Test, d)) );
  __ jr(ra);
  __ nop();
  Test test;
  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  for (int i = 0; i < kTableLength; i++) {
    test.a = inputs_D[i];
    test.b = inputs_S[i];
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    if ((test.isNaN2008 & kFCSRNaN2008FlagMask) && kArchVariant == kMips32r6) {
      CHECK_EQ(test.c, outputsNaN2008[i]);
    } else {
      CHECK_EQ(test.c, outputs[i]);
    }
    CHECK_EQ(test.d, test.c);
  }
}


TEST(round_w) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    uint32_t isNaN2008;
    double a;
    float b;
    int32_t c;  // a trunc result
    int32_t d;  // b trunc result
  }Test;
  const int kTableLength = 15;
  double inputs_D[kTableLength] = {
      2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
      -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
      2147483648.0,
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::infinity()
      };
  float inputs_S[kTableLength] = {
      2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
      -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
      2147483648.0,
      std::numeric_limits<float>::quiet_NaN(),
      std::numeric_limits<float>::infinity()
      };
  double outputs[kTableLength] = {
      2.0, 3.0, 2.0, 3.0, 4.0, 4.0,
      -2.0, -3.0, -2.0, -3.0, -4.0, -4.0,
      kFPUInvalidResult, kFPUInvalidResult,
      kFPUInvalidResult};
  double outputsNaN2008[kTableLength] = {
      2.0, 3.0, 2.0, 3.0, 4.0, 4.0,
      -2.0, -3.0, -2.0, -3.0, -4.0, -4.0,
      kFPUInvalidResult, 0,
      kFPUInvalidResult};

  __ cfc1(t1, FCSR);
  __ sw(t1, MemOperand(a0, offsetof(Test, isNaN2008)));
  __ Ldc1(f4, MemOperand(a0, offsetof(Test, a)));
  __ lwc1(f6, MemOperand(a0, offsetof(Test, b)) );
  __ round_w_d(f8, f4);
  __ round_w_s(f10, f6);
  __ swc1(f8, MemOperand(a0, offsetof(Test, c)) );
  __ swc1(f10, MemOperand(a0, offsetof(Test, d)) );
  __ jr(ra);
  __ nop();
  Test test;
  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  for (int i = 0; i < kTableLength; i++) {
    test.a = inputs_D[i];
    test.b = inputs_S[i];
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    if ((test.isNaN2008 & kFCSRNaN2008FlagMask) && kArchVariant == kMips32r6) {
      CHECK_EQ(test.c, outputsNaN2008[i]);
    } else {
      CHECK_EQ(test.c, outputs[i]);
    }
    CHECK_EQ(test.d, test.c);
  }
}


TEST(round_l) {
  if (IsFp64Mode()) {
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);
    const double dFPU64InvalidResult = static_cast<double>(kFPU64InvalidResult);
    typedef struct test_float {
      uint32_t isNaN2008;
      double a;
      float b;
      int64_t c;
      int64_t d;
    }Test;
    const int kTableLength = 15;
    double inputs_D[kTableLength] = {
        2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
        -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
        2147483648.0,
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::infinity()
        };
    float inputs_S[kTableLength] = {
        2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
        -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
        2147483648.0,
        std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::infinity()
        };
    double outputs[kTableLength] = {
        2.0, 3.0, 2.0, 3.0, 4.0, 4.0,
        -2.0, -3.0, -2.0, -3.0, -4.0, -4.0,
        2147483648.0, dFPU64InvalidResult,
        dFPU64InvalidResult};
    double outputsNaN2008[kTableLength] = {
        2.0, 3.0, 2.0, 3.0, 4.0, 4.0,
        -2.0, -3.0, -2.0, -3.0, -4.0, -4.0,
        2147483648.0,
        0,
        dFPU64InvalidResult};

    __ cfc1(t1, FCSR);
    __ sw(t1, MemOperand(a0, offsetof(Test, isNaN2008)));
    __ Ldc1(f4, MemOperand(a0, offsetof(Test, a)));
    __ lwc1(f6, MemOperand(a0, offsetof(Test, b)) );
    __ round_l_d(f8, f4);
    __ round_l_s(f10, f6);
    __ Sdc1(f8, MemOperand(a0, offsetof(Test, c)));
    __ Sdc1(f10, MemOperand(a0, offsetof(Test, d)));
    __ jr(ra);
    __ nop();
    Test test;
    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());
    for (int i = 0; i < kTableLength; i++) {
      test.a = inputs_D[i];
      test.b = inputs_S[i];
      (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
      if ((test.isNaN2008 & kFCSRNaN2008FlagMask) &&
              kArchVariant == kMips32r6) {
        CHECK_EQ(test.c, outputsNaN2008[i]);
      } else {
        CHECK_EQ(test.c, outputs[i]);
      }
      CHECK_EQ(test.d, test.c);
    }
  }
}


TEST(sub) {
  const int kTableLength = 12;
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    float a;
    float b;
    float resultS;
    double c;
    double d;
    double resultD;
  }TestFloat;

  TestFloat test;
  double inputfs_D[kTableLength] = {
    5.3, 4.8, 2.9, -5.3, -4.8, -2.9,
    5.3, 4.8, 2.9, -5.3, -4.8, -2.9
  };
  double inputft_D[kTableLength] = {
    4.8, 5.3, 2.9, 4.8, 5.3, 2.9,
    -4.8, -5.3, -2.9, -4.8, -5.3, -2.9
  };
  double outputs_D[kTableLength] = {
    0.5, -0.5, 0.0, -10.1, -10.1, -5.8,
    10.1, 10.1, 5.8, -0.5, 0.5, 0.0
  };
  float inputfs_S[kTableLength] = {
    5.3, 4.8, 2.9, -5.3, -4.8, -2.9,
    5.3, 4.8, 2.9, -5.3, -4.8, -2.9
  };
  float inputft_S[kTableLength] = {
    4.8, 5.3, 2.9, 4.8, 5.3, 2.9,
    -4.8, -5.3, -2.9, -4.8, -5.3, -2.9
  };
  float outputs_S[kTableLength] = {
    0.5, -0.5, 0.0, -10.1, -10.1, -5.8,
    10.1, 10.1, 5.8, -0.5, 0.5, 0.0
  };
  __ lwc1(f2, MemOperand(a0, offsetof(TestFloat, a)) );
  __ lwc1(f4, MemOperand(a0, offsetof(TestFloat, b)) );
  __ Ldc1(f8, MemOperand(a0, offsetof(TestFloat, c)));
  __ Ldc1(f10, MemOperand(a0, offsetof(TestFloat, d)));
  __ sub_s(f6, f2, f4);
  __ sub_d(f12, f8, f10);
  __ swc1(f6, MemOperand(a0, offsetof(TestFloat, resultS)) );
  __ Sdc1(f12, MemOperand(a0, offsetof(TestFloat, resultD)));
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  for (int i = 0; i < kTableLength; i++) {
    test.a = inputfs_S[i];
    test.b = inputft_S[i];
    test.c = inputfs_D[i];
    test.d = inputft_D[i];
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(test.resultS, outputs_S[i]);
    CHECK_EQ(test.resultD, outputs_D[i]);
  }
}


TEST(sqrt_rsqrt_recip) {
  const int kTableLength = 4;
  const double deltaDouble = 2E-15;
  const float deltaFloat = 2E-7;
  const float sqrt2_s = sqrt(2);
  const double sqrt2_d = sqrt(2);
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    float a;
    float resultS;
    float resultS1;
    float resultS2;
    double c;
    double resultD;
    double resultD1;
    double resultD2;
  }TestFloat;
  TestFloat test;

  double inputs_D[kTableLength] = {
    0.0L, 4.0L, 2.0L, 4e-28L
  };

  double outputs_D[kTableLength] = {
    0.0L, 2.0L, sqrt2_d, 2e-14L
  };
  float inputs_S[kTableLength] = {
    0.0, 4.0, 2.0, 4e-28
  };

  float outputs_S[kTableLength] = {
    0.0, 2.0, sqrt2_s, 2e-14
  };


  __ lwc1(f2, MemOperand(a0, offsetof(TestFloat, a)) );
  __ Ldc1(f8, MemOperand(a0, offsetof(TestFloat, c)));
  __ sqrt_s(f6, f2);
  __ sqrt_d(f12, f8);

  if (IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) {
    __ rsqrt_d(f14, f8);
    __ rsqrt_s(f16, f2);
    __ recip_d(f18, f8);
    __ recip_s(f4, f2);
  }
  __ swc1(f6, MemOperand(a0, offsetof(TestFloat, resultS)) );
  __ Sdc1(f12, MemOperand(a0, offsetof(TestFloat, resultD)));

  if (IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) {
    __ swc1(f16, MemOperand(a0, offsetof(TestFloat, resultS1)) );
    __ Sdc1(f14, MemOperand(a0, offsetof(TestFloat, resultD1)));
    __ swc1(f4, MemOperand(a0, offsetof(TestFloat, resultS2)) );
    __ Sdc1(f18, MemOperand(a0, offsetof(TestFloat, resultD2)));
  }
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());

  for (int i = 0; i < kTableLength; i++) {
    float f1;
    double d1;
    test.a = inputs_S[i];
    test.c = inputs_D[i];

    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));

    CHECK_EQ(test.resultS, outputs_S[i]);
    CHECK_EQ(test.resultD, outputs_D[i]);

    if (IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) {
      if (i != 0) {
        f1 = test.resultS1 - 1.0F/outputs_S[i];
        f1 = (f1 < 0) ? f1 : -f1;
        CHECK(f1 <= deltaFloat);
        d1 = test.resultD1 - 1.0L/outputs_D[i];
        d1 = (d1 < 0) ? d1 : -d1;
        CHECK(d1 <= deltaDouble);
        f1 = test.resultS2 - 1.0F/inputs_S[i];
        f1 = (f1 < 0) ? f1 : -f1;
        CHECK(f1 <= deltaFloat);
        d1 = test.resultD2 - 1.0L/inputs_D[i];
        d1 = (d1 < 0) ? d1 : -d1;
        CHECK(d1 <= deltaDouble);
      } else {
        CHECK_EQ(test.resultS1, 1.0F/outputs_S[i]);
        CHECK_EQ(test.resultD1, 1.0L/outputs_D[i]);
        CHECK_EQ(test.resultS2, 1.0F/inputs_S[i]);
        CHECK_EQ(test.resultD2, 1.0L/inputs_D[i]);
      }
    }
  }
}


TEST(neg) {
  const int kTableLength = 3;
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    float a;
    float resultS;
    double c;
    double resultD;
  }TestFloat;

  TestFloat test;
  double inputs_D[kTableLength] = {
    0.0, 4.0, -2.0
  };

  double outputs_D[kTableLength] = {
    0.0, -4.0, 2.0
  };
  float inputs_S[kTableLength] = {
    0.0, 4.0, -2.0
  };

  float outputs_S[kTableLength] = {
    0.0, -4.0, 2.0
  };
  __ lwc1(f2, MemOperand(a0, offsetof(TestFloat, a)) );
  __ Ldc1(f8, MemOperand(a0, offsetof(TestFloat, c)));
  __ neg_s(f6, f2);
  __ neg_d(f12, f8);
  __ swc1(f6, MemOperand(a0, offsetof(TestFloat, resultS)) );
  __ Sdc1(f12, MemOperand(a0, offsetof(TestFloat, resultD)));
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  for (int i = 0; i < kTableLength; i++) {
    test.a = inputs_S[i];
    test.c = inputs_D[i];
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(test.resultS, outputs_S[i]);
    CHECK_EQ(test.resultD, outputs_D[i]);
  }
}


TEST(mul) {
  const int kTableLength = 4;
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    float a;
    float b;
    float resultS;
    double c;
    double d;
    double resultD;
  }TestFloat;

  TestFloat test;
  double inputfs_D[kTableLength] = {
    5.3, -5.3, 5.3, -2.9
  };
  double inputft_D[kTableLength] = {
    4.8, 4.8, -4.8, -0.29
  };

  float inputfs_S[kTableLength] = {
    5.3, -5.3, 5.3, -2.9
  };
  float inputft_S[kTableLength] = {
    4.8, 4.8, -4.8, -0.29
  };

  __ lwc1(f2, MemOperand(a0, offsetof(TestFloat, a)) );
  __ lwc1(f4, MemOperand(a0, offsetof(TestFloat, b)) );
  __ Ldc1(f6, MemOperand(a0, offsetof(TestFloat, c)));
  __ Ldc1(f8, MemOperand(a0, offsetof(TestFloat, d)));
  __ mul_s(f10, f2, f4);
  __ mul_d(f12, f6, f8);
  __ swc1(f10, MemOperand(a0, offsetof(TestFloat, resultS)) );
  __ Sdc1(f12, MemOperand(a0, offsetof(TestFloat, resultD)));
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  for (int i = 0; i < kTableLength; i++) {
    test.a = inputfs_S[i];
    test.b = inputft_S[i];
    test.c = inputfs_D[i];
    test.d = inputft_D[i];
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(test.resultS, inputfs_S[i]*inputft_S[i]);
    CHECK_EQ(test.resultD, inputfs_D[i]*inputft_D[i]);
  }
}


TEST(mov) {
  const int kTableLength = 4;
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    double a;
    double b;
    float c;
    float d;
  }TestFloat;

  TestFloat test;
  double inputs_D[kTableLength] = {
    5.3, -5.3, 5.3, -2.9
  };
  double inputs_S[kTableLength] = {
    4.8, 4.8, -4.8, -0.29
  };

  float outputs_S[kTableLength] = {
    4.8, 4.8, -4.8, -0.29
  };
  double outputs_D[kTableLength] = {
    5.3, -5.3, 5.3, -2.9
  };

  __ Ldc1(f4, MemOperand(a0, offsetof(TestFloat, a)));
  __ lwc1(f6, MemOperand(a0, offsetof(TestFloat, c)) );
  __ mov_s(f8, f6);
  __ mov_d(f10, f4);
  __ swc1(f8, MemOperand(a0, offsetof(TestFloat, d)) );
  __ Sdc1(f10, MemOperand(a0, offsetof(TestFloat, b)));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  for (int i = 0; i < kTableLength; i++) {
    test.a = inputs_D[i];
    test.c = inputs_S[i];

    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(test.b, outputs_D[i]);
    CHECK_EQ(test.d, outputs_S[i]);
  }
}


TEST(floor_w) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    uint32_t isNaN2008;
    double a;
    float b;
    int32_t c;  // a floor result
    int32_t d;  // b floor result
  }Test;
  const int kTableLength = 15;
  double inputs_D[kTableLength] = {
      2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
      -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
      2147483648.0,
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::infinity()
      };
  float inputs_S[kTableLength] = {
      2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
      -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
      2147483648.0,
      std::numeric_limits<float>::quiet_NaN(),
      std::numeric_limits<float>::infinity()
      };
  double outputs[kTableLength] = {
      2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
      -3.0, -3.0, -3.0, -4.0, -4.0, -4.0,
      kFPUInvalidResult, kFPUInvalidResult,
      kFPUInvalidResult};
  double outputsNaN2008[kTableLength] = {
      2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
      -3.0, -3.0, -3.0, -4.0, -4.0, -4.0,
      kFPUInvalidResult,
      0,
      kFPUInvalidResult};

  __ cfc1(t1, FCSR);
  __ sw(t1, MemOperand(a0, offsetof(Test, isNaN2008)));
  __ Ldc1(f4, MemOperand(a0, offsetof(Test, a)));
  __ lwc1(f6, MemOperand(a0, offsetof(Test, b)) );
  __ floor_w_d(f8, f4);
  __ floor_w_s(f10, f6);
  __ swc1(f8, MemOperand(a0, offsetof(Test, c)) );
  __ swc1(f10, MemOperand(a0, offsetof(Test, d)) );
  __ jr(ra);
  __ nop();
  Test test;
  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  for (int i = 0; i < kTableLength; i++) {
    test.a = inputs_D[i];
    test.b = inputs_S[i];
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    if ((test.isNaN2008 & kFCSRNaN2008FlagMask) && kArchVariant == kMips32r6) {
      CHECK_EQ(test.c, outputsNaN2008[i]);
    } else {
      CHECK_EQ(test.c, outputs[i]);
    }
    CHECK_EQ(test.d, test.c);
  }
}


TEST(floor_l) {
  if (IsFp64Mode()) {
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);
    const double dFPU64InvalidResult = static_cast<double>(kFPU64InvalidResult);
    typedef struct test_float {
      uint32_t isNaN2008;
      double a;
      float b;
      int64_t c;
      int64_t d;
    }Test;
    const int kTableLength = 15;
    double inputs_D[kTableLength] = {
        2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
        -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
        2147483648.0,
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::infinity()
        };
    float inputs_S[kTableLength] = {
        2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
        -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
        2147483648.0,
        std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::infinity()
        };
    double outputs[kTableLength] = {
        2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
        -3.0, -3.0, -3.0, -4.0, -4.0, -4.0,
        2147483648.0, dFPU64InvalidResult,
        dFPU64InvalidResult};
    double outputsNaN2008[kTableLength] = {
        2.0, 2.0, 2.0, 3.0, 3.0, 3.0,
        -3.0, -3.0, -3.0, -4.0, -4.0, -4.0,
        2147483648.0,
        0,
        dFPU64InvalidResult};

    __ cfc1(t1, FCSR);
    __ sw(t1, MemOperand(a0, offsetof(Test, isNaN2008)));
    __ Ldc1(f4, MemOperand(a0, offsetof(Test, a)));
    __ lwc1(f6, MemOperand(a0, offsetof(Test, b)) );
    __ floor_l_d(f8, f4);
    __ floor_l_s(f10, f6);
    __ Sdc1(f8, MemOperand(a0, offsetof(Test, c)));
    __ Sdc1(f10, MemOperand(a0, offsetof(Test, d)));
    __ jr(ra);
    __ nop();
    Test test;
    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());
    for (int i = 0; i < kTableLength; i++) {
      test.a = inputs_D[i];
      test.b = inputs_S[i];
      (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
      if ((test.isNaN2008 & kFCSRNaN2008FlagMask) &&
              kArchVariant == kMips32r6) {
        CHECK_EQ(test.c, outputsNaN2008[i]);
      } else {
        CHECK_EQ(test.c, outputs[i]);
      }
      CHECK_EQ(test.d, test.c);
    }
  }
}


TEST(ceil_w) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    uint32_t isNaN2008;
    double a;
    float b;
    int32_t c;  // a floor result
    int32_t d;  // b floor result
  }Test;
  const int kTableLength = 15;
  double inputs_D[kTableLength] = {
      2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
      -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
      2147483648.0,
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::infinity()
      };
  float inputs_S[kTableLength] = {
      2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
      -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
      2147483648.0,
      std::numeric_limits<float>::quiet_NaN(),
      std::numeric_limits<float>::infinity()
      };
  double outputs[kTableLength] = {
      3.0, 3.0, 3.0, 4.0, 4.0, 4.0,
      -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
      kFPUInvalidResult, kFPUInvalidResult,
      kFPUInvalidResult};
  double outputsNaN2008[kTableLength] = {
      3.0, 3.0, 3.0, 4.0, 4.0, 4.0,
      -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
      kFPUInvalidResult,
      0,
      kFPUInvalidResult};

  __ cfc1(t1, FCSR);
  __ sw(t1, MemOperand(a0, offsetof(Test, isNaN2008)));
  __ Ldc1(f4, MemOperand(a0, offsetof(Test, a)));
  __ lwc1(f6, MemOperand(a0, offsetof(Test, b)) );
  __ ceil_w_d(f8, f4);
  __ ceil_w_s(f10, f6);
  __ swc1(f8, MemOperand(a0, offsetof(Test, c)) );
  __ swc1(f10, MemOperand(a0, offsetof(Test, d)) );
  __ jr(ra);
  __ nop();
  Test test;
  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  for (int i = 0; i < kTableLength; i++) {
    test.a = inputs_D[i];
    test.b = inputs_S[i];
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    if ((test.isNaN2008 & kFCSRNaN2008FlagMask) && kArchVariant == kMips32r6) {
      CHECK_EQ(test.c, outputsNaN2008[i]);
    } else {
      CHECK_EQ(test.c, outputs[i]);
    }
    CHECK_EQ(test.d, test.c);
  }
}


TEST(ceil_l) {
  if (IsFp64Mode()) {
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);
    const double dFPU64InvalidResult = static_cast<double>(kFPU64InvalidResult);
    typedef struct test_float {
      uint32_t isNaN2008;
      double a;
      float b;
      int64_t c;
      int64_t d;
    }Test;
    const int kTableLength = 15;
    double inputs_D[kTableLength] = {
        2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
        -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
        2147483648.0,
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::infinity()
        };
    float inputs_S[kTableLength] = {
        2.1, 2.6, 2.5, 3.1, 3.6, 3.5,
        -2.1, -2.6, -2.5, -3.1, -3.6, -3.5,
        2147483648.0,
        std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::infinity()
        };
    double outputs[kTableLength] = {
        3.0, 3.0, 3.0, 4.0, 4.0, 4.0,
        -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
        2147483648.0, dFPU64InvalidResult,
        dFPU64InvalidResult};
    double outputsNaN2008[kTableLength] = {
        3.0, 3.0, 3.0, 4.0, 4.0, 4.0,
        -2.0, -2.0, -2.0, -3.0, -3.0, -3.0,
        2147483648.0,
        0,
        dFPU64InvalidResult};

    __ cfc1(t1, FCSR);
    __ sw(t1, MemOperand(a0, offsetof(Test, isNaN2008)));
    __ Ldc1(f4, MemOperand(a0, offsetof(Test, a)));
    __ lwc1(f6, MemOperand(a0, offsetof(Test, b)) );
    __ ceil_l_d(f8, f4);
    __ ceil_l_s(f10, f6);
    __ Sdc1(f8, MemOperand(a0, offsetof(Test, c)));
    __ Sdc1(f10, MemOperand(a0, offsetof(Test, d)));
    __ jr(ra);
    __ nop();
    Test test;
    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());
    for (int i = 0; i < kTableLength; i++) {
      test.a = inputs_D[i];
      test.b = inputs_S[i];
      (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
      if ((test.isNaN2008 & kFCSRNaN2008FlagMask) &&
              kArchVariant == kMips32r6) {
        CHECK_EQ(test.c, outputsNaN2008[i]);
      } else {
        CHECK_EQ(test.c, outputs[i]);
      }
      CHECK_EQ(test.d, test.c);
    }
  }
}


TEST(jump_tables1) {
  // Test jump tables with forward jumps.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  Assembler assm(isolate, nullptr, 0);

  const int kNumCases = 512;
  int values[kNumCases];
  isolate->random_number_generator()->NextBytes(values, sizeof(values));
  Label labels[kNumCases];

  __ addiu(sp, sp, -4);
  __ sw(ra, MemOperand(sp));

  Label done;
  {
    __ BlockTrampolinePoolFor(kNumCases + 7);
    PredictableCodeSizeScope predictable(
        &assm, (kNumCases + 7) * Assembler::kInstrSize);
    Label here;

    __ bal(&here);
    __ nop();
    __ bind(&here);
    __ sll(at, a0, 2);
    __ addu(at, at, ra);
    __ lw(at, MemOperand(at, 5 * Assembler::kInstrSize));
    __ jr(at);
    __ nop();
    for (int i = 0; i < kNumCases; ++i) {
      __ dd(&labels[i]);
    }
  }

  for (int i = 0; i < kNumCases; ++i) {
    __ bind(&labels[i]);
    __ lui(v0, (values[i] >> 16) & 0xffff);
    __ ori(v0, v0, values[i] & 0xffff);
    __ b(&done);
    __ nop();
  }

  __ bind(&done);
  __ lw(ra, MemOperand(sp));
  __ addiu(sp, sp, 4);
  __ jr(ra);
  __ nop();

  CHECK_EQ(0, assm.UnboundLabelsCount());

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F1 f = FUNCTION_CAST<F1>(code->entry());
  for (int i = 0; i < kNumCases; ++i) {
    int res = reinterpret_cast<int>(
        CALL_GENERATED_CODE(isolate, f, i, 0, 0, 0, 0));
    ::printf("f(%d) = %d\n", i, res);
    CHECK_EQ(values[i], res);
  }
}


TEST(jump_tables2) {
  // Test jump tables with backward jumps.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  Assembler assm(isolate, nullptr, 0);

  const int kNumCases = 512;
  int values[kNumCases];
  isolate->random_number_generator()->NextBytes(values, sizeof(values));
  Label labels[kNumCases];

  __ addiu(sp, sp, -4);
  __ sw(ra, MemOperand(sp));

  Label done, dispatch;
  __ b(&dispatch);
  __ nop();

  for (int i = 0; i < kNumCases; ++i) {
    __ bind(&labels[i]);
    __ lui(v0, (values[i] >> 16) & 0xffff);
    __ ori(v0, v0, values[i] & 0xffff);
    __ b(&done);
    __ nop();
  }

  __ bind(&dispatch);
  {
    __ BlockTrampolinePoolFor(kNumCases + 7);
    PredictableCodeSizeScope predictable(
        &assm, (kNumCases + 7) * Assembler::kInstrSize);
    Label here;

    __ bal(&here);
    __ nop();
    __ bind(&here);
    __ sll(at, a0, 2);
    __ addu(at, at, ra);
    __ lw(at, MemOperand(at, 5 * Assembler::kInstrSize));
    __ jr(at);
    __ nop();
    for (int i = 0; i < kNumCases; ++i) {
      __ dd(&labels[i]);
    }
  }

  __ bind(&done);
  __ lw(ra, MemOperand(sp));
  __ addiu(sp, sp, 4);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F1 f = FUNCTION_CAST<F1>(code->entry());
  for (int i = 0; i < kNumCases; ++i) {
    int res = reinterpret_cast<int>(
        CALL_GENERATED_CODE(isolate, f, i, 0, 0, 0, 0));
    ::printf("f(%d) = %d\n", i, res);
    CHECK_EQ(values[i], res);
  }
}


TEST(jump_tables3) {
  // Test jump tables with backward jumps and embedded heap objects.
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  Assembler assm(isolate, nullptr, 0);

  const int kNumCases = 256;
  Handle<Object> values[kNumCases];
  for (int i = 0; i < kNumCases; ++i) {
    double value = isolate->random_number_generator()->NextDouble();
    values[i] = isolate->factory()->NewHeapNumber(value, IMMUTABLE, TENURED);
  }
  Label labels[kNumCases];
  Object* obj;
  int32_t imm32;

  __ addiu(sp, sp, -4);
  __ sw(ra, MemOperand(sp));

  Label done, dispatch;
  __ b(&dispatch);


  for (int i = 0; i < kNumCases; ++i) {
    __ bind(&labels[i]);
    obj = *values[i];
    imm32 = reinterpret_cast<intptr_t>(obj);
    __ lui(v0, (imm32 >> 16) & 0xffff);
    __ ori(v0, v0, imm32 & 0xffff);
    __ b(&done);
    __ nop();
  }

  __ bind(&dispatch);
  {
    __ BlockTrampolinePoolFor(kNumCases + 7);
    PredictableCodeSizeScope predictable(
        &assm, (kNumCases + 7) * Assembler::kInstrSize);
    Label here;

    __ bal(&here);
    __ nop();
    __ bind(&here);
    __ sll(at, a0, 2);
    __ addu(at, at, ra);
    __ lw(at, MemOperand(at, 5 * Assembler::kInstrSize));
    __ jr(at);
    __ nop();
    for (int i = 0; i < kNumCases; ++i) {
      __ dd(&labels[i]);
    }
  }

  __ bind(&done);
  __ lw(ra, MemOperand(sp));
  __ addiu(sp, sp, 4);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F1 f = FUNCTION_CAST<F1>(code->entry());
  for (int i = 0; i < kNumCases; ++i) {
    Handle<Object> result(
        CALL_GENERATED_CODE(isolate, f, i, 0, 0, 0, 0), isolate);
#ifdef OBJECT_PRINT
    ::printf("f(%d) = ", i);
    result->Print(std::cout);
    ::printf("\n");
#endif
    CHECK(values[i].is_identical_to(result));
  }
}


TEST(BITSWAP) {
  // Test BITSWAP
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);

    typedef struct {
      int32_t r1;
      int32_t r2;
      int32_t r3;
      int32_t r4;
    } T;
    T t;

    Assembler assm(isolate, NULL, 0);

    __ lw(a2, MemOperand(a0, offsetof(T, r1)));
    __ nop();
    __ bitswap(a1, a2);
    __ sw(a1, MemOperand(a0, offsetof(T, r1)));

    __ lw(a2, MemOperand(a0, offsetof(T, r2)));
    __ nop();
    __ bitswap(a1, a2);
    __ sw(a1, MemOperand(a0, offsetof(T, r2)));

    __ jr(ra);
    __ nop();

    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());
    t.r1 = 0x781A15C3;
    t.r2 = 0x8B71FCDE;
    Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
    USE(dummy);

    CHECK_EQ(static_cast<int32_t>(0x1E58A8C3), t.r1);
    CHECK_EQ(static_cast<int32_t>(0xD18E3F7B), t.r2);
  }
}


TEST(class_fmt) {
  if (IsMipsArchVariant(kMips32r6)) {
    // Test CLASS.fmt instruction.
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);

    typedef struct {
      double dSignalingNan;
      double dQuietNan;
      double dNegInf;
      double dNegNorm;
      double dNegSubnorm;
      double dNegZero;
      double dPosInf;
      double dPosNorm;
      double dPosSubnorm;
      double dPosZero;
      float  fSignalingNan;
      float  fQuietNan;
      float  fNegInf;
      float  fNegNorm;
      float  fNegSubnorm;
      float  fNegZero;
      float  fPosInf;
      float  fPosNorm;
      float  fPosSubnorm;
      float  fPosZero;  } T;
    T t;

    // Create a function that accepts &t, and loads, manipulates, and stores
    // the doubles t.a ... t.f.
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);

    __ Ldc1(f4, MemOperand(a0, offsetof(T, dSignalingNan)));
    __ class_d(f6, f4);
    __ Sdc1(f6, MemOperand(a0, offsetof(T, dSignalingNan)));

    __ Ldc1(f4, MemOperand(a0, offsetof(T, dQuietNan)));
    __ class_d(f6, f4);
    __ Sdc1(f6, MemOperand(a0, offsetof(T, dQuietNan)));

    __ Ldc1(f4, MemOperand(a0, offsetof(T, dNegInf)));
    __ class_d(f6, f4);
    __ Sdc1(f6, MemOperand(a0, offsetof(T, dNegInf)));

    __ Ldc1(f4, MemOperand(a0, offsetof(T, dNegNorm)));
    __ class_d(f6, f4);
    __ Sdc1(f6, MemOperand(a0, offsetof(T, dNegNorm)));

    __ Ldc1(f4, MemOperand(a0, offsetof(T, dNegSubnorm)));
    __ class_d(f6, f4);
    __ Sdc1(f6, MemOperand(a0, offsetof(T, dNegSubnorm)));

    __ Ldc1(f4, MemOperand(a0, offsetof(T, dNegZero)));
    __ class_d(f6, f4);
    __ Sdc1(f6, MemOperand(a0, offsetof(T, dNegZero)));

    __ Ldc1(f4, MemOperand(a0, offsetof(T, dPosInf)));
    __ class_d(f6, f4);
    __ Sdc1(f6, MemOperand(a0, offsetof(T, dPosInf)));

    __ Ldc1(f4, MemOperand(a0, offsetof(T, dPosNorm)));
    __ class_d(f6, f4);
    __ Sdc1(f6, MemOperand(a0, offsetof(T, dPosNorm)));

    __ Ldc1(f4, MemOperand(a0, offsetof(T, dPosSubnorm)));
    __ class_d(f6, f4);
    __ Sdc1(f6, MemOperand(a0, offsetof(T, dPosSubnorm)));

    __ Ldc1(f4, MemOperand(a0, offsetof(T, dPosZero)));
    __ class_d(f6, f4);
    __ Sdc1(f6, MemOperand(a0, offsetof(T, dPosZero)));

    // Testing instruction CLASS.S
    __ lwc1(f4, MemOperand(a0, offsetof(T, fSignalingNan)));
    __ class_s(f6, f4);
    __ swc1(f6, MemOperand(a0, offsetof(T, fSignalingNan)));

    __ lwc1(f4, MemOperand(a0, offsetof(T, fQuietNan)));
    __ class_s(f6, f4);
    __ swc1(f6, MemOperand(a0, offsetof(T, fQuietNan)));

    __ lwc1(f4, MemOperand(a0, offsetof(T, fNegInf)));
    __ class_s(f6, f4);
    __ swc1(f6, MemOperand(a0, offsetof(T, fNegInf)));

    __ lwc1(f4, MemOperand(a0, offsetof(T, fNegNorm)));
    __ class_s(f6, f4);
    __ swc1(f6, MemOperand(a0, offsetof(T, fNegNorm)));

    __ lwc1(f4, MemOperand(a0, offsetof(T, fNegSubnorm)));
    __ class_s(f6, f4);
    __ swc1(f6, MemOperand(a0, offsetof(T, fNegSubnorm)));

    __ lwc1(f4, MemOperand(a0, offsetof(T, fNegZero)));
    __ class_s(f6, f4);
    __ swc1(f6, MemOperand(a0, offsetof(T, fNegZero)));

    __ lwc1(f4, MemOperand(a0, offsetof(T, fPosInf)));
    __ class_s(f6, f4);
    __ swc1(f6, MemOperand(a0, offsetof(T, fPosInf)));

    __ lwc1(f4, MemOperand(a0, offsetof(T, fPosNorm)));
    __ class_s(f6, f4);
    __ swc1(f6, MemOperand(a0, offsetof(T, fPosNorm)));

    __ lwc1(f4, MemOperand(a0, offsetof(T, fPosSubnorm)));
    __ class_s(f6, f4);
    __ swc1(f6, MemOperand(a0, offsetof(T, fPosSubnorm)));

    __ lwc1(f4, MemOperand(a0, offsetof(T, fPosZero)));
    __ class_s(f6, f4);
    __ swc1(f6, MemOperand(a0, offsetof(T, fPosZero)));

    __ jr(ra);
    __ nop();

    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());

    t.dSignalingNan =  std::numeric_limits<double>::signaling_NaN();
    t.dQuietNan = std::numeric_limits<double>::quiet_NaN();
    t.dNegInf       = -1.0 / 0.0;
    t.dNegNorm      = -5.0;
    t.dNegSubnorm   = -DBL_MIN / 2.0;
    t.dNegZero      = -0.0;
    t.dPosInf       = 2.0 / 0.0;
    t.dPosNorm      = 275.35;
    t.dPosSubnorm   = DBL_MIN / 2.0;
    t.dPosZero      = +0.0;
    // Float test values

    t.fSignalingNan = std::numeric_limits<float>::signaling_NaN();
    t.fQuietNan     = std::numeric_limits<float>::quiet_NaN();
    t.fNegInf       = -0.5/0.0;
    t.fNegNorm      = -FLT_MIN;
    t.fNegSubnorm   = -FLT_MIN / 1.5;
    t.fNegZero      = -0.0;
    t.fPosInf       = 100000.0 / 0.0;
    t.fPosNorm      = FLT_MAX;
    t.fPosSubnorm   = FLT_MIN / 20.0;
    t.fPosZero      = +0.0;

    Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
    USE(dummy);
    // Expected double results.
    CHECK_EQ(bit_cast<int64_t>(t.dSignalingNan), 0x001);
    CHECK_EQ(bit_cast<int64_t>(t.dQuietNan),     0x002);
    CHECK_EQ(bit_cast<int64_t>(t.dNegInf),       0x004);
    CHECK_EQ(bit_cast<int64_t>(t.dNegNorm),      0x008);
    CHECK_EQ(bit_cast<int64_t>(t.dNegSubnorm),   0x010);
    CHECK_EQ(bit_cast<int64_t>(t.dNegZero),      0x020);
    CHECK_EQ(bit_cast<int64_t>(t.dPosInf),       0x040);
    CHECK_EQ(bit_cast<int64_t>(t.dPosNorm),      0x080);
    CHECK_EQ(bit_cast<int64_t>(t.dPosSubnorm),   0x100);
    CHECK_EQ(bit_cast<int64_t>(t.dPosZero),      0x200);

    // Expected float results.
    CHECK_EQ(bit_cast<int32_t>(t.fSignalingNan), 0x001);
    CHECK_EQ(bit_cast<int32_t>(t.fQuietNan),     0x002);
    CHECK_EQ(bit_cast<int32_t>(t.fNegInf),       0x004);
    CHECK_EQ(bit_cast<int32_t>(t.fNegNorm),      0x008);
    CHECK_EQ(bit_cast<int32_t>(t.fNegSubnorm),   0x010);
    CHECK_EQ(bit_cast<int32_t>(t.fNegZero),      0x020);
    CHECK_EQ(bit_cast<int32_t>(t.fPosInf),       0x040);
    CHECK_EQ(bit_cast<int32_t>(t.fPosNorm),      0x080);
    CHECK_EQ(bit_cast<int32_t>(t.fPosSubnorm),   0x100);
    CHECK_EQ(bit_cast<int32_t>(t.fPosZero),      0x200);
  }
}


TEST(ABS) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    int64_t fir;
    double a;
    float b;
    double fcsr;
  } TestFloat;

  TestFloat test;

  // Save FIR.
  __ cfc1(a1, FCSR);
  // Disable FPU exceptions.
  __ ctc1(zero_reg, FCSR);

  __ Ldc1(f4, MemOperand(a0, offsetof(TestFloat, a)));
  __ abs_d(f10, f4);
  __ Sdc1(f10, MemOperand(a0, offsetof(TestFloat, a)));

  __ lwc1(f4, MemOperand(a0, offsetof(TestFloat, b)));
  __ abs_s(f10, f4);
  __ swc1(f10, MemOperand(a0, offsetof(TestFloat, b)));

  // Restore FCSR.
  __ ctc1(a1, FCSR);

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  test.a = -2.0;
  test.b = -2.0;
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.a, 2.0);
  CHECK_EQ(test.b, 2.0);

  test.a = 2.0;
  test.b = 2.0;
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.a, 2.0);
  CHECK_EQ(test.b, 2.0);

  // Testing biggest positive number
  test.a = std::numeric_limits<double>::max();
  test.b = std::numeric_limits<float>::max();
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.a, std::numeric_limits<double>::max());
  CHECK_EQ(test.b, std::numeric_limits<float>::max());

  // Testing smallest negative number
  test.a = -std::numeric_limits<double>::max();  // lowest()
  test.b = -std::numeric_limits<float>::max();   // lowest()
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.a, std::numeric_limits<double>::max());
  CHECK_EQ(test.b, std::numeric_limits<float>::max());

  // Testing smallest positive number
  test.a = -std::numeric_limits<double>::min();
  test.b = -std::numeric_limits<float>::min();
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.a, std::numeric_limits<double>::min());
  CHECK_EQ(test.b, std::numeric_limits<float>::min());

  // Testing infinity
  test.a = -std::numeric_limits<double>::max()
          / std::numeric_limits<double>::min();
  test.b = -std::numeric_limits<float>::max()
          / std::numeric_limits<float>::min();
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.a, std::numeric_limits<double>::max()
                 / std::numeric_limits<double>::min());
  CHECK_EQ(test.b, std::numeric_limits<float>::max()
                 / std::numeric_limits<float>::min());

  test.a = std::numeric_limits<double>::quiet_NaN();
  test.b = std::numeric_limits<float>::quiet_NaN();
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK(std::isnan(test.a));
  CHECK(std::isnan(test.b));

  test.a = std::numeric_limits<double>::signaling_NaN();
  test.b = std::numeric_limits<float>::signaling_NaN();
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK(std::isnan(test.a));
  CHECK(std::isnan(test.b));
}


TEST(ADD_FMT) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    double a;
    double b;
    double c;
    float fa;
    float fb;
    float fc;
  } TestFloat;

  TestFloat test;

  __ Ldc1(f4, MemOperand(a0, offsetof(TestFloat, a)));
  __ Ldc1(f8, MemOperand(a0, offsetof(TestFloat, b)));
  __ add_d(f10, f8, f4);
  __ Sdc1(f10, MemOperand(a0, offsetof(TestFloat, c)));

  __ lwc1(f4, MemOperand(a0, offsetof(TestFloat, fa)));
  __ lwc1(f8, MemOperand(a0, offsetof(TestFloat, fb)));
  __ add_s(f10, f8, f4);
  __ swc1(f10, MemOperand(a0, offsetof(TestFloat, fc)));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());
  test.a = 2.0;
  test.b = 3.0;
  test.fa = 2.0;
  test.fb = 3.0;
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.c, 5.0);
  CHECK_EQ(test.fc, 5.0);

  test.a = std::numeric_limits<double>::max();
  test.b = -std::numeric_limits<double>::max();  // lowest()
  test.fa = std::numeric_limits<float>::max();
  test.fb = -std::numeric_limits<float>::max();  // lowest()
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.c, 0.0);
  CHECK_EQ(test.fc, 0.0);

  test.a = std::numeric_limits<double>::max();
  test.b = std::numeric_limits<double>::max();
  test.fa = std::numeric_limits<float>::max();
  test.fb = std::numeric_limits<float>::max();
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK(!std::isfinite(test.c));
  CHECK(!std::isfinite(test.fc));

  test.a = 5.0;
  test.b = std::numeric_limits<double>::signaling_NaN();
  test.fa = 5.0;
  test.fb = std::numeric_limits<float>::signaling_NaN();
  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK(std::isnan(test.c));
  CHECK(std::isnan(test.fc));
}


TEST(C_COND_FMT) {
  if ((IsMipsArchVariant(kMips32r1)) || (IsMipsArchVariant(kMips32r2))) {
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);

    typedef struct test_float {
      double dOp1;
      double dOp2;
      uint32_t dF;
      uint32_t dUn;
      uint32_t dEq;
      uint32_t dUeq;
      uint32_t dOlt;
      uint32_t dUlt;
      uint32_t dOle;
      uint32_t dUle;
      float fOp1;
      float fOp2;
      uint32_t fF;
      uint32_t fUn;
      uint32_t fEq;
      uint32_t fUeq;
      uint32_t fOlt;
      uint32_t fUlt;
      uint32_t fOle;
      uint32_t fUle;
    } TestFloat;

    TestFloat test;

    __ li(t1, 1);

    __ Ldc1(f4, MemOperand(a0, offsetof(TestFloat, dOp1)));
    __ Ldc1(f6, MemOperand(a0, offsetof(TestFloat, dOp2)));

    __ lwc1(f14, MemOperand(a0, offsetof(TestFloat, fOp1)));
    __ lwc1(f16, MemOperand(a0, offsetof(TestFloat, fOp2)));

    __ mov(t2, zero_reg);
    __ mov(t3, zero_reg);
    __ c_d(F, f4, f6, 0);
    __ c_s(F, f14, f16, 2);
    __ movt(t2, t1, 0);
    __ movt(t3, t1, 2);
    __ sw(t2, MemOperand(a0, offsetof(TestFloat, dF)) );
    __ sw(t3, MemOperand(a0, offsetof(TestFloat, fF)) );

    __ mov(t2, zero_reg);
    __ mov(t3, zero_reg);
    __ c_d(UN, f4, f6, 2);
    __ c_s(UN, f14, f16, 4);
    __ movt(t2, t1, 2);
    __ movt(t3, t1, 4);
    __ sw(t2, MemOperand(a0, offsetof(TestFloat, dUn)) );
    __ sw(t3, MemOperand(a0, offsetof(TestFloat, fUn)) );

    __ mov(t2, zero_reg);
    __ mov(t3, zero_reg);
    __ c_d(EQ, f4, f6, 4);
    __ c_s(EQ, f14, f16, 6);
    __ movt(t2, t1, 4);
    __ movt(t3, t1, 6);
    __ sw(t2, MemOperand(a0, offsetof(TestFloat, dEq)) );
    __ sw(t3, MemOperand(a0, offsetof(TestFloat, fEq)) );

    __ mov(t2, zero_reg);
    __ mov(t3, zero_reg);
    __ c_d(UEQ, f4, f6, 6);
    __ c_s(UEQ, f14, f16, 0);
    __ movt(t2, t1, 6);
    __ movt(t3, t1, 0);
    __ sw(t2, MemOperand(a0, offsetof(TestFloat, dUeq)) );
    __ sw(t3, MemOperand(a0, offsetof(TestFloat, fUeq)) );

    __ mov(t2, zero_reg);
    __ mov(t3, zero_reg);
    __ c_d(OLT, f4, f6, 0);
    __ c_s(OLT, f14, f16, 2);
    __ movt(t2, t1, 0);
    __ movt(t3, t1, 2);
    __ sw(t2, MemOperand(a0, offsetof(TestFloat, dOlt)) );
    __ sw(t3, MemOperand(a0, offsetof(TestFloat, fOlt)) );

    __ mov(t2, zero_reg);
    __ mov(t3, zero_reg);
    __ c_d(ULT, f4, f6, 2);
    __ c_s(ULT, f14, f16, 4);
    __ movt(t2, t1, 2);
    __ movt(t3, t1, 4);
    __ sw(t2, MemOperand(a0, offsetof(TestFloat, dUlt)) );
    __ sw(t3, MemOperand(a0, offsetof(TestFloat, fUlt)) );

    __ mov(t2, zero_reg);
    __ mov(t3, zero_reg);
    __ c_d(OLE, f4, f6, 4);
    __ c_s(OLE, f14, f16, 6);
    __ movt(t2, t1, 4);
    __ movt(t3, t1, 6);
    __ sw(t2, MemOperand(a0, offsetof(TestFloat, dOle)) );
    __ sw(t3, MemOperand(a0, offsetof(TestFloat, fOle)) );

    __ mov(t2, zero_reg);
    __ mov(t3, zero_reg);
    __ c_d(ULE, f4, f6, 6);
    __ c_s(ULE, f14, f16, 0);
    __ movt(t2, t1, 6);
    __ movt(t3, t1, 0);
    __ sw(t2, MemOperand(a0, offsetof(TestFloat, dUle)) );
    __ sw(t3, MemOperand(a0, offsetof(TestFloat, fUle)) );

    __ jr(ra);
    __ nop();

    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());
    test.dOp1 = 2.0;
    test.dOp2 = 3.0;
    test.fOp1 = 2.0;
    test.fOp2 = 3.0;
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(test.dF, 0U);
    CHECK_EQ(test.dUn, 0U);
    CHECK_EQ(test.dEq, 0U);
    CHECK_EQ(test.dUeq, 0U);
    CHECK_EQ(test.dOlt, 1U);
    CHECK_EQ(test.dUlt, 1U);
    CHECK_EQ(test.dOle, 1U);
    CHECK_EQ(test.dUle, 1U);
    CHECK_EQ(test.fF, 0U);
    CHECK_EQ(test.fUn, 0U);
    CHECK_EQ(test.fEq, 0U);
    CHECK_EQ(test.fUeq, 0U);
    CHECK_EQ(test.fOlt, 1U);
    CHECK_EQ(test.fUlt, 1U);
    CHECK_EQ(test.fOle, 1U);
    CHECK_EQ(test.fUle, 1U);

    test.dOp1 = std::numeric_limits<double>::max();
    test.dOp2 = std::numeric_limits<double>::min();
    test.fOp1 = std::numeric_limits<float>::min();
    test.fOp2 = -std::numeric_limits<float>::max();  // lowest()
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(test.dF, 0U);
    CHECK_EQ(test.dUn, 0U);
    CHECK_EQ(test.dEq, 0U);
    CHECK_EQ(test.dUeq, 0U);
    CHECK_EQ(test.dOlt, 0U);
    CHECK_EQ(test.dUlt, 0U);
    CHECK_EQ(test.dOle, 0U);
    CHECK_EQ(test.dUle, 0U);
    CHECK_EQ(test.fF, 0U);
    CHECK_EQ(test.fUn, 0U);
    CHECK_EQ(test.fEq, 0U);
    CHECK_EQ(test.fUeq, 0U);
    CHECK_EQ(test.fOlt, 0U);
    CHECK_EQ(test.fUlt, 0U);
    CHECK_EQ(test.fOle, 0U);
    CHECK_EQ(test.fUle, 0U);

    test.dOp1 = -std::numeric_limits<double>::max();  // lowest()
    test.dOp2 = -std::numeric_limits<double>::max();  // lowest()
    test.fOp1 = std::numeric_limits<float>::max();
    test.fOp2 = std::numeric_limits<float>::max();
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(test.dF, 0U);
    CHECK_EQ(test.dUn, 0U);
    CHECK_EQ(test.dEq, 1U);
    CHECK_EQ(test.dUeq, 1U);
    CHECK_EQ(test.dOlt, 0U);
    CHECK_EQ(test.dUlt, 0U);
    CHECK_EQ(test.dOle, 1U);
    CHECK_EQ(test.dUle, 1U);
    CHECK_EQ(test.fF, 0U);
    CHECK_EQ(test.fUn, 0U);
    CHECK_EQ(test.fEq, 1U);
    CHECK_EQ(test.fUeq, 1U);
    CHECK_EQ(test.fOlt, 0U);
    CHECK_EQ(test.fUlt, 0U);
    CHECK_EQ(test.fOle, 1U);
    CHECK_EQ(test.fUle, 1U);

    test.dOp1 = std::numeric_limits<double>::quiet_NaN();
    test.dOp2 = 0.0;
    test.fOp1 = std::numeric_limits<float>::quiet_NaN();
    test.fOp2 = 0.0;
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(test.dF, 0U);
    CHECK_EQ(test.dUn, 1U);
    CHECK_EQ(test.dEq, 0U);
    CHECK_EQ(test.dUeq, 1U);
    CHECK_EQ(test.dOlt, 0U);
    CHECK_EQ(test.dUlt, 1U);
    CHECK_EQ(test.dOle, 0U);
    CHECK_EQ(test.dUle, 1U);
    CHECK_EQ(test.fF, 0U);
    CHECK_EQ(test.fUn, 1U);
    CHECK_EQ(test.fEq, 0U);
    CHECK_EQ(test.fUeq, 1U);
    CHECK_EQ(test.fOlt, 0U);
    CHECK_EQ(test.fUlt, 1U);
    CHECK_EQ(test.fOle, 0U);
    CHECK_EQ(test.fUle, 1U);
  }
}


TEST(CMP_COND_FMT) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();
    Isolate* isolate = CcTest::i_isolate();
    HandleScope scope(isolate);
    MacroAssembler assm(isolate, NULL, 0,
                        v8::internal::CodeObjectRequired::kYes);

    typedef struct test_float {
      double dOp1;
      double dOp2;
      double dF;
      double dUn;
      double dEq;
      double dUeq;
      double dOlt;
      double dUlt;
      double dOle;
      double dUle;
      double dOr;
      double dUne;
      double dNe;
      float fOp1;
      float fOp2;
      float fF;
      float fUn;
      float fEq;
      float fUeq;
      float fOlt;
      float fUlt;
      float fOle;
      float fUle;
      float fOr;
      float fUne;
      float fNe;
    } TestFloat;

    TestFloat test;

    __ li(t1, 1);

    __ Ldc1(f4, MemOperand(a0, offsetof(TestFloat, dOp1)));
    __ Ldc1(f6, MemOperand(a0, offsetof(TestFloat, dOp2)));

    __ lwc1(f14, MemOperand(a0, offsetof(TestFloat, fOp1)));
    __ lwc1(f16, MemOperand(a0, offsetof(TestFloat, fOp2)));

    __ cmp_d(F, f2, f4, f6);
    __ cmp_s(F, f12, f14, f16);
    __ Sdc1(f2, MemOperand(a0, offsetof(TestFloat, dF)));
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, fF)) );

    __ cmp_d(UN, f2, f4, f6);
    __ cmp_s(UN, f12, f14, f16);
    __ Sdc1(f2, MemOperand(a0, offsetof(TestFloat, dUn)));
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, fUn)) );

    __ cmp_d(EQ, f2, f4, f6);
    __ cmp_s(EQ, f12, f14, f16);
    __ Sdc1(f2, MemOperand(a0, offsetof(TestFloat, dEq)));
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, fEq)) );

    __ cmp_d(UEQ, f2, f4, f6);
    __ cmp_s(UEQ, f12, f14, f16);
    __ Sdc1(f2, MemOperand(a0, offsetof(TestFloat, dUeq)));
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, fUeq)) );

    __ cmp_d(LT, f2, f4, f6);
    __ cmp_s(LT, f12, f14, f16);
    __ Sdc1(f2, MemOperand(a0, offsetof(TestFloat, dOlt)));
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, fOlt)) );

    __ cmp_d(ULT, f2, f4, f6);
    __ cmp_s(ULT, f12, f14, f16);
    __ Sdc1(f2, MemOperand(a0, offsetof(TestFloat, dUlt)));
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, fUlt)) );

    __ cmp_d(LE, f2, f4, f6);
    __ cmp_s(LE, f12, f14, f16);
    __ Sdc1(f2, MemOperand(a0, offsetof(TestFloat, dOle)));
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, fOle)) );

    __ cmp_d(ULE, f2, f4, f6);
    __ cmp_s(ULE, f12, f14, f16);
    __ Sdc1(f2, MemOperand(a0, offsetof(TestFloat, dUle)));
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, fUle)) );

    __ cmp_d(ORD, f2, f4, f6);
    __ cmp_s(ORD, f12, f14, f16);
    __ Sdc1(f2, MemOperand(a0, offsetof(TestFloat, dOr)));
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, fOr)) );

    __ cmp_d(UNE, f2, f4, f6);
    __ cmp_s(UNE, f12, f14, f16);
    __ Sdc1(f2, MemOperand(a0, offsetof(TestFloat, dUne)));
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, fUne)) );

    __ cmp_d(NE, f2, f4, f6);
    __ cmp_s(NE, f12, f14, f16);
    __ Sdc1(f2, MemOperand(a0, offsetof(TestFloat, dNe)));
    __ swc1(f12, MemOperand(a0, offsetof(TestFloat, fNe)) );

    __ jr(ra);
    __ nop();

    CodeDesc desc;
    assm.GetCode(isolate, &desc);
    Handle<Code> code = isolate->factory()->NewCode(
        desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
    F3 f = FUNCTION_CAST<F3>(code->entry());
    uint64_t dTrue  = 0xFFFFFFFFFFFFFFFF;
    uint64_t dFalse = 0x0000000000000000;
    uint32_t fTrue  = 0xFFFFFFFF;
    uint32_t fFalse = 0x00000000;

    test.dOp1 = 2.0;
    test.dOp2 = 3.0;
    test.fOp1 = 2.0;
    test.fOp2 = 3.0;
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(bit_cast<uint64_t>(test.dF), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUn), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dEq), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUeq), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dOlt), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dUlt), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dOle), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dUle), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dOr), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dUne), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dNe), dTrue);
    CHECK_EQ(bit_cast<uint32_t>(test.fF), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUn), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fEq), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUeq), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fOlt), fTrue);
    CHECK_EQ(bit_cast<uint32_t>(test.fUlt), fTrue);
    CHECK_EQ(bit_cast<uint32_t>(test.fOle), fTrue);
    CHECK_EQ(bit_cast<uint32_t>(test.fUle), fTrue);

    test.dOp1 = std::numeric_limits<double>::max();
    test.dOp2 = std::numeric_limits<double>::min();
    test.fOp1 = std::numeric_limits<float>::min();
    test.fOp2 = -std::numeric_limits<float>::max();  // lowest()
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(bit_cast<uint64_t>(test.dF), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUn), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dEq), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUeq), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dOlt), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUlt), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dOle), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUle), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dOr), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dUne), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dNe), dTrue);
    CHECK_EQ(bit_cast<uint32_t>(test.fF), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUn), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fEq), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUeq), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fOlt), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUlt), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fOle), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUle), fFalse);

    test.dOp1 = -std::numeric_limits<double>::max();  // lowest()
    test.dOp2 = -std::numeric_limits<double>::max();  // lowest()
    test.fOp1 = std::numeric_limits<float>::max();
    test.fOp2 = std::numeric_limits<float>::max();
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(bit_cast<uint64_t>(test.dF), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUn), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dEq), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dUeq), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dOlt), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUlt), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dOle), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dUle), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dOr), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dUne), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dNe), dFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fF), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUn), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fEq), fTrue);
    CHECK_EQ(bit_cast<uint32_t>(test.fUeq), fTrue);
    CHECK_EQ(bit_cast<uint32_t>(test.fOlt), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUlt), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fOle), fTrue);
    CHECK_EQ(bit_cast<uint32_t>(test.fUle), fTrue);

    test.dOp1 = std::numeric_limits<double>::quiet_NaN();
    test.dOp2 = 0.0;
    test.fOp1 = std::numeric_limits<float>::quiet_NaN();
    test.fOp2 = 0.0;
    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(bit_cast<uint64_t>(test.dF), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUn), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dEq), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUeq), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dOlt), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUlt), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dOle), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUle), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dOr), dFalse);
    CHECK_EQ(bit_cast<uint64_t>(test.dUne), dTrue);
    CHECK_EQ(bit_cast<uint64_t>(test.dNe), dFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fF), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUn), fTrue);
    CHECK_EQ(bit_cast<uint32_t>(test.fEq), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUeq), fTrue);
    CHECK_EQ(bit_cast<uint32_t>(test.fOlt), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUlt), fTrue);
    CHECK_EQ(bit_cast<uint32_t>(test.fOle), fFalse);
    CHECK_EQ(bit_cast<uint32_t>(test.fUle), fTrue);
  }
}


TEST(CVT) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test_float {
    float    cvt_d_s_in;
    double   cvt_d_s_out;
    int32_t  cvt_d_w_in;
    double   cvt_d_w_out;
    int64_t  cvt_d_l_in;
    double   cvt_d_l_out;

    float    cvt_l_s_in;
    int64_t  cvt_l_s_out;
    double   cvt_l_d_in;
    int64_t  cvt_l_d_out;

    double   cvt_s_d_in;
    float    cvt_s_d_out;
    int32_t  cvt_s_w_in;
    float    cvt_s_w_out;
    int64_t  cvt_s_l_in;
    float    cvt_s_l_out;

    float    cvt_w_s_in;
    int32_t  cvt_w_s_out;
    double   cvt_w_d_in;
    int32_t  cvt_w_d_out;
  } TestFloat;

  TestFloat test;

  // Save FCSR.
  __ cfc1(a1, FCSR);
  // Disable FPU exceptions.
  __ ctc1(zero_reg, FCSR);

#define GENERATE_CVT_TEST(x, y, z) \
  __ y##c1(f0, MemOperand(a0, offsetof(TestFloat, x##_in))); \
  __ x(f0, f0); \
  __ nop(); \
  __ z##c1(f0, MemOperand(a0, offsetof(TestFloat, x##_out)));

  GENERATE_CVT_TEST(cvt_d_s, lw, Sd)
  GENERATE_CVT_TEST(cvt_d_w, lw, Sd)
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    GENERATE_CVT_TEST(cvt_d_l, Ld, Sd)
  }

  if (IsFp64Mode()) {
    GENERATE_CVT_TEST(cvt_l_s, lw, Sd)
    GENERATE_CVT_TEST(cvt_l_d, Ld, Sd)
  }

  GENERATE_CVT_TEST(cvt_s_d, Ld, sw)
  GENERATE_CVT_TEST(cvt_s_w, lw, sw)
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    GENERATE_CVT_TEST(cvt_s_l, Ld, sw)
  }

  GENERATE_CVT_TEST(cvt_w_s, lw, sw)
  GENERATE_CVT_TEST(cvt_w_d, Ld, sw)

  // Restore FCSR.
  __ ctc1(a1, FCSR);

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());

  test.cvt_d_s_in = -0.51;
  test.cvt_d_w_in = -1;
  test.cvt_d_l_in = -1;
  test.cvt_l_s_in = -0.51;
  test.cvt_l_d_in = -0.51;
  test.cvt_s_d_in = -0.51;
  test.cvt_s_w_in = -1;
  test.cvt_s_l_in = -1;
  test.cvt_w_s_in = -0.51;
  test.cvt_w_d_in = -0.51;

  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.cvt_d_s_out, static_cast<double>(test.cvt_d_s_in));
  CHECK_EQ(test.cvt_d_w_out, static_cast<double>(test.cvt_d_w_in));
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    CHECK_EQ(test.cvt_d_l_out, static_cast<double>(test.cvt_d_l_in));
  }
  if (IsFp64Mode()) {
    CHECK_EQ(-1, test.cvt_l_s_out);
    CHECK_EQ(-1, test.cvt_l_d_out);
  }
  CHECK_EQ(test.cvt_s_d_out, static_cast<float>(test.cvt_s_d_in));
  CHECK_EQ(test.cvt_s_w_out, static_cast<float>(test.cvt_s_w_in));
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    CHECK_EQ(test.cvt_s_l_out, static_cast<float>(test.cvt_s_l_in));
  }
  CHECK_EQ(-1, test.cvt_w_s_out);
  CHECK_EQ(-1, test.cvt_w_d_out);

  test.cvt_d_s_in = 0.49;
  test.cvt_d_w_in = 1;
  test.cvt_d_l_in = 1;
  test.cvt_l_s_in = 0.49;
  test.cvt_l_d_in = 0.49;
  test.cvt_s_d_in = 0.49;
  test.cvt_s_w_in = 1;
  test.cvt_s_l_in = 1;
  test.cvt_w_s_in = 0.49;
  test.cvt_w_d_in = 0.49;

  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.cvt_d_s_out, static_cast<double>(test.cvt_d_s_in));
  CHECK_EQ(test.cvt_d_w_out, static_cast<double>(test.cvt_d_w_in));
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    CHECK_EQ(test.cvt_d_l_out, static_cast<double>(test.cvt_d_l_in));
  }
  if (IsFp64Mode()) {
    CHECK_EQ(0, test.cvt_l_s_out);
    CHECK_EQ(0, test.cvt_l_d_out);
  }
  CHECK_EQ(test.cvt_s_d_out, static_cast<float>(test.cvt_s_d_in));
  CHECK_EQ(test.cvt_s_w_out, static_cast<float>(test.cvt_s_w_in));
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    CHECK_EQ(test.cvt_s_l_out, static_cast<float>(test.cvt_s_l_in));
  }
  CHECK_EQ(0, test.cvt_w_s_out);
  CHECK_EQ(0, test.cvt_w_d_out);

  test.cvt_d_s_in = std::numeric_limits<float>::max();
  test.cvt_d_w_in = std::numeric_limits<int32_t>::max();
  test.cvt_d_l_in = std::numeric_limits<int64_t>::max();
  test.cvt_l_s_in = std::numeric_limits<float>::max();
  test.cvt_l_d_in = std::numeric_limits<double>::max();
  test.cvt_s_d_in = std::numeric_limits<double>::max();
  test.cvt_s_w_in = std::numeric_limits<int32_t>::max();
  test.cvt_s_l_in = std::numeric_limits<int64_t>::max();
  test.cvt_w_s_in = std::numeric_limits<float>::max();
  test.cvt_w_d_in = std::numeric_limits<double>::max();

  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.cvt_d_s_out, static_cast<double>(test.cvt_d_s_in));
  CHECK_EQ(test.cvt_d_w_out, static_cast<double>(test.cvt_d_w_in));
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    CHECK_EQ(test.cvt_d_l_out, static_cast<double>(test.cvt_d_l_in));
  }
  if (IsFp64Mode()) {
    CHECK_EQ(test.cvt_l_s_out, std::numeric_limits<int64_t>::max());
    CHECK_EQ(test.cvt_l_d_out, std::numeric_limits<int64_t>::max());
  }
  CHECK_EQ(test.cvt_s_d_out, static_cast<float>(test.cvt_s_d_in));
  CHECK_EQ(test.cvt_s_w_out, static_cast<float>(test.cvt_s_w_in));
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    CHECK_EQ(test.cvt_s_l_out, static_cast<float>(test.cvt_s_l_in));
  }
  CHECK_EQ(test.cvt_w_s_out, std::numeric_limits<int32_t>::max());
  CHECK_EQ(test.cvt_w_d_out, std::numeric_limits<int32_t>::max());


  test.cvt_d_s_in = -std::numeric_limits<float>::max();   // lowest()
  test.cvt_d_w_in = std::numeric_limits<int32_t>::min();  // lowest()
  test.cvt_d_l_in = std::numeric_limits<int64_t>::min();  // lowest()
  test.cvt_l_s_in = -std::numeric_limits<float>::max();   // lowest()
  test.cvt_l_d_in = -std::numeric_limits<double>::max();  // lowest()
  test.cvt_s_d_in = -std::numeric_limits<double>::max();  // lowest()
  test.cvt_s_w_in = std::numeric_limits<int32_t>::min();  // lowest()
  test.cvt_s_l_in = std::numeric_limits<int64_t>::min();  // lowest()
  test.cvt_w_s_in = -std::numeric_limits<float>::max();   // lowest()
  test.cvt_w_d_in = -std::numeric_limits<double>::max();  // lowest()

  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.cvt_d_s_out, static_cast<double>(test.cvt_d_s_in));
  CHECK_EQ(test.cvt_d_w_out, static_cast<double>(test.cvt_d_w_in));
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    CHECK_EQ(test.cvt_d_l_out, static_cast<double>(test.cvt_d_l_in));
  }
  // The returned value when converting from fixed-point to float-point
  // is not consistent between board, simulator and specification
  // in this test case, therefore modifying the test
  if (IsFp64Mode()) {
    CHECK(test.cvt_l_s_out == std::numeric_limits<int64_t>::min() ||
         test.cvt_l_s_out == std::numeric_limits<int64_t>::max());
    CHECK(test.cvt_l_d_out == std::numeric_limits<int64_t>::min() ||
          test.cvt_l_d_out == std::numeric_limits<int64_t>::max());
  }
  CHECK_EQ(test.cvt_s_d_out, static_cast<float>(test.cvt_s_d_in));
  CHECK_EQ(test.cvt_s_w_out, static_cast<float>(test.cvt_s_w_in));
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    CHECK_EQ(test.cvt_s_l_out, static_cast<float>(test.cvt_s_l_in));
  }
  CHECK(test.cvt_w_s_out == std::numeric_limits<int32_t>::min() ||
        test.cvt_w_s_out == std::numeric_limits<int32_t>::max());
  CHECK(test.cvt_w_d_out == std::numeric_limits<int32_t>::min() ||
        test.cvt_w_d_out == std::numeric_limits<int32_t>::max());


  test.cvt_d_s_in = std::numeric_limits<float>::min();
  test.cvt_d_w_in = std::numeric_limits<int32_t>::min();
  test.cvt_d_l_in = std::numeric_limits<int64_t>::min();
  test.cvt_l_s_in = std::numeric_limits<float>::min();
  test.cvt_l_d_in = std::numeric_limits<double>::min();
  test.cvt_s_d_in = std::numeric_limits<double>::min();
  test.cvt_s_w_in = std::numeric_limits<int32_t>::min();
  test.cvt_s_l_in = std::numeric_limits<int64_t>::min();
  test.cvt_w_s_in = std::numeric_limits<float>::min();
  test.cvt_w_d_in = std::numeric_limits<double>::min();

  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK_EQ(test.cvt_d_s_out, static_cast<double>(test.cvt_d_s_in));
  CHECK_EQ(test.cvt_d_w_out, static_cast<double>(test.cvt_d_w_in));
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    CHECK_EQ(test.cvt_d_l_out, static_cast<double>(test.cvt_d_l_in));
  }
  if (IsFp64Mode()) {
    CHECK_EQ(0, test.cvt_l_s_out);
    CHECK_EQ(0, test.cvt_l_d_out);
  }
  CHECK_EQ(test.cvt_s_d_out, static_cast<float>(test.cvt_s_d_in));
  CHECK_EQ(test.cvt_s_w_out, static_cast<float>(test.cvt_s_w_in));
  if ((IsMipsArchVariant(kMips32r2) || IsMipsArchVariant(kMips32r6)) &&
         IsFp64Mode()) {
    CHECK_EQ(test.cvt_s_l_out, static_cast<float>(test.cvt_s_l_in));
  }
  CHECK_EQ(0, test.cvt_w_s_out);
  CHECK_EQ(0, test.cvt_w_d_out);
}


TEST(DIV_FMT) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  typedef struct test {
    double dOp1;
    double dOp2;
    double dRes;
    float  fOp1;
    float  fOp2;
    float  fRes;
  } Test;

  Test test;

  // Save FCSR.
  __ cfc1(a1, FCSR);
  // Disable FPU exceptions.
  __ ctc1(zero_reg, FCSR);

  __ Ldc1(f4, MemOperand(a0, offsetof(Test, dOp1)));
  __ Ldc1(f2, MemOperand(a0, offsetof(Test, dOp2)));
  __ nop();
  __ div_d(f6, f4, f2);
  __ Sdc1(f6, MemOperand(a0, offsetof(Test, dRes)));

  __ lwc1(f4, MemOperand(a0, offsetof(Test, fOp1)) );
  __ lwc1(f2, MemOperand(a0, offsetof(Test, fOp2)) );
  __ nop();
  __ div_s(f6, f4, f2);
  __ swc1(f6, MemOperand(a0, offsetof(Test, fRes)) );

    // Restore FCSR.
  __ ctc1(a1, FCSR);

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F3 f = FUNCTION_CAST<F3>(code->entry());

  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));

  const int test_size = 3;

  double dOp1[test_size] = {
    5.0,
    DBL_MAX,
    DBL_MAX,
  };
  double dOp2[test_size] = {
    2.0,
    2.0,
    -DBL_MAX,
  };
  double dRes[test_size] = {
    2.5,
    DBL_MAX / 2.0,
    -1.0,
  };
  float fOp1[test_size] = {
    5.0,
    FLT_MAX,
    FLT_MAX,
  };
  float fOp2[test_size] = {
    2.0,
    2.0,
    -FLT_MAX,
  };
  float fRes[test_size] = {
    2.5,
    FLT_MAX / 2.0,
    -1.0,
  };

  for (int i = 0; i < test_size; i++) {
    test.dOp1 = dOp1[i];
    test.dOp2 = dOp2[i];
    test.fOp1 = fOp1[i];
    test.fOp2 = fOp2[i];

    (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
    CHECK_EQ(test.dRes, dRes[i]);
    CHECK_EQ(test.fRes, fRes[i]);
  }

  test.dOp1 = DBL_MAX;
  test.dOp2 = -0.0;
  test.fOp1 = FLT_MAX;
  test.fOp2 = -0.0;

  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK(!std::isfinite(test.dRes));
  CHECK(!std::isfinite(test.fRes));

  test.dOp1 = 0.0;
  test.dOp2 = -0.0;
  test.fOp1 = 0.0;
  test.fOp2 = -0.0;

  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK(std::isnan(test.dRes));
  CHECK(std::isnan(test.fRes));

  test.dOp1 = std::numeric_limits<double>::quiet_NaN();
  test.dOp2 = -5.0;
  test.fOp1 = std::numeric_limits<float>::quiet_NaN();
  test.fOp2 = -5.0;

  (CALL_GENERATED_CODE(isolate, f, &test, 0, 0, 0, 0));
  CHECK(std::isnan(test.dRes));
  CHECK(std::isnan(test.fRes));
}


uint32_t run_align(uint32_t rs_value, uint32_t rt_value, uint8_t bp) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  __ align(v0, a0, a1, bp);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());

  uint32_t res = reinterpret_cast<uint32_t>(CALL_GENERATED_CODE(
      isolate, f, rs_value, rt_value, 0, 0, 0));

  return res;
}


TEST(r6_align) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();

    struct TestCaseAlign {
      uint32_t  rs_value;
      uint32_t  rt_value;
      uint8_t   bp;
      uint32_t  expected_res;
    };

    struct TestCaseAlign tc[] = {
      // rs_value,    rt_value,    bp,  expected_res
      { 0x11223344,   0xaabbccdd,   0,  0xaabbccdd },
      { 0x11223344,   0xaabbccdd,   1,  0xbbccdd11 },
      { 0x11223344,   0xaabbccdd,   2,  0xccdd1122 },
      { 0x11223344,   0xaabbccdd,   3,  0xdd112233 },
    };

    size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseAlign);
    for (size_t i = 0; i < nr_test_cases; ++i) {
      CHECK_EQ(tc[i].expected_res, run_align(tc[i].rs_value,
                                             tc[i].rt_value, tc[i].bp));
    }
  }
}

uint32_t PC;  // The program counter.

uint32_t run_aluipc(int16_t offset) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  __ aluipc(v0, offset);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());
  PC = (uint32_t) f;  // Set the program counter.

  uint32_t res = reinterpret_cast<uint32_t>(
      CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}


TEST(r6_aluipc) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();

    struct TestCaseAluipc {
      int16_t   offset;
    };

    struct TestCaseAluipc tc[] = {
      // offset
      { -32768 },   // 0x8000
      {     -1 },   // 0xFFFF
      {      0 },
      {      1 },
      {  32767 },   // 0x7FFF
    };

    size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseAluipc);
    for (size_t i = 0; i < nr_test_cases; ++i) {
      PC = 0;
      uint32_t res = run_aluipc(tc[i].offset);
      // Now, the program_counter (PC) is set.
      uint32_t expected_res = ~0x0FFFF & (PC + (tc[i].offset << 16));
      CHECK_EQ(expected_res, res);
    }
  }
}


uint32_t run_auipc(int16_t offset) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  __ auipc(v0, offset);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());
  PC = (uint32_t) f;  // Set the program counter.

  uint32_t res = reinterpret_cast<uint32_t>(
      CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}


TEST(r6_auipc) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();

    struct TestCaseAuipc {
      int16_t   offset;
    };

    struct TestCaseAuipc tc[] = {
      // offset
      { -32768 },   // 0x8000
      {     -1 },   // 0xFFFF
      {      0 },
      {      1 },
      {  32767 },   // 0x7FFF
    };

    size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseAuipc);
    for (size_t i = 0; i < nr_test_cases; ++i) {
      PC = 0;
      uint32_t res = run_auipc(tc[i].offset);
      // Now, the program_counter (PC) is set.
      uint32_t expected_res = PC + (tc[i].offset << 16);
      CHECK_EQ(expected_res, res);
    }
  }
}


uint32_t run_lwpc(int offset) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  // 256k instructions; 2^8k
  // addiu t7, t0, 0xffff;  (0x250fffff)
  // ...
  // addiu t4, t0, 0x0000;  (0x250c0000)
  uint32_t addiu_start_1 = 0x25000000;
  for (int32_t i = 0xfffff; i >= 0xc0000; --i) {
    uint32_t addiu_new = addiu_start_1 + i;
    __ dd(addiu_new);
  }

  __ lwpc(t8, offset);         // offset 0; 0xef080000 (t8 register)
  __ mov(v0, t8);

  // 256k instructions; 2^8k
  // addiu t0, t0, 0x0000;  (0x25080000)
  // ...
  // addiu t3, t0, 0xffff;  (0x250bffff)
  uint32_t addiu_start_2 = 0x25000000;
  for (int32_t i = 0x80000; i <= 0xbffff; ++i) {
    uint32_t addiu_new = addiu_start_2 + i;
    __ dd(addiu_new);
  }

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());

  uint32_t res = reinterpret_cast<uint32_t>(
      CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}


TEST(r6_lwpc) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();

    struct TestCaseLwpc {
      int      offset;
      uint32_t expected_res;
    };

    struct TestCaseLwpc tc[] = {
      // offset,   expected_res
      { -262144,    0x250fffff },   // offset 0x40000
      {      -4,    0x250c0003 },
      {      -1,    0x250c0000 },
      {       0,    0xef080000 },
      {       1,    0x03001025 },   // mov(v0, t8)
      {       2,    0x25080000 },
      {       4,    0x25080002 },
      {  262143,    0x250bfffd },   // offset 0x3ffff
    };

    size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseLwpc);
    for (size_t i = 0; i < nr_test_cases; ++i) {
      uint32_t res = run_lwpc(tc[i].offset);
      CHECK_EQ(tc[i].expected_res, res);
    }
  }
}


uint32_t run_jic(int16_t offset) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  Label get_program_counter, stop_execution;
  __ push(ra);
  __ li(v0, 0);
  __ li(t1, 0x66);

  __ addiu(v0, v0, 0x1);        // <-- offset = -32
  __ addiu(v0, v0, 0x2);
  __ addiu(v0, v0, 0x10);
  __ addiu(v0, v0, 0x20);
  __ beq(v0, t1, &stop_execution);
  __ nop();

  __ bal(&get_program_counter);  // t0 <- program counter
  __ nop();
  __ jic(t0, offset);

  __ addiu(v0, v0, 0x100);
  __ addiu(v0, v0, 0x200);
  __ addiu(v0, v0, 0x1000);
  __ addiu(v0, v0, 0x2000);   // <--- offset = 16
  __ pop(ra);
  __ jr(ra);
  __ nop();

  __ bind(&get_program_counter);
  __ mov(t0, ra);
  __ jr(ra);
  __ nop();

  __ bind(&stop_execution);
  __ pop(ra);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());

  uint32_t res = reinterpret_cast<uint32_t>(
      CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}


TEST(r6_jic) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();

    struct TestCaseJic {
      // As rt will be used t0 register which will have value of
      // the program counter for the jic instruction.
      int16_t   offset;
      uint32_t  expected_res;
    };

    struct TestCaseJic tc[] = {
      // offset,   expected_result
      {      16,            0x2033 },
      {       4,            0x3333 },
      {     -32,              0x66 },
    };

    size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseJic);
    for (size_t i = 0; i < nr_test_cases; ++i) {
      uint32_t res = run_jic(tc[i].offset);
      CHECK_EQ(tc[i].expected_res, res);
    }
  }
}


uint64_t run_beqzc(int32_t value, int32_t offset) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  Label stop_execution;
  __ li(v0, 0);
  __ li(t1, 0x66);

  __ addiu(v0, v0, 0x1);        // <-- offset = -32
  __ addiu(v0, v0, 0x2);
  __ addiu(v0, v0, 0x10);
  __ addiu(v0, v0, 0x20);
  __ beq(v0, t1, &stop_execution);
  __ nop();

  __ beqzc(a0, offset);         // BEQZC rs, offset

  __ addiu(v0, v0,    0x1);
  __ addiu(v0, v0,  0x100);
  __ addiu(v0, v0,  0x200);
  __ addiu(v0, v0, 0x1000);
  __ addiu(v0, v0, 0x2000);   // <--- offset = 16
  __ jr(ra);
  __ nop();

  __ bind(&stop_execution);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());

  uint32_t res = reinterpret_cast<uint32_t>(
      CALL_GENERATED_CODE(isolate, f, value, 0, 0, 0, 0));

  return res;
}


TEST(r6_beqzc) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();

    struct TestCaseBeqzc {
      uint32_t  value;
      int32_t   offset;
      uint32_t  expected_res;
    };

    struct TestCaseBeqzc tc[] = {
      //    value,    offset,   expected_res
      {       0x0,        -8,           0x66 },
      {       0x0,         0,         0x3334 },
      {       0x0,         1,         0x3333 },
      {     0xabc,         1,         0x3334 },
      {       0x0,         4,         0x2033 },
    };

    size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseBeqzc);
    for (size_t i = 0; i < nr_test_cases; ++i) {
      uint32_t res = run_beqzc(tc[i].value, tc[i].offset);
      CHECK_EQ(tc[i].expected_res, res);
    }
  }
}


uint32_t run_jialc(int16_t offset) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  Label main_block, get_program_counter;
  __ push(ra);
  __ li(v0, 0);
  __ beq(v0, v0, &main_block);
  __ nop();

  // Block 1
  __ addiu(v0, v0, 0x1);        // <-- offset = -40
  __ addiu(v0, v0, 0x2);
  __ jr(ra);
  __ nop();

  // Block 2
  __ addiu(v0, v0, 0x10);        // <-- offset = -24
  __ addiu(v0, v0, 0x20);
  __ jr(ra);
  __ nop();

  // Block 3 (Main)
  __ bind(&main_block);
  __ bal(&get_program_counter);  // t0 <- program counter
  __ nop();
  __ jialc(t0, offset);
  __ addiu(v0, v0, 0x4);
  __ pop(ra);
  __ jr(ra);
  __ nop();

  // Block 4
  __ addiu(v0, v0, 0x100);      // <-- offset = 20
  __ addiu(v0, v0, 0x200);
  __ jr(ra);
  __ nop();

  // Block 5
  __ addiu(v0, v0, 0x1000);     // <--- offset = 36
  __ addiu(v0, v0, 0x2000);
  __ jr(ra);
  __ nop();

  __ bind(&get_program_counter);
  __ mov(t0, ra);
  __ jr(ra);
  __ nop();


  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());

  uint32_t res = reinterpret_cast<uint32_t>(
      CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}


TEST(r6_jialc) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();

    struct TestCaseJialc {
      int16_t   offset;
      uint32_t  expected_res;
    };

    struct TestCaseJialc tc[] = {
      // offset,   expected_res
      {     -40,            0x7 },
      {     -24,           0x34 },
      {      20,          0x304 },
      {      36,         0x3004 }
    };

    size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseJialc);
    for (size_t i = 0; i < nr_test_cases; ++i) {
      uint32_t res = run_jialc(tc[i].offset);
      CHECK_EQ(tc[i].expected_res, res);
    }
  }
}

static uint32_t run_addiupc(int32_t imm19) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  __ addiupc(v0, imm19);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());
  PC = (uint32_t) f;  // Set the program counter.

  uint32_t rs = reinterpret_cast<uint32_t>(
      CALL_GENERATED_CODE(isolate, f, imm19, 0, 0, 0, 0));

  return rs;
}


TEST(r6_addiupc) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();

    struct TestCaseAddiupc {
      int32_t   imm19;
    };

    TestCaseAddiupc tc[] = {
        //  imm19
        {-262144},  // 0x40000
        {-1},       // 0x7FFFF
        {0},
        {1},      // 0x00001
        {262143}  // 0x3FFFF
    };

    size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseAddiupc);
    for (size_t i = 0; i < nr_test_cases; ++i) {
      PC = 0;
      uint32_t res = run_addiupc(tc[i].imm19);
      // Now, the program_counter (PC) is set.
      uint32_t expected_res = PC + (tc[i].imm19 << 2);
      CHECK_EQ(expected_res, res);
    }
  }
}


int32_t run_bc(int32_t offset) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  Label continue_1, stop_execution;
  __ push(ra);
  __ li(v0, 0);
  __ li(t8, 0);
  __ li(t9, 2);   // A condition for stopping execution.

  for (int32_t i = -100; i <= -11; ++i) {
    __ addiu(v0, v0, 1);
  }

  __ addiu(t8, t8, 1);              // -10

  __ beq(t8, t9, &stop_execution);  // -9
  __ nop();                         // -8
  __ beq(t8, t8, &continue_1);      // -7
  __ nop();                         // -6

  __ bind(&stop_execution);
  __ pop(ra);                       // -5, -4
  __ jr(ra);                        // -3
  __ nop();                         // -2

  __ bind(&continue_1);
  __ bc(offset);                    // -1

  for (int32_t i = 0; i <= 99; ++i) {
    __ addiu(v0, v0, 1);
  }

  __ pop(ra);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());

  int32_t res = reinterpret_cast<int32_t>(
      CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}


TEST(r6_bc) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();

    struct TestCaseBc {
      int32_t   offset;
      int32_t   expected_res;
    };

    struct TestCaseBc tc[] = {
      //    offset,   expected_result
      {       -100,   (abs(-100) - 10) * 2        },
      {        -11,   (abs(-100) - 10 + 1)        },
      {          0,   (abs(-100) - 10 + 1 + 99)   },
      {          1,   (abs(-100) - 10 + 99)       },
      {         99,   (abs(-100) - 10 + 1)        },
    };

    size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseBc);
    for (size_t i = 0; i < nr_test_cases; ++i) {
      int32_t res = run_bc(tc[i].offset);
      CHECK_EQ(tc[i].expected_res, res);
    }
  }
}


int32_t run_balc(int32_t offset) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  Label continue_1, stop_execution;
  __ push(ra);
  __ li(v0, 0);
  __ li(t8, 0);
  __ li(t9, 2);   // A condition for stopping execution.

  __ beq(t8, t8, &continue_1);
  __ nop();

  uint32_t instruction_addiu = 0x24420001;  // addiu v0, v0, 1
  for (int32_t i = -117; i <= -57; ++i) {
    __ dd(instruction_addiu);
  }
  __ jr(ra);                        // -56
  __ nop();                         // -55

  for (int32_t i = -54; i <= -4; ++i) {
    __ dd(instruction_addiu);
  }
  __ jr(ra);                        // -3
  __ nop();                         // -2

  __ bind(&continue_1);
  __ balc(offset);                    // -1

  __ pop(ra);                         // 0, 1
  __ jr(ra);                          // 2
  __ nop();                           // 3

  for (int32_t i = 4; i <= 44; ++i) {
    __ dd(instruction_addiu);
  }
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());

  int32_t res = reinterpret_cast<int32_t>(
      CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}


uint32_t run_aui(uint32_t rs, uint16_t offset) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  __ li(t0, rs);
  __ aui(v0, t0, offset);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());

  uint32_t res =
    reinterpret_cast<uint32_t>
        (CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}


TEST(r6_aui) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();

    struct TestCaseAui {
      uint32_t   rs;
      uint16_t   offset;
      uint32_t   ref_res;
    };

    struct TestCaseAui tc[] = {
      // input, offset, result
      {0xfffeffff, 1, 0xffffffff},
      {0xffffffff, 0, 0xffffffff},
      {0, 0xffff, 0xffff0000},
      {0x0008ffff, 0xfff7, 0xffffffff},
      {32767, 32767, 0x7fff7fff},
      // overflow cases
      {0xffffffff, 0x1, 0x0000ffff},
      {0xffffffff, 0xffff, 0xfffeffff},
    };

    size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseAui);
    for (size_t i = 0; i < nr_test_cases; ++i) {
      PC = 0;
      uint32_t res = run_aui(tc[i].rs, tc[i].offset);
      CHECK_EQ(tc[i].ref_res, res);
    }
  }
}


TEST(r6_balc) {
  if (IsMipsArchVariant(kMips32r6)) {
    CcTest::InitializeVM();

    struct TestCaseBalc {
      int32_t   offset;
      int32_t   expected_res;
    };

    struct TestCaseBalc tc[] = {
      //  offset,   expected_result
      {     -117,   61  },
      {      -54,   51  },
      {        0,   0   },
      {        4,   41  },
    };

    size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseBalc);
    for (size_t i = 0; i < nr_test_cases; ++i) {
      int32_t res = run_balc(tc[i].offset);
      CHECK_EQ(tc[i].expected_res, res);
    }
  }
}


uint32_t run_bal(int16_t offset) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  __ mov(t0, ra);
  __ bal(offset);       // Equivalent for "BGEZAL zero_reg, offset".
  __ nop();

  __ mov(ra, t0);
  __ jr(ra);
  __ nop();

  __ li(v0, 1);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());

  F2 f = FUNCTION_CAST<F2>(code->entry());

  uint32_t res = reinterpret_cast<uint32_t>(
      CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}


TEST(bal) {
  CcTest::InitializeVM();

  struct TestCaseBal {
    int16_t  offset;
    uint32_t  expected_res;
  };

  struct TestCaseBal tc[] = {
    // offset, expected_res
    {       4,      1 },
  };

  size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseBal);
  for (size_t i = 0; i < nr_test_cases; ++i) {
    CHECK_EQ(tc[i].expected_res, run_bal(tc[i].offset));
  }
}


TEST(Trampoline) {
  // Private member of Assembler class.
  static const int kMaxBranchOffset = (1 << (18 - 1)) - 1;

  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, nullptr, 0,
                      v8::internal::CodeObjectRequired::kYes);
  Label done;
  size_t nr_calls = kMaxBranchOffset / (2 * Instruction::kInstrSize) + 2;

  for (size_t i = 0; i < nr_calls; ++i) {
    __ BranchShort(&done, eq, a0, Operand(a1));
  }
  __ bind(&done);
  __ Ret(USE_DELAY_SLOT);
  __ mov(v0, zero_reg);

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F2 f = FUNCTION_CAST<F2>(code->entry());

  int32_t res = reinterpret_cast<int32_t>(
      CALL_GENERATED_CODE(isolate, f, 42, 42, 0, 0, 0));
  CHECK_EQ(0, res);
}

template <class T>
struct TestCaseMaddMsub {
  T fr, fs, ft, fd_add, fd_sub;
};

template <typename T, typename F>
void helper_madd_msub_maddf_msubf(F func) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);
  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  T x = std::sqrt(static_cast<T>(2.0));
  T y = std::sqrt(static_cast<T>(3.0));
  T z = std::sqrt(static_cast<T>(5.0));
  T x2 = 11.11, y2 = 22.22, z2 = 33.33;
  TestCaseMaddMsub<T> test_cases[] = {
      {x, y, z, 0.0, 0.0},
      {x, y, -z, 0.0, 0.0},
      {x, -y, z, 0.0, 0.0},
      {x, -y, -z, 0.0, 0.0},
      {-x, y, z, 0.0, 0.0},
      {-x, y, -z, 0.0, 0.0},
      {-x, -y, z, 0.0, 0.0},
      {-x, -y, -z, 0.0, 0.0},
      {-3.14, 0.2345, -123.000056, 0.0, 0.0},
      {7.3, -23.257, -357.1357, 0.0, 0.0},
      {x2, y2, z2, 0.0, 0.0},
      {x2, y2, -z2, 0.0, 0.0},
      {x2, -y2, z2, 0.0, 0.0},
      {x2, -y2, -z2, 0.0, 0.0},
      {-x2, y2, z2, 0.0, 0.0},
      {-x2, y2, -z2, 0.0, 0.0},
      {-x2, -y2, z2, 0.0, 0.0},
      {-x2, -y2, -z2, 0.0, 0.0},
  };

  if (std::is_same<T, float>::value) {
    __ lwc1(f4, MemOperand(a0, offsetof(TestCaseMaddMsub<T>, fr)));
    __ lwc1(f6, MemOperand(a0, offsetof(TestCaseMaddMsub<T>, fs)));
    __ lwc1(f8, MemOperand(a0, offsetof(TestCaseMaddMsub<T>, ft)));
    __ lwc1(f16, MemOperand(a0, offsetof(TestCaseMaddMsub<T>, fr)));
  } else if (std::is_same<T, double>::value) {
    __ Ldc1(f4, MemOperand(a0, offsetof(TestCaseMaddMsub<T>, fr)));
    __ Ldc1(f6, MemOperand(a0, offsetof(TestCaseMaddMsub<T>, fs)));
    __ Ldc1(f8, MemOperand(a0, offsetof(TestCaseMaddMsub<T>, ft)));
    __ Ldc1(f16, MemOperand(a0, offsetof(TestCaseMaddMsub<T>, fr)));
  } else {
    UNREACHABLE();
  }

  func(assm);

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F3 f = FUNCTION_CAST<F3>(code->entry());

  const size_t kTableLength = sizeof(test_cases) / sizeof(TestCaseMaddMsub<T>);
  TestCaseMaddMsub<T> tc;
  for (size_t i = 0; i < kTableLength; i++) {
    tc.fr = test_cases[i].fr;
    tc.fs = test_cases[i].fs;
    tc.ft = test_cases[i].ft;

    (CALL_GENERATED_CODE(isolate, f, &tc, 0, 0, 0, 0));

    T res_add = 0;
    T res_sub = 0;
    if (IsMipsArchVariant(kMips32r2)) {
      res_add = (tc.fs * tc.ft) + tc.fr;
      res_sub = (tc.fs * tc.ft) - tc.fr;
    } else if (IsMipsArchVariant(kMips32r6)) {
      res_add = std::fma(tc.fs, tc.ft, tc.fr);
      res_sub = std::fma(-tc.fs, tc.ft, tc.fr);
    } else {
      UNREACHABLE();
    }

    CHECK_EQ(tc.fd_add, res_add);
    CHECK_EQ(tc.fd_sub, res_sub);
  }
}

TEST(madd_msub_s) {
  if (!IsMipsArchVariant(kMips32r2)) return;
  helper_madd_msub_maddf_msubf<float>([](MacroAssembler& assm) {
    __ madd_s(f10, f4, f6, f8);
    __ swc1(f10, MemOperand(a0, offsetof(TestCaseMaddMsub<float>, fd_add)));
    __ msub_s(f16, f4, f6, f8);
    __ swc1(f16, MemOperand(a0, offsetof(TestCaseMaddMsub<float>, fd_sub)));
  });
}

TEST(madd_msub_d) {
  if (!IsMipsArchVariant(kMips32r2)) return;
  helper_madd_msub_maddf_msubf<double>([](MacroAssembler& assm) {
    __ madd_d(f10, f4, f6, f8);
    __ Sdc1(f10, MemOperand(a0, offsetof(TestCaseMaddMsub<double>, fd_add)));
    __ msub_d(f16, f4, f6, f8);
    __ Sdc1(f16, MemOperand(a0, offsetof(TestCaseMaddMsub<double>, fd_sub)));
  });
}

TEST(maddf_msubf_s) {
  if (!IsMipsArchVariant(kMips32r6)) return;
  helper_madd_msub_maddf_msubf<float>([](MacroAssembler& assm) {
    __ maddf_s(f4, f6, f8);
    __ swc1(f4, MemOperand(a0, offsetof(TestCaseMaddMsub<float>, fd_add)));
    __ msubf_s(f16, f6, f8);
    __ swc1(f16, MemOperand(a0, offsetof(TestCaseMaddMsub<float>, fd_sub)));
  });
}

TEST(maddf_msubf_d) {
  if (!IsMipsArchVariant(kMips32r6)) return;
  helper_madd_msub_maddf_msubf<double>([](MacroAssembler& assm) {
    __ maddf_d(f4, f6, f8);
    __ Sdc1(f4, MemOperand(a0, offsetof(TestCaseMaddMsub<double>, fd_add)));
    __ msubf_d(f16, f6, f8);
    __ Sdc1(f16, MemOperand(a0, offsetof(TestCaseMaddMsub<double>, fd_sub)));
  });
}

uint32_t run_Subu(uint32_t imm, int32_t num_instr) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  Label code_start;
  __ bind(&code_start);
  __ Subu(v0, zero_reg, imm);
  CHECK_EQ(assm.SizeOfCodeGeneratedSince(&code_start),
           num_instr * Assembler::kInstrSize);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F2 f = FUNCTION_CAST<F2>(code->entry());

  uint32_t res = reinterpret_cast<uint32_t>(
      CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}

TEST(Subu) {
  CcTest::InitializeVM();

  // Test Subu macro-instruction for min_int16 and max_int16 border cases.
  // For subtracting int16 immediate values we use addiu.

  struct TestCaseSubu {
    uint32_t imm;
    uint32_t expected_res;
    int32_t num_instr;
  };

  // We call Subu(v0, zero_reg, imm) to test cases listed below.
  // 0 - imm = expected_res
  struct TestCaseSubu tc[] = {
      //    imm, expected_res, num_instr
      {0xffff8000, 0x8000, 2},  // min_int16
      // Generates ori + addu
      // We can't have just addiu because -min_int16 > max_int16 so use
      // register. We can load min_int16 to at register with addiu and then
      // subtract at with subu, but now we use ori + addu because -min_int16 can
      // be loaded using ori.
      {0x8000, 0xffff8000, 1},  // max_int16 + 1
      // Generates addiu
      // max_int16 + 1 is not int16 but -(max_int16 + 1) is, just use addiu.
      {0xffff7fff, 0x8001, 2},  // min_int16 - 1
      // Generates ori + addu
      // To load this value to at we need two instructions and another one to
      // subtract, lui + ori + subu. But we can load -value to at using just
      // ori and then add at register with addu.
      {0x8001, 0xffff7fff, 2},  // max_int16 + 2
      // Generates ori + subu
      // Not int16 but is uint16, load value to at with ori and subtract with
      // subu.
      {0x00010000, 0xffff0000, 2},
      // Generates lui + subu
      // Load value using lui to at and subtract with subu.
      {0x00010001, 0xfffeffff, 3},
      // Generates lui + ori + subu
      // We have to generate three instructions in this case.
  };

  size_t nr_test_cases = sizeof(tc) / sizeof(TestCaseSubu);
  for (size_t i = 0; i < nr_test_cases; ++i) {
    CHECK_EQ(tc[i].expected_res, run_Subu(tc[i].imm, tc[i].num_instr));
  }
}

TEST(MSA_fill_copy) {
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
    uint32_t u8;
    uint32_t u16;
    uint32_t u32;
    uint32_t s8;
    uint32_t s16;
    uint32_t s32;
  } T;
  T t;

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  {
    CpuFeatureScope fscope(&assm, MIPS_SIMD);

    __ li(t0, 0xa512b683);

    __ fill_b(w0, t0);
    __ fill_h(w2, t0);
    __ fill_w(w4, t0);
    __ copy_u_b(t1, w0, 11);
    __ sw(t1, MemOperand(a0, offsetof(T, u8)));
    __ copy_u_h(t1, w2, 6);
    __ sw(t1, MemOperand(a0, offsetof(T, u16)));
    __ copy_u_w(t1, w4, 3);
    __ sw(t1, MemOperand(a0, offsetof(T, u32)));

    __ copy_s_b(t1, w0, 8);
    __ sw(t1, MemOperand(a0, offsetof(T, s8)));
    __ copy_s_h(t1, w2, 5);
    __ sw(t1, MemOperand(a0, offsetof(T, s16)));
    __ copy_s_w(t1, w4, 1);
    __ sw(t1, MemOperand(a0, offsetof(T, s32)));

    __ jr(ra);
    __ nop();
  }

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F3 f = FUNCTION_CAST<F3>(code->entry());

  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t, 0, 0, 0, 0);
  USE(dummy);

  CHECK_EQ(0x83u, t.u8);
  CHECK_EQ(0xb683u, t.u16);
  CHECK_EQ(0xa512b683u, t.u32);
  CHECK_EQ(0xffffff83u, t.s8);
  CHECK_EQ(0xffffb683u, t.s16);
  CHECK_EQ(0xa512b683u, t.s32);
}

TEST(MSA_fill_copy_2) {
  // Similar to MSA_fill_copy test, but also check overlaping between MSA and
  // FPU registers with same numbers
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
    uint32_t w0;
    uint32_t w1;
    uint32_t w2;
    uint32_t w3;
  } T;
  T t[2];

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  {
    CpuFeatureScope fscope(&assm, MIPS_SIMD);

    __ li(t0, 0xaaaaaaaa);
    __ li(t1, 0x55555555);

    __ fill_w(w0, t0);
    __ fill_w(w2, t0);

    __ FmoveLow(f0, t1);
    __ FmoveHigh(f2, t1);

#define STORE_MSA_REG(w_reg, base, scratch)          \
  __ copy_u_w(scratch, w_reg, 0);                    \
  __ sw(scratch, MemOperand(base, offsetof(T, w0))); \
  __ copy_u_w(scratch, w_reg, 1);                    \
  __ sw(scratch, MemOperand(base, offsetof(T, w1))); \
  __ copy_u_w(scratch, w_reg, 2);                    \
  __ sw(scratch, MemOperand(base, offsetof(T, w2))); \
  __ copy_u_w(scratch, w_reg, 3);                    \
  __ sw(scratch, MemOperand(base, offsetof(T, w3)));

    STORE_MSA_REG(w0, a0, t2)
    STORE_MSA_REG(w2, a1, t2)
#undef STORE_MSA_REG

    __ jr(ra);
    __ nop();
  }

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F4 f = FUNCTION_CAST<F4>(code->entry());

  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t[0], &t[1], 0, 0, 0);
  USE(dummy);

  CHECK_EQ(0x55555555, t[0].w0);
  CHECK_EQ(0xaaaaaaaa, t[0].w1);
  CHECK_EQ(0xaaaaaaaa, t[0].w2);
  CHECK_EQ(0xaaaaaaaa, t[0].w3);
  CHECK_EQ(0xaaaaaaaa, t[1].w0);
  CHECK_EQ(0x55555555, t[1].w1);
  CHECK_EQ(0xaaaaaaaa, t[1].w2);
  CHECK_EQ(0xaaaaaaaa, t[1].w3);
}

TEST(MSA_fill_copy_3) {
  // Similar to MSA_fill_copy test, but also check overlaping between MSA and
  // FPU registers with same numbers
  CcTest::InitializeVM();
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  typedef struct {
    uint64_t d0;
    uint64_t d1;
  } T;
  T t[2];

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  {
    CpuFeatureScope fscope(&assm, MIPS_SIMD);

    __ li(t0, 0xaaaaaaaa);
    __ li(t1, 0x55555555);

    __ Move(f0, t0, t0);
    __ Move(f2, t0, t0);

    __ fill_w(w0, t1);
    __ fill_w(w2, t1);

    __ Sdc1(f0, MemOperand(a0, offsetof(T, d0)));
    __ Sdc1(f2, MemOperand(a1, offsetof(T, d0)));

    __ jr(ra);
    __ nop();
  }

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F4 f = FUNCTION_CAST<F4>(code->entry());

  Object* dummy = CALL_GENERATED_CODE(isolate, f, &t[0], &t[1], 0, 0, 0);
  USE(dummy);

  CHECK_EQ(0x5555555555555555, t[0].d0);
  CHECK_EQ(0x5555555555555555, t[1].d0);
}

typedef union {
  uint8_t b[16];
  uint16_t h[8];
  uint32_t w[4];
  uint64_t d[2];
} msa_reg_t;

template <typename T>
void run_msa_insert(int32_t rs_value, int n, msa_reg_t* w) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  CpuFeatureScope fscope(&assm, MIPS_SIMD);

  __ li(t0, -1);
  __ li(t1, rs_value);
  __ fill_w(w0, t0);

  if (std::is_same<T, int8_t>::value) {
    DCHECK(n < 16);
    __ insert_b(w0, n, t1);
  } else if (std::is_same<T, int16_t>::value) {
    DCHECK(n < 8);
    __ insert_h(w0, n, t1);
  } else if (std::is_same<T, int32_t>::value) {
    DCHECK(n < 4);
    __ insert_w(w0, n, t1);
  } else {
    UNREACHABLE();
  }

  __ copy_u_w(t2, w0, 0);
  __ sw(t2, MemOperand(a0, 0));
  __ copy_u_w(t2, w0, 1);
  __ sw(t2, MemOperand(a0, 4));
  __ copy_u_w(t2, w0, 2);
  __ sw(t2, MemOperand(a0, 8));
  __ copy_u_w(t2, w0, 3);
  __ sw(t2, MemOperand(a0, 12));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F3 f = FUNCTION_CAST<F3>(code->entry());

  (CALL_GENERATED_CODE(isolate, f, w, 0, 0, 0, 0));
}

TEST(MSA_insert) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseInsert {
    uint32_t input;
    int n;
    uint64_t exp_res_lo;
    uint64_t exp_res_hi;
  };

  struct TestCaseInsert tc_b[] = {
      // input, n,        exp_res_lo,          exp_res_hi
      {0xa2, 13, 0xffffffffffffffffu, 0xffffa2ffffffffffu},
      {0x73, 10, 0xffffffffffffffffu, 0xffffffffff73ffffu},
      {0x3494, 5, 0xffff94ffffffffffu, 0xffffffffffffffffu},
      {0xa6b8, 1, 0xffffffffffffb8ffu, 0xffffffffffffffffu}};

  for (size_t i = 0; i < sizeof(tc_b) / sizeof(TestCaseInsert); ++i) {
    msa_reg_t res;
    run_msa_insert<int8_t>(tc_b[i].input, tc_b[i].n, &res);
    CHECK_EQ(tc_b[i].exp_res_lo, res.d[0]);
    CHECK_EQ(tc_b[i].exp_res_hi, res.d[1]);
  }

  struct TestCaseInsert tc_h[] = {
      // input, n,         exp_res_lo,          exp_res_hi
      {0x85a2, 7, 0xffffffffffffffffu, 0x85a2ffffffffffffu},
      {0xe873, 5, 0xffffffffffffffffu, 0xffffffffe873ffffu},
      {0x3494, 3, 0x3494ffffffffffffu, 0xffffffffffffffffu},
      {0xa6b8, 1, 0xffffffffa6b8ffffu, 0xffffffffffffffffu}};

  for (size_t i = 0; i < sizeof(tc_h) / sizeof(TestCaseInsert); ++i) {
    msa_reg_t res;
    run_msa_insert<int16_t>(tc_h[i].input, tc_h[i].n, &res);
    CHECK_EQ(tc_h[i].exp_res_lo, res.d[0]);
    CHECK_EQ(tc_h[i].exp_res_hi, res.d[1]);
  }

  struct TestCaseInsert tc_w[] = {
      // input,     n,          exp_res_lo,          exp_res_hi
      {0xd2f085a2u, 3, 0xffffffffffffffffu, 0xd2f085a2ffffffffu},
      {0x4567e873u, 2, 0xffffffffffffffffu, 0xffffffff4567e873u},
      {0xacdb3494u, 1, 0xacdb3494ffffffffu, 0xffffffffffffffffu},
      {0x89aba6b8u, 0, 0xffffffff89aba6b8u, 0xffffffffffffffffu}};

  for (size_t i = 0; i < sizeof(tc_w) / sizeof(TestCaseInsert); ++i) {
    msa_reg_t res;
    run_msa_insert<int32_t>(tc_w[i].input, tc_w[i].n, &res);
    CHECK_EQ(tc_w[i].exp_res_lo, res.d[0]);
    CHECK_EQ(tc_w[i].exp_res_hi, res.d[1]);
  }
}

struct ExpResShf {
  uint8_t i8;
  uint64_t lo;
  uint64_t hi;
};

void run_msa_i8(SecondaryField opcode, uint64_t ws_lo, uint64_t ws_hi,
                uint8_t i8) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  CpuFeatureScope fscope(&assm, MIPS_SIMD);
  msa_reg_t res;
  uint64_t wd_lo = 0xf35862e13e38f8b0;
  uint64_t wd_hi = 0x4f41ffdef2bfe636;

#define LOAD_W_REG(lo, hi, w_reg)                            \
  __ li(t0, static_cast<uint32_t>(lo & 0xffffffff));         \
  __ li(t1, static_cast<uint32_t>((lo >> 32) & 0xffffffff)); \
  __ insert_w(w_reg, 0, t0);                                 \
  __ insert_w(w_reg, 1, t1);                                 \
  __ li(t0, static_cast<uint32_t>(hi & 0xffffffff));         \
  __ li(t1, static_cast<uint32_t>((hi >> 32) & 0xffffffff)); \
  __ insert_w(w_reg, 2, t0);                                 \
  __ insert_w(w_reg, 3, t1);

  LOAD_W_REG(ws_lo, ws_hi, w0)

  switch (opcode) {
    case ANDI_B:
      __ andi_b(w2, w0, i8);
      break;
    case ORI_B:
      __ ori_b(w2, w0, i8);
      break;
    case NORI_B:
      __ nori_b(w2, w0, i8);
      break;
    case XORI_B:
      __ xori_b(w2, w0, i8);
      break;
    case BMNZI_B:
      LOAD_W_REG(wd_lo, wd_hi, w2);
      __ bmnzi_b(w2, w0, i8);
      break;
    case BMZI_B:
      LOAD_W_REG(wd_lo, wd_hi, w2);
      __ bmzi_b(w2, w0, i8);
      break;
    case BSELI_B:
      LOAD_W_REG(wd_lo, wd_hi, w2);
      __ bseli_b(w2, w0, i8);
      break;
    case SHF_B:
      __ shf_b(w2, w0, i8);
      break;
    case SHF_H:
      __ shf_h(w2, w0, i8);
      break;
    case SHF_W:
      __ shf_w(w2, w0, i8);
      break;
    default:
      UNREACHABLE();
  }

  __ copy_u_w(t2, w2, 0);
  __ sw(t2, MemOperand(a0, 0));
  __ copy_u_w(t2, w2, 1);
  __ sw(t2, MemOperand(a0, 4));
  __ copy_u_w(t2, w2, 2);
  __ sw(t2, MemOperand(a0, 8));
  __ copy_u_w(t2, w2, 3);
  __ sw(t2, MemOperand(a0, 12));

  __ jr(ra);
  __ nop();

#undef LOAD_W_REG

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F3 f = FUNCTION_CAST<F3>(code->entry());

  (CALL_GENERATED_CODE(isolate, f, &res, 0, 0, 0, 0));

  uint64_t mask = i8 * 0x0101010101010101ull;
  switch (opcode) {
    case ANDI_B:
      CHECK_EQ(ws_lo & mask, res.d[0]);
      CHECK_EQ(ws_hi & mask, res.d[1]);
      break;
    case ORI_B:
      CHECK_EQ(ws_lo | mask, res.d[0]);
      CHECK_EQ(ws_hi | mask, res.d[1]);
      break;
    case NORI_B:
      CHECK_EQ(~(ws_lo | mask), res.d[0]);
      CHECK_EQ(~(ws_hi | mask), res.d[1]);
      break;
    case XORI_B:
      CHECK_EQ(ws_lo ^ mask, res.d[0]);
      CHECK_EQ(ws_hi ^ mask, res.d[1]);
      break;
    case BMNZI_B:
      CHECK_EQ((ws_lo & mask) | (wd_lo & ~mask), res.d[0]);
      CHECK_EQ((ws_hi & mask) | (wd_hi & ~mask), res.d[1]);
      break;
    case BMZI_B:
      CHECK_EQ((ws_lo & ~mask) | (wd_lo & mask), res.d[0]);
      CHECK_EQ((ws_hi & ~mask) | (wd_hi & mask), res.d[1]);
      break;
    case BSELI_B:
      CHECK_EQ((ws_lo & ~wd_lo) | (mask & wd_lo), res.d[0]);
      CHECK_EQ((ws_hi & ~wd_hi) | (mask & wd_hi), res.d[1]);
      break;
    case SHF_B: {
      struct ExpResShf exp_b[] = {
          // i8,              exp_lo,             exp_hi
          {0xffu, 0x11111111b9b9b9b9, 0xf7f7f7f7c8c8c8c8},
          {0x0u, 0x62626262dfdfdfdf, 0xd6d6d6d6c8c8c8c8},
          {0xe4u, 0xf35862e13e38f8b0, 0x4f41ffdef2bfe636},
          {0x1bu, 0x1b756911c3d9a7b9, 0xae94a5f79c8aefc8},
          {0xb1u, 0x662b6253e8c4df12, 0x0d3ad6803f8bc88b},
          {0x4eu, 0x62e1f358f8b03e38, 0xffde4f41e636f2bf},
          {0x27u, 0x1b697511c3a7d9b9, 0xaea594f79cef8ac8}};
      for (size_t i = 0; i < sizeof(exp_b) / sizeof(ExpResShf); ++i) {
        if (exp_b[i].i8 == i8) {
          CHECK_EQ(exp_b[i].lo, res.d[0]);
          CHECK_EQ(exp_b[i].hi, res.d[1]);
        }
      }
    } break;
    case SHF_H: {
      struct ExpResShf exp_h[] = {
          //  i8,             exp_lo,             exp_hi
          {0xffu, 0x1169116911691169, 0xf7a5f7a5f7a5f7a5},
          {0x0u, 0x12df12df12df12df, 0x8bc88bc88bc88bc8},
          {0xe4u, 0xf35862e13e38f8b0, 0x4f41ffdef2bfe636},
          {0x1bu, 0xd9c3b9a7751b1169, 0x8a9cc8ef94aef7a5},
          {0xb1u, 0x53622b6612dfc4e8, 0x80d63a0d8bc88b3f},
          {0x4eu, 0x3e38f8b0f35862e1, 0xf2bfe6364f41ffde},
          {0x27u, 0xd9c3751bb9a71169, 0x8a9c94aec8eff7a5}};
      for (size_t i = 0; i < sizeof(exp_h) / sizeof(ExpResShf); ++i) {
        if (exp_h[i].i8 == i8) {
          CHECK_EQ(exp_h[i].lo, res.d[0]);
          CHECK_EQ(exp_h[i].hi, res.d[1]);
        }
      }
    } break;
    case SHF_W: {
      struct ExpResShf exp_w[] = {
          //  i8,             exp_lo,             exp_hi
          {0xffu, 0xf7a594aef7a594ae, 0xf7a594aef7a594ae},
          {0x0u, 0xc4e812dfc4e812df, 0xc4e812dfc4e812df},
          {0xe4u, 0xf35862e13e38f8b0, 0x4f41ffdef2bfe636},
          {0x1bu, 0xc8ef8a9cf7a594ae, 0xb9a7d9c31169751b},
          {0xb1u, 0xc4e812df2b665362, 0x8b3f8bc83a0d80d6},
          {0x4eu, 0x4f41ffdef2bfe636, 0xf35862e13e38f8b0},
          {0x27u, 0x1169751bf7a594ae, 0xb9a7d9c3c8ef8a9c}};
      for (size_t i = 0; i < sizeof(exp_w) / sizeof(ExpResShf); ++i) {
        if (exp_w[i].i8 == i8) {
          CHECK_EQ(exp_w[i].lo, res.d[0]);
          CHECK_EQ(exp_w[i].hi, res.d[1]);
        }
      }
    } break;
    default:
      UNREACHABLE();
  }
}

struct TestCaseMsaI8 {
  uint64_t input_lo;
  uint64_t input_hi;
  uint8_t i8;
};

TEST(MSA_andi_ori_nori_xori) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsaI8 tc[] = {// input_lo,         input_hi,           i8
                               {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0xffu},
                               {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0x0u},
                               {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0x3bu},
                               {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0xd9u}};

  for (size_t i = 0; i < sizeof(tc) / sizeof(TestCaseMsaI8); ++i) {
    run_msa_i8(ANDI_B, tc[i].input_lo, tc[i].input_hi, tc[i].i8);
    run_msa_i8(ORI_B, tc[i].input_lo, tc[i].input_hi, tc[i].i8);
    run_msa_i8(NORI_B, tc[i].input_lo, tc[i].input_hi, tc[i].i8);
    run_msa_i8(XORI_B, tc[i].input_lo, tc[i].input_hi, tc[i].i8);
  }
}

TEST(MSA_bmnzi_bmzi_bseli) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsaI8 tc[] = {//          input_lo,          input_hi,    i8
                               {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0xffu},
                               {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0x0u},
                               {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0x3bu},
                               {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0xd9u}};

  for (size_t i = 0; i < sizeof(tc) / sizeof(TestCaseMsaI8); ++i) {
    run_msa_i8(BMNZI_B, tc[i].input_lo, tc[i].input_hi, tc[i].i8);
    run_msa_i8(BMZI_B, tc[i].input_lo, tc[i].input_hi, tc[i].i8);
    run_msa_i8(BSELI_B, tc[i].input_lo, tc[i].input_hi, tc[i].i8);
  }
}

TEST(MSA_shf) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsaI8 tc[] = {
      //          input_lo,           input_hi,    i8
      {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0xffu},  // 3333
      {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0x0u},   // 0000
      {0xf35862e13e38f8b0, 0x4f41ffdef2bfe636, 0xe4u},  // 3210
      {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0x1bu},  // 0123
      {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0xb1u},  // 2301
      {0xf35862e13e38f8b0, 0x4f41ffdef2bfe636, 0x4eu},  // 1032
      {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0x27u}   // 0213
  };

  for (size_t i = 0; i < sizeof(tc) / sizeof(TestCaseMsaI8); ++i) {
    run_msa_i8(SHF_B, tc[i].input_lo, tc[i].input_hi, tc[i].i8);
    run_msa_i8(SHF_H, tc[i].input_lo, tc[i].input_hi, tc[i].i8);
    run_msa_i8(SHF_W, tc[i].input_lo, tc[i].input_hi, tc[i].i8);
  }
}

uint32_t run_Ins(uint32_t imm, uint32_t source, uint16_t pos, uint16_t size) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  __ li(v0, imm);
  __ li(t0, source);
  __ Ins(v0, t0, pos, size);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F2 f = FUNCTION_CAST<F2>(code->entry());

  uint32_t res = reinterpret_cast<uint32_t>(
      CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}

TEST(Ins) {
  CcTest::InitializeVM();

  //       run_Ins(rt_value, rs_value, pos, size), expected_result
  CHECK_EQ(run_Ins(0x55555555, 0xabcdef01, 31, 1), 0xd5555555);
  CHECK_EQ(run_Ins(0x55555555, 0xabcdef02, 30, 2), 0x95555555);
  CHECK_EQ(run_Ins(0x01234567, 0xfabcdeff, 0, 32), 0xfabcdeff);

  // Results with positive sign.
  CHECK_EQ(run_Ins(0x55555550, 0x80000001, 0, 1), 0x55555551);
  CHECK_EQ(run_Ins(0x55555555, 0x40000001, 0, 32), 0x40000001);
  CHECK_EQ(run_Ins(0x55555555, 0x20000001, 1, 31), 0x40000003);
  CHECK_EQ(run_Ins(0x55555555, 0x80700001, 8, 24), 0x70000155);
  CHECK_EQ(run_Ins(0x55555555, 0x80007001, 16, 16), 0x70015555);
  CHECK_EQ(run_Ins(0x55555555, 0x80000071, 24, 8), 0x71555555);
  CHECK_EQ(run_Ins(0x75555555, 0x40000000, 31, 1), 0x75555555);

  // Results with negative sign.
  CHECK_EQ(run_Ins(0x85555550, 0x80000001, 0, 1), 0x85555551);
  CHECK_EQ(run_Ins(0x55555555, 0x80000001, 0, 32), 0x80000001);
  CHECK_EQ(run_Ins(0x55555555, 0x40000001, 1, 31), 0x80000003);
  CHECK_EQ(run_Ins(0x55555555, 0x80800001, 8, 24), 0x80000155);
  CHECK_EQ(run_Ins(0x55555555, 0x80008001, 16, 16), 0x80015555);
  CHECK_EQ(run_Ins(0x55555555, 0x80000081, 24, 8), 0x81555555);
  CHECK_EQ(run_Ins(0x75555555, 0x00000001, 31, 1), 0xf5555555);
}

uint32_t run_Ext(uint32_t source, uint16_t pos, uint16_t size) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);

  __ li(v0, 0xffffffff);
  __ li(t0, source);
  __ Ext(v0, t0, pos, size);
  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
  F2 f = FUNCTION_CAST<F2>(code->entry());

  uint32_t res = reinterpret_cast<uint32_t>(
      CALL_GENERATED_CODE(isolate, f, 0, 0, 0, 0, 0));

  return res;
}

TEST(Ext) {
  CcTest::InitializeVM();

  // Source values with negative sign.
  //       run_Ext(rs_value, pos, size), expected_result
  CHECK_EQ(run_Ext(0x80000001, 0, 1), 0x00000001);
  CHECK_EQ(run_Ext(0x80000001, 0, 32), 0x80000001);
  CHECK_EQ(run_Ext(0x80000002, 1, 31), 0x40000001);
  CHECK_EQ(run_Ext(0x80000100, 8, 24), 0x00800001);
  CHECK_EQ(run_Ext(0x80010000, 16, 16), 0x00008001);
  CHECK_EQ(run_Ext(0x81000000, 24, 8), 0x00000081);
  CHECK_EQ(run_Ext(0x80000000, 31, 1), 0x00000001);

  // Source values with positive sign.
  CHECK_EQ(run_Ext(0x00000001, 0, 1), 0x00000001);
  CHECK_EQ(run_Ext(0x40000001, 0, 32), 0x40000001);
  CHECK_EQ(run_Ext(0x40000002, 1, 31), 0x20000001);
  CHECK_EQ(run_Ext(0x40000100, 8, 24), 0x00400001);
  CHECK_EQ(run_Ext(0x40010000, 16, 16), 0x00004001);
  CHECK_EQ(run_Ext(0x41000000, 24, 8), 0x00000041);
  CHECK_EQ(run_Ext(0x40000000, 31, 1), 0x00000000);
}

struct TestCaseMsaI5 {
  uint64_t ws_lo;
  uint64_t ws_hi;
  uint32_t i5;
};

template <typename InstFunc, typename OperFunc>
void run_msa_i5(struct TestCaseMsaI5* input, bool i5_sign_ext,
                InstFunc GenerateI5InstructionFunc,
                OperFunc GenerateOperationFunc) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  CpuFeatureScope fscope(&assm, MIPS_SIMD);
  msa_reg_t res;
  int32_t i5 =
      i5_sign_ext ? static_cast<int32_t>(input->i5 << 27) >> 27 : input->i5;

  __ li(t0, static_cast<uint32_t>(input->ws_lo & 0xffffffff));
  __ li(t1, static_cast<uint32_t>((input->ws_lo >> 32) & 0xffffffff));
  __ insert_w(w0, 0, t0);
  __ insert_w(w0, 1, t1);
  __ li(t0, static_cast<uint32_t>(input->ws_hi & 0xffffffff));
  __ li(t1, static_cast<uint32_t>((input->ws_hi >> 32) & 0xffffffff));
  __ insert_w(w0, 2, t0);
  __ insert_w(w0, 3, t1);

  GenerateI5InstructionFunc(assm, i5);

  __ copy_u_w(t2, w2, 0);
  __ sw(t2, MemOperand(a0, 0));
  __ copy_u_w(t2, w2, 1);
  __ sw(t2, MemOperand(a0, 4));
  __ copy_u_w(t2, w2, 2);
  __ sw(t2, MemOperand(a0, 8));
  __ copy_u_w(t2, w2, 3);
  __ sw(t2, MemOperand(a0, 12));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F3 f = FUNCTION_CAST<F3>(code->entry());

  (CALL_GENERATED_CODE(isolate, f, &res, 0, 0, 0, 0));

  CHECK_EQ(GenerateOperationFunc(input->ws_lo, input->i5), res.d[0]);
  CHECK_EQ(GenerateOperationFunc(input->ws_hi, input->i5), res.d[1]);
}

TEST(MSA_addvi_subvi) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsaI5 tc[] = {
      //             ws_lo,              ws_hi,         i5
      {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0x0000001f},
      {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0x0000000f},
      {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0x00000005},
      {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0x00000010},
      {0xffab807f807fffcd, 0x7f23ff80ff567f80, 0x0000000f},
      {0x80ffefff7f12807f, 0x807f80ff7fdeff78, 0x00000010}};

#define ADDVI_DF(lanes, mask)                               \
  uint64_t res = 0;                                         \
  for (int i = 0; i < lanes / 2; ++i) {                     \
    int shift = (kMSARegSize / lanes) * i;                  \
    res |= ((((ws >> shift) & mask) + i5) & mask) << shift; \
  }                                                         \
  return res

#define SUBVI_DF(lanes, mask)                               \
  uint64_t res = 0;                                         \
  for (int i = 0; i < lanes / 2; ++i) {                     \
    int shift = (kMSARegSize / lanes) * i;                  \
    res |= ((((ws >> shift) & mask) - i5) & mask) << shift; \
  }                                                         \
  return res

  for (size_t i = 0; i < sizeof(tc) / sizeof(TestCaseMsaI5); ++i) {
    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ addvi_b(w2, w0, i5); },
        [](uint64_t ws, uint32_t i5) { ADDVI_DF(kMSALanesByte, UINT8_MAX); });

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ addvi_h(w2, w0, i5); },
        [](uint64_t ws, uint32_t i5) { ADDVI_DF(kMSALanesHalf, UINT16_MAX); });

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ addvi_w(w2, w0, i5); },
        [](uint64_t ws, uint32_t i5) { ADDVI_DF(kMSALanesWord, UINT32_MAX); });

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ addvi_d(w2, w0, i5); },
        [](uint64_t ws, uint32_t i5) { ADDVI_DF(kMSALanesDword, UINT64_MAX); });

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ subvi_b(w2, w0, i5); },
        [](uint64_t ws, uint32_t i5) { SUBVI_DF(kMSALanesByte, UINT8_MAX); });

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ subvi_h(w2, w0, i5); },
        [](uint64_t ws, uint32_t i5) { SUBVI_DF(kMSALanesHalf, UINT16_MAX); });

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ subvi_w(w2, w0, i5); },
        [](uint64_t ws, uint32_t i5) { SUBVI_DF(kMSALanesWord, UINT32_MAX); });

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ subvi_d(w2, w0, i5); },
        [](uint64_t ws, uint32_t i5) { SUBVI_DF(kMSALanesDword, UINT64_MAX); });
  }
#undef ADDVI_DF
#undef SUBVI_DF
}

TEST(MSA_maxi_mini) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsaI5 tc[] = {
      // ws_lo, ws_hi, i5
      {0x7f80ff3480ff7f00, 0x8d7fff80ff7f6780, 0x0000001f},
      {0x7f80ff3480ff7f00, 0x8d7fff80ff7f6780, 0x0000000f},
      {0x7f80ff3480ff7f00, 0x8d7fff80ff7f6780, 0x00000010},
      {0x80007fff91daffff, 0x7fff8000ffff5678, 0x0000001f},
      {0x80007fff91daffff, 0x7fff8000ffff5678, 0x0000000f},
      {0x80007fff91daffff, 0x7fff8000ffff5678, 0x00000010},
      {0x7fffffff80000000, 0x12345678ffffffff, 0x0000001f},
      {0x7fffffff80000000, 0x12345678ffffffff, 0x0000000f},
      {0x7fffffff80000000, 0x12345678ffffffff, 0x00000010},
      {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0x0000001f},
      {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0x0000000f},
      {0xf35862e13e38f8b0, 0x4f41ffdef2bfe636, 0x00000010},
      {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0x00000015},
      {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0x00000009},
      {0xf35862e13e38f8b0, 0x4f41ffdef2bfe636, 0x00000003}};

#define MAXI_MINI_S_DF(lanes, mask, func)                                     \
  [](uint64_t ws, uint32_t ui5) {                                             \
    uint64_t res = 0;                                                         \
    int64_t i5 = ArithmeticShiftRight(static_cast<int64_t>(ui5) << 59, 59);   \
    int elem_size = kMSARegSize / lanes;                                      \
    for (int i = 0; i < lanes / 2; ++i) {                                     \
      int shift = elem_size * i;                                              \
      int64_t elem =                                                          \
          static_cast<int64_t>(((ws >> shift) & mask) << (64 - elem_size)) >> \
          (64 - elem_size);                                                   \
      res |= static_cast<uint64_t>(func(elem, i5) & mask) << shift;           \
    }                                                                         \
    return res;                                                               \
  }

#define MAXI_MINI_U_DF(lanes, mask, func)                              \
  [](uint64_t ws, uint32_t ui5) {                                      \
    uint64_t res = 0;                                                  \
    int elem_size = kMSARegSize / lanes;                               \
    for (int i = 0; i < lanes / 2; ++i) {                              \
      int shift = elem_size * i;                                       \
      uint64_t elem = (ws >> shift) & mask;                            \
      res |= (func(elem, static_cast<uint64_t>(ui5)) & mask) << shift; \
    }                                                                  \
    return res;                                                        \
  }

  for (size_t i = 0; i < sizeof(tc) / sizeof(TestCaseMsaI5); ++i) {
    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ maxi_s_b(w2, w0, i5); },
        MAXI_MINI_S_DF(kMSALanesByte, UINT8_MAX, Max));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ maxi_s_h(w2, w0, i5); },
        MAXI_MINI_S_DF(kMSALanesHalf, UINT16_MAX, Max));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ maxi_s_w(w2, w0, i5); },
        MAXI_MINI_S_DF(kMSALanesWord, UINT32_MAX, Max));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ maxi_s_d(w2, w0, i5); },
        MAXI_MINI_S_DF(kMSALanesDword, UINT64_MAX, Max));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ mini_s_b(w2, w0, i5); },
        MAXI_MINI_S_DF(kMSALanesByte, UINT8_MAX, Min));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ mini_s_h(w2, w0, i5); },
        MAXI_MINI_S_DF(kMSALanesHalf, UINT16_MAX, Min));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ mini_s_w(w2, w0, i5); },
        MAXI_MINI_S_DF(kMSALanesWord, UINT32_MAX, Min));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ mini_s_d(w2, w0, i5); },
        MAXI_MINI_S_DF(kMSALanesDword, UINT64_MAX, Min));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ maxi_u_b(w2, w0, i5); },
        MAXI_MINI_U_DF(kMSALanesByte, UINT8_MAX, Max));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ maxi_u_h(w2, w0, i5); },
        MAXI_MINI_U_DF(kMSALanesHalf, UINT16_MAX, Max));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ maxi_u_w(w2, w0, i5); },
        MAXI_MINI_U_DF(kMSALanesWord, UINT32_MAX, Max));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ maxi_u_d(w2, w0, i5); },
        MAXI_MINI_U_DF(kMSALanesDword, UINT64_MAX, Max));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ mini_u_b(w2, w0, i5); },
        MAXI_MINI_U_DF(kMSALanesByte, UINT8_MAX, Min));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ mini_u_h(w2, w0, i5); },
        MAXI_MINI_U_DF(kMSALanesHalf, UINT16_MAX, Min));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ mini_u_w(w2, w0, i5); },
        MAXI_MINI_U_DF(kMSALanesWord, UINT32_MAX, Min));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ mini_u_d(w2, w0, i5); },
        MAXI_MINI_U_DF(kMSALanesDword, UINT64_MAX, Min));
  }
#undef MAXI_MINI_S_DF
#undef MAXI_MINI_U_DF
}

TEST(MSA_ceqi_clti_clei) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsaI5 tc[] = {
      {0xff69751bb9a7d9c3, 0xf7a594aec8ff8a9c, 0x0000001f},
      {0xe669ffffb9a7d9c3, 0xf7a594aeffff8a9c, 0x0000001f},
      {0xffffffffb9a7d9c3, 0xf7a594aeffffffff, 0x0000001f},
      {0x2b0b5362c4e812df, 0x3a0d80d68b3f0bc8, 0x0000000b},
      {0x2b66000bc4e812df, 0x3a0d000b8b3f8bc8, 0x0000000b},
      {0x0000000bc4e812df, 0x3a0d80d60000000b, 0x0000000b},
      {0xf38062e13e38f8b0, 0x8041ffdef2bfe636, 0x00000010},
      {0xf35880003e38f8b0, 0x4f41ffdef2bf8000, 0x00000010},
      {0xf35862e180000000, 0x80000000f2bfe636, 0x00000010},
      {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0x00000015},
      {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0x00000009},
      {0xf30062e13e38f800, 0x4f00ffdef2bf0036, 0x00000000}};

#define CEQI_CLTI_CLEI_S_DF(lanes, mask, func)                                \
  [](uint64_t ws, uint32_t ui5) {                                             \
    uint64_t res = 0;                                                         \
    int elem_size = kMSARegSize / lanes;                                      \
    int64_t i5 = ArithmeticShiftRight(static_cast<int64_t>(ui5) << 59, 59);   \
    for (int i = 0; i < lanes / 2; ++i) {                                     \
      int shift = elem_size * i;                                              \
      int64_t elem =                                                          \
          static_cast<int64_t>(((ws >> shift) & mask) << (64 - elem_size)) >> \
          (64 - elem_size);                                                   \
      res |= static_cast<uint64_t>((func)&mask) << shift;                     \
    }                                                                         \
    return res;                                                               \
  }

#define CEQI_CLTI_CLEI_U_DF(lanes, mask, func) \
  [](uint64_t ws, uint64_t ui5) {              \
    uint64_t res = 0;                          \
    int elem_size = kMSARegSize / lanes;       \
    for (int i = 0; i < lanes / 2; ++i) {      \
      int shift = elem_size * i;               \
      uint64_t elem = (ws >> shift) & mask;    \
      res |= ((func)&mask) << shift;           \
    }                                          \
    return res;                                \
  }

  for (size_t i = 0; i < sizeof(tc) / sizeof(TestCaseMsaI5); ++i) {
    run_msa_i5(&tc[i], true,
               [](MacroAssembler& assm, int32_t i5) { __ ceqi_b(w2, w0, i5); },
               CEQI_CLTI_CLEI_S_DF(kMSALanesByte, UINT8_MAX,
                                   !Compare(elem, i5) ? -1u : 0u));

    run_msa_i5(&tc[i], true,
               [](MacroAssembler& assm, int32_t i5) { __ ceqi_h(w2, w0, i5); },
               CEQI_CLTI_CLEI_S_DF(kMSALanesHalf, UINT16_MAX,
                                   !Compare(elem, i5) ? -1u : 0u));

    run_msa_i5(&tc[i], true,
               [](MacroAssembler& assm, int32_t i5) { __ ceqi_w(w2, w0, i5); },
               CEQI_CLTI_CLEI_S_DF(kMSALanesWord, UINT32_MAX,
                                   !Compare(elem, i5) ? -1u : 0u));

    run_msa_i5(&tc[i], true,
               [](MacroAssembler& assm, int32_t i5) { __ ceqi_d(w2, w0, i5); },
               CEQI_CLTI_CLEI_S_DF(kMSALanesDword, UINT64_MAX,
                                   !Compare(elem, i5) ? -1u : 0u));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ clti_s_b(w2, w0, i5); },
        CEQI_CLTI_CLEI_S_DF(kMSALanesByte, UINT8_MAX,
                            (Compare(elem, i5) == -1) ? -1u : 0u));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ clti_s_h(w2, w0, i5); },
        CEQI_CLTI_CLEI_S_DF(kMSALanesHalf, UINT16_MAX,
                            (Compare(elem, i5) == -1) ? -1u : 0u));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ clti_s_w(w2, w0, i5); },
        CEQI_CLTI_CLEI_S_DF(kMSALanesWord, UINT32_MAX,
                            (Compare(elem, i5) == -1) ? -1u : 0u));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ clti_s_d(w2, w0, i5); },
        CEQI_CLTI_CLEI_S_DF(kMSALanesDword, UINT64_MAX,
                            (Compare(elem, i5) == -1) ? -1ull : 0ull));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ clei_s_b(w2, w0, i5); },
        CEQI_CLTI_CLEI_S_DF(kMSALanesByte, UINT8_MAX,
                            (Compare(elem, i5) != 1) ? -1u : 0u));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ clei_s_h(w2, w0, i5); },
        CEQI_CLTI_CLEI_S_DF(kMSALanesHalf, UINT16_MAX,
                            (Compare(elem, i5) != 1) ? -1u : 0u));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ clei_s_w(w2, w0, i5); },
        CEQI_CLTI_CLEI_S_DF(kMSALanesWord, UINT32_MAX,
                            (Compare(elem, i5) != 1) ? -1u : 0u));

    run_msa_i5(
        &tc[i], true,
        [](MacroAssembler& assm, int32_t i5) { __ clei_s_d(w2, w0, i5); },
        CEQI_CLTI_CLEI_S_DF(kMSALanesDword, UINT64_MAX,
                            (Compare(elem, i5) != 1) ? -1ull : 0ull));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ clti_u_b(w2, w0, i5); },
        CEQI_CLTI_CLEI_U_DF(kMSALanesByte, UINT8_MAX,
                            (Compare(elem, ui5) == -1) ? -1ull : 0ull));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ clti_u_h(w2, w0, i5); },
        CEQI_CLTI_CLEI_U_DF(kMSALanesHalf, UINT16_MAX,
                            (Compare(elem, ui5) == -1) ? -1ull : 0ull));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ clti_u_w(w2, w0, i5); },
        CEQI_CLTI_CLEI_U_DF(kMSALanesWord, UINT32_MAX,
                            (Compare(elem, ui5) == -1) ? -1ull : 0ull));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ clti_u_d(w2, w0, i5); },
        CEQI_CLTI_CLEI_U_DF(kMSALanesDword, UINT64_MAX,
                            (Compare(elem, ui5) == -1) ? -1ull : 0ull));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ clei_u_b(w2, w0, i5); },
        CEQI_CLTI_CLEI_U_DF(kMSALanesByte, UINT8_MAX,
                            (Compare(elem, ui5) != 1) ? -1ull : 0ull));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ clei_u_h(w2, w0, i5); },
        CEQI_CLTI_CLEI_U_DF(kMSALanesHalf, UINT16_MAX,
                            (Compare(elem, ui5) != 1) ? -1ull : 0ull));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ clei_u_w(w2, w0, i5); },
        CEQI_CLTI_CLEI_U_DF(kMSALanesWord, UINT32_MAX,
                            (Compare(elem, ui5) != 1) ? -1ull : 0ull));

    run_msa_i5(
        &tc[i], false,
        [](MacroAssembler& assm, int32_t i5) { __ clei_u_d(w2, w0, i5); },
        CEQI_CLTI_CLEI_U_DF(kMSALanesDword, UINT64_MAX,
                            (Compare(elem, ui5) != 1) ? -1ull : 0ull));
  }
#undef CEQI_CLTI_CLEI_S_DF
#undef CEQI_CLTI_CLEI_U_DF
}

struct TestCaseMsa2R {
  uint64_t ws_lo;
  uint64_t ws_hi;
  uint64_t exp_res_lo;
  uint64_t exp_res_hi;
};

template <typename Func>
void run_msa_2r(struct TestCaseMsa2R* input, Func Generate2RInstructionFunc) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  CpuFeatureScope fscope(&assm, MIPS_SIMD);
  msa_reg_t res;

  __ li(t0, static_cast<uint32_t>(input->ws_lo & 0xffffffff));
  __ li(t1, static_cast<uint32_t>((input->ws_lo >> 32) & 0xffffffff));
  __ insert_w(w0, 0, t0);
  __ insert_w(w0, 1, t1);
  __ li(t0, static_cast<uint32_t>(input->ws_hi & 0xffffffff));
  __ li(t1, static_cast<uint32_t>((input->ws_hi >> 32) & 0xffffffff));
  __ insert_w(w0, 2, t0);
  __ insert_w(w0, 3, t1);

  Generate2RInstructionFunc(assm);

  __ copy_u_w(t2, w2, 0);
  __ sw(t2, MemOperand(a0, 0));
  __ copy_u_w(t2, w2, 1);
  __ sw(t2, MemOperand(a0, 4));
  __ copy_u_w(t2, w2, 2);
  __ sw(t2, MemOperand(a0, 8));
  __ copy_u_w(t2, w2, 3);
  __ sw(t2, MemOperand(a0, 12));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F3 f = FUNCTION_CAST<F3>(code->entry());

  (CALL_GENERATED_CODE(isolate, f, &res, 0, 0, 0, 0));

  CHECK_EQ(input->exp_res_lo, res.d[0]);
  CHECK_EQ(input->exp_res_hi, res.d[1]);
}

TEST(MSA_pcnt) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsa2R tc_b[] = {// ws_lo, ws_hi, exp_res_lo, exp_res_hi
                                 {0x0000000000000000, 0x0000000000000000, 0, 0},
                                 {0xffffffffffffffff, 0xffffffffffffffff,
                                  0x0808080808080808, 0x0808080808080808},
                                 {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c,
                                  0x0204050405050504, 0x0704030503070304},
                                 {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8,
                                  0x0404040303040207, 0x0403010504060403},
                                 {0xf35862e13e38f8b0, 0x4f41ffdef2bfe636,
                                  0x0603030405030503, 0x0502080605070504}};

  struct TestCaseMsa2R tc_h[] = {// ws_lo, ws_hi, exp_res_lo, exp_res_hi
                                 {0x0000000000000000, 0x0000000000000000, 0, 0},
                                 {0xffffffffffffffff, 0xffffffffffffffff,
                                  0x0010001000100010, 0x0010001000100010},
                                 {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c,
                                  0x00060009000a0009, 0x000b0008000a0007},
                                 {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8,
                                  0x0008000700070009, 0x00070006000a0007},
                                 {0xf35862e13e38f8b0, 0x4f41ffdef2bfe636,
                                  0x0009000700080008, 0x0007000e000c0009}};

  struct TestCaseMsa2R tc_w[] = {// ws_lo, ws_hi, exp_res_lo, exp_res_hi
                                 {0x0000000000000000, 0x0000000000000000, 0, 0},
                                 {0xffffffffffffffff, 0xffffffffffffffff,
                                  0x0000002000000020, 0x0000002000000020},
                                 {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c,
                                  0x0000000f00000013, 0x0000001300000011},
                                 {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8,
                                  0x0000000f00000010, 0x0000000d00000011},
                                 {0xf35862e13e38f8b0, 0x4f41ffdef2bfe636,
                                  0x0000001000000010, 0x0000001500000015}};

  struct TestCaseMsa2R tc_d[] = {
      // ws_lo, ws_hi, exp_res_lo, exp_res_hi
      {0x0000000000000000, 0x0000000000000000, 0, 0},
      {0xffffffffffffffff, 0xffffffffffffffff, 0x40, 0x40},
      {0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 0x22, 0x24},
      {0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 0x1f, 0x1e},
      {0xf35862e13e38f8b0, 0x4f41ffdef2bfe636, 0x20, 0x2a}};

  for (size_t i = 0; i < sizeof(tc_b) / sizeof(TestCaseMsa2R); ++i) {
    run_msa_2r(&tc_b[i], [](MacroAssembler& assm) { __ pcnt_b(w2, w0); });
    run_msa_2r(&tc_h[i], [](MacroAssembler& assm) { __ pcnt_h(w2, w0); });
    run_msa_2r(&tc_w[i], [](MacroAssembler& assm) { __ pcnt_w(w2, w0); });
    run_msa_2r(&tc_d[i], [](MacroAssembler& assm) { __ pcnt_d(w2, w0); });
  }
}

TEST(MSA_nlzc) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsa2R tc_b[] = {// ws_lo, ws_hi, exp_res_lo, exp_res_hi
                                 {0x0000000000000000, 0x0000000000000000,
                                  0x0808080808080808, 0x0808080808080808},
                                 {0xffffffffffffffff, 0xffffffffffffffff, 0, 0},
                                 {0x1169350b07030100, 0x7f011402381f0a6c,
                                  0x0301020405060708, 0x0107030602030401},
                                 {0x010806003478121f, 0x03013016073f7b08,
                                  0x0704050802010303, 0x0607020305020104},
                                 {0x0168321100083803, 0x07113f03013f1676,
                                  0x0701020308040206, 0x0503020607020301}};

  struct TestCaseMsa2R tc_h[] = {// ws_lo, ws_hi, exp_res_lo, exp_res_hi
                                 {0x0000000000000000, 0x0000000000000000,
                                  0x0010001000100010, 0x0010001000100010},
                                 {0xffffffffffffffff, 0xffffffffffffffff, 0, 0},
                                 {0x00010007000a003c, 0x37a5001e00010002,
                                  0x000f000d000c000a, 0x0002000b000f000e},
                                 {0x0026066200780edf, 0x003d0003000f00c8,
                                  0x000a000500090004, 0x000a000e000c0008},
                                 {0x335807e100480030, 0x01410fde12bf5636,
                                  0x000200050009000a, 0x0007000400030001}};

  struct TestCaseMsa2R tc_w[] = {// ws_lo, ws_hi, exp_res_lo, exp_res_hi
                                 {0x0000000000000000, 0x0000000000000000,
                                  0x0000002000000020, 0x0000002000000020},
                                 {0xffffffffffffffff, 0xffffffffffffffff, 0, 0},
                                 {0x00000005000007c3, 0x000014ae00006a9c,
                                  0x0000001d00000015, 0x0000001300000011},
                                 {0x00009362000112df, 0x000380d6003f8bc8,
                                  0x000000100000000f, 0x0000000e0000000a},
                                 {0x135862e17e38f8b0, 0x0061ffde03bfe636,
                                  0x0000000300000001, 0x0000000900000006}};

  struct TestCaseMsa2R tc_d[] = {
      // ws_lo, ws_hi, exp_res_lo, exp_res_hi
      {0x0000000000000000, 0x0000000000000000, 0x40, 0x40},
      {0xffffffffffffffff, 0xffffffffffffffff, 0, 0},
      {0x000000000000014e, 0x00000000000176da, 0x37, 0x2f},
      {0x00000062c4e812df, 0x000065d68b3f8bc8, 0x19, 0x11},
      {0x00000000e338f8b0, 0x0754534acab32654, 0x20, 0x5}};

  for (size_t i = 0; i < sizeof(tc_b) / sizeof(TestCaseMsa2R); ++i) {
    run_msa_2r(&tc_b[i], [](MacroAssembler& assm) { __ nlzc_b(w2, w0); });
    run_msa_2r(&tc_h[i], [](MacroAssembler& assm) { __ nlzc_h(w2, w0); });
    run_msa_2r(&tc_w[i], [](MacroAssembler& assm) { __ nlzc_w(w2, w0); });
    run_msa_2r(&tc_d[i], [](MacroAssembler& assm) { __ nlzc_d(w2, w0); });
  }
}

TEST(MSA_nloc) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsa2R tc_b[] = {// ws_lo, ws_hi, exp_res_lo, exp_res_hi
                                 {0xffffffffffffffff, 0xffffffffffffffff,
                                  0x0808080808080808, 0x0808080808080808},
                                 {0x0000000000000000, 0x0000000000000000, 0, 0},
                                 {0xEE96CAF4F8FCFEFF, 0x80FEEBFDC7E0F593,
                                  0x0301020405060708, 0x0107030602030401},
                                 {0xFEF7F9FFCB87EDE0, 0xFCFECFE9F8C084F7,
                                  0x0704050802010303, 0x0607020305020104},
                                 {0xFE97CDEEFFF7C7FC, 0xF8EEC0FCFEC0E989,
                                  0x0701020308040206, 0x0503020607020301}};

  struct TestCaseMsa2R tc_h[] = {// ws_lo, ws_hi, exp_res_lo, exp_res_hi
                                 {0xffffffffffffffff, 0xffffffffffffffff,
                                  0x0010001000100010, 0x0010001000100010},
                                 {0x0000000000000000, 0x0000000000000000, 0, 0},
                                 {0xFFFEFFF8FFF5FFC3, 0xC85AFFE1FFFEFFFD,
                                  0x000f000d000c000a, 0x0002000b000f000e},
                                 {0xFFD9F99DFF87F120, 0xFFC2FFFCFFF0FF37,
                                  0x000a000500090004, 0x000a000e000c0008},
                                 {0xCCA7F81EFFB7FFCF, 0xFEBEF021ED40A9C9,
                                  0x000200050009000a, 0x0007000400030001}};

  struct TestCaseMsa2R tc_w[] = {// ws_lo, ws_hi, exp_res_lo, exp_res_hi
                                 {0xffffffffffffffff, 0xffffffffffffffff,
                                  0x0000002000000020, 0x0000002000000020},
                                 {0x0000000000000000, 0x0000000000000000, 0, 0},
                                 {0xFFFFFFFAFFFFF83C, 0xFFFFEB51FFFF9563,
                                  0x0000001d00000015, 0x0000001300000011},
                                 {0xFFFF6C9DFFFEED20, 0xFFFC7F29FFC07437,
                                  0x000000100000000f, 0x0000000e0000000a},
                                 {0xECA79D1E81C7074F, 0xFF9E0021FC4019C9,
                                  0x0000000300000001, 0x0000000900000006}};

  struct TestCaseMsa2R tc_d[] = {
      // ws_lo, ws_hi, exp_res_lo, exp_res_hi
      {0xffffffffffffffff, 0xffffffffffffffff, 0x40, 0x40},
      {0x0000000000000000, 0x0000000000000000, 0, 0},
      {0xFFFFFFFFFFFFFEB1, 0xFFFFFFFFFFFE8925, 0x37, 0x2f},
      {0xFFFFFF9D3B17ED20, 0xFFFF9A2974C07437, 0x19, 0x11},
      {0xFFFFFFFF1CC7074F, 0xF8ABACB5354CD9AB, 0x20, 0x5}};

  for (size_t i = 0; i < sizeof(tc_b) / sizeof(TestCaseMsa2R); ++i) {
    run_msa_2r(&tc_b[i], [](MacroAssembler& assm) { __ nloc_b(w2, w0); });
    run_msa_2r(&tc_h[i], [](MacroAssembler& assm) { __ nloc_h(w2, w0); });
    run_msa_2r(&tc_w[i], [](MacroAssembler& assm) { __ nloc_w(w2, w0); });
    run_msa_2r(&tc_d[i], [](MacroAssembler& assm) { __ nloc_d(w2, w0); });
  }
}

struct TestCaseMsaVector {
  uint64_t wd_lo;
  uint64_t wd_hi;
  uint64_t ws_lo;
  uint64_t ws_hi;
  uint64_t wt_lo;
  uint64_t wt_hi;
};

template <typename InstFunc, typename OperFunc>
void run_msa_vector(struct TestCaseMsaVector* input,
                    InstFunc GenerateVectorInstructionFunc,
                    OperFunc GenerateOperationFunc) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  CpuFeatureScope fscope(&assm, MIPS_SIMD);
  msa_reg_t res;

#define LOAD_W_REG(lo, hi, w_reg)                            \
  __ li(t0, static_cast<uint32_t>(lo & 0xffffffff));         \
  __ li(t1, static_cast<uint32_t>((lo >> 32) & 0xffffffff)); \
  __ insert_w(w_reg, 0, t0);                                 \
  __ insert_w(w_reg, 1, t1);                                 \
  __ li(t0, static_cast<uint32_t>(hi & 0xffffffff));         \
  __ li(t1, static_cast<uint32_t>((hi >> 32) & 0xffffffff)); \
  __ insert_w(w_reg, 2, t0);                                 \
  __ insert_w(w_reg, 3, t1)

  LOAD_W_REG(input->ws_lo, input->ws_hi, w0);
  LOAD_W_REG(input->wt_lo, input->wt_hi, w2);
  LOAD_W_REG(input->wd_lo, input->wd_hi, w4);
#undef LOAD_W_REG

  GenerateVectorInstructionFunc(assm);

  __ copy_u_w(t2, w4, 0);
  __ sw(t2, MemOperand(a0, 0));
  __ copy_u_w(t2, w4, 1);
  __ sw(t2, MemOperand(a0, 4));
  __ copy_u_w(t2, w4, 2);
  __ sw(t2, MemOperand(a0, 8));
  __ copy_u_w(t2, w4, 3);
  __ sw(t2, MemOperand(a0, 12));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F3 f = FUNCTION_CAST<F3>(code->entry());

  (CALL_GENERATED_CODE(isolate, f, &res, 0, 0, 0, 0));

  CHECK_EQ(GenerateOperationFunc(input->wd_lo, input->ws_lo, input->wt_lo),
           res.d[0]);
  CHECK_EQ(GenerateOperationFunc(input->wd_hi, input->ws_hi, input->wt_hi),
           res.d[1]);
}

TEST(MSA_vector) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsaVector tc[] = {
      // wd_lo, wd_hi, ws_lo, ws_hi, wt_lo, wt_hi
      {0xf35862e13e38f8b0, 0x4f41ffdef2bfe636, 0xdcd39d91f9057627,
       0x64be4f6dbe9caa51, 0x6b23de1a687d9cb9, 0x49547aad691da4ca},
      {0xf35862e13e38f8b0, 0x4f41ffdef2bfe636, 0x401614523d830549,
       0xd7c46d613f50eddd, 0x52284cbc60a1562b, 0x1756ed510d8849cd},
      {0xf35862e13e38f8b0, 0x4f41ffdef2bfe636, 0xd6e2d2ebcb40d72f,
       0x13a619afce67b079, 0x36cce284343e40f9, 0xb4e8f44fd148bf7f}};

  for (size_t i = 0; i < sizeof(tc) / sizeof(TestCaseMsaVector); ++i) {
    run_msa_vector(
        &tc[i], [](MacroAssembler& assm) { __ and_v(w4, w0, w2); },
        [](uint64_t wd, uint64_t ws, uint64_t wt) { return ws & wt; });
    run_msa_vector(
        &tc[i], [](MacroAssembler& assm) { __ or_v(w4, w0, w2); },
        [](uint64_t wd, uint64_t ws, uint64_t wt) { return ws | wt; });
    run_msa_vector(
        &tc[i], [](MacroAssembler& assm) { __ nor_v(w4, w0, w2); },
        [](uint64_t wd, uint64_t ws, uint64_t wt) { return ~(ws | wt); });
    run_msa_vector(
        &tc[i], [](MacroAssembler& assm) { __ xor_v(w4, w0, w2); },
        [](uint64_t wd, uint64_t ws, uint64_t wt) { return ws ^ wt; });
    run_msa_vector(&tc[i], [](MacroAssembler& assm) { __ bmnz_v(w4, w0, w2); },
                   [](uint64_t wd, uint64_t ws, uint64_t wt) {
                     return (ws & wt) | (wd & ~wt);
                   });
    run_msa_vector(&tc[i], [](MacroAssembler& assm) { __ bmz_v(w4, w0, w2); },
                   [](uint64_t wd, uint64_t ws, uint64_t wt) {
                     return (ws & ~wt) | (wd & wt);
                   });
    run_msa_vector(&tc[i], [](MacroAssembler& assm) { __ bsel_v(w4, w0, w2); },
                   [](uint64_t wd, uint64_t ws, uint64_t wt) {
                     return (ws & ~wd) | (wt & wd);
                   });
  }
}

struct TestCaseMsaBit {
  uint64_t wd_lo;
  uint64_t wd_hi;
  uint64_t ws_lo;
  uint64_t ws_hi;
  uint32_t m;
};

template <typename InstFunc, typename OperFunc>
void run_msa_bit(struct TestCaseMsaBit* input, InstFunc GenerateInstructionFunc,
                 OperFunc GenerateOperationFunc) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  CpuFeatureScope fscope(&assm, MIPS_SIMD);
  msa_reg_t res;

#define LOAD_W_REG(lo, hi, w_reg)                            \
  __ li(t0, static_cast<uint32_t>(lo & 0xffffffff));         \
  __ li(t1, static_cast<uint32_t>((lo >> 32) & 0xffffffff)); \
  __ insert_w(w_reg, 0, t0);                                 \
  __ insert_w(w_reg, 1, t1);                                 \
  __ li(t0, static_cast<uint32_t>(hi & 0xffffffff));         \
  __ li(t1, static_cast<uint32_t>((hi >> 32) & 0xffffffff)); \
  __ insert_w(w_reg, 2, t0);                                 \
  __ insert_w(w_reg, 3, t1)

  LOAD_W_REG(input->ws_lo, input->ws_hi, w0);
  LOAD_W_REG(input->wd_lo, input->wd_hi, w2);
#undef LOAD_W_REG

  GenerateInstructionFunc(assm, input->m);

  __ copy_u_w(t2, w2, 0);
  __ sw(t2, MemOperand(a0, 0));
  __ copy_u_w(t2, w2, 1);
  __ sw(t2, MemOperand(a0, 4));
  __ copy_u_w(t2, w2, 2);
  __ sw(t2, MemOperand(a0, 8));
  __ copy_u_w(t2, w2, 3);
  __ sw(t2, MemOperand(a0, 12));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F3 f = FUNCTION_CAST<F3>(code->entry());

  (CALL_GENERATED_CODE(isolate, f, &res, 0, 0, 0, 0));

  CHECK_EQ(GenerateOperationFunc(input->wd_lo, input->ws_lo, input->m),
           res.d[0]);
  CHECK_EQ(GenerateOperationFunc(input->wd_hi, input->ws_hi, input->m),
           res.d[1]);
}

TEST(MSA_slli_srai_srli) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsaBit tc[] = {
      // wd_lo, wd_hi     ws_lo,              ws_hi, m
      {0, 0, 0xf35862e13e38f8b0, 0x4f41ffdef2bfe636, 3},
      {0, 0, 0x64be4f6dbe9caa51, 0x6b23de1a687d9cb9, 5},
      {0, 0, 0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 9},
      {0, 0, 0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 13},
      {0, 0, 0x566be7ba4365b70a, 0x01ebbc1937d76cb4, 21},
      {0, 0, 0x380e2deb9d3f8aae, 0x017e0de0bcc6ca42, 30},
      {0, 0, 0xa46a3a9bcb43f4e5, 0x1c62c8473bdfcffb, 45},
      {0, 0, 0xf6759d85f23b5a2b, 0x5c042ae42c6d12c1, 61}};

#define SLLI_SRLI_DF(lanes, mask, func)      \
  [](uint64_t wd, uint64_t ws, uint32_t m) { \
    uint64_t res = 0;                        \
    int elem_size = kMSARegSize / lanes;     \
    for (int i = 0; i < lanes / 2; ++i) {    \
      int shift = elem_size * i;             \
      uint64_t elem = (ws >> shift) & mask;  \
      res |= ((func)&mask) << shift;         \
    }                                        \
    return res;                              \
  }

#define SRAI_DF(lanes, mask, func)                                            \
  [](uint64_t wd, uint64_t ws, uint32_t m) {                                  \
    uint64_t res = 0;                                                         \
    int elem_size = kMSARegSize / lanes;                                      \
    for (int i = 0; i < lanes / 2; ++i) {                                     \
      int shift = elem_size * i;                                              \
      int64_t elem =                                                          \
          static_cast<int64_t>(((ws >> shift) & mask) << (64 - elem_size)) >> \
          (64 - elem_size);                                                   \
      res |= static_cast<uint64_t>((func)&mask) << shift;                     \
    }                                                                         \
    return res;                                                               \
  }

  for (size_t i = 0; i < sizeof(tc) / sizeof(TestCaseMsaBit); ++i) {
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ slli_b(w2, w0, m % 8); },
        SLLI_SRLI_DF(kMSALanesByte, UINT8_MAX, (elem << (m % elem_size))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ slli_h(w2, w0, m % 16); },
        SLLI_SRLI_DF(kMSALanesHalf, UINT16_MAX, (elem << (m % elem_size))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ slli_w(w2, w0, m % 32); },
        SLLI_SRLI_DF(kMSALanesWord, UINT32_MAX, (elem << (m % elem_size))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ slli_d(w2, w0, m % 64); },
        SLLI_SRLI_DF(kMSALanesDword, UINT64_MAX, (elem << (m % elem_size))));

    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srli_b(w2, w0, m % 8); },
        SLLI_SRLI_DF(kMSALanesByte, UINT8_MAX, (elem >> (m % elem_size))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srli_h(w2, w0, m % 16); },
        SLLI_SRLI_DF(kMSALanesHalf, UINT16_MAX, (elem >> (m % elem_size))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srli_w(w2, w0, m % 32); },
        SLLI_SRLI_DF(kMSALanesWord, UINT32_MAX, (elem >> (m % elem_size))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srli_d(w2, w0, m % 64); },
        SLLI_SRLI_DF(kMSALanesDword, UINT64_MAX, (elem >> (m % elem_size))));

    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srlri_b(w2, w0, m % 8); },
        SLLI_SRLI_DF(
            kMSALanesByte, UINT8_MAX,
            (elem >> (m % elem_size)) + ((elem >> (m % elem_size - 1)) & 0x1)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srlri_h(w2, w0, m % 16); },
        SLLI_SRLI_DF(
            kMSALanesHalf, UINT16_MAX,
            (elem >> (m % elem_size)) + ((elem >> (m % elem_size - 1)) & 0x1)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srlri_w(w2, w0, m % 32); },
        SLLI_SRLI_DF(
            kMSALanesWord, UINT32_MAX,
            (elem >> (m % elem_size)) + ((elem >> (m % elem_size - 1)) & 0x1)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srlri_d(w2, w0, m % 64); },
        SLLI_SRLI_DF(
            kMSALanesDword, UINT64_MAX,
            (elem >> (m % elem_size)) + ((elem >> (m % elem_size - 1)) & 0x1)));

    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srai_b(w2, w0, m % 8); },
        SRAI_DF(kMSALanesByte, UINT8_MAX,
                ArithmeticShiftRight(elem, m % elem_size)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srai_h(w2, w0, m % 16); },
        SRAI_DF(kMSALanesHalf, UINT16_MAX,
                ArithmeticShiftRight(elem, m % elem_size)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srai_w(w2, w0, m % 32); },
        SRAI_DF(kMSALanesWord, UINT32_MAX,
                ArithmeticShiftRight(elem, m % elem_size)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srai_d(w2, w0, m % 64); },
        SRAI_DF(kMSALanesDword, UINT64_MAX,
                ArithmeticShiftRight(elem, m % elem_size)));

    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srari_b(w2, w0, m % 8); },
        SRAI_DF(kMSALanesByte, UINT8_MAX,
                ArithmeticShiftRight(elem, m % elem_size) +
                    ((elem >> (m % elem_size - 1)) & 0x1)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srari_h(w2, w0, m % 16); },
        SRAI_DF(kMSALanesHalf, UINT16_MAX,
                ArithmeticShiftRight(elem, m % elem_size) +
                    ((elem >> (m % elem_size - 1)) & 0x1)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srari_w(w2, w0, m % 32); },
        SRAI_DF(kMSALanesWord, UINT32_MAX,
                ArithmeticShiftRight(elem, m % elem_size) +
                    ((elem >> (m % elem_size - 1)) & 0x1)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ srari_d(w2, w0, m % 64); },
        SRAI_DF(kMSALanesDword, UINT64_MAX,
                ArithmeticShiftRight(elem, m % elem_size) +
                    ((elem >> (m % elem_size - 1)) & 0x1)));
  }
#undef SLLI_SRLI_DF
#undef SRAI_DF
}

TEST(MSA_bclri_bseti_bnegi) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsaBit tc[] = {
      // wd_lo, wd_hi,    ws_lo,              ws_hi, m
      {0, 0, 0xf35862e13e38f8b0, 0x4f41ffdef2bfe636, 3},
      {0, 0, 0x64be4f6dbe9caa51, 0x6b23de1a687d9cb9, 5},
      {0, 0, 0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 9},
      {0, 0, 0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 13},
      {0, 0, 0x566be7ba4365b70a, 0x01ebbc1937d76cb4, 21},
      {0, 0, 0x380e2deb9d3f8aae, 0x017e0de0bcc6ca42, 30},
      {0, 0, 0xa46a3a9bcb43f4e5, 0x1c62c8473bdfcffb, 45},
      {0, 0, 0xf6759d85f23b5a2b, 0x5c042ae42c6d12c1, 61}};

#define BCLRI_BSETI_BNEGI_DF(lanes, mask, func) \
  [](uint64_t wd, uint64_t ws, uint32_t m) {    \
    uint64_t res = 0;                           \
    int elem_size = kMSARegSize / lanes;        \
    for (int i = 0; i < lanes / 2; ++i) {       \
      int shift = elem_size * i;                \
      uint64_t elem = (ws >> shift) & mask;     \
      res |= ((func)&mask) << shift;            \
    }                                           \
    return res;                                 \
  }

  for (size_t i = 0; i < sizeof(tc) / sizeof(TestCaseMsaBit); ++i) {
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bclri_b(w2, w0, m % 8); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesByte, UINT8_MAX,
                             (~(1ull << (m % elem_size)) & elem)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bclri_h(w2, w0, m % 16); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesHalf, UINT16_MAX,
                             (~(1ull << (m % elem_size)) & elem)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bclri_w(w2, w0, m % 32); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesWord, UINT32_MAX,
                             (~(1ull << (m % elem_size)) & elem)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bclri_d(w2, w0, m % 64); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesDword, UINT64_MAX,
                             (~(1ull << (m % elem_size)) & elem)));

    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bseti_b(w2, w0, m % 8); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesByte, UINT8_MAX,
                             ((1ull << (m % elem_size)) | elem)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bseti_h(w2, w0, m % 16); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesHalf, UINT16_MAX,
                             ((1ull << (m % elem_size)) | elem)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bseti_w(w2, w0, m % 32); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesWord, UINT32_MAX,
                             ((1ull << (m % elem_size)) | elem)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bseti_d(w2, w0, m % 64); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesDword, UINT64_MAX,
                             ((1ull << (m % elem_size)) | elem)));

    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bnegi_b(w2, w0, m % 8); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesByte, UINT8_MAX,
                             ((1ull << (m % elem_size)) ^ elem)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bnegi_h(w2, w0, m % 16); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesHalf, UINT16_MAX,
                             ((1ull << (m % elem_size)) ^ elem)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bnegi_w(w2, w0, m % 32); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesWord, UINT32_MAX,
                             ((1ull << (m % elem_size)) ^ elem)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ bnegi_d(w2, w0, m % 64); },
        BCLRI_BSETI_BNEGI_DF(kMSALanesDword, UINT64_MAX,
                             ((1ull << (m % elem_size)) ^ elem)));
  }
#undef BCLRI_BSETI_BNEGI_DF
}

TEST(MSA_binsli_binsri) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsaBit tc[] = {// wd_lo, wd_hi, ws_lo, ws_hi, m
                                {0x53f4457553bbd5b4, 0x5fb8250eacc296b2,
                                 0xf35862e13e38f8b0, 0x4f41ffdef2bfe636, 3},
                                {0xf61bfdb0f312e6fc, 0xc9437568dd1ea925,
                                 0x64be4f6dbe9caa51, 0x6b23de1a687d9cb9, 5},
                                {0x53f4457553bbd5b4, 0x5fb8250eacc296b2,
                                 0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 9},
                                {0xf61bfdb0f312e6fc, 0xc9437568dd1ea925,
                                 0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 13},
                                {0x53f4457553bbd5b4, 0x5fb8250eacc296b2,
                                 0x566be7ba4365b70a, 0x01ebbc1937d76cb4, 21},
                                {0xf61bfdb0f312e6fc, 0xc9437568dd1ea925,
                                 0x380e2deb9d3f8aae, 0x017e0de0bcc6ca42, 30},
                                {0x53f4457553bbd5b4, 0x5fb8250eacc296b2,
                                 0xa46a3a9bcb43f4e5, 0x1c62c8473bdfcffb, 45},
                                {0xf61bfdb0f312e6fc, 0xc9437568dd1ea925,
                                 0xf6759d85f23b5a2b, 0x5c042ae42c6d12c1, 61}};

#define BINSLI_BINSRI_DF(lanes, mask, func)             \
  [](uint64_t wd, uint64_t ws, uint32_t m) {            \
    uint64_t res = 0;                                   \
    int elem_size = kMSARegSize / lanes;                \
    int bits = m % elem_size + 1;                       \
    for (int i = 0; i < lanes / 2; ++i) {               \
      int shift = elem_size * i;                        \
      uint64_t ws_elem = (ws >> shift) & mask;          \
      if (bits == elem_size) {                          \
        res |= (ws_elem & mask) << shift;               \
      } else {                                          \
        uint64_t r_mask = (1ull << bits) - 1;           \
        uint64_t l_mask = r_mask << (elem_size - bits); \
        USE(l_mask);                                    \
        uint64_t wd_elem = (wd >> shift) & mask;        \
        res |= ((func)&mask) << shift;                  \
      }                                                 \
    }                                                   \
    return res;                                         \
  }

  for (size_t i = 0; i < sizeof(tc) / sizeof(TestCaseMsaBit); ++i) {
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ binsli_b(w2, w0, m % 8); },
        BINSLI_BINSRI_DF(kMSALanesByte, UINT8_MAX,
                         ((ws_elem & l_mask) | (wd_elem & ~l_mask))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ binsli_h(w2, w0, m % 16); },
        BINSLI_BINSRI_DF(kMSALanesHalf, UINT16_MAX,
                         ((ws_elem & l_mask) | (wd_elem & ~l_mask))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ binsli_w(w2, w0, m % 32); },
        BINSLI_BINSRI_DF(kMSALanesWord, UINT32_MAX,
                         ((ws_elem & l_mask) | (wd_elem & ~l_mask))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ binsli_d(w2, w0, m % 64); },
        BINSLI_BINSRI_DF(kMSALanesDword, UINT64_MAX,
                         ((ws_elem & l_mask) | (wd_elem & ~l_mask))));

    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ binsri_b(w2, w0, m % 8); },
        BINSLI_BINSRI_DF(kMSALanesByte, UINT8_MAX,
                         ((ws_elem & r_mask) | (wd_elem & ~r_mask))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ binsri_h(w2, w0, m % 16); },
        BINSLI_BINSRI_DF(kMSALanesHalf, UINT16_MAX,
                         ((ws_elem & r_mask) | (wd_elem & ~r_mask))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ binsri_w(w2, w0, m % 32); },
        BINSLI_BINSRI_DF(kMSALanesWord, UINT32_MAX,
                         ((ws_elem & r_mask) | (wd_elem & ~r_mask))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ binsri_d(w2, w0, m % 64); },
        BINSLI_BINSRI_DF(kMSALanesDword, UINT64_MAX,
                         ((ws_elem & r_mask) | (wd_elem & ~r_mask))));
  }
#undef BINSLI_BINSRI_DF
}

TEST(MSA_sat_s_sat_u) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  struct TestCaseMsaBit tc[] = {
      // wd_lo, wd_hi,    ws_lo,              ws_hi, m
      {0, 0, 0xf35862e13e3808b0, 0x4f41ffdef2bfe636, 3},
      {0, 0, 0x64be4f6dbe9caa51, 0x6b23de1a687d9cb9, 5},
      {0, 0, 0x1169751bb9a7d9c3, 0xf7a594aec8ef8a9c, 9},
      {0, 0, 0x2b665362c4e812df, 0x3a0d80d68b3f8bc8, 13},
      {0, 0, 0x566be7ba4365b70a, 0x01ebbc1937d76cb4, 21},
      {0, 0, 0x380e2deb9d3f8aae, 0x017e0de0bcc6ca42, 30},
      {0, 0, 0xa46a3a9bcb43f4e5, 0x1c62c8473bdfcffb, 45},
      {0, 0, 0xf6759d85f23b5a2b, 0x5c042ae42c6d12c1, 61}};

#define SAT_DF(lanes, mask, func)                                              \
  [](uint64_t wd, uint64_t ws, uint32_t m) {                                   \
    uint64_t res = 0;                                                          \
    int elem_size = kMSARegSize / lanes;                                       \
    m %= elem_size;                                                            \
    for (int i = 0; i < lanes / 2; ++i) {                                      \
      int shift = elem_size * i;                                               \
      uint64_t elem_u64 = (ws >> shift) & mask;                                \
      int64_t elem_i64 = static_cast<int64_t>(elem_u64 << (64 - elem_size)) >> \
                         (64 - elem_size);                                     \
      USE(elem_i64);                                                           \
      res |= ((func)&mask) << shift;                                           \
    }                                                                          \
    return res;                                                                \
  }

#define M_MAX_INT(x) static_cast<int64_t>((1LL << ((x)-1)) - 1)
#define M_MIN_INT(x) static_cast<int64_t>(-(1LL << ((x)-1)))
#define M_MAX_UINT(x) static_cast<uint64_t>(-1ULL >> (64 - (x)))

  for (size_t i = 0; i < sizeof(tc) / sizeof(TestCaseMsaBit); ++i) {
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ sat_u_b(w2, w0, m % 8); },
        SAT_DF(kMSALanesByte, UINT8_MAX,
               (elem_u64 < M_MAX_UINT(m + 1) ? elem_u64 : M_MAX_UINT(m + 1))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ sat_u_h(w2, w0, m % 16); },
        SAT_DF(kMSALanesHalf, UINT16_MAX,
               (elem_u64 < M_MAX_UINT(m + 1) ? elem_u64 : M_MAX_UINT(m + 1))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ sat_u_w(w2, w0, m % 32); },
        SAT_DF(kMSALanesWord, UINT32_MAX,
               (elem_u64 < M_MAX_UINT(m + 1) ? elem_u64 : M_MAX_UINT(m + 1))));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ sat_u_d(w2, w0, m % 64); },
        SAT_DF(kMSALanesDword, UINT64_MAX,
               (elem_u64 < M_MAX_UINT(m + 1) ? elem_u64 : M_MAX_UINT(m + 1))));

    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ sat_s_b(w2, w0, m % 8); },
        SAT_DF(
            kMSALanesByte, UINT8_MAX,
            (elem_i64 < M_MIN_INT(m + 1)
                 ? M_MIN_INT(m + 1)
                 : elem_i64 > M_MAX_INT(m + 1) ? M_MAX_INT(m + 1) : elem_i64)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ sat_s_h(w2, w0, m % 16); },
        SAT_DF(
            kMSALanesHalf, UINT16_MAX,
            (elem_i64 < M_MIN_INT(m + 1)
                 ? M_MIN_INT(m + 1)
                 : elem_i64 > M_MAX_INT(m + 1) ? M_MAX_INT(m + 1) : elem_i64)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ sat_s_w(w2, w0, m % 32); },
        SAT_DF(
            kMSALanesWord, UINT32_MAX,
            (elem_i64 < M_MIN_INT(m + 1)
                 ? M_MIN_INT(m + 1)
                 : elem_i64 > M_MAX_INT(m + 1) ? M_MAX_INT(m + 1) : elem_i64)));
    run_msa_bit(
        &tc[i],
        [](MacroAssembler& assm, uint32_t m) { __ sat_s_d(w2, w0, m % 64); },
        SAT_DF(
            kMSALanesDword, UINT64_MAX,
            (elem_i64 < M_MIN_INT(m + 1)
                 ? M_MIN_INT(m + 1)
                 : elem_i64 > M_MAX_INT(m + 1) ? M_MAX_INT(m + 1) : elem_i64)));
  }

#undef SAT_DF
#undef M_MAX_INT
#undef M_MIN_INT
#undef M_MAX_UINT
}

template <typename InstFunc, typename OperFunc>
void run_msa_i10(int32_t input, InstFunc GenerateVectorInstructionFunc,
                 OperFunc GenerateOperationFunc) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  CpuFeatureScope fscope(&assm, MIPS_SIMD);
  msa_reg_t res;

  GenerateVectorInstructionFunc(assm, input);

  __ copy_u_w(t2, w0, 0);
  __ sw(t2, MemOperand(a0, 0));
  __ copy_u_w(t2, w0, 1);
  __ sw(t2, MemOperand(a0, 4));
  __ copy_u_w(t2, w0, 2);
  __ sw(t2, MemOperand(a0, 8));
  __ copy_u_w(t2, w0, 3);
  __ sw(t2, MemOperand(a0, 12));

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F3 f = FUNCTION_CAST<F3>(code->entry());

  (CALL_GENERATED_CODE(isolate, f, &res, 0, 0, 0, 0));

  CHECK_EQ(GenerateOperationFunc(input), res.d[0]);
  CHECK_EQ(GenerateOperationFunc(input), res.d[1]);
}

TEST(MSA_ldi) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  // signed 10bit integers: -512 .. 511
  int32_t tc[] = {0, -1, 1, 256, -256, -178, 352, -512, 511};

#define LDI_DF(lanes, mask)                                        \
  [](int32_t s10) {                                                \
    uint64_t res = 0;                                              \
    int elem_size = kMSARegSize / lanes;                           \
    int64_t s10_64 =                                               \
        ArithmeticShiftRight(static_cast<int64_t>(s10) << 54, 54); \
    for (int i = 0; i < lanes / 2; ++i) {                          \
      int shift = elem_size * i;                                   \
      res |= static_cast<uint64_t>(s10_64 & mask) << shift;        \
    }                                                              \
    return res;                                                    \
  }

  for (size_t i = 0; i < sizeof(tc) / sizeof(int32_t); ++i) {
    run_msa_i10(tc[i],
                [](MacroAssembler& assm, int32_t s10) { __ ldi_b(w0, s10); },
                LDI_DF(kMSALanesByte, UINT8_MAX));
    run_msa_i10(tc[i],
                [](MacroAssembler& assm, int32_t s10) { __ ldi_h(w0, s10); },
                LDI_DF(kMSALanesHalf, UINT16_MAX));
    run_msa_i10(tc[i],
                [](MacroAssembler& assm, int32_t s10) { __ ldi_w(w0, s10); },
                LDI_DF(kMSALanesWord, UINT32_MAX));
    run_msa_i10(tc[i],
                [](MacroAssembler& assm, int32_t s10) { __ ldi_d(w0, s10); },
                LDI_DF(kMSALanesDword, UINT64_MAX));
  }
#undef LDI_DF
}

template <typename T, typename InstFunc>
void run_msa_mi10(InstFunc GenerateVectorInstructionFunc) {
  Isolate* isolate = CcTest::i_isolate();
  HandleScope scope(isolate);

  MacroAssembler assm(isolate, NULL, 0, v8::internal::CodeObjectRequired::kYes);
  CpuFeatureScope fscope(&assm, MIPS_SIMD);
  T in_test_vector[1024];
  T out_test_vector[1024];

  T* in_array_middle = in_test_vector + arraysize(in_test_vector) / 2;
  T* out_array_middle = out_test_vector + arraysize(out_test_vector) / 2;

  v8::base::RandomNumberGenerator rand_gen(FLAG_random_seed);
  for (unsigned int i = 0; i < arraysize(in_test_vector); i++) {
    in_test_vector[i] = static_cast<T>(rand_gen.NextInt());
    out_test_vector[i] = 0;
  }

  GenerateVectorInstructionFunc(assm);

  __ jr(ra);
  __ nop();

  CodeDesc desc;
  assm.GetCode(isolate, &desc);
  Handle<Code> code = isolate->factory()->NewCode(
      desc, Code::ComputeFlags(Code::STUB), Handle<Code>());
#ifdef OBJECT_PRINT
  code->Print(std::cout);
#endif
  F4 f = FUNCTION_CAST<F4>(code->entry());

  (CALL_GENERATED_CODE(isolate, f, in_array_middle, out_array_middle, 0, 0, 0));

  CHECK_EQ(memcmp(in_test_vector, out_test_vector, arraysize(in_test_vector)),
           0);
}

TEST(MSA_load_store_vector) {
  if (!IsMipsArchVariant(kMips32r6) || !CpuFeatures::IsSupported(MIPS_SIMD))
    return;

  CcTest::InitializeVM();

  run_msa_mi10<uint8_t>([](MacroAssembler& assm) {
    for (int i = -512; i < 512; i += 16) {
      __ ld_b(w0, MemOperand(a0, i));
      __ st_b(w0, MemOperand(a1, i));
    }
  });
  run_msa_mi10<uint16_t>([](MacroAssembler& assm) {
    for (int i = -512; i < 512; i += 8) {
      __ ld_h(w0, MemOperand(a0, i));
      __ st_h(w0, MemOperand(a1, i));
    }
  });
  run_msa_mi10<uint32_t>([](MacroAssembler& assm) {
    for (int i = -512; i < 512; i += 4) {
      __ ld_w(w0, MemOperand(a0, i));
      __ st_w(w0, MemOperand(a1, i));
    }
  });
  run_msa_mi10<uint64_t>([](MacroAssembler& assm) {
    for (int i = -512; i < 512; i += 2) {
      __ ld_d(w0, MemOperand(a0, i));
      __ st_d(w0, MemOperand(a1, i));
    }
  });
#undef LDI_DF
}

#undef __
