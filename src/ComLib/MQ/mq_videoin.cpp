#include "include/mq_videoin.h"
#include "../OAL/include/oal_log.h"

static int s_iVedioInMsgID = -1;

MQ_API int MQ_VideoIn_Init()
{
	if (s_iVedioInMsgID < 0)
	{
		s_iVedioInMsgID = Msg_Init(VIDEOIN_KEY);
	}

	return s_iVedioInMsgID;
}

MQ_API int MQ_VideoIn_Close()
{
	if(s_iVedioInMsgID >= 0)
	{
		return Msg_Kill(s_iVedioInMsgID);
	}
	else
	{
		return -1;
	}
}

MQ_API int MQ_VideoIn_SetRtspInfo( int iChnID, RtspInfo *pRtsp )
{
	if(s_iVedioInMsgID < 0 || pRtsp == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_VIDEOIN;
	tMqBuf.lSrc = MSG_TYPE_ONVIF;
	tMqBuf.lCmd = VI_CMD_SET_RTSP;
	tMqBuf.lLen = sizeof(int) + sizeof(RtspInfo);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), pRtsp, sizeof(RtspInfo));

	return Msg_Send(s_iVedioInMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

