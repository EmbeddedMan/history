// *** code *****************************************************************

struct line {
    int line_number;
    int size;  // of entire struct, rounded up to 4 byte boundary (used internally)
    int length;  // of bytecode (used externally)
    byte bytecode[VARIABLE];
};

#define LINESIZE  OFFSETOF(struct line, bytecode[0])

// used for code execution
struct line *code_next_line(IN bool deleted_ok, IN OUT int *line_number);  // returns NULL at eop

int code_sub_line(byte *sub_name);

// used for code management
void code_insert(IN int line_number, IN char *text, IN int text_offset);
void code_edit(int line_number_in);
void code_list(int start_line_number, int end_line_number);
void code_delete(int start_line_number, int end_line_number);
void code_save(int renum);  // coalesce ram/flash and save to alt flash and flip flash flag
void code_new(void);
void code_undo(void);
void code_mem(void);

void code_store(char *name);
void code_load(char *name);
void code_dir();
void code_purge(char *name);

void code_initialize(void);
