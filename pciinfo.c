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



/** globals **/
static uint8_t uint8pciinfoVerboseLevel = 0;



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
    uint16_t uint16DevPathIdx;         /* match with buff */
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
            uint16DevPathIdx = (uint16_t)(strstr(line1, "/vendor") - line1);
        } else {
            uint16DevPathIdx = 0;
            pciinfoVerbosePrint("Not enough memory to store PCI device path...\n");
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
    pciinfoVerbosePrint("__FUNCTION__ = %s\n", __FUNCTION__);

    /* check argument */
    if ( bar > 5 ) {
        pciinfoVerbosePrint("  ERROR:%s: Bar=%i exceeds max of 5\n", __FUNCTION__, bar);
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
    char *devicePath_in = devicePath;   // fix warning: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=84919


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
    devicePath[0] = '\0';
    snprintf(devicePath, (size_t) devicePathMax, "%s%s%d", devicePath_in, "/resource", bar);

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
