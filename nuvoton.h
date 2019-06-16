#ifndef _NUVOTON_H_
#define _NUVOTON_H_

typedef struct {
  ioportid_t  port;
  iopadid_t   pad;
} keymap_port_pin;

typedef struct {
  ioportid_t   port;
  ioportmask_t mask;
} keymap_column_mask;

extern const keymap_port_pin rows[MATRIX_ROWS];
extern const keymap_column_mask cols[MATRIX_COL_PORTS];

#endif
