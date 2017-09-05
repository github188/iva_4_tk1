#include "oal_log.h"
#include "mq_decode.h"
#include "mq_analysis.h"
#include "mq_master.h"
#include "DecodeMan.h"

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
	FILE *fp = fopen("./Logfiles/decode_execption.txt", "ab+");

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
	LogInit("ivaDecode", 10*1024*1024);// 日志最多10M

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

	int iDecodeMqId = MQ_Decode_Init();
	if(iDecodeMqId < 0)
	{
		LOG_ERROR_FMT("MQ_Decode_Init fail.");
		return 0;
	}

	int iAnalyMqId = MQ_Analy_Init();
	MQ_Master_Init();

	LOG_INFOS_FMT("Wait_Service_Start DECODE_CMD_START");
	Wait_Service_Start(iDecodeMqId, MSG_TYPE_DECODE, DECODE_CMD_START);
	LOG_INFOS_FMT("Wait_Service_Start DECODE_CMD_START OK");

	DecodeMan::Initialize();

	if(Global_BuilldEdition()!=FSS_EDITION)	{
		Start_Next_Service(iAnalyMqId, MSG_TYPE_ANALY, MSG_TYPE_DECODE, ANALY_CMD_START);
	}

	LOG_INFOS_FMT("******************ivaDecode Start******************");

	MQ_BUF tMqBuf;
	while(1)
	{
		memset(&tMqBuf, 0, sizeof(MQ_BUF));

		int iRecvLen = Msg_Recv(iDecodeMqId, MSG_TYPE_DECODE, &tMqBuf, sizeof(MQ_BUF));
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
			case DECODE_CMD_VIDEO_INFO:
				{
					int iChnID  = *((int*)(tMqBuf.data));
					DecodeVideoInfo *ptVideo = (DecodeVideoInfo*)(tMqBuf.data + sizeof(int));
					LOG_DEBUG_FMT("[VIDEO INFO] Channel%d %d*%d %dfps", iChnID,
						ptVideo->iWidth, ptVideo->iHeight,ptVideo->iFrameRate);
					
					DecodeMan::GetInstance()->SetChnVideoInfo(iChnID, ptVideo);
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

	DecodeMan::UnInitialize();
	MQ_Decode_Close();
	LogUninit();
	return 0;
}



