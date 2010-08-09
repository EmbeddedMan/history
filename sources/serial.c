// *** serial.c *******************************************************
// This file implements an interrupt driven serial device driver.

#include "main.h"

#if SERIAL_DRIVER

// This could be shrunk if RAM is precious.
enum { tx_buffer_size = 64 };

static byte tx_buffer[tx_buffer_size];
static byte tx_offset;
static byte tx_len;

INTERRUPT
void
serial_isr(void)
{
    const uint8 usr = MCF_UART0_USR;

    if (usr & MCF_UART_USR_RXRDY) {
        const uint8 byte = MCF_UART0_URB;

        if (usr & (MCF_UART_USR_FE | MCF_UART_USR_OE | MCF_UART_USR_RB)) {
            // Reset error status.
            MCF_UART0_UCR = MCF_UART_UCR_RESET_ERROR;
        } else {
            terminal_receive(&byte, 1);
        }
    }

    if (usr & MCF_UART_USR_TXRDY) {
        if (! tx_len) {
            // No more data to send.  Disable TXRDY interrupts.
            MCF_UART0_UCR = MCF_UART_UCR_RX_ENABLED | MCF_UART_UCR_TX_DISABLED;
        } else {
            // Send next byte to host.
            MCF_UART0_UTB = tx_buffer[tx_offset++];
            tx_len--;
        }
    }
}

void
serial_send(const byte *buffer, int length)
{
    int x;

    while (length > 0) {
        // Spin wait for transmitter space.  Could be more
        // aggressive by appending to existing transmit data
        // (space permitting).
        while (1) {
            x = splx(7);
            // XXX: Need assertion checking that serial interrupt are enabled and unmasked.
            if (! tx_len) {
                break;
            }
            splx(x);
        }
        
        tx_len = MIN(length, tx_buffer_size);
        tx_offset = 0;
        memcpy(tx_buffer, buffer, tx_len);
        
        length -= tx_len;
        buffer += tx_len;
        
        // Enable tx interrupts.
        MCF_UART0_UCR = MCF_UART_UCR_RX_ENABLED | MCF_UART_UCR_TX_ENABLED;
        
        splx(x);
    }
}

void
serial_initialize(void)
{
    const int uart_num = 0;
    const int baudrate = 115200;
    const uint32 divider = cpu_frequency / baudrate / 32;

    // UART 0
    // PORT UA (4 pins)
    // UCTS0#, URTS0#, URXD0, and UTXD0 are "primary" functions of
    // their pin group.
    // Set to primary function (0x01).
    MCF_GPIO_PUAPAR = 0x55;
    
    // Set URTS0# (PUA[2]) and UTXD0 (PUA[0]) as outputs.
    MCF_GPIO_DDRUA = 0x5;

    MCF_UART_UCR(uart_num) = MCF_UART_UCR_RESET_TX | 
        MCF_UART_UCR_RESET_RX | MCF_UART_UCR_RESET_MR;

    MCF_UART_UIMR(uart_num) = MCF_UART_UIMR_TXRDY | MCF_UART_UIMR_FFULL_RXRDY;
    
    // Use internal bus clock for uart clock.
    MCF_UART_UCSR(uart_num) = 0xdd;

    // Baud rate divider = (UBG1 << 8) | UBG2
    MCF_UART_UBG1(uart_num) = divider >> 8;
    MCF_UART_UBG2(uart_num) = divider;

    // Set UMR1n.
    MCF_UART0_UMR1 = MCF_UART_UMR_BC_8 | MCF_UART_UMR_PM_NONE;
    // Set UMR2n.
    MCF_UART0_UMR2 = MCF_UART_UMR_CM_NORMAL | MCF_UART_UMR_SB_STOP_BITS_1;

    // UCRn: enable transmitter and receiver.
    MCF_UART_UCR(uart_num) = MCF_UART_UCR_TX_ENABLED |
        MCF_UART_UCR_RX_ENABLED;

    // Enable uart interrupt.
    MCF_INTC0_ICR13 = MCF_INTC_ICR_IL(SPL_SERIAL)|MCF_INTC_ICR_IP(SPL_SERIAL);
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK13;
}

#endif // SERIAL_DRIVER
