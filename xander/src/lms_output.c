/*
 * LEGO® MINDSTORMS EV3
 *
 * Copyright (C) 2010-2013 The LEGO Group
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


/*! \page OutputLibrary Output Library
 *
\verbatim
Byte codes:

opOUTPUT_GET_TYPE     LAYER   NO       *TYPE                                   // Get output device type
opOUTPUT_SET_TYPE     LAYER   NO       TYPE                                    // Set output device type
opOUTPUT_RESET        LAYER   NOS                                              // Reset position  (POS=0)
opOUTPUT_STOP         LAYER   NOS      BRAKE                                   // Stop outputs    (brake or coast)
opOUTPUT_POWER        LAYER   NOS      POWER                                   // Set power       (suspend regulation and positioning)
opOUTPUT_SPEED        LAYER   NOS      SPEED                                   // Set speed (relative to polarity - enables regulation if tacho)
opOUTPUT_START        LAYER   NOS                                              // Starts outputs from present values
opOUTPUT_POLARITY     LAYER   NOS      POL                                     // Set polarity    (0=toggle)
opOUTPUT_READ         LAYER   NOS      *SPEED *STEPS                           // Read actual speed and steps from last reset
opOUTPUT_TEST         LAYER   NOS      *Busy                                   //
opOUTPUT_READY        LAYER   NOS                                              // Wait for new action completed or overruled
opOUTPUT_POSITION     LAYER   NOS      POS                                     // Set position    (absolute from last reset)

opOUTPUT_STEP_POWER   LAYER   NOS      POWER   STEP1   STEP2   STEP3   BRAKE   // Set all parameters, start if not started and power != 0
opOUTPUT_TIME_POWER   LAYER   NOS      POWER   TIME1   TIME2   TIME3   BRAKE   // Set all parameters, start if not started and power != 0
opOUTPUT_STEP_SPEED   LAYER   NOS      SPEED   STEP1   STEP2   STEP3   BRAKE   // Set all parameters, start if not started and power != 0
opOUTPUT_TIME_SPEED   LAYER   NOS      SPEED   TIME1   TIME2   TIME3   BRAKE   // Set all parameters, start if not started and power != 0
opOUTPUT_STEP_SYNC    LAYER   NOS      SPEED   TURN    STEP    BRAKE           // Set all parameters, start if not started and power != 0
opOUTPUT_TIME_SYNC    LAYER   NOS      SPEED   TURN    TIME    BRAKE           // Set all parameters, start if not started and power != 0
opOUTPUT_CLR_COUNT    LAYER   NOS                                              // Clears the tacho count used when in sensor mode
opOUTPUT_GET_COUNT    LAYER   NO       *STEPS                                  // Gets the tacho count used in sensor mode


Parameters:
                      LAYER   int8_t    [0..3]              // USB chain layer                      (0=Master, 1=Slave1)
                      NO      int8_t    [0..3]              // Output port number
                      TYPE    int8_t    [0..255]            // Output device type                   (0=none)
                      NOS     int8_t    [0x00..0x0F]        // Bit field representing output 1 to 4 (0x01, 0x02, 0x04, 0x08)
                      BRAKE   int8_t    [0..1]              // Output state after stop              (0=Coast,  1=Brake)
                      POWER   int8_t    [+-0..100%]         // Power relative to polarity
                      SPEED   int8_t    [+-0..100%]         // Speed relative to polarity           (0->BRAKE=1)
                      STEPS   int32_t   [+-0..MAX]          // Steps in degrees                     (0=infinite)
                      POL     int8_t    [+-0..1]            // Polarity +-1, 0=toggle               (multiplied to SPEED and POWER)
                      POS     int32_t   [+-0..MAX]          // Steps in degrees                     (0=infinite)
                      STEP1   int32_t   [0..MAX]            // Steps used to ramp up
                      STEP2   int32_t   [0..MAX]            // Steps used for constant speed
                      STEP3   int32_t   [0..MAX]            // Steps used to ramp down
                      TIME1   int32_t   [0..MAX]            // Time [mS] to ramp up
                      TIME2   int32_t   [0..MAX]            // Time [mS] for constant speed
                      TIME3   int32_t   [0..MAX]            // Time [mS] to ramp down
                      TURN    int16_t   [-200..200]         // Turn ratio between two syncronized motors
*/

