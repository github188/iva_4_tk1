#include "OnvifMan.h"
#include <sys/stat.h>
#include <sys/types.h>  
#include <assert.h>
#include "oal_log.h"
#include "oal_time.h"
#include "mq_master.h"
#include "libonvif2.h"
#include "dahua3dposition.h"
#include "hik3dposition.h"
#include "uniview3dposition.h"
#include "tiandy3dposition.h"
#include "proxy3dposition.h"


void * SynTime2CameraThreadFunc(void * /*args*/)
{
	time_t sLastSysTime = 0;
	int iNeedNow = 1;
	LOG_DEBUG_FMT("SynTime2CameraThread is running...");
	while(OnvifManager::GetInstance()->m_bExitThread == false)
	{
		OnvifTimeSyn tSynCfg = {0};
		OnvifManager::GetInstance()->GetTimeSynInfo(tSynCfg);
		if (!tSynCfg.bEnable)
		{
			sleep(1);
			iNeedNow = 1;
			continue;
		}

		// 该通道是否使能
		VISource tSource = {0};
		OnvifManager::GetInstance()->GetDevInfo(tSynCfg.iSynChnID, tSource);
		if (!tSource.bEnable)
		{
			sleep(1);
			iNeedNow = 1;
			continue;
		}

		// 是否达到同步周期
		time_t tNow = time(NULL);
		if (iNeedNow == 0 && (tNow - sLastSysTime < tSynCfg.uInterval && tNow > String2Time("20100101000000",eYMDHMS2)))
		{
			//LOG_DEBUG_FMT("tNow=%ld sLastSysTime=%ld uInterval=%u", tNow,sLastSysTime,tSynCfg.uInterval);
			sleep(1);
			continue;
		}

		sLastSysTime = tNow;
		iNeedNow = 0;

		// onvif获取系统时间
		time_t tOnvifTime;
		int iRet = OnvifManager::GetInstance()->Onvif_GetLocalTime(tSynCfg.iSynChnID, tOnvifTime);
		if (iRet != 0)
		{
			LOG_DEBUG_FMT("Can not get channel%d`s onvif time",tSynCfg.iSynChnID);
			sleep(1);
			continue;
		}

		tNow = time(NULL);
		char buf1[24] = {0};
		char buf2[24] = {0};
		TimeFormatString(tOnvifTime, buf1, sizeof(buf1),eYMDHMS1);
		TimeFormatString(tNow, buf2, sizeof(buf2),eYMDHMS1);

		time_t iDeta = (tNow > tOnvifTime) ? (tNow - tOnvifTime) : (tOnvifTime - tNow);
		if(iDeta >= TIME_SYN_PRECISION)
		{
			LOG_DEBUG_FMT("============TIME CHECK==========");
			LOG_DEBUG_FMT("Time in SynChn%d = %s",tSynCfg.iSynChnID, buf1);
			LOG_DEBUG_FMT("Time in System  = %s",buf2);

			MQ_Master_SetSysTime(tOnvifTime);
		}

		sleep(1);
	}

	return NULL;
}

static void * OnvifManagerageThread(void * p)
{
	static VISource s_tOldSource[MAX_CHANNEL_NUM] = {0};
	
	while(OnvifManager::GetInstance()->m_bExitThread == false)
	{
		for (int i = 0; i < MAX_CHANNEL_NUM; i++)
		{		
			VISource tNewIPC = {0};
			if(OnvifManager::GetInstance()->GetDevInfo(i, tNewIPC) == 0)
			{
				if( s_tOldSource[i].bEnable != tNewIPC.bEnable ||
					s_tOldSource[i].iStreamType != tNewIPC.iStreamType ||
					strcmp(s_tOldSource[i].szIP,   tNewIPC.szIP) != 0 ||
					strcmp(s_tOldSource[i].szUser, tNewIPC.szUser) != 0||
					strcmp(s_tOldSource[i].szPass, tNewIPC.szPass) != 0)
				{
					LOG_INFOS_FMT("Channel%d Video Source Change ",i);
					LOG_INFOS_FMT("[New Soucre] Enable:%d IP:%s User:%s Pass:%s StreamType:%d",
						tNewIPC.bEnable,tNewIPC.szIP, tNewIPC.szUser,tNewIPC.szPass,tNewIPC.iStreamType);
					memcpy(&s_tOldSource[i], &tNewIPC, sizeof(VISource));

					// 如果视频源配置改变,断掉原来的流
					OnvifManager::GetInstance()->OnvifClose(i);
				}
			}

			// 通道使能 如果断了，重连
			bool bDisConnected = OnvifManager::GetInstance()->m_bDisconnected[i];
			if(tNewIPC.bEnable && bDisConnected)
			{
				OnvifManager::GetInstance()->m_bDisconnected[i] = false;
				OnvifManager::GetInstance()->OnvifLogin(i);
			}
			else if (!tNewIPC.bEnable && !bDisConnected)
			{
				OnvifManager::GetInstance()->OnvifClose(i);
			}

			sleep(1);
		}
	}

	pthread_exit(p);
	return p;
}


