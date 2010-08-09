#include "main.h"

// *** usb ******************************************************************

#define SWRETRIES  10

#define DEVICE_DESCRIPTOR_SIZE  18
#define CONFIGURATION_DESCRIPTOR_SIZE  128

#define DEVICE_DESCRIPTOR  1
#define CONFIGURATION_DESCRIPTOR  2
#define STRING_DESCRIPTOR  3
#define ENDPOINT_DESCRIPTOR  5

#define BULK_ATTRIBUTES  2
#define INTERRUPT_ATTRIBUTES  3

#define REQUEST_CLEAR_FEATURE  0x01
#define REQUEST_SET_ADDRESS  0x05
#define REQUEST_GET_DESCRIPTOR  0x06
#define REQUEST_SET_CONFIGURATION  0x09

#define FEATURE_ENDPOINT_HALT  0x00

#define TOKEN_OUT  0x01
#define TOKEN_ACK  0x02
#define TOKEN_DATA0  0x03
#define TOKEN_IN  0x09
#define TOKEN_NAK  0x0a
#define TOKEN_DATA1  0x0b
#define TOKEN_SETUP  0x0d
#define TOKEN_STALL  0x0e

#define CLASS_SCSI  0x08
#define CLASS_PIMA  0x06
#define CLASS_CANON  0xff

#define BD_FLAGS_BC_ENC(x)  (((x) & 0x3ff) << 16)
#define BD_FLAGS_BC_DEC(y)  (((y) & 0x3ff0000) >> 16)
#define BD_FLAGS_OWN  0x80
#define BD_FLAGS_DATA  0x40
#define BD_FLAGS_DTS  0x08
#define BD_FLAGS_TOK_PID_DEC(y)  (((y) & 0x3c) >> 2)

#define MYBDT(endpoint, tx, odd)  (bdts+(endpoint)*4+(tx)*2+(odd))

static struct bdt {
    int flags;
    byte *buffer;
} *bdts;  // 512 byte aligned in buffer

// N.B. only bdt endpoint 0 is used for host mode!
static byte bdtbuffer[512+4*4*sizeof(struct bdt)];

struct endpoint endpoints[4];

byte bulk_in_ep;
byte bulk_out_ep;
byte int_ep;

bool usb_in_isr;

bool scsi_attached;  // set when usb mass storage device is attached
bool pima_attached;  // set when usb pima camera device is attached
bool canon_attached;  // set when usb canon camera device is attached

bool usb_device_configured;  // set when device is configured

static
void
parse_configuration(byte *configuration, int size)
{
    int i;

    // extract the bulk endpoint information
    for (i = 0; i < size; i += configuration[i]) {
        if (configuration[i+1] == ENDPOINT_DESCRIPTOR) {
            if (configuration[i+3] == BULK_ATTRIBUTES) {
                if (configuration[i+2] & 0x80) {
                    bulk_in_ep = (byte)(configuration[i+2] & 0xf);
                    assert(configuration[i+4]);
                    endpoints[bulk_in_ep].packetsize = configuration[i+4];
                } else {
                    bulk_out_ep = (byte)(configuration[i+2] & 0xf);
                    assert(configuration[i+4]);
                    endpoints[bulk_out_ep].packetsize = configuration[i+4];
                }
            } else if (configuration[i+3] == INTERRUPT_ATTRIBUTES) {
                int_ep = (byte)(configuration[i+2] & 0xf);
                assert(configuration[i+4]);
                endpoints[int_ep].packetsize = configuration[i+4];
                endpoints[int_ep].interrupt = 1;
            }
        }
    }
    assert(i == size);
}


static byte *device_descriptor;
static int device_descriptor_length;

static byte *configuration_descriptor;
static int configuration_descriptor_length;

static byte *string_descriptor;
static int string_descriptor_length;

static int report_number;
static byte *report_descriptor;
static int report_descriptor_length;

static usb_reset_cbfn reset_cbfn;
static usb_control_cbfn control_transfer_cbfn;
static usb_bulk_cbfn bulk_transfer_cbfn;
static usb_interrupt_cbfn interrupt_transfer_cbfn;

