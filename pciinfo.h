/***********************************************************************
 @copyright     : Siemens AG
 @license       : GPLv3
 @author        : Andreas Kaeberlein
 @address       : Clemens-Winkler-Strasse 3, 09116 Chemnitz

 @maintainer    : Andreas Kaeberlein
 @telephone     : +49 371 4810-2108
 @email         : andreas.kaeberlein@siemens.com

 @file          : pciinfo.h
 @date          : 2017-01-30

 @brief         : pci device into
                  function to acquire information about PCI devices
                  from the linux
***********************************************************************/



//--------------------------------------------------------------
// Define Guard
//--------------------------------------------------------------
#ifndef __PCIINFO_H
#define __PCIINFO_H



/**
 *  @brief  PCI device root dir in file system
 *
 *  looks in Linux system path for PCI devices with given VendorID and DeviceID
 *
 *  @param[in]  char        PCI device vendor
 *  @param[in]  char        PCI device identification
 *  @param[out] char        Linux system path to PCI device
 *  @param[in]  uint32_t    maximal length of 'devicePath'
 *  @return     int         search result
 *  @retval     0           Unique Vendor Device ID match
 *  @retval     -1          no match
 *  @retval     1           multiple matches
 *  @since      2016-10-13
 *  @author     Andreas Kaeberlein
 */
int pciinfoFind(const char vendorID[], const char deviceID[], char devicePath[],
                uint32_t devicePathMax);



/**
 *  @brief  get BAR sizes
 *
 *  extracts sizes of PCI Bars for given PCI device
 *
 *  @param[in]  sysPathPciDev       system path to PCI device
 *  @param[in]  bar                 bar number of requested bar
 *  @param[out] byteSize            bar size in byte
 *  @return     int                 state
 *  @retval     0                   OK
 *  @retval     -1                  FAIL
 *  @since      2016-10-14
 *  @author     Andreas Kaeberlein
 */
int pciinfoBarSize(const char sysPathPciDev[], uint8_t bar, uint32_t *byteSize);



/**
 *  @brief  get file-system path to BAR of PCI device
 *
 *  looks in Linux system path for PCI devices with given VendorID and DeviceID
 *
 *  @param[in]  char        PCI device vendor
 *  @param[in]  char        PCI device identification
 *  @param[out] char        Linux system path to PCI device
 *  @param[in]  uint32_t    maximal length of 'devicePath'
 *  @return     int         search result
 *  @retval     0           Unique Vendor Device ID match
 *  @retval     -1          no match
 *  @retval     1           multiple matches
 *  @since      2017-02-07
 *  @author     Andreas Kaeberlein
 */
int pciinfoBarPath(const char vendorID[], const char deviceID[], uint8_t bar,
                   char devicePath[], uint32_t devicePathMax);



/**
 *  @brief  get physical bar address
 *
 *  extracts physical address of given PCI Bars
 *
 *  @param[in]  char        system path to PCI device
 *  @param[in]  uint8_t     PCI device bar number
 *  @param[out] uint32_t    physical address
 *  @return     int         state
 *  @retval     0           OK
 *  @retval     -1          FAIL
 *  @since      2017-05-08
 *  @author     Kartik Desai
 */
int pciinfoBarPhyAddr(const char sysPathPciDev[], uint8_t barNo,
                      uint32_t *barPhyAddr);



/**
 *  @brief  set verbose level
 *
 *  enabling disabling of pciinfo additional debug information
 *
 *  @param[in]  uint8_t   PCI device vendor
 *  @return     int       state
 *  @retval     0         OK
 *  @since      2018-05-25
 *  @author     Andreas Kaeberlein
 */
void pciinfoSetVerboseLevel(uint8_t level);

//--------------------------------------------------------------
#endif // __PCIINFO_H