OnvifManager* OnvifManager::m_pInstance = NULL;

OnvifManager* OnvifManager::GetInstance()
{
	return m_pInstance;
}

int OnvifManager::Initialize()
{
	//libOnvif2Debug(1);
	libOnvif2SetTimeOut(5, 10, 2);

	if( NULL == m_pInstance)
	{
		m_pInstance = new OnvifManager();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void OnvifManager::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}

	libOnvif2ControlCleanup();
}

OnvifManager::OnvifManager()
{
	pthread_mutex_init(&m_mutex, NULL);

	pthread_mutex_lock(&m_mutex);

	memset(m_tDevs, 0, MAX_CHANNEL_NUM * sizeof(VISource));
	memset(m_tProxy, 0, MAX_CHANNEL_NUM * sizeof(ClickZoomProxy));
	memset(m_tProfileInfos, 0, MAX_CHANNEL_NUM * sizeof(OnvifProfileInfo));
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_bDisconnected[i] = true;
		m_tProfileInfos[i].hSession = -1;
	}

	m_tTimeSynInfo.bEnable = false;
	m_tTimeSynInfo.iSynChnID = 0;
	m_tTimeSynInfo.uInterval = 300;
	pthread_mutex_unlock(&m_mutex);

	m_bExitThread = false;
}

OnvifManager::~OnvifManager()
{
	m_bExitThread = true;
	pthread_join(m_manage_thread,NULL);
	pthread_join(m_timesyn_thread,NULL);
}

int OnvifManager::Run()
{
	m_bExitThread = false;
	pthread_create(&m_manage_thread, NULL, OnvifManagerageThread, this);
	pthread_create(&m_timesyn_thread, NULL, SynTime2CameraThreadFunc, this);
	return 0;
}

int OnvifManager::SetDevInfo( int iChnID, const VISource* ptSource )
{
	if(ptSource == NULL || iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_mutex);
	memcpy(&m_tDevs[iChnID], ptSource, sizeof(VISource));
	pthread_mutex_unlock(&m_mutex);
	return 0;
}

int OnvifManager::GetDevInfo( int iChnID, VISource& tSource )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_mutex);
	memcpy(&tSource, &m_tDevs[iChnID],sizeof(VISource));
	pthread_mutex_unlock(&m_mutex);
	return 0;
}

int OnvifManager::SetClickZoomProxy( int iChnID, const ClickZoomProxy* ptProxy )
{
	if(ptProxy == NULL || iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_mutex);
	memcpy(&m_tProxy[iChnID], ptProxy, sizeof(ClickZoomProxy));
	pthread_mutex_unlock(&m_mutex);
	return 0;
}

int OnvifManager::GetClickZoomProxy( int iChnID, ClickZoomProxy &tProxy )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_mutex);
	memcpy(&tProxy, &m_tDevs[iChnID],sizeof(ClickZoomProxy));
	pthread_mutex_unlock(&m_mutex);
	return 0;
}

int OnvifManager::SetTimeSynInfo( const OnvifTimeSyn* pTimeSyn )
{
	if(pTimeSyn == NULL)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_mutex);
	memcpy(&m_tTimeSynInfo, pTimeSyn, sizeof(OnvifTimeSyn));
	pthread_mutex_unlock(&m_mutex);
	return 0;
}

int OnvifManager::GetTimeSynInfo( OnvifTimeSyn & tTimeSyn )
{
	pthread_mutex_lock(&m_mutex);
	memcpy(&tTimeSyn, &m_tTimeSynInfo,sizeof(OnvifTimeSyn));
	pthread_mutex_unlock(&m_mutex);
	return 0;
}

