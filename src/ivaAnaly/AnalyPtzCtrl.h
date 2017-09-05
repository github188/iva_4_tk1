#ifndef __ANALY_PTZ_CTRL_H__
#define __ANALY_PTZ_CTRL_H__
#include "capacity.h"
#include "AlgorithmInput.h"

#define USE_PRESET_CHECK

int t_Func_PTZ_Move(int iChnID,DIRECTION dx, unsigned char speedx, DIRECTION dy, unsigned char  speedy, int stopInterval);

int t_Func_PTZ_ZoomIn(int iChnID, int stopInterval);

int t_Func_PTZ_ZoomOut(int iChnID, int stopInterval);

int t_Func_PTZ_Stop(int iChnID);

int t_Func_PTZ_ClickZoomIn(int iChnID, int x, int y, float z, int w, int h, float speedx, float speedy);

int t_Func_PTZ_SetPreset(int iChnID, int iPresetNo);

int t_Func_PTZ_ToPreset(int iChnID, int iPresetNo);

int t_Func_PTZ_DelPreset(int iChnID, int iPresetNo);

int t_Func_PTZ_GetAbsPTZ(int iChnID, float * pfP, float *pfT, float *pfZ);

int t_Func_PTZ_SetAbsPTZ(int iChnID, float fP, float fT, float fZ);



extern PTZController g_tPtzCtrler[MAX_CHANNEL_NUM];
void InitPtzCtrler();

#endif // !__ANALY_PTZ_CTRL_H__
