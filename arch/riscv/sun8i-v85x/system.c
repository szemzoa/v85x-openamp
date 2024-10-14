#include "core_rv32.h"
#include "system.h"
#include "nutheap.h"
#include "sunxi_irq.h"

#define OSC24M_FREQ	24000000

#define CONFIG_NUTHEAP_MEMORY_SIZE      (16 * 1024)
unsigned char NUTHEAP_MEMORY[CONFIG_NUTHEAP_MEMORY_SIZE];

volatile uint32_t tick = 0;

void SysTick_Handler(void *arg)
{
    csi_coret_config(OSC24M_FREQ / CONFIG_HZ, CORET_IRQn);

    tick++;
}

uint32_t system_get_ticks()
{
    return tick;
}

void system_init()
{
    int i;
    uint32_t mstatus = 0;

        /* Initialize our heap memory. */
        NutHeapAdd(NUTHEAP_MEMORY, CONFIG_NUTHEAP_MEMORY_SIZE & ~3);

	/* enable mstatus FS */
	#ifdef CONFIG_ARCH_RISCV_FPU
	    mstatus = __get_MSTATUS();
	    mstatus |= (1 << 13);
	    __set_MSTATUS(mstatus);
	#endif

	/* enable mxstatus THEADISAEE */
	uint32_t mxstatus = __get_MXSTATUS();
	mxstatus |= (1 << 22);
	/* enable mxstatus MM */
	#ifdef CONFIG_ARCH_RISCV_FPU
	    mxstatus |= (1 << 15);
	#endif
	__set_MXSTATUS(mxstatus);


	/* get interrupt level from info */
	CLIC->CLICCFG = (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

	for (i = 0; i < sizeof(CLIC->CLICINT)/sizeof(CLIC->CLICINT[0]); i++) {
    	    CLIC->CLICINT[i].IP = 0;
    	    CLIC->CLICINT[i].ATTR = 1; /* use vector interrupt */
	}

	/* tspend use positive interrupt */
	CLIC->CLICINT[Machine_Software_IRQn].ATTR = 0x3;

	irq_vectors_init();

	csi_coret_config(OSC24M_FREQ / CONFIG_HZ, CORET_IRQn);    //10ms

	arch_request_irq(CORET_IRQn, SysTick_Handler, NULL);
	enable_irq(CORET_IRQn);

	__enable_irq();
}

void system_msleep(uint32_t tmo)
{
    uint32_t ticks = system_get_ticks();

        while (system_get_ticks() - ticks < tmo)
            ;
}

void system_mdelay(uint32_t tmo)
{
        system_msleep(tmo);
}

void system_usleep(uint32_t tmo)
{
    uint32_t ticks = system_get_ticks();

        tmo = tmo / 1000;
        if (tmo == 0)
            tmo++;

        while (system_get_ticks() - ticks < tmo)
            ;
}