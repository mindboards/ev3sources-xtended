/**
 
Copyright (c) 2014 National Instruments Corp.
 
This software is subject to the terms described in the LICENSE.TXT file
 
SDG
*/


#ifndef Platform_h
#define Platform_h

namespace Vireo
{

#if defined (__APPLE__)
	typedef char  int8, Int8;
	typedef unsigned char Boolean;
#else
	typedef signed char int8, Int8;
	typedef bool Boolean;
#endif

typedef unsigned char       UInt8, Utf8Char;
typedef short               Int16;
typedef unsigned short      UInt16, Utf16Char;
#if __LP64__
typedef int                 Int32;
typedef unsigned int        UInt32;
#else
typedef int                 Int32;
typedef unsigned int        UInt32;
#endif
typedef long long           Int64;
typedef unsigned long long  UInt64;
typedef float               Single;
typedef double              Double;

// For places where the Vireo kenel function hold used data
// these types can be used.
typedef Int64               IntMax;
typedef UInt64              UIntMax;

//This two lines copied from lvthread.h
//struct VIREOHMUTEXREC;
//typedef struct VIREOHMUTEXREC *VireoMutex;
class Mutex
{
private:
    void* _nativeMutex;
public:
    Mutex();
    ~Mutex();
    void Acquire();
    void Release();
};

class MutexedScope
{
private:
    Mutex *_mutex;
public:
    MutexedScope(Mutex* pMutex)
        { _mutex = pMutex; _mutex->Acquire(); }
    ~MutexedScope()
        { _mutex->Release(); }
};


class Platform
{
public:
	static Int64 MicroSecondCount();
	//static Int64 MilliSecondCount();
	//static Int64 NanoSecondCount();

	static void AssertFailed(const char* message, const char* file, int line);
};

}

#endif //Platform_h
