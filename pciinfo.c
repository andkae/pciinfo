/***********************************************************************
 @copyright     : Siemens AG
 @license       : GPLv3
 @author        : Andreas Kaeberlein
 @address       : Clemens-Winkler-Strasse 3, 09116 Chemnitz

 @maintainer    : Andreas Kaeberlein
 @telephone     : +49 371 4810-2108
 @email         : andreas.kaeberlein@siemens.com

 @file          : pciinfo.c
 @date          : 2017-01-30
 @see           : https://github.com/andkae/pciinfo

 @brief         : pci device file search
                  function to acquire information about PCI devices
                  from the linux
***********************************************************************/



/** Includes **/
/* Standard libs */
#include <stdint.h>   /* defines fixed data types, like int8_t... */
#include <stdio.h>    /* e.g. snprintf */
#include <stdlib.h>   /* defines functions for performing general operations, f.e.
                       * strtol */
#include <string.h>   /* string handling functions */
#include <strings.h>  /* for function strcasecmp */

/* User libs */
#include "pciinfo.h"  /* function prototypes */



/**
 *  @defgroup PCIINFO_PRINTF_EN
 *
 *  redirect pciinfo_printf to printf
 *
 *  @{
 */
#ifdef PCIINFO_PRINTF_EN
    #include <stdio.h>  // allow outputs in unit test
    #define pciinfo_printf(...) printf(__VA_ARGS__)
#else
    #define pciinfo_printf(...)
#endif
/** @} */   // DEBUG



/**
 *  pciinfoFind
 *    finds linux system path based on provided vendor and device id
 */
