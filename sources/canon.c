#include "main.h"

#if CANON
// perform a usb host/device canon control transfer
int
canon_control_transfer(int in, int value, byte *buffer, int length)
{
    byte setup[SETUP_SIZE];

    usb_setup(in, SETUP_TYPE_VENDOR, SETUP_RECIP_DEVICE, length>1?0x04:0x0c, value, 0, length, setup);
    return usb_control_transfer(setup, buffer, length);
}

// perform a usb host/device canon bulk transfer
int
canon_bulk_transfer(int cmd1, int cmd2, int cmd3, byte *cmd, int cmd_length, byte *response, int response_length)
{
    int rv;
    byte buffer[0x100];
    static int serial;

    assert(0x50+cmd_length < sizeof(buffer));
    assert(0x54+response_length < sizeof(buffer));

    memset(buffer, 0, 0x50);
    buffer[0] = (char)((0x50+cmd_length)-0x40);
    buffer[4] = (char)(cmd3%0x100);
    buffer[5] = (char)(cmd3/0x100);
    buffer[0x40] = (char)0x02;
    buffer[0x44] = (char)cmd1;
    buffer[0x47] = (char)cmd2;
    buffer[0x48] = (char)((0x50+cmd_length)-0x40);
    buffer[0x4c] = (char)++serial;

    memcpy(buffer+0x50, cmd, cmd_length);

    rv = canon_control_transfer(0, 0x10, buffer, (0x50+cmd_length));
    if (rv < 0) {
        return rv;
    }
    assert(rv == (0x50+cmd_length));

    rv = usb_bulk_transfer(1, buffer, (0x54+response_length), 0);
    if (rv < 0) {
        return rv;
    }
    assert(rv >= 0x54);

    if (*(int *)(buffer+0x50)) {
        return -1;
    }

    rv -= 0x54;
    assert(rv <= response_length);
    memcpy(response, buffer+0x54, rv);
    return rv;
}

void
canon_run(void)
{
    int i;
    int rv;
    byte c;
    int once;
    int secs;
    byte cmd[16];
    byte response[16];
    byte buffer[0x80];

    once = 0;
    secs = 0;
    for (;;) {
        // see: http://www.graphics.cornell.edu/~westin/canon/ch03.html#sec.USBCameraInit

        if (! once) {
            // was the camera awoken or already awake?
            rv = canon_control_transfer(1, 0x55, &c, sizeof(c));
            assert(rv == 1);
            assert(c == 'C' || c == 'A');

            // what kind of a camera is it?
            memset(buffer, 0, 0x58);
            rv = canon_control_transfer(1, 0x01, buffer, 0x58);
            assert(rv == 0x58);

            if (c == 'A') {
                rv = canon_control_transfer(1, 0x04, buffer, sizeof(buffer));
                assert(rv == 0x80);
            } else {
                memset(buffer, 0, 0x40);
                buffer[0] = 0x10;
                memmove(buffer+0x40, buffer+0x48, 0x10);
                rv = canon_control_transfer(0, 0x11, buffer, 0x50);
                assert(rv == 0x50);

                delay(100);

                rv = usb_bulk_transfer(1, buffer, 0x40, 0);
                assert(rv == 0x40);
                delay(10);
                rv = usb_bulk_transfer(1, buffer, 0x40, 0);
                assert(rv == 0x4);

                rv = usb_bulk_transfer(-1, buffer, 0x10, 0);
                assert(rv == 0x10);
            }

            rv = canon_bulk_transfer(0x20/*lock*/, 0x12, 0x201, NULL, 0, NULL, 0);
            if (rv == -1) {
                break;
            }
            assert(rv == 0);
            led_happy();

            delay(100);

            memset(cmd, 0, sizeof(cmd));
            cmd[0] = 0x00;  // initialize
            rv = canon_bulk_transfer(0x13, 0x12, 0x201, cmd, 8, response, 8);
            if (rv == -1) {
                break;
            }

            memset(cmd, 0, sizeof(cmd));
            cmd[0] = 0x09;  // set transfer mode
            cmd[4] = 0x04;
            cmd[8] = 0x0c;
            rv = canon_bulk_transfer(0x13, 0x12, 0x201, cmd, 12, response, 8);
            if (rv == -1) {
                break;
            }

            once = 1;
        }

        rv = canon_bulk_transfer(0x01/*identify*/, 0x12, 0x201, NULL, 0, buffer, 72);
        if (rv == -1) {
            break;
        }
        assert(rv == 72);
        led_happy();

        if (ticks/1000 > secs+10) {
            secs = ticks/1000;

            memset(cmd, 0, sizeof(cmd));
            cmd[0] = 0x04;  // release shutter
            rv = canon_bulk_transfer(0x13, 0x12, 0x201, cmd, 8, response, 8);
            if (rv == -1) {
                break;
            }

            for (i = 0; i < 2; i++) {
                do {
                    delay(100);
                    rv = usb_bulk_transfer(-1, buffer, 0x10, 0);
                } while (rv == 0);  // wait
                if (rv == -1) {
                    break;
                }
            }
        }
    }
}
#endif
