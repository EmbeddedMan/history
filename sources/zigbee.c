// *** zigbee.c *******************************************************
// this file implements the zigbee wireless transport over qspi.

#include "main.h"

// N.B. to handle relaying (i.e., node 1 -> node 2 -> node3) we need
//      to implement two sets (upstream/downstream) of sequence numbers.

#define ACK_MS  40
#define WAIT_MS  50
#define DELAY_MS  5
#define THROTTLE_MS  30
#define RETRIES  10

typedef struct packet {
    uint16 magic;  // 0x4242
    uint16 txid;
    uint16 rxid;
    uint8 class;  // zb_class_none, zb_class_receive, zb_class_print
    uint8 seq;  // 0 for zb_class_none
    uint8 ackseq;
    uint8 length;
    byte payload[ZB_PAYLOAD_SIZE];
} packet_t;

int zb_nodeid;
bool zb_present;

bool zb_in_isr;

#define NCLASS  4

static int recv_cbfns;
static zb_recv_cbfn recv_cbfn[NCLASS];

int transmits;
int receives;
int drops;
int failures;
int retries;

// revisit handle multiple senders/receivers
static uint8 txseq;  // last seq transmitted
static uint8 txack;  // last transmit seq acked
static uint8 rxseq;  // last seq received
static bool rxdrop;  // we need to drop received packets
static bool rxack_bool;  // we need to ack a received seq
static int rxack_ticks;  // we need to ack a received seq now
static int rxseqid = -1;  // last nodeid received

static bool ready;
static int active;

#define IDLE  0
#define RX  2
#define TX  3

static void
zb_rxtxen(bool boo)
{
    if (boo) {
        MCF_GPIO_SETAN = (1<<5);
        MCF_GPIO_SETAS = (1<<1);
    } else {
        MCF_GPIO_CLRAN = ~(1<<5);
        MCF_GPIO_CLRAS = ~(1<<1);
    }
}

static void
zb_delay(int send)
{
    int ms;
    
    for (;;) {
        // if we've recently sent or received a packet...
        ms = ticks-active;
        if (ms >= 0 && ms < (send?THROTTLE_MS:DELAY_MS)) {
            // delay the transition to tx
            delay((send?THROTTLE_MS:DELAY_MS)-ms);
        } else {
            break;
        }
    }
}

// this function performs a read-modify-write on a tranceiver register
// via qspi. 
static void
zb_rmw(int n, int and, int or)
{
    byte buf[2+2];
    
    assert(gpl() >= SPL_IRQ4);

    memset(buf, 0xff, sizeof(buf));
    buf[1] = 0x80|n;
    qspi_transfer(buf+1, 1 + 2);
    buf[1] = 0x00|n;
    *(uint16 *)(buf+2) = (*(uint16 *)(buf+2) & and) | or;
    qspi_transfer(buf+1, 1 + 2);
}

// this function reads a tranceiver register, via qspi.
static int
zb_read(int n)
{
    byte buf[2+2];
    
    assert(gpl() >= SPL_IRQ4);

    memset(buf, 0xff, sizeof(buf));
    buf[1] = 0x80|n;
    qspi_transfer(buf+1, 1 + 2);
    
    return *(uint16 *)(buf+2);
}

// this function writes a tranceiver register, via qspi.
static void
zb_write(int n, int d)
{
    byte buf[2+2];
    
    assert(gpl() >= SPL_IRQ4);

    memset(buf, 0xff, sizeof(buf));
    buf[1] = 0x00|n;
    *(uint16 *)(buf+2) = (uint16)d;
    qspi_transfer(buf+1, 1 + 2);
}

// this function enables the receiver.
static void
zb_ready(void)
{
    assert(gpl() >= SPL_IRQ4);
    
    assert(! ready);
    ready = true;

    zb_rxtxen(false);
    (void)zb_read(0x24);
    
    // turn on the receiver
    zb_rmw(0x06, ~0x0003, RX);
    zb_rxtxen(true);
}

// this function disables the receiver.
static void
zb_unready(void)
{
    assert(gpl() >= SPL_IRQ4);

    zb_rxtxen(false);
    
    assert(ready);
    ready = false;
}

