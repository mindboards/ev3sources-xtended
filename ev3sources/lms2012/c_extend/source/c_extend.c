/*
 * extend.c
 *
 *  Created on: Jan 21, 2014
 *      Author: Xander Soldaat <xander@robotc.net>
 */

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include "lms2012.h"
#include "c_input.h"

extern INPUT_GLOBALS InputInstance;


void extendEntry(void)
{

	DATA8   Cmd;
	DATA8 	Layer;
	DATA8 	Port;
	DATA8		Dummy;
	DATA8   *DummyPtr;


	Cmd     =  *(DATA8*)PrimParPointer();
	Layer		=  *(DATA8*)PrimParPointer();
	Port    =  *(DATA8*)PrimParPointer();

	switch(Cmd)
	{
		// datalen, data*
		case EXT_IIC_READ_DATA:
		{
			Dummy = *(DATA8*)PrimParPointer();
			DummyPtr = (DATA8*)PrimParPointer();
			// do something
		}
		break;

		// writelen, wrdata*, readlen
		case EXT_IIC_WRITE_DATA:
		{
			Dummy = *(DATA8*)PrimParPointer();
			DummyPtr = (DATA8*)PrimParPointer();
			Dummy = *(DATA8*)PrimParPointer();
			// do something
		}
		break;

		// status*
		case EXT_IIC_READ_STATUS:
		{
			DummyPtr = (DATA8*)PrimParPointer();
			// do something
		}
		break;
	}
}
