[![Test](https://github.com/andkae/pciinfo/actions/workflows/test.yml/badge.svg)](https://github.com/andkae/pciinfo/actions/workflows/test.yml)
[![doxygen](https://github.com/andkae/pciinfo/actions/workflows/doxygen.yml/badge.svg)](https://github.com/andkae/pciinfo/actions/workflows/doxygen.yml)


# pciinfo
C library to extract the Linux system filehandle paths of PCI device BARs based
on vendor and device id.


## File listing
 
 | File                              | Remark                  |
 | --------------------------------- | ----------------------- |
 | [pciinfo.c](/pciinfo.c)           | library implementation  |
 | [pciinfo.h](/pciinfo.h)           | library header          |
 | [pciinfo_main.c](/pciinfo_main.c) | tests library functions |


## Functions

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


### pciinfoSetVerboseLevel
Set level of debug messages.

```c
void pciinfoSetVerboseLevel(uint8_t level);
```
