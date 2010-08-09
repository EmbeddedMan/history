#if MCF52221 || MCF52259 || MCF51JM128 || PIC32
// *** ftdi.c *********************************************************
// this file implements the FTDI transport (on top of the usb driver
// module).

#include "main.h"

#define PACKET_SIZE  64

#define FTDI_VID  0x0403
#define FTDI_PID  0x6001
#define FTDI_RID  0x0400

#define FTDI_RESET  0
#define FTDI_MODEM_CTRL  1
#define FTDI_SET_FLOW_CTRL  2
#define FTDI_SET_BAUD_RATE  3
#define FTDI_SET_DATA  4
#define FTDI_GET_MODEM_STATUS  5
#define FTDI_SET_EVENT_CHAR  6
#define FTDI_SET_ERROR_CHAR  7
#define FTDI_SET_LATENCY 9
#define FTDI_GET_LATENCY 10

static const byte ftdi_device_descriptor[] = {
    18,  // length
    0x01,  // device descriptor
    0x10, 0x01,  // 1.1
    0x00, 0x00, 0x00,  // class, subclass, protocol
    PACKET_SIZE,  // packet size
    FTDI_VID%0x100, FTDI_VID/0x100, FTDI_PID%0x100, FTDI_PID/0x100,
    FTDI_RID%0x100, FTDI_RID/0x100,
    0x01,  // manufacturer (string)
    0x02,  // product (string)
    0x00,  // sn (string)
    0x01  // num configurations
};

static const byte ftdi_configuration_descriptor[] = {
    9,  // length
    0x02,  // configuration descriptor
    32, 0,  // total length
    0x01,  // num interfaces
    0x01,  // configuration value
    0x00,  // configuration (string)
    0x80,  // attributes
    250,  // 500 mA

    9,  // length
    0x04,  // interface descriptor
    0x00,  // interface number
    0x00,  // alternate
    0x02,  // num endpoints
    0xff,  // vendor
    0xff,  // subclass
    0xff,  // protocol
    0x00,  // interface (string)

    7,  // length
    0x05,  // endpoint descriptor
    0x81,  // endpoint IN address
    0x02,  // attributes: bulk
    0x40, 0x00,  // packet size
    0x00,  // interval (ms)

    7,  // length
    0x05,  // endpoint descriptor
    0x02,  // endpoint OUT address
    0x02,  // attributes: bulk
    0x40, 0x00,  // packet size
    0x00,  // interval (ms)
};

static const byte ftdi_string_descriptor[] = {
    4,  // length
    0x03, // string descriptor
    0x09, 0x04,  // english (usa)

#if PICTOCRYPT
    26,  // length
    0x03,  // string descriptor
    'P', 0, 'i', 0, 'c', 0, 't', 0, '-', 0, 'o', 0, '-', 0, 'C', 0, 'r', 0, 'y', 0, 'p', 0, 't', 0,

    26,  // length
    0x03,  // string descriptor
    'P', 0, 'i', 0, 'c', 0, 't', 0, '-', 0, 'o', 0, '-', 0, 'C', 0, 'r', 0, 'y', 0, 'p', 0, 't', 0,
#else
    28,  // length
    0x03,  // string descriptor
    'R', 0, 'i', 0, 'c', 0, 'h', 0, ' ', 0, 'T', 0, 'e', 0, 's', 0, 't', 0, 'a', 0, 'r', 0, 'd', 0, 'i', 0,

    18,  // length
    0x03,  // string descriptor
    'C', 0, 'P', 0, 'U', 0, 'S', 0, 't', 0, 'i', 0, 'c', 0, 'k', 0,
#endif
};

bool ftdi_active;

static ftdi_reset_cbfn reset_cbfn;

static byte tx[PACKET_SIZE];  // packet from host

#define NRX  4

static byte rx[NRX][PACKET_SIZE]; // packets to host, including 2 byte headers
static int rx_length[NRX];

static byte rx_in;
static byte rx_out;

static bool discard;  // true when we don't think anyone is listening

//static int print_msecs;

// this function waits for space to be available in the transport
// buffers and then prints the specified line to the FTDI transport
// console.
void
ftdi_print(const byte *buffer, int length)
{
    int m;
    int x;
    bool start;
    static uint32 attached_count;
    
    assert(gpl() == 0);
    
    //print_msecs = msecs;

    if (! ftdi_attached || discard) {
        return;
    }

    m = 0;
    while (rx_in != rx_out) {
        delay(1);
        if (m++ > 1000) {
            discard = true;
            return;
        }
    }
    
    if (! length) {
        return;
    }
    
    // revisit -- without this delays, we can get usb hangs on boot
    if (attached_count != ftdi_attached_count) {
        delay(100);
        attached_count = ftdi_attached_count;
    }

    x = splx(7);

    start = rx_in == rx_out;

    // append to next rx_in(s)
    do {
        m = MIN(length, PACKET_SIZE-rx_length[rx_in]);

        assert(rx_length[rx_in]+m <= sizeof(rx[rx_in]));
        memcpy(rx[rx_in]+rx_length[rx_in], buffer, m);
        rx_length[rx_in] += m;

        buffer += m;
        length -= m;

        if (start || length) {
            if (length) {
                assert(rx_length[rx_in] == sizeof(rx[rx_in]));
            }
            rx_in = (rx_in+1)%NRX;
            assert(rx_in != rx_out);
            assert(rx_length[rx_in] == 2);
        }
    } while (length);

    if (start) {
        // start the rx ball rolling
        assert(rx_out != rx_in);
        assert(rx_length[rx_out] >= 2);
        usb_device_enqueue(bulk_in_ep, 1, rx[rx_out], rx_length[rx_out]);
    }

    splx(x);
}