void
usb_device_wait()
{
    // enable usb device mode
    MCF_USB_OTG_CTL = MCF_USB_OTG_CTL_USB_EN_SOF_EN;

    // pull up
    MCF_USB_OTG_OTG_CTRL = MCF_USB_OTG_OTG_CTRL_DP_HIGH|MCF_USB_OTG_OTG_CTRL_OTG_EN;

    MCF_USB_OTG_INT_STAT = 0xff;
    MCF_USB_OTG_INT_ENB = MCF_USB_OTG_INT_ENB_USB_RST_EN;

    led_sad();
}

static int resets;

void
usb_device_default()
{
    // default address 0 on reset
    resets++;
    MCF_USB_OTG_ADDR = (uint8)0;

    // enable usb device mode
    MCF_USB_OTG_CTL |= MCF_USB_OTG_CTL_ODD_RST;
    MCF_USB_OTG_CTL &= ~MCF_USB_OTG_CTL_ODD_RST;

    memset(bdtbuffer, 0, sizeof(bdtbuffer));
    memset(endpoints, 0, sizeof(endpoints));

    assert(configuration_descriptor);

    // extract the maximum packet size
    endpoints[0].packetsize = configuration_descriptor[7];

    // parse it
    parse_configuration(configuration_descriptor, configuration_descriptor_length);

    MCF_USB_OTG_INT_STAT = 0xff;
    MCF_USB_OTG_INT_ENB |= MCF_USB_OTG_INT_ENB_SLEEP_EN|MCF_USB_OTG_INT_ENB_TOK_DNE_EN;

    led_happy();
}

// enqueue a packet to the usb engine for transfer to/from the host
void
usb_device_enqueue(int endpoint, bool tx, byte *buffer, int length)
{
    bool odd;
    int flags;
    struct bdt *bdt;

    // transfer up to one packet at a time
    assert(device_descriptor[7]);
    length = MIN(length, device_descriptor[7]);

    odd = endpoints[endpoint].bdtodd[tx];
    endpoints[endpoint].bdtodd[tx] = (byte)(! odd);

    bdt = MYBDT(endpoint, tx, odd);
    bdt->buffer = (byte *)BYTESWAP((int)buffer);
    flags = BYTESWAP(bdt->flags);
    assert(! (flags & BD_FLAGS_OWN));
    assert(length <= endpoints[endpoint].packetsize);
    bdt->flags = BYTESWAP(BD_FLAGS_BC_ENC(length)|BD_FLAGS_OWN|endpoints[endpoint].toggle[tx]/*|BD_FLAGS_DTS|*/);

    MCF_USB_OTG_ENDPT(endpoint) = (uint8)(MCF_USB_OTG_ENDPT_EP_HSHK|MCF_USB_OTG_ENDPT_EP_TX_EN|MCF_USB_OTG_ENDPT_EP_RX_EN);

    // revisit -- this should be on ack!!!
    // toggle
    endpoints[endpoint].toggle[tx] = endpoints[endpoint].toggle[tx] ? 0 : BD_FLAGS_DATA;
}

static byte setup_buffer[SETUP_SIZE];  // from host
static byte next_address;  // set after successful status

static byte descriptor[DEVICE_DESCRIPTOR_SIZE];
static byte configuration[CONFIGURATION_DESCRIPTOR_SIZE];

static int line;

