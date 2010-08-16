// *** i2c.c **********************************************************
// this file performs i2c master i/o transfers.

#include "main.h"

#define I2C_BAUD  100000

#if MCF52221 || MCF52233 || MCF5211
#define MCF_I2C0_I2CR  MCF_I2C_I2CR
#define MCF_I2C0_I2SR  MCF_I2C_I2SR
#define MCF_I2C0_I2DR  MCF_I2C_I2DR
#define MCF_I2C0_I2FDR  MCF_I2C_I2FDR
#endif

static byte address;
static bool started;
static int breaks;

static
void
i2c_broke(void)
{
    i2c_initialize();
    printf("i2c reset\n");
#if STICKOS
    stop();
#endif
}

static
bool  // indicates things are awry
i2c_break(void)
{
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    if (MCF_I2C0_I2SR & MCF_I2C_I2SR_IAL) {
        i2c_broke();
        return true;
    }
#elif PIC32
    I2C_STATUS status;

    status = I2CGetStatus(I2C1);
    if (status & (I2C_ARBITRATION_LOSS|I2C_TRANSMITTER_OVERFLOW|I2C_RECEIVER_OVERFLOW)) {
        i2c_broke();
        return true;
    }
#else
#error
#endif
    if (run_breaks != breaks) {
        i2c_broke();
        return true;
    }
    return false;
}

void
i2c_start(int address_in)
{
    address = (byte)address_in;
    started = false;
}

static
bool
i2c_start_real(bool write)
{
    breaks = run_breaks;

    i2c_stop();

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // wait for i2c idle
    while (MCF_I2C0_I2SR & MCF_I2C_I2SR_IBB) {
        if (i2c_break()) {
            return started;
        }
    }

    // enable transmit
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_MTX;

    // set master mode and generate start
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_MSTA;

    // send address and read/write flag
    MCF_I2C0_I2DR = (address<<1)|(! write);

    // wait for byte transmitted
    while (! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
        if (i2c_break()) {
            return started;
        }
    }
    MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

    if (! write) {
        // enable receive
        MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MTX;
    }
#elif PIC32
    {
        I2C_RESULT rv;

        // wait for i2c idle
        while (! I2CBusIsIdle(I2C1)) {
            if (i2c_break()) {
                return started;
            }
        }

        // generate start
        rv = I2CStart(I2C1);
        if (rv != I2C_SUCCESS) {
            i2c_broke();
            return started;
        }
        assert(rv == I2C_SUCCESS);

        // wait for start
        while (I2C1CONbits.SEN || I2C1CONbits.RSEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT) {
            if (i2c_break()) {
                return started;
            }
        }

        // wait for transmitter ready
        while (! I2CTransmitterIsReady(I2C1)) {
            if (i2c_break()) {
                return started;
            }
        }

        // send address and read/write flag
        rv = I2CSendByte(I2C1, (address<<1)|(! write));
        assert(rv == I2C_SUCCESS);

        // wait for byte transmitted
        while (! I2CTransmissionHasCompleted(I2C1)) {
            if (i2c_break()) {
                return started;
            }
        }

        while (I2C1CONbits.SEN || I2C1CONbits.RSEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT) {
            if (i2c_break()) {
                return started;
            }
        }

        if (! write) {
            // enable receive
            rv = I2CReceiverEnable(I2C1, true);
            assert(rv == I2C_SUCCESS);
        }
    }
#else
#error
#endif

    started = true;
    return started;
}

static
void
i2c_repeat_start_real(bool write)
{
    breaks = run_breaks;

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // enable transmit
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_MTX;

    // generate repeat start
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_RSTA;

    // send address and read/write flag
    MCF_I2C0_I2DR = (address<<1)|(! write);

    // wait for byte transmitted
    while( ! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
        if (i2c_break()) {
            return;
        }
    }
    MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

    if (! write) {
        // enable receive
        MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MTX;
    }
#elif PIC32
    {
        byte data;
        I2C_RESULT rv;

        // generate repeat start
        rv = I2CRepeatStart(I2C1);
        assert(rv == I2C_SUCCESS);

        // wait for repeat start
        while (I2C1CONbits.SEN || I2C1CONbits.RSEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT) {
            if (i2c_break()) {
                return;
            }
        }

        // wait for transmitter ready
        while (! I2CTransmitterIsReady(I2C1)) {
            if (i2c_break()) {
                return;
            }
        }

        // send address and read/write flag
        rv = I2CSendByte(I2C1, (address<<1)|(! write));
        assert(rv == I2C_SUCCESS);

        // wait for byte transmitted
        while (! I2CTransmissionHasCompleted(I2C1)) {
            if (i2c_break()) {
                return;
            }
        }

        while (I2C1CONbits.SEN || I2C1CONbits.RSEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT) {
            if (i2c_break()) {
                return;
            }
        }

        if (! write) {
            // enable receive
            rv = I2CReceiverEnable(I2C1, true);
            assert(rv == I2C_SUCCESS);
        }
    }
#else
#error
#endif
}

void
i2c_stop(void)
{
    breaks = run_breaks;

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // enable receive
    MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MTX;

    // generate stop
    MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MSTA;

    // wait for i2c idle
    while (MCF_I2C0_I2SR & MCF_I2C_I2SR_IBB) {
        if (i2c_break()) {
            return;
        }
    }
#elif PIC32
    {
        I2C_RESULT rv;

        if (! I2CBusIsIdle(I2C1)) {
            // generate stop
            I2CStop(I2C1);
        }

        // wait for stop
        while (I2C1CONbits.SEN || I2C1CONbits.RSEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT) {
            if (i2c_break()) {
                return;
            }
        }
    }
#else
#error
#endif

    started = false;
}

