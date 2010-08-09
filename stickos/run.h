// *** run.h **********************************************************

#define MAX_TIMERS  4

#define MAX_UARTS  2
#define UART_INTS  (2*MAX_UARTS)
#define UART_INT(uart, output)  ((uart)*2+output)

extern int cw7bug;

extern bool uart_armed[UART_INTS];

extern bool run_step;
extern int run_line_number;

extern bool run_condition;

extern bool run_bytecode(bool immediate, byte *bytecode, int length);
extern bool run(bool cont, int start_line_number);
extern void stop();

