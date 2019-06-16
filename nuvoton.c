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

#include "matrix.h"
#include "suspend.h"
#include "timer.h"
#include "usb_main.h"

#include "hardfault.h"
#include "nuvoton.h"

static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debounce[MATRIX_ROWS];
static uint8_t      debounce_row[MATRIX_ROWS];

void matrix_init(void)
{
  /* pin initialization already done in board.c */
#if defined(NUMICRO_PORTAB_IRQ_VECTOR)
    nvicEnableVector(GPAB_IRQn, 1);
#endif
#if defined(NUMICRO_PORTCD_IRQ_VECTOR)
    nvicEnableVector(GPCD_IRQn, 1);
#endif
#if defined(NUMICRO_PORTCDE_IRQ_VECTOR)
    nvicEnableVector(GPCDE_IRQn, 1);
#endif
#if defined(NUMICRO_PORTCDF_IRQ_VECTOR)
    nvicEnableVector(GPCDF_IRQn, 1);
#endif
}

uint8_t matrix_scan(void)
{
    chThdSleepMilliseconds(1);
    for (int row = 0; row < MATRIX_ROWS; row++) {
        matrix_row_t data = 0;

        palClearPad(rows[row].port, rows[row].pad);
        data = board_read_column();
        palSetPad(rows[row].port, rows[row].pad);

        if (matrix_debounce[row] != data) {
            matrix_debounce[row] = data;
            debounce_row[row] = DEBOUNCE;
        } else if (debounce_row[row]) {
            debounce_row[row] -= 1;
        } else {
            matrix[row] = matrix_debounce[row];
        }
    }
    return 1;
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

static uint32_t woke_up_at = 0;
void hook_usb_suspend_loop(void) {
    /* Remote wakeup */
    if((USBD1.status & 2u) && suspend_wakeup_condition()) {
        usbWakeupHost(&USBD1);
        /* wait until SUSPEND flag is gone; scan the matrix in the meantime */
        while (USBD->ATTR & USBD_ATTR_SUSPEND_Msk)
            matrix_scan();
    } else if (timer_elapsed32(woke_up_at) > 1000) {
      for (int row = 0; row < MATRIX_ROWS; row++)
          palClearPad(rows[row].port, rows[row].pad);
      for (int col = 0; col < MATRIX_COL_PORTS; col++) {
          cols[col].port->IEN = cols[col].mask; /* enable interrupt */
          cols[col].port->IMD = cols[col].mask; /* level-triggered */
      }
      UNLOCKREG();
      CLK->PWRCON |= CLK_PWRCON_PWR_DOWN_EN_Msk; //similar to WFI
      CLK->PWRCON &= ~CLK_PWRCON_PWR_DOWN_EN_Msk; //after wakeup
      LOCKREG();
      woke_up_at = timer_read32();
      for (int row = 0; row < MATRIX_ROWS; row++)
          palSetPad(rows[row].port, rows[row].pad);
    }
}

static void _clear_and_disable_port_interrupt(ioportid_t port) {
    uint32_t isrc_val;
    isrc_val = port->ISRC;
    port->ISRC = isrc_val; /* clear pending interrupt */
    port->IEN = 0x0; /* disable interrupt */
}

#if defined(NUMICRO_PORTAB_IRQ_VECTOR)
OSAL_IRQ_HANDLER(NUMICRO_PORTAB_IRQ_VECTOR) {
    OSAL_IRQ_PROLOGUE();
    _clear_and_disable_port_interrupt(IOPORT1);
    _clear_and_disable_port_interrupt(IOPORT2);
    OSAL_IRQ_EPILOGUE();
}
#endif

#if defined(NUMICRO_PORTCD_IRQ_VECTOR)
OSAL_IRQ_HANDLER(NUMICRO_PORTCD_IRQ_VECTOR) {
    OSAL_IRQ_PROLOGUE();
    _clear_and_disable_port_interrupt(IOPORT3);
    _clear_and_disable_port_interrupt(IOPORT4);
    OSAL_IRQ_EPILOGUE();
}
#endif

#if defined(NUMICRO_PORTCDE_IRQ_VECTOR)
OSAL_IRQ_HANDLER(NUMICRO_PORTCDE_IRQ_VECTOR) {
    OSAL_IRQ_PROLOGUE();
    _clear_and_disable_port_interrupt(IOPORT3);
    _clear_and_disable_port_interrupt(IOPORT4);
    _clear_and_disable_port_interrupt(IOPORT5);
    OSAL_IRQ_EPILOGUE();
}
#endif

#if defined(NUMICRO_PORTCDF_IRQ_VECTOR)
OSAL_IRQ_HANDLER(NUMICRO_PORTCDF_IRQ_VECTOR) {
    OSAL_IRQ_PROLOGUE();
    _clear_and_disable_port_interrupt(IOPORT3);
    _clear_and_disable_port_interrupt(IOPORT4);
    _clear_and_disable_port_interrupt(IOPORT6);
    OSAL_IRQ_EPILOGUE();
}
#endif

void bootloader_jump(void)
{
    UNLOCKREG();
    FMC->ISPCON |= FMC_ISPCON_BS_Msk;
    NVIC_SystemReset();
}
