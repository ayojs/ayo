// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "test/fuzzer/wasm-fuzzer-common.h"

#include "include/v8.h"
#include "src/isolate.h"
#include "src/objects-inl.h"
#include "src/wasm/wasm-module-builder.h"
#include "src/wasm/wasm-module.h"
#include "src/zone/accounting-allocator.h"
#include "src/zone/zone.h"
#include "test/common/wasm/wasm-module-runner.h"
#include "test/fuzzer/fuzzer-support.h"

#define WASM_CODE_FUZZER_HASH_SEED 83

#if __clang__
// TODO(mostynb@opera.com): remove the using statements and these pragmas.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wheader-hygiene"
#endif

using namespace v8::internal;
using namespace v8::internal::wasm;
using namespace v8::internal::wasm::fuzzer;

#if __clang__
// TODO(mostynb@opera.com): remove the using statements and these pragmas.
#pragma clang diagnostic pop
#endif

static const char* kNameString = "name";
static const size_t kNameStringLength = 4;

int v8::internal::wasm::fuzzer::FuzzWasmSection(SectionCode section,
                                                const uint8_t* data,
                                                size_t size) {
  v8_fuzzer::FuzzerSupport* support = v8_fuzzer::FuzzerSupport::Get();
  v8::Isolate* isolate = support->GetIsolate();
  v8::internal::Isolate* i_isolate = reinterpret_cast<Isolate*>(isolate);

  // Clear any pending exceptions from a prior run.
  if (i_isolate->has_pending_exception()) {
    i_isolate->clear_pending_exception();
  }

  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);
  v8::Context::Scope context_scope(support->GetContext());
  v8::TryCatch try_catch(isolate);

  AccountingAllocator allocator;
  Zone zone(&allocator, ZONE_NAME);

  ZoneBuffer buffer(&zone);
  buffer.write_u32(kWasmMagic);
  buffer.write_u32(kWasmVersion);
  if (section == kNameSectionCode) {
    buffer.write_u8(kUnknownSectionCode);
    buffer.write_size(size + kNameStringLength + 1);
    buffer.write_u8(kNameStringLength);
    buffer.write(reinterpret_cast<const uint8_t*>(kNameString),
                 kNameStringLength);
    buffer.write(data, size);
  } else {
    buffer.write_u8(section);
    buffer.write_size(size);
    buffer.write(data, size);
  }

  ErrorThrower thrower(i_isolate, "decoder");

  std::unique_ptr<const WasmModule> module(testing::DecodeWasmModuleForTesting(
      i_isolate, &thrower, buffer.begin(), buffer.end(), kWasmOrigin));

  return 0;
}

