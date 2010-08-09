#include "main.h"

// *** ftdi *****************************************************************

#if FTDI

bool ftdi_echo = true;

#define PACKET_SIZE  64

// We have allocated 8 PIDs to you from A660 to A667 (hex).
// The PIDs must be used with VID 0403.

#define FTDI_VID  0x0403
#define FTDI_PID  0x6001  // 0xA667  // 0x8372  // 0x6001
#define FTDI_RID  0x0400

#define FTDI_RESET  0
#define FTDI_MODEM_CTRL  1
#define FTDI_SET_FLOW_CTRL  2
#define FTDI_SET_BAUD_RATE  3
#define FTDI_SET_DATA  4
#define FTDI_GET_MODEM_STATUS  5
#define FTDI_SET_EVENT_CHAR  6
#define FTDI_SET_ERROR_CHAR  7

static byte ftdi_device_descriptor[] = {
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

static byte ftdi_configuration_descriptor[] = {
    9,  // length
    0x02,  // configuration descriptor
    32, 0,  // total length
    0x01,  // num interfaces
    0x01,  // configuration value
    0x00,  // configuration (string)
    0x80,  // attributes
    100,  // 200 mA

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

static byte ftdi_string_descriptor[] = {
    4,  // length
    0x03, // string descriptor
    0x09, 0x04,  // english (usa)

    28,  // length
    0x03,  // string descriptor
    'R', 0, 'i', 0, 'c', 0, 'h', 0, ' ', 0, 'T', 0, 'e', 0, 's', 0, 't', 0, 'a', 0, 'r', 0, 'd', 0, 'i', 0,

    18,  // length
    0x03,  // string descriptor
    'C', 0, 'P', 0, 'U', 0, 'S', 0, 't', 0, 'i', 0, 'c', 0, 'k', 0,
};

static ftdi_command_cbfn command_cbfn;
static ftdi_ctrlc_cbfn ctrlc_cbfn;
static ftdi_reset_cbfn reset_cbfn;

static byte tx[PACKET_SIZE];  // packet from host

#define NRX  4

static byte rx[NRX][PACKET_SIZE]; // packets to host, including 2 byte headers
static int rx_length[NRX];

#define FTDI_INPUT_LINE_SIZE  72

static byte rx_in;
static byte rx_out;

static bool discard;
static char command[FTDI_INPUT_LINE_SIZE];

static byte hist_in;
static byte hist_out;
static bool hist_first = true;

#define NHIST  8
#define HISTWRAP(x)  (((unsigned)(x))%NHIST)
static char history[NHIST][FTDI_INPUT_LINE_SIZE];

static int ki;
static char keys[8];
static int cursor;
static char echo[FTDI_INPUT_LINE_SIZE*2];

// *** print ****************************************************************

static
void
ftdi_send(byte *buffer, int length)
{
	int m;
    int x;
    bool start;
    
    if (! length) {
    	return;
    }
    
    if (! usb_device_configured) {
        return;
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
	    assert(rx_length[rx_out] > 2);
	    usb_device_enqueue(bulk_in_ep, 1, rx[rx_out], rx_length[rx_out]);
    }

    (void)splx(x);
}

int
ftdi_print(char *line)
{
	int n;
	
	n = strlen(line);

	while (rx_in != rx_out) {
		delay(1);
	}

    ftdi_send((byte *)line, n);

    return n;
}

void
ftdi_edit(char *line)
{
    // put an unmodified copy of the line in history
	strncpy(history[hist_in], line, sizeof(history[hist_in])-1);
	hist_in = HISTWRAP(hist_in+1);

    // and then allow the user to edit it
	strncpy(command, line, sizeof(command)-1);
}

// *** command **************************************************************

void
ftdi_command_discard(bool discard_in)
{
    discard = discard_in;
}

void
ftdi_command_ack(bool edit)
{
    ki = 0;
	hist_first = true;
	
	if (! edit) {
        memset(command, 0, sizeof(command));
	    cursor = 0;
	} else {
	    printf("%s", command);
	    cursor = strlen(command);
	}
    
    // keep the tx ball rolling
    usb_device_enqueue(bulk_out_ep, 0, tx, sizeof(tx));
}

void
ftdi_command_error(int offset)
{
	int i;
	char buffer[2+FTDI_INPUT_LINE_SIZE+1];
	
	assert(offset < FTDI_INPUT_LINE_SIZE);
	
	offset += 2;  // prompt -- revisit, this is decided in cpustick.c!
	
	if (offset >= 10) {
		strcpy(buffer, "error -");
		for (i = 7; i < offset; i++) {
			buffer[i] = ' ';
		}
		buffer[i++] = '^';
		assert(i < sizeof(buffer));
		buffer[i] = '\0';
	} else {
		for (i = 0; i < offset; i++) {
			buffer[i] = ' ';
		}
		buffer[i++] = '^';
		assert(i < sizeof(buffer));
		buffer[i] = '\0';
		strcat(buffer, " - error");
	}
	printf("%s\n", buffer);
}

// *** line editing *********************************************************

enum keys {
	KEY_RIGHT,
	KEY_LEFT,
	KEY_UP,
	KEY_DOWN,
	KEY_HOME,
	KEY_END,
	KEY_BS,
	KEY_BS_DEL,
	KEY_DEL
};

struct keycode {
	char *keys;
	byte code;
} keycodes[] = {
	"\033[C", KEY_RIGHT,
	"\033[D", KEY_LEFT,
	"\033[A", KEY_UP,
	"\033[B", KEY_DOWN,
	"\033[H", KEY_HOME,
	"\033[1~", KEY_HOME,
	"\033[K", KEY_END,
	"\033[4~", KEY_END,
	"\010", KEY_BS,
	"\177", KEY_BS_DEL,  // ambiguous
	"\033[3~", KEY_DEL
};

#define KEYS_RIGHT  "\033[%dC"
#define KEYS_LEFT  "\033[%dD"
#define KEY_DELETE  "\033[P"
#define KEY_CLEAR  "\033[K"

static
void
accumulate(char c)
{
	int i;
	int n;
	int orig;
	int again;
	
	echo[0] = '\0';						
	
	if (c == '\003') {
		if (cursor) {
			sprintf(echo+strlen(echo), KEYS_LEFT, cursor);
			assert(strlen(echo) < sizeof(echo));
			cursor = 0;
		}
		strcat(echo, KEY_CLEAR);
		command[0] = '\0';
		ki = 0;
		ftdi_send((byte *)echo, strlen(echo));
		return;
	}
	
	do {
		again = false;
		
		keys[ki++] = c;
		keys[ki] = '\0';
		assert(ki < sizeof(keys));
		
		for (i = 0; i < LENGTHOF(keycodes); i++) {
			if (! strncmp(keycodes[i].keys, keys, ki)) {
				// potential match
				if (keycodes[i].keys[ki]) {
					// partial match
					return;
				}
				
				// full match
								
				switch (keycodes[i].code) {
					case KEY_RIGHT:
						if (cursor < strlen(command)) {
							strcat(echo, keycodes[i].keys);
							assert(strlen(echo) < sizeof(echo));
							cursor++;
						}
						break;
					case KEY_LEFT:
						if (cursor) {
							strcat(echo, keycodes[i].keys);
							assert(strlen(echo) < sizeof(echo));
							cursor--;	
						}
						break;
						
					case KEY_UP:
					case KEY_DOWN:
						if (keycodes[i].code == KEY_UP) {
							if (hist_first) {
								hist_out = HISTWRAP(hist_in-1);
							} else {
								hist_out = HISTWRAP(hist_out-1);
							}
						} else {
							hist_out = HISTWRAP(hist_out+1);
						}
						hist_first = false;
						for (n = 0; n < NHIST; n++) {
							if (history[hist_out][0]) {
								break;
							}
							if (keycodes[i].code == KEY_UP) {
								hist_out = HISTWRAP(hist_out-1);
							} else {
								hist_out = HISTWRAP(hist_out+1);
							}
						}
						if (n != NHIST) {
							if (cursor) {
								sprintf(echo+strlen(echo), KEYS_LEFT, cursor);
								assert(strlen(echo) < sizeof(echo));
								cursor = 0;
							}
							strcat(echo, KEY_CLEAR);
							strcpy(command, history[hist_out]);
							
							// reprint the line
							strcat(echo, command);
							cursor = strlen(command);
						}
						break;
					case KEY_HOME:
						if (cursor) {
							sprintf(echo+strlen(echo), KEYS_LEFT, cursor);
							assert(strlen(echo) < sizeof(echo));
							cursor = 0;
						}
						break;
					case KEY_END:
						if (strlen(command)-cursor) {
							sprintf(echo+strlen(echo), KEYS_RIGHT, strlen(command)-cursor);
							assert(strlen(echo) < sizeof(echo));
							cursor = strlen(command);
						}
						break;
					case KEY_BS_DEL:
					case KEY_BS:
						if (cursor) {
							strcat(echo, keycodes[KEY_LEFT].keys);
							strcat(echo, KEY_DELETE);
							cursor--;
							memmove(command+cursor, command+cursor+1, sizeof(command)-cursor-1);
						}
						break;
					case KEY_DEL:
						if (command[cursor]) {
							strcat(echo, KEY_DELETE);
							memmove(command+cursor, command+cursor+1, sizeof(command)-cursor-1);
						}
						break;
					default:
						assert(0);
				}
				ki = 0;
				ftdi_send((byte *)echo, strlen(echo));
				return;
			}
		}
		
		// no match
		
		// if we had already accumulated characters...
		if (ki > 1) {
			// we'll have to go around again
			ki--;
			again = true;
		}
		
		// process printable characters
		orig = cursor;
		for (i = 0; i < ki; i++) {
			if (isprint(keys[i])) {
				if (strlen(command) < sizeof(command)-1) {	
					memmove(command+cursor+1, command+cursor, sizeof(command)-cursor-1);
					command[cursor] = keys[i];
					cursor++;
					assert(cursor <= sizeof(command)-1);
				}
			}
		}
		
		if (cursor > orig) {
			// reprint the line
			strcat(echo, command+orig);
			
			// and back the cursor up
			assert(strlen(command+orig));
			if (strlen(command+orig)-1) {
				sprintf(echo+strlen(echo), KEYS_LEFT, strlen(command+orig)-1);
			}
		}
		
		ki = 0;
	} while (again);
	
	if (ftdi_echo) {
    	ftdi_send((byte *)echo, strlen(echo));
	}
}

// *** control transfer *****************************************************

static uint16 eeprom[64] = {
    0x0,
    0x0403,  // VID
    0x6001,  // PID
    0x0400,  // Device release number
    0x9180,  // Config descriptor and Max power consumption
    0x0018,  // Chip configuration
    0x0200,  // USB version
    
    0x1c94,  // vendor
    0x12b0,  // product
    0x12c2,  // serial
    
    0x031c, 'R', 'i', 'c', 'h', ' ', 'T', 'e', 's', 't', 'a', 'r', 'd', 'i',
    0x0312, 'C', 'P', 'U', 'S', 't', 'i', 'c', 'k',
    0x0312, 'R', 'T', '0', '0', '0', '0', '0', '0'
};

static
void
ftdi_checksum(void)
{
    unsigned char i;
    unsigned char *output;
    unsigned short value;
    unsigned short checksum;
    
    output = (unsigned char *)eeprom;
    
    // calculate checksum
    checksum = 0xAAAA;

    for (i = 0; i < LENGTHOF(eeprom)-1; i++) {
        value = output[i*2];
        value += output[(i*2)+1] << 8;

        checksum = value^checksum;
        checksum = (checksum << 1) | (checksum >> 15);
    }

    output[i*2] = checksum;
    output[(i*2)+1] = checksum >> 8;
}

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
        case 0x09:  // set latency timer?
            assert(length == 0);
            break;
        case 0x90:  // read eeprom?
            assert(length == 2);
            buffer[0] = eeprom[BYTESWAP(setup->index)%LENGTHOF(eeprom)] & 0xff;
            buffer[1] = eeprom[BYTESWAP(setup->index)%LENGTHOF(eeprom)] >> 8;
            break;
        case 0x91:  // write eeprom?
            assert(BYTESWAP(setup->index) < LENGTHOF(eeprom));
            eeprom[BYTESWAP(setup->index)] = setup->value;
            assert(length == 0);
            break;
        case 0x92:  // erase eeprom?
            assert(length == 0);
            memset(eeprom, 0xff, sizeof(eeprom));
            ftdi_checksum();
            break;
        default:
            assert(0);
            length = 0;
            break;
    }
    return length;
}

