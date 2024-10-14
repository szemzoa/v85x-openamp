#include "debug.h"
#include "main.h"

#ifdef CONFIG_ENABLE_LOG_TRACE
static int offset = 0;
extern char log_trace_buffer[LOG_TRACE_BUFFER_SIZE];
#endif

#if 0
void debug_putc(void *arg, char c)
{
#ifdef CONFIG_ENABLE_LOG_TRACE
        if (offset + 1 >= LOG_TRACE_BUFFER_SIZE)
                offset = 0;

        log_trace_buffer[offset] = c;
        log_trace_buffer[offset++ + 1] = '\0';
#else
        while ((read32(0x02500800 + 0x7c) & (0x1 << 1)) == 0)
                ;

        write32(0x02500800 + 0x00, c);

        while ((read32(0x02500800 + 0x14) & (0x1 << 6)))
                ;
#endif
}

void debug(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	xvformat(debug_putc, NULL, fmt, args);
	va_end(args);
}
#endif

void _putchar(char c)
{
#ifdef CONFIG_ENABLE_LOG_TRACE
        if (offset + 1 >= LOG_TRACE_BUFFER_SIZE)
                offset = 0;

        log_trace_buffer[offset] = c;
        log_trace_buffer[offset++ + 1] = '\0';
#else
        while ((read32(0x02500800 + 0x7c) & (0x1 << 1)) == 0)
                ;

        write32(0x02500800 + 0x00, c);

        while ((read32(0x02500800 + 0x14) & (0x1 << 6)))
                ;
#endif

}

void debug(const char *fmt, ...)
{
        va_list args;

        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
}