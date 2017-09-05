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
	/*�쳣��¼��д���ļ��� */
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
	LogInit("ivaDecode", 10*1024*1024);// ��־���10M

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



