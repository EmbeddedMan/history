#include "main.h"

#if PIMA
#define TYPE_COMMAND  1
#define TYPE_DATA  2
#define TYPE_RESPONSE  3
#define TYPE_EVENT  4

struct container {
    uint32 length;
    uint16 type;
    uint16 code;
    uint32 transaction;
};

// perform a usb host/device pima bulk transfer
static
int
pima_bulk_transfer(int in, uint16 code, byte *request, int request_length, byte *data, int data_length, byte *response, int *response_length)
{
    int rv;
    int total;
    int length;
    int initial;
    byte buffer[64];
    struct container *container;
    static int transaction;

    assert(endpoints[bulk_in_ep].packetsize <= sizeof(buffer));
    assert(endpoints[bulk_out_ep].packetsize <= sizeof(buffer));

    container = (struct container *)buffer;

    length = sizeof(*container)+request_length;
    assert(length <= sizeof(buffer));

    container->length = byteswap(length);
    container->type = byteswapshort(TYPE_COMMAND);
    container->code = byteswapshort(code);
    container->transaction = byteswap(++transaction);

    memcpy(container+1, request, request_length);

    rv = usb_bulk_transfer(0, buffer, length, 1);
    if (rv < 0) {
        return rv;
    }
    assert(rv == length);

    if (data_length) {
        // first process the initial data
        if (! in) {
            container->length = byteswap(sizeof(*container) + data_length);
            container->type = byteswapshort(TYPE_DATA);
            container->code = byteswapshort(code);
            container->transaction = byteswap(transaction);

            initial = MIN(sizeof(buffer) - sizeof(*container), data_length);
            memcpy(container+1, data, initial);

            total = usb_bulk_transfer(in, buffer, sizeof(*container)+initial, 0);
            if (total < 0) {
                return total;
            }
            assert(total == sizeof(*container)+initial);
        } else {
            total = usb_bulk_transfer(in, buffer, sizeof(buffer), 0);
            if (total < 0) {
                return total;
            }
            assert(total >= sizeof(*container) && total <= sizeof(buffer));
            assert(byteswap(container->length) >= total);
            
            if (byteswapshort(container->type) == TYPE_RESPONSE) {
                total = 0;
                goto XXX_SKIP_XXX;
            }
            
            assert(byteswapshort(container->type) == TYPE_DATA);
            assert(byteswapshort(container->code) == code);
            assert(byteswap(container->transaction) == transaction);
            
            assert(byteswap(container->length) - sizeof(*container) <= data_length);
            data_length = byteswap(container->length) - sizeof(*container);

            initial = MIN(total - sizeof(*container), data_length);
            memcpy(data, container+1, initial);
            
            total = initial;
        }

        // then process the remaining data
        rv = usb_bulk_transfer(in, data+initial, data_length-initial, 1);
        if (rv < 0) {
            return rv;
        }

        total += rv;
    } else {
        total = 0;
    }

    length = sizeof(*container)+*response_length;
    assert(length <= sizeof(buffer));

    rv = usb_bulk_transfer(1, buffer, length, 1);
    if (rv < 0) {
        return rv;
    }
    assert(rv >= sizeof(*container) && rv <= length);
    assert(byteswap(container->length) == rv);

XXX_SKIP_XXX:
    code = byteswapshort(container->code);
    if (code != RESPONSE_OK) {
        total = -code;
        assert(total < 0);
    } else {
        if (response_length) {
            assert(byteswapshort(container->type) == TYPE_RESPONSE);
            assert(byteswap(container->transaction) == transaction);
            *response_length = byteswap(container->length) - sizeof(*container);
            memcpy(response, container+1, *response_length);
        }
        assert(total >= 0);
    }
    
    return total;
}

int
pima_transfer(int in, struct pima *pima, byte *data, int data_length)
{
    int rv;
    int request[3];
    int response[3];
    int response_length;
    
    request[0] = byteswap(pima->op1);
    request[1] = byteswap(pima->op2);
    request[2] = byteswap(pima->op3);
    
    response_length = sizeof(response);
    rv = pima_bulk_transfer(in, pima->code, (byte *)request, pima->opn*sizeof(int), data, data_length, (byte *)response, &response_length);
    if (rv >= 0) {
        assert(response_length%sizeof(int) == 0);
        pima->resn = response_length/4;
        pima->res1 = byteswap(response[0]);
        pima->res2 = byteswap(response[1]);
        pima->res3 = byteswap(response[2]);
    }
    
    return rv;
}

static
void
printstring(byte *p)
{
    int i;
    int n;
    
    n = *p;
    for (i = 0; i < n; i++) {
        printf("%c", p[1+2*i]);
    }
}

static
void
printarray(uint16 type, int size, int n, byte *p)
{
    int value;
    
    while (n--) {
        switch (type & 0xff) {
            case 2:
                value = *(uint8 *)p;
                break;
            case 4:
                value = byteswapshort(*(uint16 *)p);
                break;
            case 6:
                value = byteswap(*(uint32 *)p);
                break;
            default:
                assert(0);
        }
        printf("0x%x ", value);
        p += size;
    }
}

