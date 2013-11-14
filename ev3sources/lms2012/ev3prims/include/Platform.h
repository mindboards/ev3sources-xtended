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
typedef Double              float64;  // legacy for NITime support

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
