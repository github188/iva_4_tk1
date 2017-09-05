#ifndef __ALGORITHM_LIST__
#define __ALGORITHM_LIST__

#ifdef __cplusplus
extern "C" {
#endif

//城市道路事件检测  操作类型宏定义
#define		ALG_IVR_CreateChannel			0x0001
#define		ALG_IVR_DestroyChannel			0x0002
#define		ALG_IVR_SetSinglePreset			0x0003
#define		ALG_IVR_SetEventEvidenceConf	0x0004
#define		ALG_IVR_SetControlerHandle		0x0005
#define		ALG_IVR_ResetAlg				0x0006
#define		ALG_IVR_CalibrationTest			0x0007
#define		ALG_IVR_SetProvince				0x0008
#define		ALG_IVR_GetGraphResult			0x0009
#define     ALG_IVR_ManualIllegeStop		0x0010
#define     ALG_IVR_GetVersion				0x0015
#define		ALG_IVR_YoloProcessFrame		0x0016
#define		ALG_IVR_SetPlateDiagonalLen		0x0017
#define		ALG_IVR_SetChangeDetection		0x0018
#define		ALG_IVR_SetPresetList			0x0019



//algExchange 事件结果类型宏定义
#define		ALG_RES_IVR						0x0000
#define		ALG_RES_IVR_SIM					0xff00

#ifdef __cplusplus
	}
#endif

#endif
