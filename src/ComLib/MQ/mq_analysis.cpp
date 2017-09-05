#include "include/mq_analysis.h"
#include "../OAL/include/oal_log.h"
#include "../OAL/include/oal_time.h"
#include "capacity.h"

static int s_iAnalyMsgID = -1;

MQ_API int MQ_Analy_Init()
{
	if(Global_BuilldEdition()==FSS_EDITION)	{
		return -1;
	}

	if (s_iAnalyMsgID < 0)
	{
		s_iAnalyMsgID = Msg_Init(ANALY_KEY);
	}

	return s_iAnalyMsgID;
}

MQ_API int MQ_Analy_Close()
{
	if(s_iAnalyMsgID >= 0)
	{
		return Msg_Kill(s_iAnalyMsgID);
	}

	return 0;
}

static int MQ_Analy_SendToGlobal( long lSrc, long lCmd, const void *data, int len )
{
	if(s_iAnalyMsgID < 0 || (data == NULL && len > 0))
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ANALY;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = lCmd;
	tMqBuf.lLen = len;
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("msg is too long");
		return -1;
	}

	if(data)
	{
		memcpy(tMqBuf.data, data, len);
	}

	return Msg_Send(s_iAnalyMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

static int MQ_Analy_SendToChn( long lSrc, long lCmd, int iChnID, const void *data, int len )
{
	if(s_iAnalyMsgID < 0 || (data == NULL && len != 0))
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ANALY;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = lCmd;
	tMqBuf.lLen = sizeof(int) + len;
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("msg is too long");
		return -1;
	}
	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	if(data)
	{
		memcpy(tMqBuf.data+sizeof(int), data, len);
	}

	return Msg_Send(s_iAnalyMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

static int MQ_Analy_SendToChnScn( long lSrc, long lCmd, int iChnID, int iScnID, const void *data, int len )
{
	if(s_iAnalyMsgID < 0 || (data == NULL && len != 0))
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ANALY;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = lCmd;
	tMqBuf.lLen = 2*sizeof(int) + len;
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("msg is too long");
		return -1;
	}
	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), &iScnID, sizeof(int));
	if(data)
	{
		memcpy(tMqBuf.data+2*sizeof(int), data, len);
	}

	return Msg_Send(s_iAnalyMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Analy_DeviceInfo( const DeviceInfoT * ptInfo )
{
	return MQ_Analy_SendToGlobal(MSG_TYPE_MASTER, ANALY_CMD_DEVICEINFO, ptInfo, sizeof(DeviceInfoT));
}

MQ_API int MQ_Analy_SoftwareLicense( const AuthStatusT * ptInfo )
{
	return MQ_Analy_SendToGlobal(MSG_TYPE_MASTER, ANALY_CMD_LICENSE, ptInfo, sizeof(AuthStatusT));
}

MQ_API int MQ_Analy_RootPath( const char * pszRootPath )
{
	return MQ_Analy_SendToGlobal(MSG_TYPE_MASTER, ANALY_CMD_ROOTPATH, pszRootPath, pszRootPath?strlen(pszRootPath):0);
}

MQ_API int MQ_Analy_FullStatus(const StoreFullStatusT * ptInfo )
{
	return MQ_Analy_SendToGlobal(MSG_TYPE_MASTER, ANALY_CMD_STORE_FULL, ptInfo, sizeof(StoreFullStatusT));
}

MQ_API int MQ_Analy_LPR( const LPRCfg * ptInfo )
{
	return MQ_Analy_SendToGlobal(MSG_TYPE_MASTER, ANALY_CMD_LPR, ptInfo, sizeof(LPRCfg));
}

MQ_API int MQ_Analy_Dispatch( const DispatchCtrlT * ptInfo )
{
	return MQ_Analy_SendToGlobal(MSG_TYPE_MASTER, ANALY_CMD_DISPATCH, ptInfo, sizeof(DispatchCtrlT));
}

MQ_API int MQ_Analy_Chn_AnalyEnable( int iChnID, bool bEnable )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_CHN_OPEN, iChnID, &bEnable, sizeof(bool));
}

MQ_API int MQ_Analy_Chn_AnalyPlan( int iChnID, const AnalyPlan * ptInfo )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_CHN_PLAN, iChnID, ptInfo, sizeof(AnalyPlan));
}

