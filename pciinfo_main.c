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
 @see           : https://github.com/andkae/pciinfo

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
#include <ctype.h>          // toupper
#include <getopt.h>         // CLI parser


/** User Libs **/
#include "pciinfo.h"



/**
 *  @defgroup MSG_LEVEL
 *  Notification level for output messages
 *  @{
 */
#define MSG_LEVEL_BRIEF         0           /**< Brief Message level */
#define MSG_LEVEL_NORM          1           /**< Normal Message level */
#define MSG_LEVEL_VERB          2           /**< Verbose Message Level */
/** @} */   // MSG_LEVEL



/**
 *  @defgroup DEFAULT_SETTINGS
 *  Default settings of the tool
 *  @{
 */
#define DEF_SET_VID         "0x110A"    /**< Siemens Vendor ID */
#define DEF_SET_DID         "0x4080"    /**< NC FPGA Device ID */
/** @} */   // DEFAULT_SETTINGS



/**
 *  @brief touppers
 *
 *  Converts string from lower to uppercase
 *
 *  @param[in,out]  s               string to converted in upper letters
 *  @return         none
 *  @since          January 11, 2025
 */
void touppers(char* s) {
    for ( size_t i = 0; i < strlen(s); ++i ) {
        s[i] = (char) toupper((int) s[i]);
    }
}



/**
 *  @brief main
 *
 *  CLI tool for library demonstration
 *
 *  @return         int             exit state of function
 *  @since          May 5, 2018
 */
