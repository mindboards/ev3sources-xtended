#include <fcntl.h>
#include <unistd.h>

extern "C" {
#include "lms2012.h"
// #include "bytecodes.h"
#include "c_output.h"
#include "c_daisy.h"
}

#include "ExecutionContext.h"
#include "StringUtilities.h"

using namespace Vireo;

#ifndef DISABLE_DAISYCHAIN_COM_CALL
static DATA8 DaisyBuf[64];
#endif

VIVM_FUNCTION_SIGNATURE3(OutputStop, UInt8, UInt8, UInt8)
{
    UInt8 layer = _Param(0);
    UInt8 nos   = _Param(1);
    UInt8 brake = _Param(2);

    DATA8 OutputData[3];
    UBYTE len = 0;
    IP TmpIp = GetObjectIp();
    DSPSTAT DspStat = NOBREAK;

    if (layer == 0)
    {
        OutputData[0] = opOUTPUT_STOP;
        OutputData[1] = nos;
        OutputData[2] = brake;

        if (OutputInstance.PwmFile >= 0)
            write(OutputInstance.PwmFile, OutputData, sizeof(OutputData));
    }
    #ifndef DISABLE_DAISYCHAIN_COM_CALL
    else
    {
        if (cDaisyReady() != BUSY)
        {
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = opOUTPUT_STOP;
            len            += cOutputPackParam((DATA32)    0, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)  nos, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)brake, &(DaisyBuf[len]));
            if (OK != cDaisyDownStreamCmd(DaisyBuf, len, layer))
            {
                SetObjectIp(TmpIp - 1);
                DspStat = BUSYBREAK;
            }
        }
        else
        {
            SetObjectIp(TmpIp - 1);
            DspStat = BUSYBREAK;
        }
    }
    #endif
    SetDispatchStatus(DspStat);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE3(OutputSpeed, UInt8, UInt8, Int8)
{
    UInt8 layer = _Param(0);
    UInt8 nos   = _Param(1);
    Int8  speed = _Param(2);

    UBYTE len = 0;
    IP TmpIp = GetObjectIp();
    DSPSTAT DspStat = NOBREAK;

    if (layer == 0)
    {
        OutputData[0] = opOUTPUT_SPEED;
        OutputData[1] = nos;
        OutputData[2] = speed;

        if (OutputInstance.PwmFile >= 0)
            write(OutputInstance.PwmFile, OutputData, sizeof(OutputData));
    }
    #ifndef DISABLE_DAISYCHAIN_COM_CALL
    else
    {
        if (cDaisyReady() != BUSY)
        {
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = opOUTPUT_SPEED;
            len            += cOutputPackParam((DATA32)    0, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)  nos, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)speed, &(DaisyBuf[len]));
            if (OK != cDaisyDownStreamCmd(DaisyBuf, len, layer))
            {
                SetObjectIp(TmpIp - 1);
                DspStat = BUSYBREAK;
            }
        }
        else
        {
            SetObjectIp(TmpIp - 1);
            DspStat = BUSYBREAK;
        }
    }
    #endif
    SetDispatchStatus(DspStat);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE3(OutputPower, UInt8, UInt8, Int8)
{
    UInt8 layer = _Param(0);
    UInt8 nos   = _Param(1);
    Int8  power = _Param(2);

    UBYTE len = 0;
    IP TmpIp = GetObjectIp();
    DSPSTAT DspStat = NOBREAK;

    if (layer == 0)
    {
        OutputData[0] = opOUTPUT_POWER;
        OutputData[1] = nos;
        OutputData[2] = power;

        if (OutputInstance.PwmFile >= 0)
            write(OutputInstance.PwmFile, OutputData, sizeof(OutputData));
    }
    #ifndef DISABLE_DAISYCHAIN_COM_CALL
    else
    {
        if (cDaisyReady() != BUSY)
        {
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = opOUTPUT_POWER;
            len            += cOutputPackParam((DATA32)    0, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)  nos, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)power, &(DaisyBuf[len]));
            if (OK != cDaisyDownStreamCmd(DaisyBuf, len, layer))
            {
                SetObjectIp(TmpIp - 1);
                DspStat = BUSYBREAK;
            }
        }
        else
        {
            SetObjectIp(TmpIp - 1);
            DspStat = BUSYBREAK;
        }
    }
    #endif
    SetDispatchStatus(DspStat);


    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE2(OutputStart, UInt8, UInt8)
{
    UInt8 layer = _Param(0);
    UInt8 nos   = _Param(1);

    DATA8 OutputData[2];
    UBYTE len = 0;
    IP TmpIp = GetObjectIp();
    DSPSTAT DspStat = NOBREAK;

    if (layer == 0)
    {
        OutputData[0] = opOUTPUT_START;
        OutputData[1] = nos;

        if (OutputInstance.PwmFile >= 0)
        {
            write(OutputInstance.PwmFile, OutputData, sizeof(OutputData));
            for (DATA8 i = 0; i < OUTPUTS; i++)
            {
                if (nos & (0x01 << i))
                    OutputInstance.Owner[i] = CallingObjectId();
            }
        }
    }
    #ifndef DISABLE_DAISYCHAIN_COM_CALL
    else
    {
        if (cDaisyReady() != BUSY)
        {
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = opOUTPUT_START;
            len            += cOutputPackParam((DATA32)    0, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)  nos, &(DaisyBuf[len]));
            if (OK != cDaisyDownStreamCmd(DaisyBuf, len, layer))
            {
                SetObjectIp(TmpIp - 1);
                DspStat = BUSYBREAK;
            }
        }
        else
        {
            SetObjectIp(TmpIp - 1);
            DspStat = BUSYBREAK;
        }
    }
    #endif
    SetDispatchStatus(DspStat);


    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE3(OutputPolarity, UInt8, UInt8, Int8)
{
    UInt8 layer = _Param(0);
    UInt8 nos   = _Param(1);
    Int8  pol   = _Param(2);

    UBYTE len = 0;
    IP TmpIp = GetObjectIp();
    DSPSTAT DspStat = NOBREAK;

    if (layer == 0)
    {
        OutputData[0] = opOUTPUT_POLARITY;
        OutputData[1] = nos;
        OutputData[2] = pol;

        if (OutputInstance.PwmFile >= 0)
            write(OutputInstance.PwmFile, OutputData, sizeof(OutputData));
    }
    #ifndef DISABLE_DAISYCHAIN_COM_CALL
    else
    {
        if (cDaisyReady() != BUSY)
        {
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = opOUTPUT_POLARITY;
            len            += cOutputPackParam((DATA32)  0, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)nos, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)pol, &(DaisyBuf[len]));
            if (OK != cDaisyDownStreamCmd(DaisyBuf, len, layer))
            {
                SetObjectIp(TmpIp - 1);
                DspStat = BUSYBREAK;
            }
        }
        else
        {
            SetObjectIp(TmpIp - 1);
            DspStat = BUSYBREAK;
        }
    }
    #endif
    SetDispatchStatus(DspStat);

    return _NextInstruction();
}


