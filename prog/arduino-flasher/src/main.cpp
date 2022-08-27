#include <config.h>
#include <nRF24LU1P.h>

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

  // Reset and start SPI (p. 153)
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);
  digitalWrite(PIN_PROG, LOW);
  digitalWrite(PIN_PROG, HIGH);
  delay(2000);
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
      writeEnable();
      Serial.write(readFSR());
      break;
    }
    }
  }
}
