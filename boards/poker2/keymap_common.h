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

/*
 * scan matrix
 */
#include "keymap.h"

/*
   The Dxx identifiers are taken from the back of PCB from the diodes used
   by the key.

   PokerII ISO PCBs don't have the key at diode D14 populated.
   PokerII ANSI PCBs are probably missing the keys at the diodes D68/D69.
*/

#define KMAP( \
    D01, D02, D03, D04, D05, D06, D07, D08, D09, D10, D11, D12, D13, D15, \
    D16, D17, D18, D19, D20, D21, D22, D23, D24, D25, D26, D27, D28, D14, \
    D30, D31, D32, D33, D34, D35, D36, D37, D38, D39, D40, D41, D68, D42, \
    D43, D69, D44, D45, D46, D47, D48, D49, D50, D51, D52, D53, D54, \
    D57, D59, D60,           D61,                D62, D64, D63, D56  \
) \
{ \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_##D64, KC_##D63, KC_NO,    KC_NO    }, \
    { KC_NO,    KC_##D43, KC_##D54, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_##D59, KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_##D24, KC_##D28, KC_##D38, KC_NO,    KC_##D51, KC_NO,    KC_##D13, KC_##D09 }, \
    { KC_##D25, KC_NO,    KC_##D39, KC_NO,    KC_##D52, KC_NO,    KC_NO,    KC_##D10 }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_##D57, KC_NO    }, \
    { KC_##D17, KC_##D16, KC_##D31, KC_##D01, KC_##D44, KC_NO,    KC_##D56, KC_##D02 }, \
    { KC_##D18, KC_##D30, KC_##D32, KC_##D69, KC_##D45, KC_NO,    KC_NO,    KC_##D03 }, \
    { KC_##D19, KC_NO,    KC_##D33, KC_NO,    KC_##D46, KC_NO,    KC_NO,    KC_##D04 }, \
    { KC_##D20, KC_##D21, KC_##D34, KC_##D35, KC_##D47, KC_##D48, KC_##D06, KC_##D05 }, \
    { KC_##D23, KC_##D22, KC_##D37, KC_##D36, KC_##D50, KC_##D49, KC_##D07, KC_##D08 }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_##D61, KC_NO,    KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_##D15, KC_##D14, KC_NO,    KC_##D42, KC_NO,    KC_NO,    KC_NO    }, \
    { KC_NO,    KC_NO,    KC_NO,    KC_##D60, KC_NO,    KC_##D62, KC_NO,    KC_NO    }, \
    { KC_##D26, KC_##D27, KC_##D40, KC_##D41, KC_##D68, KC_##D53, KC_##D12, KC_##D11 }, \
},
