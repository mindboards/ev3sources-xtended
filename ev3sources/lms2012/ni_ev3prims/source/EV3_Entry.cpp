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

using namespace Vireo;

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
    fprintf(stderr, "done.\n");
#endif
}


void vm_exit()
{
#ifdef DEBUG_DYNLOAD
    fprintf(stderr, "LABVIEW: %s called\n", __func__);
#endif
    // Here's where the cleanup is done, dynamically allocated memory, buffers, that sort of thing


}

EggShell *pRootShell;
EggShell *pShell;

void VireoInit(void)
{
    try {
        char *fileName = (char *) PrimParPointer();

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
        pShell->TheExecutionContext()->SetDelayedLoad(true);

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
        *(DATA8*)PrimParPointer() = (DATA8) state;
    } catch (...) {
        SetDispatchStatus(FAILBREAK);
    }
}

void VireoMemAccess()
{
    try {
        Int8 peekOrPoke = *(Int8 *) PrimParPointer();
        char *viName = (char *) PrimParPointer();
        char *eltName = (char *) PrimParPointer();
        Int32 bufferSize = *(Int32 *) PrimParPointer();
        char *buffer = (char *) PrimParPointer();
        Int32 *result = (Int32 *) PrimParPointer();

        if (peekOrPoke == 0)
            *result = EggShell_PeekMemory(pShell, viName, eltName, bufferSize, buffer);
        else
            *result = EggShell_PokeMemory(pShell, viName, eltName, bufferSize, buffer);
    } catch (...) {
        SetDispatchStatus(FAILBREAK);
    }
}

