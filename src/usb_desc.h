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


#include <stdint.h>
#include <stddef.h>

// Descriptor types 
enum descriptor_type_t
{
  DEVICE_DESCRIPTOR = 1,
  CONFIGURATION_DESCRIPTOR,
  STRING_DESCRIPTOR,
  INTERFACE_DESCRIPTOR,
  ENDPOINT_DESCRIPTOR,
};

// String descriptor indexes
enum string_descriptor_indexes_t
{
  STRING_DESCRIPTOR_LANGUAGE = 0,
  STRING_DESCRIPTOR_MANUFACTURER,
  STRING_DESCRIPTOR_PRODUCT,
};

// Device descriptor 
typedef struct {
   uint8_t  bLength;
   uint8_t  bDescriptorType;
   uint16_t bcdUSB;
   uint8_t  bDeviceClass;
   uint8_t  bDeviceSubClass;
   uint8_t  bDeviceProtocol;
   uint8_t  bMaxPacketSize0;
   uint16_t idVendor;
   uint16_t idProduct;
   uint16_t bcdDevice;
   uint8_t  iManufacturer;
   uint8_t  iProduct;
   uint8_t  iSerialNumber;
   uint8_t  bNumConfigurations;
} device_descriptor_t;

// Interface descriptor 
typedef struct {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;
  uint8_t bNumEndpoints; 
  uint8_t bInterfaceClass; 
  uint8_t bInterfaceSubClass; 
  uint8_t bInterfaceProtocol;
  uint8_t iInterface; 
} interface_descriptor_t;

// Endpoint descriptor 
typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint8_t  bEndpointAddress; 
  uint8_t  bmAttributes; 
  uint16_t wMaxPacketSize; 
  uint8_t  bInterval; 
} endpoint_descriptor_t;


typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t bcdHID; 
  uint8_t  bCountryCode; 
  uint8_t  bNumDescriptors; 
  uint8_t  bReportDescriptorType; 
  uint16_t  wDescriptorLength;
} hid_descriptor_t;

// Configuration descriptor, EP1 IN and EP1 OUT
typedef struct {
  uint8_t  bLength;
  uint8_t  bDescriptorType;
  uint16_t wTotalLength; 
  uint8_t  bNumInterfaces;
  uint8_t  bConfigurationValue;
  uint8_t  iConfiguration;
  uint8_t  bmAttributes; 
  uint8_t  bMaxPower; 
  interface_descriptor_t interface2_descriptor;
  hid_descriptor_t hid_descriptor;
  endpoint_descriptor_t endpoint_2_in_descriptor;
  interface_descriptor_t interface3_descriptor;
  hid_descriptor_t hid2_descriptor;
  endpoint_descriptor_t endpoint_3_in_descriptor;
} configuration_descriptor_t;

// Device descriptor 
extern __code const device_descriptor_t device_descriptor; 

// Configuration descriptor 
extern __code const configuration_descriptor_t configuration_descriptor; 

// Language, manufacturer, and product device strings
extern __code char * device_strings[3];

extern __code char usbHidReportDescriptor[];

extern __code char usbHidReportDescriptor2[];
