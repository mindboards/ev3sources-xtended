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

extern "C" {
#include "c_input.h"
}

#include "ExecutionContext.h"
#include "StringUtilities.h"

using namespace Vireo;

VIVM_FUNCTION_SIGNATURE4(InputGetTypeMode, UInt8, UInt8, UInt8, UInt8)
{
    UInt8  layer = _Param(0);
    UInt8  no    = _Param(1);
    UInt8 *type  = _ParamPointer(2); // reference
    UInt8 *mode  = _ParamPointer(3); // reference

    DATA8 device = no + (layer * INPUT_PORTS);

    if (device < DEVICES)
    {
        *type = InputInstance.DeviceType[device];
        *mode = InputInstance.DeviceMode[device];
    }
    else
    {
        *type = TYPE_NONE;
        *mode = 0;
    }

    return _NextInstruction();
}

// Functionally equivalent to opINPUT_READ/cInputRead
VIVM_FUNCTION_SIGNATURE5(InputReadPct, UInt8, UInt8, UInt8, UInt8, UInt8)
{
    UInt8  layer = _Param(0);
    UInt8  no    = _Param(1);
    UInt8  type  = _Param(2);
    UInt8  mode  = _Param(3);
    UInt8 *pct   = _ParamPointer(4); // reference

    DATA8 device = no + (layer * INPUT_PORTS);

    if (device < DEVICES)
    {
        cInputSetType(device, type, mode, __LINE__);
    }
    *pct = cInputReadDevicePct(device,0,0,NULL);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE6(InputReadSi, UInt8, UInt8, UInt8, UInt8, UInt8, TypedArray1dCoreRef)
{
    UInt8  layer = _Param(0);
    UInt8  no    = _Param(1);
    UInt8  type  = _Param(2);
    UInt8  mode  = _Param(3);
    UInt8  count  = _Param(4);
    TypedArray1dCoreRef data = _Param(5); // single reference

    DATA8 device = no + (layer * INPUT_PORTS);
    cInputSetType(device, type, mode, __LINE__);
    data->Resize(count);

    for (UInt8 i = 0; i < count; i++)
    {
        if (device < DEVICES)
            *(Single *) data->BeginAt(i) = cInputReadDeviceSi(device,i,0,NULL);
        else
            *(Single *) data->BeginAt(i) = DATA32_NAN;
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE4(InputReadRaw, UInt8, UInt8, UInt8, TypedArray1dCoreRef)
{
    UInt8 layer = _Param(0);
    UInt8 no    = _Param(1);
    UInt8 count = _Param(2);
    TypedArray1dCoreRef data = _Param(3); // single reference

    DATA8 device = no + (layer * INPUT_PORTS);
    data->Resize(count);

    for (UInt8 i = 0; i < count; i++)
    {
        if (device < DEVICES)
            *(Single *) data->BeginAt(i) = cInputReadDeviceRaw(device,i,0,NULL);
        else
            *(Single *) data->BeginAt(i) = DATA32_NAN;
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE5(InputWriteRaw, UInt8, UInt8, UInt8, UInt8, TypedArray1dCoreRef)
{
    UInt8  layer = _Param(0);
    UInt8  no    = _Param(1);
    UInt8  type  = _Param(2);
    UInt8  count = _Param(3);
    TypedArray1dCoreRef data = _Param(4); // single

    DATA8 device = no + (layer * INPUT_PORTS);
    data->Resize(count);

    if (device < DEVICES)
        for (UInt8 i = 0; i < count; i++)
            cInputWriteDeviceRaw(device, InputInstance.DeviceData[device].Connection, type, *(Single *) data->BeginAt(i));

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE4(InputIicSetup, UInt8, UInt8, TypedArray1dCoreRef, TypedArray1dCoreRef)
{
    UInt8  layer  = _Param(0);
    UInt8  no     = _Param(1);
    TypedArray1dCoreRef command  = _Param(2); // uInt8
    TypedArray1dCoreRef response = _Param(3); // uInt8 reference

    DATA8 device = no + (layer * INPUT_PORTS);
    UInt8  repeat = 1;
    UInt16 time   = 0;

    if (cInputSetupDevice(device, repeat, time, command->Length(), (DATA8 *) command->BeginAt(0), response->Length(), (DATA8 *) response->BeginAt(0)))
        return _this;
    else
        return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE2(InputClearChanges, UInt8, UInt8)
{
    UInt8  layer = _Param(0);
    UInt8  no    = _Param(1);

    DATA8 device = no + (layer * INPUT_PORTS);

    if (device < DEVICES)
    {
        InputInstance.DeviceData[device].Changes = (DATA32)0;
        InputInstance.DeviceData[device].Bumps   = (DATA32)0;
    }

    return _NextInstruction();
}

#include "TypeDefiner.h"
VIREO_DEFINE_BEGIN(EV3_IO)
    VIREO_DEFINE_FUNCTION(InputGetTypeMode, "p(i(.UInt8),i(.UInt8),o(.UInt8),o(.UInt8))");
    VIREO_DEFINE_FUNCTION(InputReadPct, "p(i(.UInt8),i(.UInt8),i(.UInt8),i(.UInt8),o(.UInt8))");
    VIREO_DEFINE_FUNCTION(InputReadSi, "p(i(.UInt8),i(.UInt8),i(.UInt8),i(.UInt8),i(.UInt8),o(.Array))");
    VIREO_DEFINE_FUNCTION(InputReadRaw, "p(i(.UInt8),i(.UInt8),i(.UInt8),o(.Array))");
    VIREO_DEFINE_FUNCTION(InputWriteRaw, "p(i(.UInt8),i(.UInt8),i(.UInt8),i(.UInt8),i(.Array))");
    VIREO_DEFINE_FUNCTION(InputIicSetup, "p(i(.UInt8),i(.UInt8),i(.Array),o(.Array))");
    VIREO_DEFINE_FUNCTION(InputClearChanges, "p(i(.UInt8),i(.UInt8))");
VIREO_DEFINE_END()

