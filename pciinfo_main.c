/***********************************************************************
 @copyright     : Siemens AG
 @license       : GPLv3
 @author        : Andreas Kaeberlein
 @address       : Clemens-Winkler-Strasse 3, 09116 Chemnitz

 @maintainer    : Andreas Kaeberlein
 @telephone     : +49 371 4810-2108
 @email         : andreas.kaeberlein@siemens.com

 @file          : pciinfo_main.c
 @date          : 2017-01-30

 @brief         : pciinfo CLI
                  provides comand line interface for pciinfo.h library
***********************************************************************/



/** Standard libs **/
#include <stdio.h>          // f.e. printf
#include <stdlib.h>         // defines four variables, several macros,
                            // and various functions for performing
                            // general functions
#include <stdint.h>         // defines fiexd data types, like int8_t...
#include <unistd.h>         // system call wrapper functions such as fork, pipe and I/O primitives (read, write, close, etc.).
#include <string.h>         // string handling functions
#include <stdarg.h>         // variable parameter lists
#include <fcntl.h>          // manipulate file descriptor
#include <ctype.h>          // used for testing and mapping characters


/** User Libs **/
#include "pciinfo.h"



/**
 *  Main
 *  ----
 */
int main (int argc, char *argv[])
{
    /** variables **/
    char        *vendorID, *deviceID;
    char        pciSysPath[256];
    uint32_t    barSize;
    uint32_t    phyAdr;



    /* check for root rights */
    if (getuid()) {
        printf("ERROR: root rights required! Try sudo.\n");
        exit(EXIT_FAILURE);
    }

    /* to few arguments */
    if(argc < 2) {
        // main     0x110A  0x4080
        // argv[0]  [1]     [2]
        printf("ERROR:pciinfo_main:main: Two few arguments.\n");
        printf("\n");
        printf("\n");
        printf("Usage:  %s { vendorID } { deviceID }\n", argv[0]);
        printf("  vendorID : vendor identification of PCI device f.e. 0x110A\n");
        printf("  deviceID : device identification of PCI device f.e. 0x4080\n");
        printf("\n");
        printf("\n");
        exit(EXIT_FAILURE);
    }

    /* fill command line arguments in variables */
    vendorID    = argv[1];
    deviceID    = argv[2];

    /* acquire system path */
    if ( pciinfoFind(vendorID, deviceID, pciSysPath, sizeof(pciSysPath)/sizeof(pciSysPath[0])) != 0 ) {
        printf("ERROR:pciinfo_main:main: Failed to find device with vid=%s and did=%s\n", vendorID, deviceID);
        exit(EXIT_FAILURE);
    }
    printf("Linux system path for vid=%s and did=%s:\n", vendorID, deviceID);
    printf("  '%s'\n", pciSysPath);
    printf("\n");


    /* get pyhsical addresses */
    printf("Physical PCI Addresses:\n");
    for (uint8_t i=0; i<6; i++) {
        if ( pciinfoBarPhyAddr(pciSysPath, i, &phyAdr) == 0 ) {
            printf("  BAR%i=0x%08x\n", i, phyAdr);
        } else {
            printf("  BAR%i=unused\n", i);
        }
    }
    printf("\n");


    /* get Bar Sizes */
    printf("PCI BAR Sizes:\n");
    for (uint8_t i=0; i<6; i++) {
        pciinfoBarSize(pciSysPath, i, &barSize);
        printf("  BAR%i=%12i Byte\n", i, barSize);
    }
    printf("\n");


    /* gracefull end */
    exit(EXIT_SUCCESS);

};
