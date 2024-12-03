#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/GITTEST_PEDC_MON_PIC32_V1_1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/GITTEST_PEDC_MON_PIC32_V1_1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../file/BASModule/BASSys_App.c ../file/BASModule/BASSys_Reg.c ../file/BASModule/BASSys_Uart.c ../file/canbus/drv/ApiCanCom.c ../file/canbus/drv/ApiCanMst.c ../file/canbus/drv/ApiCanSlv.c ../file/canbus/drv/ApiMain.c ../file/canbus/drv/CanDrv.c ../file/canbus/drv/UpApiMst.c ../file/canbus/drv/UpApiSlv.c ../file/canbus/drv/UpBios.c ../file/canbus/drv/Update.c ../file/canbus/canbus.c ../file/canbus/ObjDict.c ../file/CANModule/can.c ../file/CANModule/dcu.c ../file/common/i2cDrv.c ../file/common/Ini_Cpu.c ../file/common/sysTimer.c ../file/common/uartDrv.c ../file/common/usrCRC.c ../file/common/uartDMA.c ../file/common/uartRxBuff.c ../file/IOCfg/IOConfig.c ../file/IOHandle/IO_HANDLE.c ../file/IOHandle/mainIO.c ../file/IPSModule/IPSUart.c ../file/IPSModule/IPS_App.c ../file/IPSModule/manLogicPcb.c ../file/MMSModule/MMS_App.c ../file/MMSModule/MMSUart.c ../file/MMSModule/saveIbpX.c ../file/PIC24Module/PIC24_App.c ../file/PIC24Update/P24UpdateDrv.c ../file/PIC24Update/P24UpdateIF.c ../file/SigModule/SigSys_App.c ../file/SigModule/SigSys_Reg.c ../file/SigModule/SigSys_Uart.c ../file/TestCode/test.c ../file/Main.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/2028936924/BASSys_App.o ${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o ${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o ${OBJECTDIR}/_ext/26038253/ApiCanCom.o ${OBJECTDIR}/_ext/26038253/ApiCanMst.o ${OBJECTDIR}/_ext/26038253/ApiCanSlv.o ${OBJECTDIR}/_ext/26038253/ApiMain.o ${OBJECTDIR}/_ext/26038253/CanDrv.o ${OBJECTDIR}/_ext/26038253/UpApiMst.o ${OBJECTDIR}/_ext/26038253/UpApiSlv.o ${OBJECTDIR}/_ext/26038253/UpBios.o ${OBJECTDIR}/_ext/26038253/Update.o ${OBJECTDIR}/_ext/1303770836/canbus.o ${OBJECTDIR}/_ext/1303770836/ObjDict.o ${OBJECTDIR}/_ext/78931352/can.o ${OBJECTDIR}/_ext/78931352/dcu.o ${OBJECTDIR}/_ext/1316680719/i2cDrv.o ${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o ${OBJECTDIR}/_ext/1316680719/sysTimer.o ${OBJECTDIR}/_ext/1316680719/uartDrv.o ${OBJECTDIR}/_ext/1316680719/usrCRC.o ${OBJECTDIR}/_ext/1316680719/uartDMA.o ${OBJECTDIR}/_ext/1316680719/uartRxBuff.o ${OBJECTDIR}/_ext/1090910534/IOConfig.o ${OBJECTDIR}/_ext/840646674/IO_HANDLE.o ${OBJECTDIR}/_ext/840646674/mainIO.o ${OBJECTDIR}/_ext/1660958476/IPSUart.o ${OBJECTDIR}/_ext/1660958476/IPS_App.o ${OBJECTDIR}/_ext/1660958476/manLogicPcb.o ${OBJECTDIR}/_ext/1234305445/MMS_App.o ${OBJECTDIR}/_ext/1234305445/MMSUart.o ${OBJECTDIR}/_ext/1234305445/saveIbpX.o ${OBJECTDIR}/_ext/784496404/PIC24_App.o ${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o ${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o ${OBJECTDIR}/_ext/1364844071/SigSys_App.o ${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o ${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o ${OBJECTDIR}/_ext/2129265379/test.o ${OBJECTDIR}/_ext/761014165/Main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/2028936924/BASSys_App.o.d ${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o.d ${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o.d ${OBJECTDIR}/_ext/26038253/ApiCanCom.o.d ${OBJECTDIR}/_ext/26038253/ApiCanMst.o.d ${OBJECTDIR}/_ext/26038253/ApiCanSlv.o.d ${OBJECTDIR}/_ext/26038253/ApiMain.o.d ${OBJECTDIR}/_ext/26038253/CanDrv.o.d ${OBJECTDIR}/_ext/26038253/UpApiMst.o.d ${OBJECTDIR}/_ext/26038253/UpApiSlv.o.d ${OBJECTDIR}/_ext/26038253/UpBios.o.d ${OBJECTDIR}/_ext/26038253/Update.o.d ${OBJECTDIR}/_ext/1303770836/canbus.o.d ${OBJECTDIR}/_ext/1303770836/ObjDict.o.d ${OBJECTDIR}/_ext/78931352/can.o.d ${OBJECTDIR}/_ext/78931352/dcu.o.d ${OBJECTDIR}/_ext/1316680719/i2cDrv.o.d ${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o.d ${OBJECTDIR}/_ext/1316680719/sysTimer.o.d ${OBJECTDIR}/_ext/1316680719/uartDrv.o.d ${OBJECTDIR}/_ext/1316680719/usrCRC.o.d ${OBJECTDIR}/_ext/1316680719/uartDMA.o.d ${OBJECTDIR}/_ext/1316680719/uartRxBuff.o.d ${OBJECTDIR}/_ext/1090910534/IOConfig.o.d ${OBJECTDIR}/_ext/840646674/IO_HANDLE.o.d ${OBJECTDIR}/_ext/840646674/mainIO.o.d ${OBJECTDIR}/_ext/1660958476/IPSUart.o.d ${OBJECTDIR}/_ext/1660958476/IPS_App.o.d ${OBJECTDIR}/_ext/1660958476/manLogicPcb.o.d ${OBJECTDIR}/_ext/1234305445/MMS_App.o.d ${OBJECTDIR}/_ext/1234305445/MMSUart.o.d ${OBJECTDIR}/_ext/1234305445/saveIbpX.o.d ${OBJECTDIR}/_ext/784496404/PIC24_App.o.d ${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o.d ${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o.d ${OBJECTDIR}/_ext/1364844071/SigSys_App.o.d ${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o.d ${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o.d ${OBJECTDIR}/_ext/2129265379/test.o.d ${OBJECTDIR}/_ext/761014165/Main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/2028936924/BASSys_App.o ${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o ${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o ${OBJECTDIR}/_ext/26038253/ApiCanCom.o ${OBJECTDIR}/_ext/26038253/ApiCanMst.o ${OBJECTDIR}/_ext/26038253/ApiCanSlv.o ${OBJECTDIR}/_ext/26038253/ApiMain.o ${OBJECTDIR}/_ext/26038253/CanDrv.o ${OBJECTDIR}/_ext/26038253/UpApiMst.o ${OBJECTDIR}/_ext/26038253/UpApiSlv.o ${OBJECTDIR}/_ext/26038253/UpBios.o ${OBJECTDIR}/_ext/26038253/Update.o ${OBJECTDIR}/_ext/1303770836/canbus.o ${OBJECTDIR}/_ext/1303770836/ObjDict.o ${OBJECTDIR}/_ext/78931352/can.o ${OBJECTDIR}/_ext/78931352/dcu.o ${OBJECTDIR}/_ext/1316680719/i2cDrv.o ${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o ${OBJECTDIR}/_ext/1316680719/sysTimer.o ${OBJECTDIR}/_ext/1316680719/uartDrv.o ${OBJECTDIR}/_ext/1316680719/usrCRC.o ${OBJECTDIR}/_ext/1316680719/uartDMA.o ${OBJECTDIR}/_ext/1316680719/uartRxBuff.o ${OBJECTDIR}/_ext/1090910534/IOConfig.o ${OBJECTDIR}/_ext/840646674/IO_HANDLE.o ${OBJECTDIR}/_ext/840646674/mainIO.o ${OBJECTDIR}/_ext/1660958476/IPSUart.o ${OBJECTDIR}/_ext/1660958476/IPS_App.o ${OBJECTDIR}/_ext/1660958476/manLogicPcb.o ${OBJECTDIR}/_ext/1234305445/MMS_App.o ${OBJECTDIR}/_ext/1234305445/MMSUart.o ${OBJECTDIR}/_ext/1234305445/saveIbpX.o ${OBJECTDIR}/_ext/784496404/PIC24_App.o ${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o ${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o ${OBJECTDIR}/_ext/1364844071/SigSys_App.o ${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o ${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o ${OBJECTDIR}/_ext/2129265379/test.o ${OBJECTDIR}/_ext/761014165/Main.o

