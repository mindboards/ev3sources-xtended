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

#ifndef Instruction_h
#define Instruction_h

#include "BuildConfig.h"

namespace Vireo
{

//------------------------------------------------------------
// For small MCUs special annotation is needed to read data from flash.
#ifdef VIVM_HARVARD
    #ifdef PROGMEM
        #define _PROGMEM PROGMEM
        // AVR uses GCC which supports 'typeof' extension Cx standard would be decltype, tough arduinos
        // gcc does not yet have full support for this function
        #define _PROGMEM_PTR(__this_ptr, _field) ((typeof(__this_ptr->_field))pgm_read_word(&(__this_ptr->_field)))
    #else
        #error : Need defines to support harvard architechure
    #endif
#else
    #define _PROGMEM
    #define _PROGMEM_PTR(__this_ptr, _field)  (__this_ptr->_field)
#endif

//------------------------------------------------------------
// The minimal Instructions "object"
struct InstructionCore;

typedef InstructionCore* (VIVM_FASTCALL _PROGMEM *InstructionFunction) (InstructionCore*);

struct InstructionCore
{
	InstructionFunction  _function;
#ifndef VIREO_PACKED_INSTRUCTIONS
	InstructionCore* _next;
#endif
};

struct Instruction0 : public InstructionCore
{
};

//------------------------------------------------------------
// Templates for making type specific instrucitons 
template <class type0>
struct Instruction1 : public Instruction0
{
    type0* _p0;
};

template <class type0, class type1>
struct Instruction2 : public Instruction1<type0>
{
    type1* _p1;
};

template <class type0, class type1, class type2>
struct Instruction3 : public Instruction2<type0, type1>
{
    type2* _p2;
};

template <class type0, class type1, class type2, class type3>
struct Instruction4 : public Instruction3<type0, type1, type2>
{
    type3* _p3;
};

template <class type0, class type1, class type2, class type3, class type4>
struct Instruction5 : public Instruction4<type0, type1, type2, type3>
{
    type4* _p4;
};

template <class type0, class type1, class type2, class type3, class type4, class type5>
struct Instruction6 : public Instruction5<type0, type1, type2, type3, type4>
{
    type5* _p5;
};

template <class type0, class type1, class type2, class type3, class type4, class type5, class type6>
struct Instruction7 : public Instruction6<type0, type1, type2, type3, type4, type5>
{
    type6* _p6;
};

struct VarArgInstruction : public InstructionCore
{
    size_t _count;
};

struct GenericInstruction : public InstructionCore
{
	void* _args[1];  // may be zero or more elements
};

//------------------------------------------------------------
#ifdef VIREO_PACKED_INSTRUCTIONS
    // Standard VarArg functions have the param count in the begining of the instruciton.
    #define _NextInstructionV() ( (InstructionCore*) ((size_t*)((VarArgInstruction*)_this + 1) + (int)_this->_count) )
    #define _NextInstruction() (_this + 1)
    //TODO tail calls dont work in VectorSnippers
    // #define _TailCallNextInstruction() (_this + 1)->_function(_this+1)
#else
    #define _NextInstructionV() (_this->_next)
    #define _NextInstruction() (_this->_next)
    // TODO tail calls dont work in VectorSnippers
    // #define _TailCallNextInstruction() (_this->_next->_function(_this->_next))
#endif

#define _Param(n)               (*_PROGMEM_PTR(_this, _p##n))
#define _ParamPointer(n)        (_PROGMEM_PTR(_this, _p##n))
#define _ParamVarArgCount()     ((int)_PROGMEM_PTR(_this, _count))
#define _ParamDef(t, n) t* _p##n
#define _ParamImmediateDef(t, n) t _pi##n
#define _ParamImmediate(n)      (_PROGMEM_PTR(_this, _pi##n))

#ifdef VIVM_PERF
#define VIVM_PERF_BODY	_this->_executionCount++;
#else
#define VIVM_PERF_BODY
#endif

#ifdef _DEBUG
#define VIVM_TAIL_CALLS_USE_JMP 0
#else
#define VIVM_TAIL_CALLS_USE_JMP 1
#endif

#if  VIVM_TAIL_CALLS_USE_JMP
// Compiler supports direct jump to last function. Save trip back to exec.
#define VIVM_TAIL(__instruction)  ((__instruction)->_function(__instruction))
#else
// Arbitrarily deep recursion doesn help, jsut return
#define VIVM_TAIL(__instruction)  (_this->_next)
#endif

//------------------------------------------------------------
//  Platform specifc high performance single parameter function declaration
//
//  Each Macro expands into the following
//      1. Instruction Fucntion prototype for that method
//      2. Type specific class that derives from from template instantiated to the specific parameter types
//      3. An init method that is used at least experiementaly on static allocation targets (e.g. arduino)
//      4. The begining of the function it self.  (prototype is necessary for the init method.
//


#define VIVM_FUNCTION_SIGNATURET(_name_, t)  extern InstructionCore* VIVM_FASTCALL _name_ (t* _this)

#define VIVM_FUNCTION_SIGNATURE0(_name_) \
    VIVM_INSTRUCTION_LINKAGE InstructionCore* VIVM_FASTCALL _name_ (InstructionCore* _this _PROGMEM);\
    struct  _name_##Struct : public InstructionCore {}; \
    VIVM_INSTRUCTION_LINKAGE InstructionCore* VIVM_FASTCALL _name_ (InstructionCore* _this _PROGMEM)

#define VIVM_FUNCTION_SIGNATURE1(_name_, t0) \
    VIVM_INSTRUCTION_LINKAGE InstructionCore* VIVM_FASTCALL _name_ (Instruction1<t0>* _this _PROGMEM ); \
    struct _name_##Struct : public Instruction1<t0>  {\
    void _Init(t0 *p0) {_function = (InstructionFunction)_name_; _p0 = p0;} }; \
    VIVM_INSTRUCTION_LINKAGE InstructionCore* VIVM_FASTCALL _name_ (Instruction1<t0>* _this _PROGMEM)

