/**

Copyright (c) 2014 National Instruments Corp.
 
This software is subject to the terms described in the LICENSE.TXT file

SDG
*/


#ifndef TypeAndDataManager_h
#define TypeAndDataManager_h

#define STL_MAP
#define VIREO_TRACK_MEMORY_QUANTITY

#include <new>      // for new placement

#ifdef STL_MAP
    #include <map>
#endif

#include <stdio.h>
#include "Platform.h"
#include "DataTypes.h"
#include "StringUtilities.h"
#include "Instruction.h"

namespace Vireo
{

class TypeCommon;
class TypeManager;
class ExecutionContext;
class IDataProcs;
    
typedef TypeCommon  *TypeRef;
typedef TypeManager *TypeManagerRef;

// StaticType is used for functions tha take types determined at load time.
// specifiying StaticType for the parameter will result in the instruction holding a TypeCommon*
// Instead of a TypeRef*
typedef TypeCommon StaticType;

class TypedArrayCore;

template <class T>
class TypedArray1D;

#define TADM_NEW_PLACEMENT(_class_) new (TypeManagerScope::Current()->Malloc(sizeof(_class_))) _class_
#define TADM_NEW_PLACEMENT_DYNAMIC(_class_, _d_) new (TypeManagerScope::Current()->Malloc(_class_::StructSize(_d_))) _class_
    

// EncodingEnum defines the base set of encodings used to annotate the underlying semantics
// of a low level bit block. It is the key for serialization to and from binary, ASCII
// or other formats.
enum EncodingEnum {
    kEncoding_None = 0,
    // Aggregates and References
    kEncoding_Cluster,
    kEncoding_ParameterBlock,
    kEncoding_Array,
    kEncoding_Generic,
    kEncoding_Stream,       //Like array but can't assume random acess.
    
    //Bitblock
    kEncoding_Boolean,
    kEncoding_Bits,
    kEncoding_Enum,
    kEncoding_UInt,
    kEncoding_SInt,
    kEncoding_Int1sCompliment,
    kEncoding_IEEE754Binary,
    kEncoding_Ascii,
    kEncoding_Unicode,
    kEncoding_Pointer,              // Some systems may have more than one pointer type cdoe/data
    
    kEncodingBitFieldSize = 5,   //Room for up to 32 primitive encoding types
};

// UsageTypeEnum defines how parameters in a native function of VIs ParamBlock will be used.
enum UsageTypeEnum {
    kUsageTypeSimple = 0,       // Default for clusters, code assumed to read and write at will, not allowed in ParamBlock
    kUsageTypeInput = 1,        // Caller copies in value, VI will not change it.
    kUsageTypeOutput = 2,       // Caller provides storage(if array) VI sets value, ingores incomming value
    kUsageTypeInputOutput = 3,  // Like output, but VI uses initial value.
    kUsageTypeStatic = 4,       // Allocated value persists from call to call
    kUsageTypeTemp =  5,        // Storage typically carried from call to call but can be freed up.
    kUsageTypeImmediate =  6,   // For native function value in instruction block is imediate value not a pointer
};

// PointerTypeEnum defines the type of internal pointer stored in DefaultPointer type.
enum PointerTypeEnum {
    kPTNotAPointer = 0,
    kPTInt,
    kPTInstructionFunction,
    kPTGenericFucntionPropType,
    kPTGenericFunctionCodeGen,
};

// PointerTypeEnum defines how a pointer to data will be used.
enum PointerAccessEnum {
    kPAInit = 0,
    kPARead = 1,
    kPAWrite = 2,
    kPAReadWrite = 3,
    kPAClear = 4,
};

//------------------------------------------------------------
// When an instruction has a StaticTypeAndData parameter there will be two
// pointers. Instructions that take a VarArg set of StaticTypeAndData arguments
// can treat the block of pointer-pairs as an array of this type.
struct StaticTypeAndData
{
    TypeRef  _paramType;
    void*    _pData;
};

#ifdef STL_MAP
#else
    class DictionaryElt
    {
    public:
        SubString   first;
        TypeRef     second;
    };
    //------------------------------------------------------------
    // Dictionary a bit more hardcoded than map for smaller worlds
    class SimpleDictionary
    {
    public:
        void clear() {};
        DictionaryElt* begin() {return null;};
        DictionaryElt* end() {return null;};
        DictionaryElt* find(SubString& value) {return null;};
        TypeRef& operator[] (const SubString& k) { return _t; };
        Int32 size() {return null;};
    private:
        TypeRef _t;
    };
#endif

inline IntIndex Min(IntIndex a, IntIndex b) { return a < b ? a : b; }
inline IntIndex Max(IntIndex a, IntIndex b) { return a > b ? a : b; }
void PrintType(TypeRef type, const char* message);

//------------------------------------------------------------
class TypeManager
{
public:
    static TypeManager* New(TypeManager* tmParent);
    static void Delete(TypeManager* tm);
    
private:
    TypeManager*    _rootTypeManager;   // null if it is the root, or it is not using a root.

#ifdef STL_MAP
    typedef std::map<SubString, TypeRef, ComapreSubString>::iterator  TypeDictionaryIterator;
    std::map<SubString, TypeRef, ComapreSubString>  _typeNameDictionary;
#else
    typedef DictionaryElt* TypeDictionaryIterator;
    SimpleDictionary       _typeNameDictionary;
#endif