# Source Files
SOURCEFILES=../file/BASModule/BASSys_App.c ../file/BASModule/BASSys_Reg.c ../file/BASModule/BASSys_Uart.c ../file/canbus/drv/ApiCanCom.c ../file/canbus/drv/ApiCanMst.c ../file/canbus/drv/ApiCanSlv.c ../file/canbus/drv/ApiMain.c ../file/canbus/drv/CanDrv.c ../file/canbus/drv/UpApiMst.c ../file/canbus/drv/UpApiSlv.c ../file/canbus/drv/UpBios.c ../file/canbus/drv/Update.c ../file/canbus/canbus.c ../file/canbus/ObjDict.c ../file/CANModule/can.c ../file/CANModule/dcu.c ../file/common/i2cDrv.c ../file/common/Ini_Cpu.c ../file/common/sysTimer.c ../file/common/uartDrv.c ../file/common/usrCRC.c ../file/common/uartDMA.c ../file/common/uartRxBuff.c ../file/IOCfg/IOConfig.c ../file/IOHandle/IO_HANDLE.c ../file/IOHandle/mainIO.c ../file/IPSModule/IPSUart.c ../file/IPSModule/IPS_App.c ../file/IPSModule/manLogicPcb.c ../file/MMSModule/MMS_App.c ../file/MMSModule/MMSUart.c ../file/MMSModule/saveIbpX.c ../file/PIC24Module/PIC24_App.c ../file/PIC24Update/P24UpdateDrv.c ../file/PIC24Update/P24UpdateIF.c ../file/SigModule/SigSys_App.c ../file/SigModule/SigSys_Reg.c ../file/SigModule/SigSys_Uart.c ../file/TestCode/test.c ../file/Main.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/GITTEST_PEDC_MON_PIC32_V1_1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MX795F512H
MP_LINKER_FILE_OPTION=,--script="..\linker\linkerELF_512.ld"
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
${OBJECTDIR}/_ext/2028936924/BASSys_App.o: ../file/BASModule/BASSys_App.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2028936924" 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_App.o.d 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_App.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2028936924/BASSys_App.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/2028936924/BASSys_App.o.d" -o ${OBJECTDIR}/_ext/2028936924/BASSys_App.o ../file/BASModule/BASSys_App.c  
	