/*
 * SYNCRONIZATION:
 *
 *  Speed -100 to +100 is move forward or move backwards
 *  Turn ratio is how tight you turn and to what direction you turn
 *   - 0 value is moving straight forward
 *   - Negative values turns to the left
 *   - Positive values turns to the right
 *   - Value -100 stops the left motor
 *   - Value +100 stops the right motor
 *   - Values less than -100 makes the left motor run the opposite
 *     direction of the right motor (Spin)
 *   - Values greater than +100 makes the right motor run the opposite
 *     direction of the left motor (Spin)
 *
 *  Example: opOUTPUT_TIME_SYNC(0, 10, 100, 50, 10000,1)
 *
 *  0     = Layer
 *  10    = Motor bit field - Motor B and D
 *  100   = Motor speed     - Motor B will run at speed 100 (because ratio is positive)
 *  10    = Turn ratio      - Motor D will run at speed 50
 *  10000 = time in mS      - Motors will run for 10 sec.
 *  1     = Brake bit       - When 10 sec. has elapsed then brake both motors
 *
 *
 *  Example: opOUTPUT_TIME_SYNC(0, 10, 100, 150, 10000,1)
 *
 *  0     = Layer
 *  10    = Motor bit field - Motor B and D
 *  100   = Motor speed     - Motor B will run at speed 100 (because ratio is positive)
 *  10    = Turn ratio      - Motor D will run at speed -50
 *  10000 = time in mS      - Motors will run for 10 sec.
 *  1     = Brake bit       - When 10 sec. has elapsed then brake both motors
 *
 *
 *  Example: opOUTPUT_TIME_SYNC(0, 10, 100, -50, 10000,1)
 *
 *  0     = Layer
 *  10    = Motor bit field - Motor B and D
 *  100   = Motor speed     - Motor B will run at speed  50 (because ratio is positive)
 *  10    = Turn ratio      - Motor D will run at speed 100
 *  10000 = time in mS      - Motors will run for 10 sec.
 *  1     = Brake bit       - When 10 sec. has elapsed then brake both motors
 *
 *
 *  Example: opOUTPUT_TIME_SYNC(0, 10, 200, -150, 10000,1)
 *
 *  0     = Layer
 *  10    = Motor bit field - Motor B and D
 *  100   = Motor speed     - Motor B will run at speed -50 (because ratio is positive)
 *  10    = Turn ratio      - Motor D will run at speed 100
 *  10000 = time in mS      - Motors will run for 10 sec.
 *  1     = Brake bit       - When 10 sec. has elapsed then brake both motors
 *
 *\

\endverbatim
 *
 */


#include  "lms2012.h"
#include  "lms_output.h"
#ifndef    DISABLE_DAISYCHAIN_COM_CALL
  #include  "c_daisy.h"
#endif

#include  <stdio.h>
#include  <fcntl.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <string.h>
#include  <signal.h>
#include  <sys/mman.h>

OUTPUT_GLOBALS OutputInstance;
#ifndef    DISABLE_DAISYCHAIN_COM_CALL
  static  int8_t  DaisyBuf[64];
#endif

uint DELAY_COUNTER = 0;
uint8_t	BusyOnes = 0;

void      outputResetAll(void)
{
  uint8_t   Tmp;
  int8_t   StopArr[3];

  for(Tmp = 0; Tmp < OUTPUTS; Tmp++)
  {
    OutputInstance.Owner[Tmp] = 0;
  }

  StopArr[0] = (int8_t)opOUTPUT_STOP;
  StopArr[1] = 0x0F;
  StopArr[2] = 0x00;
  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,StopArr,3);
  }
}


