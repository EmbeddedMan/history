/*
 *
 * Copyright 2005 by InterNiche Technologies Inc. All rights reserved.
 *
 *  10/100 Fast Ethernet Controller Driver
 *
 * FILENAME: fec.h
 *
 *
 * PORTABLE: no
 */

#ifndef _FEC_H_
#define _FEC_H_ 1

/*********************************************************************
*
* Fast Ethernet Controller (FEC) Module
*
*********************************************************************/

struct fec_mib
{
    volatile u_long   rmon_t_drop;            /* Count of frames not counted correctly */
    volatile u_long   rmon_t_packets;         /* RMON Tx packet count */
    volatile u_long   rmon_t_bc_pkt;          /* RMON Tx Broadcast Packets */
    volatile u_long   rmon_t_mc_pkt;          /* RMON Tx Multicast Packets */
    volatile u_long   rmon_t_crc_align;       /* RMON Tx Packets w CRC/Align error */
    volatile u_long   rmon_t_undersize;       /* RMON Tx Packets < 64 bytes, good crc */
    volatile u_long   rmon_t_oversize;        /* RMON Tx Packets > MAX_FL bytes, good crc */
    volatile u_long   rmon_t_frag;            /* RMON Tx Packets < 64 bytes, bad crc */
    volatile u_long   rmon_t_jab;             /* RMON Tx Packets > MAX_FL bytes, bad crc */
    volatile u_long   rmon_t_col;             /* RMON Tx collision  count */
    volatile u_long   rmon_t_p64;             /* RMON Tx 64 byte packets */
    volatile u_long   rmon_t_p65to127;        /* RMON Tx 65 to 127 byte packets */
    volatile u_long   rmon_t_p128to255;       /* RMON Tx 128 to 255 byte packets */
    volatile u_long   rmon_t_p256to511;       /* RMON Tx 256 to 511 byte packets */
    volatile u_long   rmon_t_p512to1023;      /* RMON Tx 512 to 1023 byte packets */
    volatile u_long   rmon_t_p1024to2047;     /* RMON Tx 1024 to 2047 byte packets */
    volatile u_long   rmon_t_p_gte2048;       /* RMON Tx packets w >= 2048 bytes */
    volatile u_long   rmon_t_octets;          /* RMON Tx Octets */
    volatile u_long   ieee_t_drop;            /* Count of Frames not counted correctly */
    volatile u_long   ieee_t_frame_ok;        /* Frames Transmitted OK */  
    volatile u_long   ieee_t_1col;            /* Frames Transmitted with Single Collision */
    volatile u_long   ieee_t_mcol;            /* Frames Transmitted with Multiple Collision */
    volatile u_long   ieee_t_def;             /* Frames Transmitted after Deferral Delay */
    volatile u_long   ieee_t_lcol;            /* Frames Transmitted with Late Collision */
    volatile u_long   ieee_t_excol;           /* Frames Transmitted with Excessive Collisions */
    volatile u_long   ieee_t_macerr;          /* Frames Transmitted with Tx FIFO Underrun */
    volatile u_long   ieee_t_cseerr;          /* Frames Transmitted with Carrier Sense Error */
    volatile u_long   ieee_t_sqe;             /* Frames Transmitted with SQE Error */
    volatile u_long   ieee_t_fdxfc;           /* Flow Control Pause frames transmitted */
    volatile u_long   ieee_t_octets_ok;       /* Octet count for Frames Transmitted w/o Error */
    volatile u_long   RESERVED_0278;
    volatile u_long   RESERVED_027c;
    volatile u_long   RESERVED_0280;
    volatile u_long   rmon_r_packets;         /* RMON Rx packet count */
    volatile u_long   rmon_r_bc_pkt;          /* RMON Rx Broadcast Packets */
    volatile u_long   rmon_r_mc_pkt;          /* RMON Rx Multicast Packets */
    volatile u_long   rmon_r_crc_align;       /* RMON Rx Packets w CRC/Align error */
    volatile u_long   rmon_r_undersize;       /* RMON Rx Packets < 64 bytes, good crc */
    volatile u_long   rmon_r_oversize;        /* RMON Rx Packets > MAX_FL bytes, good crc */
    volatile u_long   rmon_r_frag;            /* RMON Rx Packets < 64 bytes, bad crc */
    volatile u_long   rmon_r_jab;             /* RMON Rx Packets > MAX_FL bytes, bad crc */
    volatile u_long   rmon_r_resvd_0;         /* Reserved */
    //
    volatile u_long   rmon_r_p64;             /* RMON Rx 64 byte packets */
    volatile u_long   rmon_r_p65to127;        /* RMON Rx 65 to 127 byte packets */
    volatile u_long   rmon_r_p128to255;       /* RMON Rx 128 to 255 byte packets */
    volatile u_long   rmon_r_p256to511;       /* RMON Rx 256 to 511 byte packets */
    volatile u_long   rmon_r_p512to1023;      /* RMON Rx 512 to 1023 byte packets */
    volatile u_long   rmon_r_p1024to2047;     /* RMON Rx 1024 to 2047 byte packets */
    volatile u_long   rmon_r_p_gte2048;       /* RMON Rx packets w >= 2048 bytes */
    volatile u_long   rmon_r_octets;          /* RMON Rx Octets */
    volatile u_long   ieee_r_drop;            /* Count of Frames not counted correctly */
    volatile u_long   ieee_r_frame_ok;        /* Frames Received OK */  
    volatile u_long   ieee_r_crc;             /* Frames Received with CRC Error */
    volatile u_long   ieee_r_align;           /* Frames Received with Alignment Error */
    volatile u_long   ieee_r_macerr;          /* Receive FIFO Overflow count */
    volatile u_long   ieee_r_fdxfc;           /* Flow Control Pause frames received */
    volatile u_long   ieee_r_octets_ok;       /* Octet count for Frames Rcvd w/o Error */
};

