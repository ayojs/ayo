// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_OBJECTS_SHARED_FUNCTION_INFO_H_
#define V8_OBJECTS_SHARED_FUNCTION_INFO_H_

#include "src/objects.h"
#include "src/objects/script.h"

// Has to be the last include (doesn't have include guards):
#include "src/objects/object-macros.h"

namespace v8 {
namespace internal {

class CoverageInfo;
class DebugInfo;

class PreParsedScopeData : public Struct {
 public:
  DECL_ACCESSORS(scope_data, PodArray<uint32_t>)
  DECL_ACCESSORS(child_data, FixedArray)

  static const int kScopeDataOffset = Struct::kHeaderSize;
  static const int kChildDataOffset = kScopeDataOffset + kPointerSize;
  static const int kSize = kChildDataOffset + kPointerSize;

  DECL_CAST(PreParsedScopeData)
  DECL_PRINTER(PreParsedScopeData)
  DECL_VERIFIER(PreParsedScopeData)

 private:
  DISALLOW_IMPLICIT_CONSTRUCTORS(PreParsedScopeData);
};

// SharedFunctionInfo describes the JSFunction information that can be
// shared by multiple instances of the function.
class SharedFunctionInfo : public HeapObject {
 public:
  static constexpr Object* const kNoSharedNameSentinel = Smi::kZero;

  // [name]: Returns shared name if it exists or an empty string otherwise.
  inline String* name() const;
  inline void set_name(String* name);

  // [code]: Function code.
  DECL_ACCESSORS(code, Code)

  // Get the abstract code associated with the function, which will either be
  // a Code object or a BytecodeArray.
  inline AbstractCode* abstract_code();

  // Tells whether or not this shared function info is interpreted.
  //
  // Note: function->IsInterpreted() does not necessarily return the same value
  // as function->shared()->IsInterpreted() because the closure might have been
  // optimized.
  inline bool IsInterpreted() const;

  inline void ReplaceCode(Code* code);
  inline bool HasBaselineCode() const;

  // Set up the link between shared function info and the script. The shared
  // function info is added to the list on the script.
  V8_EXPORT_PRIVATE static void SetScript(
      Handle<SharedFunctionInfo> shared, Handle<Object> script_object,
      bool reset_preparsed_scope_data = true);

  // Layout description of the optimized code map.
  static const int kEntriesStart = 0;
  static const int kContextOffset = 0;
  static const int kCachedCodeOffset = 1;
  static const int kEntryLength = 2;
  static const int kInitialLength = kEntriesStart + kEntryLength;

  static const int kNotFound = -1;
  static const int kInvalidLength = -1;

  // Helpers for assembly code that does a backwards walk of the optimized code
  // map.
  static const int kOffsetToPreviousContext =
      FixedArray::kHeaderSize + kPointerSize * (kContextOffset - kEntryLength);
  static const int kOffsetToPreviousCachedCode =
      FixedArray::kHeaderSize +
      kPointerSize * (kCachedCodeOffset - kEntryLength);

  // [scope_info]: Scope info.
  DECL_ACCESSORS(scope_info, ScopeInfo)

  // The outer scope info for the purpose of parsing this function, or the hole
  // value if it isn't yet known.
  DECL_ACCESSORS(outer_scope_info, HeapObject)

  // [construct stub]: Code stub for constructing instances of this function.
  DECL_ACCESSORS(construct_stub, Code)

  // Sets the given code as the construct stub, and marks builtin code objects
  // as a construct stub.
  void SetConstructStub(Code* code);

  // Returns if this function has been compiled to native code yet.
  inline bool is_compiled() const;

  // [length]: The function length - usually the number of declared parameters.
  // Use up to 2^30 parameters. The value is only reliable when the function has
  // been compiled.
  inline int GetLength() const;
  inline bool HasLength() const;
  inline void set_length(int value);

  // [internal formal parameter count]: The declared number of parameters.
  // For subclass constructors, also includes new.target.
  // The size of function's frame is internal_formal_parameter_count + 1.
  DECL_INT_ACCESSORS(internal_formal_parameter_count)

  // Set the formal parameter count so the function code will be
  // called without using argument adaptor frames.
  inline void DontAdaptArguments();

  // [expected_nof_properties]: Expected number of properties for the
  // function. The value is only reliable when the function has been compiled.
  DECL_INT_ACCESSORS(expected_nof_properties)