RESULT    outputInit(void)
{
  RESULT      Result = FAIL;
  MOTORDATA   *pTmp;

  // To ensure that pMotor is never uninitialised
  OutputInstance.pMotor  =  OutputInstance.MotorData;

  // Open the handle for writing commands
  OutputInstance.PwmFile  =  open(PWM_DEVICE_NAME,O_RDWR);

  if (OutputInstance.PwmFile >= 0)
  {

    // Open the handle for reading motor values - shared memory
    OutputInstance.MotorFile  =  open(MOTOR_DEVICE_NAME,O_RDWR | O_SYNC);
    if (OutputInstance.MotorFile >= 0)
    {
      pTmp  =  (MOTORDATA*)mmap(0, sizeof(OutputInstance.MotorData), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, OutputInstance.MotorFile, 0);

      if (pTmp == MAP_FAILED)
      {
//        LogErrorNumber(OUTPUT_SHARED_MEMORY);
        close(OutputInstance.MotorFile);
        close(OutputInstance.PwmFile);
      }
      else
      {
        OutputInstance.pMotor  =  (MOTORDATA*)pTmp;
        Result  =  outputOpen();
      }
    }
  }

  return (Result);
}


RESULT    outputOpen(void)
{
  RESULT  Result = FAIL;

  uint8_t   PrgStart =  opPROGRAM_START;

  outputResetAll();

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,&PrgStart,1);
  }

  Result  =  OK;

  return (Result);
}


RESULT    outputExit(void)
{
  RESULT  Result = FAIL;

  outputResetAll();

  if (OutputInstance.MotorFile >= 0)
  {
    munmap(OutputInstance.pMotor,sizeof(OutputInstance.MotorData));
    close(OutputInstance.MotorFile);
  }

  if (OutputInstance.PwmFile >= 0)
  {
    close(OutputInstance.PwmFile);
  }

  Result  =  OK;

  return (Result);
}


void    outputSetTypes(char *pTypes)
{
  int8_t   TypeArr[5];

  TypeArr[0] = opOUTPUT_SET_TYPE;
  memcpy(&TypeArr[1], pTypes, 4);

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,TypeArr,sizeof(TypeArr));
  }

}


void ResetDelayCounter(uint8_t Pattern)
{
	BusyOnes = Pattern;
	DELAY_COUNTER = 0;
}

uint8_t     cMotorGetBusyFlags(void)
{
  int     test, test2;
  char    BusyReturn[10]; // Busy mask

  if (OutputInstance.PwmFile >= 0)
  {
    read(OutputInstance.PwmFile,BusyReturn,4);
    sscanf(BusyReturn,"%u %u",&test,&test2);
  }
  else
  {
    test = 0;
  }
  if(DELAY_COUNTER < 25)
  {
	  test = BusyOnes;
	  DELAY_COUNTER++;
  }

  return(test);
}


void      cMotorSetBusyFlags(uint8_t Flags)
{
  if (OutputInstance.MotorFile >= 0)
  {
    write(OutputInstance.MotorFile, &Flags, sizeof(Flags));
  }
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_SET_TYPE (LAYER, NO, TYPE)  </b>
 *
 *- Set output type\n
 *- Dispatch status unchanged
 *
 *  \param  (int8_t)   LAYER   - Chain layer number [0..3]
 *  \param  (int8_t)   NO      - Output no [0..3]
 *  \param  (int8_t)   TYPE    - Output device type
 */
/*! \brief  opOUTPUT_SET_TYPE byte code
 *
 */
void      outputSetType(int8_t motor, int8_t Type)
{
  if ((motor >= 0) && (motor < OUTPUTS))
  {
    if (OutputInstance.OutputType[motor] != Type)
    {
      OutputInstance.OutputType[motor]  =  Type;

      if ((Type == TYPE_NONE) || (Type == TYPE_ERROR))
      {
#ifdef DEBUG
        printf("                Output %c Disable\r\n",'A' + motor);
#endif
      }
      else
      {
#ifdef DEBUG
        printf("                Output %c Enable\r\n",'A' + motor);
#endif
      }
    }
  }
}


/*! \page cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_RESET (LAYER, NOS)  </b>
 *
 *- Resets the Tacho counts \n
 *- Dispatch status unchanged
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 */
/*! \brief  opOUTPUT_RESET byte code
 *
 */
void      outputReset(int8_t motorMask)
{
  int8_t   ResetArr[2];

  ResetArr[0] = opOUTPUT_RESET;
  ResetArr[1] = motorMask;

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,ResetArr,sizeof(ResetArr));
  }
}


