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

// C Entry Points for primary functions, so they can be called from language that can not
// dynamically link to c++ well.

using namespace Vireo;

VIREO_EXPORT Int32 Vireo_Version();

VIREO_EXPORT void* EggShell_Create(EggShell* parent);

VIREO_EXPORT void EggShell_REPL(EggShell* pShell, const char* commands);

VIREO_EXPORT Int32 EggShell_ExecuteSlices(EggShell* pShell, Int32 numSlices);

VIREO_EXPORT void EggShell_SetDelayedLoad(EggShell* pShell, bool value);

VIREO_EXPORT void EggShell_Delete(EggShell* pShell);

VIREO_EXPORT void ExecutionContext_EnqueRunQueue(ExecutionContext* pContext, VIClump* pClump);

VIREO_EXPORT Int32 ExecutionContext_ExecuteSlices(ExecutionContext* pContext, Int32 numSlices);

VIREO_EXPORT Int32 EggShell_PeekMemory(EggShell* pShell, const char* viName, const char* eltName, Int32 bufferSize, char* buffer);

VIREO_EXPORT Int32 EggShell_PokeMemory(EggShell* pShell, const char* viName, const char* eltName, Int32 bufferSize, char* buffer);

//------------------------------------------------------------
//------------------------------------------------------------
VIREO_EXPORT void Clump_DecrementFireCount(VIClump* clump);
//------------------------------------------------------------
//------------------------------------------------------------
VIREO_EXPORT TypeRef TypeManager_Define(TypeManager* typeManager, const char* typeName, const char* typeString);
//------------------------------------------------------------
VIREO_EXPORT TypeRef TypeManager_FindType(TypeManager* typeManager, const char* typeName);
//------------------------------------------------------------
VIREO_EXPORT Int32 TypeRef_TopAQSize(TypeRef typeRef);
//------------------------------------------------------------
VIREO_EXPORT Boolean TypeRef_IsFlat(TypeRef typeRef);
//------------------------------------------------------------
VIREO_EXPORT Boolean TypeRef_IsValid(TypeRef typeRef);
//------------------------------------------------------------
VIREO_EXPORT Boolean TypeRef_HasCustomDefault(TypeRef typeRef);
//------------------------------------------------------------
VIREO_EXPORT EncodingEnum TypeRef_BitEncoding(TypeRef typeRef);
//------------------------------------------------------------
VIREO_EXPORT Int32 TypeRef_Alignment(TypeRef typeRef);
//------------------------------------------------------------
VIREO_EXPORT void TypeRef_Name(TypeRef typeRef, Int32* bufferSize, char* buffer);
//------------------------------------------------------------
VIREO_EXPORT void TypeRef_ElementName(TypeRef typeRef, Int32* bufferSize, char* buffer);
//------------------------------------------------------------
VIREO_EXPORT Int32 TypeRef_ElementOffset(TypeRef typeRef);
//------------------------------------------------------------
VIREO_EXPORT Int32 TypeRef_Rank(TypeRef typeRef);
//------------------------------------------------------------
VIREO_EXPORT Int32 TypeRef_SubElementCount(TypeRef typeRef);
//------------------------------------------------------------
VIREO_EXPORT TypeRef TypeRef_GetSubElementByPath(TypeRef typeRef, char* buffer, Int32 *offset);
//------------------------------------------------------------
VIREO_EXPORT TypeRef TypeRef_GetSubElementByName(TypeRef typeRef, char* buffer);
//------------------------------------------------------------
VIREO_EXPORT TypeRef TypeRef_GetSubElementByIndex(TypeRef typeRef, Int32 index);
//------------------------------------------------------------
//------------------------------------------------------------
VIREO_EXPORT Int32 Data_RawBlockSize(TypedBlock* object);
//------------------------------------------------------------
VIREO_EXPORT Int32 Data_Length(TypedBlock* object);
//------------------------------------------------------------
VIREO_EXPORT TypeRef Data_Type(TypedBlock* object);
//------------------------------------------------------------
VIREO_EXPORT Int32 Data_GetLength(TypedBlock* object, Int32 dimension);
//------------------------------------------------------------
VIREO_EXPORT void Data_Resize1D(TypedBlock* object, Int32 size);
//------------------------------------------------------------
VIREO_EXPORT void Data_ResizeDimensions(TypedBlock* object, Int32 rank, Int32* sizes);
//------------------------------------------------------------
VIREO_EXPORT void* Data_RawPointerFromOffset(TypedBlock* object, Int32 offset);
//------------------------------------------------------------
VIREO_EXPORT void Data_Read1Byte(TypedBlock* object, Int32 offset, Int8* value);
//------------------------------------------------------------
VIREO_EXPORT void Data_Write1Byte(TypedBlock* object, Int32 offset, Int8 value);
//------------------------------------------------------------
VIREO_EXPORT void Data_Read2Bytes(TypedBlock* object, Int32 offset, Int16* value);
//------------------------------------------------------------
VIREO_EXPORT void Data_Write2Bytes(TypedBlock* object, Int32 offset, Int16 value);
//------------------------------------------------------------
VIREO_EXPORT void Data_Read4Bytes(TypedBlock* object, Int32 offset, Int32* value);
//------------------------------------------------------------
VIREO_EXPORT void Data_Write4Bytes(TypedBlock* object, Int32 offset, Int32 value);
//------------------------------------------------------------
VIREO_EXPORT void Data_Read8Bytes(TypedBlock* object, Int32 offset, Int64* value);
//------------------------------------------------------------
VIREO_EXPORT void Data_Write8Bytes(TypedBlock* object, Int32 offset, Int64 value);
//------------------------------------------------------------
VIREO_EXPORT void Data_ReadPointer(TypedBlock* object, Int32 offset, void** value);
//------------------------------------------------------------
VIREO_EXPORT void Data_WritePointer(TypedBlock* object, Int32 offset, void* value);
//------------------------------------------------------------
VIREO_EXPORT void Data_ReadBytes(TypedBlock* object, Int32 offset, Int32 count, Int32* buffer);
//------------------------------------------------------------
VIREO_EXPORT void Data_WriteBytes(TypedBlock* object, Int32 offset, Int32 count, Int32* buffer);

