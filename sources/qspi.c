// *** qspi.c *********************************************************
// this file performs qspi i/o transfers.

#include "main.h"

#define QSPI_BAUD_FAST  500000  // zigbee
#define QSPI_BAUD_SLOW  150000  // default

static bool csiv;

// perform both output and input qspi i/o
void
qspi_transfer(byte *buffer, int length)
{
#if MCF52221 || MCF52233
    int i;
    int x;
    int request;

    x = splx(7);
    
    // while there is data remaining...
    while (length) {
        // process up to 16 bytes at a time
        request = MIN(length, 16);

        // for all bytes...
        for (i = 0; i < request; i++) {
            // set up the command
            MCF_QSPI_QAR = MCF_QSPI_QAR_CMD+i;
            MCF_QSPI_QDR = MCF_QSPI_QDR_CONT;

            // copy tx data to qspi ram
            MCF_QSPI_QAR = MCF_QSPI_QAR_TRANS+i;
            MCF_QSPI_QDR = buffer[i];
        }

        // set the queue pointers
        assert(request);
        MCF_QSPI_QWR = (csiv?0:MCF_QSPI_QWR_CSIV)|MCF_QSPI_QWR_ENDQP(request-1)|MCF_QSPI_QWR_NEWQP(0);

        // start the transfer
        MCF_QSPI_QDLYR = MCF_QSPI_QDLYR_SPE;

        // wait for transfer complete
        assert(! (MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF));
        while (! (MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF)) {
        }
        MCF_QSPI_QIR = MCF_QSPI_QIR_SPIF;

        assert((MCF_QSPI_QWR & 0xf0) >> 4 == request-1);
        assert(! (MCF_QSPI_QDLYR & MCF_QSPI_QDLYR_SPE));

        // for all bytes...
        for (i = 0; i < request; i++) {
            // copy rx data from qspi ram
            MCF_QSPI_QAR = MCF_QSPI_QAR_RECV+i;
            buffer[i] = MCF_QSPI_QDR;
        }

        buffer += request;
        length -= request;
    }

    // transfer complete
    MCF_QSPI_QWR = csiv?MCF_QSPI_QWR_CSIV:0;
    
    splx(x);
#elif MCF51JM128
    // cs active
    if (csiv) {
        PTED &= ~PTEDD_PTEDD7_MASK;
    } else {
        PTED |= PTEDD_PTEDD7_MASK;
    }
    
    while (length) {
        // N.B. spi needs us to read the status register even for release code!
        ASSERT(SPI1S & SPI1S_SPTEF_MASK);
        ASSERT(! (SPI1S & SPI1S_SPRF_MASK));
        
        SPI1DL = *buffer;
        
        while (! (SPI1S & SPI1S_SPTEF_MASK)) {
            // NULL
        }
        
        while (! (SPI1S & SPI1S_SPRF_MASK)) {
            // NULL
        }
        
        *buffer = SPI1DL;
        
        buffer++;
        length--;
    }

    // cs inactive
    if (csiv) {
        PTED |= PTEDD_PTEDD7_MASK;
    } else {
        PTED &= ~PTEDD_PTEDD7_MASK;
    }
#endif
}

extern void
qspi_inactive(bool csiv_in)
{
    csiv = csiv_in;
#if MCF52221 || MCF52233
    MCF_QSPI_QWR = csiv?MCF_QSPI_QWR_CSIV:0;
#elif MCF51JM128
    if (csiv) {
        PTED |= PTEDD_PTEDD7_MASK;
    } else {
        PTED &= ~PTEDD_PTEDD7_MASK;
    }
#endif
}

extern void
qspi_baud_fast(void)
{
#if MCF52221 || MCF52233
    // initialize qspi master at 500k baud
    assert(bus_frequency/QSPI_BAUD_FAST < 256);
    MCF_QSPI_QMR = MCF_QSPI_QMR_MSTR|/*MCF_QSPI_QMR_CPOL|MCF_QSPI_QMR_CPHA|*/bus_frequency/QSPI_BAUD_FAST;
#elif MCF51JM128
    int log2;
    int divisor;
    
    // initialize qspi master at 150k baud
    log2 = 0;
    divisor = bus_frequency/QSPI_BAUD_FAST/2;
    while (divisor > 8) {
        divisor /= 2;
        log2++;
    }
    assert(log2 < 8 && (divisor-1) < 8);
    SPI1BR_SPR = log2;
    SPI1BR_SPPR = divisor-1;
#endif
}

extern void
qspi_initialize(void)
{
#if MCF52221 || MCF52233
    // QS is primary
    MCF_GPIO_PQSPAR = 0x1555;

    // initialize qspi master at 150k baud
    assert(bus_frequency/QSPI_BAUD_SLOW < 256);
    MCF_QSPI_QMR = MCF_QSPI_QMR_MSTR|/*MCF_QSPI_QMR_CPOL|MCF_QSPI_QMR_CPHA|*/bus_frequency/QSPI_BAUD_SLOW;
#elif MCF51JM128
    int log2;
    int divisor;
    
    // E7 is gpio output
    PTEDD |= PTEDD_PTEDD7_MASK;
    
    SPI1C1 = SPI1C1_SPE_MASK|SPI1C1_MSTR_MASK;
    SPI1C2 = 0;
    
    // initialize qspi master at 150k baud
    log2 = 0;
    divisor = bus_frequency/QSPI_BAUD_SLOW/2;
    while (divisor > 8) {
        divisor /= 2;
        log2++;
    }
    assert(log2 < 8 && (divisor-1) < 8);
    SPI1BR_SPR = log2;
    SPI1BR_SPPR = divisor-1;
#endif

    // initialize qspi to active low chip select
    qspi_inactive(1);
}