// called by usb on device attach
__declspec(interrupt)
void
usb_isr(void)
{
    int rv;
    
    assert(! usb_in_isr);
    usb_in_isr = true;

    if (MCF_USB_OTG_INT_STAT & MCF_USB_OTG_INT_STAT_TOK_DNE) {
        int bc;
        int tx;
        int odd;
        int pid;
        int stat;
        int flags;
        byte *data;
        int endpoint;
        int endpoint2;
        short length;
        short value;
        struct bdt *bdt;
        struct setup *setup;

        stat = MCF_USB_OTG_STAT;
        tx = !! (stat & MCF_USB_OTG_STAT_TX);
        odd = !! (stat & MCF_USB_OTG_STAT_ODD);
        endpoint = (stat & 0xf0) >> 4;

        // ensure we just used the previous slot!
        assert(!!odd == !endpoints[endpoint].bdtodd[tx]);

        bdt = MYBDT(endpoint, tx, odd);

        flags = BYTESWAP(bdt->flags);
        assert(! (flags & BD_FLAGS_OWN));

        bc = BD_FLAGS_BC_DEC(flags);
        assert(bc >= 0);

        pid = BD_FLAGS_TOK_PID_DEC(flags);
        // if we're starting a new control transfer...
        if (pid == TOKEN_SETUP) {
            assert(! endpoint);
            assert(bc == 8);
            assert(! tx);
            
            setup = (struct setup *)BYTESWAP((int)bdt->buffer);
            assert((void *)setup == (void *)setup_buffer);

            MCF_USB_OTG_CTL &= ~MCF_USB_OTG_CTL_TXSUSPEND_TOKENBUSY;

            length = BYTESWAP(setup->length);

            endpoints[endpoint].data_pid = TOKEN_OUT;
            endpoints[endpoint].data_length = 0;
            endpoints[endpoint].data_offset = 0;

            // is this a standard command...
            if (! (setup->requesttype & 0x60)) {
                value = BYTESWAP(setup->value);
                if (setup->request == REQUEST_GET_DESCRIPTOR) {
                    endpoints[endpoint].data_pid = TOKEN_IN;

                    if ((value >> 8) == DEVICE_DESCRIPTOR) {
                        assert(device_descriptor_length);
                        endpoints[endpoint].data_length = MIN(device_descriptor_length, length);
                        memcpy(endpoints[endpoint].data_buffer, device_descriptor, endpoints[endpoint].data_length);
                    } else if ((value >> 8) == CONFIGURATION_DESCRIPTOR) {
                        assert(configuration_descriptor_length);
                        endpoints[endpoint].data_length = MIN(configuration_descriptor_length, length);
                        memcpy(endpoints[endpoint].data_buffer, configuration_descriptor, endpoints[endpoint].data_length);
                    } else if ((value >> 8) == STRING_DESCRIPTOR) {
                        int i;
                        int j;

                        i = value & 0xff;
                        j = 0;
                        while (i-- && j < string_descriptor_length) {
                            j += string_descriptor[j];
                        }
                        if (i != -1) {
                            assert(j == string_descriptor_length);
                            endpoints[endpoint].data_length = 0;  // what to return here?
                        } else {
                            assert(string_descriptor[j]);
                            endpoints[endpoint].data_length = MIN(string_descriptor[j], length);
                            memcpy(endpoints[endpoint].data_buffer, string_descriptor+j, endpoints[endpoint].data_length);
                        }
                    } else if ((value >> 8) == report_number) {
                        assert(report_descriptor_length);
                        endpoints[endpoint].data_length = MIN(report_descriptor_length, length);
                        memcpy(endpoints[endpoint].data_buffer, report_descriptor, endpoints[endpoint].data_length);
                    } else {
                        assert(0);
                    }

                    // data starts with data1
                    assert(endpoints[endpoint].toggle[1]);
                    usb_device_enqueue(0, 1, endpoints[endpoint].data_buffer, endpoints[endpoint].data_length);
                } else {
                    if (setup->request == REQUEST_CLEAR_FEATURE) {
                        assert(! length);
                        if (setup->requesttype == 0x02 && ! value) {
                            endpoint2 = BYTESWAP(setup->index) & 0x0f;
                            assert(endpoint2);
                            endpoints[endpoint2].toggle[0] = 0;
                            endpoints[endpoint2].toggle[1] = 0;
                        }
                    } else if (setup->request == REQUEST_SET_ADDRESS) {
                        next_address = value;
                    } else if (setup->request == REQUEST_SET_CONFIGURATION) {
                        assert(value == 1);
                        usb_device_configured = 1;
                    } else {
                        assert(0);
                    }

                    // prepare to transfer status (in the other direction)
                    usb_device_enqueue(0, 1, NULL, 0);
                }
            } else {
                // this is a class or vendor command
                if (setup->requesttype & 0x80/*in*/) {
                    // host wants to receive data, get it from our caller!
                    assert(control_transfer_cbfn);
                    rv = control_transfer_cbfn(setup, endpoints[endpoint].data_buffer, length);
                    assert(rv >= 0);
                    assert(rv <= length);

                    // prepare to send data, TOKEN_IN(s) will follow
                    endpoints[endpoint].data_pid = TOKEN_IN;
                    assert(rv > 0);  // if you don't have a length, use out!
                    endpoints[endpoint].data_length = rv;
                    usb_device_enqueue(0, 1, endpoints[endpoint].data_buffer, endpoints[endpoint].data_length);
                } else {
                    if (length) {
                        // we will receive data, TOKEN_OUT(s) will follow
                        endpoints[endpoint].data_length = length;
                        usb_device_enqueue(0, 0, endpoints[endpoint].data_buffer, sizeof(endpoints[endpoint].data_buffer));
                    } else {
                        // put it to our caller!
                        assert(control_transfer_cbfn);
                        rv = control_transfer_cbfn((struct setup *)setup_buffer, NULL, 0);
                        assert(rv != -1);

                        // status uses data1
                        assert(endpoints[endpoint].toggle[1] == BD_FLAGS_DATA);

                        // prepare to transfer status (in the other direction)
                        usb_device_enqueue(0, 1, NULL, 0);
                    }
                }
            }
            assert(endpoints[endpoint].data_length <= sizeof(endpoints[endpoint].data_buffer));
        } else if (! endpoint) {
            assert(pid == TOKEN_IN || pid == TOKEN_OUT);
            data = (byte *)BYTESWAP((int)bdt->buffer);

            // if this is part of the data transfer...
            if (pid == endpoints[endpoint].data_pid) {
                assert((char *)data >= (char *)endpoints[endpoint].data_buffer && (char *)data < (char *)endpoints[endpoint].data_buffer+sizeof(endpoints[endpoint].data_buffer));
                if (pid == TOKEN_IN) {
                    assert(tx);
                    // we just sent data to the host
                    endpoints[endpoint].data_offset += bc;
                    assert(endpoints[endpoint].data_offset <= endpoints[endpoint].data_length);

                    // if there's more data to send...
                    if (endpoints[endpoint].data_offset != endpoints[endpoint].data_length) {
                        // send it
                        usb_device_enqueue(0, 1, endpoints[endpoint].data_buffer+endpoints[endpoint].data_offset, endpoints[endpoint].data_length-endpoints[endpoint].data_offset);
                    } else {
                        // status uses data1
                        assert(endpoints[endpoint].toggle[0] == BD_FLAGS_DATA);

                        // prepare to transfer status (in the other direction)
                        usb_device_enqueue(0, 0, NULL, 0);
                    }
                } else {
                    assert(! tx);
                    // we just received data from the host
                    endpoints[endpoint].data_offset += bc;
                    assert(endpoints[endpoint].data_offset <= endpoints[endpoint].data_length);

                    // if there's more data to receive...
                    if (endpoints[endpoint].data_offset != endpoints[endpoint].data_length) {
                        // receive it
                        usb_device_enqueue(0, 0, endpoints[endpoint].data_buffer+endpoints[endpoint].data_offset, endpoints[endpoint].data_length-endpoints[endpoint].data_offset);
                    } else {
                        // put it to our caller!
                        assert(control_transfer_cbfn);
                        rv = control_transfer_cbfn((struct setup *)setup_buffer, endpoints[endpoint].data_buffer, endpoints[endpoint].data_length);
                        assert(rv != -1);

                        // status uses data1
                        assert(endpoints[endpoint].toggle[1] == BD_FLAGS_DATA);

                        // prepare to transfer status (in the other direction)
                        usb_device_enqueue(0, 1, NULL, 0);
                    }
                }
            } else {
                // we just transferred status
                assert(! data);

                // update our address after status
                if (next_address) {
                    MCF_USB_OTG_ADDR |= next_address;
                    next_address = 0;
                }

                // setup always uses data0; following transactions start with data1
                endpoints[endpoint].toggle[0] = 0;
                endpoints[endpoint].toggle[1] = BD_FLAGS_DATA;

                // prepare to receive setup token
                usb_device_enqueue(0, 0, setup_buffer, sizeof(setup_buffer));
            }
        } else if (! endpoints[endpoint].interrupt) {
            assert(pid == TOKEN_IN || pid == TOKEN_OUT);
            data = (byte *)BYTESWAP((int)bdt->buffer);

            // we just received or sent data from or to the host
            assert(bulk_transfer_cbfn);
            bulk_transfer_cbfn(pid == TOKEN_IN, data, bc);
        } else {
            // this is our interrupt endpoint
            assert(interrupt_transfer_cbfn);
            interrupt_transfer_cbfn();
        }

        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_TOK_DNE;
    }

    if (MCF_USB_OTG_INT_STAT & MCF_USB_OTG_INT_STAT_USB_RST) {
        usb_device_configured = 0;

        usb_device_default();

        assert(reset_cbfn);
        reset_cbfn();

        // setup always uses data0; following transactions start with data1
        endpoints[0].toggle[0] = 0;
        endpoints[0].toggle[1] = BD_FLAGS_DATA;

        // prepare to receive setup token
        usb_device_enqueue(0, 0, setup_buffer, sizeof(setup_buffer));

        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_USB_RST;
    }

    if (MCF_USB_OTG_INT_STAT & MCF_USB_OTG_INT_STAT_SLEEP) {
        usb_device_configured = 0;

        MCF_USB_OTG_INT_ENB &= ~MCF_USB_OTG_INT_ENB_SLEEP_EN;
        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_SLEEP;
    }

    assert(usb_in_isr);
    usb_in_isr = false;
}

