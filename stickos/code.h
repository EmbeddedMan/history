// *** code.h *********************************************************

struct line {
    int line_number;
    int size;  // of entire struct, rounded up to 4 byte boundary (used internally)
    int length;  // of bytecode (used externally)
    byte bytecode[VARIABLE];
};

#define LINESIZE  OFFSETOF(struct line, bytecode[0])


extern bool code_indent;

// used for code execution
struct line *code_next_line(IN bool deleted_ok, IN OUT int *line_number);  // returns NULL at eop

struct line *code_line(enum bytecode code, const byte *name);

// used for code management
void code_insert(IN int line_number, IN char *text, IN int text_offset);
void code_edit(int line_number_in);
void code_list(bool profile, int start_line_number, int end_line_number);
void code_delete(int start_line_number, int end_line_number);
void code_save(int renum);  // coalesce ram/flash and save to alt flash and flip flash flag
void code_new(void);
void code_undo(void);
void code_mem(void);

void code_store(char *name);
void code_load(char *name);
void code_dir();
void code_purge(char *name);

// our profiler
void code_timer_poll(void);

void code_clear2(void);

void code_initialize(void);

