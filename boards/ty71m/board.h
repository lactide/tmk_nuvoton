/*
    Copyright (C) 2019 Frank Zschockelt

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

#include <stdint.h>
/* Board identifier. */
#define BOARD_TY71M_RGB_BT_V1_3_2017
#define BOARD_NAME              "Drevo Calibur TY71M"

/* USB Device descriptor parameter */
#define VENDOR_ID       0x6666 /* prototype VID */
#define PRODUCT_ID      0x4010 /* original pid of Drevo Calibur */
#define DEVICE_VER      0x0001
/* in python2: list(u"whatever".encode('utf-16-le')) */
/*   at most 32 characters or the ugly hack in usb_main.c borks */
#define MANUFACTURER "TMK"
#define USBSTR_MANUFACTURER    'T', '\x00', 'M', '\x00', 'K', '\x00'
#define PRODUCT "Calibur"
#define USBSTR_PRODUCT         'C', '\x00', 'a', '\x00', 'l', '\x00', 'i', '\x00', 'b', '\x00', 'u', '\x00', 'r', '\x00'
#define DESCRIPTION "TMK keyboard firmware for Drevo Calibur"

/* key matrix size */
#define MATRIX_ROWS 9
#define MATRIX_COLS 8
#define MATRIX_COL_PORTS 3

/*
 * MCU type
 */
#define NUC123LxxANx

/* Board oscillators-related settings. */

/* GPIO pins. */

/*
 * IO pins assignments.
 */

/*
 * IO lines assignments.
 */

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
  uint32_t board_read_column(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
