/*
 * c_dynload.c
 *
 *  Created on: Jan 21, 2014
 *      Author: Xander Soldaat <xander@robotc.net>
 */

#include  "c_dynload.h"
#include "lms2012.h"
#include <stdio.h>

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
	int index = 0;
	// VM type set to no type at all
	virtualMachineInfo.vmIndex = -1;

	// Initialise all of the pointers to functions
	virtualMachineInfo.vm_exit = NULL;

	for (index = 0; index < DYNLOAD_MAX_ENTRYPOINTS; index++)
	{
		virtualMachineInfo.entryPointFunc[0] = NULL;
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
	// Execute our entry (or should that be exit) point function,
  // this will clean up anything that requires it, in the VM
	if (virtualMachineInfo.vm_exit != NULL)
		(virtualMachineInfo.vm_exit)();

	// Reinitialise everything to 0
	dynloadInit();

	// Close the .so
	dlclose(virtualMachineInfo.soHandle);
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
/*! \brief  opINPUT_READ byte code
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

#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Called with index: %d\n", vmIndex);
#endif

  // A VM has already been loaded.
  if (virtualMachineInfo.vmIndex < 0)
  {
  	res = FAIL;
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
			sprintf(fullVMPath, "%s/%s", DYNLOAD_VM_SO_PATH, DYNLOAD_VM_ROBOTC_SO_NAME);
			break;
		default:
#ifdef DEBUG_DYNLOAD
			fprintf(stderr, "DYNLOAD: Illegal VM Index %d\n", vmIndex);
#endif
			res = FAIL;
			*(DATA8*)PrimParPointer() =  res;
			return;
	}

#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Loading %s: ", fullVMPath);
#endif

	// You can change this to another type of binding.
	// This one only resolves the required symbols.
	virtualMachineInfo.soHandle = dlopen(fullVMPath, RTLD_LAZY);
	if (!virtualMachineInfo.soHandle) {
#ifdef DEBUG_DYNLOAD
		fprintf(stderr, "DYNLOAD: %s\n", dlerror());
#endif
		res = FAIL;
		*(DATA8*)PrimParPointer() =  res;
		return;
	}

#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "done\n", fullVMPath);
#endif

	dlerror();    /* Clear any existing error */

	// Setup our pointer to function.  The *(void **) construction
	// is taken from the dlopen man page, apparently it's required.

#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Binding to vm_init: ");
#endif

	*(void **) (&initFunc) = dlsym(virtualMachineInfo.soHandle, "vm_init");

#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "done\n");
#endif

	// If an error occured revolving our entry point, shout about it.
	if ((error = dlerror()) != NULL)
	{
#ifdef DEBUG_DYNLOAD
		fprintf(stderr, "DYNLOAD: %s\n", error);
#endif
		dlclose(virtualMachineInfo.soHandle);
		res = FAIL;
		*(DATA8*)PrimParPointer() =  res;
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
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Entry point %s called\n", __func__);
#endif

	if (virtualMachineInfo.entryPointFunc[0] != NULL)
		(virtualMachineInfo.entryPointFunc[0])();
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
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Entry point %s called\n", __func__);
#endif

	if (virtualMachineInfo.entryPointFunc[1] != NULL)
		(virtualMachineInfo.entryPointFunc[1])();
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
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Entry point %s called\n", __func__);
#endif

	if (virtualMachineInfo.entryPointFunc[2] != NULL)
		(virtualMachineInfo.entryPointFunc[2])();
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
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Entry point %s called\n", __func__);
#endif

	if (virtualMachineInfo.entryPointFunc[3] != NULL)
		(virtualMachineInfo.entryPointFunc[3])();
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
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Entry point %s called\n", __func__);
#endif

	if (virtualMachineInfo.entryPointFunc[4] != NULL)
		(virtualMachineInfo.entryPointFunc[4])();
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
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Entry point %s called\n", __func__);
#endif

	if (virtualMachineInfo.entryPointFunc[5] != NULL)
		(virtualMachineInfo.entryPointFunc[5])();
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
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Entry point %s called\n", __func__);
#endif

	if (virtualMachineInfo.entryPointFunc[6] != NULL)
		(virtualMachineInfo.entryPointFunc[6])();
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
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Entry point %s called\n", __func__);
#endif

	if (virtualMachineInfo.entryPointFunc[7] != NULL)
		(virtualMachineInfo.entryPointFunc[7])();
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
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Entry point %s called\n", __func__);
#endif

	if (virtualMachineInfo.entryPointFunc[8] != NULL)
		(virtualMachineInfo.entryPointFunc[8])();
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
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "DYNLOAD: Entry point %s called\n", __func__);
#endif

	if (virtualMachineInfo.entryPointFunc[9] != NULL)
		(virtualMachineInfo.entryPointFunc[9])();
}
