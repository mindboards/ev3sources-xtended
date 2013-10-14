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


/*! \page debug Terminal Configuration
 *
    \verbatim


TERMINAL CONFIGURATION (in top of "lms2012.h")

Normal release:                     DEBUG not defined, TERMINAL_ENABLED = 0, DEBUG_UART = 4

Printf working:                     DEBUG not defined, TERMINAL_ENABLED = 0, DEBUG_UART = 0

Development with debug messages:    DEBUG defined, TERMINAL_ENABLED = 1, DEBUG_UART = 0

*/


//#define   DEBUG                         //!< When defined debug messages is output on standard I/O (d_uart is different)

#define   TERMINAL_ENABLED      0       //!< DEBUG terminal enabled (0 = disabled, 1 = enabled)
#define   DEBUG_UART            4       //!< UART used for debug (0 = port1, 1 = port2, ... 4 = none)

#define   DEBUG_RECHARGEABLE

#ifndef   LMS2012_H_
#define   LMS2012_H_

#include <stdint.h>

#define   S1                    0
#define   S2                    1
#define   S3                    2
#define   S4                    3


//  HARDWARE PLATFORM
#define   EP2                   4       //!< Schematics revision D
#define   FINALB                3       //!< Schematics revision B and C
#define   FINAL                 2       //!< Final prototype
#define   SIMULATION            0       //!< LEGO digital simulation

#define   PLATFORM_START        FINAL   //!< Oldest supported hardware (older versions will use this)
#define   PLATFORM_END          EP2     //!< Newest supported hardware (newer versions will use this)

// Will be removed when not used anymore
#define   A4                    -1
#define   EVALBOARD             -2
#define   ONE2ONE               1       //!< First real size prototype


#ifdef LEGO_SIMULATION
  #define HARDWARE SIMULATION
#else

#ifndef   HARDWARE
  #define HARDWARE      FINAL           //!< Actual hardware platform (must be one of above)
#endif

#endif

//  Support for module parameter "HwId"
//
//  Readout   File    int       PCB
//
//  V1.00     10      10        MP      (h = home, e = education)
//  V0.50     05      5         EP3
//  V0.40     04      4         EP2
//  V0.30     03      3         EP1     (FINALB)  (DEFAULT if file "HwId" not found)
//  V0.20     02      2         FINAL

#ifdef HW_ID_SUPPORT

char      *HwId = "03";

#define   HWID          (((HwId[0] - '0') * 10) + (HwId[1] - '0'))

#endif


//  FIRMWARE VARIANTS

//#define   OLDCALL                       //!< Don't use optimised sub calls
//#define   DISABLE_PRU_UARTS             //!< Don't use port 3 and 4 for UART sensors
//#define   DISABLE_OLD_COLOR             //!< Don't support NXT color sensor
//#define   DISABLE_ADC                   //!< Don't use ADC (no clock EMC test)
//#define   ADC_BITBANGING                //!< Don't use SPI for a/d converter
//#define   DISABLE_DAISYCHAIN
//#define   DISABLE_DAISYCHAIN_COM_CALL
//#define   DISABLE_FAST_DATALOG_BUFFER
//#define   DISABLE_BUMBED
//#define   LOG_ASCII
//#define   DISABLE_FIQ_IIC
#define   UART1_FAKE_INTERRUPT          //!< Don't use real interrupt on UART1 (linux prompt)
//#define   DISABLE_LOW_VOLTAGE           //!< Don't shut down on low voltage
//#define   ENABLE_HIGH_CURRENT           //!< Don't shut down on high current
//#define   DISABLE_LOW_MEMORY            //!< Don't check low memory
//#define   DISABLE_UART_DATA_ERROR       //!< Don't reset UART sensor if timeout or crc error
#define   DISABLE_PREEMPTED_VM          //!< Don't run VM as preempted
//#define   DISABLE_SDCARD_SUPPORT        //!< Don't use SD card
#define   DISABLE_USBSTICK_SUPPORT      //!< Don't use USB stick
//#define   ENABLE_PERFORMANCE_TEST       //!< Show performance bar in the top line
//#define   ENABLE_LOAD_TEST              //!< Show integrated current in the top line
//#define   ENABLE_MEMORY_TEST            //!< Show used memory in the top line
//#define   ENABLE_STATUS_TEST
//#define   DISABLE_VIRTUAL_BATT_TEMP
//#define   DISABLE_SOUND
//#define   DISABLE_PAR_ALIGNMENT         //!< Disable possibility to align sub call parameter types
//#define   DISABLE_NEW_CALL_MUTEX        //!< Disable smart object switching after return from non reentrant sub call (enables blocked thread call)
//#define   DISABLE_SYSTEM_BYTECODE       //!< Disable the use of opSYSTEM command
//#define   DISABLE_FILENAME_CHECK        //!< Disable "c_memory" filename check
//#define   DISABLE_AD_WORD_PROTECT       //!< Disable A/D word result protection

#define   TESTDEVICE    3

#ifndef PCASM
#include  <asm/types.h>
#endif