int pciinfoFind(const char vendorID[], const char deviceID[], char devicePath[],
                uint32_t devicePathMax)
{
    /** used variables **/
    char     cmd[256];                 /* command buffer */
    char     devPath[256];             /* path buffer */
    char     line1[1024], line2[1024]; /* read buffer */
    uint16_t uint16DevPathIdx;         /* match with buff */
    uint8_t  uint8FoundDevice;         /* device found */
    FILE     *foundVendor;             /* system call answer */
    FILE     *foundDevice;

    /* function call message */
    pciinfo_printf("__FUNCTION__ = %s\n", __FUNCTION__);

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
            uint16DevPathIdx = (uint16_t)(strstr(line1, "/vendor") - line1);
        } else {
            uint16DevPathIdx = 0;
            pciinfo_printf("Not enough memory to store PCI device path...\n");
        }
        strncpy(devPath, line1, uint16DevPathIdx);
        devPath[uint16DevPathIdx] = '\0';    /* termination character */

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
        if (0 == strcasecmp(line2, deviceID)) {
            ++uint8FoundDevice;    /* increment match counter */
            strncpy(devicePath, devPath, devicePathMax);
            pciinfo_printf("vendor=%s/device=%s in '%s'\n", vendorID, line2, devPath);
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
 *  pciinfoBarExist
 *    check if bar is present
 */
int pciinfoBarExist(const char sysPathPciDev[])
{
    /** variables **/
    int     intRet = 0;         // bit index indicates bar present y/n
    char    charCmd[2048];      // string for buffering command
    char    charResult[1024];   // command result one file line
    FILE    *cmdResult;         // stored command result


    /* function call message */
    pciinfo_printf("__FUNCTION__ = %s\n", __FUNCTION__);

    /* iterate over 6 possible bars */
    for ( uint8_t i = 0; i < 6; i++ ) {
        /*
         *  build system call request for bar size
         *    ls /sys/bus/pci/devices/0000:03:0d.0/resource*
         */
        snprintf(charCmd, sizeof(charCmd), "ls %s/resource%d 2>/dev/null", sysPathPciDev, i);
        /* help message */
        pciinfo_printf("  INFO:%s:BAR%d:CMD: %s\n", __FUNCTION__, i, charCmd);
        /* execute command */
        cmdResult = popen(charCmd, "r");
        /* parse result */
        while (EOF != fscanf(cmdResult, "%s", charResult)) {
            /* debug */
            pciinfo_printf("  INFO:%s:BAR%d:RESULT: %s\n", __FUNCTION__, i, charResult);
            /* check length */
            if ( 0 < strlen(charResult) ) {
                intRet |= (int) (1<<i);     // bar is present
            }
        }
        /* close file handle */
        pclose(cmdResult);
    }
    /* return */
    return intRet;
}



/**
 *  pciinfoBarSize
 *    gets bar size from linux
 */
int pciinfoBarSize(const char sysPathPciDev[], uint8_t bar, uint32_t *byteSize)
{

    /** used variables **/
    char        charCmd[1024];              // string for buffering command
    char        resultLine[1024];           // command result one file line
    uint32_t    i;                          // iterator
    FILE        *cmdResult;                 // stored command result


    /* function call message */
    pciinfo_printf("__FUNCTION__ = %s\n", __FUNCTION__);

    /* check argument */
    if ( bar > 5 ) {
        pciinfo_printf("  ERROR:%s: Bar=%i exceeds max of 5\n", __FUNCTION__, bar);
        return -1;
    }

    /* init */
    *byteSize = 0;
    charCmd[0] = '\0';

    /*  build system call request for bar size
     *  ls -la /sys/bus/pci/devices/0000:03:0d.0/resource*
     */
    snprintf(charCmd, sizeof(charCmd), "ls -la %s/resource%d 2>/dev/null", sysPathPciDev, bar);

    /* execute command */
    cmdResult = popen(charCmd, "r");

    /* process acquired data */
    i = 0;
    while (EOF != fscanf(cmdResult, "%s", resultLine)) {
        /*  get fifth element in table, is file size
         *  -rw------- 1 root root 32768 Feb 11 11:40 /sys/bus/pci/devices/0000:03:0d.0/resource0
         */
        if (4 == i) {
            *byteSize = (uint32_t) strtol(resultLine, NULL, 10);
            break;  // no further processing of string required after size extaction
        }
        ++i;    // increment counter
    }

    /* close file handle */
    pclose(cmdResult);

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
    /** used variables **/
    char    charBar[5];
    FILE    *fptr;      // pointer to file


    /* function call message */
    pciinfo_printf("__FUNCTION__ = %s\n", __FUNCTION__);

    /* find pci device */
    devicePath[0] = '\0';
    if (0 != pciinfoFind(vendorID, deviceID, devicePath, devicePathMax)) {
        pciinfo_printf  (   "  ERROR:%s: Unable to find PCI Device with VendorID=%s and DeviceID=%s\n",
                            __FUNCTION__, vendorID, deviceID
                        );
        return -1;
    }

    /* build final path */
    charBar[0] = '\0';
    sprintf(charBar, "%d", bar);
    strncat(devicePath, "/resource", (size_t) devicePathMax-1);
    strncat(devicePath, charBar, (size_t) devicePathMax-1);

    /*  check if file could be open */
    fptr = fopen(devicePath, "r");
    if (NULL == fptr) {
        pciinfo_printf  (  "  ERROR:%s: File '%s' not found\n",
                           __FUNCTION__, devicePath
                        );
        devicePath[0] = '\0';   // make invalid
        return -1;              // failed
    }
    fclose(fptr);   // close

    /* finish function */
    return 0;
}



/**
 *  pciinfoBarPhyAdr
 *    get physical address of PCI bar
 */
int pciinfoBarPhyAdr(const char sysPathPciDev[], uint8_t barNo,
                      uint32_t *barPhyAddr)
{
    /** used variables **/
    char    sysPathTemp[1024] = {'\0'}; /* temporary buffer variable */
    char    line[1023];                 /* read buffer */
    uint8_t index;
    FILE    *fptr;

    /* function call message */
    pciinfo_printf("__FUNCTION__ = %s\n", __FUNCTION__);
    pciinfo_printf("  INFO:%s:DEVICE: '%s'.\n", __FUNCTION__, sysPathPciDev);

    /* check for enough memory */
    if ( (strlen(sysPathPciDev) + 10) > sizeof(sysPathTemp) ) {
        pciinfo_printf("  ERROR:%s: Not enough memory\n", __FUNCTION__);
        return -1;
    }

    /*  build path to bar
    *  before: /sys/bus/pci/devices/0000:04:0d.0/
    *  after:  /sys/bus/pci/devices/0000:04:0d.0/resource
    */
    strncpy(sysPathTemp, sysPathPciDev, sizeof(sysPathTemp));
    strncat(sysPathTemp, "/resource",  sizeof(sysPathTemp)-1);

    /* open file handle */
    fptr = fopen(sysPathTemp, "r");
    if (NULL == fptr) {
        pciinfo_printf("  ERROR:%s: failed to open '%s'.\n", __FUNCTION__, sysPathTemp);
        return -1;
    }

    /* iterate over all possible pci bars */
    /* line number is the bar number */
    for (index = 0; index < barNo; index++) {
        if (NULL == fgets(line, sizeof(line), fptr)) {
            pciinfo_printf("  ERROR:%s: bar %i not resent\n", __FUNCTION__, barNo);
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
