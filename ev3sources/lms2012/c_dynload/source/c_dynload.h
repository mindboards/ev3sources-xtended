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


#ifndef C_DYNLOAD_H_
#define C_DYNLOAD_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>

#define DYNLOAD_MAX_VM_FILENAME      64
#define DYNLOAD_MAX_ENTRYPOINT_NAME  64
#define DYNLOAD_MAX_BUFSIZE         256
#define DYNLOAD_MAX_ENTRYPOINTS		 	 10

#define	DYNLOAD_VM_ROBOTC							0
#define DYNLOAD_VM_LABVIEW						1

#define DYNLOAD_VM_SO_PATH						"/home/root/lms2012/3rdparty-vm"

#define DYNLOAD_VM_ROBOTC_SO_NAME			"librobotc.so"
#define DYNLOAD_VM_LABVIEW_SO_NAME		"libvireobridge.so"


typedef void (*tEntryPointFunc)(void);

#ifdef DEBUG_DYNLOAD
unsigned long updateCounter;
#endif

struct tVirtualMachineInfo
{
	int vmIndex;
	void *soHandle;
  char fileName[DYNLOAD_MAX_VM_FILENAME];
  int entryPoints;
  char entryPointName[DYNLOAD_MAX_ENTRYPOINTS][DYNLOAD_MAX_ENTRYPOINT_NAME];
  tEntryPointFunc entryPointFunc[DYNLOAD_MAX_ENTRYPOINTS];
  tEntryPointFunc vm_exit;
  tEntryPointFunc vm_update;
  tEntryPointFunc vm_close;
};

typedef void (*vmInitPointFunc)(struct tVirtualMachineInfo*);

void dynloadInit();
void dynloadVMExit();
void dynloadVMLoad();
void dynLoadGetVM();
void dynLoadVMClose();

void dynloadUpdateVM();
void dynloadEntry_0();
void dynloadEntry_1();
void dynloadEntry_2();
void dynloadEntry_3();
void dynloadEntry_4();
void dynloadEntry_5();
void dynloadEntry_6();
void dynloadEntry_7();
void dynloadEntry_8();
void dynloadEntry_9();

#endif /* C_DYNLOAD_H_ */
