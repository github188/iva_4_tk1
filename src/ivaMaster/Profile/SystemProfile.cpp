#include "SystemProfile.h"
#include <stdio.h>
#include "oal_unicode.h"
#include "capacity.h"
#include "oal_time.h"

#define SEGMENT_LEN  256

SystemProfile* SystemProfile::m_pInstance = NULL;

SystemProfile::SystemProfile()
{
	m_pCfgFile = NULL;
}

int SystemProfile::Initialize()
{
	int iRet=-1;
	if(NULL == m_pInstance)
	{
		m_pInstance = new SystemProfile();

		m_pInstance->m_pCfgFile = new CProfileIni(CFG_DIR_PATH, IVA_CFG_FILE_NAME);
		m_pInstance->m_pCfgFile->ReadProfile();
		m_pInstance->SendInitCfg2Other();
	}
	return 0;
}

void SystemProfile::UnInitialize()
{
	if(m_pInstance)
	{
		if(m_pInstance->m_pCfgFile)
		{
			delete m_pInstance->m_pCfgFile;
			m_pInstance->m_pCfgFile = NULL;
		}

		delete m_pInstance;
		m_pInstance=NULL;
	}
}

SystemProfile* SystemProfile::GetInstance()
{
	return m_pInstance;
}


void SystemProfile::SendInitCfg2Other()
{
	// Onvif
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		ClickZoomProxy tProxy = {0};
		GetClickZoomProxy(i, &tProxy);
		MQ_Onvif_Set_Proxy(MSG_TYPE_MASTER, i, &tProxy);
		usleep(10000);
	}

	// Record
	RecStrategy tRecStrategy = {0};
	GetRecStrategy(&tRecStrategy);
	MQ_Record_SetStrategy(&tRecStrategy);
	usleep(10000);

	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		RecPlan tRecPlan = {0};
		GetChnRecPlan(i, &tRecPlan);
		MQ_Record_SetPlan(i, &tRecPlan);
		usleep(10000);

		bool bEnable = false;
		GetChnRecPlanSwitch(i,bEnable);
		MQ_Record_SetEnable(i,bEnable);
		usleep(10000);
	}

	// VIdeoOut
	VoStrategy tVoStrategy = {0};
	GetVideoOutStrategy(&tVoStrategy);
	MQ_VideoOut_SetStrategy(&tVoStrategy);
	usleep(10000);

	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		RtmpServer tSvr = {0};
		GetVideoOutRtmpSvr(i, &tSvr);
		MQ_VideoOut_SetRtmpServer(i, &tSvr);
		usleep(10000);

		bool bEnable = false;
		GetVideoOutSwitch(i,bEnable);
		MQ_VideoOut_SetEnable(i,bEnable);
		usleep(10000);
	}
	
	// Upload
	UploadStrategy tUpStrategy = {0};
	GetUploadSvrInfo(&tUpStrategy);
	MQ_Upload_SetStrategy(&tUpStrategy);
	usleep(10000);

	bool bUploadEnable = false;
	GetUploadSwitch(bUploadEnable);
	MQ_Upload_Enable(bUploadEnable);
	usleep(10000);

	FtpAdvance tAdv = {0};
	GetUploadFtpAdv(&tAdv);
	MQ_Upload_SetFtpAdv(&tAdv);
	usleep(10000);

	NotifySvr tNotifySvr = {0};
	GetNotifySvrInfo(&tNotifySvr);
	MQ_Upload_Notify_SetServer(&tNotifySvr);
	usleep(10000);

	bool bNotifyEnable = false;
	GetNotifySwitch(bNotifyEnable);
	MQ_Upload_Notify_Enable(bNotifyEnable);
	usleep(10000);

	// Analy
	DeviceInfoT tDeviceInfo = {0};
	GetDeviceInfo(&tDeviceInfo);
	MQ_Analy_DeviceInfo(&tDeviceInfo);
	usleep(10000);

	LPRCfg tLprCfg = {0};
	GetLprCfg(&tLprCfg);
	MQ_Analy_LPR(&tLprCfg);
	usleep(10000);

	DispatchCtrlT tDispatchCfg = {0};
	GetDispatch(&tDispatchCfg);
	MQ_Analy_Dispatch(&tDispatchCfg);
	usleep(10000);

	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		PlaceInfoT tPlaceInfo = {0};
		GetChnPlace(i, &tPlaceInfo);
		MQ_Analy_Chn_PlaceInfo(i, &tPlaceInfo);
		usleep(10000);

		AnalyPlan tPlan = {0};
		GetChnAnalyPlan(i, &tPlan);
		MQ_Analy_Chn_AnalyPlan(i, &tPlan);
		usleep(10000);

		for (int j = 0; j < MAX_SCENE_NUM; j++)
		{
			SceneInfoT tSceneInfo = {0};
			GetChnSceneInfo(i, j, &tSceneInfo);
			MQ_Analy_Chn_Scn_Info(i, j, &tSceneInfo);
			usleep(10000);

			if (tSceneInfo.iPresetID > 0)
			{
				AbsPosition tPos = {0};
				SystemProfile::GetInstance()->GetPresetPosition(i, tSceneInfo.iPresetID, &tPos);

				MQ_Analy_Chn_Preset_Pos(i, tSceneInfo.iPresetID, &tPos);
				usleep(10000);
			}
		}
	}

	for (int i = 0; i < eIT_Event_Max; i++)
	{
		DetermineT tDetermine = {0};
		GetEventDetermine(i, &tDetermine);
		MQ_Analy_Ent_Determine(i, &tDetermine);
		usleep(10000);

		EvidenceModelT tEviModel = {0};
		GetEventEvidenceModel(i, &tEviModel);
		MQ_Analy_Ent_EvidenceModel(i, &tEviModel);
		usleep(10000);

		EventDef tDef = {0};
		Global_EventDef(i, &tDef);
		for (int j = 0; j < tDef.iEviModelNum; j++)
		{
			ImgMakeupT tImgMakeup = {0};
			GetEventImgMakeUp(i, j, &tImgMakeup);
			MQ_Analy_Ent_ImgMakeup(i, j, &tImgMakeup);
			usleep(10000);
		}

		ImgComposeT tImgCom = {0};
		GetEventImgCom(i, &tImgCom);
		MQ_Analy_Ent_ImgCom(i, &tImgCom);
		usleep(10000);

		ImgOSDT tImgOSD = {0};
		GetEventImgOsd(i, &tImgOSD);
		MQ_Analy_Ent_ImgOSD(i, &tImgOSD);
		usleep(10000);

		ImgQualityT tImgQua = {0};
		GetEventImgQuality(i, &tImgQua);
		MQ_Analy_Ent_ImgQuality(i, &tImgQua);
		usleep(10000);

		RecEvidenceT tRecEvi = {0};
		GetEventRec(i, &tRecEvi);
		MQ_Analy_Ent_RecEvidence(i, &tRecEvi);
		usleep(10000);

		EventCodeT tCode = {0};
		GetEventCodeInfo(i, &tCode);
		MQ_Analy_Ent_Code(i, &tCode);
		usleep(10000);

		FileNamingT tNaming = {0};
		GetEventFileNamingInfo(i, &tNaming);
		MQ_Analy_Ent_FileNaming(i, &tNaming);
		usleep(10000);
	}

	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		bool bEnable = false;
		GetChnAnalyPlanSwitch(i, bEnable);
		MQ_Analy_Chn_AnalyPlanEnable(i, bEnable);
		usleep(10000);
	}
}

