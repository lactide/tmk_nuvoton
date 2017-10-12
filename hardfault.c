#include "cmparams.h"

#include "hardfault.h"

void hardfault_handler_c(uint32_t *pulFaultStackAddress)
{
    hardfault_regs.r0 = pulFaultStackAddress[0];
    hardfault_regs.r1 = pulFaultStackAddress[1];
    hardfault_regs.r2 = pulFaultStackAddress[2];
    hardfault_regs.r3 = pulFaultStackAddress[3];
    hardfault_regs.r12 = pulFaultStackAddress[4];
    hardfault_regs.lr = pulFaultStackAddress[5];
    hardfault_regs.pc = pulFaultStackAddress[6] | HARDFAULT_HAPPENED_MASK;
    hardfault_regs.psr = pulFaultStackAddress[7];
    NVIC_SystemReset();
}

void HardFault_Handler(void) __attribute__((naked));

void HardFault_Handler(void)
{
    asm volatile(
        "movs r0, #4;"
        "mov  r1, lr;"
        "tst  r0, r1;" /* Check EXC_RETURN[2] */
        "beq 1f;"
        "mrs r0, psp;"
        "ldr r1,=hardfault_handler_c;"
        "bx r1;"
        "1:mrs r0,msp;"
        "ldr r1,=hardfault_handler_c;"
        "bx r1;"
        : : : "r0"
    );
}


