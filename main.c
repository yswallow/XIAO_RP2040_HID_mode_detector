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
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"
#include "device/dcd.h"
#include "usb_descriptors.h"

#include "hardware/resets.h"
#include "hardware/gpio.h"
#include "pico/time.h"

#include "hardware/structs/scb.h"

#include "hardware/pll.h"
#include "hardware/clocks.h"
#include "hardware/structs/pll.h"
#include "hardware/structs/clocks.h"

#define LED_UNDEFINED 17
#define LED_BOOT_MODE 16
#define LED_REPORT_MODE 25

#define LED_ON false
#define LED_OFF true
//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

uint32_t __uninitialized_ram(device_mode);

static void hid_task(void);
static void write_serial_port(void);

void led_task(void) {
    gpio_put( LED_UNDEFINED, (! (device_mode==DEVICE_MODE_BOOT || device_mode==DEVICE_MODE_REPORT)) ? LED_ON : LED_OFF );
    gpio_put( LED_BOOT_MODE, device_mode==DEVICE_MODE_BOOT ? LED_ON : LED_OFF );
    gpio_put( LED_REPORT_MODE, device_mode==DEVICE_MODE_REPORT ? LED_ON : LED_OFF );
}

/*------------- MAIN -------------*/
int main(void)
{
  clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                    48 * MHZ,
                    48 * MHZ);
  
  board_init();
  tusb_init();
  
  gpio_init(LED_UNDEFINED);
  gpio_init(LED_BOOT_MODE);
  gpio_init(LED_REPORT_MODE);
  
  gpio_set_dir(LED_UNDEFINED, true);
  gpio_set_dir(LED_BOOT_MODE, true);
  gpio_set_dir(LED_REPORT_MODE, true);

  while(1)
  {
    tud_task(); // tinyusb device task
    led_task();
    hid_task();
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{

}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    device_mode = 0;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+
static uint8_t keycode[6] = {0};
static void send_down(bool btn) {
  if(btn) {
    keycode[0] = HID_KEY_ARROW_DOWN;
  } else {
    keycode[0] = 0;
  }

  if( device_mode==DEVICE_MODE_BOOT ) {
    if( tud_hid_n_ready(BOOT_ITF_NUM_KEYBOARD) ) {
      tud_hid_n_keyboard_report(BOOT_ITF_NUM_KEYBOARD, 0, 0, keycode);
    }
  } else {
    if( tud_hid_ready() ) {
      tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);   
    }
  }
}

void reset_usb(void) 
{
  scb_hw->aircr = 0x05FA0004;
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
static void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 20 ;
  static uint32_t start_ms = 0;

  if ( board_millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  uint32_t const btn = board_button_read();
  
  send_down(btn);
  if(btn) {
    write_serial_port();
  }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
  (void) instance;
  (void) len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;
}

// Invoked when received SET_PROTOCOL request
// protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol)
{
  (void) instance;
  
  if( protocol == HID_PROTOCOL_BOOT ) {
    if( device_mode==DEVICE_MODE_REPORT ) {
      device_mode = DEVICE_MODE_BOOT;
      reset_usb();
    } else {
      device_mode = DEVICE_MODE_BOOT;
    }
  } else {
    if( device_mode!=DEVICE_MODE_REPORT ) {
      device_mode = DEVICE_MODE_REPORT;
      reset_usb();
    } else {
      device_mode = DEVICE_MODE_REPORT;
    }
  }
}

// CDC
const char hello[] = "Hello, World!\r\n";
const uint8_t hello_len = strlen(hello);

static void write_serial_port(void)
{
  for(uint8_t i=0; i<hello_len; i++)
  {
    tud_cdc_n_write_char(0, hello[i]);
  }
  tud_cdc_n_write_flush(0);
}

