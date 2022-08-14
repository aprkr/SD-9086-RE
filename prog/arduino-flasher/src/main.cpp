#include <Arduino.h>
#include <SPI.h>
#include <config.h>
#include <nRF24LU1P.h>

byte readFSR()
{
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(RDSR);
  byte fsr = SPI.transfer(0);
  digitalWrite(PIN_CS, HIGH);
  return fsr;
}

void waitFlash()
{
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(RDSR);
  while ((SPI.transfer(0) & RDYN_MASK) == RDYN_MASK)
  {
    delayMicroseconds(20);
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
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(WREN);
  digitalWrite(PIN_CS, HIGH);
}

void eraseAll()
{
  writeEnable();
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(ERASE_ALL);
  digitalWrite(PIN_CS, HIGH);
  waitFlash();
}

void setup()
{
  pinMode(PIN_CS, OUTPUT);
  pinMode(PIN_PROG, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);

  digitalWrite(PIN_CS, HIGH);
  digitalWrite(PIN_PROG, HIGH);
  digitalWrite(PIN_RESET, HIGH);

  SPI.begin();
  SPI.beginTransaction(SPISettings(2 * 1000000, MSBFIRST, SPI_MODE0)); // (p. 146), could go to 8 MHz max

  Serial.begin(SERIAL_BAUD);
  Serial.setTimeout(100);

  // Reset and start SPI (p. 153)
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);
  digitalWrite(PIN_PROG, LOW);
  digitalWrite(PIN_PROG, HIGH);
  delay(2000);
}

// Will erase all data, only use if RDISMB is set
void resetInfoPage()
{
  eraseAll();
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(WRSR);
  SPI.transfer(0b00101000); // write enable and enable infopage
  digitalWrite(PIN_CS, HIGH);
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(ERASE_PAGE);
  SPI.transfer(0);
  digitalWrite(PIN_CS, HIGH);
  waitFlash();
  writeEnable();
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(PROGRAM);
  SPI.transfer16(0x000B);                      // Chip id 2 byte address
  byte id[5] = {0x36, 0xD2, 0x85, 0x92, 0xA9}; // 5 byte chip id
  SPI.transfer(id, 5);
  digitalWrite(PIN_CS, HIGH);
  waitFlash();
}

void writeFlash()
{
  eraseAll();
  while (!Serial.available())
  {
  }
  byte numPages = Serial.read(); // 32 for 16kb or 64 for 32kb
  uint16_t pageAddress = 0x00;
  byte buf[BUF_SIZE] = {};
  for (byte i = 0; i < numPages * 2; i++)
  {
    writeEnable();
    digitalWrite(PIN_CS, LOW);
    SPI.transfer(PROGRAM);
    SPI.transfer16(pageAddress);
    for (byte j = 0; j < PAGE_SIZE / 2 / BUF_SIZE; j++)
    {
      while (Serial.available() < BUF_SIZE)
      {
      }
      Serial.readBytes(buf, BUF_SIZE);
      Serial.write(BUF_OK);
      SPI.transfer(buf, BUF_SIZE);
    }
    digitalWrite(PIN_CS, HIGH);
    pageAddress += PAGE_SIZE / 2;
    waitFlash();
  }
}

void readInfoPage()
{
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(WRSR);
  SPI.transfer(0b00001000);
  digitalWrite(PIN_CS, HIGH);
  readFlash(0x00, PAGE_SIZE);
  digitalWrite(PIN_CS, LOW);
  SPI.transfer(WRSR);
  SPI.transfer(0b00000000);
  digitalWrite(PIN_CS, HIGH);
}

void loop()
{
  if (Serial.available() > 0)
  {
    byte command = Serial.read();
    switch (command)
    {
    case READ_SER:
    {
      while (!Serial.available())
      {
      }
      byte numPages = Serial.read();
      for (byte i = 0; i < numPages; i++)
      {
        readFlash(i * PAGE_SIZE, PAGE_SIZE);
      }
      break;
    }
    case PROGRAM_SER: {
      writeFlash();
      break;
    }
    case READ_INFO_SER: {
      readInfoPage();
      break;
    }
    case RESET_INFO_SER: {
      resetInfoPage();
    }
    default: {
      Serial.write(command);
      break;
    }
    }
  }
}
