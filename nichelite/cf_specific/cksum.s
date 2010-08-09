/* FILENAME: cksum.s
 *
 * Copyright 2005 By InterNiche Technologies Inc. All rights reserved
 *
 * This version for ColdFire CPU, CodeWarrior IDE
 *
 * PORTABLE: NO
 */

      .text

/* FUNCTION: cksum
 *
 * Compute the checksum as per RFC 1071
 *
 * Param1: ptr;         pointer to buffer (16-bit aligned)
 * PARAM2: count;       length of buffer in 16-bit words
 *
 * RESULTS: cksum;      16-bit checksum of the words in the buffer
 */

      .global     cksum
      .global     _cksum

; d0 = ptr to buffer (word aligned)
; d1 = word count
; d2 = sum
; d3 = temp
; d3-d7,a1-a5 = data buffer


cksum:
_cksum:
      link     a6,#-28
      movem.l  d2-d7/a2,(sp)
      clr.l    d0             // checksum accumulator
      clr.l    d7
      move.l   8(a6),a0       // buffer pointer
      move.l   12(a6),d1      // word count
      ble      done
      move.l   a0,d2
      and.l    #2,d2
      beq      loop64a

/* process the first half-word */

      move.w   (a0)+,d0
      subq.l   #1,d1
      bgt      loop64a
      bra      done

/* process 64 bytes per loop interation */

loop64:
      movem.l  (a0),d2-d6/a2
      add.l    #24,a0
      add.l    a2,d0
      addx.l   d2,d0
      addx.l   d3,d0
      addx.l   d4,d0
      addx.l   d5,d0
      addx.l   d6,d0
      movem.l  (a0),d2-d6
      add.l    #20,a0
      addx.l   d2,d0
      addx.l   d3,d0
      addx.l   d4,d0
      addx.l   d5,d0
      addx.l   d6,d0
      movem.l  (a0),d2-d6
      add.l    #20,a0
      addx.l   d2,d0
      addx.l   d3,d0
      addx.l   d4,d0
      addx.l   d5,d0
      addx.l   d6,d0
      addx.l   d7,d0          // adds 0 + final carry
loop64a:
      sub.l   #32,d1
      bge      loop64

/* do blocks of 24 bytes */

      addi.l   #32-12,d1
      blt      partial
loop24:
      movem.l  (a0),d2-d6/a2
      add.l    #24,a0
      add.l    a2,d0
      addx.l   d2,d0
      addx.l   d3,d0
      addx.l   d4,d0
      addx.l   d5,d0
      addx.l   d6,d0
      addx.l   d7,d0
      sub.l    #12,d1
      bge      loop24

/* last partial block */

partial:
      add.l    #12-2,d1
      blt      last2
partial1:
      move.l   (a0)+,d3
      add.l    d3,d0 
      addx.l   d7,d0
      subq.l   #2,d1
      bge      partial1

/* remaining 2 bytes? */

last2:
      addq.l   #1,d1
      bne      fold
      clr.l    d2
      move.w   (a0)+,d2
      add.l    d2,d0
      addx.l   d7,d0
    
/* fold the checksum into a 16-bit value */

fold:
      move.l   d0,d2
      swap     d2
      add.l    d2,d0
      addx.l   d7,d0
      and.l    #$ffff,d0

done:
      movem.l  (sp),d2-d7/a2
      unlk     a6
      rts

