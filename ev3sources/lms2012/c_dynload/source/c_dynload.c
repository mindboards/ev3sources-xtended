/*
 * c_dynload.c
 *
 *  Created on: Jan 21, 2014
 *      Author: Xander Soldaat <xander@robotc.net>
 * 
 * Copyright (C) 2014 Robomatter/National Instruments
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

#include "lms2012.h"
#include  "c_dynload.h"
#include <stdio.h>

#ifdef  DEBUG_DYNLOAD
#define DEBUG_DYNLOAD_VMLOAD
#define DEBUG_DYNLOAD_VMCLOSE
//#define DEBUG_DYNLOAD_VMUPDATE
//#define DEBUG_DYNLOAD_ENTRY
#endif

struct tVirtualMachineInfo virtualMachineInfo;


/*! \page cDynload Dynload
 *  <hr size="1"/>
 *  <b>     dynloadInit ()  </b>
 *
 *- Initialise the dynamic VM loading system
 *
 */
/*! \brief  dynloadInit()
 *
 */
void dynloadInit()
{

#ifdef DEBUG_DYNLOAD
	updateCounter = 0;
#endif

	int index = 0;
	// VM type set to no type at all
	virtualMachineInfo.vmIndex = -1;


	// Set the handle to NULL
	virtualMachineInfo.soHandle = NULL;

	// Initialise all of the pointers to functions
	virtualMachineInfo.vm_exit = NULL;
	virtualMachineInfo.vm_update = NULL;
	virtualMachineInfo.vm_close = NULL;

	for (index = 0; index < DYNLOAD_MAX_ENTRYPOINTS; index++)
	{
		virtualMachineInfo.entryPointFunc[index] = NULL;
	}
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     dynLoadExit ()  </b>
 *
 *- Clean up the dynamic VM loading system\n
 *- Calls the VM's exit_vm() function to tidy up.
 *
 */
/*! \brief  dynLoadExit()
 *
 */
void dynloadVMExit()
{
	// Only execute this if a VM is loaded
	if (virtualMachineInfo.vmIndex >= 0)
	{
		// Execute our entry (or should that be exit) point function,
		// this will clean up anything that requires it, in the VM
		if (virtualMachineInfo.vm_exit != NULL)
			(virtualMachineInfo.vm_exit)();
		else
			SetDispatchStatus(FAILBREAK);

		// Close the .so
		dlclose(virtualMachineInfo.soHandle);

		// Reinitialise everything to 0
		dynloadInit();
	}
}



/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_VMLOAD (VMINDEX)  </b>
 *
 * - Load the selected VM
 *
 *  \param  (DATA8)   VMINDEX - VM Index, Robotc = 0, Labview = 1
 *  \return (DATA8)   RESULT  - OK if VM loaded ok FAIL if it did not.
 */
/*! \brief  opDYNLOAD_VMLOAD(VMINDEX, RESULT) byte code
 *
 */
void dynloadVMLoad()
{
	RESULT res = FAIL;

	char fullVMPath[128];

  char *error = NULL;

  char vmIndex;

  vmInitPointFunc initFunc = NULL;

  vmIndex = *(char *)PrimParPointer();

#ifdef DEBUG_DYNLOAD_VMLOAD
	fprintf(stderr, "DYNLOAD: Called with index: %d\r\n", vmIndex);
#endif

  // A VM has already been loaded.
  if (virtualMachineInfo.vmIndex >= 0)
  {
  	// If the loaded VM is the one we were interested in
  	// do nothing and report OK
  	// Otherwise report failure
  	if (virtualMachineInfo.vmIndex == vmIndex)
  	{
#ifdef DEBUG_DYNLOAD_VMLOAD
  		fprintf(stderr, "DYNLOAD: VM %d already loaded, doing nothing\r\n", vmIndex);
#endif
  		res = OK;
  	}
  	else
  	{
#ifdef DEBUG_DYNLOAD_VMLOAD
  		fprintf(stderr, "DYNLOAD: VM attempting to load %d, VM %d already loaded\r\n", vmIndex, virtualMachineInfo.vmIndex);
#endif
  		res = FAIL;
  	}
		*(DATA8*)PrimParPointer() =  res;
  	return;
  }


  // Select the appropriate shared object to load
	switch(vmIndex)
	{
		case DYNLOAD_VM_ROBOTC:
			sprintf(fullVMPath, "%s/%s", DYNLOAD_VM_SO_PATH, DYNLOAD_VM_ROBOTC_SO_NAME);
			break;
		case DYNLOAD_VM_LABVIEW:
			sprintf(fullVMPath, "%s/%s", DYNLOAD_VM_SO_PATH, DYNLOAD_VM_LABVIEW_SO_NAME);
			break;
		default:
#ifdef DEBUG_DYNLOAD_VMLOAD
			fprintf(stderr, "DYNLOAD: Illegal VM Index %d\r\n", vmIndex);
#endif
			res = FAIL;
			*(DATA8*)PrimParPointer() =  res;

			// Reset the struct and return
			dynloadInit();
			return;
	}

#ifdef DEBUG_DYNLOAD_VMLOAD
	fprintf(stderr, "DYNLOAD: Loading %s: \r\n", fullVMPath);
#endif

	// You can change this to another type of binding.
	// This one only resolves the required symbols.
	virtualMachineInfo.soHandle = dlopen(fullVMPath, RTLD_LAZY);
	if (!virtualMachineInfo.soHandle) {
#ifdef DEBUG_DYNLOAD_VMLOAD
		fprintf(stderr, "DYNLOAD: %s\r\n", dlerror());
#endif
		res = FAIL;
		*(DATA8*)PrimParPointer() =  res;

		// Reset the struct and return
		dynloadInit();
		return;
	}

#ifdef DEBUG_DYNLOAD_VMLOAD
	fprintf(stderr, "DYNLOAD: Loading completed successfully\r\n");
#endif

	dlerror();    /* Clear any existing error */

	// Setup our pointer to function.  The *(void **) construction
	// is taken from the dlopen man page, apparently it's required.

#ifdef DEBUG_DYNLOAD_VMLOAD
	fprintf(stderr, "DYNLOAD: Binding to vm_init: ");
#endif

	*(void **) (&initFunc) = dlsym(virtualMachineInfo.soHandle, "vm_init");

#ifdef DEBUG_DYNLOAD_VMLOAD
	fprintf(stderr, "done\r\n");
#endif

	// If an error occured resolving our entry point, shout about it.
	if ((error = dlerror()) != NULL)
	{
#ifdef DEBUG_DYNLOAD_VMLOAD
		fprintf(stderr, "DYNLOAD: %s\r\n", error);
#endif
		dlclose(virtualMachineInfo.soHandle);
		res = FAIL;
		*(DATA8*)PrimParPointer() =  res;

		// Reset the struct and return
		dynloadInit();
		return;
	}

	// Set the VM type
	virtualMachineInfo.vmIndex = vmIndex;

	// Execute our entry point function, this will set up the internal entry points
	// to the entry points in the struct
	(initFunc)(&virtualMachineInfo);

	res = OK;
	*(DATA8*)PrimParPointer() =  res;
	return;
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_GET_VM </b>
 *
 * - Get the index of the currently loaded VM
 *
 *  \return (DATA8)   RESULT  - VM Index, Robotc = 0, Labview = 1, -1 for no loaded VM
 */
/*! \brief  opDYNLOAD_GET_VM(RESULT) byte code
 *
 */
void dynLoadGetVM()
{
	*(char *)PrimParPointer() = (char)virtualMachineInfo.vmIndex;
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     dynloadUpdateVM </b>
 *
 * - This function get called every 2ms by the scheduler
 *
 */
/*! \brief  dynloadUpdateVM()
 *
 */
void dynloadUpdateVM()
{
#ifdef DEBUG_DYNLOAD_VMUPDATE
	updateCounter++;
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.vm_update != NULL))
	{
#ifdef DEBUG_DYNLOAD_VMUPDATE
		// Print this every 1000 ticks
		if ((updateCounter % 1000) == 0)
		{
			fprintf(stderr, "DYNLOAD: dynloadUpdateVM called: %lu: assigned\r\n", updateCounter);
		}
#endif
		virtualMachineInfo.vm_update();
	}
#ifdef DEBUG_DYNLOAD_VMUPDATE
	else
	{
		// Print this every 1000 ticks
		if ((updateCounter % 1000) == 0)
		{
			fprintf(stderr, "DYNLOAD: dynloadUpdateVM called: %lu: NULL\r\n", updateCounter);
		}
	}
#endif
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     dynloadUpdateVM </b>
 *
 * - This function get called by ProgramEnd() when a program ends
 *
 */
/*! \brief  dynLoadVMClose()
 *
 */
void dynLoadVMClose()
{
#ifdef DEBUG_DYNLOAD_VMUPDATE
	updateCounter++;
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.vm_close != NULL))
	{
#ifdef DEBUG_DYNLOAD_VMCLOSE
		fprintf(stderr, "DYNLOAD: dynLoadVMClose assigned\r\n");
#endif
		virtualMachineInfo.vm_close();
	}
#ifdef DEBUG_DYNLOAD_VMCLOSE
	else
	{
		fprintf(stderr, "DYNLOAD: dynLoadVMClose called NULL\r\n");
	}
#endif
}