#define VIVM_FUNCTION_SIGNATURE2(_name_, t0, t1) \
    VIVM_INSTRUCTION_LINKAGE InstructionCore* VIVM_FASTCALL _name_ (Instruction2<t0, t1>* _this _PROGMEM);\
    struct _name_##Struct : public Instruction2<t0, t1> { \
    void _Init(t0 *p0, t1 *p1) {_function = (InstructionFunction)_name_; _p0 = p0; _p1 = p1;} }; \
    VIVM_INSTRUCTION_LINKAGE InstructionCore* VIVM_FASTCALL _name_ (Instruction2<t0, t1>* _this _PROGMEM)

#define VIVM_FUNCTION_SIGNATURE3(_name_, t0, t1, t2) \
    VIVM_INSTRUCTION_LINKAGE InstructionCore* VIVM_FASTCALL _name_ (Instruction3<t0, t1, t2>* _this _PROGMEM);\
    struct _name_##Struct : Instruction3<t0, t1, t2> { \
    void _Init(t0 *p0, t1 *p1, t2 *p2) {_function = (InstructionFunction)_name_; _p0 = p0; _p1 = p1; _p2 = p2;} }; \
    VIVM_INSTRUCTION_LINKAGE InstructionCore* VIVM_FASTCALL _name_ (Instruction3<t0, t1, t2>* _this _PROGMEM)

#define VIVM_FUNCTION_SIGNATUREV(_name_, _type_) \
    static InstructionCore* VIVM_FASTCALL _name_ (_type_* _this)

#define VIVM_FUNCTION_SIGNATURE4(_name_, t0, t1, t2, t3) \
    VIVM_INSTRUCTION_LINKAGE InstructionCore* VIVM_FASTCALL _name_ (Instruction4<t0, t1, t2, t3>* _this _PROGMEM);\
    struct _name_##Struct : Instruction4<t0, t1, t2, t3> { \
    void _Init(t0 *p0, t1 *p1, t2 *p2, t3 *p3) {_function = (InstructionFunction)_name_; _p0 = p0; _p1 = p1; _p2 = p2; _p3 = p3;} }; \
    VIVM_INSTRUCTION_LINKAGE InstructionCore* VIVM_FASTCALL _name_ (Instruction4<t0, t1, t2, t3>* _this _PROGMEM)

