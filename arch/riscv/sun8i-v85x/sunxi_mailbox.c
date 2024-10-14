#include <stddef.h>
#include "debug.h"
#include "core_rv32.h"
#include "sunxi_irq.h"

#include "sunxi_mailbox.h"
#include "io.h"
#include "main.h"
#include "csi_rv32_gcc.h"

static struct msg_endpoint *it_edp = NULL;

#define MSGBOX_MAX_QUEUE 8

static inline int calculte_n(int local, int remote)
{
/*
 *                 | remote core (send)   |
 *                 | ARM:0 | DSP:1 | RV:2 |
 * local  |  ARM:0 |   /   |   0   |  1   |
 * core   |  DSP:1 |   0   |   /   |  1   |
 * (recv) |  RV :2 |   0   |   1   |  /   |
*/
	if (remote < local)
		return remote;
	else
		return remote - 1;
}

static void irq_msgbox_channel_handler(struct msg_endpoint *medp)
{
	void *msg_sts, *msg_reg, *msg_irq_s;
	uint32_t data;

	msg_sts = (void *)MSGBOX_MSG_STA_REG(
		medp->local_amp, calculte_n(medp->local_amp, medp->remote_amp),
		medp->read_ch);
	msg_reg = (void *)MSGBOX_MSG_REG(
		medp->local_amp, calculte_n(medp->local_amp, medp->remote_amp),
		medp->read_ch);
	msg_irq_s = (void *)MSGBOX_RD_IRQ_STA_REG(
		medp->local_amp, calculte_n(medp->local_amp, medp->remote_amp));

	while (read32(msg_sts)) {
		data = read32(msg_reg);
		if (medp->receive)
			medp->receive(data, (void *)medp);
	}

//	writel(1 << (medp->read_ch * 2), msg_irq_s);
	write32(msg_irq_s, 1 << (medp->read_ch * 2));
}

static void irq_msgbox_handler(void *arg)
{
	struct msg_endpoint *t;

	for (t = it_edp; t != NULL; t = t->next) {
		irq_msgbox_channel_handler(t);
	}
}

uint32_t hal_msgbox_init(void)
{
	arch_request_irq(E907_MBOX_E907_IRQn, irq_msgbox_handler, (void *)it_edp);
	enable_irq(E907_MBOX_E907_IRQn);

	return 0;
}

static void msgbox_enable_rec_int(struct msg_endpoint *medp)
{
	void *msg_irq_e;

	msg_irq_e = (void *)MSGBOX_RD_IRQ_EN_REG(
		medp->local_amp, calculte_n(medp->local_amp, medp->remote_amp));

//	writel(read32(msg_irq_e) | (1 << (medp->read_ch * 2)), msg_irq_e);
	write32(msg_irq_e, read32(msg_irq_e) | (1 << (medp->read_ch * 2)));

}

static void msgbox_disable_rec_int(struct msg_endpoint *medp)
{
	void *msg_irq_e;

	msg_irq_e = (void *)MSGBOX_RD_IRQ_EN_REG(
		medp->local_amp, calculte_n(medp->local_amp, medp->remote_amp));

//	writel(read32(msg_irq_e) & ~(1 << (medp->read_ch * 2)), msg_irq_e);
	write32(msg_irq_e, read32(msg_irq_e) & ~(1 << (medp->read_ch * 2)));
}

uint32_t hal_msgbox_alloc_channel(struct msg_endpoint *edp, int32_t remote,
			      int32_t read, int32_t write)
{
	edp->local_amp = THIS_MSGBOX_USE;
	edp->remote_amp = remote;
	edp->read_ch = read;
	edp->write_ch = write;

//	hal_mutex_lock(it_edp_mutex);
	/* add to global list */
	if (it_edp == NULL) {
		it_edp = edp;
	} else {
		struct msg_endpoint *t = it_edp;
		while (t) {
			if (t->next == NULL) {
				t->next = edp;
				break;
			}
			t = t->next;
		}
	}
	edp->next = NULL;
//	hal_mutex_unlock(it_edp_mutex);

	if (read >= 0)
		msgbox_enable_rec_int(edp);

	return 0;
}

void hal_msgbox_free_channel(struct msg_endpoint *edp)
{
	struct msg_endpoint *t = it_edp;

	if (t == edp) {
		it_edp = t->next;
	} else {
		while (t) {
			if (t->next == edp) {
				t->next = t->next->next;
				break;
			}
			t = t->next;
		}
	}
	edp->next = NULL;

	int rev_int = 0;
	t = it_edp;
	while (t) {
		if (t->read_ch >= 0) {
			rev_int = 1;
			break;
		}
	}

	if (rev_int == 0)
		msgbox_disable_rec_int(edp);

	return ;
}

static void msgbox_channel_send_data(struct msg_endpoint *medp, uint32_t data)
{
	void *msg_sts, *msg_reg;

	msg_sts = (void *)MSGBOX_MSG_STA_REG(
		medp->remote_amp, calculte_n(medp->remote_amp, medp->local_amp),
		medp->write_ch);
	msg_reg = (void *)MSGBOX_MSG_REG(
		medp->remote_amp, calculte_n(medp->remote_amp, medp->local_amp),
		medp->write_ch);

	while (read32(msg_sts) == MSGBOX_MAX_QUEUE);
//	writel(data, msg_reg);
	write32(msg_reg, data);
}

uint32_t hal_msgbox_channel_send(struct msg_endpoint *medp, uint8_t *bf,
			    uint32_t len)
{
	uint32_t data, i;

	data = 0;

	for (i = 0; i < len; i++) {

		if (!(i % 4))
			data = 0;

		data |= *bf++ << ((i % 4) << 3);

		if ((i % 4) == 3 || i == len - 1) {
			msgbox_channel_send_data(medp, data);
		}
	}

	return 0;
}
