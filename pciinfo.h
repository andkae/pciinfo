/***********************************************************************
 Copyright		: (c) All Rights Reserved
 Company		: Siemens AG
 Address		: Clemens-Winkler-Strasse 3, 09116 Chemnitz
 Telephone		: +49 371 4750

 @author		: Andreas Kaeberlein

 eMail			: andreas.kaeberlein.ext@siemens.com

 @file			: pciinfo.h

 @brief			: function to get infos about pci devices

 @date			: 2017-01-30
***********************************************************************/



//--------------------------------------------------------------
// Define Guard
//--------------------------------------------------------------
#ifndef __PCI_PCIINFO
#define __PCI_PCIINFO


/** Standard libs **/
#include <stdio.h>			// f.e. printf
#include <stdlib.h>			// defines four variables, several macros,
							// and various functions for performing
							// general functions
#include <stdint.h>			// defines fiexd data types, like int8_t...
#include <unistd.h>			// system call wrapper functions such as fork, pipe and I/O primitives (read, write, close, etc.).
#include <string.h>			// string handling functions
#include <stdarg.h>			// variable parameter lists




/** @brief	looks in linux system path for PCI devices with given
 * 			VendorID and DeviceID
 * 
 * 	@param[in]	vendorID		PCI device vendor
 *	@param[in]	deviceID		PCI device identification
 * 	@param[out]	devicePath		linux system path to PCI device
 * 	@param[in]	devicePathMax	maximal length of 'devicePath'
 * 	@return		1: no match; 0: unique, >0: multiple
 * 	@since 		2016-10-13
 * 	@author		Andreas Kaeberlein
 */
int pciinfoFind(char vendorID[], char deviceID[], char devicePath[], uint32_t devicePathMax);



/** @brief	extracts sizes of PCI Bars for given PCI device
 * 
 * 	@param[in]	sysPathPciDev	system path to pci device
 * 	@param[out]	byteSize		six elements, sizes of bar in byte
 * 	@return		state
 * 	@since 		2016-10-14
 * 	@author		Andreas Kaeberlein
 */
int pcimemioBarSize(char sysPathPciDev[], uint32_t byteSize[]);


//--------------------------------------------------------------
#endif // __PCI_PCIINFO
