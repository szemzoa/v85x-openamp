#ifndef __VIRT_UART_H__
#define __VIRT_UART_H__

#include "sunxi_openamp.h"

typedef struct __virt_uart_t {
    char 		  *name;
    struct rpmsg_endpoint ept;
    uint8_t     	  *rx_buf;
    uint16_t    	  rx_size;
    void    		  (*cb)(struct __virt_uart_t *vuart);
} virt_uart_t;

extern int virt_uart_init(struct sunxi_openamp_t *soa, virt_uart_t *vuart);
extern int virt_uart_deinit(virt_uart_t *vuart);
extern void virt_uart_register_cb(virt_uart_t *vuart, void (*cb)(virt_uart_t *vuart));
extern int virt_uart_transmit(virt_uart_t *vuart, const void *data, uint16_t size);

#endif

