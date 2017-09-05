#include <stdio.h>
#include <stdlib.h>
#include "oal_log.h"
#include "mq_videoin.h"
#include "mq_onvif.h"
#include "mq_master.h"
#include "mq_decode.h"
#include "mq_record.h"
#include "mq_videoout.h"
#include "StreamMan.h"

#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "WS2_32")
#else
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
	FILE *fp = fopen("./Logfiles/videoin_execption.txt", "ab+");

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
	LogInit("ivaVideoIn", 10*1024*1024);// 日志最多10M

#ifdef WIN32
	WSADATA wd;
	if(WSAStartup(MAKEWORD(2,2),&wd)  != 0)
	{
		printf("WSAStartup error！");
		return 1;
	}
#else
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

	int iVideoInMqId = MQ_VideoIn_Init();
	if(iVideoInMqId < 0)
	{
		LOG_ERROR_FMT("MQ_VideoIn_Init fail.\n");
		return 0;
	}

	MQ_Onvif_Init();
	MQ_Master_Init();
	int iDecodeMqId = MQ_Decode_Init();
	int iRecordMqId = MQ_Record_Init();
	int iVideoOutMqId = MQ_VideoOut_Init();

	StreamMan::Initialize();

	if(Global_BuilldEdition()!=FSS_EDITION)	{
		Start_Next_Service(iDecodeMqId, MSG_TYPE_DECODE, MSG_TYPE_VIDEOIN, DECODE_CMD_START);
	}
	Start_Next_Service(iRecordMqId, MSG_TYPE_RECORD, MSG_TYPE_VIDEOIN, RECORD_CMD_START);
	Start_Next_Service(iVideoOutMqId, MSG_TYPE_VIDEOOUT, MSG_TYPE_VIDEOIN, VO_CMD_START);

	LOG_INFOS_FMT("******************ivaVideoIn Start******************");

	MQ_BUF tMqBuf;
	while(1)
	{
		memset(&tMqBuf, 0, sizeof(MQ_BUF));

		int iRecvLen = Msg_Recv(iVideoInMqId, MSG_TYPE_VIDEOIN, &tMqBuf, sizeof(MQ_BUF));
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
			case VI_CMD_SET_RTSP:
				{
					int iChnID = *((int *)(tMqBuf.data));
					RtspInfo *ptInfo = (RtspInfo*)(tMqBuf.data + sizeof(int));
					LOG_DEBUG_FMT("[SET RTSP] Channel%d", iChnID);
					LOG_DEBUG_FMT("Solution: %d * %d",ptInfo->iWidth, ptInfo->iHeight);
					LOG_DEBUG_FMT("RtspUrl : %s",ptInfo->szUrl);
					StreamMan::GetInstance()->SetChnRtspInfo(iChnID, ptInfo);
					break;
				}
			default:
				{
					LOG_WARNS_FMT("Unknown CMD, %d!\n",tMqBuf.lCmd);
					break;
				}
			}
		}
	}

	StreamMan::UnInitialize();
	MQ_VideoIn_Close();
#ifdef WIN32
	WSACleanup();
#endif
	LogUninit();
	return 0;
}


