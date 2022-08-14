#define SERIAL_BAUD 115200
#define BUF_SIZE 32
#define BUF_OK 0x01
#define READ_SER 0x01
#define PROGRAM_SER 0x02
#define READ_INFO_SER 0x03
#define RESET_INFO_SER 0x04

#define PIN_CS 5
#define PIN_PROG 9
#define PIN_RESET 8 // Not actually going to reset pin in my case, this line is used to power the board