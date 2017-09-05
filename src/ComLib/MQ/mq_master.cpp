#include "include/mq_master.h"

static int s_iMasterMsgID = -1;

MQ_API int MQ_Master_Init()
{
	if (s_iMasterMsgID < 0)
	{
		s_iMasterMsgID = Msg_Init(MASTER_KEY);
	}

	return s_iMasterMsgID;
}

MQ_API int MQ_Master_Close()
{
	if(s_iMasterMsgID >= 0)
	{
		return Msg_Kill(s_iMasterMsgID);
	}

	return 0;
}


MQ_API int MQ_Master_ISAPI_Request( ISAPI_REQ* req, const char *req_json, ISAPI_RESP * resp, char *resp_json )
{
	if(s_iMasterMsgID < 0 || req == NULL || resp == NULL || resp_json == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_MASTER;
	tMqBuf.lSrc = MSG_TYPE_BOA;
	tMqBuf.lCmd = MASTER_CMD_ISAPI;
	tMqBuf.lLen = sizeof(ISAPI_REQ) + (req_json ? strlen(req_json) : 0);
	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, req, sizeof(ISAPI_REQ));
	if (req_json)
	{
		memcpy(tMqBuf.data+sizeof(ISAPI_REQ), req_json, strlen(req_json));
	}
	
	int iRet = Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
	if(iRet < 0)
	{
		printf("Msg_Send error:%s.\n", strerror(errno));
		return -1;
	}
	
	int iRecvLen = Msg_Recv(s_iMasterMsgID, MSG_TYPE_BOA, &tMqBuf, sizeof(MQ_BUF));
	if(iRecvLen < 0)
	{
		printf("Msg_Recv error:%s.\n", strerror(errno));
		return -1;
	}

	memcpy(resp, tMqBuf.data, sizeof(ISAPI_RESP));
	if((tMqBuf.lLen-sizeof(ISAPI_RESP))>0)
	{
		memcpy(resp_json, tMqBuf.data+sizeof(ISAPI_RESP), tMqBuf.lLen-sizeof(ISAPI_RESP));
		resp_json[tMqBuf.lLen-sizeof(ISAPI_RESP)] = '\0';
	}
	
	return 0;
}


MQ_API int MQ_Master_ISAPI_Response( ISAPI_RESP* resp, const char *resp_json )
{
	if(s_iMasterMsgID < 0 || resp == NULL || resp_json == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_BOA;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = MASTER_CMD_ISAPI;
	tMqBuf.lLen = sizeof(ISAPI_RESP) + strlen(resp_json);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, resp, sizeof(ISAPI_RESP));
	memcpy(tMqBuf.data+sizeof(ISAPI_RESP), resp_json, strlen(resp_json));

	return Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}


