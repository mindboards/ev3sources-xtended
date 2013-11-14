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

void Format(SubString *format, Int32 count, StaticTypeAndData arguments[], Utf8StringRef buffer);

#if 0
Utf16Char ToUpperInvariant(Utf16Char c);
Utf16Char ToUpper(Utf16Char c);
Utf16Char ToLowerInvariant(Utf16Char c);
Utf16Char ToLower(Utf16Char c);

void Utf16toAscii (Int32 count, Utf16Char* stringIn, char* stringOut);
void AsciiToUtf16 (Int32 count, const char* stringIn, Utf16Char* stringOut);
    IntIndex FindMatches(Utf16Array1D *stringIn, Utf16Array1D *searchString, Int32 offset, Boolean ignoreCase);
#endif


} // namespace Vireo

#endif //StringUtilities_h
