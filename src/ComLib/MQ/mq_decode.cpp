#include "include/mq_decode.h"
#include "capacity.h"

static int s_iDecodeMsgID = -1;

MQ_API int MQ_Decode_Init()
{
	if(Global_BuilldEdition()==FSS_EDITION)	{
		return -1;
	}

	if (s_iDecodeMsgID < 0)
	{
		s_iDecodeMsgID = Msg_Init(DECODE_KEY);
	}

	return s_iDecodeMsgID;
}

MQ_API int MQ_Decode_Close()
{
	if(s_iDecodeMsgID >= 0)
	{
		return Msg_Kill(s_iDecodeMsgID);
	}
	else
	{
		return -1;
	}
}

MQ_API int MQ_Decode_VideoInfo( int iChnID, DecodeVideoInfo* ptInfo )
{
	if(s_iDecodeMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_DECODE;
	tMqBuf.lSrc = MSG_TYPE_VIDEOIN;
	tMqBuf.lCmd = DECODE_CMD_VIDEO_INFO;
	tMqBuf.lLen = sizeof(int) + sizeof(DecodeVideoInfo);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), ptInfo, sizeof(DecodeVideoInfo));

	return Msg_Send(s_iDecodeMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);

}
