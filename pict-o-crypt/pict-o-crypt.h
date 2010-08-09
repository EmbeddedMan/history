// *** pict-o-crypt.h *************************************************

#define AESBITS  256

extern char *volatile main_command;

extern bool panic;

void
main_timer_poll(void);

extern void
main_run(void);

extern void
main_initialize();

