// *** ftdi.h *********************************************************

typedef void (*ftdi_reset_cbfn)(void);

int
ftdi_print(char *line);

void
ftdi_send(byte *line, int length);

void
ftdi_command_ack(void);

void
ftdi_register(ftdi_reset_cbfn reset);  // revisit -- register receive upcall!

