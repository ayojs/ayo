// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/ic/keyed-store-generic.h"

#include "src/code-factory.h"
#include "src/code-stub-assembler.h"
#include "src/contexts.h"
#include "src/ic/accessor-assembler.h"
#include "src/interface-descriptors.h"
#include "src/isolate.h"
#include "src/objects-inl.h"

namespace v8 {
namespace internal {

using compiler::Node;

class KeyedStoreGenericAssembler : public AccessorAssembler {
 public:
  explicit KeyedStoreGenericAssembler(compiler::CodeAssemblerState* state)
      : AccessorAssembler(state) {}

  void KeyedStoreGeneric(LanguageMode language_mode);

  void StoreIC_Uninitialized(LanguageMode language_mode);

 private:
  enum UpdateLength {
    kDontChangeLength,
    kIncrementLengthByOne,
    kBumpLengthWithGap
  };

  enum UseStubCache { kUseStubCache, kDontUseStubCache };

  void EmitGenericElementStore(Node* receiver, Node* receiver_map,
                               Node* instance_type, Node* intptr_index,
                               Node* value, Node* context, Label* slow);

  void EmitGenericPropertyStore(Node* receiver, Node* receiver_map,
                                const StoreICParameters* p, Label* slow,
                                LanguageMode language_mode,
                                UseStubCache use_stub_cache = kUseStubCache);

  void BranchIfPrototypesHaveNonFastElements(Node* receiver_map,
                                             Label* non_fast_elements,
                                             Label* only_fast_elements);

  void TryRewriteElements(Node* receiver, Node* receiver_map, Node* elements,
                          Node* native_context, ElementsKind from_kind,
                          ElementsKind to_kind, Label* bailout);

  void StoreElementWithCapacity(Node* receiver, Node* receiver_map,
                                Node* elements, Node* elements_kind,
                                Node* intptr_index, Node* value, Node* context,
                                Label* slow, UpdateLength update_length);

  void MaybeUpdateLengthAndReturn(Node* receiver, Node* index, Node* value,
                                  UpdateLength update_length);

  void TryChangeToHoleyMapHelper(Node* receiver, Node* receiver_map,
                                 Node* native_context, ElementsKind packed_kind,
                                 ElementsKind holey_kind, Label* done,
                                 Label* map_mismatch, Label* bailout);
  void TryChangeToHoleyMap(Node* receiver, Node* receiver_map,
                           Node* current_elements_kind, Node* context,
                           ElementsKind packed_kind, Label* bailout);
  void TryChangeToHoleyMapMulti(Node* receiver, Node* receiver_map,
                                Node* current_elements_kind, Node* context,
                                ElementsKind packed_kind,
                                ElementsKind packed_kind_2, Label* bailout);

  void LookupPropertyOnPrototypeChain(Node* receiver_map, Node* name,
                                      Label* accessor,
                                      Variable* var_accessor_pair,
                                      Variable* var_accessor_holder,
                                      Label* readonly, Label* bailout);

