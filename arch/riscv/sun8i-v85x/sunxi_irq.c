#include <stddef.h>
#include <stdint.h>
#include "csi_rv32_gcc.h"
#include "sunxi_irq.h"
#include "core_rv32.h"
#include "debug.h"

extern void Default_Handler(void);
extern void SysTick_Handler(void);

void (*g_irqvector[CLIC_IRQ_NUM])(void);
void (*g_nmivector)(void);

typedef void (*irq_flow_handler_t)(int irq, void *data);


static const char *irq_string[CLIC_IRQ_NUM] = {
	[0 ... 2] = NULL,
	"Machine_Software",
	"User_Timer",
	"Supervisor_Timer",
	NULL,
	"CORET",
	[8 ... 10] = NULL,
	"Machine_External",
	[12 ... 15] = NULL,
	"CPUX_MBOX_RX",     /* CPUX MSGBOX READ IRQ FOR CPUX */
	"CPUX_MBOX_TX",     /* CPUX MSGBOX READ IRQ FOR E907 */
	"UART0",
	"UART1",
	"UART2",
	"UART3",
	[22 ... 24] = NULL,
	"TWI0",
	"TWI1",
	"TWI2",
	"TWI3",
	"TWI4",
	NULL,
	"SPI0",
	"SPI1",
	"SPI2",
	"PWM",
	"SPI_FLASH",
	"WIEGAND",
	"SPI3",
	[38 ... 39] = NULL,
	"DMIC",
	"AUDIO_CODEC",
	"I2S_PCM0",
	"I2S_PCM1",
	NULL,
	"USB_OTG",
	"USB_EHCI",
	"USB_OHCI",
	[48 ... 55] = NULL,
	"SMHC0",
	"SMHC1",
	"SMHC2",
	"MSI",
	"SMC",
	NULL,
	"GMAC",
	NULL,
	"CCU_FERR",
	"AHB_HREADY_TIME_OUT",
	"DMA_CPUX_NS",
	"DMA_CPUX_S",
	"CE_NS",
	"CE_S",
	"SPINLOCK",
	"HSTIMER0",
	"HSTIMER1",
	"GPADC",
	"THS",
	"TIMER0",
	"TIMER1",
	"TIMER2",
	"TIMER3",
	"WDG",
	"IOMMU",
	"NPU",
	"VE",
	"GPIOA_NS",
	"GPIOA_S",
	[85 ... 86] = NULL,
	"GPIOC_NS",
	"GPIOC_S",
	"GPIOD_NS",
	"GPIOD_S",
	"GPIOE_NS",
	"GPIOE_S",
	"GPIOF_NS",
	"GPIOF_S",
	"GPIOG_NS",
	"GPIOG_S",
	"GPIOH_NS",
	"GPIOH_S",
	"GPIOI_NS",
	"GPIOI_S",
	"DMAC_E907_NS",
	"DMAC_E907_S",
	"DE",
	NULL,
	"G2D",
	"LCD",
	NULL,
	"DSI",
	[109 ... 110] = NULL,
	"CSI_DMA0",
	"CSI_DMA1",
	"CSI_DMA2",
	"CSI_DMA3",
	NULL,
	"CSI_PARSER0",
	"CSI_PARSER1",
	"CSI_PARSER2",
	NULL,
	"CSI_CMB",
	"CSI_TDM",
	"CSI_TOP_PKT",
	NULL,
	"CSI_ISP0",
	"CSI_ISP1",
	"CSI_ISP2",
	"CSI_ISP3",
	"VIPP0",
	"VIPP1",
	"VIPP2",
	"VIPP3",
	[132 ... 143] = NULL,
	"E907_MBOX_RX",     /* e907 msgbox read irq for e907 Interrupt */
	"E907_MBOX_TX",     /* e907 msgbox write irq for cpux Interrupt */
	"E907_WDG",
	"E907_TIMER0",
	"E907_TIMER1",
	"E907_TIMER2",
	"E907_TIMER3",
	NULL,
	"NMI",
	"PPU",
	"ALARM",
	"AHBS_HREADY_TIME_OUT",
	"PMC",
	"GIC_C0",
	"TWD",
	NULL,
};

struct arch_irq_desc
{
    hal_irq_handler_t handle_irq;
    void *data;
};

static struct arch_irq_desc arch_irqs_desc[E907_SUNXI_IRQ_MAX];

static void clic_null_handler(void *data)
{
    return;
}

void clic_common_handler(void)
{
    int id = (__get_MCAUSE() & 0xfff) - CLIC_PERIPH_IRQ_OFFSET;
    if (arch_irqs_desc[id].handle_irq &&
        arch_irqs_desc[id].handle_irq != (void *)clic_null_handler)
    {
        arch_irqs_desc[id].handle_irq(arch_irqs_desc[id].data);
    }
    else
    {
        debug("no handler for irq %d\r\n", id);
    }
}

void show_irqs(void)
{
    int i;
    const char *irq_name;

	debug("IRQ    Status    Name\r\n");
	for (i = 0; i < CLIC_IRQ_NUM; i++) {
		if (csi_vic_get_enabled_irq(i)) {
			irq_name = irq_string[i];
			if (irq_name)
				debug("%3d    Enabled   %s\r\n", i, irq_name);
			else
				debug("%3d    Enabled\r\n", i);
		}
	}
}

void enable_irq(unsigned int irq_num)
{
    if (NMI_EXPn != irq_num)
        csi_vic_enable_irq(irq_num);
}

void disable_irq(unsigned int irq_num)
{
    if (NMI_EXPn != irq_num)
        csi_vic_disable_irq(irq_num);
}

int32_t arch_request_irq(int32_t irq, hal_irq_handler_t handler, void *data)
{
    if (irq < CLIC_PERIPH_IRQ_OFFSET)
    {
        g_irqvector[irq] = (void *)handler;
        return irq;
    }

    if (irq + CLIC_PERIPH_IRQ_OFFSET < CLIC_IRQ_NUM)
    {
        if (handler && arch_irqs_desc[irq - CLIC_PERIPH_IRQ_OFFSET].handle_irq == (void *)clic_null_handler)
        {
            arch_irqs_desc[irq - CLIC_PERIPH_IRQ_OFFSET].handle_irq = (void *)handler;
            arch_irqs_desc[irq - CLIC_PERIPH_IRQ_OFFSET].data = data;
        }
        return irq;
    }
    debug("Wrong irq NO.(%u) to request !!\r\n", irq);
    return -1;
}

void arch_free_irq(uint32_t irq)
{
    if (irq < CLIC_PERIPH_IRQ_OFFSET)
    {
        g_irqvector[irq] = (void *)Default_Handler;
        return;
    }
    if (irq + CLIC_PERIPH_IRQ_OFFSET < CLIC_IRQ_NUM)
    {
        arch_irqs_desc[irq - CLIC_PERIPH_IRQ_OFFSET].handle_irq = (void *)clic_null_handler;
    }
    return;
}

void irq_vectors_init(void)
{
    int i;

    for (i = CLIC_PERIPH_IRQ_OFFSET; i < CLIC_IRQ_NUM; i++)
    {
        disable_irq(i - CLIC_PERIPH_IRQ_OFFSET);
        g_irqvector[i] = (void *)clic_common_handler;
        arch_irqs_desc[i - CLIC_PERIPH_IRQ_OFFSET].handle_irq = (void *)clic_null_handler;
        arch_irqs_desc[i - CLIC_PERIPH_IRQ_OFFSET].data = NULL;
    }

    g_irqvector[CORET_IRQn] = SysTick_Handler;
}
