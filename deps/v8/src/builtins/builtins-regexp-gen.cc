// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/builtins/builtins-regexp-gen.h"

#include "src/builtins/builtins-constructor-gen.h"
#include "src/builtins/builtins-utils-gen.h"
#include "src/builtins/builtins.h"
#include "src/code-factory.h"
#include "src/code-stub-assembler.h"
#include "src/objects/regexp-match-info.h"
#include "src/regexp/regexp-macro-assembler.h"

namespace v8 {
namespace internal {

using compiler::Node;

// -----------------------------------------------------------------------------
// ES6 section 21.2 RegExp Objects

Node* RegExpBuiltinsAssembler::AllocateRegExpResult(Node* context, Node* length,
                                                    Node* index, Node* input) {
  CSA_ASSERT(this, IsFixedArray(context));
  CSA_ASSERT(this, TaggedIsSmi(index));
  CSA_ASSERT(this, TaggedIsSmi(length));
  CSA_ASSERT(this, IsString(input));

#ifdef DEBUG
  Node* const max_length = SmiConstant(JSArray::kInitialMaxFastElementArray);
  CSA_ASSERT(this, SmiLessThanOrEqual(length, max_length));
#endif  // DEBUG

  // Allocate the JSRegExpResult together with its elements fixed array.
  // Initial preparations first.

  Node* const length_intptr = SmiUntag(length);
  const ElementsKind elements_kind = PACKED_ELEMENTS;

  Node* const elements_size = GetFixedArrayAllocationSize(
      length_intptr, elements_kind, INTPTR_PARAMETERS);
  Node* const total_size =
      IntPtrAdd(elements_size, IntPtrConstant(JSRegExpResult::kSize));

  static const int kRegExpResultOffset = 0;
  static const int kElementsOffset =
      kRegExpResultOffset + JSRegExpResult::kSize;

  // The folded allocation.

  Node* const result = Allocate(total_size);
  Node* const elements = InnerAllocate(result, kElementsOffset);

  // Initialize the JSRegExpResult.

  Node* const native_context = LoadNativeContext(context);
  Node* const map =
      LoadContextElement(native_context, Context::REGEXP_RESULT_MAP_INDEX);
  StoreMapNoWriteBarrier(result, map);

  Node* const empty_array = EmptyFixedArrayConstant();
  DCHECK(Heap::RootIsImmortalImmovable(Heap::kEmptyFixedArrayRootIndex));
  StoreObjectFieldNoWriteBarrier(result, JSArray::kPropertiesOrHashOffset,
                                 empty_array);
  StoreObjectFieldNoWriteBarrier(result, JSArray::kElementsOffset, elements);
  StoreObjectFieldNoWriteBarrier(result, JSArray::kLengthOffset, length);

  StoreObjectFieldNoWriteBarrier(result, JSRegExpResult::kIndexOffset, index);
  StoreObjectField(result, JSRegExpResult::kInputOffset, input);

  // Initialize the elements.

  DCHECK(!IsDoubleElementsKind(elements_kind));
  const Heap::RootListIndex map_index = Heap::kFixedArrayMapRootIndex;
  DCHECK(Heap::RootIsImmortalImmovable(map_index));
  StoreMapNoWriteBarrier(elements, map_index);
  StoreObjectFieldNoWriteBarrier(elements, FixedArray::kLengthOffset, length);

  Node* const zero = IntPtrConstant(0);
  FillFixedArrayWithValue(elements_kind, elements, zero, length_intptr,
                          Heap::kUndefinedValueRootIndex);

  return result;
}

Node* RegExpBuiltinsAssembler::FastLoadLastIndex(Node* regexp) {
  // Load the in-object field.
  static const int field_offset =
      JSRegExp::kSize + JSRegExp::kLastIndexFieldIndex * kPointerSize;
  return LoadObjectField(regexp, field_offset);
}

Node* RegExpBuiltinsAssembler::SlowLoadLastIndex(Node* context, Node* regexp) {
  // Load through the GetProperty stub.
  return GetProperty(context, regexp, isolate()->factory()->lastIndex_string());
}

Node* RegExpBuiltinsAssembler::LoadLastIndex(Node* context, Node* regexp,
                                             bool is_fastpath) {
  return is_fastpath ? FastLoadLastIndex(regexp)
                     : SlowLoadLastIndex(context, regexp);
}

// The fast-path of StoreLastIndex when regexp is guaranteed to be an unmodified
// JSRegExp instance.
void RegExpBuiltinsAssembler::FastStoreLastIndex(Node* regexp, Node* value) {
  // Store the in-object field.
  static const int field_offset =
      JSRegExp::kSize + JSRegExp::kLastIndexFieldIndex * kPointerSize;
  StoreObjectField(regexp, field_offset, value);
}

void RegExpBuiltinsAssembler::SlowStoreLastIndex(Node* context, Node* regexp,
                                                 Node* value) {
  // Store through runtime.
  // TODO(ishell): Use SetPropertyStub here once available.
  Node* const name = HeapConstant(isolate()->factory()->lastIndex_string());
  Node* const language_mode = SmiConstant(STRICT);
  CallRuntime(Runtime::kSetProperty, context, regexp, name, value,
              language_mode);
}

void RegExpBuiltinsAssembler::StoreLastIndex(Node* context, Node* regexp,
                                             Node* value, bool is_fastpath) {
  if (is_fastpath) {
    FastStoreLastIndex(regexp, value);
  } else {
    SlowStoreLastIndex(context, regexp, value);
  }
}

Node* RegExpBuiltinsAssembler::ConstructNewResultFromMatchInfo(
    Node* const context, Node* const regexp, Node* const match_info,
    Node* const string) {
  CSA_ASSERT(this, IsFixedArrayMap(LoadMap(match_info)));
  CSA_ASSERT(this, IsJSRegExp(regexp));
  CSA_ASSERT(this, IsString(string));

  Label named_captures(this), out(this);

  TNode<IntPtrT> num_indices = SmiUntag(LoadFixedArrayElement(
      match_info, RegExpMatchInfo::kNumberOfCapturesIndex));
  Node* const num_results = SmiTag(WordShr(num_indices, 1));
  Node* const start =
      LoadFixedArrayElement(match_info, RegExpMatchInfo::kFirstCaptureIndex);
  Node* const end = LoadFixedArrayElement(
      match_info, RegExpMatchInfo::kFirstCaptureIndex + 1);

  // Calculate the substring of the first match before creating the result array
  // to avoid an unnecessary write barrier storing the first result.
  Node* const first = SubString(context, string, start, end);

  Node* const result =
      AllocateRegExpResult(context, num_results, start, string);
  Node* const result_elements = LoadElements(result);

  StoreFixedArrayElement(result_elements, 0, first, SKIP_WRITE_BARRIER);

  // If no captures exist we can skip named capture handling as well.
  GotoIf(SmiEqual(num_results, SmiConstant(1)), &out);

  // Store all remaining captures.
  Node* const limit = IntPtrAdd(
      IntPtrConstant(RegExpMatchInfo::kFirstCaptureIndex), num_indices);

  VARIABLE(var_from_cursor, MachineType::PointerRepresentation(),
           IntPtrConstant(RegExpMatchInfo::kFirstCaptureIndex + 2));
  VARIABLE(var_to_cursor, MachineType::PointerRepresentation(),
           IntPtrConstant(1));

  Variable* vars[] = {&var_from_cursor, &var_to_cursor};
  Label loop(this, 2, vars);

  Goto(&loop);
  BIND(&loop);
  {
    Node* const from_cursor = var_from_cursor.value();
    Node* const to_cursor = var_to_cursor.value();
    Node* const start = LoadFixedArrayElement(match_info, from_cursor);

    Label next_iter(this);
    GotoIf(SmiEqual(start, SmiConstant(-1)), &next_iter);

    Node* const from_cursor_plus1 = IntPtrAdd(from_cursor, IntPtrConstant(1));
    Node* const end = LoadFixedArrayElement(match_info, from_cursor_plus1);

    Node* const capture = SubString(context, string, start, end);
    StoreFixedArrayElement(result_elements, to_cursor, capture);
    Goto(&next_iter);

    BIND(&next_iter);
    var_from_cursor.Bind(IntPtrAdd(from_cursor, IntPtrConstant(2)));
    var_to_cursor.Bind(IntPtrAdd(to_cursor, IntPtrConstant(1)));
    Branch(UintPtrLessThan(var_from_cursor.value(), limit), &loop,
           &named_captures);
  }

  BIND(&named_captures);
  {
    // We reach this point only if captures exist, implying that this is an
    // IRREGEXP JSRegExp.

    CSA_ASSERT(this, IsJSRegExp(regexp));
    CSA_ASSERT(this, SmiGreaterThan(num_results, SmiConstant(1)));

    // Preparations for named capture properties. Exit early if the result does
    // not have any named captures to minimize performance impact.

    Node* const data = LoadObjectField(regexp, JSRegExp::kDataOffset);
    CSA_ASSERT(this, SmiEqual(LoadFixedArrayElement(data, JSRegExp::kTagIndex),
                              SmiConstant(JSRegExp::IRREGEXP)));

    // The names fixed array associates names at even indices with a capture
    // index at odd indices.
    Node* const names =
        LoadFixedArrayElement(data, JSRegExp::kIrregexpCaptureNameMapIndex);
    GotoIf(SmiEqual(names, SmiConstant(0)), &out);

    // Allocate a new object to store the named capture properties.
    // TODO(jgruber): Could be optimized by adding the object map to the heap
    // root list.
    // TODO(jgruber): Replace CreateDataProperty runtime calls once we have
    // equivalent functionality in CSA.

    Node* const native_context = LoadNativeContext(context);
    Node* const map = LoadContextElement(
        native_context, Context::SLOW_OBJECT_WITH_NULL_PROTOTYPE_MAP);
    Node* const properties =
        AllocateNameDictionary(NameDictionary::kInitialCapacity);

    Node* const group_object = AllocateJSObjectFromMap(map, properties);

    // Store it on the result as a 'group' property.

    {
      Node* const name = HeapConstant(isolate()->factory()->groups_string());
      CallRuntime(Runtime::kCreateDataProperty, context, result, name,
                  group_object);
    }

    // One or more named captures exist, add a property for each one.

    CSA_ASSERT(this, HasInstanceType(names, FIXED_ARRAY_TYPE));
    Node* const names_length = LoadAndUntagFixedArrayBaseLength(names);
    CSA_ASSERT(this, IntPtrGreaterThan(names_length, IntPtrConstant(0)));

    VARIABLE(var_i, MachineType::PointerRepresentation());
    var_i.Bind(IntPtrConstant(0));

    Variable* vars[] = {&var_i};
    const int vars_count = sizeof(vars) / sizeof(vars[0]);
    Label loop(this, vars_count, vars);

    Goto(&loop);
    BIND(&loop);
    {
      Node* const i = var_i.value();
      Node* const i_plus_1 = IntPtrAdd(i, IntPtrConstant(1));
      Node* const i_plus_2 = IntPtrAdd(i_plus_1, IntPtrConstant(1));

      Node* const name = LoadFixedArrayElement(names, i);
      Node* const index = LoadFixedArrayElement(names, i_plus_1);
      Node* const capture =
          LoadFixedArrayElement(result_elements, SmiUntag(index));

      CallRuntime(Runtime::kCreateDataProperty, context, group_object, name,
                  capture);

      var_i.Bind(i_plus_2);
      Branch(IntPtrGreaterThanOrEqual(var_i.value(), names_length), &out,
             &loop);
    }
  }

  BIND(&out);
  return result;
}

void RegExpBuiltinsAssembler::GetStringPointers(
    Node* const string_data, Node* const offset, Node* const last_index,
    Node* const string_length, String::Encoding encoding,
    Variable* var_string_start, Variable* var_string_end) {
  DCHECK_EQ(var_string_start->rep(), MachineType::PointerRepresentation());
  DCHECK_EQ(var_string_end->rep(), MachineType::PointerRepresentation());

  const ElementsKind kind = (encoding == String::ONE_BYTE_ENCODING)
                                ? UINT8_ELEMENTS
                                : UINT16_ELEMENTS;

  Node* const from_offset = ElementOffsetFromIndex(
      IntPtrAdd(offset, last_index), kind, INTPTR_PARAMETERS);
  var_string_start->Bind(IntPtrAdd(string_data, from_offset));

  Node* const to_offset = ElementOffsetFromIndex(
      IntPtrAdd(offset, string_length), kind, INTPTR_PARAMETERS);
  var_string_end->Bind(IntPtrAdd(string_data, to_offset));
}

Node* RegExpBuiltinsAssembler::RegExpExecInternal(Node* const context,
                                                  Node* const regexp,
                                                  Node* const string,
                                                  Node* const last_index,
                                                  Node* const match_info) {
// Just jump directly to runtime if native RegExp is not selected at compile
// time or if regexp entry in generated code is turned off runtime switch or
// at compilation.
#ifdef V8_INTERPRETED_REGEXP
  return CallRuntime(Runtime::kRegExpExec, context, regexp, string, last_index,
                     match_info);
#else   // V8_INTERPRETED_REGEXP
  CSA_ASSERT(this, TaggedIsNotSmi(regexp));
  CSA_ASSERT(this, IsJSRegExp(regexp));

  CSA_ASSERT(this, TaggedIsNotSmi(string));
  CSA_ASSERT(this, IsString(string));

  CSA_ASSERT(this, IsNumber(last_index));
  CSA_ASSERT(this, IsFixedArrayMap(LoadReceiverMap(match_info)));

  Node* const int_zero = IntPtrConstant(0);

  ToDirectStringAssembler to_direct(state(), string);

  VARIABLE(var_result, MachineRepresentation::kTagged);
  Label out(this), atom(this), runtime(this, Label::kDeferred);

  // External constants.
  Node* const isolate_address =
      ExternalConstant(ExternalReference::isolate_address(isolate()));
  Node* const regexp_stack_memory_address_address = ExternalConstant(
      ExternalReference::address_of_regexp_stack_memory_address(isolate()));
  Node* const regexp_stack_memory_size_address = ExternalConstant(
      ExternalReference::address_of_regexp_stack_memory_size(isolate()));
  Node* const static_offsets_vector_address = ExternalConstant(
      ExternalReference::address_of_static_offsets_vector(isolate()));

  // At this point, last_index is definitely a canonicalized non-negative
  // number, which implies that any non-Smi last_index is greater than
  // the maximal string length. If lastIndex > string.length then the matcher
  // must fail.

  Label if_failure(this);
  Node* const smi_string_length = LoadStringLength(string);
  {
    CSA_ASSERT(this, IsNumberNormalized(last_index));
    CSA_ASSERT(this, IsNumberPositive(last_index));
    Node* const last_index_is_not_smi = TaggedIsNotSmi(last_index);
    Node* const last_index_is_oob =
        SmiGreaterThan(last_index, smi_string_length);
    GotoIf(Word32Or(last_index_is_not_smi, last_index_is_oob), &if_failure);
  }

  Node* const data = LoadObjectField(regexp, JSRegExp::kDataOffset);
  {
    // Check that the RegExp has been compiled (data contains a fixed array).
    CSA_ASSERT(this, TaggedIsNotSmi(data));
    CSA_ASSERT(this, HasInstanceType(data, FIXED_ARRAY_TYPE));

    // Dispatch on the type of the RegExp.
    {
      Label next(this), unreachable(this, Label::kDeferred);
      Node* const tag = LoadAndUntagToWord32FixedArrayElement(
          data, IntPtrConstant(JSRegExp::kTagIndex));

      int32_t values[] = {
          JSRegExp::IRREGEXP, JSRegExp::ATOM, JSRegExp::NOT_COMPILED,
      };
      Label* labels[] = {&next, &atom, &runtime};

      STATIC_ASSERT(arraysize(values) == arraysize(labels));
      Switch(tag, &unreachable, values, labels, arraysize(values));

      BIND(&unreachable);
      Unreachable();

      BIND(&next);
    }

    // Check (number_of_captures + 1) * 2 <= offsets vector size
    // Or              number_of_captures <= offsets vector size / 2 - 1
    Node* const capture_count =
        LoadFixedArrayElement(data, JSRegExp::kIrregexpCaptureCountIndex);
    CSA_ASSERT(this, TaggedIsSmi(capture_count));

    STATIC_ASSERT(Isolate::kJSRegexpStaticOffsetsVectorSize >= 2);
    GotoIf(SmiAbove(
               capture_count,
               SmiConstant(Isolate::kJSRegexpStaticOffsetsVectorSize / 2 - 1)),
           &runtime);
  }

  // Ensure that a RegExp stack is allocated. This check is after branching off
  // for ATOM regexps to avoid unnecessary trips to runtime.
  {
    Node* const stack_size =
        Load(MachineType::IntPtr(), regexp_stack_memory_size_address);
    GotoIf(IntPtrEqual(stack_size, int_zero), &runtime);
  }

  // Unpack the string if possible.

  to_direct.TryToDirect(&runtime);

  // Load the irregexp code object and offsets into the subject string. Both
  // depend on whether the string is one- or two-byte.

  Node* const int_last_index = SmiUntag(last_index);

  VARIABLE(var_string_start, MachineType::PointerRepresentation());
  VARIABLE(var_string_end, MachineType::PointerRepresentation());
  VARIABLE(var_code, MachineRepresentation::kTagged);

  {
    Node* const int_string_length = SmiUntag(smi_string_length);
    Node* const direct_string_data = to_direct.PointerToData(&runtime);

    Label next(this), if_isonebyte(this), if_istwobyte(this, Label::kDeferred);
    Branch(IsOneByteStringInstanceType(to_direct.instance_type()),
           &if_isonebyte, &if_istwobyte);

    BIND(&if_isonebyte);
    {
      GetStringPointers(direct_string_data, to_direct.offset(), int_last_index,
                        int_string_length, String::ONE_BYTE_ENCODING,
                        &var_string_start, &var_string_end);
      var_code.Bind(
          LoadFixedArrayElement(data, JSRegExp::kIrregexpLatin1CodeIndex));
      Goto(&next);
    }

    BIND(&if_istwobyte);
    {
      GetStringPointers(direct_string_data, to_direct.offset(), int_last_index,
                        int_string_length, String::TWO_BYTE_ENCODING,
                        &var_string_start, &var_string_end);
      var_code.Bind(
          LoadFixedArrayElement(data, JSRegExp::kIrregexpUC16CodeIndex));
      Goto(&next);
    }

    BIND(&next);
  }

  // Check that the irregexp code has been generated for the actual string
  // encoding. If it has, the field contains a code object; and otherwise it
  // contains the uninitialized sentinel as a smi.

  Node* const code = var_code.value();
#ifdef DEBUG
  {
    Label next(this);
    GotoIfNot(TaggedIsSmi(code), &next);

    CSA_ASSERT(this,
               SmiEqual(code, SmiConstant(JSRegExp::kUninitializedValue)));
    Goto(&next);

    BIND(&next);
  }
#endif
  GotoIf(TaggedIsSmi(code), &runtime);
  CSA_ASSERT(this, HasInstanceType(code, CODE_TYPE));

  Label if_success(this), if_exception(this, Label::kDeferred);
  {
    IncrementCounter(isolate()->counters()->regexp_entry_native(), 1);

    // Set up args for the final call into generated Irregexp code.

    MachineType type_int32 = MachineType::Int32();
    MachineType type_tagged = MachineType::AnyTagged();
    MachineType type_ptr = MachineType::Pointer();

    // Result: A NativeRegExpMacroAssembler::Result return code.
    MachineType retval_type = type_int32;

    // Argument 0: Original subject string.
    MachineType arg0_type = type_tagged;
    Node* const arg0 = string;

    // Argument 1: Previous index.
    MachineType arg1_type = type_int32;
    Node* const arg1 = TruncateWordToWord32(int_last_index);

    // Argument 2: Start of string data.
    MachineType arg2_type = type_ptr;
    Node* const arg2 = var_string_start.value();

    // Argument 3: End of string data.
    MachineType arg3_type = type_ptr;
    Node* const arg3 = var_string_end.value();

    // Argument 4: static offsets vector buffer.
    MachineType arg4_type = type_ptr;
    Node* const arg4 = static_offsets_vector_address;

    // Argument 5: Set the number of capture registers to zero to force global
    // regexps to behave as non-global.  This does not affect non-global
    // regexps.
    MachineType arg5_type = type_int32;
    Node* const arg5 = Int32Constant(0);

    // Argument 6: Start (high end) of backtracking stack memory area.
    Node* const stack_start =
        Load(MachineType::Pointer(), regexp_stack_memory_address_address);
    Node* const stack_size =
        Load(MachineType::IntPtr(), regexp_stack_memory_size_address);
    Node* const stack_end = IntPtrAdd(stack_start, stack_size);

    MachineType arg6_type = type_ptr;
    Node* const arg6 = stack_end;

    // Argument 7: Indicate that this is a direct call from JavaScript.
    MachineType arg7_type = type_int32;
    Node* const arg7 = Int32Constant(1);

    // Argument 8: Pass current isolate address.
    MachineType arg8_type = type_ptr;
    Node* const arg8 = isolate_address;

    Node* const code_entry =
        IntPtrAdd(BitcastTaggedToWord(code),
                  IntPtrConstant(Code::kHeaderSize - kHeapObjectTag));

    Node* const result = CallCFunction9(
        retval_type, arg0_type, arg1_type, arg2_type, arg3_type, arg4_type,
        arg5_type, arg6_type, arg7_type, arg8_type, code_entry, arg0, arg1,
        arg2, arg3, arg4, arg5, arg6, arg7, arg8);

    // Check the result.
    // We expect exactly one result since we force the called regexp to behave
    // as non-global.
    Node* const int_result = ChangeInt32ToIntPtr(result);
    GotoIf(IntPtrEqual(int_result,
                       IntPtrConstant(NativeRegExpMacroAssembler::SUCCESS)),
           &if_success);
    GotoIf(IntPtrEqual(int_result,
                       IntPtrConstant(NativeRegExpMacroAssembler::FAILURE)),
           &if_failure);
    GotoIf(IntPtrEqual(int_result,
                       IntPtrConstant(NativeRegExpMacroAssembler::EXCEPTION)),
           &if_exception);

    CSA_ASSERT(this,
               IntPtrEqual(int_result,
                           IntPtrConstant(NativeRegExpMacroAssembler::RETRY)));
    Goto(&runtime);
  }

  BIND(&if_success);
  {
    // Check that the last match info has space for the capture registers and
    // the additional information. Ensure no overflow in add.
    STATIC_ASSERT(FixedArray::kMaxLength < kMaxInt - FixedArray::kLengthOffset);
    Node* const available_slots =
        SmiSub(LoadFixedArrayBaseLength(match_info),
               SmiConstant(RegExpMatchInfo::kLastMatchOverhead));
    Node* const capture_count =
        LoadFixedArrayElement(data, JSRegExp::kIrregexpCaptureCountIndex);
    // Calculate number of register_count = (capture_count + 1) * 2.
    Node* const register_count =
        SmiShl(SmiAdd(capture_count, SmiConstant(1)), 1);
    GotoIf(SmiGreaterThan(register_count, available_slots), &runtime);

    // Fill match_info.

    StoreFixedArrayElement(match_info, RegExpMatchInfo::kNumberOfCapturesIndex,
                           register_count, SKIP_WRITE_BARRIER);
    StoreFixedArrayElement(match_info, RegExpMatchInfo::kLastSubjectIndex,
                           string);
    StoreFixedArrayElement(match_info, RegExpMatchInfo::kLastInputIndex,
                           string);

    // Fill match and capture offsets in match_info.
    {
      Node* const limit_offset = ElementOffsetFromIndex(
          register_count, INT32_ELEMENTS, SMI_PARAMETERS, 0);

      Node* const to_offset = ElementOffsetFromIndex(
          IntPtrConstant(RegExpMatchInfo::kFirstCaptureIndex), PACKED_ELEMENTS,
          INTPTR_PARAMETERS, RegExpMatchInfo::kHeaderSize - kHeapObjectTag);
      VARIABLE(var_to_offset, MachineType::PointerRepresentation(), to_offset);

      VariableList vars({&var_to_offset}, zone());
      BuildFastLoop(
          vars, int_zero, limit_offset,
          [=, &var_to_offset](Node* offset) {
            Node* const value = Load(MachineType::Int32(),
                                     static_offsets_vector_address, offset);
            Node* const smi_value = SmiFromWord32(value);
            StoreNoWriteBarrier(MachineRepresentation::kTagged, match_info,
                                var_to_offset.value(), smi_value);
            Increment(&var_to_offset, kPointerSize);
          },
          kInt32Size, INTPTR_PARAMETERS, IndexAdvanceMode::kPost);
    }

    var_result.Bind(match_info);
    Goto(&out);
  }

  BIND(&if_failure);
  {
    var_result.Bind(NullConstant());
    Goto(&out);
  }

  BIND(&if_exception);
  {
// A stack overflow was detected in RegExp code.
#ifdef DEBUG
    Node* const pending_exception_address = ExternalConstant(ExternalReference(
        IsolateAddressId::kPendingExceptionAddress, isolate()));
    CSA_ASSERT(this, IsTheHole(Load(MachineType::AnyTagged(),
                                    pending_exception_address)));
#endif  // DEBUG
    CallRuntime(Runtime::kThrowStackOverflow, context);
    Unreachable();
  }

  BIND(&runtime);
  {
    Node* const result = CallRuntime(Runtime::kRegExpExec, context, regexp,
                                     string, last_index, match_info);
    var_result.Bind(result);
    Goto(&out);
  }

  BIND(&atom);
  {
    // TODO(jgruber): A call with 4 args stresses register allocation, this
    // should probably just be inlined.
    Node* const result = CallBuiltin(Builtins::kRegExpExecAtom, context, regexp,
                                     string, last_index, match_info);
    var_result.Bind(result);
    Goto(&out);
  }

  BIND(&out);
  return var_result.value();
#endif  // V8_INTERPRETED_REGEXP
}

// ES#sec-regexp.prototype.exec
// RegExp.prototype.exec ( string )
// Implements the core of RegExp.prototype.exec but without actually
// constructing the JSRegExpResult. Returns either null (if the RegExp did not
// match) or a fixed array containing match indices as returned by
// RegExpExecStub.
Node* RegExpBuiltinsAssembler::RegExpPrototypeExecBodyWithoutResult(
    Node* const context, Node* const regexp, Node* const string,
    Label* if_didnotmatch, const bool is_fastpath) {
  Node* const null = NullConstant();
  Node* const int_zero = IntPtrConstant(0);
  Node* const smi_zero = SmiConstant(0);

  if (is_fastpath) {
    CSA_ASSERT(this, IsFastRegExpNoPrototype(context, regexp));
  } else {
    ThrowIfNotInstanceType(context, regexp, JS_REGEXP_TYPE,
                           "RegExp.prototype.exec");
  }

  CSA_ASSERT(this, IsString(string));
  CSA_ASSERT(this, IsJSRegExp(regexp));

  VARIABLE(var_result, MachineRepresentation::kTagged);
  Label out(this);

  // Load lastIndex.
  VARIABLE(var_lastindex, MachineRepresentation::kTagged);
  {
    Node* const regexp_lastindex = LoadLastIndex(context, regexp, is_fastpath);
    var_lastindex.Bind(regexp_lastindex);

    if (is_fastpath) {
      // ToLength on a positive smi is a nop and can be skipped.
      CSA_ASSERT(this, TaggedIsPositiveSmi(regexp_lastindex));
    } else {
      // Omit ToLength if lastindex is a non-negative smi.
      Label call_tolength(this, Label::kDeferred), next(this);
      Branch(TaggedIsPositiveSmi(regexp_lastindex), &next, &call_tolength);

      BIND(&call_tolength);
      {
        var_lastindex.Bind(ToLength_Inline(context, regexp_lastindex));
        Goto(&next);
      }

      BIND(&next);
    }
  }

  // Check whether the regexp is global or sticky, which determines whether we
  // update last index later on.
  Node* const flags = LoadObjectField(regexp, JSRegExp::kFlagsOffset);
  Node* const is_global_or_sticky = WordAnd(
      SmiUntag(flags), IntPtrConstant(JSRegExp::kGlobal | JSRegExp::kSticky));
  Node* const should_update_last_index =
      WordNotEqual(is_global_or_sticky, int_zero);

  // Grab and possibly update last index.
  Label run_exec(this);
  {
    Label if_doupdate(this), if_dontupdate(this);
    Branch(should_update_last_index, &if_doupdate, &if_dontupdate);

    BIND(&if_doupdate);
    {
      Node* const lastindex = var_lastindex.value();

      Label if_isoob(this, Label::kDeferred);
      GotoIfNot(TaggedIsSmi(lastindex), &if_isoob);
      Node* const string_length = LoadStringLength(string);
      GotoIfNot(SmiLessThanOrEqual(lastindex, string_length), &if_isoob);
      Goto(&run_exec);

      BIND(&if_isoob);
      {
        StoreLastIndex(context, regexp, smi_zero, is_fastpath);
        var_result.Bind(null);
        Goto(if_didnotmatch);
      }
    }

    BIND(&if_dontupdate);
    {
      var_lastindex.Bind(smi_zero);
      Goto(&run_exec);
    }
  }

  Node* match_indices;
  Label successful_match(this);
  BIND(&run_exec);
  {
    // Get last match info from the context.
    Node* const native_context = LoadNativeContext(context);
    Node* const last_match_info = LoadContextElement(
        native_context, Context::REGEXP_LAST_MATCH_INFO_INDEX);

    // Call the exec stub.
    match_indices = RegExpExecInternal(context, regexp, string,
                                       var_lastindex.value(), last_match_info);
    var_result.Bind(match_indices);

    // {match_indices} is either null or the RegExpMatchInfo array.
    // Return early if exec failed, possibly updating last index.
    GotoIfNot(WordEqual(match_indices, null), &successful_match);

    GotoIfNot(should_update_last_index, if_didnotmatch);

    StoreLastIndex(context, regexp, smi_zero, is_fastpath);
    Goto(if_didnotmatch);
  }

  BIND(&successful_match);
  {
    GotoIfNot(should_update_last_index, &out);

    // Update the new last index from {match_indices}.
    Node* const new_lastindex = LoadFixedArrayElement(
        match_indices, RegExpMatchInfo::kFirstCaptureIndex + 1);

    StoreLastIndex(context, regexp, new_lastindex, is_fastpath);
    Goto(&out);
  }

  BIND(&out);
  return var_result.value();
}

// ES#sec-regexp.prototype.exec
// RegExp.prototype.exec ( string )
Node* RegExpBuiltinsAssembler::RegExpPrototypeExecBody(Node* const context,
                                                       Node* const regexp,
                                                       Node* const string,
                                                       const bool is_fastpath) {
  Node* const null = NullConstant();

  VARIABLE(var_result, MachineRepresentation::kTagged);

  Label if_didnotmatch(this), out(this);
  Node* const indices_or_null = RegExpPrototypeExecBodyWithoutResult(
      context, regexp, string, &if_didnotmatch, is_fastpath);

  // Successful match.
  {
    Node* const match_indices = indices_or_null;
    Node* const result =
        ConstructNewResultFromMatchInfo(context, regexp, match_indices, string);
    var_result.Bind(result);
    Goto(&out);
  }

  BIND(&if_didnotmatch);
  {
    var_result.Bind(null);
    Goto(&out);
  }

  BIND(&out);
  return var_result.value();
}

Node* RegExpBuiltinsAssembler::ThrowIfNotJSReceiver(
    Node* context, Node* maybe_receiver, MessageTemplate::Template msg_template,
    char const* method_name) {
  Label out(this), throw_exception(this, Label::kDeferred);
  VARIABLE(var_value_map, MachineRepresentation::kTagged);

  GotoIf(TaggedIsSmi(maybe_receiver), &throw_exception);

  // Load the instance type of the {value}.
  var_value_map.Bind(LoadMap(maybe_receiver));
  Node* const value_instance_type = LoadMapInstanceType(var_value_map.value());

  Branch(IsJSReceiverInstanceType(value_instance_type), &out, &throw_exception);

  // The {value} is not a compatible receiver for this method.
  BIND(&throw_exception);
  {
    Node* const value_str =
        CallBuiltin(Builtins::kToString, context, maybe_receiver);
    ThrowTypeError(context, msg_template, StringConstant(method_name),
                   value_str);
  }

  BIND(&out);
  return var_value_map.value();
}

Node* RegExpBuiltinsAssembler::IsFastRegExpNoPrototype(Node* const context,
                                                       Node* const object,
                                                       Node* const map) {
  Label out(this);
  VARIABLE(var_result, MachineRepresentation::kWord32);

  Node* const native_context = LoadNativeContext(context);
  Node* const regexp_fun =
      LoadContextElement(native_context, Context::REGEXP_FUNCTION_INDEX);
  Node* const initial_map =
      LoadObjectField(regexp_fun, JSFunction::kPrototypeOrInitialMapOffset);
  Node* const has_initialmap = WordEqual(map, initial_map);

  var_result.Bind(has_initialmap);
  GotoIfNot(has_initialmap, &out);

  // The smi check is required to omit ToLength(lastIndex) calls with possible
  // user-code execution on the fast path.
  Node* const last_index = FastLoadLastIndex(object);
  var_result.Bind(TaggedIsPositiveSmi(last_index));
  Goto(&out);

  BIND(&out);
  return var_result.value();
}

Node* RegExpBuiltinsAssembler::IsFastRegExpNoPrototype(Node* const context,
                                                       Node* const object) {
  CSA_ASSERT(this, TaggedIsNotSmi(object));
  return IsFastRegExpNoPrototype(context, object, LoadMap(object));
}

// RegExp fast path implementations rely on unmodified JSRegExp instances.
// We use a fairly coarse granularity for this and simply check whether both
// the regexp itself is unmodified (i.e. its map has not changed), its
// prototype is unmodified, and lastIndex is a non-negative smi.
void RegExpBuiltinsAssembler::BranchIfFastRegExp(Node* const context,
                                                 Node* const object,
                                                 Node* const map,
                                                 Label* const if_isunmodified,
                                                 Label* const if_ismodified) {
  CSA_ASSERT(this, WordEqual(LoadMap(object), map));

  // TODO(ishell): Update this check once map changes for constant field
  // tracking are landing.

  Node* const native_context = LoadNativeContext(context);
  Node* const regexp_fun =
      LoadContextElement(native_context, Context::REGEXP_FUNCTION_INDEX);
  Node* const initial_map =
      LoadObjectField(regexp_fun, JSFunction::kPrototypeOrInitialMapOffset);
  Node* const has_initialmap = WordEqual(map, initial_map);

  GotoIfNot(has_initialmap, if_ismodified);

  Node* const initial_proto_initial_map =
      LoadContextElement(native_context, Context::REGEXP_PROTOTYPE_MAP_INDEX);
  Node* const proto_map = LoadMap(CAST(LoadMapPrototype(map)));
  Node* const proto_has_initialmap =
      WordEqual(proto_map, initial_proto_initial_map);

  GotoIfNot(proto_has_initialmap, if_ismodified);

  // The smi check is required to omit ToLength(lastIndex) calls with possible
  // user-code execution on the fast path.
  Node* const last_index = FastLoadLastIndex(object);
  Branch(TaggedIsPositiveSmi(last_index), if_isunmodified, if_ismodified);
}

void RegExpBuiltinsAssembler::BranchIfFastRegExp(Node* const context,
                                                 Node* const object,
                                                 Label* const if_isunmodified,
                                                 Label* const if_ismodified) {
  CSA_ASSERT(this, TaggedIsNotSmi(object));
  BranchIfFastRegExp(context, object, LoadMap(object), if_isunmodified,
                     if_ismodified);
}

Node* RegExpBuiltinsAssembler::IsFastRegExp(Node* const context,
                                            Node* const object) {
  Label yup(this), nope(this), out(this);
  VARIABLE(var_result, MachineRepresentation::kWord32);

  BranchIfFastRegExp(context, object, &yup, &nope);

  BIND(&yup);
  var_result.Bind(Int32Constant(1));
  Goto(&out);

  BIND(&nope);
  var_result.Bind(Int32Constant(0));
  Goto(&out);

  BIND(&out);
  return var_result.value();
}

void RegExpBuiltinsAssembler::BranchIfFastRegExpResult(Node* const context,
                                                       Node* const object,
                                                       Label* if_isunmodified,
                                                       Label* if_ismodified) {
  // Could be a Smi.
  Node* const map = LoadReceiverMap(object);

  Node* const native_context = LoadNativeContext(context);
  Node* const initial_regexp_result_map =
      LoadContextElement(native_context, Context::REGEXP_RESULT_MAP_INDEX);

  Branch(WordEqual(map, initial_regexp_result_map), if_isunmodified,
         if_ismodified);
}

// Slow path stub for RegExpPrototypeExec to decrease code size.
TF_BUILTIN(RegExpPrototypeExecSlow, RegExpBuiltinsAssembler) {
  Node* const regexp = Parameter(Descriptor::kReceiver);
  Node* const string = Parameter(Descriptor::kString);
  Node* const context = Parameter(Descriptor::kContext);

  Return(RegExpPrototypeExecBody(context, regexp, string, false));
}

// Fast path stub for ATOM regexps. String matching is done by StringIndexOf,
// and {match_info} is updated on success.
// The slow path is implemented in RegExpImpl::AtomExec.
TF_BUILTIN(RegExpExecAtom, RegExpBuiltinsAssembler) {
  Node* const regexp = Parameter(Descriptor::kRegExp);
  Node* const subject_string = Parameter(Descriptor::kString);
  Node* const last_index = Parameter(Descriptor::kLastIndex);
  Node* const match_info = Parameter(Descriptor::kMatchInfo);
  Node* const context = Parameter(Descriptor::kContext);

  CSA_ASSERT(this, IsJSRegExp(regexp));
  CSA_ASSERT(this, IsString(subject_string));
  CSA_ASSERT(this, TaggedIsPositiveSmi(last_index));
  CSA_ASSERT(this, IsFixedArray(match_info));

  Node* const data = LoadObjectField(regexp, JSRegExp::kDataOffset);
  CSA_ASSERT(this, IsFixedArray(data));
  CSA_ASSERT(this, SmiEqual(LoadFixedArrayElement(data, JSRegExp::kTagIndex),
                            SmiConstant(JSRegExp::ATOM)));

  // Callers ensure that last_index is in-bounds.
  CSA_ASSERT(this,
             SmiLessThanOrEqual(last_index, LoadStringLength(subject_string)));

  Node* const needle_string =
      LoadFixedArrayElement(data, JSRegExp::kAtomPatternIndex);
  CSA_ASSERT(this, IsString(needle_string));

  Node* const match_from =
      CallBuiltin(Builtins::kStringIndexOf, context, subject_string,
                  needle_string, last_index);
  CSA_ASSERT(this, TaggedIsSmi(match_from));

  Label if_failure(this), if_success(this);
  Branch(SmiEqual(match_from, SmiConstant(-1)), &if_failure, &if_success);

  BIND(&if_success);
  {
    CSA_ASSERT(this, TaggedIsPositiveSmi(match_from));
    CSA_ASSERT(this, SmiLessThan(match_from, LoadStringLength(subject_string)));

    const int kNumRegisters = 2;
    STATIC_ASSERT(RegExpMatchInfo::kInitialCaptureIndices >= kNumRegisters);

    Node* const match_to = SmiAdd(match_from, LoadStringLength(needle_string));

    StoreFixedArrayElement(match_info, RegExpMatchInfo::kNumberOfCapturesIndex,
                           SmiConstant(kNumRegisters), SKIP_WRITE_BARRIER);
    StoreFixedArrayElement(match_info, RegExpMatchInfo::kLastSubjectIndex,
                           subject_string);
    StoreFixedArrayElement(match_info, RegExpMatchInfo::kLastInputIndex,
                           subject_string);
    StoreFixedArrayElement(match_info, RegExpMatchInfo::kFirstCaptureIndex,
                           match_from, SKIP_WRITE_BARRIER);
    StoreFixedArrayElement(match_info, RegExpMatchInfo::kFirstCaptureIndex + 1,
                           match_to, SKIP_WRITE_BARRIER);

    Return(match_info);
  }

  BIND(&if_failure);
  Return(NullConstant());
}

// ES#sec-regexp.prototype.exec
// RegExp.prototype.exec ( string )
TF_BUILTIN(RegExpPrototypeExec, RegExpBuiltinsAssembler) {
  Node* const maybe_receiver = Parameter(Descriptor::kReceiver);
  Node* const maybe_string = Parameter(Descriptor::kString);
  Node* const context = Parameter(Descriptor::kContext);

  // Ensure {maybe_receiver} is a JSRegExp.
  ThrowIfNotInstanceType(context, maybe_receiver, JS_REGEXP_TYPE,
                         "RegExp.prototype.exec");
  Node* const receiver = maybe_receiver;

  // Convert {maybe_string} to a String.
  Node* const string = ToString_Inline(context, maybe_string);

  Label if_isfastpath(this), if_isslowpath(this);
  Branch(IsFastRegExpNoPrototype(context, receiver), &if_isfastpath,
         &if_isslowpath);

  BIND(&if_isfastpath);
  {
    Node* const result =
        RegExpPrototypeExecBody(context, receiver, string, true);
    Return(result);
  }

  BIND(&if_isslowpath);
  {
    Node* const result = CallBuiltin(Builtins::kRegExpPrototypeExecSlow,
                                     context, receiver, string);
    Return(result);
  }
}

Node* RegExpBuiltinsAssembler::FlagsGetter(Node* const context,
                                           Node* const regexp,
                                           bool is_fastpath) {
  Isolate* isolate = this->isolate();

  Node* const int_zero = IntPtrConstant(0);
  Node* const int_one = IntPtrConstant(1);
  VARIABLE(var_length, MachineType::PointerRepresentation(), int_zero);
  VARIABLE(var_flags, MachineType::PointerRepresentation());

  Node* const is_dotall_enabled = IsDotAllEnabled(isolate);

  // First, count the number of characters we will need and check which flags
  // are set.

  if (is_fastpath) {
    // Refer to JSRegExp's flag property on the fast-path.
    CSA_ASSERT(this, IsJSRegExp(regexp));
    Node* const flags_smi = LoadObjectField(regexp, JSRegExp::kFlagsOffset);
    Node* const flags_intptr = SmiUntag(flags_smi);
    var_flags.Bind(flags_intptr);

#define CASE_FOR_FLAG(FLAG)                                  \
  do {                                                       \
    Label next(this);                                        \
    GotoIfNot(IsSetWord(flags_intptr, FLAG), &next);         \
    var_length.Bind(IntPtrAdd(var_length.value(), int_one)); \
    Goto(&next);                                             \
    BIND(&next);                                             \
  } while (false)

    CASE_FOR_FLAG(JSRegExp::kGlobal);
    CASE_FOR_FLAG(JSRegExp::kIgnoreCase);
    CASE_FOR_FLAG(JSRegExp::kMultiline);
    {
      Label next(this);
      GotoIfNot(is_dotall_enabled, &next);
      CASE_FOR_FLAG(JSRegExp::kDotAll);
      Goto(&next);
      BIND(&next);
    }
    CASE_FOR_FLAG(JSRegExp::kUnicode);
    CASE_FOR_FLAG(JSRegExp::kSticky);
#undef CASE_FOR_FLAG
  } else {
    DCHECK(!is_fastpath);

    // Fall back to GetProperty stub on the slow-path.
    var_flags.Bind(int_zero);

#define CASE_FOR_FLAG(NAME, FLAG)                                          \
  do {                                                                     \
    Label next(this);                                                      \
    Node* const flag = GetProperty(                                        \
        context, regexp, isolate->factory()->InternalizeUtf8String(NAME)); \
    Label if_isflagset(this);                                              \
    BranchIfToBooleanIsTrue(flag, &if_isflagset, &next);                   \
    BIND(&if_isflagset);                                                   \
    var_length.Bind(IntPtrAdd(var_length.value(), int_one));               \
    var_flags.Bind(WordOr(var_flags.value(), IntPtrConstant(FLAG)));       \
    Goto(&next);                                                           \
    BIND(&next);                                                           \
  } while (false)

    CASE_FOR_FLAG("global", JSRegExp::kGlobal);
    CASE_FOR_FLAG("ignoreCase", JSRegExp::kIgnoreCase);
    CASE_FOR_FLAG("multiline", JSRegExp::kMultiline);
    {
      Label next(this);
      GotoIfNot(is_dotall_enabled, &next);
      CASE_FOR_FLAG("dotAll", JSRegExp::kDotAll);
      Goto(&next);
      BIND(&next);
    }
    CASE_FOR_FLAG("unicode", JSRegExp::kUnicode);
    CASE_FOR_FLAG("sticky", JSRegExp::kSticky);
#undef CASE_FOR_FLAG
  }

  // Allocate a string of the required length and fill it with the corresponding
  // char for each set flag.

  {
    Node* const result = AllocateSeqOneByteString(context, var_length.value());
    Node* const flags_intptr = var_flags.value();

    VARIABLE(var_offset, MachineType::PointerRepresentation(),
             IntPtrConstant(SeqOneByteString::kHeaderSize - kHeapObjectTag));

#define CASE_FOR_FLAG(FLAG, CHAR)                              \
  do {                                                         \
    Label next(this);                                          \
    GotoIfNot(IsSetWord(flags_intptr, FLAG), &next);           \
    Node* const value = Int32Constant(CHAR);                   \
    StoreNoWriteBarrier(MachineRepresentation::kWord8, result, \
                        var_offset.value(), value);            \
    var_offset.Bind(IntPtrAdd(var_offset.value(), int_one));   \
    Goto(&next);                                               \
    BIND(&next);                                               \
  } while (false)

    CASE_FOR_FLAG(JSRegExp::kGlobal, 'g');
    CASE_FOR_FLAG(JSRegExp::kIgnoreCase, 'i');
    CASE_FOR_FLAG(JSRegExp::kMultiline, 'm');
    {
      Label next(this);
      GotoIfNot(is_dotall_enabled, &next);
      CASE_FOR_FLAG(JSRegExp::kDotAll, 's');
      Goto(&next);
      BIND(&next);
    }
    CASE_FOR_FLAG(JSRegExp::kUnicode, 'u');
    CASE_FOR_FLAG(JSRegExp::kSticky, 'y');
#undef CASE_FOR_FLAG

    return result;
  }
}

// ES#sec-isregexp IsRegExp ( argument )
Node* RegExpBuiltinsAssembler::IsRegExp(Node* const context,
                                        Node* const maybe_receiver) {
  Label out(this), if_isregexp(this);

  VARIABLE(var_result, MachineRepresentation::kWord32, Int32Constant(0));

  GotoIf(TaggedIsSmi(maybe_receiver), &out);
  GotoIfNot(IsJSReceiver(maybe_receiver), &out);

  Node* const receiver = maybe_receiver;

  // Check @@match.
  {
    Node* const value =
        GetProperty(context, receiver, isolate()->factory()->match_symbol());

    Label match_isundefined(this), match_isnotundefined(this);
    Branch(IsUndefined(value), &match_isundefined, &match_isnotundefined);

    BIND(&match_isundefined);
    Branch(IsJSRegExp(receiver), &if_isregexp, &out);

    BIND(&match_isnotundefined);
    BranchIfToBooleanIsTrue(value, &if_isregexp, &out);
  }

  BIND(&if_isregexp);
  var_result.Bind(Int32Constant(1));
  Goto(&out);

  BIND(&out);
  return var_result.value();
}

// ES#sec-regexpinitialize
// Runtime Semantics: RegExpInitialize ( obj, pattern, flags )
Node* RegExpBuiltinsAssembler::RegExpInitialize(Node* const context,
                                                Node* const regexp,
                                                Node* const maybe_pattern,
                                                Node* const maybe_flags) {
  CSA_ASSERT(this, IsJSRegExp(regexp));

  // Normalize pattern.
  Node* const pattern = Select<Object>(
      IsUndefined(maybe_pattern), [=] { return EmptyStringConstant(); },
      [=] { return ToString_Inline(context, maybe_pattern); },
      MachineRepresentation::kTagged);

  // Normalize flags.
  Node* const flags = Select<Object>(
      IsUndefined(maybe_flags), [=] { return EmptyStringConstant(); },
      [=] { return ToString_Inline(context, maybe_flags); },
      MachineRepresentation::kTagged);

  // Initialize.

  return CallRuntime(Runtime::kRegExpInitializeAndCompile, context, regexp,
                     pattern, flags);
}

// ES #sec-get-regexp.prototype.flags
TF_BUILTIN(RegExpPrototypeFlagsGetter, RegExpBuiltinsAssembler) {
  Node* const maybe_receiver = Parameter(Descriptor::kReceiver);
  Node* const context = Parameter(Descriptor::kContext);

  Node* const map = ThrowIfNotJSReceiver(context, maybe_receiver,
                                         MessageTemplate::kRegExpNonObject,
                                         "RegExp.prototype.flags");
  Node* const receiver = maybe_receiver;

  Label if_isfastpath(this), if_isslowpath(this, Label::kDeferred);
  Branch(IsFastRegExpNoPrototype(context, receiver, map), &if_isfastpath,
         &if_isslowpath);

  BIND(&if_isfastpath);
  Return(FlagsGetter(context, receiver, true));

  BIND(&if_isslowpath);
  Return(FlagsGetter(context, receiver, false));
}

// ES#sec-regexp-pattern-flags
// RegExp ( pattern, flags )
TF_BUILTIN(RegExpConstructor, RegExpBuiltinsAssembler) {
  Node* const pattern = Parameter(Descriptor::kPattern);
  Node* const flags = Parameter(Descriptor::kFlags);
  Node* const new_target = Parameter(Descriptor::kNewTarget);
  Node* const context = Parameter(Descriptor::kContext);

  Isolate* isolate = this->isolate();

  VARIABLE(var_flags, MachineRepresentation::kTagged, flags);
  VARIABLE(var_pattern, MachineRepresentation::kTagged, pattern);
  VARIABLE(var_new_target, MachineRepresentation::kTagged, new_target);

  Node* const native_context = LoadNativeContext(context);
  Node* const regexp_function =
      LoadContextElement(native_context, Context::REGEXP_FUNCTION_INDEX);

  Node* const pattern_is_regexp = IsRegExp(context, pattern);

  {
    Label next(this);

    GotoIfNot(IsUndefined(new_target), &next);
    var_new_target.Bind(regexp_function);

    GotoIfNot(pattern_is_regexp, &next);
    GotoIfNot(IsUndefined(flags), &next);

    Node* const value =
        GetProperty(context, pattern, isolate->factory()->constructor_string());

    GotoIfNot(WordEqual(value, regexp_function), &next);
    Return(pattern);

    BIND(&next);
  }

  {
    Label next(this), if_patternisfastregexp(this),
        if_patternisslowregexp(this);
    GotoIf(TaggedIsSmi(pattern), &next);

    GotoIf(IsJSRegExp(pattern), &if_patternisfastregexp);

    Branch(pattern_is_regexp, &if_patternisslowregexp, &next);

    BIND(&if_patternisfastregexp);
    {
      Node* const source = LoadObjectField(pattern, JSRegExp::kSourceOffset);
      var_pattern.Bind(source);

      {
        Label inner_next(this);
        GotoIfNot(IsUndefined(flags), &inner_next);

        Node* const value = FlagsGetter(context, pattern, true);
        var_flags.Bind(value);
        Goto(&inner_next);

        BIND(&inner_next);
      }

      Goto(&next);
    }

    BIND(&if_patternisslowregexp);
    {
      {
        Node* const value =
            GetProperty(context, pattern, isolate->factory()->source_string());
        var_pattern.Bind(value);
      }

      {
        Label inner_next(this);
        GotoIfNot(IsUndefined(flags), &inner_next);

        Node* const value =
            GetProperty(context, pattern, isolate->factory()->flags_string());
        var_flags.Bind(value);
        Goto(&inner_next);

        BIND(&inner_next);
      }

      Goto(&next);
    }

    BIND(&next);
  }

  // Allocate.

  VARIABLE(var_regexp, MachineRepresentation::kTagged);
  {
    Label allocate_jsregexp(this), allocate_generic(this, Label::kDeferred),
        next(this);
    Branch(WordEqual(var_new_target.value(), regexp_function),
           &allocate_jsregexp, &allocate_generic);

    BIND(&allocate_jsregexp);
    {
      Node* const initial_map = LoadObjectField(
          regexp_function, JSFunction::kPrototypeOrInitialMapOffset);
      Node* const regexp = AllocateJSObjectFromMap(initial_map);
      var_regexp.Bind(regexp);
      Goto(&next);
    }

    BIND(&allocate_generic);
    {
      ConstructorBuiltinsAssembler constructor_assembler(this->state());
      Node* const regexp = constructor_assembler.EmitFastNewObject(
          context, regexp_function, var_new_target.value());
      var_regexp.Bind(regexp);
      Goto(&next);
    }

    BIND(&next);
  }

  Node* const result = RegExpInitialize(context, var_regexp.value(),
                                        var_pattern.value(), var_flags.value());
  Return(result);
}

// ES#sec-regexp.prototype.compile
// RegExp.prototype.compile ( pattern, flags )
TF_BUILTIN(RegExpPrototypeCompile, RegExpBuiltinsAssembler) {
  Node* const maybe_receiver = Parameter(Descriptor::kReceiver);
  Node* const maybe_pattern = Parameter(Descriptor::kPattern);
  Node* const maybe_flags = Parameter(Descriptor::kFlags);
  Node* const context = Parameter(Descriptor::kContext);

  ThrowIfNotInstanceType(context, maybe_receiver, JS_REGEXP_TYPE,
                         "RegExp.prototype.compile");
  Node* const receiver = maybe_receiver;

  VARIABLE(var_flags, MachineRepresentation::kTagged, maybe_flags);
  VARIABLE(var_pattern, MachineRepresentation::kTagged, maybe_pattern);

  // Handle a JSRegExp pattern.
  {
    Label next(this);

    GotoIf(TaggedIsSmi(maybe_pattern), &next);
    GotoIfNot(IsJSRegExp(maybe_pattern), &next);

    Node* const pattern = maybe_pattern;

    // {maybe_flags} must be undefined in this case, otherwise throw.
    {
      Label next(this);
      GotoIf(IsUndefined(maybe_flags), &next);

      Node* const message_id = SmiConstant(MessageTemplate::kRegExpFlags);
      TailCallRuntime(Runtime::kThrowTypeError, context, message_id);

      BIND(&next);
    }

    Node* const new_flags = FlagsGetter(context, pattern, true);
    Node* const new_pattern = LoadObjectField(pattern, JSRegExp::kSourceOffset);

    var_flags.Bind(new_flags);
    var_pattern.Bind(new_pattern);

    Goto(&next);
    BIND(&next);
  }

  Node* const result = RegExpInitialize(context, receiver, var_pattern.value(),
                                        var_flags.value());
  Return(result);
}

// ES6 21.2.5.10.
// ES #sec-get-regexp.prototype.source
TF_BUILTIN(RegExpPrototypeSourceGetter, RegExpBuiltinsAssembler) {
  Node* const receiver = Parameter(Descriptor::kReceiver);
  Node* const context = Parameter(Descriptor::kContext);

  // Check whether we have an unmodified regexp instance.
  Label if_isjsregexp(this), if_isnotjsregexp(this, Label::kDeferred);

  GotoIf(TaggedIsSmi(receiver), &if_isnotjsregexp);
  Branch(IsJSRegExp(receiver), &if_isjsregexp, &if_isnotjsregexp);

  BIND(&if_isjsregexp);
  {
    Node* const source = LoadObjectField(receiver, JSRegExp::kSourceOffset);
    Return(source);
  }

  BIND(&if_isnotjsregexp);
  {
    Isolate* isolate = this->isolate();
    Node* const native_context = LoadNativeContext(context);
    Node* const regexp_fun =
        LoadContextElement(native_context, Context::REGEXP_FUNCTION_INDEX);
    Node* const initial_map =
        LoadObjectField(regexp_fun, JSFunction::kPrototypeOrInitialMapOffset);
    Node* const initial_prototype = LoadMapPrototype(initial_map);

    Label if_isprototype(this), if_isnotprototype(this);
    Branch(WordEqual(receiver, initial_prototype), &if_isprototype,
           &if_isnotprototype);

    BIND(&if_isprototype);
    {
      const int counter = v8::Isolate::kRegExpPrototypeSourceGetter;
      Node* const counter_smi = SmiConstant(counter);
      CallRuntime(Runtime::kIncrementUseCounter, context, counter_smi);

      Node* const result =
          HeapConstant(isolate->factory()->NewStringFromAsciiChecked("(?:)"));
      Return(result);
    }

    BIND(&if_isnotprototype);
    {
      Node* const message_id = SmiConstant(MessageTemplate::kRegExpNonRegExp);
      Node* const method_name_str =
          HeapConstant(isolate->factory()->NewStringFromAsciiChecked(
              "RegExp.prototype.source"));
      TailCallRuntime(Runtime::kThrowTypeError, context, message_id,
                      method_name_str);
    }
  }
}

// Fast-path implementation for flag checks on an unmodified JSRegExp instance.
Node* RegExpBuiltinsAssembler::FastFlagGetter(Node* const regexp,
                                              JSRegExp::Flag flag) {
  Node* const flags = LoadObjectField(regexp, JSRegExp::kFlagsOffset);
  Node* const mask = SmiConstant(flag);
  return SmiToWord32(SmiAnd(flags, mask));
}

// Load through the GetProperty stub.
Node* RegExpBuiltinsAssembler::SlowFlagGetter(Node* const context,
                                              Node* const regexp,
                                              JSRegExp::Flag flag) {
  Factory* factory = isolate()->factory();

  Label out(this);
  VARIABLE(var_result, MachineRepresentation::kWord32);

  Handle<String> name;
  switch (flag) {
    case JSRegExp::kGlobal:
      name = factory->global_string();
      break;
    case JSRegExp::kIgnoreCase:
      name = factory->ignoreCase_string();
      break;
    case JSRegExp::kMultiline:
      name = factory->multiline_string();
      break;
    case JSRegExp::kDotAll:
      UNREACHABLE();  // Never called for dotAll.
      break;
    case JSRegExp::kSticky:
      name = factory->sticky_string();
      break;
    case JSRegExp::kUnicode:
      name = factory->unicode_string();
      break;
    default:
      UNREACHABLE();
  }

  Node* const value = GetProperty(context, regexp, name);

  Label if_true(this), if_false(this);
  BranchIfToBooleanIsTrue(value, &if_true, &if_false);

  BIND(&if_true);
  {
    var_result.Bind(Int32Constant(1));
    Goto(&out);
  }

  BIND(&if_false);
  {
    var_result.Bind(Int32Constant(0));
    Goto(&out);
  }

  BIND(&out);
  return var_result.value();
}

Node* RegExpBuiltinsAssembler::FlagGetter(Node* const context,
                                          Node* const regexp,
                                          JSRegExp::Flag flag,
                                          bool is_fastpath) {
  return is_fastpath ? FastFlagGetter(regexp, flag)
                     : SlowFlagGetter(context, regexp, flag);
}

void RegExpBuiltinsAssembler::FlagGetter(Node* context, Node* receiver,
                                         JSRegExp::Flag flag, int counter,
                                         const char* method_name) {
  Isolate* isolate = this->isolate();

  // Check whether we have an unmodified regexp instance.
  Label if_isunmodifiedjsregexp(this),
      if_isnotunmodifiedjsregexp(this, Label::kDeferred);

  GotoIf(TaggedIsSmi(receiver), &if_isnotunmodifiedjsregexp);
  Branch(IsJSRegExp(receiver), &if_isunmodifiedjsregexp,
         &if_isnotunmodifiedjsregexp);

  BIND(&if_isunmodifiedjsregexp);
  {
    // Refer to JSRegExp's flag property on the fast-path.
    Node* const is_flag_set = FastFlagGetter(receiver, flag);
    Return(SelectBooleanConstant(is_flag_set));
  }

  BIND(&if_isnotunmodifiedjsregexp);
  {
    Node* const native_context = LoadNativeContext(context);
    Node* const regexp_fun =
        LoadContextElement(native_context, Context::REGEXP_FUNCTION_INDEX);
    Node* const initial_map =
        LoadObjectField(regexp_fun, JSFunction::kPrototypeOrInitialMapOffset);
    Node* const initial_prototype = LoadMapPrototype(initial_map);

    Label if_isprototype(this), if_isnotprototype(this);
    Branch(WordEqual(receiver, initial_prototype), &if_isprototype,
           &if_isnotprototype);

    BIND(&if_isprototype);
    {
      if (counter != -1) {
        Node* const counter_smi = SmiConstant(counter);
        CallRuntime(Runtime::kIncrementUseCounter, context, counter_smi);
      }
      Return(UndefinedConstant());
    }

    BIND(&if_isnotprototype);
    {
      Node* const message_id = SmiConstant(MessageTemplate::kRegExpNonRegExp);
      Node* const method_name_str = HeapConstant(
          isolate->factory()->NewStringFromAsciiChecked(method_name));
      CallRuntime(Runtime::kThrowTypeError, context, message_id,
                  method_name_str);
      Unreachable();
    }
  }
}

// ES6 21.2.5.4.
// ES #sec-get-regexp.prototype.global
TF_BUILTIN(RegExpPrototypeGlobalGetter, RegExpBuiltinsAssembler) {
  Node* context = Parameter(Descriptor::kContext);
  Node* receiver = Parameter(Descriptor::kReceiver);
  FlagGetter(context, receiver, JSRegExp::kGlobal,
             v8::Isolate::kRegExpPrototypeOldFlagGetter,
             "RegExp.prototype.global");
}

// ES6 21.2.5.5.
// ES #sec-get-regexp.prototype.ignorecase
TF_BUILTIN(RegExpPrototypeIgnoreCaseGetter, RegExpBuiltinsAssembler) {
  Node* context = Parameter(Descriptor::kContext);
  Node* receiver = Parameter(Descriptor::kReceiver);
  FlagGetter(context, receiver, JSRegExp::kIgnoreCase,
             v8::Isolate::kRegExpPrototypeOldFlagGetter,
             "RegExp.prototype.ignoreCase");
}

// ES6 21.2.5.7.
// ES #sec-get-regexp.prototype.multiline
TF_BUILTIN(RegExpPrototypeMultilineGetter, RegExpBuiltinsAssembler) {
  Node* context = Parameter(Descriptor::kContext);
  Node* receiver = Parameter(Descriptor::kReceiver);
  FlagGetter(context, receiver, JSRegExp::kMultiline,
             v8::Isolate::kRegExpPrototypeOldFlagGetter,
             "RegExp.prototype.multiline");
}

Node* RegExpBuiltinsAssembler::IsDotAllEnabled(Isolate* isolate) {
  Node* flag_ptr = ExternalConstant(
      ExternalReference::address_of_regexp_dotall_flag(isolate));
  Node* const flag_value = Load(MachineType::Int8(), flag_ptr);
  return Word32NotEqual(flag_value, Int32Constant(0));
}

// ES #sec-get-regexp.prototype.dotAll
TF_BUILTIN(RegExpPrototypeDotAllGetter, RegExpBuiltinsAssembler) {
  Node* context = Parameter(Descriptor::kContext);
  Node* receiver = Parameter(Descriptor::kReceiver);
  static const int kNoCounter = -1;
  CSA_ASSERT(this, IsDotAllEnabled(isolate()));
  FlagGetter(context, receiver, JSRegExp::kDotAll, kNoCounter,
             "RegExp.prototype.dotAll");
}

// ES6 21.2.5.12.
// ES #sec-get-regexp.prototype.sticky
TF_BUILTIN(RegExpPrototypeStickyGetter, RegExpBuiltinsAssembler) {
  Node* context = Parameter(Descriptor::kContext);
  Node* receiver = Parameter(Descriptor::kReceiver);
  FlagGetter(context, receiver, JSRegExp::kSticky,
             v8::Isolate::kRegExpPrototypeStickyGetter,
             "RegExp.prototype.sticky");
}

// ES6 21.2.5.15.
// ES #sec-get-regexp.prototype.unicode
TF_BUILTIN(RegExpPrototypeUnicodeGetter, RegExpBuiltinsAssembler) {
  Node* context = Parameter(Descriptor::kContext);
  Node* receiver = Parameter(Descriptor::kReceiver);
  FlagGetter(context, receiver, JSRegExp::kUnicode,
             v8::Isolate::kRegExpPrototypeUnicodeGetter,
             "RegExp.prototype.unicode");
}

// ES#sec-regexpexec Runtime Semantics: RegExpExec ( R, S )
Node* RegExpBuiltinsAssembler::RegExpExec(Node* context, Node* regexp,
                                          Node* string) {
  VARIABLE(var_result, MachineRepresentation::kTagged);
  Label out(this);

  // Take the slow path of fetching the exec property, calling it, and
  // verifying its return value.

  // Get the exec property.
  Node* const exec =
      GetProperty(context, regexp, isolate()->factory()->exec_string());

  // Is {exec} callable?
  Label if_iscallable(this), if_isnotcallable(this);

  GotoIf(TaggedIsSmi(exec), &if_isnotcallable);

  Node* const exec_map = LoadMap(exec);
  Branch(IsCallableMap(exec_map), &if_iscallable, &if_isnotcallable);

  BIND(&if_iscallable);
  {
    Callable call_callable = CodeFactory::Call(isolate());
    Node* const result = CallJS(call_callable, context, exec, regexp, string);

    var_result.Bind(result);
    GotoIf(WordEqual(result, NullConstant()), &out);

    ThrowIfNotJSReceiver(context, result,
                         MessageTemplate::kInvalidRegExpExecResult, "unused");

    Goto(&out);
  }

  BIND(&if_isnotcallable);
  {
    ThrowIfNotInstanceType(context, regexp, JS_REGEXP_TYPE,
                           "RegExp.prototype.exec");

    Node* const result = CallBuiltin(Builtins::kRegExpPrototypeExecSlow,
                                     context, regexp, string);
    var_result.Bind(result);
    Goto(&out);
  }

  BIND(&out);
  return var_result.value();
}

// ES#sec-regexp.prototype.test
// RegExp.prototype.test ( S )
TF_BUILTIN(RegExpPrototypeTest, RegExpBuiltinsAssembler) {
  Node* const maybe_receiver = Parameter(Descriptor::kReceiver);
  Node* const maybe_string = Parameter(Descriptor::kString);
  Node* const context = Parameter(Descriptor::kContext);

  // Ensure {maybe_receiver} is a JSReceiver.
  ThrowIfNotJSReceiver(context, maybe_receiver,
                       MessageTemplate::kIncompatibleMethodReceiver,
                       "RegExp.prototype.test");
  Node* const receiver = maybe_receiver;

  // Convert {maybe_string} to a String.
  Node* const string = ToString_Inline(context, maybe_string);

  Label fast_path(this), slow_path(this);
  BranchIfFastRegExp(context, receiver, &fast_path, &slow_path);

  BIND(&fast_path);
  {
    Label if_didnotmatch(this);
    RegExpPrototypeExecBodyWithoutResult(context, receiver, string,
                                         &if_didnotmatch, true);
    Return(TrueConstant());

    BIND(&if_didnotmatch);
    Return(FalseConstant());
  }

  BIND(&slow_path);
  {
    // Call exec.
    Node* const match_indices = RegExpExec(context, receiver, string);

    // Return true iff exec matched successfully.
    Node* const result =
        SelectBooleanConstant(WordNotEqual(match_indices, NullConstant()));
    Return(result);
  }
}

Node* RegExpBuiltinsAssembler::AdvanceStringIndex(Node* const string,
                                                  Node* const index,
                                                  Node* const is_unicode,
                                                  bool is_fastpath) {
  CSA_ASSERT(this, IsString(string));
  CSA_ASSERT(this, IsNumberNormalized(index));
  if (is_fastpath) CSA_ASSERT(this, TaggedIsPositiveSmi(index));

  // Default to last_index + 1.
  Node* const index_plus_one = NumberInc(index);
  VARIABLE(var_result, MachineRepresentation::kTagged, index_plus_one);

  // Advancing the index has some subtle issues involving the distinction
  // between Smis and HeapNumbers. There's three cases:
  // * {index} is a Smi, {index_plus_one} is a Smi. The standard case.
  // * {index} is a Smi, {index_plus_one} overflows into a HeapNumber.
  //   In this case we can return the result early, because
  //   {index_plus_one} > {string}.length.
  // * {index} is a HeapNumber, {index_plus_one} is a HeapNumber. This can only
  //   occur when {index} is outside the Smi range since we normalize
  //   explicitly. Again we can return early.
  if (is_fastpath) {
    // Must be in Smi range on the fast path. We control the value of {index}
    // on all call-sites and can never exceed the length of the string.
    STATIC_ASSERT(String::kMaxLength + 2 < Smi::kMaxValue);
    CSA_ASSERT(this, TaggedIsPositiveSmi(index_plus_one));
  }

  Label if_isunicode(this), out(this);
  GotoIfNot(is_unicode, &out);

  // Keep this unconditional (even on the fast path) just to be safe.
  Branch(TaggedIsPositiveSmi(index_plus_one), &if_isunicode, &out);

  BIND(&if_isunicode);
  {
    Node* const string_length = LoadStringLength(string);
    GotoIfNot(SmiLessThan(index_plus_one, string_length), &out);

    Node* const lead = StringCharCodeAt(string, index);
    GotoIfNot(Word32Equal(Word32And(lead, Int32Constant(0xFC00)),
                          Int32Constant(0xD800)),
              &out);

    Node* const trail = StringCharCodeAt(string, index_plus_one);
    GotoIfNot(Word32Equal(Word32And(trail, Int32Constant(0xFC00)),
                          Int32Constant(0xDC00)),
              &out);

    // At a surrogate pair, return index + 2.
    Node* const index_plus_two = NumberInc(index_plus_one);
    var_result.Bind(index_plus_two);

    Goto(&out);
  }

  BIND(&out);
  return var_result.value();
}

namespace {

// Utility class implementing a growable fixed array through CSA.
class GrowableFixedArray {
  typedef CodeStubAssembler::Label Label;
  typedef CodeStubAssembler::Variable Variable;

