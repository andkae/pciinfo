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
CFLAGS=-c -O -Wall -Wextra


all: ./bin/pciinfo_main


./bin/pciinfo_main: ./bin/pciinfo_main.o ./bin/pciinfo.o


./bin/pciinfo_main.o: ./src/pciinfo_main.c
	$(CC) $(CFLAGS) ./src/pciinfo_main.c -o ./bin/pciinfo_main.o

	
./bin/pciinfo.o: ./src/pciinfo.c
	$(CC) $(CFLAGS) ./src/pciinfo.c -o ./bin/pciinfo.o

clean:
	rm ./bin/*o ./bin/pciinfo_main
