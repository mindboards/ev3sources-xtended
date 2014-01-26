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
	fprintf(stderr, "ROBOTC: %s called\n", __func__);
#endif

	// Here's where the entry points are configured.
	// Not all of them have to be setup, as long as you know how
	// many there are.
	virtualMachineInfo->entryPointFunc[0] = &robotc_entry_0;
	virtualMachineInfo->entryPointFunc[1] = &robotc_entry_1;
	virtualMachineInfo->entryPointFunc[2] = &robotc_entry_2;
	virtualMachineInfo->entryPointFunc[3] = &robotc_entry_3;

	(virtualMachineInfo->vm_exit) = &vm_exit;

}


void vm_exit()
{
#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "ROBOTC: %s called\n", __func__);
#endif
	// Here's where the cleanup is done, dynamically allocated memory, buffers, that sort of thing
}


void robotc_entry_0()
{
	uint8_t cmd = *(char *) PrimParPointer();
	uint8_t datalen = *(char *) PrimParPointer();
	void *data = 0;

#ifdef DEBUG_DYNLOAD
	fprintf(stderr, "ROBOTC: %s called. cmd: %d, datalen: %d\n", __func__, cmd, datalen);
#endif

	// Only pick a variable off that stack if you should
	if (datalen > 0)
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
