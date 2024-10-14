ARCH:=$(TOPDIR)/arch

INCLUDE_DIRS += -I $(ARCH)/riscv -I $(ARCH)/riscv/include -I $(ARCH)/riscv/sun8i-v85x

SRCS += $(ARCH)/riscv/trap.c
SRCS += $(ARCH)/riscv/sun8i-v85x/system.c
SRCS += $(ARCH)/riscv/sun8i-v85x/sunxi_irq.c
SRCS += $(ARCH)/riscv/sun8i-v85x/sunxi_gpio.c
SRCS += $(ARCH)/riscv/sun8i-v85x/sunxi_usart.c
SRCS += $(ARCH)/riscv/sun8i-v85x/sunxi_mailbox.c
SRCS += $(ARCH)/riscv/sun8i-v85x/sunxi_openamp.c
SRCS += $(ARCH)/riscv/sun8i-v85x/sunxi_virt_uart.c
ASRCS += $(ARCH)/riscv/sun8i-v85x/vectors.S

ASRCS += $(ARCH)/riscv/sun8i-v85x/startup_gcc.S

LDFLAGS += -T $(ARCH)/riscv/sun8i-v85x/link.ld
