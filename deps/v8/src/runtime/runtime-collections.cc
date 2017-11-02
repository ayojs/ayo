// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/runtime/runtime-utils.h"

#include "src/arguments.h"
#include "src/conversions-inl.h"
#include "src/factory.h"

namespace v8 {
namespace internal {

RUNTIME_FUNCTION(Runtime_TheHole) {
  SealHandleScope shs(isolate);
  DCHECK_EQ(0, args.length());
  return isolate->heap()->the_hole_value();
}

RUNTIME_FUNCTION(Runtime_GetExistingHash) {
  SealHandleScope shs(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);
  return object->GetHash();
}

RUNTIME_FUNCTION(Runtime_GenericHash) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);
  return object->GetOrCreateHash(isolate);
}

RUNTIME_FUNCTION(Runtime_SetGrow) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSSet, holder, 0);
  Handle<OrderedHashSet> table(OrderedHashSet::cast(holder->table()));
  table = OrderedHashSet::EnsureGrowable(table);
  holder->set_table(*table);
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_SetShrink) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSSet, holder, 0);
  Handle<OrderedHashSet> table(OrderedHashSet::cast(holder->table()));
  table = OrderedHashSet::Shrink(table);
  holder->set_table(*table);
  return isolate->heap()->undefined_value();
}

RUNTIME_FUNCTION(Runtime_SetIteratorClone) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSSetIterator, holder, 0);
  return *isolate->factory()->NewJSSetIterator(
      handle(holder->map(), isolate),
      handle(OrderedHashSet::cast(holder->table()), isolate),
      Smi::ToInt(holder->index()));
}

RUNTIME_FUNCTION(Runtime_MapShrink) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSMap, holder, 0);
  Handle<OrderedHashMap> table(OrderedHashMap::cast(holder->table()));
  table = OrderedHashMap::Shrink(table);
  holder->set_table(*table);
  return isolate->heap()->undefined_value();
}

RUNTIME_FUNCTION(Runtime_MapGrow) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSMap, holder, 0);
  Handle<OrderedHashMap> table(OrderedHashMap::cast(holder->table()));
  table = OrderedHashMap::EnsureGrowable(table);
  holder->set_table(*table);
  return isolate->heap()->undefined_value();
}

RUNTIME_FUNCTION(Runtime_MapIteratorClone) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSMapIterator, holder, 0);
  return *isolate->factory()->NewJSMapIterator(
      handle(holder->map(), isolate),
      handle(OrderedHashMap::cast(holder->table()), isolate),
      Smi::ToInt(holder->index()));
}

RUNTIME_FUNCTION(Runtime_GetWeakMapEntries) {
  HandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSWeakCollection, holder, 0);
  CONVERT_NUMBER_CHECKED(int, max_entries, Int32, args[1]);
  CHECK(max_entries >= 0);
  return *JSWeakCollection::GetEntries(holder, max_entries);
}

RUNTIME_FUNCTION(Runtime_WeakCollectionInitialize) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSWeakCollection, weak_collection, 0);
  JSWeakCollection::Initialize(weak_collection, isolate);
  return *weak_collection;
}


RUNTIME_FUNCTION(Runtime_WeakCollectionDelete) {
  HandleScope scope(isolate);
  DCHECK_EQ(3, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSWeakCollection, weak_collection, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 1);
  CONVERT_SMI_ARG_CHECKED(hash, 2)
  CHECK(key->IsJSReceiver() || key->IsSymbol());
  Handle<ObjectHashTable> table(
      ObjectHashTable::cast(weak_collection->table()));
  CHECK(table->IsKey(isolate, *key));
  bool was_present = JSWeakCollection::Delete(weak_collection, key, hash);
  return isolate->heap()->ToBoolean(was_present);
}


RUNTIME_FUNCTION(Runtime_WeakCollectionSet) {
  HandleScope scope(isolate);
  DCHECK_EQ(4, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSWeakCollection, weak_collection, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, key, 1);
  CHECK(key->IsJSReceiver() || key->IsSymbol());
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 2);
  CONVERT_SMI_ARG_CHECKED(hash, 3)
  Handle<ObjectHashTable> table(
      ObjectHashTable::cast(weak_collection->table()));
  CHECK(table->IsKey(isolate, *key));
  JSWeakCollection::Set(weak_collection, key, value, hash);
  return *weak_collection;
}


RUNTIME_FUNCTION(Runtime_GetWeakSetValues) {
  HandleScope scope(isolate);
  DCHECK_EQ(2, args.length());
  CONVERT_ARG_HANDLE_CHECKED(JSWeakCollection, holder, 0);
  CONVERT_NUMBER_CHECKED(int, max_values, Int32, args[1]);
  CHECK(max_values >= 0);
  return *JSWeakCollection::GetEntries(holder, max_values);
}

RUNTIME_FUNCTION(Runtime_IsJSMap) {
  SealHandleScope shs(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_CHECKED(Object, obj, 0);
  return isolate->heap()->ToBoolean(obj->IsJSMap());
}

RUNTIME_FUNCTION(Runtime_IsJSSet) {
  SealHandleScope shs(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_CHECKED(Object, obj, 0);
  return isolate->heap()->ToBoolean(obj->IsJSSet());
}

RUNTIME_FUNCTION(Runtime_IsJSWeakMap) {
  SealHandleScope shs(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_CHECKED(Object, obj, 0);
  return isolate->heap()->ToBoolean(obj->IsJSWeakMap());
}

RUNTIME_FUNCTION(Runtime_IsJSWeakSet) {
  SealHandleScope shs(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_CHECKED(Object, obj, 0);
  return isolate->heap()->ToBoolean(obj->IsJSWeakSet());
}

}  // namespace internal
}  // namespace v8