int main (int argc, char *argv[])
{
    /** variables **/
    int         intMsgLevel = MSG_LEVEL_NORM;   // CLI: message level
    int8_t      int8CliNoArg;       // no arguments to CLI provided
    int8_t      int8CliPath;        // CLI: output path to linux file handles
    int8_t      int8CliBarExists;   // CLI: discover existing PCI Bars
    int8_t      int8CliBar;         // CLI: output physical bar adresses
    int8_t      int8CliBarNum;      // CLI: select bar to output
    int8_t      int8CliSize;        // CLI: output bar size
    int8_t      int8CliSizeNum;     // CLI: select bar to output

    uint8_t     uint8IterStart;     // help iterator two merge the 'all' programming path with 'dedicated' programming path
    uint8_t     uint8IterStop;      // help iterator two merge the 'all' programming path with 'dedicated' programming path
    char        charVid[8];         // PCI Vendor ID
    char        charDid[8];         // PCI device ID
    char        charPath[1024];     // path to pci file system handle
    int         intExistingBar;     // bit index belongs to bar number
    uint32_t    uint32BarPhyAdr;    // physical bar address
    uint32_t    uint32BarSize;      // bar size in byte


    /* command line parser */
    int opt;                            // switch for parameter
    int arg_index = 0;                  // argument index
    const struct option longopt[] = {   // CLI options
        /* flags */
        { "brief",      no_argument,        &intMsgLevel, MSG_LEVEL_BRIEF },
        { "verbose",    no_argument,        &intMsgLevel, MSG_LEVEL_VERB },
        /* We distinguish them by their indices
         *   required_argument: requires ':'  in shortop
         *   optional_argument: requires '::' in shortop
         */
        {"vendorID",    required_argument,  0,  'v'},   // PCI vendor id
        {"deviceID",    required_argument,  0,  'd'},   // PCI device id
        {"path",        no_argument,        0,  'p'},   // output linux system path to device
        {"exist",       no_argument,        0,  'e'},   // output linux system path to device
        {"adr",         optional_argument,  0,  'a'},   // bar physical addresses
        {"size",        optional_argument,  0,  's'},   // bar sizes
        {"bus",         no_argument,        0,  'b'},   // PCI bus and function
        {"help",        no_argument,        0,  'h'},   // print help
        /* Protection */
        {0,             0,                  0,  0 }     // NULL
    };
    static const char shortopt[] = "v:d:pea::s::bh";



    /* init variables */
    int8CliNoArg = 1;       // no CLI called
    int8CliPath = 0;        // Path output disabled
    int8CliBarExists = 0;   // BAR existence
    int8CliBar = 0;         // BAR: Physical address output disabled
    int8CliSize = 0;        // BAR: Size output disabled
    strncpy(charVid, DEF_SET_VID, sizeof(charVid)); // default Vendor ID
    strncpy(charDid, DEF_SET_DID, sizeof(charDid)); // default Device ID


    /* Parse CLI */
    while (-1 != (opt = getopt_long(argc, argv, shortopt, longopt, &arg_index))) {
        // process parameters
        switch (opt)
        {
            /* Process Flags */
            case 0:
                break;

            /* Argument: 'vendorID=<VID>' */
            case 'v':
                /* check for enough memory */
                if ( (strlen(optarg) + 1) > sizeof(charVid) ) {
                    if ( MSG_LEVEL_NORM <= intMsgLevel ) {
                        printf("[ FAIL ]   not enough static memory\n");
                    }
                    goto ERO_END_L0;
                }
                /* copy */
                strncpy(charVid, optarg, sizeof(charVid));
                break;

            /* Argument: 'deviceID=<VID>' */
            case 'd':
                /* check for enough memory */
                if ( (strlen(optarg) + 1) > sizeof(charDid) ) {
                    if ( MSG_LEVEL_NORM <= intMsgLevel ) {
                        printf("[ FAIL ]   not enough static memory\n");
                    }
                    goto ERO_END_L0;
                }
                /* copy */
                strncpy(charDid, optarg, sizeof(charDid));
                break;

            /* Argument: 'path' */
            case 'p':
                int8CliPath = 1;    // output path
                int8CliNoArg = 0;   // CLI used
                break;

            /* Argument: 'exist' - check if bar exists */
            case 'e':
                int8CliBarExists = 1;   // BARs are existent output
                int8CliNoArg = 0;       // CLI used
                break;

            /* Argument: 'adr' - physical bar address */
            case 'a':
                if ( NULL == optarg ) { // default
                    int8CliBarNum = -1; // output all available physical bar addresses
                } else {
                    if ( (0 > atoi(optarg)) || ( 5 < atoi(optarg)) ) {
                        if ( MSG_LEVEL_NORM <= intMsgLevel ) {
                            printf("[ FAIL ]   PCI Bar number only between 0 and 5\n");
                        }
                        goto ERO_END_L0;
                    }
                    int8CliBarNum = (int8_t) atoi(optarg);

                }
                int8CliNoArg = 0;   // CLI used
                int8CliBar = 1;     // enable phy address bar output
                break;

            /* Argument: 'size' - get barsize in byte */
            case 's':
                if ( NULL == optarg ) { // default
                    int8CliSizeNum = -1;    // output all available physical bar addresses
                } else {
                    if ( (0 > atoi(optarg)) || ( 5 < atoi(optarg)) ) {
                        if ( MSG_LEVEL_NORM <= intMsgLevel ) {
                            printf("[ FAIL ]   PCI Bar number only between 0 and 5\n");
                        }
                        goto ERO_END_L0;
                    }
                    int8CliSizeNum = (int8_t) atoi(optarg);

                }
                int8CliNoArg = 0;   // CLI used
                int8CliSize = 1;    // enable phy address bar output
                break;



            /* Something went wrong */
            default:
                if ( MSG_LEVEL_NORM <= intMsgLevel ) {
                    printf("[ FAIL ]   unrecognized option '-%c' use '--help' for proper args.\n", opt);
                }
                goto ERO_END_L0;
        }
    }


    /* Entry Message */
    if ( MSG_LEVEL_NORM <= intMsgLevel ) {
        printf("[ INFO ]   PCIINFO started\n");
    }

    /* CLI called, otherwise output all info */
    if ( 0 != int8CliNoArg ) {
        int8CliPath = 1;        // output path to device
        int8CliBar = 1;         // enable phy address bar output
        int8CliBarNum = -1;     // output all available bars
        int8CliSize = 1;        // enable size output
        int8CliSizeNum = -1;    // output all availabe sizes
        int8CliBarExists = 1;   // output bar existence
    }

    /* find path based on VID/DID */
    charPath[0] = '\0';
    if ( 0 != pciinfoFind(charVid, charDid, charPath, sizeof(charPath)) ) {
        if ( MSG_LEVEL_NORM <= intMsgLevel ) {
            printf("[ FAIL ]   PCI device with VID=%s/DID=%s not found\n", charVid, charDid);
        }
        goto ERO_END_L0;
    }
    if ( MSG_LEVEL_NORM <= intMsgLevel ) {
        printf("[ OKAY ]   FOUND PCI device with VID=%s/DID=%s\n", charVid, charDid);
    }

    /* prepare for bash variable output */
    touppers(charVid);  // convert to uper case
    touppers(charDid);

    /* PCI Device Path */
    if ( 0 != int8CliPath ) {
        if ( MSG_LEVEL_NORM <= intMsgLevel ) {
            printf("[ INFO ]   PCI Device File Handle\n");
            printf("             %s\n", charPath);
        } else {
            printf("PCIINFO_%s_%s_PATH=\"%s\"\n", charVid+2, charDid+2, charPath);
        }
    }

    /* BAR existences */
    intExistingBar = pciinfoBarExist(charPath); // discover bars
    if ( 0 != int8CliBarExists ) {
        if ( MSG_LEVEL_NORM <= intMsgLevel ) {
            if ( 0 == intExistingBar ) {
                printf("[ WARN ]   No PCI BARs found\n");
            } else {
                printf("[ INFO ]   Available PCI BARs:\n");
                printf("             ");
                for ( uint8_t i = 0; i < 6; i++ ) {
                    if ( 0 != (intExistingBar & (1<<i)) ) {
                        printf("BAR%d ", i);
                    }
                }
                printf("\n");
            }
        } else {
            for ( uint8_t i = 0; i < 6; i++ ) {
                if ( MSG_LEVEL_BRIEF <= intMsgLevel ) {
                    if ( 0 != (intExistingBar & (1<<i)) ) {
                        printf("PCIINFO_%s_%s_BAR_%d_EXISTS=y\n", charVid+2, charDid+2, i);
                    } else {
                        printf("PCIINFO_%s_%s_BAR_%d_EXISTS=n\n", charVid+2, charDid+2, i);
                    }
                }
            }
        }
    }

    /* BAR physical addresses, '-a' | '--adr=<barNum>' */
    if ( 0 != int8CliBar ) {
        /* 'all' or 'dedicated' BAR */
        if ( -1 == int8CliBarNum ) {
            uint8IterStart = 0;
            uint8IterStop = 6;
        } else {
            uint8IterStart = (uint8_t) int8CliBarNum;
            uint8IterStop = (uint8_t) (int8CliBarNum + 1);
        }
        /* entry message */
        if ( MSG_LEVEL_NORM <= intMsgLevel ) {
            printf("[ INFO ]   Physical BAR adress:\n");
        }
        /* print bars */
        for ( uint8_t i = uint8IterStart; i < uint8IterStop; i++ ) {
            if ( 0 != (intExistingBar & (1<<i)) ) {
                /* OK: get adr */
                if ( 0 == pciinfoBarPhyAdr(charPath, i, &uint32BarPhyAdr) ) {
                    if ( MSG_LEVEL_NORM <= intMsgLevel ) {
                        printf("             BAR%d = 0x%x\n", i, uint32BarPhyAdr);
                    } else if ( MSG_LEVEL_BRIEF <= intMsgLevel ) {
                        printf("PCIINFO_%s_%s_BAR_%d_ADR=0x%x\n", charVid+2, charDid+2, i, uint32BarPhyAdr);
                    }
                /* FAIL: get adr */
                } else {
                    if ( MSG_LEVEL_NORM <= intMsgLevel ) {
                        printf("             BAR%d = NaN\n", i);
                    } else if ( MSG_LEVEL_BRIEF <= intMsgLevel ) {
                        printf("PCIINFO_%s_%s_BAR_%d_ADR=NaN\n", charVid+2, charDid+2, i);
                    }
                }
            }
        }
    }

    /* BAR physical sizes, '-s' | '--size=<barNum>' */
    if ( 0 != int8CliSize ) {
        /* 'all' or 'dedicated' BAR */
        if ( -1 == int8CliSizeNum ) {
            uint8IterStart = 0;
            uint8IterStop = 6;
        } else {
            uint8IterStart = (uint8_t) int8CliBarNum;
            uint8IterStop = (uint8_t) (int8CliBarNum + 1);
        }
        /* entry message */
        if ( MSG_LEVEL_NORM <= intMsgLevel ) {
            printf("[ INFO ]   BAR size in byte:\n");
        }
        /* print bars */
        for ( uint8_t i = uint8IterStart; i < uint8IterStop; i++ ) {
            if ( 0 != (intExistingBar & (1<<i)) ) {
                /* OK: get adr */
                if ( 0 == pciinfoBarSize(charPath, i, &uint32BarSize) ) {
                    if ( MSG_LEVEL_NORM <= intMsgLevel ) {
                        printf("             BAR%d = %d\n", i, uint32BarSize);
                    } else if ( MSG_LEVEL_BRIEF <= intMsgLevel ) {
                        printf("PCIINFO_%s_%s_BAR_%d_SIZE=%d\n", charVid+2, charDid+2, i, uint32BarSize);
                    }
                /* FAIL: get adr */
                } else {
                    if ( MSG_LEVEL_NORM <= intMsgLevel ) {
                        printf("             BAR%d = NaN\n", i);
                    } else if ( MSG_LEVEL_BRIEF <= intMsgLevel ) {
                        printf("PCIINFO_%s_%s_BAR_%d_SIZE=NaN\n", charVid+2, charDid+2, i);
                    }
                }
            }
        }
    }






    /* gracefull end */
    goto GD_END_L0; // avoid compile warning
    GD_END_L0:
        if ( MSG_LEVEL_NORM <= intMsgLevel ) {
            printf("[ OKAY ]   ended normally\n");
        }
        exit(EXIT_SUCCESS);

    /* Error L0 End */
    goto ERO_END_L0;
    ERO_END_L0:
        if ( MSG_LEVEL_NORM <= intMsgLevel ) {
            printf("[ FAIL ]   ended abnormally :-(\n");
        }
        exit(EXIT_FAILURE);




//    /* acquire bar path */
//    printf("Bar Paths:\n");
//    for (uint8_t i=0; i<6; i++) {
//        if ( 0 == pciinfoBarPath(vendorID, deviceID, i, pciSysPath, sizeof(pciSysPath)/sizeof(pciSysPath[0])) ) {
//            printf("  BAR%i: '%s'\n", i, pciSysPath);
//        } else {
//            printf("  BAR%i: not defined\n", i);
//        }
//    }
//    printf("\n");
//
//
//    /* gracefull end */
//    exit(EXIT_SUCCESS);

};