// Hardware

#define   OUTPUTS                       4                     //!< Number of output ports in the system
#define   INPUTS                        4                      //!< Number of input  ports in the system
#define   BUTTONS                       6                     //!< Number of buttons in the system
#define   LEDS                          4                        //!< Number of LEDs in the system

#define   CHAIN_DEPT                    4                  //!< Number of bricks in the USB daisy chain (master + slaves)

#define   EVENT_BT_PIN                  vmEVENT_BT_PIN



// Files
#define   DETAILS_FILE                  vmDETAILS_FILE                //!< File containing firmware version

#define   SETTINGS_DIR                "/home/root/lms2012/sys/settings"
#define   EXT_CONFIG                  ".rcf"                        //!< Robot Configuration File

#define   CALDATA_FILE_NAME             "caldata"
#define   SYSPERMISSIONS                (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/*! \page system
 *
 *  \verbatim
 */

#define   PROJECT                       "LMS2012"
#define   VERS                          1.04
#define   SPECIALVERS                   'H'           //!< Minor version (not shown if less than ASCII zero)


#define   MAX_PROGRAMS          1                     //!< Max number of programs (including UI and direct commands) running at a time
#define   MAX_BREAKPOINTS       4                     //!< Max number of breakpoints (opCODES depends on this value)
#define   MAX_LABELS            32                    //!< Max number of labels per program
#define   MAX_DEVICE_TYPE       127                   //!< Highest type number (positive)
#define   MAX_VALID_TYPE        101                   //!< Highest valid type
#define   MAX_DEVICE_MODES      8                     //!< Max number of modes in one device
#define   MAX_DEVICE_DATASETS   8                     //!< Max number of data sets in one device
#define   MAX_DEVICE_DATALENGTH 32                    //!< Max device data length

#define   MAX_DEVICE_BUSY_TIME  1200                  //!< Max number of mS a device can be busy when read

#define   MAX_DEVICE_TYPES      ((MAX_DEVICE_TYPE + 1) * MAX_DEVICE_MODES)//!< Max number of different device types and modes (max type data list size)

#define   MAX_FRAMES_PER_SEC    10                    //!< Max frames per second update in display

#define   CACHE_DEEPT           10                    //!< Max number of programs cached (in RECENT FILES MENU)
#define   MAX_HANDLES           250                   //!< Max number of handles to memory pools and arrays in one program

#define   MAX_ARRAY_SIZE        1000000000            //!< Max array size
#define   MIN_ARRAY_ELEMENTS    0                     //!< Min elements in a int8_t array

#define   INSTALLED_MEMORY      6000                  //!< Flash allocated to hold user programs/data
#define   RESERVED_MEMORY       100                   //!< Memory reserve for system [KB]
#define   LOW_MEMORY            500                   //!< Low memory warning [KB]

#define   LOGBUFFER_SIZE        1000                  //!< Min log buffer size
#define   DEVICE_LOGBUF_SIZE    300                   //!< Device log buffer size (black layer buffer)
#define   MIN_LIVE_UPDATE_TIME  10                    //!< [mS] Min sample time when live update

#define   MIN_IIC_REPEAT_TIME   10                    //!< [mS] Min IIC device repeat time
#define   MAX_IIC_REPEAT_TIME   1000                  //!< [mS] Max IIC device repeat time

#define   MAX_COMMAND_BYTECODES 64                    //!< Max number of byte codes in a debug terminal direct command
#define   MAX_COMMAND_LOCALS    64                    //!< Max number of bytes allocated for direct command local variables
#define   MAX_COMMAND_GLOBALS   1021                  //!< Max number of bytes allocated for direct command global variables

#define   UI_PRIORITY           20                    //!< UI byte codes before switching VM thread
#define   C_PRIORITY            200                   //!< C call byte codes

#ifndef DISABLE_PREEMPTED_VM
#define   PRG_PRIORITY          2000                  //!< Prg byte codes before switching VM thread
#else
#define   PRG_PRIORITY          200                   //!< Prg byte codes before switching VM thread
#endif

#define   BUTTON_DEBOUNCE_TIME        30
#define   BUTTON_START_REPEAT_TIME    400
#define   BUTTON_REPEAT_TIME          200

#define   LONG_PRESS_TIME       3000                  //!< [mS] Time pressed before long press recognised

#define   ADC_REF               5000                  //!< [mV]  maximal value on ADC
#define   ADC_RES               4095                  //!< [CNT] maximal count on ADC

#define   IN1_ID_HYSTERESIS     50                    //!< [mV]  half of the span one Id takes up on input connection 1 voltage
#define   OUT5_ID_HYSTERESIS    100                   //!< [mV]  half of the span one Id takes up on output connection 5 voltage

#define   DEVICE_UPDATE_TIME    1000000               //!< Min device (sensor) update time [nS]
#define   DELAY_TO_TYPEDATA     10000                 //!< Time from daisy chain active to upload type data [mS]
#define   DAISYCHAIN_MODE_TIME  10                    //!< Time for daisy chain change mode [mS]
#define   MAX_FILE_HANDLES      64                    //!< Max number of down load file handles
#define   MIN_HANDLE            3                     //!< Min file handle to close

