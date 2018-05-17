/***********************************************************************
 Copyright		: (c) All Rights Reserved
 Company		: Siemens AG
 Address		: Clemens-Winkler-Strasse 3, 09116 Chemnitz
 Telephone		: +49 371 4750

 @author		: Andreas Kaeberlein
				
 eMail			: andreas.kaeberlein.ext@siemens.com

 @file			: pciInfoCmdline.c

 @brief			: provides comand line interface for pciinfo.h library

 @date			: 2017-01-30
***********************************************************************/



/** Standard libs **/
#include <stdio.h>			// f.e. printf
#include <stdlib.h>			// defines four variables, several macros,
							// and various functions for performing
							// general functions
#include <stdint.h>			// defines fiexd data types, like int8_t...
#include <unistd.h>			// system call wrapper functions such as fork, pipe and I/O primitives (read, write, close, etc.).
#include <string.h>			// string handling functions
#include <stdarg.h>			// variable parameter lists
#include <fcntl.h>			// manipulate file descriptor
#include <ctype.h>			// used for testing and mapping characters


/** User Libs **/
#include "pciinfo.h"



/** 
 *  Main
 * 	----
 */
int main (int argc, char *argv[])
{
	/** variables **/
	char 		*vendorID, *deviceID, *cmd;
	char		pciSysPath[256];
	uint32_t	barSizes[6];


	
	/* check for root rights */
	if (getuid()) {
		printf("ERROR: root rights required! Try sudo.\n");
		exit(EXIT_FAILURE);
	}	
	
	/* to few arguments */
	if(argc < 3) {
		// main		typ 	0x110A	0x4080
		// argv[0]  [1]   	[2]     [3]
		printf("ERROR:pciinfo_main:%s: Two few arguments.\n", __FUNCTION__);
		printf("\n");
		printf("\n");
		printf("Usage:  %s { cmd } { vendorID } { deviceID }\n", argv[0]);
		printf("  cmd      : command for providing information\n");
		printf("    'f'    : find pci device in unix system path\n");
		printf("    'b'    : get bar sizes of pci device\n");
		printf("  vendorID : vendor identification of PCI device f.e. 0x110A\n");
		printf("  deviceID : device identification of PCI device f.e. 0x4080\n");
		printf("\n");
		printf("\n");
		exit(EXIT_FAILURE);
	}	
	
	/* fill command line arguments in variables */
	cmd			= argv[1];
	vendorID	= argv[2];
	deviceID	= argv[3];
	
	/* process command */
    switch(*cmd) {
		case 'f':
			pciinfoFind(vendorID, deviceID, pciSysPath, sizeof(pciSysPath)/sizeof(pciSysPath[0]));
			printf("Found PCI device system path: '%s'\n\n", pciSysPath);
			break;
		case 'b':
			pciinfoFind(vendorID, deviceID, pciSysPath, sizeof(pciSysPath)/sizeof(pciSysPath[0]));
			pciinfoBarSize(pciSysPath, barSizes);
			break;
		default:
			fprintf(stderr, "\nUnknown command:\t%s { f | b } \n"
				"\tf    : find pci device system path"
				"\tb    : get bar sizes of pci device\n\n",
				argv[0]);
			exit(EXIT_FAILURE);
	}
	
	/* gracefull end */
	exit(EXIT_SUCCESS);

};