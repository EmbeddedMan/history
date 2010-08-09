// *** parse.h ********************************************************

void parse_trim(IN char **p);
bool parse_char(IN OUT char **text, IN char c);
bool parse_word(IN OUT char **text, IN const char *word);
bool parse_words(IN OUT char **text_in, IN const char *words);
char *parse_find_keyword(IN OUT char *text, IN char* word);
bool parse_const(IN OUT char **text, IN OUT int *length, IN OUT byte *bytecode);
char *parse_match_paren(char *p);
char *parse_match_quote(char *p);

bool parse_var(IN bool lvalue, IN int obase, IN bool allow_array_index, IN OUT char **text, IN OUT int *length, IN OUT byte *bytecode);
int unparse_var_lvalue(byte *bytecode_in, char **out);

bool parse_expression(IN int obase, IN OUT char **text, IN OUT int *length, IN OUT byte *bytecode);
int unparse_expression(int tbase, byte *bytecode_in, int length, char **out);

bool parse_line(IN char *text, OUT int *length, OUT byte *bytecode, OUT int *syntax_error);
bool parse_line_code(IN byte code, IN char *text, OUT int *length, OUT byte *bytecode, OUT int *syntax_error);
void unparse_bytecode(IN byte *bytecode, IN int length, OUT char *text);
void unparse_bytecode_code(IN byte code, IN byte *bytecode, IN int length, OUT char *text);

