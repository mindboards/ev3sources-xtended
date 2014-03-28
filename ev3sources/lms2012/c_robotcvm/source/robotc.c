/*
 * robotc.c
 *
 *  Created on: Jan 21, 2014
 *      Author: Xander Soldaat <xander@robotc.net>
 */

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include "c_dynload.h"
#include "lms2012.h"
#include "robotc.h"

void vm_init(struct tVirtualMachineInfo *virtualMachineInfo)
{
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "ROBOTC: %s called: ", __func__);
#endif

	// Here's where the entry points are configured.
	// Not all of them have to be setup, as long as you know how
	// many there are.
	virtualMachineInfo->entryPointFunc[0] = &robotc_entry_0;
	virtualMachineInfo->entryPointFunc[1] = &robotc_entry_1;
	virtualMachineInfo->entryPointFunc[2] = &robotc_entry_2;
	virtualMachineInfo->entryPointFunc[3] = &robotc_entry_3;

	(virtualMachineInfo->vm_exit) = &vm_exit;
	(virtualMachineInfo->vm_update) = &vm_update;

#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "done.\n");
#endif
}


void vm_exit()
{
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "ROBOTC: %s called\n", __func__);
#endif
	// Here's where the cleanup is done, dynamically allocated memory, buffers, that sort of thing


}


void vm_update()
{
#ifdef DEBUG_DYNLOAD
	static unsigned long updateCounter = 0;
	if ((updateCounter++ % 1000) == 0)
	fprintf(stderr, "ROBOTC: vm_update called: %lu!\r\n", updateCounter);
#endif
}

void robotc_entry_0()
{
  char *inputs   = (char *) PrimParPointer();
  char *outputs  = (char *) PrimParPointer();
  uint16_t dataSize = *(uint16_t *)PrimParPointer();

	void *data = 0;

#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "ROBOTC: %s called. inputs: %d, outputs: %d, datalen: %d\n", __func__, *inputs, *outputs, dataSize);
#endif

	// Only pick a variable off that stack if you should
	if (dataSize > 0)
		data = PrimParPointer();

	// do something here
}

void robotc_entry_1()
{
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "ROBOTC: %s called\n", __func__);
#endif

	// do something here as well, or not
}

void robotc_entry_2()
{
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "ROBOTC: %s called\n", __func__);
#endif
	// you may want to consider doing something here
}

void robotc_entry_3()
{
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "ROBOTC: %s called\n", __func__);
#endif
	// or here
}