#define   ID_LENGTH             7                     //!< Id length  (BT MAC id) (incl. zero terminator)
#define   NAME_LENGTH           12                    //!< Name length (not including zero termination)

#define   ERROR_BUFFER_SIZE     8                     //!< Number of errors in buffer

#define   PWM_DEVICE            "lms_pwm"             //!< PWM device name
#define   PWM_DEVICE_NAME       "/dev/lms_pwm"        //!< PWM device file name

#define   MOTOR_DEVICE          "lms_motor"           //!< TACHO device name
#define   MOTOR_DEVICE_NAME     "/dev/lms_motor"      //!< TACHO device file name

#define   ANALOG_DEVICE         "lms_analog"          //!< ANALOG device name
#define   ANALOG_DEVICE_NAME    "/dev/lms_analog"     //!< ANALOG device file name

#define   POWER_DEVICE          "lms_power"           //!< POWER device name
#define   POWER_DEVICE_NAME     "/dev/lms_power"      //!< POWER device file name

#define   DCM_DEVICE            "lms_dcm"             //!< DCM device name
#define   DCM_DEVICE_NAME       "/dev/lms_dcm"        //!< DCM device file name

#define   UI_DEVICE             "lms_ui"              //!< UI device name
#define   UI_DEVICE_NAME        "/dev/lms_ui"         //!< UI device file name

#define   LCD_DEVICE            "lms_display"         //!< DISPLAY device name
//#define   LCD_DEVICE_NAME       "/dev/lms_display"    //!< DISPLAY device file name
#define   LCD_DEVICE_NAME       "/dev/fb0"            //!< DISPLAY device file name

#define   UART_DEVICE           "lms_uart"            //!< UART device name
#define   UART_DEVICE_NAME      "/dev/lms_uart"       //!< UART device file name

#define   USBDEV_DEVICE         "lms_usbdev"          //!< USB device
#define   USBDEV_DEVICE_NAME    "/dev/lms_usbdev"     //!< USB device

#define   USBHOST_DEVICE        "lms_usbhost"         //!< USB host
#define   USBHOST_DEVICE_NAME   "/dev/lms_usbhost"    //!< USB host

#define   SOUND_DEVICE          "lms_sound"           //!< SOUND device name
#define   SOUND_DEVICE_NAME     "/dev/lms_sound"      //!< SOUND device

#define   IIC_DEVICE            "lms_iic"             //!< IIC device name
#define   IIC_DEVICE_NAME       "/dev/lms_iic"        //!< IIC device

#define   BT_DEVICE             "lms_bt"              //!< BT device name
#define   BT_DEVICE_NAME        "/dev/lms_bt"         //!< BT device

#define   UPDATE_DEVICE          "lms_update"         //!< UPDATE device name
#define   UPDATE_DEVICE_NAME     "/dev/lms_update"    //!< UPDATE device

#define   TEST_PIN_DEVICE       "lms_tst_pin"         //!< TEST pin device name
#define   TEST_PIN_DEVICE_NAME  "/dev/lms_tst_pin"    //!< TEST pin device file name

#define   TEST_UART_DEVICE      "lms_tst_uart"        //!< TEST UART device name
#define   TEST_UART_DEVICE_NAME "/dev/lms_tst_uart"   //!< TEST UART device file name


#define   DIR_DEEPT             vmDIR_DEEPT           //!< Max directory items allocated

// File

//***********************************************************************************************************************
//! \todo Filename sizes should only use vmPATHSIZE, vmNAMESIZE, vmEXTSIZE and vmFILENAMESIZE

#define   FILENAMESIZE          120        //!< All inclusive (path, filename, extension and zero termination)
#define   FILENAME_SIZE         52                    //!< User filename size without extension including zero
#define   FOLDERNAME_SIZE       10                    //!< Folder name size relative to "lms2012" folder including zero
#define   SUBFOLDERNAME_SIZE    FILENAME_SIZE         //!< Sub folder name size without "/" including zero

#define   MAX_FILENAME_SIZE     (FOLDERNAME_SIZE + SUBFOLDERNAME_SIZE + FILENAME_SIZE + 5)

//***********************************************************************************************************************

#define   TYPEDATE_FILE_NAME    "typedata"            //!< TypeData filename
#define   ICON_FILE_NAME        "icon"                //!< Icon image filename
#define   TEXT_FILE_NAME        "text"                //!< Text filename

//#define   DEMO_FILE_NAME        "../prjs/BrkProg_SAVE/Demo.rpf"

#define   char_NAN     ((int8_t)(-128))
#define   short_NAN    ((int16_t)(-32768))
#define   int_NAN    ((int32_t)(0x80000000))
#define   float_NAN     ((float)0 / (float)0) //(0x7FC00000)


#define   DEFAULT_FOLDER        "ui"                  //!< Folder containing the first small programs
#define   DEFAULT_UI            "ui"                  //!< Default user interface

