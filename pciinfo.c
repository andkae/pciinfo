/******************************************************************************
 @Copyright          : (c) All Rights Reserved
   Company           : Siemens AG
   Address           : Clemens-Winkler-Strasse 3, 09116 Chemnitz
   Telephone         : +49 371 4750

 @Author             : Kaeberlein, Andreas <andreas.kaeberlein.ext@siemens.com>
 @Date               : 2017-01-30 08:51:15
 @File               : pciinfo.c
 @Version            :
 @Brief              : function to get infos about pci devices
 @Last Modified by   : z003su8e
 @Last Modified time : 2018-08-15 11:48:52
*******************************************************************************/


/** Includes **/
/* Standard libs */
#include <stdio.h>    /* e.g. printf */
#include <stdlib.h>   /* defines four variables, several macros, and various
                       * functions for performing general operations */
#include <stdarg.h>   /* variable parameter lists */
#include <stdint.h>   /* defines fixed data types, like int8_t... */
#include <string.h>   /* string handling functions */
#include <unistd.h>   /* system call wrapper functions such as fork, pipe
                       * and I/O primitives (read, write, close, etc.) */

/* User libs */
#include "pciinfo.h"  /* function prototypes */



/** globals **/
volatile uint8_t uint8pciinfoVerboseLevel = 0;



/**
 *  pciinfoVerbosePrint
 *  -------------------
 */
static void pciinfoVerbosePrint(const char *template, ...)
{
	/** variables **/
	va_list ap;

	/* switch print off */
	if (0 != uint8pciinfoVerboseLevel) {
		va_start(ap, template);
		vprintf(template, ap);
		va_end(ap);
	}
}



/**
 *  setVerboseLevel
 *  ---------------
 */
void pciinfoSetVerboseLevel(uint8_t level)
{
	uint8pciinfoVerboseLevel = level;
}



/**
 *  pciinfoFind
 *  -----------
 */
int pciinfoFind(const char vendorID[], const char deviceID[], char devicePath[],
                uint32_t devicePathMax)
{
	/** used variables **/
	char     cmd[256];                 /* command buffer */
	char     devPath[256];             /* path buffer */
	char     line1[1024], line2[1024]; /* read buffer */
	uint32_t uint32DevPathIdx;         /* match with buff */
	uint8_t  uint8FoundDevice;         /* device found */
	FILE     *foundVendor;             /* system call answer */
	FILE     *foundDevice;

	/* function call message */
	pciinfoVerbosePrint("__FUNCTION__ = %s\n", __FUNCTION__);

	/* search in system path for pci devices*/
	strcpy(cmd, "grep -irnw /sys/bus/pci/devices/*/vendor -e ");
	strcat(cmd, vendorID);
	foundVendor = popen(cmd, "r");

	/* process found device list for device ID */
	uint8FoundDevice = 0;
	while (EOF != fscanf(foundVendor, "%s", line1)) {
		/*  extract device path for given vendor id
		 *  before: /sys/bus/pci/devices/0000:03:0d.0/vendor:1:0x110a
		 *  after:  /sys/bus/pci/devices/0000:03:0d.0
		 */
		if ( (strstr(line1, "/vendor") - line1) <
		    ((int)(sizeof(devPath) / sizeof(devPath[0]))) ) {
			uint32DevPathIdx = strstr(line1, "/vendor") - line1;
		} else {
			uint32DevPathIdx = 0;
			pciinfoVerbosePrint("Not enough memory to store PCI device path...\n");
		}
		strncpy(devPath, line1, uint32DevPathIdx);
		devPath[uint32DevPathIdx] = '\0';    /* termination character */

		/*  build command for device id look up
		 *  cat /sys/bus/pci/devices/0000:03:0d.0/device
		 */
		strcpy(cmd, "cat ");
		strcat(cmd, devPath);
		strcat(cmd, "/device");
		foundDevice = popen(cmd, "r");

		/* process system call response */
		if ( fscanf(foundDevice, "%s", line2) ) {    /* read only first line */
		}
		if (0 == strcmp(line2, deviceID)) {
			++uint8FoundDevice;    /* increment match counter */
			strncpy(devicePath, devPath, devicePathMax);
			pciinfoVerbosePrint("vendor=%s/device=%s in '%s'\n", vendorID, line2, devPath);
		}

		/* close opened pipe */
		pclose(foundDevice);
	}

	/* close opened pipe */
	pclose(foundVendor);

	/* finish function */
	return (uint8FoundDevice - 1);
}



/**
 *  pciinfoBarSize
 *  --------------
 */
