UTILS:=$(TOPDIR)/utils

INCLUDE_DIRS += -I $(UTILS)

SRCS += $(UTILS)/debug.c
SRCS += $(UTILS)/printf.c
SRCS += $(UTILS)/nutheap.c
SRCS += $(UTILS)/ringbuffer.c

