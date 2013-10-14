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


#ifndef C_OUTPUT_H_
#define C_OUTPUT_H_

// #include "lms2012.h"

#define  opPROGRAM_STOP              0x02//      0010
#define  opPROGRAM_START             0x03//      0011
#define  opOUTPUT_GET_TYPE           0xA0//     00000
#define  opOUTPUT_SET_TYPE           0xA1//     00001
#define  opOUTPUT_RESET              0xA2//     00010
#define  opOUTPUT_STOP               0xA3//     00011
#define  opOUTPUT_POWER              0xA4//     00100
#define  opOUTPUT_SPEED              0xA5//     00101
#define  opOUTPUT_START              0xA6//     00110
#define  opOUTPUT_POLARITY           0xA7//     00111
#define  opOUTPUT_READ               0xA8//     01000
#define  opOUTPUT_TEST               0xA9//     01001
#define  opOUTPUT_READY              0xAA//     01010
#define  opOUTPUT_POSITION           0xAB//     01011
#define  opOUTPUT_STEP_POWER         0xAC//     01100
#define  opOUTPUT_TIME_POWER         0xAD//     01101
#define  opOUTPUT_STEP_SPEED         0xAE//     01110
#define opOUTPUT_TIME_SPEED          0xAF//     01111

#define  opOUTPUT_STEP_SYNC          0xB0//     10000
#define  opOUTPUT_TIME_SYNC          0xB1//     10001
#define  opOUTPUT_CLR_COUNT          0xB2//     10010
#define  opOUTPUT_GET_COUNT          0xB3//     10011

#define  opOUTPUT_PRG_STOP           0xB4//     10100

void outputResetAll(void);
RESULT outputInit(void);
RESULT outputOpen(void);
RESULT outputExit(void);
void outputSetTypes(char *pTypes);
void ResetDelayCounter(uint8_t Pattern);
uint8_t cMotorGetBusyFlags(void);
void cMotorSetBusyFlags(uint8_t Flags);
void outputSetType(int8_t motor, int8_t Type);
void outputReset(int8_t motorMask);
void outputStop(int8_t motorMask, int8_t Brake);
void outputSpeed(int8_t motorMask, int8_t Speed);
void outputPower(int8_t motorMask, int8_t Power);
void outputStart(int8_t motorMask);
void outputPolarity(int8_t motorMask, int8_t Pol);
void outputStepPower(int8_t motorMask, int8_t Power, int32_t Step1, int32_t Step2, int32_t Step3, int8_t Brake);
void outputTimePower(int8_t motorMask, int8_t Power, int32_t Time1, int32_t Time2, int32_t Time3, int8_t Brake);
void outputStepSpeed(int8_t motorMask, int8_t Speed, int32_t Step1, int32_t Step2, int32_t Step3, int8_t Brake);
void outputTimeSpeed(int8_t motorMask, int8_t Speed, int32_t Time1, int32_t Time2, int32_t Time3, int8_t Brake);
void outputStepSync(int8_t motorMask, int8_t Speed, int16_t Turn, int32_t Step, int8_t Brake);
void outputTimeSync(int8_t motorMask, int8_t Speed, int16_t Turn, int32_t Time, int8_t Brake);
void cOutputRead(int8_t motor, int8_t *Speed, int32_t *Tacho);
void outputTest(int8_t motorMask, int8_t *Busy);
void outputClrCount(int8_t motorMask);
void outputGetCount(int8_t motor, int32_t *Tacho);

typedef struct
{
  //*****************************************************************************
  // Output Global variables
  //*****************************************************************************

  int8_t       OutputType[OUTPUTS];
  uint16_t    Owner[OUTPUTS];

  int         PwmFile;
  int         MotorFile;

  MOTORDATA   MotorData[OUTPUTS];
  MOTORDATA   *pMotor;
}
OUTPUT_GLOBALS;

#if       (HARDWARE == SIMULATION)
  extern OUTPUT_GLOBALS * gOutputInstance;
  #define OutputInstance (*gOutputInstance)

  void setOutputInstance(OUTPUT_GLOBALS * _Instance);
  OUTPUT_GLOBALS * getOutputInstance();
#else
  extern OUTPUT_GLOBALS OutputInstance;
#endif

#endif /* C_OUTPUT_H_ */
