# **********************************************************************
#  @copyright	: Siemens AG
#  @license		: GPLv3
#  @author		: Andreas Kaeberlein
#  @address		: Clemens-Winkler-Strasse 3, 09116 Chemnitz
#
#  @maintainer	: Andreas Kaeberlein
#  @telephone	: +49 371 4810-2108
#  @email		: andreas.kaeberlein@siemens.com
#
#  @file		: Makefile
#  @date		: 2017-01-30
#
#  @brief		: Build
#				  builds sources with all dependencies
# **********************************************************************



# select compiler
CC = gcc

# set linker
LINKER = gcc

# set compiler flags
ifeq ($(origin CFLAGS), undefined)
  CFLAGS = -c -O -Wall -Wextra -Wimplicit -Wconversion -I .
endif

# linking flags here
ifeq ($(origin LFLAGS), undefined)
  LFLAGS = -Wall -Wextra -I. -lm
endif


all: pciinfo_main

pciinfo_main: pciinfo_main.o pciinfo.o
	$(LINKER) ./bin/pciinfo_main.o ./bin/pciinfo.o $(LFLAGS) -o ./bin/pciinfo_main

pciinfo_main.o: ./pciinfo_main.c
	$(CC) $(CFLAGS) ./pciinfo_main.c -o ./bin/pciinfo_main.o

pciinfo.o: ./pciinfo.c
	$(CC) $(CFLAGS) ./pciinfo.c -o ./bin/pciinfo.o

ci: ./pciinfo.c
	$(CC) $(CFLAGS) -Werror ./pciinfo.c -o ./bin/pciinfo.o

clean:
	rm -f ./bin/*.o ./bin/pciinfo_main