  // [feedback_metadata] - describes ast node feedback from full-codegen and
  // (increasingly) from crankshafted code where sufficient feedback isn't
  // available.
  DECL_ACCESSORS(feedback_metadata, FeedbackMetadata)

  // [function_literal_id] - uniquely identifies the FunctionLiteral this
  // SharedFunctionInfo represents within its script, or -1 if this
  // SharedFunctionInfo object doesn't correspond to a parsed FunctionLiteral.
  DECL_INT_ACCESSORS(function_literal_id)

#if V8_SFI_HAS_UNIQUE_ID
  // [unique_id] - For --trace-maps purposes, an identifier that's persistent
  // even if the GC moves this SharedFunctionInfo.
  DECL_INT_ACCESSORS(unique_id)
#endif

  // [instance class name]: class name for instances.
  DECL_ACCESSORS(instance_class_name, Object)

  // [function data]: This field holds some additional data for function.
  // Currently it has one of:
  //  - a FunctionTemplateInfo to make benefit the API [IsApiFunction()].
  //  - a BytecodeArray for the interpreter [HasBytecodeArray()].
  //  - a FixedArray with Asm->Wasm conversion [HasAsmWasmData()].
  DECL_ACCESSORS(function_data, Object)

  inline bool IsApiFunction();
  inline FunctionTemplateInfo* get_api_func_data();
  inline void set_api_func_data(FunctionTemplateInfo* data);
  inline bool HasBytecodeArray() const;
  inline BytecodeArray* bytecode_array() const;
  inline void set_bytecode_array(BytecodeArray* bytecode);
  inline void ClearBytecodeArray();
  inline bool HasAsmWasmData() const;
  inline FixedArray* asm_wasm_data() const;
  inline void set_asm_wasm_data(FixedArray* data);
  inline void ClearAsmWasmData();

  // [function identifier]: This field holds an additional identifier for the
  // function.
  //  - a Smi identifying a builtin function [HasBuiltinFunctionId()].
  //  - a String identifying the function's inferred name [HasInferredName()].
  // The inferred_name is inferred from variable or property
  // assignment of this function. It is used to facilitate debugging and
  // profiling of JavaScript code written in OO style, where almost
  // all functions are anonymous but are assigned to object
  // properties.
  DECL_ACCESSORS(function_identifier, Object)

  inline bool HasBuiltinFunctionId();
  inline BuiltinFunctionId builtin_function_id();
  inline void set_builtin_function_id(BuiltinFunctionId id);
  inline bool HasInferredName();
  inline String* inferred_name();
  inline void set_inferred_name(String* inferred_name);

  // [script]: Script from which the function originates.
  DECL_ACCESSORS(script, Object)

  // [start_position_and_type]: Field used to store both the source code
  // position, whether or not the function is a function expression,
  // and whether or not the function is a toplevel function. The two
  // least significants bit indicates whether the function is an
  // expression and the rest contains the source code position.
  DECL_INT_ACCESSORS(start_position_and_type)

  // The function is subject to debugging if a debug info is attached.
  inline bool HasDebugInfo() const;
  DebugInfo* GetDebugInfo() const;

  // Break infos are contained in DebugInfo, this is a convenience method
  // to simplify access.
  bool HasBreakInfo() const;

  // Coverage infos are contained in DebugInfo, this is a convenience method
  // to simplify access.
  bool HasCoverageInfo() const;
  CoverageInfo* GetCoverageInfo() const;

  // [debug info]: Debug information.
  DECL_ACCESSORS(debug_info, Object)

  // PreParsedScopeData or null.
  DECL_ACCESSORS(preparsed_scope_data, Object)

  inline bool HasPreParsedScopeData() const;

  // Bit field containing various information collected for debugging.
  // This field is either stored on the kDebugInfo slot or inside the
  // debug info struct.
  int debugger_hints() const;
  void set_debugger_hints(int value);

  // Indicates that the function was created by the Function function.
  // Though it's anonymous, toString should treat it as if it had the name
  // "anonymous".  We don't set the name itself so that the system does not
  // see a binding for it.
  DECL_BOOLEAN_ACCESSORS(name_should_print_as_anonymous)

  // Indicates that the function is either an anonymous expression
  // or an arrow function (the name field can be set through the API,
  // which does not change this flag).
  DECL_BOOLEAN_ACCESSORS(is_anonymous_expression)

