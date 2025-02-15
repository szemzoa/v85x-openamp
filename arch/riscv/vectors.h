#ifndef CPUPORT_H__
#define CPUPORT_H__

#ifdef ARCH_RISCV_64
#define DFSTORE                 fsd
#define DFLOAD                  fld
#define SFSTORE                 fsw
#define SFLOAD                  flw
#define STORE                   sd
#define LOAD                    ld
#define REGBYTES                8
#define SFREGBYTES              4
#define DFREGBYTES              8
#else
#define DFSTORE                 fsd
#define DFLOAD                  fld
#define SFSTORE                 fsw
#define SFLOAD                  flw
#define STORE                   sw
#define LOAD                    lw

#define REGBYTES                4
#define SFREGBYTES              4
#define DFREGBYTES              8
#endif

#ifdef CONFIG_ARCH_RISCV_FPU

#ifdef CONFIG_FPU_DOUBLE
#define FSTORE                  fsd
#define FLOAD                   fld
#define FREGBYTES               8
#define rv_floatreg_t           rt_int64_t
#else
#define FSTORE                  fsw
#define FLOAD                   flw
#define FREGBYTES               4
#define rv_floatreg_t           rt_int32_t
#endif
#endif

#endif



