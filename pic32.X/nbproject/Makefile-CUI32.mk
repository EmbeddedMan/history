#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile

# Environment
MKDIR=mkdir -p
RM=rm -f 
CP=cp 

# Macros
CND_CONF=CUI32
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.elf
else
IMAGE_TYPE=production
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.elf
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/startup.o ${OBJECTDIR}/_ext/1472/vectors.o ${OBJECTDIR}/_ext/35980457/basic0.o ${OBJECTDIR}/_ext/35980457/cpustick.o ${OBJECTDIR}/_ext/35980457/parse2.o ${OBJECTDIR}/_ext/35980457/run2.o ${OBJECTDIR}/_ext/592584297/adc.o ${OBJECTDIR}/_ext/592584297/cdcacm.o ${OBJECTDIR}/_ext/592584297/clone.o ${OBJECTDIR}/_ext/592584297/flash.o ${OBJECTDIR}/_ext/592584297/i2c.o ${OBJECTDIR}/_ext/592584297/kbd.o ${OBJECTDIR}/_ext/592584297/lcd.o ${OBJECTDIR}/_ext/592584297/led.o ${OBJECTDIR}/_ext/592584297/pin.o ${OBJECTDIR}/_ext/592584297/printf.o ${OBJECTDIR}/_ext/592584297/qspi.o ${OBJECTDIR}/_ext/592584297/scsi.o ${OBJECTDIR}/_ext/592584297/serial.o ${OBJECTDIR}/_ext/592584297/sleep.o ${OBJECTDIR}/_ext/592584297/terminal.o ${OBJECTDIR}/_ext/592584297/text.o ${OBJECTDIR}/_ext/592584297/timer.o ${OBJECTDIR}/_ext/592584297/usb.o ${OBJECTDIR}/_ext/592584297/util.o ${OBJECTDIR}/_ext/592584297/zigflea.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH=C:\\Program\ Files\ \(x86\)\\Java\\jdk1.6.0_29\\jre/bin/
OS_CURRENT="$(shell uname -s)"
############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
MP_CC=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.00\\bin\\pic32-gcc.exe
# MP_BC is not defined
MP_AS=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.00\\bin\\pic32-as.exe
MP_LD=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.00\\bin\\pic32-ld.exe
MP_AR=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.00\\bin\\pic32-ar.exe
# MP_BC is not defined
MP_CC_DIR=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.00\\bin
# MP_BC_DIR is not defined
MP_AS_DIR=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.00\\bin
MP_LD_DIR=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.00\\bin
MP_AR_DIR=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.00\\bin
# MP_BC_DIR is not defined

.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-CUI32.mk dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.elf