    Int32       _aqBitCount;
    Mutex  		_mutex;
    TypeCommon* _badType;
    TypeCommon* _typeList;          // list of all Types allocated by this TypeManager
    
friend class TDViaParser;
    TypeRef FindTypeInternal(const SubString* name);
        
    // TODO The manager needs to define the Addressable Quantum size (bit in an addressable item, often a octet
    // but some times it is larger (e.g. 16 or 32) the CDC 7600 was 60
    // also defines alignment rules. Each element in a cluster is addressable
private:
    TypeManager(TypeManager* typeManager);
public:
    void    DeleteTypes(Boolean finalTime);
    void    TrackType(TypeCommon* type);
    void    GetTypes(TypedArray1D<TypeRef>*);
    void    PrintMemoryStat(const char*, Boolean last);
    
    TypeManager *RootTypeManager() { return _rootTypeManager; }
    TypeRef Define(SubString* name, TypeRef type);
    TypeRef FindType(const SubString* name);
    void*   FindNamedTypedBlock(SubString* name, PointerAccessEnum mode);
    void*   FindNamedObject(SubString* name);
    TypeRef BadType();

    Int32   AQAlignment(Int32 size);
    Int32   AlignAQOffset(Int32 offset, Int32 size);
    Int32   BitCountToAQSize(Int32 bitCount);
    Int32   PointerToAQSize() {return sizeof(void*); }
    Int32   AQBitSize() {return _aqBitCount; }
    
public:
    NIError RegisterType(const char* name, const char* typeString);
	NIError DefineCustomPointerTypeWithValue(const char* name, void* pointer, TypeRef type, PointerTypeEnum pointerType);
	NIError DefineCustomDataProcs(const char* name, IDataProcs* pDataProcs, TypeRef type);
    
public:
    // Low level allocation functions
    // TODO pull out into its own class.
    void* Malloc(size_t countAQ);
    void* Realloc(void* pBuffer, size_t countAQ, size_t preserveAQ);
    void Free(void* pBuffer);
    
    Boolean AllocationPermitted(size_t countAQ);
    void TrackAllocation(void* id, size_t countAQ, Boolean bAlloc);

    Int32  _totalAllocations;
    Int32  _totalAllocationFailures;
    size_t _totalAQAllocated;
    size_t _maxAllocated;
    size_t _allocationLimit;

    size_t TotalAQAllocated()       { return _totalAQAllocated; }
    Int32 TotalAllocations()        { return _totalAllocations; }
    size_t MaxAllocated()           { return _maxAllocated; }
    
    Int32 AQBitCount() {return 8;}

public:
    static void* GlobalMalloc(size_t countAQ);
    static void GlobalFree(void* pBuffer);
    
#ifdef VIREO_PERF_COUNTERS
private:
    Int64 _lookUpsFound;
    Int64 _lookUpsRoutedToOwner;
    Int64 _lookUpsNotResolved;
    Int64 _pathsParsed;
public:
    Int64 LookUpsFound()            { return _lookUpsFound;}
    Int64 LookUpsRoutedToOwner()    { return _lookUpsRoutedToOwner;}
    Int64 LookUpsNotResolved()      { return _lookUpsNotResolved;}
    Int64 PathsParshed()            { return _pathsParsed;}
    void  PathParsed()              { _pathsParsed++;}
#endif
};

class TypeManagerScope
{
#ifndef VIVM_SINGLE_EXECUTION_CONTEXT
private:
    TypeManager* _saveTypeManager;
    VIVM_THREAD_LOCAL static TypeManager* ThreadsTypeManager;
public:
    TypeManagerScope(TypeManager* typeManager)
    {
      _saveTypeManager = TypeManagerScope::ThreadsTypeManager;
      TypeManagerScope::ThreadsTypeManager = typeManager;
    }
    ~TypeManagerScope()
    {
        TypeManagerScope::ThreadsTypeManager = _saveTypeManager;
    }
    static TypeManager* Current()
    {
        VIREO_ASSERT(TypeManagerScope::ThreadsTypeManager!= null);
        return TypeManagerScope::ThreadsTypeManager;
    }
#else
    TypeManagerScope(TypeManager* typeManager) {}
    ~TypeManagerScope() {}
#endif
};

//------------------------------------------------------------
// InlineArray - A class to help dynamic classes/structures that
// end with an array whose size is set at construction time.
template <class T>
class InlineArray
{
private:
    T* _end;
    T _array[1];
public:
    static IntIndex ExtraStructSize(Int32 count){ return (count - 1) * sizeof(T); }
    InlineArray(Int32 length)                   { _end = &_array[length]; }
    T* Begin()                                  { return _array; }
    T* End()                                    { return _end; }
    void Assign(const T* source, Int32 count)   { memcpy(Begin(), source, count * sizeof(T)); }
    T& operator[] (const Int32 index)           { return _array[index]; }
    IntIndex Length()                           { return (IntIndex)(End() - Begin()); }
};

//------------------------------------------------------------
// Visitor class for types.
class TypeVisitor
{
public:
    virtual void VisitBad(TypeRef type) = 0;
    virtual void VisitBitBlock(TypeRef type) = 0;
    virtual void VisitBitCluster(TypeRef type) = 0;
    virtual void VisitCluster(TypeRef type)  = 0;
    virtual void VisitParamBlock(TypeRef type)  = 0;
    virtual void VisitEquivalence(TypeRef type) = 0;
    virtual void VisitArray(TypeRef type)  = 0;
    virtual void VisitElement(TypeRef type) = 0;
    virtual void VisitNamed(TypeRef type) = 0;
    virtual void VisitPointer(TypeRef type) = 0;
    virtual void VisitDefaultValue(TypeRef type) = 0;
    virtual void VisitCustomDefaultPointer(TypeRef type) = 0;
    virtual void VisitCustomDataProc(TypeRef type) = 0;
};

//------------------------------------------------------------
class TypeCommon
{
// Core internal methods are for keeping track of Type bjects in seperate
// TypeManager layers
    friend class TypeManager;
private:
    TypeCommon*     _next;              // Linked list of all Types in a TypeManager
    TypeManager*    _typeManager;       // TypeManger that owns this type
public:
    TypeCommon(TypeManager* typeManager);
    TypeManager* TheTypeManager()       { return _typeManager; }
    TypeRef Next()                      { return _next; }
public:
    // Internal to the TypeManager, but this is hard to secifiy in C++
    virtual ~TypeCommon() {};

// Core properties are calculated and stored in each node of a TypeTree.
// This allows inlineD code anD avoids messaging several layers down to calculate
// values that never change once a TypeTree is created.
// For bit fields to pack as tight as possible they need to have the same field type (Int16).
// If field types are mixed (int for some and bool for others, etc)
// compilers will typicially pad till the next byte for each change.
// The goal is to encode all inforamtion in 64 bits,
// smaller targets might be half that.
protected:
    Int32   _topAQSize;
    UInt16  _rank:8;            // 0 for scalar, 0 or greater for arrays room for rank upto 255
    UInt16  _alignment:8;       // In AQ units  //TODO could be stored as binary exponent in 3 bits
   