int SystemProfile::GetRecStrategy( RecStrategy* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "RecStrategy");

	m_pCfgFile->GetProfileValueNumber(szSegMent, "FileFormat", pInfo->iFileFormat, REC_FILE_MP4);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "PacketMode", pInfo->iPacketMode, REC_PACKET_SIZE);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "PacketSize", pInfo->iPacketSize, 20);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "PacketTime", pInfo->iPacketTime, 10);

	return 0;
}

int SystemProfile::SetRecStrategy( const RecStrategy* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "RecStrategy");

	m_pCfgFile->SetProfileValueNumber(szSegMent, "FileFormat", pInfo->iFileFormat);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "PacketMode", pInfo->iPacketMode);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "PacketSize", pInfo->iPacketSize);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "PacketTime", pInfo->iPacketTime);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetChnRecPlan( int iChnID, RecPlan* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	char szKey[SEGMENT_LEN] = {0};

	sprintf(szSegMent, "RecPlan_%d_Mon",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tMon[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tMon[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Tue",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tTue[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tTue[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Wed",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tWed[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tWed[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Thu",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tThu[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tThu[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Fri",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tFri[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tFri[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Sat",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tSat[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tSat[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Sun",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tSun[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tSun[i].iEndTime);
	}
	return 0;
}

int SystemProfile::SetChnRecPlan( int iChnID, const RecPlan* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	char szKey[SEGMENT_LEN] = {0};

	sprintf(szSegMent, "RecPlan_%d_Mon",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tMon[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tMon[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Tue",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tTue[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tTue[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Wed",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tWed[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tWed[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Thu",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tThu[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tThu[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Fri",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tFri[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tFri[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Sat",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tSat[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tSat[i].iEndTime);
	}

	sprintf(szSegMent, "RecPlan_%d_Sun",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tSun[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tSun[i].iEndTime);
	}
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetChnRecPlanSwitch( int iChnID, bool & bEnable )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Record_%d", iChnID);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Enable", bEnable);
	return 0;
}

int SystemProfile::SetChnRecPlanSwitch( int iChnID, bool bEnable )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Record_%d", iChnID);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Enable", bEnable);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetVideoOutStrategy( VoStrategy* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "VO_Strategy");

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Protocol", pInfo->iProtocol, VO_PROTOCOL_NONE);

	return 0;
}

int SystemProfile::SetVideoOutStrategy( const VoStrategy* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "VO_Strategy");

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Protocol", pInfo->iProtocol);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetVideoOutRtmpSvr( int iChnID, RtmpServer* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "RTMP_Server_%d", iChnID);

	DeviceInfoT tDev = {0};
	GetDeviceInfo(&tDev);

	char szdefualturl[128] = {0};
	sprintf(szdefualturl, "rtmp://192.168.0.1:1935/live/%s_%d", tDev.szDevID, iChnID);

	m_pCfgFile->GetProfileValueString(szSegMent, "Addr", pInfo->szUrl, szdefualturl);
	return 0;
}

int SystemProfile::SetVideoOutRtmpSvr( int iChnID, const RtmpServer* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "RTMP_Server_%d", iChnID);

	m_pCfgFile->SetProfileValueString(szSegMent, "Addr", pInfo->szUrl);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetVideoOutSwitch( int iChnID, bool & bEnable )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "VideoOut_%d", iChnID);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Enable", bEnable);
	return 0;
}

int SystemProfile::SetVideoOutSwitch( int iChnID, bool bEnable )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "VideoOut_%d", iChnID);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Enable", bEnable);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetDeviceInfo(DeviceInfoT* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	const char * szSegMent = "DevInfo";

	m_pCfgFile->GetProfileValueString(szSegMent, "DevID", pInfo->szDevID, "00000001");
	m_pCfgFile->GetProfileValueString(szSegMent, "NetID", pInfo->szNetID);
	m_pCfgFile->GetProfileValueString(szSegMent, "RegID", pInfo->szRegID);
	m_pCfgFile->GetProfileValueString(szSegMent, "ManID", pInfo->szManID);

	return 0;
}

int SystemProfile::SetDeviceInfo(const DeviceInfoT* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	const char * szSegMent = "DevInfo";

	m_pCfgFile->SetProfileValueString(szSegMent, "DevID", pInfo->szDevID);
	m_pCfgFile->SetProfileValueString(szSegMent, "NetID", pInfo->szNetID);
	m_pCfgFile->SetProfileValueString(szSegMent, "RegID", pInfo->szRegID);
	m_pCfgFile->SetProfileValueString(szSegMent, "ManID", pInfo->szManID);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetLprCfg( LPRCfg* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	const char * szSegMent = "LPR";
	m_pCfgFile->GetProfileValueNumber(szSegMent, "DefProv",   pInfo->iDefProv,   eHuBei);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "HanziConf", pInfo->iHanziConf, 90);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "MinSize",   pInfo->iMinSize,	 100);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "DupTime",   pInfo->iDupTime,   0);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "DupSens",   pInfo->iDupSens,	 4);

	return 0;
}

int SystemProfile::SetLprCfg( const LPRCfg* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	const char * szSegMent = "LPR";
	m_pCfgFile->SetProfileValueNumber(szSegMent, "DefProv",   pInfo->iDefProv);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "HanziConf", pInfo->iHanziConf);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "MinSize",   pInfo->iMinSize);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "DupTime",   pInfo->iDupTime);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "DupSens",   pInfo->iDupSens);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetDispatch( DispatchCtrlT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	const char * szSegMent = "AlgDispatch";
	m_pCfgFile->GetProfileValueNumber(szSegMent, "DefProv",   pInfo->iSensitivity,   eSenCompromise);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "HanziConf", pInfo->iPriority,		 ePriByTarget);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "MinSize",   pInfo->iSceneMinStay,	 30);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "RepeatTime",pInfo->iRepeatTime,	 600);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "DupTime",   pInfo->bPtzLock);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "DupSens",   pInfo->iLockTime);

	return 0;
}

