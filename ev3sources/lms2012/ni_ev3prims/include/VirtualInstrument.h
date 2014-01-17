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

#ifndef VirtualInstrument_h
#define VirtualInstrument_h

#include "DataTypes.h"
#include "EventLog.h"

namespace Vireo
{

class VIClump;

#define VI_TypeName             "VirtualInstrument"
#define ReentrantVI_TypeName    "ReentrantVirtualInstrument"

#define VI_TypeString             \
"c(                               \
    e(.ExecutionContext Context)  \
    e(.Variant ParamBlock)        \
    e(.Variant DataSpace)         \
    e(.VIClumpArray1D Clumps)     \
)"


class VirtualInstrument
{
private:
    ExecutionContext*       _executionContext;
    TypedBlock*             _paramBlock;          // All clumps in subVI share the same param block
    TypedBlock*             _dataSpace;           // All clumps in subVI share the same data
    TypedArray1D<VIClump>*  _clumps;
public :
    NIError Init(ExecutionContext* context, Int32 clumpCount, TypeRef paramBlockType, TypeRef dataSpaceType);
    void PressGo();
public:
    VirtualInstrument(ExecutionContext *context, int clumps, TypeRef paramBlockType, TypeRef dataSpaceType);
    ExecutionContext* OwningContext()   {return _executionContext;}
    TypedBlock* ParamBlock()            {return _paramBlock;}
    TypedBlock* DataSpace()             {return _dataSpace;}
    TypedArray1D<VIClump>* Clumps()     {return _clumps;}
};

class FunctionClump
{
public:
	InstructionCore* _codeStart;        // first instruction object in clump. may be shared  between multipl QEs
};

// Still tinking about how to arrange these classes
class QueueClump
{
private:
	InstructionCore* _codeStart;        // first instruction object in clump. may be shared  between multipl QEs
};

#define VIClump_TypeString          \
"c(                                 \
e(.DataPointer,CodeStart)           \
e(.DataPointer,Next)                \
e(.DataPointer,Owner)               \
e(.DataPointer,NextWaitingCaller)   \
e(.DataPointer,Caller)              \
e(.DataPointer,SavePC)              \
e(.Int64,WakeUpInfo)                \
e(.Int32,FireCount)                 \
e(.Int32,ShortCount)                \
)"



// A QueueElt (QE) will be allocated for every VI and every sub clump for VIs that have parallelism.
// Reentrant callers need to maintain seperate state so a copy is needed for each call site.
class VIClump : public FunctionClump
{
public:
    VIClump*            _next;              // Next element in Q this clump is part of.
    VirtualInstrument*  _owningVI;
	//TODO we need the parameter typedef somewhere, perhaps it is a copy in instruction in the VI
	// advantages it that aliases wont duplicate it. VBW typeref will be in copyin/copyout instruction
	VIClump*            _waitingClumps;     // If this clump is busy when called then callers are linked here.
	VIClump*            _caller; 			// Used for sub vi calls, clump to restart once done.
	InstructionCore*    _savePc;            // Save when paused either due to sub vi call, or time slicing
	uTicTimeType        _wakeUpInfo;		// If clump is suspended, used to determine if wake up condition exists (e.g. time)
	CounterType         _fireCount;         // What to reset shortCount to.
	CounterType         _shortCount;		// Greater than 0 is not in run queue, when it goes to zero it gets enqueued
    
public:
    void Trigger();
    void Wait();
    CounterType         FireCount() { return _fireCount;}
    CounterType         ShortCount() { return _shortCount;}
    
    void InsertIntoWaitList(VIClump* elt);
    VirtualInstrument*  OwningVI() {return _owningVI;};
    TypeManager*        TheTypeManager();
    
#ifdef VIVM_DYNAMIC_ALLOCATION
public:
    void InitData();
    void ClearData();
    void FreeInstructions(InstructionCore* instruction);
#endif
};

// The SubVI Call instruciton contains a pointer tot he root clump
// and sub snippets for parameters in and out.
struct CallVIInstruction : public InstructionCore
{
    _ParamImmediateDef(VIClump*, viRootClump);
    _ParamImmediateDef(InstructionCore*, copyInSnippet);
    _ParamImmediateDef(InstructionCore*, copyOutSnippet);
#if 0
    // rescheduleSnippet is the only r/w element in generated, an issue if if instructions are stored in FLASH
    // the state could be stored in the current clump a well since a clump can only be
    // suspend for one VI cal at a time.
    // _ParamImmediateDef(InstructionCore*, rescheduleSnippet);
#endif
};

#ifdef VIVM_DYNAMIC_ALLOCATION
class VIClumpInstructionBuilder
{
private:
    // The clump that instructions are being built for.
    VIClump*             _clump;

    // When instruciton building starts save the address of the fist instruciton in _firstInstructionPointer.
    InstructionCore*    _firstInstructionPointer;
    
