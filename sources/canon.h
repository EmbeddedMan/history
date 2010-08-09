// perform a usb host/device canon control transfer
int
canon_control_transfer(int in, int value, byte *buffer, int length);

// perform a usb host/device canon bulk transfer
int
canon_bulk_transfer(int cmd1, int cmd2, int cmd3, byte *cmd, int cmd_length, byte *response, int response_length);

void
canon_run(void);