int SystemProfile::SetDispatch( const DispatchCtrlT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	const char * szSegMent = "AlgDispatch";
	m_pCfgFile->SetProfileValueNumber(szSegMent, "DefProv",   pInfo->iSensitivity);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "HanziConf", pInfo->iPriority);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "MinSize",   pInfo->iSceneMinStay);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "RepeatTime",pInfo->iRepeatTime);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "DupTime",   pInfo->bPtzLock);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "DupSens",   pInfo->iLockTime);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetChnAnalyPlan( int iChnID, AnalyPlan* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	char szKey[SEGMENT_LEN] = {0};

	sprintf(szSegMent, "AnalyPlan_%d_Mon",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tMon[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tMon[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Tue",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tTue[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tTue[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Wed",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tWed[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tWed[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Thu",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tThu[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tThu[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Fri",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tFri[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tFri[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Sat",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tSat[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tSat[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Sun",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tSun[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey, pInfo->tSun[i].iEndTime);
	}
	return 0;
}

int SystemProfile::SetChnAnalyPlan( int iChnID, const AnalyPlan* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	char szKey[SEGMENT_LEN] = {0};

	sprintf(szSegMent, "AnalyPlan_%d_Mon",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tMon[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tMon[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Tue",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tTue[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tTue[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Wed",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tWed[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tWed[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Thu",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tThu[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tThu[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Fri",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tFri[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tFri[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Sat",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tSat[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tSat[i].iEndTime);
	}

	sprintf(szSegMent, "AnalyPlan_%d_Sun",iChnID);
	for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
	{
		sprintf(szKey, "Plan%dStart", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tSun[i].iStartTime);

		sprintf(szKey, "Plan%dEnd", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey, pInfo->tSun[i].iEndTime);
	}
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetChnAnalyPlanSwitch( int iChnID, bool & bEnable )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Analy_%d", iChnID);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Enable", bEnable);
	return 0;
}

int SystemProfile::SetChnAnalyPlanSwitch( int iChnID, bool bEnable )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Analy_%d", iChnID);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Enable", bEnable);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetChnPlace( int iChnID, PlaceInfoT* pInfo )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Place_%d", iChnID);

	m_pCfgFile->GetProfileValueString(szSegMent, "AreaName",    pInfo->szAreaName);
	m_pCfgFile->GetProfileValueString(szSegMent, "AreaID",      pInfo->szAreaID);
	m_pCfgFile->GetProfileValueString(szSegMent, "MonitoryName",pInfo->szMonitoryName);
	m_pCfgFile->GetProfileValueString(szSegMent, "MonitoryID",  pInfo->szMonitoryID);
	m_pCfgFile->GetProfileValueString(szSegMent, "PlaceName",   pInfo->szPlaceName);
	m_pCfgFile->GetProfileValueString(szSegMent, "PlaceID",     pInfo->szPlaceID);
	return 0;
}

int SystemProfile::SetChnPlace( int iChnID, const PlaceInfoT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Place_%d", iChnID);

	m_pCfgFile->SetProfileValueString(szSegMent, "AreaName",    pInfo->szAreaName);
	m_pCfgFile->SetProfileValueString(szSegMent, "AreaID",      pInfo->szAreaID);
	m_pCfgFile->SetProfileValueString(szSegMent, "MonitoryName",pInfo->szMonitoryName);
	m_pCfgFile->SetProfileValueString(szSegMent, "MonitoryID",  pInfo->szMonitoryID);
	m_pCfgFile->SetProfileValueString(szSegMent, "PlaceName",   pInfo->szPlaceName);
	m_pCfgFile->SetProfileValueString(szSegMent, "PlaceID",     pInfo->szPlaceID);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::DeleteChnSceneInfo( int iChnID )
{
	SceneInfoT tSceneInfo = {0};
	tSceneInfo.iPresetID = -1;
	tSceneInfo.iStayTime = 600;
	tSceneInfo.iDirection = e2UN;
	for (int iScnID = 0; iScnID < MAX_SCENE_NUM; iScnID++)
	{
		sprintf(tSceneInfo.szSceneName, "scene%d", iScnID+1);

		SetChnSceneInfo(iChnID, iScnID, &tSceneInfo);
	}

	return 0;
}

int SystemProfile::GetChnSceneInfo( int iChnID, int iScnID, SceneInfoT* pInfo )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Chn%d_Scene%d", iChnID, iScnID);

	char  szSceneName[256] ={0};
	sprintf(szSceneName, "scene%d", iScnID+1);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "PresetID",   pInfo->iPresetID, -1);
	m_pCfgFile->GetProfileValueString(szSegMent, "SceneName",  pInfo->szSceneName, szSceneName);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "StayTime",   pInfo->iStayTime, 600);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Direction",  pInfo->iDirection, e2UN);
	return 0;
}

int SystemProfile::SetChnSceneInfo( int iChnID, int iScnID, const SceneInfoT* pInfo )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Chn%d_Scene%d", iChnID, iScnID);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "PresetID",   pInfo->iPresetID);
	m_pCfgFile->SetProfileValueString(szSegMent, "SceneName",  pInfo->szSceneName);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "StayTime",   pInfo->iStayTime);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Direction",  pInfo->iDirection);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetPresetPosition( int iChnID, int iPresetNo, AbsPosition* pInfo )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Chn%d_Preset%d", iChnID, iPresetNo);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Pan",   pInfo->fPan);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "fTilt", pInfo->fTilt);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Zoom",  pInfo->fZoom);
	return 0;
}

int SystemProfile::SetPresetPosition( int iChnID, int iPresetNo, const AbsPosition* pInfo )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Chn%d_Preset%d", iChnID, iPresetNo);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Pan",   pInfo->fPan);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "fTilt", pInfo->fTilt);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Zoom",  pInfo->fZoom);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetEventDetermine( int iEvent, DetermineT* pInfo )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Determine", iEvent);

	int     iValue1 = 0,iValue2 = 0;
	double  fValue1 = 0,fValue2 = 0;
	Global_DefualtThreshold(iEvent, iValue1, iValue2, fValue1, fValue2);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "iValue1",  pInfo->iValue1, iValue1);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "iValue2",  pInfo->iValue2, iValue2);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "fValue1",  pInfo->fValue1, fValue1);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "fValue2",  pInfo->fValue2, fValue2);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::SetEventDetermine( int iEvent, const DetermineT* pInfo )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Determine", iEvent);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "iValue1",  pInfo->iValue1);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "iValue2",  pInfo->iValue2);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "fValue1",  pInfo->fValue1);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "fValue2",  pInfo->fValue2);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetEventEvidenceModel( int iEvent, EvidenceModelT* pInfo )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_EviModel", iEvent);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Model",  pInfo->iModel);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "EnableEvent",  pInfo->bEnableEvent);
	return 0;
}

