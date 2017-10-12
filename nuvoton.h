#ifndef _NUVOTON_H_
#define _NUVOTON_H_

typedef struct {
  ioportid_t  port;
  iopadid_t   pad;
} keymap_port_pin;

extern const keymap_port_pin rows[MATRIX_ROWS];

#endif
