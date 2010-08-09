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
static
void
main_command_cbfn(char *command)
{
    // pass the command to the run loop
    assert(! main_command);
    main_command = command;
}

// this function is called by the FTDI transport when the user presses
// Ctrl-C.
static
void
main_ctrlc_cbfn(void)
{
}

// this function is called by the FTDI transport when the USB device
// is reset.
static
void
main_reset_cbfn(void)
{
}

bool panic;

static
bool
do_scsi_panic(void)
{
    int n;
    int rv;
    int lba;
    int max;
    byte buf[8];
    byte cdb[10];
    static uint32 b = 0xff33cc00;
    
    led_unknown();

    (void)PhysReadWriteSector(true, big_buffer, 0, sizeof(big_buffer)/SECTOR_SIZE);

    // we'll write 00, then cc, then 33, then ff
    memset(big_buffer, b&0xff, sizeof(big_buffer));
    
    // read capacity
    memset(cdb, 0, sizeof(cdb));
    cdb[0] = 0x25;  // read capacity
    rv = scsi_bulk_transfer(1, cdb, 10, buf, 8);
    assert(rv == 8);
    assert(*(int *)(buf+4) == 0x200);
    max = *(int *)buf;
    
    for (lba = 0; lba <= max; lba += n) {
        // if we're not near the end of the disk...
        if (lba < max - 2*sizeof(big_buffer)/SECTOR_SIZE) {
            // go fast
            n = sizeof(big_buffer)/SECTOR_SIZE;
        } else {
            // go slow and ignore errors (some drives misreport their size)
            n = 1;
        }
        rv = PhysReadWriteSector(false, big_buffer, lba, n);
        
        // if the write failed and we're not near the end of the disk...
        if (rv && n != 1) {
            return false;
        } else {
            led_happy_progress();
        }
        
    }
    DFS_HostFlush(0);
    
    b >>= 8;
    if (! b) {
        b = 0xff33cc00;
    }

    return true;
}

static
void
do_scsi_wait(void)
{
    int rv;
    byte cdb[6];
    byte buf[36];
    
    // wait for usb disconnect
    DFS_HostFlush(0);
    DFS_HostPurge();
    
    do {
        if (panic) {
            if (! do_scsi_panic()) {
                DFS_HostFlush(0);
                led_sad(code_usb);
                printf("panic failed\n");
            } else {
                led_happy();
            }
            panic = false;
        }

        sleep_poll();
        delay(1000);

        // inquiry
        memset(cdb, 0, sizeof(cdb));
        cdb[0] = 0x12;  // inquiry
        cdb[4] = 36;
        rv = scsi_bulk_transfer(1, cdb, 6, buf, 36);
    } while (rv == 36);
}

static
void
do_other_wait(void)
{
    int rv;
    struct setup setup;
    byte configuration[18];
    
    do {
        sleep_poll();
        delay(1000);

        // get the configuration descriptor
        usb_setup(1, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_GET_DESCRIPTOR, (CONFIGURATION_DESCRIPTOR<<8)|0, 0, sizeof(configuration), &setup);
        rv = usb_control_transfer(&setup, configuration, sizeof(configuration));
    } while (rv > 0);
}

// if we see the pause state radically changing, we panic and wipe
// the card!
void
main_poll(void)
{
    bool pause;
    static int changes;
    static int last_pause;
    static int last_seconds;
    
    pause = (MCF_GPIO_SETNQ & 0x10) == 0;
    if (pause != last_pause) {
        // N.B. we can't panic without a card attached
        if (scsi_attached) {
            changes++;
        }
        last_pause = pause;
    }
    if (seconds != last_seconds) {
        if (changes) {
            changes--;
        }
        last_seconds = seconds;
    }
    if (changes >= 6) {
        panic = true;
    }
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
void
main_run(void)
{
    // go to sleep SLEEP_DELAY seconds after progress stops
    sleep_delay(SLEEP_DELAY);
    
    for (;;) {
        led_unknown();
        
        // if our usb device is attached...
        if (scsi_attached) {
            if (! walkfs(0) || ! walkfs(1)) {
                DFS_HostFlush(0);
                led_sad(code_usb);
                printf("walkfs failed\n");
            } else {
                led_happy();
            }
            
            do_scsi_wait();
            usb_host_detach();
        }
        
        // if some other usb device is attached...
        if (other_attached) {
            do_other_wait();
            usb_host_detach();
        }

        // if our usb host is attached...
        if (ftdi_attached) {
            main_run_admin();
        }

        os_yield();
        sleep_poll();
    }
}

void
main_initialize()
{
    params_t params;
    
    // if we're in device mode...
    if (! usb_host_mode) {
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

