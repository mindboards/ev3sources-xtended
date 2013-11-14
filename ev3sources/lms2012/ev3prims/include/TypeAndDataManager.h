/**

Copyright (c) 2013 National Instruments Corp.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

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
#include "Instruction.h"

namespace Vireo
{

class TypeCommon;
class TypedBlock;
class TypeManager;
class ExecutionContext;

#define TADM_NEW_PLACEMENT(_class_) new (TypeManagerScope::Current()->Malloc(sizeof(_class_))) _class_
#define TADM_NEW_PLACEMENT_DYNAMIC(_class_, _d_) new (TypeManagerScope::Current()->Malloc(_class_::StructSize(_d_))) _class_

typedef TypeCommon  *TypeRef;
typedef TypeManager *TypeManagerRef;
    
// StaticType is used for functions tha take typed determined at load time.
// specifiying StaticType for the paramete will result in the instruction holding a TypeCommon*
// Instead of a TypeRef*
typedef TypeCommon StaticType;

// Starting to bootstrap the type manager.
class TypedArray1DCore;
template <class T>
class TypedArray1D;
typedef  TypedArray1D<TypeRef> *TypeRefArray1DRef;


// Bit Encoding
// This is the base set of encodings used to annotated the underlying semantics
// of low level bit block, and as a key for serialization to and from binary, ASCII
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
    kEncoding_Pointer,      //TODO any need to differentiate between code/data/flash at this point?
    
    kEncodingBitFieldSize = 5,   //Room for up to 32 primitive encoding types
};

enum UsageTypeEnum {
    kUsageTypeSimple = 0,
    kUsageTypeInput = 1,
    kUsageTypeOutput = 2,
    kUsageTypeInputOutput =3,
    kUsageTypeStatic = 4,
    kUsageTypeTemp =  5,
    kUsageTypeImmediate =  6,
};

enum PointerTypeEnum {
    kPTNotAPointer = 0,
    kPTInt,
    kPTInstructionFunction,
    kPTGenericFucntionPropType,
    kPTGenericFuncitonCodeGen,
};
#if 0
//------------------------------------------------------------
typedef void (*TypeDefinerCallback)(TypeManager* pTypeManager);

class TypeDefiner
{
private:
    TypeDefiner*            _pNext;
    TypeDefinerCallback     _pCallback;
    const char*             _pNameSpace;
public :
    static void DefineTypes(TypeManager *pTypeManager);
    TypeDefiner(TypeDefinerCallback pCallback, const char* pNameSapce);
};

#define TOKENPASTE(x, y)    x ## y
#define TOKENPASTE2(x, y)   TOKENPASTE(x, y)

#define VIREO_DEFINE_BEGIN(_section) \
    static void TOKENPASTE2(DefineTypes, __LINE__) (TypeManager *tm);          \
    static TypeDefiner TOKENPASTE2(TheTypeDefiner, __LINE__) (TOKENPASTE2(DefineTypes, __LINE__), #_section); \
    static void TOKENPASTE2(DefineTypes, __LINE__) (TypeManager *tm)           \
    {

#define VIREO_DEFINE_END() \
    }
    
#define VIREO_DEFINE_TYPE(_name_, _type_) \
    (tm->Define(#_name_, _type_));
    
#define VIREO_DEFINE_FUNCTION(_name_, _typeTypeString_) \
    (tm->DefineCustomPointerTypeWithValue(#_name_, (void*)_name_, _typeTypeString_, kPTInstructionFunction));
    
#define VIREO_DEFINE_VALUE(_name_, value, _typeTypeString_) \
    (tm->DefineCustomPointerTypeWithValue(#_name_, value, _typeTypeString_, kPTInt));
    
#define VIREO_DEFINE_GENERIC(_name_, _typeTypeString_, _genericEmitProc_) \
    (tm->DefineCustomPointerTypeWithValue(#_name_, (void*)_genericEmitProc_, _typeTypeString_, kPTGenericFuncitonCodeGen));
#endif
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
//------------------------------------------------------------
class TypeManager
{
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
    
friend class TypeAndDataStringParser;
    TypeRef FindTypeInternal(SubString* name);
        
    // TODO The manager needs to define the Addressable Quantum size (bit in an addressable item, often a octet
    // but some times it is larger (e.g. 16 or 32) the CDC 7600 was 60
    // also defines alignment rules. Each element in a cluster is addressable
public:
    TypeManager(TypeManager* typeManager);
    ~TypeManager();
    void    DeleteTypes(Boolean finalTime);
    void    TrackType(TypeCommon* type);
    void    GetTypes(TypeRefArray1DRef);
    
    TypeManager *RootTypeManager() { return _rootTypeManager; }
    TypeRef Define(SubString* name, TypeRef type);
    TypeRef FindType(SubString* name);
    void*   FindNamedTypedBlock(SubString* name);
    TypeRef BadType();

    Int32   AQAlignment(Int32 size);
    Int32   AlignAQOffset(Int32 offset, Int32 size);
    Int32   BitCountToAQSize(Int32 bitCount);
    Int32   PointerToAQSize() {return sizeof(void*);}
    Int32   AQBitSize() {return _aqBitCount;}
    
public:
    NIError RegisterType(const char* name, const char* typeString);
	NIError DefineCustomPointerTypeWithValue(const char* name, void* pointer, TypeRef type, PointerTypeEnum pointerType);
    
public:
    // High level allocation functions
    NIError CreateTypedBlock(TypeRef typeRef, TypedBlock** pData);
    // NIError FreeTypedBlock(TypedBlock** pData);
public:
    // Low level allocation functions
    // TODO pull out into its own class.
    void* Malloc(size_t countAQ);
    void* ReAlloc(void* pBuffer, size_t countAQ);
    void Free(void* pBuffer);
    
    Boolean AllocationPermitted(size_t countAQ);
    void TrackAllocation(void* id, size_t countAQ);

    Int32  _totalAllocations;
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
        VIVM_CORE_ASSERT(TypeManagerScope::ThreadsTypeManager!= null);
        return TypeManagerScope::ThreadsTypeManager;
    }
#else
    TypeManagerScope(TypeManager* typeManager) {}
    ~TypeManagerScope() {}
#endif
};

//------------------------------------------------------------
// InlineArray - A class to help dynamic structures that contain a single
// fixed size aray initialized at construction time.
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
class TypeCommon
{
// Core internal methods are for keeping track of Type bjects in seperate
// TypeManager layers
    friend class TypeManager;
private:
    TypeCommon* _next;      // Linked list of all Type Nodes in a TypeManager
    TypeManager* _typeManager;  // TypeManger that owns this type
public:
    TypeCommon(TypeManager* typeManager);
    TypeManager* TheTypeManager()               { return _typeManager; }
public:
    // Internal to the TypeManager, but this is hard to secifiy in C++
    virtual ~TypeCommon() {};

// Core properties are calculated and stored in each node of a TypeTree.
// this allows for inline ocde an avoids messaging several layers down to calculate
// values that never change once a TypeTree is created.
// the values are stored in these members. For bit fields to pack as tight as possible
// they need to have the same field type (Int16). If they are mixed (int for some and bool for others)
// compilers will typicially pad till the next byte.
// The goal is to encode all inforamtion 64 bits,
// smaller targets might be half that.
protected:
    Int32   _topAQSize;
    UInt16  _rank:8;            // 0 for scalar, 1 or greater means arrays room for rank upto 255
    UInt16  _alignment:8;       // In AQ units  //TODO could be stored as binary exponent in 3 bits
   
    UInt16  _encoding:kEncodingBitFieldSize; // aggirgate or single format
    UInt16  _isFlat:1;          // ( 0) All data is contained in TopAQ elements ( e.g. no pointers)
    UInt16  _isValid:1;         // ( 1) contains no invalid types
    UInt16  _isBitLevel:1;      // ( 2) is a bitblock or bitcluster

    UInt16  _hasCustomDefault:1;  // ( 3) a non 0 non null value
    UInt16  _wasModified:1;     // ( 4) Internal, true when a type resolves a missing piece
    UInt16  _isFullyResolved:1; // ( 5) a non 0 non null value. TODO for forward references
    UInt16  _isFixedSize:1;     // ( 6) Total number of elements is fixed (false for variable arrays)
    UInt16  _hasGenericType:1;  // ( 10) is a bitblock or bitcluster
    
    //  properties unique to prototype elements. they are never merged up
    UInt16  _elementUsageType:3;// (7-9) ElementType::UsageType
    //  properties unique to CustomValue objects (a private field)
    UInt16  _extraAllocatedData:1;//(11)
    //  properties unique to CustomPointerType objects
    UInt16  _pointerType:3;     //(12-14)
                                // (15) not used
    
public:
    // Public API
    EncodingEnum BitEncoding()      { return (EncodingEnum) _encoding; }
    Int32   Alignment()             { return _alignment; }
    Int32   TopAQSize()             { return _topAQSize; }
    Boolean HasCustomDefault()        { return _hasCustomDefault != 0; }
    Int32   Rank()                  { return _rank; }
    Boolean IsArray()               { return _rank > 0; }
    Boolean IsObject()              { return IsArray(); }   // Arrays are the only type of object that exists so far
    Boolean IsFlat()                { return _isFlat != 0; }
    Boolean IsValid()               { return _isValid != 0; }
    Boolean IsBitLevel()            { return _isBitLevel != 0; }
    Boolean HasGenericType()        { return _hasGenericType != 0; }
    Boolean WasModified()           { return _wasModified != 0; }

    // Properties of an element in  collection.
    Boolean IsInputParam()          { return (_elementUsageType == kUsageTypeInput) || (_elementUsageType == kUsageTypeInputOutput); }
    Boolean IsOutputParam()         { return (_elementUsageType == kUsageTypeOutput) || (_elementUsageType == kUsageTypeInputOutput); }
    Boolean IsStaticParam()         { return _elementUsageType == kUsageTypeStatic; }
    Boolean IsTempParam()           { return _elementUsageType == kUsageTypeTemp; }
    UsageTypeEnum ElementUsageType(){ return (UsageTypeEnum)_elementUsageType; }

    // Properties for CustomValuePointers
    PointerTypeEnum PointerType(){ return (PointerTypeEnum)_pointerType; }

    virtual Int32   SubElementCount()                   { return 0; }
    virtual TypeRef GetSubElementByName(SubString* name){ return null; }
    virtual TypeRef GetSubElement(Int32 index)          { return null; }
    virtual Int32   BitSize()  {return _topAQSize*8;}  // TODO defer to type manager for scale factor;
    
    // Element name in an aggrigate (e.g cluster field name such as "top")
    virtual void GetName(SubString* name)               { name->AliasAssign(null, null); }
    virtual void GetElementName(SubString* name)        { name->AliasAssign(null, null); }
    virtual TypeRef GetBaseType()                       { return null; }
    virtual IntIndex* GetDimensionLengths()             { return null; }
    
    // Offset in in AQs in the container aggrigate
    virtual IntIndex ElementOffset()                    { return 0; }

    // Methods for working with individual elements
    virtual void*   Begin()                             { return null; }
    virtual NIError InitData(void* pData);
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
    Boolean IsA(SubString* name);
    
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
    virtual Int32   SubElementCount()                   { return _wrapped->SubElementCount(); }
    virtual TypeRef GetSubElementByName(SubString* name){ return _wrapped->GetSubElementByName(name); }
    virtual TypeRef GetSubElement(Int32 index)          { return _wrapped->GetSubElement(index); }
    virtual Int32   BitSize()                           { return _wrapped->BitSize(); }
    virtual void    GetName(SubString* name)            { _wrapped->GetName(name); }
    virtual TypeRef GetBaseType()                       { return _wrapped; }
    virtual IntIndex* GetDimensionLengths()             { return _wrapped->GetDimensionLengths(); }
    // Data operations
    virtual void*   Begin()                             { return _wrapped->Begin(); }
    virtual NIError InitData(void* pData)               { return _wrapped->InitData(pData); }
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
//    TypeRef     _type;
    InlineArray<char> _name;
    NamedType(TypeManager* typeManager, SubString* name, TypeRef type);
public:
    static IntIndex StructSize(SubString* name) { return sizeof(NamedType) + InlineArray<char>::ExtraStructSize(name->Length()); }
    static NamedType* New(TypeManager* typeManager, SubString* name, TypeRef type);
    
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
    Int32               _offset;  // Relative to the begining of the aggrigate
    InlineArray<char>   _elementName;

public:
    static IntIndex StructSize(SubString* name) { return sizeof(ElementType) + InlineArray<char>::ExtraStructSize(name->Length()); }
    static ElementType* New(TypeManager* typeManager, SubString* name, TypeRef wrappedType, UsageTypeEnum usageType);
    
    virtual void    GetElementName(SubString* name)     { name->AliasAssign(_elementName.Begin(), _elementName.End()); }
    virtual IntIndex ElementOffset()                    { return _offset; }
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
    InlineArray<ElementType*>   _elements;

    AggrigateType(TypeManager* typeManager, TypeRef elements[], Int32 count)
    : TypeCommon(typeManager), _elements(count)
    {
        _elements.Assign((ElementType**)elements, count);
    }
    static IntIndex StructSize(Int32 count) { return sizeof(AggrigateType) + InlineArray<ElementType*>::ExtraStructSize(count); }

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
    virtual NIError InitData(void* pData)   { return kNIError_Success; }
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
    virtual NIError InitData(void* pData);
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
    static IntIndex StructSize(Int32 count) { return AggrigateType::StructSize(count); }
public:
    static ClusterType* New(TypeManager* typeManager, TypeRef elements[], Int32 count);
    virtual NIError InitData(void* pData);
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
    virtual NIError InitData(void* pData)
        {
            return kNIError_Success;
        }
    virtual NIError CopyData(const void* pData, void* pDataCopy)
        {
            VIVM_CORE_ASSERT(false); //TODO
            return kNIError_kInsufficientResources;
        }
    virtual NIError ClearData(void* pData)
        {
         //b   VIVM_CORE_ASSERT(false); //TODO
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
    
    // EmptyInstance is a singleton for each instance of an ArrayType used as the default
    // value for dynamic arrays.
    // TypedArray1DCore *_emptyInstance;     // TODO
    
    // In the type dimension is described as follows:
    // negative=bounded, positive=fixed, zero=fix with no elements
    // negative VariableDimensionSentinel means varible, and will not be prealocated.
    IntIndex    _dimensionLengths[1];
    
    virtual Int32   SubElementCount()                   { return 1; }
    virtual TypeRef GetSubElementByName(SubString* name){ return null; }
    virtual TypeRef GetSubElement(Int32 index)          { return index == 0 ? _wrapped : null; }
    virtual void    GetName(SubString* name)            { name->AliasAssignCStr("Array"); }
    virtual TypeRef GetBaseType()                       { return null; }
    virtual IntIndex* GetDimensionLengths()             { return &_dimensionLengths[0]; }

    virtual NIError InitData(void* pData);
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
    ~DefaultValueType();
public:
    static DefaultValueType* New(TypeManager* typeManager, TypeRef type);
    // The custom default value. Either stored in the object or in a TypedBlock.
    union {
        TypedBlock*     _typedBlock;
        void*           _pointer;
        Double			_double;
        AQBlock8        _aqBlock8;
        AQBlock4        _aqBlock4;
        AQBlock2        _aqBlock2;
        AQBlock1        _aqBlock1;
    } u;
public:
    virtual void*   Begin();
    virtual NIError InitData(void* pData);
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
    
    virtual NIError InitData(void* pData)
    {
        *(void**)pData = _defaultPointerValue;
        return kNIError_Success;
    }
    virtual void*   Begin()                             { return &_defaultPointerValue; }
};
//------------------------------------------------------------
// TypedArray1DCore -
// The base class contains all the member fields,
// and the core algorithms for arrays
//------------------------------------------------------------
typedef TypedArray1DCore* TypedArray1dCoreRef;
class TypedArray1DCore
{
protected:
#ifdef VIVM_TYPE_MANAGER
    AQBlock1*               _pRawBufferBegin;
    AQBlock1*               _pRawBufferEnd;
    TypeRef                 _typeRef;
    TypeRef                 _eltTypeRef;
    AQBlock1*               _pCapactiy;
#endif
    // _dimensionAndSlabLengths works as follows
    // For an array of Rank 2, there will be 2 DimensionLenghts followed by
    // 2 slabLengths. slabLengths are precalculated in AQSize used for indexing.
    // Final offset is the dot product of the index vector and the slabLength vector.
    IntIndex                _dimensionAndSlabLengths[2];
    static IntIndex StructSize(Int32 rank) { return sizeof(TypedArray1DCore) + ((rank-1) * sizeof(IntIndex) * 2); }
    
    TypedArray1DCore(Int32 rank)
    {
        memset(this, 0, TypedArray1DCore::StructSize(rank));
    }
public:
    AQBlock1* BeginAt(IntIndex index)
    {
        VIVM_CORE_ASSERT(ElementType() != null)
        AQBlock1* begin = (RawBegin() + (index * ElementType()->TopAQSize()));
        VIVM_CORE_ASSERT(begin <= _pRawBufferEnd)  //Is there a need to return a pointer to the 'end'
        return begin;
    }
public:
    AQBlock1* RawBegin()            { return _pRawBufferBegin; }
    AQBlock1* RawEnd()              { return _pRawBufferEnd; }
    void* BeginAtAQ(IntIndex index) { return RawBegin() + index; }
    
public:
    TypeRef Type()                  { return _typeRef; }
    TypeRef ElementType()           { return _eltTypeRef; }
    
protected:
    NIError AQAlloc(IntIndex countBytes);
    NIError AQRealloc(IntIndex countBytes);
    void AQFree();
    
public:
    IntIndex AQSize()               { return (IntIndex) (_pRawBufferEnd - _pRawBufferBegin); }
    Boolean ValidateAccess(IntIndex index, IntIndex range);
    
    static Boolean ValidateHandle(TypedArray1DCore* block)
    {
        // TODO: Allow for block valiate mode where all allocations and frees are tracked in a map
        return (block != null);
    }
    
    IntIndex GetLength(IntIndex i)
    {
        VIVM_CORE_ASSERT((i >= 0) && (i < Type()->Rank())); // TODO remove, initially I want to catch any of these.
        if ((i >= 0) && (i < Type()->Rank())) {
            return _dimensionAndSlabLengths[i];
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
        IntIndex *pDimLength = _dimensionAndSlabLengths;
        IntIndex *pEndDimLength = _dimensionAndSlabLengths + _typeRef->Rank();
        IntIndex length = 1;
        while (pDimLength < pEndDimLength) {
            length *= *pDimLength++;
        }
        return length;
    }
    
    // Cpacity is product of all potential dimension lengths ( differs from actual size
    // in bounded arrays. Could be extended to work with optimistic allocations.
    IntIndex Capactiy()
    {
        return ((IntIndex)(_pRawBufferEnd - _pRawBufferBegin)) / _eltTypeRef->TopAQSize();
    }
    
    IntIndex AQBlockLength(IntIndex count) { return ElementType()->TopAQSize() * count; }
    
    NIError InitData();
    NIError ClearData();
    
    // Resize for multi dim arrays
    NIError ResizeDimensions(Int32 rank, Int32 *dimensionLengths);
    
    // Make this array match the shape of the reference type.
    NIError ResizeToMatch(TypedArray1DCore* pReference);
    
    // Resize for 1d arrays
    NIError Resize(Int32 size);
    
public:
    // Insert space for additional element and optional copy values in to the new location
    NIError Insert1D(IntIndex position, IntIndex count, const void* pSource = null);
    NIError Remove1D(IntIndex position, IntIndex count);
    
};

// TypedArray1D -  a template class that provides staticly typed methods.
// the template assumes that template type matches the runt-time type.
template <class T>
class TypedArray1D : public TypedArray1DCore// TODO could/should this drive from TypeCommon?
{
public:
    T* Begin()                  { return (T*) TypedArray1DCore::RawBegin(); }
    T* End()                    { return (T*) TypedArray1DCore::RawEnd(); }
    T  At(IntIndex index)       { return *(T*) BeginAt(index);};
    T* BeginAt(IntIndex index)  { return (T*) TypedArray1DCore::BeginAt(index); }
    template <class T2> T2 AtAQ(IntIndex index) { return *(T2*)BeginAtAQ(index); }
    
    // TODO Indexing every element of a multi-dim array with these would be pretty costly
    // TODO these provide no bounds checking
    // Each of these is designed to only be called for the correctly dimensioned array
    T* ElementAddress(IntIndex i) { return Begin(i); }
    T* ElementAddress(IntIndex i, Int32 j) { return BeginAt((j * _dimensionAndSlabLengths[0]) + i); }
    T* ElementAddress(IntIndex i, Int32 j, Int32 k)
    {
        VIVM_CORE_ASSERT(false);
        // calculate dot produt
        return BeginAt((j * _dimensionAndSlabLengths[2]) + i);
    }
    
    NIError Append(T element) { return Insert1D(Length(), 1, &element); }
    NIError Append(IntIndex count, const T* pElements) { return Insert1D(Length(), count, pElements); }
        
public:
protected:
    // This creates a block of bytes described by the type supplied. There are two
    // types of blocks.
    //  (1) If the type is an array then the block will hold 0 or more elements of the array. If its zero
    //      no memory will be allocated for the element buffer
    //  (2) If the type is a cluster then the block will hold exactly 1 instance of that cluster.
    //      and memory is allocated so teh element buffer can hold the cluster
    TypedArray1D(TypeRef typeRef)
    : TypedArray1DCore(typeRef->IsArray() ? typeRef->Rank() : 0)
    {
        _typeRef = typeRef;
        if (typeRef->IsArray()) {
            // Arrays: cache a copy of the element TypeRef and leave the buffer empty
            _eltTypeRef = typeRef->GetSubElement((Int32)0);
            VIVM_CORE_ASSERT(_pRawBufferBegin == null);
            VIVM_CORE_ASSERT(_pRawBufferEnd == null);
            ResizeDimensions(typeRef->Rank(), typeRef->GetDimensionLengths());
        } else {
            // Clusters: null out _eltTypeRef, and allocate storage.
            _eltTypeRef = null;
            AQAlloc(typeRef->TopAQSize());
        }
    };
};

//------------------------------------------------------------
// Diagram types often processed by C++ code.
// They are not direcly creatable, they must be created
// as TypedBlocks when the dataspace is created
typedef TypedArray1D< Int32 > Int32Array1D;
typedef TypedArray1D< Utf8Char > Utf8String, *Utf8StringRef;
typedef TypedArray1D< Utf8StringRef > Utf8StringArray1D, *Utf8StringArray1DRef;
typedef TypedArray1D< TypeRef > TypeRefArray1D;

//------------------------------------------------------------
typedef TypedBlock* TypedBlockRef;
class TypedBlock : public TypedArray1D<UInt8>
{
private:
    TypedBlock(TypeRef typeRef) :TypedArray1D<UInt8>(typeRef) {}
public:
    static NIError Create(TypeRef typeRef, TypedBlockRef* pData);
};

//------------------------------------------------------------
// Used for making short null terminated strings when needed for calling OS APIs
// TODO size could be templated.
class TempStackCString
{
    enum {MaxLength = 255};
private:
    Utf8Char    _buffer[MaxLength+1];
    Utf8Char*   _end;
public:
    
    TempStackCString(char* begin, Int32 length)
    {
        length = (length < MaxLength) ? length : MaxLength;
        _end = _buffer + length;
        memcpy(_buffer, begin, length);
        *_end = (Utf8Char) 0;
    }
    
    TempStackCString(Utf8String* string)
    {
        Int32 length = (string->Length() < MaxLength) ? string->Length() : MaxLength;
        _end = _buffer + length;
        memcpy(_buffer, string->Begin(), length);
        *_end = (Utf8Char) 0;
    }

    TempStackCString(SubString* string)
    {
        Int32 length = (string->Length() < MaxLength) ? string->Length() : MaxLength;
        _end = _buffer + length;
        memcpy(_buffer, string->Begin(), length);
        *_end = (Utf8Char) 0;
    }
    void Append(SubString* string) 
    {
        Utf8Char* newEnd = _end + string->Length();
        if(newEnd > _buffer + MaxLength) {
            newEnd = _buffer + MaxLength;
        }
        size_t length = newEnd - _end;
        memcpy(_end, string->Begin(), length);
        _end = newEnd;
        *_end = (Utf8Char) 0;
    }
    
    char*   BeginCString()  {return (char*) _buffer;}
    char*   End()           {return (char*) _end;}
};
} // namespace Vireo

#endif //TypeAndDataManager_h