    UInt16  _encoding:kEncodingBitFieldSize; // aggirgate or single format
    UInt16  _isFlat:1;          // ( 0) All data is contained in TopAQ elements ( e.g. no pointers)
    UInt16  _isValid:1;         // ( 1) Contains no invalid types
    UInt16  _isBitLevel:1;      // ( 2) Is a bitblock or bitcluster

    UInt16  _hasCustomDefault:1;// ( 3) A non 0 non null value
    UInt16  _wasModified:1;     // ( 4) Internal, true when a type resolves a missing piece
    UInt16  _isFullyResolved:1; // ( 5) A non 0 non null value. TODO for forward references
    UInt16  _isFixedSize:1;     // ( 6) Total number of elements is fixed (false for variable arrays)
    UInt16  _hasGenericType:1;  // ( 7) The type contians some generic property values
    UInt16  _hasPadding:1;      // ( 8) To satisfy alignment requirements for elements TopAQSize() includes some padding
    
    //  properties unique to prototype elements. they are never merged up
    UInt16  _elementUsageType:3;// (9-11) ElementType::UsageType
    //  properties unique to CustomPointerType objects
    UInt16  _pointerType:3;     // (12-14)
    UInt16  _ownsDefDefData:1;  // (15) Owns DefaultDefault data (clusters and arrays)
    
public:
    // Public API
    EncodingEnum BitEncoding()      { return (EncodingEnum) _encoding; }
    Int32   Alignment()             { return _alignment; }
    Int32   TopAQSize()             { return _topAQSize; }
    Boolean HasCustomDefault()      { return _hasCustomDefault != 0; }
    Int32   Rank()                  { return _rank; }
    Boolean IsArray()               { return BitEncoding() == kEncoding_Array; }
    Boolean IsFlat()                { return _isFlat != 0; }
    Boolean IsValid()               { return _isValid != 0; }
    Boolean IsBitLevel()            { return _isBitLevel != 0; }
    Boolean HasPadding()            { return _hasPadding != 0; }
    Boolean HasGenericType()        { return _hasGenericType != 0; }
    Boolean WasModified()           { return _wasModified != 0; }

    // Properties of an element in  collection.
    Boolean IsInputParam()          { return (_elementUsageType == kUsageTypeInput) || (_elementUsageType == kUsageTypeInputOutput); }
    Boolean IsOutputParam()         { return (_elementUsageType == kUsageTypeOutput) || (_elementUsageType == kUsageTypeInputOutput); }
    Boolean IsStaticParam()         { return _elementUsageType == kUsageTypeStatic; }
    Boolean IsTempParam()           { return _elementUsageType == kUsageTypeTemp; }
    Boolean IsOptionalParam()       { return true; }//TODO {return _elementUsageType == kUsageTypeOptionalInput ;}
    UsageTypeEnum ElementUsageType(){ return (UsageTypeEnum)_elementUsageType; }
    Boolean OwnsDefDefData()        { return _ownsDefDefData != 0; }

    // Properties for CustomValuePointers
    PointerTypeEnum PointerType(){ return (PointerTypeEnum)_pointerType; }