  // Indicates that the the shared function info is deserialized from cache.
  DECL_BOOLEAN_ACCESSORS(deserialized)

  // Indicates that the function cannot cause side-effects.
  DECL_BOOLEAN_ACCESSORS(has_no_side_effect)

  // Indicates that |has_no_side_effect| has been computed and set.
  DECL_BOOLEAN_ACCESSORS(computed_has_no_side_effect)

  // Indicates that the function should be skipped during stepping.
  DECL_BOOLEAN_ACCESSORS(debug_is_blackboxed)

  // Indicates that |debug_is_blackboxed| has been computed and set.
  DECL_BOOLEAN_ACCESSORS(computed_debug_is_blackboxed)

  // Indicates that the function has been reported for binary code coverage.
  DECL_BOOLEAN_ACCESSORS(has_reported_binary_coverage)

  // The function's name if it is non-empty, otherwise the inferred name.
  String* DebugName();

  // The function cannot cause any side effects.
  bool HasNoSideEffect();

  // Used for flags such as --turbo-filter.
  bool PassesFilter(const char* raw_filter);

  // Position of the 'function' token in the script source.
  DECL_INT_ACCESSORS(function_token_position)

  // Position of this function in the script source.
  DECL_INT_ACCESSORS(start_position)

  // End position of this function in the script source.
  DECL_INT_ACCESSORS(end_position)

  // Returns true if the function has shared name.
  inline bool has_shared_name() const;

  // Is this function a named function expression in the source code.
  DECL_BOOLEAN_ACCESSORS(is_named_expression)

  // Is this function a top-level function (scripts, evals).
  DECL_BOOLEAN_ACCESSORS(is_toplevel)

  // Bit field containing various information collected by the compiler to
  // drive optimization.
  DECL_INT_ACCESSORS(compiler_hints)

  // Indicates if this function can be lazy compiled.
  DECL_BOOLEAN_ACCESSORS(allows_lazy_compilation)

  // Indicates the language mode.
  inline LanguageMode language_mode();
  inline void set_language_mode(LanguageMode language_mode);

  // False if the function definitely does not allocate an arguments object.
  DECL_BOOLEAN_ACCESSORS(uses_arguments)

  // True if the function has any duplicated parameter names.
  DECL_BOOLEAN_ACCESSORS(has_duplicate_parameters)

  // Indicates whether the function is a native function.
  // These needs special treatment in .call and .apply since
  // null passed as the receiver should not be translated to the
  // global object.
  DECL_BOOLEAN_ACCESSORS(native)

  // Indicate that this function should always be inlined in optimized code.
  DECL_BOOLEAN_ACCESSORS(force_inline)

  // Whether this function was created from a FunctionDeclaration.
  DECL_BOOLEAN_ACCESSORS(is_declaration)

  // Indicates that asm->wasm conversion failed and should not be re-attempted.
  DECL_BOOLEAN_ACCESSORS(is_asm_wasm_broken)

  inline FunctionKind kind() const;

  // Defines the index in a native context of closure's map instantiated using
  // this shared function info.
  DECL_INT_ACCESSORS(function_map_index)

  // Clear uninitialized padding space. This ensures that the snapshot content
  // is deterministic.
  inline void clear_padding();

  // Recalculates the |map_index| value after modifications of this shared info.
  inline void UpdateFunctionMapIndex();

  // Indicates whether optimizations have been disabled for this shared function
  // info. If we cannot optimize the function we disable optimization to avoid
  // spending time attempting to optimize it again.
  inline bool optimization_disabled() const;

  // The reason why optimization was disabled.
  inline BailoutReason disable_optimization_reason() const;

  // Disable (further) attempted optimization of all functions sharing this
  // shared function info.
  void DisableOptimization(BailoutReason reason);

  // [source code]: Source code for the function.
  bool HasSourceCode() const;
  Handle<Object> GetSourceCode();
  Handle<Object> GetSourceCodeHarmony();

  // Tells whether this function should be subject to debugging.
  inline bool IsSubjectToDebugging();

  // Whether this function is defined in user-provided JavaScript code.
  inline bool IsUserJavaScript();

  // Check whether or not this function is inlineable.
  bool IsInlineable();

  // Source size of this function.
  int SourceSize();

