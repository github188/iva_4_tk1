#include "include/mq_record.h"

static int s_iRecordMsgID = -1;

MQ_API int MQ_Record_Init()
{
	if (s_iRecordMsgID < 0)
	{
		s_iRecordMsgID = Msg_Init(RECORD_KEY);
	}

	return s_iRecordMsgID;
}

MQ_API int MQ_Record_Close()
{
	if(s_iRecordMsgID >= 0)
	{
		return Msg_Kill(s_iRecordMsgID);
	}

	return 0;
}


MQ_API int MQ_Record_SoftwareLicense( const AuthStatusT * ptInfo )
{
	if(s_iRecordMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_RECORD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = RECORD_CMD_LICENSE;
	tMqBuf.lLen = sizeof(AuthStatusT);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, ptInfo, sizeof(AuthStatusT));

	return Msg_Send(s_iRecordMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Record_SourceChange( int iChnID, const RecSrcStat *ptStat )
{
	if(s_iRecordMsgID < 0 || ptStat == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_RECORD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = RECORD_CMD_SOURCE_CHANGED;
	tMqBuf.lLen = sizeof(int)+sizeof(RecSrcStat);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), ptStat, sizeof(RecSrcStat));

	return Msg_Send(s_iRecordMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Record_SetStrategy( const RecStrategy * ptStrategy )
{
	if(s_iRecordMsgID < 0 || ptStrategy == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_RECORD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = RECORD_CMD_SET_STRATEGY;
	tMqBuf.lLen = sizeof(RecStrategy);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, ptStrategy, sizeof(RecStrategy));

	return Msg_Send(s_iRecordMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Record_SetPlan( int iChnID, const RecPlan *ptPlan )
{
	if(s_iRecordMsgID < 0 || ptPlan == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_RECORD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = RECORD_CMD_SET_PLAN;
	tMqBuf.lLen = sizeof(int) + sizeof(RecPlan);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), ptPlan, sizeof(RecPlan));

	return Msg_Send(s_iRecordMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Record_SetEnable( int iChnID, bool bEnable )
{
	if(s_iRecordMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_RECORD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = RECORD_CMD_SET_ENABLE;
	tMqBuf.lLen = sizeof(int) + sizeof(bool);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, &iChnID, sizeof(int));
	memcpy(tMqBuf.data+sizeof(int), &bEnable, sizeof(bEnable));

	return Msg_Send(s_iRecordMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}


MQ_API int MQ_Record_RootPath( const char * pszRoot )
{
	if(s_iRecordMsgID < 0 || pszRoot == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_RECORD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = RECORD_CMD_SET_ROOTPATH;
	tMqBuf.lLen = strlen(pszRoot);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, pszRoot, strlen(pszRoot));

	return Msg_Send(s_iRecordMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}

MQ_API int MQ_Record_FullStatus( const StoreFullStatusT * ptInfo )
{
	if(s_iRecordMsgID < 0 || ptInfo == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};

	tMqBuf.lDst = MSG_TYPE_RECORD;
	tMqBuf.lSrc = MSG_TYPE_MASTER;
	tMqBuf.lCmd = RECORD_CMD_SET_FULLSTAT;
	tMqBuf.lLen = sizeof(StoreFullStatusT);

	if(tMqBuf.lLen > MSG_DATA_SIZE)
	{
		printf("MqBuf.lLen:%ld > %d.\n", tMqBuf.lLen, MSG_DATA_SIZE);
		return -1;
	}

	memcpy(tMqBuf.data, ptInfo, sizeof(StoreFullStatusT));

	return Msg_Send(s_iRecordMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);

}