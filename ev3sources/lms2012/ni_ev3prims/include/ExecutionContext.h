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
// cooperativly wiht other thread operations much like a message pump does. ECs
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
    Boolean      _delayedLoad;
public:
    TypeManager* TheTypeManager()    { return _theTypeManager; }
    Boolean      DelayedLoad()       { return _delayedLoad; }
    void         SetDelayedLoad(Boolean value)  { _delayedLoad = value; }

private:
    ECONTEXT    Queue         _runQueue;				// Elts ready To run
	ECONTEXT    VIClump*      _sleepingList;			// Elts waiting for something external to wake them up
	ECONTEXT    CounterType   _breakoutCount;
	ECONTEXT    CounterType   _breakoutCountReset;
    
public:
	ExecutionContext(TypeManager* typeManager);
	ECONTEXT    uTicTimeType    MicroSeconds();
#ifdef VIVM_SUPPORTS_ISR
    ECONTEXT    VIClump*       _triggeredIsrList;               // Elts waiting for something external to wake them up
    ECONTEXT    void            IsrEnqueue(QueueElt* elt);
#endif
	ECONTEXT    VIClump*        RunngQueueElt() {return _runningQueueElt;}
    ECONTEXT    void            CheckOccurrences(uTicTimeType t);		// Will put items on the run queue if it is time. or ready bit is set.

    // Run a string of insrucitons to completion, no concurrency. 
    ECONTEXT    void            ExecuteFunction(FunctionClump* fclump);  // Run a simple function to completion.
    
    // Run the concurrent execution system for a short period of time
	ECONTEXT    ExecutionState  ExecuteSlices(Int32 numSlices);
	ECONTEXT    InstructionCore* SuspendRunningQueueElt(InstructionCore* whereToWakeUp);
	InstructionCore*            Done();
	InstructionCore*            Stop();
	void Trace(const char* message);
	InstructionCore*            WaitMicroseconds(Int64 count, InstructionCore* next);
	ECONTEXT    void            EnqueueRunQueue(VIClump* elt);
	ECONTEXT    VIClump*        _runningQueueElt;		// Eement actually running
    
public:
    static  Instruction0 _culDeSac;
    static  Boolean _classInited;
    static void ClassInit(); 
    
#ifdef VIVM_USING_ASSERTS
	void Assert_Hidden(Boolean test, const char* message, const char* file, int line);
    #define VIVM_ASSERT( _TEST_ ) THREAD_EXEC()->Assert_Hidden( _TEST_, #_TEST_, __FILE__, __LINE__ );
#else
    #define VIVM_ASSERT( _TEST_ )
#endif
};

#ifdef VIVM_SINGLE_EXECUTION_CONTEXT
    // A single global instance allows allows all field references
    // to resolver to a fixed globall address. This avoid pointer+offset
    // instructions taht are costly on small MCUs
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
// When runtime functions need stack based instances of a TypeAndDataManager
// they can use the StackVar class. Its not intended for non stack use since the life time
// of TypeAndDataMangert values are generally conneced to data spaces.
// The macro prevents the need for RTTI.
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
