// *** pict-o-crypt.c *************************************************
// this file implements the main loop of pict-o-crypt, where we either
// wait for and then process administrative commands, or walk the
// filesystem and encrypt files.

#include "main.h"

// *** ftdi interface ***

// this is the command line we just received from the FTDI transport
// user console
char *volatile main_command;

// this function is called by the FTDI transport when a new command
// command line has been received from the user.
static void
main_command_cbfn(char *command)
{
    // pass the command to the run loop
    assert(! main_command);
    main_command = command;
}

// this function is called by the FTDI transport when the user presses
// Ctrl-C.
static void
main_ctrlc_cbfn(void)
{
}

// this function is called by the FTDI transport when the USB device
// is reset.
static void
main_reset_cbfn(void)
{
}

// this function implements the main admin loop of pict-o-crypt,
// where we wait for and then process administrative commands.
static
void
main_run_admin(void)
{
    bool ready;
    bool autoend;
    static int first;

    // we just poll here waiting for commands
    for (;;) {
        os_yield();        
        sleep_poll();

        ready = 0;
        autoend = false;
        if (main_command) {
            if (terminal_echo) {
                printf("\n");
            }
            admin_run(main_command);
            ready = 1;
        }

        if (autoend || ready) {
            if (! first) {
                printf(" \n"); delay(1);  // revisit -- why???
                admin_run("help about");
                first = 1;
            }
            printf("> ");
        }

        if (ready) {
            ready = 0;
            if (main_command) {
                main_command = NULL;
                terminal_command_ack(false);
            }
        }
    }
}

// this function implements the main loop of pict-o-crypt.
extern void
main_run(void)
{
    led_unknown();
    
    for (;;) {
        // if our usb device is attached...
        if (scsi_attached) {
            if (! walkfs(0) || ! walkfs(1)) {
                led_sad(code_usb);
                printf("walkfs failed\n");
                DFS_HostFlush(0);
                delay(10000);
            }
            DFS_HostPurge();
            usb_host_detach();
        }

        // if our usb host is attached...
        if (ftdi_attached) {
            main_run_admin();
        }
    }
}

extern void
main_initialize()
{
    params_t params;
    
    // if we're in device mode...
    if (! host_mode) {
        // register device mode callbacks
        terminal_register(main_command_cbfn, main_ctrlc_cbfn);
    #if MCF52221
        ftdi_register(main_reset_cbfn);
    #endif
    }
    
    // set default params
    params_get(&params);
    if (params.generation == -1) {
        memset(&params, 0, sizeof(params));
        params_default_aeskey(&params);
        params_default_files(&params);
        params_set(&params);
    }
}

