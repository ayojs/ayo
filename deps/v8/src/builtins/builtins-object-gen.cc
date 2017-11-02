// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/builtins/builtins-utils-gen.h"
#include "src/builtins/builtins.h"
#include "src/code-stub-assembler.h"

namespace v8 {
namespace internal {

// -----------------------------------------------------------------------------
// ES6 section 19.1 Object Objects

typedef compiler::Node Node;

class ObjectBuiltinsAssembler : public CodeStubAssembler {
 public:
  explicit ObjectBuiltinsAssembler(compiler::CodeAssemblerState* state)
      : CodeStubAssembler(state) {}

 protected:
  void ReturnToStringFormat(Node* context, Node* string);
};

void ObjectBuiltinsAssembler::ReturnToStringFormat(Node* context,
                                                   Node* string) {
  Node* lhs = StringConstant("[object ");
  Node* rhs = StringConstant("]");

  Callable callable =
      CodeFactory::StringAdd(isolate(), STRING_ADD_CHECK_NONE, NOT_TENURED);

  Return(CallStub(callable, context, CallStub(callable, context, lhs, string),
                  rhs));
}

TF_BUILTIN(ObjectPrototypeHasOwnProperty, ObjectBuiltinsAssembler) {
  Node* object = Parameter(Descriptor::kReceiver);
  Node* key = Parameter(Descriptor::kKey);
  Node* context = Parameter(Descriptor::kContext);

  Label call_runtime(this), return_true(this), return_false(this),
      to_primitive(this);

  // Smi receivers do not have own properties, just perform ToPrimitive on the
  // key.
  Label if_objectisnotsmi(this);
  Branch(TaggedIsSmi(object), &to_primitive, &if_objectisnotsmi);
  BIND(&if_objectisnotsmi);

  Node* map = LoadMap(object);
  Node* instance_type = LoadMapInstanceType(map);

  {
    VARIABLE(var_index, MachineType::PointerRepresentation());
    VARIABLE(var_unique, MachineRepresentation::kTagged);

    Label if_index(this), if_unique_name(this), if_notunique_name(this);
    TryToName(key, &if_index, &var_index, &if_unique_name, &var_unique,
              &call_runtime, &if_notunique_name);

    BIND(&if_unique_name);
    TryHasOwnProperty(object, map, instance_type, var_unique.value(),
                      &return_true, &return_false, &call_runtime);

    BIND(&if_index);
    {
      // Handle negative keys in the runtime.
      GotoIf(IntPtrLessThan(var_index.value(), IntPtrConstant(0)),
             &call_runtime);
      TryLookupElement(object, map, instance_type, var_index.value(),
                       &return_true, &return_false, &return_false,
                       &call_runtime);
    }

    BIND(&if_notunique_name);
    {
      Label not_in_string_table(this);
      TryInternalizeString(key, &if_index, &var_index, &if_unique_name,
                           &var_unique, &not_in_string_table, &call_runtime);

      BIND(&not_in_string_table);
      {
        // If the string was not found in the string table, then no regular
        // object can have a property with that name, so return |false|.
        // "Special API objects" with interceptors must take the slow path.
        Branch(IsSpecialReceiverInstanceType(instance_type), &call_runtime,
               &return_false);
      }
    }
  }
  BIND(&to_primitive);
  GotoIf(IsNumber(key), &return_false);
  Branch(IsName(key), &return_false, &call_runtime);

  BIND(&return_true);
  Return(BooleanConstant(true));

  BIND(&return_false);
  Return(BooleanConstant(false));

  BIND(&call_runtime);
  Return(CallRuntime(Runtime::kObjectHasOwnProperty, context, object, key));
}

// ES #sec-object.keys
TF_BUILTIN(ObjectKeys, ObjectBuiltinsAssembler) {
  Node* object = Parameter(Descriptor::kObject);
  Node* context = Parameter(Descriptor::kContext);

  VARIABLE(var_length, MachineRepresentation::kTagged);
  VARIABLE(var_elements, MachineRepresentation::kTagged);
  Label if_empty(this, Label::kDeferred), if_fast(this),
      if_slow(this, Label::kDeferred), if_join(this);

  // Check if the {object} has a usable enum cache.
  GotoIf(TaggedIsSmi(object), &if_slow);
  Node* object_map = LoadMap(object);
  Node* object_bit_field3 = LoadMapBitField3(object_map);
  Node* object_enum_length =
      DecodeWordFromWord32<Map::EnumLengthBits>(object_bit_field3);
  GotoIf(
      WordEqual(object_enum_length, IntPtrConstant(kInvalidEnumCacheSentinel)),
      &if_slow);

  // Ensure that the {object} doesn't have any elements.
  CSA_ASSERT(this, IsJSObjectMap(object_map));
  Node* object_elements = LoadObjectField(object, JSObject::kElementsOffset);
  GotoIfNot(IsEmptyFixedArray(object_elements), &if_slow);
  Branch(WordEqual(object_enum_length, IntPtrConstant(0)), &if_empty, &if_fast);

  BIND(&if_fast);
  {
    // The {object} has a usable enum cache, use that.
    Node* object_descriptors = LoadMapDescriptors(object_map);
    Node* object_enum_cache_bridge = LoadObjectField(
        object_descriptors, DescriptorArray::kEnumCacheBridgeOffset);
    Node* object_enum_cache = LoadObjectField(
        object_enum_cache_bridge, DescriptorArray::kEnumCacheBridgeCacheOffset);

    // Allocate a JSArray and copy the elements from the {object_enum_cache}.
    Node* array = nullptr;
    Node* elements = nullptr;
    Node* native_context = LoadNativeContext(context);
    Node* array_map = LoadJSArrayElementsMap(PACKED_ELEMENTS, native_context);
    Node* array_length = SmiTag(object_enum_length);
    std::tie(array, elements) = AllocateUninitializedJSArrayWithElements(
        PACKED_ELEMENTS, array_map, array_length, nullptr, object_enum_length,
        INTPTR_PARAMETERS);
    CopyFixedArrayElements(PACKED_ELEMENTS, object_enum_cache, elements,
                           object_enum_length, SKIP_WRITE_BARRIER);
    Return(array);
  }

  BIND(&if_empty);
  {
    // The {object} doesn't have any enumerable keys.
    var_length.Bind(SmiConstant(0));
    var_elements.Bind(EmptyFixedArrayConstant());
    Goto(&if_join);
  }

  BIND(&if_slow);
  {
    // Let the runtime compute the elements.
    Node* elements = CallRuntime(Runtime::kObjectKeys, context, object);
    var_length.Bind(LoadObjectField(elements, FixedArray::kLengthOffset));
    var_elements.Bind(elements);
    Goto(&if_join);
  }

  BIND(&if_join);
  {
    // Wrap the elements into a proper JSArray and return that.
    Node* native_context = LoadNativeContext(context);
    Node* array_map = LoadJSArrayElementsMap(PACKED_ELEMENTS, native_context);
    Node* array = AllocateUninitializedJSArrayWithoutElements(
        PACKED_ELEMENTS, array_map, var_length.value(), nullptr);
    StoreObjectFieldNoWriteBarrier(array, JSArray::kElementsOffset,
                                   var_elements.value());
    Return(array);
  }
}

// ES #sec-object.prototype.isprototypeof
TF_BUILTIN(ObjectPrototypeIsPrototypeOf, ObjectBuiltinsAssembler) {
  Node* receiver = Parameter(Descriptor::kReceiver);
  Node* value = Parameter(Descriptor::kValue);
  Node* context = Parameter(Descriptor::kContext);
  Label if_receiverisnullorundefined(this, Label::kDeferred),
      if_valueisnotreceiver(this, Label::kDeferred);

  // We only check whether {value} is a Smi here, so that the
  // prototype chain walk below can safely access the {value}s
  // map. We don't rule out Primitive {value}s, since all of
  // them have null as their prototype, so the chain walk below
  // immediately aborts and returns false anyways.
  GotoIf(TaggedIsSmi(value), &if_valueisnotreceiver);

  // Check if {receiver} is either null or undefined and in that case,
  // invoke the ToObject builtin, which raises the appropriate error.
  // Otherwise we don't need to invoke ToObject, since {receiver} is
  // either already a JSReceiver, in which case ToObject is a no-op,
  // or it's a Primitive and ToObject would allocate a fresh JSValue
  // wrapper, which wouldn't be identical to any existing JSReceiver
  // found in the prototype chain of {value}, hence it will return
  // false no matter if we search for the Primitive {receiver} or
  // a newly allocated JSValue wrapper for {receiver}.
  GotoIf(IsNull(receiver), &if_receiverisnullorundefined);
  GotoIf(IsUndefined(receiver), &if_receiverisnullorundefined);

  // Loop through the prototype chain looking for the {receiver}.
  Return(HasInPrototypeChain(context, value, receiver));

  BIND(&if_receiverisnullorundefined);
  {
    // If {value} is a primitive HeapObject, we need to return
    // false instead of throwing an exception per order of the
    // steps in the specification, so check that first here.
    GotoIfNot(IsJSReceiver(value), &if_valueisnotreceiver);

    // Simulate the ToObject invocation on {receiver}.
    CallBuiltin(Builtins::kToObject, context, receiver);
    Unreachable();
  }

  BIND(&if_valueisnotreceiver);
  Return(FalseConstant());
}

// ES #sec-object.prototype.tostring
TF_BUILTIN(ObjectPrototypeToString, ObjectBuiltinsAssembler) {
  Label checkstringtag(this), if_apiobject(this, Label::kDeferred),
      if_arguments(this), if_array(this), if_boolean(this), if_date(this),
      if_error(this), if_function(this), if_number(this, Label::kDeferred),
      if_object(this), if_primitive(this), if_proxy(this, Label::kDeferred),
      if_regexp(this), if_string(this), if_symbol(this, Label::kDeferred),
      if_value(this);

  Node* receiver = Parameter(Descriptor::kReceiver);
  Node* context = Parameter(Descriptor::kContext);

  // This is arranged to check the likely cases first.
  VARIABLE(var_default, MachineRepresentation::kTagged);
  VARIABLE(var_holder, MachineRepresentation::kTagged, receiver);
  GotoIf(TaggedIsSmi(receiver), &if_number);
  Node* receiver_map = LoadMap(receiver);
  Node* receiver_instance_type = LoadMapInstanceType(receiver_map);
  GotoIf(IsPrimitiveInstanceType(receiver_instance_type), &if_primitive);
  const struct {
    InstanceType value;
    Label* label;
  } kJumpTable[] = {{JS_OBJECT_TYPE, &if_object},
                    {JS_ARRAY_TYPE, &if_array},
                    {JS_FUNCTION_TYPE, &if_function},
                    {JS_REGEXP_TYPE, &if_regexp},
                    {JS_ARGUMENTS_TYPE, &if_arguments},
                    {JS_DATE_TYPE, &if_date},
                    {JS_BOUND_FUNCTION_TYPE, &if_function},
                    {JS_API_OBJECT_TYPE, &if_apiobject},
                    {JS_SPECIAL_API_OBJECT_TYPE, &if_apiobject},
                    {JS_PROXY_TYPE, &if_proxy},
                    {JS_ERROR_TYPE, &if_error},
                    {JS_VALUE_TYPE, &if_value}};
  size_t const kNumCases = arraysize(kJumpTable);
  Label* case_labels[kNumCases];
  int32_t case_values[kNumCases];
  for (size_t i = 0; i < kNumCases; ++i) {
    case_labels[i] = kJumpTable[i].label;
    case_values[i] = kJumpTable[i].value;
  }
  Switch(receiver_instance_type, &if_object, case_values, case_labels,
         arraysize(case_values));

  BIND(&if_apiobject);
  {
    // Lookup the @@toStringTag property on the {receiver}.
    VARIABLE(var_tag, MachineRepresentation::kTagged,
             GetProperty(context, receiver,
                         isolate()->factory()->to_string_tag_symbol()));
    Label if_tagisnotstring(this), if_tagisstring(this);
    GotoIf(TaggedIsSmi(var_tag.value()), &if_tagisnotstring);
    Branch(IsString(var_tag.value()), &if_tagisstring, &if_tagisnotstring);
    BIND(&if_tagisnotstring);
    {
      var_tag.Bind(
          CallStub(Builtins::CallableFor(isolate(), Builtins::kClassOf),
                   context, receiver));
      Goto(&if_tagisstring);
    }
    BIND(&if_tagisstring);
    ReturnToStringFormat(context, var_tag.value());
  }

  BIND(&if_arguments);
  {
    var_default.Bind(LoadRoot(Heap::karguments_to_stringRootIndex));
    Goto(&checkstringtag);
  }

  BIND(&if_array);
  {
    var_default.Bind(LoadRoot(Heap::karray_to_stringRootIndex));
    Goto(&checkstringtag);
  }

  BIND(&if_boolean);
  {
    Node* native_context = LoadNativeContext(context);
    Node* boolean_constructor =
        LoadContextElement(native_context, Context::BOOLEAN_FUNCTION_INDEX);
    Node* boolean_initial_map = LoadObjectField(
        boolean_constructor, JSFunction::kPrototypeOrInitialMapOffset);
    Node* boolean_prototype =
        LoadObjectField(boolean_initial_map, Map::kPrototypeOffset);
    var_default.Bind(LoadRoot(Heap::kboolean_to_stringRootIndex));
    var_holder.Bind(boolean_prototype);
    Goto(&checkstringtag);
  }

  BIND(&if_date);
  {
    var_default.Bind(LoadRoot(Heap::kdate_to_stringRootIndex));
    Goto(&checkstringtag);
  }

  BIND(&if_error);
  {
    var_default.Bind(LoadRoot(Heap::kerror_to_stringRootIndex));
    Goto(&checkstringtag);
  }

  BIND(&if_function);
  {
    var_default.Bind(LoadRoot(Heap::kfunction_to_stringRootIndex));
    Goto(&checkstringtag);
  }

  BIND(&if_number);
  {
    Node* native_context = LoadNativeContext(context);
    Node* number_constructor =
        LoadContextElement(native_context, Context::NUMBER_FUNCTION_INDEX);
    Node* number_initial_map = LoadObjectField(
        number_constructor, JSFunction::kPrototypeOrInitialMapOffset);
    Node* number_prototype =
        LoadObjectField(number_initial_map, Map::kPrototypeOffset);
    var_default.Bind(LoadRoot(Heap::knumber_to_stringRootIndex));
    var_holder.Bind(number_prototype);
    Goto(&checkstringtag);
  }

  BIND(&if_object);
  {
    CSA_ASSERT(this, IsJSReceiver(receiver));
    var_default.Bind(LoadRoot(Heap::kobject_to_stringRootIndex));
    Goto(&checkstringtag);
  }

  BIND(&if_primitive);
  {
    Label return_null(this), return_undefined(this);

    GotoIf(IsStringInstanceType(receiver_instance_type), &if_string);
    GotoIf(IsBooleanMap(receiver_map), &if_boolean);
    GotoIf(IsHeapNumberMap(receiver_map), &if_number);
    GotoIf(IsSymbolMap(receiver_map), &if_symbol);
    Branch(IsUndefined(receiver), &return_undefined, &return_null);

    BIND(&return_undefined);
    Return(LoadRoot(Heap::kundefined_to_stringRootIndex));

    BIND(&return_null);
    Return(LoadRoot(Heap::knull_to_stringRootIndex));
  }

  BIND(&if_proxy);
  {
    // If {receiver} is a proxy for a JSArray, we default to "[object Array]",
    // otherwise we default to "[object Object]" or "[object Function]" here,
    // depending on whether the {receiver} is callable. The order matters here,
    // i.e. we need to execute the %ArrayIsArray check before the [[Get]] below,
    // as the exception is observable.
    Node* receiver_is_array =
        CallRuntime(Runtime::kArrayIsArray, context, receiver);
    Node* builtin_tag = SelectTaggedConstant<Object>(
        IsTrue(receiver_is_array), LoadRoot(Heap::kArray_stringRootIndex),
        SelectTaggedConstant<Object>(IsCallableMap(receiver_map),
                                     LoadRoot(Heap::kFunction_stringRootIndex),
                                     LoadRoot(Heap::kObject_stringRootIndex)));

    // Lookup the @@toStringTag property on the {receiver}.
    VARIABLE(var_tag, MachineRepresentation::kTagged,
             GetProperty(context, receiver,
                         isolate()->factory()->to_string_tag_symbol()));
    Label if_tagisnotstring(this), if_tagisstring(this);
    GotoIf(TaggedIsSmi(var_tag.value()), &if_tagisnotstring);
    Branch(IsString(var_tag.value()), &if_tagisstring, &if_tagisnotstring);
    BIND(&if_tagisnotstring);
    {
      var_tag.Bind(builtin_tag);
      Goto(&if_tagisstring);
    }
    BIND(&if_tagisstring);
    ReturnToStringFormat(context, var_tag.value());
  }

  BIND(&if_regexp);
  {
    var_default.Bind(LoadRoot(Heap::kregexp_to_stringRootIndex));
    Goto(&checkstringtag);
  }

  BIND(&if_string);
  {
    Node* native_context = LoadNativeContext(context);
    Node* string_constructor =
        LoadContextElement(native_context, Context::STRING_FUNCTION_INDEX);
    Node* string_initial_map = LoadObjectField(
        string_constructor, JSFunction::kPrototypeOrInitialMapOffset);
    Node* string_prototype =
        LoadObjectField(string_initial_map, Map::kPrototypeOffset);
    var_default.Bind(LoadRoot(Heap::kstring_to_stringRootIndex));
    var_holder.Bind(string_prototype);
    Goto(&checkstringtag);
  }

  BIND(&if_symbol);
  {
    Node* native_context = LoadNativeContext(context);
    Node* symbol_constructor =
        LoadContextElement(native_context, Context::SYMBOL_FUNCTION_INDEX);
    Node* symbol_initial_map = LoadObjectField(
        symbol_constructor, JSFunction::kPrototypeOrInitialMapOffset);
    Node* symbol_prototype =
        LoadObjectField(symbol_initial_map, Map::kPrototypeOffset);
    var_default.Bind(LoadRoot(Heap::kobject_to_stringRootIndex));
    var_holder.Bind(symbol_prototype);
    Goto(&checkstringtag);
  }

  BIND(&if_value);
  {
    Node* receiver_value = LoadJSValueValue(receiver);
    GotoIf(TaggedIsSmi(receiver_value), &if_number);
    Node* receiver_value_map = LoadMap(receiver_value);
    GotoIf(IsHeapNumberMap(receiver_value_map), &if_number);
    GotoIf(IsBooleanMap(receiver_value_map), &if_boolean);
    Branch(IsSymbolMap(receiver_value_map), &if_symbol, &if_string);
  }

  BIND(&checkstringtag);
  {
    // Check if all relevant maps (including the prototype maps) don't
    // have any interesting symbols (i.e. that none of them have the
    // @@toStringTag property).
    Label loop(this, &var_holder), return_default(this),
        return_generic(this, Label::kDeferred);
    Goto(&loop);
    BIND(&loop);
    {
      Node* holder = var_holder.value();
      GotoIf(IsNull(holder), &return_default);
      Node* holder_map = LoadMap(holder);
      Node* holder_bit_field3 = LoadMapBitField3(holder_map);
      GotoIf(IsSetWord32<Map::MayHaveInterestingSymbols>(holder_bit_field3),
             &return_generic);
      var_holder.Bind(LoadMapPrototype(holder_map));
      Goto(&loop);
    }

    BIND(&return_generic);
    {
      Node* tag = GetProperty(
          context, CallBuiltin(Builtins::kToObject, context, receiver),
          LoadRoot(Heap::kto_string_tag_symbolRootIndex));
      GotoIf(TaggedIsSmi(tag), &return_default);
      GotoIfNot(IsString(tag), &return_default);
      ReturnToStringFormat(context, tag);
    }

    BIND(&return_default);
    Return(var_default.value());
  }
}

// ES6 #sec-object.prototype.valueof
TF_BUILTIN(ObjectPrototypeValueOf, CodeStubAssembler) {
  Node* receiver = Parameter(Descriptor::kReceiver);
  Node* context = Parameter(Descriptor::kContext);

  Return(CallBuiltin(Builtins::kToObject, context, receiver));
}

// ES #sec-object.create
TF_BUILTIN(ObjectCreate, ObjectBuiltinsAssembler) {
  int const kPrototypeArg = 0;
  int const kPropertiesArg = 1;

  Node* argc =
      ChangeInt32ToIntPtr(Parameter(BuiltinDescriptor::kArgumentsCount));
  CodeStubArguments args(this, argc);

  Node* prototype = args.GetOptionalArgumentValue(kPrototypeArg);
  Node* properties = args.GetOptionalArgumentValue(kPropertiesArg);
  Node* context = Parameter(BuiltinDescriptor::kContext);

  Label call_runtime(this, Label::kDeferred), prototype_valid(this),
      no_properties(this);
  {
    Comment("Argument 1 check: prototype");
    GotoIf(WordEqual(prototype, NullConstant()), &prototype_valid);
    BranchIfJSReceiver(prototype, &prototype_valid, &call_runtime);
  }

  BIND(&prototype_valid);
  {
    Comment("Argument 2 check: properties");
    // Check that we have a simple object
    GotoIf(TaggedIsSmi(properties), &call_runtime);
    // Undefined implies no properties.
    GotoIf(WordEqual(properties, UndefinedConstant()), &no_properties);
    Node* properties_map = LoadMap(properties);
    GotoIf(IsSpecialReceiverMap(properties_map), &call_runtime);
    // Stay on the fast path only if there are no elements.
    GotoIfNot(WordEqual(LoadElements(properties),
                        LoadRoot(Heap::kEmptyFixedArrayRootIndex)),
              &call_runtime);
    // Handle dictionary objects or fast objects with properties in runtime.
    Node* bit_field3 = LoadMapBitField3(properties_map);
    GotoIf(IsSetWord32<Map::DictionaryMap>(bit_field3), &call_runtime);
    Branch(IsSetWord32<Map::NumberOfOwnDescriptorsBits>(bit_field3),
           &call_runtime, &no_properties);
  }

  // Create a new object with the given prototype.
  BIND(&no_properties);
  {
    VARIABLE(map, MachineRepresentation::kTagged);
    VARIABLE(properties, MachineRepresentation::kTagged);
    Label non_null_proto(this), instantiate_map(this), good(this);

    Branch(WordEqual(prototype, NullConstant()), &good, &non_null_proto);

    BIND(&good);
    {
      map.Bind(LoadContextElement(
          context, Context::SLOW_OBJECT_WITH_NULL_PROTOTYPE_MAP));
      properties.Bind(AllocateNameDictionary(NameDictionary::kInitialCapacity));
      Goto(&instantiate_map);
    }

    BIND(&non_null_proto);
    {
      properties.Bind(EmptyFixedArrayConstant());
      Node* object_function =
          LoadContextElement(context, Context::OBJECT_FUNCTION_INDEX);
      Node* object_function_map = LoadObjectField(
          object_function, JSFunction::kPrototypeOrInitialMapOffset);
      map.Bind(object_function_map);
      GotoIf(WordEqual(prototype, LoadMapPrototype(map.value())),
             &instantiate_map);
      // Try loading the prototype info.
      Node* prototype_info =
          LoadMapPrototypeInfo(LoadMap(prototype), &call_runtime);
      Comment("Load ObjectCreateMap from PrototypeInfo");
      Node* weak_cell =
          LoadObjectField(prototype_info, PrototypeInfo::kObjectCreateMap);
      GotoIf(WordEqual(weak_cell, UndefinedConstant()), &call_runtime);
      map.Bind(LoadWeakCellValue(weak_cell, &call_runtime));
      Goto(&instantiate_map);
    }

    BIND(&instantiate_map);
    {
      Node* instance = AllocateJSObjectFromMap(map.value(), properties.value());
      args.PopAndReturn(instance);
    }
  }

  BIND(&call_runtime);
  {
    Node* result =
        CallRuntime(Runtime::kObjectCreate, context, prototype, properties);
    args.PopAndReturn(result);
  }
}

TF_BUILTIN(CreateIterResultObject, ObjectBuiltinsAssembler) {
  Node* const value = Parameter(Descriptor::kValue);
  Node* const done = Parameter(Descriptor::kDone);
  Node* const context = Parameter(Descriptor::kContext);

  Node* const native_context = LoadNativeContext(context);
  Node* const map =
      LoadContextElement(native_context, Context::ITERATOR_RESULT_MAP_INDEX);

  Node* const result = AllocateJSObjectFromMap(map);

  StoreObjectFieldNoWriteBarrier(result, JSIteratorResult::kValueOffset, value);
  StoreObjectFieldNoWriteBarrier(result, JSIteratorResult::kDoneOffset, done);

  Return(result);
}

TF_BUILTIN(HasProperty, ObjectBuiltinsAssembler) {
  Node* key = Parameter(Descriptor::kKey);
  Node* object = Parameter(Descriptor::kObject);
  Node* context = Parameter(Descriptor::kContext);

  Return(HasProperty(object, key, context, kHasProperty));
}

TF_BUILTIN(InstanceOf, ObjectBuiltinsAssembler) {
  Node* object = Parameter(Descriptor::kLeft);
  Node* callable = Parameter(Descriptor::kRight);
  Node* context = Parameter(Descriptor::kContext);

  Return(InstanceOf(object, callable, context));
}

// ES6 section 7.3.19 OrdinaryHasInstance ( C, O )
TF_BUILTIN(OrdinaryHasInstance, ObjectBuiltinsAssembler) {
  Node* constructor = Parameter(Descriptor::kLeft);
  Node* object = Parameter(Descriptor::kRight);
  Node* context = Parameter(Descriptor::kContext);

  Return(OrdinaryHasInstance(context, constructor, object));
}

TF_BUILTIN(GetSuperConstructor, ObjectBuiltinsAssembler) {
  Node* object = Parameter(Descriptor::kObject);
  Node* context = Parameter(Descriptor::kContext);

  Return(GetSuperConstructor(object, context));
}

TF_BUILTIN(CreateGeneratorObject, ObjectBuiltinsAssembler) {
  Node* closure = Parameter(Descriptor::kClosure);
  Node* receiver = Parameter(Descriptor::kReceiver);
  Node* context = Parameter(Descriptor::kContext);

  // Get the initial map from the function, jumping to the runtime if we don't
  // have one.
  Node* maybe_map =
      LoadObjectField(closure, JSFunction::kPrototypeOrInitialMapOffset);
  Label runtime(this);
  GotoIf(DoesntHaveInstanceType(maybe_map, MAP_TYPE), &runtime);

  Node* shared =
      LoadObjectField(closure, JSFunction::kSharedFunctionInfoOffset);
  Node* bytecode_array =
      LoadObjectField(shared, SharedFunctionInfo::kFunctionDataOffset);
  Node* frame_size = ChangeInt32ToIntPtr(LoadObjectField(
      bytecode_array, BytecodeArray::kFrameSizeOffset, MachineType::Int32()));
  Node* size = WordSar(frame_size, IntPtrConstant(kPointerSizeLog2));
  Node* register_file = AllocateFixedArray(HOLEY_ELEMENTS, size);
  FillFixedArrayWithValue(HOLEY_ELEMENTS, register_file, IntPtrConstant(0),
                          size, Heap::kUndefinedValueRootIndex);

  Node* const result = AllocateJSObjectFromMap(maybe_map);

  StoreObjectFieldNoWriteBarrier(result, JSGeneratorObject::kFunctionOffset,
                                 closure);
  StoreObjectFieldNoWriteBarrier(result, JSGeneratorObject::kContextOffset,
                                 context);
  StoreObjectFieldNoWriteBarrier(result, JSGeneratorObject::kReceiverOffset,
                                 receiver);
  StoreObjectFieldNoWriteBarrier(result, JSGeneratorObject::kRegisterFileOffset,
                                 register_file);
  Node* executing = SmiConstant(JSGeneratorObject::kGeneratorExecuting);
  StoreObjectFieldNoWriteBarrier(result, JSGeneratorObject::kContinuationOffset,
                                 executing);
  HandleSlackTracking(context, result, maybe_map, JSGeneratorObject::kSize);
  Return(result);

  BIND(&runtime);
  {
    Return(CallRuntime(Runtime::kCreateJSGeneratorObject, context, closure,
                       receiver));
  }
}

}  // namespace internal
}  // namespace v8