#define   DEFAULT_VOLUME        vmDEFAULT_VOLUME
#define   DEFAULT_SLEEPMINUTES  vmDEFAULT_SLEEPMINUTES

#define   COM_CMD_DEVICE_NAME   USBDEV_DEVICE_NAME    //!< USB HID command pipe device file name

/*! \endverbatim
 *
 */

/*! \page powermanagement Power Management
 *
 *  This section describes various constants used in the power management
 *
 *
 *\n
 *  <hr size="1"/>
 *  <b>Battery Indicator</b>
 *\n
 *  <hr size="1"/>
 *  \verbatim
 */

#define   BATT_INDICATOR_HIGH           7500          //!< Battery indicator high [mV]
#define   BATT_INDICATOR_LOW            6200          //!< Battery indicator low [mV]

#define   ACCU_INDICATOR_HIGH           7500          //!< Rechargeable battery indicator high [mV]
#define   ACCU_INDICATOR_LOW            7100          //!< Rechargeable battery indicator low [mV]

/*! \endverbatim
 *  \subpage pmbattind
 *\n
 *  <hr size="1"/>
 *  <b>Low Voltage Shutdown</b>
 *\n
 *  <hr size="1"/>
 *  \verbatim
 */

#define   LOW_VOLTAGE_SHUTDOWN_TIME     10000         //!< Time from shutdown lower limit to shutdown [mS]

#define   BATT_WARNING_HIGH             6.2           //!< Battery voltage warning upper limit [V]
#define   BATT_WARNING_LOW              5.5           //!< Battery voltage warning lower limit [V]
#define   BATT_SHUTDOWN_HIGH            5.5           //!< Battery voltage shutdown upper limit [V]
#define   BATT_SHUTDOWN_LOW             4.5           //!< Battery voltage shutdown lower limit [V]

#define   ACCU_WARNING_HIGH             7.1           //!< Rechargeable battery voltage warning upper limit [V]
#define   ACCU_WARNING_LOW              6.5           //!< Rechargeable battery voltage warning lower limit [V]
#define   ACCU_SHUTDOWN_HIGH            6.5           //!< Rechargeable battery voltage shutdown upper limit [V]
#define   ACCU_SHUTDOWN_LOW             6.0           //!< Rechargeable battery voltage shutdown lower limit [V]

 /*! \endverbatim
  *  \subpage pmbattsd
  *\n
  */
#ifdef ENABLE_HIGH_CURRENT
 /*! \page powermanagement
  *
  *  <hr size="1"/>
  *  <b>High Load Shutdown</b>
  *\n
  *  <hr size="1"/>
  *  \verbatim
  */

#define   LOAD_SHUTDOWN_FAIL            4.0           //!< Current limit for instantly shutdown [A]
#define   LOAD_SHUTDOWN_HIGH            3.0           //!< Current limit for integrated current shutdown [A*S]
#define   LOAD_BREAK_EVEN               1.75          //!< Current limit for integrated current break even [A*S]

#define   LOAD_SLOPE_UP                 0.2           //!< Current slope when current is above break even [A/S]
#define   LOAD_SLOPE_DOWN               0.1           //!< Current slope when current is below break even [A/S]

 /*! \endverbatim
  *  \subpage pmloadsd
  *\n
  */
#endif

#ifndef DISABLE_VIRTUAL_BATT_TEMP
 /*! \page powermanagement
  *
  *  <hr size="1"/>
  *  <b>High Temperature Shutdown</b>
  *\n
  *  <hr size="1"/>
  *  \verbatim
  */

#define   TEMP_SHUTDOWN_FAIL            45.0          //!< Temperature rise before fail shutdown  [C]
#define   TEMP_SHUTDOWN_WARNING         40.0          //!< Temperature rise before warning        [C]

 /*! \endverbatim
  *  \subpage pmtempsd
  *\n
  */
#endif

// Per start of (polution) defines
#define   MAX_SOUND_DATA_SIZE   250
#define   SOUND_CHUNK           250
#define SOUND_ADPCM_CHUNK       125
#define SOUND_MASTER_CLOCK  132000000
#define SOUND_TONE_MASTER_CLOCK 1031250
#define SOUND_MIN_FRQ       250
#define SOUND_MAX_FRQ       10000
#define SOUND_MAX_LEVEL     8
#define SOUND_FILE_BUFFER_SIZE  SOUND_CHUNK + 2 // 12.8 mS @ 8KHz
#define SOUND_BUFFER_COUNT  3
#define SOUND_FILE_FORMAT_NORMAL    0x0100  // RSO-file
#define SOUND_FILE_FORMAT_COMPRESSED  0x0101  // RSO-file compressed
// Per end of defines


/*!
 *
 * \n
 */


#define   VtoC(V)               ((uint16_t)((V * ADC_RES) / ADC_REF))
#define   CtoV(C)               ((uint16_t)((C * ADC_REF) / ADC_RES))
#define   MtoV(M)               ((uint16_t)((M * ADC_REF * 100) / (ADC_RES * 52)))

