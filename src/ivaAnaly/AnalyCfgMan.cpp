/**************************************************************************
# Copyright : JAYA	
# Name	    : algcfgman.cpp
# Describe  : 算法分析
# Date      : 12/15/2015
# Author    : libo
**************************************************************************/
#include <stdio.h>
#include <string.h> 
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "AnalyCfgMan.h"
#include "oal_log.h"
#include "oal_unicode.h"
#include "mq_database.h"
#include "mq_upload.h"

char g_HANZI_LIST[31][3] = {
	"京","新","蒙","云","豫","辽","贵",
	"津","闽","冀","晋","吉","黑","沪",
	"苏","浙","皖","赣","鲁","鄂","湘",
	"粤","桂","琼","川","藏","陕","甘",
	"青","宁","渝"
};

AnalyCfgMan* AnalyCfgMan::m_pInstance = NULL;

AnalyCfgMan* AnalyCfgMan::GetInstance()
{
	return m_pInstance;
}

int AnalyCfgMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new AnalyCfgMan();
		assert(m_pInstance);
	}
	return (m_pInstance == NULL ? -1 : 0);
}


void AnalyCfgMan::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

AnalyCfgMan::AnalyCfgMan()
{
	pthread_mutex_init(&m_tMutex, NULL);

	//pthread_mutex_lock(&m_tMutex);
	m_tAuthStat.eAuthType = eProhibit;
	m_tAuthStat.tTimeOut  = 0;
	memset(&m_tDevInfo, 0, sizeof(m_tDevInfo));
	strcpy(m_szRootPath, DEFAULT_ROOT_PATH);
	m_tStoreFull.bFull = false;
	m_tStoreFull.iHow2DO = 0;
	m_tLprCfg.iDefProv = eHuBei;
	m_tLprCfg.iHanziConf = 90;
	m_tLprCfg.iMinSize = 130;
	m_tLprCfg.iDupTime = 0;
	m_tLprCfg.iDupSens = 4;
	m_tDispatchCtrl.iSensitivity = eSenCompromise;
	m_tDispatchCtrl.iPriority = ePriByScene;
	m_tDispatchCtrl.iSceneMinStay = 30;
	m_tDispatchCtrl.iRepeatTime = 600;
	m_tDispatchCtrl.bPtzLock = false;
	m_tDispatchCtrl.iLockTime = 0;
	
	memset(&m_tChnInfo, 0, MAX_CHANNEL_NUM*sizeof(AlgChannelCfg));
	for(int iChnID = 0; iChnID <  MAX_CHANNEL_NUM; iChnID++)
	{
		m_tChnInfo[iChnID].bOpenAlg = false;
		m_tChnInfo[iChnID].bUsePlan = false;
		m_tChnInfo[iChnID].tSceneCfg[MAX_SCENE_NUM];
		ClearChnRuleAnaly(iChnID);
	}

	for(int e = 0; e < eIT_Event_Max; e++)
	{
		m_tEventCfg[e].tDetermine.iValue1 = 2;
		m_tEventCfg[e].tEviModel.bEnableEvent = false;
		m_tEventCfg[e].tEviModel.iModel = 1;
		m_tEventCfg[e].tImgCom.iComModel2 = eNotCom;
		m_tEventCfg[e].tImgCom.iComModel3 = eNotCom;
		m_tEventCfg[e].tImgCom.iComModel4 = eMatCom;
		m_tEventCfg[e].tImgOsd.iFontSize = 32;
		m_tEventCfg[e].tImgOsd.tFontColor.r = 0xFF;
		m_tEventCfg[e].tImgQua.iSizeLimit = 500;
		m_tEventCfg[e].tImgQua.iWidthLimit = 0;
		m_tEventCfg[e].tImgQua.iHeightLimit = 0;
		m_tEventCfg[e].tRecCfg.bEnable = true;
		m_tEventCfg[e].tRecCfg.iRecModel = eRecJoint;
		m_tEventCfg[e].tRecCfg.iFileFormat = REC_FILE_MP4;
		m_tEventCfg[e].tRecCfg.iMinTime = 5;
		strcpy(m_tEventCfg[e].tNaming.szIllegalImg,"%IP%_%YYYY%-%MM%-%DD%_%HH%-%mm%-%SS%-%XXX%");
		strcpy(m_tEventCfg[e].tNaming.szIllegalRec,"%IP%_%YYYY%-%MM%-%DD%_%HH%-%mm%-%SS%-%XXX%");
		strcpy(m_tEventCfg[e].tNaming.szEventImg,"%IP%_%YYYY%-%MM%-%DD%_%HH%-%mm%-%SS%-%XXX%_notify");
		strcpy(m_tEventCfg[e].tNaming.szEventRec,"%IP%_%YYYY%-%MM%-%DD%_%HH%-%mm%-%SS%-%XXX%_notify");
	}

	//pthread_mutex_unlock(&m_tMutex);

	m_pPlate1History = NULL;
	m_pPlate2History = NULL;
	pthread_mutex_init(&m_tPlateHistoryMutex,NULL);
}