void
pima_run(void)
{
#if 0
    int i;
    int j;
#endif
    int rv;
    int once;
#if 0
    int k;
    int m;
    int n;
    int size;
    uint16 type;
    byte *p;
#endif
    struct pima pima;
    byte buffer[0x200];
    
    once = 0;
    for (;;) {
        pima.code = REQUEST_GET_DEVICE_INFO;
        pima.opn = 0;
        rv = pima_transfer(1, &pima, buffer, sizeof(buffer));
        if (rv == -1) {
            break;
        }
        assert(rv > 0);
        led_happy();

        /*
        if (! once) {
            pima.code = REQUEST_OPEN_SESSION;
            pima.opn = 1;
            pima.op1 = 1;  // session id
            rv = pima_transfer(1, &pima, NULL, 0);
            if (rv == -1) {
                break;
            }
            assert(rv == 0);
            led_happy();

            once = 1;
        }
        */

        // try shooting mode sequence?
            pima.code = 0x9008;
            pima.opn = 0;
            rv = pima_transfer(1, &pima, NULL, 0);
            if (rv == -1) {
                break;
            }
        
#if 0
        for (i = 0x1000; i < 0x10000; i++) {
            if (i == 0x1002 || i == 0x1003 || i == 0x100f || (i >= 0x1010 && i <= 0x1013) || i == 0x1016) {
                printf("0x%x:  skipped\n", i);
                continue;
            }
            if (i == 0x9006 || i == 0x9019 || i == 0x901b || i == 0x9021) {
                printf("0x%x:  skipped\n", i);
                continue;
            }
            pima.code = i;
            pima.opn = 0;
            rv = pima_transfer(1, &pima, buffer, sizeof(buffer));
            if (rv == -1) {
                break;
            }
            if (rv != -8197) {
                if (rv < 0) {
                    printf("0x%x: -0x%x\n", i, -rv);
                } else {
                    printf("0x%x:  0x%x\n", i, rv);
                }
                j = 0;
            } else {
                if (j++ == 1024) {
                    j = 0;
                }
            }
        }
#endif        
#if 0
        for (i = 0xd000; i < 0xd800; i++) {
            pima.code = PTP_OC_GetDevicePropDesc;
            pima.opn = 1;
            pima.op1 = i;
            rv = pima_transfer(1, &pima, buffer, sizeof(buffer));
            if (rv == -1) {
                break;
            }
            if (rv > 0) {
                printf("\nProperty %x\n", i);
                assert(byteswapshort(*(short *)buffer) == i);
                type = byteswapshort(*(short *)(buffer+2));
                if (type == 0xffff) {
                    printf("  string %s\n", buffer[4]?"get/set":"get");
                    printf("  defval = ");
                    p = buffer+5;
                    printstring(p);
                    p += 1+(*p)*2;
                    printf("\n");
                    printf("  curval = ");
                    printstring(p);
                    p += 1+(*p)*2;
                    printf("\n");
                } else {
                    switch (type & 0xff) {
                        case 2:
                            size = 1;
                            break;
                        case 4:
                            size = 2;
                            break;
                        case 6:
                            size = 4;
                            break;
                        default:
                            assert(0);
                    }
                    printf("  uint%d %s\n", size*8, buffer[4]?"get/set":"get");
                    p = buffer+5;
                    for (k = 0; k < 2; k++) {
                        if (k) {
                            printf("  curval: ");
                        } else {
                            printf("  defval: ");
                        }
                        
                        if (type & 0xff00) {
                            assert((type & 0xff00) == 0x4000);
                            n = byteswap(*(int *)p);
                            p += 4;
                        } else {
                            n = 1;
                        }
                        printarray(type, size, n, p);
                        p += n*size;
                        printf("\n");
                    }
                }
                switch (*p++) {
                    case 0:
                        break;
                    case 1:
                        printf("  range form\n");
                        assert(0);
                        break;
                    case 2:
                        printf("  enumeration form:\n");
                        m = byteswapshort(*(short *)p);
                        p += 2;
                        while (m--) {
                            printf("    ");
                            if (type & 0xff00) {
                                assert((type & 0xff00) == 0x4000);
                                n = byteswap(*(int *)p);
                                p += 4;
                            } else {
                                n = 1;
                            }
                            printarray(type, size, n, p);
                            p += n*size;
                            printf("\n");
                        }
                        break;
                    default:
                        assert(0);                    
                }
            }
            led_happy();
        }
        printf("\n\n\n");
#endif

        /*
        if (ticks/1000 > secs+10) {
            secs = ticks/1000;

            request[0] = 0;  // default store
            request[1] = 0;  // default format
            rv = pima_bulk_transfer(0, REQUEST_INITIATE_CAPTURE, (byte *)request, 2*sizeof(request[0]), NULL, 0, NULL, NULL);
            if (rv == -1) {
                break;
            }
            assert(rv > 0);
            led_happy();
        }
        */
    }
}
#endif