#define   KB                    1024

enum
{
  FALSE = 0,
  TRUE  = 1,
};

 /*! \page formats
  *
  *        Data formats used in device type formats
  *  \verbatim
  */

 typedef   enum
 {
   DATA_8        = 0x00,                 //!< int8_t  (don't change)
   DATA_16       = 0x01,                 //!< int16_t (don't change)
   DATA_32       = 0x02,                 //!< int32_t (don't change)
   DATA_F        = 0x03,                 //!< float  (don't change)
   DATA_S        = 0x04,                 //!< Zero terminated string
   DATA_A        = 0x05,                 //!< Array handle

   DATA_V        = 0x07,                 //!< Variable type

   DATA_PCT      = 0x10,                 //!< Percent (used in opINPUT_READEXT)
   DATA_RAW      = 0x12,                 //!< Raw     (used in opINPUT_READEXT)
   DATA_SI       = 0x13,                 //!< SI unit (used in opINPUT_READEXT)

   DATA_FORMATS
 }
 DATA_FORMAT;


// Reserved device types
typedef   enum
{
//  TYPE_KEEP                     =   0,  //!< Type value that won't change type in byte codes
  TYPE_NXT_TOUCH                =   1,  //!< Device is NXT touch sensor
  TYPE_NXT_LIGHT                =   2,  //!< Device is NXT light sensor
  TYPE_NXT_SOUND                =   3,  //!< Device is NXT sound sensor
  TYPE_NXT_COLOR                =   4,  //!< Device is NXT color sensor

  TYPE_TACHO                    =   7,  //!< Device is a tacho motor
  TYPE_MINITACHO                =   8,  //!< Device is a mini tacho motor
  TYPE_NEWTACHO                 =   9,  //!< Device is a new tacho motor

  TYPE_THIRD_PARTY_START        =  50,
  TYPE_THIRD_PARTY_END          =  99,

  TYPE_IIC_UNKNOWN              = 100,

  TYPE_NXT_TEST                 = 101,  //!< Device is a NXT ADC test sensor

  TYPE_NXT_IIC                  = 123,  //!< Device is NXT IIC sensor
  TYPE_TERMINAL                 = 124,  //!< Port is connected to a terminal
  TYPE_UNKNOWN                  = 125,  //!< Port not empty but type has not been determined
  TYPE_NONE                     = 126,  //!< Port empty or not available
  TYPE_ERROR                    = 127,  //!< Port not empty and type is invalid
}
TYPE;



/*! \page connections Connections
 *
 *  \anchor connectiontypes
 *
 *  Following defines sets the input and output port connection type\n
 *
 *  These connection types are evaluated in the DCM driver \ref DcmDriver "Device Detection Manager Driver"
 *
 *  \verbatim
 */

typedef   enum
{
  CONN_UNKNOWN                  = 111,  //!< Connection is fake (test)

  CONN_DAISYCHAIN               = 117,  //!< Connection is daisy chained
  CONN_NXT_COLOR                = 118,  //!< Connection type is NXT color sensor
  CONN_NXT_DUMB                 = 119,  //!< Connection type is NXT analog sensor
  CONN_NXT_IIC                  = 120,  //!< Connection type is NXT IIC sensor

  CONN_INPUT_DUMB               = 121,  //!< Connection type is LMS2012 input device with ID resistor
  CONN_INPUT_UART               = 122,  //!< Connection type is LMS2012 UART sensor

  CONN_OUTPUT_DUMB              = 123,  //!< Connection type is LMS2012 output device with ID resistor
  CONN_OUTPUT_INTELLIGENT       = 124,  //!< Connection type is LMS2012 output device with communication
  CONN_OUTPUT_TACHO             = 125,  //!< Connection type is LMS2012 tacho motor with series ID resistance

  CONN_NONE                     = 126,  //!< Port empty or not available
  CONN_ERROR                    = 127,  //!< Port not empty and type is invalid
}
CONN;

/*\endverbatim
 *
 *  \n
 */



/*! \page objectstatus Object Status
 *
\verbatim

Image load  ->   Initialize all objects


Initialize  ->   if TriggerCount == 0 ->  RUNNING   (VMTHREAD)
                 else                 ->  STOPPED   (waiting for 1. trigger)

1.Trigger   ->   Initialise           ->  WAITING

Triggered   ->   Enqueue              ->  RUNNING

Done        ->   Dequeue              ->  STOPPED


Program start
           |
           v
        STOPPED -------> WAITING -------> RUNNING --------,
           ^    1.trig/          n.trig/          done/   |
           |                     Reset+Enqueue    Dequeue |
           |                                              |
           '----------------------------------------------'

\endverbatim
 */

/*! \page results Results

    Describes result from executing functions

    \verbatim */


typedef   enum
{
  OK            = 0,                    //!< No errors to report
  BUSY          = 1,                    //!< Busy - try again
  FAIL          = 2,                    //!< Something failed
  STOP          = 4                     //!< Stopped
}
RESULT;


