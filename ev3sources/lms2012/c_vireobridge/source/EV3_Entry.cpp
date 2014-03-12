/*
 * Copyright (c) 2013 National Instruments Corp.
 *
 * This file is part of the Vireo runtime module for the EV3.
 *
 * The Vireo runtime module for the EV3 is free software; you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * The Vireo runtime module for the EV3 is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "ExecutionContext.h"
#include "EggShell.h"
#include "CEntryPoints.h"

#include "EV3_Entry.h"
extern "C" {
#include "lms2012.h"
#include "c_dynload.h"
}

#define MEMACCESS_MAX_INPUTS_SIZE 1024

using namespace Vireo;

EggShell *pRootShell;
EggShell *pShell;

void vm_init(struct tVirtualMachineInfo *virtualMachineInfo)
{
#ifdef DEBUG_DYNLOAD
    fprintf(stderr, "LABVIEW: %s called: ", __func__);
#endif

    // Here's where the entry points are configured.
    // Not all of them have to be setup, as long as you know how
    // many there are.
    virtualMachineInfo->entryPointFunc[0] = &VireoInit;
    virtualMachineInfo->entryPointFunc[1] = &VireoStep;
    virtualMachineInfo->entryPointFunc[2] = &VireoMemAccess;

    (virtualMachineInfo->vm_exit) = &vm_exit;

#ifdef DEBUG_DYNLOAD
    fprintf(stderr, "done.\r\n");
#endif
}


void vm_exit()
{
#ifdef DEBUG_DYNLOAD
    fprintf(stderr, "LABVIEW: %s called\r\n", __func__);
#endif
    // Here's where the cleanup is done, dynamically allocated memory, buffers, that sort of thing
    if (pShell)
    {
        pShell->Delete();
        pShell = null;
    }
    if (pRootShell)
    {
        pRootShell->Delete();
        pRootShell = null;
    }
}

void VireoInit(void)
{
    try {
        // Get the fileName and pop off unused parameters.
        char *fileName = (char *) PrimParPointer();
        PrimParPointer();
        PrimParPointer();

        if (pShell)
        {
            pShell->Delete();
            pShell = null;
        }
        if (pRootShell)
        {
            pRootShell->Delete();
            pRootShell = null;
        }
        pRootShell = EggShell::Create(null);
        pShell = EggShell::Create(pRootShell);

        SubString  input;
        pShell->ReadFile(fileName, &input);
        pShell->REPL(&input);
    } catch (...) {
        SetDispatchStatus(FAILBREAK);
    }
}

void VireoStep()
{
    try {
        ExecutionState state = pShell->TheExecutionContext()->ExecuteSlices(400);

        // Store the execution state as a return value and pop off unused parameters.
        PrimParPointer();
        *(DATA8*)PrimParPointer() = (DATA8) (state != kExecutionState_None);
        PrimParPointer();
    } catch (...) {
        SetDispatchStatus(FAILBREAK);
    }
}

void VireoMemAccess()
{
    // Arguments for Eggshell peek and poke are passed in with PrimParPointer.
    // peekOrPoke, viName, and eltSize are packed into inputs.
    // result is passed in outputs, and dataSize is passed by itself.
    // data is passed in inputs for peek and in outputs for poke.
    try {
        char *inputs   = (char *) PrimParPointer();
        char *outputs  = (char *) PrimParPointer();
        Int16 dataSize = *(Int16 *)PrimParPointer();

        Int8 peekOrPoke = *(Int8 *)inputs;
        char *viName = inputs + 1;
        char *eltName;
        char *data;
        Int32 *result = (Int32 *) outputs;

        UInt16 offset = 1;
        while ((offset < MEMACCESS_MAX_INPUTS_SIZE) && inputs[offset++])
            ;
        eltName = inputs + offset;

        if (peekOrPoke == 0) { // peek
            if (offset >= MEMACCESS_MAX_INPUTS_SIZE)
                throw;
            data = outputs + sizeof(Int32);
            *result = EggShell_PeekMemory(pShell, viName, eltName, dataSize, data);
        } else if (peekOrPoke == 1) { // poke
            while ((offset < MEMACCESS_MAX_INPUTS_SIZE) && inputs[offset++])
                ;
            offset += 3 - ((offset - 1) % 4); // Make the data offset 4-byte aligned
            if (offset >= MEMACCESS_MAX_INPUTS_SIZE)
                throw;
            data = inputs + offset;
            *result = EggShell_PokeMemory(pShell, viName, eltName, dataSize, data);
        } else
            throw;
    } catch (...) {
        SetDispatchStatus(FAILBREAK);
    }
}

