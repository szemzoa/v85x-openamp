#ifndef __MAIN_H__
#define __MAIN_H__

#include <inttypes.h>
#include "debug.h"

//#define CONFIG_ENABLE_CONSOLE

#define CONFIG_ENABLE_LOG_TRACE
#define LOG_TRACE_BUFFER_SIZE          2048

#define DEBUG_RISCV

#ifdef DEBUG_RISCV
#define debug_riscv(...)      \
        do {                    \
                debug("v85x-riscv: ");     \
                debug(__VA_ARGS__); \
        } while (0)
#else
#define debug_riscv(...)
#endif

#endif
