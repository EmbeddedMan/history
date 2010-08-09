#include "main.h"

// *** cpustick *************************************************************

#if CPUSTICK

char *cpustick_ready;

bool cpustick_edit;

static void
cpustick_command(char *command)
{
    // pass the command to the run loop
    assert(! cpustick_ready);
    cpustick_ready = command;
}

static void
cpustick_ctrlc(void)
{
    stop();
}

static int autoran;

static void
cpustick_reset(void)
{
}

extern void
cpustick_run(void)
{
    bool ready;
    bool autoend;
    static int first;

    // we just poll here waiting for commands
    for (;;) {
        sleep_poll();
        
        ready = 0;
        autoend = false;
        if (! autoran && ! led_disable_autorun && var_get_flash(FLASH_AUTORUN) == 1) {
            basic_run("run");
            autoend = true;
        } else if (cpustick_ready) {
            printf("\n");
            basic_run(cpustick_ready);
            ready = 1;
        }
        autoran = true;
        
        if (autoend || ready) {
            if (! first) {
            	printf(" \n"); delay(1);  // revisit -- why???
                basic_run("help about");
                first = 1;
            }
            printf("> ");
        }
                
        if (ready) {
            ready = 0;
            if (cpustick_ready) {
                cpustick_ready = NULL;
                ftdi_command_ack(cpustick_edit);
                cpustick_edit = false;
            }
        }
    }
}

extern void
cpustick_register(void)
{
    ftdi_register(cpustick_command, cpustick_ctrlc, cpustick_reset);
}

#endif
