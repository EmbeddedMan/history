// *** printf.h *******************************************************

#if BADGE_BOARD
extern bool printf_scroll;
#endif

int
printf_write(char *buffer, int n);

bool
open_log_file(void);

bool
append_log_file(char *buffer);

void
flush_log_file(void);

int
snprintf(char *buffer, int length, const char *format, ...);

int
sprintf(char *buffer, const char *format, ...);

int
vsprintf(char *buffer, const char *format, va_list ap);

int
printf(const char *format, ...);

int
vprintf(const char *format, va_list ap);

#if IN_MEMORY_TRACE

void
trace(const char *fmt, ...);

void
trace_reset(void);

#endif // IN_MEMORY_TRACE
