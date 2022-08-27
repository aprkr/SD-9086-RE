#!/usr/bin/env python

import serial, time, sys, logging
from serial.tools import list_ports

# Setup logging
logging.basicConfig(level=logging.INFO, format='[%(asctime)s.%(msecs)03d]  %(message)s', datefmt="%Y-%m-%d %H:%M:%S")

# Constants
BUFFER_SIZE = 32
PAGE_SIZE = 512


# Verify that we received a command line argument
if len(sys.argv) < 2:
  print('Usage: ./spi-flash.py path-to-firmware.bin')
  quit()

# # Read in the firmware
with open(sys.argv[1], 'rb') as f:
  data = f.read()

# # Zero pad the data to a multiple of 512 bytes
if len(data) % PAGE_SIZE > 0: data += b'\xFF' * (PAGE_SIZE - len(data) % PAGE_SIZE)

numPages = int(len(data) / PAGE_SIZE)
if (numPages != 32 and numPages != 64):
    print("Firmware isn't 16kb or 32kb")
    quit()

logging.info("Looking for Arduino COM port")
comport = None
search = 'USB VID:PID=2341:0043'.lower()
for port in list_ports.comports():
  if search in port[2].lower():
    comport = port[0]
    break
if not comport:
  raise Exception('Failed to find Arduino COM port.')

logging.info('Connecting to Arduino over serial at {0}'.format(comport))
ser = serial.Serial(port=comport, baudrate=115200, timeout=5)
time.sleep(2)

pagesToWrite = []
for x in range(int(numPages)):
  page = data[x*PAGE_SIZE:(x+1)*PAGE_SIZE]
  if (page.count(0xFF) == PAGE_SIZE):
    continue
  else:
    pagesToWrite.append(x)

ser.write(bytearray([0x02, len(pagesToWrite)]))
for x in pagesToWrite:
  ser.write(bytearray([x]))
  page = data[x*PAGE_SIZE:(x+1)*PAGE_SIZE]
  for y in range(int(PAGE_SIZE / BUFFER_SIZE)):
    pageBuf = page[y*BUFFER_SIZE:(y+1)*BUFFER_SIZE]
    ser.write(pageBuf)
    ser.read(1)


ser.write(bytearray([0x01, numPages]))
read = bytes()
success = True
for x in range(PAGE_SIZE * numPages):
    read += ser.read(1)
for x in pagesToWrite:
  if (data[x*PAGE_SIZE:(x+1)*PAGE_SIZE] != read[x*PAGE_SIZE:(x+1)*PAGE_SIZE]):
    success = False
    print("Data mismatch at page: " + str(x))
ser.close()
if (success):
  logging.info("Firmware programming completed successfully")
else:
  logging.info("Firmware programming failed")