/*
 * kmodtest.c
 *
 *  Created on: Jan 21, 2014
 *      Author: Xander Soldaat <xander@robotc.net>
 */


#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lms2012.h"


// Globals to keep things simple
DEVCON devcon;
IICDAT iic_dat;
IIC  *pIic;

int IicFile = -1;
int DcmFile = -1;
int UartFile = -1;

void usage()
{
	fprintf (stderr, "Usage: kmodtest <port> <write len> <write data> <read len>\n");
	fprintf (stderr, "port      : 0-3\n");
//	fprintf (stderr, "address   : 2-254\n");
	fprintf (stderr, "write len : 1-32\n");
	fprintf (stderr, "write data: up to 32 bytes in hex, space separated\n");
	fprintf (stderr, "read len  : 0-32\n");
	exit(1);
}

void parseData(uint8_t *writedata, char *datastring, uint8_t writelen)
{
	char *token;
	int j;
  for (j = 0, token = datastring; ;j++, datastring = NULL) {
		token = strtok(datastring, " ");
		if (token == NULL)
		{
				break;
		}
		*(writedata + j) = (uint8_t)strtol(token, NULL, 16);
//		printf("byte[%d]: 0x%02X\n", j, *(writedata + j));
  }
  if (j != writelen)
  {
  	fprintf (stderr, "Number of data bytes not equal to specific length\n");
  	usage();
  }
}

int initI2Cport(uint8_t port)
{
	int Index = 0;
	char Buf[5];

	// Open device driver file
	IicFile = open(IIC_DEVICE_NAME, O_RDWR | O_SYNC);

	if (IicFile == -1)
	{
		fprintf(stderr, "Could not open %s\n", IIC_DEVICE_NAME);
		exit(1);
	}


	// Create mmap to kernel module
	pIic = (IIC*)mmap(0, sizeof(IIC), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, IicFile, 0);
	if (pIic == MAP_FAILED)
	{
		fprintf(stderr, "Could not mmap %s\n", IIC_DEVICE_NAME);
		exit(1);
	}

	UartFile    =  open(UART_DEVICE_NAME,O_RDWR | O_SYNC);

	if (UartFile == -1)
	{
		fprintf(stderr, "Could not open %s\n", UART_DEVICE_NAME);
		exit(1);
	}

  // SETUP DRIVERS

	DcmFile = open(DCM_DEVICE_NAME, O_RDWR | O_SYNC);
	if (DcmFile == -1)
	{
		fprintf(stderr, "Could not open %s\n", DCM_DEVICE_NAME);
		exit(1);
	}

  for (Index = 0;Index < INPUTS;Index++)
  { // Initialise pin setup string to do nothing

    Buf[Index]    =  '-';
  }
  Buf[Index]      =  0;

  // insert "pins" in setup string
  Buf[port]     = 0x46;
  write(DcmFile,Buf,INPUTS);

	// Open the files for
  for (Index = 0;Index < INPUTS;Index++)
  { // build setup string for UART and IIC driver

    devcon.Connection[Index]  =  CONN_NONE;
    devcon.Type[Index]        =  TYPE_NONE;
    devcon.Mode[Index]        =  0;
  }

  // Now set it up for the I2C port that we're interested in
  devcon.Connection[port] = CONN_NXT_IIC;
  devcon.Type[port] = TYPE_IIC_UNKNOWN;
  devcon.Mode[port] = 0;

  // write setup string to kernel module
  ioctl(UartFile,UART_SET_CONN,&devcon);
  ioctl(IicFile,IIC_SET_CONN,&devcon);

	return (OK);
}

int writeDataOld(IICDAT *iicdatPtr)
{
	if (ioctl(IicFile, IIC_SETUP, iicdatPtr) != 0)
	{
		fprintf(stderr, "Could not ioctl: IIC_SETUP");
		exit(1);
	}

	return iicdatPtr->Result;
}


