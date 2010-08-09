// *** serial.h ************************************************************

void
serial_initialize(void);

void
serial_send(const byte *buffer, int length);

INTERRUPT
void
serial_isr(void);
