// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/builtins/builtins-proxy-gen.h"
#include "src/builtins/builtins-utils-gen.h"
#include "src/builtins/builtins-utils.h"
#include "src/builtins/builtins.h"

#include "src/counters.h"
#include "src/objects-inl.h"

namespace v8 {
namespace internal {

// ES6 section 26.2.1.1 Proxy ( target, handler ) for the [[Call]] case.
TF_BUILTIN(ProxyConstructor, CodeStubAssembler) {
  Node* context = Parameter(Descriptor::kContext);
  ThrowTypeError(context, MessageTemplate::kConstructorNotFunction, "Proxy");
}

void ProxiesCodeStubAssembler::GotoIfRevokedProxy(Node* object,
                                                  Label* if_proxy_revoked) {
  Label proxy_not_revoked(this);
  GotoIfNot(IsJSProxy(object), &proxy_not_revoked);
  Branch(IsJSReceiver(LoadObjectField(object, JSProxy::kHandlerOffset)),
         &proxy_not_revoked, if_proxy_revoked);
  BIND(&proxy_not_revoked);
}

Node* ProxiesCodeStubAssembler::AllocateProxy(Node* target, Node* handler,
                                              Node* context) {
  VARIABLE(map, MachineRepresentation::kTagged);

  Label callable_target(this), constructor_target(this), none_target(this),
      create_proxy(this);

  Node* nativeContext = LoadNativeContext(context);

  Branch(IsCallable(target), &callable_target, &none_target);

  BIND(&callable_target);
  {
    // Every object that is a constructor is implicitly callable
    // so it's okay to nest this check here
    GotoIf(IsConstructor(target), &constructor_target);
    map.Bind(
        LoadContextElement(nativeContext, Context::PROXY_CALLABLE_MAP_INDEX));
    Goto(&create_proxy);
  }
  BIND(&constructor_target);
  {
    map.Bind(LoadContextElement(nativeContext,
                                Context::PROXY_CONSTRUCTOR_MAP_INDEX));
    Goto(&create_proxy);
  }
  BIND(&none_target);
  {
    map.Bind(LoadContextElement(nativeContext, Context::PROXY_MAP_INDEX));
    Goto(&create_proxy);
  }

  BIND(&create_proxy);
  Node* proxy = Allocate(JSProxy::kSize);
  StoreMapNoWriteBarrier(proxy, map.value());
  StoreObjectFieldRoot(proxy, JSProxy::kPropertiesOrHashOffset,
                       Heap::kEmptyPropertyDictionaryRootIndex);
  StoreObjectFieldNoWriteBarrier(proxy, JSProxy::kTargetOffset, target);
  StoreObjectFieldNoWriteBarrier(proxy, JSProxy::kHandlerOffset, handler);
  StoreObjectFieldNoWriteBarrier(proxy, JSProxy::kHashOffset,
                                 UndefinedConstant());

  return proxy;
}

Node* ProxiesCodeStubAssembler::AllocateJSArrayForCodeStubArguments(
    Node* context, CodeStubArguments& args, Node* argc, ParameterMode mode) {
  Node* array = nullptr;
  Node* elements = nullptr;
  Node* native_context = LoadNativeContext(context);
  Node* array_map = LoadJSArrayElementsMap(PACKED_ELEMENTS, native_context);
  Node* argc_smi = ParameterToTagged(argc, mode);
  std::tie(array, elements) = AllocateUninitializedJSArrayWithElements(
      PACKED_ELEMENTS, array_map, argc_smi, nullptr, argc, INTPTR_PARAMETERS);

  VARIABLE(index, MachineType::PointerRepresentation());
  index.Bind(IntPtrConstant(FixedArrayBase::kHeaderSize - kHeapObjectTag));
  VariableList list({&index}, zone());
  args.ForEach(list, [this, elements, &index](Node* arg) {
    StoreNoWriteBarrier(MachineRepresentation::kTagged, elements, index.value(),
                        arg);
    Increment(&index, kPointerSize);
  });
  return array;
}

// ES6 section 26.2.1.1 Proxy ( target, handler ) for the [[Construct]] case.
TF_BUILTIN(ProxyConstructor_ConstructStub, ProxiesCodeStubAssembler) {
  int const kTargetArg = 0;
  int const kHandlerArg = 1;

  Node* argc =
      ChangeInt32ToIntPtr(Parameter(BuiltinDescriptor::kArgumentsCount));
  CodeStubArguments args(this, argc);

  Node* target = args.GetOptionalArgumentValue(kTargetArg);
  Node* handler = args.GetOptionalArgumentValue(kHandlerArg);
  Node* context = Parameter(BuiltinDescriptor::kContext);

  Label throw_proxy_non_object(this, Label::kDeferred),
      throw_proxy_handler_or_target_revoked(this, Label::kDeferred),
      return_create_proxy(this);

  GotoIf(TaggedIsSmi(target), &throw_proxy_non_object);
  GotoIfNot(IsJSReceiver(target), &throw_proxy_non_object);
  GotoIfRevokedProxy(target, &throw_proxy_handler_or_target_revoked);

  GotoIf(TaggedIsSmi(handler), &throw_proxy_non_object);
  GotoIfNot(IsJSReceiver(handler), &throw_proxy_non_object);
  GotoIfRevokedProxy(handler, &throw_proxy_handler_or_target_revoked);

  args.PopAndReturn(AllocateProxy(target, handler, context));

  BIND(&throw_proxy_non_object);
  ThrowTypeError(context, MessageTemplate::kProxyNonObject);

  BIND(&throw_proxy_handler_or_target_revoked);
  ThrowTypeError(context, MessageTemplate::kProxyHandlerOrTargetRevoked);
}

TF_BUILTIN(CallProxy, ProxiesCodeStubAssembler) {
  Node* argc = Parameter(Descriptor::kActualArgumentsCount);
  Node* argc_ptr = ChangeInt32ToIntPtr(argc);
  Node* proxy = Parameter(Descriptor::kFunction);
  Node* context = Parameter(Descriptor::kContext);

  CSA_ASSERT(this, IsJSProxy(proxy));
  CSA_ASSERT(this, IsCallable(proxy));

  Label throw_proxy_handler_revoked(this, Label::kDeferred),
      trap_undefined(this);

  // 1. Let handler be the value of the [[ProxyHandler]] internal slot of O.
  Node* handler = LoadObjectField(proxy, JSProxy::kHandlerOffset);

  // 2. If handler is null, throw a TypeError exception.
  CSA_ASSERT(this, IsNullOrJSReceiver(handler));
  GotoIfNot(IsJSReceiver(handler), &throw_proxy_handler_revoked);

  // 3. Assert: Type(handler) is Object.
  CSA_ASSERT(this, IsJSReceiver(handler));

  // 4. Let target be the value of the [[ProxyTarget]] internal slot of O.
  Node* target = LoadObjectField(proxy, JSProxy::kTargetOffset);

  // 5. Let trap be ? GetMethod(handler, "apply").
  // 6. If trap is undefined, then
  Handle<Name> trap_name = factory()->apply_string();
  Node* trap = GetMethod(context, handler, trap_name, &trap_undefined);

  CodeStubArguments args(this, argc_ptr);
  Node* receiver = args.GetReceiver();

  // 7. Let argArray be CreateArrayFromList(argumentsList).
  Node* array = AllocateJSArrayForCodeStubArguments(context, args, argc_ptr,
                                                    INTPTR_PARAMETERS);

  // 8. Return Call(trap, handler, «target, thisArgument, argArray»).
  Node* result = CallJS(CodeFactory::Call(isolate()), context, trap, handler,
                        target, receiver, array);
  args.PopAndReturn(result);

  BIND(&trap_undefined);
  {
    // 6.a. Return Call(target, thisArgument, argumentsList).
    TailCallStub(CodeFactory::Call(isolate()), context, target, argc);
  }

  BIND(&throw_proxy_handler_revoked);
  { ThrowTypeError(context, MessageTemplate::kProxyRevoked, "apply"); }
}

TF_BUILTIN(ConstructProxy, ProxiesCodeStubAssembler) {
  Node* argc = Parameter(Descriptor::kActualArgumentsCount);
  Node* argc_ptr = ChangeInt32ToIntPtr(argc);
  Node* proxy = Parameter(Descriptor::kFunction);
  Node* new_target = Parameter(Descriptor::kNewTarget);
  Node* context = Parameter(Descriptor::kContext);

  CSA_ASSERT(this, IsJSProxy(proxy));
  CSA_ASSERT(this, IsCallable(proxy));

  Label throw_proxy_handler_revoked(this, Label::kDeferred),
      trap_undefined(this), not_an_object(this, Label::kDeferred);

  // 1. Let handler be the value of the [[ProxyHandler]] internal slot of O.
  Node* handler = LoadObjectField(proxy, JSProxy::kHandlerOffset);

  // 2. If handler is null, throw a TypeError exception.
  CSA_ASSERT(this, IsNullOrJSReceiver(handler));
  GotoIfNot(IsJSReceiver(handler), &throw_proxy_handler_revoked);

  // 3. Assert: Type(handler) is Object.
  CSA_ASSERT(this, IsJSReceiver(handler));

  // 4. Let target be the value of the [[ProxyTarget]] internal slot of O.
  Node* target = LoadObjectField(proxy, JSProxy::kTargetOffset);

  // 5. Let trap be ? GetMethod(handler, "construct").
  // 6. If trap is undefined, then
  Handle<Name> trap_name = factory()->construct_string();
  Node* trap = GetMethod(context, handler, trap_name, &trap_undefined);

  CodeStubArguments args(this, argc_ptr);

  // 7. Let argArray be CreateArrayFromList(argumentsList).
  Node* array = AllocateJSArrayForCodeStubArguments(context, args, argc_ptr,
                                                    INTPTR_PARAMETERS);

  // 8. Let newObj be ? Call(trap, handler, « target, argArray, newTarget »).
  Node* new_obj = CallJS(CodeFactory::Call(isolate()), context, trap, handler,
                         target, array, new_target);

  // 9. If Type(newObj) is not Object, throw a TypeError exception.
  GotoIf(TaggedIsSmi(new_obj), &not_an_object);
  GotoIfNot(IsJSReceiver(new_obj), &not_an_object);

  // 10. Return newObj.
  args.PopAndReturn(new_obj);

  BIND(&not_an_object);
  {
    ThrowTypeError(context, MessageTemplate::kProxyConstructNonObject, new_obj);
  }

  BIND(&trap_undefined);
  {
    // 6.a. Assert: target has a [[Construct]] internal method.
    CSA_ASSERT(this, IsConstructor(target));

    // 6.b. Return ? Construct(target, argumentsList, newTarget).
    TailCallStub(CodeFactory::Construct(isolate()), context, target, new_target,
                 argc);
  }

  BIND(&throw_proxy_handler_revoked);
  { ThrowTypeError(context, MessageTemplate::kProxyRevoked, "construct"); }
}

TF_BUILTIN(ProxyHasProperty, ProxiesCodeStubAssembler) {
  Node* context = Parameter(Descriptor::kContext);
  Node* proxy = Parameter(Descriptor::kProxy);
  Node* name = Parameter(Descriptor::kName);

  CSA_ASSERT(this, IsJSProxy(proxy));

  // 1. Assert: IsPropertyKey(P) is true.
  CSA_ASSERT(this, IsName(name));
  CSA_ASSERT(this, Word32Equal(IsPrivateSymbol(name), Int32Constant(0)));

  Label throw_proxy_handler_revoked(this, Label::kDeferred),
      trap_undefined(this),
      if_try_get_own_property_bailout(this, Label::kDeferred),
      trap_not_callable(this, Label::kDeferred), return_true(this),
      return_false(this), check_target_desc(this);

  // 2. Let handler be O.[[ProxyHandler]].
  Node* handler = LoadObjectField(proxy, JSProxy::kHandlerOffset);

  // 3. If handler is null, throw a TypeError exception.
  // 4. Assert: Type(handler) is Object.
  GotoIfNot(IsJSReceiver(handler), &throw_proxy_handler_revoked);

  // 5. Let target be O.[[ProxyTarget]].
  Node* target = LoadObjectField(proxy, JSProxy::kTargetOffset);

  // 6. Let trap be ? GetMethod(handler, "has").
  // 7. If trap is undefined, then (see 7.a below).
  Handle<Name> trap_name = factory()->has_string();
  Node* trap = GetMethod(context, handler, trap_name, &trap_undefined);

  GotoIf(TaggedIsSmi(trap), &trap_not_callable);
  GotoIfNot(IsCallable(trap), &trap_not_callable);

  // 8. Let booleanTrapResult be ToBoolean(? Call(trap, handler, « target, P
  // »)).
  BranchIfToBooleanIsTrue(CallJS(CodeFactory::Call(isolate()), context, trap,
                                 handler, target, name),
                          &return_true, &check_target_desc);

  BIND(&check_target_desc);
  {
    // 9. If booleanTrapResult is false, then (see 9.a. in CheckHasTrapResult).
    CheckHasTrapResult(context, target, proxy, name, &return_false,
                       &if_try_get_own_property_bailout);
  }

  BIND(&if_try_get_own_property_bailout);
  {
    CallRuntime(Runtime::kCheckProxyHasTrap, context, name, target);
    Return(FalseConstant());
  }

  BIND(&trap_undefined);
  {
    // 7.a. Return ? target.[[HasProperty]](P).
    TailCallStub(Builtins::CallableFor(isolate(), Builtins::kHasProperty),
                 context, name, target);
  }

  BIND(&return_false);
  Return(FalseConstant());

  BIND(&return_true);
  Return(TrueConstant());

  BIND(&throw_proxy_handler_revoked);
  ThrowTypeError(context, MessageTemplate::kProxyRevoked, "has");

  BIND(&trap_not_callable);
  ThrowTypeError(context, MessageTemplate::kPropertyNotFunction, trap,
                 StringConstant("has"), proxy);
}

void ProxiesCodeStubAssembler::CheckHasTrapResult(Node* context, Node* target,
                                                  Node* proxy, Node* name,
                                                  Label* check_passed,
                                                  Label* if_bailout) {
  Node* target_map = LoadMap(target);
  VARIABLE(var_value, MachineRepresentation::kTagged);
  VARIABLE(var_details, MachineRepresentation::kWord32);
  VARIABLE(var_raw_value, MachineRepresentation::kTagged);

  Label if_found_value(this, Label::kDeferred),
      throw_non_configurable(this, Label::kDeferred),
      throw_non_extensible(this, Label::kDeferred);

  // 9.a. Let targetDesc be ? target.[[GetOwnProperty]](P).
  Node* instance_type = LoadInstanceType(target);
  TryGetOwnProperty(context, target, target, target_map, instance_type, name,
                    &if_found_value, &var_value, &var_details, &var_raw_value,
                    check_passed, if_bailout);

  // 9.b. If targetDesc is not undefined, then (see 9.b.i. below).
  BIND(&if_found_value);
  {
    // 9.b.i. If targetDesc.[[Configurable]] is false, throw a TypeError
    // exception.
    Node* non_configurable = IsSetWord32(
        var_details.value(), PropertyDetails::kAttributesDontDeleteMask);
    GotoIf(non_configurable, &throw_non_configurable);

    // 9.b.ii. Let extensibleTarget be ? IsExtensible(target).
    Node* target_extensible = IsExtensibleMap(target_map);

    // 9.b.iii. If extensibleTarget is false, throw a TypeError exception.
    GotoIfNot(target_extensible, &throw_non_extensible);
    Goto(check_passed);
  }

  BIND(&throw_non_configurable);
  { ThrowTypeError(context, MessageTemplate::kProxyHasNonConfigurable, name); }

  BIND(&throw_non_extensible);
  { ThrowTypeError(context, MessageTemplate::kProxyHasNonExtensible, name); }
}

}  // namespace internal
}  // namespace v8
