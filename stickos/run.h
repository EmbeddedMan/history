// *** run.h **********************************************************

#define MAX_TIMERS  4

#define TIMER_INTS  MAX_TIMERS

enum {
    num_watchpoints = 4, // tunable number of watchpoints.  values >8 will require minor widening of bit fields.
    all_watchpoints_mask = (1<<num_watchpoints)-1 // mask of all watchpoints.
};

#if ! GCC
extern int cw7bug;
#endif

extern bool run_step;
extern bool run_sleep;

extern int run_line_number;

extern bool run_condition;

extern bool run_printf;

extern bool running;  // for profiler

extern uint32 possible_watchpoints_mask;
extern bool watch_mode_smart;

extern int run_evaluate(const byte *bytecode_in, int length, IN OUT const char **lvalue_var_name, OUT int32 *value);

extern bool run_bytecode(bool immediate, const byte *bytecode, int length);
extern bool run_bytecode_code(uint code, bool immediate, const byte *bytecode, int length);

extern void run_clear(bool flash);

extern bool run(bool cont, int start_line_number);
extern void stop(void);

extern void run_initialize(void);