#define     TYPE_NAME_LENGTH    11
#define     SYMBOL_LENGTH       4       //!< Symbol leng th (not including zero)

/*! \struct TYPES
 *          Device type data
 */
typedef   struct // if data type changes - remember to change "inputTypeDataInit" !
{
  char     Name[TYPE_NAME_LENGTH + 1]; //!< Device name
  int8_t     Type;                       //!< Device type
  int8_t     Connection;
  int8_t     Mode;                       //!< Device mode
  int8_t     DataSets;
  int8_t     Format;
  int8_t     Figures;
  int8_t     Decimals;
  int8_t     Views;
  float     RawMin;                     //!< Raw minimum value      (e.c. 0.0)
  float     RawMax;                     //!< Raw maximum value      (e.c. 1023.0)
  float     PctMin;                     //!< Percent minimum value  (e.c. -100.0)
  float     PctMax;                     //!< Percent maximum value  (e.c. 100.0)
  float     SiMin;                      //!< SI unit minimum value  (e.c. -100.0)
  float     SiMax;                      //!< SI unit maximum value  (e.c. 100.0)
  uint16_t     InvalidTime;                //!< mS from type change to valid data
  uint16_t     IdValue;                    //!< Device id value        (e.c. 0 ~ UART)
  int8_t     Pins;                       //!< Device pin setup
  uint8_t     Symbol[SYMBOL_LENGTH + 1];  //!< SI unit symbol
  uint16_t     Align;
}
TYPES;

#define     TYPE_PARAMETERS         19  //!< Number of members in the structure above
#define     MAX_DEVICE_INFOLENGTH   54  //!< Number of bytes in the structure above (can not be changed)


/*!\enum  ERR
 *
 *        Describes error numbers
 */
typedef   enum
{
  TOO_MANY_ERRORS_TO_BUFFER,
  TYPEDATA_TABEL_FULL,
  TYPEDATA_FILE_NOT_FOUND,
  ANALOG_DEVICE_FILE_NOT_FOUND,
  ANALOG_SHARED_MEMORY,
  UART_DEVICE_FILE_NOT_FOUND,
  UART_SHARED_MEMORY,
  IIC_DEVICE_FILE_NOT_FOUND,
  IIC_SHARED_MEMORY,
  DISPLAY_SHARED_MEMORY,
  UI_SHARED_MEMORY,
  UI_DEVICE_FILE_NOT_FOUND,
  LCD_DEVICE_FILE_NOT_FOUND,
  OUTPUT_SHARED_MEMORY,
  COM_COULD_NOT_OPEN_FILE,
  COM_NAME_TOO_SHORT,
  COM_NAME_TOO_LONG,
  COM_INTERNAL,
  VM_INTERNAL,
  VM_PROGRAM_VALIDATION,
  VM_PROGRAM_NOT_STARTED,
  VM_PROGRAM_FAIL_BREAK,
  VM_PROGRAM_INSTRUCTION_BREAK,
  VM_PROGRAM_NOT_FOUND,
  SOUND_DEVICE_FILE_NOT_FOUND,
  SOUND_SHARED_MEMORY,
  FILE_OPEN_ERROR,
  FILE_READ_ERROR,
  FILE_WRITE_ERROR,
  FILE_CLOSE_ERROR,
  FILE_GET_HANDLE_ERROR,
  FILE_NAME_ERROR,
  USB_SHARED_MEMORY,
  OUT_OF_MEMORY,
  ERRORS
}
ERR;


#ifndef   DISABLE_OLD_COLOR

#define   COLORS                        4
#define   CALPOINTS                     3


/*! \page NxtColorMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *
 *  \verbatim
 */

typedef   struct
{
  uint32_t   Calibration[CALPOINTS][COLORS];
  uint16_t   CalLimits[CALPOINTS - 1];
  uint16_t   Crc;
  uint16_t   ADRaw[COLORS];
  uint16_t   SensorRaw[COLORS];
}
COLORSTRUCT;

/*\endverbatim
 *
 *  \n
 */

#endif


//        INTERFACE BETWEEN SHARED LIBRARIES AND MODULES

/*! \page AnalogModuleMemory
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the raw analogue values for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

typedef   struct
{
  int16_t  InPin1[INPUTS];         //!< Analog value at input port connection 1
  int16_t  InPin6[INPUTS];         //!< Analog value at input port connection 6
  int16_t  OutPin5[OUTPUTS];       //!< Analog value at output port connection 5
  int16_t  BatteryTemp;            //!< Battery temperature
  int16_t  MotorCurrent;           //!< Current flowing to motors
  int16_t  BatteryCurrent;         //!< Current flowing from the battery
  int16_t  Cell123456;             //!< Voltage at battery cell 1, 2, 3,4, 5, and 6
#ifndef DISABLE_FAST_DATALOG_BUFFER
  int16_t  Pin1[INPUTS][DEVICE_LOGBUF_SIZE];      //!< Raw value from analog device
  int16_t  Pin6[INPUTS][DEVICE_LOGBUF_SIZE];      //!< Raw value from analog device
  uint16_t   Actual[INPUTS];
  uint16_t   LogIn[INPUTS];
  uint16_t   LogOut[INPUTS];
#endif
#ifndef   DISABLE_OLD_COLOR
  COLORSTRUCT  NxtCol[INPUTS];
#endif
  int16_t  OutPin5Low[OUTPUTS];    //!< Analog value at output port connection 5 when connection 6 is low

  int8_t   Updated[INPUTS];

  int8_t   InDcm[INPUTS];          //!< Input port device types
  int8_t   InConn[INPUTS];

  int8_t   OutDcm[OUTPUTS];        //!< Output port device types
  int8_t   OutConn[OUTPUTS];
#ifndef DISABLE_PREEMPTED_VM
  uint16_t   PreemptMilliSeconds;
#endif
}
ANALOG;

/*\endverbatim
 *
 *  \n
 */


