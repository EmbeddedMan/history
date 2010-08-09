// *** run.h **********************************************************

#define MAX_TIMERS  4

#if ! GCC
extern int cw7bug;
#endif

extern bool run_step;
extern bool run_sleep;

extern int run_line_number;

extern bool run_condition;

extern bool run_printf;

extern bool running;  // for profiler

extern int run_evaluate(const byte *bytecode_in, int length, IN OUT const char **lvalue_var_name, OUT int *value);

extern bool run_bytecode(bool immediate, const byte *bytecode, int length);
extern bool run_bytecode_code(byte code, bool immediate, const byte *bytecode, int length);

extern void run_clear(bool flash);

extern bool run(bool cont, int start_line_number);
extern void stop();

