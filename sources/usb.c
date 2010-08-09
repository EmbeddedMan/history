#include "main.h"

#define SWRETRIES  10

// *** usb ******************************************************************

#define DEVICE_DESCRIPTOR_SIZE  18
#define CONFIGURATION_DESCRIPTOR_SIZE  128

#define DEVICE_DESCRIPTOR  1
#define CONFIGURATION_DESCRIPTOR  2
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

// N.B. only bdt endpoint 0 is used for host mode!
static byte bdtbuffer[512+32];
static struct bdt {
    int flags;
    byte *buffer;
} *bdts;  // 512 byte aligned in buffer

static byte bdtodd[2];  // keep track of rx [0] and tx [1] bdt even (0) and odd (1)

struct endpoint endpoints[16];

byte bulk_in_ep;
byte bulk_out_ep;
byte int_ep;

bool scsi_attached;  // set when usb mass storage device is attached
bool pima_attached;  // set when usb pima camera device is attached
bool canon_attached;  // set when usb canon camera device is attached

// initialize a setup data0 buffer
void
usb_setup(int in, int type, int recip, int request, int value, int index, int length, byte *setup)
{
    setup[0] = (byte)((in<<7)|(type << 5)|recip);
    setup[1] = (byte)request;
    setup[2] = (byte)value;
    setup[3] = (byte)(value >> 8);
    setup[4] = (byte)index;
    setup[5] = (byte)(index >> 8);
    setup[6] = (byte)(length);
    setup[7] = (byte)(length >> 8);
}

// perform a usb host/device transaction
static
int
transaction(int endpoint, int token, void *buffer, int length)
{
    int bc;
    int tx;
    int pid;
    int odd;
    int stat;
    int toggle;
    int flags;
    int retry;
    int int_stat;
    struct bdt *bdt;

    if (token == TOKEN_SETUP) {
        tx = 1;
    
        // setup always uses data0; following transactions start with data1
        assert(! endpoint);
        endpoints[0].toggle[tx] = 0;
        endpoints[0].toggle[! tx] = BD_FLAGS_DATA;
    } else if (token == TOKEN_IN) {
        tx = 0;
    } else {
        assert(token == TOKEN_OUT);
        tx = 1;
    }

    toggle = endpoints[endpoint].toggle[tx];

    retry = 0;
    for (;;) {
        odd = bdtodd[tx];
        bdtodd[tx] = (byte)(! odd);
        
        // N.B. only bdt endpoint 0 is used for host mode!
        bdt = MYBDT(0, tx, odd);
        flags = byteswap(bdt->flags);
        assert(! (flags & BD_FLAGS_OWN));
        assert(length <= endpoints[endpoint].packetsize);
        bdt->flags = byteswap(BD_FLAGS_BC_ENC(length)|BD_FLAGS_OWN|toggle);
        bdt->buffer = (byte *)byteswap((int)buffer);

        assert(! (MCF_USB_OTG_CTL & MCF_USB_OTG_CTL_TXSUSPEND_TOKENBUSY));

        MCF_USB_OTG_TOKEN = (uint8)(MCF_USB_OTG_TOKEN_TOKEN_PID(token)|MCF_USB_OTG_TOKEN_TOKEN_ENDPT(endpoint));

        // wait for token done or reset
        for (;;) {
            int_stat = MCF_USB_OTG_INT_STAT;
            if (int_stat & (MCF_USB_OTG_INT_STAT_TOK_DNE|MCF_USB_OTG_INT_STAT_USB_RST)) {
                break;
            }
        }

        stat = MCF_USB_OTG_STAT;
        flags = byteswap(bdt->flags);

        // if we got token done...
        if (int_stat & MCF_USB_OTG_INT_STAT_TOK_DNE) {
            MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_TOK_DNE;
        }

        // if we got reset...
        if (int_stat & MCF_USB_OTG_INT_STAT_USB_RST) {
            MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_USB_RST;
            return -1;
        }

        assert(! (flags & BD_FLAGS_OWN));
        bc = BD_FLAGS_BC_DEC(flags);
        pid = BD_FLAGS_TOK_PID_DEC(flags);

        if (pid) {
            assert(tx == !! (stat & MCF_USB_OTG_STAT_TX));
            assert(odd == !! (stat & MCF_USB_OTG_STAT_ODD));
            assert(0 == (stat >> 4));
        }

        switch (pid) {
            case TOKEN_DATA0:
            case TOKEN_DATA1:  // we received a data0/data1 packet
                assert(! tx);
                // if we expected it...
                if (pid == (toggle?TOKEN_DATA1:TOKEN_DATA0)) {
                    // flip the data toggle to acknowledge
                    endpoints[endpoint].toggle[tx] ^= BD_FLAGS_DATA;
                    return bc;
                } else {
                    // otherwise, ignore the packet and try again
                    if (retry++ < SWRETRIES) {
                        delay(100);
                        continue;
                    }
                    return -1;
                }
            case TOKEN_ACK:  // the device accepted the data packet we sent
                assert(tx);
                // flip the data toggle to acknowledge
                endpoints[endpoint].toggle[tx] ^= BD_FLAGS_DATA;
                return bc;
            case TOKEN_STALL:
                if (pid == TOKEN_STALL) {
                    int rv;
                    byte setup[SETUP_SIZE];
                
                    // clear the stall!
                    usb_setup(0, SETUP_TYPE_STANDARD, SETUP_RECIP_ENDPOINT, REQUEST_CLEAR_FEATURE, FEATURE_ENDPOINT_HALT, endpoint, 0, setup);
                    rv = usb_control_transfer(setup, NULL, 0);
                    assert(! rv);
                    
                    endpoints[endpoint].toggle[0] = 0;
                    endpoints[endpoint].toggle[1] = 0;
                    
                    return -TOKEN_STALL;
                }
            default:
                assert(0);
                // FALL THRU
            case 0:  // bus timeout
            case TOKEN_NAK:
            case 0xf:  // data error
                // ignore the packet and try again
                if (retry++ < SWRETRIES) {
                    delay(100);
                    continue;
                }
                assert(0);
                return -1;
        }
    }
}

