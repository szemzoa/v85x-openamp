#include "main.h"
#include "rsc_table.h"
#include "openamp/open_amp.h"

#define RPMSG_IPU_C0_FEATURES		1
#define VRING_COUNT         		2

/* VirtIO rpmsg device id */
#define VIRTIO_ID_RPMSG_            	7

#ifdef CONFIG_ENABLE_LOG_TRACE
char log_trace_buffer[LOG_TRACE_BUFFER_SIZE];
#endif

struct shared_resource_table __attribute__((__section__(".resource_table"))) resource_table = {
	.version = 1,
#ifdef CONFIG_ENABLE_LOG_TRACE
	.num = 2,
#else
	.num = 1,
#endif
	.reserved = {0, 0},
	.offset = {
		offsetof(struct shared_resource_table, vdev),
		offsetof(struct shared_resource_table, rv_trace),
	},

	/* Virtio device entry */
	.vdev= {
		RSC_VDEV, VIRTIO_ID_RPMSG_, 0, RPMSG_IPU_C0_FEATURES, 0, 0, 0,
		VRING_COUNT, {0, 0},
	},

	/* Vring rsc entry - part of vdev rsc entry */
	.vring0 = {VRING_TX_ADDRESS, VRING_ALIGNMENT, VRING_NUM_BUFFS, VRING0_ID, 0},
	.vring1 = {VRING_RX_ADDRESS, VRING_ALIGNMENT, VRING_NUM_BUFFS, VRING1_ID, 0},

#ifdef CONFIG_ENABLE_LOG_TRACE
	.rv_trace = {
		RSC_TRACE,
		(uint32_t)log_trace_buffer, LOG_TRACE_BUFFER_SIZE, 0, "riscv_log",
	},
#endif
};

void resource_table_init(int RPMsgRole, void **table_ptr, int *length)
{
  (void)RPMsgRole;
  *length = sizeof(resource_table);
  *table_ptr = (void *)&resource_table;
}

void resource_table_dump(void *rsc, int size)
{
	int i, j;
	struct shared_resource_table *res = rsc;
	char *res_start;

	debug("Resource table: entries: %d \r\n",res->num);

	for (i = 0; i < res->num; i++) {

		res_start = rsc;
		res_start += res->offset[i];
		uint32_t type = *(uint32_t *)res_start;

		if (type == RSC_CARVEOUT) {
			struct fw_rsc_carveout *shm = 
				(struct fw_rsc_carveout *)res_start;
			debug("entry%d-%s-shm: pa:0x%08x da:%08x len:0x%x\r\n",
					i, shm->name, shm->pa, shm->da, shm->len);
		} 
		else if (type == RSC_VDEV) {
			struct fw_rsc_vdev *vdev =
				(struct fw_rsc_vdev *)res_start;

			debug("entry%d-vdev%d: notifyid:%d cfg_len:%d\r\n",
					i, vdev->id, vdev->notifyid, vdev->config_len);
			for (j = 0; j < vdev->num_of_vrings; j++) {
				struct fw_rsc_vdev_vring *vring = &vdev->vring[j];
				debug("entry%d-vring%d: notifyid:%d da:0x%08x\r\n",
						i, j, vring->notifyid, vring->da);
			}
		}
	}
}