void
usb_register(usb_reset_cbfn reset, usb_control_cbfn control_transfer, usb_bulk_cbfn bulk_transfer, usb_interrupt_cbfn interrupt_transfer)
{
    reset_cbfn = reset;
    control_transfer_cbfn = control_transfer;
    bulk_transfer_cbfn = bulk_transfer;
    interrupt_transfer_cbfn = interrupt_transfer;
}

void
usb_device_descriptor(byte *descriptor, int length)
{
    device_descriptor = descriptor;
    device_descriptor_length = length;
}

void
usb_configuration_descriptor(byte *descriptor, int length)
{
    configuration_descriptor = descriptor;
    configuration_descriptor_length = length;
}

void
usb_string_descriptor(byte *descriptor, int length)
{
    string_descriptor = descriptor;
    string_descriptor_length = length;
}

void
usb_report_descriptor(int number, byte *descriptor, int length)
{
    report_number = number;
    report_descriptor = descriptor;
    report_descriptor_length = length;
}

void
usb_initialize(void)
{
    bdts = (struct bdt *)(((unsigned int)bdtbuffer+512)/512*512);

    // enable usb interrupt
    __VECTOR_RAM[117] = (uint32)usb_isr;
    MCF_INTC0_ICR53 = MCF_INTC_ICR_IL(SPL_USB)|MCF_INTC_ICR_IP(SPL_USB);
    MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_INT_MASK53;  // usb
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;

    // initialize usb timing
    MCF_USB_OTG_USB_CTRL = MCF_USB_OTG_USB_CTRL_CLK_SRC(3);
    MCF_USB_OTG_SOF_THLD = 74;

    // initialize usb bdt
    assert(! (((unsigned int)bdts >> 8) & 1));
    MCF_USB_OTG_BDT_PAGE_01 = (uint8)((unsigned int)bdts >> 8);
    MCF_USB_OTG_BDT_PAGE_02 = (uint8)((unsigned int)bdts >> 16);
    MCF_USB_OTG_BDT_PAGE_03 = (uint8)((unsigned int)bdts >> 24);

    // enable usb to interrupt on reset
    usb_device_wait();
}
