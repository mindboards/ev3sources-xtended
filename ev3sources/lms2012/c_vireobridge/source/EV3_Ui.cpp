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

#include <fcntl.h>
#include <unistd.h>

extern "C" {
#include "c_ui.h"
#include "d_lcd.h"
#include "c_memory.h"
}

#include "TypeAndDataManager.h"
#include "StringUtilities.h"

using namespace Vireo;

VIVM_FUNCTION_SIGNATURE1(UiSetLED, Int8)
{
    Int8 state = _Param(0);

    state = state > 0           ? state : 0;
    state = state < LEDPATTERNS ? state : LEDPATTERNS-1;

    cUiSetLed(state);
    UiInstance.RunLedEnabled = 0;
    SetDispatchStatus(NOBREAK);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE0(UiClear)
{
    UiInstance.RunScreenEnabled = 0;
    if ((UiInstance.ScreenBlocked == 0) || ((CurrentProgramId() == UiInstance.ScreenPrgId) && (CallingObjectId() == UiInstance.ScreenObjId)))
    {
        UiInstance.Font = NORMAL_FONT;
        memset(&((*UiInstance.pLcd).Lcd[0]), BG_COLOR, LCD_BUFFER_SIZE);
        UiInstance.ScreenBusy = 1;
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE0(UiUpdate)
{
    UiInstance.RunScreenEnabled = 0;
    if ((UiInstance.ScreenBlocked == 0) || ((CurrentProgramId() == UiInstance.ScreenPrgId) && (CallingObjectId() == UiInstance.ScreenObjId)))
    {
        cUiUpdateLcd();
        UiInstance.ScreenBusy = 0;
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE0(UiRestore)
{
    UiInstance.RunScreenEnabled = 3;
    SetDispatchStatus(NOBREAK);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE4(UiText, UInt8, UInt16, UInt16, StringRef)
{
    UInt8     color  = _Param(0);
    UInt16    x      = _Param(1);
    UInt16    y      = _Param(2);
    TempStackCStringFromString string(_Param(3));

    UiInstance.RunScreenEnabled = 0;
    if ((UiInstance.ScreenBlocked == 0) || ((CurrentProgramId() == UiInstance.ScreenPrgId) && (CallingObjectId() == UiInstance.ScreenObjId)))
    {
        dLcdDrawText((*UiInstance.pLcd).Lcd, color, x, y, UiInstance.Font, (DATA8 *) string.BeginCStr());
        UiInstance.ScreenBusy = 1;
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE4(UiBmpFile, UInt8, UInt16, UInt16, StringRef)
{
    UInt8     color    = _Param(0);
    UInt16    x        = _Param(1);
    UInt16    y        = _Param(2);
    TempStackCStringFromString fileName(_Param(3));

    UiInstance.RunScreenEnabled = 0;
    if ((UiInstance.ScreenBlocked == 0) || ((CurrentProgramId() == UiInstance.ScreenPrgId) && (CallingObjectId() == UiInstance.ScreenObjId)))
    {
        char pathName[MAX_FILENAME_SIZE];
        UBYTE pBmp[LCD_BUFFER_SIZE];

        if (*((Utf8Char *) fileName.BeginCStr()) != '.')
        {
            GetResourcePath(pathName, MAX_FILENAME_SIZE);
        }
        sprintf(pathName, "%s%s", pathName, fileName.BeginCStr());

        if (cMemoryGetImage((DATA8 *) pathName, LCD_BUFFER_SIZE, pBmp) == OK)
        {
            dLcdDrawBitmap((*UiInstance.pLcd).Lcd, color, x, y, pBmp);
            UiInstance.ScreenBusy = 1;
        }
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE3(UiPoint, UInt8, UInt16, UInt16)
{
    UInt8  color = _Param(0);
    UInt16 x     = _Param(1);
    UInt16 y     = _Param(2);

    UiInstance.RunScreenEnabled = 0;
    if ((UiInstance.ScreenBlocked == 0) || ((CurrentProgramId() == UiInstance.ScreenPrgId) && (CallingObjectId() == UiInstance.ScreenObjId)))
    {
        dLcdDrawPixel((*UiInstance.pLcd).Lcd, color, x, y);
        UiInstance.ScreenBusy = 1;
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE5(UiLine, UInt8, UInt16, UInt16, UInt16, UInt16)
{
    UInt8  color  = _Param(0);
    UInt16 x0     = _Param(1);
    UInt16 y0     = _Param(2);
    UInt16 x1     = _Param(3);
    UInt16 y1     = _Param(4);

    UiInstance.RunScreenEnabled = 0;
    if ((UiInstance.ScreenBlocked == 0) || ((CurrentProgramId() == UiInstance.ScreenPrgId) && (CallingObjectId() == UiInstance.ScreenObjId)))
    {
        dLcdDrawLine((*UiInstance.pLcd).Lcd, color, x0, y0, x1, y1);
        UiInstance.ScreenBusy = 1;
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE6(UiRectangle, UInt8, UInt16, UInt16, UInt16, UInt16, UInt8)
{
    UInt8  color  = _Param(0);
    UInt16 x0     = _Param(1);
    UInt16 y0     = _Param(2);
    UInt16 width  = _Param(3);
    UInt16 height = _Param(4);
    UInt8  fill   = _Param(5);

    UiInstance.RunScreenEnabled = 0;
    if ((UiInstance.ScreenBlocked == 0) || ((CurrentProgramId() == UiInstance.ScreenPrgId) && (CallingObjectId() == UiInstance.ScreenObjId)))
    {
        if (fill == 0)
            dLcdRect((*UiInstance.pLcd).Lcd, color, x0, y0, width, height);
        if (fill == 1)
            dLcdFillRect((*UiInstance.pLcd).Lcd, color, x0, y0, width, height);
        UiInstance.ScreenBusy = 1;
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE5(UiCircle, UInt8, UInt16, UInt16, UInt16, UInt8)
{
    UInt8  color = _Param(0);
    UInt16 x     = _Param(1);
    UInt16 y     = _Param(2);
    UInt16 r     = _Param(3);
    UInt8  fill  = _Param(4);

    UiInstance.RunScreenEnabled = 0;
    if ((UiInstance.ScreenBlocked == 0) || ((CurrentProgramId() == UiInstance.ScreenPrgId) && (CallingObjectId() == UiInstance.ScreenObjId)))
    {
        if (fill == 0)
            dLcdDrawCircle((*UiInstance.pLcd).Lcd, color, x, y, r);
        if (fill == 1)
            dLcdDrawFilledCircle((*UiInstance.pLcd).Lcd, color, x, y, r);
        UiInstance.ScreenBusy = 1;
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE2(UiButtonPressed, Int8, Int8)
{
    Int8 button = _Param(0);
    Int8 *state = _ParamPointer(1); // reference

    if (state)
    {
        if ((UiInstance.ScreenBlocked == 0) || ((CurrentProgramId() == UiInstance.ScreenPrgId) && (CallingObjectId() == UiInstance.ScreenObjId)))
            *state = cUiGetPress(button);
        else
            *state = 0;
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE1(UiGetVBattery, Single)
{
    Single *voltage = _ParamPointer(0); // reference

    if (voltage)
        *voltage = UiInstance.Vbatt;

    return _NextInstruction();
}

#include "TypeDefiner.h"
VIREO_DEFINE_BEGIN(EV3_IO)
    VIREO_DEFINE_FUNCTION(UiSetLED,"p(i(.Int8))");
    VIREO_DEFINE_FUNCTION(UiClear,"p()");
    VIREO_DEFINE_FUNCTION(UiUpdate,"p()");
    VIREO_DEFINE_FUNCTION(UiRestore,"p()");
    VIREO_DEFINE_FUNCTION(UiText,"p(i(.UInt8),i(.UInt16),i(.UInt16),i(.String))");
    VIREO_DEFINE_FUNCTION(UiBmpFile,"p(i(.UInt8),i(.UInt16),i(.UInt16),i(.String))");
    VIREO_DEFINE_FUNCTION(UiPoint,"p(i(.UInt8),i(.UInt16),i(.UInt16))");
    VIREO_DEFINE_FUNCTION(UiLine,"p(i(.UInt8),i(.UInt16),i(.UInt16),i(.UInt16),i(.UInt16))");
    VIREO_DEFINE_FUNCTION(UiRectangle,"p(i(.UInt8),i(.UInt16),i(.UInt16),i(.UInt16),i(.UInt16),i(.UInt8))");
    VIREO_DEFINE_FUNCTION(UiCircle,"p(i(.UInt8),i(.UInt16),i(.UInt16),i(.UInt16),i(.UInt8))");
    VIREO_DEFINE_FUNCTION(UiButtonPressed,"p(i(.Int8) o(.Int8))");
    VIREO_DEFINE_FUNCTION(UiGetVBattery,"p(o(.Single))");
VIREO_DEFINE_END()

