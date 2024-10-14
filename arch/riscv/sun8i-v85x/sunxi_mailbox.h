#ifndef __SUNXI_MAILBOX_H__
#define __SUNXI_MAILBOX_H__

#include <stdint.h>

#define MSGBOX_RISCV 1
#define MSGBOX_RISCV_BASE 0x06020000

#define MSGBOX_ARM   0
#define MSGBOX_ARM_BASE 0x03003000

struct msg_endpoint {
	int32_t local_amp;
	int32_t remote_amp;
	int32_t write_ch;
	int32_t read_ch;
	void *data;
	void (*receive)(uint32_t l, void *d);
	void (*tx_done)(void *d);

	void *priv;

	struct msg_endpoint *next;
};

static inline void *msgbox_base_get(int *m)
{
	void *b = 0;

	switch (*m) {
	case MSGBOX_ARM:
		b = (void *)MSGBOX_ARM_BASE;
		break;
	case MSGBOX_RISCV:
		b = (void *)MSGBOX_RISCV_BASE;
		break;
	default:
		break;
	}

	return b;
}

static inline void *MSGBOX_VER_REG(int m, int n)
{
	void *msgbox_base = msgbox_base_get(&m);

	return (msgbox_base + 0x10 + (n)*0x100);
}

static inline void *MSGBOX_RD_IRQ_EN_REG(int m, int n)
{
	void *msgbox_base = msgbox_base_get(&m);

	return (msgbox_base + 0x20 + (n)*0x100);
}

static inline void *MSGBOX_RD_IRQ_STA_REG(int m, int n)
{
	void *msgbox_base = msgbox_base_get(&m);

	return (msgbox_base + 0x24 + (n)*0x100);
}

static inline void *MSGBOX_WR_IRQ_EN_REG(int m, int n)
{
	void *msgbox_base = msgbox_base_get(&m);

	return (msgbox_base + 0x30 + (n)*0x100);
}

static inline void *MSGBOX_WR_IRQ_STA_REG(int m, int n)
{
	void *msgbox_base = msgbox_base_get(&m);

	return (msgbox_base + 0x34 + (n)*0x100);
}

static inline void *MSGBOX_DEBUG_REG(int m, int n)
{
	void *msgbox_base = msgbox_base_get(&m);

	return (msgbox_base + 0x40 + (n)*0x100);
}

static inline void *MSGBOX_FIFO_STA_REG(int m, int n, int p)
{
	void *msgbox_base = msgbox_base_get(&m);

	return (msgbox_base + 0x50 + (n)*0x100 + (p)*0x4);
}

static inline void *MSGBOX_MSG_STA_REG(int m, int n, int p)
{
	void *msgbox_base = msgbox_base_get(&m);

	return (msgbox_base + 0x60 + (n)*0x100 + (p)*0x4);
}

static inline void *MSGBOX_MSG_REG(int m, int n, int p)
{
	void *msgbox_base = msgbox_base_get(&m);

	return (msgbox_base + 0x70 + (n)*0x100 + (p)*0x4);
}

static inline void *MSGBOX_WR_THRESHOLD_REG(int m, int n, int p)
{
	void *msgbox_base = msgbox_base_get(&m);

	return (msgbox_base + 0x80 + (n)*0x100 + (p)*0x4);
}

#define THIS_MSGBOX_USE 	MSGBOX_RISCV

extern uint32_t hal_msgbox_init(void);
extern uint32_t hal_msgbox_alloc_channel(struct msg_endpoint *edp, int32_t remote,
			      int32_t read, int32_t write);
extern uint32_t hal_msgbox_channel_send(struct msg_endpoint *edp, uint8_t *bf,
			     uint32_t len);
extern void hal_msgbox_free_channel(struct msg_endpoint *edp);

#endif
