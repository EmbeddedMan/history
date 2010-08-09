// *** usb ******************************************************************

#define SETUP_SIZE  8

#define SETUP_TYPE_STANDARD  0
#define SETUP_TYPE_CLASS  1
#define SETUP_TYPE_VENDOR  2

#define SETUP_RECIP_DEVICE  0
#define SETUP_RECIP_INTERFACE  1
#define SETUP_RECIP_ENDPOINT  2

struct setup {
    byte requesttype;  // 7:in, 6-5:type, 4-0:recip
    byte request;
    short value;
    short index;
    short length;
};

extern struct endpoint {
    byte toggle[2];  // rx [0] and tx [1] next packet data0 (0) or data1 (BD_FLAGS_DATA)
    byte bdtodd[2];  // rx [0] and tx [1] next bdt even (0) or odd (1)
    byte packetsize;
    bool interrupt;

    byte data_pid;  // TOKEN_IN -> data to host; TOKEN_OUT -> data from host
    int data_offset;  // current offset in data stream
    int data_length;  // max offset in data stream
    byte data_buffer[64];  // data to or from host
} endpoints[4];

extern bool usb_in_isr;  // set when in isr

extern bool scsi_attached;  // set when usb mass storage device is attached
extern bool pima_attached;  // set when usb pima camera device is attached
extern bool canon_attached;  // set when usb canon camera device is attached

extern bool usb_device_configured;  // set when usb device is configured

extern byte bulk_in_ep;
extern byte bulk_out_ep;
extern byte int_ep;

// *** host ***

// initialize a setup data0 buffer
void
usb_setup(int in, int type, int recip, byte request, short value, short index, short length, struct setup *setup);

// perform a usb host/device control transfer
int
usb_control_transfer(struct setup *setup, byte *buffer, int length);

// perform a usb host/device bulk transfer
int
usb_bulk_transfer(int in, byte *buffer, int length, bool null_or_short);

// detach from the device and prepare to re-attach
void
usb_detach(void);

// *** device ***

// enqueue a packet to the usb engine for transfer to/from the host
void
usb_device_enqueue(int endpoint, bool tx, byte *buffer, int length);

typedef void (*usb_reset_cbfn)(void);
typedef int (*usb_control_cbfn)(struct setup *setup, byte *buffer, int length);
typedef int (*usb_bulk_cbfn)(bool in, byte *buffer, int length);
typedef void (*usb_interrupt_cbfn)(void);

void
usb_register(usb_reset_cbfn reset, usb_control_cbfn control_transfer, usb_bulk_cbfn bulk_transfer, usb_interrupt_cbfn interrupt_transfer);

void
usb_device_descriptor(byte *descriptor, int length);

void
usb_configuration_descriptor(byte *descriptor, int length);

void
usb_string_descriptor(byte *descriptor, int length);

void
usb_report_descriptor(int number, byte *descriptor, int length);

// *** init ***

void
usb_initialize(void);