// this function transmits a packet from the current zb_nodeid.
static void
zb_packet_transmit(
    IN uint16 rxid,
    IN uint8 class,
    IN uint8 seq,
    IN uint8 ackseq,
    IN uint8 length_in,
    IN byte *payload
    )
{
    int irq;
    int length;
    packet_t *packet;
    byte buf[2+sizeof(packet_t)];
    
    assert(rxid != -1);
    assert(gpl() >= SPL_IRQ4);
    
    transmits++;
        
    zb_rxtxen(false);
    (void)zb_read(0x24);
    
    length = length_in;
    
    assert(length <= sizeof(packet->payload));
    assert(OFFSETOF(packet_t, payload) + length <= ZB_PAYLOAD_SIZE);
    
    if ((OFFSETOF(packet_t, payload) + length)&1) {
        length++;
    }
    
    // set payload length including 2 byte crc
    zb_rmw(0x03, ~0x807f, OFFSETOF(packet_t, payload) + length + 2);
    
    // set payload data
    buf[1] = 0x00|0x02;
    packet = (packet_t *)(buf+2);
    packet->magic = 0x4242;
    packet->txid = zb_nodeid;
    packet->rxid = rxid;
    packet->class = class;
    packet->seq = seq;
    packet->ackseq = ackseq;
    packet->length = length_in;
    memcpy(packet->payload, payload, length_in);
    qspi_transfer(buf+1, 1 + OFFSETOF(packet_t, payload) + length);

    // transmit packet
    zb_rmw(0x06, ~0x0003, TX);
    
    zb_rxtxen(true);

    // wait for transmit complete    
    do {
        irq = zb_read(0x24);
    } while (! (irq & 0x40));
    
    active = ticks;
    
    zb_rxtxen(false);
}

// this function receives a waiting packet for the current zb_nodeid.
static bool
zb_packet_receive(
    OUT uint16 *txid,
    OUT uint8 *class,
    OUT uint8 *seq,
    OUT uint8 *ackseq,
    OUT uint8 *length_out,
    OUT byte *payload
    )
{
    int length;
    packet_t *packet;
    byte buf[2+2+sizeof(packet_t)];
        
    assert(gpl() >= SPL_IRQ4);
    
    receives++;

    active = ticks;

    zb_rxtxen(false);
    
    length = (zb_read(0x2d) & 0x7f) - 2;  // exclude 2 byte crc
    if (length&1) {
        length++;
    }
    
    if (length < OFFSETOF(packet_t, payload)) {
        return false;
    }
    
    // get payload data
    memset(buf, 0xff, sizeof(buf));
    buf[1] = 0x80|0x01;
    
    // N.B. See 5.5.3.1.1. The first word (or 2 bytes) read during a
    // Packet RAM read should be discarded as the internal Packet
    // RAM address is not accessed for the first word read operation.
    
    packet = (packet_t *)(buf+2+2);
    qspi_transfer(buf+1, 1 + 2 + length);
    
    // if not for me...
    if (packet->magic != 0x4242 || packet->rxid != zb_nodeid) {
        return false;
    }
    
    assert(packet->length <= length);
    assert(packet->length <= sizeof(packet->payload));
    
    assert(packet->txid != -1);
    
    *txid = packet->txid;
    *class = packet->class;
    *seq = packet->seq;
    *ackseq = packet->ackseq;
    *length_out = packet->length;
    memcpy(payload, packet->payload, packet->length);
    
    return true;
}

static
void
zb_packet_deliver(
    IN uint16 txid,
    IN uint8 class,
    IN uint8 seq,
    IN uint8 ackseq,
    IN uint8 length,
    IN byte *payload
    )
{
    // if this is an explicit ack...
    if (class == zb_class_none) {
        // just remember the ack status
        assert(! seq);
        txack = ackseq;
        return;
    }
    
    // if we're dropping received packets...
    if (rxdrop && class == zb_class_receive) {
        // just remember the ack status
        txack = ackseq;
        drops++;
        return;
    }
    
    // schedule a delayed ack
    rxack_ticks = ticks+ACK_MS;
    rxack_bool = true;
    
    // if this is from a new node or a new boot...
    if (txid != rxseqid || ! seq) {
        // resequence with the stream
        rxseqid = txid;
        rxseq = seq-1;
    }

    // if the packet is not a duplicate...
    if (seq != rxseq) {
        // advance the stream
        rxseq = seq;
        txack = ackseq;
    
        // if someone is waiting for it...
        if (recv_cbfn[class]) {
            zb_ready();
        
            // deliver it
            assert(recv_cbfn)
            recv_cbfn[class](txid, length, payload);
            
            zb_unready();
        }
    }
}

