// *** basic2.h *******************************************************

enum bytecode2 {
    code_private = code_max,
#if BADGE_BOARD
    code_scroll,
    code_set,
    code_clear,
#endif
    code_max_max
};

// revisit -- move to var2.h???
enum flash_var2 {
#if MCF52233
    FLASH_IPADDRESS = FLASH_NEXT,
#endif
    FLASH_LAST_LAST
};

extern char *help_about;

#if BADGE_BOARD
extern char *help_about_short;
#endif

void basic2_help(IN char *text_in);

bool basic2_run(char *text_in);

