/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdint.h>

#ifndef USB_DESCRIPTORS_H_
#define USB_DESCRIPTORS_H_

enum
{
  DEVICE_MODE_BOOT = 0xDEADBEEF,
  DEVICE_MODE_REPORT = 0x00C0FFEE,
};

extern uint32_t device_mode;
void reset_usb(void);

enum
{
  BOOT_ITF_NUM_KEYBOARD = 0,
  BOOT_ITF_NUM_CDC,
  BOOT_ITF_NUM_CDC_DATA,
  BOOT_ITF_NUM_HID,
  BOOT_ITF_NUM_MSC,
  BOOT_ITF_NUM_TOTAL
};

enum
{
  REPORT_ITF_NUM_CDC = 0,
  REPORT_ITF_NUM_CDC_DATA,
  REPORT_ITF_NUM_HID,
  REPORT_ITF_NUM_TOTAL
};

enum
{
  REPORT_ID_KEYBOARD = 1,
  REPORT_ID_MOUSE,
  REPORT_ID_CONSUMER_CONTROL,
  REPORT_ID_GAMEPAD,
  REPORT_ID_COUNT
};

#endif /* USB_DESCRIPTORS_H_ */
