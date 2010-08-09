// *** parse ****************************************************************

bool parse_line(IN char *text, OUT int *length, OUT byte *bytecode, OUT int *syntax_error);
void unparse_bytecode(IN byte *bytecode, IN int length, OUT char *text);
