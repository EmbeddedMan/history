#define REQUEST_GET_DEVICE_INFO  0x1001
#define REQUEST_OPEN_SESSION  0x1002
#define REQUEST_INITIATE_CAPTURE  0x100e

#define RESPONSE_OK  0x2001

struct pima {
    uint16 code;  // IN
    uint16 opn;  // IN
    uint32 op1;
    uint32 op2;
    uint32 op3;
    uint16 resn;  // OUT
    uint32 res1;
    uint32 res2;
    uint32 res3;
};

int
pima_transfer(int in, struct pima *pima, byte *data, int data_length);

void
pima_run(void);

#define PTP_VENDOR_CANON                  0x0000000B

/* Operation Codes */
#define PTP_OC_Undefined                      0x1000
#define PTP_OC_GetDeviceInfo                  0x1001
#define PTP_OC_OpenSession                    0x1002
#define PTP_OC_CloseSession                   0x1003
#define PTP_OC_GetStorageIDs                  0x1004
#define PTP_OC_GetStorageInfo                 0x1005
#define PTP_OC_GetNumObjects                  0x1006
#define PTP_OC_GetObjectHandles               0x1007
#define PTP_OC_GetObjectInfo                  0x1008
#define PTP_OC_GetObject                      0x1009
#define PTP_OC_GetThumb                       0x100A
#define PTP_OC_DeleteObject                   0x100B
#define PTP_OC_SendObjectInfo                 0x100C
#define PTP_OC_SendObject                     0x100D
#define PTP_OC_InitiateCapture                0x100E
#define PTP_OC_FormatStore                    0x100F
#define PTP_OC_ResetDevice                    0x1010
#define PTP_OC_SelfTest                       0x1011
#define PTP_OC_SetObjectProtection            0x1012
#define PTP_OC_PowerDown                      0x1013
#define PTP_OC_GetDevicePropDesc              0x1014
#define PTP_OC_GetDevicePropValue             0x1015
#define PTP_OC_SetDevicePropValue             0x1016
#define PTP_OC_ResetDevicePropValue           0x1017
#define PTP_OC_TerminateOpenCapture           0x1018
#define PTP_OC_MoveObject                     0x1019
#define PTP_OC_CopyObject                     0x101A
#define PTP_OC_GetPartialObject               0x101B
#define PTP_OC_InitiateOpenCapture            0x101C

/* Canon extension Operation Codes */
#define PTP_OC_CANON_GetObjectSize            0x9001
#define PTP_OC_CANON_StartShootingMode        0x9008
#define PTP_OC_CANON_EndShootingMode          0x9009
#define PTP_OC_CANON_ViewfinderOn             0x900B
#define PTP_OC_CANON_ViewfinderOff            0x900C
#define PTP_OC_CANON_ReflectChanges           0x900D
#define PTP_OC_CANON_CheckEvent               0x9013
#define PTP_OC_CANON_FocusLock                0x9014
#define PTP_OC_CANON_FocusUnlock              0x9015
#define PTP_OC_CANON_InitiateCaptureInMemory  0x901A
#define PTP_OC_CANON_GetPartialObject         0x901B
#define PTP_OC_CANON_GetViewfinderImage       0x901d
#define PTP_OC_CANON_GetChanges               0x9020
#define PTP_OC_CANON_GetFolderEntries         0x9021

/* Device Properties Codes */
#define PTP_DPC_Undefined                     0x5000
#define PTP_DPC_BatteryLevel                  0x5001
#define PTP_DPC_FunctionalMode                0x5002
#define PTP_DPC_ImageSize                     0x5003
#define PTP_DPC_CompressionSetting            0x5004
#define PTP_DPC_WhiteBalance                  0x5005
#define PTP_DPC_RGBGain                       0x5006
#define PTP_DPC_FNumber                       0x5007
#define PTP_DPC_FocalLength                   0x5008
#define PTP_DPC_FocusDistance                 0x5009
#define PTP_DPC_FocusMode                     0x500A
#define PTP_DPC_ExposureMeteringMode          0x500B
#define PTP_DPC_FlashMode                     0x500C
#define PTP_DPC_ExposureTime                  0x500D
#define PTP_DPC_ExposureProgramMode           0x500E
#define PTP_DPC_ExposureIndex                 0x500F
#define PTP_DPC_ExposureBiasCompensation      0x5010
#define PTP_DPC_DateTime                      0x5011
#define PTP_DPC_CaptureDelay                  0x5012
#define PTP_DPC_StillCaptureMode              0x5013
#define PTP_DPC_Contrast                      0x5014
#define PTP_DPC_Sharpness                     0x5015
#define PTP_DPC_DigitalZoom                   0x5016
#define PTP_DPC_EffectMode                    0x5017
#define PTP_DPC_BurstNumber                   0x5018
#define PTP_DPC_BurstInterval                 0x5019
#define PTP_DPC_TimelapseNumber               0x501A
#define PTP_DPC_TimelapseInterval             0x501B
#define PTP_DPC_FocusMeteringMode             0x501C
#define PTP_DPC_UploadURL                     0x501D
#define PTP_DPC_Artist                        0x501E
#define PTP_DPC_CopyrightInfo                 0x501F

/* Canon extension device property codes */
#define PTP_DPC_CANON_BeepMode                0xD001
#define PTP_DPC_CANON_ViewfinderMode          0xD003
#define PTP_DPC_CANON_ImageQuality            0xD006
#define PTP_DPC_CANON_D007                    0xD007
#define PTP_DPC_CANON_ImageSize               0xD008
#define PTP_DPC_CANON_FlashMode               0xD00A
#define PTP_DPC_CANON_TvAvSetting             0xD00C
#define PTP_DPC_CANON_MeteringMode            0xD010
#define PTP_DPC_CANON_MacroMode               0xD011
#define PTP_DPC_CANON_FocusingPoint           0xD012
#define PTP_DPC_CANON_WhiteBalance            0xD013
#define PTP_DPC_CANON_ISOSpeed                0xD01C
#define PTP_DPC_CANON_Aperture                0xD01D
#define PTP_DPC_CANON_ShutterSpeed            0xD01E
#define PTP_DPC_CANON_ExpCompensation         0xD01F
#define PTP_DPC_CANON_D029                    0xD029
#define PTP_DPC_CANON_Zoom                    0xD02A
#define PTP_DPC_CANON_SizeQualityMode         0xD02C
#define PTP_DPC_CANON_FlashMemory             0xD031
#define PTP_DPC_CANON_CameraModel             0xD032
#define PTP_DPC_CANON_CameraOwner             0xD033
#define PTP_DPC_CANON_UnixTime                0xD034
#define PTP_DPC_CANON_ViewfinderOutput        0xD036
#define PTP_DPC_CANON_RealImageWidth          0xD039
#define PTP_DPC_CANON_PhotoEffect             0xD040
#define PTP_DPC_CANON_AssistLight             0xD041
#define PTP_DPC_CANON_D045                    0xD045
