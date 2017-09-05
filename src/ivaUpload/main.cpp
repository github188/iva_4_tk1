#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oal_log.h"
#include "mq_upload.h"
#include "mq_master.h"
#include "mq_database.h"
#include "UploadMan.h"
#include "Preview/PreviewServer.h"

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
	FILE *fp = fopen("./Logfiles/upload_execption.txt", "ab+");

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
#include "Notify/NotifyMan.h"
#include "oal_unicode.h"

int main(int argc, char **argv)
{
	LogInit("ivaUpload", 10*1024*1024);// 日志最多10M

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

	int iUploadMqId = MQ_Upload_Init();
	if(iUploadMqId < 0)
	{
		LOG_ERROR_FMT("MQ_Upload_Init fail.\n");
		return 0;
	}

	MQ_Master_Init();
	MQ_Database_Init();

	UploadMan::Initialize();
	PreviewClientMan::Initialize();
	NotifyMan::Initialize();

	LOG_INFOS_FMT("******************ivaUpload Start******************");

	MQ_BUF tMqBuf;
	while(1)
	{
		memset(&tMqBuf, 0, sizeof(MQ_BUF));
		int iRecvLen = Msg_Recv(iUploadMqId, MSG_TYPE_UPLOAD, &tMqBuf, sizeof(MQ_BUF));
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
			case UPLOAD_CMD_STRATEGY:
				{
					UploadStrategy *ptStrategy = (UploadStrategy*)tMqBuf.data;
					LOG_DEBUG_FMT("[UPLOAD STRATEGY] Model:%d  DeleteOK:%d", 
						ptStrategy->iModel, ptStrategy->bDelOk);

					if(ptStrategy->iModel == UPLOAD_TO_PLATFORM)
					{
						if(ptStrategy->tPlatform.iPort == 0)
						{
							LOG_DEBUG_FMT("\tPlatform:%s, ID:%s",
								ptStrategy->tPlatform.szAddr,
								ptStrategy->tPlatform.szPlatID);
						}
						else
						{
							LOG_DEBUG_FMT("\tPlatform:%s:%d, ID:%s",
								ptStrategy->tPlatform.szAddr,
								ptStrategy->tPlatform.iPort,
								ptStrategy->tPlatform.szPlatID);
						}
					}
					else if (ptStrategy->iModel == UPLOAD_TO_MQTT)
					{
						if(ptStrategy->tMqttSvr.iPort == 0)
						{
							LOG_DEBUG_FMT("\tMQTT Server:%s, ID:%s",
								ptStrategy->tMqttSvr.szAddr,
								ptStrategy->tMqttSvr.szPlatID);
						}
						else
						{
							LOG_DEBUG_FMT("\tMQTT Server:%s:%d, ID:%s",
								ptStrategy->tMqttSvr.szAddr,
								ptStrategy->tMqttSvr.iPort,
								ptStrategy->tMqttSvr.szPlatID);
						}
					}
					else
					{
						LOG_DEBUG_FMT("\tFTP:%s:%d@%s:%s", 
							ptStrategy->tFtpInfo.szAddr,ptStrategy->tFtpInfo.iPort,
							ptStrategy->tFtpInfo.szUser,ptStrategy->tFtpInfo.szPass);
					}

					UploadMan::GetInstance()->SetUploadStrategy(ptStrategy);
					break;
				}
			case UPLOAD_CMD_ENABLE:
				{
					bool bEnable = *((bool*)tMqBuf.data);
					LOG_DEBUG_FMT("[UPLOAD ENABLE] %d", bEnable);
					UploadMan::GetInstance()->SetUploadEnable(bEnable);
					break;
				}
			case UPLOAD_CMD_FTPADV:
				{
					FtpAdvance *ptAdv = (FtpAdvance*)tMqBuf.data;
					LOG_DEBUG_FMT("[UPLOAD FTP ADVANCE] Flag:%d Charset:%s",ptAdv->iContentFlag, ptAdv->iCharSet==eCharset_UTF8?"UTF-8":"GB2312");
					LOG_DEBUG_FMT("\tIllegalDir:%s", ptAdv->szIllegalDirGrammar);
					LOG_DEBUG_FMT("\tEventDir:%s", ptAdv->szEventDirGrammar);
					LOG_DEBUG_FMT("\tTollDir:%s", ptAdv->szTollDirGrammar);
					UploadMan::GetInstance()->SetUploadFtpAdv(ptAdv);
					break;
				}
			case UPLOAD_CMD_RESULT_PREVIEW:
				{
					AnalyResult *ptResult = (AnalyResult*)tMqBuf.data;

					char szEventDesc[1024] = {0};
					char szPlate[PLATE_LEN] = {0}; 
					char szPlateColor[16] = {0};

					UTF8_To_GB2312(ptResult->szPlate, strlen(ptResult->szPlate), szPlate, sizeof(szPlate));
					UTF8_To_GB2312(ptResult->szPlateColor, strlen(ptResult->szPlateColor), szPlateColor, sizeof(szPlateColor));
					UTF8_To_GB2312(ptResult->szEventDesc, strlen(ptResult->szEventDesc), szEventDesc, sizeof(szEventDesc));

					LOG_DEBUG_FMT("[RESULT PREVIEW] ID:%d ChnID:%d  Plate:%s Color:%s %s", 
						ptResult->iEventID,
						ptResult->iChnID, szPlate, szPlateColor,szEventDesc);

					PreviewClientMan::GetInstance()->SendAnalyResultPreivew(ptResult);					
					break;
				}
			case UPLOAD_CMD_TRACK_PREVIEW:
				{
					int iChnID = *((int *)(tMqBuf.data));
					AnalyTracks *ptTrack = (AnalyTracks *)(tMqBuf.data+sizeof(int));
					PreviewClientMan::GetInstance()->SendAnalyTrackPreivew(iChnID,ptTrack);					
					break;
				}
			case UPLOAD_CMD_NOTIFY_SERVER:
				{
					NotifySvr *ptSvr = (NotifySvr *)tMqBuf.data;
					LOG_DEBUG_FMT("[NOTIFY SERVER] %s:%d AttachImg:%d", 
						ptSvr->szAddr, ptSvr->iPort, ptSvr->bAttachImg);
					NotifyMan::GetInstance()->SetServer(ptSvr);					
					break;
				}
			case UPLOAD_CMD_NOTIFY_ENABLE:
				{
					bool bEnable = *((bool*)tMqBuf.data);
					LOG_DEBUG_FMT("[NOTIFY ENABLE] %d", bEnable);
					NotifyMan::GetInstance()->SetEnable(bEnable);
					break;
				}
			case UPLOAD_CMD_NOTIFY:
				{
					AnalyNotify *ptNotifyData = (AnalyNotify *)(tMqBuf.data);
					LOG_DEBUG_FMT("[NOTIFY]");
					NotifyMan::GetInstance()->AddNewNotify(ptNotifyData);					
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

	NotifyMan::UnInitialize();
	PreviewClientMan::UnInitialize();
	UploadMan::UnInitialize();
	MQ_Upload_Close();
#ifdef WIN32
	WSACleanup();
#endif
	LogUninit();
	return 0;
}
