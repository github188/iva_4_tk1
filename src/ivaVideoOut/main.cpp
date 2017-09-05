#include "oal_log.h"
#include "mq_videoout.h"
#include "RtmpMan.h"
#include "RtspMan.h"
#include "H264Man.h"


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
	FILE *fp = fopen("./Logfiles/videoout_execption.txt", "ab+");

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
	LogInit("ivaVideoOut", 10*1024*1024);// 日志最多10M

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

	int iVideoOutMqId = MQ_VideoOut_Init();
	if(iVideoOutMqId < 0)
	{
		LOG_ERROR_FMT("MQ_VideoOut_Init fail.\n");
		return 0;
	}

	LOG_INFOS_FMT("Wait_Service_Start VO_CMD_START");
	Wait_Service_Start(iVideoOutMqId, MSG_TYPE_VIDEOOUT, VO_CMD_START);
	LOG_INFOS_FMT("Wait_Service_Start VO_CMD_START OK");

	H264Man::Initialize();
	RtmpMan::Initialize();
	RtspMan::Initialize();

	LOG_INFOS_FMT("******************ivaVideoOut Start******************");

	MQ_BUF tMqBuf;
	while(1)
	{
		memset(&tMqBuf, 0, sizeof(MQ_BUF));

		int iRecvLen = Msg_Recv(iVideoOutMqId, MSG_TYPE_VIDEOOUT, &tMqBuf, sizeof(MQ_BUF));
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
			case VO_CMD_SET_STRATEGY:
				{
					VoStrategy *ptInfo = (VoStrategy*)(tMqBuf.data);
					LOG_DEBUG_FMT("[SET STRATEGY] Protocol: %s", ptInfo->iProtocol==VO_PROTOCOL_RTMP?"RTMP":
						(ptInfo->iProtocol==VO_PROTOCOL_RTSP?"RTSP":"NO"));

					RtmpMan::GetInstance()->SetStrategy(ptInfo);
					RtspMan::GetInstance()->SetStrategy(ptInfo);
					break;
				}
			case VO_CMD_SET_RTMPSERVER:
				{
					int iChnID = *((int*)(tMqBuf.data));
					RtmpServer *ptInfo = (RtmpServer*)(tMqBuf.data + sizeof(int));
					LOG_DEBUG_FMT("[SET SERVER] Channel%d URL:%s", iChnID, ptInfo->szUrl);

					RtmpMan::GetInstance()->SetServerInfo(iChnID, ptInfo);
					break;
				}
			case VO_CMD_SET_ENABLE:
				{
					int iChnID   = *((int*)(tMqBuf.data));
					bool bEnable = *((bool*)(tMqBuf.data + sizeof(int)));
					LOG_DEBUG_FMT("[SET ENABLE] Channel%d, Enable:%d", iChnID, bEnable);
					RtmpMan::GetInstance()->SetEnable(iChnID, bEnable);
					RtspMan::GetInstance()->SetEnable(iChnID, bEnable);
					break;
				}
			case VO_CMD_SOURCE_CHANGED:
				{
					int iChnID  = *((int*)(tMqBuf.data));
					RecSrcStat *ptStat = (RecSrcStat*)(tMqBuf.data + sizeof(int));
					LOG_DEBUG_FMT("[SOURCE CHANGED] Channel%d Enable:%d Changed:%d", iChnID,ptStat->bEnable, ptStat->bChanged);

					//int iRecStatus = 0;
					if (!ptStat->bEnable)
					{
						//RtmpMan::GetInstance()->SetEnable(iChnID, false);
					}
					else// if (ptStat->bChanged)
					{
						//iRecStatus = RTMP_NO_H264INFO;
						//RtmpMan::GetInstance()->Reset(iChnID);
					}
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

	RtmpMan::UnInitialize();
	RtspMan::UnInitialize();
	H264Man::UnInitialize();
	MQ_VideoOut_Close();
	LogUninit();
	return 0;
}