/*! \page UartModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the uart values for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

#define   UART_DATA_LENGTH        MAX_DEVICE_DATALENGTH
#define   UART_BUFFER_SIZE        64

typedef   struct
{
  TYPES   TypeData[INPUTS][MAX_DEVICE_MODES]; //!< TypeData

#ifndef DISABLE_FAST_DATALOG_BUFFER
  uint16_t   Repeat[INPUTS][DEVICE_LOGBUF_SIZE];
  int8_t   Raw[INPUTS][DEVICE_LOGBUF_SIZE][UART_DATA_LENGTH];      //!< Raw value from UART device
  uint16_t   Actual[INPUTS];
  uint16_t   LogIn[INPUTS];
#else
  int8_t   Raw[INPUTS][UART_DATA_LENGTH];      //!< Raw value from UART device
#endif
  int8_t   Status[INPUTS];                     //!< Status
  int8_t   Output[INPUTS][UART_DATA_LENGTH];   //!< Bytes to UART device
  int8_t   OutputLength[INPUTS];
}
UART;

/*\endverbatim
 *
 *  \n
 */


#define   UART_PORT_CHANGED       0x01        //!< Input port changed
#define   UART_DATA_READY         0x08        //!< Data is ready
#define   UART_WRITE_REQUEST      0x10        //!< Write request


typedef   struct
{
  int8_t     Connection[INPUTS];
  int8_t     Type[INPUTS];
  int8_t     Mode[INPUTS];
}
DEVCON;


typedef   struct
{
  TYPES   TypeData;
  int8_t   Port;
  int8_t   Mode;
}
UARTCTL;

#define   UART_SET_CONN           _IOWR('u',0,DEVCON)
#define   UART_READ_MODE_INFO     _IOWR('u',1,UARTCTL)
#define   UART_NACK_MODE_INFO     _IOWR('u',2,UARTCTL)
#define   UART_CLEAR_CHANGED      _IOWR('u',3,UARTCTL)


/*! \page IicModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the iic values for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

#define   IIC_DATA_LENGTH         MAX_DEVICE_DATALENGTH
#define   IIC_NAME_LENGTH         8

typedef   struct
{
  TYPES   TypeData[INPUTS][MAX_DEVICE_MODES]; //!< TypeData

#ifndef DISABLE_FAST_DATALOG_BUFFER
  uint16_t   Repeat[INPUTS][DEVICE_LOGBUF_SIZE];
  int8_t   Raw[INPUTS][DEVICE_LOGBUF_SIZE][IIC_DATA_LENGTH];      //!< Raw value from IIC device
  uint16_t   Actual[INPUTS];
  uint16_t   LogIn[INPUTS];
#else
  int8_t   Raw[INPUTS][IIC_DATA_LENGTH];      //!< Raw value from IIC device
#endif
  int8_t   Status[INPUTS];                     //!< Status
  int8_t   Changed[INPUTS];
  int8_t   Output[INPUTS][IIC_DATA_LENGTH];    //!< Bytes to IIC device
  int8_t   OutputLength[INPUTS];
}
IIC;

/*\endverbatim
 *
 *  \n
 */


#define   IIC_PORT_CHANGED       0x01         //!< Input port changed
#define   IIC_DATA_READY         0x08         //!< Data is ready
#define   IIC_WRITE_REQUEST      0x10         //!< Write request


typedef   struct
{
  TYPES   TypeData;
  int8_t   Port;
  int8_t   Mode;
}
IICCTL;


typedef   struct
{
  RESULT  Result;
  int8_t   Port;
  int8_t   Repeat;
  int16_t  Time;
  int8_t   WrLng;
  int8_t   WrData[IIC_DATA_LENGTH];
  int8_t   RdLng;
  int8_t   RdData[IIC_DATA_LENGTH];
}
IICDAT;


typedef   struct
{
  int8_t   Port;
  int16_t  Time;
  int8_t   Type;
  int8_t   Mode;
  int8_t   Manufacturer[IIC_NAME_LENGTH + 1];
  int8_t   SensorType[IIC_NAME_LENGTH + 1];
  int8_t   SetupLng;
  uint32_t   SetupString;
  int8_t   PollLng;
  uint32_t   PollString;
  int8_t   ReadLng;
}
IICSTR;