    virtual void    Visit(TypeVisitor *tv)              { tv->VisitBad(this); }
    virtual TypeRef BaseType()                          { return null; }
    virtual Int32   SubElementCount()                   { return 0; }
    virtual TypeRef GetSubElementByName(SubString* name){ return null; }
    virtual TypeRef GetSubElement(Int32 index)          { return null; }
    virtual Int32   BitSize()  {return _topAQSize*8;}  // TODO defer to type manager for scale factor;
    
    // Element name in an aggrigate (e.g cluster field name such as "top")
    virtual void GetName(SubString* name)               { name->AliasAssign(null, null); }
    virtual void GetElementName(SubString* name)        { name->AliasAssign(null, null); }
    virtual IntIndex* GetDimensionLengths()             { return null; }
    
    // Offset in in AQs in the container aggrigate
    virtual IntIndex ElementOffset()                    { return 0; }

    // Methods for working with individual elements
    virtual void*   Begin(PointerAccessEnum mode)       { return null; }
    virtual NIError InitData(void* pData, TypeRef pattern = null);
    virtual NIError CopyData(const void* pData, void* pDataCopy);
    virtual NIError ClearData(void* pData);
    
    // Methods for working with linear blocks of elements
    // The base TypeCommon class short cuts simple cases
    // and defers non-flat to element virtual methods.
    NIError InitData(void* pData, IntIndex count);
    NIError CopyData(const void* pData, void* pDataCopy, IntIndex count);
    NIError ClearData(void* pData, IntIndex count);
    NIError MultiCopyData(const void* pSingleData, void* pDataCopy, IntIndex count);
    
    Boolean CompareType(TypeRef otherType);
    Boolean IsA(const SubString* name);
    Boolean IsA(TypeRef otherType, Boolean compatibleArrays);
    
    TypeRef GetSubElementFromPath(SubString* name, Int32 *offset);
};

//------------------------------------------------------------
// ElementType - Gives a type its field name and offset properties. Used inside clusters
//------------------------------------------------------------
class WrappedType : public TypeCommon
{
protected:
    TypeRef _wrapped;
    WrappedType(TypeManager *typeManager, TypeRef type);
public:
    // Type operations
    virtual TypeRef BaseType()                          { return _wrapped; }
    virtual Int32   SubElementCount()                   { return _wrapped->SubElementCount(); }
    virtual TypeRef GetSubElementByName(SubString* name){ return _wrapped->GetSubElementByName(name); }
    virtual TypeRef GetSubElement(Int32 index)          { return _wrapped->GetSubElement(index); }
    virtual Int32   BitSize()                           { return _wrapped->BitSize(); }
    virtual void    GetName(SubString* name)            { _wrapped->GetName(name); }
    virtual IntIndex* GetDimensionLengths()             { return _wrapped->GetDimensionLengths(); }
    // Data operations
    virtual void*   Begin(PointerAccessEnum mode)       { return _wrapped->Begin(mode); }
    virtual NIError InitData(void* pData, TypeRef pattern = null)
    { return _wrapped->InitData(pData, pattern ? pattern : this); }
    virtual NIError CopyData(const void* pData, void* pDataCopy)  { return _wrapped->CopyData(pData, pDataCopy); }
    virtual NIError ClearData(void* pData)              { return _wrapped->ClearData(pData); }
};

// TODO forward declarations ( this covers asynchronous resolution of sub VIs as well
// for the most part types are not mutable.
// here might be the exceptions
// 1. if a name is not resolved it can be kept on a short list. when the name is introduced
// the the type tree knows it need to be patched. The node in question replaced the pointer to the bad node to the
// the newly introduced type and marks itself as wasModified = true;
// then the list of type is sweeped and those that refer to modified types re finalize them selves ( fix name?)
// and mark them selves as wasModified. This repeats it self until no nodes are modified.
// the scan is O(n) with a small C for n Types at that level of the type manager and Type Mangers that
// the derive from it.
// 2. for the Named Type node the value may be changed. This does not change the type, only the result of what
// the type->InitValue method does. For a variante type this means the type of the value may change
// but not notiosn that the value is a variant. A bit tenious perhaps. s

// TODO, when named types wrap other named types should they be considered equivilent in all cases?
// or is there the idea of private versus public derivation.
// In other words hiding the implimentation at least by default. digging past the that layer requires
// the unwrap primitive which may not be allowed in some contexts.
class NamedType : public WrappedType
{
private:
    InlineArray<Utf8Char>   _name;
    NamedType(TypeManager* typeManager, SubString* name, TypeRef type);
public:
    static IntIndex StructSize(SubString* name)
        { return sizeof(NamedType) + InlineArray<Utf8Char>::ExtraStructSize(name->Length()); }
    static NamedType* New(TypeManager* typeManager, SubString* name, TypeRef type);
    