 public:
  explicit GrowableFixedArray(CodeStubAssembler* a)
      : assembler_(a),
        var_array_(a, MachineRepresentation::kTagged),
        var_length_(a, MachineType::PointerRepresentation()),
        var_capacity_(a, MachineType::PointerRepresentation()) {
    Initialize();
  }

  Node* length() const { return var_length_.value(); }

  Variable* var_array() { return &var_array_; }
  Variable* var_length() { return &var_length_; }
  Variable* var_capacity() { return &var_capacity_; }

  void Push(Node* const value) {
    CodeStubAssembler* a = assembler_;

    Node* const length = var_length_.value();
    Node* const capacity = var_capacity_.value();

    Label grow(a), store(a);
    a->Branch(a->IntPtrEqual(capacity, length), &grow, &store);

    a->BIND(&grow);
    {
      Node* const new_capacity = NewCapacity(a, capacity);
      Node* const new_array = ResizeFixedArray(length, new_capacity);

      var_capacity_.Bind(new_capacity);
      var_array_.Bind(new_array);
      a->Goto(&store);
    }

    a->BIND(&store);
    {
      Node* const array = var_array_.value();
      a->StoreFixedArrayElement(array, length, value);

      Node* const new_length = a->IntPtrAdd(length, a->IntPtrConstant(1));
      var_length_.Bind(new_length);
    }
  }

