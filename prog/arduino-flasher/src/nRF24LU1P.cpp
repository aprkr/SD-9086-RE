#include <nRF24LU1P.h>

byte readFSR()
{
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(RDSR);
  byte fsr = SPI.transfer(0);
  digitalWrite(PIN_CS, HIGH);
  return fsr;
}

void sendByte(byte data) {
    digitalWrite(PIN_CS, LOW);
    SPI.transfer(data);
    digitalWrite(PIN_CS, HIGH);
}

void sendTwoBytes(byte byte1, byte byte2) {
    digitalWrite(PIN_CS, LOW);
    SPI.transfer(byte1);
    SPI.transfer(byte2);
    digitalWrite(PIN_CS, HIGH);
}

void waitFlash()
{
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(RDSR);
  while ((SPI.transfer(0) & RDYN_MASK) == RDYN_MASK)
  {
    delayMicroseconds(10);
  }
  digitalWrite(PIN_CS, HIGH);
}

void readFlash(uint16_t startAddr, uint16_t numBytes)
{
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(READ);
  SPI.transfer16(startAddr);
  for (uint16_t i = 0; i < numBytes; i++)
  {
    Serial.write(SPI.transfer(0));
  }
  digitalWrite(PIN_CS, HIGH);
}

void writeEnable()
{
  sendByte(WREN);
}

void eraseAll()
{
  writeEnable();
  sendByte(ERASE_ALL);
  waitFlash();
}

// Will erase all data, only use if RDISMB is set
void resetInfoPage()
{
  eraseAll();
  sendTwoBytes(WRSR, 0b00101000); // write enable and enable infopage
  sendTwoBytes(ERASE_PAGE, 0);
  waitFlash();
  writeEnable();
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(PROGRAM);
  SPI.transfer16(0x000B);                      // Chip id 2 byte address
  byte id[5] = {0x36, 0xD2, 0x85, 0x92, 0xA9}; // 5 byte chip id, the other one is 0x91812b594c, also first two bytes were 0x5a5a on that one
  SPI.transfer(id, 5);
  digitalWrite(PIN_CS, HIGH);
  sendTwoBytes(WRSR, 0); // disable infopage
  waitFlash();
}

void writeFlash()
{
  while (!Serial.available()) { }
  byte numPages = Serial.read(); // max 32 for 16kb or 64 for 32kb
  byte buf[BUF_SIZE] = {};
  for (byte i = 0; i < numPages; i++) {
    while (!Serial.available()) { }
    byte curPage = Serial.read();
    writeEnable();
    sendTwoBytes(ERASE_PAGE, curPage);
    waitFlash();
    uint16_t pageAddress = curPage * PAGE_SIZE;
    for (int j = 0; j < 2; j++) {
        writeEnable();
        digitalWrite(PIN_CS, LOW);
        SPI.transfer(PROGRAM);
        SPI.transfer16(pageAddress);
        for (byte k = 0; k < PROG_BUF_SIZE / BUF_SIZE; k++) {
            while (Serial.available() < BUF_SIZE) { }
            Serial.readBytes(buf, BUF_SIZE);
            Serial.write(BUF_OK);
            SPI.transfer(buf, BUF_SIZE);
        }
        digitalWrite(PIN_CS, HIGH);
        pageAddress += PROG_BUF_SIZE;
        waitFlash();
    }
  }
}

void readInfoPage()
{
  sendTwoBytes(WRSR, 0b00001000);
  readFlash(0x00, PAGE_SIZE);
  sendTwoBytes(WRSR, 0b00000000);
}