/*! \page cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_STOP (LAYER, NOS)  </b>
 *
 *- Stops the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \param  (int8_t)   BRAKE   - Brake [0,1]
 */
/*! \brief  opOUTPUT_STOP byte code
 *
 */
void      outputStop(int8_t motorMask, int8_t Brake)
{
  int8_t   StopArr[3];

  StopArr[0] = (int8_t)opOUTPUT_STOP;
  StopArr[1] = motorMask;
  StopArr[2] = Brake;

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,StopArr,sizeof(StopArr));
  }
}


/*! \page cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_SPEED (LAYER, NOS, SPEED)  </b>
 *
 *- Set speed of the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \param  (int8_t)   SPEED   - Speed [-100..100%]
 *
 *
 */
/*! \brief  opOUTPUT_SPEED byte code
 *
 */
void      outputSpeed(int8_t motorMask, int8_t Speed)
{
  int8_t   SetSpeed[3];

  SetSpeed[0] = (int8_t)opOUTPUT_SPEED;
  SetSpeed[1] = motorMask;
  SetSpeed[2] = Speed;

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,SetSpeed,sizeof(SetSpeed));
  }
}


/*! \page cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_POWER (LAYER, NOS, SPEED)  </b>
 *
 *- Set power of the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \param  (int8_t)   POWER   - Power [-100..100%]
 */
/*! \brief  opOUTPUT_POWER byte code
 *
 */
void      outputPower(int8_t motorMask, int8_t Power)
{
  int8_t   SetPower[3];

  SetPower[0] = (int8_t)opOUTPUT_POWER;
  SetPower[1] = motorMask;
  SetPower[2] = Power;
  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,SetPower,sizeof(SetPower));
  }
}


/*! \page cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_START (LAYER, NOS)  </b>
 *
 *- Starts the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 */
/*! \brief  opOUTPUT_START byte code
 *
 */
void      outputStart(int8_t motorMask)
{
  int8_t   Tmp;

  int8_t   StartMotor[2];

  StartMotor[0] = (int8_t)opOUTPUT_START;
  StartMotor[1] = motorMask;
  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,StartMotor,sizeof(StartMotor));

    for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
    {
      if (motorMask & (0x01 << Tmp))
      {
        OutputInstance.Owner[Tmp] = 0;
      }
    }
  }

}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_POLARITY (LAYER, NOS, POL)  </b>
 *
 *- Set polarity of the outputs\n
 *- Dispatch status unchanged
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \param  (int8_t)   POL     - Polarity [-1,0,1]
 *
 *-  Polarity:
 *   - -1 makes the motor run backward
 *   -  1 makes the motor run forward
 *   -  0 makes the motor run the opposite direction
 */
/*! \brief  opOUTPUT_POLARITY byte code
 *
 */
void      outputPolarity(int8_t motorMask, int8_t Pol)
{
  int8_t   Polarity[3];

  Polarity[0] =  (int8_t)opOUTPUT_POLARITY;
  Polarity[1] =  motorMask;
  Polarity[2] =  Pol;

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,Polarity,sizeof(Polarity));
  }
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_STEP_POWER (LAYER, NOS, POWER, STEP1, STEP2, STEP3, BRAKE)  </b>
 *
 *- Set Ramp up, constant and rampdown steps and power of the outputs\n
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \param  (int8_t)   POWER   - Power [-100..100]
 *  \param  (int32_t)  STEP1   - Tacho pulses [0..MAX]
 *  \param  (int32_t)  STEP2   - Tacho pulses [0..MAX]
 *  \param  (int32_t)  STEP3   - Tacho pulses [0..MAX]
 *  \param  (int8_t)   BRAKE   - 0 = Coast, 1 = BRAKE
 */