  Node* ToJSArray(Node* const context) {
    CodeStubAssembler* a = assembler_;

    const ElementsKind kind = PACKED_ELEMENTS;

    Node* const native_context = a->LoadNativeContext(context);
    Node* const array_map = a->LoadJSArrayElementsMap(kind, native_context);

    // Shrink to fit if necessary.
    {
      Label next(a);

      Node* const length = var_length_.value();
      Node* const capacity = var_capacity_.value();

      a->GotoIf(a->WordEqual(length, capacity), &next);

      Node* const array = ResizeFixedArray(length, length);
      var_array_.Bind(array);
      var_capacity_.Bind(length);
      a->Goto(&next);

      a->BIND(&next);
    }

    Node* const result_length = a->SmiTag(length());
    Node* const result = a->AllocateUninitializedJSArrayWithoutElements(
        kind, array_map, result_length, nullptr);

    // Note: We do not currently shrink the fixed array.

    a->StoreObjectField(result, JSObject::kElementsOffset, var_array_.value());

    return result;
  }

 private:
  void Initialize() {
    CodeStubAssembler* a = assembler_;

    const ElementsKind kind = PACKED_ELEMENTS;

    static const int kInitialArraySize = 8;
    Node* const capacity = a->IntPtrConstant(kInitialArraySize);
    Node* const array = a->AllocateFixedArray(kind, capacity);

    a->FillFixedArrayWithValue(kind, array, a->IntPtrConstant(0), capacity,
                               Heap::kTheHoleValueRootIndex);

    var_array_.Bind(array);
    var_capacity_.Bind(capacity);
    var_length_.Bind(a->IntPtrConstant(0));
  }

