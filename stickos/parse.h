// *** parse.h ********************************************************

void parse_trim(IN char **p);
bool parse_char(IN OUT char **text, IN char c);
bool parse_word(IN OUT char **text, IN char *word);

bool parse_line(IN char *text, OUT int *length, OUT byte *bytecode, OUT int *syntax_error);
void unparse_bytecode(IN byte *bytecode, IN int length, OUT char *text);