/*! \brief  opOUTPUT_STEP_POWER byte code
 *
 */
void      outputStepPower(int8_t motorMask, int8_t Power, int32_t Step1, int32_t Step2, int32_t Step3, int8_t Brake)
{
  int8_t       Tmp;
  STEPPOWER   StepPower;

  StepPower.Cmd    =   opOUTPUT_STEP_POWER;
  StepPower.Nos    =  motorMask;
  StepPower.Power  =  Power;
  StepPower.Step1  =  Step1;
  StepPower.Step2  =  Step2;
  StepPower.Step3  =  Step3;
  StepPower.Brake  =  Brake;

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,(int8_t*)&(StepPower.Cmd),sizeof(StepPower));

    for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
    {
      // Set calling id for all involved inputs
      if (StepPower.Nos & (0x01 << Tmp))
      {
        OutputInstance.Owner[Tmp] = 0;
      }
    }
  }
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_TIME_POWER (LAYER, NOS, POWER, TIME1, TIME2, TIME3, BRAKE)  </b>
 *
 *- Set Ramp up, constant and rampdown steps and power of the outputs\n
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \param  (int8_t)   POWER   - Power [-100..100]
 *  \param  (int32_t)  TIME1   - Time in Ms [0..MAX]
 *  \param  (int32_t)  TIME2   - Time in Ms [0..MAX]
 *  \param  (int32_t)  TIME3   - Time in Ms [0..MAX]
 *  \param  (int8_t)   BRAKE   - 0 = Coast, 1 = BRAKE
 */
/*! \brief  opOUTPUT_TIME_POWER byte code
 *
 */
void      outputTimePower(int8_t motorMask, int8_t Power, int32_t Time1, int32_t Time2, int32_t Time3, int8_t Brake)
{
  int8_t       Tmp;
  TIMEPOWER   TimePower;

  TimePower.Cmd    =  opOUTPUT_TIME_POWER;
  TimePower.Nos    =  motorMask;
  TimePower.Power  =  Power;
  TimePower.Time1  =  Time1;
  TimePower.Time2  =  Time2;
  TimePower.Time3  =  Time3;
  TimePower.Brake  =  Brake;


  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,(int8_t*)&(TimePower.Cmd),sizeof(TimePower));

    for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
    {
      // Set calling id for all involved inputs
      if (TimePower.Nos & (0x01 << Tmp))
      {
        OutputInstance.Owner[Tmp] = 0;
      }
    }
  }
}

/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_STEP_SPEED (LAYER, NOS, SPEED, STEP1, STEP2, STEP3, BRAKE)  </b>
 *
 *- Set Ramp up, constant and rampdown steps and power of the outputs\n
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \param  (int8_t)   SPEED   - Power [-100..100]
 *  \param  (int32_t)  STEP1   - Tacho pulses [0..MAX]
 *  \param  (int32_t)  STEP2   - Tacho pulses [0..MAX]
 *  \param  (int32_t)  STEP3   - Tacho pulses [0..MAX]
 *  \param  (int8_t)   BRAKE   - 0 = Coast, 1 = BRAKE
 */
/*! \brief  opOUTPUT_STEP_SPEED byte code
 *
 */
void      outputStepSpeed(int8_t motorMask, int8_t Speed, int32_t Step1, int32_t Step2, int32_t Step3, int8_t Brake)
{
  int8_t       Tmp;
  STEPSPEED   StepSpeed;

  StepSpeed.Cmd    =   opOUTPUT_STEP_SPEED;
  StepSpeed.Nos    =  motorMask;
  StepSpeed.Speed  =  Speed;
  StepSpeed.Step1  =  Step1;
  StepSpeed.Step2  =  Step2;
  StepSpeed.Step3  =  Step3;
  StepSpeed.Brake  =  Brake;

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,(int8_t*)&(StepSpeed.Cmd),sizeof(StepSpeed));

    for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
    {// Set calling id for all involved inputs

      if (StepSpeed.Nos & (0x01 << Tmp))
      {
        OutputInstance.Owner[Tmp] = 0;
      }
    }
  }

}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_TIME_SPEED (LAYER, NOS, SPEED, STEP1, STEP2, STEP3, BRAKE)  </b>
 *
 *- Set Ramp up, constant and rampdown steps and power of the outputs\n
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \param  (int8_t)   SPEED   - Power [-100..100]
 *  \param  (int32_t)  STEP1   - Time in mS [0..MAX]
 *  \param  (int32_t)  STEP2   - Time in mS [0..MAX]
 *  \param  (int32_t)  STEP3   - Time in mS [0..MAX]
 *  \param  (int8_t)   BRAKE   - 0 = Coast, 1 = BRAKE
 */