int SystemProfile::SetEventEvidenceModel( int iEvent, const EvidenceModelT* pInfo )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_EviModel", iEvent);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Model",  pInfo->iModel);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "EnableEvent",  pInfo->bEnableEvent);
	return m_pCfgFile->WriteProfile();
}

void  DefualtEviModel(int iEvent, int iModel, ImgMakeupT &tMakeup)
{
	if (iEvent == eIT_Illegal_Park)
	{
		if (iModel == 0)// 即停即走
		{
			tMakeup.iImgNum = 4;
			tMakeup.iSnapPos[0] = eFull;
			tMakeup.iSnapPos[1] = eFull;
			tMakeup.iSnapPos[2] = eCloser;
			tMakeup.iSnapPos[3] = eFeature;
			tMakeup.iInterval[0] = 0;
			tMakeup.iInterval[1] = 2;
			tMakeup.iInterval[2] = 0;
			tMakeup.iInterval[3] = 0;
		}
		else if (iModel == 1)// 二次抓拍
		{
			tMakeup.iImgNum = 4;
			tMakeup.iSnapPos[0] = eFull;
			tMakeup.iSnapPos[1] = eFeature;
			tMakeup.iSnapPos[2] = eFull;
			tMakeup.iSnapPos[3] = eFeature;
			tMakeup.iInterval[0] = 0;
			tMakeup.iInterval[1] = 0;
			tMakeup.iInterval[2] = 0;
			tMakeup.iInterval[3] = 0;
		}
	}
}

int SystemProfile::GetEventImgMakeUp( int iEvent, int iModel, ImgMakeupT* pInfo )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	ImgMakeupT tDefInfo = {0};
	DefualtEviModel(iEvent, iModel, tDefInfo);
	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Model%d_Makeup", iEvent, iModel);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Model",  pInfo->iImgNum, tDefInfo.iImgNum);
	for (int i = 0; i < MAX_IMG_NUM; i++)
	{
		char szKey[SEGMENT_LEN] = {0};
		sprintf(szKey, "SnapPos%d", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey,  pInfo->iSnapPos[i], tDefInfo.iSnapPos[i]);

		sprintf(szKey, "Interval%d", i);
		m_pCfgFile->GetProfileValueNumber(szSegMent, szKey,  pInfo->iInterval[i], tDefInfo.iInterval[i]);
	}
	return 0;
}

