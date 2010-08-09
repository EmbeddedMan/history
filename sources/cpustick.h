// *** cpustick.h *****************************************************

extern char *volatile cpustick_ready;

extern bool cpustick_edit;
extern bool cpustick_prompt;

extern void
cpustick_run(void);

extern void
cpustick_register(void);

