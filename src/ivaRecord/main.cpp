#include "oal_log.h"
#include "mq_record.h"
#include "mq_master.h"
#include "mq_database.h"
#include "RecordMan.h"

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
	/*异常记录会写入文件中 */
	FILE *fp = fopen("./Logfiles/record_execption.txt", "ab+");

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
	LogInit("ivaRecord", 10*1024*1024);// 日志最多10M

#ifndef WIN32
	//signal(SIGINT,  &exception_dump);// 中断信号
	signal(SIGSEGV, &exception_dump);// 分段错误
	signal(SIGBUS,  &exception_dump);// 总线错误
	signal(SIGQUIT, &exception_dump);// 退出信号
	signal(SIGILL,  &exception_dump);// 非法指令
	signal(SIGTRAP, &exception_dump);// 跟踪/断点中断
	signal(SIGFPE,  &exception_dump);// 浮点异常
	signal(SIGABRT, &exception_dump);// 放弃
	signal(SIGPIPE, SIG_IGN);		 // 忽略 管道错误 一般socket被远端关闭会产生
	signal(SIGHUP,  SIG_IGN);		 // 忽略 挂起信号
#endif

	int iRecordMqId = MQ_Record_Init();
	if(iRecordMqId < 0)
	{
		LOG_ERROR_FMT("MQ_Record_Init fail.\n");
		return 0;
	}

	MQ_Master_Init();
	MQ_Database_Init();

	LOG_INFOS_FMT("Wait_Service_Start RECORD_CMD_START");
	Wait_Service_Start(iRecordMqId, MSG_TYPE_RECORD, RECORD_CMD_START);
	LOG_INFOS_FMT("Wait_Service_Start RECORD_CMD_START OK");

	RecordMan::Initialize();

	LOG_INFOS_FMT("******************ivaRecord Start******************");

	MQ_BUF tMqBuf;
	while(1)
	{
		memset(&tMqBuf, 0, sizeof(MQ_BUF));

		int iRecvLen = Msg_Recv(iRecordMqId, MSG_TYPE_RECORD, &tMqBuf, sizeof(MQ_BUF));
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
			case RECORD_CMD_LICENSE:
				{
					AuthStatusT *ptStat = (AuthStatusT*)tMqBuf.data;
					LOG_DEBUG_FMT("[LICENSE] AuthType:%d", ptStat->eAuthType);
					RecordMan::GetInstance()->SetAuthInfo(ptStat);
					break;
				}
			case RECORD_CMD_SOURCE_CHANGED:
				{
					int iChnID  = *((int*)(tMqBuf.data));
					RecSrcStat *ptStat = (RecSrcStat*)(tMqBuf.data + sizeof(int));
					LOG_DEBUG_FMT("[SOURCE CHANGED] Channel%d Enable:%d Changed:%d", iChnID,ptStat->bEnable, ptStat->bChanged);
					
					int iRecStatus = 0;
					if (!ptStat->bEnable)
					{
						iRecStatus = REC_UNINITIALIZED;
					}
					else// if (ptStat->bChanged)
					{
						iRecStatus = REC_NO_H264INFO;
					}
					RecordMan::GetInstance()->SetChnRecStat(iChnID, iRecStatus);
					break;
				}
			case RECORD_CMD_SET_STRATEGY:
				{
					RecStrategy *ptInfo = (RecStrategy*)(tMqBuf.data);
					LOG_DEBUG_FMT("[SET STRATEGY]");
					LOG_DEBUG_FMT("-FileFormat: %s",ptInfo->iFileFormat==REC_FILE_FLV?"flv":(ptInfo->iFileFormat==REC_FILE_AVI?"avi":"mp4"));
					LOG_DEBUG_FMT("-PacketMode: by %s",ptInfo->iPacketMode==REC_PACKET_TIME?"time":"size");
					LOG_DEBUG_FMT("-PacketSize: %dM",ptInfo->iPacketSize);
					LOG_DEBUG_FMT("-PacketTime: %dMins",ptInfo->iPacketTime);
					RecordMan::GetInstance()->SetStrategy(ptInfo);
					break;
				}
			case RECORD_CMD_SET_PLAN:
				{
					int iChnID = *((int*)(tMqBuf.data));
					RecPlan *ptInfo = (RecPlan*)(tMqBuf.data + sizeof(int));
					LOG_DEBUG_FMT("[SET PLAN] Channel%d", iChnID);
					char szMon[256] = {0};
					for (int i = 0; i < MAX_REC_PLAN_NUM; i++)
					{
						if (ptInfo->tMon[i].iStartTime != ptInfo->tMon[i].iEndTime)
						{
							int iH1 = ptInfo->tMon[i].iStartTime/60/60;
							int iM1 = ptInfo->tMon[i].iStartTime/60%60;
							int iS1 = ptInfo->tMon[i].iStartTime%60;
							int iH2 = ptInfo->tMon[i].iEndTime/60/60;
							int iM2 = ptInfo->tMon[i].iEndTime/60%60;
							int iS2 = ptInfo->tMon[i].iEndTime%60;
							sprintf(szMon+strlen(szMon), "(%02d:%02d:%02d -- %02d:%02d:%02d) ",
							iH1,iM1,iS1, iH2,iM2,iS2);
						}
					}
					LOG_DEBUG_FMT("Mon:%s", szMon);
					char szTue[256] = {0};
					for (int i = 0; i < MAX_REC_PLAN_NUM; i++)
					{
						if (ptInfo->tTue[i].iStartTime != ptInfo->tTue[i].iEndTime)
						{
							int iH1 = ptInfo->tTue[i].iStartTime/60/60;
							int iM1 = ptInfo->tTue[i].iStartTime/60%60;
							int iS1 = ptInfo->tTue[i].iStartTime%60;
							int iH2 = ptInfo->tTue[i].iEndTime/60/60;
							int iM2 = ptInfo->tTue[i].iEndTime/60%60;
							int iS2 = ptInfo->tTue[i].iEndTime%60;
							sprintf(szTue+strlen(szTue), "(%02d:%02d:%02d -- %02d:%02d:%02d) ",
								iH1,iM1,iS1, iH2,iM2,iS2);
						}
					}
					LOG_DEBUG_FMT("Tue:%s", szTue);
					char szWed[256] = {0};
					for (int i = 0; i < MAX_REC_PLAN_NUM; i++)
					{
						if (ptInfo->tWed[i].iStartTime != ptInfo->tWed[i].iEndTime)
						{
							int iH1 = ptInfo->tWed[i].iStartTime/60/60;
							int iM1 = ptInfo->tWed[i].iStartTime/60%60;
							int iS1 = ptInfo->tWed[i].iStartTime%60;
							int iH2 = ptInfo->tWed[i].iEndTime/60/60;
							int iM2 = ptInfo->tWed[i].iEndTime/60%60;
							int iS2 = ptInfo->tWed[i].iEndTime%60;
							sprintf(szWed+strlen(szWed), "(%02d:%02d:%02d -- %02d:%02d:%02d) ",
								iH1,iM1,iS1, iH2,iM2,iS2);
						}
					}
					LOG_DEBUG_FMT("Wed:%s", szWed);
					char szThu[256] = {0};
					for (int i = 0; i < MAX_REC_PLAN_NUM; i++)
					{
						if (ptInfo->tThu[i].iStartTime != ptInfo->tThu[i].iEndTime)
						{
							int iH1 = ptInfo->tThu[i].iStartTime/60/60;
							int iM1 = ptInfo->tThu[i].iStartTime/60%60;
							int iS1 = ptInfo->tThu[i].iStartTime%60;
							int iH2 = ptInfo->tThu[i].iEndTime/60/60;
							int iM2 = ptInfo->tThu[i].iEndTime/60%60;
							int iS2 = ptInfo->tThu[i].iEndTime%60;
							sprintf(szThu+strlen(szThu), "(%02d:%02d:%02d -- %02d:%02d:%02d) ",
								iH1,iM1,iS1, iH2,iM2,iS2);
						}
					}
					LOG_DEBUG_FMT("Thu:%s", szThu);
					char szFri[256] = {0};
					for (int i = 0; i < MAX_REC_PLAN_NUM; i++)
					{
						if (ptInfo->tFri[i].iStartTime != ptInfo->tFri[i].iEndTime)
						{
							int iH1 = ptInfo->tFri[i].iStartTime/60/60;
							int iM1 = ptInfo->tFri[i].iStartTime/60%60;
							int iS1 = ptInfo->tFri[i].iStartTime%60;
							int iH2 = ptInfo->tFri[i].iEndTime/60/60;
							int iM2 = ptInfo->tFri[i].iEndTime/60%60;
							int iS2 = ptInfo->tFri[i].iEndTime%60;
							sprintf(szFri+strlen(szFri), "(%02d:%02d:%02d -- %02d:%02d:%02d) ",
								iH1,iM1,iS1, iH2,iM2,iS2);
						}
					}
					LOG_DEBUG_FMT("Fri:%s", szFri);
					char szSat[256] = {0};
					for (int i = 0; i < MAX_REC_PLAN_NUM; i++)
					{
						if (ptInfo->tSat[i].iStartTime != ptInfo->tSat[i].iEndTime)
						{
							int iH1 = ptInfo->tSat[i].iStartTime/60/60;
							int iM1 = ptInfo->tSat[i].iStartTime/60%60;
							int iS1 = ptInfo->tSat[i].iStartTime%60;
							int iH2 = ptInfo->tSat[i].iEndTime/60/60;
							int iM2 = ptInfo->tSat[i].iEndTime/60%60;
							int iS2 = ptInfo->tSat[i].iEndTime%60;
							sprintf(szSat+strlen(szSat), "(%02d:%02d:%02d -- %02d:%02d:%02d) ",
								iH1,iM1,iS1, iH2,iM2,iS2);
						}
					}
					LOG_DEBUG_FMT("Sat:%s", szSat);
					char szSun[256] = {0};
					for (int i = 0; i < MAX_REC_PLAN_NUM; i++)
					{
						if (ptInfo->tSun[i].iStartTime != ptInfo->tSun[i].iEndTime)
						{
							int iH1 = ptInfo->tSun[i].iStartTime/60/60;
							int iM1 = ptInfo->tSun[i].iStartTime/60%60;
							int iS1 = ptInfo->tSun[i].iStartTime%60;
							int iH2 = ptInfo->tSun[i].iEndTime/60/60;
							int iM2 = ptInfo->tSun[i].iEndTime/60%60;
							int iS2 = ptInfo->tSun[i].iEndTime%60;
							sprintf(szSun+strlen(szSun), "(%02d:%02d:%02d -- %02d:%02d:%02d) ",
								iH1,iM1,iS1, iH2,iM2,iS2);
						}
					}
					LOG_DEBUG_FMT("Sun:%s", szSun);
					RecordMan::GetInstance()->SetPlan(iChnID, ptInfo);
					break;
				}
			case RECORD_CMD_SET_ENABLE:
				{
					int iChnID   = *((int*)(tMqBuf.data));
					bool bEnable = *((bool*)(tMqBuf.data + sizeof(int)));
					LOG_DEBUG_FMT("[SET ENABLE] Channel%d, Enable:%d", iChnID, bEnable);
					RecordMan::GetInstance()->SetEnable(iChnID, bEnable);
					break;
				}
			case RECORD_CMD_SET_ROOTPATH:
				{
					const char * pszRootPath = (const char *)(tMqBuf.data);
					LOG_DEBUG_FMT("[SET ROOTPATH] %s", pszRootPath);
					RecordMan::GetInstance()->SetRootPath(pszRootPath);
					break;
				}
			case RECORD_CMD_SET_FULLSTAT:
				{
					const StoreFullStatusT * ptInfo = (const StoreFullStatusT *)(tMqBuf.data);
					LOG_DEBUG_FMT("[SET FULLSTAT] Full:%d How2DO:%d", ptInfo->bFull, ptInfo->iHow2DO);
					RecordMan::GetInstance()->SetFullStatus(ptInfo);
					break;
				}
			default:
				{
					LOG_WARNS_FMT("Unknown CMD, %d!\n",tMqBuf.lCmd);
					break;
				}
			};
		}
	}

	RecordMan::UnInitialize();
	MQ_Record_Close();
	LogUninit();
	return 0;
}