int SystemProfile::SetEventImgMakeUp( int iEvent, int iModel, const ImgMakeupT* pInfo )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Model%d_Makeup", iEvent, iModel);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Model",  pInfo->iImgNum);
	for (int i = 0; i < MAX_IMG_NUM; i++)
	{
		char szKey[SEGMENT_LEN] = {0};
		sprintf(szKey, "SnapPos%d", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey,  pInfo->iSnapPos[i]);

		sprintf(szKey, "Interval%d", i);
		m_pCfgFile->SetProfileValueNumber(szSegMent, szKey,  pInfo->iInterval[i]);
	}
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetEventImgCom( int iEvent, ImgComposeT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Compose", iEvent);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Model2", pInfo->iComModel2, eNotCom);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Model3", pInfo->iComModel3, eNotCom);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Model4", pInfo->iComModel4, eMatCom);
	return 0;
}

int SystemProfile::SetEventImgCom( int iEvent, const ImgComposeT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Compose", iEvent);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Model2", pInfo->iComModel2);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Model3", pInfo->iComModel3);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Model4", pInfo->iComModel4);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetEventImgOsd( int iEvent, ImgOSDT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_OSD", iEvent);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "BackColorR", pInfo->tBackColor.r);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "BackColorG", pInfo->tBackColor.g);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "BackColorB", pInfo->tBackColor.b);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "FontColorR", pInfo->tFontColor.r, 255);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "FontColorG", pInfo->tFontColor.g);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "FontColorB", pInfo->tFontColor.b);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "FontSize", pInfo->iFontSize, 32);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "PosModel", pInfo->iPosModel, eEachIn);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Content", pInfo->iContentFlag, OSD_TIME|OSD_PLACE|OSD_PLATE|OSD_EVENT);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Left", pInfo->iLeft);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Top", pInfo->iTop);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::SetEventImgOsd( int iEvent, const ImgOSDT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_OSD", iEvent);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "BackColorR", pInfo->tBackColor.r);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "BackColorG", pInfo->tBackColor.g);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "BackColorB", pInfo->tBackColor.b);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "FontColorR", pInfo->tFontColor.r);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "FontColorG", pInfo->tFontColor.g);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "FontColorB", pInfo->tFontColor.b);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "FontSize", pInfo->iFontSize);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "PosModel", pInfo->iPosModel);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Content", pInfo->iContentFlag);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Left", pInfo->iLeft);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Top", pInfo->iTop);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetEventImgQuality( int iEvent, ImgQualityT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Quality", iEvent);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "SizeLimit", pInfo->iSizeLimit, 500);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "WidthLimit", pInfo->iWidthLimit, 1920);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "HeightLimit", pInfo->iHeightLimit, 1080);
	return 0;

}

int SystemProfile::SetEventImgQuality( int iEvent, const ImgQualityT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Quality", iEvent);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "SizeLimit", pInfo->iSizeLimit);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "WidthLimit", pInfo->iWidthLimit);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "HeightLimit", pInfo->iHeightLimit);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetEventRec( int iEvent, RecEvidenceT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Record", iEvent);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Enable", pInfo->bEnable, true);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "RecModel", pInfo->iRecModel,eRecJoint);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "FileFormat", pInfo->iFileFormat,REC_FILE_MP4);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "PreTime", pInfo->iPreTime);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "AftTime", pInfo->iAftTime);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "MinTime", pInfo->iMinTime,5);
	return 0;
}

int SystemProfile::SetEventRec( int iEvent, const RecEvidenceT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Record", iEvent);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Enable", pInfo->bEnable);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "RecModel", pInfo->iRecModel);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "FileFormat", pInfo->iFileFormat);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "PreTime", pInfo->iPreTime);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "AftTime", pInfo->iAftTime);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "MinTime", pInfo->iMinTime);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetEventCodeInfo( int iEvent, EventCodeT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}
	
	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Code", iEvent);
	m_pCfgFile->GetProfileValueString(szSegMent, "Code", pInfo->szCode, Global_DefualtCode(iEvent));
	m_pCfgFile->GetProfileValueString(szSegMent, "Desc", pInfo->szDesc, Global_DefualtDesc(iEvent));
	return 0;
}

