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
#include "action_layer.h"
#include "bootloader.h"
#include "debug.h"
#include "led.h"
#include "host.h"

#include "keymap_common.h"
#include "hardfault.h"

enum function_id {
  PRINT_HARDFAULT_PC,
  DEBUGPRINT_LDROM,
};

/*
 * qwerty/dvorak Keymap
 * Capslock is Esc when tapped, activates Layer 1 when holding.
 * Layer 2 is similar to PokerII, with some changes:
 * - Mouse on JKLI, Mousekeys on N/M/, and Wheel on H/Y
 * - Capslock on Right Shift
 * - Insert is below Backspace (ansi) or between PgUp/Enter (iso)
 * - Bootloader on ^
 * - Poor man's Debugger on Tab (show hardfault program counter)
 * - Toggle dvorak/qwerty on PN
 * - DEBUGPRINT_LDROM is mapped at FN2 (Capslock+RCTL):
 *   It prints the 4k LDROM section of a NUC122 MCU via the
 *   hid_console (use hid_listen from pjrc to get the hexdump)
*/
const uint8_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [0] = /* qwerty */
  KMAP(GRV,  1,    2,    3,    4,    5,    6,    7,    8,    9,    0,    MINS, EQL,  BSPC,
       TAB,  Q,    W,    E,    R,    T,    Y,    U,    I,    O,    P,    LBRC, RBRC, BSLS,
       FN0,  A,    S,    D,    F,    G,    H,    J,    K,    L,    SCLN, QUOT, NUHS, ENT,
       LSFT, NUBS, Z,    X,    C,    V,    B,    N,    M,    COMM, DOT,  SLSH, RSFT,
       LCTL, LGUI, LALT,               SPACE,                RALT, RGUI, APP,  RCTL)
  [1] = /* dvorak */
  KMAP(GRV,  1,    2,    3,    4,    5,    6,    7,    8,    9,    0,    LBRC, RBRC,  BSPC,
       TAB,  QUOT, COMM, DOT,  P,    Y,    F,    G,    C,    R,    L,    SLSH, EQL,   BSLS,
       FN0,  A,    O,    E,    U,    I,    D,    H,    T,    N,    S,    MINS, NUHS,  ENT,
       LSFT, NUBS, SCLN, Q,    J,    K,    X,    B,    M,    W,    V,    Z,    RSFT,
       LCTL, LGUI, LALT,               SPACE,                RALT, RGUI, APP,  RCTL)
  [2] =
  KMAP(BTLD, F1,   F2,   F3,   F4,   F5,   F6,   F7,   F8,   F9,   F10,  F11,  F12,  DEL,
       FN1,  TRNS, UP,   TRNS, TRNS, TRNS, WH_U, TRNS, MS_U, TRNS, PSCR, SLCK, PAUS, INS,
       TRNS, LEFT, DOWN, RGHT, TRNS, TRNS, WH_D, MS_L, MS_D, MS_R, HOME, PGUP, INS,  TRNS,
       TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, TRNS, BTN1, BTN2, BTN3, END,  PGDN, CAPS,
       TRNS, TRNS, TRNS,                TRNS,                TRNS, TRNS, FN3,  FN2)
};

const action_t fn_actions[] = {
  [0] = ACTION_LAYER_TAP_KEY(2, KC_ESC),
  [1] = ACTION_FUNCTION_TAP(PRINT_HARDFAULT_PC),
  [2] = ACTION_FUNCTION_TAP(DEBUGPRINT_LDROM),
  [3] = ACTION_DEFAULT_LAYER_TOGGLE(1), /* toggle qwerty/dvorak */
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

void hook_early_init(void)
{
    uint8_t dip_switch = board_read_dip_switch();
    if (dip_switch & 0x8) {
        bootloader_jump();
    }
    debug_enable = (dip_switch & 0x4);
#ifdef NKRO_ENABLE
    keyboard_nkro = (dip_switch & 0x2);
#endif
    default_layer_set(1 << (dip_switch & 0x1));

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
    // lights LED when layer 2 is enabled
    if (layer_state & (1L<<2)) {
        palClearPad(SPACE_LEFT_LED_PORT, SPACE_LEFT_LED_PIN);
    } else {
        palSetPad(SPACE_LEFT_LED_PORT, SPACE_LEFT_LED_PIN);
    }
}
