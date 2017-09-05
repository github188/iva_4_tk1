#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mq_database.h"
#include "oal_log.h"
#include "Sqlite3Database.h"

#ifndef WIN32
#include <execinfo.h>
#include <signal.h>
#include "oal_time.h"
const char * PrintSigNo(int signo)
{
	switch (signo)
	{
	case SIGSEGV:
		return "SIGSEGV";
	case SIGBUS:
		return "SIGBUS";
	case SIGQUIT:
		return "SIGQUIT";
	case SIGILL:
		return "SIGILL";
	case SIGTRAP:
		return "SIGTRAP";
	case SIGFPE:
		return "SIGFPE";
	case SIGABRT:
		return "SIGABRT";
	default:
		return "UNKOWN";
	}
}
void exception_dump(int signo)
{
	/*�쳣��¼��д���ļ��� */
	FILE *fp = fopen("./Logfiles/database_execption.txt", "ab+");

	void *array[100];
	size_t size = backtrace (array, 100);
	char ** strings = backtrace_symbols(array, size);

	char szTime[40] = {0};
	TimeFormatString(time(NULL), szTime, 40, eYMDHMS1);

	char szExpMsg[512] = {0};
	sprintf(szExpMsg,"\r\n%s signal=%d:%s\r\n",	szTime, signo, PrintSigNo(signo));

	printf ("%s", szExpMsg);
	if(fp)
	{
		fwrite(szExpMsg, 1, strlen(szExpMsg), fp);
	}

	for (size_t i = 0; i < size; i++)
	{
		char szExpMsg[1024] = {0};
		sprintf(szExpMsg, "%s\r\n", strings[i]);
		printf ("%s", szExpMsg);
		if(fp)
		{
			fwrite(szExpMsg, 1, strlen(szExpMsg), fp);
		}
	}

	free(strings);

	if(fp)
	{
		fclose(fp);
	}
	exit(0);
}
#endif

int main(int argc, char **argv)
{
	LogInit("ivaDatabase", 10*1024*1024);// ��־���10M

#ifndef WIN32
	//signal(SIGINT,  &exception_dump);// �ж��ź�
	signal(SIGSEGV, &exception_dump);// �ֶδ���
	signal(SIGBUS,  &exception_dump);// ���ߴ���
	signal(SIGQUIT, &exception_dump);// �˳��ź�
	signal(SIGILL,  &exception_dump);// �Ƿ�ָ��
	signal(SIGTRAP, &exception_dump);// ����/�ϵ��ж�
	signal(SIGFPE,  &exception_dump);// �����쳣
	signal(SIGABRT, &exception_dump);// ����
	signal(SIGPIPE, SIG_IGN);		 // ���� �ܵ����� һ��socket��Զ�˹رջ����
	signal(SIGHUP,  SIG_IGN);		 // ���� �����ź�
#endif

	// ��ʼ����Ϣ����
	int iDatabaseMqId = MQ_Database_Init();
	if(iDatabaseMqId < 0)
	{
		LOG_ERROR_FMT("MQ_Database_Init fail.\n");
		return 0;
	}

	int iRet = Sqlite3Database::Initialize();
	if(iRet != 0)
	{
		LOG_ERROR_FMT("Initialize fail.\n");
		return 0;
	}

	LOG_INFOS_FMT("******************ivaDataBase Start******************");

	MQ_BUF tMqBuf;
	while(1)
	{
		memset(&tMqBuf, 0, sizeof(MQ_BUF));

		int iRecvLen = Msg_Recv(iDatabaseMqId, MSG_TYPE_DATABASE, &tMqBuf, sizeof(MQ_BUF));
		if(iRecvLen < 0)
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
			switch(tMqBuf.lCmd)
			{
			case DATABASE_CMD_INSERT_DELETE_UPDATE:
				{
					Sqlite3Database::GetInstance()->Excute((const char *)tMqBuf.data);
					break;
				}
			case DATABASE_CMD_SELECT:
				{
					DbQueryResult tResult = {0};

					Sqlite3Database::GetInstance()->Select((const char *)tMqBuf.data, tResult);
					MQ_DataBase_Answer_Select(tMqBuf.lSrc, &tResult);
				break;
				}
			case DATABASE_CMD_SELECT_COUNT:
				{
					int iCnt = Sqlite3Database::GetInstance()->Select_Count((const char *)tMqBuf.data);
					MQ_DataBase_Answer_Count(tMqBuf.lSrc, iCnt);
					break;
				}
			default:
				{
					LOG_DEBUG_FMT("Unknown CMD, %d!",tMqBuf.lCmd);
					break;
				}
			}
		}
	}

	Sqlite3Database::UnInitialize();
	MQ_DataBase_Close();
	LogUninit();
	return 0;
}