    virtual void    Visit(TypeVisitor *tv)          { tv->VisitNamed(this); }
    virtual void    GetName(SubString* name)        { name->AliasAssign(_name.Begin(), _name.End()); }
    virtual void    GetElementName(SubString* name) { name->AliasAssign(null, null); }
};
//------------------------------------------------------------
// ElementType - Gives a type its field name and offset properties. Used inside clusters
//------------------------------------------------------------
class ElementType : public WrappedType
{
private:
    ElementType(TypeManager* typeManager, SubString* name, TypeRef wrappedType, UsageTypeEnum usageType);

public:
    Int32                   _offset;  // Relative to the begining of the aggrigate
    InlineArray<Utf8Char>   _elementName;

public:
    static IntIndex StructSize(SubString* name) { return sizeof(ElementType) + InlineArray<Utf8Char>::ExtraStructSize(name->Length()); }
    static ElementType* New(TypeManager* typeManager, SubString* name, TypeRef wrappedType, UsageTypeEnum usageType);
    
    virtual void    Visit(TypeVisitor *tv)          { tv->VisitElement(this); }
    virtual void    GetElementName(SubString* name) { name->AliasAssign(_elementName.Begin(), _elementName.End()); }
    virtual IntIndex ElementOffset()                { return _offset; }
};
//------------------------------------------------------------
// BitBlockType - A raw set of bits
//------------------------------------------------------------
class BitBlockType : public TypeCommon
{
private:
    Int32   _bitSize;
    BitBlockType(TypeManager* typeManager, Int32 size, EncodingEnum encoding);
public:
    static BitBlockType* New(TypeManager* typeManager, Int32 size, EncodingEnum encoding);
    virtual void    Visit(TypeVisitor *tv)          { tv->VisitBitBlock(this); }
    virtual Int32   BitSize() {return _bitSize;};
};
//------------------------------------------------------------
// AggrigateType - A collection of sub types (an abstract class)
// Since this class is variable size class that derive from it can not
// have member variables.
//------------------------------------------------------------
class AggrigateType : public TypeCommon
{
protected:
    Int32 _bitSize;  // only used by BitCluster
    
protected:
    // The default value for the type, may be used
    // At this point only used by the ClusterType class but it needs to come
    // before the inlined array, so it is in this class.
    enum   { kSharedNullsBufferLength = 128 };
    static UInt8 _sharedNullsBuffer[kSharedNullsBufferLength];
    void*   _pDefault;

protected:
    InlineArray<ElementType*>   _elements;

    AggrigateType(TypeManager* typeManager, TypeRef elements[], Int32 count)
    : TypeCommon(typeManager), _elements(count)
    {
        _pDefault = null;
        _elements.Assign((ElementType**)elements, count);
    }
    static IntIndex StructSize(Int32 count)
    {
        return sizeof(AggrigateType) + InlineArray<ElementType*>::ExtraStructSize(count);
    }

public:
    virtual ~AggrigateType() {};
    virtual Int32   SubElementCount();
    virtual TypeRef GetSubElementByName(SubString* name);
    virtual TypeRef GetSubElement(Int32 index);
};
//------------------------------------------------------------
// A bit cluster a collection of bitfields. Bit clusters
// can be nested. Elements of the cluster cannot be dietly addressed
// select operations can extract or set them with in an addressabel type.
//------------------------------------------------------------
class BitClusterType : public AggrigateType
{
private:
    BitClusterType(TypeManager* typeManager, TypeRef elements[], Int32 count);
    static IntIndex StructSize(Int32 count) { return AggrigateType::StructSize(count); }
public:
    static BitClusterType* New(TypeManager* typeManager, TypeRef elements[], Int32 count);
    virtual void    Visit(TypeVisitor *tv)  { tv->VisitBitCluster(this); }
    virtual NIError InitData(void* pData, TypeRef pattern = null)   { return kNIError_Success; }
    virtual Int32 BitSize()                 { return _bitSize; }
};
//------------------------------------------------------------
// A Equivalence defines a set of diferent ways to look at a block of memory
// It is a bit like what a c union can be used for though it is stricter.
// all equivalence members bust be the same AQSize and must be flat
//------------------------------------------------------------
class EquivalenceType : public AggrigateType
{
private:
    EquivalenceType(TypeManager* typeManager, TypeRef elements[], Int32 count);
    static IntIndex StructSize(Int32 count) { return AggrigateType::StructSize(count); }
public:
    static EquivalenceType* New(TypeManager* typeManager, TypeRef elements[], Int32 count);
    virtual void    Visit(TypeVisitor *tv)  { tv->VisitEquivalence(this); }
    virtual void*   Begin(PointerAccessEnum mode);
    virtual NIError InitData(void* pData, TypeRef pattern = null);
    virtual NIError CopyData(const void* pData, void* pDataCopy);
    virtual NIError ClearData(void* pData);
};
//------------------------------------------------------------
// A Cluster is a collection of elements of potentially
// different types. the cluster and all elements can
// be individually be addressed
//------------------------------------------------------------
class ClusterType : public AggrigateType
{
private:
    ClusterType(TypeManager* typeManager, TypeRef elements[], Int32 count);
    virtual ~ClusterType();
    static IntIndex StructSize(Int32 count) { return AggrigateType::StructSize(count); }
public:
    static ClusterType* New(TypeManager* typeManager, TypeRef elements[], Int32 count);
    virtual void    Visit(TypeVisitor *tv)  { tv->VisitCluster(this); }
    virtual void*   Begin(PointerAccessEnum mode);
    virtual NIError InitData(void* pData, TypeRef pattern = null);
    virtual NIError CopyData(const void* pData, void* pDataCopy);
    virtual NIError ClearData(void* pData);
};
//------------------------------------------------------------
// A Parameter list is like a cluster, excpet :
//   1. Parameter fields have a input ooutput properties
//   2. Parameters are top-copied in
//   3. Parameters are top-cleared to zero, since the data is never owned by the
//      parameer block
//------------------------------------------------------------
class ParamBlockType : public AggrigateType
{
private:
    ParamBlockType(TypeManager* typeManager, TypeRef elements[], Int32 count);
    static IntIndex StructSize(Int32 count) { return AggrigateType::StructSize(count); }
public:
    static ParamBlockType* New(TypeManager* typeManager, TypeRef elements[], Int32 count);
    virtual void    Visit(TypeVisitor *tv)  { tv->VisitParamBlock(this); }
    virtual NIError InitData(void* pData, TypeRef pattern = null)
        {
            return kNIError_Success;
        }
    virtual NIError CopyData(const void* pData, void* pDataCopy)
        {
            VIREO_ASSERT(false); //TODO
            return kNIError_kInsufficientResources;
        }
    virtual NIError ClearData(void* pData)
        {
            return kNIError_kInsufficientResources;
        }
};
//------------------------------------------------------------
// ArrayType - A multiple dimension collection of a sub element type
//------------------------------------------------------------
class ArrayType : public WrappedType
{
private:
    ArrayType(TypeManager* typeManager, TypeRef elementType, IntIndex rank, IntIndex* dimensionLengths);
    static IntIndex StructSize(Int32 rank) { return sizeof(ArrayType) + ((rank-1) * sizeof(IntIndex)); }

public:
    enum { MaximumRank = 255 };   
    static ArrayType* New(TypeManager* typeManager, TypeRef elementType, IntIndex rank, IntIndex* dimensionLengths);
    