  Node* NewCapacity(CodeStubAssembler* a,
                    compiler::SloppyTNode<IntPtrT> current_capacity) {
    CSA_ASSERT(a, a->IntPtrGreaterThan(current_capacity, a->IntPtrConstant(0)));

    // Growth rate is analog to JSObject::NewElementsCapacity:
    // new_capacity = (current_capacity + (current_capacity >> 1)) + 16.

    Node* const new_capacity = a->IntPtrAdd(
        a->IntPtrAdd(current_capacity, a->WordShr(current_capacity, 1)),
        a->IntPtrConstant(16));

    return new_capacity;
  }

  // Creates a new array with {new_capacity} and copies the first
  // {element_count} elements from the current array.
  Node* ResizeFixedArray(Node* const element_count, Node* const new_capacity) {
    CodeStubAssembler* a = assembler_;

    CSA_ASSERT(a, a->IntPtrGreaterThan(element_count, a->IntPtrConstant(0)));
    CSA_ASSERT(a, a->IntPtrGreaterThan(new_capacity, a->IntPtrConstant(0)));
    CSA_ASSERT(a, a->IntPtrGreaterThanOrEqual(new_capacity, element_count));

    const ElementsKind kind = PACKED_ELEMENTS;
    const WriteBarrierMode barrier_mode = UPDATE_WRITE_BARRIER;
    const CodeStubAssembler::ParameterMode mode =
        CodeStubAssembler::INTPTR_PARAMETERS;
    const CodeStubAssembler::AllocationFlags flags =
        CodeStubAssembler::kAllowLargeObjectAllocation;

    Node* const from_array = var_array_.value();
    Node* const to_array =
        a->AllocateFixedArray(kind, new_capacity, mode, flags);
    a->CopyFixedArrayElements(kind, from_array, kind, to_array, element_count,
                              new_capacity, barrier_mode, mode);

    return to_array;
  }

