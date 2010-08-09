// *** qspi.c *********************************************************
// this file performs qspi i/o transfers.

#include "main.h"

#define QSPI_BAUD_FAST  500000  // zigbee
#define QSPI_BAUD_SLOW  150000  // default

static int csiv;
static int csav;

// perform both output and input qspi i/o
void
qspi_transfer(byte *buffer, int length)
{
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
        MCF_QSPI_QWR = csav|MCF_QSPI_QWR_ENDQP(request-1)|MCF_QSPI_QWR_NEWQP(0);

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
    MCF_QSPI_QWR = csiv;
    
    splx(x);
}

extern void
qspi_inactive(bool csiv_in)
{
    csiv = csiv_in?MCF_QSPI_QWR_CSIV:0;  // inactive
    csav = csiv_in?0:MCF_QSPI_QWR_CSIV;  // active
    MCF_QSPI_QWR = csiv;
}

extern void
qspi_baud_fast(void)
{
    // initialize qspi master at 500k baud
    assert(fsys_frequency/2/QSPI_BAUD_FAST < 256);
    MCF_QSPI_QMR = MCF_QSPI_QMR_MSTR|/*MCF_QSPI_QMR_CPOL|MCF_QSPI_QMR_CPHA|*/fsys_frequency/2/QSPI_BAUD_FAST;
}

extern void
qspi_initialize(void)
{
    // QS is primary
    MCF_GPIO_PQSPAR = 0x1555;

    // initialize qspi master at 150k baud
    assert(fsys_frequency/2/QSPI_BAUD_SLOW < 256);
    MCF_QSPI_QMR = MCF_QSPI_QMR_MSTR|/*MCF_QSPI_QMR_CPOL|MCF_QSPI_QMR_CPHA|*/fsys_frequency/2/QSPI_BAUD_SLOW;

    // initialize qspi to active low chip select
    qspi_inactive(1);
}