VIVM_FUNCTION_SIGNATURE7(OutputStepPower, UInt8, UInt8, Int8, UInt32, UInt32, UInt32, UInt8)
{
    UInt8  layer = _Param(0);
    UInt8  nos   = _Param(1);
    Int8   power = _Param(2);
    UInt32 step1 = _Param(3);
    UInt32 step2 = _Param(4);
    UInt32 step3 = _Param(5);
    UInt8  brake = _Param(6);

    IP TmpIp = GetObjectIp();
    DSPSTAT DspStat = NOBREAK;


    if (layer == 0)
    {
        STEPPOWER StepPower;
        StepPower.Cmd    =  opOUTPUT_STEP_POWER;
        StepPower.Nos    =  nos;
        StepPower.Power  =  power;
        StepPower.Step1  =  step1;
        StepPower.Step2  =  step2;
        StepPower.Step3  =  step3;
        StepPower.Brake  =  brake;

        if (OutputInstance.PwmFile >= 0)
        {
            write(OutputInstance.PwmFile, (DATA8*)&(StepPower.Cmd), sizeof(StepPower));
            for (DATA8 i = 0; i < OUTPUTS; i++)
            {
                if (nos & (0x01 << i))
                    OutputInstance.Owner[i] = CallingObjectId();
            }
        }
    }
    #ifndef DISABLE_DAISYCHAIN_COM_CALL
    else
    {
        if (cDaisyReady() != BUSY)
        {
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = opOUTPUT_STEP_POWER;
            len            += cOutputPackParam((DATA32)    0, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)  nos, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)power, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)step1, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)step2, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)step3, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)brake, &(DaisyBuf[len]));
            if (OK != cDaisyDownStreamCmd(DaisyBuf, len, layer))
            {
                SetObjectIp(TmpIp - 1);
                DspStat = BUSYBREAK;
            }
        }
        else
        {
            SetObjectIp(TmpIp - 1);
            DspStat = BUSYBREAK;
        }
    }
    #endif
    SetDispatchStatus(DspStat);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE7(OutputStepSpeed, UInt8, UInt8, Int8, UInt32, UInt32, UInt32, UInt8)
{
    UInt8  layer = _Param(0);
    UInt8  nos   = _Param(1);
    Int8   speed = _Param(2);
    UInt32 step1 = _Param(3);
    UInt32 step2 = _Param(4);
    UInt32 step3 = _Param(5);
    UInt8  brake = _Param(6);

    IP TmpIp = GetObjectIp();
    DSPSTAT DspStat = NOBREAK;


    if (layer == 0)
    {
        STEPSPEED StepSpeed;
        StepSpeed.Cmd    =  opOUTPUT_STEP_SPEED;
        StepSpeed.Nos    =  nos;
        StepSpeed.Speed  =  speed;
        StepSpeed.Step1  =  step1;
        StepSpeed.Step2  =  step2;
        StepSpeed.Step3  =  step3;
        StepSpeed.Brake  =  brake;

        if (OutputInstance.PwmFile >= 0)
        {
            write(OutputInstance.PwmFile, (DATA8*)&(StepSpeed.Cmd), sizeof(StepSpeed));
            for (DATA8 i = 0; i < OUTPUTS; i++)
            {
                if (StepSpeed.Nos & (0x01 << i))
                    OutputInstance.Owner[i] = CallingObjectId();
            }
        }
    }
    #ifndef DISABLE_DAISYCHAIN_COM_CALL
    else
    {
        if (cDaisyReady() != BUSY)
        {
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = opOUTPUT_STEP_SPEED;
            len            += cOutputPackParam((DATA32)    0, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)  nos, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)speed, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)step1, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)step2, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)step3, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)brake, &(DaisyBuf[len]));
            if (OK != cDaisyDownStreamCmd(DaisyBuf, len, layer))
            {
                SetObjectIp(TmpIp - 1);
                DspStat = BUSYBREAK;
            }
        }
        else
        {
            SetObjectIp(TmpIp - 1);
            DspStat = BUSYBREAK;
        }
    }
    #endif
    SetDispatchStatus(DspStat);

    return _NextInstruction();
}