    // _pDefault is a singleton for each instance of an ArrayType used as the default
    // value, allocated one demand
    void*   _pDefault;
    
    // In the type dimension is described as follows:
    // negative=bounded, positive=fixed, zero=fix with no elements
    // negative VariableDimensionSentinel means varible, and will not be prealocated.
    IntIndex    _dimensionLengths[1];
    
    virtual void    Visit(TypeVisitor *tv)              { tv->VisitArray(this); }
    virtual TypeRef BaseType()                          { return null; } // arrays are a more advanced wrapping of a type.
    virtual Int32   SubElementCount()                   { return 1; }
    virtual TypeRef GetSubElementByName(SubString* name){ return Rank() == 0 ? _wrapped->GetSubElementByName(name) : null ; }
    virtual TypeRef GetSubElement(Int32 index)          { return index == 0 ? _wrapped : null; }
    virtual void    GetName(SubString* name)            { name->AliasAssignCStr("Array"); }
    virtual IntIndex* GetDimensionLengths()             { return &_dimensionLengths[0]; }

    virtual void*   Begin(PointerAccessEnum mode);
    virtual NIError InitData(void* pData, TypeRef pattern = null);
    virtual NIError CopyData(const void* pData, void* pDataCopy);
    virtual NIError ClearData(void* pData);

};
//------------------------------------------------------------
// DefaultValueType - A type that can initialize values to non zero/empty values
//------------------------------------------------------------
class DefaultValueType : public WrappedType
{
private:
    DefaultValueType(TypeManager* typeManager, TypeRef type);
    static IntIndex StructSize(TypeRef type)            { return sizeof(DefaultValueType) + type->TopAQSize(); }
public:
    static DefaultValueType* New(TypeManager* typeManager, TypeRef type);
public:
    virtual void    Visit(TypeVisitor *tv)              { tv->VisitDefaultValue(this); }
    virtual void*   Begin(PointerAccessEnum mode);
    virtual NIError InitData(void* pData, TypeRef pattern = null);
};
//------------------------------------------------------------
// PointerType - A type that describes a pointer to another type
//------------------------------------------------------------
class PointerType : public WrappedType
{
protected:
    PointerType(TypeManager* typeManager, TypeRef type);
public:
    static PointerType* New(TypeManager* typeManager, TypeRef type);
    virtual void    Visit(TypeVisitor *tv)              { tv->VisitPointer(this); }
    virtual TypeRef GetSubElement(Int32 index)          { return index == 0 ? _wrapped : null; }
    virtual Int32   SubElementCount()                   { return 1; }
    virtual TypeRef GetSubElementByName(SubString* name){ return null; }
};
//------------------------------------------------------------
// CustomPointerType - A pointer with an initial value
//------------------------------------------------------------
class CustomPointerType : public PointerType
{
private:
    CustomPointerType(TypeManager* typeManager, TypeRef type, void* pointer, PointerTypeEnum pointerType);
    CustomPointerType();
public:
    void*           _defaultPointerValue;
public:
    static CustomPointerType* New(TypeManager* typeManager, TypeRef type, void* pointer, PointerTypeEnum pointerType);
    
