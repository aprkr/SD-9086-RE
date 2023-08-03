#include <string.h>
#include <stdbool.h>

#include "usb.h"
#include "radio.h"
#include "nRF24LU1P.h"
#include "private.h"

void radio_irq() __interrupt(9)  __using(1) {
  ien0 = 0x00;
  receive_packet();
  ien0 = 0x80;
}

// Configure addressing on pipe 0
void configure_address(uint8_t * address, uint8_t length)
{
  write_register_byte(EN_RXADDR, ENRX_P0 | 0b10);
  write_register_byte(SETUP_AW, length - 2);
  uint8_t addr[5] = {0xC6, 0xC2, 0xC2, 0xC2, 0xC2};
  write_register(TX_ADDR, &addr[0], 5);
  write_register(RX_ADDR_P0, address, length);
  write_register(RX_ADDR_P1, mouse_address, 5);
}

// Configure MAC layer functionality on pipe 0
void configure_mac(uint8_t feature, uint8_t dynpd, uint8_t en_aa)
{
  write_register_byte(FEATURE, feature);
  write_register_byte(DYNPD, dynpd | 0b10);
  write_register_byte(EN_AA, 0b11);
}

// Configure PHY layer on pipe 0
void configure_phy(uint8_t config, uint8_t rf_setup, uint8_t rx_pw)
{
  write_register_byte(CONFIG, config);
  write_register_byte(RF_SETUP, rf_setup);
  write_register_byte(RX_PW_P0, rx_pw);
}

// Transfer a byte over SPI
uint8_t spi_transfer(uint8_t byte)
{
  RFDAT = byte;
  RFRDY = 0;
  while(!RFRDY);
  return RFDAT;
}

// Write a register over SPI
void spi_write(uint8_t command, uint8_t * buffer, uint8_t length)
{
  int x;
  rfcsn = 0;
  spi_transfer(command);
  for(x = 0; x < length; x++) spi_transfer(buffer[x]);
  rfcsn = 1;
}

// Read a register over SPI
void spi_read(uint8_t command, uint8_t * buffer, uint8_t length)
{
  int x;
  rfcsn = 0;
  spi_transfer(command);
  for(x = 0; x < length; x++) buffer[x] = spi_transfer(0xFF);
  rfcsn = 1;
}

// Write a single byte register over SPI
void write_register_byte(uint8_t reg, uint8_t byte)
{
  write_register(reg, &byte, 1);
}

// Read a single byte register over SPI
uint8_t read_register_byte(uint8_t reg)
{
  uint8_t value;
  read_register(reg, &value, 1);
  return value;
}


void set_channel(uint8_t channel) {
  rfce = 0;
  write_register_byte(RF_CH, channel);
  flush_rx();
  flush_tx();
  rfce = 1;
  return;
}

void set_address() {

  // CE low
  rfce = 0;
  // Configure the address
  configure_address(&keyboard_address[0], 5);

  // Enable dynamic payload length, disable automatic ACK handling
  configure_mac(EN_DPL | EN_ACK_PAY, DPL_P0, ENAA_NONE);

  // 2Mbps data rate, enable RX, 16-bit CRC
  configure_phy(EN_CRC | CRC0 | PRIM_RX | PWR_UP | 0b00110000, RATE_2M, 0);

  // CE high
  rfce = 1;

  // Flush the FIFOs
  flush_rx();
  flush_tx();
}

void init_radio() {
  set_channel(keyboard_channel);
  set_address();
  ien1 = 0b10010;
  for (uint8_t i = 0; i < 15; i++) {
    AESKIN = 0x0;
  }
  AESKIN = AESKEY;
  for (uint8_t i = 0; i < 15; i++) {
    AESD = 0x0;
  }
}

void receive_packet() {
  uint8_t value;

  read_register(R_RX_PL_WID, &value, 1);
  if(value <= 32)
  {
    if (value < 6) {
      read_register(R_RX_PAYLOAD, &in3buf[0], value);
      if (in3buf[0] == 0xF0) {
        in3buf[0] = 1;
        in3bc = value;
      } else {
        in3buf[0] = 2;
        in3bc = value;
      }
      write_register_byte(STATUS, 0b01111110);
      return;
    }
    // Read the payload and write it to EP1
    read_register(R_RX_PAYLOAD, &packet[0], value);
    
    AESD = packet[11];
    AESCS = 0b00010001;
    while ((AESCS & 0b00000001) != 0) { }
    value = AESD;
    value = AESD;
    value = AESD;
    value = AESD;
    value = AESD;

    for (int8_t i = 10; i >= 0; i--) {
      packet[i] ^= AESD;
    }
    if (keyboard_checksum != packet[11]) {
      if (packet[2] != 0 || packet[3] != 0) {
        in3buf[0] = 3;
        in3buf[1] = packet[3];
        in3buf[2] = packet[2];
        in3bc = 3;
      }
      keyboard_checksum = packet[11];
      if (packet[2] == 0 && packet[3] == 0) {
        in3buf[0] = 3;
        in3buf[1] = 0;
        in3buf[2] = 0;
        in3bc = 3;
      }
      in2buf[0] = packet[1];
      in2buf[1] = 0;
      for (uint8_t i = 2; i < 8; i++) {
        in2buf[i] = packet[i + 2];
      }
      in2bc = 8;
    }

    for (uint8_t i = 0; i < 15; i++) {
      AESD = 0x0;
    }
    write_register_byte(STATUS, 0b01111110);
    return;
  }
  else
  {
    // Invalid payload width
    flush_rx();
    return;
  }
}

// Handle a USB radio request
void handle_radio_request(uint8_t request, uint8_t * data)
{
  // Enter the Nordic bootloader
  if(request == LAUNCH_NORDIC_BOOTLOADER)
  {
    nordic_bootloader();
    return;
  }
}

