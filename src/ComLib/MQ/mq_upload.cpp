#include "include/mq_upload.h"
#include "oal_log.h"
#include "capacity.h"

static int s_iUploadMsgID = -1;

MQ_API int MQ_Upload_Init()
{
	if(Global_BuilldEdition()==FSS_EDITION)	{
		return -1;
	}

	if (s_iUploadMsgID < 0)
	{
		s_iUploadMsgID = Msg_Init(UPLOAD_KEY);
	}

	return s_iUploadMsgID;
}

MQ_API int MQ_Upload_Close()
{
	if(s_iUploadMsgID >= 0)
	{
		return Msg_Kill(s_iUploadMsgID);
	}
	else
	{
		return -1;
	}
}

MQ_API int MQ_Upload_SetStrategy( const UploadStrategy * ptInfo )
{
	if(s_iUploadMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_UPLOAD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = UPLOAD_CMD_STRATEGY;
	tMqBuf.lLen = sizeof(UploadStrategy);
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("msg is too long");
		return -1;
	}
	memcpy(tMqBuf.data, ptInfo, sizeof(UploadStrategy));

	return Msg_Send(s_iUploadMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Upload_Enable( bool bEnable )
{
	if(s_iUploadMsgID < 0 )
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_UPLOAD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = UPLOAD_CMD_ENABLE;
	tMqBuf.lLen = sizeof(bool);
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("msg is too long");
		return -1;
	}
	memcpy(tMqBuf.data, &bEnable, sizeof(bool));

	return Msg_Send(s_iUploadMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Upload_SetFtpAdv( const FtpAdvance * ptInfo )
{
	if(s_iUploadMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_UPLOAD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = UPLOAD_CMD_FTPADV;
	tMqBuf.lLen = sizeof(FtpAdvance);
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("msg is too long");
		return -1;
	}
	memcpy(tMqBuf.data, ptInfo, sizeof(FtpAdvance));

	return Msg_Send(s_iUploadMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Upload_ResultPreview( const AnalyResult * ptInfo )
{
	if(s_iUploadMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_UPLOAD;
	tMqBuf.lSrc = MSG_TYPE_ANALY;
	tMqBuf.lCmd = UPLOAD_CMD_RESULT_PREVIEW;
	tMqBuf.lLen = sizeof(AnalyResult);
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("msg is too long");
		return -1;
	}
	memcpy(tMqBuf.data, ptInfo, sizeof(AnalyResult));

	return Msg_Send(s_iUploadMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Upload_TrackPreview( int iChnID, const AnalyTracks * ptInfo )
{
	if(s_iUploadMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_UPLOAD;
	tMqBuf.lSrc = MSG_TYPE_ANALY;
	tMqBuf.lCmd = UPLOAD_CMD_TRACK_PREVIEW;
	tMqBuf.lLen = sizeof(int) + sizeof(AnalyTracks);
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("msg is too long");
		return -1;
	}
	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), ptInfo, sizeof(AnalyTracks));

	return Msg_Send(s_iUploadMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Upload_Notify_SetServer( const NotifySvr * ptInfo )
{
	if(s_iUploadMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_UPLOAD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = UPLOAD_CMD_NOTIFY_SERVER;
	tMqBuf.lLen = sizeof(NotifySvr);
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("msg is too long");
		return -1;
	}
	memcpy(tMqBuf.data, ptInfo, sizeof(NotifySvr));

	return Msg_Send(s_iUploadMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Upload_Notify_Enable( bool bEnable )
{
	if(s_iUploadMsgID < 0 )
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_UPLOAD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = UPLOAD_CMD_NOTIFY_ENABLE;
	tMqBuf.lLen = sizeof(bool);
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("msg is too long");
		return -1;
	}
	memcpy(tMqBuf.data, &bEnable, sizeof(bool));

	return Msg_Send(s_iUploadMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_Upload_Notify_New( const AnalyNotify * ptInfo )
{
	if(s_iUploadMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_UPLOAD;
	tMqBuf.lSrc = MSG_TYPE_ANALY;
	tMqBuf.lCmd = UPLOAD_CMD_NOTIFY;
	tMqBuf.lLen = sizeof(AnalyNotify);
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("msg is too long");
		return -1;
	}
	memcpy(tMqBuf.data, ptInfo, sizeof(AnalyNotify));

	return Msg_Send(s_iUploadMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}
