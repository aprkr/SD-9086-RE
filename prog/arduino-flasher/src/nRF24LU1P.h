#include <config.h>

// SPI commands
#define WREN 0x06
#define WRDIS 0x04
#define RDSR 0x05
#define WRSR 0x01
#define READ 0x03
#define PROGRAM 0x02
#define ERASE_PAGE 0x52
#define ERASE_ALL 0x62
#define RDFPCR 0x89
#define RDISIP 0x84
#define RDISMB 0x85
#define ENDEBUG 0x86

#define PAGE_SIZE 512
#define PROG_BUF_SIZE 256
#define RDYN_MASK 0b00010000

byte readFSR();
void sendByte(byte data);
void eraseAll();
void waitFlash();
void writeEnable();
void resetInfoPage();
void writeFlash();
void readFlash(uint16_t startAddr, uint16_t numBytes);
void readInfoPage();