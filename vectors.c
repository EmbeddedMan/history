// *** vector.c *******************************************************
// this file contains page1 interrupt vectors for all interrupts.  edit
// this file to hook up __declspec(interrupt) functions to hardware
// interrupts.

#if ! PIC32 && ! MC9S08QE128 && ! MC9S12DT256 && ! MC9S12DP512
#include "main.h"

// *** page1 ***

#pragma explicit_zero_data on
#pragma define_section page1 ".page1" far_absolute R

#define NOP  0x4E71
#define JMP  0x4EF9
#define HALT  0x4AC8
#define RTE  0x4e73

#define UINT32JMP  (NOP<<16|JMP)
#define UINT32HALT  (HALT<<16|RTE)

#if MCF52233
extern __declspec(interrupt) void fec_isr(void);
#endif

#if MCF52221 || MCF52233 || MCF52259 || MCF5211
// this is the software interrupt vector table, in page1.
#if ! BADGE_BOARD && ! DEMO_KIT
DECLSPEC_PAGE1
#else
const
#endif
uint32 _swvect[256] = {
#if ! FLASHER
    (uint32)flash_upgrade_ram_begin, (uint32)flash_upgrade_ram_end,
#else
    0, 0,
#endif
    (uint32)init, 0,
    UINT32HALT, 0,                               // 2
    UINT32HALT, 0,                               // 3
    UINT32HALT, 0,                               // 4
    UINT32HALT, 0,                               // 5
    UINT32HALT, 0,                               // 6
    UINT32HALT, 0,                               // 7
    UINT32HALT, 0,                               // 8
    UINT32HALT, 0,                               // 9
    UINT32HALT, 0,                               // 10
    UINT32HALT, 0,                               // 11
    UINT32HALT, 0,                               // 12
    UINT32HALT, 0,                               // 13
    UINT32HALT, 0,                               // 14
    UINT32HALT, 0,                               // 15
    UINT32HALT, 0,                               // 16
    UINT32HALT, 0,                               // 17
    UINT32HALT, 0,                               // 18
    UINT32HALT, 0,                               // 19
    UINT32HALT, 0,                               // 20
    UINT32HALT, 0,                               // 21
    UINT32HALT, 0,                               // 22
    UINT32HALT, 0,                               // 23
    UINT32HALT, 0,                               // 24
    UINT32HALT, 0,                               // 25
    UINT32HALT, 0,                               // 26
    UINT32HALT, 0,                               // 27
    UINT32HALT, 0,                               // 28
    UINT32HALT, 0,                               // 29
    UINT32HALT, 0,                               // 30
    UINT32HALT, 0,                               // 31
    UINT32HALT, 0,                               // 32
    UINT32HALT, 0,                               // 33
    UINT32HALT, 0,                               // 34
    UINT32HALT, 0,                               // 35
    UINT32HALT, 0,                               // 36
    UINT32HALT, 0,                               // 37
    UINT32HALT, 0,                               // 38
    UINT32HALT, 0,                               // 39
    UINT32HALT, 0,                               // 40
    UINT32HALT, 0,                               // 41
    UINT32HALT, 0,                               // 42
    UINT32HALT, 0,                               // 43
    UINT32HALT, 0,                               // 44
    UINT32HALT, 0,                               // 45
    UINT32HALT, 0,                               // 46
    UINT32HALT, 0,                               // 47
    UINT32HALT, 0,                               // 48
    UINT32HALT, 0,                               // 49
    UINT32HALT, 0,                               // 50
    UINT32HALT, 0,                               // 51
    UINT32HALT, 0,                               // 52
    UINT32HALT, 0,                               // 53
    UINT32HALT, 0,                               // 54
    UINT32HALT, 0,                               // 55
    UINT32HALT, 0,                               // 56
    UINT32HALT, 0,                               // 57
    UINT32HALT, 0,                               // 58
    UINT32HALT, 0,                               // 59
    UINT32HALT, 0,                               // 60
    UINT32HALT, 0,                               // 61
    UINT32HALT, 0,                               // 62
    UINT32HALT, 0,                               // 63
    UINT32HALT, 0,                               // 64
#if ! FLASHER && ! PICTOCRYPT && (MCF52259 || MCF5211)
    UINT32JMP, (uint32)zb_isr,                   // 65 - irq1*
#else
    UINT32HALT, 0,                               // 65
#endif
    UINT32HALT, 0,                               // 66
    UINT32HALT, 0,                               // 67
#if ! FLASHER && ! PICTOCRYPT && ! MCF52259 && ! MCF5211
    UINT32JMP, (uint32)zb_isr,                   // 68 - irq4*
#else
    UINT32HALT, 0,                               // 68
#endif
    UINT32HALT, 0,                               // 69
    UINT32HALT, 0,                               // 70
    UINT32HALT, 0,                               // 71
    UINT32HALT, 0,                               // 72
    UINT32HALT, 0,                               // 73
    UINT32HALT, 0,                               // 74
    UINT32HALT, 0,                               // 75
    UINT32HALT, 0,                               // 76
#if ! FLASHER && ! PICTOCRYPT
    UINT32JMP, (uint32)serial_isr,               // 77 - uart0
#else
    UINT32HALT, 0,                               // 77
#endif
    UINT32HALT, 0,                               // 78
    UINT32HALT, 0,                               // 79
    UINT32HALT, 0,                               // 80
    UINT32HALT, 0,                               // 81
    UINT32HALT, 0,                               // 82
    UINT32HALT, 0,                               // 83
    UINT32HALT, 0,                               // 84
    UINT32HALT, 0,                               // 85
    UINT32HALT, 0,                               // 86
#if MCF52233
    UINT32JMP, (uint32)fec_isr,                  // 87 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 88 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 89 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 90 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 91 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 92 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 93 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 94 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 95 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 96 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 97 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 98 ifec.c
    UINT32JMP, (uint32)fec_isr,                  // 99 ifec.c
#else
    UINT32HALT, 0,                               // 87
    UINT32HALT, 0,                               // 88
    UINT32HALT, 0,                               // 89
    UINT32HALT, 0,                               // 90
    UINT32HALT, 0,                               // 91
    UINT32HALT, 0,                               // 92
    UINT32HALT, 0,                               // 93
    UINT32HALT, 0,                               // 94
    UINT32HALT, 0,                               // 95
    UINT32HALT, 0,                               // 96
    UINT32HALT, 0,                               // 97
    UINT32HALT, 0,                               // 98
    UINT32HALT, 0,                               // 99
#endif
    UINT32HALT, 0,                               // 100
    UINT32HALT, 0,                               // 101
    UINT32HALT, 0,                               // 102
    UINT32HALT, 0,                               // 103
    UINT32HALT, 0,                               // 104
    UINT32HALT, 0,                               // 105
    UINT32HALT, 0,                               // 106
    UINT32HALT, 0,                               // 107
    UINT32HALT, 0,                               // 108
    UINT32HALT, 0,                               // 109
    UINT32HALT, 0,                               // 110
    UINT32HALT, 0,                               // 111
    UINT32HALT, 0,                               // 112
    UINT32HALT, 0,                               // 113
    UINT32HALT, 0,                               // 114
    UINT32HALT, 0,                               // 115
    UINT32HALT, 0,                               // 116
#if (MCF52221 || MCF52259) && ! FLASHER
    UINT32JMP, (uint32)usb_isr,                  // 117 usb.c
#else
    UINT32HALT, 0,                               // 117
#endif
    UINT32HALT, 0,                               // 118
    UINT32JMP, (uint32)timer_isr,                // 119 timer.c
    UINT32HALT, 0,                               // 120
    UINT32HALT, 0,                               // 121
    UINT32HALT, 0,                               // 122
    UINT32HALT, 0,                               // 123
    UINT32HALT, 0,                               // 124
    UINT32HALT, 0,                               // 125
    UINT32HALT, 0,                               // 126
    UINT32HALT, 0,                               // 127
};
#elif MCF51JM128 || MCF51CN128 || MCF51QE128
// this is the software interrupt vector table, in page1.
#if ! BADGE_BOARD && ! DEMO_KIT
__declspec(page1)
#else
const
#endif
uint32 _swvect[256] = {
    (uint32)flash_upgrade_ram_begin, (uint32)flash_upgrade_ram_end,
    (uint32)init, 0,
    UINT32HALT, 0,                               // 2
    UINT32HALT, 0,                               // 3
    UINT32HALT, 0,                               // 4
    UINT32HALT, 0,                               // 5
    UINT32HALT, 0,                               // 6
    UINT32HALT, 0,                               // 7
    UINT32HALT, 0,                               // 8
    UINT32HALT, 0,                               // 9
    UINT32HALT, 0,                               // 10
    UINT32HALT, 0,                               // 11
    UINT32HALT, 0,                               // 12
    UINT32HALT, 0,                               // 13
    UINT32HALT, 0,                               // 14
    UINT32HALT, 0,                               // 15
    UINT32HALT, 0,                               // 16
    UINT32HALT, 0,                               // 17
    UINT32HALT, 0,                               // 18
    UINT32HALT, 0,                               // 19
    UINT32HALT, 0,                               // 20
    UINT32HALT, 0,                               // 21
    UINT32HALT, 0,                               // 22
    UINT32HALT, 0,                               // 23
    UINT32HALT, 0,                               // 24
    UINT32HALT, 0,                               // 25
    UINT32HALT, 0,                               // 26
    UINT32HALT, 0,                               // 27
    UINT32HALT, 0,                               // 28
    UINT32HALT, 0,                               // 29
    UINT32HALT, 0,                               // 30
    UINT32HALT, 0,                               // 31
    UINT32HALT, 0,                               // 32
    UINT32HALT, 0,                               // 33
    UINT32HALT, 0,                               // 34
    UINT32HALT, 0,                               // 35
    UINT32HALT, 0,                               // 36
    UINT32HALT, 0,                               // 37
    UINT32HALT, 0,                               // 38
    UINT32HALT, 0,                               // 39
    UINT32HALT, 0,                               // 40
    UINT32HALT, 0,                               // 41
    UINT32HALT, 0,                               // 42
    UINT32HALT, 0,                               // 43
    UINT32HALT, 0,                               // 44
    UINT32HALT, 0,                               // 45
    UINT32HALT, 0,                               // 46
    UINT32HALT, 0,                               // 47
    UINT32HALT, 0,                               // 48
    UINT32HALT, 0,                               // 49
    UINT32HALT, 0,                               // 50
    UINT32HALT, 0,                               // 51
    UINT32HALT, 0,                               // 52
    UINT32HALT, 0,                               // 53
    UINT32HALT, 0,                               // 54
    UINT32HALT, 0,                               // 55
    UINT32HALT, 0,                               // 56
    UINT32HALT, 0,                               // 57
    UINT32HALT, 0,                               // 58
    UINT32HALT, 0,                               // 59
    UINT32HALT, 0,                               // 60
    UINT32HALT, 0,                               // 61
    UINT32HALT, 0,                               // 62
    UINT32HALT, 0,                               // 63
    UINT32JMP, (uint32)zb_pre_isr,               // 64 zigflea.c
    UINT32HALT, 0,                               // 65
    UINT32HALT, 0,                               // 66
    UINT32HALT, 0,                               // 67
    UINT32HALT, 0,                               // 68
#if MCF51JM128
    UINT32JMP, (uint32)usb_isr,                  // 69 usb.c
#else
    UINT32HALT, 0,                               // 69
#endif
    UINT32HALT, 0,                               // 70
    UINT32HALT, 0,                               // 71
    UINT32HALT, 0,                               // 72
    UINT32HALT, 0,                               // 73
    UINT32HALT, 0,                               // 74
    UINT32HALT, 0,                               // 75
    UINT32HALT, 0,                               // 76
#if MCF51QE128
    UINT32JMP, (uint32)serial_isr,               // 77 serial.c
#else
    UINT32HALT, 0,                               // 77
#endif
    UINT32HALT, 0,                               // 78
    UINT32HALT, 0,                               // 79
    UINT32HALT, 0,                               // 80
    UINT32HALT, 0,                               // 81
#if MCF51JM128
    UINT32JMP, (uint32)serial_isr,               // 82 serial.c
#else
    UINT32HALT, 0,                               // 82
#endif
#if MCF51CN128
    UINT32JMP, (uint32)serial_isr,               // 83 serial.c
#else
    UINT32HALT, 0,                               // 83
#endif
    UINT32HALT, 0,                               // 84
    UINT32HALT, 0,                               // 85
#if MCF51QE128
    UINT32JMP, (uint32)timer_isr,                // 86 timer.c
#else
    UINT32HALT, 0,                               // 86
#endif
    UINT32HALT, 0,                               // 87
    UINT32HALT, 0,                               // 88
    UINT32HALT, 0,                               // 89
    UINT32HALT, 0,                               // 90
#if MCF51JM128
    UINT32JMP, (uint32)timer_isr,                // 91 timer.c
#else
    UINT32HALT, 0,                               // 91
#endif
    UINT32HALT, 0,                               // 92
    UINT32HALT, 0,                               // 93
    UINT32HALT, 0,                               // 94
    UINT32HALT, 0,                               // 95
    UINT32HALT, 0,                               // 96
    UINT32HALT, 0,                               // 97
    UINT32HALT, 0,                               // 98
#if MCF51QE128
    UINT32JMP, (uint32)zb_isr,                   // 99 zigflea.c
#else
    UINT32HALT, 0,                               // 99
#endif
    UINT32HALT, 0,                               // 100
    UINT32HALT, 0,                               // 101
    UINT32HALT, 0,                               // 102
    UINT32HALT, 0,                               // 103
    UINT32HALT, 0,                               // 104
    UINT32HALT, 0,                               // 105
#if MCF51CN128
    UINT32JMP, (uint32)zb_isr,                   // 106 zigflea.c
#else
    UINT32HALT, 0,                               // 106
#endif
#if MCF51JM128
    UINT32JMP, (uint32)zb_isr,                   // 107 zigflea.c
#else
    UINT32HALT, 0,                               // 107
#endif
    UINT32HALT, 0,                               // 108
    UINT32HALT, 0,                               // 109
    UINT32HALT, 0,                               // 110
    UINT32HALT, 0,                               // 111
    UINT32HALT, 0,                               // 112
    UINT32HALT, 0,                               // 113
#if MCF51CN128
    UINT32JMP, (uint32)timer_isr,                // 114 timer.c
#else
    UINT32HALT, 0,                               // 114
#endif
    UINT32HALT, 0,                               // 115
    UINT32HALT, 0,                               // 116
    UINT32HALT, 0,                               // 117
    UINT32HALT, 0,                               // 118
    UINT32HALT, 0,                               // 119
    UINT32HALT, 0,                               // 120
    UINT32HALT, 0,                               // 121
    UINT32HALT, 0,                               // 122
    UINT32HALT, 0,                               // 123
    UINT32HALT, 0,                               // 124
    UINT32HALT, 0,                               // 125
    UINT32HALT, 0,                               // 126
    UINT32HALT, 0,                               // 127
};
#else
#error
#endif
#endif