  void CheckFieldType(Node* descriptors, Node* name_index, Node* representation,
                      Node* value, Label* bailout);
  void OverwriteExistingFastProperty(Node* object, Node* object_map,
                                     Node* properties, Node* descriptors,
                                     Node* descriptor_name_index, Node* details,
                                     Node* value, Label* slow);
};

void KeyedStoreGenericGenerator::Generate(compiler::CodeAssemblerState* state,
                                          LanguageMode language_mode) {
  KeyedStoreGenericAssembler assembler(state);
  assembler.KeyedStoreGeneric(language_mode);
}

void StoreICUninitializedGenerator::Generate(
    compiler::CodeAssemblerState* state, LanguageMode language_mode) {
  KeyedStoreGenericAssembler assembler(state);
  assembler.StoreIC_Uninitialized(language_mode);
}

void KeyedStoreGenericAssembler::BranchIfPrototypesHaveNonFastElements(
    Node* receiver_map, Label* non_fast_elements, Label* only_fast_elements) {
  VARIABLE(var_map, MachineRepresentation::kTagged);
  var_map.Bind(receiver_map);
  Label loop_body(this, &var_map);
  Goto(&loop_body);

  BIND(&loop_body);
  {
    Node* map = var_map.value();
    Node* prototype = LoadMapPrototype(map);
    GotoIf(WordEqual(prototype, NullConstant()), only_fast_elements);
    Node* prototype_map = LoadMap(prototype);
    var_map.Bind(prototype_map);
    Node* instance_type = LoadMapInstanceType(prototype_map);
    STATIC_ASSERT(JS_PROXY_TYPE < JS_OBJECT_TYPE);
    STATIC_ASSERT(JS_VALUE_TYPE < JS_OBJECT_TYPE);
    GotoIf(Int32LessThanOrEqual(instance_type,
                                Int32Constant(LAST_CUSTOM_ELEMENTS_RECEIVER)),
           non_fast_elements);
    Node* elements_kind = LoadMapElementsKind(prototype_map);
    STATIC_ASSERT(FIRST_ELEMENTS_KIND == FIRST_FAST_ELEMENTS_KIND);
    GotoIf(IsFastElementsKind(elements_kind), &loop_body);
    GotoIf(Word32Equal(elements_kind, Int32Constant(NO_ELEMENTS)), &loop_body);
    Goto(non_fast_elements);
  }
}

void KeyedStoreGenericAssembler::TryRewriteElements(
    Node* receiver, Node* receiver_map, Node* elements, Node* native_context,
    ElementsKind from_kind, ElementsKind to_kind, Label* bailout) {
  DCHECK(IsFastPackedElementsKind(from_kind));
  ElementsKind holey_from_kind = GetHoleyElementsKind(from_kind);
  ElementsKind holey_to_kind = GetHoleyElementsKind(to_kind);
  if (AllocationSite::ShouldTrack(from_kind, to_kind)) {
    TrapAllocationMemento(receiver, bailout);
  }
  Label perform_transition(this), check_holey_map(this);
  VARIABLE(var_target_map, MachineRepresentation::kTagged);
  // Check if the receiver has the default |from_kind| map.
  {
    Node* packed_map = LoadJSArrayElementsMap(from_kind, native_context);
    GotoIf(WordNotEqual(receiver_map, packed_map), &check_holey_map);
    var_target_map.Bind(
        LoadContextElement(native_context, Context::ArrayMapIndex(to_kind)));
    Goto(&perform_transition);
  }

  // Check if the receiver has the default |holey_from_kind| map.
  BIND(&check_holey_map);
  {
    Node* holey_map = LoadContextElement(
        native_context, Context::ArrayMapIndex(holey_from_kind));
    GotoIf(WordNotEqual(receiver_map, holey_map), bailout);
    var_target_map.Bind(LoadContextElement(
        native_context, Context::ArrayMapIndex(holey_to_kind)));
    Goto(&perform_transition);
  }

  // Found a supported transition target map, perform the transition!
  BIND(&perform_transition);
  {
    if (IsDoubleElementsKind(from_kind) != IsDoubleElementsKind(to_kind)) {
      Node* capacity = SmiUntag(LoadFixedArrayBaseLength(elements));
      GrowElementsCapacity(receiver, elements, from_kind, to_kind, capacity,
                           capacity, INTPTR_PARAMETERS, bailout);
    }
    StoreMap(receiver, var_target_map.value());
  }
}

void KeyedStoreGenericAssembler::TryChangeToHoleyMapHelper(
    Node* receiver, Node* receiver_map, Node* native_context,
    ElementsKind packed_kind, ElementsKind holey_kind, Label* done,
    Label* map_mismatch, Label* bailout) {
  Node* packed_map = LoadJSArrayElementsMap(packed_kind, native_context);
  GotoIf(WordNotEqual(receiver_map, packed_map), map_mismatch);
  if (AllocationSite::ShouldTrack(packed_kind, holey_kind)) {
    TrapAllocationMemento(receiver, bailout);
  }
  Node* holey_map =
      LoadContextElement(native_context, Context::ArrayMapIndex(holey_kind));
  StoreMap(receiver, holey_map);
  Goto(done);
}

void KeyedStoreGenericAssembler::TryChangeToHoleyMap(
    Node* receiver, Node* receiver_map, Node* current_elements_kind,
    Node* context, ElementsKind packed_kind, Label* bailout) {
  ElementsKind holey_kind = GetHoleyElementsKind(packed_kind);
  Label already_holey(this);

  GotoIf(Word32Equal(current_elements_kind, Int32Constant(holey_kind)),
         &already_holey);
  Node* native_context = LoadNativeContext(context);
  TryChangeToHoleyMapHelper(receiver, receiver_map, native_context, packed_kind,
                            holey_kind, &already_holey, bailout, bailout);
  BIND(&already_holey);
}

void KeyedStoreGenericAssembler::TryChangeToHoleyMapMulti(
    Node* receiver, Node* receiver_map, Node* current_elements_kind,
    Node* context, ElementsKind packed_kind, ElementsKind packed_kind_2,
    Label* bailout) {
  ElementsKind holey_kind = GetHoleyElementsKind(packed_kind);
  ElementsKind holey_kind_2 = GetHoleyElementsKind(packed_kind_2);
  Label already_holey(this), check_other_kind(this);

  GotoIf(Word32Equal(current_elements_kind, Int32Constant(holey_kind)),
         &already_holey);
  GotoIf(Word32Equal(current_elements_kind, Int32Constant(holey_kind_2)),
         &already_holey);

  Node* native_context = LoadNativeContext(context);
  TryChangeToHoleyMapHelper(receiver, receiver_map, native_context, packed_kind,
                            holey_kind, &already_holey, &check_other_kind,
                            bailout);
  BIND(&check_other_kind);
  TryChangeToHoleyMapHelper(receiver, receiver_map, native_context,
                            packed_kind_2, holey_kind_2, &already_holey,
                            bailout, bailout);
  BIND(&already_holey);
}

void KeyedStoreGenericAssembler::MaybeUpdateLengthAndReturn(
    Node* receiver, Node* index, Node* value, UpdateLength update_length) {
  if (update_length != kDontChangeLength) {
    Node* new_length = SmiTag(Signed(IntPtrAdd(index, IntPtrConstant(1))));
    StoreObjectFieldNoWriteBarrier(receiver, JSArray::kLengthOffset, new_length,
                                   MachineRepresentation::kTagged);
  }
  Return(value);
}

void KeyedStoreGenericAssembler::StoreElementWithCapacity(
    Node* receiver, Node* receiver_map, Node* elements, Node* elements_kind,
    Node* intptr_index, Node* value, Node* context, Label* slow,
    UpdateLength update_length) {
  if (update_length != kDontChangeLength) {
    CSA_ASSERT(this, Word32Equal(LoadMapInstanceType(receiver_map),
                                 Int32Constant(JS_ARRAY_TYPE)));
    // Check if the length property is writable. The fast check is only
    // supported for fast properties.
    GotoIf(IsDictionaryMap(receiver_map), slow);
    // The length property is non-configurable, so it's guaranteed to always
    // be the first property.
    Node* descriptors = LoadMapDescriptors(receiver_map);
    Node* details =
        LoadFixedArrayElement(descriptors, DescriptorArray::ToDetailsIndex(0));
    GotoIf(IsSetSmi(details, PropertyDetails::kAttributesReadOnlyMask), slow);
  }
  STATIC_ASSERT(FixedArray::kHeaderSize == FixedDoubleArray::kHeaderSize);
  const int kHeaderSize = FixedArray::kHeaderSize - kHeapObjectTag;

  Label check_double_elements(this), check_cow_elements(this);
  Node* elements_map = LoadMap(elements);
  GotoIf(WordNotEqual(elements_map, LoadRoot(Heap::kFixedArrayMapRootIndex)),
         &check_double_elements);

  // FixedArray backing store -> Smi or object elements.
  {
    Node* offset = ElementOffsetFromIndex(intptr_index, PACKED_ELEMENTS,
                                          INTPTR_PARAMETERS, kHeaderSize);
    // Check if we're about to overwrite the hole. We can safely do that
    // only if there can be no setters on the prototype chain.
    // If we know that we're storing beyond the previous array length, we
    // can skip the hole check (and always assume the hole).
    {
      Label hole_check_passed(this);
      if (update_length == kDontChangeLength) {
        Node* element = Load(MachineType::AnyTagged(), elements, offset);
        GotoIf(WordNotEqual(element, TheHoleConstant()), &hole_check_passed);
      }
      BranchIfPrototypesHaveNonFastElements(receiver_map, slow,
                                            &hole_check_passed);
      BIND(&hole_check_passed);
    }

    // Check if the value we're storing matches the elements_kind. Smis
    // can always be stored.
    {
      Label non_smi_value(this);
      GotoIfNot(TaggedIsSmi(value), &non_smi_value);
      // If we're about to introduce holes, ensure holey elements.
      if (update_length == kBumpLengthWithGap) {
        TryChangeToHoleyMapMulti(receiver, receiver_map, elements_kind, context,
                                 PACKED_SMI_ELEMENTS, PACKED_ELEMENTS, slow);
      }
      StoreNoWriteBarrier(MachineRepresentation::kTagged, elements, offset,
                          value);
      MaybeUpdateLengthAndReturn(receiver, intptr_index, value, update_length);

      BIND(&non_smi_value);
    }

    // Check if we already have object elements; just do the store if so.
    {
      Label must_transition(this);
      STATIC_ASSERT(PACKED_SMI_ELEMENTS == 0);
      STATIC_ASSERT(HOLEY_SMI_ELEMENTS == 1);
      GotoIf(Int32LessThanOrEqual(elements_kind,
                                  Int32Constant(HOLEY_SMI_ELEMENTS)),
             &must_transition);
      if (update_length == kBumpLengthWithGap) {
        TryChangeToHoleyMap(receiver, receiver_map, elements_kind, context,
                            PACKED_ELEMENTS, slow);
      }
      Store(elements, offset, value);
      MaybeUpdateLengthAndReturn(receiver, intptr_index, value, update_length);

      BIND(&must_transition);
    }

    // Transition to the required ElementsKind.
    {
      Label transition_to_double(this), transition_to_object(this);
      Node* native_context = LoadNativeContext(context);
      Branch(WordEqual(LoadMap(value), LoadRoot(Heap::kHeapNumberMapRootIndex)),
             &transition_to_double, &transition_to_object);
      BIND(&transition_to_double);
      {
        // If we're adding holes at the end, always transition to a holey
        // elements kind, otherwise try to remain packed.
        ElementsKind target_kind = update_length == kBumpLengthWithGap
                                       ? HOLEY_DOUBLE_ELEMENTS
                                       : PACKED_DOUBLE_ELEMENTS;
        TryRewriteElements(receiver, receiver_map, elements, native_context,
                           PACKED_SMI_ELEMENTS, target_kind, slow);
        // Reload migrated elements.
        Node* double_elements = LoadElements(receiver);
        Node* double_offset =
            ElementOffsetFromIndex(intptr_index, PACKED_DOUBLE_ELEMENTS,
                                   INTPTR_PARAMETERS, kHeaderSize);
        // Make sure we do not store signalling NaNs into double arrays.
        Node* double_value = Float64SilenceNaN(LoadHeapNumberValue(value));
        StoreNoWriteBarrier(MachineRepresentation::kFloat64, double_elements,
                            double_offset, double_value);
        MaybeUpdateLengthAndReturn(receiver, intptr_index, value,
                                   update_length);
      }

      BIND(&transition_to_object);
      {
        // If we're adding holes at the end, always transition to a holey
        // elements kind, otherwise try to remain packed.
        ElementsKind target_kind = update_length == kBumpLengthWithGap
                                       ? HOLEY_ELEMENTS
                                       : PACKED_ELEMENTS;
        TryRewriteElements(receiver, receiver_map, elements, native_context,
                           PACKED_SMI_ELEMENTS, target_kind, slow);
        // The elements backing store didn't change, no reload necessary.
        CSA_ASSERT(this, WordEqual(elements, LoadElements(receiver)));
        Store(elements, offset, value);
        MaybeUpdateLengthAndReturn(receiver, intptr_index, value,
                                   update_length);
      }
    }
  }

  BIND(&check_double_elements);
  Node* fixed_double_array_map = LoadRoot(Heap::kFixedDoubleArrayMapRootIndex);
  GotoIf(WordNotEqual(elements_map, fixed_double_array_map),
         &check_cow_elements);
  // FixedDoubleArray backing store -> double elements.
  {
    Node* offset = ElementOffsetFromIndex(intptr_index, PACKED_DOUBLE_ELEMENTS,
                                          INTPTR_PARAMETERS, kHeaderSize);
    // Check if we're about to overwrite the hole. We can safely do that
    // only if there can be no setters on the prototype chain.
    {
      Label hole_check_passed(this);
      // If we know that we're storing beyond the previous array length, we
      // can skip the hole check (and always assume the hole).
      if (update_length == kDontChangeLength) {
        Label found_hole(this);
        LoadDoubleWithHoleCheck(elements, offset, &found_hole,
                                MachineType::None());
        Goto(&hole_check_passed);
        BIND(&found_hole);
      }
      BranchIfPrototypesHaveNonFastElements(receiver_map, slow,
                                            &hole_check_passed);
      BIND(&hole_check_passed);
    }

    // Try to store the value as a double.
    {
      Label non_number_value(this);
      Node* double_value = TryTaggedToFloat64(value, &non_number_value);

      // Make sure we do not store signalling NaNs into double arrays.
      double_value = Float64SilenceNaN(double_value);
      // If we're about to introduce holes, ensure holey elements.
      if (update_length == kBumpLengthWithGap) {
        TryChangeToHoleyMap(receiver, receiver_map, elements_kind, context,
                            PACKED_DOUBLE_ELEMENTS, slow);
      }
      StoreNoWriteBarrier(MachineRepresentation::kFloat64, elements, offset,
                          double_value);
      MaybeUpdateLengthAndReturn(receiver, intptr_index, value, update_length);

      BIND(&non_number_value);
    }

    // Transition to object elements.
    {
      Node* native_context = LoadNativeContext(context);
      ElementsKind target_kind = update_length == kBumpLengthWithGap
                                     ? HOLEY_ELEMENTS
                                     : PACKED_ELEMENTS;
      TryRewriteElements(receiver, receiver_map, elements, native_context,
                         PACKED_DOUBLE_ELEMENTS, target_kind, slow);
      // Reload migrated elements.
      Node* fast_elements = LoadElements(receiver);
      Node* fast_offset = ElementOffsetFromIndex(
          intptr_index, PACKED_ELEMENTS, INTPTR_PARAMETERS, kHeaderSize);
      Store(fast_elements, fast_offset, value);
      MaybeUpdateLengthAndReturn(receiver, intptr_index, value, update_length);
    }
  }

  BIND(&check_cow_elements);
  {
    // TODO(jkummerow): Use GrowElementsCapacity instead of bailing out.
    Goto(slow);
  }
}

void KeyedStoreGenericAssembler::EmitGenericElementStore(
    Node* receiver, Node* receiver_map, Node* instance_type, Node* intptr_index,
    Node* value, Node* context, Label* slow) {
  Label if_fast(this), if_in_bounds(this), if_increment_length_by_one(this),
      if_bump_length_with_gap(this), if_grow(this), if_nonfast(this),
      if_typed_array(this), if_dictionary(this);
  Node* elements = LoadElements(receiver);
  Node* elements_kind = LoadMapElementsKind(receiver_map);
  Branch(IsFastElementsKind(elements_kind), &if_fast, &if_nonfast);
  BIND(&if_fast);

  Label if_array(this);
  GotoIf(Word32Equal(instance_type, Int32Constant(JS_ARRAY_TYPE)), &if_array);
  {
    Node* capacity = SmiUntag(LoadFixedArrayBaseLength(elements));
    Branch(UintPtrLessThan(intptr_index, capacity), &if_in_bounds, &if_grow);
  }
  BIND(&if_array);
  {
    Node* length = SmiUntag(LoadFastJSArrayLength(receiver));
    GotoIf(UintPtrLessThan(intptr_index, length), &if_in_bounds);
    Node* capacity = SmiUntag(LoadFixedArrayBaseLength(elements));
    GotoIf(UintPtrGreaterThanOrEqual(intptr_index, capacity), &if_grow);
    Branch(WordEqual(intptr_index, length), &if_increment_length_by_one,
           &if_bump_length_with_gap);
  }

  BIND(&if_in_bounds);
  {
    StoreElementWithCapacity(receiver, receiver_map, elements, elements_kind,
                             intptr_index, value, context, slow,
                             kDontChangeLength);
  }

  BIND(&if_increment_length_by_one);
  {
    StoreElementWithCapacity(receiver, receiver_map, elements, elements_kind,
                             intptr_index, value, context, slow,
                             kIncrementLengthByOne);
  }

  BIND(&if_bump_length_with_gap);
  {
    StoreElementWithCapacity(receiver, receiver_map, elements, elements_kind,
                             intptr_index, value, context, slow,
                             kBumpLengthWithGap);
  }

  // Out-of-capacity accesses (index >= capacity) jump here. Additionally,
  // an ElementsKind transition might be necessary.
  // The index can also be negative at this point! Jump to the runtime in that
  // case to convert it to a named property.
  BIND(&if_grow);
  {
    Comment("Grow backing store");
    // TODO(jkummerow): Support inline backing store growth.
    Goto(slow);
  }

  // Any ElementsKind > LAST_FAST_ELEMENTS_KIND jumps here for further
  // dispatch.
  BIND(&if_nonfast);
  {
    STATIC_ASSERT(LAST_ELEMENTS_KIND == LAST_FIXED_TYPED_ARRAY_ELEMENTS_KIND);
    GotoIf(Int32GreaterThanOrEqual(
               elements_kind,
               Int32Constant(FIRST_FIXED_TYPED_ARRAY_ELEMENTS_KIND)),
           &if_typed_array);
    GotoIf(Word32Equal(elements_kind, Int32Constant(DICTIONARY_ELEMENTS)),
           &if_dictionary);
    Goto(slow);
  }

  BIND(&if_dictionary);
  {
    Comment("Dictionary");
    // TODO(jkummerow): Support storing to dictionary elements.
    Goto(slow);
  }

  BIND(&if_typed_array);
  {
    Comment("Typed array");
    // TODO(jkummerow): Support typed arrays.
    Goto(slow);
  }
}

void KeyedStoreGenericAssembler::LookupPropertyOnPrototypeChain(
    Node* receiver_map, Node* name, Label* accessor,
    Variable* var_accessor_pair, Variable* var_accessor_holder, Label* readonly,
    Label* bailout) {
  Label ok_to_write(this);
  VARIABLE(var_holder, MachineRepresentation::kTagged);
  var_holder.Bind(LoadMapPrototype(receiver_map));
  VARIABLE(var_holder_map, MachineRepresentation::kTagged);
  var_holder_map.Bind(LoadMap(var_holder.value()));

  Variable* merged_variables[] = {&var_holder, &var_holder_map};
  Label loop(this, arraysize(merged_variables), merged_variables);
  Goto(&loop);
  BIND(&loop);
  {
    Node* holder = var_holder.value();
    Node* holder_map = var_holder_map.value();
    Node* instance_type = LoadMapInstanceType(holder_map);
    Label next_proto(this);
    {
      Label found(this), found_fast(this), found_dict(this), found_global(this);
      VARIABLE(var_meta_storage, MachineRepresentation::kTagged);
      VARIABLE(var_entry, MachineType::PointerRepresentation());
      TryLookupProperty(holder, holder_map, instance_type, name, &found_fast,
                        &found_dict, &found_global, &var_meta_storage,
                        &var_entry, &next_proto, bailout);
      BIND(&found_fast);
      {
        Node* descriptors = var_meta_storage.value();
        Node* name_index = var_entry.value();
        Node* details =
            LoadDetailsByKeyIndex<DescriptorArray>(descriptors, name_index);
        JumpIfDataProperty(details, &ok_to_write, readonly);

        // Accessor case.
        // TODO(jkummerow): Implement a trimmed-down LoadAccessorFromFastObject.
        VARIABLE(var_details, MachineRepresentation::kWord32);
        LoadPropertyFromFastObject(holder, holder_map, descriptors, name_index,
                                   &var_details, var_accessor_pair);
        var_accessor_holder->Bind(holder);
        Goto(accessor);
      }

      BIND(&found_dict);
      {
        Node* dictionary = var_meta_storage.value();
        Node* entry = var_entry.value();
        Node* details =
            LoadDetailsByKeyIndex<NameDictionary>(dictionary, entry);
        JumpIfDataProperty(details, &ok_to_write, readonly);

        // Accessor case.
        var_accessor_pair->Bind(
            LoadValueByKeyIndex<NameDictionary>(dictionary, entry));
        var_accessor_holder->Bind(holder);
        Goto(accessor);
      }

      BIND(&found_global);
      {
        Node* dictionary = var_meta_storage.value();
        Node* entry = var_entry.value();
        Node* property_cell =
            LoadValueByKeyIndex<GlobalDictionary>(dictionary, entry);
        Node* value =
            LoadObjectField(property_cell, PropertyCell::kValueOffset);
        GotoIf(WordEqual(value, TheHoleConstant()), &next_proto);
        Node* details = LoadAndUntagToWord32ObjectField(
            property_cell, PropertyCell::kDetailsOffset);
        JumpIfDataProperty(details, &ok_to_write, readonly);

        // Accessor case.
        var_accessor_pair->Bind(value);
        var_accessor_holder->Bind(holder);
        Goto(accessor);
      }
    }

    BIND(&next_proto);
    // Bailout if it can be an integer indexed exotic case.
    GotoIf(Word32Equal(instance_type, Int32Constant(JS_TYPED_ARRAY_TYPE)),
           bailout);
    Node* proto = LoadMapPrototype(holder_map);
    GotoIf(WordEqual(proto, NullConstant()), &ok_to_write);
    var_holder.Bind(proto);
    var_holder_map.Bind(LoadMap(proto));
    Goto(&loop);
  }
  BIND(&ok_to_write);
}

void KeyedStoreGenericAssembler::CheckFieldType(Node* descriptors,
                                                Node* name_index,
                                                Node* representation,
                                                Node* value, Label* bailout) {
  Label r_smi(this), r_double(this), r_heapobject(this), all_fine(this);
  // Ignore FLAG_track_fields etc. and always emit code for all checks,
  // because this builtin is part of the snapshot and therefore should
  // be flag independent.
  GotoIf(Word32Equal(representation, Int32Constant(Representation::kSmi)),
         &r_smi);
  GotoIf(Word32Equal(representation, Int32Constant(Representation::kDouble)),
         &r_double);
  GotoIf(
      Word32Equal(representation, Int32Constant(Representation::kHeapObject)),
      &r_heapobject);
  GotoIf(Word32Equal(representation, Int32Constant(Representation::kNone)),
         bailout);
  CSA_ASSERT(this, Word32Equal(representation,
                               Int32Constant(Representation::kTagged)));
  Goto(&all_fine);

  BIND(&r_smi);
  { Branch(TaggedIsSmi(value), &all_fine, bailout); }

  BIND(&r_double);
  {
    GotoIf(TaggedIsSmi(value), &all_fine);
    Node* value_map = LoadMap(value);
    // While supporting mutable HeapNumbers would be straightforward, such
    // objects should not end up here anyway.
    CSA_ASSERT(this,
               WordNotEqual(value_map,
                            LoadRoot(Heap::kMutableHeapNumberMapRootIndex)));
    Branch(IsHeapNumberMap(value_map), &all_fine, bailout);
  }

  BIND(&r_heapobject);
  {
    GotoIf(TaggedIsSmi(value), bailout);
    Node* field_type =
        LoadValueByKeyIndex<DescriptorArray>(descriptors, name_index);
    intptr_t kNoneType = reinterpret_cast<intptr_t>(FieldType::None());
    intptr_t kAnyType = reinterpret_cast<intptr_t>(FieldType::Any());
    // FieldType::None can't hold any value.
    GotoIf(WordEqual(field_type, IntPtrConstant(kNoneType)), bailout);
    // FieldType::Any can hold any value.
    GotoIf(WordEqual(field_type, IntPtrConstant(kAnyType)), &all_fine);
    CSA_ASSERT(this, IsWeakCell(field_type));
    // Cleared WeakCells count as FieldType::None, which can't hold any value.
    field_type = LoadWeakCellValue(field_type, bailout);
    // FieldType::Class(...) performs a map check.
    CSA_ASSERT(this, IsMap(field_type));
    Branch(WordEqual(LoadMap(value), field_type), &all_fine, bailout);
  }

  BIND(&all_fine);
}

void KeyedStoreGenericAssembler::OverwriteExistingFastProperty(
    Node* object, Node* object_map, Node* properties, Node* descriptors,
    Node* descriptor_name_index, Node* details, Node* value, Label* slow) {
  // Properties in descriptors can't be overwritten without map transition.
  GotoIf(Word32NotEqual(DecodeWord32<PropertyDetails::LocationField>(details),
                        Int32Constant(kField)),
         slow);

  if (FLAG_track_constant_fields) {
    // TODO(ishell): Taking the slow path is not necessary if new and old
    // values are identical.
    GotoIf(Word32Equal(DecodeWord32<PropertyDetails::ConstnessField>(details),
                       Int32Constant(kConst)),
           slow);
  }

  Label done(this);
  Node* representation =
      DecodeWord32<PropertyDetails::RepresentationField>(details);

  CheckFieldType(descriptors, descriptor_name_index, representation, value,
                 slow);
  Node* field_index =
      DecodeWordFromWord32<PropertyDetails::FieldIndexField>(details);
  Node* inobject_properties = LoadMapInobjectProperties(object_map);

  Label inobject(this), backing_store(this);
  Branch(UintPtrLessThan(field_index, inobject_properties), &inobject,
         &backing_store);

  BIND(&inobject);
  {
    Node* field_offset = TimesPointerSize(IntPtrAdd(
        IntPtrSub(LoadMapInstanceSize(object_map), inobject_properties),
        field_index));
    Label tagged_rep(this), double_rep(this);
    Branch(Word32Equal(representation, Int32Constant(Representation::kDouble)),
           &double_rep, &tagged_rep);
    BIND(&double_rep);
    {
      Node* double_value = ChangeNumberToFloat64(value);
      if (FLAG_unbox_double_fields) {
        StoreObjectFieldNoWriteBarrier(object, field_offset, double_value,
                                       MachineRepresentation::kFloat64);
      } else {
        Node* mutable_heap_number = LoadObjectField(object, field_offset);
        StoreHeapNumberValue(mutable_heap_number, double_value);
      }
      Goto(&done);
    }

    BIND(&tagged_rep);
    {
      StoreObjectField(object, field_offset, value);
      Goto(&done);
    }
  }

  BIND(&backing_store);
  {
    Node* backing_store_index = IntPtrSub(field_index, inobject_properties);
    Label tagged_rep(this), double_rep(this);
    Branch(Word32Equal(representation, Int32Constant(Representation::kDouble)),
           &double_rep, &tagged_rep);
    BIND(&double_rep);
    {
      Node* double_value = ChangeNumberToFloat64(value);
      Node* mutable_heap_number =
          LoadFixedArrayElement(properties, backing_store_index);
      StoreHeapNumberValue(mutable_heap_number, double_value);
      Goto(&done);
    }
    BIND(&tagged_rep);
    {
      StoreFixedArrayElement(properties, backing_store_index, value);
      Goto(&done);
    }
  }
  BIND(&done);
}

void KeyedStoreGenericAssembler::EmitGenericPropertyStore(
    Node* receiver, Node* receiver_map, const StoreICParameters* p, Label* slow,
    LanguageMode language_mode, UseStubCache use_stub_cache) {
  VARIABLE(var_accessor_pair, MachineRepresentation::kTagged);
  VARIABLE(var_accessor_holder, MachineRepresentation::kTagged);
  Label stub_cache(this), fast_properties(this), dictionary_properties(this),
      accessor(this), readonly(this);
  Node* bitfield3 = LoadMapBitField3(receiver_map);
  Branch(IsSetWord32<Map::DictionaryMap>(bitfield3), &dictionary_properties,
         &fast_properties);

  BIND(&fast_properties);
  {
    Comment("fast property store");
    Node* descriptors = LoadMapDescriptors(receiver_map);
    Label descriptor_found(this), lookup_transition(this);
    VARIABLE(var_name_index, MachineType::PointerRepresentation());
    Label* notfound = use_stub_cache == kUseStubCache ? &stub_cache : slow;
    DescriptorLookup(p->name, descriptors, bitfield3, &descriptor_found,
                     &var_name_index, &lookup_transition);

    BIND(&descriptor_found);
    {
      Node* name_index = var_name_index.value();
      Node* details =
          LoadDetailsByKeyIndex<DescriptorArray>(descriptors, name_index);
      Label data_property(this);
      JumpIfDataProperty(details, &data_property, &readonly);

      // Accessor case.
      // TODO(jkummerow): Implement a trimmed-down LoadAccessorFromFastObject.
      VARIABLE(var_details, MachineRepresentation::kWord32);
      LoadPropertyFromFastObject(receiver, receiver_map, descriptors,
                                 name_index, &var_details, &var_accessor_pair);
      var_accessor_holder.Bind(receiver);
      Goto(&accessor);

      BIND(&data_property);
      {
        CheckForAssociatedProtector(p->name, slow);
        Node* properties = LoadFastProperties(receiver);
        OverwriteExistingFastProperty(receiver, receiver_map, properties,
                                      descriptors, name_index, details,
                                      p->value, slow);
        Return(p->value);
      }
    }

    BIND(&lookup_transition);
    {
      Comment("lookup transition");
      VARIABLE(var_handler, MachineRepresentation::kTagged);
      Label tuple3(this), fixedarray(this), found_handler(this, &var_handler);
      Node* maybe_handler =
          LoadObjectField(receiver_map, Map::kTransitionsOrPrototypeInfoOffset);
      GotoIf(TaggedIsSmi(maybe_handler), notfound);
      Node* handler_map = LoadMap(maybe_handler);
      GotoIf(WordEqual(handler_map, Tuple3MapConstant()), &tuple3);
      GotoIf(WordEqual(handler_map, FixedArrayMapConstant()), &fixedarray);

      // TODO(jkummerow): Consider implementing TransitionArray search.
      Goto(notfound);

      VARIABLE(var_transition_cell, MachineRepresentation::kTagged);
      Label check_key(this, &var_transition_cell);
      BIND(&tuple3);
      {
        var_transition_cell.Bind(LoadObjectField(
            maybe_handler, StoreHandler::kTransitionCellOffset));
        Goto(&check_key);
      }

      BIND(&fixedarray);
      {
        var_transition_cell.Bind(LoadFixedArrayElement(
            maybe_handler, StoreHandler::kTransitionCellIndex));
        Goto(&check_key);
      }

      BIND(&check_key);
      {
        Node* transition = LoadWeakCellValue(var_transition_cell.value(), slow);
        Node* transition_bitfield3 = LoadMapBitField3(transition);
        GotoIf(IsSetWord32<Map::Deprecated>(transition_bitfield3), slow);
        Node* nof =
            DecodeWord32<Map::NumberOfOwnDescriptorsBits>(transition_bitfield3);
        Node* last_added = Int32Sub(nof, Int32Constant(1));
        Node* transition_descriptors = LoadMapDescriptors(transition);
        Node* key = DescriptorArrayGetKey(transition_descriptors, last_added);
        GotoIf(WordNotEqual(key, p->name), slow);
        var_handler.Bind(maybe_handler);
        Goto(&found_handler);
      }

      BIND(&found_handler);
      {
        Comment("KeyedStoreGeneric found transition handler");
        HandleStoreICHandlerCase(p, var_handler.value(), notfound);
      }
    }
  }

  BIND(&dictionary_properties);
  {
    Comment("dictionary property store");
    // We checked for LAST_CUSTOM_ELEMENTS_RECEIVER before, which rules out
    // seeing global objects here (which would need special handling).

    VARIABLE(var_name_index, MachineType::PointerRepresentation());
    Label dictionary_found(this, &var_name_index), not_found(this);
    Node* properties = LoadSlowProperties(receiver);
    NameDictionaryLookup<NameDictionary>(properties, p->name, &dictionary_found,
                                         &var_name_index, &not_found);
    BIND(&dictionary_found);
    {
      Label overwrite(this);
      Node* details = LoadDetailsByKeyIndex<NameDictionary>(
          properties, var_name_index.value());
      JumpIfDataProperty(details, &overwrite, &readonly);

      // Accessor case.
      var_accessor_pair.Bind(LoadValueByKeyIndex<NameDictionary>(
          properties, var_name_index.value()));
      var_accessor_holder.Bind(receiver);
      Goto(&accessor);

      BIND(&overwrite);
      {
        CheckForAssociatedProtector(p->name, slow);
        StoreValueByKeyIndex<NameDictionary>(properties, var_name_index.value(),
                                             p->value);
        Return(p->value);
      }
    }

    BIND(&not_found);
    {
      CheckForAssociatedProtector(p->name, slow);
      Label extensible(this);
      GotoIf(IsPrivateSymbol(p->name), &extensible);
      Node* bitfield2 = LoadMapBitField2(receiver_map);
      Branch(IsSetWord32(bitfield2, 1 << Map::kIsExtensible), &extensible,
             slow);

      BIND(&extensible);
      LookupPropertyOnPrototypeChain(receiver_map, p->name, &accessor,
                                     &var_accessor_pair, &var_accessor_holder,
                                     &readonly, slow);
      Add<NameDictionary>(properties, p->name, p->value, slow);
      Return(p->value);
    }
  }

  BIND(&accessor);
  {
    Label not_callable(this);
    Node* accessor_pair = var_accessor_pair.value();
    GotoIf(IsAccessorInfoMap(LoadMap(accessor_pair)), slow);
    CSA_ASSERT(this, HasInstanceType(accessor_pair, ACCESSOR_PAIR_TYPE));
    Node* setter = LoadObjectField(accessor_pair, AccessorPair::kSetterOffset);
    Node* setter_map = LoadMap(setter);
    // FunctionTemplateInfo setters are not supported yet.
    GotoIf(IsFunctionTemplateInfoMap(setter_map), slow);
    GotoIfNot(IsCallableMap(setter_map), &not_callable);

    Callable callable = CodeFactory::Call(isolate());
    CallJS(callable, p->context, setter, receiver, p->value);
    Return(p->value);

    BIND(&not_callable);
    {
      if (language_mode == STRICT) {
        Node* message = SmiConstant(MessageTemplate::kNoSetterInCallback);
        TailCallRuntime(Runtime::kThrowTypeError, p->context, message, p->name,
                        var_accessor_holder.value());
      } else {
        DCHECK_EQ(SLOPPY, language_mode);
        Return(p->value);
      }
    }
  }

  BIND(&readonly);
  {
    if (language_mode == STRICT) {
      Node* message = SmiConstant(MessageTemplate::kStrictReadOnlyProperty);
      Node* type = Typeof(p->receiver);
      TailCallRuntime(Runtime::kThrowTypeError, p->context, message, p->name,
                      type, p->receiver);
    } else {
      DCHECK_EQ(SLOPPY, language_mode);
      Return(p->value);
    }
  }

  if (use_stub_cache == kUseStubCache) {
    BIND(&stub_cache);
    Comment("stub cache probe");
    VARIABLE(var_handler, MachineRepresentation::kTagged);
    Label found_handler(this, &var_handler), stub_cache_miss(this);
    TryProbeStubCache(isolate()->store_stub_cache(), receiver, p->name,
                      &found_handler, &var_handler, &stub_cache_miss);
    BIND(&found_handler);
    {
      Comment("KeyedStoreGeneric found handler");
      HandleStoreICHandlerCase(p, var_handler.value(), &stub_cache_miss);
    }
    BIND(&stub_cache_miss);
    {
      Comment("KeyedStoreGeneric_miss");
      TailCallRuntime(Runtime::kKeyedStoreIC_Miss, p->context, p->value,
                      p->slot, p->vector, p->receiver, p->name);
    }
  }
}

void KeyedStoreGenericAssembler::KeyedStoreGeneric(LanguageMode language_mode) {
  typedef StoreWithVectorDescriptor Descriptor;

  Node* receiver = Parameter(Descriptor::kReceiver);
  Node* name = Parameter(Descriptor::kName);
  Node* value = Parameter(Descriptor::kValue);
  Node* slot = Parameter(Descriptor::kSlot);
  Node* vector = Parameter(Descriptor::kVector);
  Node* context = Parameter(Descriptor::kContext);

  VARIABLE(var_index, MachineType::PointerRepresentation());
  VARIABLE(var_unique, MachineRepresentation::kTagged);
  var_unique.Bind(name);  // Dummy initialization.
  Label if_index(this), if_unique_name(this), slow(this);

  GotoIf(TaggedIsSmi(receiver), &slow);
  Node* receiver_map = LoadMap(receiver);
  Node* instance_type = LoadMapInstanceType(receiver_map);
  // Receivers requiring non-standard element accesses (interceptors, access
  // checks, strings and string wrappers, proxies) are handled in the runtime.
  GotoIf(Int32LessThanOrEqual(instance_type,
                              Int32Constant(LAST_CUSTOM_ELEMENTS_RECEIVER)),
         &slow);

  TryToName(name, &if_index, &var_index, &if_unique_name, &var_unique, &slow);

  BIND(&if_index);
  {
    Comment("integer index");
    EmitGenericElementStore(receiver, receiver_map, instance_type,
                            var_index.value(), value, context, &slow);
  }

  BIND(&if_unique_name);
  {
    Comment("key is unique name");
    StoreICParameters p(context, receiver, var_unique.value(), value, slot,
                        vector);
    EmitGenericPropertyStore(receiver, receiver_map, &p, &slow, language_mode);
  }

  BIND(&slow);
  {
    Comment("KeyedStoreGeneric_slow");
    TailCallRuntime(Runtime::kSetProperty, context, receiver, name, value,
                    SmiConstant(language_mode));
  }
}

void KeyedStoreGenericAssembler::StoreIC_Uninitialized(
    LanguageMode language_mode) {
  typedef StoreWithVectorDescriptor Descriptor;

  Node* receiver = Parameter(Descriptor::kReceiver);
  Node* name = Parameter(Descriptor::kName);
  Node* value = Parameter(Descriptor::kValue);
  Node* slot = Parameter(Descriptor::kSlot);
  Node* vector = Parameter(Descriptor::kVector);
  Node* context = Parameter(Descriptor::kContext);

  Label miss(this);

  GotoIf(TaggedIsSmi(receiver), &miss);
  Node* receiver_map = LoadMap(receiver);
  Node* instance_type = LoadMapInstanceType(receiver_map);
  // Receivers requiring non-standard element accesses (interceptors, access
  // checks, strings and string wrappers, proxies) are handled in the runtime.
  GotoIf(Int32LessThanOrEqual(instance_type,
                              Int32Constant(LAST_SPECIAL_RECEIVER_TYPE)),
         &miss);

  // Optimistically write the state transition to the vector.
  StoreFeedbackVectorSlot(vector, slot,
                          LoadRoot(Heap::kpremonomorphic_symbolRootIndex),
                          SKIP_WRITE_BARRIER, 0, SMI_PARAMETERS);

  StoreICParameters p(context, receiver, name, value, slot, vector);
  EmitGenericPropertyStore(receiver, receiver_map, &p, &miss, language_mode,
                           kDontUseStubCache);

  BIND(&miss);
  {
    // Undo the optimistic state transition.
    StoreFeedbackVectorSlot(vector, slot,
                            LoadRoot(Heap::kuninitialized_symbolRootIndex),
                            SKIP_WRITE_BARRIER, 0, SMI_PARAMETERS);
    TailCallRuntime(Runtime::kStoreIC_Miss, context, value, slot, vector,
                    receiver, name);
  }
}

}  // namespace internal
}  // namespace v8
