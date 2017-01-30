# ***********************************************************************
# Copyright		: (c) All Rights Reserved
# Company		: Siemens AG
# Address		: Clemens-Winkler-Strasse 3, 09116 Chemnitz
# Telephone		: +49 371 4750
# 
# @author		: Andreas Kaeberlein
#			
# eMail			: andreas.kaeberlein.ext@siemens.com
#
# @file			: Makefile
#
# @brief		: build project
#
# @date			: 2017-01-30
# *********************************************************************/



# select compiler
CC=gcc

# set compiler flags
CFLAGS=-c -Wall


all: pciInfoCmdline


pciInfoCmdline: pciInfoCmdline.o pciinfo.o


pciInfoCmdline.o: pciInfoCmdline.c
	$(CC) $(CFLAGS) pciInfoCmdline.c

	
pciinfo.o: pciinfo.c
	$(CC) $(CFLAGS) pciinfo.c

clean:
	rm *o pciInfoCmdline
