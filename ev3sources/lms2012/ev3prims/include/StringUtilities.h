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

#ifndef StringUtilities_h
#define StringUtilities_h

#include "DataTypes.h"

namespace Vireo {

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
class SubString : public SimpleSubVector<Utf8Char>
{
    public:
    static Boolean IsAscii(char c)      { return !(c & 0x10); }
    static Boolean IsEolChar(char c)    { return (c == '\r') || (c == '\n'); }
    static Boolean IsSpaceChar(char c)  { return (c == ' ') || (c == '\t') || IsEolChar(c); }
    static Boolean IsNumberChar(char c) { return (c >= '0' && c <= '9'); }
    static Boolean IsIdentifierChar(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_'); };
    static Boolean IsSymbolChar(char c) { return (c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '^') || (c >= '{' && c <= '~'); };
    public:
    SubString()                         {}
    SubString(const char * begin)       { AliasAssign((const Utf8Char*)begin, (const Utf8Char*)(begin ? (begin + strlen(begin)) : begin)); }
    SubString(const Utf8Char * begin, const Utf8Char *end) { AliasAssign(begin, end); }
    SubString(SubString* original)      { AliasAssign(original->Begin(), original->End()); }
    void AliasAssignCStr(const char* begin) { AliasAssign((const Utf8Char*)begin, (const Utf8Char*)(begin + strlen(begin))); }
    
    void EatToEol();
    void EatLeadingSpaces();
    void EatOptionalComma();
    Int32 ReadEscapeToken(SubString* token);
    bool SplitString(SubString* beforeMatch, SubString* afterMatch, char separator) const;
    
    Int32 LengthAferProcessingEscapes();
    void ProcessEscapes(char* begin, char* end);
    
    Boolean Compare(const SubString *string)  const { return Compare(string->Begin(), string->Length()); }
    Boolean Compare(const Utf8Char* begin, IntIndex length) const;
    Boolean CompareCStr(const char* begin) const;
    Boolean ComparePrefix(const Utf8Char* begin, Int32 length) const ;
    Boolean ComparePrefixCStr(const char* begin) const { return ComparePrefix ((const Utf8Char*)begin, (IntIndex)strlen((const char*)begin)); }
    Boolean ReadRawChar(char* token);
    Boolean ReadChar(char token);
    Boolean ReadChar(const char* token) { return ReadChar(token[0]);};  // TODO depricate, one used one place and thath place is depricated
    Boolean ReadInt(IntMax* value);
    Boolean ParseDouble(Double* value);
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
            const Utf8Char* ba = a.Begin();
            const Utf8Char* bb = b.Begin();
            const Utf8Char* ea = a.End();
            
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

//------------------------------------------------------------
// Used for making short null terminated strings when needed for calling OS APIs
class TempStackCString
{
    enum { MaxLength = 255 };
private:
    Utf8Char    _buffer[MaxLength+1];
    Utf8Char*   _end;
public:
    
    TempStackCString(char* begin, Int32 length)
    {
        length = (length < MaxLength) ? length : MaxLength;
        _end = _buffer + length;
        memcpy(_buffer, begin, length);
        *_end = (Utf8Char) 0;
    }
    TempStackCString(SubString* string)
    {
        Int32 length = (string->Length() < MaxLength) ? string->Length() : MaxLength;
        _end = _buffer + length;
        memcpy(_buffer, string->Begin(), length);
        *_end = (Utf8Char) 0;
    }
    void Append(SubString* string)
    {
        Utf8Char* newEnd = _end + string->Length();
        if(newEnd > _buffer + MaxLength) {
            newEnd = _buffer + MaxLength;
        }
        size_t length = newEnd - _end;
        memcpy(_end, string->Begin(), length);
        _end = newEnd;
        *_end = (Utf8Char) 0;
    }
    
    char*       BeginCStr() { return (char*) _buffer; }
    Utf8Char*   End()       { return _end; }
};

} // namespace Vireo

#endif //StringUtilities_h