int OnvifManager::GetChnSolution( int iChnID, int &width, int &height )
{
	int max = 0,min = 0;
	for(int i = 0; i < m_tProfileInfos[iChnID].iStreamCnt; i++)
	{
		if(m_tProfileInfos[iChnID].tStreamInfo[i].width > m_tProfileInfos[iChnID].tStreamInfo[max].width)
			max = i;
		if(m_tProfileInfos[iChnID].tStreamInfo[i].width < m_tProfileInfos[iChnID].tStreamInfo[min].width)
			min = i;
	}

	if (m_tDevs[iChnID].iStreamType == STREAM_TYPE_SUB)
	{
		width  = m_tProfileInfos[iChnID].tStreamInfo[min].width;
		height = m_tProfileInfos[iChnID].tStreamInfo[min].height;
	}
	else
	{
		width  = m_tProfileInfos[iChnID].tStreamInfo[max].width;
		height = m_tProfileInfos[iChnID].tStreamInfo[max].height;
	}
	return 0;
}

int OnvifManager::OnvifLogin(int iChnID)
{
	VISource tDevInfo;
	int iRet = GetDevInfo(iChnID, tDevInfo);
	if(iRet != 0)
	{
		LOG_ERROR_FMT("GetDevInfo %d error",iChnID);
		return -1;
	}

	// 关闭已有链接
	if(m_tProfileInfos[iChnID].hSession > 0)
	{
		OnvifClose(iChnID);
	}

	int hSession = libOnvif2ControlCreate(tDevInfo.szIP);
	if(hSession < 0)
	{
		LOG_ERROR_FMT("libOnvif2ControlCreate failed, Channel=%d ip=%s\n",iChnID, tDevInfo.szIP);
		return -1;
	}

	pthread_mutex_lock(&m_mutex);
	m_tProfileInfos[iChnID].hSession = hSession;
	pthread_mutex_unlock(&m_mutex);
	
	iRet = libOnvif2DeviceLogin(hSession, tDevInfo.szUser, tDevInfo.szPass);
	if(iRet != 0)
	{
		LOG_ERROR_FMT("libOnvif2DeviceLogin failed, Channel=%d ip=%s\n",iChnID, tDevInfo.szIP);
		return -1;
	}

	int iBestIndex = libOnvif2ReportAfterLogin(hSession);
	if(iBestIndex < 0)
	{
		LOG_ERROR_FMT("libOnvif2ReportAfterLogin failed, Channel=%d ip=%s\n",iChnID, tDevInfo.szIP);
		return -1;
	}

	pthread_mutex_lock(&m_mutex);
	m_bDisconnected[iChnID] = false;
	pthread_mutex_unlock(&m_mutex);

	PTZInfo tPtzInfo = {0};
	libOnvif2GetPTZInfo(hSession, iBestIndex, &tPtzInfo);

	pthread_mutex_lock(&m_mutex);
	memcpy(&m_tProfileInfos[iChnID].tPtzLoginInfo, &tPtzInfo, sizeof(PTZInfo));
	pthread_mutex_unlock(&m_mutex);

	// 获取视频URL
	pthread_mutex_lock(&m_mutex);
	m_tProfileInfos[iChnID].iStreamCnt = 0;
	memset(&m_tProfileInfos[iChnID].tStreamInfo, 0, MAX_STREAM_NUM*sizeof(StreamInfo));
	pthread_mutex_unlock(&m_mutex);

	int iCnt = libOnvif2GetProfileSize(hSession);
	for(int i = 0; i < iCnt && m_tProfileInfos[iChnID].iStreamCnt < MAX_STREAM_NUM; i++)
	{
		StreamInfo tStreamInfo = {0};
		int iret = libOnvif2GetStreamInfo(hSession, i, &tStreamInfo);
		if(iret != 0 || strlen(tStreamInfo.URI) < 1)
		{
			continue;
		}
		pthread_mutex_lock(&m_mutex);
		memcpy(m_tProfileInfos[iChnID].tStreamInfo+m_tProfileInfos[iChnID].iStreamCnt, &tStreamInfo, sizeof(StreamInfo));
		m_tProfileInfos[iChnID].iStreamCnt++;
		pthread_mutex_unlock(&m_mutex);
	}

	// 登录成功以后，获取TPZCAP通知MASTER
	PTZCap tPtzCap;
	pthread_mutex_lock(&m_mutex);
	tPtzCap.bSptPtz = (m_tProfileInfos[iChnID].tPtzLoginInfo.bSupport==1);
	tPtzCap.bSptAbsMove = (m_tProfileInfos[iChnID].tPtzLoginInfo.bSupportAbsMove==1);
	tPtzCap.bSptClickZoom = true;
	tPtzCap.iMaxPresetNum = MAX_PRESET_ID;
	pthread_mutex_unlock(&m_mutex);
	MQ_Master_SetPtzCap(iChnID, &tPtzCap);

	// 登录成功以后，获取主码流和子码流的RTSP通知MASTER/VIDEOIN
	RtspInfo tMainRtsp = {0};
	RtspInfo tSubRtsp = {0};
	int max = 0,min = 0;
	pthread_mutex_lock(&m_mutex);
	for(int i = 0; i < m_tProfileInfos[iChnID].iStreamCnt; i++)
	{
		if(m_tProfileInfos[iChnID].tStreamInfo[i].width >m_tProfileInfos[iChnID].tStreamInfo[max].width)
			max = i;
		if(m_tProfileInfos[iChnID].tStreamInfo[i].width < m_tProfileInfos[iChnID].tStreamInfo[min].width)
			min = i;
	}
	strcpy(tMainRtsp.szUrl,m_tProfileInfos[iChnID].tStreamInfo[max].URI);
	tMainRtsp.iWidth = m_tProfileInfos[iChnID].tStreamInfo[max].width;
	tMainRtsp.iHeight= m_tProfileInfos[iChnID].tStreamInfo[max].height;
	strcpy(tSubRtsp.szUrl,m_tProfileInfos[iChnID].tStreamInfo[min].URI);
	tSubRtsp.iWidth = m_tProfileInfos[iChnID].tStreamInfo[min].width;
	tSubRtsp.iHeight= m_tProfileInfos[iChnID].tStreamInfo[min].height;
	pthread_mutex_unlock(&m_mutex);
	
	// 发送给VideoIn
	if (m_tDevs[iChnID].iStreamType == STREAM_TYPE_SUB)
	{
		MQ_VideoIn_SetRtspInfo(iChnID, &tSubRtsp);
	}
	else
	{
		MQ_VideoIn_SetRtspInfo(iChnID, &tMainRtsp);
	}

	// 发送给Master
	MQ_Master_SetRtspInfo(iChnID, STREAM_TYPE_MAIN,&tMainRtsp);
	MQ_Master_SetRtspInfo(iChnID, STREAM_TYPE_SUB,&tSubRtsp);

	return 0;
}

