/*
  Copyright (C) 2016 Bastille Networks

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "usb_desc.h"

// Device descriptor 
__code const device_descriptor_t device_descriptor = 
{
  .bLength            = 18,     // Size of this struct
  .bDescriptorType    = DEVICE_DESCRIPTOR,
  .bcdUSB             = 0x0110, // USB 2.0
  .bDeviceClass       = 0x00,
  .bDeviceSubClass    = 0x00,
  .bDeviceProtocol    = 0x00,
  .bMaxPacketSize0    = 64,     // EP0 max packet size
  .idVendor           = 0x1915, // Nordic Semiconductor
  .idProduct          = 0x0102, // Nordic bootloader product ID incremebted by 1
  .bcdDevice          = 0x0001, // Device version number 
  .iManufacturer      = STRING_DESCRIPTOR_MANUFACTURER,
  .iProduct           = STRING_DESCRIPTOR_PRODUCT,
  .iSerialNumber      = 0,
  .bNumConfigurations = 1,      // Configuration count
};

// Configuration descriptor 
__code const configuration_descriptor_t configuration_descriptor = 
{
  .bLength                = 9,     // Size of the configuration descriptor
  .bDescriptorType        = CONFIGURATION_DESCRIPTOR,
  .wTotalLength           = sizeof(configuration_descriptor),    // Total size of the configuration descriptor and EP/interface descriptors 
  .bNumInterfaces         = 1,     // Interface count
  .bConfigurationValue    = 1,     // Configuration identifer
  .iConfiguration         = 0,
  .bmAttributes           = 0xa0,  // Bus powered
  .bMaxPower              = 100,   // Max power of 100*2mA = 200mA 
  .interface2_descriptor = 
  {
    .bLength            = 9,    // Size of the interface descriptor 
    .bDescriptorType    = INTERFACE_DESCRIPTOR,
    .bInterfaceNumber   = 1,    // Interface index
    .bAlternateSetting  = 0,   
    .bNumEndpoints      = 1,    // EP2 IN
    .bInterfaceClass    = 0x3, // HID interface class
    .bInterfaceSubClass = 0x0, // No interface subclass
    .bInterfaceProtocol = 0x1,
    .iInterface         = 0,
  },
  .hid_descriptor = 
  {
    .bLength            = 9,    // Size of the interface descriptor 
    .bDescriptorType    = 0x21,
    .bcdHID             = 0x111,    
    .bCountryCode       = 0,   
    .bNumDescriptors    = 0x1, 
    .bReportDescriptorType    = 0x22, 
    .wDescriptorLength  = sizeof(usbHidReportDescriptor),
  },
  .endpoint_2_in_descriptor = 
  {
    .bLength            = 7,    // Size of the endpoint descriptor 
    .bDescriptorType    = ENDPOINT_DESCRIPTOR,
    .bEndpointAddress   = 0b10000010, // EP2 IN
    .bmAttributes       = 0b11, // Interrupt EP
    .wMaxPacketSize     = 8,   // 8 byte packet buffer
    .bInterval          = 10,
  },
};

// String descriptor values 
__code char * device_strings[3] = 
{
  "\x04\x09",          // Language (EN-US)
  "RFStorm",           // Manufacturer
  "Research Firmware", // Product
};

__code char usbHidReportDescriptor[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    
    0x19, 0xE0,                    
    0x29, 0xe7,                    
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8), fail here (only 25 bytes?)
    0x81, 0x01,                    //   INPUT (Cnst)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0,                          // END_COLLECTION
};