/* Status bits in buffer descriptors */
#define MCF_FEC_TxBD_R                  0x8000
#define MCF_FEC_TxBD_INUSE              0x4000
#define MCF_FEC_TxBD_TO1                0x4000
#define MCF_FEC_TxBD_W                  0x2000
#define MCF_FEC_TxBD_TO2                0x1000
#define MCF_FEC_TxBD_L                  0x0800
#define MCF_FEC_TxBD_TC                 0x0400
#define MCF_FEC_TxBD_DEF                0x0200
#define MCF_FEC_TxBD_HB                 0x0100
#define MCF_FEC_TxBD_LC                 0x0080
#define MCF_FEC_TxBD_RL                 0x0040
#define MCF_FEC_TxBD_UN                 0x0002
#define MCF_FEC_TxBD_CSL                0x0001

#define MCF_FEC_RxBD_E                  0x8000
#define MCF_FEC_RxBD_INUSE              0x4000
#define MCF_FEC_RxBD_R01                0x4000
#define MCF_FEC_RxBD_W                  0x2000
#define MCF_FEC_RxBD_R02                0x1000
#define MCF_FEC_RxBD_L                  0x0800
#define MCF_FEC_RxBD_M                  0x0100
#define MCF_FEC_RxBD_BC                 0x0080
#define MCF_FEC_RxBD_MC                 0x0040
#define MCF_FEC_RxBD_LG                 0x0020
#define MCF_FEC_RxBD_NO                 0x0010
#define MCF_FEC_RxBD_CR                 0x0004
#define MCF_FEC_RxBD_OV                 0x0002
#define MCF_FEC_RxBD_TR                 0x0001

#define MCF_FEC_EIR_ALL_EVENTS   \
   ( MCF_FEC_EIR_HBERR | MCF_FEC_EIR_BABR | MCF_FEC_EIR_BABT | \
     MCF_FEC_EIR_GRA   | MCF_FEC_EIR_TXF  | MCF_FEC_EIR_TXB  | \
     MCF_FEC_EIR_RXF   | MCF_FEC_EIR_RXB  | MCF_FEC_EIR_MII  | \
     MCF_FEC_EIR_EBERR | MCF_FEC_EIR_LC   | MCF_FEC_EIR_RL   | \
     MCF_FEC_EIR_UN )

#define MCF_FEC_EIMR_ALL_MASKS   \
   ( MCF_FEC_EIMR_HBERR | MCF_FEC_EIMR_BABR | MCF_FEC_EIMR_BABT | \
     MCF_FEC_EIMR_GRA   | MCF_FEC_EIMR_TXF  | 0                 | \
     MCF_FEC_EIMR_RXF   | 0                 | MCF_FEC_EIMR_MII  | \
     MCF_FEC_EIMR_EBERR | MCF_FEC_EIMR_LC   | MCF_FEC_EIMR_RL   | \
     MCF_FEC_EIMR_UN )

#define MCF_FEC_RxBD_ERRS   \
   ( MCF_FEC_RxBD_LG | MCF_FEC_RxBD_NO |  MCF_FEC_RxBD_CR | \
     MCF_FEC_RxBD_OV | MCF_FEC_RxBD_TR ) 

#endif /* _FEC_H_ */
