#include "include/mq_interface.h"
#include "../OAL/include/oal_log.h"


MQ_API int Msg_Init( int msgKey )
{
	int qid = -1;

#ifndef WIN32
	key_t key = msgKey;

	qid = msgget(key, 0);
	if(qid < 0)
	{
		qid = msgget(key,IPC_CREAT|0666);
	}
#endif

	return qid;
}


MQ_API int Msg_Send( int qid, void *pData , int size )
{
#ifndef WIN32
	return msgsnd( qid, pData, size-sizeof(long), 0 );
#else
	return -1;
#endif
}

MQ_API int Msg_Recv( int qid, int msg_type, void *pData , int size )
{
#ifndef WIN32
	return msgrcv( qid, pData, size-sizeof(long), msg_type, 0);
#else
	return -1;
#endif
}

MQ_API int Msg_Recv_NoWait( int qid, int msg_type, void *pData , int size )
{
#ifndef WIN32
	return msgrcv( qid, pData, size-sizeof(long), msg_type, IPC_NOWAIT);
#else
	return -1;
#endif
}

MQ_API int Msg_Send_Rsv( int qid, int msg_type, void *pData , int size )
{
#ifndef WIN32
	int ret = msgsnd( qid, pData, size-sizeof(long), 0 );
	if( ret < 0 )
	{
		return ret;
	}

	return msgrcv( qid, pData, size-sizeof(long), msg_type, 0);
#else
	return -1;
#endif
}

MQ_API int Msg_Kill( int qid )
{
#ifndef WIN32
	msgctl( qid, IPC_RMID, NULL);
#endif
	return 0;
}

MQ_API int Wait_Service_Start( int qid, long lDst, long lCmd )
{
	int	iRet = -1;

	while (true)
	{
		MQ_BUF tMqBuf;
		memset(&tMqBuf, 0, sizeof(MQ_BUF));

		int msg_size = Msg_Recv( qid, lDst, &tMqBuf, sizeof(MQ_BUF));
		if( msg_size < 0 )
		{
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				LOG_ERROR_FMT("Msg_Recv failed, %s",strerror(errno));
				break;
			}
		}
		else
		{
			if(tMqBuf.lCmd == lCmd)
			{
				iRet = 0;
				break;
			}
		}
	}

	return iRet;
}


MQ_API int Start_Next_Service(int qid, long lDst, long lSrc, long lCmd)
{
	int iRet = -1;
	MQ_BUF tMqBuf;

	tMqBuf.lDst = lDst;
	tMqBuf.lSrc = lSrc;
	tMqBuf.lCmd = lCmd;
	tMqBuf.lLen = 0;

	iRet = Msg_Send(qid, &tMqBuf , MSG_HEADER_SIZE + tMqBuf.lLen );

	if(iRet < 0)
	{
		LOG_ERROR_FMT("Msg_Send error:%s",strerror(errno));
	}

	return iRet;
}

