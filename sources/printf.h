// N.B. we add buffering to the default printf since we can only process
//      a few BDM accesses per second thru the "Trap #14" mechanism... :-(

int
printf(char *format, ...);
