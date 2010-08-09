// *** cpustick.c *****************************************************
// this file implements the main program loop of stickos, where we
// wait for and then process (elsewhere) stickos commands.

#include "main.h"

// this is the command line we just received from the FTDI transport
// user console
char *volatile cpustick_ready;

bool cpustick_edit;
bool cpustick_prompt = true;

// this function is called by the FTDI transport when a new command
// command line has been received from the user.
static void
cpustick_command(char *command)
{
    // pass the command to the run loop
    assert(! cpustick_ready);
    cpustick_ready = command;
}

// this function is called by the FTDI transport when the user presses
// Ctrl-C.
static void
cpustick_ctrlc(void)
{
    stop();
}

// this function is called by the FTDI transport when the USB device
// is reset.
static void
cpustick_reset(void)
{
}


static int autoran;

// this function implements the main program loop of stickos, where we
// wait for and then process (elsewhere) stickos commands.extern void
void
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
            if (ftdi_echo) {
                printf("\n");
            }
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
            if (cpustick_prompt) {
                printf("> ");
            } else {
                printf("done\n");
            }
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

// this function is called by upper level code to register callback
// functions.
extern void
cpustick_register(void)
{
    ftdi_register(cpustick_command, cpustick_ctrlc, cpustick_reset);
}