 private:
  CodeStubAssembler* const assembler_;
  Variable var_array_;
  Variable var_length_;
  Variable var_capacity_;
};

}  // namespace

void RegExpBuiltinsAssembler::RegExpPrototypeMatchBody(Node* const context,
                                                       Node* const regexp,
                                                       Node* const string,
                                                       const bool is_fastpath) {
  CSA_ASSERT(this, IsString(string));
  if (is_fastpath) CSA_ASSERT(this, IsFastRegExp(context, regexp));

  Node* const null = NullConstant();
  Node* const int_zero = IntPtrConstant(0);
  Node* const smi_zero = SmiConstant(0);

  Node* const is_global =
      FlagGetter(context, regexp, JSRegExp::kGlobal, is_fastpath);

  Label if_isglobal(this), if_isnotglobal(this);
  Branch(is_global, &if_isglobal, &if_isnotglobal);

  BIND(&if_isnotglobal);
  {
    Node* const result =
        is_fastpath ? RegExpPrototypeExecBody(context, regexp, string, true)
                    : RegExpExec(context, regexp, string);
    Return(result);
  }

  BIND(&if_isglobal);
  {
    Node* const is_unicode =
        FlagGetter(context, regexp, JSRegExp::kUnicode, is_fastpath);

    StoreLastIndex(context, regexp, smi_zero, is_fastpath);

    // Allocate an array to store the resulting match strings.

    GrowableFixedArray array(this);

    // Loop preparations. Within the loop, collect results from RegExpExec
    // and store match strings in the array.

    Variable* vars[] = {array.var_array(), array.var_length(),
                        array.var_capacity()};
    Label loop(this, 3, vars), out(this);
    Goto(&loop);

    BIND(&loop);
    {
      VARIABLE(var_match, MachineRepresentation::kTagged);

      Label if_didmatch(this), if_didnotmatch(this);
      if (is_fastpath) {
        // On the fast path, grab the matching string from the raw match index
        // array.
        Node* const match_indices = RegExpPrototypeExecBodyWithoutResult(
            context, regexp, string, &if_didnotmatch, true);

        Node* const match_from = LoadFixedArrayElement(
            match_indices, RegExpMatchInfo::kFirstCaptureIndex);
        Node* const match_to = LoadFixedArrayElement(
            match_indices, RegExpMatchInfo::kFirstCaptureIndex + 1);

        Node* match = SubString(context, string, match_from, match_to);
        var_match.Bind(match);

        Goto(&if_didmatch);
      } else {
        DCHECK(!is_fastpath);
        Node* const result = RegExpExec(context, regexp, string);

        Label load_match(this);
        Branch(WordEqual(result, null), &if_didnotmatch, &load_match);

        BIND(&load_match);
        {
          Label fast_result(this), slow_result(this);
          BranchIfFastRegExpResult(context, result, &fast_result, &slow_result);

          BIND(&fast_result);
          {
            Node* const result_fixed_array = LoadElements(result);
            Node* const match = LoadFixedArrayElement(result_fixed_array, 0);

            // The match is guaranteed to be a string on the fast path.
            CSA_ASSERT(this, IsStringInstanceType(LoadInstanceType(match)));

            var_match.Bind(match);
            Goto(&if_didmatch);
          }

          BIND(&slow_result);
          {
            // TODO(ishell): Use GetElement stub once it's available.
            Node* const match = GetProperty(context, result, smi_zero);
            var_match.Bind(ToString_Inline(context, match));
            Goto(&if_didmatch);
          }
        }
      }

      BIND(&if_didnotmatch);
      {
        // Return null if there were no matches, otherwise just exit the loop.
        GotoIfNot(IntPtrEqual(array.length(), int_zero), &out);
        Return(null);
      }

      BIND(&if_didmatch);
      {
        Node* match = var_match.value();

        // Store the match, growing the fixed array if needed.

        array.Push(match);

        // Advance last index if the match is the empty string.

        Node* const match_length = LoadStringLength(match);
        GotoIfNot(SmiEqual(match_length, smi_zero), &loop);

        Node* last_index = LoadLastIndex(context, regexp, is_fastpath);
        if (is_fastpath) {
          CSA_ASSERT(this, TaggedIsPositiveSmi(last_index));
        } else {
          last_index = ToLength_Inline(context, last_index);
        }

        Node* const new_last_index =
            AdvanceStringIndex(string, last_index, is_unicode, is_fastpath);

        if (is_fastpath) {
          // On the fast path, we can be certain that lastIndex can never be
          // incremented to overflow the Smi range since the maximal string
          // length is less than the maximal Smi value.
          STATIC_ASSERT(String::kMaxLength < Smi::kMaxValue);
          CSA_ASSERT(this, TaggedIsPositiveSmi(new_last_index));
        }

        StoreLastIndex(context, regexp, new_last_index, is_fastpath);

        Goto(&loop);
      }
    }

    BIND(&out);
    {
      // Wrap the match in a JSArray.

      Node* const result = array.ToJSArray(context);
      Return(result);
    }
  }
}

// ES#sec-regexp.prototype-@@match
// RegExp.prototype [ @@match ] ( string )
TF_BUILTIN(RegExpPrototypeMatch, RegExpBuiltinsAssembler) {
  Node* const maybe_receiver = Parameter(Descriptor::kReceiver);
  Node* const maybe_string = Parameter(Descriptor::kString);
  Node* const context = Parameter(Descriptor::kContext);

  // Ensure {maybe_receiver} is a JSReceiver.
  ThrowIfNotJSReceiver(context, maybe_receiver,
                       MessageTemplate::kIncompatibleMethodReceiver,
                       "RegExp.prototype.@@match");
  Node* const receiver = maybe_receiver;

  // Convert {maybe_string} to a String.
  Node* const string = ToString_Inline(context, maybe_string);

  Label fast_path(this), slow_path(this);
  BranchIfFastRegExp(context, receiver, &fast_path, &slow_path);

  BIND(&fast_path);
  RegExpPrototypeMatchBody(context, receiver, string, true);

  BIND(&slow_path);
  RegExpPrototypeMatchBody(context, receiver, string, false);
}

void RegExpBuiltinsAssembler::RegExpPrototypeSearchBodyFast(
    Node* const context, Node* const regexp, Node* const string) {
  CSA_ASSERT(this, IsFastRegExp(context, regexp));
  CSA_ASSERT(this, IsString(string));

  // Grab the initial value of last index.
  Node* const previous_last_index = FastLoadLastIndex(regexp);

  // Ensure last index is 0.
  FastStoreLastIndex(regexp, SmiConstant(0));

  // Call exec.
  Label if_didnotmatch(this);
  Node* const match_indices = RegExpPrototypeExecBodyWithoutResult(
      context, regexp, string, &if_didnotmatch, true);

  // Successful match.
  {
    // Reset last index.
    FastStoreLastIndex(regexp, previous_last_index);

    // Return the index of the match.
    Node* const index = LoadFixedArrayElement(
        match_indices, RegExpMatchInfo::kFirstCaptureIndex);
    Return(index);
  }

  BIND(&if_didnotmatch);
  {
    // Reset last index and return -1.
    FastStoreLastIndex(regexp, previous_last_index);
    Return(SmiConstant(-1));
  }
}

void RegExpBuiltinsAssembler::RegExpPrototypeSearchBodySlow(
    Node* const context, Node* const regexp, Node* const string) {
  CSA_ASSERT(this, IsJSReceiver(regexp));
  CSA_ASSERT(this, IsString(string));

  Isolate* const isolate = this->isolate();

  Node* const smi_zero = SmiConstant(0);

  // Grab the initial value of last index.
  Node* const previous_last_index = SlowLoadLastIndex(context, regexp);

  // Ensure last index is 0.
  {
    Label next(this);
    GotoIf(SameValue(previous_last_index, smi_zero), &next);

    SlowStoreLastIndex(context, regexp, smi_zero);
    Goto(&next);
    BIND(&next);
  }

  // Call exec.
  Node* const exec_result = RegExpExec(context, regexp, string);

  // Reset last index if necessary.
  {
    Label next(this);
    Node* const current_last_index = SlowLoadLastIndex(context, regexp);

    GotoIf(SameValue(current_last_index, previous_last_index), &next);

    SlowStoreLastIndex(context, regexp, previous_last_index);
    Goto(&next);

    BIND(&next);
  }

  // Return -1 if no match was found.
  {
    Label next(this);
    GotoIfNot(WordEqual(exec_result, NullConstant()), &next);
    Return(SmiConstant(-1));
    BIND(&next);
  }

  // Return the index of the match.
  {
    Label fast_result(this), slow_result(this, Label::kDeferred);
    BranchIfFastRegExpResult(context, exec_result, &fast_result, &slow_result);

    BIND(&fast_result);
    {
      Node* const index =
          LoadObjectField(exec_result, JSRegExpResult::kIndexOffset);
      Return(index);
    }

    BIND(&slow_result);
    {
      Return(GetProperty(context, exec_result,
                         isolate->factory()->index_string()));
    }
  }
}

// ES#sec-regexp.prototype-@@search
// RegExp.prototype [ @@search ] ( string )
TF_BUILTIN(RegExpPrototypeSearch, RegExpBuiltinsAssembler) {
  Node* const maybe_receiver = Parameter(Descriptor::kReceiver);
  Node* const maybe_string = Parameter(Descriptor::kString);
  Node* const context = Parameter(Descriptor::kContext);

  // Ensure {maybe_receiver} is a JSReceiver.
  ThrowIfNotJSReceiver(context, maybe_receiver,
                       MessageTemplate::kIncompatibleMethodReceiver,
                       "RegExp.prototype.@@search");
  Node* const receiver = maybe_receiver;

  // Convert {maybe_string} to a String.
  Node* const string = ToString_Inline(context, maybe_string);

  Label fast_path(this), slow_path(this);
  BranchIfFastRegExp(context, receiver, &fast_path, &slow_path);

  BIND(&fast_path);
  RegExpPrototypeSearchBodyFast(context, receiver, string);

  BIND(&slow_path);
  RegExpPrototypeSearchBodySlow(context, receiver, string);
}

// Generates the fast path for @@split. {regexp} is an unmodified, non-sticky
// JSRegExp, {string} is a String, and {limit} is a Smi.
void RegExpBuiltinsAssembler::RegExpPrototypeSplitBody(Node* const context,
                                                       Node* const regexp,
                                                       Node* const string,
                                                       Node* const limit) {
  CSA_ASSERT(this, IsFastRegExp(context, regexp));
  CSA_ASSERT(this, Word32BinaryNot(FastFlagGetter(regexp, JSRegExp::kSticky)));
  CSA_ASSERT(this, TaggedIsSmi(limit));
  CSA_ASSERT(this, IsString(string));

  Node* const null = NullConstant();
  Node* const smi_zero = SmiConstant(0);
  Node* const int_zero = IntPtrConstant(0);
  Node* const int_limit = SmiUntag(limit);

  const ElementsKind kind = PACKED_ELEMENTS;
  const ParameterMode mode = CodeStubAssembler::INTPTR_PARAMETERS;

  Node* const allocation_site = nullptr;
  Node* const native_context = LoadNativeContext(context);
  Node* const array_map = LoadJSArrayElementsMap(kind, native_context);

  Label return_empty_array(this, Label::kDeferred);

  // If limit is zero, return an empty array.
  {
    Label next(this), if_limitiszero(this, Label::kDeferred);
    Branch(SmiEqual(limit, smi_zero), &return_empty_array, &next);
    BIND(&next);
  }

  Node* const string_length = LoadStringLength(string);

  // If passed the empty {string}, return either an empty array or a singleton
  // array depending on whether the {regexp} matches.
  {
    Label next(this), if_stringisempty(this, Label::kDeferred);
    Branch(SmiEqual(string_length, smi_zero), &if_stringisempty, &next);

    BIND(&if_stringisempty);
    {
      Node* const last_match_info = LoadContextElement(
          native_context, Context::REGEXP_LAST_MATCH_INFO_INDEX);

      Node* const match_indices = RegExpExecInternal(context, regexp, string,
                                                     smi_zero, last_match_info);

      Label return_singleton_array(this);
      Branch(WordEqual(match_indices, null), &return_singleton_array,
             &return_empty_array);

      BIND(&return_singleton_array);
      {
        Node* const length = SmiConstant(1);
        Node* const capacity = IntPtrConstant(1);
        Node* const result = AllocateJSArray(kind, array_map, capacity, length,
                                             allocation_site, mode);

        Node* const fixed_array = LoadElements(result);
        StoreFixedArrayElement(fixed_array, 0, string);

        Return(result);
      }
    }

    BIND(&next);
  }

  // Loop preparations.

  GrowableFixedArray array(this);

  VARIABLE(var_last_matched_until, MachineRepresentation::kTagged);
  VARIABLE(var_next_search_from, MachineRepresentation::kTagged);

  var_last_matched_until.Bind(smi_zero);
  var_next_search_from.Bind(smi_zero);

  Variable* vars[] = {array.var_array(), array.var_length(),
                      array.var_capacity(), &var_last_matched_until,
                      &var_next_search_from};
  const int vars_count = sizeof(vars) / sizeof(vars[0]);
  Label loop(this, vars_count, vars), push_suffix_and_out(this), out(this);
  Goto(&loop);

  BIND(&loop);
  {
    Node* const next_search_from = var_next_search_from.value();
    Node* const last_matched_until = var_last_matched_until.value();

    CSA_ASSERT(this, TaggedIsSmi(next_search_from));
    CSA_ASSERT(this, TaggedIsSmi(last_matched_until));

    // We're done if we've reached the end of the string.
    {
      Label next(this);
      Branch(SmiEqual(next_search_from, string_length), &push_suffix_and_out,
             &next);
      BIND(&next);
    }

    // Search for the given {regexp}.

    Node* const last_match_info = LoadContextElement(
        native_context, Context::REGEXP_LAST_MATCH_INFO_INDEX);

    Node* const match_indices = RegExpExecInternal(
        context, regexp, string, next_search_from, last_match_info);

    // We're done if no match was found.
    {
      Label next(this);
      Branch(WordEqual(match_indices, null), &push_suffix_and_out, &next);
      BIND(&next);
    }

    Node* const match_from = LoadFixedArrayElement(
        match_indices, RegExpMatchInfo::kFirstCaptureIndex);

    // We're done if the match starts beyond the string.
    {
      Label next(this);
      Branch(WordEqual(match_from, string_length), &push_suffix_and_out, &next);
      BIND(&next);
    }

    Node* const match_to = LoadFixedArrayElement(
        match_indices, RegExpMatchInfo::kFirstCaptureIndex + 1);

    // Advance index and continue if the match is empty.
    {
      Label next(this);

      GotoIfNot(SmiEqual(match_to, next_search_from), &next);
      GotoIfNot(SmiEqual(match_to, last_matched_until), &next);

      Node* const is_unicode = FastFlagGetter(regexp, JSRegExp::kUnicode);
      Node* const new_next_search_from =
          AdvanceStringIndex(string, next_search_from, is_unicode, true);
      var_next_search_from.Bind(new_next_search_from);
      Goto(&loop);

      BIND(&next);
    }

    // A valid match was found, add the new substring to the array.
    {
      Node* const from = last_matched_until;
      Node* const to = match_from;

      Node* const substr = SubString(context, string, from, to);
      array.Push(substr);

      GotoIf(WordEqual(array.length(), int_limit), &out);
    }

    // Add all captures to the array.
    {
      Node* const num_registers = LoadFixedArrayElement(
          match_indices, RegExpMatchInfo::kNumberOfCapturesIndex);
      Node* const int_num_registers = SmiUntag(num_registers);

      VARIABLE(var_reg, MachineType::PointerRepresentation());
      var_reg.Bind(IntPtrConstant(2));

      Variable* vars[] = {array.var_array(), array.var_length(),
                          array.var_capacity(), &var_reg};
      const int vars_count = sizeof(vars) / sizeof(vars[0]);
      Label nested_loop(this, vars_count, vars), nested_loop_out(this);
      Branch(IntPtrLessThan(var_reg.value(), int_num_registers), &nested_loop,
             &nested_loop_out);

      BIND(&nested_loop);
      {
        Node* const reg = var_reg.value();
        Node* const from = LoadFixedArrayElement(
            match_indices, reg,
            RegExpMatchInfo::kFirstCaptureIndex * kPointerSize, mode);
        Node* const to = LoadFixedArrayElement(
            match_indices, reg,
            (RegExpMatchInfo::kFirstCaptureIndex + 1) * kPointerSize, mode);

        Label select_capture(this), select_undefined(this), store_value(this);
        VARIABLE(var_value, MachineRepresentation::kTagged);
        Branch(SmiEqual(to, SmiConstant(-1)), &select_undefined,
               &select_capture);

        BIND(&select_capture);
        {
          Node* const substr = SubString(context, string, from, to);
          var_value.Bind(substr);
          Goto(&store_value);
        }

        BIND(&select_undefined);
        {
          Node* const undefined = UndefinedConstant();
          var_value.Bind(undefined);
          Goto(&store_value);
        }

        BIND(&store_value);
        {
          array.Push(var_value.value());
          GotoIf(WordEqual(array.length(), int_limit), &out);

          Node* const new_reg = IntPtrAdd(reg, IntPtrConstant(2));
          var_reg.Bind(new_reg);

          Branch(IntPtrLessThan(new_reg, int_num_registers), &nested_loop,
                 &nested_loop_out);
        }
      }

      BIND(&nested_loop_out);
    }

    var_last_matched_until.Bind(match_to);
    var_next_search_from.Bind(match_to);
    Goto(&loop);
  }

  BIND(&push_suffix_and_out);
  {
    Node* const from = var_last_matched_until.value();
    Node* const to = string_length;

    Node* const substr = SubString(context, string, from, to);
    array.Push(substr);

    Goto(&out);
  }

  BIND(&out);
  {
    Node* const result = array.ToJSArray(context);
    Return(result);
  }

  BIND(&return_empty_array);
  {
    Node* const length = smi_zero;
    Node* const capacity = int_zero;
    Node* const result = AllocateJSArray(kind, array_map, capacity, length,
                                         allocation_site, mode);
    Return(result);
  }
}

// Helper that skips a few initial checks.
TF_BUILTIN(RegExpSplit, RegExpBuiltinsAssembler) {
  Node* const regexp = Parameter(Descriptor::kRegExp);
  Node* const string = Parameter(Descriptor::kString);
  Node* const maybe_limit = Parameter(Descriptor::kLimit);
  Node* const context = Parameter(Descriptor::kContext);

  CSA_ASSERT(this, IsFastRegExp(context, regexp));
  CSA_ASSERT(this, IsString(string));

  // TODO(jgruber): Even if map checks send us to the fast path, we still need
  // to verify the constructor property and jump to the slow path if it has
  // been changed.

  // Convert {maybe_limit} to a uint32, capping at the maximal smi value.

  VARIABLE(var_limit, MachineRepresentation::kTagged, maybe_limit);
  Label if_limitissmimax(this), runtime(this, Label::kDeferred);

  {
    Label next(this);

    GotoIf(IsUndefined(maybe_limit), &if_limitissmimax);
    GotoIf(TaggedIsPositiveSmi(maybe_limit), &next);

    var_limit.Bind(ToUint32(context, maybe_limit));
    {
      // ToUint32(limit) could potentially change the shape of the RegExp
      // object. Recheck that we are still on the fast path and bail to runtime
      // otherwise.
      {
        Label next(this);
        BranchIfFastRegExp(context, regexp, &next, &runtime);
        BIND(&next);
      }

      Branch(TaggedIsPositiveSmi(var_limit.value()), &next, &if_limitissmimax);
    }

    BIND(&if_limitissmimax);
    {
      // TODO(jgruber): In this case, we can probably avoid generation of limit
      // checks in Generate_RegExpPrototypeSplitBody.
      var_limit.Bind(SmiConstant(Smi::kMaxValue));
      Goto(&next);
    }

    BIND(&next);
  }

  // Due to specific shortcuts we take on the fast path (specifically, we don't
  // allocate a new regexp instance as specced), we need to ensure that the
  // given regexp is non-sticky to avoid invalid results. See crbug.com/v8/6706.

  GotoIf(FastFlagGetter(regexp, JSRegExp::kSticky), &runtime);

  // We're good to go on the fast path, which is inlined here.

  RegExpPrototypeSplitBody(context, regexp, string, var_limit.value());

  BIND(&runtime);
  {
    // The runtime call passes in limit to ensure the second ToUint32(limit)
    // call is not observable.
    CSA_ASSERT(this, IsNumber(var_limit.value()));
    Return(CallRuntime(Runtime::kRegExpSplit, context, regexp, string,
                       var_limit.value()));
  }
}

// ES#sec-regexp.prototype-@@split
// RegExp.prototype [ @@split ] ( string, limit )
TF_BUILTIN(RegExpPrototypeSplit, RegExpBuiltinsAssembler) {
  const int kStringArg = 0;
  const int kLimitArg = 1;

  Node* argc =
      ChangeInt32ToIntPtr(Parameter(BuiltinDescriptor::kArgumentsCount));
  CodeStubArguments args(this, argc);

  Node* const maybe_receiver = args.GetReceiver();
  Node* const maybe_string = args.GetOptionalArgumentValue(kStringArg);
  Node* const maybe_limit = args.GetOptionalArgumentValue(kLimitArg);
  Node* const context = Parameter(BuiltinDescriptor::kContext);

  // Ensure {maybe_receiver} is a JSReceiver.
  ThrowIfNotJSReceiver(context, maybe_receiver,
                       MessageTemplate::kIncompatibleMethodReceiver,
                       "RegExp.prototype.@@split");
  Node* const receiver = maybe_receiver;

  // Convert {maybe_string} to a String.
  Node* const string = ToString_Inline(context, maybe_string);

  Label stub(this), runtime(this, Label::kDeferred);
  BranchIfFastRegExp(context, receiver, &stub, &runtime);

  BIND(&stub);
  args.PopAndReturn(CallBuiltin(Builtins::kRegExpSplit, context, receiver,
                                string, maybe_limit));

  BIND(&runtime);
  args.PopAndReturn(CallRuntime(Runtime::kRegExpSplit, context, receiver,
                                string, maybe_limit));
}

Node* RegExpBuiltinsAssembler::ReplaceGlobalCallableFastPath(
    Node* context, Node* regexp, Node* string, Node* replace_callable) {
  // The fast path is reached only if {receiver} is a global unmodified
  // JSRegExp instance and {replace_callable} is callable.

  CSA_ASSERT(this, IsFastRegExp(context, regexp));
  CSA_ASSERT(this, IsCallable(replace_callable));
  CSA_ASSERT(this, IsString(string));

  Isolate* const isolate = this->isolate();

  Node* const null = NullConstant();
  Node* const undefined = UndefinedConstant();
  Node* const int_zero = IntPtrConstant(0);
  Node* const int_one = IntPtrConstant(1);
  Node* const smi_zero = SmiConstant(0);

  Node* const native_context = LoadNativeContext(context);

  Label out(this);
  VARIABLE(var_result, MachineRepresentation::kTagged);

  // Set last index to 0.
  FastStoreLastIndex(regexp, smi_zero);

  // Allocate {result_array}.
  Node* result_array;
  {
    ElementsKind kind = PACKED_ELEMENTS;
    Node* const array_map = LoadJSArrayElementsMap(kind, native_context);
    Node* const capacity = IntPtrConstant(16);
    Node* const length = smi_zero;
    Node* const allocation_site = nullptr;
    ParameterMode capacity_mode = CodeStubAssembler::INTPTR_PARAMETERS;

    result_array = AllocateJSArray(kind, array_map, capacity, length,
                                   allocation_site, capacity_mode);
  }

  // Call into runtime for RegExpExecMultiple.
  Node* last_match_info =
      LoadContextElement(native_context, Context::REGEXP_LAST_MATCH_INFO_INDEX);
  Node* const res = CallRuntime(Runtime::kRegExpExecMultiple, context, regexp,
                                string, last_match_info, result_array);

  // Reset last index to 0.
  FastStoreLastIndex(regexp, smi_zero);

  // If no matches, return the subject string.
  var_result.Bind(string);
  GotoIf(WordEqual(res, null), &out);

  // Reload last match info since it might have changed.
  last_match_info =
      LoadContextElement(native_context, Context::REGEXP_LAST_MATCH_INFO_INDEX);

  Node* const res_length = LoadJSArrayLength(res);
  Node* const res_elems = LoadElements(res);
  CSA_ASSERT(this, HasInstanceType(res_elems, FIXED_ARRAY_TYPE));

  Node* const num_capture_registers = LoadFixedArrayElement(
      last_match_info, RegExpMatchInfo::kNumberOfCapturesIndex);

  Label if_hasexplicitcaptures(this), if_noexplicitcaptures(this),
      create_result(this);
  Branch(SmiEqual(num_capture_registers, SmiConstant(2)),
         &if_noexplicitcaptures, &if_hasexplicitcaptures);

  BIND(&if_noexplicitcaptures);
  {
    // If the number of captures is two then there are no explicit captures in
    // the regexp, just the implicit capture that captures the whole match. In
    // this case we can simplify quite a bit and end up with something faster.
    // The builder will consist of some integers that indicate slices of the
    // input string and some replacements that were returned from the replace
    // function.

    VARIABLE(var_match_start, MachineRepresentation::kTagged);
    var_match_start.Bind(smi_zero);

    Node* const end = SmiUntag(res_length);
    VARIABLE(var_i, MachineType::PointerRepresentation());
    var_i.Bind(int_zero);

    Variable* vars[] = {&var_i, &var_match_start};
    Label loop(this, 2, vars);
    Goto(&loop);
    BIND(&loop);
    {
      Node* const i = var_i.value();
      GotoIfNot(IntPtrLessThan(i, end), &create_result);

      Node* const elem = LoadFixedArrayElement(res_elems, i);

      Label if_issmi(this), if_isstring(this), loop_epilogue(this);
      Branch(TaggedIsSmi(elem), &if_issmi, &if_isstring);

      BIND(&if_issmi);
      {
        // Integers represent slices of the original string.
        Label if_isnegativeorzero(this), if_ispositive(this);
        BranchIfSmiLessThanOrEqual(elem, smi_zero, &if_isnegativeorzero,
                                   &if_ispositive);

        BIND(&if_ispositive);
        {
          TNode<IntPtrT> int_elem = SmiUntag(elem);
          TNode<IntPtrT> new_match_start =
              Signed(IntPtrAdd(WordShr(int_elem, IntPtrConstant(11)),
                               WordAnd(int_elem, IntPtrConstant(0x7ff))));
          var_match_start.Bind(SmiTag(new_match_start));
          Goto(&loop_epilogue);
        }

        BIND(&if_isnegativeorzero);
        {
          Node* const next_i = IntPtrAdd(i, int_one);
          var_i.Bind(next_i);

          Node* const next_elem = LoadFixedArrayElement(res_elems, next_i);

          Node* const new_match_start = SmiSub(next_elem, elem);
          var_match_start.Bind(new_match_start);
          Goto(&loop_epilogue);
        }
      }

      BIND(&if_isstring);
      {
        CSA_ASSERT(this, IsString(elem));

        Callable call_callable = CodeFactory::Call(isolate);
        Node* const replacement_obj =
            CallJS(call_callable, context, replace_callable, undefined, elem,
                   var_match_start.value(), string);

        Node* const replacement_str = ToString_Inline(context, replacement_obj);
        StoreFixedArrayElement(res_elems, i, replacement_str);

        Node* const elem_length = LoadStringLength(elem);
        Node* const new_match_start =
            SmiAdd(var_match_start.value(), elem_length);
        var_match_start.Bind(new_match_start);

        Goto(&loop_epilogue);
      }

      BIND(&loop_epilogue);
      {
        var_i.Bind(IntPtrAdd(var_i.value(), int_one));
        Goto(&loop);
      }
    }
  }

  BIND(&if_hasexplicitcaptures);
  {
    Node* const from = int_zero;
    Node* const to = SmiUntag(res_length);
    const int increment = 1;

    BuildFastLoop(from, to,
                  [this, res_elems, isolate, native_context, context, undefined,
                   replace_callable](Node* index) {
                    Node* const elem = LoadFixedArrayElement(res_elems, index);

                    Label do_continue(this);
                    GotoIf(TaggedIsSmi(elem), &do_continue);

                    // elem must be an Array.
                    // Use the apply argument as backing for global RegExp
                    // properties.

                    CSA_ASSERT(this, HasInstanceType(elem, JS_ARRAY_TYPE));

                    // TODO(jgruber): Remove indirection through
                    // Call->ReflectApply.
                    Callable call_callable = CodeFactory::Call(isolate);
                    Node* const reflect_apply = LoadContextElement(
                        native_context, Context::REFLECT_APPLY_INDEX);

                    Node* const replacement_obj =
                        CallJS(call_callable, context, reflect_apply, undefined,
                               replace_callable, undefined, elem);

                    // Overwrite the i'th element in the results with the string
                    // we got back from the callback function.

                    Node* const replacement_str =
                        ToString_Inline(context, replacement_obj);
                    StoreFixedArrayElement(res_elems, index, replacement_str);

                    Goto(&do_continue);
                    BIND(&do_continue);
                  },
                  increment, CodeStubAssembler::INTPTR_PARAMETERS,
                  CodeStubAssembler::IndexAdvanceMode::kPost);

    Goto(&create_result);
  }

  BIND(&create_result);
  {
    Node* const result = CallRuntime(Runtime::kStringBuilderConcat, context,
                                     res, res_length, string);
    var_result.Bind(result);
    Goto(&out);
  }

  BIND(&out);
  return var_result.value();
}

Node* RegExpBuiltinsAssembler::ReplaceSimpleStringFastPath(
    Node* context, Node* regexp, Node* string, Node* replace_string) {
  // The fast path is reached only if {receiver} is an unmodified
  // JSRegExp instance, {replace_value} is non-callable, and
  // ToString({replace_value}) does not contain '$', i.e. we're doing a simple
  // string replacement.

  Node* const int_zero = IntPtrConstant(0);
  Node* const smi_zero = SmiConstant(0);

  CSA_ASSERT(this, IsFastRegExp(context, regexp));
  CSA_ASSERT(this, IsString(replace_string));
  CSA_ASSERT(this, IsString(string));

  Label out(this);
  VARIABLE(var_result, MachineRepresentation::kTagged);

  // Load the last match info.
  Node* const native_context = LoadNativeContext(context);
  Node* const last_match_info =
      LoadContextElement(native_context, Context::REGEXP_LAST_MATCH_INFO_INDEX);

  // Is {regexp} global?
  Label if_isglobal(this), if_isnonglobal(this);
  Node* const flags = LoadObjectField(regexp, JSRegExp::kFlagsOffset);
  Node* const is_global =
      WordAnd(SmiUntag(flags), IntPtrConstant(JSRegExp::kGlobal));
  Branch(WordEqual(is_global, int_zero), &if_isnonglobal, &if_isglobal);

  BIND(&if_isglobal);
  {
    // Hand off global regexps to runtime.
    FastStoreLastIndex(regexp, smi_zero);
    Node* const result =
        CallRuntime(Runtime::kStringReplaceGlobalRegExpWithString, context,
                    string, regexp, replace_string, last_match_info);
    var_result.Bind(result);
    Goto(&out);
  }

  BIND(&if_isnonglobal);
  {
    // Run exec, then manually construct the resulting string.
    Label if_didnotmatch(this);
    Node* const match_indices = RegExpPrototypeExecBodyWithoutResult(
        context, regexp, string, &if_didnotmatch, true);

    // Successful match.
    {
      Node* const subject_start = smi_zero;
      Node* const match_start = LoadFixedArrayElement(
          match_indices, RegExpMatchInfo::kFirstCaptureIndex);
      Node* const match_end = LoadFixedArrayElement(
          match_indices, RegExpMatchInfo::kFirstCaptureIndex + 1);
      Node* const subject_end = LoadStringLength(string);

      Label if_replaceisempty(this), if_replaceisnotempty(this);
      Node* const replace_length = LoadStringLength(replace_string);
      Branch(SmiEqual(replace_length, smi_zero), &if_replaceisempty,
             &if_replaceisnotempty);

      BIND(&if_replaceisempty);
      {
        // TODO(jgruber): We could skip many of the checks that using SubString
        // here entails.

        Node* const first_part =
            SubString(context, string, subject_start, match_start);
        Node* const second_part =
            SubString(context, string, match_end, subject_end);

        Node* const result = StringAdd(context, first_part, second_part);
        var_result.Bind(result);
        Goto(&out);
      }

      BIND(&if_replaceisnotempty);
      {
        Node* const first_part =
            SubString(context, string, subject_start, match_start);
        Node* const second_part = replace_string;
        Node* const third_part =
            SubString(context, string, match_end, subject_end);

        Node* result = StringAdd(context, first_part, second_part);
        result = StringAdd(context, result, third_part);

        var_result.Bind(result);
        Goto(&out);
      }
    }

    BIND(&if_didnotmatch);
    {
      var_result.Bind(string);
      Goto(&out);
    }
  }

  BIND(&out);
  return var_result.value();
}

// Helper that skips a few initial checks.
TF_BUILTIN(RegExpReplace, RegExpBuiltinsAssembler) {
  Node* const regexp = Parameter(Descriptor::kRegExp);
  Node* const string = Parameter(Descriptor::kString);
  Node* const replace_value = Parameter(Descriptor::kReplaceValue);
  Node* const context = Parameter(Descriptor::kContext);

  CSA_ASSERT(this, IsFastRegExp(context, regexp));
  CSA_ASSERT(this, IsString(string));

  Label checkreplacestring(this), if_iscallable(this),
      runtime(this, Label::kDeferred);

  // 2. Is {replace_value} callable?
  GotoIf(TaggedIsSmi(replace_value), &checkreplacestring);
  Branch(IsCallableMap(LoadMap(replace_value)), &if_iscallable,
         &checkreplacestring);

  // 3. Does ToString({replace_value}) contain '$'?
  BIND(&checkreplacestring);
  {
    Node* const replace_string = ToString_Inline(context, replace_value);

    // ToString(replaceValue) could potentially change the shape of the RegExp
    // object. Recheck that we are still on the fast path and bail to runtime
    // otherwise.
    {
      Label next(this);
      BranchIfFastRegExp(context, regexp, &next, &runtime);
      BIND(&next);
    }

    Node* const dollar_string = HeapConstant(
        isolate()->factory()->LookupSingleCharacterStringFromCode('$'));
    Node* const dollar_ix =
        CallBuiltin(Builtins::kStringIndexOf, context, replace_string,
                    dollar_string, SmiConstant(0));
    GotoIfNot(SmiEqual(dollar_ix, SmiConstant(-1)), &runtime);

    Return(
        ReplaceSimpleStringFastPath(context, regexp, string, replace_string));
  }

  // {regexp} is unmodified and {replace_value} is callable.
  BIND(&if_iscallable);
  {
    Node* const replace_fn = replace_value;

    // Check if the {regexp} is global.
    Label if_isglobal(this), if_isnotglobal(this);

    Node* const is_global = FastFlagGetter(regexp, JSRegExp::kGlobal);
    Branch(is_global, &if_isglobal, &if_isnotglobal);

    BIND(&if_isglobal);
    Return(ReplaceGlobalCallableFastPath(context, regexp, string, replace_fn));

    BIND(&if_isnotglobal);
    Return(CallRuntime(Runtime::kStringReplaceNonGlobalRegExpWithFunction,
                       context, string, regexp, replace_fn));
  }

  BIND(&runtime);
  Return(CallRuntime(Runtime::kRegExpReplace, context, regexp, string,
                     replace_value));
}

// ES#sec-regexp.prototype-@@replace
// RegExp.prototype [ @@replace ] ( string, replaceValue )
TF_BUILTIN(RegExpPrototypeReplace, RegExpBuiltinsAssembler) {
  const int kStringArg = 0;
  const int kReplaceValueArg = 1;

  Node* argc =
      ChangeInt32ToIntPtr(Parameter(BuiltinDescriptor::kArgumentsCount));
  CodeStubArguments args(this, argc);

  Node* const maybe_receiver = args.GetReceiver();
  Node* const maybe_string = args.GetOptionalArgumentValue(kStringArg);
  Node* const replace_value = args.GetOptionalArgumentValue(kReplaceValueArg);
  Node* const context = Parameter(BuiltinDescriptor::kContext);

  // RegExpPrototypeReplace is a bit of a beast - a summary of dispatch logic:
  //
  // if (!IsFastRegExp(receiver)) CallRuntime(RegExpReplace)
  // if (IsCallable(replace)) {
  //   if (IsGlobal(receiver)) {
  //     // Called 'fast-path' but contains several runtime calls.
  //     ReplaceGlobalCallableFastPath()
  //   } else {
  //     CallRuntime(StringReplaceNonGlobalRegExpWithFunction)
  //   }
  // } else {
  //   if (replace.contains("$")) {
  //     CallRuntime(RegExpReplace)
  //   } else {
  //     ReplaceSimpleStringFastPath()  // Bails to runtime for global regexps.
  //   }
  // }

  // Ensure {maybe_receiver} is a JSReceiver.
  ThrowIfNotJSReceiver(context, maybe_receiver,
                       MessageTemplate::kIncompatibleMethodReceiver,
                       "RegExp.prototype.@@replace");
  Node* const receiver = maybe_receiver;

  // Convert {maybe_string} to a String.
  Node* const string = ToString_Inline(context, maybe_string);

  // Fast-path checks: 1. Is the {receiver} an unmodified JSRegExp instance?
  Label stub(this), runtime(this, Label::kDeferred);
  BranchIfFastRegExp(context, receiver, &stub, &runtime);

  BIND(&stub);
  args.PopAndReturn(CallBuiltin(Builtins::kRegExpReplace, context, receiver,
                                string, replace_value));

  BIND(&runtime);
  args.PopAndReturn(CallRuntime(Runtime::kRegExpReplace, context, receiver,
                                string, replace_value));
}

// Simple string matching functionality for internal use which does not modify
// the last match info.
TF_BUILTIN(RegExpInternalMatch, RegExpBuiltinsAssembler) {
  Node* const regexp = Parameter(Descriptor::kRegExp);
  Node* const string = Parameter(Descriptor::kString);
  Node* const context = Parameter(Descriptor::kContext);

  Node* const null = NullConstant();
  Node* const smi_zero = SmiConstant(0);

  CSA_ASSERT(this, IsJSRegExp(regexp));
  CSA_ASSERT(this, IsString(string));

  Node* const native_context = LoadNativeContext(context);
  Node* const internal_match_info = LoadContextElement(
      native_context, Context::REGEXP_INTERNAL_MATCH_INFO_INDEX);

  Node* const match_indices = RegExpExecInternal(context, regexp, string,
                                                 smi_zero, internal_match_info);

  Label if_matched(this), if_didnotmatch(this);
  Branch(WordEqual(match_indices, null), &if_didnotmatch, &if_matched);

  BIND(&if_didnotmatch);
  Return(null);

  BIND(&if_matched);
  {
    Node* result =
        ConstructNewResultFromMatchInfo(context, regexp, match_indices, string);
    Return(result);
  }
}

}  // namespace internal
}  // namespace v8