MP_PROCESSOR_OPTION=32MX440F512H
MP_LINKER_FILE_OPTION=
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/592584297/util.o: ../sources/util.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/util.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/util.o.ok ${OBJECTDIR}/_ext/592584297/util.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/util.o.d -o ${OBJECTDIR}/_ext/592584297/util.o ../sources/util.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/util.o.d -o ${OBJECTDIR}/_ext/592584297/util.o ../sources/util.c   2>&1  > ${OBJECTDIR}/_ext/592584297/util.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/util.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/util.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/util.o.d > ${OBJECTDIR}/_ext/592584297/util.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/util.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/util.o.tmp ${OBJECTDIR}/_ext/592584297/util.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/util.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/util.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/util.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/util.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/util.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/timer.o: ../sources/timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/timer.o.ok ${OBJECTDIR}/_ext/592584297/timer.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/timer.o.d -o ${OBJECTDIR}/_ext/592584297/timer.o ../sources/timer.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/timer.o.d -o ${OBJECTDIR}/_ext/592584297/timer.o ../sources/timer.c   2>&1  > ${OBJECTDIR}/_ext/592584297/timer.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/timer.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/timer.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/timer.o.d > ${OBJECTDIR}/_ext/592584297/timer.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/timer.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/timer.o.tmp ${OBJECTDIR}/_ext/592584297/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/timer.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/timer.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/timer.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/timer.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/timer.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/terminal.o: ../sources/terminal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/terminal.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/terminal.o.ok ${OBJECTDIR}/_ext/592584297/terminal.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/terminal.o.d -o ${OBJECTDIR}/_ext/592584297/terminal.o ../sources/terminal.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/terminal.o.d -o ${OBJECTDIR}/_ext/592584297/terminal.o ../sources/terminal.c   2>&1  > ${OBJECTDIR}/_ext/592584297/terminal.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/terminal.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/terminal.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/terminal.o.d > ${OBJECTDIR}/_ext/592584297/terminal.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/terminal.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/terminal.o.tmp ${OBJECTDIR}/_ext/592584297/terminal.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/terminal.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/terminal.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/terminal.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/terminal.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/terminal.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/flash.o: ../sources/flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/flash.o.ok ${OBJECTDIR}/_ext/592584297/flash.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/flash.o.d -o ${OBJECTDIR}/_ext/592584297/flash.o ../sources/flash.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/flash.o.d -o ${OBJECTDIR}/_ext/592584297/flash.o ../sources/flash.c   2>&1  > ${OBJECTDIR}/_ext/592584297/flash.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/flash.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/flash.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/flash.o.d > ${OBJECTDIR}/_ext/592584297/flash.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/flash.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/flash.o.tmp ${OBJECTDIR}/_ext/592584297/flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/flash.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/flash.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/flash.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/flash.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/flash.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/text.o: ../sources/text.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/text.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/text.o.ok ${OBJECTDIR}/_ext/592584297/text.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/text.o.d -o ${OBJECTDIR}/_ext/592584297/text.o ../sources/text.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/text.o.d -o ${OBJECTDIR}/_ext/592584297/text.o ../sources/text.c   2>&1  > ${OBJECTDIR}/_ext/592584297/text.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/text.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/text.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/text.o.d > ${OBJECTDIR}/_ext/592584297/text.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/text.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/text.o.tmp ${OBJECTDIR}/_ext/592584297/text.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/text.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/text.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/text.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/text.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/text.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/led.o: ../sources/led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/led.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/led.o.ok ${OBJECTDIR}/_ext/592584297/led.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/led.o.d -o ${OBJECTDIR}/_ext/592584297/led.o ../sources/led.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/led.o.d -o ${OBJECTDIR}/_ext/592584297/led.o ../sources/led.c   2>&1  > ${OBJECTDIR}/_ext/592584297/led.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/led.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/led.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/led.o.d > ${OBJECTDIR}/_ext/592584297/led.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/led.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/led.o.tmp ${OBJECTDIR}/_ext/592584297/led.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/led.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/led.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/led.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/led.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/led.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/35980457/cpustick.o: ../cpustick/cpustick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/cpustick.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/cpustick.o.ok ${OBJECTDIR}/_ext/35980457/cpustick.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/cpustick.o.d -o ${OBJECTDIR}/_ext/35980457/cpustick.o ../cpustick/cpustick.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/cpustick.o.d -o ${OBJECTDIR}/_ext/35980457/cpustick.o ../cpustick/cpustick.c   2>&1  > ${OBJECTDIR}/_ext/35980457/cpustick.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/35980457/cpustick.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/35980457/cpustick.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/35980457/cpustick.o.d > ${OBJECTDIR}/_ext/35980457/cpustick.o.tmp
	@${RM} ${OBJECTDIR}/_ext/35980457/cpustick.o.d 
	@${CP} ${OBJECTDIR}/_ext/35980457/cpustick.o.tmp ${OBJECTDIR}/_ext/35980457/cpustick.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/cpustick.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/35980457/cpustick.o.err 
	@cat ${OBJECTDIR}/_ext/35980457/cpustick.o.err 
	@if [ -f ${OBJECTDIR}/_ext/35980457/cpustick.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/35980457/cpustick.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/i2c.o: ../sources/i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/i2c.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/i2c.o.ok ${OBJECTDIR}/_ext/592584297/i2c.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/i2c.o.d -o ${OBJECTDIR}/_ext/592584297/i2c.o ../sources/i2c.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/i2c.o.d -o ${OBJECTDIR}/_ext/592584297/i2c.o ../sources/i2c.c   2>&1  > ${OBJECTDIR}/_ext/592584297/i2c.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/i2c.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/i2c.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/i2c.o.d > ${OBJECTDIR}/_ext/592584297/i2c.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/i2c.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/i2c.o.tmp ${OBJECTDIR}/_ext/592584297/i2c.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/i2c.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/i2c.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/i2c.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/i2c.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/i2c.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/adc.o: ../sources/adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/adc.o.ok ${OBJECTDIR}/_ext/592584297/adc.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/adc.o.d -o ${OBJECTDIR}/_ext/592584297/adc.o ../sources/adc.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/adc.o.d -o ${OBJECTDIR}/_ext/592584297/adc.o ../sources/adc.c   2>&1  > ${OBJECTDIR}/_ext/592584297/adc.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/adc.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/adc.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/adc.o.d > ${OBJECTDIR}/_ext/592584297/adc.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/adc.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/adc.o.tmp ${OBJECTDIR}/_ext/592584297/adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/adc.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/adc.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/adc.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/adc.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/adc.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/1472/vectors.o: ../vectors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/vectors.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/vectors.o.ok ${OBJECTDIR}/_ext/1472/vectors.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/vectors.o.d -o ${OBJECTDIR}/_ext/1472/vectors.o ../vectors.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/vectors.o.d -o ${OBJECTDIR}/_ext/1472/vectors.o ../vectors.c   2>&1  > ${OBJECTDIR}/_ext/1472/vectors.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1472/vectors.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1472/vectors.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/vectors.o.d > ${OBJECTDIR}/_ext/1472/vectors.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1472/vectors.o.d 
	@${CP} ${OBJECTDIR}/_ext/1472/vectors.o.tmp ${OBJECTDIR}/_ext/1472/vectors.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/vectors.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/1472/vectors.o.err 
	@cat ${OBJECTDIR}/_ext/1472/vectors.o.err 
	@if [ -f ${OBJECTDIR}/_ext/1472/vectors.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1472/vectors.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/qspi.o: ../sources/qspi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/qspi.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/qspi.o.ok ${OBJECTDIR}/_ext/592584297/qspi.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/qspi.o.d -o ${OBJECTDIR}/_ext/592584297/qspi.o ../sources/qspi.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/qspi.o.d -o ${OBJECTDIR}/_ext/592584297/qspi.o ../sources/qspi.c   2>&1  > ${OBJECTDIR}/_ext/592584297/qspi.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/qspi.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/qspi.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/qspi.o.d > ${OBJECTDIR}/_ext/592584297/qspi.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/qspi.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/qspi.o.tmp ${OBJECTDIR}/_ext/592584297/qspi.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/qspi.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/qspi.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/qspi.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/qspi.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/qspi.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/zigflea.o: ../sources/zigflea.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/zigflea.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/zigflea.o.ok ${OBJECTDIR}/_ext/592584297/zigflea.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/zigflea.o.d -o ${OBJECTDIR}/_ext/592584297/zigflea.o ../sources/zigflea.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/zigflea.o.d -o ${OBJECTDIR}/_ext/592584297/zigflea.o ../sources/zigflea.c   2>&1  > ${OBJECTDIR}/_ext/592584297/zigflea.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/zigflea.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/zigflea.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/zigflea.o.d > ${OBJECTDIR}/_ext/592584297/zigflea.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/zigflea.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/zigflea.o.tmp ${OBJECTDIR}/_ext/592584297/zigflea.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/zigflea.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/zigflea.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/zigflea.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/zigflea.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/zigflea.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/sleep.o: ../sources/sleep.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/sleep.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/sleep.o.ok ${OBJECTDIR}/_ext/592584297/sleep.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/sleep.o.d -o ${OBJECTDIR}/_ext/592584297/sleep.o ../sources/sleep.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/sleep.o.d -o ${OBJECTDIR}/_ext/592584297/sleep.o ../sources/sleep.c   2>&1  > ${OBJECTDIR}/_ext/592584297/sleep.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/sleep.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/sleep.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/sleep.o.d > ${OBJECTDIR}/_ext/592584297/sleep.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/sleep.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/sleep.o.tmp ${OBJECTDIR}/_ext/592584297/sleep.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/sleep.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/sleep.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/sleep.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/sleep.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/sleep.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/35980457/basic0.o: ../cpustick/basic0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/basic0.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/basic0.o.ok ${OBJECTDIR}/_ext/35980457/basic0.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/basic0.o.d -o ${OBJECTDIR}/_ext/35980457/basic0.o ../cpustick/basic0.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/basic0.o.d -o ${OBJECTDIR}/_ext/35980457/basic0.o ../cpustick/basic0.c   2>&1  > ${OBJECTDIR}/_ext/35980457/basic0.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/35980457/basic0.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/35980457/basic0.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/35980457/basic0.o.d > ${OBJECTDIR}/_ext/35980457/basic0.o.tmp
	@${RM} ${OBJECTDIR}/_ext/35980457/basic0.o.d 
	@${CP} ${OBJECTDIR}/_ext/35980457/basic0.o.tmp ${OBJECTDIR}/_ext/35980457/basic0.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/basic0.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/35980457/basic0.o.err 
	@cat ${OBJECTDIR}/_ext/35980457/basic0.o.err 
	@if [ -f ${OBJECTDIR}/_ext/35980457/basic0.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/35980457/basic0.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/printf.o: ../sources/printf.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/printf.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/printf.o.ok ${OBJECTDIR}/_ext/592584297/printf.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/printf.o.d -o ${OBJECTDIR}/_ext/592584297/printf.o ../sources/printf.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/printf.o.d -o ${OBJECTDIR}/_ext/592584297/printf.o ../sources/printf.c   2>&1  > ${OBJECTDIR}/_ext/592584297/printf.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/printf.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/printf.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/printf.o.d > ${OBJECTDIR}/_ext/592584297/printf.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/printf.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/printf.o.tmp ${OBJECTDIR}/_ext/592584297/printf.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/printf.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/printf.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/printf.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/printf.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/printf.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/lcd.o: ../sources/lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/lcd.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/lcd.o.ok ${OBJECTDIR}/_ext/592584297/lcd.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/lcd.o.d -o ${OBJECTDIR}/_ext/592584297/lcd.o ../sources/lcd.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/lcd.o.d -o ${OBJECTDIR}/_ext/592584297/lcd.o ../sources/lcd.c   2>&1  > ${OBJECTDIR}/_ext/592584297/lcd.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/lcd.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/lcd.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/lcd.o.d > ${OBJECTDIR}/_ext/592584297/lcd.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/lcd.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/lcd.o.tmp ${OBJECTDIR}/_ext/592584297/lcd.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/lcd.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/lcd.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/lcd.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/lcd.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/lcd.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/serial.o: ../sources/serial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/serial.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/serial.o.ok ${OBJECTDIR}/_ext/592584297/serial.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/serial.o.d -o ${OBJECTDIR}/_ext/592584297/serial.o ../sources/serial.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/serial.o.d -o ${OBJECTDIR}/_ext/592584297/serial.o ../sources/serial.c   2>&1  > ${OBJECTDIR}/_ext/592584297/serial.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/serial.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/serial.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/serial.o.d > ${OBJECTDIR}/_ext/592584297/serial.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/serial.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/serial.o.tmp ${OBJECTDIR}/_ext/592584297/serial.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/serial.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/serial.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/serial.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/serial.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/serial.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/1472/startup.o: ../startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/startup.o.ok ${OBJECTDIR}/_ext/1472/startup.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/startup.o.d -o ${OBJECTDIR}/_ext/1472/startup.o ../startup.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/startup.o.d -o ${OBJECTDIR}/_ext/1472/startup.o ../startup.c   2>&1  > ${OBJECTDIR}/_ext/1472/startup.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1472/startup.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1472/startup.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/startup.o.d > ${OBJECTDIR}/_ext/1472/startup.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1472/startup.o.d 
	@${CP} ${OBJECTDIR}/_ext/1472/startup.o.tmp ${OBJECTDIR}/_ext/1472/startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/startup.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/1472/startup.o.err 
	@cat ${OBJECTDIR}/_ext/1472/startup.o.err 
	@if [ -f ${OBJECTDIR}/_ext/1472/startup.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1472/startup.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/35980457/parse2.o: ../cpustick/parse2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/parse2.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/parse2.o.ok ${OBJECTDIR}/_ext/35980457/parse2.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/parse2.o.d -o ${OBJECTDIR}/_ext/35980457/parse2.o ../cpustick/parse2.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/parse2.o.d -o ${OBJECTDIR}/_ext/35980457/parse2.o ../cpustick/parse2.c   2>&1  > ${OBJECTDIR}/_ext/35980457/parse2.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/35980457/parse2.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/35980457/parse2.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/35980457/parse2.o.d > ${OBJECTDIR}/_ext/35980457/parse2.o.tmp
	@${RM} ${OBJECTDIR}/_ext/35980457/parse2.o.d 
	@${CP} ${OBJECTDIR}/_ext/35980457/parse2.o.tmp ${OBJECTDIR}/_ext/35980457/parse2.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/parse2.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/35980457/parse2.o.err 
	@cat ${OBJECTDIR}/_ext/35980457/parse2.o.err 
	@if [ -f ${OBJECTDIR}/_ext/35980457/parse2.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/35980457/parse2.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/35980457/run2.o: ../cpustick/run2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/run2.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/run2.o.ok ${OBJECTDIR}/_ext/35980457/run2.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/run2.o.d -o ${OBJECTDIR}/_ext/35980457/run2.o ../cpustick/run2.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/run2.o.d -o ${OBJECTDIR}/_ext/35980457/run2.o ../cpustick/run2.c   2>&1  > ${OBJECTDIR}/_ext/35980457/run2.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/35980457/run2.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/35980457/run2.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/35980457/run2.o.d > ${OBJECTDIR}/_ext/35980457/run2.o.tmp
	@${RM} ${OBJECTDIR}/_ext/35980457/run2.o.d 
	@${CP} ${OBJECTDIR}/_ext/35980457/run2.o.tmp ${OBJECTDIR}/_ext/35980457/run2.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/run2.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/35980457/run2.o.err 
	@cat ${OBJECTDIR}/_ext/35980457/run2.o.err 
	@if [ -f ${OBJECTDIR}/_ext/35980457/run2.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/35980457/run2.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/main.o.d -o ${OBJECTDIR}/_ext/1472/main.o ../main.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/main.o.d -o ${OBJECTDIR}/_ext/1472/main.o ../main.c   2>&1  > ${OBJECTDIR}/_ext/1472/main.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1472/main.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1472/main.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/main.o.d > ${OBJECTDIR}/_ext/1472/main.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${CP} ${OBJECTDIR}/_ext/1472/main.o.tmp ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/1472/main.o.err 
	@cat ${OBJECTDIR}/_ext/1472/main.o.err 
	@if [ -f ${OBJECTDIR}/_ext/1472/main.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1472/main.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/usb.o: ../sources/usb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/usb.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/usb.o.ok ${OBJECTDIR}/_ext/592584297/usb.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/usb.o.d -o ${OBJECTDIR}/_ext/592584297/usb.o ../sources/usb.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/usb.o.d -o ${OBJECTDIR}/_ext/592584297/usb.o ../sources/usb.c   2>&1  > ${OBJECTDIR}/_ext/592584297/usb.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/usb.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/usb.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/usb.o.d > ${OBJECTDIR}/_ext/592584297/usb.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/usb.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/usb.o.tmp ${OBJECTDIR}/_ext/592584297/usb.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/usb.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/usb.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/usb.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/usb.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/usb.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/scsi.o: ../sources/scsi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/scsi.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/scsi.o.ok ${OBJECTDIR}/_ext/592584297/scsi.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/scsi.o.d -o ${OBJECTDIR}/_ext/592584297/scsi.o ../sources/scsi.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/scsi.o.d -o ${OBJECTDIR}/_ext/592584297/scsi.o ../sources/scsi.c   2>&1  > ${OBJECTDIR}/_ext/592584297/scsi.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/scsi.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/scsi.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/scsi.o.d > ${OBJECTDIR}/_ext/592584297/scsi.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/scsi.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/scsi.o.tmp ${OBJECTDIR}/_ext/592584297/scsi.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/scsi.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/scsi.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/scsi.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/scsi.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/scsi.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/cdcacm.o: ../sources/cdcacm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/cdcacm.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/cdcacm.o.ok ${OBJECTDIR}/_ext/592584297/cdcacm.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/cdcacm.o.d -o ${OBJECTDIR}/_ext/592584297/cdcacm.o ../sources/cdcacm.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/cdcacm.o.d -o ${OBJECTDIR}/_ext/592584297/cdcacm.o ../sources/cdcacm.c   2>&1  > ${OBJECTDIR}/_ext/592584297/cdcacm.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/cdcacm.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/cdcacm.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/cdcacm.o.d > ${OBJECTDIR}/_ext/592584297/cdcacm.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/cdcacm.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/cdcacm.o.tmp ${OBJECTDIR}/_ext/592584297/cdcacm.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/cdcacm.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/cdcacm.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/cdcacm.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/cdcacm.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/cdcacm.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/pin.o: ../sources/pin.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/pin.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/pin.o.ok ${OBJECTDIR}/_ext/592584297/pin.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/pin.o.d -o ${OBJECTDIR}/_ext/592584297/pin.o ../sources/pin.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/pin.o.d -o ${OBJECTDIR}/_ext/592584297/pin.o ../sources/pin.c   2>&1  > ${OBJECTDIR}/_ext/592584297/pin.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/pin.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/pin.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/pin.o.d > ${OBJECTDIR}/_ext/592584297/pin.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/pin.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/pin.o.tmp ${OBJECTDIR}/_ext/592584297/pin.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/pin.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/pin.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/pin.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/pin.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/pin.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/kbd.o: ../sources/kbd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/kbd.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/kbd.o.ok ${OBJECTDIR}/_ext/592584297/kbd.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/kbd.o.d -o ${OBJECTDIR}/_ext/592584297/kbd.o ../sources/kbd.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/kbd.o.d -o ${OBJECTDIR}/_ext/592584297/kbd.o ../sources/kbd.c   2>&1  > ${OBJECTDIR}/_ext/592584297/kbd.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/kbd.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/kbd.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/kbd.o.d > ${OBJECTDIR}/_ext/592584297/kbd.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/kbd.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/kbd.o.tmp ${OBJECTDIR}/_ext/592584297/kbd.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/kbd.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/kbd.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/kbd.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/kbd.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/kbd.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/clone.o: ../sources/clone.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/clone.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/clone.o.ok ${OBJECTDIR}/_ext/592584297/clone.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/clone.o.d -o ${OBJECTDIR}/_ext/592584297/clone.o ../sources/clone.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/clone.o.d -o ${OBJECTDIR}/_ext/592584297/clone.o ../sources/clone.c   2>&1  > ${OBJECTDIR}/_ext/592584297/clone.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/clone.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/clone.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/clone.o.d > ${OBJECTDIR}/_ext/592584297/clone.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/clone.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/clone.o.tmp ${OBJECTDIR}/_ext/592584297/clone.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/clone.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/clone.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/clone.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/clone.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/clone.o.ok; else exit 1; fi
	
else
${OBJECTDIR}/_ext/592584297/util.o: ../sources/util.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/util.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/util.o.ok ${OBJECTDIR}/_ext/592584297/util.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/util.o.d -o ${OBJECTDIR}/_ext/592584297/util.o ../sources/util.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/util.o.d -o ${OBJECTDIR}/_ext/592584297/util.o ../sources/util.c   2>&1  > ${OBJECTDIR}/_ext/592584297/util.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/util.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/util.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/util.o.d > ${OBJECTDIR}/_ext/592584297/util.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/util.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/util.o.tmp ${OBJECTDIR}/_ext/592584297/util.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/util.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/util.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/util.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/util.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/util.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/timer.o: ../sources/timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/timer.o.ok ${OBJECTDIR}/_ext/592584297/timer.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/timer.o.d -o ${OBJECTDIR}/_ext/592584297/timer.o ../sources/timer.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/timer.o.d -o ${OBJECTDIR}/_ext/592584297/timer.o ../sources/timer.c   2>&1  > ${OBJECTDIR}/_ext/592584297/timer.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/timer.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/timer.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/timer.o.d > ${OBJECTDIR}/_ext/592584297/timer.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/timer.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/timer.o.tmp ${OBJECTDIR}/_ext/592584297/timer.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/timer.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/timer.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/timer.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/timer.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/timer.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/terminal.o: ../sources/terminal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/terminal.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/terminal.o.ok ${OBJECTDIR}/_ext/592584297/terminal.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/terminal.o.d -o ${OBJECTDIR}/_ext/592584297/terminal.o ../sources/terminal.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/terminal.o.d -o ${OBJECTDIR}/_ext/592584297/terminal.o ../sources/terminal.c   2>&1  > ${OBJECTDIR}/_ext/592584297/terminal.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/terminal.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/terminal.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/terminal.o.d > ${OBJECTDIR}/_ext/592584297/terminal.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/terminal.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/terminal.o.tmp ${OBJECTDIR}/_ext/592584297/terminal.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/terminal.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/terminal.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/terminal.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/terminal.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/terminal.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/flash.o: ../sources/flash.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/flash.o.ok ${OBJECTDIR}/_ext/592584297/flash.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/flash.o.d -o ${OBJECTDIR}/_ext/592584297/flash.o ../sources/flash.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/flash.o.d -o ${OBJECTDIR}/_ext/592584297/flash.o ../sources/flash.c   2>&1  > ${OBJECTDIR}/_ext/592584297/flash.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/flash.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/flash.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/flash.o.d > ${OBJECTDIR}/_ext/592584297/flash.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/flash.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/flash.o.tmp ${OBJECTDIR}/_ext/592584297/flash.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/flash.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/flash.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/flash.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/flash.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/flash.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/text.o: ../sources/text.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/text.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/text.o.ok ${OBJECTDIR}/_ext/592584297/text.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/text.o.d -o ${OBJECTDIR}/_ext/592584297/text.o ../sources/text.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/text.o.d -o ${OBJECTDIR}/_ext/592584297/text.o ../sources/text.c   2>&1  > ${OBJECTDIR}/_ext/592584297/text.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/text.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/text.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/text.o.d > ${OBJECTDIR}/_ext/592584297/text.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/text.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/text.o.tmp ${OBJECTDIR}/_ext/592584297/text.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/text.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/text.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/text.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/text.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/text.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/led.o: ../sources/led.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/led.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/led.o.ok ${OBJECTDIR}/_ext/592584297/led.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/led.o.d -o ${OBJECTDIR}/_ext/592584297/led.o ../sources/led.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/led.o.d -o ${OBJECTDIR}/_ext/592584297/led.o ../sources/led.c   2>&1  > ${OBJECTDIR}/_ext/592584297/led.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/led.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/led.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/led.o.d > ${OBJECTDIR}/_ext/592584297/led.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/led.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/led.o.tmp ${OBJECTDIR}/_ext/592584297/led.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/led.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/led.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/led.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/led.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/led.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/35980457/cpustick.o: ../cpustick/cpustick.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/cpustick.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/cpustick.o.ok ${OBJECTDIR}/_ext/35980457/cpustick.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/cpustick.o.d -o ${OBJECTDIR}/_ext/35980457/cpustick.o ../cpustick/cpustick.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/cpustick.o.d -o ${OBJECTDIR}/_ext/35980457/cpustick.o ../cpustick/cpustick.c   2>&1  > ${OBJECTDIR}/_ext/35980457/cpustick.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/35980457/cpustick.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/35980457/cpustick.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/35980457/cpustick.o.d > ${OBJECTDIR}/_ext/35980457/cpustick.o.tmp
	@${RM} ${OBJECTDIR}/_ext/35980457/cpustick.o.d 
	@${CP} ${OBJECTDIR}/_ext/35980457/cpustick.o.tmp ${OBJECTDIR}/_ext/35980457/cpustick.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/cpustick.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/35980457/cpustick.o.err 
	@cat ${OBJECTDIR}/_ext/35980457/cpustick.o.err 
	@if [ -f ${OBJECTDIR}/_ext/35980457/cpustick.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/35980457/cpustick.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/i2c.o: ../sources/i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/i2c.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/i2c.o.ok ${OBJECTDIR}/_ext/592584297/i2c.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/i2c.o.d -o ${OBJECTDIR}/_ext/592584297/i2c.o ../sources/i2c.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/i2c.o.d -o ${OBJECTDIR}/_ext/592584297/i2c.o ../sources/i2c.c   2>&1  > ${OBJECTDIR}/_ext/592584297/i2c.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/i2c.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/i2c.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/i2c.o.d > ${OBJECTDIR}/_ext/592584297/i2c.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/i2c.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/i2c.o.tmp ${OBJECTDIR}/_ext/592584297/i2c.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/i2c.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/i2c.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/i2c.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/i2c.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/i2c.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/adc.o: ../sources/adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/adc.o.ok ${OBJECTDIR}/_ext/592584297/adc.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/adc.o.d -o ${OBJECTDIR}/_ext/592584297/adc.o ../sources/adc.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/adc.o.d -o ${OBJECTDIR}/_ext/592584297/adc.o ../sources/adc.c   2>&1  > ${OBJECTDIR}/_ext/592584297/adc.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/adc.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/adc.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/adc.o.d > ${OBJECTDIR}/_ext/592584297/adc.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/adc.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/adc.o.tmp ${OBJECTDIR}/_ext/592584297/adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/adc.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/adc.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/adc.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/adc.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/adc.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/1472/vectors.o: ../vectors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/vectors.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/vectors.o.ok ${OBJECTDIR}/_ext/1472/vectors.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/vectors.o.d -o ${OBJECTDIR}/_ext/1472/vectors.o ../vectors.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/vectors.o.d -o ${OBJECTDIR}/_ext/1472/vectors.o ../vectors.c   2>&1  > ${OBJECTDIR}/_ext/1472/vectors.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1472/vectors.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1472/vectors.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/vectors.o.d > ${OBJECTDIR}/_ext/1472/vectors.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1472/vectors.o.d 
	@${CP} ${OBJECTDIR}/_ext/1472/vectors.o.tmp ${OBJECTDIR}/_ext/1472/vectors.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/vectors.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/1472/vectors.o.err 
	@cat ${OBJECTDIR}/_ext/1472/vectors.o.err 
	@if [ -f ${OBJECTDIR}/_ext/1472/vectors.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1472/vectors.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/qspi.o: ../sources/qspi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/qspi.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/qspi.o.ok ${OBJECTDIR}/_ext/592584297/qspi.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/qspi.o.d -o ${OBJECTDIR}/_ext/592584297/qspi.o ../sources/qspi.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/qspi.o.d -o ${OBJECTDIR}/_ext/592584297/qspi.o ../sources/qspi.c   2>&1  > ${OBJECTDIR}/_ext/592584297/qspi.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/qspi.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/qspi.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/qspi.o.d > ${OBJECTDIR}/_ext/592584297/qspi.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/qspi.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/qspi.o.tmp ${OBJECTDIR}/_ext/592584297/qspi.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/qspi.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/qspi.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/qspi.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/qspi.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/qspi.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/zigflea.o: ../sources/zigflea.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/zigflea.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/zigflea.o.ok ${OBJECTDIR}/_ext/592584297/zigflea.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/zigflea.o.d -o ${OBJECTDIR}/_ext/592584297/zigflea.o ../sources/zigflea.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/zigflea.o.d -o ${OBJECTDIR}/_ext/592584297/zigflea.o ../sources/zigflea.c   2>&1  > ${OBJECTDIR}/_ext/592584297/zigflea.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/zigflea.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/zigflea.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/zigflea.o.d > ${OBJECTDIR}/_ext/592584297/zigflea.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/zigflea.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/zigflea.o.tmp ${OBJECTDIR}/_ext/592584297/zigflea.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/zigflea.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/zigflea.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/zigflea.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/zigflea.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/zigflea.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/sleep.o: ../sources/sleep.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/sleep.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/sleep.o.ok ${OBJECTDIR}/_ext/592584297/sleep.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/sleep.o.d -o ${OBJECTDIR}/_ext/592584297/sleep.o ../sources/sleep.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/sleep.o.d -o ${OBJECTDIR}/_ext/592584297/sleep.o ../sources/sleep.c   2>&1  > ${OBJECTDIR}/_ext/592584297/sleep.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/sleep.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/sleep.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/sleep.o.d > ${OBJECTDIR}/_ext/592584297/sleep.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/sleep.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/sleep.o.tmp ${OBJECTDIR}/_ext/592584297/sleep.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/sleep.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/sleep.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/sleep.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/sleep.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/sleep.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/35980457/basic0.o: ../cpustick/basic0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/basic0.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/basic0.o.ok ${OBJECTDIR}/_ext/35980457/basic0.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/basic0.o.d -o ${OBJECTDIR}/_ext/35980457/basic0.o ../cpustick/basic0.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/basic0.o.d -o ${OBJECTDIR}/_ext/35980457/basic0.o ../cpustick/basic0.c   2>&1  > ${OBJECTDIR}/_ext/35980457/basic0.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/35980457/basic0.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/35980457/basic0.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/35980457/basic0.o.d > ${OBJECTDIR}/_ext/35980457/basic0.o.tmp
	@${RM} ${OBJECTDIR}/_ext/35980457/basic0.o.d 
	@${CP} ${OBJECTDIR}/_ext/35980457/basic0.o.tmp ${OBJECTDIR}/_ext/35980457/basic0.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/basic0.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/35980457/basic0.o.err 
	@cat ${OBJECTDIR}/_ext/35980457/basic0.o.err 
	@if [ -f ${OBJECTDIR}/_ext/35980457/basic0.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/35980457/basic0.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/printf.o: ../sources/printf.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/printf.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/printf.o.ok ${OBJECTDIR}/_ext/592584297/printf.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/printf.o.d -o ${OBJECTDIR}/_ext/592584297/printf.o ../sources/printf.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/printf.o.d -o ${OBJECTDIR}/_ext/592584297/printf.o ../sources/printf.c   2>&1  > ${OBJECTDIR}/_ext/592584297/printf.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/printf.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/printf.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/printf.o.d > ${OBJECTDIR}/_ext/592584297/printf.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/printf.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/printf.o.tmp ${OBJECTDIR}/_ext/592584297/printf.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/printf.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/printf.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/printf.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/printf.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/printf.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/lcd.o: ../sources/lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/lcd.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/lcd.o.ok ${OBJECTDIR}/_ext/592584297/lcd.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/lcd.o.d -o ${OBJECTDIR}/_ext/592584297/lcd.o ../sources/lcd.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/lcd.o.d -o ${OBJECTDIR}/_ext/592584297/lcd.o ../sources/lcd.c   2>&1  > ${OBJECTDIR}/_ext/592584297/lcd.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/lcd.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/lcd.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/lcd.o.d > ${OBJECTDIR}/_ext/592584297/lcd.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/lcd.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/lcd.o.tmp ${OBJECTDIR}/_ext/592584297/lcd.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/lcd.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/lcd.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/lcd.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/lcd.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/lcd.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/serial.o: ../sources/serial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/serial.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/serial.o.ok ${OBJECTDIR}/_ext/592584297/serial.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/serial.o.d -o ${OBJECTDIR}/_ext/592584297/serial.o ../sources/serial.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/serial.o.d -o ${OBJECTDIR}/_ext/592584297/serial.o ../sources/serial.c   2>&1  > ${OBJECTDIR}/_ext/592584297/serial.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/serial.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/serial.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/serial.o.d > ${OBJECTDIR}/_ext/592584297/serial.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/serial.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/serial.o.tmp ${OBJECTDIR}/_ext/592584297/serial.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/serial.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/serial.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/serial.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/serial.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/serial.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/1472/startup.o: ../startup.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/startup.o.ok ${OBJECTDIR}/_ext/1472/startup.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/startup.o.d -o ${OBJECTDIR}/_ext/1472/startup.o ../startup.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/startup.o.d -o ${OBJECTDIR}/_ext/1472/startup.o ../startup.c   2>&1  > ${OBJECTDIR}/_ext/1472/startup.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1472/startup.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1472/startup.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/startup.o.d > ${OBJECTDIR}/_ext/1472/startup.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1472/startup.o.d 
	@${CP} ${OBJECTDIR}/_ext/1472/startup.o.tmp ${OBJECTDIR}/_ext/1472/startup.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/startup.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/1472/startup.o.err 
	@cat ${OBJECTDIR}/_ext/1472/startup.o.err 
	@if [ -f ${OBJECTDIR}/_ext/1472/startup.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1472/startup.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/35980457/parse2.o: ../cpustick/parse2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/parse2.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/parse2.o.ok ${OBJECTDIR}/_ext/35980457/parse2.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/parse2.o.d -o ${OBJECTDIR}/_ext/35980457/parse2.o ../cpustick/parse2.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/parse2.o.d -o ${OBJECTDIR}/_ext/35980457/parse2.o ../cpustick/parse2.c   2>&1  > ${OBJECTDIR}/_ext/35980457/parse2.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/35980457/parse2.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/35980457/parse2.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/35980457/parse2.o.d > ${OBJECTDIR}/_ext/35980457/parse2.o.tmp
	@${RM} ${OBJECTDIR}/_ext/35980457/parse2.o.d 
	@${CP} ${OBJECTDIR}/_ext/35980457/parse2.o.tmp ${OBJECTDIR}/_ext/35980457/parse2.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/parse2.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/35980457/parse2.o.err 
	@cat ${OBJECTDIR}/_ext/35980457/parse2.o.err 
	@if [ -f ${OBJECTDIR}/_ext/35980457/parse2.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/35980457/parse2.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/35980457/run2.o: ../cpustick/run2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/35980457 
	@${RM} ${OBJECTDIR}/_ext/35980457/run2.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/run2.o.ok ${OBJECTDIR}/_ext/35980457/run2.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/run2.o.d -o ${OBJECTDIR}/_ext/35980457/run2.o ../cpustick/run2.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/35980457/run2.o.d -o ${OBJECTDIR}/_ext/35980457/run2.o ../cpustick/run2.c   2>&1  > ${OBJECTDIR}/_ext/35980457/run2.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/35980457/run2.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/35980457/run2.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/35980457/run2.o.d > ${OBJECTDIR}/_ext/35980457/run2.o.tmp
	@${RM} ${OBJECTDIR}/_ext/35980457/run2.o.d 
	@${CP} ${OBJECTDIR}/_ext/35980457/run2.o.tmp ${OBJECTDIR}/_ext/35980457/run2.o.d 
	@${RM} ${OBJECTDIR}/_ext/35980457/run2.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/35980457/run2.o.err 
	@cat ${OBJECTDIR}/_ext/35980457/run2.o.err 
	@if [ -f ${OBJECTDIR}/_ext/35980457/run2.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/35980457/run2.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.ok ${OBJECTDIR}/_ext/1472/main.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/main.o.d -o ${OBJECTDIR}/_ext/1472/main.o ../main.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/1472/main.o.d -o ${OBJECTDIR}/_ext/1472/main.o ../main.c   2>&1  > ${OBJECTDIR}/_ext/1472/main.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/1472/main.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/1472/main.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/1472/main.o.d > ${OBJECTDIR}/_ext/1472/main.o.tmp
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${CP} ${OBJECTDIR}/_ext/1472/main.o.tmp ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/1472/main.o.err 
	@cat ${OBJECTDIR}/_ext/1472/main.o.err 
	@if [ -f ${OBJECTDIR}/_ext/1472/main.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/1472/main.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/usb.o: ../sources/usb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/usb.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/usb.o.ok ${OBJECTDIR}/_ext/592584297/usb.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/usb.o.d -o ${OBJECTDIR}/_ext/592584297/usb.o ../sources/usb.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/usb.o.d -o ${OBJECTDIR}/_ext/592584297/usb.o ../sources/usb.c   2>&1  > ${OBJECTDIR}/_ext/592584297/usb.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/usb.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/usb.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/usb.o.d > ${OBJECTDIR}/_ext/592584297/usb.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/usb.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/usb.o.tmp ${OBJECTDIR}/_ext/592584297/usb.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/usb.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/usb.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/usb.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/usb.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/usb.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/scsi.o: ../sources/scsi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/scsi.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/scsi.o.ok ${OBJECTDIR}/_ext/592584297/scsi.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/scsi.o.d -o ${OBJECTDIR}/_ext/592584297/scsi.o ../sources/scsi.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/scsi.o.d -o ${OBJECTDIR}/_ext/592584297/scsi.o ../sources/scsi.c   2>&1  > ${OBJECTDIR}/_ext/592584297/scsi.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/scsi.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/scsi.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/scsi.o.d > ${OBJECTDIR}/_ext/592584297/scsi.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/scsi.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/scsi.o.tmp ${OBJECTDIR}/_ext/592584297/scsi.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/scsi.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/scsi.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/scsi.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/scsi.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/scsi.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/cdcacm.o: ../sources/cdcacm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/cdcacm.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/cdcacm.o.ok ${OBJECTDIR}/_ext/592584297/cdcacm.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/cdcacm.o.d -o ${OBJECTDIR}/_ext/592584297/cdcacm.o ../sources/cdcacm.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/cdcacm.o.d -o ${OBJECTDIR}/_ext/592584297/cdcacm.o ../sources/cdcacm.c   2>&1  > ${OBJECTDIR}/_ext/592584297/cdcacm.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/cdcacm.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/cdcacm.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/cdcacm.o.d > ${OBJECTDIR}/_ext/592584297/cdcacm.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/cdcacm.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/cdcacm.o.tmp ${OBJECTDIR}/_ext/592584297/cdcacm.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/cdcacm.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/cdcacm.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/cdcacm.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/cdcacm.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/cdcacm.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/pin.o: ../sources/pin.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/pin.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/pin.o.ok ${OBJECTDIR}/_ext/592584297/pin.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/pin.o.d -o ${OBJECTDIR}/_ext/592584297/pin.o ../sources/pin.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/pin.o.d -o ${OBJECTDIR}/_ext/592584297/pin.o ../sources/pin.c   2>&1  > ${OBJECTDIR}/_ext/592584297/pin.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/pin.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/pin.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/pin.o.d > ${OBJECTDIR}/_ext/592584297/pin.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/pin.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/pin.o.tmp ${OBJECTDIR}/_ext/592584297/pin.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/pin.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/pin.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/pin.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/pin.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/pin.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/kbd.o: ../sources/kbd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/kbd.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/kbd.o.ok ${OBJECTDIR}/_ext/592584297/kbd.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/kbd.o.d -o ${OBJECTDIR}/_ext/592584297/kbd.o ../sources/kbd.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/kbd.o.d -o ${OBJECTDIR}/_ext/592584297/kbd.o ../sources/kbd.c   2>&1  > ${OBJECTDIR}/_ext/592584297/kbd.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/kbd.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/kbd.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/kbd.o.d > ${OBJECTDIR}/_ext/592584297/kbd.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/kbd.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/kbd.o.tmp ${OBJECTDIR}/_ext/592584297/kbd.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/kbd.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/kbd.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/kbd.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/kbd.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/kbd.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/592584297/clone.o: ../sources/clone.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/592584297 
	@${RM} ${OBJECTDIR}/_ext/592584297/clone.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/clone.o.ok ${OBJECTDIR}/_ext/592584297/clone.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/clone.o.d -o ${OBJECTDIR}/_ext/592584297/clone.o ../sources/clone.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DHIDBL=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/592584297/clone.o.d -o ${OBJECTDIR}/_ext/592584297/clone.o ../sources/clone.c   2>&1  > ${OBJECTDIR}/_ext/592584297/clone.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/592584297/clone.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/592584297/clone.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/592584297/clone.o.d > ${OBJECTDIR}/_ext/592584297/clone.o.tmp
	@${RM} ${OBJECTDIR}/_ext/592584297/clone.o.d 
	@${CP} ${OBJECTDIR}/_ext/592584297/clone.o.tmp ${OBJECTDIR}/_ext/592584297/clone.o.d 
	@${RM} ${OBJECTDIR}/_ext/592584297/clone.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/592584297/clone.o.err 
	@cat ${OBJECTDIR}/_ext/592584297/clone.o.err 
	@if [ -f ${OBJECTDIR}/_ext/592584297/clone.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/592584297/clone.o.ok; else exit 1; fi
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.elf: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.elf ${OBJECTFILES}        -Wl,--defsym=__MPLAB_BUILD=1,--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__ICD2RAM=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,--defsym=_min_heap_size=0,--defsym=_min_stack_size=3072,-L"../../../../Program Files/Microchip/MPLAB C32/lib",-L"../../../../Program Files/Microchip/MPLAB C32/pic32mx/lib",-Map="$(BINDIR_)$(TARGETBASE).map" ../pic32stickos.X/dist/default/pic32stickos.x.a  -Wl,--script=../elf32pic32mx.ld 
else
dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.elf: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.elf ${OBJECTFILES}        -Wl,--defsym=__MPLAB_BUILD=1,--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=0,--defsym=_min_stack_size=3072,-L"../../../../Program Files/Microchip/MPLAB C32/lib",-L"../../../../Program Files/Microchip/MPLAB C32/pic32mx/lib",-Map="$(BINDIR_)$(TARGETBASE).map" ../pic32stickos.X/dist/default/pic32stickos.x.a  -Wl,--script=../elf32pic32mx.ld
	${MP_CC_DIR}\\pic32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/pic32.X.${IMAGE_TYPE}.elf  
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/CUI32
	${RM} -r dist/CUI32

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