// this function is called when a packet is ready to be received.
__declspec(interrupt)
void
zb_isr(void)
{
    int irq;
    uint16 txid;
    uint8 class;
    uint8 seq;
    uint8 ackseq;
    uint8 length;
    static byte payload[ZB_PAYLOAD_SIZE];

    assert(! zb_in_isr)
    zb_in_isr = true;
    
    (void)splx(-SPL_IRQ4);
    
    zb_unready();
    
    // if we received a packet...
    irq = zb_read(0x24);
    if (irq & 0x80) {
        // if the crc was good...
        if (irq & 0x01) {
            // if the packet was for us...
            if (zb_packet_receive(&txid, &class, &seq, &ackseq, &length, payload)) {
                // deliver it to upper layers
                zb_packet_deliver(txid, class, seq, ackseq, length, payload);
            }
        }
    }
    
    zb_ready();
    
    assert(zb_in_isr);
    zb_in_isr = false;
}

bool
zb_send(
    IN int nodeid,
    IN uint8 class_in,
    IN int length_in,
    IN byte *buffer
    )
{
    int x;
    int xx;
    int irq;
    int retry;
    int start;
    bool boo;
    uint16 txid;
    uint8 class;
    uint8 seq;
    uint8 ackseq;
    uint8 length;
    static byte payload[ZB_PAYLOAD_SIZE];
    
    assert(gpl() < SPL_IRQ4);
    assert(! zb_in_isr);
    
    if (! zb_present) {
        return false;
    }
    
    xx = splx(SPL_USB);

    retry = 0;
    boo = false;
    for (;;) {
        // if we have exhausted our retries...
        if (++retry > RETRIES) {
            // send has failed
            boo = false;
            failures++;
            break;
        }
        
        assert(retry);
        zb_delay(retry);
        
        // we hold off receive interrupts while doing a foreground receive
        x = splx(SPL_IRQ4);
        
        zb_unready();
        
        // we're sending an implicit ack
        rxack_bool = false;

        // send the packet
        zb_packet_transmit(nodeid, class_in, txseq, rxseq, length_in, buffer);
        
        // wait for an ack
        start = ticks;
        for (;;) {
            // enable receiver
            zb_ready();

            // wait for receive complete or timeout
            do {
                irq = zb_read(0x24);
                if (! (irq & 0x80)) {
                    zb_poll();
                }
            } while (! (irq & 0x80) && ticks-start <= WAIT_MS);
            
            zb_unready();

            // if we've timed out...
            if (ticks-start > WAIT_MS) {
                // N.B. we might have retries left
                break;
            }
            
            // we received a packet
            assert(irq & 0x80);
            
            // if it had good crc...
            if (irq & 0x01) {
                // if the packet was for us...
                if (zb_packet_receive(&txid, &class, &seq, &ackseq, &length, payload)) {
                    // deliver it to upper layers
                    zb_packet_deliver(txid, class, seq, ackseq, length, payload);
                    
                    // if we got an ack...
                    if (txack == txseq) {
                        // send is complete
                        boo = true;
                        break;
                    }
                }
            }
        }
        
        zb_ready();
        splx(x);
        
        if (boo) {
            break;
        }
        
        retries++;
    }
    
    // bump the transmit sequence number
    txseq++;
    if (! txseq) {
        // N.B. we reserve sequence number 0 to indicate a new boot
        txseq++;
    }
    
    splx(xx);
    
    return boo;
}

void
zb_poll(void)
{
    int x;
    
    //assert(gpl() == 0);

    if (rxack_bool && ticks-rxack_ticks > 0) {
        x = splx(SPL_IRQ4);
    
        zb_unready();
        
        // send the explicit ack
        rxack_bool = false;
        zb_packet_transmit(rxseqid, zb_class_none, 0, rxseq, 0, NULL);
        
        zb_ready();
        
        splx(x);
    }
}

void
zb_drop(bool drop)
{
    rxdrop = drop;
}

void
zb_register(uint8 class, zb_recv_cbfn cbfn)
{
    assert(class < NCLASS);
    
    if (! recv_cbfn[class] && cbfn) {
        recv_cbfns++;
    }
    if (recv_cbfn[class] && ! cbfn) {
        recv_cbfns--;
    }
    
    recv_cbfn[class] = cbfn;
}

