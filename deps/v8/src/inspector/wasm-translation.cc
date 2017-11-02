// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/inspector/wasm-translation.h"

#include <algorithm>

#include "src/debug/debug-interface.h"
#include "src/inspector/protocol/Debugger.h"
#include "src/inspector/script-breakpoint.h"
#include "src/inspector/string-util.h"
#include "src/inspector/v8-debugger-agent-impl.h"
#include "src/inspector/v8-debugger-script.h"
#include "src/inspector/v8-debugger.h"
#include "src/inspector/v8-inspector-impl.h"

#if __clang__
// TODO(mostynb@opera.com): remove the using statements and these pragmas.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wheader-hygiene"
#endif

using namespace v8_inspector;
using namespace v8;

#if __clang__
// TODO(mostynb@opera.com): remove the using statements and these pragmas.
#pragma clang diagnostic pop
#endif

class WasmTranslation::TranslatorImpl {
 public:
  struct TransLocation {
    WasmTranslation* translation;
    String16 script_id;
    int line;
    int column;
    TransLocation(WasmTranslation* translation, String16 script_id, int line,
                  int column)
        : translation(translation),
          script_id(script_id),
          line(line),
          column(column) {}
  };

  virtual void Init(Isolate*, WasmTranslation*, V8DebuggerAgentImpl*) = 0;
  virtual void Translate(TransLocation*) = 0;
  virtual void TranslateBack(TransLocation*) = 0;
  virtual ~TranslatorImpl() {}

  class RawTranslator;
  class DisassemblingTranslator;
};

class WasmTranslation::TranslatorImpl::RawTranslator
    : public WasmTranslation::TranslatorImpl {
 public:
  void Init(Isolate*, WasmTranslation*, V8DebuggerAgentImpl*) {}
  void Translate(TransLocation*) {}
  void TranslateBack(TransLocation*) {}
};

