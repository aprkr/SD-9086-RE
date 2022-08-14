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
#define RDYN_MASK 0b00010000