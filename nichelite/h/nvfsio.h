/*
 * FILENAME: nvfsio.h
 *
 *
 * Copyright 1997- 2006 By InterNiche Technologies Inc. All rights reserved
 *
 * Portions Copyright 1986 by Carnegie Mellon
 * Portions Copyright 1984 by the Massachusetts Institute of Technology
 *
 * Copyright (c) 1982, 1986, 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation and other 
 * materials related to such distribution and use acknowledge that 
 * the software was developed by the University of California, Berkeley.
 * The name of the University may not be used to endorse or promote 
 * products derived from this software without specific prior written 
 * permission. THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * Rights, responsibilities and use of this software are controlled by
 * the agreement found in the "LICENSE.H" file distributed with this
 * source code.  "LICENSE.H" may not be removed from this distribution,
 * modified, enhanced nor references to it omitted.
 *
 *
 * Definitions for generic file system NV parm IO.
 *
 * MODULE: TCP
 *
 *
 * PORTABLE: yes
 */

#ifndef _NVFSIO_H_
#define  _NVFSIO_H_  1

/* define these only if building with NVPARMS */
#ifdef INCLUDE_NVPARMS

struct nvparm_info;
struct nvparm_format;

int   get_file_parms(char * filename, struct nvparm_format *, int *);

#ifndef  NV_LINELENGTH
#define  NV_LINELENGTH     80
#endif

/* map non-volatile parameters file IO to VFS or local disk system,
 * as appropriate
 */

#ifdef HT_LOCALFS    /* write NV parameters file to disk */
#define  NV_FILE        FILE
#define  nv_fopen       fopen
#define  nv_fclose      fclose
#define  nv_fprintf     fprintf
#define  nv_fgets       fgets
#define  nv_fread       fread
#define  nv_fwrite      fwrite
#define  nv_fseek       fseek
#else /* no HT_LOCALFS, must use VFS */
#ifndef VFS_FILES
#error must have VFS_FILES or HT_LOCALFS
#endif   /* no VFSFILES */
#define  NV_FILE        VFILE
#define  nv_fopen       vfopen
#define  nv_fclose      vfclose
#define  nv_fread       vfread
#define  nv_fwrite      vfwrite
#define  nv_fseek       vfseek
#ifndef  _NVFSIO_C_  /* Don't include varargs protottype in code file */
int      nv_fprintf(void * fd, char * format, ...);
char *   nv_fgets(char * buffer, int maxlen, void * fd);
#endif   /* _NVFSIO_C_ */
#endif   /*  HT_LOCALFS */


/* optional per-port routine to write nvfs image to flash part */
extern   void (*nv_writeflash)(void);

#endif   /* INCLUDE_NVPARMS */


#ifdef   HT_SYNCDEV
/* The next two routines implement the mapping from vfs_sync to a 
 * generic flash device. The flash code must export these routines
 * for SYNCDEV to work.
 */
extern   int   ProgramFlash(u_long fd, char * buf, unsigned bytes_to_write);
extern   int   EraseFlash(int sector);
#endif /* HT_SYNCDEV */

#endif   /* _NVFSIO_H_ */


