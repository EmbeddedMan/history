// *** zigbee.h *******************************************************

enum {
    zb_class_none,
    zb_class_receive,
    zb_class_print,
    zb_class_remote_set
};

#define ZB_PAYLOAD_SIZE  114

extern int zb_nodeid;
extern bool zb_present;

extern int transmits;
extern int receives;
extern int drops;
extern int failures;
extern int retries;

typedef void (*zb_recv_cbfn)(int nodeid, int length, byte *buffer);

#if ! _WIN32
__declspec(interrupt)
void
zb_isr(void);
#endif

bool
zb_send(int nodeid, uint8 class, int length, byte *buffer);

void
zb_poll(void);

void
zb_drop(bool drop);

void
zb_register(uint8 class, zb_recv_cbfn cbfn);

void
zb_initialize(void);