void
i2c_read_write(bool write, byte *buffer, int length)
{
    breaks = run_breaks;

    // if we need a start...
    if (! started) {
        // if we timed out...
        if (! i2c_start_real(write)) {
            return;
        }
    } else {
        // we need a restart
        i2c_repeat_start_real(write);
    }

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    if (write) {
        while (length--) {
            // send data
            MCF_I2C0_I2DR = *buffer++;

            // wait for byte transmitted
            while( ! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
                if (i2c_break()) {
                    return;
                }
            }
            MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

            // if no ack...
            if (MCF_I2C0_I2SR & MCF_I2C_I2SR_RXAK) {
                break;
            }
        }
    } else {
        // if this is not the (second to the) last byte...
        if (length > 1) {
            // ack
            MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_TXAK;
        } else if (length == 1) {
            // no ack
            MCF_I2C0_I2CR |= MCF_I2C_I2CR_TXAK;
        } else {
            assert(0);
        }

        // dummy read starts the read process from the slave
        (void)MCF_I2C0_I2DR;

        while (length--) {
            // wait for byte received
            while( ! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
                if (i2c_break()) {
                    return;
                }
            }
            MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

            // if this is not the (second to the) last byte...
            if (length > 1) {
                // ack
                MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_TXAK;
            } else if (length == 1) {
                // no ack
                MCF_I2C0_I2CR |= MCF_I2C_I2CR_TXAK;
            } else {
                // stop before we accidentally initiate a new read process from the slave
                i2c_stop();
            }

            // get the data
            *buffer++ = (byte)MCF_I2C0_I2DR;
        }
    }
#elif PIC32
    {
        I2C_RESULT rv;

        if (write) {
            while (length--) {
                // wait for transmitter ready
                while (! I2CTransmitterIsReady(I2C1)) {
                    if (i2c_break()) {
                        return;
                    }
                }

                // send data
                rv = I2CSendByte(I2C1, *buffer++);
                assert(rv == I2C_SUCCESS);

                // wait for byte transmitted
                while (! I2CTransmissionHasCompleted(I2C1)) {
                    if (i2c_break()) {
                        return;
                    }
                }

                // if no ack...
                if (! I2CByteWasAcknowledged(I2C1)) {
                    break;
                }
            }
        } else {
            while (length--) {
                // wait for byte received and previous acknowledge
                while(! I2CReceivedDataIsAvailable(I2C1)) {
                    if (i2c_break()) {
                        return;
                    }
                }

                // if this is not the last byte...
                if (length) {
                    // ack
                    I2CAcknowledgeByte(I2C1, true);
                    while (I2C1CONbits.SEN || I2C1CONbits.RSEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT) {
                        if (i2c_break()) {
                            return;
                        }
                    }

                    // enable receive
                    rv = I2CReceiverEnable(I2C1, true);
                    assert(rv == I2C_SUCCESS);
                } else if (length == 1) {
                    // no ack
                    I2CAcknowledgeByte(I2C1, false);
                    while (I2C1CONbits.SEN || I2C1CONbits.RSEN || I2C1CONbits.PEN || I2C1CONbits.RCEN || I2C1CONbits.ACKEN || I2C1STATbits.TRSTAT) {
                        if (i2c_break()) {
                            return;
                        }
                    }
                }

                // get the data
                *buffer++ = I2CGetByte(I2C1);
            }
        }
    }
#else
#error
#endif
}

bool
i2c_ack()
{
    breaks = run_breaks;

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    return !!(MCF_I2C0_I2SR & MCF_I2C_I2SR_RXAK);
#elif PIC32
    return I2CByteWasAcknowledged(I2C1);
#else
#error
#endif
}

void
i2c_initialize(void)
{
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    int ic;
    int div;

    // AS is primary
    MCF_GPIO_PASPAR = 0x05;

    // stop i2c
    MCF_I2C0_I2CR = 0;

    // find a baud rate less than 100kHz
    ic = 0x23;
    div = 16;
    while (bus_frequency/div > I2C_BAUD) {
        ic += 4;
        div *= 2;
    }
    assert(ic <= 0x3f);
    MCF_I2C0_I2FDR = MCF_I2C_I2FDR_IC(ic);

    // start i2c
    MCF_I2C0_I2CR = MCF_I2C_I2CR_IEN;

    // if i2c needs a whack on the head...
    if(MCF_I2C0_I2SR & MCF_I2C_I2SR_IBB) {
        MCF_I2C0_I2CR = 0;
        MCF_I2C0_I2CR = MCF_I2C_I2CR_IEN|MCF_I2C_I2CR_MSTA;
        (void)MCF_I2C0_I2DR;
        MCF_I2C0_I2SR = 0;
        MCF_I2C0_I2CR = 0;
        MCF_I2C0_I2CR = MCF_I2C_I2CR_IEN;
    }
#elif PIC32
    I2CEnable(I2C1, false);

    I2CSetFrequency(I2C1, bus_frequency, I2C_BAUD);

    I2CEnable(I2C1, true);
#else
#error
#endif
}
