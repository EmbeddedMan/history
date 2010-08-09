// *** pict-o-crypt.h *************************************************

extern void
getaeskey(IN OUT char **text, OUT bool *hexkey, OUT byte aeskey[AESBITS/8]);

extern void
admin_run(char *text);

extern void
admin_initialize();