// this function implements the FTDI usb setup control transfer.
static int
ftdi_control_transfer(struct setup *setup, byte *buffer, int length)
{
    switch(setup->request) {
        case FTDI_RESET:
            // revisit -- what to do?
            length = 0;
            break;
        case FTDI_MODEM_CTRL:
            length = 0;
            break;
        case FTDI_SET_FLOW_CTRL:
            length = 0;
            break;
        case FTDI_SET_BAUD_RATE:
            length = 0;
            break;
        case FTDI_SET_DATA:
            length = 0;
            break;
        case FTDI_GET_MODEM_STATUS:
            assert(length == 2);
            buffer[0] = 0xf0;  // RLSD, RI, DSR, CTS asserted
            buffer[1] = 0;
            break;
        case FTDI_SET_EVENT_CHAR:
            length = 0;
            break;
        case FTDI_SET_ERROR_CHAR:
            length = 0;
            break;
        case FTDI_SET_LATENCY:
            length = 0;
            break;
        case FTDI_GET_LATENCY:
            assert(length == 1);
            buffer[0] = 1; // XXX: just a guess
            break;
        case 0x90:  // read eeprom?
            assert(length == 2);
            buffer[0] = 0;
            buffer[1] = 0;
            break;
        case 0x91:  // write eeprom?
            assert(length == 0);
            break;
        case 0x92:  // erase eeprom?
            assert(length == 0);
            break;
        default:
            assert(0);
            length = 0;
            break;
    }
    return length;
}

static bool waiting;

// this function acknowledges receipt of an FTDI command from upper
// level code.
void
ftdi_command_ack(void)
{
    int x;

    x = splx(7);

    if (waiting) {
        // start the tx ball rolling
        usb_device_enqueue(bulk_out_ep, 0, tx, sizeof(tx));
        waiting = false;
    }
    
    splx(x);
}

//void
//ftdi_poll(void)
//{
//    char buffer[2];
//    if ((msecs - print_msecs) >= 2000) {
//    buffer[0] = '.';
//    buffer[1] = ('Q'-'@');  // resume
//        ftdi_print((byte *)".", 2);
//        print_msecs = msecs;
//    }
//}

// this function implements the FTDI usb bulk transfer.
static int
ftdi_bulk_transfer(bool in, byte *buffer, int length)
{
    if (! in) {
        discard = false;
    
        ftdi_active = true;
        
        // accumulate commands
        if (terminal_receive(buffer, length)) {
            // keep the tx ball rolling
            usb_device_enqueue(bulk_out_ep, 0, tx, sizeof(tx));
        } else {
            // drop the ball
            waiting = true;            
        }
    } else {
        rx_length[rx_out] = 2;
        rx_out = (rx_out+1)%NRX;

        // if there is more data to transfer...
        if (rx_length[rx_out] > 2) {
            if (rx_in == rx_out) {
                rx_in = (rx_in+1)%NRX;
                assert(rx_in != rx_out);
                assert(rx_length[rx_in] == 2);
            }

            // keep the rx ball rolling
            assert(rx_out != rx_in);
            assert(rx_length[rx_out] > 2);
            usb_device_enqueue(bulk_in_ep, 1, rx[rx_out], rx_length[rx_out]);
        }
    }

    return 0;
}

// this function is called by the usb driver when the USB device
// is reset.
static void
ftdi_reset(void)
{
    int i;

    for (i = 0; i < NRX; i++) {
        rx[i][0] = 0xf1;  // ftdi header
        rx[i][1] = 0x61;
        rx_length[i] = 2;
    }

    // start the tx ball rolling
    usb_device_enqueue(bulk_out_ep, 0, tx, sizeof(tx));

    assert(reset_cbfn);
    reset_cbfn();
}

static int
check(const byte *descriptor, int length)
{
    int i;
    int j;

    i = 0;
    j = 0;
    while (i < length) {
        i += descriptor[i];
        j++;
    }
    assert(i == length);
    return j;
}

// this function is called by upper level code to register callback
// functions.
void
ftdi_register(ftdi_reset_cbfn reset)
{
    int i;

    for (i = 0; i < NRX; i++) {
        rx[i][0] = 0xf1;  // ftdi header
        rx[i][1] = 0x61;
        rx_length[i] = 2;
    }

    reset_cbfn = reset;

    usb_register(ftdi_reset, ftdi_control_transfer, ftdi_bulk_transfer);

    assert(check(ftdi_device_descriptor, sizeof(ftdi_device_descriptor)) == 1);
    usb_device_descriptor(ftdi_device_descriptor, sizeof(ftdi_device_descriptor));

    assert(check(ftdi_configuration_descriptor, sizeof(ftdi_configuration_descriptor)) == 4);
    usb_configuration_descriptor(ftdi_configuration_descriptor, sizeof(ftdi_configuration_descriptor));

    assert(check(ftdi_string_descriptor, sizeof(ftdi_string_descriptor)) == 3);
    usb_string_descriptor(ftdi_string_descriptor, sizeof(ftdi_string_descriptor));
}
#endif