class WasmTranslation::TranslatorImpl::DisassemblingTranslator
    : public WasmTranslation::TranslatorImpl {
  using OffsetTable = debug::WasmDisassembly::OffsetTable;

 public:
  DisassemblingTranslator(Isolate* isolate, Local<debug::WasmScript> script)
      : script_(isolate, script) {}

  void Init(Isolate* isolate, WasmTranslation* translation,
            V8DebuggerAgentImpl* agent) override {
    // Register fake scripts for each function in this wasm module/script.
    Local<debug::WasmScript> script = script_.Get(isolate);
    int num_functions = script->NumFunctions();
    int num_imported_functions = script->NumImportedFunctions();
    DCHECK_LE(0, num_imported_functions);
    DCHECK_LE(0, num_functions);
    DCHECK_GE(num_functions, num_imported_functions);
    String16 script_id = String16::fromInteger(script->Id());
    for (int func_idx = num_imported_functions; func_idx < num_functions;
         ++func_idx) {
      AddFakeScript(isolate, script_id, func_idx, translation, agent);
    }
  }

  void Translate(TransLocation* loc) override {
    const OffsetTable& offset_table = GetOffsetTable(loc);
    DCHECK(!offset_table.empty());
    uint32_t byte_offset = static_cast<uint32_t>(loc->column);

    // Binary search for the given offset.
    unsigned left = 0;                                            // inclusive
    unsigned right = static_cast<unsigned>(offset_table.size());  // exclusive
    while (right - left > 1) {
      unsigned mid = (left + right) / 2;
      if (offset_table[mid].byte_offset <= byte_offset) {
        left = mid;
      } else {
        right = mid;
      }
    }

    loc->script_id = GetFakeScriptId(loc);
    if (offset_table[left].byte_offset == byte_offset) {
      loc->line = offset_table[left].line;
      loc->column = offset_table[left].column;
    } else {
      loc->line = 0;
      loc->column = 0;
    }
  }

  void TranslateBack(TransLocation* loc) override {
    int func_index = GetFunctionIndexFromFakeScriptId(loc->script_id);
    const OffsetTable* reverse_table = GetReverseTable(func_index);
    if (!reverse_table) return;
    DCHECK(!reverse_table->empty());
    v8::Isolate* isolate = loc->translation->isolate_;

    // Binary search for the given line and column.
    unsigned left = 0;                                              // inclusive
    unsigned right = static_cast<unsigned>(reverse_table->size());  // exclusive
    while (right - left > 1) {
      unsigned mid = (left + right) / 2;
      auto& entry = (*reverse_table)[mid];
      if (entry.line < loc->line ||
          (entry.line == loc->line && entry.column <= loc->column)) {
        left = mid;
      } else {
        right = mid;
      }
    }

    int found_byte_offset = 0;
    // [left] is <= <line,column>, or left==0 and [0] > <line,column>.
    // We are searching for the smallest entry >= <line,column> which is still
    // on the same line. This must be either [left] or [left + 1].
    // If we don't find such an entry, we might have hit the special case of
    // pointing after the last line, which is translated to the end of the
    // function (one byte after the last function byte).
    if ((*reverse_table)[left].line == loc->line &&
        (*reverse_table)[left].column >= loc->column) {
      found_byte_offset = (*reverse_table)[left].byte_offset;
    } else if (left + 1 < reverse_table->size() &&
               (*reverse_table)[left + 1].line == loc->line &&
               (*reverse_table)[left + 1].column >= loc->column) {
      found_byte_offset = (*reverse_table)[left + 1].byte_offset;
    } else if (left == reverse_table->size() - 1 &&
               (*reverse_table)[left].line == loc->line - 1 &&
               loc->column == 0) {
      std::pair<int, int> func_range =
          script_.Get(isolate)->GetFunctionRange(func_index);
      DCHECK_LE(func_range.first, func_range.second);
      found_byte_offset = func_range.second - func_range.first;
    }

    loc->script_id = String16::fromInteger(script_.Get(isolate)->Id());
    loc->line = func_index;
    loc->column = found_byte_offset;
  }

 private:
  String16 GetFakeScriptUrl(v8::Isolate* isolate, int func_index) {
    Local<debug::WasmScript> script = script_.Get(isolate);
    String16 script_name = toProtocolString(script->Name().ToLocalChecked());
    int numFunctions = script->NumFunctions();
    int numImported = script->NumImportedFunctions();
    String16Builder builder;
    builder.appendAll("wasm://wasm/", script_name, '/');
    if (numFunctions - numImported > 300) {
      size_t digits = String16::fromInteger(numFunctions - 1).length();
      String16 thisCategory = String16::fromInteger((func_index / 100) * 100);
      DCHECK_LE(thisCategory.length(), digits);
      for (size_t i = thisCategory.length(); i < digits; ++i)
        builder.append('0');
      builder.appendAll(thisCategory, '/');
    }
    builder.appendAll(script_name, '-');
    builder.appendNumber(func_index);
    return builder.toString();
  }

  String16 GetFakeScriptId(const String16 script_id, int func_index) {
    return String16::concat(script_id, '-', String16::fromInteger(func_index));
  }
  String16 GetFakeScriptId(const TransLocation* loc) {
    return GetFakeScriptId(loc->script_id, loc->line);
  }

  void AddFakeScript(v8::Isolate* isolate, const String16& underlyingScriptId,
                     int func_idx, WasmTranslation* translation,
                     V8DebuggerAgentImpl* agent) {
    String16 fake_script_id = GetFakeScriptId(underlyingScriptId, func_idx);
    String16 fake_script_url = GetFakeScriptUrl(isolate, func_idx);

    v8::Local<debug::WasmScript> script = script_.Get(isolate);
    // TODO(clemensh): Generate disassembly lazily when queried by the frontend.
    debug::WasmDisassembly disassembly = script->DisassembleFunction(func_idx);

    DCHECK_EQ(0, offset_tables_.count(func_idx));
    offset_tables_.insert(
        std::make_pair(func_idx, std::move(disassembly.offset_table)));
    String16 source(disassembly.disassembly.data(),
                    disassembly.disassembly.length());
    std::unique_ptr<V8DebuggerScript> fake_script =
        V8DebuggerScript::CreateWasm(isolate, translation, script,
                                     fake_script_id, std::move(fake_script_url),
                                     source);

    translation->AddFakeScript(fake_script->scriptId(), this);
    agent->didParseSource(std::move(fake_script), true);
  }

  int GetFunctionIndexFromFakeScriptId(const String16& fake_script_id) {
    size_t last_dash_pos = fake_script_id.reverseFind('-');
    DCHECK_GT(fake_script_id.length(), last_dash_pos);
    bool ok = true;
    int func_index = fake_script_id.substring(last_dash_pos + 1).toInteger(&ok);
    DCHECK(ok);
    return func_index;
  }

  const OffsetTable& GetOffsetTable(const TransLocation* loc) {
    int func_index = loc->line;
    auto it = offset_tables_.find(func_index);
    // TODO(clemensh): Once we load disassembly lazily, the offset table
    // might not be there yet. Load it lazily then.
    DCHECK(it != offset_tables_.end());
    return it->second;
  }

  const OffsetTable* GetReverseTable(int func_index) {
    auto it = reverse_tables_.find(func_index);
    if (it != reverse_tables_.end()) return &it->second;

    // Find offset table, copy and sort it to get reverse table.
    it = offset_tables_.find(func_index);
    if (it == offset_tables_.end()) return nullptr;

    OffsetTable reverse_table = it->second;
    // Order by line, column, then byte offset.
    auto cmp = [](OffsetTable::value_type el1, OffsetTable::value_type el2) {
      if (el1.line != el2.line) return el1.line < el2.line;
      if (el1.column != el2.column) return el1.column < el2.column;
      return el1.byte_offset < el2.byte_offset;
    };
    std::sort(reverse_table.begin(), reverse_table.end(), cmp);

    auto inserted = reverse_tables_.insert(
        std::make_pair(func_index, std::move(reverse_table)));
    DCHECK(inserted.second);
    return &inserted.first->second;
  }

  Global<debug::WasmScript> script_;

  // We assume to only disassemble a subset of the functions, so store them in a
  // map instead of an array.
  std::unordered_map<int, const OffsetTable> offset_tables_;
  std::unordered_map<int, const OffsetTable> reverse_tables_;
};