int writeData(IICDAT *iicdatPtr)
{
	if (ioctl(IicFile, IIC_WRITE_DATA, iicdatPtr) != 0)
	{
		fprintf(stderr, "Could not ioctl: IIC_WRITE_DATA");
		exit(1);
	}

	return iicdatPtr->Result;
}

int readData(IICDAT *iicdatPtr)
{
	if (ioctl(IicFile, IIC_READ_DATA, iicdatPtr) != 0)
	{
		fprintf(stderr, "Could not ioctl: IIC_READ_DATA");
		exit(1);
	}
	return iicdatPtr->Result;
}

int readStatus(IICDAT *iicdatPtr)
{
	if (ioctl(IicFile, IIC_READ_STATUS, iicdatPtr) != 0)
	{
		fprintf(stderr, "Could not ioctl: IIC_READ_STATUS");
		exit(1);
	}
	return iicdatPtr->Result;
}

int main (int argc, char *argv[])
{
	int i = 0;
	uint8_t port = 0;
	uint8_t writedata[IIC_DATA_LENGTH];
	uint8_t writelen = 0;
	uint8_t readdata[IIC_DATA_LENGTH];
	uint8_t readlen = 0;
	uint8_t result = FAIL;
	int status = 0;

	memset(iic_dat.RdData, 0, IIC_DATA_LENGTH);
	memset(iic_dat.WrData, 0, IIC_DATA_LENGTH);

	if (argc != 5)
	{
		usage();
	}

	// Convert the port
	port = (uint8_t)atoi(argv[1]);
	if (port > vmINPUTS)
	{
		fprintf(stderr, "Bad port number\n");
		usage();
	}

	// The amount of bytes to send
	writelen = (uint8_t)atoi(argv[2]);
	if (writelen < 1 || writelen > IIC_DATA_LENGTH)
	{
		fprintf(stderr, "Bad write len: %d\n", writelen);
		usage();
	}

	// Parse the data passed to the program to be sent to the sensor
	parseData(writedata, argv[3], writelen);

	// The amount of bytes to read
	readlen = (uint8_t)atoi(argv[4]);
	if (readlen > IIC_DATA_LENGTH)
	{
		fprintf(stderr, "Bad read len: %d\n", writelen);
		usage();
	}

	if (initI2Cport(port) != OK)
	{
		fprintf(stderr, "Failed to initialise I2C for port %d\n", port);
		exit(FAIL);
	}
	// Populate the I2C data struct

	iic_dat.Port     =  port;
	iic_dat.Repeat   =  1;
	iic_dat.Time     =  0;
	iic_dat.WrLng    =  writelen;
	iic_dat.RdLng    =  readlen;
	iic_dat.Result   =  BUSY;

	memcpy(&iic_dat.WrData[0], writedata, iic_dat.WrLng);


//	while (writeDataOld(&iic_dat) == BUSY)
//	{
//		switch(iic_dat.Result)
//		{
//			case OK: fprintf(stderr, "O"); break;
//			case BUSY: fprintf(stderr, "B"); break;
//			case FAIL: fprintf(stderr, "F"); break;
//			case STOP: fprintf(stderr, "S"); break;
//		}
//		usleep(1000);
//	}

	writeData(&iic_dat);
	do
	{
		status = readStatus(&iic_dat);
		switch(status)
		{
			case OK: fprintf(stderr, "O"); break;
			case BUSY: fprintf(stderr, "B"); break;
			case FAIL: fprintf(stderr, "F"); break;
			case STOP: fprintf(stderr, "S"); break;
		}
		usleep(1000);
	} while (status == BUSY);
	fprintf(stderr, ":");

	readData(&iic_dat);

	if (iic_dat.RdLng > 0)
	{
		for (i = 0; i < iic_dat.RdLng; i++)
		{
			printf("0x%02X ", (uint8_t)(iic_dat.RdData[i] & 0xFF));
		}
		printf("\n");
	}

	exit(OK);
}