int OnvifManager::OnvifClose(int iChnID)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession > 0)
	{
		libOnvif2ControlDelete(m_tProfileInfos[iChnID].hSession);
	}
	
	pthread_mutex_lock(&m_mutex);
	
	m_tProfileInfos[iChnID].hSession = -1;
	memset(&m_tProfileInfos[iChnID].tPtzLoginInfo, 0, sizeof(PTZInfo));
	m_tProfileInfos[iChnID].iStreamCnt = 0;
	memset(&m_tProfileInfos[iChnID].tStreamInfo, 0, MAX_STREAM_NUM*sizeof(StreamInfo));
	m_bDisconnected[iChnID] = true;

	pthread_mutex_unlock(&m_mutex);

	// 关闭以后通知不能用了
	RtspInfo tRtsp = {0};
	MQ_VideoIn_SetRtspInfo(iChnID, &tRtsp);
	MQ_Master_SetRtspInfo(iChnID, STREAM_TYPE_MAIN,&tRtsp);
	MQ_Master_SetRtspInfo(iChnID, STREAM_TYPE_SUB,&tRtsp);

	return 0;
}

int OnvifManager::Onvif_Ptz_Move(int iChnID, int iAction, int iSpeed)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	int iRet = libOnvif2PTZStartMove(m_tProfileInfos[iChnID].hSession, m_tProfileInfos[iChnID].tPtzLoginInfo.ProfileToken, (PTZ_ACTION)iAction, iSpeed);
	if(iRet != 0)
	{
		m_bDisconnected[iChnID] = true;
	}
	return iRet;
}

int OnvifManager::Onvif_Ptz_Stop(int iChnID)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	int iRet = libOnvif2PTZStopMove(m_tProfileInfos[iChnID].hSession, m_tProfileInfos[iChnID].tPtzLoginInfo.ProfileToken);
	if(iRet != 0)
	{
		m_bDisconnected[iChnID] = true;
	}
	return iRet;
}

int OnvifManager::Onvif_Aperture_Move( int iChnID, int /*iAction*/, int /*iSpeed*/ )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	LOG_WARNS_FMT("Aperture is not implemented");
	return 0;
}

