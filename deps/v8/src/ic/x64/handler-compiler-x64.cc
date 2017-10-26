// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#if V8_TARGET_ARCH_X64

#include "src/ic/handler-compiler.h"

#include "src/api-arguments.h"
#include "src/field-type.h"
#include "src/ic/call-optimization.h"
#include "src/ic/ic.h"
#include "src/isolate-inl.h"

namespace v8 {
namespace internal {

#define __ ACCESS_MASM(masm)

void PropertyHandlerCompiler::PushVectorAndSlot(Register vector,
                                                Register slot) {
  MacroAssembler* masm = this->masm();
  STATIC_ASSERT(LoadWithVectorDescriptor::kSlot <
                LoadWithVectorDescriptor::kVector);
  STATIC_ASSERT(StoreWithVectorDescriptor::kSlot <
                StoreWithVectorDescriptor::kVector);
  STATIC_ASSERT(StoreTransitionDescriptor::kSlot <
                StoreTransitionDescriptor::kVector);
  __ Push(slot);
  __ Push(vector);
}


void PropertyHandlerCompiler::PopVectorAndSlot(Register vector, Register slot) {
  MacroAssembler* masm = this->masm();
  __ Pop(vector);
  __ Pop(slot);
}


void PropertyHandlerCompiler::DiscardVectorAndSlot() {
  MacroAssembler* masm = this->masm();
  // Remove vector and slot.
  __ addp(rsp, Immediate(2 * kPointerSize));
}

void PropertyHandlerCompiler::GenerateDictionaryNegativeLookup(
    MacroAssembler* masm, Label* miss_label, Register receiver,
    Handle<Name> name, Register scratch0, Register scratch1) {
  DCHECK(name->IsUniqueName());
  DCHECK(!receiver.is(scratch0));
  Counters* counters = masm->isolate()->counters();
  __ IncrementCounter(counters->negative_lookups(), 1);
  __ IncrementCounter(counters->negative_lookups_miss(), 1);

  __ movp(scratch0, FieldOperand(receiver, HeapObject::kMapOffset));

  const int kInterceptorOrAccessCheckNeededMask =
      (1 << Map::kHasNamedInterceptor) | (1 << Map::kIsAccessCheckNeeded);

  // Bail out if the receiver has a named interceptor or requires access checks.
  __ testb(FieldOperand(scratch0, Map::kBitFieldOffset),
           Immediate(kInterceptorOrAccessCheckNeededMask));
  __ j(not_zero, miss_label);

  // Check that receiver is a JSObject.
  __ CmpInstanceType(scratch0, FIRST_JS_RECEIVER_TYPE);
  __ j(below, miss_label);

  // Load properties array.
  Register properties = scratch0;
  __ movp(properties,
          FieldOperand(receiver, JSObject::kPropertiesOrHashOffset));

  // Check that the properties array is a dictionary.
  __ CompareRoot(FieldOperand(properties, HeapObject::kMapOffset),
                 Heap::kHashTableMapRootIndex);
  __ j(not_equal, miss_label);

  Label done;
  NameDictionaryLookupStub::GenerateNegativeLookup(masm, miss_label, &done,
                                                   properties, name, scratch1);
  __ bind(&done);
  __ DecrementCounter(counters->negative_lookups_miss(), 1);
}

// Generate call to api function.
void PropertyHandlerCompiler::GenerateApiAccessorCall(
    MacroAssembler* masm, const CallOptimization& optimization,
    Handle<Map> receiver_map, Register receiver, Register scratch,
    bool is_store, Register store_parameter, Register accessor_holder,
    int accessor_index) {
  DCHECK(!accessor_holder.is(scratch));
  DCHECK(optimization.is_simple_api_call());

  __ PopReturnAddressTo(scratch);
  // accessor_holder
  __ Push(accessor_holder);
  // receiver
  __ Push(receiver);
  // Write the arguments to stack frame.
  if (is_store) {
    DCHECK(!receiver.is(store_parameter));
    DCHECK(!scratch.is(store_parameter));
    __ Push(store_parameter);
  }
  __ PushReturnAddressFrom(scratch);
  // Stack now matches JSFunction abi.

  // Abi for CallApiCallbackStub.
  Register callee = rdi;
  Register data = rbx;
  Register holder = rcx;
  Register api_function_address = rdx;
  scratch = no_reg;

  // Put callee in place.
  __ LoadAccessor(callee, accessor_holder, accessor_index,
                  is_store ? ACCESSOR_SETTER : ACCESSOR_GETTER);

  // Put holder in place.
  CallOptimization::HolderLookup holder_lookup;
  optimization.LookupHolderOfExpectedType(receiver_map, &holder_lookup);
  switch (holder_lookup) {
    case CallOptimization::kHolderIsReceiver:
      __ Move(holder, receiver);
      break;
    case CallOptimization::kHolderFound:
      __ movp(holder, FieldOperand(receiver, HeapObject::kMapOffset));
      __ movp(holder, FieldOperand(holder, Map::kPrototypeOffset));
      break;
    case CallOptimization::kHolderNotFound:
      UNREACHABLE();
      break;
  }

  Isolate* isolate = masm->isolate();
  Handle<CallHandlerInfo> api_call_info = optimization.api_call_info();
  // Put call data in place.
  if (api_call_info->data()->IsUndefined(isolate)) {
    __ LoadRoot(data, Heap::kUndefinedValueRootIndex);
  } else {
    if (optimization.is_constant_call()) {
      __ movp(data,
              FieldOperand(callee, JSFunction::kSharedFunctionInfoOffset));
      __ movp(data,
              FieldOperand(data, SharedFunctionInfo::kFunctionDataOffset));
      __ movp(data, FieldOperand(data, FunctionTemplateInfo::kCallCodeOffset));
    } else {
      __ movp(data,
              FieldOperand(callee, FunctionTemplateInfo::kCallCodeOffset));
    }
    __ movp(data, FieldOperand(data, CallHandlerInfo::kDataOffset));
  }

  // Put api_function_address in place.
  Address function_address = v8::ToCData<Address>(api_call_info->callback());
  __ Move(api_function_address, function_address,
          RelocInfo::EXTERNAL_REFERENCE);

  // Jump to stub.
  CallApiCallbackStub stub(isolate, is_store, !optimization.is_constant_call());
  __ TailCallStub(&stub);
}


void PropertyHandlerCompiler::GenerateCheckPropertyCell(
    MacroAssembler* masm, Handle<JSGlobalObject> global, Handle<Name> name,
    Register scratch, Label* miss) {
  Handle<PropertyCell> cell = JSGlobalObject::EnsureEmptyPropertyCell(
      global, name, PropertyCellType::kInvalidated);
  Isolate* isolate = masm->isolate();
  DCHECK(cell->value()->IsTheHole(isolate));
  Handle<WeakCell> weak_cell = isolate->factory()->NewWeakCell(cell);
  __ LoadWeakValue(scratch, weak_cell, miss);
  __ Cmp(FieldOperand(scratch, PropertyCell::kValueOffset),
         isolate->factory()->the_hole_value());
  __ j(not_equal, miss);
}


void NamedStoreHandlerCompiler::GenerateStoreViaSetter(
    MacroAssembler* masm, Handle<Map> map, Register receiver, Register holder,
    int accessor_index, int expected_arguments, Register scratch) {
  // ----------- S t a t e -------------
  //  -- rsp[0] : return address
  // -----------------------------------
  {
    FrameScope scope(masm, StackFrame::INTERNAL);

    // Save context register
    __ pushq(rsi);
    // Save value register, so we can restore it later.
    __ Push(value());

    if (accessor_index >= 0) {
      DCHECK(!holder.is(scratch));
      DCHECK(!receiver.is(scratch));
      DCHECK(!value().is(scratch));
      // Call the JavaScript setter with receiver and value on the stack.
      if (map->IsJSGlobalObjectMap()) {
        // Swap in the global receiver.
        __ movp(scratch,
                FieldOperand(receiver, JSGlobalObject::kGlobalProxyOffset));
        receiver = scratch;
      }
      __ Push(receiver);
      __ Push(value());
      __ LoadAccessor(rdi, holder, accessor_index, ACCESSOR_SETTER);
      __ Set(rax, 1);
      __ Call(masm->isolate()->builtins()->CallFunction(
                  ConvertReceiverMode::kNotNullOrUndefined),
              RelocInfo::CODE_TARGET);
    } else {
      // If we generate a global code snippet for deoptimization only, remember
      // the place to continue after deoptimization.
      masm->isolate()->heap()->SetSetterStubDeoptPCOffset(masm->pc_offset());
    }

    // We have to return the passed value, not the return value of the setter.
    __ Pop(rax);

    // Restore context register.
    __ popq(rsi);
  }
  __ ret(0);
}

void NamedLoadHandlerCompiler::GenerateLoadViaGetterForDeopt(
    MacroAssembler* masm) {
  {
    FrameScope scope(masm, StackFrame::INTERNAL);
    // Remember the place to continue after deoptimization.
    masm->isolate()->heap()->SetGetterStubDeoptPCOffset(masm->pc_offset());
    // Restore context register.
    __ popq(rsi);
  }
  __ ret(0);
}

#undef __
#define __ ACCESS_MASM((masm()))


void NamedStoreHandlerCompiler::GenerateRestoreName(Label* label,
                                                    Handle<Name> name) {
  if (!label->is_unused()) {
    __ bind(label);
    __ Move(this->name(), name);
  }
}

void PropertyHandlerCompiler::GenerateAccessCheck(
    Handle<WeakCell> native_context_cell, Register scratch1, Register scratch2,
    Label* miss, bool compare_native_contexts_only) {
  Label done;
  // Load current native context.
  __ movp(scratch1, NativeContextOperand());
  // Load expected native context.
  __ LoadWeakValue(scratch2, native_context_cell, miss);
  __ cmpp(scratch1, scratch2);

  if (!compare_native_contexts_only) {
    __ j(equal, &done);

    // Compare security tokens of current and expected native contexts.
    __ movp(scratch1, ContextOperand(scratch1, Context::SECURITY_TOKEN_INDEX));
    __ movp(scratch2, ContextOperand(scratch2, Context::SECURITY_TOKEN_INDEX));
    __ cmpp(scratch1, scratch2);
  }
  __ j(not_equal, miss);

  __ bind(&done);
}

Register PropertyHandlerCompiler::CheckPrototypes(
    Register object_reg, Register holder_reg, Register scratch1,
    Register scratch2, Handle<Name> name, Label* miss) {
  Handle<Map> receiver_map = map();

  // Make sure there's no overlap between holder and object registers.
  DCHECK(!scratch1.is(object_reg) && !scratch1.is(holder_reg));
  DCHECK(!scratch2.is(object_reg) && !scratch2.is(holder_reg) &&
         !scratch2.is(scratch1));

  Handle<Cell> validity_cell =
      Map::GetOrCreatePrototypeChainValidityCell(receiver_map, isolate());
  if (!validity_cell.is_null()) {
    DCHECK_EQ(Smi::FromInt(Map::kPrototypeChainValid), validity_cell->value());
    __ Move(scratch1, validity_cell);
    __ SmiCompare(FieldOperand(scratch1, Cell::kValueOffset),
                  Smi::FromInt(Map::kPrototypeChainValid));
    __ j(not_equal, miss);
  }

  // Keep track of the current object in register reg.  On the first
  // iteration, reg is an alias for object_reg, on later iterations,
  // it is an alias for holder_reg.
  Register reg = object_reg;
  int depth = 0;

  Handle<JSObject> current = Handle<JSObject>::null();
  if (receiver_map->IsJSGlobalObjectMap()) {
    current = isolate()->global_object();
  }

  Handle<Map> current_map(receiver_map->GetPrototypeChainRootMap(isolate()),
                          isolate());
  Handle<Map> holder_map(holder()->map());
  // Traverse the prototype chain and check the maps in the prototype chain for
  // fast and global objects or do negative lookup for normal objects.
  while (!current_map.is_identical_to(holder_map)) {
    ++depth;

    if (current_map->IsJSGlobalObjectMap()) {
      GenerateCheckPropertyCell(masm(), Handle<JSGlobalObject>::cast(current),
                                name, scratch2, miss);
    } else if (current_map->is_dictionary_map()) {
      DCHECK(!current_map->IsJSGlobalProxyMap());  // Proxy maps are fast.
      DCHECK(name->IsUniqueName());
      DCHECK(current.is_null() ||
             current->property_dictionary()->FindEntry(name) ==
                 NameDictionary::kNotFound);

      if (depth > 1) {
        Handle<WeakCell> weak_cell =
            Map::GetOrCreatePrototypeWeakCell(current, isolate());
        __ LoadWeakValue(reg, weak_cell, miss);
      }
      GenerateDictionaryNegativeLookup(masm(), miss, reg, name, scratch1,
                                       scratch2);
    }

    reg = holder_reg;  // From now on the object will be in holder_reg.
    // Go to the next object in the prototype chain.
    current = handle(JSObject::cast(current_map->prototype()));
    current_map = handle(current->map());
  }

  DCHECK(!current_map->IsJSGlobalProxyMap());

  // Log the check depth.
  LOG(isolate(), IntEvent("check-maps-depth", depth + 1));

  if (depth != 0) {
    Handle<WeakCell> weak_cell =
        Map::GetOrCreatePrototypeWeakCell(current, isolate());
    __ LoadWeakValue(reg, weak_cell, miss);
  }

  // Return the register containing the holder.
  return reg;
}


void NamedLoadHandlerCompiler::FrontendFooter(Handle<Name> name, Label* miss) {
  if (!miss->is_unused()) {
    Label success;
    __ jmp(&success);
    __ bind(miss);
    DCHECK(kind() == Code::LOAD_IC);
    PopVectorAndSlot();
    TailCallBuiltin(masm(), MissBuiltin(kind()));
    __ bind(&success);
  }
}


void NamedStoreHandlerCompiler::FrontendFooter(Handle<Name> name, Label* miss) {
  if (!miss->is_unused()) {
    Label success;
    __ jmp(&success);
    GenerateRestoreName(miss, name);
    PopVectorAndSlot();
    TailCallBuiltin(masm(), MissBuiltin(kind()));
    __ bind(&success);
  }
}

void NamedStoreHandlerCompiler::ZapStackArgumentsRegisterAliases() {
  STATIC_ASSERT(!StoreWithVectorDescriptor::kPassLastArgsOnStack);
}

Handle<Code> NamedStoreHandlerCompiler::CompileStoreCallback(
    Handle<JSObject> object, Handle<Name> name, Handle<AccessorInfo> callback,
    LanguageMode language_mode) {
  Register holder_reg = Frontend(name);

  __ PopReturnAddressTo(scratch1());
  __ Push(receiver());
  __ Push(holder_reg);
  // If the callback cannot leak, then push the callback directly,
  // otherwise wrap it in a weak cell.
  if (callback->data()->IsUndefined(isolate()) || callback->data()->IsSmi()) {
    __ Push(callback);
  } else {
    Handle<WeakCell> cell = isolate()->factory()->NewWeakCell(callback);
    __ Push(cell);
  }
  __ Push(name);
  __ Push(value());
  __ Push(Smi::FromInt(language_mode));
  __ PushReturnAddressFrom(scratch1());

  // Do tail-call to the runtime system.
  __ TailCallRuntime(Runtime::kStoreCallbackProperty);

  // Return the generated code.
  return GetCode(kind(), name);
}


Register NamedStoreHandlerCompiler::value() {
  return StoreDescriptor::ValueRegister();
}


#undef __
}  // namespace internal
}  // namespace v8

#endif  // V8_TARGET_ARCH_X64
