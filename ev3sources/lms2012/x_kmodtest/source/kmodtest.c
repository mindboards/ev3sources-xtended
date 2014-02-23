/*
 * kmodtest.c
 *
 *  Created on: Jan 21, 2014
 *      Author: Xander Soldaat <xander@robotc.net>
 */

#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lms2012.h"

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
		printf("byte[%d]: %s\n", j, token);
  }
  if (j != writelen)
  {
  	fprintf (stderr, "Number of data bytes not equal to specific length\n");
  	usage();
  }
}

int main (int argc, char *argv[])
{
	uint8_t port = 0;
	uint8_t address = 0;
	uint8_t writedata[IIC_DATA_LENGTH];
	uint8_t writelen = 0;
	uint8_t readdata[IIC_DATA_LENGTH];
	uint8_t readlen = 0;

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

	parseData(&writedata, argv[4], writelen);

	fprintf (stderr, "Writing %d bytes to port %d, expecting %d bytes in reply\n", writelen, port, readlen);
	exit(OK);
}
