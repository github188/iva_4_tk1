#include "include/mq_onvif.h"
#include "oal_log.h"
#include "oal_time.h"

static int s_iOnvifMsgID = -1;

MQ_API int MQ_Onvif_Init()
{
	if (s_iOnvifMsgID < 0)
	{
		s_iOnvifMsgID = Msg_Init(ONVIF_KEY);
	}

	return s_iOnvifMsgID;
}

MQ_API int MQ_Onvif_Close()
{
	if(s_iOnvifMsgID >= 0)
	{
		return Msg_Kill(s_iOnvifMsgID);
	}

	return 0;
}

MQ_API int MQ_Onvif_Set_VISource( long lSrc, int iChnID, const VISource *ptInfo )
{
	if(s_iOnvifMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_SET_DEV;
	tMqBuf.lLen = sizeof(int) + sizeof(VISource);
	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), ptInfo, sizeof(VISource));
	
	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Onvif_Set_Proxy( long lSrc, int iChnID, const ClickZoomProxy *ptInfo )
{
	if(s_iOnvifMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_SET_PROXY;
	tMqBuf.lLen = sizeof(int) + sizeof(ClickZoomProxy);
	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), ptInfo, sizeof(ClickZoomProxy));

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Onvif_Set_TimeSyn( long lSrc, const OnvifTimeSyn *ptInfo )
{
	if(s_iOnvifMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_SET_TIMESYN;
	tMqBuf.lLen = sizeof(OnvifTimeSyn);
	memcpy(tMqBuf.data, ptInfo, sizeof(OnvifTimeSyn));

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Onvif_Ptz_Move( long lSrc, int iChnID, int iAction, int iSpeed )
{
	if(s_iOnvifMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_PTZ_MOVE;
	tMqBuf.lLen = 3*sizeof(int);

	int * data = (int *)(tMqBuf.data);
	data[0] = iChnID;
	data[1] = iAction;
	data[2] = iSpeed;

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Onvif_Ptz_Stop( long lSrc, int iChnID )
{
	if(s_iOnvifMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_PTZ_STOP;
	tMqBuf.lLen = sizeof(int);
	memcpy(tMqBuf.data, &iChnID, sizeof(int));

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}


MQ_API int MQ_Onvif_Aperture_Move( long lSrc, int iChnID, int iAction, int iSpeed )
{
	if(s_iOnvifMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_APERTURE_MOVE;
	tMqBuf.lLen = 3*sizeof(int);

	int * data = (int *)(tMqBuf.data);
	data[0] = iChnID;
	data[1] = iAction;
	data[2] = iSpeed;

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Onvif_Aperture_Stop( long lSrc, int iChnID )
{
	if(s_iOnvifMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_APERTURE_STOP;
	tMqBuf.lLen = sizeof(int);
	memcpy(tMqBuf.data, &iChnID, sizeof(int));

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Onvif_Focus_Move( long lSrc, int iChnID, int iAction, int iSpeed )
{
	if(s_iOnvifMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_FOCUS_MOVE;
	tMqBuf.lLen = 3*sizeof(int);

	int * data = (int *)(tMqBuf.data);
	data[0] = iChnID;
	data[1] = iAction;
	data[2] = iSpeed;

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);

}

MQ_API int MQ_Onvif_Focus_Stop( long lSrc, int iChnID )
{
	if(s_iOnvifMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_FOCUS_STOP;
	tMqBuf.lLen = sizeof(int);
	memcpy(tMqBuf.data, &iChnID, sizeof(int));

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Onvif_Get_Presets( long lSrc, int iChnID, PresetArray *ptPresetArray )
{
	if(s_iOnvifMsgID < 0 || ptPresetArray == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	memset(&tMqBuf, 0, sizeof(MQ_BUF));

	// 清空历史消息队列
	int try_cnt = 100;
	int msg_size = -1;
	do
	{
		msg_size = Msg_Recv_NoWait(s_iOnvifMsgID, lSrc, &tMqBuf, sizeof(MQ_BUF));
		try_cnt--;
	}while(msg_size > 0 && try_cnt > 0);

	// 发送请求
	memset(&tMqBuf, 0, sizeof(MQ_BUF));
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_GET_PRESETS;
	tMqBuf.lLen = sizeof(int);
	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	int iRet = Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
	if(iRet < 0)
	{
		LOG_ERROR_FMT("Msg_Send failed, %s", strerror(errno));
		return iRet;
	}

	// 接收回应
	memset(&tMqBuf, 0, sizeof(MQ_BUF));
	try_cnt = 100;
	do
	{
		msg_size = Msg_Recv_NoWait(s_iOnvifMsgID, lSrc, &tMqBuf, sizeof(MQ_BUF));
		if(msg_size < 0)
		{
			try_cnt--;
			usleep(50*1000);
		}
	}while(msg_size < 0 && (errno == ENOMSG || errno == EINTR) && try_cnt > 0);

	if(msg_size < 0)
	{
		LOG_WARNS_FMT("No Response");
		return -1;
	}
	
	int * data = (int *)(tMqBuf.data);
	iRet = data[0];
	ptPresetArray->iPresetNum = data[1];
	if (ptPresetArray->iPresetNum < 0 || ptPresetArray->iPresetNum >= MAX_PRESET_NUM)
	{
		LOG_WARNS_FMT("ptPresetArray->iPresetNum = %d",ptPresetArray->iPresetNum);
		return -1;
	}

	PresetNode * ptList = (PresetNode *)(tMqBuf.data + 2*sizeof(int));
	for (int i = 0; i < ptPresetArray->iPresetNum; i++)
	{
		memcpy(ptPresetArray->arPresets + i, ptList + i, sizeof(PresetNode));
	}

	return iRet;
}

MQ_API int MQ_Onvif_Preset( long lSrc, int iChnID, int iAction, int iPresetID )
{
	if(s_iOnvifMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_PRESET;
	tMqBuf.lLen = 3*sizeof(int);

	int * data = (int *)(tMqBuf.data);
	data[0] = iChnID;
	data[1] = iAction;
	data[2] = iPresetID;

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Onvif_Get_AbsPos( long lSrc, int iChnID, AbsPosition * ptPos )
{
	if(s_iOnvifMsgID < 0 || ptPos == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	memset(&tMqBuf, 0, sizeof(MQ_BUF));

	// 清空历史消息队列
	int try_cnt = 100;
	int msg_size = -1;
	do
	{
		msg_size = Msg_Recv_NoWait(s_iOnvifMsgID, lSrc, &tMqBuf, sizeof(MQ_BUF));
		try_cnt--;
	}while(msg_size > 0 && try_cnt > 0);

	// 发送请求
	memset(&tMqBuf, 0, sizeof(MQ_BUF));
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_GET_ABS;
	tMqBuf.lLen = sizeof(int);
	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	int iRet = Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
	if(iRet < 0)
	{
		LOG_ERROR_FMT("Msg_Send failed, %s", strerror(errno));
		return iRet;
	}

	// 接收回应
	memset(&tMqBuf, 0, sizeof(MQ_BUF));
	try_cnt = 100;
	do
	{
		msg_size = Msg_Recv_NoWait(s_iOnvifMsgID, lSrc, &tMqBuf, sizeof(MQ_BUF));
		if(msg_size < 0)
		{
			try_cnt--;
			usleep(50*1000);
		}
	}while(msg_size < 0 && (errno == ENOMSG || errno == EINTR) && try_cnt > 0);

	if(msg_size < 0)
	{
		LOG_WARNS_FMT("No Response");
		return -1;
	}

	iRet = *((int *)(tMqBuf.data));
	memcpy(ptPos, tMqBuf.data+sizeof(int), sizeof(AbsPosition));
	return iRet;
}

MQ_API int MQ_Onvif_Abs_Move( long lSrc, int iChnID, const AbsPosition *ptPos )
{
	if(s_iOnvifMsgID < 0 || ptPos == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_ABS_MOVE;
	tMqBuf.lLen = sizeof(int)+sizeof(AbsPosition);

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), ptPos, sizeof(AbsPosition));

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Onvif_ClickZoom( long lSrc, int iChnID, const ClickArea *ptArea )
{
	if(s_iOnvifMsgID < 0 || ptArea == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_ONVIF;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = ONVIF_CMD_CLICKZOOM;
	tMqBuf.lLen = sizeof(int)+sizeof(ClickArea);

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), ptArea, sizeof(ClickArea));

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Onvif_Answer_Presets( long lDes, int iRet, const PresetArray *ptPresetArray )
{
	if(s_iOnvifMsgID < 0 || ptPresetArray == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf;
	tMqBuf.lDst = lDes;
	tMqBuf.lSrc = MSG_TYPE_ONVIF;
	tMqBuf.lCmd = ONVIF_CMD_GET_PRESETS;
	tMqBuf.lLen = sizeof(int)+sizeof(int)+sizeof(PresetNode)*ptPresetArray->iPresetNum;
	int * data = (int *)(tMqBuf.data);
	data[0] = iRet;
	data[1] = ptPresetArray->iPresetNum;
	
	for (int i = 0; i < ptPresetArray->iPresetNum; i++)
	{
		memcpy(tMqBuf.data+2*sizeof(int)+i*sizeof(PresetNode), ptPresetArray->arPresets+i, sizeof(PresetNode));
	}

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Onvif_Answer_AbsPos( long lDes, int iRet, const AbsPosition *ptPos )
{
	if(s_iOnvifMsgID < 0 || ptPos == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf;
	tMqBuf.lDst = lDes;
	tMqBuf.lSrc = MSG_TYPE_ONVIF;
	tMqBuf.lCmd = ONVIF_CMD_GET_ABS;
	tMqBuf.lLen = sizeof(int)+sizeof(AbsPosition);
	memcpy(tMqBuf.data, &iRet, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), ptPos, sizeof(AbsPosition));

	return Msg_Send(s_iOnvifMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}



