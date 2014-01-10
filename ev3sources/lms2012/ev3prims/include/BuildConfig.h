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

#ifndef BuildConfig_h
#define BuildConfig_h

#if (linux == 1)
	#define kVireoOS_linuxU 1
#endif

#if defined(WIN32)
	#define kVireoOS_win32U 1
#endif

#define INT8_MAX         127
#define INT16_MAX        32767
#define INT32_MAX        2147483647
#define INT64_MAX        9223372036854775807LL
#define INT8_MIN         -128
#define INT16_MIN        -32768

#ifndef INT32_MIN
    #define INT32_MIN        (-INT32_MAX-1)
#endif

#ifndef INT64_MIN
    #define INT64_MIN        (-INT64_MAX-1)
#endif



// Definitions common for most platforms
// PLatfrom specifici overrides are found inthe sections below
#define VIVM_UNROLL_EXEC

#define VIVM_TYPE_MANAGER

#define VIREO_MAIN main

// VIVM_FASTCALL if there is a key word that allows functions to use register
// passing this may help. It has for windows Not currently used.
// ARM usesregisters always. and clang x86/x64 uses registers
#define VIVM_FASTCALL           // define to empty sring

#define VIVM_INSTRUCTION_LINKAGE  static

#define VIVM_SUPPORTS_COMPLEX_NUMBERS 1

#define VIVM_ARRAY_INDEX Int32

// VIVM_DYNAMIC_ALLOCATION - legacy arduino project, but needs to be revived
// the runtime can be linked with structures statically created. In this
// case there is not need to dynamically create Instructions, Cumps, VIs, etc.
#define VIVM_DYNAMIC_ALLOCATION 1

#define USE_C99_TYPES

#define VIREO_SUPPORTS_FULL_STL

#define VIVM_BREAKOUT_COUNT 20

#define VIREO_PERF_COUNTERS

// TODO how to define globals as thread local?
#define VIVM_THREAD_LOCAL

#define VIREO_POSIX_FILEIO 1

//------------------------------------------------------------
#if defined(__ARDUINO__)

    //#define VIVM_HARVARD
    #include <avr/pgmspace.h>

    #define kVireoOS_wiring

    #define VIREO_PACKED_INSTRUCTIONS

    #define VIVM_SUPPORTS_ISR
    #define VIVM_ISR_DISABLE  cli();
    #define VIVM_ISR_ENABLE  sei();

    #define VIVM_SUPPORTS_FUNCTION_REGISTRATION_BY_NUMBER
    #define VIVM_SINGLE_EXECUTION_CONTEXT
    #define VIVM_USING_ASSERTS

    #define VIVM_BREAKOUT_COUNT 10
    #define VIVM_INSTRUCTION_LINKAGE  extern "C"

    #ifdef VIVM_ENABLE_TRACE
        #define VIVM_TRACE(message)  {Serial.print(message); Serial.print("\n");}
        #define VIVM_TRACE_FUNCTION(name)   VIVM_TRACE(name)
    #endif

    #define VIREO_EXPORT extern "C"

#elif defined(__PIC32MX__)

    //------------------------------------------------------------
    #define kVireoOS_wiring

    #define VIREO_PACKED_INSTRUCTIONS

    #define VIVM_SUPPORTS_ISR
    #define VIVM_ISR_DISABLE  
    #define VIVM_ISR_ENABLE 

    #define VIVM_SUPPORTS_FUNCTION_REGISTRATION_BY_NUMBER
    #define VIVM_SINGLE_EXECUTION_CONTEXT
    #define VIVM_USING_ASSERTS

    #define VIVM_BREAKOUT_COUNT 10
    #define VIVM_INSTRUCTION_LINKAGE  extern "C"

    #ifdef VIVM_ENABLE_TRACE
        #define VIVM_TRACE(message)  {Serial.print(message); Serial.print("\n");}
    #endif
    #define VIREO_EXPORT extern "C"

//------------------------------------------------------------
#elif defined (__APPLE__)

    #define kVireoOS_macosxU 1
    #define VIREO_DATE_TIME_STDLIB
    //#define VIREO_PACKED_INSTRUCTIONS

    //#define VIVM_SUPPORTS_ISR
    #define VIVM_ISR_DISABLE
    #define VIVM_ISR_ENABLE

    #define VIVM_SUPPORTS_NI_AAL
    #define VIVM_SUPPORTS_FUNCTION_REGISTRATION_BY_NAME
    #define VIVM_USING_ASSERTS

