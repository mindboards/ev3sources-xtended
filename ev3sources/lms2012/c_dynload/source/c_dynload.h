/*
 * c_dynload.h
 *
 *  Created on: Jan 21, 2014
 *      Author: Xander Soldaat <xander@robotc.net>
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

#define DYNLOAD_VM_ROBOTC_SO_NAME			"robotc.so"
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
};

typedef void (*vmInitPointFunc)(struct tVirtualMachineInfo*);

void dynloadInit();
void dynloadVMExit();
void dynloadVMLoad();
void dynLoadGetVM();

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
