LIB_METAL:=$(TOPDIR)/openamp/libmetal
OPEN_AMP:=$(TOPDIR)/openamp/openamp
SUNXI_ADDON:=$(TOPDIR)/openamp/sunxi_addon

INCLUDE_DIRS += -I $(TOPDIR)/include -I $(TOPDIR)/include/openamp
CFLAGS += -DVIRTIO_SLAVE_ONLY

SRCS    +=  $(LIB_METAL)/device.c
SRCS    +=  $(LIB_METAL)/dma.c
SRCS    +=  $(LIB_METAL)/init.c
SRCS    +=  $(LIB_METAL)/io.c
SRCS    +=  $(LIB_METAL)/irq.c
SRCS    +=  $(LIB_METAL)/log.c
SRCS    +=  $(LIB_METAL)/shmem.c
SRCS    +=  $(LIB_METAL)/softirq.c
SRCS    +=  $(LIB_METAL)/version.c
SRCS    +=  $(LIB_METAL)/system/generic/condition.c
SRCS    +=  $(LIB_METAL)/system/generic/device.c
SRCS    +=  $(LIB_METAL)/system/generic/init.c
SRCS    +=  $(LIB_METAL)/system/generic/io.c
SRCS    +=  $(LIB_METAL)/system/generic/irq.c
SRCS    +=  $(LIB_METAL)/system/generic/shmem.c
SRCS    +=  $(LIB_METAL)/system/generic/time.c
SRCS    +=  $(LIB_METAL)/system/generic/sunxi/sys.c

SRCS    +=  $(OPEN_AMP)/remoteproc/elf_loader.c
SRCS    +=  $(OPEN_AMP)/remoteproc/remoteproc.c
SRCS    +=  $(OPEN_AMP)/remoteproc/remoteproc_virtio.c
SRCS    +=  $(OPEN_AMP)/remoteproc/rsc_table_parser.c
SRCS    +=  $(OPEN_AMP)/virtio/virtio.c
SRCS    +=  $(OPEN_AMP)/virtio/virtqueue.c
SRCS    +=  $(OPEN_AMP)/rpmsg/rpmsg.c
SRCS    +=  $(OPEN_AMP)/rpmsg/rpmsg_virtio.c
SRCS    +=  $(OPEN_AMP)/virtio_mmio/virtio_mmio_drv.c
