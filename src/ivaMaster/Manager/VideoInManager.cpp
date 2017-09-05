#include "VideoInManager.h"
#include "oal_log.h"
#include "mq_master.h"
#include "../Profile/SystemProfile.h"
#include "../Profile/AnalyProfile.h"

VIManufacturer g_tSupportMan[4] = {
	{M_HIK,		"hikvision", "海康威视"},
	{M_DH,		"dahua",	 "大华科技"},
	{M_TIANDY,	"tiandy",	 "天地伟业"},
	{M_UNIVIEW, "uniview",	 "宇视科技"}
};

VISolution g_tSupportSolution[2]={
	{"1080P", 1920, 1080},
	{"720P",  960, 540}
};

VideoInManager* VideoInManager::m_pInstance = NULL;

VideoInManager::VideoInManager()
{
	pthread_mutex_init(&m_tMutex, NULL);
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		SystemProfile::GetInstance()->GetVideoSourceInfo(i, m_tAllSource+i);

		m_iAllStatus[i] = m_tAllSource[i].bEnable?VIS_ABNORMAL:VIS_NOCFG;

		m_tAllMainRtsp[i].iWidth = 0;
		m_tAllMainRtsp[i].iHeight = 0;
		memset(m_tAllMainRtsp[i].szUrl, 0, sizeof(m_tAllMainRtsp[i].szUrl));

		m_tAllSubRtsp[i].iWidth = 0;
		m_tAllSubRtsp[i].iHeight = 0;
		memset(m_tAllSubRtsp[i].szUrl, 0, sizeof(m_tAllSubRtsp[i].szUrl));

		m_tAllPtzCap[i].bSptPtz = false;
		m_tAllPtzCap[i].bSptAbsMove = false;
		m_tAllPtzCap[i].bSptClickZoom = false;
		m_tAllPtzCap[i].iMaxPresetNum = 0;

		// 发送给ONVIF
		MQ_Onvif_Set_VISource(MSG_TYPE_MASTER, i, m_tAllSource+i);

		RecSrcStat tRecSrc = {0};
		tRecSrc.bEnable = m_tAllSource[i].bEnable;
		tRecSrc.bChanged = false;
		MQ_Record_SourceChange(i, &tRecSrc);
		MQ_VideoOut_SourceChange(i, &tRecSrc);
	}
}

VideoInManager::~VideoInManager()
{
	pthread_mutex_destroy(&m_tMutex);
}

VideoInManager* VideoInManager::GetInstance()
{
	return m_pInstance;
}

int VideoInManager::Initialize()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new VideoInManager();
	}
	return 0;
}

void VideoInManager::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

int VideoInManager::GetChnStatus( int iChnID )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	int iStat = VIS_ABNORMAL;
	pthread_mutex_lock(&m_tMutex);
	iStat = m_iAllStatus[iChnID];
	pthread_mutex_unlock(&m_tMutex);

	return iStat;
}

int VideoInManager::SetChnStatus( int iChnID, int iStatus )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || iStatus < VIS_NOCFG || iStatus > VIS_ABNORMAL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	m_iAllStatus[iChnID] = iStatus;
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int VideoInManager::GetChnVideoSource( int iChnID, VISource & tSource )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&tSource, m_tAllSource+iChnID, sizeof(VISource));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int VideoInManager::SetChnVideoSource( int iChnID, const VISource & tSource )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	bool bChange = false;
	bool bNeedClearAnaly = false;
	pthread_mutex_lock(&m_tMutex);
	if (memcmp(m_tAllSource+iChnID, &tSource, sizeof(VISource))!=0)
	{
		bChange = true;
		if (m_tAllSource[iChnID].iStreamType != tSource.iStreamType ||
			strcmp(m_tAllSource[iChnID].szIP, tSource.szIP)!=0)
		{
			bNeedClearAnaly = true;
		}
		memcpy(m_tAllSource+iChnID, &tSource, sizeof(VISource));

		if (!tSource.bEnable)
		{
			m_iAllStatus[iChnID] = VIS_NOCFG;
			memset(m_tAllMainRtsp+iChnID, 0, sizeof(RtspInfo));
			memset(m_tAllSubRtsp+iChnID, 0, sizeof(RtspInfo));
			memset(m_tAllPtzCap+iChnID, 0, sizeof(PTZCap));
		}
	}
	pthread_mutex_unlock(&m_tMutex);

	if (bChange)
	{
		LOG_INFOS_FMT("Channel%d Video Source is change", iChnID);

		// 保存配置文件
		int iRet = SystemProfile::GetInstance()->SetVideoSourceInfo(iChnID,&tSource);
		if (iRet != 0)
		{
			LOG_ERROR_FMT("Save Profile failed.");
			return -1;
		}

		if (bNeedClearAnaly)
		{
			SystemProfile::GetInstance()->DeleteChnSceneInfo(iChnID);
			AnalyProfile::GetInstance()->DeleteChn(iChnID);

			MQ_Analy_Chn_ClearAnaly(iChnID);
		}

		// 发送到接入模块
		MQ_Onvif_Set_VISource(MSG_TYPE_MASTER, iChnID, &tSource);

		RecSrcStat tRecSrc = {0};
		tRecSrc.bEnable = tSource.bEnable;
		tRecSrc.bChanged = true;
		MQ_Record_SourceChange(iChnID, &tRecSrc);
		MQ_VideoOut_SourceChange(iChnID, &tRecSrc);
	}

	return 0;
}