/*! \brief  opOUTPUT_TIME_SPEED byte code
 *
 */
void      outputTimeSpeed(int8_t motorMask, int8_t Speed, int32_t Time1, int32_t Time2, int32_t Time3, int8_t Brake)
{
  int8_t       Tmp;
  TIMESPEED   TimeSpeed;

  TimeSpeed.Cmd    =   (int8_t)opOUTPUT_TIME_SPEED;
  TimeSpeed.Nos    =  motorMask;
  TimeSpeed.Speed  =  Speed;
  TimeSpeed.Time1  =  Time1;
  TimeSpeed.Time2  =  Time2;
  TimeSpeed.Time3  =  Time3;
  TimeSpeed.Brake  =  Brake;

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,(int8_t*)&(TimeSpeed.Cmd),sizeof(TimeSpeed));

    for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
    {
      // Set calling id for all involved inputs
      if (TimeSpeed.Nos & (0x01 << Tmp))
      {
        OutputInstance.Owner[Tmp] = 0;
      }
    }
  }
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_STEP_SYNC (LAYER, NOS, SPEED, TURN, STEP, BRAKE)  </b>
 *
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \param  (int8_t)   SPEED   - Power              [-100..100]
 *  \param  (int16_t)  TURN    - Turn Ratio         [-200..200]
 *  \param  (int32_t)  STEP    - Tacho Pulses       [0..MAX]
 *  \param  (int8_t)   BRAKE   - 0 = Coast, 1 = BRAKE
 */
/*! \brief  opOUTPUT_STEP_SYNC byte code
 *
 */
void      outputStepSync(int8_t motorMask, int8_t Speed, int16_t Turn, int32_t Step, int8_t Brake)
{
  int8_t       Tmp;
  STEPSYNC    StepSync;

  StepSync.Cmd   =   opOUTPUT_STEP_SYNC;
  StepSync.Nos   =  motorMask;
  StepSync.Speed =  Speed;
  StepSync.Turn  =  Turn;
  StepSync.Step  =  Step;
  StepSync.Brake =  Brake;

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,(int8_t*)&(StepSync.Cmd),sizeof(StepSync));

    for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
    {
      // Set calling id for all involved outputs
      if (StepSync.Nos & (0x01 << Tmp))
      {
        OutputInstance.Owner[Tmp] = 0;
      }
    }
  }
}


/*! \page cOutput Output
 *  <hr size="1"/>
 *  <b>     opOUTPUT_TIME_SYNC (LAYER, NOS, SPEED, TURN, STEP, BRAKE)  </b>
 *
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \param  (int8_t)   SPEED   - Power              [-100..100]
 *  \param  (int16_t)  TURN    - Turn Ratio         [-200..200]
 *  \param  (int32_t)  TIME    - Time in ms         [0..MAX]
 *  \param  (int8_t)   BRAKE   - 0 = Coast, 1 = BRAKE
 *
 */
/*! \brief  opOUTPUT_STEP_SYNC byte code
 *
 */
void      outputTimeSync(int8_t motorMask, int8_t Speed, int16_t Turn, int32_t Time, int8_t Brake)
{
  int8_t     Tmp;
  TIMESYNC  TimeSync;

  TimeSync.Cmd   =  opOUTPUT_TIME_SYNC;
  TimeSync.Nos   =  motorMask;
  TimeSync.Speed =  Speed;
  TimeSync.Turn  =  Turn;
  TimeSync.Time  =  Time;
  TimeSync.Brake =  Brake;

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile,(int8_t*)&(TimeSync.Cmd),sizeof(TimeSync));

    for (Tmp = 0; Tmp < OUTPUTS; Tmp++)
    {
      // Set calling id for all involved outputs
      if (TimeSync.Nos & (0x01 << Tmp))
      {
        OutputInstance.Owner[Tmp] = 0;
      }
    }
  }
}