void
zb_initialize(void)
{
    int x;
    int id;
    
    assert(sizeof(packet_t) <= 125);
    
#if DEMO
    // an2/gpt0=0 for rst*
    // an3/gpt1=0 for attn*
    // an5=0 for rxtxen
#if MCF52221
    MCF_GPIO_PANPAR = 0x00;
    MCF_GPIO_DDRAN |= (1<<2)|(1<<3)|(1<<5);
#elif MCF52233
    MCF_GPIO_PANPAR = 0x00;
    MCF_GPIO_DDRAN |= (1<<5);
    MCF_GPIO_PTAPAR = 0x00;
    MCF_GPIO_DDRTA |= (1<<0)|(1<<1);
#else
#error
#endif

    qspi_inactive(1);

    // rst*
#if MCF52221
    MCF_GPIO_SETAN = (1<<2);
#else
    MCF_GPIO_SETTA = (1<<0);
#endif
    MCF_GPIO_SETAS = (1<<0);

    delay(50);

    // if zigbee is present...
    x = splx(SPL_IRQ4);
    id = zb_read(0x2c);
    if ((id & 0xfb00) != 0x6000 && (id & 0xfb00) != 0x6400) {
        // no zigbee present
        splx(x);
        return;
    }
    
    zb_present = true;
    qspi_baud_fast();

    // scl=0 for rst*
    // sda=0 for rxtxen
    MCF_GPIO_PASPAR = 0x00;
    MCF_GPIO_DDRAS |= (1<<0)|(1<<1);
    
    
    // rst*
#if MCF52221
    MCF_GPIO_CLRAN = ~(1<<2);
#else
    MCF_GPIO_CLRTA = ~(1<<0);
#endif
    MCF_GPIO_CLRAS = ~(1<<0);
    delay(1);
#if MCF52221
    MCF_GPIO_SETAN = (1<<2);
#else
    MCF_GPIO_SETTA = (1<<0);
#endif
    MCF_GPIO_SETAS = (1<<0);
    delay(50);
    
    // rxtxen*
    MCF_GPIO_CLRAN = ~((1<<5)|(1<<3));
    
#else
#error
#endif
    
    zb_write(0x1b, 0x80ff);  // disable timer 1
    zb_write(0x1c, 0xffff);
    zb_write(0x1d, 0x80ff);  // disable timer 2
    zb_write(0x1e, 0xffff);
    zb_write(0x1f, 0x00ff);  // enable timer 3 (13202 only)
    zb_write(0x20, 0xffff);
    zb_write(0x21, 0x80ff);  // disable timer 4 (13202 only)
    zb_write(0x22, 0x0000);
    
    // N.B. MC13201CE.pdf Bug No. 2:
    // Enable Timer Compare 3 always to generate an
    // interrupt. If the interrupt occurs and the RX state
    // was enabled. Take appropriate action, such as
    // restarting RX.
    
    zb_rmw(0x05, ~0x9f1f, 0x0204);  // enable pll_lock_mask irq, tmr3_mask
    zb_rmw(0x06, ~0x1f83, 0x0100);  // enable rx_rcvd_mask irq
    zb_rmw(0x07, ~0xfae3, 0x5000);  // enable ct_bias_en, RF_switch_mode
#if REV2PCB
    zb_rmw(0x09, ~0x00a0, 0x00a0);  // out of idle indicator on gpio1 (part a)
    zb_rmw(0x0b, ~0x0081, 0x0080);  // out of idle indicator on gpio1 (part b)
#endif

    zb_ready();
    splx(x);
    
    zb_nodeid = main_nodeid();

    // NQ is primary (irq4)
    irq4_enable = true;
    MCF_GPIO_PNQPAR = (MCF_GPIO_PNQPAR &~ (3<<(4*2))) | (1<<(4*2));  // irq4 is primary

    // program irq4 for level trigger
    MCF_EPORT_EPPAR = (MCF_EPORT_EPPAR &~ MCF_EPORT_EPPAR_EPPA4_BOTH) | MCF_EPORT_EPPAR_EPPA4_LEVEL;
    MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE4;

    // enable irq4 interrupt
    MCF_INTC0_ICR04 = MCF_INTC_ICR_IL(SPL_IRQ4)|MCF_INTC_ICR_IP(SPL_IRQ4);
    //MCF_INTC0_IMRH &= ~0;
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK4;  // irq4
}

