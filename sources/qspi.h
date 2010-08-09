// *** qspi.h *********************************************************

extern void
qspi_transfer(byte *buffer, int length);

extern void
qspi_inactive(bool csiv);

extern void
qspi_baud_fast(void);

extern void
qspi_initialize(void);

