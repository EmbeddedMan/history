# makefile for Interniche allports directory. This builds a library
# containing various test applications and helper routines for the
# InterNiche TCP/IP stack.
#
#       This makefile should work with most development environments. 
# tool names, paths & flags should be set the included file "..\cflags.mak"
#
# Copyright 2002 by InterNiche Technologies Inc. All rights reserved.
#

#set tools, and environment
include ../cflags.mak

OBJS= \
  allports.$(OBJ) \
  timeouts.$(OBJ) \
  tk_misc.$(OBJ)
 


# local file dependancies
DFILES = $(DEPEND_ALL) makefile

# local file dependancies
INCLUDE=$(ICMD). $(ICMD)../mppp $(ICMD)../modem $(ICMD)../dhcpsrv

# build target 
LIBNAME=allports.lib

all: $(LIBNAME)

$(LIBNAME): $(OBJS) $(DFILES)
	$(LIB2) $(LIBCMD2)

#
# Build rules:
#
.c.$(OBJ):
	$(CC) $(CFLAGS) $(INCLUDE) $*.c
	$(LIB) $(LIBCMD)

#
# "make clean" support:
#
clean:
	$(RM) *.$(OBJ)
	$(RM) *.lib
	$(CLEANCMD)

#
# Dependancy lists:
#

allports.$(OBJ): allports.c $(DFILES)

timeouts.$(OBJ): timeouts.c $(DFILES)

tk_misc.$(OBJ): tk_misc.c $(DFILES)