#define   IIC_SET_CONN            _IOWR('i',2,DEVCON)
#define   IIC_READ_TYPE_INFO      _IOWR('i',3,IICCTL)
#define   IIC_SETUP               _IOWR('i',5,IICDAT)
#define   IIC_SET                 _IOWR('i',6,IICSTR)




#define   TST_PIN_LENGTH    8

typedef   struct
{
  int8_t   Port;
  int8_t   Length;
  int8_t   String[TST_PIN_LENGTH + 1];
}
TSTPIN;

#define   TST_PIN_ON              _IOWR('t',1,TSTPIN)
#define   TST_PIN_OFF             _IOWR('t',2,TSTPIN)
#define   TST_PIN_READ            _IOWR('t',3,TSTPIN)
#define   TST_PIN_WRITE           _IOWR('t',4,TSTPIN)




#define   TST_UART_LENGTH         UART_BUFFER_SIZE

typedef   struct
{
  int32_t  Bitrate;
  int8_t   Port;
  int8_t   Length;
  int8_t   String[TST_UART_LENGTH];
}
TSTUART;

#define   TST_UART_ON             _IOWR('t',5,TSTUART)
#define   TST_UART_OFF            _IOWR('t',6,TSTUART)
#define   TST_UART_EN             _IOWR('t',7,TSTUART)
#define   TST_UART_DIS            _IOWR('t',8,TSTUART)
#define   TST_UART_READ           _IOWR('t',9,TSTUART)
#define   TST_UART_WRITE          _IOWR('t',10,TSTUART)




/*! \page UiModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the ui button states for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

typedef   struct
{
  int8_t   Pressed[BUTTONS];                   //!< Pressed status
}
UI;


/*\endverbatim
 *
 *  \n
 */


/*! \page SoundModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the sound ready flag for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

typedef   struct
{
  int8_t   Status;                       //!< Status
}
SOUND;

/*\endverbatim
 *
 *  \n
 */


/*! \page UsbModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the USB Speed (FULL or HIGH) for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

enum  {
        FULL_SPEED,
        HIGH_SPEED
      };

typedef   struct
{
  int8_t   Speed;
}
USB_SPEED;

/*\endverbatim
 *
 *  \n
 */

/*! \page VmNonvolatileMemory
 *
 *  <b>     VM non volatile Memory </b>
 *
 *  <hr size="1"/>
 *
 *  This struct will be loaded at start up and save when closing down VM
 *
 *  \verbatim
 */

typedef   struct
{
  int8_t   VolumePercent;                //!< System default volume [0..100%]
  int8_t   SleepMinutes;                 //!< System sleep          [0..120min] (0 = ~)
}
NONVOL;

/*\endverbatim
 *
 *  \n
 */


/*
 *                      Motor/OUTPUT Typedef
 */
typedef struct
{
  int32_t TachoCounts;
  int8_t Speed;
  int32_t TachoSensor;
}MOTORDATA;


typedef struct
{
  int8_t   Cmd;
  int8_t   Nos;
  int8_t   Power;
  int32_t  Step1;
  int32_t  Step2;
  int32_t  Step3;
  int8_t   Brake;
} STEPPOWER;


typedef struct
{
  int8_t   Cmd;
  int8_t   Nos;
  int8_t   Power;
  int32_t  Time1;
  int32_t  Time2;
  int32_t  Time3;
  int8_t   Brake;
} TIMEPOWER;

typedef struct
{
  int8_t   Cmd;
  int8_t   Nos;
  int8_t   Speed;
  int32_t  Step1;
  int32_t  Step2;
  int32_t  Step3;
  int8_t   Brake;
} STEPSPEED;

typedef struct
{
  int8_t   Cmd;
  int8_t   Nos;
  int8_t   Speed;
  int32_t  Time1;
  int32_t  Time2;
  int32_t  Time3;
  int8_t   Brake;
} TIMESPEED;

typedef struct
{
  int8_t   Cmd;
  int8_t   Nos;
  int8_t   Speed;
  int16_t  Turn;
  int32_t  Step;
  int8_t   Brake;
} STEPSYNC;

typedef struct
{
  int8_t   Cmd;
  int8_t   Nos;
  int8_t   Speed;
  int16_t  Turn;
  int32_t  Time;
  int8_t   Brake;
} TIMESYNC;
/*
 *                 End of Motor/OUTPUT Typedef
 */


#define     PRINTBUFFERSIZE             160

typedef   enum
{
  NO_BUTTON                     = 0,
  UP_BUTTON                     = 1,
  ENTER_BUTTON                  = 2,
  DOWN_BUTTON                   = 3,
  RIGHT_BUTTON                  = 4,
  LEFT_BUTTON                   = 5,
  BACK_BUTTON                   = 6,
  ANY_BUTTON                    = 7,

  BUTTONTYPES                   = 8
}
BUTTONTYPE;

#endif /* LMS2012_H_ */