/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_ENTRY_0(CMD, DATALEN)  </b>
 *
 *- Execute Entry Point function 0 in Third Party VM                                 \n
 *
 *  \param  (DATA8)    CMD        - Sub command to be executed                       \n
 *  \param  (DATA8)    LENGTH     - Amount of data passed to this opcode  					 \n
 *  \return (DATA8)    LENGTH			- Amount of data returned													 \n
 *  \return (Type varies)  VALUE  - Data from opcode        												 \n
 *
 */
 /*! \brief  opDYNLOAD_ENTRY_0(CMD, DATALEN) byte code
 *
 */
void dynloadEntry_0()
{
#ifdef DEBUG_DYNLOAD_ENTRY
	fprintf(stderr, "DYNLOAD: Entry point %s called\r\n", __func__);
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.entryPointFunc[0] != NULL))
		(virtualMachineInfo.entryPointFunc[0])();
	else
		SetDispatchStatus(FAILBREAK);
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_ENTRY_1(CMD, DATALEN)  </b>
 *
 *- Execute Entry Point function 1 in Third Party VM                                 \n
 *
 *  \param  (DATA8)    CMD        - Sub command to be executed                       \n
 *  \param  (DATA8)    LENGTH     - Amount of data passed to this opcode  					 \n
 *  \return (DATA8)    LENGTH			- Amount of data returned													 \n
 *  \return (Type varies)  VALUE  - Data from opcode        												 \n
 *
 */
 /*! \brief  opDYNLOAD_ENTRY_1(CMD, DATALEN) byte code
 *
 */
