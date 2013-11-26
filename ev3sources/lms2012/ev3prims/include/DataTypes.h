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
    kNIError_kInsufficientResources = 1,
    kNIError_kResourceNotFound = 2,
    kNIError_kArrayRankMismatch = 3,    // Arrays ranks do not fir function requirements (typically they must be the same)
    kNIError_kCantDecode = 4,           // Data in stream does not fit grammar
    kNIError_kLogicFailure = 5,
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
class typed_subvector
{
protected:
    const T*  _begin;
    const T*  _end;
public:
    typed_subvector()
    {
        _begin = _end = null;
    }
    
    typed_subvector(const T* begin, const T* end)
    {
        assign(begin, end);
    }
    
    void AliasAssign(typed_subvector *subVector)
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
    
    void AliasAssignCStr(const T* begin)
    {
        _begin = begin;
        _end = _begin + strlen(_begin);
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
    
    bool Compare(typed_subvector *subVector)
    {
        return Compare(subVector->Begin(), subVector->Length());
    }
};

enum TokenTraits
{
    TokenTraits_UnRecognized = 0,
    TokenTraits_SingleQuotedString = 1, //  'abc'
    TokenTraits_DoubleQuotedString = 2, //  "ABC"
    TokenTraits_EscapeSequences = 2048, //  "abc\""  escape sequence is not decoded, 
    TokenTraits_AlphaNum = 4,           //  a123
    TokenTraits_Integer = 8,            //  123
    TokenTraits_Negative = 16,          //  -123
    TokenTraits_RealNumber = 32,        //  1.23    also allow for scientific, inf, nan
    TokenTraits_EmbeddedDots = 64,      //  a.b.c.d  returned as one token
    TokenTraits_WildCard = 128,         //  *
    TokenTraits_Parens = 256,           //  ()    typically added to others to allow expression
    TokenTraits_Boolean = 512,          //  t or f
    TokenTraits_Comma = 1024,

    TokenTraits_Any  = 0xFF, // TODO add all the fields above
};
    
//------------------------------------------------------------
class SubString : public typed_subvector<char>
{
public:
    static Boolean IsAscii(char c)      { return !(c & 0x10); }
    static Boolean IsEolChar(char c)    { return (c == '\r') || (c == '\n'); }
    static Boolean IsSpaceChar(char c)  { return (c == ' ') || (c == '\t') || IsEolChar(c); }
    static Boolean IsNumberChar(char c) { return (c >= '0' && c <= '9'); }
    static Boolean IsIdentifierChar(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_'); };
    static Boolean IsSymbolChar(char c) { return (c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '^') || (c >= '{' && c <= '~'); };
public:
    SubString()    {}
    SubString(const char * begin) { AliasAssign(begin, begin ? (begin + strlen(begin)) : begin);}
    SubString(const Utf8Char * begin, const Utf8Char *end) { AliasAssign((const char*)begin, (const char*)end);}
    SubString(const char * begin, const char *end) { AliasAssign(begin, end);}
    SubString(SubString* original)  {AliasAssign(original->Begin(), original->End());}
    
    void EatToEol();
    void EatLeadingSpaces();
    void EatOptionalComma();
    Int32 ReadEscapeToken(SubString* token);
    bool SplitString(SubString* beforeMatch, SubString* afterMatch, char separator);

    Int32 LengthAferProcessingEscapes();
    void ProcessEscapes(char* begin, char* end);
    
    Boolean Compare(SubString *string)
    {
        return Compare(string->Begin(), string->Length());
    }
    
    Boolean Compare(const char* begin, IntIndex length);
    Boolean Compare(const char* begin);
    Boolean ComparePrefix(const char* begin, Int32 length);
    Boolean ComparePrefix(const char* begin) { return ComparePrefix (begin, (IntIndex)strlen(begin)); }
    Boolean ReadRawChar(char* token);
    Boolean ReadChar(char token);
    Boolean ReadChar(const char* token) { return ReadChar(token[0]);};  // TODO depricate, one used one place and thath place is depricated
    Boolean ReadInt(Int32* value);
    Boolean ParseDouble(double* value);
    Boolean ReadToken(SubString* token);
    void TrimQuotedString();
    TokenTraits ReadValueToken(SubString* token, TokenTraits allowedTraits);
};

// Macro to help with %.* formats. Example => printf("%.*s", FMT_LEN_BEGIN(arg))
#define FMT_LEN_BEGIN(_substring_)   (int)(_substring_)->Length(), (_substring_)->Begin()

class ComapreSubString
{
public:
    bool operator()(const SubString &a, const SubString &b) const
    {
        Int32 aSize = a.Length();
        Int32 bSize = b.Length();
        if (aSize != bSize) {
            return aSize < bSize;
        } else {
            const char* ba = a.Begin();
            const char* bb = b.Begin();
            const char* ea = a.End();
            
            while (ba < ea) {
                if (*ba !=  *bb) {
                    return *ba < *bb;
                } else {
                    ba++;
                    bb++;
                }
            }
            return false;
          //  return strncmp(a.Begin(), b.Begin(), aSize) < 0;
        }
    }
};

}

#include "TypeAndDataManager.h"

#endif // DataTypes_h
