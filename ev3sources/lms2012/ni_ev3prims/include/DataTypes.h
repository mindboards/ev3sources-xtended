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

#ifndef DataTypes_h
#define DataTypes_h

#include "string.h"     // strlen()

#include "BuildConfig.h"

#include "Platform.h"

namespace Vireo
{

enum { kVariableSizeSentinel = INT32_MIN };

typedef Int32 CounterType;

typedef enum {
    kNIError_Success = 0,
    kNIError_kInsufficientResources = 1,// Typically memory
    kNIError_kResourceNotFound = 2,
    kNIError_kArrayRankMismatch = 3,    // Arrays ranks do not fir function requirements (typically they must be the same)
    kNIError_kCantDecode = 4,           // Data in stream does not fit grammar
    kNIError_kCantEncode = 5,           // Data type not supported by encoder
    kNIError_kLogicFailure = 6,
    kNIError_ValueTruncated = 7,
} NIError ;

// Types used for structs and pointers when the symantics of the underlying bits may vary
typedef UInt8   AQBlock1;
typedef Int16   AQBlock2;
typedef Int32   AQBlock4;
typedef Int64   AQBlock8;

// Type used for index to most collections (may be Int32 on Int64 machines)
typedef VIVM_ARRAY_INDEX        IntIndex;

#if defined (__ARDUINO__)
	typedef UInt32 uTicTimeType;
#else
	typedef UInt64 uTicTimeType;
#endif
    
#ifdef __cplusplus
#define null 0
#else
#define null ((void *)0)
#endif

#define VIVM_CORE_ASSERT( _TEST_ ) VireoAssert_Hidden( _TEST_, #_TEST_, __FILE__, __LINE__ );
void VireoAssert_Hidden(Boolean test, const char* message, const char* file, int line);

//------------------------------------------------------------
template <class T>
class SimpleSubVector
{
protected:
    const T*  _begin;
    const T*  _end;
public:
    SimpleSubVector()
    {
        _begin = _end = null;
    }
    
    SimpleSubVector(const T* begin, const T* end)
    {
        assign(begin, end);
    }
    
    void AliasAssign(SimpleSubVector *subVector)
    {
        if (subVector) {
            _begin = subVector->Begin();
            _end = subVector->End();
        } else {
            _begin = null;
            _end = null;
        }
    }

    void AliasAssign(const T* begin, const T* end)
    {
        _begin = begin;
        _end = end;
    }
    
    IntIndex CopyToBoundedBuffer(IntIndex bufferSize, T* buffer)
    {
        IntIndex length = Length();
        if (bufferSize < length)
            length = bufferSize;
        memcpy(buffer, Begin(), length * sizeof(T));
        return length;
    }

    const T*  Begin() const  { return _begin; }
    const T*  End()   const  { return _end; }
    IntIndex Length()  const   { return (IntIndex)(_end - _begin); }
    bool Compare(const T* begin2, Int32 length2)
    {
        return (length2 == Length() && (memcmp(_begin, begin2, Length()) == 0));
    }
    
    bool Compare(const SimpleSubVector *subVector)
    {
        return Compare(subVector->Begin(), subVector->Length());
    }
};

}

#endif // DataTypes_h
