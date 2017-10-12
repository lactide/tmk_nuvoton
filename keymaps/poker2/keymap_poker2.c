/*
Copyright 2017 Frank Zschockelt <tmk@freakysoft.de>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"

/*
 * scan matrix
 */
#include "bootloader.h"
#include "debug.h"
#include "led.h"

#include "keymap_common.h"
#include "hardfault.h"

enum function_id {
  PRINT_HARDFAULT_PC,
  DEBUGPRINT_LDROM,
};

/*
 * QWERTZ Keymap
 * Capslock is Esc when tapped, activates Layer 1 when holding.
 * Layer 1 is similar to PokerII, with some changes:
 * - Mouse on T/F/G/H, Mousekeys on J/K/L, Wheel on C/V
 * - Capslock on Right Shift
 * - Bootloader on ^
 * - Poor man's Debugger on Tab (show hardfault program counter)
 *
 * Currently not enabled:
 * - DEBUGPRINT_LDROM is not mapped currently (would be FN2):
 *   It would print the 4k LDROM section of a NUC122 MCU via the
 *   hid_console (use hid_listen from pjrc to get the hexdump)
*/
const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] =
  KMAP(GRV,  1,    2,    3,    4,    5,    6,    7,    8,    9,    0,    MINS, EQL,  BSPC,
       TAB,  Q,    W,    E,    R,    T,    Z,    U,    I,    O,    P,    LBRC, RBRC, BSLS,
       FN0,  A,    S,    D,    F,    G,    H,    J,    K,    L,    SCLN, QUOT, BSLS, ENT,
       LSFT, NUBS, Y,    X,    C,    V,    B,    N,    M,    COMM, DOT,  SLSH, RSFT,
       LCTL, LGUI, LALT,               SPACE,                RALT, RGUI, APP,  RCTL)
  [1] =
  KMAP(BTLD, F1,   F2,   F3,   F4,   F5,   F6,   F7,   F8,   F9,   F10,  F11,  F12,  DEL,
       FN1,  TRNS, UP,   TRNS, TRNS, MS_U, TRNS, TRNS, INS,  TRNS, PSCR, SLCK, PAUS, TRNS,
       TRNS, LEFT, DOWN, RGHT, MS_L, MS_D, MS_R, BTN3, BTN1, BTN2, HOME, PGUP, TRNS, TRNS,
       TRNS, TRNS, TRNS, TRNS, WH_D, WH_U, TRNS, TRNS, TRNS, TRNS, END,  PGDN, CAPS,
       TRNS, TRNS, TRNS,                TRNS,                TRNS, TRNS, TRNS, TRNS)
};

const action_t fn_actions[] = {
  [0] = ACTION_LAYER_TAP_KEY(1, KC_ESC),
  [1] = ACTION_FUNCTION_TAP(PRINT_HARDFAULT_PC),
  [2] = ACTION_FUNCTION_TAP(DEBUGPRINT_LDROM),
};

/*
 * user defined action function
 */

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    (void)opt;
    switch (id) {
        case DEBUGPRINT_LDROM:
          if (record->event.pressed) {
              debug_enable = true;
              UNLOCKREG();
              FMC->ISPCON |= FMC_ISPCON_LDUEN_Msk | FMC_ISPCON_ISPEN_Msk;
              for (uint32_t addr=0x100000; addr < 0x101000; addr+=4) {
                  FMC->ISPADR = addr;
                  FMC->ISPCMD = 0x00;
                  FMC->ISPTRG = 1;
                  while (FMC->ISPTRG)
                    ;
                  if (addr % 16 == 0) {
                    dprintln();
                    chThdSleepMilliseconds(3);
                  }
                  dprintf("%02x%02x %02x%02x ",
                          FMC->ISPDAT & 0xff,
                          (FMC->ISPDAT & 0xff00) >> 8,
                          (FMC->ISPDAT & 0xff0000) >> 16,
                          (FMC->ISPDAT & 0xff000000) >> 24);
              }
              LOCKREG();
              debug_enable = false;
          }
          break;
        case PRINT_HARDFAULT_PC:
          if (record->event.pressed) {
              if ((hardfault_regs.pc & HARDFAULT_HAPPENED_MASK) == HARDFAULT_HAPPENED_MASK) {
                  uint8_t nibble;
                  uint8_t code;
                  for (int i = 12; i>=0; i-=4) {
                      nibble = (hardfault_regs.pc >> i) & 0x0f;
                      if (nibble == 0x0) {
                          code = KC_0;
                      } else if (nibble < 0xa) {
                          nibble--;
                          code = KC_1+nibble;
                      } else {
                          nibble-=0xa;
                          code = KC_A+nibble;
                      }
                      register_code(code);
                      unregister_code(code);
                  }
                  palSetPad(SPACE_RIGHT_LED_PORT, SPACE_RIGHT_LED_PIN);
                }
          }
          break;
    }
}

void hook_late_init(void)
{
    switch (board_read_dip_switch()) {
      case 0x8:
        debug_enable = true;
        break;
      case 0xf:
        bootloader_jump();
        break;
    }

    if ((hardfault_regs.pc & HARDFAULT_HAPPENED_MASK) == HARDFAULT_HAPPENED_MASK) {
        palClearPad(SPACE_RIGHT_LED_PORT, SPACE_RIGHT_LED_PIN);
    }
}

void led_set(uint8_t usb_led) {
    if (usb_led & (1<<USB_LED_CAPS_LOCK)) {
        palClearPad(CAPSLOCK_LED_PORT, CAPSLOCK_LED_PIN);
    } else {
        palSetPad(CAPSLOCK_LED_PORT, CAPSLOCK_LED_PIN);
    }
}

void hook_layer_change(uint32_t layer_state)
{
    // lights LED when layer 1 is enabled
    if (layer_state & (1L<<1)) {
        palClearPad(SPACE_LEFT_LED_PORT, SPACE_LEFT_LED_PIN);
    } else {
        palSetPad(SPACE_LEFT_LED_PORT, SPACE_LEFT_LED_PIN);
    }
}
