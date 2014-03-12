/**
 
Copyright (c) 2014 National Instruments Corp.
 
This software is subject to the terms described in the LICENSE.TXT file
 
SDG
*/


#ifndef EggShell_h
#define EggShell_h

namespace Vireo
{
 
class TDViaParser;

class EggShell
{
public:
    static EggShell* Create(EggShell* parent);

private:
    TypeManager          *_typeManger;
    ExecutionContext    *_execContext;
public:
    Int64          _commandCount;
    SubString*     _pString;
    char*          _mallocBuffer;

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
    NIError ParseDefine(TDViaParser* parser);
};

} // namespace Vireo

#endif // EggShell_h
