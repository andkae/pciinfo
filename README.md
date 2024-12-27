[![Test](https://github.com/andkae/pciinfo/actions/workflows/test.yml/badge.svg)](https://github.com/andkae/pciinfo/actions/workflows/test.yml)
[![doxygen](https://github.com/andkae/pciinfo/actions/workflows/doxygen.yml/badge.svg)](https://github.com/andkae/pciinfo/actions/workflows/doxygen.yml)


# pciinfo
C library to extract the Linux system filehandle paths of PCI device BARs based
on vendor and device id.


## How-to

### Clone
```bash
git clone https://github.com/andkae/pciinfo.git
```


### Build
The [Makefile](/Makefile) builds the repository:
```bash
$ make
gcc -c -O -Wall -Wextra -Wimplicit -Wconversion -I . ./pciinfo_main.c -o ./bin/pciinfo_main.o
gcc -c -O -Wall -Wextra -Wimplicit -Wconversion -I . ./pciinfo.c -o ./bin/pciinfo.o
gcc ./bin/pciinfo_main.o ./bin/pciinfo.o -Wall -Wextra -I. -lm -o ./bin/pciinfo_main
```

The library part itself can be built with:
```bash
gcc -c -O -Wall -Wextra -Wimplicit -Wconversion -I . ./pciinfo.c -o ./bin/pciinfo.o
```


### Enable Debug Output
To enable the debug ```printf``` add the compile switch ```-DPCIINFO_PRINTF_EN``` to the gcc call.


## [API](/pciinfo.h)

### pciinfoFind
Finds the PCI device memory mapped file paths based on a given
vendor and device id.

```c
int pciinfoFind(const char vendorID[], const char deviceID[], char devicePath[], uint32_t devicePathMax)
```


### pciinfoBarSize
Acquires bar sizes from os.

```c
int pciinfoBarSize(const char sysPathPciDev[], uint32_t byteSize[]);
```


### pciinfoBarPath
Extracts from Linux path to memory mapped file of PCI bar.

```c
int pciinfoBarPath(const char vendorID[], const char deviceID[], uint8_t bar, char devicePath[], uint32_t devicePathMax);
```


### pciinfoBarPhyAddr
Gets the PCI bar assigned physical addresses.

```c
int pciinfoBarPhyAddr(const char sysPathPciDev[], uint8_t barNo, uint32_t *barPhyAddr);
```

