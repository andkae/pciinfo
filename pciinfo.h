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
int pciinfoBarSize(char sysPathPciDev[], uint32_t byteSize[]);



/** @brief	looks in linux system path for PCI devices with given
 * 			VendorID and DeviceID
 * 
 * 	@param[in]	vendorID		PCI device vendor
 *	@param[in]	deviceID		PCI device identification
 *	@param[in]	bar				PCI bar
 * 	@param[out]	devicePath		linux system path to PCI device
 * 	@param[in]	devicePathMax	maximal length of 'devicePath'
 * 	@return		1: no match; 0: unique, >0: multiple
 * 	@since 		2017-02-07
 * 	@author		Andreas Kaeberlein
 */
int pciinfoBarPath(char vendorID[], char deviceID[], uint8_t bar, char devicePath[], uint32_t devicePathMax);



/** @brief	extracts physical address of given PCI Bars
 * 
 * 	@param[in]	sysPathPciDev	system path to pci device
 * 	@param[in]	barNo			pci device bar number
 * 	@param[out]	barPhyAddr		physical address
 * 	@return		state
 * 	@since 		2017-05-08
 * 	@author		Kartik Desai
 */
int pciinfoBarPhyAddr(char sysPathPciDev[], uint8_t barNo, uint32_t * barPhyAddr);


//--------------------------------------------------------------
#endif // __PCI_PCIINFO