int pciinfoBarSize(const char sysPathPciDev[], uint32_t byteSize[])
{

	/** used variables **/
	char    cmd[256], rawCmd[256]; /* string for buffering command */
	char    resultLine[256];       /* command result one file line */
	uint8_t i, j;                  /* iterator */
	FILE    *cmdResult;            /* stored command result */

	/* function call message */
	pciinfoVerbosePrint("__FUNCTION__ = %s\n", __FUNCTION__);
	pciinfoVerbosePrint("device: '%s'\n", sysPathPciDev);

	/* init bar data */
	for (i = 0; i < 6; i++) {
		byteSize[i] = 0;
	}

	/*  build system call request for bar size
	 *  ls -la /sys/bus/pci/devices/0000:03:0d.0/resource*
	 */
	if ( (7 + strlen(sysPathPciDev) + 9 + 1 + 5) <
	    (sizeof(rawCmd) / sizeof(rawCmd[0])) ) {
		strcpy(rawCmd, "ls -la ");      /* 7 chars */
		strcat(rawCmd, sysPathPciDev);
		strcat(rawCmd, "/resource");    /* 9 chars */
	} else {
		pciinfoVerbosePrint("To few memory allocated ...abort\n");
		rawCmd[0] = '\0';
		return -1;
	}

	/* iterate over six possible pci bars */
	for (i = 0; i < 6; i++) {
		/* build final command and execute*/
		sprintf(cmd, "%s%d 2>&1", rawCmd, i);    /* 2>&1 redirect stderr */
		cmdResult = popen(cmd, "r");

		/* process acquired data */
		j = 0;
		while (EOF != fscanf(cmdResult, "%s", resultLine)) {
			/*  get fifth element in table, is file size
			 *  -rw------- 1 root root 32768 Feb 11 11:40 /sys/bus/pci/devices/0000:03:0d.0/resource0
			 */
			if (4 == j) {
				byteSize[i] = strtoll(resultLine, NULL, 10);
			}
			++j;    /* increment counter */
		}

		/* close file handle */
		pclose(cmdResult);
	}

	/* print bar sizes */
	pciinfoVerbosePrint("\tSizes:\n");
	pciinfoVerbosePrint("\t------\n");
	for (i = 0; i < 6; i++) {
		pciinfoVerbosePrint("\tBAR%d:%I32u Byte\n", i, byteSize[i]);
	}

	/* finish function */
	return 0;
}



/**
 *  pciinfoBarPath
 *  --------------
 */
int pciinfoBarPath(const char vendorID[], const char deviceID[], uint8_t bar,
                   char devicePath[], uint32_t devicePathMax)
{

	/* function call message */
	pciinfoVerbosePrint("__FUNCTION__ = %s\n", __FUNCTION__);

	/* find pci device */
	if (0 != pciinfoFind(vendorID, deviceID, devicePath, devicePathMax)) {
		pciinfoVerbosePrint(
		    "Unable to find PCI Device with VendorID=%s and DeviceID=%s\n",
		    vendorID, deviceID);
		return -1;
	}

	/* build final path */
	if ((strlen(devicePath) + 10) < devicePathMax) {
		sprintf(devicePath, "%s%s%d", devicePath, "/resource", bar);
	} else {
		pciinfoVerbosePrint("To few memory allocated ...abort\n");
		return -1;
	}

	/* finish function */
	return 0;
}



/**
 *  pciinfoBarPhyAddr
 *  -----------------
 */
int pciinfoBarPhyAddr(const char sysPathPciDev[], uint8_t barNo,
                      uint32_t *barPhyAddr)
{
	/** used variables **/
	char    sysPathTemp[1024] = {'\0'}; /* temporary buffer variable */
	char    line[1023];                 /* read buffer */
	uint8_t index;
	FILE    *fptr;

	/* function call message */
	pciinfoVerbosePrint("__FUNCTION__ = %s\n", __FUNCTION__);
	pciinfoVerbosePrint("device: '%s'\n", sysPathPciDev);

	/*  build path to bar
	*  before: /sys/bus/pci/devices/0000:04:0d.0/
	*  after:  /sys/bus/pci/devices/0000:04:0d.0/resource
	*/
	strncpy(sysPathTemp, sysPathPciDev,
	        (sizeof(sysPathTemp) / sizeof(sysPathTemp[0])));
	strcat(sysPathTemp, "/resource");

	/* open file handle */
	fptr = fopen(sysPathTemp, "r");
	if (NULL == fptr) {
		pciinfoVerbosePrint("ERROR: failed to open '%s'.\n", sysPathTemp);
		return -1;
	}

	/* iterate over all possible pci bars */
	/* line number is the bar number */
	for (index = 0; index < barNo; index++) {
		if (NULL == fgets(line, sizeof(line), fptr)) {
			pciinfoVerbosePrint("ERROR: bar %s not resent\n", barNo);
			fclose(fptr);
			return -1;
		}
	}
	/* first column is the starting address of the bar */
	if ( fscanf(fptr, "%x", barPhyAddr) ) {
	}

	/* close opened pipe */
	fclose(fptr);

	/* finish function */
	return 0;
}
