// *** ftdi.h *********************************************************

typedef void (*ftdi_command_cbfn)(char *command);
typedef void (*ftdi_ctrlc_cbfn)(void);
typedef void (*ftdi_reset_cbfn)(void);

extern bool ftdi_echo;

int
ftdi_print(char *line);

void
ftdi_edit(char *line);

void
ftdi_command_discard(bool discard);

void
ftdi_command_ack(bool edit);

void
ftdi_command_error(int offset);

void
ftdi_register(ftdi_command_cbfn command, ftdi_ctrlc_cbfn ctrlc, ftdi_reset_cbfn reset);