// *** bulk transfer ********************************************************

static int
ftdi_bulk_transfer(bool in, byte *buffer, int length)
{
    int i;
    int j;

    if (! in) {
        // accumulate commands
        i = strlen(command);
        for (j = 0; j < length; j++) {
            if (buffer[j] == '\003') {
                assert(ctrlc_cbfn);
                ctrlc_cbfn();
            }
            
            if (! discard) {
            	if (buffer[j] == '\r') {
            		if (strcmp(history[HISTWRAP(hist_in-1)], command)) {
	            		strcpy(history[hist_in], command);
	            		hist_in = HISTWRAP(hist_in+1);
            		}
            		
                    assert(command_cbfn);
                    command_cbfn(command);
                                        
                    // wait for ftdi_command_ack()
                    return 0;
            	} else {
            		accumulate(buffer[j]);
            	}
            }
        }

        // keep the tx ball rolling
        usb_device_enqueue(bulk_out_ep, 0, tx, sizeof(tx));
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

// *** initialization *******************************************************

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
check(byte *descriptor, int length)
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

void
ftdi_register(ftdi_command_cbfn command, ftdi_ctrlc_cbfn ctrlc, ftdi_reset_cbfn reset)
{
	int i;
	
	for (i = 0; i < NRX; i++) {
		rx[i][0] = 0xf1;  // ftdi header
		rx[i][1] = 0x61;
		rx_length[i] = 2;
	}
	
    command_cbfn = command;
    ctrlc_cbfn = ctrlc;
    reset_cbfn = reset;
    
    ftdi_checksum();

    usb_register(ftdi_reset, ftdi_control_transfer, ftdi_bulk_transfer, NULL);

    assert(check(ftdi_device_descriptor, sizeof(ftdi_device_descriptor)) == 1);
    usb_device_descriptor(ftdi_device_descriptor, sizeof(ftdi_device_descriptor));

    assert(check(ftdi_configuration_descriptor, sizeof(ftdi_configuration_descriptor)) == 4);
    usb_configuration_descriptor(ftdi_configuration_descriptor, sizeof(ftdi_configuration_descriptor));

    assert(check(ftdi_string_descriptor, sizeof(ftdi_string_descriptor)) == 3);
    usb_string_descriptor(ftdi_string_descriptor, sizeof(ftdi_string_descriptor));
}

#endif
