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
#include "VirtualInstrument.h"
#include "EggShell.h"
#include "CEntryPoints.h"

#include "EV3_Entry.h"
extern "C" {
#include "lms2012.h"
}

using namespace Vireo;

EggShell *pRootShell;
EggShell *pShell;

void VireoInit(void)
{
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
}

void VireoStep()
{
    *(DATA8*)PrimParPointer() = (DATA8) pShell->TheExecutionContext()->ExecuteSlices(20);
}

void VireoMemAccess()
{
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
}

