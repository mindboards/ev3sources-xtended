/*
 * Copyright (c) 2013-2014 National Instruments Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

extern "C" {
#include "c_input.h"
}

#include "TypeAndDataManager.h"
#include "StringUtilities.h"

using namespace Vireo;

VIREO_FUNCTION_SIGNATURE4(InputGetTypeMode, UInt8, UInt8, UInt8, UInt8)
{
    UInt8  layer = _Param(0);
    UInt8  no    = _Param(1);
    UInt8 type;
    UInt8 mode;

    DATA8 device = no + (layer * INPUT_PORTS);

    if (device < DEVICES)
    {
        type = InputInstance.DeviceType[device];
        mode = InputInstance.DeviceMode[device];
    }
    else
    {
        type = TYPE_NONE;
        mode = 0;
    }

    if (_ParamPointer(2))
        _Param(2) = type;
    if (_ParamPointer(3))
        _Param(3) = mode;

    return _NextInstruction();
}

// Functionally equivalent to opINPUT_READ/cInputRead
VIREO_FUNCTION_SIGNATURE5(InputReadPct, UInt8, UInt8, UInt8, UInt8, UInt8)
{
    UInt8 layer = _Param(0);
    UInt8 no    = _Param(1);
    UInt8 type  = _Param(2);
    UInt8 mode  = _Param(3);
    UInt8 pct;

    DATA8 device = no + (layer * INPUT_PORTS);

    if (device < DEVICES)
    {
        cInputSetType(device, type, mode, __LINE__);
    }
    pct = cInputReadDevicePct(device,0,0,NULL);

    if (_ParamPointer(4))
        _Param(4) = pct;

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE6(InputReadSi, UInt8, UInt8, UInt8, UInt8, UInt8, TypedArrayCoreRef)
{
    UInt8  layer = _Param(0);
    UInt8  no    = _Param(1);
    UInt8  type  = _Param(2);
    UInt8  mode  = _Param(3);
    UInt8  count  = _Param(4);
    TypedArrayCoreRef data;

    if (_ParamPointer(5))
        data = _Param(5);
    else
        return _NextInstruction();

    DATA8 device = no + (layer * INPUT_PORTS);
    cInputSetType(device, type, mode, __LINE__);
    data->Resize1D(count);

    for (UInt8 i = 0; i < count; i++)
    {
        if (device < DEVICES)
            *(Single *) data->BeginAt(i) = cInputReadDeviceSi(device,i,0,NULL);
        else
            *(Single *) data->BeginAt(i) = DATA32_NAN;
    }

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE4(InputReadRaw, UInt8, UInt8, UInt8, TypedArrayCoreRef)
{
    UInt8 layer = _Param(0);
    UInt8 no    = _Param(1);
    UInt8 count = _Param(2);
    TypedArrayCoreRef data;

    if (_ParamPointer(3))
        data = _Param(3);
    else
        return _NextInstruction();

    DATA8 device = no + (layer * INPUT_PORTS);
    data->Resize1D(count);


    for (UInt8 i = 0; i < count; i++)
    {
        if (device < DEVICES)
            *(Single *) data->BeginAt(i) = cInputReadDeviceRaw(device,i,0,NULL);
        else
            *(Single *) data->BeginAt(i) = DATA32_NAN;
    }

    return _NextInstruction();
}

// opINPUT_READY
VIREO_FUNCTION_SIGNATURE2(InputReady, UInt8, UInt8)
{
    UInt8 layer = _Param(0);
    UInt8 no    = _Param(1);

    DATA8 device = no + (layer * INPUT_PORTS);

    if (device < DEVICES)
    {
        if (InputInstance.DeviceData[device].DevStatus == BUSY)
        {
            SetDispatchStatus(BUSYBREAK);
            return _this;
        }
    }

    return _NextInstruction();
}

// opINPUT_WRITE
VIREO_FUNCTION_SIGNATURE4(InputWrite, UInt8, UInt8, UInt8, TypedArrayCoreRef)
{
    UInt8  layer = _Param(0);
    UInt8  no    = _Param(1);
    UInt8  count = _Param(2);
    TypedArrayCoreRef data = _Param(3);

    DATA8   Tmp;
    DATA8   Buffer[UART_DATA_LENGTH + 1];
    DSPSTAT DspStat = FAILBREAK;
    DATA8 Device = no + (layer * INPUT_PORTS);

    if (data->Length() < (IntIndex) count)
        count = data->Length();

    if (Device < INPUT_DEVICES)
    {
        if (InputInstance.DeviceType[Device] != TYPE_TERMINAL)
        {
            if (InputInstance.DeviceData[Device].Connection == CONN_INPUT_UART)
            {
                if ((count > 0) && (count <= UART_DATA_LENGTH))
                {
                    if (((*InputInstance.pUart).Status[Device] & UART_WRITE_REQUEST))
                    {
                        DspStat  =  BUSYBREAK;
                    }
                    else
                    {
                        InputInstance.DeviceData[Device].DevStatus  =  BUSY;

                        (*InputInstance.pUart).Status[Device]      &= ~UART_DATA_READY;

                        Buffer[0]  =  Device;
                        for (Tmp = 0;Tmp < count;Tmp++)
                        {
                            Buffer[Tmp + 1]  =  *(DATA8 *)data->BeginAt(Tmp);
                        }

                        // write setup string to "UART Device Controller" driver
                        if (InputInstance.UartFile >= MIN_HANDLE)
                        {
                            write(InputInstance.UartFile,Buffer,count + 1);
                        }
                        DspStat  =  NOBREAK;
                    }
                }
            }
            else
            {
            // don't bother if not UART

            DspStat  =  NOBREAK;
            }
        }
        else
        { // don't bother if TERMINAL

            DspStat  =  NOBREAK;
        }
    }

    SetDispatchStatus(DspStat);

    return DspStat == BUSYBREAK ? _this : _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE5(InputIicSetup, UInt8, UInt8, TypedArrayCoreRef, UInt8, TypedArrayCoreRef)
{
    UInt8  layer  = _Param(0);
    UInt8  no     = _Param(1);
    TypedArrayCoreRef command  = _Param(2);
    DATA8 responseLength = _Param(3);
    DATA8 *responseBegin;

    if (_ParamPointer(4))
    {
        TypedArrayCoreRef response = _Param(4);
        response->Resize1D(responseLength);
        responseBegin = (DATA8 *) response->BeginAt(0);
    }
    else
    {
        responseLength = 0;
        responseBegin = 0;
    }

    DATA8 device = no + (layer * INPUT_PORTS);
    UInt8  repeat = 1;
    UInt16 time   = 0;
    RESULT result = BUSY;

    if (cInputSetupDevice(device, repeat, time, command->Length(), (DATA8 *) command->BeginAt(0), responseLength, responseBegin, &result) == BUSY)
        return _this;
    else
        return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE2(InputClearChanges, UInt8, UInt8)
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

VIREO_FUNCTION_SIGNATURE3(InputSetAutoID, UInt8, UInt8, UInt8)
{
    UInt8  layer  = _Param(0);
    UInt8  no     = _Param(1);
    UInt8  enable = _Param(2);

    DATA8 Device = no + (layer * INPUT_PORTS);

    if (Device >= INPUTS)
    {
        return _NextInstruction();
    }

    int Index;
    char Buf[6];

    // Configure auto-id
    Buf[0] = 'e';

    // Initialise auto-id string to do nothing
    for (Index = 0; Index < INPUTS; Index++)
    {
        Buf[Index + 1]    =  '-';
    }

    // NULL terminate for good karma
    Buf[5]      =  0;

    // Configure the port's auto-id
    Buf[Device + 1] = enable;

    // Write the string to the kernel module (/dev/lms_analog)
    write(InputInstance.AdcFile, Buf, 6);

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE3(InputSetConn, UInt8, UInt8, UInt8)
{
    UInt8  layer  = _Param(0);
    UInt8  no     = _Param(1);
    UInt8  conn   = _Param(2);

    DATA8 Device = no + (layer * INPUT_PORTS);

    if (Device >= INPUTS)
    {
        return _NextInstruction();
    }

    int Index;
    char Buf[6];

    // Configure the connection type
    Buf[0] = 't';

    // Initialise connection type setup string to do nothing
    for (Index = 0;Index < INPUTS;Index++)
    {
        Buf[Index + 1]    =  '-';
    }

    // NULL terminate for good karma
    Buf[5]      =  0;

    // Set the port to the specified connection
    switch (conn)
    {
        case CONN_NXT_IIC:
        case CONN_NXT_DUMB:
        case CONN_INPUT_DUMB:
        case CONN_NONE:
        {
            Buf[Device + 1] = conn;
            break;
        }
        default:
            return _NextInstruction();
    }

    // Write the string to the kernel module (/dev/lms_analog)
    write(InputInstance.AdcFile, Buf, 6);

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE5(InputIICRead, UInt8, UInt8, UInt8, TypedArrayCoreRef, UInt8)
{
    UInt8  layer   = _Param(0);
    UInt8  no      = _Param(1);
    UInt8  length  = _Param(2);
    TypedArrayCoreRef data = _ParamPointer(3) ? _Param(3) : null;
    UInt8 *pResult = _ParamPointer(4);

    DATA8 Device = no + (layer * INPUT_PORTS);

    if (Device >= INPUTS)
    {
        if (pResult)
            *pResult = FAIL;
        return _NextInstruction();
    }

    if (length > MAX_DEVICE_DATALENGTH)
        length = MAX_DEVICE_DATALENGTH;

    InputInstance.IicDat.Port     =  Device;
    InputInstance.IicDat.RdLng    =  length;

    ioctl(InputInstance.IicFile,IIC_READ_DATA,&InputInstance.IicDat);

    if (InputInstance.IicDat.Result == OK)
    {
        if (data)
        {
            data->Resize1D(length);
            memcpy(data->BeginAt(0), &InputInstance.IicDat.RdData[0], InputInstance.IicDat.RdLng);
        }
        if (pResult)
            *pResult = OK;
    }
    else
    {
        if (pResult)
            *pResult = FAIL;
    }

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE5(InputIICWrite, UInt8, UInt8, TypedArrayCoreRef, UInt8, UInt8)
{
    UInt8  layer   = _Param(0);
    UInt8  no      = _Param(1);
    TypedArrayCoreRef command = _Param(2);
    UInt8  RdLng   = _Param(3);
    UInt8 *pResult = _ParamPointer(4);

    DATA8 Device = no + (layer * INPUT_PORTS);

    if (Device >= INPUTS)
    {
        if (pResult)
            *pResult = FAIL;
        return _NextInstruction();
    }

    if (command->Length() > MAX_DEVICE_DATALENGTH)
    {
        command->Resize1D(MAX_DEVICE_DATALENGTH);
    }
    if (RdLng > MAX_DEVICE_DATALENGTH)
    {
        RdLng  =  MAX_DEVICE_DATALENGTH;
    }

    InputInstance.IicDat.Port     =  Device;
    InputInstance.IicDat.Repeat   =  1;       // Only do a one-shot
    InputInstance.IicDat.Time     =  0;       // No specified repeat time
    InputInstance.IicDat.WrLng    =  command->Length();
    InputInstance.IicDat.RdLng    =  RdLng;

    memcpy(&InputInstance.IicDat.WrData[0],command->BeginAt(0), InputInstance.IicDat.WrLng);
    ioctl(InputInstance.IicFile,IIC_WRITE_DATA,&InputInstance.IicDat);
    if (pResult)
        *pResult  =  InputInstance.IicDat.Result;

    return _NextInstruction();
}

VIREO_FUNCTION_SIGNATURE3(InputIICStatus, UInt8, UInt8, UInt8)
{
    UInt8  layer   = _Param(0);
    UInt8  no      = _Param(1);
    UInt8 *pResult = _ParamPointer(2);

    DATA8 Device = no + (layer * INPUT_PORTS);

    if (Device >= INPUTS)
    {
        if (pResult)
            *pResult = FAIL;
        return _NextInstruction();
    }

    InputInstance.IicDat.Port     =  Device;
    ioctl(InputInstance.IicFile,IIC_READ_STATUS,&InputInstance.IicDat);
    if (pResult)
        *pResult  =  InputInstance.IicDat.Result;

    return _NextInstruction();
}

#include "TypeDefiner.h"
DEFINE_VIREO_BEGIN(EV3_IO)
    DEFINE_VIREO_FUNCTION(InputGetTypeMode, "p(i(.UInt8),i(.UInt8),o(.UInt8),o(.UInt8))");
    DEFINE_VIREO_FUNCTION(InputReadPct, "p(i(.UInt8),i(.UInt8),i(.UInt8),i(.UInt8),o(.UInt8))");
    DEFINE_VIREO_FUNCTION(InputReadSi, "p(i(.UInt8),i(.UInt8),i(.UInt8),i(.UInt8),i(.UInt8),o(.Array))");
    DEFINE_VIREO_FUNCTION(InputReadRaw, "p(i(.UInt8),i(.UInt8),i(.UInt8),o(.Array))");
    DEFINE_VIREO_FUNCTION(InputReady, "p(i(.UInt8),i(.UInt8))");
    DEFINE_VIREO_FUNCTION(InputWrite, "p(i(.UInt8),i(.UInt8),i(.UInt8),i(.Array))");
    DEFINE_VIREO_FUNCTION(InputIicSetup, "p(i(.UInt8),i(.UInt8),i(.Array),i(.UInt8),o(.Array))");
    DEFINE_VIREO_FUNCTION(InputClearChanges, "p(i(.UInt8),i(.UInt8))");
    DEFINE_VIREO_FUNCTION(InputSetAutoID, "p(i(.UInt8),i(.UInt8),i(.UInt8))");
    DEFINE_VIREO_FUNCTION(InputSetConn, "p(i(.UInt8),i(.UInt8),i(.UInt8))");
    DEFINE_VIREO_FUNCTION(InputIICRead, "p(i(.UInt8),i(.UInt8),i(.UInt8),o(.Array),o(.UInt8))");
    DEFINE_VIREO_FUNCTION(InputIICWrite, "p(i(.UInt8),i(.UInt8),i(.Array),i(.UInt8),o(.UInt8))");
    DEFINE_VIREO_FUNCTION(InputIICStatus, "p(i(.UInt8),i(.UInt8),o(.UInt8))");
DEFINE_VIREO_END()

