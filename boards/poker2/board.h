/*
    Copyright (C) 2017 Frank Zschockelt

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#ifndef _BOARD_H_
#define _BOARD_H_

/* Board identifier. */
#define BOARD_KBC_POKERPRO_K61A_2_02_02
#define BOARD_NAME              "KBC PokerPro K61A"

/* USB Device descriptor parameter */
#define VENDOR_ID       0x6666 /* prototype VID */
#define PRODUCT_ID      0x0611 /* original PID of PokerII */
#define DEVICE_VER      0x0001
/* in python2: list(u"whatever".encode('utf-16-le')) */
/*   at most 32 characters or the ugly hack in usb_main.c borks */
#define MANUFACTURER "TMK"
#define USBSTR_MANUFACTURER    'T', '\x00', 'M', '\x00', 'K', '\x00'
#define PRODUCT "Poker II"
#define USBSTR_PRODUCT         'P', '\x00', 'o', '\x00', 'k', '\x00', 'e', '\x00', 'r', '\x00', ' ', '\x00', 'I', '\x00', 'I', '\x00'
#define DESCRIPTION "TMK keyboard firmware for Poker II"

/* key matrix size */
#define MATRIX_ROWS 15
#define MATRIX_COLS 8

/*
 * MCU type
 */
#define NUC122Zxxxx

/* Board oscillators-related settings. */

/* GPIO pins. */

/*
 * IO pins assignments.
 */

#define CAPSLOCK_LED_PORT     IOPORT2
#define CAPSLOCK_LED_PIN      9

#define SPACE_LEFT_LED_PORT   IOPORT4
#define SPACE_LEFT_LED_PIN    5

#define SPACE_RIGHT_LED_PORT  IOPORT4
#define SPACE_RIGHT_LED_PIN   4

#define DIPSWITCH_PORT        IOPORT4
#define DIPSWITCH_MASK        0x0f

/*
 * IO lines assignments.
 */

#define board_read_column()     (palReadPort(IOPORT2) ^ 0xff)
#define board_read_dip_switch() ((palReadPort(DIPSWITCH_PORT) & DIPSWITCH_MASK) ^ DIPSWITCH_MASK)

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
