#ifndef __SUNXI_OPENAMP_H__
#define __SUNXI_OPENAMP_H__

#include "openamp/open_amp.h"
#include "sunxi_mailbox.h"

struct sunxi_openamp_t {
    char   *name;
    struct virtio_device *vdev;
    struct rpmsg_virtio_device rvdev;
    struct msg_endpoint edp;
};

extern struct sunxi_openamp_t sunxi_openamp;

extern int  sunxi_openamp_init(struct sunxi_openamp_t *soa, int RPMsgRole, rpmsg_ns_bind_cb ns_bind_cb);
extern void sunxi_openamp_deinit(struct sunxi_openamp_t *soa);
extern int  sunxi_openamp_create_endpoint(struct sunxi_openamp_t *soa, struct rpmsg_endpoint *ept, const char *name,
        	    uint32_t dest, rpmsg_ept_cb cb, rpmsg_ns_unbind_cb unbind_cb);
extern int  sunxi_mailbox_notify(void *priv, uint32_t id);

#endif