VIVM_FUNCTION_SIGNATURE6(OutputStepSync, UInt8, UInt8, Int8, Int16, UInt32, UInt8)
{
    UInt8  layer = _Param(0);
    UInt8  nos   = _Param(1);
    Int8   speed = _Param(2);
    Int16  turn  = _Param(3);
    UInt32 step  = _Param(4);
    UInt8  brake = _Param(5);

    IP TmpIp = GetObjectIp();
    DSPSTAT DspStat = NOBREAK;

    if (layer == 0)
    {
        STEPSYNC StepSync;
        StepSync.Cmd   =  opOUTPUT_STEP_SYNC;
        StepSync.Nos   =  nos;
        StepSync.Speed =  speed;
        StepSync.Turn  =  turn;
        StepSync.Step  =  step;
        StepSync.Brake =  brake;

        if (OutputInstance.PwmFile >= 0)
        {
            write(OutputInstance.PwmFile, (DATA8*)&(StepSync.Cmd), sizeof(StepSync));
            for (DATA8 i = 0; i < OUTPUTS; i++)
            {
                if (StepSync.Nos & (0x01 << i))
                    OutputInstance.Owner[i] = CallingObjectId();
            }
        }
    }
    #ifndef DISABLE_DAISYCHAIN_COM_CALL
    else
    {
        if (cDaisyReady() != BUSY)
        {
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = 0;
            DaisyBuf[len++] = opOUTPUT_STEP_SPEED;
            len            += cOutputPackParam((DATA32)    0, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)  nos, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)speed, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32) turn, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32) step, &(DaisyBuf[len]));
            len            += cOutputPackParam((DATA32)brake, &(DaisyBuf[len]));
            if (OK != cDaisyDownStreamCmd(DaisyBuf, len, layer))
            {
                SetObjectIp(TmpIp - 1);
                DspStat = BUSYBREAK;
            }
        }
        else
        {
            SetObjectIp(TmpIp - 1);
            DspStat = BUSYBREAK;
        }
    }
    #endif
    SetDispatchStatus(DspStat);


    return _NextInstruction();
}

VIREO_DEFINE_BEGIN(EV3_IO)
    // Types
    //VIREO_DEFINE_TYPE(FileHandle, ".DataPointer")
    // Values
    // VIREO_DEFINE_FUNCTION(OutputSetType, "p(e(.UInt8),e(.UInt8),e(.UInt8))");

    VIREO_DEFINE_FUNCTION(OutputStop, "p(e(.UInt8),e(.UInt8),e(.UInt8))");
    VIREO_DEFINE_FUNCTION(OutputPower, "p(e(.UInt8),e(.UInt8),e(.Int8))");
    VIREO_DEFINE_FUNCTION(OutputSpeed, "p(e(.UInt8),e(.UInt8),e(.Int8))");
    VIREO_DEFINE_FUNCTION(OutputStart, "p(e(.UInt8),e(.UInt8))");
    VIREO_DEFINE_FUNCTION(OutputPolarity, "p(e(.UInt8),e(.UInt8),e(.Int8))");

    VIREO_DEFINE_FUNCTION(OutputStepPower, "p(e(.UInt8),e(.UInt8),e(.Int8),e(.UInt32),e(.UInt32),e(.UInt32),e(.UInt8))");
    VIREO_DEFINE_FUNCTION(OutputStepSpeed, "p(e(.UInt8),e(.UInt8),e(.Int8),e(.UInt32),e(.UInt32),e(.UInt32),e(.UInt8))");
    VIREO_DEFINE_FUNCTION(OutputStepSync, "p(e(.UInt8),e(.UInt8),e(.Int8),e(.Int16),e(.UInt32),e(.UInt8))");

VIREO_DEFINE_END()

