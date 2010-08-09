#include "MCF52221.h"

#define NULL  0
#define MIN(a, b)  ((a) < (b) ? (a) : (b))
#define MAX(a, b)  ((a) > (b) ? (a) : (b))

#define assert(x)  if (! (x)) { asm { halt } }

typedef unsigned char bool;
typedef unsigned char byte;

#include "accel.h"
#include "led.h"
#include "timer.h"
#include "usb.h"
#include "canon.h"
#include "scsi.h"
#include "pima.h"
#include "util.h"

extern __declspec(system) uint32 __VECTOR_RAM[];

#ifdef INTERNAL_FLASH
#define LED  1
#define ACCEL  1
#define SCSI  1
#define PIMA  1
#define CANON  1
#else  // INTERNAL_RAM
#define LED  0
#define ACCEL  0
#define SCSI  0
#define PIMA  1
#define CANON  0
#endif

#define RICH  1
