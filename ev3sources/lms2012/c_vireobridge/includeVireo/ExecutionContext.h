/**
 
Copyright (c) 2014 National Instruments Corp.
 
This software is subject to the terms described in the LICENSE.TXT file
 
SDG
*/


#ifndef ExecutionContext_h
#define ExecutionContext_h

#include "TypeAndDataManager.h"
#include "Instruction.h"

namespace Vireo
{
//------------------------------------------------------------
class VIClump;
class FunctionClump;

class Queue
{
public :
	VIClump* _head;
	VIClump* _tail;
public:
	Queue();
	Boolean IsEmpty() { return (this->_head == null); }
	VIClump* Dequeue();
	void Enqueue(VIClump*);
};

enum ExecutionState
{
    kExecutionState_None = 0,
    kExecutionState_ClumpsInRunQueue = 0x01,
    kExecutionState_ClumpsWaitingOnTime = 0x02,
    kExecutionState_ClumpsWaitingOnQueues = 0x04,
    kExecutionState_ClumpsWaitingOnISRs = 0x08,
};

// Each thread can have at most one ExecutionContext (ECs). ExecutionContexts can work
// cooperatively with other thread operations much like a message pump does. ECs
// may be the only tasks a thread has. 
//
// All access to the outside , graphics, time, IO
// needs to be derived from an object connected to the context.

#ifdef VIVM_SINGLE_EXECUTION_CONTEXT
    #define ECONTEXT static
#else
    #define ECONTEXT 
#endif

//------------------------------------------------------------
class ExecutionContext
{

private:
    TypeManager* _theTypeManager;
public:
    TypeManager* TheTypeManager()    { return _theTypeManager; }

private:
    ECONTEXT    Queue         _runQueue;				// Elts ready To run
	ECONTEXT    VIClump*      _sleepingList;			// Elts waiting for something external to wake them up
	ECONTEXT    CounterType   _breakoutCount;

public:
	ExecutionContext(TypeManager* typeManager);
	ECONTEXT    uTicTimeType    MicroSeconds();
#ifdef VIVM_SUPPORTS_ISR
    ECONTEXT    VIClump*        _triggeredIsrList;               // Elts waiting for something external to wake them up
    ECONTEXT    void            IsrEnqueue(QueueElt* elt);
#endif
	ECONTEXT    VIClump*        RunngQueueElt() {return _runningQueueElt;}
    ECONTEXT    void            CheckOccurrences(uTicTimeType t);		// Will put items on the run queue if it is time. or ready bit is set.

    // Run a string of instructions to completion, no concurrency. 
    ECONTEXT    void            ExecuteFunction(FunctionClump* fclump);  // Run a simple function to completion.
    
    // Run the concurrent execution system for a short period of time
	ECONTEXT    ExecutionState  ExecuteSlices(Int32 numSlices);
	ECONTEXT    InstructionCore* SuspendRunningQueueElt(InstructionCore* whereToWakeUp);
	InstructionCore*            Stop();
    ECONTEXT    void            ClearBreakout() { _breakoutCount = 0; }

	void Trace(const char* message);
	InstructionCore*            WaitMicroseconds(Int64 count, InstructionCore* next);
	ECONTEXT    void            EnqueueRunQueue(VIClump* elt);
	ECONTEXT    VIClump*        _runningQueueElt;		// Element actually running
    
private:
    static Boolean _classInited;
    static Instruction0 _culDeSac;
    static InstructionFunction _culDeSacFunction;
public:
    static inline Boolean IsCulDeSac(InstructionCore* pInstruciton) { return pInstruciton->_function == _culDeSacFunction; }

    static void ClassInit();
};

#ifdef VIVM_SINGLE_EXECUTION_CONTEXT
    // A single global instance allows allows all field references
    // to resolver to a fixed global address. This avoid pointer+offset
    // instructions that are costly on small MCUs
    extern ExecutionContext gSingleExecutionContext;
    #define THREAD_EXEC()	(&gSingleExecutionContext)
#else
//    extern VIVM_THREAD_LOCAL ExecutionContext* gpExec;
#endif


class ExecutionContextScope
{
#ifndef VIVM_SINGLE_EXECUTION_CONTEXT
    ExecutionContext* _saveExec;
    TypeManagerScope  _typeManagerScope;
    VIVM_THREAD_LOCAL static ExecutionContext* _threadsExecutionContext;

public:
    ExecutionContextScope(ExecutionContext* context)
    : _typeManagerScope(context->TheTypeManager())
    {
        _saveExec = _threadsExecutionContext;
        _threadsExecutionContext = context;
    }
    ~ExecutionContextScope()
    {
        _threadsExecutionContext = _saveExec;
    }
    static ExecutionContext* Current()
    {
        return (ExecutionContext*) _threadsExecutionContext;
    }
#else
    ExecutionContextScope(ExecutionContext* context) {}
    ~ExecutionContextScope() {}
#endif
    
    #define THREAD_EXEC() ExecutionContextScope::Current()
};
    
//------------------------------------------------------------
// When runtime functions need stack based instances of a
// TypeAndDataManager array values they can use the StackVar class.
// Its not intended for non stack use since the life time
// of TypeAndDataMangert values are generally connected to data spaces.
// The macro prevents the need for C++ RTTI.

#define STACK_VAR(_t_, _v_) StackVar<_t_> _v_(#_t_)
template <class T>
class StackVar
{
public:
    T *Value;
    StackVar(const char* tName)
    {
        SubString stringTypeName(tName);
        static TypeRef type = null;
        if (!type) {
            type = THREAD_EXEC()->TheTypeManager()->FindType(&stringTypeName);
            VIREO_ASSERT(type->IsArray() && !type->IsFlat());
        }
        Value = null;
        if (type) {
            type->InitData(&Value);
        }
    }
    ~StackVar()
    {
        if (Value) {
            Value->Type()->ClearData(&Value);
        }
    };
};
    
} // namespace Vireo

#endif //ExecutionContext_h