// perform a usb host/device control transfer
int
usb_control_transfer(byte *setup, byte *buffer, int length)
{
    int in;
    int rv;
    int total;
    int request;

    rv = transaction(0, TOKEN_SETUP, setup, SETUP_SIZE);
    if (rv < 0) {
        return rv;
    }
    assert(rv == SETUP_SIZE);

    in = !! (setup[0] & 0x80);
    assert(in ? length : 1);  // if you don't have a length, use out!

    total = 0;
    while (total < length) {
        request = MIN(endpoints[0].packetsize, length-total);
        rv = transaction(0, in?TOKEN_IN:TOKEN_OUT, buffer+total, request);
        if (rv < 0) {
            return rv;
        }
        total += rv;
        if (rv < request) {
            break;
        }
    }

    // N.B. we always switch directions from the previous transaction
    // N.B. the new direction was initialized to data1 at setup time
    rv = transaction(0, in?TOKEN_OUT:TOKEN_IN, NULL, 0);
    if (rv < 0) {
        return rv;
    }
    assert(! rv);

    return total;
}

// perform a usb host/device bulk transfer
int
usb_bulk_transfer(int in, byte *buffer, int length, bool null_or_short)
{
    int rv;
    int total;
    int request;
    int endpoint;
    uint8 endpt0;

    endpt0 = MCF_USB_OTG_ENDPT0;

    if (in == -1) {
        MCF_USB_OTG_ENDPT0 |= MCF_USB_OTG_ENDPT_RETRY_DIS;
        endpoint = int_ep;
    } else if (in) {
        endpoint = bulk_in_ep;
    } else {
        endpoint = bulk_out_ep;
    }
    assert(endpoint);
    assert(endpoints[endpoint].packetsize);

    total = 0;
    while (total < length) {
        request = MIN(endpoints[endpoint].packetsize, length-total);
        rv = transaction(endpoint, in?TOKEN_IN:TOKEN_OUT, buffer+total, request);
        if (rv < 0) {
            total = rv;
            break;
        }
        total += rv;
        if (rv < request) {
            break;
        }
    }

    // if the caller wants to end with a null or short packet and we ended
    // with a full packet...
    if (null_or_short && rv == endpoints[endpoint].packetsize) {
        rv = transaction(endpoint, in?TOKEN_IN:TOKEN_OUT, NULL, 0);
        if (rv < 0) {
            total = rv;
        }
    }

    MCF_USB_OTG_ENDPT0 = endpt0;

    return total;
}

// detach from the device and prepare to re-attach
void
usb_detach()
{
    delay(100);  // debounce
    
    MCF_USB_OTG_CTL = MCF_USB_OTG_CTL_ODD_RST;
    MCF_USB_OTG_CTL = MCF_USB_OTG_CTL_HOST_MODE_EN;
    
    memset(bdtodd, 0, sizeof(bdtodd));
    memset(bdtbuffer, 0, sizeof(bdtbuffer));
    memset(endpoints, 0, sizeof(endpoints));
    
    scsi_attached = 0;
    pima_attached = 0;
    canon_attached = 0;

    MCF_USB_OTG_INT_STAT = 0xff;
    MCF_USB_OTG_INT_ENB = MCF_USB_OTG_INT_STAT_ATTACH;
    
    led_sad();
}

static byte descriptor[DEVICE_DESCRIPTOR_SIZE];
static byte configuration[CONFIGURATION_DESCRIPTOR_SIZE];