#if __LP64__
//    #define VIVM_ARRAY_INDEX Int64
#endif
    //------------------------------------------------------------
    // Macros for tracking memory usage.
    // They can only be used in context where TheDataManager
    // is a visible method.
    #define VIREO_EXPORT extern "C"

//------------------------------------------------------------
#elif defined (__OS_ANDROID__)

    #define VIVM_ISR_DISABLE
    #define VIVM_ISR_ENABLE

    #define VIREO_DATE_TIME_STDLIB

    #define VIVM_SUPPORTS_NI_AAL
    #define VIVM_SUPPORTS_FUNCTION_REGISTRATION_BY_NAME
    #define VIVM_USING_ASSERTS

    //------------------------------------------------------------
    // Macros for tracking memory usage.
    // They can only be used in context where TheDataManager
    // is a visible method.
    #define VIREO_EXPORT extern "C"

#elif (kVireoOS_win32U || kVireoOS_win64U)
    #define snprintf _snprintf
    #define isinf !_finite
    #define isnan _isnan
    #define VIREO_MAIN  __cdecl main

    #define VIREO_DATE_TIME_STDLIB
	#define _HAS_EXCEPTIONS 0
    //#define VIVM_SUPPORTS_ISR
    #define VIVM_ISR_DISABLE
    #define VIVM_ISR_ENABLE

    #define VIVM_SUPPORTS_NI_AAL
    #define VIVM_SUPPORTS_FUNCTION_REGISTRATION_BY_NAME
    #define VIVM_USING_ASSERTS
    #define VIVM_FASTCALL     // TODO any register mechanism supported
    #define VIVM_THREAD_LOCAL __declspec(thread)

    #define USE_C99_TYPES

    #define VIREO_EXPORT extern "C" __declspec(dllexport)

    //------------------------------------------------------------
    // Macros for tracking memory usage.
    // They can only be used in context where TheDataManager
    // is a visible method.

#elif kVireoOS_linuxU

    #define VIREO_DATE_TIME_STDLIB

    #define _HAS_EXCEPTIONS 0
    //#define VIVM_SUPPORTS_ISR
    #define VIVM_ISR_DISABLE
    #define VIVM_ISR_ENABLE

    #define VIVM_SUPPORTS_FUNCTION_REGISTRATION_BY_NAME
    #define VIVM_USING_ASSERTS

    #define USE_C99_TYPES

    #define VIREO_EXPORT extern "C"

    //------------------------------------------------------------
    // Macros for tracking memory usage.
    // They can only be used in context where TheDataManager
    // is a visible method.

#elif defined (kVireoOS_emscripten)
    #define VIREO_DATE_TIME_STDLIB

    #define _HAS_EXCEPTIONS 0
    //#define VIVM_SUPPORTS_ISR
    #define VIVM_ISR_DISABLE
    #define VIVM_ISR_ENABLE

    #define VIVM_SUPPORTS_FUNCTION_REGISTRATION_BY_NAME
    #define VIVM_USING_ASSERTS

    #define USE_C99_TYPES

    #define VIREO_EXPORT extern "C"

#elif kVireoOS_vxworks
    #undef VIREO_POSIX_FILEIO
    #define VIREO_DATE_TIME_VXWORKS

    #define _HAS_EXCEPTIONS 0
    #define VIVM_ISR_DISABLE
    #define VIVM_ISR_ENABLE

    #define VIVM_SUPPORTS_FUNCTION_REGISTRATION_BY_NAME
    #define VIVM_USING_ASSERTS
    #define VIVM_THREAD_LOCAL

    #define USE_C99_TYPES

    #define VIREO_EXPORT extern "C"

    //------------------------------------------------------------
    // Macros for tracking memory usage.
    // They can only be used in context where TheDataManager
    // is a visible method.

#else

   #error : need to define this platfrom
#endif

#ifndef VIVM_ENABLE_TRACE
    #define VIVM_TRACE(message)
    #define VIVM_TRACE_FUNCTION(name)
#endif

#endif // BuildConfig_h
