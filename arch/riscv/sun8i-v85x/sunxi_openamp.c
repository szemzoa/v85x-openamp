#include "sunxi_openamp.h"
#include "rsc_table.h"
#include "metal/sys.h"
#include "metal/device.h"
#include "main.h"
#include "io.h"

#define SHM_DEVICE_NAME         "SUN8I_SHM"

static struct metal_io_region *shm_io;
static struct metal_io_region *rsc_io;
static struct shared_resource_table *rsc_table;
static struct rpmsg_virtio_shm_pool shpool;
static metal_phys_addr_t shm_physmap;

struct sunxi_openamp_t sunxi_openamp = {
	.name = "sunxi_openamp",
};

static int sunxi_openamp_notify(void *priv, uint32_t id);
static void sunxi_openamp_receive(uint32_t d, void *arg);


struct metal_device shm_device = {
    .name = SHM_DEVICE_NAME,
    .num_regions = 2,
    .regions = {
        {
    	    .virt = NULL
        }, /* shared memory */
        {
    	    .virt = NULL
        }, /* rsc_table memory */
    },
    .node = { 
	NULL 
    },
    .irq_num = 0,
    .irq_info = NULL
};

static int _shmem_init(int RPMsgRole)
{
    int status = 0;
    struct metal_device *device = NULL;
    struct metal_init_params metal_params = METAL_INIT_DEFAULTS;
    void* rsc_tab_addr = NULL;
    int rsc_size = 0;

	metal_init(&metal_params);

	if ((status = metal_register_generic_device(&shm_device)) != 0)
		return status;

	if ((status = metal_device_open("generic", SHM_DEVICE_NAME, &device)) != 0)
		return status;

	shm_physmap = SHM_START_ADDRESS;
	metal_io_init(&device->regions[0], (void *)SHM_START_ADDRESS, &shm_physmap,
		SHM_SIZE, (unsigned int)-1, 0, NULL);

	if ((shm_io = metal_device_io_region(device, 0)) == NULL)
		return -1;

	/* Initialize resources table variables */
	resource_table_init(RPMsgRole, &rsc_tab_addr, &rsc_size);
	rsc_table = (struct shared_resource_table *)rsc_tab_addr;
	if (!rsc_table)
		return -1;

	// resource_table_dump(rsc_table, rsc_size);

	metal_io_init(&device->regions[1], rsc_table,
               (metal_phys_addr_t *)rsc_table, rsc_size, -1U, 0, NULL);

	if ((rsc_io = metal_device_io_region(device, 1)) == NULL)
		return -1;

	return 0;
}

int sunxi_openamp_init(struct sunxi_openamp_t *soa, int RPMsgRole, rpmsg_ns_bind_cb ns_bind_cb)
{
    struct fw_rsc_vdev_vring *vring_rsc = NULL;
    int status = 0;
    uint32_t reg32;

	/* MSGBOX1 reset deassert, open clock gate */
	reg32 = read32(0x2001000 + 0x071c);
	reg32 |= (1 << 1) | (1 << 17);
	write32(0x2001000 + 0x071c, reg32);

	hal_msgbox_init();

	soa->edp.receive = sunxi_openamp_receive;
	soa->edp.priv = (void *)soa;

	/* edp, remote, read, write */
	hal_msgbox_alloc_channel(&soa->edp, 0, 1, 0);

	/* Libmetal Initilalization */
	if ((status = _shmem_init(RPMsgRole)))
		return status;

	if ((soa->vdev = rproc_virtio_create_vdev(RPMsgRole, VDEV_ID, &rsc_table->vdev,
                      rsc_io, (void *)soa, sunxi_openamp_notify, NULL)) == NULL) {
        
                return -1;
        }

	rproc_virtio_wait_remote_ready(soa->vdev);

	vring_rsc = &rsc_table->vring0;
	if ((status = rproc_virtio_init_vring(soa->vdev, 0, vring_rsc->notifyid,
            	    (void *)vring_rsc->da, shm_io, vring_rsc->num, vring_rsc->align)) != 0) {

		return status;
        }

	vring_rsc = &rsc_table->vring1;
	if ((status = rproc_virtio_init_vring(soa->vdev, 1, vring_rsc->notifyid,
		    (void *)vring_rsc->da, shm_io, vring_rsc->num, vring_rsc->align)) != 0) {
	
		return status;	
	}

	rpmsg_virtio_init_shm_pool(&shpool, (void *)VRING_BUFF_ADDRESS, (size_t)SHM_SIZE);
	rpmsg_init_vdev(&soa->rvdev, soa->vdev, ns_bind_cb, shm_io, &shpool);

	return 0;
}

void sunxi_openamp_deinit(struct sunxi_openamp_t *soa)
{
	rpmsg_deinit_vdev(&soa->rvdev);
	metal_finish();
}

int sunxi_openamp_create_endpoint(struct sunxi_openamp_t *soa, struct rpmsg_endpoint *ept, const char *name,
            uint32_t dest, rpmsg_ept_cb cb, rpmsg_ns_unbind_cb unbind_cb)
{
	return rpmsg_create_ept(ept, &soa->rvdev.rdev, name, RPMSG_ADDR_ANY, dest, cb,
		          unbind_cb);
}

static void sunxi_openamp_receive(uint32_t d, void *arg)
{
    struct msg_endpoint *medp = (struct msg_endpoint *)arg;
    struct sunxi_openamp_t *soa = (struct sunxi_openamp_t *)medp->priv;

        rproc_virtio_notified(soa->rvdev.vdev, VRING1_ID);
}

static int sunxi_openamp_notify(void *priv, uint32_t id)
{
    struct sunxi_openamp_t *soa = (struct sunxi_openamp_t *)priv;
    uint32_t channel = VRING0_ID;

	hal_msgbox_channel_send(&soa->edp, (void *)&channel, sizeof(channel));

	return 0;
}