  // Returns `false` if formal parameters include rest parameters, optional
  // parameters, or destructuring parameters.
  // TODO(caitp): make this a flag set during parsing
  inline bool has_simple_parameters();

  // Initialize a SharedFunctionInfo from a parsed function literal.
  static void InitFromFunctionLiteral(Handle<SharedFunctionInfo> shared_info,
                                      FunctionLiteral* lit);

  // Sets the expected number of properties based on estimate from parser.
  void SetExpectedNofPropertiesFromEstimate(FunctionLiteral* literal);

  // Dispatched behavior.
  DECL_PRINTER(SharedFunctionInfo)
  DECL_VERIFIER(SharedFunctionInfo)
#ifdef OBJECT_PRINT
  void PrintSourceCode(std::ostream& os);
#endif

  // Iterate over all shared function infos in a given script.
  class ScriptIterator {
   public:
    explicit ScriptIterator(Handle<Script> script);
    ScriptIterator(Isolate* isolate, Handle<FixedArray> shared_function_infos);
    SharedFunctionInfo* Next();

    // Reset the iterator to run on |script|.
    void Reset(Handle<Script> script);

   private:
    Isolate* isolate_;
    Handle<FixedArray> shared_function_infos_;
    int index_;
    DISALLOW_COPY_AND_ASSIGN(ScriptIterator);
  };

  // Iterate over all shared function infos on the heap.
  class GlobalIterator {
   public:
    explicit GlobalIterator(Isolate* isolate);
    SharedFunctionInfo* Next();

   private:
    Script::Iterator script_iterator_;
    WeakFixedArray::Iterator noscript_sfi_iterator_;
    SharedFunctionInfo::ScriptIterator sfi_iterator_;
    DisallowHeapAllocation no_gc_;
    DISALLOW_COPY_AND_ASSIGN(GlobalIterator);
  };

  DECL_CAST(SharedFunctionInfo)

  // Constants.
  static const int kDontAdaptArgumentsSentinel = -1;

#if V8_SFI_HAS_UNIQUE_ID
  static const int kUniqueIdFieldSize = kInt32Size;
#else
  // Just to not break the postmortrem support with conditional offsets
  static const int kUniqueIdFieldSize = 0;
#endif

// Layout description.
#define SHARED_FUNCTION_INFO_FIELDS(V)        \
  /* Pointer fields. */                       \
  V(kCodeOffset, kPointerSize)                \
  V(kNameOffset, kPointerSize)                \
  V(kScopeInfoOffset, kPointerSize)           \
  V(kOuterScopeInfoOffset, kPointerSize)      \
  V(kConstructStubOffset, kPointerSize)       \
  V(kInstanceClassNameOffset, kPointerSize)   \
  V(kFunctionDataOffset, kPointerSize)        \
  V(kScriptOffset, kPointerSize)              \
  V(kDebugInfoOffset, kPointerSize)           \
  V(kFunctionIdentifierOffset, kPointerSize)  \
  V(kFeedbackMetadataOffset, kPointerSize)    \
  V(kPreParsedScopeDataOffset, kPointerSize)  \
  V(kEndOfPointerFieldsOffset, 0)             \
  /* Raw data fields. */                      \
  V(kFunctionLiteralIdOffset, kInt32Size)     \
  V(kUniqueIdOffset, kUniqueIdFieldSize)      \
  V(kLengthOffset, kInt32Size)                \
  V(kFormalParameterCountOffset, kInt32Size)  \
  V(kExpectedNofPropertiesOffset, kInt32Size) \
  V(kStartPositionAndTypeOffset, kInt32Size)  \
  V(kEndPositionOffset, kInt32Size)           \
  V(kFunctionTokenPositionOffset, kInt32Size) \
  V(kCompilerHintsOffset, kInt32Size)         \
  /* Total size. */                           \
  V(kSize, 0)

  DEFINE_FIELD_OFFSET_CONSTANTS(HeapObject::kHeaderSize,
                                SHARED_FUNCTION_INFO_FIELDS)

  static const int kAlignedSize = POINTER_SIZE_ALIGN(kSize);