MQ_API int MQ_Master_SetRtspInfo( int iChnID, int iStreamType, RtspInfo *pRtsp )
{
	if(s_iMasterMsgID < 0 || pRtsp == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_MASTER;
	tMqBuf.lSrc = MSG_TYPE_ONVIF;
	tMqBuf.lCmd = MASTER_CMD_SET_RTSP;
	tMqBuf.lLen = 2*sizeof(int) + sizeof(RtspInfo);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), &iStreamType, sizeof(int));
	memcpy(tMqBuf.data+2*sizeof(int), pRtsp, sizeof(RtspInfo));

	return Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Master_SetPtzCap( int iChnID, PTZCap *pPtzCap )
{
	if(s_iMasterMsgID < 0 || pPtzCap == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_MASTER;
	tMqBuf.lSrc = MSG_TYPE_ONVIF;
	tMqBuf.lCmd = MASTER_CMD_SET_PTZCAP;
	tMqBuf.lLen = sizeof(int) + sizeof(PTZCap);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), pPtzCap, sizeof(PTZCap));

	return Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Master_SetSysTime( time_t tTime )
{
	if(s_iMasterMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_MASTER;
	tMqBuf.lSrc = MSG_TYPE_ONVIF;
	tMqBuf.lCmd = MASTER_CMD_SET_SYSTIME;
	tMqBuf.lLen = sizeof(time_t);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &tTime, sizeof(time_t));

	return Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Master_SetVIStatus( int iChnID, int iStatus )
{
	if(s_iMasterMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_MASTER;
	tMqBuf.lSrc = MSG_TYPE_VIDEOIN;
	tMqBuf.lCmd = MASTER_CMD_VIDEOIN_STAT;
	tMqBuf.lLen = sizeof(int) + sizeof(int);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), &iStatus, sizeof(int));

	return Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Master_SetRecStatus( int iChnID, int iStatus )
{
	if(s_iMasterMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_MASTER;
	tMqBuf.lSrc = MSG_TYPE_RECORD;
	tMqBuf.lCmd = MASTER_CMD_RECORD_STAT;
	tMqBuf.lLen = sizeof(int) + sizeof(int);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), &iStatus, sizeof(int));

	return Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Master_SetAnalyVersion( const char * szVer )
{
	if(s_iMasterMsgID < 0 || szVer == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_MASTER;
	tMqBuf.lSrc = MSG_TYPE_ANALY;
	tMqBuf.lCmd = MASTER_CMD_ANALY_VERSION;
	tMqBuf.lLen = strlen(szVer)+1;

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, szVer, strlen(szVer));
	tMqBuf.data[strlen(szVer)] = '\0';
	return Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Master_SetAnalyStatus( int iChnID, int iStatus )
{
	if(s_iMasterMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_MASTER;
	tMqBuf.lSrc = MSG_TYPE_ANALY;
	tMqBuf.lCmd = MASTER_CMD_ANALY_STAT;
	tMqBuf.lLen = sizeof(int) + sizeof(int);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), &iStatus, sizeof(int));

	return Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Master_SetUploadStatus( UploadStatus * ptStatus )
{
	if(s_iMasterMsgID < 0 || ptStatus == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_MASTER;
	tMqBuf.lSrc = MSG_TYPE_UPLOAD;
	tMqBuf.lCmd = MASTER_CMD_UPLOAD_STAT;
	tMqBuf.lLen = sizeof(UploadStatus);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, ptStatus, sizeof(UploadStatus));

	return Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}


MQ_API int MQ_Master_GetAllStatus(char *resp_json)
{
	if(s_iMasterMsgID < 0 || resp_json == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_MASTER;
	tMqBuf.lSrc = MSG_TYPE_UPLOAD;
	tMqBuf.lCmd = MASTER_CMD_GETALLSTAT;
	tMqBuf.lLen = 0;

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	int iRet = Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
	if(iRet < 0)
	{
		printf("Msg_Send error:%s.\n", strerror(errno));
		return -1;
	}

	int iRecvLen = Msg_Recv(s_iMasterMsgID, MSG_TYPE_UPLOAD, &tMqBuf, sizeof(MQ_BUF));
	if(iRecvLen < 0)
	{
		printf("Msg_Recv error:%s.\n", strerror(errno));
		return -1;
	}

	memcpy(resp_json, tMqBuf.data, tMqBuf.lLen);
	resp_json[tMqBuf.lLen] = '\0';
	return 0;
}

MQ_API int MQ_Master_SetSysPlatTime( time_t tTime )
{
	if(s_iMasterMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_MASTER;
	tMqBuf.lSrc = MSG_TYPE_UPLOAD;
	tMqBuf.lCmd = MASTER_CMD_SET_SYSTIME;
	tMqBuf.lLen = sizeof(time_t);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &tTime, sizeof(time_t));

	return Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Master_Response_AllStatus( const char *resp_json )
{
	if(s_iMasterMsgID < 0 || resp_json == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_UPLOAD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = MASTER_CMD_GETALLSTAT;
	tMqBuf.lLen = strlen(resp_json);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, resp_json, strlen(resp_json));

	return Msg_Send(s_iMasterMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