void dynloadEntry_1()
{
#ifdef DEBUG_DYNLOAD_ENTRY
	fprintf(stderr, "DYNLOAD: Entry point %s called\r\n", __func__);
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.entryPointFunc[1] != NULL))
		(virtualMachineInfo.entryPointFunc[1])();
	else
		SetDispatchStatus(FAILBREAK);
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_ENTRY_2(CMD, DATALEN)  </b>
 *
 *- Execute Entry Point function 2 in Third Party VM                                 \n
 *
 *  \param  (DATA8)    CMD        - Sub command to be executed                       \n
 *  \param  (DATA8)    LENGTH     - Amount of data passed to this opcode  					 \n
 *  \return (DATA8)    LENGTH			- Amount of data returned													 \n
 *  \return (Type varies)  VALUE  - Data from opcode        												 \n
 *
 */
 /*! \brief  opDYNLOAD_ENTRY_2(CMD, DATALEN) byte code
 *
 */
void dynloadEntry_2()
{
#ifdef DEBUG_DYNLOAD_ENTRY
	fprintf(stderr, "DYNLOAD: Entry point %s called\r\n", __func__);
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.entryPointFunc[2] != NULL))
		(virtualMachineInfo.entryPointFunc[2])();
	else
		SetDispatchStatus(FAILBREAK);
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_ENTRY_3(CMD, DATALEN)  </b>
 *
 *- Execute Entry Point function 3 in Third Party VM                                 \n
 *
 *  \param  (DATA8)    CMD        - Sub command to be executed                       \n
 *  \param  (DATA8)    LENGTH     - Amount of data passed to this opcode  					 \n
 *  \return (DATA8)    LENGTH			- Amount of data returned													 \n
 *  \return (Type varies)  VALUE  - Data from opcode        												 \n
 *
 */
 /*! \brief  opDYNLOAD_ENTRY_3(CMD, DATALEN) byte code
 *
 */
void dynloadEntry_3()
{
#ifdef DEBUG_DYNLOAD_ENTRY
	fprintf(stderr, "DYNLOAD: Entry point %s called\r\n", __func__);
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.entryPointFunc[3] != NULL))
		(virtualMachineInfo.entryPointFunc[3])();
	else
		SetDispatchStatus(FAILBREAK);
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_ENTRY_4(CMD, DATALEN)  </b>
 *
 *- Execute Entry Point function 4 in Third Party VM                                 \n
 *
 *  \param  (DATA8)    CMD        - Sub command to be executed                       \n
 *  \param  (DATA8)    LENGTH     - Amount of data passed to this opcode  					 \n
 *  \return (DATA8)    LENGTH			- Amount of data returned													 \n
 *  \return (Type varies)  VALUE  - Data from opcode        												 \n
 *
 */
 /*! \brief  opDYNLOAD_ENTRY_4(CMD, DATALEN) byte code
 *
 */
void dynloadEntry_4()
{
#ifdef DEBUG_DYNLOAD_ENTRY
	fprintf(stderr, "DYNLOAD: Entry point %s called\r\n", __func__);
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.entryPointFunc[4] != NULL))
		(virtualMachineInfo.entryPointFunc[4])();
	else
		SetDispatchStatus(FAILBREAK);
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_ENTRY_5(CMD, DATALEN)  </b>
 *
 *- Execute Entry Point function 5 in Third Party VM                                 \n
 *
 *  \param  (DATA8)    CMD        - Sub command to be executed                       \n
 *  \param  (DATA8)    LENGTH     - Amount of data passed to this opcode  					 \n
 *  \return (DATA8)    LENGTH			- Amount of data returned													 \n
 *  \return (Type varies)  VALUE  - Data from opcode        												 \n
 *
 */
 /*! \brief  opDYNLOAD_ENTRY_5(CMD, DATALEN) byte code
 *
 */
