// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Note 1: Any file that includes this one should include object-macros-undef.h
// at the bottom.

// Note 2: This file is deliberately missing the include guards (the undeffing
// approach wouldn't work otherwise).

// The accessors with RELAXED_, ACQUIRE_, and RELEASE_ prefixes should be used
// for fields that can be written to and read from multiple threads at the same
// time. See comments in src/base/atomicops.h for the memory ordering sematics.

#define DECL_BOOLEAN_ACCESSORS(name) \
  inline bool name() const;          \
  inline void set_##name(bool value);

#define DECL_INT_ACCESSORS(name) \
  inline int name() const;       \
  inline void set_##name(int value);

#define DECL_INT32_ACCESSORS(name) \
  inline int32_t name() const;     \
  inline void set_##name(int32_t value);

#define DECL_ACCESSORS(name, type)    \
  inline type* name() const;          \
  inline void set_##name(type* value, \
                         WriteBarrierMode mode = UPDATE_WRITE_BARRIER);

#define DECL_CAST(type)                      \
  INLINE(static type* cast(Object* object)); \
  INLINE(static const type* cast(const Object* object));

#define CAST_ACCESSOR(type)                       \
  type* type::cast(Object* object) {              \
    SLOW_DCHECK(object->Is##type());              \
    return reinterpret_cast<type*>(object);       \
  }                                               \
  const type* type::cast(const Object* object) {  \
    SLOW_DCHECK(object->Is##type());              \
    return reinterpret_cast<const type*>(object); \
  }

#define INT_ACCESSORS(holder, name, offset)                         \
  int holder::name() const { return READ_INT_FIELD(this, offset); } \
  void holder::set_##name(int value) { WRITE_INT_FIELD(this, offset, value); }

#define INT32_ACCESSORS(holder, name, offset)                             \
  int32_t holder::name() const { return READ_INT32_FIELD(this, offset); } \
  void holder::set_##name(int32_t value) {                                \
    WRITE_INT32_FIELD(this, offset, value);                               \
  }

#define ACCESSORS_CHECKED2(holder, name, type, offset, get_condition, \
                           set_condition)                             \
  type* holder::name() const {                                        \
    DCHECK(get_condition);                                            \
    return type::cast(READ_FIELD(this, offset));                      \
  }                                                                   \
  void holder::set_##name(type* value, WriteBarrierMode mode) {       \
    DCHECK(set_condition);                                            \
    WRITE_FIELD(this, offset, value);                                 \
    CONDITIONAL_WRITE_BARRIER(GetHeap(), this, offset, value, mode);  \
  }
#define ACCESSORS_CHECKED(holder, name, type, offset, condition) \
  ACCESSORS_CHECKED2(holder, name, type, offset, condition, condition)

#define ACCESSORS(holder, name, type, offset) \
  ACCESSORS_CHECKED(holder, name, type, offset, true)

// Getter that returns a Smi as an int and writes an int as a Smi.
#define SMI_ACCESSORS_CHECKED(holder, name, offset, condition) \
  int holder::name() const {                                   \
    DCHECK(condition);                                         \
    Object* value = READ_FIELD(this, offset);                  \
    return Smi::ToInt(value);                                  \
  }                                                            \
  void holder::set_##name(int value) {                         \
    DCHECK(condition);                                         \
    WRITE_FIELD(this, offset, Smi::FromInt(value));            \
  }

#define SMI_ACCESSORS(holder, name, offset) \
  SMI_ACCESSORS_CHECKED(holder, name, offset, true)

#define SYNCHRONIZED_SMI_ACCESSORS(holder, name, offset)    \
  int holder::synchronized_##name() const {                 \
    Object* value = ACQUIRE_READ_FIELD(this, offset);       \
    return Smi::ToInt(value);                               \
  }                                                         \
  void holder::synchronized_set_##name(int value) {         \
    RELEASE_WRITE_FIELD(this, offset, Smi::FromInt(value)); \
  }

#define RELAXED_SMI_ACCESSORS(holder, name, offset)         \
  int holder::relaxed_read_##name() const {                 \
    Object* value = RELAXED_READ_FIELD(this, offset);       \
    return Smi::ToInt(value);                               \
  }                                                         \
  void holder::relaxed_write_##name(int value) {            \
    RELAXED_WRITE_FIELD(this, offset, Smi::FromInt(value)); \
  }

#define BOOL_GETTER(holder, field, name, offset) \
  bool holder::name() const { return BooleanBit::get(field(), offset); }

#define BOOL_ACCESSORS(holder, field, name, offset)                      \
  bool holder::name() const { return BooleanBit::get(field(), offset); } \
  void holder::set_##name(bool value) {                                  \
    set_##field(BooleanBit::set(field(), offset, value));                \
  }

