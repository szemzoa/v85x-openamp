#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "sunxi_irq.h"
#include "debug.h"
#include "main.h"
#include <core_rv32.h>

void (*trap_c_callback)(void);

void trap_c(uint32_t *regs)
{
    debug("\r\n");
    debug("mepc   : %08x\r\n", regs[31]);
    debug("mstatus: %08x\r\n", regs[32]);

    if (trap_c_callback)
        trap_c_callback();

    while (1);
}

