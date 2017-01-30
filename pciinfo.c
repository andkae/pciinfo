/***********************************************************************
 Copyright		: (c) All Rights Reserved
 Company		: Siemens AG
 Address		: Clemens-Winkler-Strasse 3, 09116 Chemnitz
 Telephone		: +49 371 4750

 @author		: Andreas Kaeberlein

 eMail			: andreas.kaeberlein.ext@siemens.com

 @file			: pcimemio.c

 @brief			: function to get infos about pci devices

 @date			: 2017-01-30
***********************************************************************/



/** Includes **/
#include "pciinfo.h"




 /** globals **/
uint8_t uint8pciinfoVerboseLevel = 0;



/** 
 *  pciinfoVerbosePrint
 * 	-------------------
 */
void pciinfoVerbosePrint(char *template, ...)
{
	/** variables **/
	va_list ap;
	
	/* switch print off */
	if (uint8pciinfoVerboseLevel != 0)
	{
		va_start(ap, template);
		vprintf(template, ap);
		va_end(ap);
	};
}



/** 
 *  setVerboseLevel
 * 	---------------
 */
void pciinfoSetVerboseLevel(uint8_t level)
{
	uint8pciinfoVerboseLevel = level;
}



/** 
 *  pciinfoFind
 * 	-----------
 */
int pciinfoFind(char vendorID[], char deviceID[], char devicePath[], uint32_t devicePathMax)
{
	/** used variables **/
	char 		cmd[256]; 					// command buffer
	char		devPath[256];				// path buffer
	char 		line[1024], line2[1024];	// read buffer
	uint32_t	uint32DevPathIdx;			// match with buf
	uint8_t		uint8FoundDevice;			// device found
	FILE 		*foundVendor;				// system call answer
	FILE		*foundDevice;



	/* function call message */
	pciinfoVerbosePrint("__FUNCTION__ = %s\n", __FUNCTION__);
	
	/* search in system path for pci devices*/
	strcpy(cmd, "grep -irnw /sys/bus/pci/devices/*/vendor -e ");
	strcat(cmd, vendorID);
	foundVendor = popen(cmd, "r");
	
	/* process found device list for device ID */
	uint8FoundDevice  = 0;
	while(fscanf(foundVendor, "%s", line) != EOF) {	
		/* 	extract device path for given vendor id
		 * 	before: /sys/bus/pci/devices/0000:03:0d.0/vendor:1:0x110a
		 * 	after:	/sys/bus/pci/devices/0000:03:0d.0
		 */
		if (strstr(line, "/vendor")-line < (sizeof(devPath)/sizeof(devPath[0]))) {
			uint32DevPathIdx = strstr(line, "/vendor")-line;
		} else {
			uint32DevPathIdx = 0;
			pciinfoVerbosePrint("Not enough memory to store PCI device path...\n");
		}
		strncpy(devPath, line, uint32DevPathIdx);
		devPath[uint32DevPathIdx] = '\0';		// termination character
		
		/*	build command for devide id look up
		 * 	cat /sys/bus/pci/devices/0000:03:0d.0/device
		*/
		strcpy(cmd, "cat ");
		strcat(cmd, devPath);
		strcat(cmd, "/device");
		foundDevice = popen(cmd, "r");
		
		/* process system call response */
		fscanf(foundDevice, "%s", line2);	// read only first line
		if (strcmp(line2, deviceID) == 0) {
			++uint8FoundDevice;				// increment match counter
			strncpy(devicePath, devPath, devicePathMax);
			pciinfoVerbosePrint("vendor=%s/device=%s in '%s'\n", vendorID, line2, devPath);
		}
		
		/* close opened pipe */
		pclose(foundDevice);
	}

	/* close opened pipe */
	pclose(foundVendor);

	/* finish function */
	return uint8FoundDevice-1;
}



/** 
 *  pciinfoBarSize
 * 	--------------
 */
int pciinfoBarSize(char sysPathPciDev[], uint32_t byteSize[])
{
	
	/** used variables **/
	char 	cmd[256], rawCmd[256];		// string for buffering command
	char 	resultLine[256];			// command result one file line
	uint8_t	i, j;						// iterator
	FILE 	*cmdResult;					// stored command result



	/* function call message */
	pciinfoVerbosePrint("__FUNCTION__ = %s\n", __FUNCTION__);
	pciinfoVerbosePrint("device: '%s'\n", sysPathPciDev);
	
	/* init bar data */
	for (i=0; i<6; i++) {
		byteSize[i]	= 0;
	}
	
	/*	build system call request for barsize 
	 * 	ls -la /sys/bus/pci/devices/0000:03:0d.0/resource*
	 */
	if (7+strlen(sysPathPciDev)+9+1+5 < sizeof(rawCmd)/sizeof(rawCmd[0])) {
		strcpy(rawCmd, "ls -la ");			// 7 chars
		strcat(rawCmd, sysPathPciDev);
		strcat(rawCmd, "/resource");		// 9 chars
	} else {
		pciinfoVerbosePrint("To few memory alocated ...abort\n");
		rawCmd[0] = '\0';	
		return -1;
	};
	
	/* iterate over six possible pci bars */
	for (i=0; i<6; i++) {
		/* build final command and execute*/
		sprintf(cmd, "%s%d 2>&1", rawCmd, i);	// 2>&1 redirect stderr
		cmdResult = popen(cmd, "r");
		
		/* process acquired data */
		j = 0;
		while(fscanf(cmdResult, "%s", resultLine) != EOF) {
			/* 	get fifth element in table, is filesize 
			 * 	-rw------- 1 root root 32768 Feb 11 11:40 /sys/bus/pci/devices/0000:03:0d.0/resource0
			 */
			if (j == 4) {
				byteSize[i] = strtoll(resultLine, NULL, 10);
			}
			++j;		// increment counter
		}
		
		/* close file handle */
		pclose(cmdResult);
	}
	
	/* print bar sizes */
	printf("\tSizes:\n");
	printf("\t------\n");
	for (i=0; i<6; i++) {
		printf("\tBAR%d:%I32u Byte\n", i, byteSize[i]);
	}
	
	/* finish function */
	return 0;
}
