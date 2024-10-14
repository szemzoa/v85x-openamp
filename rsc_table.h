#ifndef __RSC_TABLE_H__
#define __RSC_TABLE_H__

#include "openamp/open_amp.h"

#define SHM_START_ADDRESS       0x43d80000
#define SHM_SIZE                0x40000

#define VRING_RX_ADDRESS	((unsigned int)-1)  /* allocated by Master processor: CA7 */
#define VRING_TX_ADDRESS	((unsigned int)-1)  /* allocated by Master processor: CA7 */
#define VRING_BUFF_ADDRESS	((unsigned int)-1)  /* allocated by Master processor: CA7 */
#define VRING_ALIGNMENT         16        	    /* fixed to match with linux constraint */
#define VRING_NUM_BUFFS         16                  /* number of rpmsg buffer */

#define NUM_RESOURCE_ENTRIES    2
#define VRING_COUNT             2

#define VDEV_ID                 0xFF
#define VRING0_ID               0         /* VRING0 ID (master to remote) fixed to 0 for linux compatibility */
#define VRING1_ID               1         /* VRING1 ID (remote to master) fixed to 1 for linux compatibility */

/* Resource table for the given remote */
struct shared_resource_table {
	unsigned int version;
	unsigned int num;
	unsigned int reserved[2];
	unsigned int offset[NUM_RESOURCE_ENTRIES];
	/* text carveout entry */

	/* rpmsg vdev entry */
	struct fw_rsc_vdev vdev;
	struct fw_rsc_vdev_vring vring0;
	struct fw_rsc_vdev_vring vring1;
	struct fw_rsc_trace rv_trace;
};

void resource_table_init(int RPMsgRole, void **table_ptr, int *length);

#endif

