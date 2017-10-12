#ifndef _HARDFAULT_H_
#define _HARDFAULT_H_


#define HARDFAULT_HAPPENED_MASK  0xDEAD0000u

typedef struct {
  uint32_t pc;
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t psr;
} m0_hardfault_regs;

extern m0_hardfault_regs hardfault_regs;

#endif