MQ_API int MQ_Analy_Chn_AnalyPlanEnable( int iChnID, bool bEnable )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_CHN_PLAN_OPEN, iChnID, &bEnable, sizeof(bool));
}

MQ_API int MQ_Analy_Chn_AnalyNotifyEnable( int iChnID, bool bEnable )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_CHN_NOTIFY_OPEN, iChnID, &bEnable, sizeof(bool));
}

MQ_API int MQ_Analy_Chn_PlaceInfo( int iChnID, const PlaceInfoT * ptInfo )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_CHN_PLACE, iChnID, ptInfo, sizeof(PlaceInfoT));
}

MQ_API int MQ_Analy_Chn_ClearAnaly( int iChnID )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_CHN_CLEAR, iChnID, NULL, 0);
}

MQ_API int MQ_Analy_Chn_ManualSnap( int iChnID, const ManualSnapT * ptInfo )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_CHN_MANUAL, iChnID, ptInfo, sizeof(ManualSnapT));
}

MQ_API int MQ_Analy_Chn_Preset_Pos( int iChnID, int iPresetID, const AbsPosition * ptInfo )
{
	return MQ_Analy_SendToChnScn(MSG_TYPE_MASTER, ANALY_CMD_CHN_PRESET_POS, iChnID, iPresetID, ptInfo, sizeof(AbsPosition));
}

MQ_API int MQ_Analy_Chn_Scn_Info( int iChnID, int iScnID, const SceneInfoT * ptInfo )
{
	return MQ_Analy_SendToChnScn(MSG_TYPE_MASTER, ANALY_CMD_CHN_SCENE_INFO, iChnID, iScnID, ptInfo, sizeof(SceneInfoT));
}

MQ_API int MQ_Analy_Chn_Scn_Rule( int iChnID, int iScnID, const AnalyRuleT * ptInfo )
{
	return MQ_Analy_SendToChnScn(MSG_TYPE_MASTER, ANALY_CMD_CHN_SCENE_RULE, iChnID, iScnID, ptInfo, sizeof(AnalyRuleT));
}

MQ_API int MQ_Analy_Ent_Determine( int iEntID, const DetermineT * ptInfo )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_EVENT_DETERMINE, iEntID, ptInfo, sizeof(DetermineT));
}

MQ_API int MQ_Analy_Ent_EvidenceModel( int iEntID, const EvidenceModelT * ptInfo )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_EVENT_EVIDENCE, iEntID, ptInfo, sizeof(EvidenceModelT));
}

MQ_API int MQ_Analy_Ent_ImgMakeup( int iEntID, int iModelType, const ImgMakeupT * ptInfo )
{
	return MQ_Analy_SendToChnScn(MSG_TYPE_MASTER, ANALY_CMD_EVENT_IMG_MAKEUP, iEntID, iModelType, ptInfo, sizeof(ImgMakeupT));
}

MQ_API int MQ_Analy_Ent_ImgCom( int iEntID, const ImgComposeT * ptInfo )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_EVENT_IMG_COM, iEntID, ptInfo, sizeof(ImgComposeT));
}

MQ_API int MQ_Analy_Ent_ImgOSD( int iEntID, const ImgOSDT * ptInfo )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_EVENT_IMG_OSD, iEntID, ptInfo, sizeof(ImgOSDT));
}

MQ_API int MQ_Analy_Ent_ImgQuality( int iEntID, const ImgQualityT * ptInfo )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_EVENT_IMG_QUALITY, iEntID, ptInfo, sizeof(ImgQualityT));
}

MQ_API int MQ_Analy_Ent_RecEvidence( int iEntID, const RecEvidenceT * ptInfo )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_EVENT_REC, iEntID, ptInfo, sizeof(RecEvidenceT));
}

MQ_API int MQ_Analy_Ent_Code( int iEntID, const EventCodeT * ptInfo )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_EVENT_CODE, iEntID, ptInfo, sizeof(EventCodeT));
}

MQ_API int MQ_Analy_Ent_FileNaming( int iEntID, const FileNamingT * ptInfo )
{
	return MQ_Analy_SendToChn(MSG_TYPE_MASTER, ANALY_CMD_EVENT_NAMING, iEntID, ptInfo, sizeof(FileNamingT));
}
