#include "sunxi_virt_uart.h"
#include "metal/utilities.h"

#define RPMSG_SERVICE_NAME              "rpmsg-tty"

static int virt_uart_read_cb(struct rpmsg_endpoint *ept, void *data,
			    size_t len, uint32_t src, void *priv)
{
    virt_uart_t *vuart = metal_container_of(ept, virt_uart_t, ept);
    (void)src;

	vuart->rx_buf = data;
	vuart->rx_size = len;

	if (vuart->cb != NULL)
		vuart->cb(vuart);

	return 0;
}

int virt_uart_init(struct sunxi_openamp_t *soa, virt_uart_t *vuart)
{
	/* Create a endpoint for rmpsg communication */
	if (sunxi_openamp_create_endpoint(soa, &vuart->ept, 
		    RPMSG_SERVICE_NAME, RPMSG_ADDR_ANY, virt_uart_read_cb, NULL) < 0) {

	    return -1;  	  	  	
	}

	return 0;
}

int virt_uart_deinit(virt_uart_t *vuart)
{
	rpmsg_destroy_ept(&vuart->ept);

	return 0;
}

void virt_uart_register_cb(virt_uart_t *vuart, void (*cb)(virt_uart_t *vuart))
{
	vuart->cb = cb;
}

int virt_uart_transmit(virt_uart_t *vuart, const void *data, uint16_t size)
{
	if (size > (RPMSG_BUFFER_SIZE - 16))
	  return -1;

	return rpmsg_send(&vuart->ept, data, size);
}