int SystemProfile::SetEventCodeInfo( int iEvent, const EventCodeT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Code", iEvent);
	m_pCfgFile->SetProfileValueString(szSegMent, "Code", pInfo->szCode);
	m_pCfgFile->SetProfileValueString(szSegMent, "Desc", pInfo->szDesc);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetEventFileNamingInfo( int iEvent, FileNamingT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Naming", iEvent);
	
	m_pCfgFile->GetProfileValueNumber(szSegMent, "TimeModel", pInfo->iTimeModel, eUseEndTime);
	m_pCfgFile->GetProfileValueString(szSegMent, "IllegalImg", pInfo->szIllegalImg,"%YYYY%-%MM%-%DD%_%HH%-%mm%-%SS%-%XXX%");
	m_pCfgFile->GetProfileValueString(szSegMent, "IllegalRec", pInfo->szIllegalRec,"%YYYY%-%MM%-%DD%_%HH%-%mm%-%SS%-%XXX%");
	m_pCfgFile->GetProfileValueString(szSegMent, "EventImg",   pInfo->szEventImg,"%YYYY%-%MM%-%DD%_%HH%-%mm%-%SS%-%XXX%_notify");
	m_pCfgFile->GetProfileValueString(szSegMent, "EventRec",   pInfo->szEventRec,"%YYYY%-%MM%-%DD%_%HH%-%mm%-%SS%-%XXX%_notify");

	return 0;
}

int SystemProfile::SetEventFileNamingInfo( int iEvent, const FileNamingT* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Event%d_Naming", iEvent);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "TimeModel",  pInfo->iTimeModel);
	m_pCfgFile->SetProfileValueString(szSegMent, "IllegalImg", pInfo->szIllegalImg);
	m_pCfgFile->SetProfileValueString(szSegMent, "IllegalRec", pInfo->szIllegalRec);
	m_pCfgFile->SetProfileValueString(szSegMent, "EventImg",   pInfo->szEventImg);
	m_pCfgFile->SetProfileValueString(szSegMent, "EventRec",   pInfo->szEventRec);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetUploadSvrInfo( UploadStrategy* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Upload");

	// 上传模式
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Model",   pInfo->iModel);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "DelOK",   pInfo->bDelOk, true);

	// FTP配置
	m_pCfgFile->GetProfileValueString(szSegMent, "FtpAddr", pInfo->tFtpInfo.szAddr);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "FtpPort", pInfo->tFtpInfo.iPort, 21);
	m_pCfgFile->GetProfileValueString(szSegMent, "FtpUser", pInfo->tFtpInfo.szUser);
	m_pCfgFile->GetProfileValueString(szSegMent, "FtpPass", pInfo->tFtpInfo.szPass);

	// HTTP配置
	m_pCfgFile->GetProfileValueString(szSegMent, "PlatAddr", pInfo->tPlatform.szAddr);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "PlatPort", pInfo->tPlatform.iPort, 0);
	m_pCfgFile->GetProfileValueString(szSegMent, "PlatID",   pInfo->tPlatform.szPlatID);

	// MQTT配置
	m_pCfgFile->GetProfileValueString(szSegMent, "MqttAddr", pInfo->tMqttSvr.szAddr);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "MqttPort", pInfo->tMqttSvr.iPort, 1883);
	m_pCfgFile->GetProfileValueString(szSegMent, "MqttID",   pInfo->tMqttSvr.szPlatID);

	return 0;
}

int SystemProfile::SetUploadSvrInfo( const UploadStrategy* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Upload");

	// 上传模式
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Model",   pInfo->iModel);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "DelOK",   pInfo->bDelOk);

	// FTP配置
	m_pCfgFile->SetProfileValueString(szSegMent, "FtpAddr", pInfo->tFtpInfo.szAddr);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "FtpPort", pInfo->tFtpInfo.iPort);
	m_pCfgFile->SetProfileValueString(szSegMent, "FtpUser", pInfo->tFtpInfo.szUser);
	m_pCfgFile->SetProfileValueString(szSegMent, "FtpPass", pInfo->tFtpInfo.szPass);

	// 平台配置
	m_pCfgFile->SetProfileValueString(szSegMent, "PlatAddr", pInfo->tPlatform.szAddr);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "PlatPort", pInfo->tPlatform.iPort);
	m_pCfgFile->SetProfileValueString(szSegMent, "PlatID",   pInfo->tPlatform.szPlatID);

	// MQTT配置
	m_pCfgFile->SetProfileValueString(szSegMent, "MqttAddr", pInfo->tMqttSvr.szAddr);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "MqttPort", pInfo->tMqttSvr.iPort);
	m_pCfgFile->SetProfileValueString(szSegMent, "MqttID",   pInfo->tMqttSvr.szPlatID);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetUploadSwitch( bool & bEnable )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Upload");

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Enable", bEnable);
	return 0;
}

int SystemProfile::SetUploadSwitch( bool bEnable )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Upload");

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Enable", bEnable);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetUploadFtpAdv( FtpAdvance* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "FtpAdvance");

	// 上传模式
	m_pCfgFile->GetProfileValueNumber(szSegMent, "ContentFlag", pInfo->iContentFlag, (FTP_FLAG_IMG|FTP_FLAG_REC));
	m_pCfgFile->GetProfileValueNumber(szSegMent, "CharSet", pInfo->iCharSet, eCharset_GB2312);
	m_pCfgFile->GetProfileValueString(szSegMent, "IllegalDir", pInfo->szIllegalDirGrammar, "%IP%/%YYYY%-%MM%-%DD%");
	m_pCfgFile->GetProfileValueString(szSegMent, "EventDir", pInfo->szEventDirGrammar, "%IP%/%YYYY%-%MM%-%DD%");
	m_pCfgFile->GetProfileValueString(szSegMent, "TollDir", pInfo->szTollDirGrammar, "%IP%/%YYYY%-%MM%-%DD%");

	return 0;
}

int SystemProfile::SetUploadFtpAdv( const FtpAdvance* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "FtpAdvance");

	// 上传模式
	m_pCfgFile->SetProfileValueNumber(szSegMent, "ContentFlag", pInfo->iContentFlag);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "CharSet", pInfo->iCharSet);
	m_pCfgFile->SetProfileValueString(szSegMent, "IllegalDir", pInfo->szIllegalDirGrammar);
	m_pCfgFile->SetProfileValueString(szSegMent, "EventDir", pInfo->szEventDirGrammar);
	m_pCfgFile->SetProfileValueString(szSegMent, "TollDir", pInfo->szTollDirGrammar);


	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetNotifySvrInfo( NotifySvr* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Notify");

	m_pCfgFile->GetProfileValueString(szSegMent, "Addr", pInfo->szAddr);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Port", pInfo->iPort);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "AttachImg", pInfo->bAttachImg);

	return 0;

}

int SystemProfile::SetNotifySvrInfo( const NotifySvr* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Notify");

	m_pCfgFile->SetProfileValueString(szSegMent, "Addr", pInfo->szAddr);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Port", pInfo->iPort);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "AttachImg", pInfo->bAttachImg);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetNotifySwitch( bool & bEnable )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Notify");

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Enable", bEnable);
	return 0;
}

