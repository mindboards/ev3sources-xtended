/**
 
Copyright (c) 2014 National Instruments Corp.
 
This software is subject to the terms described in the LICENSE.TXT file
 
SDG
*/


#ifndef TypeDefiner_h
#define TypeDefiner_h

#include "TypeAndDataManager.h"

//------------------------------------------------------------
namespace Vireo {

typedef void (*TypeDefinerCallback)(TypeManager& typeManager);

class TypeDefiner
{
private:
    TypeDefiner*            _pNext;
    TypeDefinerCallback     _pCallback;
    const char*             _pNameSpace;
public :
    static void DefineStandardTypes(TypeManager& tm);
    static void DefineTypes(TypeManager& tm);
    static TypeRef Define(TypeManager& tm, const char* name, const char* typeCStr);
    static TypeRef Define(TypeManager& tm, SubString* name, SubString* wrappedTypeString);
    static TypeRef ParseAndBuidType(TypeManager& tm, SubString* typeString);
    static void DefineCustomPointerTypeWithValue(TypeManager& tm, const char* name, void* instruction, const char* typeString,PointerTypeEnum pointerType);
    static void DefineCustomValue(TypeManager& tm, const char* name, Int32 value, const char* typeString);
    static void DefineCustomDataProcs(TypeManager& tm, const char* name, IDataProcs* pDataProcs, const char* typeString);

    TypeDefiner(TypeDefinerCallback pCallback, const char* pNameSapce);
};

}

#define TOKENPASTE(x, y, z)    x ## y ## z
#define TOKENPASTE2(x, y, z)   TOKENPASTE(x, y, z)

#define DEFINE_VIREO_BEGIN(_section_) \
static void TOKENPASTE2(DefineTypes, _section_, __LINE__) (TypeManager& tm); \
static TypeDefiner TOKENPASTE2(TheTypeDefiner, _section_, __LINE__) (TOKENPASTE2(DefineTypes, _section_, __LINE__), #_section_); \
static void TOKENPASTE2(DefineTypes, _section_, __LINE__) (TypeManager& tm) {

#define DEFINE_VIREO_END()   }

#define DEFINE_VIREO_TYPE(_name_, _type_) \
(TypeDefiner::Define(tm, #_name_, _type_));

#define DEFINE_VIREO_FUNCTION(_name_, _typeTypeString_) \
(TypeDefiner::DefineCustomPointerTypeWithValue(tm, #_name_, (void*)_name_, _typeTypeString_, kPTInstructionFunction));

#define DEFINE_VIREO_FUNCTION_NAME(_symbol_, _name_, _typeTypeString_) \
(TypeDefiner::DefineCustomPointerTypeWithValue(tm, #_name_, (void*)_symbol_, _typeTypeString_, kPTInstructionFunction));

#define DEFINE_VIREO_VALUE(_name_, value, _typeTypeString_) \
(TypeDefiner::DefineCustomValue(tm, #_name_, value, _typeTypeString_));

#define DEFINE_VIREO_GENERIC(_name_, _typeTypeString_, _genericEmitProc_) \
(TypeDefiner::DefineCustomPointerTypeWithValue(tm, #_name_, (void*)_genericEmitProc_, _typeTypeString_, kPTGenericFunctionCodeGen));

#define DEFINE_VIREO_CUSTOM_DP(_name_, _type_, _allocClass_) \
(TypeDefiner::DefineCustomDataProcs(tm, #_name_, _allocClass_, _type_));

#endif // TypeDefiner_h

