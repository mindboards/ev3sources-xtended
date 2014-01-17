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
    static TypeRef Define(TypeManager& tm, const char* name, const char* typeString);
    static TypeRef Define(TypeManager& tm, SubString* name, SubString* wrappedTypeString);
    static void DefineCustomPointerTypeWithValue(TypeManager& tm, const char* name, void* instruction, const char* typeString,PointerTypeEnum pointerType);
    static void DefineCustomValue(TypeManager& tm, const char* name, Int32 value, const char* typeString);

    TypeDefiner(TypeDefinerCallback pCallback, const char* pNameSapce);
};

}

#define TOKENPASTE(x, y, z)    x ## y ## z
#define TOKENPASTE2(x, y, z)   TOKENPASTE(x, y, z)

#define VIREO_DEFINE_BEGIN(_section_) \
static void TOKENPASTE2(DefineTypes, _section_, __LINE__) (TypeManager& tm); \
static TypeDefiner TOKENPASTE2(TheTypeDefiner, _section_, __LINE__) (TOKENPASTE2(DefineTypes, _section_, __LINE__), #_section_); \
static void TOKENPASTE2(DefineTypes, _section_, __LINE__) (TypeManager& tm) {

#define VIREO_DEFINE_END()   }

#define VIREO_DEFINE_TYPE(_name_, _type_) \
(TypeDefiner::Define(tm, #_name_, _type_));

#define VIREO_DEFINE_FUNCTION(_name_, _typeTypeString_) \
(TypeDefiner::DefineCustomPointerTypeWithValue(tm, #_name_, (void*)_name_, _typeTypeString_, kPTInstructionFunction));

#define VIREO_DEFINE_FUNCTION_NAME(_symbol_, _name_, _typeTypeString_) \
(TypeDefiner::DefineCustomPointerTypeWithValue(tm, #_name_, (void*)_symbol_, _typeTypeString_, kPTInstructionFunction));

#define VIREO_DEFINE_VALUE(_name_, value, _typeTypeString_) \
(TypeDefiner::DefineCustomValue(tm, #_name_, value, _typeTypeString_));

#define VIREO_DEFINE_GENERIC(_name_, _typeTypeString_, _genericEmitProc_) \
(TypeDefiner::DefineCustomPointerTypeWithValue(tm, #_name_, (void*)_genericEmitProc_, _typeTypeString_, kPTGenericFunctionCodeGen));

#endif // TypeDefiner_h

