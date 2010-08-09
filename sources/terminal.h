typedef void (*terminal_command_cbfn)(char *command);
typedef void (*terminal_ctrlc_cbfn)(void);

extern bool terminal_echo;

void
terminal_print(byte *buffer, int length);

bool
terminal_receive(byte *buffer, int length);

void
terminal_edit(char *line);

void
terminal_command_discard(bool discard);

void
terminal_command_ack(bool edit);

void
terminal_command_error(int offset);

void
terminal_register(terminal_command_cbfn command, terminal_ctrlc_cbfn ctrlc);

