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
CND_CONF=chipKIT_Max32
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
FINAL_IMAGE=dist/${CND_CONF}/pic32stickos.x.a
else
IMAGE_TYPE=production
FINAL_IMAGE=dist/${CND_CONF}/pic32stickos.x.a
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/555720668/block.o ${OBJECTDIR}/_ext/555720668/fat32.o ${OBJECTDIR}/_ext/724208933/basic.o ${OBJECTDIR}/_ext/724208933/code.o ${OBJECTDIR}/_ext/724208933/parse.o ${OBJECTDIR}/_ext/724208933/run.o ${OBJECTDIR}/_ext/724208933/vars.o


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
MP_CC=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.02\\bin\\pic32-gcc.exe
# MP_BC is not defined
MP_AS=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.02\\bin\\pic32-as.exe
MP_LD=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.02\\bin\\pic32-ld.exe
MP_AR=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.02\\bin\\pic32-ar.exe
# MP_BC is not defined
MP_CC_DIR=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.02\\bin
# MP_BC_DIR is not defined
MP_AS_DIR=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.02\\bin
MP_LD_DIR=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.02\\bin
MP_AR_DIR=C:\\Program\ Files\ \(x86\)\\Microchip\\mplabc32\\v2.02\\bin
# MP_BC_DIR is not defined

.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-chipKIT_Max32.mk dist/${CND_CONF}/pic32stickos.x.a