    virtual NIError InitData(void* pData, TypeRef pattern = null)
    {
        *(void**)pData = _defaultPointerValue;
        return kNIError_Success;
    }
    virtual void*   Begin(PointerAccessEnum mode)       { return &_defaultPointerValue; }
};
//------------------------------------------------------------
// CustomDataProcType - A type that has custom Init/Copy/Clear functions
//---------------------------------------------------------
class IDataProcs {
public:
    virtual NIError InitData(TypeRef type, void* pData, TypeRef pattern = null)  { return type->InitData(pData, pattern); }
    virtual NIError CopyData(TypeRef type, const void* pData, void* pDataCopy) { return type->CopyData(pData, pDataCopy); }
    virtual NIError ClearData(TypeRef type, void* pData) { return type->ClearData(pData); }
};
    
class CustomDataProcType : public WrappedType
{
protected:
    CustomDataProcType(TypeManager* typeManager, TypeRef type, IDataProcs *pAlloc);
    IDataProcs*    _pDataProcs;
public:
    static CustomDataProcType* New(TypeManager* typeManager, TypeRef type, IDataProcs *pIAlloc);
    virtual void    Visit(TypeVisitor *tv)              { tv->VisitPointer(_wrapped); }
    virtual NIError InitData(void* pData, TypeRef pattern = null)   { return _pDataProcs->InitData(_wrapped, pData, pattern ? pattern : this); }
    virtual NIError CopyData(const void* pData, void* pDataCopy) { return _pDataProcs->CopyData(_wrapped, pData, pDataCopy); }
    virtual NIError ClearData(void* pData)              { return _pDataProcs->ClearData(_wrapped, pData); }
};

//------------------------------------------------------------
// TypedArrayCore -
// The base class contains all the member fields,
// and the core algorithms for arrays
//------------------------------------------------------------
typedef TypedArrayCore *TypedArrayCoreRef, TypedBlock; // TODO get rid of TypedBlock
typedef TypedBlock *TypedBlockRef;  // TODO => merge into ArrayCoreRef

class TypedArrayCore  // =>TypedArrayCore
{
protected:
#ifdef VIVM_TYPE_MANAGER
    AQBlock1*               _pRawBufferBegin;
    AQBlock1*               _pRawBufferEnd;
    TypeRef                 _typeRef;
    TypeRef                 _eltTypeRef;
#endif

    // _dimensionAndSlabLengths works as follows
    // For example, in an array of Rank 2, there will be 2 DimensionLengths followed by
    // 2 slabLengths. slabLengths are precalculated in AQSize used for indexing.
    // For the inner most dimension the slab length is the length of the element.
    // Final offset is the dot product of the index vector and the slabLength vector.
private:
    IntIndex                _dimensionAndSlabLengths[2];
public:
    IntIndex* GetDimensionLengths() { return _dimensionAndSlabLengths; }
    IntIndex* GetSlabLengths()      { return &_dimensionAndSlabLengths[0] + _typeRef->Rank(); }
    
protected:
    static IntIndex StructSize(Int32 rank)  { return sizeof(TypedArrayCore) + ((rank-1) * sizeof(IntIndex) * 2); }
    TypedArrayCore(TypeRef type);
public:
    static TypedArrayCore* New(TypeRef type);
    static void Delete(TypedArrayCore*);

public:
    AQBlock1* BeginAt(IntIndex index)
    {
        VIREO_ASSERT(index >= 0)
        VIREO_ASSERT(ElementType() != null)
        AQBlock1* begin = (RawBegin() + (index * ElementType()->TopAQSize()));
        VIREO_ASSERT(begin <= _pRawBufferEnd)  //Is there a need to return a pointer to the 'end'
        return begin;
    }
public:
    void* RawObj()                  { VIREO_ASSERT(_typeRef->Rank() == 0); return RawBegin(); } // some extra asserts fo  ZDAs
    AQBlock1* RawBegin()            { return _pRawBufferBegin; }
    AQBlock1* RawEnd()              { return _pRawBufferEnd; }
    void* BeginAtAQ(IntIndex index) { return RawBegin() + index; }
    
public:
    TypeRef Type()                  { return _typeRef; }
    TypeRef ElementType()           { return _eltTypeRef; }
    Boolean SetElementType(TypeRef, Boolean preserveElements);
    
protected:
    Boolean AQAlloc(IntIndex countBytes);
    Boolean AQRealloc(IntIndex countBytes, IntIndex preserveBytes);
    void AQFree();
    
public:
    static Boolean ValidateHandle(TypedArrayCore* block)
    {
        // TODO: Allow for block valiate mode where all allocations and frees are tracked in a map
        return (block != null);
    }
    
    IntIndex GetLength(IntIndex i)
    {
        VIREO_ASSERT((i >= 0) && (i < Type()->Rank())); // TODO remove, initially I want to catch any of these.
        if ((i >= 0) && (i < Type()->Rank())) {
            return GetDimensionLengths()[i];
        } else {
            return 0;
        }
    }
    
    // Total Length  (product of all dimension lengths)
    // For actual arrays (not types) this will always be regular whole number.
    // Types may be variable, fixed, or bounded.
    IntIndex Length()
    {
        // TODO Its going to make sense for dim 0 to
        // alwasy be the cumulative length, and for 1d arrays that will be the only entry
        // for 2d there will be cumulateive and individual, with strides for each dim as well
        // It will only calculated when resized then adn this will work better as an inlined function 
        IntIndex *pDimLength = GetDimensionLengths();
        IntIndex *pEndDimLength = pDimLength + _typeRef->Rank();
        IntIndex length = 1;
        while (pDimLength < pEndDimLength) {
            length *= *pDimLength++;
        }
        return length;
    }
    
