// *** run.h **********************************************************

#define MAX_TIMERS  4

extern int cw7bug;

extern bool run_step;
extern bool run_sleep;

extern int run_line_number;

extern bool run_condition;

extern bool run_printf;

extern bool running;  // for profiler

extern int run_evaluate(byte *bytecode_in, int length, OUT int *value);

extern bool run_bytecode(bool immediate, byte *bytecode, int length);
extern bool run_bytecode_code(byte code, bool immediate, byte *bytecode, int length);

extern void run_clear(bool flash);

extern bool run(bool cont, int start_line_number);
extern void stop();

