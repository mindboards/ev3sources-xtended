#include <fcntl.h>
#include <unistd.h>

extern "C" {
#include "lms2012.h"
// #include "bytecodes.h"
#include "c_ui.h"
#include "d_lcd.h"
#include "c_memory.h"
}

#include "ExecutionContext.h"
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

VIVM_FUNCTION_SIGNATURE4(UiText, UInt8, UInt16, UInt16, Utf8String*)
{
    UInt8       color  = _Param(0);
    UInt16      x      = _Param(1);
    UInt16      y      = _Param(2);
    Utf8String *string = _Param(3);

    UiInstance.RunScreenEnabled = 0;
    if ((UiInstance.ScreenBlocked == 0) || ((CurrentProgramId() == UiInstance.ScreenPrgId) && (CallingObjectId() == UiInstance.ScreenObjId)))
    {
        dLcdDrawText((*UiInstance.pLcd).Lcd, color, x, y, UiInstance.Font, (DATA8 *) string->Begin());
        UiInstance.ScreenBusy = 1;
    }

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE4(UiBmpFile, UInt8, UInt16, UInt16, Utf8String*)
{
    UInt8       color    = _Param(0);
    UInt16      x        = _Param(1);
    UInt16      y        = _Param(2);
    Utf8String *fileName = _Param(3);

    UiInstance.RunScreenEnabled = 0;
    if ((UiInstance.ScreenBlocked == 0) || ((CurrentProgramId() == UiInstance.ScreenPrgId) && (CallingObjectId() == UiInstance.ScreenObjId)))
    {
        char pathName[MAX_FILENAME_SIZE];
        UBYTE pBmp[LCD_BUFFER_SIZE];

        if (*((Utf8Char *) fileName->Begin()) != '.')
        {
            GetResourcePath(pathName, MAX_FILENAME_SIZE);
        }
        sprintf(pathName, "%s%s", pathName, fileName->Begin());

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

VIREO_DEFINE_BEGIN(EV3_IO)
    // Types
    //VIREO_DEFINE_TYPE(FileHandle, ".DataPointer")
    // Values
    VIREO_DEFINE_FUNCTION(UiSetLED,"p(e(.Int8))");
    VIREO_DEFINE_FUNCTION(UiClear,"p()");
    VIREO_DEFINE_FUNCTION(UiUpdate,"p()");
    VIREO_DEFINE_FUNCTION(UiRestore,"p()");
    VIREO_DEFINE_FUNCTION(UiText,"p(e(.UInt8),e(.UInt16),e(.UInt16),e(.Utf8String))");
    VIREO_DEFINE_FUNCTION(UiBmpFile,"p(e(.UInt8),e(.UInt16),e(.UInt16),e(.Utf8String))");
    VIREO_DEFINE_FUNCTION(UiPoint,"p(e(.UInt8),e(.UInt16),e(.UInt16))");
    VIREO_DEFINE_FUNCTION(UiRectangle,"p(e(.UInt8),e(.UInt16),e(.UInt16),e(.UInt16),e(.UInt16),e(.UInt8))");
    VIREO_DEFINE_FUNCTION(UiCircle,"p(e(.UInt8),e(.UInt16),e(.UInt16),e(.UInt16),e(.UInt8))");
VIREO_DEFINE_END()

