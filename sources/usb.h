// *** usb ******************************************************************

#define SETUP_SIZE  8

#define SETUP_TYPE_STANDARD  0
#define SETUP_TYPE_CLASS  1
#define SETUP_TYPE_VENDOR  2

#define SETUP_RECIP_DEVICE  0
#define SETUP_RECIP_INTERFACE  1
#define SETUP_RECIP_ENDPOINT  2

extern struct endpoint {
    byte toggle[2];  // rx [0] and tx [1] next packet data0 (0) or data1 (BD_FLAGS_DATA)
    byte packetsize;
} endpoints[16];

extern bool scsi_attached;  // set when usb mass storage device is attached
extern bool pima_attached;  // set when usb pima camera device is attached
extern bool canon_attached;  // set when usb canon camera device is attached

extern byte bulk_in_ep;
extern byte bulk_out_ep;
extern byte int_ep;

// initialize a setup data0 buffer
void
usb_setup(int in, int type, int recip, int request, int value, int index, int length, byte *setup);

// perform a usb host/device control transfer
int
usb_control_transfer(byte *setup, byte *buffer, int length);

// perform a usb host/device bulk transfer
int
usb_bulk_transfer(int in, byte *buffer, int length, bool null_or_short);

// detach from the device and prepare to re-attach
void
usb_detach(void);

void
usb_initialize(void);
