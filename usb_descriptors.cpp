#include "tusb.h"

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+
uint8_t const desc_hid_report[] = {
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x05, // USAGE (Gamepad)
    0xa1, 0x01, // COLLECTION (Application)

    // 8-bit X, Y, Z, RZ, RX, and RY values
    0x05, 0x01, //   USAGE_PAGE (Generic Desktop)
    0x09, 0x30, //   USAGE (X)
    0x09, 0x31, //   USAGE (Y)
    0x09, 0x32, //   USAGE (Z)
    0x09, 0x35, //   USAGE (Rz)
    0x09, 0x33, //   USAGE (Rx)
    0x09, 0x34, //   USAGE (Ry)
    0x15, 0x81, // LOGICAL_MINIMUM (-127)
    0x25, 0x7F, // LOGICAL_MAXIMUM (127)
    0x75, 0x08, //   REPORT_SIZE (8)
    0x95, 0x06, //   REPORT_COUNT (6)
    0x81, 0x02, //   INPUT (Data,Var,Abs)

    // 4-bit hat (D-Pad) + padding
    0x05, 0x01,       //   USAGE_PAGE (Generic Desktop)
    0x09, 0x39,       //   USAGE (Hat switch)
    0x15, 0x00,       //   LOGICAL_MINIMUM (0)
    0x25, 0x07,       //   LOGICAL_MAXIMUM (7)
    0x35, 0x00,       //   PHYSICAL_MINIMUM (0)
    0x46, 0x3B, 0x01, //   PHYSICAL_MAXIMUM (315)
    0x75, 0x04,       //   REPORT_SIZE (4)
    0x95, 0x01,       //   REPORT_COUNT (1)
    0x65, 0x14,       //   UNIT (Eng Rot:Angular Pos)
    0x81, 0x42,       //   INPUT (Data,Var,Abs,Null)

    // Padding to align the rest of the report
    0x75, 0x04, //   REPORT_SIZE (4)
    0x95, 0x01, //   REPORT_COUNT (1)
    0x81, 0x03, //   INPUT (Cnst,Var,Abs)

    // 32-bit Buttons
    0x05, 0x09, //   USAGE_PAGE (Button)
    0x19, 0x01, //   USAGE_MINIMUM (Button 1)
    0x29, 0x20, //   USAGE_MAXIMUM (Button 32)
    0x15, 0x00, //   LOGICAL_MINIMUM (0)
    0x25, 0x01, //   LOGICAL_MAXIMUM (1)
    0x75, 0x01, //   REPORT_SIZE (1)
    0x95, 0x20, //   REPORT_COUNT (32)
    0x81, 0x02, //   INPUT (Data,Var,Abs)

    0xc0 // END_COLLECTION
};

// Callback function to return the HID report descriptor
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
  (void)instance;
  return desc_hid_report;
}

//--------------------------------------------------------------------+
// Device Descriptor
//--------------------------------------------------------------------+
uint8_t const desc_device[] = {
    0x12,                   // bLength
    TUSB_DESC_DEVICE,       // bDescriptorType
    0x00, 0x02,             // bcdUSB 2.0
    0x00,                   // bDeviceClass (use class in interface descriptor)
    0x00,                   // bDeviceSubClass
    0x00,                   // bDeviceProtocol
    CFG_TUD_ENDPOINT0_SIZE, // bMaxPacketSize0
    0xCA, 0xFE,             // idVendor (Replace with a real Vendor ID if needed)
    0xCA, 0x01,             // idProduct (Replace with a real Product ID)
    0x00, 0x01,             // bcdDevice
    0x01,                   // iManufacturer
    0x02,                   // iProduct
    0x03,                   // iSerialNumber
    0x01                    // bNumConfigurations
};

// Callback function to return the device descriptor
uint8_t const *tud_descriptor_device_cb(void)
{
  return desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
uint8_t const desc_configuration[] = {
    0x09,                    // bLength
    TUSB_DESC_CONFIGURATION, // bDescriptorType
    0x22, 0x00,              // wTotalLength (34 bytes)
    0x01,                    // bNumInterfaces
    0x01,                    // bConfigurationValue
    0x00,                    // iConfiguration
    0xA0,                    // bmAttributes (Bus Powered, Remote Wakeup)
    0x32,                    // bMaxPower (100 mA)

    // Interface Descriptor
    0x09,           // bLength
    0x04,           // bDescriptorType (Interface)
    0x00,           // bInterfaceNumber
    0x00,           // bAlternateSetting
    0x01,           // bNumEndpoints
    TUSB_CLASS_HID, // bInterfaceClass (HID)
    0x00,           // bInterfaceSubClass (No Boot)
    0x00,           // bInterfaceProtocol (None)
    0x00,           // iInterface

    // HID Descriptor
    0x09,                          // bLength
    0x21,                          // bDescriptorType (HID)
    0x11, 0x01,                    // bcdHID (HID v1.11)
    0x00,                          // bCountryCode
    0x01,                          // bNumDescriptors
    0x22,                          // bDescriptorType (Report)
    sizeof(desc_hid_report), 0x00, // wDescriptorLength

    // Endpoint Descriptor
    0x07,               // bLength
    TUSB_DESC_ENDPOINT, // bDescriptorType
    0x81,               // bEndpointAddress (IN, EP1)
    0x03,               // bmAttributes (Interrupt)
    0x08, 0x00,         // wMaxPacketSize (8 bytes)
    0x0A                // bInterval (10ms)
};

// Callback function to return the configuration descriptor
uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
  (void)index;
  return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+
char const *string_desc_arr[] = {
    (const char[]){0x09, 0x04}, // Supported language: English (0x0409)
    "Gamepad Manufacturer",     // Manufacturer
    "Simple Gamepad",           // Product
    "123456",                   // Serial Number
};

static uint16_t _desc_str[32];

// Callback function to return the string descriptor
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void)langid;

  if (index == 0)
  {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 + 2);
    return _desc_str;
  }

  if (index >= sizeof(string_desc_arr) / sizeof(string_desc_arr[0]))
    return NULL;

  const char *str = string_desc_arr[index];

  uint8_t chr_count = strlen(str);
  for (uint8_t i = 0; i < chr_count; i++)
  {
    _desc_str[1 + i] = str[i];
  }

  _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);
  return _desc_str;
}

//--------------------------------------------------------------------+
// HID Report Callbacks
//--------------------------------------------------------------------+
// Invoked when received GET_REPORT control request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)bufsize;
}