// called by usb on device attach
__declspec(interrupt)
void
usb_isr(void)
{
    int i;
    int rv;
    int size;
    byte setup[SETUP_SIZE];

    delay(100);  // debounce
    
    if (MCF_USB_OTG_INT_STAT & MCF_USB_OTG_INT_STAT_ATTACH) {
        MCF_USB_OTG_INT_ENB = 0;
        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_ATTACH;

        // default address 0 on attach
        MCF_USB_OTG_ADDR = (uint8)0;

        // if this is a low speed device...
        if (! (MCF_USB_OTG_CTL & MCF_USB_OTG_CTL_JSTATE)) {
            MCF_USB_OTG_ADDR |= MCF_USB_OTG_ADDR_LS_EN;
        }

        // reset the device
        MCF_USB_OTG_CTL |= MCF_USB_OTG_CTL_RESET;
        delay(10);
        MCF_USB_OTG_CTL &= ~MCF_USB_OTG_CTL_RESET;
        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_USB_RST;

        // enable sof
        MCF_USB_OTG_CTL |= MCF_USB_OTG_CTL_USB_EN_SOF_EN;
        MCF_USB_OTG_INT_STAT = MCF_USB_OTG_INT_STAT_SLEEP|MCF_USB_OTG_INT_STAT_RESUME;

        delay(100);  // post reset

        // enable transfers
        MCF_USB_OTG_ENDPT0 = MCF_USB_OTG_ENDPT_HOST_WO_HUB/*|MCF_USB_OTG_ENDPT_RETRY_DIS*/;
        MCF_USB_OTG_ENDPT0 |= (uint8)(MCF_USB_OTG_ENDPT_EP_HSHK|MCF_USB_OTG_ENDPT_EP_TX_EN|MCF_USB_OTG_ENDPT_EP_RX_EN);

        // data0 follows configuration event
        memset(endpoints, 0, sizeof(endpoints));

        // default packetsize
        endpoints[0].packetsize = 8;

        // get the first 8 bytes of the device descriptor
        usb_setup(1, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_GET_DESCRIPTOR, (DEVICE_DESCRIPTOR<<8)|0, 0, 8, setup);
        rv = usb_control_transfer(setup, descriptor, 8);
        if (rv == -1) {
            // N.B. we get spurios attach interrupts when a mab cable is
            // plugged in even with no device at the other end of it...
            usb_detach();
            return;
        }
        assert(rv == 8);

        // extract the maximum packet size
        endpoints[0].packetsize = descriptor[7];

        // then get the whole device descriptor
        usb_setup(1, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_GET_DESCRIPTOR, (DEVICE_DESCRIPTOR<<8)|0, 0, sizeof(descriptor), setup);
        rv = usb_control_transfer(setup, descriptor, sizeof(descriptor));
        assert(rv == sizeof(descriptor));
        led_happy();

        // set address to 1
        usb_setup(0, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_SET_ADDRESS, 1, 0, 0, setup);
        rv = usb_control_transfer(setup, NULL, 0);
        assert(rv == 0);
        MCF_USB_OTG_ADDR |= 1;
        led_happy();

        delay(10);  // post set address recovery

        // get the first 9 bytes of the configuration descriptor
        usb_setup(1, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_GET_DESCRIPTOR, (CONFIGURATION_DESCRIPTOR<<8)|0, 0, 9, setup);
        rv = usb_control_transfer(setup, configuration, 9);
        assert(rv > 0);
        led_happy();

        size = configuration[2];
        assert(size >= 9 && size < sizeof(configuration));

        // then get the whole configuration descriptor
        usb_setup(1, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_GET_DESCRIPTOR, (CONFIGURATION_DESCRIPTOR<<8)|0, 0, size, setup);
        rv = usb_control_transfer(setup, configuration, size);
        assert(rv == size);
        led_happy();

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
                }
            }
        }
        assert(i == rv);
        assert(bulk_in_ep && bulk_out_ep);

        // set configuration
        usb_setup(0, SETUP_TYPE_STANDARD, SETUP_RECIP_DEVICE, REQUEST_SET_CONFIGURATION, configuration[5], 0, 0, setup);
        rv = usb_control_transfer(setup, NULL, 0);
        assert(rv == 0);
        led_happy();

        if (descriptor[4] == CLASS_SCSI || (descriptor[4] == 0x00 && configuration[9+5] == CLASS_SCSI)) {
            scsi_attached = 1;
        } else if (descriptor[4] == CLASS_PIMA || (descriptor[4] == 0x00 && configuration[9+5] == CLASS_PIMA)) {
            pima_attached = 1;
        } else if (descriptor[4] == CLASS_CANON) {
            canon_attached = 1;
        } else {
            assert(0);
        }
    }
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
    // N.B. we assume external DP/DM pull-downs!
    MCF_USB_OTG_USB_CTRL = MCF_USB_OTG_USB_CTRL_CLK_SRC(1);
    MCF_USB_OTG_SOF_THLD = 74;

    // initialize usb bdt
    assert(! (((unsigned int)bdts >> 8) & 1));
    MCF_USB_OTG_BDT_PAGE_01 = (uint8)((unsigned int)bdts >> 8);
    MCF_USB_OTG_BDT_PAGE_02 = (uint8)((unsigned int)bdts >> 16);
    MCF_USB_OTG_BDT_PAGE_03 = (uint8)((unsigned int)bdts >> 24);
    
    // usb power on
    MCF_GPIO_CLRUA = (uint8)~0x08;

    // enable usb to interrupt on attach
    usb_detach();
}