WasmTranslation::WasmTranslation(v8::Isolate* isolate)
    : isolate_(isolate), mode_(Disassemble) {}

WasmTranslation::~WasmTranslation() { Clear(); }

void WasmTranslation::AddScript(Local<debug::WasmScript> script,
                                V8DebuggerAgentImpl* agent) {
  std::unique_ptr<TranslatorImpl> impl;
  switch (mode_) {
    case Raw:
      impl.reset(new TranslatorImpl::RawTranslator());
      break;
    case Disassemble:
      impl.reset(new TranslatorImpl::DisassemblingTranslator(isolate_, script));
      break;
  }
  DCHECK(impl);
  auto inserted =
      wasm_translators_.insert(std::make_pair(script->Id(), std::move(impl)));
  // Check that no mapping for this script id existed before.
  DCHECK(inserted.second);
  // impl has been moved, use the returned iterator to call Init.
  inserted.first->second->Init(isolate_, this, agent);
}

void WasmTranslation::Clear() {
  wasm_translators_.clear();
  fake_scripts_.clear();
}

// Translation "forward" (to artificial scripts).
bool WasmTranslation::TranslateWasmScriptLocationToProtocolLocation(
    String16* script_id, int* line_number, int* column_number) {
  DCHECK(script_id && line_number && column_number);
  bool ok = true;
  int script_id_int = script_id->toInteger(&ok);
  if (!ok) return false;

  auto it = wasm_translators_.find(script_id_int);
  if (it == wasm_translators_.end()) return false;
  TranslatorImpl* translator = it->second.get();

  TranslatorImpl::TransLocation trans_loc(this, std::move(*script_id),
                                          *line_number, *column_number);
  translator->Translate(&trans_loc);

  *script_id = std::move(trans_loc.script_id);
  *line_number = trans_loc.line;
  *column_number = trans_loc.column;

  return true;
}

// Translation "backward" (from artificial to real scripts).
bool WasmTranslation::TranslateProtocolLocationToWasmScriptLocation(
    String16* script_id, int* line_number, int* column_number) {
  auto it = fake_scripts_.find(*script_id);
  if (it == fake_scripts_.end()) return false;
  TranslatorImpl* translator = it->second;

  TranslatorImpl::TransLocation trans_loc(this, std::move(*script_id),
                                          *line_number, *column_number);
  translator->TranslateBack(&trans_loc);

  *script_id = std::move(trans_loc.script_id);
  *line_number = trans_loc.line;
  *column_number = trans_loc.column;

  return true;
}

void WasmTranslation::AddFakeScript(const String16& scriptId,
                                    TranslatorImpl* translator) {
  DCHECK_EQ(0, fake_scripts_.count(scriptId));
  fake_scripts_.insert(std::make_pair(scriptId, translator));
}