    // Cpacity is product of all potential dimension lengths ( differs from actual size
    // in bounded arrays. Could be extended to work with optimistic allocations.
    IntIndex Capacity()
    {
        return ((IntIndex)(_pRawBufferEnd - _pRawBufferBegin)) / _eltTypeRef->TopAQSize();
    }
    
    // Calculate the length of a contigious chunk of elements
    IntIndex AQBlockLength(IntIndex count) { return ElementType()->TopAQSize() * count; }
    
    // Resize for multi dim arrays
    Boolean ResizeDimensions(Int32 rank, IntIndex *dimensionLengths, Boolean preserveOld);
    
    // Make this array match the shape of the reference type.
    Boolean ResizeToMatchOrEmpty(TypedArrayCore* pReference);
    
    // Resize for 1d arrays, if not enough memory leave as is.
    Boolean Resize1D(IntIndex length);
    
    // Resize ,if not enough memory, then size to zero
    Boolean Resize1DOrEmpty(IntIndex length);

private:
    // Resize for 1d arrays, if not enough memory leave as is.
    Boolean ResizeCore(IntIndex aqLength, IntIndex currentLength, IntIndex length, Boolean preserveElements);
    
public:
    NIError Replace1D(IntIndex position, IntIndex count, const void* pSource, Boolean truncate);
    NIError Insert1D(IntIndex position, IntIndex count, const void* pSource = null);
    NIError Remove1D(IntIndex position, IntIndex count);    
};

// TypedArray1D - a template class that provides staticly typed methods.
// the template assumes that template type matches the runt-time type.
template <class T>
class TypedArray1D : public TypedArrayCore
{
public:
    T* Obj()                    { return (T*) RawObj(); }
    T* Begin()                  { return (T*) TypedArrayCore::RawBegin(); }
    T* End()                    { return (T*) TypedArrayCore::RawEnd(); }
    T  At(IntIndex index)       { return *(T*) BeginAt(index);};
    T* BeginAt(IntIndex index)  { return (T*) TypedArrayCore::BeginAt(index); }
    template <class T2> T2 AtAQ(IntIndex index) { return *(T2*)BeginAtAQ(index); }
    
    // TODO Indexing every element of a multi-dim array with these would be pretty costly
    // TODO these provide no bounds checking
    // Each of these is designed to only be called for the correctly dimensioned array
    T* ElementAddress(IntIndex i) { return Begin(i); }
    T* ElementAddress(IntIndex i, Int32 j) { return BeginAt((j * GetDimensionLengths()[0]) + i); }
    T* ElementAddress(IntIndex i, Int32 j, Int32 k)
    {
        VIREO_ASSERT(false);
        // calculate dot produt
        return null;
    }
    
    NIError Append(T element)                           { return Insert1D(Length(), 1, &element); }
    NIError Append(IntIndex count, const T* pElements)  { return Insert1D(Length(), count, pElements); }
    NIError Append(TypedArray1D* array) { return Insert1D(Length(), array->Length(), array->Begin()); }
    NIError CopyFrom(IntIndex count, const T* pElements){ return Replace1D(0, count, pElements, true); }
};

//------------------------------------------------------------
// Diagram types often processed by C++ code.
// They are not direcly creatable, they must be created
// as TypedBlocks when the dataspace is created
class String : public TypedArray1D< Utf8Char >
{
public:
    SubString MakeSubStringAlias()              { return SubString(Begin(), End()); }
    void CopyFromSubString(SubString* string)   { CopyFrom(string->Length(), string->Begin()); }
    void AppendCStr(const char* cstr)           { Append((IntIndex)strlen(cstr), (Utf8Char*)cstr); }
    void AppendSubString(SubString* string)     { Append((IntIndex)string->Length(), (Utf8Char*)string->Begin()); }
};

typedef String *StringRef;
typedef TypedArray1D< UInt8 > BinaryBuffer, *BinaryBufferRef;
typedef TypedArray1D< Int32 > Int32Array1D;
typedef TypedArray1D< StringRef > StringArray1D, *StringArray1DRef;
typedef TypedArray1D< TypeRef > TypeRefArray1D;

//------------------------------------------------------------
// Utility functions to read and write numbers to memory basd on size and encoding
NIError ReadIntFromMemory(EncodingEnum encoding, Int32 aqSize, void* pData, IntMax* pValue);
NIError WriteIntToMemory(EncodingEnum encoding, Int32 aqSize, void* pData, IntMax value);
NIError ReadRealFromMemory(EncodingEnum encoding, Int32 aqSize, void* pData, Double* pValue);
NIError WriteRealToMemory(EncodingEnum encoding, Int32 aqSize, void* pData, Double value);

//------------------------------------------------------------
// A derivative of the TempStackCStringFromString that knows
// how to construct a CString from a TypedData string.
class TempStackCStringFromString : public TempStackCString
{
public:
    TempStackCStringFromString(StringRef string)
    : TempStackCString((char*)string->Begin(), string->Length())
    { }
};

} // namespace Vireo

#endif //TypeAndDataManager_h
