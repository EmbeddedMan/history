// *** i2c.c **********************************************************
// this file performs i2c master i/o transfers.

#include "main.h"

#if MCF52221
#define MCF_I2C0_I2CR  MCF_I2C_I2CR
#define MCF_I2C0_I2SR  MCF_I2C_I2SR
#define MCF_I2C0_I2DR  MCF_I2C_I2DR
#define MCF_I2C0_I2FDR  MCF_I2C_I2FDR
#endif

#define I2C_BAUD  100000

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
XXX_AGAIN_XXX:
    // wait for i2c idle
    while (MCF_I2C0_I2SR & MCF_I2C_I2SR_IBB) {
        // NULL
    }
    
    // enable transmit
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_MTX;
    
    // set master mode and generate start
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_MSTA;
    
    // send address and read/write flag
    MCF_I2C0_I2DR = (address<<1)|(! write);
    
    // if we did not get the bus...
    //if (! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IBB)) {
        //while (! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IBB)) {
            // NULL
        //}
        //goto XXX_AGAIN_XXX;
    //}
    
    // wait for byte transmitted
    while( ! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
        // NULL
    }
    MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;
    
    if (! write) {
        // enable receive
        MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MTX;
    }
}

static
void
i2c_repeat_start_real(bool write)
{
    // enable transmit
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_MTX;

    // generate repeat start
    MCF_I2C0_I2CR |= MCF_I2C_I2CR_RSTA;
    
    // send address and read/write flag
    MCF_I2C0_I2DR = (address<<1)|(! write);
    
    // wait for byte transmitted
    while( ! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
        // NULL
    }
    MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

    if (! write) {
        // enable receive
        MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MTX;
    }
}

void
i2c_stop(void)
{
    // generate stop
    MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MSTA;
    
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

    if (write) {
        while (length--) {
            // send data
            MCF_I2C0_I2DR = *buffer++;
            
            // wait for byte transmitted
            while( ! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
                // NULL
            }
            MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;
            
            // if no ack...
            if (MCF_I2C0_I2SR & MCF_I2C_I2SR_RXAK) {
                break;
            }
        }        
    } else {
        // dummy read starts the read process from the slave
        (void)MCF_I2C0_I2DR;
        
        while (length--) {
            // if this is not the last byte...
            if (length) {
                // ack
                MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_TXAK;
            } else {
                // no ack
                MCF_I2C0_I2CR |= MCF_I2C_I2CR_TXAK;
            }

            // wait for byte received
            while( ! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
                // NULL
            }
            MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;
            
            // get the data
            *buffer++ = (byte)MCF_I2C0_I2DR;
        }
        
        // wait for byte received
        while( ! (MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF)) {
            // NULL
        }
        MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;
    }    
}

bool
i2c_ack()
{
    return !!(MCF_I2C0_I2SR & MCF_I2C_I2SR_RXAK);
}

void
i2c_initialize(void)
{
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

}