int SystemProfile::SetNotifySwitch( bool bEnable )
{
	if(m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "Notify");

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Enable", bEnable);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetSynTimeInfo(SynTimeCFG* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	m_pCfgFile->GetProfileValueNumber("TimeSyn", "SynType",  pInfo->iSynType);
	m_pCfgFile->GetProfileValueNumber("TimeSyn", "Interval", pInfo->uInterval,(u32)1440);
	m_pCfgFile->GetProfileValueNumber("TimeSyn", "SynChn",	 pInfo->iSynChnID);
	m_pCfgFile->GetProfileValueString("TimeSyn", "NtpAddr",  pInfo->szNtpAddr);

	return 0;
}

int SystemProfile::SetSynTimeInfo(const SynTimeCFG* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	m_pCfgFile->SetProfileValueNumber("TimeSyn", "SynType",  pInfo->iSynType);
	m_pCfgFile->SetProfileValueNumber("TimeSyn", "Interval", pInfo->uInterval);
	m_pCfgFile->SetProfileValueNumber("TimeSyn", "SynChn",	 pInfo->iSynChnID);
	m_pCfgFile->SetProfileValueString("TimeSyn", "NtpAddr",  pInfo->szNtpAddr);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetTimerRebootInfo(TimerRebootCfg* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	m_pCfgFile->GetProfileValueNumber("TimerReboot", "Enable",  pInfo->bEnable);
	m_pCfgFile->GetProfileValueNumber("TimerReboot", "UseNum",  pInfo->uUseNum);
	if(pInfo->uUseNum < 0) pInfo->uUseNum = 0;
	if(pInfo->uUseNum > MAX_REBOOT_TIME_SIZE) pInfo->uUseNum = MAX_REBOOT_TIME_SIZE;

	for(int i=0; i < pInfo->uUseNum; ++i)
	{
		char szKeyTime[8] = {0};
		sprintf(szKeyTime, "Time%d", i);
		m_pCfgFile->GetProfileValueNumber("TimerReboot", szKeyTime,  pInfo->uTimes[i]);
	}

	return 0;
}

int SystemProfile::SetTimerRebootInfo(const TimerRebootCfg* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	// 删除旧配置
	TimerRebootCfg tOldInfo = {0};
	GetTimerRebootInfo(&tOldInfo);
	for(int i = 0; i < tOldInfo.uUseNum; ++i)
	{
		char szKeyTime[8] = {0};
		sprintf(szKeyTime, "Time%d", i);
		m_pCfgFile->DelProfileSegmentKey("TimerReboot",szKeyTime);
	}

	m_pCfgFile->SetProfileValueNumber("TimerReboot", "Enable",  pInfo->bEnable);
	int iCnt = pInfo->uUseNum;
	if(iCnt < 0) iCnt = 0;
	if(iCnt > MAX_REBOOT_TIME_SIZE) iCnt = MAX_REBOOT_TIME_SIZE;
	m_pCfgFile->SetProfileValueNumber("TimerReboot", "UseNum",  iCnt);

	for(int i=0; i < iCnt; ++i)
	{
		char szKeyTime[8] = {0};
		sprintf(szKeyTime, "Time%d", i);
		m_pCfgFile->SetProfileValueNumber("TimerReboot", szKeyTime, pInfo->uTimes[i]);
	}

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetDiskStategyInfo(DiskFullStrategy* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	m_pCfgFile->GetProfileValueNumber("DiskStategyInfo", "MinSize", pInfo->uMinSize, (u32)300);
	m_pCfgFile->GetProfileValueNumber("DiskStategyInfo", "How2DO",  pInfo->iHow2DO);
	return 0;
}

int SystemProfile::SetDiskStategyInfo(const DiskFullStrategy* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	m_pCfgFile->SetProfileValueNumber("DiskStategyInfo", "MinSize", pInfo->uMinSize);
	m_pCfgFile->SetProfileValueNumber("DiskStategyInfo", "How2DO",  pInfo->iHow2DO);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetDiskUsageCfg(DiskUsageCfg* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "DiskUsage");

	m_pCfgFile->GetProfileValueNumber(szSegMent, "HDD",  pInfo->iHddUseType, DISK_USE_DISABLE);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "SD",   pInfo->iSdUseType,  DISK_USE_DISABLE);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "USB",  pInfo->iUsbUseType, DISK_USE_DISABLE);

	return 0;
}

int SystemProfile::SetDiskUsageCfg(const DiskUsageCfg* pInfo)
{
	// 删除旧配置
	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "DiskUsage");

	m_pCfgFile->SetProfileValueNumber(szSegMent, "HDD",  pInfo->iHddUseType);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "SD",   pInfo->iSdUseType);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "USB",  pInfo->iUsbUseType);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetVideoSourceInfo(int iChnID, VISource* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "VideoSource_%d", iChnID);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Enable",   pInfo->bEnable);
	m_pCfgFile->GetProfileValueString(szSegMent, "IP",       pInfo->szIP);
	m_pCfgFile->GetProfileValueString(szSegMent, "User",     pInfo->szUser, "admin");
	m_pCfgFile->GetProfileValueString(szSegMent, "Pass",     pInfo->szPass);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Company",  pInfo->iManuCode);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "StreamType", pInfo->iStreamType);

	return 0;
}