#define BIT_FIELD_ACCESSORS(holder, field, name, BitField)      \
  typename BitField::FieldType holder::name() const {           \
    return BitField::decode(field());                           \
  }                                                             \
  void holder::set_##name(typename BitField::FieldType value) { \
    set_##field(BitField::update(field(), value));              \
  }

#define TYPE_CHECKER(type, instancetype)           \
  bool HeapObject::Is##type() const {              \
    return map()->instance_type() == instancetype; \
  }

#define FIELD_ADDR(p, offset) \
  (reinterpret_cast<byte*>(p) + offset - kHeapObjectTag)

#define FIELD_ADDR_CONST(p, offset) \
  (reinterpret_cast<const byte*>(p) + offset - kHeapObjectTag)

#define READ_FIELD(p, offset) \
  (*reinterpret_cast<Object* const*>(FIELD_ADDR_CONST(p, offset)))

#define ACQUIRE_READ_FIELD(p, offset)           \
  reinterpret_cast<Object*>(base::Acquire_Load( \
      reinterpret_cast<const base::AtomicWord*>(FIELD_ADDR_CONST(p, offset))))

#define RELAXED_READ_FIELD(p, offset)           \
  reinterpret_cast<Object*>(base::Relaxed_Load( \
      reinterpret_cast<const base::AtomicWord*>(FIELD_ADDR_CONST(p, offset))))

#ifdef V8_CONCURRENT_MARKING
#define WRITE_FIELD(p, offset, value)                             \
  base::Relaxed_Store(                                            \
      reinterpret_cast<base::AtomicWord*>(FIELD_ADDR(p, offset)), \
      reinterpret_cast<base::AtomicWord>(value));
#else
#define WRITE_FIELD(p, offset, value) \
  (*reinterpret_cast<Object**>(FIELD_ADDR(p, offset)) = value)
#endif

#define RELEASE_WRITE_FIELD(p, offset, value)                     \
  base::Release_Store(                                            \
      reinterpret_cast<base::AtomicWord*>(FIELD_ADDR(p, offset)), \
      reinterpret_cast<base::AtomicWord>(value));

#define RELAXED_WRITE_FIELD(p, offset, value)                     \
  base::Relaxed_Store(                                            \
      reinterpret_cast<base::AtomicWord*>(FIELD_ADDR(p, offset)), \
      reinterpret_cast<base::AtomicWord>(value));

#define WRITE_BARRIER(heap, object, offset, value)          \
  heap->incremental_marking()->RecordWrite(                 \
      object, HeapObject::RawField(object, offset), value); \
  heap->RecordWrite(object, HeapObject::RawField(object, offset), value);

#define CONDITIONAL_WRITE_BARRIER(heap, object, offset, value, mode)        \
  if (mode != SKIP_WRITE_BARRIER) {                                         \
    if (mode == UPDATE_WRITE_BARRIER) {                                     \
      heap->incremental_marking()->RecordWrite(                             \
          object, HeapObject::RawField(object, offset), value);             \
    }                                                                       \
    heap->RecordWrite(object, HeapObject::RawField(object, offset), value); \
  }

#define READ_DOUBLE_FIELD(p, offset) \
  ReadDoubleValue(FIELD_ADDR_CONST(p, offset))

#define WRITE_DOUBLE_FIELD(p, offset, value) \
  WriteDoubleValue(FIELD_ADDR(p, offset), value)

#define READ_INT_FIELD(p, offset) \
  (*reinterpret_cast<const int*>(FIELD_ADDR_CONST(p, offset)))

#define WRITE_INT_FIELD(p, offset, value) \
  (*reinterpret_cast<int*>(FIELD_ADDR(p, offset)) = value)

#define RELAXED_READ_INTPTR_FIELD(p, offset) \
  static_cast<intptr_t>(base::Relaxed_Load(  \
      reinterpret_cast<const base::AtomicWord*>(FIELD_ADDR_CONST(p, offset))))

#define READ_INTPTR_FIELD(p, offset) \
  (*reinterpret_cast<const intptr_t*>(FIELD_ADDR_CONST(p, offset)))

#define RELAXED_WRITE_INTPTR_FIELD(p, offset, value)              \
  base::Relaxed_Store(                                            \
      reinterpret_cast<base::AtomicWord*>(FIELD_ADDR(p, offset)), \
      static_cast<base::AtomicWord>(value));

#define WRITE_INTPTR_FIELD(p, offset, value) \
  (*reinterpret_cast<intptr_t*>(FIELD_ADDR(p, offset)) = value)

#define READ_UINT8_FIELD(p, offset) \
  (*reinterpret_cast<const uint8_t*>(FIELD_ADDR_CONST(p, offset)))

#define WRITE_UINT8_FIELD(p, offset, value) \
  (*reinterpret_cast<uint8_t*>(FIELD_ADDR(p, offset)) = value)