${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o: ../file/BASModule/BASSys_Reg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2028936924" 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o.d 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o.d" -o ${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o ../file/BASModule/BASSys_Reg.c  
	
${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o: ../file/BASModule/BASSys_Uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2028936924" 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o.d" -o ${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o ../file/BASModule/BASSys_Uart.c  
	
${OBJECTDIR}/_ext/26038253/ApiCanCom.o: ../file/canbus/drv/ApiCanCom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanCom.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanCom.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/ApiCanCom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/ApiCanCom.o.d" -o ${OBJECTDIR}/_ext/26038253/ApiCanCom.o ../file/canbus/drv/ApiCanCom.c  
	
${OBJECTDIR}/_ext/26038253/ApiCanMst.o: ../file/canbus/drv/ApiCanMst.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanMst.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanMst.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/ApiCanMst.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/ApiCanMst.o.d" -o ${OBJECTDIR}/_ext/26038253/ApiCanMst.o ../file/canbus/drv/ApiCanMst.c  
	
${OBJECTDIR}/_ext/26038253/ApiCanSlv.o: ../file/canbus/drv/ApiCanSlv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanSlv.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanSlv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/ApiCanSlv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/ApiCanSlv.o.d" -o ${OBJECTDIR}/_ext/26038253/ApiCanSlv.o ../file/canbus/drv/ApiCanSlv.c  
	
${OBJECTDIR}/_ext/26038253/ApiMain.o: ../file/canbus/drv/ApiMain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiMain.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiMain.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/ApiMain.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/ApiMain.o.d" -o ${OBJECTDIR}/_ext/26038253/ApiMain.o ../file/canbus/drv/ApiMain.c  
	
${OBJECTDIR}/_ext/26038253/CanDrv.o: ../file/canbus/drv/CanDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/CanDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/CanDrv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/CanDrv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/CanDrv.o.d" -o ${OBJECTDIR}/_ext/26038253/CanDrv.o ../file/canbus/drv/CanDrv.c  
	
${OBJECTDIR}/_ext/26038253/UpApiMst.o: ../file/canbus/drv/UpApiMst.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpApiMst.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpApiMst.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/UpApiMst.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/UpApiMst.o.d" -o ${OBJECTDIR}/_ext/26038253/UpApiMst.o ../file/canbus/drv/UpApiMst.c  
	
${OBJECTDIR}/_ext/26038253/UpApiSlv.o: ../file/canbus/drv/UpApiSlv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpApiSlv.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpApiSlv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/UpApiSlv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/UpApiSlv.o.d" -o ${OBJECTDIR}/_ext/26038253/UpApiSlv.o ../file/canbus/drv/UpApiSlv.c  
	
${OBJECTDIR}/_ext/26038253/UpBios.o: ../file/canbus/drv/UpBios.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpBios.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpBios.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/UpBios.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/UpBios.o.d" -o ${OBJECTDIR}/_ext/26038253/UpBios.o ../file/canbus/drv/UpBios.c  
	
${OBJECTDIR}/_ext/26038253/Update.o: ../file/canbus/drv/Update.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/Update.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/Update.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/Update.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/Update.o.d" -o ${OBJECTDIR}/_ext/26038253/Update.o ../file/canbus/drv/Update.c  
	
${OBJECTDIR}/_ext/1303770836/canbus.o: ../file/canbus/canbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1303770836" 
	@${RM} ${OBJECTDIR}/_ext/1303770836/canbus.o.d 
	@${RM} ${OBJECTDIR}/_ext/1303770836/canbus.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1303770836/canbus.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1303770836/canbus.o.d" -o ${OBJECTDIR}/_ext/1303770836/canbus.o ../file/canbus/canbus.c  
	
${OBJECTDIR}/_ext/1303770836/ObjDict.o: ../file/canbus/ObjDict.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1303770836" 
	@${RM} ${OBJECTDIR}/_ext/1303770836/ObjDict.o.d 
	@${RM} ${OBJECTDIR}/_ext/1303770836/ObjDict.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1303770836/ObjDict.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1303770836/ObjDict.o.d" -o ${OBJECTDIR}/_ext/1303770836/ObjDict.o ../file/canbus/ObjDict.c  
	
${OBJECTDIR}/_ext/78931352/can.o: ../file/CANModule/can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/78931352" 
	@${RM} ${OBJECTDIR}/_ext/78931352/can.o.d 
	@${RM} ${OBJECTDIR}/_ext/78931352/can.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/78931352/can.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/78931352/can.o.d" -o ${OBJECTDIR}/_ext/78931352/can.o ../file/CANModule/can.c  
	
${OBJECTDIR}/_ext/78931352/dcu.o: ../file/CANModule/dcu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/78931352" 
	@${RM} ${OBJECTDIR}/_ext/78931352/dcu.o.d 
	@${RM} ${OBJECTDIR}/_ext/78931352/dcu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/78931352/dcu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/78931352/dcu.o.d" -o ${OBJECTDIR}/_ext/78931352/dcu.o ../file/CANModule/dcu.c  
	
${OBJECTDIR}/_ext/1316680719/i2cDrv.o: ../file/common/i2cDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/i2cDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/i2cDrv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/i2cDrv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/i2cDrv.o.d" -o ${OBJECTDIR}/_ext/1316680719/i2cDrv.o ../file/common/i2cDrv.c  
	
${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o: ../file/common/Ini_Cpu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o.d" -o ${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o ../file/common/Ini_Cpu.c  
	
${OBJECTDIR}/_ext/1316680719/sysTimer.o: ../file/common/sysTimer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/sysTimer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/sysTimer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/sysTimer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/sysTimer.o.d" -o ${OBJECTDIR}/_ext/1316680719/sysTimer.o ../file/common/sysTimer.c  
	
${OBJECTDIR}/_ext/1316680719/uartDrv.o: ../file/common/uartDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartDrv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/uartDrv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/uartDrv.o.d" -o ${OBJECTDIR}/_ext/1316680719/uartDrv.o ../file/common/uartDrv.c  
	
${OBJECTDIR}/_ext/1316680719/usrCRC.o: ../file/common/usrCRC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/usrCRC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/usrCRC.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/usrCRC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/usrCRC.o.d" -o ${OBJECTDIR}/_ext/1316680719/usrCRC.o ../file/common/usrCRC.c  
	
${OBJECTDIR}/_ext/1316680719/uartDMA.o: ../file/common/uartDMA.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartDMA.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartDMA.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/uartDMA.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/uartDMA.o.d" -o ${OBJECTDIR}/_ext/1316680719/uartDMA.o ../file/common/uartDMA.c  
	
${OBJECTDIR}/_ext/1316680719/uartRxBuff.o: ../file/common/uartRxBuff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartRxBuff.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartRxBuff.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/uartRxBuff.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/uartRxBuff.o.d" -o ${OBJECTDIR}/_ext/1316680719/uartRxBuff.o ../file/common/uartRxBuff.c  
	
${OBJECTDIR}/_ext/1090910534/IOConfig.o: ../file/IOCfg/IOConfig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1090910534" 
	@${RM} ${OBJECTDIR}/_ext/1090910534/IOConfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/1090910534/IOConfig.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1090910534/IOConfig.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1090910534/IOConfig.o.d" -o ${OBJECTDIR}/_ext/1090910534/IOConfig.o ../file/IOCfg/IOConfig.c  
	
${OBJECTDIR}/_ext/840646674/IO_HANDLE.o: ../file/IOHandle/IO_HANDLE.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/840646674" 
	@${RM} ${OBJECTDIR}/_ext/840646674/IO_HANDLE.o.d 
	@${RM} ${OBJECTDIR}/_ext/840646674/IO_HANDLE.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/840646674/IO_HANDLE.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/840646674/IO_HANDLE.o.d" -o ${OBJECTDIR}/_ext/840646674/IO_HANDLE.o ../file/IOHandle/IO_HANDLE.c  
	
${OBJECTDIR}/_ext/840646674/mainIO.o: ../file/IOHandle/mainIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/840646674" 
	@${RM} ${OBJECTDIR}/_ext/840646674/mainIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/840646674/mainIO.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/840646674/mainIO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/840646674/mainIO.o.d" -o ${OBJECTDIR}/_ext/840646674/mainIO.o ../file/IOHandle/mainIO.c  
	
${OBJECTDIR}/_ext/1660958476/IPSUart.o: ../file/IPSModule/IPSUart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1660958476" 
	@${RM} ${OBJECTDIR}/_ext/1660958476/IPSUart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1660958476/IPSUart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1660958476/IPSUart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1660958476/IPSUart.o.d" -o ${OBJECTDIR}/_ext/1660958476/IPSUart.o ../file/IPSModule/IPSUart.c  
	
${OBJECTDIR}/_ext/1660958476/IPS_App.o: ../file/IPSModule/IPS_App.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1660958476" 
	@${RM} ${OBJECTDIR}/_ext/1660958476/IPS_App.o.d 
	@${RM} ${OBJECTDIR}/_ext/1660958476/IPS_App.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1660958476/IPS_App.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1660958476/IPS_App.o.d" -o ${OBJECTDIR}/_ext/1660958476/IPS_App.o ../file/IPSModule/IPS_App.c  
	
${OBJECTDIR}/_ext/1660958476/manLogicPcb.o: ../file/IPSModule/manLogicPcb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1660958476" 
	@${RM} ${OBJECTDIR}/_ext/1660958476/manLogicPcb.o.d 
	@${RM} ${OBJECTDIR}/_ext/1660958476/manLogicPcb.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1660958476/manLogicPcb.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1660958476/manLogicPcb.o.d" -o ${OBJECTDIR}/_ext/1660958476/manLogicPcb.o ../file/IPSModule/manLogicPcb.c  
	
${OBJECTDIR}/_ext/1234305445/MMS_App.o: ../file/MMSModule/MMS_App.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1234305445" 
	@${RM} ${OBJECTDIR}/_ext/1234305445/MMS_App.o.d 
	@${RM} ${OBJECTDIR}/_ext/1234305445/MMS_App.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1234305445/MMS_App.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1234305445/MMS_App.o.d" -o ${OBJECTDIR}/_ext/1234305445/MMS_App.o ../file/MMSModule/MMS_App.c  
	
${OBJECTDIR}/_ext/1234305445/MMSUart.o: ../file/MMSModule/MMSUart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1234305445" 
	@${RM} ${OBJECTDIR}/_ext/1234305445/MMSUart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1234305445/MMSUart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1234305445/MMSUart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1234305445/MMSUart.o.d" -o ${OBJECTDIR}/_ext/1234305445/MMSUart.o ../file/MMSModule/MMSUart.c  
	
${OBJECTDIR}/_ext/1234305445/saveIbpX.o: ../file/MMSModule/saveIbpX.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1234305445" 
	@${RM} ${OBJECTDIR}/_ext/1234305445/saveIbpX.o.d 
	@${RM} ${OBJECTDIR}/_ext/1234305445/saveIbpX.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1234305445/saveIbpX.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1234305445/saveIbpX.o.d" -o ${OBJECTDIR}/_ext/1234305445/saveIbpX.o ../file/MMSModule/saveIbpX.c  
	
${OBJECTDIR}/_ext/784496404/PIC24_App.o: ../file/PIC24Module/PIC24_App.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/784496404" 
	@${RM} ${OBJECTDIR}/_ext/784496404/PIC24_App.o.d 
	@${RM} ${OBJECTDIR}/_ext/784496404/PIC24_App.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/784496404/PIC24_App.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/784496404/PIC24_App.o.d" -o ${OBJECTDIR}/_ext/784496404/PIC24_App.o ../file/PIC24Module/PIC24_App.c  
	
${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o: ../file/PIC24Update/P24UpdateDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1014434161" 
	@${RM} ${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o.d" -o ${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o ../file/PIC24Update/P24UpdateDrv.c  
	
${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o: ../file/PIC24Update/P24UpdateIF.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1014434161" 
	@${RM} ${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o.d 
	@${RM} ${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o.d" -o ${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o ../file/PIC24Update/P24UpdateIF.c  
	
${OBJECTDIR}/_ext/1364844071/SigSys_App.o: ../file/SigModule/SigSys_App.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1364844071" 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_App.o.d 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_App.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1364844071/SigSys_App.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1364844071/SigSys_App.o.d" -o ${OBJECTDIR}/_ext/1364844071/SigSys_App.o ../file/SigModule/SigSys_App.c  
	
${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o: ../file/SigModule/SigSys_Reg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1364844071" 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o.d" -o ${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o ../file/SigModule/SigSys_Reg.c  
	
${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o: ../file/SigModule/SigSys_Uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1364844071" 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o.d" -o ${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o ../file/SigModule/SigSys_Uart.c  
	
${OBJECTDIR}/_ext/2129265379/test.o: ../file/TestCode/test.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2129265379" 
	@${RM} ${OBJECTDIR}/_ext/2129265379/test.o.d 
	@${RM} ${OBJECTDIR}/_ext/2129265379/test.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2129265379/test.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/2129265379/test.o.d" -o ${OBJECTDIR}/_ext/2129265379/test.o ../file/TestCode/test.c  
	
${OBJECTDIR}/_ext/761014165/Main.o: ../file/Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/761014165" 
	@${RM} ${OBJECTDIR}/_ext/761014165/Main.o.d 
	@${RM} ${OBJECTDIR}/_ext/761014165/Main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/761014165/Main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG   -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/761014165/Main.o.d" -o ${OBJECTDIR}/_ext/761014165/Main.o ../file/Main.c  
	
else
${OBJECTDIR}/_ext/2028936924/BASSys_App.o: ../file/BASModule/BASSys_App.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2028936924" 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_App.o.d 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_App.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2028936924/BASSys_App.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/2028936924/BASSys_App.o.d" -o ${OBJECTDIR}/_ext/2028936924/BASSys_App.o ../file/BASModule/BASSys_App.c  
	
${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o: ../file/BASModule/BASSys_Reg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2028936924" 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o.d 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o.d" -o ${OBJECTDIR}/_ext/2028936924/BASSys_Reg.o ../file/BASModule/BASSys_Reg.c  
	
${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o: ../file/BASModule/BASSys_Uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2028936924" 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o.d" -o ${OBJECTDIR}/_ext/2028936924/BASSys_Uart.o ../file/BASModule/BASSys_Uart.c  
	
${OBJECTDIR}/_ext/26038253/ApiCanCom.o: ../file/canbus/drv/ApiCanCom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanCom.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanCom.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/ApiCanCom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/ApiCanCom.o.d" -o ${OBJECTDIR}/_ext/26038253/ApiCanCom.o ../file/canbus/drv/ApiCanCom.c  
	
${OBJECTDIR}/_ext/26038253/ApiCanMst.o: ../file/canbus/drv/ApiCanMst.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanMst.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanMst.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/ApiCanMst.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/ApiCanMst.o.d" -o ${OBJECTDIR}/_ext/26038253/ApiCanMst.o ../file/canbus/drv/ApiCanMst.c  
	
${OBJECTDIR}/_ext/26038253/ApiCanSlv.o: ../file/canbus/drv/ApiCanSlv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanSlv.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiCanSlv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/ApiCanSlv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/ApiCanSlv.o.d" -o ${OBJECTDIR}/_ext/26038253/ApiCanSlv.o ../file/canbus/drv/ApiCanSlv.c  
	
${OBJECTDIR}/_ext/26038253/ApiMain.o: ../file/canbus/drv/ApiMain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiMain.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/ApiMain.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/ApiMain.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/ApiMain.o.d" -o ${OBJECTDIR}/_ext/26038253/ApiMain.o ../file/canbus/drv/ApiMain.c  
	
${OBJECTDIR}/_ext/26038253/CanDrv.o: ../file/canbus/drv/CanDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/CanDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/CanDrv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/CanDrv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/CanDrv.o.d" -o ${OBJECTDIR}/_ext/26038253/CanDrv.o ../file/canbus/drv/CanDrv.c  
	
${OBJECTDIR}/_ext/26038253/UpApiMst.o: ../file/canbus/drv/UpApiMst.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpApiMst.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpApiMst.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/UpApiMst.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/UpApiMst.o.d" -o ${OBJECTDIR}/_ext/26038253/UpApiMst.o ../file/canbus/drv/UpApiMst.c  
	
${OBJECTDIR}/_ext/26038253/UpApiSlv.o: ../file/canbus/drv/UpApiSlv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpApiSlv.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpApiSlv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/UpApiSlv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/UpApiSlv.o.d" -o ${OBJECTDIR}/_ext/26038253/UpApiSlv.o ../file/canbus/drv/UpApiSlv.c  
	
${OBJECTDIR}/_ext/26038253/UpBios.o: ../file/canbus/drv/UpBios.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpBios.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/UpBios.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/UpBios.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/UpBios.o.d" -o ${OBJECTDIR}/_ext/26038253/UpBios.o ../file/canbus/drv/UpBios.c  
	
${OBJECTDIR}/_ext/26038253/Update.o: ../file/canbus/drv/Update.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/26038253" 
	@${RM} ${OBJECTDIR}/_ext/26038253/Update.o.d 
	@${RM} ${OBJECTDIR}/_ext/26038253/Update.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/26038253/Update.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/26038253/Update.o.d" -o ${OBJECTDIR}/_ext/26038253/Update.o ../file/canbus/drv/Update.c  
	
${OBJECTDIR}/_ext/1303770836/canbus.o: ../file/canbus/canbus.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1303770836" 
	@${RM} ${OBJECTDIR}/_ext/1303770836/canbus.o.d 
	@${RM} ${OBJECTDIR}/_ext/1303770836/canbus.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1303770836/canbus.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1303770836/canbus.o.d" -o ${OBJECTDIR}/_ext/1303770836/canbus.o ../file/canbus/canbus.c  
	
${OBJECTDIR}/_ext/1303770836/ObjDict.o: ../file/canbus/ObjDict.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1303770836" 
	@${RM} ${OBJECTDIR}/_ext/1303770836/ObjDict.o.d 
	@${RM} ${OBJECTDIR}/_ext/1303770836/ObjDict.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1303770836/ObjDict.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1303770836/ObjDict.o.d" -o ${OBJECTDIR}/_ext/1303770836/ObjDict.o ../file/canbus/ObjDict.c  
	
${OBJECTDIR}/_ext/78931352/can.o: ../file/CANModule/can.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/78931352" 
	@${RM} ${OBJECTDIR}/_ext/78931352/can.o.d 
	@${RM} ${OBJECTDIR}/_ext/78931352/can.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/78931352/can.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/78931352/can.o.d" -o ${OBJECTDIR}/_ext/78931352/can.o ../file/CANModule/can.c  
	
${OBJECTDIR}/_ext/78931352/dcu.o: ../file/CANModule/dcu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/78931352" 
	@${RM} ${OBJECTDIR}/_ext/78931352/dcu.o.d 
	@${RM} ${OBJECTDIR}/_ext/78931352/dcu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/78931352/dcu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/78931352/dcu.o.d" -o ${OBJECTDIR}/_ext/78931352/dcu.o ../file/CANModule/dcu.c  
	
${OBJECTDIR}/_ext/1316680719/i2cDrv.o: ../file/common/i2cDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/i2cDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/i2cDrv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/i2cDrv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/i2cDrv.o.d" -o ${OBJECTDIR}/_ext/1316680719/i2cDrv.o ../file/common/i2cDrv.c  
	
${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o: ../file/common/Ini_Cpu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o.d" -o ${OBJECTDIR}/_ext/1316680719/Ini_Cpu.o ../file/common/Ini_Cpu.c  
	
${OBJECTDIR}/_ext/1316680719/sysTimer.o: ../file/common/sysTimer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/sysTimer.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/sysTimer.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/sysTimer.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/sysTimer.o.d" -o ${OBJECTDIR}/_ext/1316680719/sysTimer.o ../file/common/sysTimer.c  
	
${OBJECTDIR}/_ext/1316680719/uartDrv.o: ../file/common/uartDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartDrv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/uartDrv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/uartDrv.o.d" -o ${OBJECTDIR}/_ext/1316680719/uartDrv.o ../file/common/uartDrv.c  
	
${OBJECTDIR}/_ext/1316680719/usrCRC.o: ../file/common/usrCRC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/usrCRC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/usrCRC.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/usrCRC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/usrCRC.o.d" -o ${OBJECTDIR}/_ext/1316680719/usrCRC.o ../file/common/usrCRC.c  
	
${OBJECTDIR}/_ext/1316680719/uartDMA.o: ../file/common/uartDMA.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartDMA.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartDMA.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/uartDMA.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/uartDMA.o.d" -o ${OBJECTDIR}/_ext/1316680719/uartDMA.o ../file/common/uartDMA.c  
	
${OBJECTDIR}/_ext/1316680719/uartRxBuff.o: ../file/common/uartRxBuff.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1316680719" 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartRxBuff.o.d 
	@${RM} ${OBJECTDIR}/_ext/1316680719/uartRxBuff.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1316680719/uartRxBuff.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1316680719/uartRxBuff.o.d" -o ${OBJECTDIR}/_ext/1316680719/uartRxBuff.o ../file/common/uartRxBuff.c  
	
${OBJECTDIR}/_ext/1090910534/IOConfig.o: ../file/IOCfg/IOConfig.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1090910534" 
	@${RM} ${OBJECTDIR}/_ext/1090910534/IOConfig.o.d 
	@${RM} ${OBJECTDIR}/_ext/1090910534/IOConfig.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1090910534/IOConfig.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1090910534/IOConfig.o.d" -o ${OBJECTDIR}/_ext/1090910534/IOConfig.o ../file/IOCfg/IOConfig.c  
	
${OBJECTDIR}/_ext/840646674/IO_HANDLE.o: ../file/IOHandle/IO_HANDLE.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/840646674" 
	@${RM} ${OBJECTDIR}/_ext/840646674/IO_HANDLE.o.d 
	@${RM} ${OBJECTDIR}/_ext/840646674/IO_HANDLE.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/840646674/IO_HANDLE.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/840646674/IO_HANDLE.o.d" -o ${OBJECTDIR}/_ext/840646674/IO_HANDLE.o ../file/IOHandle/IO_HANDLE.c  
	
${OBJECTDIR}/_ext/840646674/mainIO.o: ../file/IOHandle/mainIO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/840646674" 
	@${RM} ${OBJECTDIR}/_ext/840646674/mainIO.o.d 
	@${RM} ${OBJECTDIR}/_ext/840646674/mainIO.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/840646674/mainIO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/840646674/mainIO.o.d" -o ${OBJECTDIR}/_ext/840646674/mainIO.o ../file/IOHandle/mainIO.c  
	
${OBJECTDIR}/_ext/1660958476/IPSUart.o: ../file/IPSModule/IPSUart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1660958476" 
	@${RM} ${OBJECTDIR}/_ext/1660958476/IPSUart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1660958476/IPSUart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1660958476/IPSUart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1660958476/IPSUart.o.d" -o ${OBJECTDIR}/_ext/1660958476/IPSUart.o ../file/IPSModule/IPSUart.c  
	
${OBJECTDIR}/_ext/1660958476/IPS_App.o: ../file/IPSModule/IPS_App.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1660958476" 
	@${RM} ${OBJECTDIR}/_ext/1660958476/IPS_App.o.d 
	@${RM} ${OBJECTDIR}/_ext/1660958476/IPS_App.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1660958476/IPS_App.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1660958476/IPS_App.o.d" -o ${OBJECTDIR}/_ext/1660958476/IPS_App.o ../file/IPSModule/IPS_App.c  
	
${OBJECTDIR}/_ext/1660958476/manLogicPcb.o: ../file/IPSModule/manLogicPcb.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1660958476" 
	@${RM} ${OBJECTDIR}/_ext/1660958476/manLogicPcb.o.d 
	@${RM} ${OBJECTDIR}/_ext/1660958476/manLogicPcb.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1660958476/manLogicPcb.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1660958476/manLogicPcb.o.d" -o ${OBJECTDIR}/_ext/1660958476/manLogicPcb.o ../file/IPSModule/manLogicPcb.c  
	
${OBJECTDIR}/_ext/1234305445/MMS_App.o: ../file/MMSModule/MMS_App.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1234305445" 
	@${RM} ${OBJECTDIR}/_ext/1234305445/MMS_App.o.d 
	@${RM} ${OBJECTDIR}/_ext/1234305445/MMS_App.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1234305445/MMS_App.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1234305445/MMS_App.o.d" -o ${OBJECTDIR}/_ext/1234305445/MMS_App.o ../file/MMSModule/MMS_App.c  
	
${OBJECTDIR}/_ext/1234305445/MMSUart.o: ../file/MMSModule/MMSUart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1234305445" 
	@${RM} ${OBJECTDIR}/_ext/1234305445/MMSUart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1234305445/MMSUart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1234305445/MMSUart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1234305445/MMSUart.o.d" -o ${OBJECTDIR}/_ext/1234305445/MMSUart.o ../file/MMSModule/MMSUart.c  
	
${OBJECTDIR}/_ext/1234305445/saveIbpX.o: ../file/MMSModule/saveIbpX.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1234305445" 
	@${RM} ${OBJECTDIR}/_ext/1234305445/saveIbpX.o.d 
	@${RM} ${OBJECTDIR}/_ext/1234305445/saveIbpX.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1234305445/saveIbpX.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1234305445/saveIbpX.o.d" -o ${OBJECTDIR}/_ext/1234305445/saveIbpX.o ../file/MMSModule/saveIbpX.c  
	
${OBJECTDIR}/_ext/784496404/PIC24_App.o: ../file/PIC24Module/PIC24_App.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/784496404" 
	@${RM} ${OBJECTDIR}/_ext/784496404/PIC24_App.o.d 
	@${RM} ${OBJECTDIR}/_ext/784496404/PIC24_App.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/784496404/PIC24_App.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/784496404/PIC24_App.o.d" -o ${OBJECTDIR}/_ext/784496404/PIC24_App.o ../file/PIC24Module/PIC24_App.c  
	
${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o: ../file/PIC24Update/P24UpdateDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1014434161" 
	@${RM} ${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o.d" -o ${OBJECTDIR}/_ext/1014434161/P24UpdateDrv.o ../file/PIC24Update/P24UpdateDrv.c  
	
${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o: ../file/PIC24Update/P24UpdateIF.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1014434161" 
	@${RM} ${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o.d 
	@${RM} ${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o.d" -o ${OBJECTDIR}/_ext/1014434161/P24UpdateIF.o ../file/PIC24Update/P24UpdateIF.c  
	
${OBJECTDIR}/_ext/1364844071/SigSys_App.o: ../file/SigModule/SigSys_App.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1364844071" 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_App.o.d 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_App.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1364844071/SigSys_App.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1364844071/SigSys_App.o.d" -o ${OBJECTDIR}/_ext/1364844071/SigSys_App.o ../file/SigModule/SigSys_App.c  
	
${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o: ../file/SigModule/SigSys_Reg.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1364844071" 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o.d 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o.d" -o ${OBJECTDIR}/_ext/1364844071/SigSys_Reg.o ../file/SigModule/SigSys_Reg.c  
	
${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o: ../file/SigModule/SigSys_Uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1364844071" 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o.d" -o ${OBJECTDIR}/_ext/1364844071/SigSys_Uart.o ../file/SigModule/SigSys_Uart.c  
	
${OBJECTDIR}/_ext/2129265379/test.o: ../file/TestCode/test.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2129265379" 
	@${RM} ${OBJECTDIR}/_ext/2129265379/test.o.d 
	@${RM} ${OBJECTDIR}/_ext/2129265379/test.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2129265379/test.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/2129265379/test.o.d" -o ${OBJECTDIR}/_ext/2129265379/test.o ../file/TestCode/test.c  
	
${OBJECTDIR}/_ext/761014165/Main.o: ../file/Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/761014165" 
	@${RM} ${OBJECTDIR}/_ext/761014165/Main.o.d 
	@${RM} ${OBJECTDIR}/_ext/761014165/Main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/761014165/Main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION) -fdata-sections -DPCB_MON_SZ20_V1 -DPRIVATE_STATIC_DIS -I"../file" -I"../file/canbus" -I"../file/CANModule" -I"../file/canbus/drv" -I"../file/Common" -I"../file/SigModule" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32-libs/peripheral/i2c/source" -I"E:/Program Files (x86)/Microchip/MPLAB C32 Suite/pic32mx/include" -I"../file/IbpPslSig_Uart" -I"../file/IOCfg" -I"../file/IOHandle" -I"../file/IPSModule" -I"../file/MMSModule" -I"../file/PIC24Module" -I"../file/PIC24Update" -I"../file/TestCode" -I"../file/SWLogic" -I"../file/BASModule" -MMD -MF "${OBJECTDIR}/_ext/761014165/Main.o.d" -o ${OBJECTDIR}/_ext/761014165/Main.o ../file/Main.c  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/GITTEST_PEDC_MON_PIC32_V1_1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    ../linker/linkerELF_512.ld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)    -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/GITTEST_PEDC_MON_PIC32_V1_1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}       -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=_min_heap_size=0,--defsym=_min_stack_size=1024,-L"C:/Program Files (x86)/Microchip/MPLAB C32/lib",-L"C:/Program Files (x86)/Microchip/MPLAB C32/pic32mx/lib",-L".",-Map="${DISTDIR}/PEDC_IBP_P32mx795F512H.X.${IMAGE_TYPE}.map",--report-mem,--cref 
else
dist/${CND_CONF}/${IMAGE_TYPE}/GITTEST_PEDC_MON_PIC32_V1_1.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   ../linker/linkerELF_512.ld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/GITTEST_PEDC_MON_PIC32_V1_1.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}       -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=0,--defsym=_min_stack_size=1024,-L"C:/Program Files (x86)/Microchip/MPLAB C32/lib",-L"C:/Program Files (x86)/Microchip/MPLAB C32/pic32mx/lib",-L".",-Map="${DISTDIR}/PEDC_IBP_P32mx795F512H.X.${IMAGE_TYPE}.map",--report-mem,--cref
	${MP_CC_DIR}\\pic32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/GITTEST_PEDC_MON_PIC32_V1_1.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