int SystemProfile::SetVideoSourceInfo(int iChnID, const VISource* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "VideoSource_%d", iChnID);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Enable",   pInfo->bEnable);
	m_pCfgFile->SetProfileValueString(szSegMent, "IP",       pInfo->szIP);
	m_pCfgFile->SetProfileValueString(szSegMent, "User",     pInfo->szUser);
	m_pCfgFile->SetProfileValueString(szSegMent, "Pass",     pInfo->szPass);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Company",  pInfo->iManuCode);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "StreamType", pInfo->iStreamType);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetClickZoomProxy( int iChnID, ClickZoomProxy* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "ClickZoom_%d", iChnID);

	m_pCfgFile->GetProfileValueNumber(szSegMent, "Model",		pInfo->iModel);
	m_pCfgFile->GetProfileValueString(szSegMent, "ProxyIp", 	pInfo->szAddr);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "ProxyPort",  	pInfo->iPort, (int)8100);
	return 0;
}

int SystemProfile::SetClickZoomProxy( int iChnID, const ClickZoomProxy* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "ClickZoom_%d", iChnID);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Model",		pInfo->iModel);
	m_pCfgFile->SetProfileValueString(szSegMent, "ProxyIp", 	pInfo->szAddr);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "ProxyPort",  	pInfo->iPort);

	return m_pCfgFile->WriteProfile();
}


int SystemProfile::GetUserInfo(int iUserID, USER_INFO* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "UserInfo_%d", iUserID);
	
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Enable",pInfo->enable);
	m_pCfgFile->GetProfileValueString(szSegMent, "Name",  pInfo->szName);
	m_pCfgFile->GetProfileValueString(szSegMent, "Pass",  pInfo->szPass);
	m_pCfgFile->GetProfileValueString(szSegMent, "Remark",pInfo->szRemark);
	m_pCfgFile->GetProfileValueNumber(szSegMent, "Role",  pInfo->iRole, (s32)2);
	return 0;
}

int SystemProfile::SetUserInfo(int iUserID, const USER_INFO* pInfo)
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	// 删除旧配置
	char szSegMent[SEGMENT_LEN] = {0};
	sprintf(szSegMent, "UserInfo_%d", iUserID);

	m_pCfgFile->SetProfileValueNumber(szSegMent, "Enable",pInfo->enable);
	m_pCfgFile->SetProfileValueString(szSegMent, "Name",  pInfo->szName);
	m_pCfgFile->SetProfileValueString(szSegMent, "Pass",  pInfo->szPass);
	m_pCfgFile->SetProfileValueString(szSegMent, "Remark",pInfo->szRemark);
	m_pCfgFile->SetProfileValueNumber(szSegMent, "Role",  pInfo->iRole);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetOperateLogLimit( int & iOpLogMax )
{
	m_pCfgFile->GetProfileValueNumber("OpLog", "MaxLimit",iOpLogMax, 1000);
	return 0;
}

int SystemProfile::SetOperateLogLimit( const int & iOpLogMax )
{
	m_pCfgFile->SetProfileValueNumber("OpLog", "MaxLimit",iOpLogMax);
	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetAlarmStrategy( AlarmStrategy * pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	m_pCfgFile->GetProfileValueNumber("AlarmStrategy", "SameInterval", pInfo->iSameInterval, 5);
	m_pCfgFile->GetProfileValueNumber("AlarmStrategy", "KeepDays", pInfo->iKeepDays, 30);

	return 0;
}

int SystemProfile::SetAlarmStrategy( const AlarmStrategy * pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	m_pCfgFile->SetProfileValueNumber("AlarmStrategy", "SameInterval", pInfo->iSameInterval);
	m_pCfgFile->SetProfileValueNumber("AlarmStrategy", "KeepDays", pInfo->iKeepDays);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::Get4GInfo( FourGCFG* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	m_pCfgFile->GetProfileValueNumber("4G", "Open", pInfo->bOpen);
	m_pCfgFile->GetProfileValueNumber("4G", "TelecomID", pInfo->iTelecomID, e10086);

	return 0;
}

int SystemProfile::Set4GInfo( const FourGCFG* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	m_pCfgFile->SetProfileValueNumber("4G", "Open", pInfo->bOpen);
	m_pCfgFile->SetProfileValueNumber("4G", "TelecomID", pInfo->iTelecomID);

	return m_pCfgFile->WriteProfile();
}

int SystemProfile::GetWlanHotInfo( WlanHotCFG* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	m_pCfgFile->GetProfileValueNumber("WlanHot", "Enable",   pInfo->bEnable);
	m_pCfgFile->GetProfileValueString("WlanHot", "SSID",	 pInfo->szSSID, "ceye");
	m_pCfgFile->GetProfileValueString("WlanHot", "Password", pInfo->szPwd,  "Y2V5ZQo=");
	m_pCfgFile->GetProfileValueNumber("WlanHot", "Frequency",pInfo->iFrequency, e2_4GHz);
	m_pCfgFile->GetProfileValueNumber("WlanHot", "MaxClient",pInfo->iMaxClient, 8);

	return 0;
}

int SystemProfile::SetWlanHotInfo( const WlanHotCFG* pInfo )
{
	if(pInfo == NULL || m_pCfgFile == NULL)
	{
		return -1;
	}

	m_pCfgFile->SetProfileValueNumber("WlanHot", "Enable",   pInfo->bEnable);
	m_pCfgFile->SetProfileValueString("WlanHot", "SSID",	 pInfo->szSSID);
	m_pCfgFile->SetProfileValueString("WlanHot", "Password", pInfo->szPwd);
	m_pCfgFile->SetProfileValueNumber("WlanHot", "Frequency",pInfo->iFrequency);
	m_pCfgFile->SetProfileValueNumber("WlanHot", "MaxClient",pInfo->iMaxClient);

	return m_pCfgFile->WriteProfile();
}