int OnvifManager::Onvif_Aperture_Stop( int iChnID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	LOG_WARNS_FMT("Aperture is not implemented");
	return 0;
}

int OnvifManager::Onvif_Focus_Move( int iChnID, int iAction,int iSpeed )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	int iRet = libOnvif2FocusMove(m_tProfileInfos[iChnID].hSession, m_tProfileInfos[iChnID].tPtzLoginInfo.ProfileToken,(FOCUS_ACTION)iAction,iSpeed);
	if(iRet != 0)
	{
		m_bDisconnected[iChnID] = true;
	}
	return iRet;
}

int OnvifManager::Onvif_Focus_Stop( int iChnID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	int iRet = libOnvif2FocusStop(m_tProfileInfos[iChnID].hSession, m_tProfileInfos[iChnID].tPtzLoginInfo.ProfileToken);
	if(iRet != 0)
	{
		m_bDisconnected[iChnID] = true;
	}
	return iRet;
}

int OnvifManager::Onvif_GetPresets( int iChnID, PresetArray *ptPresets )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM || ptPresets == NULL)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	LIST_Preset tPresetList;
	int iRet = libOnvif2GetPresets(m_tProfileInfos[iChnID].hSession, m_tProfileInfos[iChnID].tPtzLoginInfo.ProfileToken, &tPresetList);
	if(iRet != 0)
	{
		m_bDisconnected[iChnID] = true;
	}
	else
	{
		ptPresets->iPresetNum = 0;
		LIST_Preset::iterator iter = tPresetList.begin();
		for(;iter != tPresetList.end() && ptPresets->iPresetNum < MAX_PRESET_NUM; iter++)
		{
			if(strncmp(iter->Name, ONVIF2_PRESET_NAME, strlen(ONVIF2_PRESET_NAME)) == 0)
			{
				int iPresetID = atoi(iter->Name+strlen(ONVIF2_PRESET_NAME));
				if(iPresetID >= MIN_PRESET_ID && iPresetID <= MAX_PRESET_ID)
				{
					ptPresets->arPresets[ptPresets->iPresetNum].iPresetID = iPresetID;
					strcpy(ptPresets->arPresets[ptPresets->iPresetNum].szName, iter->Name);
					ptPresets->iPresetNum++;
				}
			}
		}
	}
	return iRet;
}

int OnvifManager::Onvif_SetPreset( int iChnID, int iPresetID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	int iRet = libOnvif2CreatePreset(m_tProfileInfos[iChnID].hSession, m_tProfileInfos[iChnID].tPtzLoginInfo.ProfileToken,iPresetID);
	if(iRet != 0)
	{
		m_bDisconnected[iChnID] = true;
	}
	return iRet;
}

int OnvifManager::Onvif_DelPreset( int iChnID, int iPresetID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	int iRet = libOnvif2DelPreset(m_tProfileInfos[iChnID].hSession, m_tProfileInfos[iChnID].tPtzLoginInfo.ProfileToken,iPresetID);
	if(iRet != 0)
	{
		m_bDisconnected[iChnID] = true;
	}
	return iRet;
}

int OnvifManager::Onvif_GoToPreset( int iChnID, int iPresetID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	int iRet = libOnvif2GotoPreset(m_tProfileInfos[iChnID].hSession, m_tProfileInfos[iChnID].tPtzLoginInfo.ProfileToken,iPresetID);
	if(iRet != 0)
	{
		m_bDisconnected[iChnID] = true;
	}
	return iRet;
}

int OnvifManager::Onvif_GetAbsPos( int iChnID, AbsPosition &tPos )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}
	
	PTZAbsPosition pos = {0};
	int iRet = libOnvif2PTZGetAbsolutePosition(m_tProfileInfos[iChnID].hSession, m_tProfileInfos[iChnID].tPtzLoginInfo.ProfileToken,&pos);
	if(iRet != 0 && iRet != -2)
	{
		m_bDisconnected[iChnID] = true;
	}
	
	if(pos._Pan == 0 && pos._Tilt == 0 && pos._Zoom == 0 && m_tProfileInfos[iChnID].tPtzLoginInfo.bSupportAbsMove == false)
	{
		LOG_WARNS_FMT("IPC is not Support Abs Move\n");
		return -2;
	}

	tPos.fPan  = pos._Pan;
	tPos.fTilt = pos._Tilt;
	tPos.fZoom = pos._Zoom;
	return iRet;
}