int VideoInManager::AddChnVideoSource( const VISource & tSource )
{
	int iChnID = -1;

	pthread_mutex_lock(&m_tMutex);
	for(int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		if (m_tAllSource[i].bEnable == false)
		{
			iChnID = i;
			break;
		}
	}
	if (iChnID == -1)
	{
		LOG_WARNS_FMT("No free channel to use");
	}
	else
	{
		memcpy(m_tAllSource+iChnID, &tSource, sizeof(VISource));
	}
	pthread_mutex_unlock(&m_tMutex);

	if (iChnID != -1)
	{
		LOG_INFOS_FMT("Channel%d Video Source is change", iChnID);

		// 保存配置文件
		int iRet = SystemProfile::GetInstance()->SetVideoSourceInfo(iChnID,&tSource);
		if (iRet != 0)
		{
			LOG_ERROR_FMT("Save Profile failed.");
			return -1;
		}

		// 发送到接入模块
		MQ_Onvif_Set_VISource(MSG_TYPE_MASTER, iChnID, &tSource);

		RecSrcStat tRecSrc = {0};
		tRecSrc.bEnable = tSource.bEnable;
		tRecSrc.bChanged = true;
		MQ_Record_SourceChange(iChnID, &tRecSrc);
		MQ_VideoOut_SourceChange(iChnID, &tRecSrc);
	}

	return 0;
}

int VideoInManager::DisableChnVideoSource( int iChnID )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	if (m_tAllSource[iChnID].bEnable)
	{
		LOG_INFOS_FMT("Channel%d Video Source is Disable", iChnID);
		m_tAllSource[iChnID].bEnable = false;	
		m_iAllStatus[iChnID] = VIS_NOCFG;

		// 保存配置文件
		int iRet = SystemProfile::GetInstance()->SetVideoSourceInfo(iChnID,&m_tAllSource[iChnID]);
		if (iRet != 0)
		{
			LOG_ERROR_FMT("Save Profile failed.");
			pthread_mutex_unlock(&m_tMutex);
			return -1;
		}

		// 发送到接入模块
		MQ_Onvif_Set_VISource(MSG_TYPE_MASTER, iChnID, &m_tAllSource[iChnID]);

		RecSrcStat tRecSrc = {0};
		tRecSrc.bEnable = m_tAllSource[iChnID].bEnable;
		tRecSrc.bChanged = true;
		MQ_Record_SourceChange(iChnID, &tRecSrc);
		MQ_VideoOut_SourceChange(iChnID, &tRecSrc);
	}
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int VideoInManager::GetChnRtsp( int iChnID, int iStreamType, RtspInfo & tRtsp )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	if (iStreamType == STREAM_TYPE_MAIN)
	{
		pthread_mutex_lock(&m_tMutex);
		memcpy(&tRtsp, m_tAllMainRtsp+iChnID, sizeof(RtspInfo));
		pthread_mutex_unlock(&m_tMutex);
		return 0;
	}
	else if (iStreamType == STREAM_TYPE_SUB)
	{
		pthread_mutex_lock(&m_tMutex);
		memcpy(&tRtsp, m_tAllSubRtsp+iChnID, sizeof(RtspInfo));
		pthread_mutex_unlock(&m_tMutex);
		return 0;
	}
	else
	{
		return -1;
	}
}

int VideoInManager::SetChnRtsp( int iChnID, int iStreamType, const RtspInfo * ptRtsp )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || ptRtsp == NULL)
	{
		return -1;
	}

	if (iStreamType == STREAM_TYPE_MAIN)
	{
		pthread_mutex_lock(&m_tMutex);
		memcpy(m_tAllMainRtsp+iChnID, ptRtsp, sizeof(RtspInfo));
		pthread_mutex_unlock(&m_tMutex);
		return 0;
	}
	else if (iStreamType == STREAM_TYPE_SUB)
	{
		pthread_mutex_lock(&m_tMutex);
		memcpy(m_tAllSubRtsp+iChnID, ptRtsp, sizeof(RtspInfo));
		pthread_mutex_unlock(&m_tMutex);
		return 0;
	}
	else
	{
		return -1;
	}
}

int VideoInManager::GetChnPtzCap( int iChnID, PTZCap & tPtzCap )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&tPtzCap, m_tAllPtzCap+iChnID, sizeof(PTZCap));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int VideoInManager::SetChnPtzCap( int iChnID, const PTZCap * ptPtzCap )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(m_tAllPtzCap+iChnID, ptPtzCap, sizeof(PTZCap));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}