/*! \page   cOutput
 *  <hr size="1"/>
 *  <b>     opOUTPUT_READ (LAYER, NO, *SPEED, *TACHO) </b>
 *
 *  \param  (int8_t)   NO      - Output no [0..3]
 *  \param  (int8_t)   *SPEED  - Speed [-100..100]
 *  \param  (int32_t)  *TACHO  - Tacho pulses [-MAX .. +MAX]
 *
 */
/*! \brief  opOUTPUT_READ byte code
 *
 */
void      cOutputRead(int8_t motor, int8_t *Speed, int32_t *Tacho)
{

  *Speed = 0;
  *Tacho = 0;

  if (motor < OUTPUTS)
  {
    *Speed  =  OutputInstance.pMotor[motor].Speed;
    *Tacho  =  OutputInstance.pMotor[motor].TachoCounts;
  }

}


/*! \page   cOutput
 *  \anchor opOUTPUT_TEST \n
 *  <hr size="1"/>
 *  <b>     opOUTPUT_TEST (LAYER, NOS, BUSY) </b>
 *
 *- Testing if output is not used \n
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \return (int8_t)   BUSY    - Output busy flag (0 = ready, 1 = Busy)
 */
/*! \brief  opOUTPUT_TEST byte code
 *
 */
void      outputTest(int8_t motorMask, int8_t *Busy)
{

  int     test;
  int     test2;

  char    BusyReturn[20]; // Busy mask

  *Busy = 0;

  if (OutputInstance.PwmFile >= 0)
  {
    read(OutputInstance.PwmFile,BusyReturn,10);
    sscanf(BusyReturn,"%u %u",&test,&test2);

    if (motorMask & (int8_t)test2)
    {
      *Busy = 1;
    }
  }
}


/*! \page   cOutput
 *  \anchor opOUTPUT_TEST \n
 *  <hr size="1"/>
 *  <b>     opOUTPUT_CLR_COUNT (LAYER, NOS) </b>
 *
 *- Clearing tacho count when used as sensor \n
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 */
/*! \brief  opOUTPUT_CLR_COUNT byte code
 *
 */
void      outputClrCount(int8_t motorMask)
{

  int8_t   ClrCnt[2];
  uint8_t   Tmp;

  ClrCnt[0]   =  opOUTPUT_CLR_COUNT;
  ClrCnt[1]   =  motorMask;

  if (OutputInstance.PwmFile >= 0)
  {
    write(OutputInstance.PwmFile, ClrCnt, sizeof(ClrCnt));
  }

  // Also the user layer entry to get immediate clear
  for(Tmp = 0; Tmp < OUTPUTS; Tmp++)
  {
    if (ClrCnt[1] & (1 << Tmp))
    {
      OutputInstance.pMotor[Tmp].TachoSensor = 0;
    }
  }
}


/*! \page   cOutput
 *  \anchor opOUTPUT_TEST \n
 *  <hr size="1"/>
 *  <b>     opOUTPUT_GET_COUNT (LAYER, NOS, *TACHO) </b>
 *
 *- Getting tacho count when used as sensor - values are in shared memory \n
 *
 *
 *  \param  (int8_t)   motorMask     - Output bit field [0x00..0x0F]
 *  \param  (int32_t)  *TACHO  - Tacho pulses [-MAX .. +MAX]
 */
/*! \brief  opOUTPUT_GET_COUNT byte code
 *
 */
void      outputGetCount(int8_t motor, int32_t *Tacho)
{
  *Tacho = 0;

  if (motor < OUTPUTS)
  {
    *Tacho  =  OutputInstance.pMotor[motor].TachoSensor;
  }

}


//*****************************************************************************