    // When an instruction is made remember where its 'next' field is so that it can be
    // when the next instruction is generated.
    InstructionCore**   _pLastNextInstructionPointer;
    
    // When The instruciton set is commited the pointer to the first instruciton will be written
    // to the _pOriginalNextInstructionPointer in one operation.
    // If this can be gaurentedd to be atomic onthe target platform code paths can be changed
    // on the fly.
    InstructionCore**   _pOriginalNextInstructionPointer;
public:
    VIClumpInstructionBuilder();
    void                StartBuilding(VIClump* clump, InstructionCore** startLocation);
    void                CommitBuiltInstructions();
    InstructionCore*    AllocInstructionCore(Int32 argumentCount);
    InstructionCore*    CreateInstruction(TypeRef instructionType, Int32 argCount, void* args[]);
};

struct PatchInfo
{
    enum PatchType {
        Perch = 0,
        NamedTypes = 1,
    };
    
    PatchType   _patchType;
    void**      _whereToPeek;
    void**      _whereToPatch;
};

class ClumpParseState
{
    // The compiler (clang) really did not want to allow static const pointers so they are #defines
#define kPerchUndefined     ((InstructionCore*)0)    // What a branch sees when it is used before a perch
#define kPerchBeingAlocated ((InstructionCore*)1)    // Perches awaiting the next instruction address see this
    static const Int32 kMaxPerches = 200;   // TODO dynamic
    static const Int32 kMaxArguments = 100; // TODO dynamic
    static const Int32 kMaxPatchInfos = 100; // TODO allow more
public:
    EventLog*       _pLog;
    Int32           _argCount;
    void*           _argPointers[kMaxArguments];
    TypeRef         _argTypes[kMaxArguments];
    
    Int32           _argPatches[kMaxArguments];     // Arguments that need patching
    Int32           _argPatchCount;
    
    PatchInfo       _patchInfos[kMaxPatchInfos];
    Int32           _patchInfoCount;
    
    Int32           _perchCount;
    InstructionCore* _perches[kMaxPerches];
    
    VirtualInstrument *_vi;
    VIClump*        _clump;
    // ----
    Int32           _formalParameterIndex;
    TypeRef         _formalParameterType;
    // ----
    TypeRef         _paramBlockType;
    AQBlock1*       _paramBlockBase;
    TypedBlock*     _paramBlock;
    // ----
    TypeRef         _dataSpaceType;
    AQBlock1*       _dataSpaceBase;
    TypedBlock*     _dataSpace;
    
    // ---
    // The type that has the pointer to the specific target of the function.
    TypeRef         _instructionPointerType;
    
    // The calling signature descriptor for the instruciton's function. Its the BaseType Of the PointerType.
    TypeRef         _instructionType;
    
    // When a Perch instruciton is found the target address will be the next instruction
    // _recordNextInstructionAddress  lets the state know when that patchup needed.
    Int32           _recordNextInstructionAddress;
    
    size_t*          _pVarArgCount;
    Boolean         _bIsVI;
    
    // Class to help thread instrucitons together.
    VIClumpInstructionBuilder _builder;
    
    //------------------------------------------------------------
                    ClumpParseState(VIClump* clump, EventLog* pLog);
    void            StartSnippet(InstructionCore** startLocation);
    TypeRef         FormalParameterType();
    TypeRef         ReadFormalParameterType();
    void            SetClumpFireCount(Int32 fireCount);
    TypeRef         StartInstruction(SubString* opName);
    TypeRef         ReresolveInstruction(SubString* opName, bool allowErrors);
    TypeRef         ResolveActualArgumentAddress(SubString* argument, AQBlock1** ppData);
    NIError         AddDataTargetArgument(SubString* argument, Boolean prependType);
    NIError         AddStaticString(SubString* argument);
    void            InternalAddArg(TypeRef actualType, void* arg);
    void            InternalAddArgNeedingPatch(PatchInfo::PatchType patchType, void** whereToPeek);
    Boolean         VarArgParameterDetected()   { return _pVarArgCount != null; }
    Boolean         GenericFunction()           { return _instructionType->HasGenericType(); }
    void            AddVarArgCount();
    void            MarkPerch(SubString* perchToken);
    NIError         AddBranchTargetArgument(SubString* branchTargetToken);
    NIError         AddClumpTargetArgument(SubString* clumpIndexToken);
    VirtualInstrument*  AddSubVITargetArgument(SubString* subVIName);
    NIError         AddInstructionFunctionArgument(SubString* instructionNameToken);
    InstructionCore*    EmitCallVIInstruction();
    InstructionCore*    EmitInstruction();
    void            CommitSubSnippet();
    void            EmitClumpEnd();
};

typedef InstructionCore* (VIVM_FASTCALL _PROGMEM *GenericEmitFunction) (ClumpParseState*);

#endif
}
#endif //VirtualInstrument_h

