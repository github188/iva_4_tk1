#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "oal_log.h"
#include "mq_onvif.h"
#include "mq_videoin.h"
#include "mq_master.h"
#include "OnvifMan.h"

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

int main()
{
	LogInit("ivaOnvif", 10*1024*1024);// 日志最多10M

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
	int iOnvifMqId = MQ_Onvif_Init();
	if(iOnvifMqId < 0)
	{
		LOG_ERROR_FMT("MQ_Onvif_Init fail.");
		return 0;
	}

	MQ_VideoIn_Init();
	MQ_Master_Init();

	OnvifManager::Initialize();

	LOG_INFOS_FMT("******************ivaOnvif Start******************");

	MQ_BUF tMqBuf;
	while(1)
	{
		memset(&tMqBuf, 0, sizeof(MQ_BUF));

		int iRecvLen = Msg_Recv(iOnvifMqId, MSG_TYPE_ONVIF, &tMqBuf, sizeof(MQ_BUF));
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
			case ONVIF_CMD_SET_DEV:// iChnID+VISource
				{
					int iChnID = *((int *)(tMqBuf.data));
					VISource *ptInfo = (VISource*)(tMqBuf.data + sizeof(int));

					LOG_DEBUG_FMT("[SET SOURCE] Channel%d", iChnID);
					LOG_DEBUG_FMT("Enable  : %d",ptInfo->bEnable);
					LOG_DEBUG_FMT("Company : %d",ptInfo->iManuCode);
					LOG_DEBUG_FMT("Stream  : %d",ptInfo->iStreamType);
					LOG_DEBUG_FMT("Addr    : %s",ptInfo->szIP);
					LOG_DEBUG_FMT("User    : %s",ptInfo->szUser);
					LOG_DEBUG_FMT("Pass    : %s",ptInfo->szPass);

					OnvifManager::GetInstance()->SetDevInfo(iChnID, ptInfo);
					break;
				}
			case ONVIF_CMD_SET_PROXY:
				{
					int iChnID = *((int *)(tMqBuf.data));
					ClickZoomProxy *ptInfo = (ClickZoomProxy*)(tMqBuf.data + sizeof(int));

					LOG_DEBUG_FMT("[SET PROXY] Channel%d", iChnID);
					LOG_DEBUG_FMT("Model  : %d",ptInfo->iModel);
					LOG_DEBUG_FMT("Addr : %s:%d",ptInfo->szAddr, ptInfo->iPort);

					OnvifManager::GetInstance()->SetClickZoomProxy(iChnID, ptInfo);
					break;
				}
			case ONVIF_CMD_SET_TIMESYN:
				{
					OnvifTimeSyn *ptInfo = (OnvifTimeSyn*)(tMqBuf.data);

					LOG_DEBUG_FMT("[SET TIMESYN]");
					LOG_DEBUG_FMT("Enable   : %d",ptInfo->bEnable);
					LOG_DEBUG_FMT("SynChn   : %d",ptInfo->iSynChnID);
					LOG_DEBUG_FMT("Interval : %u",ptInfo->uInterval);

					OnvifManager::GetInstance()->SetTimeSynInfo(ptInfo);
					break;
				}
			case ONVIF_CMD_PTZ_MOVE:
				{
					int *data = (int *)(tMqBuf.data);
					int iChnID  = data[0];
					int iAction = data[1];
					int iSpeed  = data[2];
					LOG_DEBUG_FMT("[PTZ MOVE] Channel%d Action:%d Speed:%d",iChnID, iAction, iSpeed);
					OnvifManager::GetInstance()->Onvif_Ptz_Move(iChnID, iAction, iSpeed);
					break;
				}
			case ONVIF_CMD_PTZ_STOP:
				{
					int iChnID  = *((int *)(tMqBuf.data));
					LOG_DEBUG_FMT("[MOVE STOP] Channel%d",iChnID);
					OnvifManager::GetInstance()->Onvif_Ptz_Stop(iChnID);
					break;
				}
			case ONVIF_CMD_APERTURE_MOVE:
				{
					int *data = (int *)(tMqBuf.data);
					int iChnID  = data[0];
					int iAction = data[1];
					int iSpeed  = data[2];
					LOG_DEBUG_FMT("[APERTURE MOVE] Channel%d Action:%d Speed:%d",iChnID, iAction, iSpeed);
					OnvifManager::GetInstance()->Onvif_Aperture_Move(iChnID, iAction, iSpeed);
					break;
				}
			case ONVIF_CMD_APERTURE_STOP:
				{
					int iChnID  = *((int *)(tMqBuf.data));
					LOG_DEBUG_FMT("[APERTURE STOP] Channel%d",iChnID);
					OnvifManager::GetInstance()->Onvif_Aperture_Stop(iChnID);
					break;
				}
			case ONVIF_CMD_FOCUS_MOVE:
				{
					int *data = (int *)(tMqBuf.data);
					int iChnID  = data[0];
					int iAction = data[1];
					int iSpeed  = data[2];
					LOG_DEBUG_FMT("[FOCUS MOVE] Channel%d Action:%d Speed:%d",iChnID, iAction, iSpeed);
					OnvifManager::GetInstance()->Onvif_Focus_Move(iChnID, iAction, iSpeed);
					break;
				}
			case ONVIF_CMD_FOCUS_STOP:
				{
					int iChnID  = *((int *)(tMqBuf.data));
					LOG_DEBUG_FMT("[FOCUS STOP] Channel%d",iChnID);
					OnvifManager::GetInstance()->Onvif_Focus_Stop(iChnID);
					break;
				}
			case ONVIF_CMD_GET_PRESETS:
				{
					int iChnID  = *((int *)(tMqBuf.data));
					LOG_DEBUG_FMT("[GET PRESETS] Channel%d",iChnID);
					
					PresetArray *ptPresets =  (PresetArray*)malloc(sizeof(PresetArray));
					int iRet = OnvifManager::GetInstance()->Onvif_GetPresets(iChnID, ptPresets);

					MQ_Onvif_Answer_Presets(tMqBuf.lSrc, iRet, ptPresets);

					if (ptPresets)
					{
						free(ptPresets);
						ptPresets = NULL;
					}
					break;
				}
			case ONVIF_CMD_PRESET:
				{
					int *data = (int *)(tMqBuf.data);
					int iChnID  = data[0];
					int iAction = data[1];
					int iPresetID  = data[2];
					
					if (iAction == PRESET_GOTO)
					{
						LOG_DEBUG_FMT("[GOTO PRESET] Channel%d PresetID:%d",iChnID, iPresetID);
						OnvifManager::GetInstance()->Onvif_GoToPreset(iChnID, iPresetID);
					}
					else if (iAction == PRESET_SET)
					{
						LOG_DEBUG_FMT("[SET PRESET] Channel%d PresetID:%d",iChnID, iPresetID);
						OnvifManager::GetInstance()->Onvif_SetPreset(iChnID, iPresetID);
					}
					else if (iAction == PRESET_DEL)
					{
						LOG_DEBUG_FMT("[DEL PRESET] Channel%d PresetID:%d",iChnID, iPresetID);
						OnvifManager::GetInstance()->Onvif_DelPreset(iChnID, iPresetID);
					}
					else
					{
						LOG_ERROR_FMT("[PRESET] Channel%d PresetID:%d iAction=%d unknown",iChnID, iPresetID,iAction);
					}
					break;
				}
			case ONVIF_CMD_GET_ABS:
				{
					int iChnID  = *((int *)(tMqBuf.data));
					LOG_DEBUG_FMT("[GET ABS PTZ] Channel%d",iChnID);

					AbsPosition tAbsPos = {0};
					int iRet = OnvifManager::GetInstance()->Onvif_GetAbsPos(iChnID, tAbsPos);

					MQ_Onvif_Answer_AbsPos(tMqBuf.lSrc, iRet, &tAbsPos);

					break;
				}
			case ONVIF_CMD_ABS_MOVE:
				{
					int iChnID  = *((int *)(tMqBuf.data));
					AbsPosition *ptAbsPos = (AbsPosition*)(tMqBuf.data + sizeof(int));
					LOG_DEBUG_FMT("[ABS PTZ MOVE] Channel%d Pan:%f, Tilt:%f, Zoom:%f", iChnID, 
						ptAbsPos->fPan, ptAbsPos->fTilt, ptAbsPos->fZoom);

					OnvifManager::GetInstance()->Onvif_AbsMove(iChnID, ptAbsPos);
					break;
				}
			case ONVIF_CMD_CLICKZOOM:
				{
					int iChnID  = *((int *)(tMqBuf.data));
					ClickArea *ptArea = (ClickArea*)(tMqBuf.data + sizeof(int));
					LOG_DEBUG_FMT("[3D ZOOM] Channel%d StartPoint:(%d, %d) EndPoint:(%d, %d)", iChnID,
						ptArea->StartPoint.x,ptArea->StartPoint.y,
						ptArea->EndPoint.x,ptArea->EndPoint.y);
					OnvifManager::GetInstance()->Onvif_ClickZoom(iChnID, ptArea);
					break;
				}
			default:
				LOG_WARNS_FMT("Unknown CMD, %d!",tMqBuf.lCmd);
				break;
			}
		}
	}

	OnvifManager::UnInitialize();
	MQ_Onvif_Close();
#ifdef WIN32
	WSACleanup();
#endif
	LogUninit();
	return 0;
}