#define VIVM_FUNCTION_SIGNATURE5(_name_, t0, t1, t2, t3, t4)  extern InstructionCore* VIVM_FASTCALL _name_ (Instruction5<t0, t1, t2, t3, t4>* _this)

#define VIVM_FUNCTION_SIGNATURE6(_name_, t0, t1, t2, t3, t4, t5)  extern InstructionCore* VIVM_FASTCALL _name_ (Instruction6<t0, t1, t2, t3, t4, t5>* _this)

#define VIVM_FUNCTION_SIGNATURE7(_name_, t0, t1, t2, t3, t4, t5, t6)  extern InstructionCore* VIVM_FASTCALL _name_ (Instruction7<t0, t1, t2, t3, t4, t5, t6>* _this)

//------------------------------------------------------------
#define DECLARE_VIVM_PRIMITIVE0(_name_, _body_)	\
VIVM_FUNCTION_SIGNATURE0(_name_)				\
{										\
VIVM_TRACE_FUNCTION(#_name_)            \
VIVM_PERF_BODY							\
(_body_);								\
return _NextInstruction();				\
}

#define DECLARE_VIVM_PRIMITIVE1(_name_, _t0_, _body_)	\
VIVM_FUNCTION_SIGNATURE1(_name_, _t0_)				\
{										\
VIVM_TRACE_FUNCTION(#_name_)            \
VIVM_PERF_BODY							\
(_body_);								\
return _NextInstruction();				\
}

#define DECLARE_VIVM_PRIMITIVE2(_name_, _t0_, _t1_, _body_)	\
VIVM_FUNCTION_SIGNATURE2(_name_, _t0_, _t1_)				\
{										\
VIVM_TRACE_FUNCTION(#_name_)            \
VIVM_PERF_BODY							\
(_body_);								\
return _NextInstruction();				\
}

#define DECLARE_VIVM_PRIMITIVE3(_name_, _t0_, _t1_, _t2_, _body_)	\
VIVM_FUNCTION_SIGNATURE3(_name_, _t0_, _t1_, _t2_)				\
{										\
VIVM_TRACE_FUNCTION(#_name_)            \
VIVM_PERF_BODY							\
(_body_);								\
return _NextInstruction();              \
}

#define DECLARE_VIVM_PRIMITIVE4(_name_, _t0_, _t1_, _t2_, _t3_, _body_)	\
VIVM_FUNCTION_SIGNATURE4(_name_, _t0_, _t1_, _t2_, _t3_)				\
{										\
VIVM_TRACE_FUNCTION(#_name_)            \
VIVM_PERF_BODY							\
(_body_);								\
return _NextInstruction();				\
}

// This relies on tail call optimization for both directons
// putting code in to check for back branches dynamically would be a waste.
// Yields should be added by the instruciton thread builder for back branches.
// arg 0 is reserved for use as the "if true" target address
#define DECLARE_VIVM_CONDITIONAL_BRANCH(_name_, _t1_, _t2_, _test_) \
VIVM_FUNCTION_SIGNATURE3(_name_, InstructionCore, _t1_, _t2_) \
{									\
VIVM_PERF_BODY                      \
if (_test_)                         \
{									\
return _this->_p0;                  \
}									\
return VIVM_TAIL(_NextInstruction()); \
}

// This relies on tail call optimization for both directons
// putting code in to check for back branches dynamically would be a waste.
// Yields should be added by the instruciton thread builder for back branches.
// arg 0 is reserved for use as the "if true" target address
#define DECLARE_VIVM_CONDITIONAL_BRANCH1(_name_, _t1_,  _test_) \
VIVM_FUNCTION_SIGNATURE2(_name_, InstructionCore, _t1_) \
{									\
VIVM_PERF_BODY                      \
if (_test_)                         \
{									\
return _this->_p0;                  \
}									\
return VIVM_TAIL(_NextInstruction()); \
}

} // namespace Vireo
#endif //Instruction_h
