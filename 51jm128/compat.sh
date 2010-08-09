exec >compat.h

cat <<EOF
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

#define MCF_USB_OTG_ENDPT(x) ENDPT_ARR[4*(x)]
#define MCF_USB_OTG_ENDPT_EP_HSHK ENDPT0_EP_HSHK
#define MCF_USB_OTG_ENDPT_EP_RX_EN ENDPT0_EP_RX_EN
#define MCF_USB_OTG_ENDPT_EP_TX_EN ENDPT0_EP_TX_EN
#define MCF_USB_OTG_ENDPT_HOST_WO_HUB ENDPT0_HOST_WO_HUB
#define MCF_USB_OTG_ENDPT_RETRY_DIS ENDPT0_RETRY_DIS
#define MCF_USB_OTG_USB_CTRL_CLK_SRC(x)  (x)

#define MCF_CFM_CFMCLKD  FCDIV
#define MCF_CFM_CFMCLKD_DIVLD  FCDIV_FDIVLD_MASK
#define MCF_CFM_CFMUSTAT  FSTAT
#define MCF_CFM_CFMUSTAT_CBEIF  FSTAT_FCBEF_MASK
#define MCF_CFM_CFMUSTAT_PVIOL  FSTAT_FPVIOL_MASK
#define MCF_CFM_CFMUSTAT_ACCERR  FSTAT_FACCERR_MASK
#define MCF_CFM_CFMCMD  FCMD
#define MCF_CFM_CFMUSTAT_CCIF  FSTAT_FCCF_MASK
#define MCF_CFM_CFMCLKD_PRDIV8  FCDIV_PRDIV8_MASK
#define MCF_CFM_CFMCLKD_DIV(x)  (x)

#define MCF_CFM_CFMCMD_PAGE_ERASE  0x40
#define MCF_CFM_CFMCMD_WORD_PROGRAM  0x20

#define MCF_USB_OTG_TOKEN_TOKEN_PID(x)  ((x)<<4)
#define MCF_USB_OTG_TOKEN_TOKEN_ENDPT(x)  (x)

// automatic below

EOF

cat ../sources/usb.c | 
    grep "\<MCF_USB_OTG_" |
    sed 's!\<MCF_USB_OTG_!|MCF_USB_OTG_!g' | tr '|' "\012" |
    grep MCF_USB_OTG_  | sed 's![^0-9A-Z_(].*!!' | sort -u | grep -v "(" | grep -v "ENDPT_" |
    while read d; do
        if grep " ${d#MCF_USB_OTG_}_MASK" mcf51jm128.h >/dev/null; then
            echo "#define $d ${d#MCF_USB_OTG_}_MASK"
        else
            echo "#define $d ${d#MCF_USB_OTG_}"
        fi
    done
