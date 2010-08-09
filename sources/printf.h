// *** printf.h *******************************************************

#if BADGE_BOARD
extern bool printf_scroll;
#endif

int
snprintf(char *buffer, unsigned long length, const char *format, ...);

int
sprintf(char *buffer, const char *format, ...);

int
printf(const char *format, ...);

int
vprintf(const char *format, va_list ap);

int
vsprintf(char *outbuf, const char *format, va_list ap);