  typedef FixedBodyDescriptor<kCodeOffset, kEndOfPointerFieldsOffset, kSize>
      BodyDescriptor;
  // No weak fields.
  typedef BodyDescriptor BodyDescriptorWeak;

// Bit fields in |start_position_and_type|.
#define START_POSITION_AND_TYPE_BIT_FIELDS(V, _) \
  V(IsNamedExpressionBit, bool, 1, _)            \
  V(IsTopLevelBit, bool, 1, _)                   \
  V(StartPositionBits, int, 30, _)

  DEFINE_BIT_FIELDS(START_POSITION_AND_TYPE_BIT_FIELDS)
#undef START_POSITION_AND_TYPE_BIT_FIELDS

// Bit positions in |compiler_hints|.
#define COMPILER_HINTS_BIT_FIELDS(V, _)    \
  V(IsNativeBit, bool, 1, _)               \
  V(IsStrictBit, bool, 1, _)               \
  V(FunctionKindBits, FunctionKind, 10, _) \
  V(HasDuplicateParametersBit, bool, 1, _) \
  V(AllowLazyCompilationBit, bool, 1, _)   \
  V(UsesArgumentsBit, bool, 1, _)          \
  V(NeedsHomeObjectBit, bool, 1, _)        \
  V(ForceInlineBit, bool, 1, _)            \
  V(IsDeclarationBit, bool, 1, _)          \
  V(IsAsmWasmBrokenBit, bool, 1, _)        \
  V(FunctionMapIndexBits, int, 5, _)       \
  V(DisabledOptimizationReasonBits, BailoutReason, 7, _)

  DEFINE_BIT_FIELDS(COMPILER_HINTS_BIT_FIELDS)
#undef COMPILER_HINTS_BIT_FIELDS

  // Bailout reasons must fit in the DisabledOptimizationReason bitfield.
  STATIC_ASSERT(kLastErrorMessage <= DisabledOptimizationReasonBits::kMax);

  // Masks for checking if certain FunctionKind bits are set without fully
  // decoding of the FunctionKind bit field.
  static const int kClassConstructorMask = FunctionKind::kClassConstructor
                                           << FunctionKindBits::kShift;
  static const int kDerivedConstructorMask = FunctionKind::kDerivedConstructor
                                             << FunctionKindBits::kShift;

// Bit positions in |debugger_hints|.
#define DEBUGGER_HINTS_BIT_FIELDS(V, _)        \
  V(IsAnonymousExpressionBit, bool, 1, _)      \
  V(NameShouldPrintAsAnonymousBit, bool, 1, _) \
  V(IsDeserializedBit, bool, 1, _)             \
  V(HasNoSideEffectBit, bool, 1, _)            \
  V(ComputedHasNoSideEffectBit, bool, 1, _)    \
  V(DebugIsBlackboxedBit, bool, 1, _)          \
  V(ComputedDebugIsBlackboxedBit, bool, 1, _)  \
  V(HasReportedBinaryCoverageBit, bool, 1, _)

  DEFINE_BIT_FIELDS(DEBUGGER_HINTS_BIT_FIELDS)
#undef DEBUGGER_HINTS_BIT_FIELDS

 private:
  // [raw_name]: Function name string or kNoSharedNameSentinel.
  DECL_ACCESSORS(raw_name, Object)

  inline void set_kind(FunctionKind kind);

  // Indicates that this function uses a super property (or an eval that may
  // use a super property).
  // This is needed to set up the [[HomeObject]] on the function instance.
  DECL_BOOLEAN_ACCESSORS(needs_home_object)

  friend class Factory;
  friend class V8HeapExplorer;
  FRIEND_TEST(PreParserTest, LazyFunctionLength);

  inline int length() const;

  DISALLOW_IMPLICIT_CONSTRUCTORS(SharedFunctionInfo);
};

// Result of searching in an optimized code map of a SharedFunctionInfo. Note
// that both {code} and {vector} can be NULL to pass search result status.
struct CodeAndVector {
  Code* code;              // Cached optimized code.
  FeedbackVector* vector;  // Cached feedback vector.
};

// Printing support.
struct SourceCodeOf {
  explicit SourceCodeOf(SharedFunctionInfo* v, int max = -1)
      : value(v), max_length(max) {}
  const SharedFunctionInfo* value;
  int max_length;
};

std::ostream& operator<<(std::ostream& os, const SourceCodeOf& v);

}  // namespace internal
}  // namespace v8

#include "src/objects/object-macros-undef.h"

#endif  // V8_OBJECTS_SHARED_FUNCTION_INFO_H_
