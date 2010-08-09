extern byte *end_of_static;
extern uint32 fsys_frequency;
extern uint32 oscillator_frequency;
extern bool debugger_attached;
extern bool disable_autorun;
extern bool host_mode;
extern uint16 flash_checksum;
#if PICTOCRYPT
extern byte big_buffer[8192];
#else
extern byte big_buffer[2048];
#endif