AnalyCfgMan::~AnalyCfgMan()
{
	pthread_mutex_destroy(&m_tMutex);

	pthread_mutex_lock(&m_tPlateHistoryMutex);
	while(m_pPlate1History)
	{
		PLATE_BUFFER * p = m_pPlate1History;
		m_pPlate1History = m_pPlate1History->next;

		free(p);
		p = NULL;
	}
	while(m_pPlate2History)
	{
		PLATE_BUFFER * p = m_pPlate2History;
		m_pPlate2History = m_pPlate2History->next;

		free(p);
		p = NULL;
	}

	for(Preset2PosMap::iterator it = m_tPresetPosMap.begin();it != m_tPresetPosMap.end();it++)
	{
		AbsPosition* ptPos = it->second;

		if(ptPos != NULL)
		{
			delete ptPos;
			ptPos = NULL;
		}
	}

	m_tPresetPosMap.clear();
	pthread_mutex_unlock(&m_tPlateHistoryMutex);
	pthread_mutex_destroy(&m_tPlateHistoryMutex);
}

int AnalyCfgMan::SetAuthInfo( const AuthStatusT* ptInfo )
{
	if(ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tAuthStat, ptInfo, sizeof(AuthStatusT));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int AnalyCfgMan::SetDevInfo(const DeviceInfoT* ptInfo)
{
	if(ptInfo == NULL)
	{
		return -1;
	}
	
	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tDevInfo, ptInfo, sizeof(DeviceInfoT));
	pthread_mutex_unlock(&m_tMutex);
	
	return 0;
}

