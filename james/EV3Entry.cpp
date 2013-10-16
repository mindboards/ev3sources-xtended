//
//  Copyright (c) 2013 Paul Austin. All rights reserved.
//

// #include "DataTypes.h"
// #include "TypeAndDataManager.h"
// #include "TypeAndDataCodecUtf8.h"
#include "VirtualInstrument.h"
#include "ExecutionContext.h"
#include "EggShell.h"

#include "EV3Entry.h"
extern "C" {
#include "lms2012.h"
}

using namespace Vireo;

EggShell *pShell;

// This function is associated with opVIREO_INIT instruction.
// Initialize Vireo and read a program from a given filename.
void VireoInit(void)
{
    char *fileName = (char *) PrimParPointer();

    if (pShell)
        pShell->Delete();
    pShell = EggShell::Create(null);
    AsciiSubString  input;
    pShell->ReadFile(fileName, &input);
    pShell->REPL(&input);
}

// Step the execution of the current program.
void VireoStep()
{
    *(DATA8*)PrimParPointer() = (DATA8) pShell->TheExecutionContext()->ExecuteSlices(1);
}

