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

void
i2c_start(int address_in)
{
    address = (byte)address_in;
    started = false;
}

static
void
i2c_start_real(bool write)
{
    i2c_stop();

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
//XXX_AGAIN_XXX:
    // wait for i2c idle
    while (MCF_I2C0_I2SR & MCF_I2C_I2SR_IBB) {
        assert(! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IAL));
    }
    
    // enable transmit
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_MTX;
    
    // set master mode and generate start
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_MSTA;
    
    // send address and read/write flag
    MCF_I2C0_I2DR = (address<<1)|(! write);
    
    // XXX -- why doesn't this work?
    // if we did not get the bus...
    //if (! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IBB)) {
        //while (! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IBB)) {
            //assert(! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IAL));
        //}
        //goto XXX_AGAIN_XXX;
    //}
    
    // wait for byte transmitted
    while( ! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
        assert(! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IAL));
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

        // generate start
        rv = I2CStart(I2C1);
        assert(rv == I2C_SUCCESS);

        // send address and read/write flag
        data = (address<<1)|(! write);
        i2c_read_write(true, &data, sizeof(data));
    }
#else
#error
#endif
}

static
void
i2c_repeat_start_real(bool write)
{
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // enable transmit
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_MTX;

    // generate repeat start
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_RSTA;
    
    // send address and read/write flag
    MCF_I2C0_I2DR = (address<<1)|(! write);
    
    // wait for byte transmitted
    while( ! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
        assert(! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IAL));
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

        // send address and read/write flag
        data = (address<<1)|(! write);
        i2c_read_write(true, &data, sizeof(data));
    }
#else
#error
#endif
}

void
i2c_stop(void)
{
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    // enable receive
    MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MTX;

    // generate stop
    MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MSTA;

    // wait for i2c idle
    while (MCF_I2C0_I2SR & MCF_I2C_I2SR_IBB) {
        assert(! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IAL));
    }
#elif PIC32
    {
        // generate stop
        I2CStop(I2C1);
    }
#else
#error
#endif

    started = false;
}

void
i2c_read_write(bool write, byte *buffer, int length)
{
    // if we need a start...
    if (! started) {
        i2c_start_real(write);
        started = true;
    // otherwise, if we need a restart...
    } else {
        i2c_repeat_start_real(write);
    }

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    if (write) {
        while (length--) {
            // send data
            MCF_I2C0_I2DR = *buffer++;
            
            // wait for byte transmitted
            while( ! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
                assert(! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IAL));
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
                assert(! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IAL));
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
                // send data
                rv = I2CSendByte(I2C1, *buffer++);
                assert(rv == I2C_SUCCESS);

                // wait for byte transmitted
                while (! I2CTransmissionHasCompleted(I2C1)) {
                    assert(! (I2CGetStatus(I2C1) & I2C_ARBITRATION_LOSS));
                }

                // if no ack...
                if (! I2CByteWasAcknowledged(I2C1)) {
                    break;
                }
            }
        } else {
            while (length--) {
                while (length--) {
                    // wait for byte received and previous acknowledge
                    while(! I2CReceivedDataIsAvailable(I2C1)) {
                        assert(! (I2CGetStatus(I2C1) & I2C_ARBITRATION_LOSS));
                    }
                    while (! (I2C1CON && _I2C1CON_ACKEN_MASK)) {
                        assert(! (I2CGetStatus(I2C1) & I2C_ARBITRATION_LOSS));
                    }

                    // if this is not the last byte...
                    if (length) {
                        // ack
                        I2CAcknowledgeByte(I2C1, true);
                    } else if (length == 1) {
                        // no ack
                        I2CAcknowledgeByte(I2C1, false);
                    }

                    // get the data
                    *buffer++ = I2CGetByte(I2C1);
                }
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
#if MCF52221 || MCF52233 || MCF52259 || MCF5211
    return !!(MCF_I2C0_I2SR & MCF_I2C_I2SR_RXAK);
#elif PIC32
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
    I2C1CON = 0;

    assert(bus_frequency/I2C_BAUD/2 - 2 < 4096);
    I2C1BRG = bus_frequency/I2C_BAUD/2 - 2;
    
    I2C1CON = _I2C1CON_ON_MASK|_I2C1CON_SCLREL_MASK;
#else
#error
#endif
}
