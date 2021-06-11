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
  CFLAGS = -c -O -Wall -Wextra -Wconversion
endif

# linking flags here
ifeq ($(origin LFLAGS), undefined)
  LFLAGS = -Wall -Wextra -I. -lm
endif


all: pciinfo_main

pciinfo_main: pciinfo_main.o pciinfo.o
	$(LINKER) ./obj/pciinfo_main.o ./obj/pciinfo.o $(LFLAGS) -o ./bin/pciinfo_main

pciinfo_main.o: ./src/pciinfo_main.c
	$(CC) $(CFLAGS) ./src/pciinfo_main.c -o ./obj/pciinfo_main.o

pciinfo.o: ./src/pciinfo.c
	$(CC) $(CFLAGS) ./src/pciinfo.c -o ./obj/pciinfo.o

ci: ./src/pciinfo.c
	$(CC) $(CFLAGS) -Werror ./src/pciinfo.c -o ./obj/pciinfo.o

clean:
	rm -f ./obj/*o ./bin/pciinfo_main
