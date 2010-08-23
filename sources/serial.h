// *** serial.h ************************************************************

extern bool serial_active;

extern int serial_baudrate;

void
serial_disable(void);

void
serial_command_ack(void);

#if ! PIC32
INTERRUPT
void
serial_isr(void);
#endif

void
serial_send(const byte *buffer, int length);

void
serial_initialize(void);

