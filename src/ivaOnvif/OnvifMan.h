#ifndef __ONVIF_MAN_H__
#define __ONVIF_MAN_H__

#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "libonvif2.h"
#include "capacity.h"
#include "mq_onvif.h"
#include "mq_videoin.h"


#define MAX_STREAM_NUM 4
typedef struct _OnvifProfileInfo
{
	int 		  hSession;
	PTZInfo 	  tPtzLoginInfo;
	int           iStreamCnt;
	StreamInfo 	  tStreamInfo[MAX_STREAM_NUM];
}OnvifProfileInfo;

class OnvifManager
{
public:
	static OnvifManager* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	OnvifManager();
	~OnvifManager();

public:
	int Run();

	int SetDevInfo(int iChnID, const VISource* ptSource);
	int GetDevInfo(int iChnID, VISource& tSource);

	int SetClickZoomProxy(int iChnID, const ClickZoomProxy* ptProxy);
	int GetClickZoomProxy(int iChnID, ClickZoomProxy& tProxy);

	int SetTimeSynInfo(const OnvifTimeSyn* ptTimeSyn);
	int GetTimeSynInfo(OnvifTimeSyn & tTimeSyn);

	int GetChnSolution(int iChnID, int &width, int &height);

	int OnvifLogin(int iChnID);
	int OnvifClose(int iChnID);

	int Onvif_Ptz_Move(int iChnID, int iAction,int iSpeed);
	int Onvif_Ptz_Stop(int iChnID);

	int Onvif_Aperture_Move(int iChnID, int iAction,int iSpeed);
	int Onvif_Aperture_Stop(int iChnID);

	int Onvif_Focus_Move(int iChnID, int iAction,int iSpeed);
	int Onvif_Focus_Stop(int iChnID);

	int Onvif_GetPresets(int iChnID, PresetArray *ptPresets);	
	int Onvif_SetPreset(int iChnID, int iPresetID);	
	int Onvif_DelPreset(int iChnID, int iPresetID);	
	int Onvif_GoToPreset(int iChnID, int iPresetID);

	int Onvif_GetAbsPos(int iChnID, AbsPosition &tPos);	
	int Onvif_AbsMove(int iChnID, const AbsPosition* ptPos);

	int Onvif_ClickZoom(int iChnID, const ClickArea * ptArea);	

	int Onvif_GetLocalTime(int iChnID, time_t & tNow);

	bool m_bExitThread;
	bool m_bDisconnected[MAX_CHANNEL_NUM];

private:
	VISource		 m_tDevs[MAX_CHANNEL_NUM];
	ClickZoomProxy	 m_tProxy[MAX_CHANNEL_NUM];
	OnvifProfileInfo m_tProfileInfos[MAX_CHANNEL_NUM];
	OnvifTimeSyn     m_tTimeSynInfo;
	pthread_mutex_t  m_mutex;
	pthread_t 		 m_manage_thread;
	pthread_t 		 m_timesyn_thread;

private:
	static OnvifManager* m_pInstance;
};



#endif //__ONVIF_MAN_H__

