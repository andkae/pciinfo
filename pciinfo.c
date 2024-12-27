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

 @brief         : pci device into
                  function to acquire information about PCI devices
                  from the linux
***********************************************************************/



/** Includes **/
/* Standard libs */
#include <stdarg.h>   /* variable parameter lists */
#include <stdint.h>   /* defines fixed data types, like int8_t... */
#include <stdio.h>    /* e.g. printf */
#include <stdlib.h>   /* defines four variables, several macros, and various
                       * functions for performing general operations */
#include <string.h>   /* string handling functions */
#include <strings.h>  /* for function strcasecmp */
#include <unistd.h>   /* system call wrapper functions such as fork, pipe
                       * and I/O primitives (read, write, close, etc.) */

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
 *  pciinfoBarSize
 *      gets bar size from linux
 */
int pciinfoBarSize(const char sysPathPciDev[], uint8_t bar, uint32_t *byteSize)
{

    /** used variables **/
    const char  charLs[] = "ls -la ";       // build command for dir list
    const char  charBar[] = "/resource";    // prepare acces bar file handle
    char        cmd[1024];                  // string for buffering command
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
    cmd[0] = '\0';

    /*  build system call request for bar size
     *  ls -la /sys/bus/pci/devices/0000:03:0d.0/resource*
     */
    snprintf (  cmd,
                (size_t) sizeof(cmd) / sizeof(cmd[0]),
                "%s%s%s%d 2>&1",
                charLs, sysPathPciDev, charBar, bar
             );

    /* execute command */
    cmdResult = popen(cmd, "r");

    /* process acquired data */
    i = 0;
    while (EOF != fscanf(cmdResult, "%s", resultLine)) {
        /*  get fifth element in table, is file size
         *  -rw------- 1 root root 32768 Feb 11 11:40 /sys/bus/pci/devices/0000:03:0d.0/resource0
         */
        if (4 == i) {
            *byteSize = (uint32_t) strtol(resultLine, NULL, 10);
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
    char charBar[5];


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

    /*  check if exist
     *  SRC: https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c
     */
    if ( access(devicePath, F_OK ) ) {
        pciinfo_printf  (  "  ERROR:%s: File '%s' not found\n",
                           __FUNCTION__, devicePath
                        );
        devicePath[0] = '\0';   // make invalid
        return -1;              // failed
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
    pciinfo_printf("__FUNCTION__ = %s\n", __FUNCTION__);
    pciinfo_printf("device: '%s'\n", sysPathPciDev);

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
        pciinfo_printf("ERROR: failed to open '%s'.\n", sysPathTemp);
        return -1;
    }

    /* iterate over all possible pci bars */
    /* line number is the bar number */
    for (index = 0; index < barNo; index++) {
        if (NULL == fgets(line, sizeof(line), fptr)) {
            pciinfo_printf("ERROR: bar %i not resent\n", barNo);
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
