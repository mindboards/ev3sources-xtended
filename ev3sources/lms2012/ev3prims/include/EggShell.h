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

#ifndef EggShell_h
#define EggShell_h

namespace Vireo
{
 
class EggShell
{
public:
    static EggShell* Create(EggShell* parent);

private:
    TypeManager          *_typeManger;
    ExecutionContext    *_execContext;
public:
    Int64          _commandCount;
    SubString      _string;
    char*          _mallocBuffer;
    Boolean        _traceEnabled;

public:
    NIError REPL(SubString *commandBuffer);
    NIError ReadFile(const char* name, SubString *string);
    NIError ReadStdinLine(SubString *string);
    Int64   CommanCount() { return _commandCount;}
    Boolean ShowStats;
    NIError Delete();    
    TypeManager* TheTypeManager()           { return _typeManger; }
    ExecutionContext* TheExecutionContext() { return _execContext; }
private:
    EggShell(TypeManager *typeManger, ExecutionContext *execContext);
    NIError ParseEnqueueVI();
    NIError ParseDefine();
    NIError ParsePrint();
    NIError PrintType(TypeRef t, Int32 level);
};

} // namespace Vireo

#endif // EggShell_h
