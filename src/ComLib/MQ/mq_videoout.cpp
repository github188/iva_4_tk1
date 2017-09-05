#include "include/mq_videoout.h"

static int s_iVedioOutMsgID = -1;

MQ_API int MQ_VideoOut_Init()
{
	if (s_iVedioOutMsgID < 0)
	{
		s_iVedioOutMsgID = Msg_Init(VIDEOOUT_KEY);
	}

	return s_iVedioOutMsgID;
}

MQ_API int MQ_VideoOut_Close()
{
	if(s_iVedioOutMsgID >= 0)
	{
		return Msg_Kill(s_iVedioOutMsgID);
	}

	return 0;
}

MQ_API int MQ_VideoOut_SetStrategy( const VoStrategy * ptStrategy )
{
	if(s_iVedioOutMsgID < 0 || ptStrategy == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_VIDEOOUT;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = VO_CMD_SET_STRATEGY;
	tMqBuf.lLen = sizeof(VoStrategy);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, ptStrategy, sizeof(VoStrategy));

	return Msg_Send(s_iVedioOutMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_VideoOut_SetRtmpServer( int iChnID, const RtmpServer * ptInfo )
{
	if(s_iVedioOutMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_VIDEOOUT;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = VO_CMD_SET_RTMPSERVER;
	tMqBuf.lLen = sizeof(int) + sizeof(RtmpServer);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), ptInfo, sizeof(RtmpServer));

	return Msg_Send(s_iVedioOutMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_VideoOut_SetEnable( int iChnID, bool bEnable )
{
	if(s_iVedioOutMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_VIDEOOUT;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = VO_CMD_SET_ENABLE;
	tMqBuf.lLen = sizeof(int) + sizeof(bool);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), &bEnable, sizeof(bEnable));

	return Msg_Send(s_iVedioOutMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_VideoOut_SourceChange( int iChnID, const RecSrcStat *ptStat )
{
	if(s_iVedioOutMsgID < 0 || ptStat == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_VIDEOOUT;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = VO_CMD_SOURCE_CHANGED;
	tMqBuf.lLen = sizeof(int) + sizeof(RecSrcStat);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), ptStat, sizeof(RecSrcStat));

	return Msg_Send(s_iVedioOutMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}
