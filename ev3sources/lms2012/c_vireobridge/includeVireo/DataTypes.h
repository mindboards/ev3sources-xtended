/**
 
Copyright (c) 2014 National Instruments Corp.
 
This software is subject to the terms described in the LICENSE.TXT file
 
SDG
*/


#ifndef DataTypes_h
#define DataTypes_h

#include "string.h"     // strlen()

#include "BuildConfig.h"

#include "Platform.h"

namespace Vireo
{

//------------------------------------------------------------
#ifdef __cplusplus
#define null 0
#else
#define null ((void *)0)
#endif

//------------------------------------------------------------
enum { kVariableSizeSentinel = INT32_MIN };
// Type used for index to most collections (may be Int32 on Int64 machines)
typedef VIREO_ARRAY_INDEX_TYPE        IntIndex;

typedef Int32 CounterType;

//------------------------------------------------------------
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

// Types used for structs and pointers when the semantics of the underlying bits may vary
typedef UInt8   AQBlock1;
typedef Int16   AQBlock2;
typedef Int32   AQBlock4;
typedef Int64   AQBlock8;

//------------------------------------------------------------
#if defined (__ARDUINO__)
	typedef UInt32 uTicTimeType;
#else
	typedef UInt64 uTicTimeType;
#endif

//------------------------------------------------------------
//#define VIREO_ASSERT_SUPPRESS
#ifdef VIREO_ASSERT_SUPPRESS
    #define VIREO_ASSERT( _TEST_ )
#else
    #define VIREO_ASSERT( _TEST_ ) VireoAssert_Hidden( _TEST_, #_TEST_, __FILE__, __LINE__ );
    void VireoAssert_Hidden(Boolean test, const char* message, const char* file, int line);
#endif

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
