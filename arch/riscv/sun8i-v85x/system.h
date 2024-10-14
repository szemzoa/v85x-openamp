#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#define CONFIG_HZ 			1000

extern void system_init(void);
extern uint32_t system_get_ticks(void);
extern void system_msleep(uint32_t tmo);
extern void system_mdelay(uint32_t tmo);
extern void system_usleep(uint32_t tmo);

#endif
