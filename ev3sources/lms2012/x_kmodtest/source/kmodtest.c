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

void usage()
{
	fprintf (stderr, "Usage: kmodtest <port> <address> <write len> <write data> <read len>\n");
	fprintf (stderr, "port      : 0-3\n");
	fprintf (stderr, "address   : 2-254\n");
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
		printf("byte[%d]: 0x%02X\n", j, *(writedata + j));
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

	// Open the files for
  for (Index = 0;Index < INPUTS;Index++)
  { // build setup string for UART and IIC driver

    devcon.Connection[Index]  =  CONN_UNKNOWN;
    devcon.Type[Index]        =  TYPE_UNKNOWN;
    devcon.Mode[Index]        =  0;
  }

  // Now set it up for the I2C port that we're interested in
  devcon.Connection[port] = CONN_NXT_IIC;
  devcon.Type[port] = TYPE_NXT_IIC;
  devcon.Mode[port] = 0;

  // write setup string to kernel module
  ioctl(IicFile,IIC_SET_CONN,&devcon);

	return (OK);
}

int writeData(IICDAT *iicdatPtr)
{
	return (OK);
}

int readData(IICDAT *iicdatPtr)
{
	return (OK);
}

int readStatus(IICDAT *iicdatPtr)
{
	return (OK);
}

int main (int argc, char *argv[])
{
	uint8_t port = 0;
	uint8_t address = 0;
	uint8_t writedata[IIC_DATA_LENGTH];
	uint8_t writelen = 0;
	uint8_t readdata[IIC_DATA_LENGTH];
	uint8_t readlen = 0;
	uint8_t result = FAIL;

	if (argc != 6)
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

	// Convert the address
	address = (uint8_t)atoi(argv[2]);
	if (address % 2)
	{
		fprintf(stderr, "Bad address number\n");
		usage();
	}

	writelen = (uint8_t)atoi(argv[3]);
	if (writelen < 1 || writelen > IIC_DATA_LENGTH)
	{
		fprintf(stderr, "Bad write len: %d\n", writelen);
		usage();
	}

	parseData(writedata, argv[4], writelen);

	readlen = (uint8_t)atoi(argv[5]);
	if (readlen > IIC_DATA_LENGTH)
	{
		fprintf(stderr, "Bad read len: %d\n", writelen);
		usage();
	}

	fprintf (stderr, "Writing %d bytes to port %d, expecting %d bytes in reply\n", writelen, port, readlen);

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

	memcpy(&iic_dat.WrData[0], readdata, iic_dat.WrLng);

	ioctl(IicFile, IIC_WRITE_DATA, &iic_dat);

	while (1 == 1)
	{
		if (result == OK)
		{
			break;
		}
		else if ((result == FAIL) || (result == STOP))
		{
			fprintf (stderr, "There was an error writing to port %d", port);
		}
		// Wait 1 ms
		usleep(1000);
	}


	exit(OK);
}