MP_PROCESSOR_OPTION=32MX795F512L
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
${OBJECTDIR}/_ext/724208933/vars.o: ../stickos/vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/vars.o.ok ${OBJECTDIR}/_ext/724208933/vars.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/vars.o.d -o ${OBJECTDIR}/_ext/724208933/vars.o ../stickos/vars.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/vars.o.d -o ${OBJECTDIR}/_ext/724208933/vars.o ../stickos/vars.c   2>&1  > ${OBJECTDIR}/_ext/724208933/vars.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/724208933/vars.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/724208933/vars.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/724208933/vars.o.d > ${OBJECTDIR}/_ext/724208933/vars.o.tmp
	@${RM} ${OBJECTDIR}/_ext/724208933/vars.o.d 
	@${CP} ${OBJECTDIR}/_ext/724208933/vars.o.tmp ${OBJECTDIR}/_ext/724208933/vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/vars.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/724208933/vars.o.err 
	@cat ${OBJECTDIR}/_ext/724208933/vars.o.err 
	@if [ -f ${OBJECTDIR}/_ext/724208933/vars.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/724208933/vars.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/724208933/run.o: ../stickos/run.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/run.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/run.o.ok ${OBJECTDIR}/_ext/724208933/run.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/run.o.d -o ${OBJECTDIR}/_ext/724208933/run.o ../stickos/run.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/run.o.d -o ${OBJECTDIR}/_ext/724208933/run.o ../stickos/run.c   2>&1  > ${OBJECTDIR}/_ext/724208933/run.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/724208933/run.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/724208933/run.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/724208933/run.o.d > ${OBJECTDIR}/_ext/724208933/run.o.tmp
	@${RM} ${OBJECTDIR}/_ext/724208933/run.o.d 
	@${CP} ${OBJECTDIR}/_ext/724208933/run.o.tmp ${OBJECTDIR}/_ext/724208933/run.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/run.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/724208933/run.o.err 
	@cat ${OBJECTDIR}/_ext/724208933/run.o.err 
	@if [ -f ${OBJECTDIR}/_ext/724208933/run.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/724208933/run.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/555720668/fat32.o: ../pict-o-crypt/fat32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/555720668 
	@${RM} ${OBJECTDIR}/_ext/555720668/fat32.o.d 
	@${RM} ${OBJECTDIR}/_ext/555720668/fat32.o.ok ${OBJECTDIR}/_ext/555720668/fat32.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/555720668/fat32.o.d -o ${OBJECTDIR}/_ext/555720668/fat32.o ../pict-o-crypt/fat32.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/555720668/fat32.o.d -o ${OBJECTDIR}/_ext/555720668/fat32.o ../pict-o-crypt/fat32.c   2>&1  > ${OBJECTDIR}/_ext/555720668/fat32.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/555720668/fat32.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/555720668/fat32.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/555720668/fat32.o.d > ${OBJECTDIR}/_ext/555720668/fat32.o.tmp
	@${RM} ${OBJECTDIR}/_ext/555720668/fat32.o.d 
	@${CP} ${OBJECTDIR}/_ext/555720668/fat32.o.tmp ${OBJECTDIR}/_ext/555720668/fat32.o.d 
	@${RM} ${OBJECTDIR}/_ext/555720668/fat32.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/555720668/fat32.o.err 
	@cat ${OBJECTDIR}/_ext/555720668/fat32.o.err 
	@if [ -f ${OBJECTDIR}/_ext/555720668/fat32.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/555720668/fat32.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/724208933/code.o: ../stickos/code.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/code.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/code.o.ok ${OBJECTDIR}/_ext/724208933/code.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/code.o.d -o ${OBJECTDIR}/_ext/724208933/code.o ../stickos/code.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/code.o.d -o ${OBJECTDIR}/_ext/724208933/code.o ../stickos/code.c   2>&1  > ${OBJECTDIR}/_ext/724208933/code.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/724208933/code.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/724208933/code.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/724208933/code.o.d > ${OBJECTDIR}/_ext/724208933/code.o.tmp
	@${RM} ${OBJECTDIR}/_ext/724208933/code.o.d 
	@${CP} ${OBJECTDIR}/_ext/724208933/code.o.tmp ${OBJECTDIR}/_ext/724208933/code.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/code.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/724208933/code.o.err 
	@cat ${OBJECTDIR}/_ext/724208933/code.o.err 
	@if [ -f ${OBJECTDIR}/_ext/724208933/code.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/724208933/code.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/555720668/block.o: ../pict-o-crypt/block.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/555720668 
	@${RM} ${OBJECTDIR}/_ext/555720668/block.o.d 
	@${RM} ${OBJECTDIR}/_ext/555720668/block.o.ok ${OBJECTDIR}/_ext/555720668/block.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/555720668/block.o.d -o ${OBJECTDIR}/_ext/555720668/block.o ../pict-o-crypt/block.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/555720668/block.o.d -o ${OBJECTDIR}/_ext/555720668/block.o ../pict-o-crypt/block.c   2>&1  > ${OBJECTDIR}/_ext/555720668/block.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/555720668/block.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/555720668/block.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/555720668/block.o.d > ${OBJECTDIR}/_ext/555720668/block.o.tmp
	@${RM} ${OBJECTDIR}/_ext/555720668/block.o.d 
	@${CP} ${OBJECTDIR}/_ext/555720668/block.o.tmp ${OBJECTDIR}/_ext/555720668/block.o.d 
	@${RM} ${OBJECTDIR}/_ext/555720668/block.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/555720668/block.o.err 
	@cat ${OBJECTDIR}/_ext/555720668/block.o.err 
	@if [ -f ${OBJECTDIR}/_ext/555720668/block.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/555720668/block.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/724208933/basic.o: ../stickos/basic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/basic.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/basic.o.ok ${OBJECTDIR}/_ext/724208933/basic.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/basic.o.d -o ${OBJECTDIR}/_ext/724208933/basic.o ../stickos/basic.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/basic.o.d -o ${OBJECTDIR}/_ext/724208933/basic.o ../stickos/basic.c   2>&1  > ${OBJECTDIR}/_ext/724208933/basic.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/724208933/basic.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/724208933/basic.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/724208933/basic.o.d > ${OBJECTDIR}/_ext/724208933/basic.o.tmp
	@${RM} ${OBJECTDIR}/_ext/724208933/basic.o.d 
	@${CP} ${OBJECTDIR}/_ext/724208933/basic.o.tmp ${OBJECTDIR}/_ext/724208933/basic.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/basic.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/724208933/basic.o.err 
	@cat ${OBJECTDIR}/_ext/724208933/basic.o.err 
	@if [ -f ${OBJECTDIR}/_ext/724208933/basic.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/724208933/basic.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/724208933/parse.o: ../stickos/parse.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/parse.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/parse.o.ok ${OBJECTDIR}/_ext/724208933/parse.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/parse.o.d -o ${OBJECTDIR}/_ext/724208933/parse.o ../stickos/parse.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/parse.o.d -o ${OBJECTDIR}/_ext/724208933/parse.o ../stickos/parse.c   2>&1  > ${OBJECTDIR}/_ext/724208933/parse.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/724208933/parse.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/724208933/parse.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/724208933/parse.o.d > ${OBJECTDIR}/_ext/724208933/parse.o.tmp
	@${RM} ${OBJECTDIR}/_ext/724208933/parse.o.d 
	@${CP} ${OBJECTDIR}/_ext/724208933/parse.o.tmp ${OBJECTDIR}/_ext/724208933/parse.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/parse.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/724208933/parse.o.err 
	@cat ${OBJECTDIR}/_ext/724208933/parse.o.err 
	@if [ -f ${OBJECTDIR}/_ext/724208933/parse.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/724208933/parse.o.ok; else exit 1; fi
	
else
${OBJECTDIR}/_ext/724208933/vars.o: ../stickos/vars.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/vars.o.ok ${OBJECTDIR}/_ext/724208933/vars.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/vars.o.d -o ${OBJECTDIR}/_ext/724208933/vars.o ../stickos/vars.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/vars.o.d -o ${OBJECTDIR}/_ext/724208933/vars.o ../stickos/vars.c   2>&1  > ${OBJECTDIR}/_ext/724208933/vars.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/724208933/vars.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/724208933/vars.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/724208933/vars.o.d > ${OBJECTDIR}/_ext/724208933/vars.o.tmp
	@${RM} ${OBJECTDIR}/_ext/724208933/vars.o.d 
	@${CP} ${OBJECTDIR}/_ext/724208933/vars.o.tmp ${OBJECTDIR}/_ext/724208933/vars.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/vars.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/724208933/vars.o.err 
	@cat ${OBJECTDIR}/_ext/724208933/vars.o.err 
	@if [ -f ${OBJECTDIR}/_ext/724208933/vars.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/724208933/vars.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/724208933/run.o: ../stickos/run.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/run.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/run.o.ok ${OBJECTDIR}/_ext/724208933/run.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/run.o.d -o ${OBJECTDIR}/_ext/724208933/run.o ../stickos/run.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/run.o.d -o ${OBJECTDIR}/_ext/724208933/run.o ../stickos/run.c   2>&1  > ${OBJECTDIR}/_ext/724208933/run.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/724208933/run.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/724208933/run.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/724208933/run.o.d > ${OBJECTDIR}/_ext/724208933/run.o.tmp
	@${RM} ${OBJECTDIR}/_ext/724208933/run.o.d 
	@${CP} ${OBJECTDIR}/_ext/724208933/run.o.tmp ${OBJECTDIR}/_ext/724208933/run.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/run.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/724208933/run.o.err 
	@cat ${OBJECTDIR}/_ext/724208933/run.o.err 
	@if [ -f ${OBJECTDIR}/_ext/724208933/run.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/724208933/run.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/555720668/fat32.o: ../pict-o-crypt/fat32.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/555720668 
	@${RM} ${OBJECTDIR}/_ext/555720668/fat32.o.d 
	@${RM} ${OBJECTDIR}/_ext/555720668/fat32.o.ok ${OBJECTDIR}/_ext/555720668/fat32.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/555720668/fat32.o.d -o ${OBJECTDIR}/_ext/555720668/fat32.o ../pict-o-crypt/fat32.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/555720668/fat32.o.d -o ${OBJECTDIR}/_ext/555720668/fat32.o ../pict-o-crypt/fat32.c   2>&1  > ${OBJECTDIR}/_ext/555720668/fat32.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/555720668/fat32.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/555720668/fat32.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/555720668/fat32.o.d > ${OBJECTDIR}/_ext/555720668/fat32.o.tmp
	@${RM} ${OBJECTDIR}/_ext/555720668/fat32.o.d 
	@${CP} ${OBJECTDIR}/_ext/555720668/fat32.o.tmp ${OBJECTDIR}/_ext/555720668/fat32.o.d 
	@${RM} ${OBJECTDIR}/_ext/555720668/fat32.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/555720668/fat32.o.err 
	@cat ${OBJECTDIR}/_ext/555720668/fat32.o.err 
	@if [ -f ${OBJECTDIR}/_ext/555720668/fat32.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/555720668/fat32.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/724208933/code.o: ../stickos/code.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/code.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/code.o.ok ${OBJECTDIR}/_ext/724208933/code.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/code.o.d -o ${OBJECTDIR}/_ext/724208933/code.o ../stickos/code.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/code.o.d -o ${OBJECTDIR}/_ext/724208933/code.o ../stickos/code.c   2>&1  > ${OBJECTDIR}/_ext/724208933/code.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/724208933/code.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/724208933/code.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/724208933/code.o.d > ${OBJECTDIR}/_ext/724208933/code.o.tmp
	@${RM} ${OBJECTDIR}/_ext/724208933/code.o.d 
	@${CP} ${OBJECTDIR}/_ext/724208933/code.o.tmp ${OBJECTDIR}/_ext/724208933/code.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/code.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/724208933/code.o.err 
	@cat ${OBJECTDIR}/_ext/724208933/code.o.err 
	@if [ -f ${OBJECTDIR}/_ext/724208933/code.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/724208933/code.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/555720668/block.o: ../pict-o-crypt/block.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/555720668 
	@${RM} ${OBJECTDIR}/_ext/555720668/block.o.d 
	@${RM} ${OBJECTDIR}/_ext/555720668/block.o.ok ${OBJECTDIR}/_ext/555720668/block.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/555720668/block.o.d -o ${OBJECTDIR}/_ext/555720668/block.o ../pict-o-crypt/block.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/555720668/block.o.d -o ${OBJECTDIR}/_ext/555720668/block.o ../pict-o-crypt/block.c   2>&1  > ${OBJECTDIR}/_ext/555720668/block.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/555720668/block.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/555720668/block.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/555720668/block.o.d > ${OBJECTDIR}/_ext/555720668/block.o.tmp
	@${RM} ${OBJECTDIR}/_ext/555720668/block.o.d 
	@${CP} ${OBJECTDIR}/_ext/555720668/block.o.tmp ${OBJECTDIR}/_ext/555720668/block.o.d 
	@${RM} ${OBJECTDIR}/_ext/555720668/block.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/555720668/block.o.err 
	@cat ${OBJECTDIR}/_ext/555720668/block.o.err 
	@if [ -f ${OBJECTDIR}/_ext/555720668/block.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/555720668/block.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/724208933/basic.o: ../stickos/basic.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/basic.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/basic.o.ok ${OBJECTDIR}/_ext/724208933/basic.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/basic.o.d -o ${OBJECTDIR}/_ext/724208933/basic.o ../stickos/basic.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/basic.o.d -o ${OBJECTDIR}/_ext/724208933/basic.o ../stickos/basic.c   2>&1  > ${OBJECTDIR}/_ext/724208933/basic.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/724208933/basic.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/724208933/basic.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/724208933/basic.o.d > ${OBJECTDIR}/_ext/724208933/basic.o.tmp
	@${RM} ${OBJECTDIR}/_ext/724208933/basic.o.d 
	@${CP} ${OBJECTDIR}/_ext/724208933/basic.o.tmp ${OBJECTDIR}/_ext/724208933/basic.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/basic.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/724208933/basic.o.err 
	@cat ${OBJECTDIR}/_ext/724208933/basic.o.err 
	@if [ -f ${OBJECTDIR}/_ext/724208933/basic.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/724208933/basic.o.ok; else exit 1; fi
	
${OBJECTDIR}/_ext/724208933/parse.o: ../stickos/parse.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/724208933 
	@${RM} ${OBJECTDIR}/_ext/724208933/parse.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/parse.o.ok ${OBJECTDIR}/_ext/724208933/parse.o.err 
	@echo ${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/parse.o.d -o ${OBJECTDIR}/_ext/724208933/parse.o ../stickos/parse.c  
	@-${MP_CC} $(MP_EXTRA_CC_PRE)  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -mno-float -DPIC32=1 -DSTICKOS=1 -DCHIPKIT=1 -I".." -I"../pict-o-crypt" -I"../cpustick" -I"../headers" -I"../sources" -I"../stickos" -Os -fomit-frame-pointer -fno-builtin -MMD -MF ${OBJECTDIR}/_ext/724208933/parse.o.d -o ${OBJECTDIR}/_ext/724208933/parse.o ../stickos/parse.c   2>&1  > ${OBJECTDIR}/_ext/724208933/parse.o.err ; if [ $$? -eq 0 ] ; then touch ${OBJECTDIR}/_ext/724208933/parse.o.ok ; fi 
	@touch ${OBJECTDIR}/_ext/724208933/parse.o.d 
	
ifneq (,$(findstring MINGW32,$(OS_CURRENT))) 
	@sed -e 's/\\$$/__EOL__/g' -e 's/\\ /__ESCAPED_SPACES__/g' -e 's/\\/\//g' -e 's/__ESCAPED_SPACES__/\\ /g' -e 's/__EOL__$$/\\/g' ${OBJECTDIR}/_ext/724208933/parse.o.d > ${OBJECTDIR}/_ext/724208933/parse.o.tmp
	@${RM} ${OBJECTDIR}/_ext/724208933/parse.o.d 
	@${CP} ${OBJECTDIR}/_ext/724208933/parse.o.tmp ${OBJECTDIR}/_ext/724208933/parse.o.d 
	@${RM} ${OBJECTDIR}/_ext/724208933/parse.o.tmp
endif
	@touch ${OBJECTDIR}/_ext/724208933/parse.o.err 
	@cat ${OBJECTDIR}/_ext/724208933/parse.o.err 
	@if [ -f ${OBJECTDIR}/_ext/724208933/parse.o.ok ] ; then rm -f ${OBJECTDIR}/_ext/724208933/parse.o.ok; else exit 1; fi
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: archive
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/pic32stickos.x.a: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF} 
	${MP_AR} $(MP_EXTRA_AR_PRE) r dist/${CND_CONF}/pic32stickos.x.a ${OBJECTFILES}     
else
dist/${CND_CONF}/pic32stickos.x.a: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF} 
	${MP_AR} $(MP_EXTRA_AR_PRE) r dist/${CND_CONF}/pic32stickos.x.a ${OBJECTFILES}     
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/chipKIT_Max32
	${RM} -r dist/chipKIT_Max32

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
