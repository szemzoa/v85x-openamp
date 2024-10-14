#include <stddef.h>
#include "main.h"
#include "system.h"
#include "csi_rv32_gcc.h"
#include "nutheap.h"
#include "openamp/open_amp.h"
#include "sunxi_openamp.h"
#include "rsc_table.h"
#include "rpmsg_virtio.h"
#include "sunxi_virt_uart.h"

#define SUCCESS_STR				"success"
#define FAILED_STR				"failed"

virt_uart_t vuart0 = {
    .name = "virtual uart#0",
};

virt_uart_t vuart1 = {
    .name = "virtual uart#1",
};

void vuart0_rx_cb(virt_uart_t *vuart)
{
    char buf[RPMSG_BUFFER_SIZE];
    
	debug_riscv("%s receive: [%s]\r\n", vuart->name, (char *)vuart->rx_buf);

	sprintf(buf, "v85x-riscv: reply to [%s] on %s\n", vuart->rx_buf, vuart->name);
	virt_uart_transmit(vuart, buf, strlen(buf));
}

void vuart1_rx_cb(virt_uart_t *vuart)
{
    char buf[RPMSG_BUFFER_SIZE];

	debug_riscv("%s receive: [%s]\r\n", vuart->name, (char *)vuart->rx_buf);

	sprintf(buf, "v85x-riscv: reply to [%s] on %s\n", vuart->rx_buf, vuart->name);
	virt_uart_transmit(vuart, buf, strlen(buf));
}

int main()
{
    uint32_t current_ticks;
    int rc;

	system_init();

	rc = sunxi_openamp_init(&sunxi_openamp, RPMSG_REMOTE, NULL);
	debug_riscv("[%s]: init %s\r\n", sunxi_openamp.name, rc == 0 ? SUCCESS_STR : FAILED_STR);

	rc = virt_uart_init(&sunxi_openamp, &vuart0);
	debug_riscv("[%s]: %s init %s\r\n", sunxi_openamp.name, vuart0.name, rc == 0 ? SUCCESS_STR : FAILED_STR);

	rc = virt_uart_init(&sunxi_openamp, &vuart1);
	debug_riscv("[%s]: %s init %s\r\n", sunxi_openamp.name, vuart1.name, rc == 0 ? SUCCESS_STR : FAILED_STR);

	/* Need to register callback for message reception by channels */
	virt_uart_register_cb(&vuart0, vuart0_rx_cb);
	virt_uart_register_cb(&vuart1, vuart1_rx_cb);

	current_ticks = system_get_ticks();

	for(;;) {
	    if (system_get_ticks() - current_ticks > 2000) {
		current_ticks = system_get_ticks();
	    }
	}
}