void dynloadEntry_5()
{
#ifdef DEBUG_DYNLOAD_ENTRY
	fprintf(stderr, "DYNLOAD: Entry point %s called\r\n", __func__);
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.entryPointFunc[5] != NULL))
		(virtualMachineInfo.entryPointFunc[5])();
	else
		SetDispatchStatus(FAILBREAK);
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_ENTRY_6(CMD, DATALEN)  </b>
 *
 *- Execute Entry Point function 6 in Third Party VM                                 \n
 *
 *  \param  (DATA8)    CMD        - Sub command to be executed                       \n
 *  \param  (DATA8)    LENGTH     - Amount of data passed to this opcode  					 \n
 *  \return (DATA8)    LENGTH			- Amount of data returned													 \n
 *  \return (Type varies)  VALUE  - Data from opcode        												 \n
 *
 */
 /*! \brief  opDYNLOAD_ENTRY_6(CMD, DATALEN) byte code
 *
 */
void dynloadEntry_6()
{
#ifdef DEBUG_DYNLOAD_ENTRY
	fprintf(stderr, "DYNLOAD: Entry point %s called\r\n", __func__);
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.entryPointFunc[6] != NULL))
		(virtualMachineInfo.entryPointFunc[6])();
	else
		SetDispatchStatus(FAILBREAK);
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_ENTRY_7(CMD, DATALEN)  </b>
 *
 *- Execute Entry Point function 7 in Third Party VM                                 \n
 *
 *  \param  (DATA8)    CMD        - Sub command to be executed                       \n
 *  \param  (DATA8)    LENGTH     - Amount of data passed to this opcode  					 \n
 *  \return (DATA8)    LENGTH			- Amount of data returned													 \n
 *  \return (Type varies)  VALUE  - Data from opcode        												 \n
 *
 */
 /*! \brief  opDYNLOAD_ENTRY_7(CMD, DATALEN) byte code
 *
 */
void dynloadEntry_7()
{
#ifdef DEBUG_DYNLOAD_ENTRY
	fprintf(stderr, "DYNLOAD: Entry point %s called\r\n", __func__);
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.entryPointFunc[7] != NULL))
		(virtualMachineInfo.entryPointFunc[7])();
	else
		SetDispatchStatus(FAILBREAK);
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_ENTRY_8(CMD, DATALEN)  </b>
 *
 *- Execute Entry Point function 8 in Third Party VM                                 \n
 *
 *  \param  (DATA8)    CMD        - Sub command to be executed                       \n
 *  \param  (DATA8)    LENGTH     - Amount of data passed to this opcode  					 \n
 *  \return (DATA8)    LENGTH			- Amount of data returned													 \n
 *  \return (Type varies)  VALUE  - Data from opcode        												 \n
 *
 */
 /*! \brief  opDYNLOAD_ENTRY_8(CMD, DATALEN) byte code
 *
 */
void dynloadEntry_8()
{
#ifdef DEBUG_DYNLOAD_ENTRY
	fprintf(stderr, "DYNLOAD: Entry point %s called\r\n", __func__);
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.entryPointFunc[8] != NULL))
		(virtualMachineInfo.entryPointFunc[8])();
	else
		SetDispatchStatus(FAILBREAK);
}


/*! \page cDynload
 *  <hr size="1"/>
 *  <b>     opDYNLOAD_ENTRY_9(CMD, DATALEN)  </b>
 *
 *- Execute Entry Point function 9 in Third Party VM                                 \n
 *
 *  \param  (DATA8)    CMD        - Sub command to be executed                       \n
 *  \param  (DATA8)    LENGTH     - Amount of data passed to this opcode  					 \n
 *  \return (DATA8)    LENGTH			- Amount of data returned													 \n
 *  \return (Type varies)  VALUE  - Data from opcode        												 \n
 *
 */
 /*! \brief  opDYNLOAD_ENTRY_9(CMD, DATALEN) byte code
 *
 */
void dynloadEntry_9()
{
#ifdef DEBUG_DYNLOAD_ENTRY
	fprintf(stderr, "DYNLOAD: Entry point %s called\r\n", __func__);
#endif

	if ((virtualMachineInfo.vmIndex >= 0) && (virtualMachineInfo.entryPointFunc[9] != NULL))
		(virtualMachineInfo.entryPointFunc[9])();
	else
		SetDispatchStatus(FAILBREAK);
}
