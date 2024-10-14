#ifndef __IO_H__
#define __IO_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* common register access operation. */
#define read8(reg)          		(*(volatile uint8_t  *)(long)(reg))
#define read16(reg)          		(*(volatile uint16_t *)(reg))
#define read32(reg)          		(*(volatile uint32_t *)(reg))
#define write8(reg,value)   		(*(volatile uint8_t  *)(long)(reg) = (value))
#define write16(reg,value)   		(*(volatile uint16_t *)(reg) = (value))
#define write32(reg,value)   		(*(volatile uint32_t *)(reg) = (value))

#endif