int OnvifManager::Onvif_AbsMove( int iChnID, const AbsPosition* ptPos )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM || ptPos == NULL)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}
	
	PTZAbsPosition pos = {0};
	pos._Pan  = ptPos->fPan;
	pos._Tilt = ptPos->fTilt;
	pos._Zoom = ptPos->fZoom;
	int iRet = libOnvif2PTZAbsoluteMove(m_tProfileInfos[iChnID].hSession, m_tProfileInfos[iChnID].tPtzLoginInfo.ProfileToken,&pos,100,100);
	if(iRet != 0)
	{
		m_bDisconnected[iChnID] = true;
	}
	return iRet;
}

int OnvifManager::Onvif_ClickZoom( int iChnID, const ClickArea * ptArea )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM || ptArea == NULL)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	int width = 0, height = 0;
	if (GetChnSolution(iChnID, width, height) != 0 || width == 0 || height == 0)
	{
		LOG_WARNS_FMT("Haven't solution");
		return -1;
	}

	int iRet = -1;

	// 长宽归一化到1920*1080
	int x0 = ptArea->StartPoint.x * 1920 / width;
	int y0 = ptArea->StartPoint.y * 1080 / height;
	int x1 = ptArea->EndPoint.x	* 1920 / width;
	int y1 = ptArea->EndPoint.y	* 1080 / height;

	if(m_tProxy[iChnID].iModel == CLICK_ZOOM_PROXY)
	{
		Proxy3DPosition::GetInstance()->SetDevInfo(iChnID, m_tDevs[iChnID].szIP, m_tDevs[iChnID].szUser, m_tDevs[iChnID].szPass, m_tDevs[iChnID].iManuCode);
		Proxy3DPosition::GetInstance()->SetProxyInfo(iChnID, m_tProxy[iChnID].szAddr, m_tProxy[iChnID].iPort);
		iRet = Proxy3DPosition::GetInstance()->ClickZoomIn(iChnID, x0, y0, x1, y1);
	}
	else//私有协议
	{
		if(m_tDevs[iChnID].iManuCode == M_HIK)
		{
			Hik3DPosition::GetInstance()->SetDevInfo(iChnID, m_tDevs[iChnID].szIP, m_tDevs[iChnID].szUser, m_tDevs[iChnID].szPass);
			iRet = Hik3DPosition::GetInstance()->ClickZoomIn(iChnID, x0, y0, x1, y1);
		}
		else if(m_tDevs[iChnID].iManuCode == M_DH)
		{
			DaHua3DPosition::GetInstance()->SetDevInfo(iChnID, m_tDevs[iChnID].szIP, m_tDevs[iChnID].szUser, m_tDevs[iChnID].szPass);
			iRet = DaHua3DPosition::GetInstance()->ClickZoomIn(iChnID, x0, y0, x1, y1);
		}
		else if(m_tDevs[iChnID].iManuCode == M_TIANDY)
		{
			Tiandy3DPosition::GetInstance()->SetDevInfo(iChnID, m_tDevs[iChnID].szIP, m_tDevs[iChnID].szUser, m_tDevs[iChnID].szPass);
			iRet = Tiandy3DPosition::GetInstance()->ClickZoomIn(iChnID, x0, y0, x1, y1);
		}
		else if(m_tDevs[iChnID].iManuCode == M_UNIVIEW)
		{
			Uniview3DPosition::GetInstance()->SetDevInfo(iChnID, m_tDevs[iChnID].szIP, m_tDevs[iChnID].szUser, m_tDevs[iChnID].szPass);
			iRet = Uniview3DPosition::GetInstance()->ClickZoomIn(iChnID, x0, y0, x1, y1);
		}
		else
		{
			LOG_WARNS_FMT("not support clickzoom, unknown company");
			iRet = -1;
		}
	}

	return iRet;
}

int OnvifManager::Onvif_GetLocalTime( int iChnID, time_t & tNow )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM )
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_tProfileInfos[iChnID].hSession < 0)
	{
		LOG_WARNS_FMT("Haven't login successfully");
		return -1;
	}

	char szDataTime[128] = {0};
	int iRet = libOnvif2GetDateTime(m_tProfileInfos[iChnID].hSession, szDataTime);
	if(iRet != 0)
	{
		m_bDisconnected[iChnID] = true;
	}

	tNow = String2Time(szDataTime, eYMDHMS1);
	return iRet;
}

