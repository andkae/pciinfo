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


/** @brief	PCI device root dir in file system
 * 			
 * 	looks in linux system path for PCI devices with given VendorID and DeviceID
 *
 * 	@param[in]	char				PCI device vendor
 * 	@param[in]	char				PCI device identification
 * 	@param[out]	char				linux system path to PCI device
 * 	@param[in]	uint32_t			maximal length of 'devicePath'
 * 	@return		int					search result
 *	@retval		0					Unique Vendor Device ID match
 *	@retval		-1					no match
 *  @retval		1					multiple matches
 * 	@since 		2016-10-13
 * 	@author		Andreas Kaeberlein
 */
int pciinfoFind(char vendorID[], char deviceID[], char devicePath[], uint32_t devicePathMax);



/** @brief	get BAR sizes
 * 
 * 	extracts sizes of PCI Bars for given PCI device
 *
 * 	@param[in]	char				system path to pci device
 * 	@param[out]	uint32_t			array of uint32_t with six elements, sizes of BAR in byte
 * 	@return		int					state
 *	@retval		0					OK
 *	@retval		-1					FAIL
 * 	@since 		2016-10-14
 * 	@author		Andreas Kaeberlein
 */
int pciinfoBarSize(char sysPathPciDev[], uint32_t byteSize[]);



/** @brief	get filesystem path to BAR of PCI device
 * 			
 * 	looks in linux system path for PCI devices with given VendorID and DeviceID
 * 
 * 	@param[in]	char				PCI device vendor
 *	@param[in]	char				PCI device identification
 * 	@param[out]	char				linux system path to PCI device
 * 	@param[in]	uint32_t			maximal length of 'devicePath'
 * 	@return		int					search result
 *	@retval		0					Unique Vendor Device ID match
 *	@retval		-1					no match
 *  @retval		1					multiple matches
 * 	@since 		2017-02-07
 * 	@author		Andreas Kaeberlein
 */
int pciinfoBarPath(char vendorID[], char deviceID[], uint8_t bar, char devicePath[], uint32_t devicePathMax);



/** @brief	get physical bar address
 * 
 *	extracts physical address of given PCI Bars
 *
 * 	@param[in]	char				system path to pci device
 * 	@param[in]	uint8_t				pci device bar number
 * 	@param[out]	uint32_t			physical address
 * 	@return		int					state
 *	@retval		0					OK
 *	@retval		-1					FAIL
 * 	@since 		2017-05-08
 * 	@author		Kartik Desai
 */
int pciinfoBarPhyAddr(char sysPathPciDev[], uint8_t barNo, uint32_t * barPhyAddr);



/** @brief	set verbose level
 * 
 *  enabling disabling of pciinfo addional debug information
 * 
 * 	@param[in]	uint8_t				PCI device vendor
 * 	@return		int					state
 *  @retval		0					OK
 * 	@since 		2018-05-25
 * 	@author		Andreas Kaeberlein
 */
void pciinfoSetVerboseLevel(uint8_t level);


//--------------------------------------------------------------
#endif // __PCI_PCIINFO