#define RELAXED_WRITE_INT8_FIELD(p, offset, value)                             \
  base::Relaxed_Store(reinterpret_cast<base::Atomic8*>(FIELD_ADDR(p, offset)), \
                      static_cast<base::Atomic8>(value));

#define READ_INT8_FIELD(p, offset) \
  (*reinterpret_cast<const int8_t*>(FIELD_ADDR_CONST(p, offset)))

#define RELAXED_READ_INT8_FIELD(p, offset) \
  static_cast<int8_t>(base::Relaxed_Load(  \
      reinterpret_cast<const base::Atomic8*>(FIELD_ADDR_CONST(p, offset))))

#define WRITE_INT8_FIELD(p, offset, value) \
  (*reinterpret_cast<int8_t*>(FIELD_ADDR(p, offset)) = value)

#define READ_UINT16_FIELD(p, offset) \
  (*reinterpret_cast<const uint16_t*>(FIELD_ADDR_CONST(p, offset)))

#define WRITE_UINT16_FIELD(p, offset, value) \
  (*reinterpret_cast<uint16_t*>(FIELD_ADDR(p, offset)) = value)

#define READ_INT16_FIELD(p, offset) \
  (*reinterpret_cast<const int16_t*>(FIELD_ADDR_CONST(p, offset)))

#define WRITE_INT16_FIELD(p, offset, value) \
  (*reinterpret_cast<int16_t*>(FIELD_ADDR(p, offset)) = value)

#define READ_UINT32_FIELD(p, offset) \
  (*reinterpret_cast<const uint32_t*>(FIELD_ADDR_CONST(p, offset)))

#define WRITE_UINT32_FIELD(p, offset, value) \
  (*reinterpret_cast<uint32_t*>(FIELD_ADDR(p, offset)) = value)

#define READ_INT32_FIELD(p, offset) \
  (*reinterpret_cast<const int32_t*>(FIELD_ADDR_CONST(p, offset)))

#define WRITE_INT32_FIELD(p, offset, value) \
  (*reinterpret_cast<int32_t*>(FIELD_ADDR(p, offset)) = value)

#define READ_FLOAT_FIELD(p, offset) \
  (*reinterpret_cast<const float*>(FIELD_ADDR_CONST(p, offset)))

#define WRITE_FLOAT_FIELD(p, offset, value) \
  (*reinterpret_cast<float*>(FIELD_ADDR(p, offset)) = value)

#define READ_UINT64_FIELD(p, offset) \
  (*reinterpret_cast<const uint64_t*>(FIELD_ADDR_CONST(p, offset)))

#define WRITE_UINT64_FIELD(p, offset, value) \
  (*reinterpret_cast<uint64_t*>(FIELD_ADDR(p, offset)) = value)

#define READ_INT64_FIELD(p, offset) \
  (*reinterpret_cast<const int64_t*>(FIELD_ADDR_CONST(p, offset)))

#define WRITE_INT64_FIELD(p, offset, value) \
  (*reinterpret_cast<int64_t*>(FIELD_ADDR(p, offset)) = value)

#define READ_BYTE_FIELD(p, offset) \
  (*reinterpret_cast<const byte*>(FIELD_ADDR_CONST(p, offset)))

#define RELAXED_READ_BYTE_FIELD(p, offset) \
  static_cast<byte>(base::Relaxed_Load(    \
      reinterpret_cast<const base::Atomic8*>(FIELD_ADDR_CONST(p, offset))))

#define WRITE_BYTE_FIELD(p, offset, value) \
  (*reinterpret_cast<byte*>(FIELD_ADDR(p, offset)) = value)

#define RELAXED_WRITE_BYTE_FIELD(p, offset, value)                             \
  base::Relaxed_Store(reinterpret_cast<base::Atomic8*>(FIELD_ADDR(p, offset)), \
                      static_cast<base::Atomic8>(value));

#ifdef VERIFY_HEAP
#define DECL_VERIFIER(Name) void Name##Verify();
#else
#define DECL_VERIFIER(Name)
#endif

#define DEFINE_DEOPT_ELEMENT_ACCESSORS(name, type)       \
  type* DeoptimizationInputData::name() {                \
    return type::cast(get(k##name##Index));              \
  }                                                      \
  void DeoptimizationInputData::Set##name(type* value) { \
    set(k##name##Index, value);                          \
  }

#define DEFINE_DEOPT_ENTRY_ACCESSORS(name, type)                \
  type* DeoptimizationInputData::name(int i) {                  \
    return type::cast(get(IndexForEntry(i) + k##name##Offset)); \
  }                                                             \
  void DeoptimizationInputData::Set##name(int i, type* value) { \
    set(IndexForEntry(i) + k##name##Offset, value);             \
  }
