/*
 * Copyright (c) 2013 National Instruments Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

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

void VireoPeek()
{
    SubString VIName((char *) PrimParPointer());
    SubString name((char *) PrimParPointer());
    UInt16 count = *(UInt16*)PrimParPointer();
    void *ppp = PrimParPointer();

    VirtualInstrument *VI;
    TypedBlock *dataSpace;
    TypedBlock *paramBlock;
    AQBlock1 *pData = null;
    Int32 offset = 0;

    // TODO: Get rid of this
    memset(ppp, 0, 20);

    // If the shell or VI do not exist, do nothing.
    // TODO: Checking for a nonexistant VI causes a segfault.
    if (pShell == null || (VI = (VirtualInstrument *) pShell->TheExecutionContext()->TheTypeManager()->FindNamedTypedBlock(&VIName)) == null)
        return;

    dataSpace = VI->DataSpace();
    paramBlock = VI->ParamBlock();

    // Search the dataSpace and paramBlock for the desired element
    TypeRef actualType = dataSpace->Type()->GetSubElementFromPath(&name, &offset);
    if (actualType != null)
        pData = dataSpace->RawBegin() + offset;
    else if (paramBlock)
    {
        actualType = paramBlock->Type()->GetSubElementFromPath(&name, &offset);
        if (actualType != null)
            pData = paramBlock->RawBegin() + offset;
    }

    if (actualType != null)
    {
printf("ppp value = %x, data value = %x\r\n", *(Int32*)ppp, *(Int32*)pData);
        // Reply with the count and element width
        if (count == 0)
        {
            ((UInt16 *) ppp)[0] = actualType->IsArray() ? (*(TypedBlock **)pData)->Length() : 1;
            ((UInt16 *) ppp)[1] = actualType->IsArray() ? actualType->GetSubElement(0)->TopAQSize() : actualType->TopAQSize();
        }
        // Reply with an array which is assumed to be inlined
        else if (actualType->IsArray())
        {
            size_t aqlength = count * actualType->TopAQSize();
            memcpy(ppp, **(void ***)pData, aqlength);
        }
        // Reply with a scalar.
        else
            actualType->CopyData(pData, ppp, count);
printf("ppp value = %x, data value = %x\r\n\r\n", *(Int32*)ppp, *(Int32*)pData);
    }
}

void VireoPoke()
{
    SubString VIName((char *) PrimParPointer());
    SubString name((char *) PrimParPointer());
    UInt16 count = *(UInt16*)PrimParPointer();
    void *ppp = PrimParPointer();

    if (pShell)
    {
        VirtualInstrument *VI = (VirtualInstrument *) pShell->TheExecutionContext()->TheTypeManager()->FindNamedTypedBlock(&VIName);
        if (VI)
        {
            TypedBlock *dataSpace = VI->DataSpace();
            TypedBlock *paramBlock = VI->ParamBlock();

            AQBlock1 *pData = null;
            Int32 offset = 0;
            TypeRef actualType = dataSpace->Type()->GetSubElementFromPath(&name, &offset);
            if (actualType != null)
                pData = dataSpace->RawBegin() + offset;
            else if (paramBlock)
            {
                actualType = paramBlock->Type()->GetSubElementFromPath(&name, &offset);
                if (actualType != null)
                    pData = paramBlock->RawBegin() + offset;
            }

            if (actualType != null)
            {
printf("ppp value = %x, data value = %x\r\n", *(Int32*)ppp, *(Int32*)pData);
                actualType->CopyData(ppp, pData, count);
printf("ppp value = %x, data value = %x\r\n", *(Int32*)ppp, *(Int32*)pData);
            }
        }
    }
}

