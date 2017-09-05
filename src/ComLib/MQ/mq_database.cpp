#include "include/mq_database.h"
#include "../OAL/include/oal_log.h"

static int s_iDatabaseMsgID = -1;

MQ_API int MQ_Database_Init()
{
	if (s_iDatabaseMsgID < 0)
	{
		s_iDatabaseMsgID = Msg_Init(DATABASE_KEY);
	}

	return s_iDatabaseMsgID;
}

MQ_API int MQ_DataBase_Close()
{
	if(s_iDatabaseMsgID >= 0)
	{
		return Msg_Kill(s_iDatabaseMsgID);
	}

	return 0;
}

MQ_API int MQ_DataBase_Update( long lSrc, const char *sql )
{
	if(s_iDatabaseMsgID < 0 || sql == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_DATABASE;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = DATABASE_CMD_INSERT_DELETE_UPDATE;
	tMqBuf.lLen = strlen(sql);
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("Sql is too long");
		return -1;
	}
	memcpy(tMqBuf.data, sql, tMqBuf.lLen);

	return Msg_Send(s_iDatabaseMsgID, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen);
}


MQ_API int MQ_DataBase_Insert( long lSrc, const char *sql )
{
	return MQ_DataBase_Update(lSrc, sql);
}

MQ_API int MQ_DataBase_Delete( long lSrc, const char *sql )
{
	return MQ_DataBase_Update(lSrc, sql);
}

MQ_API int MQ_DataBase_Select_Count( long lSrc, const char *sql )
{
	if(s_iDatabaseMsgID < 0 || sql == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_DATABASE;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = DATABASE_CMD_SELECT_COUNT;
	tMqBuf.lLen = strlen(sql);
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("Sql is too long");
		return -1;
	}
	memcpy(tMqBuf.data, sql, tMqBuf.lLen);

	int iRet = Msg_Send(s_iDatabaseMsgID, &tMqBuf, MSG_HEADER_SIZE + tMqBuf.lLen);
	if (iRet != 0)
	{
		LOG_ERROR_FMT("Msg_Send failed, %s", strerror(errno));
		return iRet;
	}

	int iRecvLen = Msg_Recv(s_iDatabaseMsgID, lSrc, &tMqBuf, sizeof(MQ_BUF));
	if(iRecvLen < 0)
	{
		LOG_ERROR_FMT("Msg_Recv failed, %s", strerror(errno));
		return iRecvLen;
	}
	else
	{
		int iCnt = *((int *)tMqBuf.data);
		//printf("CNT:%d\n",iCnt);
		return iCnt;
	}
}


MQ_API int MQ_DataBase_Select( long lSrc, const char *sql, DbQueryResult *ptResult )
{
	if(s_iDatabaseMsgID < 0 || sql == NULL || ptResult == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf = {0};
	tMqBuf.lDst = MSG_TYPE_DATABASE;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = DATABASE_CMD_SELECT;
	tMqBuf.lLen = strlen(sql);
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		LOG_ERROR_FMT("Sql is too long");
		return -1;
	}
	memcpy(tMqBuf.data, sql, tMqBuf.lLen);

	int iRet = Msg_Send(s_iDatabaseMsgID, &tMqBuf, MSG_HEADER_SIZE + tMqBuf.lLen);
	if (iRet != 0)
	{
		LOG_ERROR_FMT("Msg_Send failed, %s", strerror(errno));
		return iRet;
	}
	
	// 接收查询回应
	memset(&tMqBuf, 0, sizeof(MQ_BUF));
	int iRecvLen = Msg_Recv(s_iDatabaseMsgID, lSrc, &tMqBuf, sizeof(MQ_BUF));
	if(iRecvLen < 0)
	{
		LOG_ERROR_FMT("Msg_Recv failed, %s", strerror(errno));
		return iRecvLen;
	}
	else
	{
		ptResult->iRow = *((int*)(tMqBuf.data+0*sizeof(int)));
		ptResult->iCol = *((int*)(tMqBuf.data+1*sizeof(int)));
		ptResult->iLen = *((int*)(tMqBuf.data+2*sizeof(int)));
		//printf("1 %p ROW:%d COL:%d LEN:%d\n",ptResult, ptResult->iRow,ptResult->iCol,ptResult->iLen);
		memcpy(ptResult->data, (tMqBuf.data+3*sizeof(int)), ptResult->iLen);
		return 0;
	}
}

MQ_API int MQ_DataBase_Answer_Count( long lDes, int iCnt )
{
	if(s_iDatabaseMsgID < 0)
	{
		return -1;
	}

	MQ_BUF tMqBuf;
	tMqBuf.lDst = lDes;
	tMqBuf.lSrc = MSG_TYPE_DATABASE;
	tMqBuf.lCmd = DATABASE_CMD_SELECT_COUNT;
	tMqBuf.lLen = sizeof(int);
	*((int *)(tMqBuf.data)) = iCnt;
	//printf("CNT:%d\n",iCnt);

	return Msg_Send(s_iDatabaseMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

MQ_API int MQ_DataBase_Answer_Select( long lDes, const DbQueryResult *ptResult )
{
	if(s_iDatabaseMsgID < 0 || ptResult == NULL)
	{
		return -1;
	}

	MQ_BUF tMqBuf;
	tMqBuf.lDst = lDes;
	tMqBuf.lSrc = MSG_TYPE_DATABASE;
	tMqBuf.lCmd = DATABASE_CMD_SELECT;
	tMqBuf.lLen = sizeof(int)*3+ptResult->iLen;
	if(tMqBuf.lLen >= MSG_DATA_SIZE)
	{
		//printf("tMqBuf.lLen >= MSG_DATA_SIZE\n");
		tMqBuf.lLen = sizeof(int)*3;
		*((int*)(tMqBuf.data+0*sizeof(int))) = 0;
		*((int*)(tMqBuf.data+1*sizeof(int))) = ptResult->iCol;
		*((int*)(tMqBuf.data+2*sizeof(int))) = 0;
	}
	else
	{
		*((int*)(tMqBuf.data+0*sizeof(int))) = ptResult->iRow;
		*((int*)(tMqBuf.data+1*sizeof(int))) = ptResult->iCol;
		*((int*)(tMqBuf.data+2*sizeof(int))) = ptResult->iLen;
		memcpy(tMqBuf.data+3*sizeof(int), ptResult->data, ptResult->iLen);
	}
	//printf("ROW:%d COL:%d LEN:%d\n",ptResult->iRow,ptResult->iCol,ptResult->iLen);

	return Msg_Send(s_iDatabaseMsgID, &tMqBuf, MSG_HEADER_SIZE+tMqBuf.lLen);
}

