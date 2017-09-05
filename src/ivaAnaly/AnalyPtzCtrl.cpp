#include "mq_onvif.h"
#include "AnalyPtzCtrl.h"
#include "AnalyCfgMan.h"
#include "oal_time.h"

int t_Func_PTZ_Move(int iChnID,DIRECTION dx, unsigned char speedx, DIRECTION dy, unsigned char  speedy, int stopInterval)
{
	int iAction = 0;

	switch (dx)
	{
	case UP:
		iAction = PTZ_MOVE_UP;
		break;
	case DOWN:
		iAction = PTZ_MOVE_DOWN;
		break;
	case LEFT:
		iAction = PTZ_MOVE_LEFT;
		break;
	case RIGHT:
		iAction = PTZ_MOVE_RIGHT;
		break;
	default:
		return -1;
	}

	return MQ_Onvif_Ptz_Move(MSG_TYPE_ANALY, iChnID, iAction, 100);
}

int t_Func_PTZ_ZoomIn(int iChnID, int stopInterval)
{
	return MQ_Onvif_Ptz_Move(MSG_TYPE_ANALY, iChnID, PTZ_MOVE_ZOOM_IN, 100);
}

int t_Func_PTZ_ZoomOut(int iChnID, int stopInterval)
{
	return MQ_Onvif_Ptz_Move(MSG_TYPE_ANALY, iChnID, PTZ_MOVE_ZOOM_OUT, 100);
}

int t_Func_PTZ_Stop(int iChnID)
{
	return MQ_Onvif_Ptz_Stop(MSG_TYPE_ANALY, iChnID);
}

int t_Func_PTZ_ClickZoomIn(int iChnID, int x, int y, float z, int w, int h, float speedx, float speedy)
{
	ClickArea tClickArea;
	memset(&tClickArea,0,sizeof(ClickArea));
	tClickArea.StartPoint.x = x;
	tClickArea.StartPoint.y = y;
	tClickArea.EndPoint.x = x + w;
	tClickArea.EndPoint.y =  y + h;
	printf("x=%d y=%d w=%d h=%d\n",  x, y, w, h);

	return MQ_Onvif_ClickZoom(MSG_TYPE_ANALY,iChnID, &tClickArea);
}

int t_Func_PTZ_SetPreset(int iChnID, int iPresetNo)
{
	return MQ_Onvif_Preset(MSG_TYPE_ANALY,iChnID, PRESET_SET, iPresetNo);
}

