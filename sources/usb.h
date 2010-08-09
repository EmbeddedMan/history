// *** usb.h **********************************************************

#define SETUP_SIZE  8

struct setup {
    byte requesttype;  // 7:in, 6-5:type, 4-0:recip
    byte request;
    short value;
    short index;
    short length;
};

extern bool usb_in_isr;  // set when in isr

extern bool usb_device_configured;  // set when usb device is configured

extern byte bulk_in_ep;
extern byte bulk_out_ep;
extern byte int_ep;

// enqueue a packet to the usb engine for transfer to/from the host
void
usb_device_enqueue(int endpoint, bool tx, byte *buffer, int length);

typedef void (*usb_reset_cbfn)(void);
typedef int (*usb_control_cbfn)(struct setup *setup, byte *buffer, int length);
typedef int (*usb_bulk_cbfn)(bool in, byte *buffer, int length);
typedef void (*usb_interrupt_cbfn)(void);

void
usb_register(usb_reset_cbfn reset, usb_control_cbfn control_transfer, usb_bulk_cbfn bulk_transfer);

void
usb_device_descriptor(byte *descriptor, int length);

void
usb_configuration_descriptor(byte *descriptor, int length);

void
usb_string_descriptor(byte *descriptor, int length);

void
usb_initialize(void);