int WasmExecutionFuzzer::FuzzWasmModule(

    const uint8_t* data, size_t size) {
  // Save the flag so that we can change it and restore it later.
  bool generate_test = FLAG_wasm_code_fuzzer_gen_test;
  if (generate_test) {
    OFStream os(stdout);

    os << "// Copyright 2017 the V8 project authors. All rights reserved."
       << std::endl;
    os << "// Use of this source code is governed by a BSD-style license that "
          "can be"
       << std::endl;
    os << "// found in the LICENSE file." << std::endl;
    os << std::endl;
    os << "load(\"test/mjsunit/wasm/wasm-constants.js\");" << std::endl;
    os << "load(\"test/mjsunit/wasm/wasm-module-builder.js\");" << std::endl;
    os << std::endl;
    os << "(function() {" << std::endl;
    os << "  var builder = new WasmModuleBuilder();" << std::endl;
    os << "  builder.addMemory(16, 32, false);" << std::endl;
    os << "  builder.addFunction(\"test\", kSig_i_iii)" << std::endl;
    os << "    .addBodyWithEnd([" << std::endl;
  }
  v8_fuzzer::FuzzerSupport* support = v8_fuzzer::FuzzerSupport::Get();
  v8::Isolate* isolate = support->GetIsolate();
  Isolate* i_isolate = reinterpret_cast<Isolate*>(isolate);

  // Clear any pending exceptions from a prior run.
  if (i_isolate->has_pending_exception()) {
    i_isolate->clear_pending_exception();
  }

  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);
  v8::Context::Scope context_scope(support->GetContext());
  v8::TryCatch try_catch(isolate);
  HandleScope scope(i_isolate);

  AccountingAllocator allocator;
  Zone zone(&allocator, ZONE_NAME);

  ZoneBuffer buffer(&zone);
  int32_t num_args = 0;
  std::unique_ptr<WasmValue[]> interpreter_args;
  std::unique_ptr<Handle<Object>[]> compiler_args;
  if (!GenerateModule(i_isolate, &zone, data, size, buffer, num_args,
                      interpreter_args, compiler_args)) {
    return 0;
  }

  v8::internal::wasm::testing::SetupIsolateForWasmModule(i_isolate);

  ErrorThrower interpreter_thrower(i_isolate, "Interpreter");
  ModuleWireBytes wire_bytes(buffer.begin(), buffer.end());

  MaybeHandle<WasmModuleObject> compiled_module =
      SyncCompile(i_isolate, &interpreter_thrower, wire_bytes);
  // Clear the flag so that the WebAssembly code is not printed twice.
  FLAG_wasm_code_fuzzer_gen_test = false;
  bool compiles = !compiled_module.is_null();

  if (generate_test) {
    OFStream os(stdout);
    os << "            ])" << std::endl
       << "            .exportFunc();" << std::endl;
    if (compiles) {
      os << "  var module = builder.instantiate();" << std::endl
         << "  module.exports.test(1, 2, 3);" << std::endl;
    } else {
      OFStream os(stdout);
      os << "  assertThrows(function() { builder.instantiate(); });"
         << std::endl;
    }
    os << "})();" << std::endl;
  }

  bool validates = wasm::SyncValidate(i_isolate, wire_bytes);

  if (compiles != validates) {
    uint32_t hash = StringHasher::HashSequentialString(
        data, static_cast<int>(size), WASM_CODE_FUZZER_HASH_SEED);
    V8_Fatal(__FILE__, __LINE__,
             "compiles != validates (%d vs %d); WasmCodeFuzzerHash=%x",
             compiles, validates, hash);
  }

  if (!compiles) return 0;

  int32_t result_interpreted;
  bool possible_nondeterminism = false;
  {
    MaybeHandle<WasmInstanceObject> interpreter_instance = SyncInstantiate(
        i_isolate, &interpreter_thrower, compiled_module.ToHandleChecked(),
        MaybeHandle<JSReceiver>(), MaybeHandle<JSArrayBuffer>());

    if (interpreter_thrower.error()) {
      return 0;
    }
    result_interpreted = testing::InterpretWasmModule(
        i_isolate, interpreter_instance.ToHandleChecked(), &interpreter_thrower,
        0, interpreter_args.get(), &possible_nondeterminism);
  }

  // Do not execute the generated code if the interpreter did not finished after
  // a bounded number of steps.
  if (interpreter_thrower.error()) {
    return 0;
  }

  int32_t result_compiled;
  {
    ErrorThrower compiler_thrower(i_isolate, "Compiler");
    MaybeHandle<WasmInstanceObject> compiled_instance = SyncInstantiate(
        i_isolate, &compiler_thrower, compiled_module.ToHandleChecked(),
        MaybeHandle<JSReceiver>(), MaybeHandle<JSArrayBuffer>());

    DCHECK(!compiler_thrower.error());
    result_compiled = testing::CallWasmFunctionForTesting(
        i_isolate, compiled_instance.ToHandleChecked(), &compiler_thrower,
        "main", num_args, compiler_args.get());
  }

  // The WebAssembly spec allows the sign bit of NaN to be non-deterministic.
  // This sign bit may cause result_interpreted to be different than
  // result_compiled. Therefore we do not check the equality of the results
  // if the execution may have produced a NaN at some point.
  if (possible_nondeterminism) return 0;

  if (result_interpreted == bit_cast<int32_t>(0xdeadbeef)) {
    CHECK(i_isolate->has_pending_exception());
    i_isolate->clear_pending_exception();
  } else {
    CHECK(!i_isolate->has_pending_exception());
    if (result_interpreted != result_compiled) {
      V8_Fatal(__FILE__, __LINE__, "WasmCodeFuzzerHash=%x",
               StringHasher::HashSequentialString(data, static_cast<int>(size),
                                                  WASM_CODE_FUZZER_HASH_SEED));
    }
  }
  return 0;
}
