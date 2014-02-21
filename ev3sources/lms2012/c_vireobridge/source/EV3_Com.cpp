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
#include "c_com.h"
#include "c_bt.h"

// #include "c_wifi.h"
// Including c_wifi.h caused problems, so the relevant declarations are reproduced here.
RESULT cWiFiTurnOn(void);
RESULT cWiFiTurnOff(void);
RESULT cWiFiConnectToAp(int Index);
RESULT cWiFiDisconnect(void);
RESULT cWiFiGetIndexFromName(char *Name, UBYTE *Index);
RESULT cWiFiGetName(char *ApName, int Index, char Length);
unsigned char cWiFiGetFlags(int Index);
enum
{
  VISIBLE = 0x1,
  CONNECTED = 0x02,
  WPA2 = 0x04,
  KNOWN = 0x08,
  UNKNOWN = 0x80
};
}

#include "TypeAndDataManager.h"

using namespace Vireo;

VIVM_FUNCTION_SIGNATURE3(MailBoxOpen, Int8, StringRef, Int8)
{
    Int8 boxNo        = _Param(0);
    TempStackCStringFromString boxName(_Param(1));
    Int8 dataType     = _Param(2);

    DSPSTAT DspStat = FAILBREAK;

    if (OK != ComInstance.MailBox[boxNo].Status)
    {
        snprintf((char*)(&(ComInstance.MailBox[boxNo].Name[0])), 50,"%s",boxName.BeginCStr());
        memset(ComInstance.MailBox[boxNo].Content, 0, MAILBOX_CONTENT_SIZE);
        ComInstance.MailBox[boxNo].Type      =  dataType;
        ComInstance.MailBox[boxNo].Status    =  OK;
        ComInstance.MailBox[boxNo].ReadCnt   =  0;
        ComInstance.MailBox[boxNo].WriteCnt  =  0;
        DspStat                              =  NOBREAK;
    }
    SetDispatchStatus(DspStat);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE1(MailBoxClose, Int8)
{
    Int8 boxNo          = _Param(0);

    DSPSTAT DspStat = NOBREAK;

    ComInstance.MailBox[boxNo].Status = FAIL;
    SetDispatchStatus(DspStat);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE3(MailBoxRead, Int8, Int8, TypedArrayCoreRef)
{
    Int8 boxNo   = _Param(0);
    Int8 length  = _Param(1);
    TypedArrayCoreRef data;
    if (_ParamPointer(2))
        data = _Param(2);
    else
        return _NextInstruction();

    if (OK == ComInstance.MailBox[boxNo].Status)
    {
        switch(ComInstance.MailBox[boxNo].Type)
        {
            case DATA_8:
            {
                length = Min(length, MAILBOX_CONTENT_SIZE / sizeof(DATA8));
                data->Resize(length * sizeof(DATA8));
                for (UBYTE i = 0; i < length; i++)
                    *(DATA8*)data->BeginAt(i*sizeof(DATA8)) = ((DATA8*)(ComInstance.MailBox[boxNo].Content))[i];
                break;
            }
            case DATA_16:
            {
                length = Min(length, MAILBOX_CONTENT_SIZE / sizeof(DATA16));
                data->Resize(length * sizeof(DATA16));
                for (UBYTE i = 0; i < length; i++)
                    *(DATA16*)data->BeginAt(i*sizeof(DATA16)) = ((DATA16*)(ComInstance.MailBox[boxNo].Content))[i];
                break;
            }
            case DATA_32:
            {
                length = Min(length, MAILBOX_CONTENT_SIZE / sizeof(DATA32));
                data->Resize(length * sizeof(DATA32));
                for (UBYTE i = 0; i < length; i++)
                    *(DATA32*)data->BeginAt(i*sizeof(DATA32)) = ((DATA32*)(ComInstance.MailBox[boxNo].Content))[i];
                break;
            }
            case DATA_F:
            {
                length = Min(length, MAILBOX_CONTENT_SIZE / sizeof(DATAF));
                data->Resize(length * sizeof(DATAF));
                for (UBYTE i = 0; i < length; i++)
                    *(DATAF*)data->BeginAt(i*sizeof(DATAF)) = ((DATAF*)(ComInstance.MailBox[boxNo].Content))[i];
                break;
            }
            case DATA_S:
            {
                data->Resize(MAILBOX_CONTENT_SIZE);
                IntIndex stringLength = snprintf((char*)data->BeginAt(0), MAILBOX_CONTENT_SIZE, "%s", (char*)ComInstance.MailBox[boxNo].Content);
                data->Resize(stringLength);
                break;
            }
            case DATA_A:
            {
                data->Resize(ComInstance.MailBox[boxNo].DataSize);
                memcpy(data->BeginAt(0), (char*)&((DATA8*)(ComInstance.MailBox[boxNo].Content))[0], data->Length());
                break;
            }
        }

        if (ComInstance.MailBox[boxNo].WriteCnt != ComInstance.MailBox[boxNo].ReadCnt)
            ComInstance.MailBox[boxNo].ReadCnt++;
    }

    SetDispatchStatus(NOBREAK);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE3(MailBoxWrite, StringRef, StringRef, TypedArrayCoreRef)
{
    TempStackCStringFromString brickName(_Param(0));
    TempStackCStringFromString boxName(_Param(1));
    TypedArrayCoreRef data = _Param(2); // uInt8

    UBYTE ChNos;
    UBYTE ComChNo;
    UBYTE ChNoArr[NO_OF_BT_CHS];
    UWORD PayloadSize = data->Length();

    WRITE_MAILBOX         *pComMbx;
    WRITE_MAILBOX_PAYLOAD *pComMbxPayload;

    ChNos = cBtGetChNo((UBYTE*)brickName.BeginCStr(), ChNoArr);

    for(UBYTE i = 0; i < ChNos; i++)
    {
        ComChNo = ChNoArr[i] + BTSLAVE;                               // Ch nos offset from BT module

        // Valid channel found
        if ((0 == ComInstance.TxBuf[ComChNo].Writing) && (TXIDLE == ComInstance.TxBuf[ComChNo].State))
        {
            // Buffer is empty
            pComMbx             =  (WRITE_MAILBOX*)ComInstance.TxBuf[ComChNo].Buf;

            // First part of message
            (*pComMbx).CmdSize  =  SIZEOF_WRITEMAILBOX - sizeof(CMDSIZE);
            (*pComMbx).MsgCount =  1;
            (*pComMbx).CmdType  =  SYSTEM_COMMAND_NO_REPLY;
            (*pComMbx).Cmd      =  WRITEMAILBOX;
            (*pComMbx).NameSize =  strlen((char*)boxName.BeginCStr()) + 1;
            snprintf((char*)(*pComMbx).Name,(*pComMbx).NameSize,"%s",(char*)boxName.BeginCStr());

            (*pComMbx).CmdSize += (*pComMbx).NameSize;

            // Payload part of message
            pComMbxPayload            = (WRITE_MAILBOX_PAYLOAD*) &(ComInstance.TxBuf[ComChNo].Buf[(*pComMbx).CmdSize + sizeof(CMDSIZE)]);
            (*pComMbxPayload).SizeLsb = (UBYTE) (PayloadSize & 0x00FF);
            (*pComMbxPayload).SizeMsb = (UBYTE)((PayloadSize >> 8) & 0x00FF);
            memcpy((*pComMbxPayload).Payload, data->BeginAt(0), PayloadSize);
            (*pComMbx).CmdSize += (PayloadSize + SIZEOF_WRITETOMAILBOXPAYLOAD);

            ComInstance.TxBuf[ComChNo].BlockLen  =  (*pComMbx).CmdSize + sizeof(CMDSIZE);
            ComInstance.TxBuf[ComChNo].Writing   =  1;
        }
    }
    SetDispatchStatus(NOBREAK);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE2(ComSetOnOff, Int8, Int8)
{
    Int8 hardware = _Param(0);
    Int8 onoff    = _Param(1);

    DSPSTAT DspStat = FAILBREAK;

    if (hardware == HW_BT)
    {
        if (BtSetOnOff(onoff) != FAIL)
            DspStat = NOBREAK;
    }
    else if (hardware == HW_WIFI)
    {
        if (onoff && (cWiFiTurnOn() != FAIL))
            DspStat = NOBREAK;
        else if (!onoff && (cWiFiTurnOff() != FAIL))
            DspStat = NOBREAK;
    }

    SetDispatchStatus(DspStat);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE3(ComSetConnection, Int8, StringRef, Int8)
{
    Int8 hardware = _Param(0);
    TempStackCStringFromString name(_Param(1));
    Int8 connect  = _Param(2);

    DSPSTAT DspStat = FAILBREAK;
    DATA8 item;

    if (hardware == HW_BT)
    {
        if (connect && (cBtConnect((UBYTE *) name.BeginCStr()) != FAIL))
            DspStat = NOBREAK;
        else if (!connect && (cBtDisconnect((UBYTE *) name.BeginCStr()) != FAIL))
            DspStat = NOBREAK;
    }
    else if (hardware == HW_WIFI)
    {
        if (connect && (cWiFiGetIndexFromName(name.BeginCStr(), (UBYTE*)&item) == OK))
        {
            cWiFiConnectToAp((int)item);
            DspStat = NOBREAK;
        }
        else if (!connect && (cWiFiDisconnect() == OK))
            DspStat = NOBREAK;
    }

    SetDispatchStatus(DspStat);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE7(ComGetFavourItem, Int8, Int8, Int8, StringRef, Int8, Int8, Int8)
{
    Int8 hardware   = _Param(0);
    Int8 item       = _Param(1);
    Int8 length     = _Param(2);
    StringRef name;
    Int8 paired = 0;
    Int8 connected = 0;
    Int8 type = 0;

    Utf8Char *nameBegin;
    DSPSTAT DspStat = FAILBREAK;
    UBYTE Flags;

    if (_ParamPointer(3))
    {
        name = _Param(3);
        name->Resize(length == -1 ? vmBRICKNAMESIZE : length);
        length = name->Length();
        nameBegin = name->Begin();
    }
    else
    {
        length = 0;
        nameBegin = 0;
    }

    if (hardware == HW_BT)
    {
        paired = 1;
        type = ICON_UNKNOWN;

        cBtGetDevListEntry(item, &connected, &type, (UBYTE*)nameBegin, length);
        DspStat = NOBREAK;
    }
    else if (hardware == HW_WIFI)
    {
        cWiFiGetName((char*)nameBegin, (int)item, length);
        Flags = cWiFiGetFlags(item);

        if (Flags & CONNECTED)
            connected = 1;
        if (Flags & KNOWN)
            paired = 1;
        if (Flags & WPA2)
            type = 1;

        DspStat = NOBREAK;
    }

    if (_ParamPointer(4))
        _Param(4) = paired;
    if (_ParamPointer(5))
        _Param(5) = connected;
    if (_ParamPointer(6))
        _Param(6) = type;

    SetDispatchStatus(DspStat);

    return _NextInstruction();
}

#include "TypeDefiner.h"
VIREO_DEFINE_BEGIN(EV3_IO)
    VIREO_DEFINE_FUNCTION(MailBoxOpen, "p(i(.Int8),i(.String),i(.Int8))");
    VIREO_DEFINE_FUNCTION(MailBoxClose, "p(i(.Int8))");
    VIREO_DEFINE_FUNCTION(MailBoxRead, "p(i(.Int8),i(.Int8),o(.Array))");
    VIREO_DEFINE_FUNCTION(MailBoxWrite, "p(i(.String),i(.String),o(.Array))");
    VIREO_DEFINE_FUNCTION(ComSetOnOff, "p(i(.Int8),i(.Int8))");
    VIREO_DEFINE_FUNCTION(ComSetConnection, "p(i(.Int8),i(.String),i(.Int8))");
    VIREO_DEFINE_FUNCTION(ComGetFavourItem, "p(i(.Int8),i(.Int8),i(.Int8),o(.String),o(.Int8),o(.Int8),o(.Int8))");
VIREO_DEFINE_END()