int AnalyCfgMan::SetRootPath( const char * pszRootPath )
{
	if(pszRootPath == NULL || strlen(pszRootPath) < 1)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	strcpy(m_szRootPath, pszRootPath);
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetStoreFull( const StoreFullStatusT * ptInfo )
{
	if(ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tStoreFull, ptInfo, sizeof(StoreFullStatusT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetLprCfg( const LPRCfg *ptInfo )
{
	if(ptInfo == NULL)
	{
		return -1;
	}

	bool bChanged = false;

	pthread_mutex_lock(&m_tMutex);
	bChanged = (m_tLprCfg.iDupTime != ptInfo->iDupTime)?true:false;
	memcpy(&m_tLprCfg, ptInfo, sizeof(LPRCfg));
	pthread_mutex_unlock(&m_tMutex);

	if(bChanged)
	{
		InitPlateFilter(true);
		InitPlateFilter(false);
	}

	return 0;
}

int AnalyCfgMan::SetDispatchCtrl( const DispatchCtrlT *ptInfo )
{
	if(ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tDispatchCtrl, ptInfo, sizeof(DispatchCtrlT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetChnAlgOpen(int iChnID, bool bOpen)
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	m_tChnInfo[iChnID].bOpenAlg = bOpen;
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetChnPlaceInfoT( int iChnID, const PlaceInfoT * ptInfo )
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tChnInfo[iChnID].tChnPlaceInfo, ptInfo, sizeof(PlaceInfoT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetChnPlan( int iChnID, const AnalyPlan *ptInfo )
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tChnInfo[iChnID].tRunPlan, ptInfo, sizeof(AnalyPlan));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetChnPlanOpen(int iChnID,bool bOpen)
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}
	pthread_mutex_lock(&m_tMutex);
	m_tChnInfo[iChnID].bUsePlan = bOpen;
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetChnNotifyOpen( int iChnID, bool bOpen )
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}
	pthread_mutex_lock(&m_tMutex);
	m_tChnInfo[iChnID].bUseNotify = bOpen;
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetChnPresetPos( int iChnID, int iPreset, const AbsPosition * ptInfo )
{
	if( iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || 
		iPreset < 0 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	AbsPosition* ptPos = m_tPresetPosMap[iPreset];
	if(ptPos == NULL)
	{
		ptPos = new AbsPosition;
		if(ptPos ==NULL)
		{
			printf("Out of memory\n");
			pthread_mutex_unlock(&m_tMutex);
			return -1;
		}

		m_tPresetPosMap[iPreset] = ptPos;
	}

	ptPos->fPan = ptInfo->fPan;
	ptPos->fTilt = ptInfo->fTilt;
	ptPos->fZoom = ptInfo->fZoom;
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetChnSceneInfo( int iChnID, int iScene, const SceneInfoT *ptInfo )
{
	if( iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || 
		iScene < 0 || iScene > MAX_SCENE_NUM-1 || 
		ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tChnInfo[iChnID].tSceneCfg[iScene].tInfo, ptInfo, sizeof(SceneInfoT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetChnSceneRule( int iChnID, int iScene, const AnalyRuleT *ptInfo )
{
	if( iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || 
		iScene < 0 || iScene > MAX_SCENE_NUM-1 || 
		ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tChnInfo[iChnID].tSceneCfg[iScene].tRule, ptInfo, sizeof(AnalyRuleT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetEventDetermine( int iEvent, const DetermineT* ptInfo )
{
	if( iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tEventCfg[iEvent].tDetermine, ptInfo, sizeof(DetermineT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetEventEvidenceModel( int iEvent, const EvidenceModelT* ptInfo )
{
	if( iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tEventCfg[iEvent].tEviModel, ptInfo, sizeof(EvidenceModelT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetEventImgMakeup( int iEvent, int iEviModel, const ImgMakeupT* ptInfo )
{
	if( iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	if (iEviModel == 0)
	{
		memcpy(&m_tEventCfg[iEvent].tImgMakeup1, ptInfo, sizeof(ImgMakeupT));
	}
	else if (iEviModel == 1)
	{
		memcpy(&m_tEventCfg[iEvent].tImgMakeup2, ptInfo, sizeof(ImgMakeupT));
	}
	pthread_mutex_unlock(&m_tMutex);

	return 0;

}

int AnalyCfgMan::SetEventImgCompose( int iEvent, const ImgComposeT* ptInfo )
{
	if( iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tEventCfg[iEvent].tImgCom, ptInfo, sizeof(ImgComposeT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetEventImgOSD( int iEvent, const ImgOSDT* ptInfo )
{
	if( iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tEventCfg[iEvent].tImgOsd, ptInfo, sizeof(ImgOSDT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetEventImgQuality( int iEvent, const ImgQualityT* ptInfo )
{
	if( iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tEventCfg[iEvent].tImgQua, ptInfo, sizeof(ImgQualityT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetEventRecEvidence( int iEvent, const RecEvidenceT* ptInfo )
{
	if( iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tEventCfg[iEvent].tRecCfg, ptInfo, sizeof(RecEvidenceT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetEventCode( int iEvent, const EventCodeT* ptInfo )
{
	if( iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tEventCfg[iEvent].tCode, ptInfo, sizeof(EventCodeT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::SetEventFileNaming( int iEvent, const FileNamingT * ptInfo )
{
	if( iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tEventCfg[iEvent].tNaming, ptInfo, sizeof(FileNamingT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::ClearChnRuleAnaly( int iChnID )
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	for(int i = 0; i < MAX_SCENE_NUM; i++)
	{
		memset(&m_tChnInfo[iChnID].tSceneCfg[i], 0, sizeof(AlgSceneCfg));
		m_tChnInfo[iChnID].tSceneCfg[i].tInfo.iPresetID = -1;
		m_tChnInfo[iChnID].tSceneCfg[i].tInfo.iStayTime = 3600;
		sprintf(m_tChnInfo[iChnID].tSceneCfg[i].tInfo.szSceneName,"Scene%d",i+1);
	}

	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

bool AnalyCfgMan::IsAuthorised()
{
	if(m_tAuthStat.eAuthType == eProhibit)
	{
		return false;
	}
	else if(m_tAuthStat.eAuthType == ePermanent)
	{
		return true;
	}
	else if(m_tAuthStat.eAuthType == eTemporary)
	{
		if(time(NULL) > m_tAuthStat.tTimeOut)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool AnalyCfgMan::ChnAlgIsOpen(int iChnID)
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return false;
	}
	bool bOpen = false;
	pthread_mutex_lock(&m_tMutex);
	bOpen = m_tChnInfo[iChnID].bOpenAlg;
	pthread_mutex_unlock(&m_tMutex);
	return bOpen;
}

bool AnalyCfgMan::ChnPlanIsOpen(int iChnID)
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return false;
	}
	bool bPlanUse = false;
	pthread_mutex_lock(&m_tMutex);
	bPlanUse = m_tChnInfo[iChnID].bUsePlan;
	pthread_mutex_unlock(&m_tMutex);
	return bPlanUse;
}

bool AnalyCfgMan::ChnNotifyIsOpen(int iChnID)
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return false;
	}
	bool bNotifyUse = false;
	pthread_mutex_lock(&m_tMutex);
	bNotifyUse = m_tChnInfo[iChnID].bUseNotify;
	pthread_mutex_unlock(&m_tMutex);
	return bNotifyUse;
}

bool AnalyCfgMan::IsNeedStopAnalyForFull()
{
	bool bNeed = false;
	pthread_mutex_lock(&m_tMutex);
	bNeed = (m_tStoreFull.bFull && m_tStoreFull.iHow2DO == eFull2StopAnaly);
	pthread_mutex_unlock(&m_tMutex);

	return bNeed;
}

bool AnalyCfgMan::IsNeedStopWriteForFull()
{
	bool bNeed = false;
	pthread_mutex_lock(&m_tMutex);
	bNeed = (m_tStoreFull.bFull && m_tStoreFull.iHow2DO == eFull2StopSave);
	pthread_mutex_unlock(&m_tMutex);

	return bNeed;
}

int AnalyCfgMan::GetDevInfo(DeviceInfoT* ptInfo)
{
	if(ptInfo == NULL)
	{
		return -1;
	}
	
	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tDevInfo,sizeof(DeviceInfoT));
	pthread_mutex_unlock(&m_tMutex);
	
	return 0;
}

const char * AnalyCfgMan::GetRootPath()
{
	return m_szRootPath;
}

int AnalyCfgMan::GetLprCfg( LPRCfg * ptInfo )
{
	if(ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo, &m_tLprCfg, sizeof(LPRCfg));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetDispatchCtrl( DispatchCtrlT * ptInfo )
{
	if(ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tDispatchCtrl,sizeof(DispatchCtrlT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::ChnIsInPlan(int iChnID,int iWeekDay, int iTime)
{
	int whichone = -1;
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || iWeekDay < 0 || iWeekDay > 6)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);

	TIME_SLOT *pSlots = NULL;
	switch (iWeekDay)
	{
	case 0:
		pSlots = m_tChnInfo[iChnID].tRunPlan.tSun;
		break;
	case 1:
		pSlots = m_tChnInfo[iChnID].tRunPlan.tMon;
		break;
	case 2:
		pSlots = m_tChnInfo[iChnID].tRunPlan.tTue;
		break;
	case 3:
		pSlots = m_tChnInfo[iChnID].tRunPlan.tWed;
		break;
	case 4:
		pSlots = m_tChnInfo[iChnID].tRunPlan.tThu;
		break;
	case 5:
		pSlots = m_tChnInfo[iChnID].tRunPlan.tFri;
		break;
	case 6:
		pSlots = m_tChnInfo[iChnID].tRunPlan.tSat;
		break;
	default:
		break;
	}
	if (pSlots)
	{
		for(int i = 0; i < MAX_ANALY_PLAN_NUM; i++)
		{
			if( pSlots[i].iStartTime <= iTime && iTime <= pSlots[i].iEndTime &&
				(pSlots[i].iStartTime != 0 || iTime <= pSlots[i].iEndTime != 0))
			{
				whichone = i;
				break;
			}
		}
	}
	
	pthread_mutex_unlock(&m_tMutex);
	
	return whichone;
}

int AnalyCfgMan::GetChnPlaceInfoT( int iChnID, PlaceInfoT * ptInfo )
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tChnInfo[iChnID].tChnPlaceInfo,sizeof(PlaceInfoT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetChnSceneInfo( int iChnID, int iScene, SceneInfoT * ptInfo )
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || iScene < 0 || iChnID > MAX_SCENE_NUM-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tChnInfo[iChnID].tSceneCfg[iScene].tInfo,sizeof(SceneInfoT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetChnSceneRule( int iChnID, int iScene, AnalyRuleT * ptInfo )
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || iScene < 0 || iChnID > MAX_SCENE_NUM-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tChnInfo[iChnID].tSceneCfg[iScene].tRule,sizeof(AnalyRuleT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetEventDetermine( int iEvent, DetermineT * ptInfo )
{
	if(iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tEventCfg[iEvent].tDetermine,sizeof(DetermineT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetEventEvidenceModel( int iEvent, EvidenceModelT * ptInfo )
{
	if(iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tEventCfg[iEvent].tEviModel,sizeof(EvidenceModelT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetEventImgMakeup( int iEvent, int iEviModel, ImgMakeupT * ptInfo )
{
	if(iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	if (iEviModel == 0)
	{
		memcpy(ptInfo, &m_tEventCfg[iEvent].tImgMakeup1, sizeof(ImgMakeupT));
	}
	else if (iEviModel == 1)
	{
		memcpy(ptInfo, &m_tEventCfg[iEvent].tImgMakeup2, sizeof(ImgMakeupT));
	}
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetEventImgCompose( int iEvent, ImgComposeT * ptInfo )
{
	if(iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tEventCfg[iEvent].tImgCom,sizeof(ImgComposeT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetEventImgOSD( int iEvent, ImgOSDT * ptInfo )
{
	if(iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tEventCfg[iEvent].tImgOsd,sizeof(ImgOSDT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetEventImgQuality( int iEvent, ImgQualityT * ptInfo )
{
	if(iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tEventCfg[iEvent].tImgQua,sizeof(ImgQualityT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetEventRecEvidence( int iEvent, RecEvidenceT * ptInfo )
{
	if(iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tEventCfg[iEvent].tRecCfg,sizeof(RecEvidenceT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetEventCode( int iEvent, EventCodeT * ptInfo )
{
	if(iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tEventCfg[iEvent].tCode,sizeof(EventCodeT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetEventFileNaming( int iEvent, FileNamingT * ptInfo )
{
	if(iEvent < 0 || iEvent > eIT_Event_Max-1 || ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptInfo,&m_tEventCfg[iEvent].tNaming,sizeof(FileNamingT));
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetIllegeStopTime()
{
	return m_tEventCfg[eIT_Illegal_Park].tDetermine.iValue1;
}

int AnalyCfgMan::GetChnSceneIdByPreset(int iChnID, int iPresetID)
{
	int sceneno = 0;

	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return 0;
	}

	pthread_mutex_lock(&m_tMutex);

	for(int i = 0; i <  MAX_SCENE_NUM; i++)
	{
		if(m_tChnInfo[iChnID].tSceneCfg[i].tInfo.iPresetID == iPresetID)
		{
			sceneno = i;
			break;
		}
	}
	pthread_mutex_unlock(&m_tMutex);

	return sceneno;
}

int AnalyCfgMan::GetChnScenePresetCfg(int iChnID, int iSceneID, tSinglePresetConf* ptCfg)
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM - 1 ||
		iSceneID < 0 || iSceneID > MAX_SCENE_NUM - 1 || ptCfg == NULL)
	{
		return -1;
	}

	if(iSceneID < 0 || iSceneID > MAX_SCENE_NUM-1)
	{
	
		return -1;// 没有执行的场景
	}

	memset(ptCfg,0,sizeof(tSinglePresetConf));

	pthread_mutex_lock(&m_tMutex);
	AlgSceneCfg * ptScene = &(m_tChnInfo[iChnID].tSceneCfg[iSceneID]);
	
	ptCfg->u32ChannelID      = iChnID;
	ptCfg->s32PresetId		 = ptScene->tInfo.iPresetID;
	ptCfg->u32FuncFlag       = USE_PARK;
	ptCfg->fClickZoomInRate  = 1.0f;
	ptCfg->fClickZoomDistance= 1.0f;
	if (ptScene->tRule.tTargetSize.bEnable)
	{
		int w1 = 0, w2 = 0;
		if (ptScene->tRule.tTargetSize.atMinPoints[0].x > ptScene->tRule.tTargetSize.atMinPoints[1].x)
			w1 = ptScene->tRule.tTargetSize.atMinPoints[0].x - ptScene->tRule.tTargetSize.atMinPoints[1].x;
		else
			w1 = ptScene->tRule.tTargetSize.atMinPoints[1].x - ptScene->tRule.tTargetSize.atMinPoints[0].x;

		if (ptScene->tRule.tTargetSize.atMaxPoints[0].x > ptScene->tRule.tTargetSize.atMaxPoints[1].x)
			w2 = ptScene->tRule.tTargetSize.atMaxPoints[0].x - ptScene->tRule.tTargetSize.atMaxPoints[1].x;
		else
			w2 = ptScene->tRule.tTargetSize.atMaxPoints[1].x - ptScene->tRule.tTargetSize.atMaxPoints[0].x;
			
		if (w1 > w2){
			int w=w1;w1=w2;w2=w;
		}

		ptCfg->fMaxObjSize   = (float)w1;
		ptCfg->fMinObjSize   = (float)w2;
	}
	else
	{
		ptCfg->fMaxObjSize   = 0.0f;
		ptCfg->fMinObjSize   = 0.0f;

	}
	ptCfg->tAbsPtz.pan       = 0.0f;
	ptCfg->tAbsPtz.tilt		 = 0.0f;
	ptCfg->tAbsPtz.zoom		 = 0.0f;
	ptCfg->tAbsPtz.focallen	 = 0.0f;
	
	ptCfg->u32IllegeStopTime = GetIllegeStopTime();
	ptCfg->fReverseDriveDis  = 0;
	ptCfg->fIllegeOccupyDis  = 0;
	ptCfg->u32UTurnAngle     = 0;
	ptCfg->u32PressLineTime  = 0;
	ptCfg->fPressLineDis     = 0;
		
	ptCfg->tSheildConf.u32AreaCount = ptScene->tRule.iSheildAreaNum;
	for(int i = 0; i < ptCfg->tSheildConf.u32AreaCount; i++)
	{
		ptCfg->tSheildConf.stArea[i].u32PointNum = ptScene->tRule.atSheildAreas[i].iPtNum;
		for(int j = 0; j < ptCfg->tSheildConf.stArea[i].u32PointNum; j++)
		{
			ptCfg->tSheildConf.stArea[i].stPoint[j].x = ptScene->tRule.atSheildAreas[i].atPoints[j].x;
			ptCfg->tSheildConf.stArea[i].stPoint[j].y = ptScene->tRule.atSheildAreas[i].atPoints[j].y;
		}
	}

	ptCfg->tCalibConf.fActualHeight = ptScene->tRule.tCalib.length;
	ptCfg->tCalibConf.fActualWidth  = ptScene->tRule.tCalib.width;
	for(int i = 0; i < 4; i++)
	{
		ptCfg->tCalibConf.tPtsInImage[i].x = ptScene->tRule.tCalib.atPoints[i].x;
		ptCfg->tCalibConf.tPtsInImage[i].y = ptScene->tRule.tCalib.atPoints[i].y;
	}

	ptCfg->tLaneConf.u32LaneNum = 0;
	ptCfg->tSolidConf.u32LineNum = 0;
	ptCfg->tUTurnConf.SingleUTurnNum = 0;
	ptCfg->LaneChangeConf.SingleUTurnNum = 0;
	ptCfg->tParkArea.u32AreaCount = ptScene->tRule.iAnalyAreaNum;
	for(int i = 0; i < ptCfg->tParkArea.u32AreaCount; i++)
	{
		ptCfg->tParkArea.stArea[i].u32PointNum = ptScene->tRule.atAnalyAreas[i].iPtNum;
		for(int j = 0; j < ptCfg->tParkArea.stArea[i].u32PointNum; j++)
		{
			ptCfg->tParkArea.stArea[i].stPoint[j].x = ptScene->tRule.atAnalyAreas[i].atPoints[j].x;
			ptCfg->tParkArea.stArea[i].stPoint[j].y = ptScene->tRule.atAnalyAreas[i].atPoints[j].y;
		}
	}
	
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int AnalyCfgMan::GetChnAllPresetCfg( int iChnID, ValidPresetSet* ptCfg )
{
	if( iChnID < 0 || iChnID > MAX_CHANNEL_NUM - 1 || ptCfg == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);

	int index = 0;
	ptCfg->iPresetNum = 0;
	for(int i = 0; i < MAX_SCENE_NUM; i++)
	{
		if(m_tChnInfo[iChnID].tSceneCfg[i].tInfo.iPresetID > 0 && m_tChnInfo[iChnID].tSceneCfg[i].tInfo.iStayTime > 0)
		{
			AlgSceneCfg * ptScene = &(m_tChnInfo[iChnID].tSceneCfg[i]);

			ptCfg->SinglePreset[index].u32ChannelID      = iChnID;
			ptCfg->SinglePreset[index].s32PresetId		 = ptScene->tInfo.iPresetID;
			ptCfg->SinglePreset[index].u32FuncFlag       = USE_PARK;
			ptCfg->SinglePreset[index].fClickZoomInRate  = 1.0f;
			ptCfg->SinglePreset[index].fClickZoomDistance= 1.0f;
			if (ptScene->tRule.tTargetSize.bEnable)
			{
				int w1 = 0, w2 = 0;
				if (ptScene->tRule.tTargetSize.atMinPoints[0].x > ptScene->tRule.tTargetSize.atMinPoints[1].x)
					w1 = ptScene->tRule.tTargetSize.atMinPoints[0].x - ptScene->tRule.tTargetSize.atMinPoints[1].x;
				else
					w1 = ptScene->tRule.tTargetSize.atMinPoints[1].x - ptScene->tRule.tTargetSize.atMinPoints[0].x;

				if (ptScene->tRule.tTargetSize.atMaxPoints[0].x > ptScene->tRule.tTargetSize.atMaxPoints[1].x)
					w2 = ptScene->tRule.tTargetSize.atMaxPoints[0].x - ptScene->tRule.tTargetSize.atMaxPoints[1].x;
				else
					w2 = ptScene->tRule.tTargetSize.atMaxPoints[1].x - ptScene->tRule.tTargetSize.atMaxPoints[0].x;

				if (w1 > w2){
					int w=w1;w1=w2;w2=w;
				}

				ptCfg->SinglePreset[index].fMaxObjSize   = (float)w1;
				ptCfg->SinglePreset[index].fMinObjSize   = (float)w2;
			}
			else
			{
				ptCfg->SinglePreset[index].fMaxObjSize   = 0.0f;
				ptCfg->SinglePreset[index].fMinObjSize   = 0.0f;
			}
			ptCfg->SinglePreset[index].tAbsPtz.pan       = 0.0f;
			ptCfg->SinglePreset[index].tAbsPtz.tilt		 = 0.0f;
			ptCfg->SinglePreset[index].tAbsPtz.zoom		 = 0.0f;
			ptCfg->SinglePreset[index].tAbsPtz.focallen	 = 0.0f;

			ptCfg->SinglePreset[index].u32IllegeStopTime = GetIllegeStopTime();
			ptCfg->SinglePreset[index].fReverseDriveDis  = 0;
			ptCfg->SinglePreset[index].fIllegeOccupyDis  = 0;
			ptCfg->SinglePreset[index].u32UTurnAngle     = 0;
			ptCfg->SinglePreset[index].u32PressLineTime  = 0;
			ptCfg->SinglePreset[index].fPressLineDis     = 0;

			ptCfg->SinglePreset[index].tSheildConf.u32AreaCount = ptScene->tRule.iSheildAreaNum;
			for(int i = 0; i < ptCfg->SinglePreset[index].tSheildConf.u32AreaCount; i++)
			{
				ptCfg->SinglePreset[index].tSheildConf.stArea[i].u32PointNum = ptScene->tRule.atSheildAreas[i].iPtNum;
				for(int j = 0; j < ptCfg->SinglePreset[index].tSheildConf.stArea[i].u32PointNum; j++)
				{
					ptCfg->SinglePreset[index].tSheildConf.stArea[i].stPoint[j].x = ptScene->tRule.atSheildAreas[i].atPoints[j].x;
					ptCfg->SinglePreset[index].tSheildConf.stArea[i].stPoint[j].y = ptScene->tRule.atSheildAreas[i].atPoints[j].y;
				}
			}

			ptCfg->SinglePreset[index].tCalibConf.fActualHeight = ptScene->tRule.tCalib.length;
			ptCfg->SinglePreset[index].tCalibConf.fActualWidth  = ptScene->tRule.tCalib.width;
			for(int i = 0; i < 4; i++)
			{
				ptCfg->SinglePreset[index].tCalibConf.tPtsInImage[i].x = ptScene->tRule.tCalib.atPoints[i].x;
				ptCfg->SinglePreset[index].tCalibConf.tPtsInImage[i].y = ptScene->tRule.tCalib.atPoints[i].y;
			}

			ptCfg->SinglePreset[index].tLaneConf.u32LaneNum = 0;
			ptCfg->SinglePreset[index].tSolidConf.u32LineNum = 0;
			ptCfg->SinglePreset[index].tUTurnConf.SingleUTurnNum = 0;
			ptCfg->SinglePreset[index].LaneChangeConf.SingleUTurnNum = 0;
			ptCfg->SinglePreset[index].tParkArea.u32AreaCount = ptScene->tRule.iAnalyAreaNum;
			for(int i = 0; i < ptCfg->SinglePreset[index].tParkArea.u32AreaCount; i++)
			{
				ptCfg->SinglePreset[index].tParkArea.stArea[i].u32PointNum = ptScene->tRule.atAnalyAreas[i].iPtNum;
				for(int j = 0; j < ptCfg->SinglePreset[index].tParkArea.stArea[i].u32PointNum; j++)
				{
					ptCfg->SinglePreset[index].tParkArea.stArea[i].stPoint[j].x = ptScene->tRule.atAnalyAreas[i].atPoints[j].x;
					ptCfg->SinglePreset[index].tParkArea.stArea[i].stPoint[j].y = ptScene->tRule.atAnalyAreas[i].atPoints[j].y;
				}
			}

			ptCfg->iPresetNum++;
			index++;
		}
	}

	pthread_mutex_unlock(&m_tMutex);

	return 0;
}

int AnalyCfgMan::GetPresetPTZ( int iChnID, int iPresetID, AbsPosition *ptInfo )
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || iPresetID < 0)
	{
		return -1;
	}

	int iRet = -1;
	pthread_mutex_lock(&m_tMutex);
	AbsPosition * ptPos = m_tPresetPosMap[iPresetID];
	if (ptPos)
	{
		iRet = 0;
		ptInfo->fPan = ptPos->fPan;
		ptInfo->fTilt = ptPos->fTilt;
		ptInfo->fZoom = ptPos->fZoom;
	}
	pthread_mutex_unlock(&m_tMutex);

	return iRet;
}

int AnalyCfgMan::GetSceneGlobalCruise(int iChnID, SceneCruiseT *ptSceneCruise)
{
	if( iChnID < 0 || iChnID > MAX_CHANNEL_NUM - 1 || ptSceneCruise == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);

	int index = 0;
	ptSceneCruise->iCnt = 0;
	for(int i = 0; i < MAX_SCENE_NUM; i++)
	{
		if(m_tChnInfo[iChnID].tSceneCfg[i].tInfo.iPresetID > 0 && m_tChnInfo[iChnID].tSceneCfg[i].tInfo.iStayTime > 0)
		{
			ptSceneCruise->iCnt++;
			ptSceneCruise->SceneNodes[index].iSceneID  = i;
			ptSceneCruise->SceneNodes[index].iHoldTime = m_tChnInfo[iChnID].tSceneCfg[i].tInfo.iStayTime;
			index++;
		}
	}
	
	pthread_mutex_unlock(&m_tMutex);

	return 0;
}


void AnalyCfgMan::InitPlateFilter(bool bEvent)
{
	// 过滤时间
	int resnap_time = m_tLprCfg.iDupTime;
	if(resnap_time <= 0)
	{
		return;
	}
	
	// 从数据库查询在过滤时间范围内的记录
	char sqlCmd[1024] = {0};
	memset(sqlCmd,0,sizeof(sqlCmd));
	time_t tBegin = time(NULL)-resnap_time*60;
	sprintf(sqlCmd, "select CREATETIME,PLATE from AnalyTable where DATATYPE=%d and CREATETIME>%ld order by CREATETIME desc limit 300;",bEvent?eEventData:eIllegaldata,tBegin);

	DbQueryResult tDbResult = {0};
	MQ_DataBase_Select(MSG_TYPE_ANALY, sqlCmd, &tDbResult);
	
	// 将历史车牌记到内存缓存中
	pthread_mutex_lock(&m_tPlateHistoryMutex);
	
	PLATE_BUFFER ** pphistory = bEvent ? &m_pPlate1History : &m_pPlate2History;

	int nRow = tDbResult.iRow;
	int nCol = tDbResult.iCol;
	char *pData = (char *)(tDbResult.data);
	for (int i = 0; i < nRow; i++)
	{
		time_t tCreateTime = 0;
		char plate[32] = {0};
		for (int j = 0; j < nCol; j++)
		{
			char szBuffer[32] = {0};
			int len = *((int *)pData);
			pData = pData + sizeof(int);
			if(len > 0 && len < 320)
			{
				memcpy(szBuffer, pData, len);
				szBuffer[len] = '\0';
			}
			pData = pData + len;

			if (j == 0)
			{
				tCreateTime = atol(szBuffer);
			}
			else if (j == 1)
			{
				if(strlen(szBuffer) > 0)
				{
					UTF8_To_GB2312(szBuffer,strlen(szBuffer),plate,sizeof(plate));
				}
			}
		}

		if(tCreateTime > time(NULL)-resnap_time*60 && strlen(plate) > 0)
		{
			PLATE_BUFFER * pNew = (PLATE_BUFFER*)malloc(sizeof(PLATE_BUFFER));
			if (pNew == NULL)
			{
				LOG_ERROR_FMT("malloc failed");
				break;
			}
			strcpy(pNew->plate,plate);
			pNew->t = tCreateTime;
			pNew->next = *pphistory;
			*pphistory = pNew;
			printf("Palte History Init:%s Create at %ld\n",plate,tCreateTime);
		}
	}

	pthread_mutex_unlock(&m_tPlateHistoryMutex);
}

bool AnalyCfgMan::SimilarPlateFilter(bool bEvent, char * plate)
{
	if(plate == NULL)
	{
		return false;
	}

	// 过滤时间
	int resnap_time = m_tLprCfg.iDupTime;
	//if(resnap_time <= 0)
	//{
	//	return false;
	//}

	pthread_mutex_lock(&m_tPlateHistoryMutex);

	// 删除超时的车牌
	time_t tNow = time(NULL);
	PLATE_BUFFER ** pphistory = bEvent?&m_pPlate1History:&m_pPlate2History;
	PLATE_BUFFER * p1 = *pphistory;
	PLATE_BUFFER * p2 = NULL;
	while(p1)
	{
		if(tNow - p1->t > resnap_time*60)
		{
			printf("%s Create at %ld,Now is %ld,total %ld > resnaptime %ds,[DEL]\n",p1->plate,p1->t,tNow,(tNow-p1->t),resnap_time*60);
			PLATE_BUFFER * pdel = p1;
			if(p2 == NULL)
			{
				*pphistory = p1->next;
			}
			else
			{
				p2->next = p1->next;
			}

			p1 = p1->next;

			free(pdel);
			pdel = NULL;
		}
		else
		{
			p2 = p1;
			p1 = p1->next;
		}
	}
	
	// 遍历链表找出相似的车牌
	bool bSimilar = false;
	PLATE_BUFFER * p = *pphistory;
	while(p)
	{
		//printf("%s Create at %ld,Now is %ld,total %ld < resnaptime %d judge similar to [%s]\n",p->plate,p->t,tNow,(tNow-p->t),resnap_time*60,plate);
		int iSameCnt = 0;
		for(int i = 0; i < strlen(p->plate) && i < strlen(plate); i++)
		{
			if(   ((p->plate[i] >= 'A' && p->plate[i] <= 'Z')||(p->plate[i] >= '0' && p->plate[i] <= '9'))
				&&((plate[i] >= 'A' && plate[i] <= 'Z')||(plate[i] >= '0' && plate[i] <= '9'))
				&& p->plate[i] == plate[i])
			{
				iSameCnt++;
			}
		}

		if(iSameCnt>=m_tLprCfg.iDupSens)
		{
			printf("%s Create at %ld,Now is %ld,total %ld < resnaptime %d is similar to [%s]\n",p->plate,p->t,tNow,(tNow-p->t),resnap_time*60,plate);
			bSimilar = true;
			break;
		}
		p = p->next;
	}

	// 不相似加入到链表
	if(bSimilar == false && resnap_time > 0)
	{
		PLATE_BUFFER * p = (PLATE_BUFFER*)malloc(sizeof(PLATE_BUFFER));
		if(p)
		{
			strcpy(p->plate,plate);
			p->t = tNow;
			p->next = *pphistory;
			*pphistory = p;
			
			printf("%s Create at %ld\n",plate,tNow);
		}
	}
	pthread_mutex_unlock(&m_tPlateHistoryMutex);

	return bSimilar;
}


void AnalyCfgMan::CheckDefualtHanZi(char * plate)
{
	if(plate == NULL)
	{
		return;
	}
	
	if(plate == NULL || strlen(plate) < 2)
	{
		return;
	}

	LPRCfg tLprCfg = {0};
	AnalyCfgMan::GetInstance()->GetLprCfg(&tLprCfg);
	if(tLprCfg.iHanziConf < 60)
	{
		return;
	}

	int iDef = (tLprCfg.iDefProv-eBeiJing)%31;
	LOG_DEBUG_FMT("车牌汉字矫正前:%s",plate);

	for (int i = 0; i < 31; i++)
	{
		if (i != iDef)
		{
			ReplaceKeyWord(plate, g_HANZI_LIST[i],  g_HANZI_LIST[iDef]);
		}
	}
	LOG_DEBUG_FMT("车牌汉字矫正后:%s",plate);
}

bool AnalyCfgMan::PlateReasonableCheck(char * plate)
{
	if(plate == NULL || strlen(plate) == 0)
	{
		return true;
	}
	
	static int JIGUAN_LIST[31][26] = 
	{
	   //A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},//	 北京	"京",ABCDEFGHIJKLMNOPQRSTUVWXYZ
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},//	 新疆 	"新",ABCDEFGH-JKLMNOPQR--------
		{1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0},//	 内蒙 	"蒙",ABCDEFGH-JKLM-O-----------
		{1,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},//	 云南 	"云",A-CDEFGH-JKLMNOPQRS-------
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0,0},//	 河南 	"豫",ABCDEFGH-JKLMNOPQRS-U-----
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},//	 辽宁 	"辽",ABCDEFGH-JKLMNOP----------
		{1,1,1,1,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},//	 贵州 	"贵",ABCDEFGH-J----O-----------
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},//	 天津 	"津",ABCDEFGHIJKLMNOPQRSTUVWXYZ
		{1,1,1,1,1,1,1,1,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},//	 福建	"闽",ABCDEFGH-JK---O-----------
		{1,1,1,1,1,1,1,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,0,0,0,0},//	 河北 	"冀",ABCDEFGH-J----O--R-T------
		{1,1,1,1,1,1,0,1,0,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0},//	 山西 	"晋",ABCDEF-H-JKLM-O-----------
		{1,1,1,1,1,1,1,1,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},//	 吉林 	"吉",ABCDEFGH-JK---O-----------
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0},//	 黑龙江 	"黑",ABCDEFGH-JKLMNOP-R--------
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},//	 上海 	"沪",ABCDEFGHIJKLMNOPQRSTUVWXYZ
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},//	 江苏 	"苏",ABCDEFGH-JKLMNO-----------
		{1,1,1,1,1,1,1,1,0,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0},//	 浙江 	"浙",ABCDEFGH-JKL--O-----------
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0},//	 安徽 	"皖",ABCDEFGH-JKLMNOP-RS-------
		{1,1,1,1,1,1,1,1,0,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0},//	 江西 	"赣",ABCDEFGH-JKLM-O-----------
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,1,0},//	 山东 	"鲁",ABCDEFGH-JKLMNOPQRS-UV--Y-
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},//	 湖北 	"鄂",ABCDEFGH-JKLMNOPQRS-------
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,0,0,1,0,1,0,0,0,0,0},//	 湖南 	"湘",ABCDEFGH-JKLMNO---S-U-----
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},//	 广东 	"粤",ABCDEFGH-JKLMNOPQRSTUVWXYZ
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0},//	 广西 	"桂",ABCDEFGH-JKLMNOP-R--------
		{1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},//	 海南 	"琼",ABCDE---------O-----------
		{1,1,1,1,1,1,0,1,0,1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,1,1},//	 四川	"川",ABCDEF-H-JKLM-O-QRSTUVWXYZ
		{1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},//	 西藏 	"藏",ABCDEFG-------O-----------
		{1,1,1,1,1,1,1,1,0,1,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0},//	 陕西 	"陕",ABCDEFGH-JK---O------V----
		{1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0},//	 甘肃 	"甘",ABCDEFGH-JKLMNOP----------
		{1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},//	 青海 	"青",ABCDEFGH------O-----------
		{1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},//	 宁夏 	"宁",ABCDE-O-------------------
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} //	 重庆 	"渝",ABCDEFGHIJKLMNOPQRSTUVWXYZ
	};


	// 英文字母过多
	int A2ZCnt = 0;
	for(int i = 0; i < strlen(plate); i++)
	{
		if(plate[i] >= 'A' && plate[i] <= 'Z')
		{
			A2ZCnt++;
		}
	}
	if(A2ZCnt > 4)
	{
		return false;
	}

	return true;
}

int ReplaceKeyWord(char * str,const char * keywords, const char * value, int recursion)
{
	char outstr[2048] = {0};
	char * p = strstr(str, keywords);
	if(p)
	{
		//copy 开始到p的位置
		memcpy(outstr,str,p-str);
		//copy 值
		memcpy(outstr+strlen(outstr),value,strlen(value));
		//copy 剩余的部分
		memcpy(outstr+strlen(outstr),p+strlen(keywords),strlen(str)-(p-str)-strlen(keywords));

		// 递归调用
		if (recursion)
		{
			ReplaceKeyWord(outstr,keywords,value);
		}

		// 拷贝回去
		strcpy(str,outstr);

		return 0;
	}
	else
	{
		return -1;
	}
}