int t_Func_PTZ_ToPreset(int iChnID, int iPresetNo)
{
	MQ_Onvif_Preset(MSG_TYPE_ANALY, iChnID, PRESET_GOTO, iPresetNo);
	printf("goto chn%d preset %d\n",  iChnID, iPresetNo);

	// 等待球机到位
	AbsPosition tPos = {0};
	int iRet = AnalyCfgMan::GetInstance()->GetPresetPTZ(iChnID, iPresetNo, &tPos);
	if( iRet == 0 && (tPos.fPan != 0.0f || tPos.fTilt != 0.0f || tPos.fZoom != 0.0f))
	{
		printf("[Channel%d Preset%d] Need:%f %f %f\n",iChnID, iPresetNo, tPos.fPan, tPos.fTilt, tPos.fZoom);
		float detaP = 0.0f, detaT = 0.0f, detaZ = 0.0f;

		int iTryTimes = 0;
		while(iTryTimes < 6)
		{
			sleep(2);

			float fP1 = 0.0f, fT1 = 0.0f, fZ1 = 0.0f;
			iRet = t_Func_PTZ_GetAbsPTZ(iChnID, &fP1, &fT1, &fZ1);
			printf("[Channel%d Preset%d] Real:%f %f %f\n",iChnID, iPresetNo, fP1, fT1, fZ1);

			// 球机不支持
			if(iRet == -2)
			{
				return 0;
			}
			else if( iTryTimes > 0 && tPos.fPan == 0.0f && tPos.fTilt == 0.0f && tPos.fZoom == 0.0f)
			{
				return 0;
			}

			detaP = fP1 > tPos.fPan ? fP1 -  tPos.fPan :  tPos.fPan -fP1;
			detaT = fT1 > tPos.fTilt ? fT1 - tPos.fTilt : tPos.fTilt -fT1;
			detaZ = fZ1 > tPos.fZoom ? fZ1 - tPos.fZoom : tPos.fZoom -fZ1;
			if(detaP <= 0.01f && detaT <= 0.01f && detaZ <= 0.01f)
			{
				printf("[Channel%d Preset%d] OK\n",iChnID, iPresetNo);
				break;
			}

			iTryTimes++;
		}

#ifdef USE_PRESET_CHECK
		if(detaP > 0.01f || detaT > 0.01f || detaZ > 0.01f)
		{
			t_Func_PTZ_SetAbsPTZ(iChnID, tPos.fPan, tPos.fTilt, tPos.fZoom);
			printf("[Channel%d Preset%d] SetPTZ:%f %f %f\n",iChnID, iPresetNo, tPos.fPan, tPos.fTilt, tPos.fZoom);

			iTryTimes = 0;
			while(iTryTimes < 6)
			{
				sleep(2);

				float fP1 = 0.0f, fT1 = 0.0f, fZ1 = 0.0f;
				iRet = t_Func_PTZ_GetAbsPTZ(iChnID, &fP1, &fT1, &fZ1);
				
				// 球机不支持
				if(iRet == -2)
				{
					return 0;
				}

				detaP = fP1 > tPos.fPan ? fP1 -  tPos.fPan :  tPos.fPan -fP1;
				detaT = fT1 > tPos.fTilt ? fT1 - tPos.fTilt : tPos.fTilt -fT1;
				detaZ = fZ1 > tPos.fZoom ? fZ1 - tPos.fZoom : tPos.fZoom -fZ1;
				if(detaP <= 0.01f && detaT <= 0.01f && detaZ <= 0.01f)
				{
					t_Func_PTZ_SetPreset(iChnID, iPresetNo);
					printf("[Channel%d Preset%d] SetPreset\n", iChnID, iPresetNo);
					break;
				}

				iTryTimes++;
			}			

			if(iTryTimes >= 6)
			{
				MQ_Onvif_Preset(MSG_TYPE_ANALY, iChnID, PRESET_GOTO, iPresetNo);
			}
		}
	}
#endif
	return 0;
}

int t_Func_PTZ_DelPreset(int iChnID, int iPresetNo)
{
	return MQ_Onvif_Preset(MSG_TYPE_ANALY, iChnID, PRESET_DEL, iPresetNo);
}

int t_Func_PTZ_GetAbsPTZ(int iChnID, float * pfP, float *pfT, float *pfZ)
{
	if(pfP == NULL || pfT == NULL || pfZ == NULL)
	{
		return -1;
	}
	
	AbsPosition tPos = {0};
	int ret = MQ_Onvif_Get_AbsPos(MSG_TYPE_ANALY, iChnID, &tPos);
	*pfP = tPos.fPan;
	*pfT = tPos.fTilt;
	*pfZ = tPos.fZoom;
	return ret;
}

int t_Func_PTZ_SetAbsPTZ(int iChnID, float fP, float fT, float fZ)
{
	AbsPosition tPos = {0};
	tPos.fPan = fP;
	tPos.fTilt = fT;
	tPos.fZoom = fZ;
	return MQ_Onvif_Abs_Move(MSG_TYPE_ANALY,iChnID,&tPos);
}

PTZController g_tPtzCtrler[MAX_CHANNEL_NUM];

void InitPtzCtrler()
{
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		g_tPtzCtrler[i].iPort = i;
		g_tPtzCtrler[i].Stop = t_Func_PTZ_Stop;
		g_tPtzCtrler[i].Move = t_Func_PTZ_Move;
		g_tPtzCtrler[i].ZoomIn = t_Func_PTZ_ZoomIn;
		g_tPtzCtrler[i].ZoomOut = t_Func_PTZ_ZoomOut;
		g_tPtzCtrler[i].ClickZoomIn = t_Func_PTZ_ClickZoomIn;
		g_tPtzCtrler[i].SetPreset = t_Func_PTZ_SetPreset;
		g_tPtzCtrler[i].GotoPreset = t_Func_PTZ_ToPreset;
		g_tPtzCtrler[i].DeletePreset = t_Func_PTZ_DelPreset;
		g_tPtzCtrler[i].GetAbsPTZ = t_Func_PTZ_GetAbsPTZ;
		g_tPtzCtrler[i].SetAbsPTZ = t_Func_PTZ_SetAbsPTZ;
	}
}
