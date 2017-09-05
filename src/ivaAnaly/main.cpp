#include "oal_log.h"
#include "oal_time.h"
#include "mq_onvif.h"
#include "mq_database.h"
#include "mq_upload.h"
#include "mq_master.h"
#include "mq_analysis.h"
#include "AnalyDispatcher.h"
#include "AnalyCfgMan.h"

extern char g_HANZI_LIST[31][3];

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
	FILE *fp = fopen("./Logfiles/analy_execption.txt", "ab+");

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
	LogInit("ivaAnaly", 10*1024*1024);// 日志最多10M

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

	int iAnalyMqId = MQ_Analy_Init();
	if(iAnalyMqId < 0)
	{
		LOG_ERROR_FMT("MQ_Analy_Init fail.\n");
		return 0;
	}

	MQ_Onvif_Init();
	MQ_Database_Init();
	MQ_Upload_Init();
	MQ_Master_Init();

	LOG_INFOS_FMT("Wait_Service_Start ANALY_CMD_START");
	Wait_Service_Start(iAnalyMqId, MSG_TYPE_ANALY, ANALY_CMD_START);
	LOG_INFOS_FMT("Wait_Service_Start ANALY_CMD_START OK");

	// 初始化算法调度
	AnalyDispatcher::Initialize();

	LOG_INFOS_FMT("******************ivaAnaly Start******************");

	MQ_BUF tMqBuf;
	while(1)
	{
		memset(&tMqBuf, 0, sizeof(MQ_BUF));

		int iRecvLen = Msg_Recv(iAnalyMqId, MSG_TYPE_ANALY, &tMqBuf, sizeof(MQ_BUF));
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
			case ANALY_CMD_DEVICEINFO:
				{
					DeviceInfoT * ptInfo = (DeviceInfoT *)tMqBuf.data;
					AnalyCfgMan::GetInstance()->SetDevInfo(ptInfo);
					printf("[设备信息]\n");
					printf("\t入网号: %s\n", ptInfo->szNetID);
					printf("\t注册号: %s\n", ptInfo->szRegID);
					printf("\t设备号: %s\n", ptInfo->szDevID);
					printf("\t厂商号: %s\n", ptInfo->szManID);
					break;
				}
			case ANALY_CMD_LICENSE:
				{
					AuthStatusT * ptInfo = (AuthStatusT *)tMqBuf.data;
					AnalyCfgMan::GetInstance()->SetAuthInfo(ptInfo);
					printf("[授权信息] %s\n", ptInfo->eAuthType==eProhibit?"未授权":(ptInfo->eAuthType==ePermanent?"永久授权":"临时授权"));
					if(ptInfo->eAuthType==eTemporary)
					{
						char buf[40] = {0};
						TimeFormatString(ptInfo->tTimeOut, buf,40, eYMD1);
						printf("\t授权到期: %s\n", buf);
					}
					break;
				}
			case ANALY_CMD_ROOTPATH:
				{
					const char * pszPath = (const char *)tMqBuf.data;
					AnalyCfgMan::GetInstance()->SetRootPath(pszPath);
					printf("[存储目录] %s\n", pszPath);
					break;
				}
			case ANALY_CMD_STORE_FULL:
				{
					StoreFullStatusT * ptInfo = (StoreFullStatusT *)tMqBuf.data;
					AnalyCfgMan::GetInstance()->SetStoreFull(ptInfo);
					printf("[存储状态] %s %s\n", ptInfo->bFull?"已满":"未满",
						ptInfo->iHow2DO==eFull2CyclicCovering?"循环覆盖":(
						ptInfo->iHow2DO==eFull2StopAnaly?"停止分析":"停止写入"));
					break;
				}
			case ANALY_CMD_LPR:
				{
					LPRCfg * ptInfo = (LPRCfg *)tMqBuf.data;
					AnalyCfgMan::GetInstance()->SetLprCfg(ptInfo);
					printf("[车牌识别]\n");
					printf("\t默认省份:%s 置信度:%d%% 最小尺寸:%d\n", g_HANZI_LIST[ptInfo->iDefProv%31], ptInfo->iHanziConf, ptInfo->iMinSize);
					printf("\t重复间隔:%d 重复精度:%d\n", ptInfo->iDupTime,ptInfo->iDupSens);
					break;
				}
			case ANALY_CMD_DISPATCH:
				{
					DispatchCtrlT * ptInfo = (DispatchCtrlT *)tMqBuf.data;
					AnalyCfgMan::GetInstance()->SetDispatchCtrl(ptInfo);

					printf("[调度策略]\n");
					printf("\t灵敏度:%s\n", 
						ptInfo->iSensitivity==eSenFast?"快速":(
						ptInfo->iSensitivity==eSenAccuracy?"准确":"平衡"));
					printf("\t优先级:%s\n", 
						ptInfo->iPriority==ePriByScene?"场景优先":"目标优先");
					printf("\t最小停留:%d\n", ptInfo->iSceneMinStay);
					printf("\t重复抓拍:%d\n", ptInfo->iRepeatTime);
					break;
				}
			case ANALY_CMD_CHN_OPEN:
				{
					int iChnID = *((int *)(tMqBuf.data));
					bool bEnable = *((bool *)(tMqBuf.data+sizeof(int)));
					AnalyCfgMan::GetInstance()->SetChnAlgOpen(iChnID, bEnable);
					printf("[分析开关] CHN:%d %s\n",iChnID, bEnable?"开启":"关闭");
					break;
				}
			case ANALY_CMD_CHN_PLAN:
				{
					int iChnID = *((int *)(tMqBuf.data));
					AnalyPlan *ptInfo = (AnalyPlan *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetChnPlan(iChnID, ptInfo);
					printf("[分析计划] CHN:%d\n",iChnID);
					char szMon[256] = {0};
					for (int i = 0; i < MAX_ANALY_PLAN_NUM; i++)
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
					printf("\tMon:%s\n", szMon);
					char szTue[256] = {0};
					for (int i = 0; i < MAX_ANALY_PLAN_NUM; i++)
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
					printf("\tTue:%s\n", szTue);
					char szWed[256] = {0};
					for (int i = 0; i < MAX_ANALY_PLAN_NUM; i++)
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
					printf("\tWed:%s\n", szWed);
					char szThu[256] = {0};
					for (int i = 0; i < MAX_ANALY_PLAN_NUM; i++)
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
					printf("\tThu:%s\n", szThu);
					char szFri[256] = {0};
					for (int i = 0; i < MAX_ANALY_PLAN_NUM; i++)
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
					printf("\tFri:%s\n", szFri);
					char szSat[256] = {0};
					for (int i = 0; i < MAX_ANALY_PLAN_NUM; i++)
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
					printf("\tSat:%s\n", szSat);
					char szSun[256] = {0};
					for (int i = 0; i < MAX_ANALY_PLAN_NUM; i++)
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
					printf("\tSun:%s\n", szSun);
					break;
				}
			case ANALY_CMD_CHN_PLAN_OPEN:
				{
					int iChnID = *((int *)(tMqBuf.data));
					bool bEnable = *((bool *)(tMqBuf.data+sizeof(int)));
					AnalyCfgMan::GetInstance()->SetChnPlanOpen(iChnID, bEnable);
					printf("[计划开关] CHN:%d %s\n", iChnID, bEnable?"开启":"关闭");
					break;
				}
			case ANALY_CMD_CHN_NOTIFY_OPEN:
				{
					int iChnID = *((int *)(tMqBuf.data));
					bool bEnable = *((bool *)(tMqBuf.data+sizeof(int)));
					AnalyCfgMan::GetInstance()->SetChnNotifyOpen(iChnID, bEnable);
					printf("[通知开关] CHN:%d %s\n", iChnID, bEnable?"开启":"关闭");
					break;
				}
			case ANALY_CMD_CHN_PLACE:
				{
					int iChnID = *((int *)(tMqBuf.data));
					PlaceInfoT * ptInfo = (PlaceInfoT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetChnPlaceInfoT(iChnID, ptInfo);
					printf("[通道信息] CHN:%d\n",iChnID);
					printf("\t区域名称: %s\n", ptInfo->szAreaName);
					printf("\t区域编号: %s\n", ptInfo->szAreaID);
					printf("\t监控名称: %s\n", ptInfo->szMonitoryName);
					printf("\t监控编号: %s\n", ptInfo->szMonitoryID);
					printf("\t地点名称: %s\n", ptInfo->szPlaceName);
					printf("\t地点编号: %s\n", ptInfo->szPlaceID);
					break;
				}
			case ANALY_CMD_CHN_CLEAR:
				{
					int iChnID = *((int *)(tMqBuf.data));
					AnalyCfgMan::GetInstance()->ClearChnRuleAnaly(iChnID);
					printf("[清空配置] CHN:%d\n",iChnID);
					break;
				}
			case ANALY_CMD_CHN_MANUAL:
				{
					int iChnID = *((int *)(tMqBuf.data));
					ManualSnapT *ptInfo = (ManualSnapT *)(tMqBuf.data+sizeof(int));
					AnalyDispatcher::GetInstance()->ManualSnap(iChnID, ptInfo);
					printf("[手动抓拍] CHN:%d Event:%d Pos:(%d %d %d %d)\n",iChnID, ptInfo->iEntID, 
						ptInfo->tRect.x, ptInfo->tRect.y, ptInfo->tRect.w, ptInfo->tRect.h);
					break;
				}
			case ANALY_CMD_CHN_PRESET_POS:
				{
					int iChnID = *((int *)(tMqBuf.data));
					int iPresetID = *((int *)(tMqBuf.data+sizeof(int)));
					AbsPosition *ptInfo = (AbsPosition *)(tMqBuf.data+2*sizeof(int));
					AnalyCfgMan::GetInstance()->SetChnPresetPos(iChnID, iPresetID, ptInfo);
					printf("[预置位坐标] CHN:%d, Preset:%d\n", iChnID, iPresetID);
					printf("\tPan:%g Tilt:%g Zoom:%g\n", ptInfo->fPan, ptInfo->fTilt, ptInfo->fZoom);
					break;
				}
			case ANALY_CMD_CHN_SCENE_INFO:
				{
					int iChnID = *((int *)(tMqBuf.data));
					int iSceneID = *((int *)(tMqBuf.data+sizeof(int)));
					SceneInfoT * ptInfo = (SceneInfoT *)(tMqBuf.data+2*sizeof(int));
					AnalyCfgMan::GetInstance()->SetChnSceneInfo(iChnID, iSceneID, ptInfo);
					printf("[场景信息] CHN:%d, SCE:%d\n", iChnID, iSceneID);
					printf("\t预置位:%d\n", ptInfo->iPresetID);
					printf("\t时间:%ds\n", ptInfo->iStayTime);
					printf("\t方向:%d\n", ptInfo->iDirection);
					printf("\t名称:%s\n", ptInfo->szSceneName);
					break;
				}
			case ANALY_CMD_CHN_SCENE_RULE:
				{
					int iChnID = *((int *)(tMqBuf.data));
					int iSceneID = *((int *)(tMqBuf.data+sizeof(int)));
					AnalyRuleT * ptInfo = (AnalyRuleT *)(tMqBuf.data+2*sizeof(int));
					AnalyCfgMan::GetInstance()->SetChnSceneRule(iChnID, iSceneID, ptInfo);
					printf("[分析规则] CHN:%d, SCE:%d\n", iChnID, iSceneID);
					printf("\t场景标定:长%.2f 宽%.2f ",ptInfo->tCalib.length,ptInfo->tCalib.width);
					printf("(%d,%d) (%d,%d) (%d,%d) (%d,%d)\n",
						ptInfo->tCalib.atPoints[0].x,ptInfo->tCalib.atPoints[0].y,
						ptInfo->tCalib.atPoints[1].x,ptInfo->tCalib.atPoints[1].y,
						ptInfo->tCalib.atPoints[2].x,ptInfo->tCalib.atPoints[2].y,
						ptInfo->tCalib.atPoints[3].x,ptInfo->tCalib.atPoints[3].y);

					printf("\t目标大小:%s \n",ptInfo->tTargetSize.bEnable?"使能":"禁用");
					printf("\t最小目标(%d,%d) (%d,%d) 最大目标(%d,%d) (%d,%d)\n",
						ptInfo->tTargetSize.atMinPoints[0].x,ptInfo->tTargetSize.atMinPoints[0].y,
						ptInfo->tTargetSize.atMinPoints[1].x,ptInfo->tTargetSize.atMinPoints[1].y,
						ptInfo->tTargetSize.atMaxPoints[0].x,ptInfo->tTargetSize.atMaxPoints[0].y,
						ptInfo->tTargetSize.atMaxPoints[1].x,ptInfo->tTargetSize.atMaxPoints[1].y);

					printf("\t分析区域:%d\n",ptInfo->iAnalyAreaNum);
					for(int i = 0; i < ptInfo->iAnalyAreaNum; i++)
					{
						printf("\t%d ",i+1);
						for(int j = 0; j < ptInfo->atAnalyAreas[i].iPtNum; j++)
						{
							printf("(%d,%d) ",ptInfo->atAnalyAreas[i].atPoints[j].x,
								ptInfo->atAnalyAreas[i].atPoints[j].y);
						}
						printf("\n");
					}

					printf("\t屏蔽区域:%d\n",ptInfo->iSheildAreaNum);
					for(int i = 0; i < ptInfo->iSheildAreaNum; i++)
					{
						printf("\t%d ",i+1);
						for(int j = 0; j < ptInfo->atSheildAreas[i].iPtNum; j++)
						{
							printf("(%d,%d) ",ptInfo->atSheildAreas[i].atPoints[j].x,
								ptInfo->atSheildAreas[i].atPoints[j].y);
						}
						printf("\n");
					}
					break;
				}
			case ANALY_CMD_EVENT_DETERMINE:
				{
					int iEvent = *((int *)(tMqBuf.data));
					DetermineT * ptInfo = (DetermineT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventDetermine(iEvent, ptInfo);
					printf("[判决条件] Event:%d\n", iEvent);
					printf("\t%d %d %f %f\n", ptInfo->iValue1,ptInfo->iValue2,ptInfo->fValue1,ptInfo->fValue2);
					break;
				}
			case ANALY_CMD_EVENT_EVIDENCE:
				{
					int iEvent = *((int *)(tMqBuf.data));
					EvidenceModelT  * ptInfo = (EvidenceModelT  *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventEvidenceModel(iEvent, ptInfo);
					printf("[取证模式] Event:%d\n",iEvent);
					printf("\t模式:%d\n",ptInfo->iModel);
					printf("\t事件:%s\n",ptInfo->bEnableEvent?"开启":"关闭");
					break;
				}
			case ANALY_CMD_EVENT_IMG_MAKEUP:
				{
					int iEvent = *((int *)(tMqBuf.data));
					int iEviModel = *((int *)(tMqBuf.data+sizeof(int)));
					ImgMakeupT * ptInfo = (ImgMakeupT *)(tMqBuf.data+2*sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventImgMakeup(iEvent, iEviModel, ptInfo);
					printf("[图片取证] Event:%d EviModel:%d\n", iEvent, iEviModel);
					printf("\t图片数量:%d张",ptInfo->iImgNum);
					for (int i = 0; i < ptInfo->iImgNum; i++)
					{
						if (i>0)
						{
							printf(" %d",ptInfo->iInterval[i]);
						}
						printf(" %s",ptInfo->iSnapPos[i]==eFull?"全景":(ptInfo->iSnapPos[i]==eCloser?"近景":"特写"));
					}
					printf("\n");
					break;
				}
			case ANALY_CMD_EVENT_IMG_COM:
				{
					int iEvent = *((int *)(tMqBuf.data));
					ImgComposeT * ptInfo = (ImgComposeT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventImgCompose(iEvent, ptInfo);
					printf("[图片合成] Event:%d\n",iEvent);
					printf("\t2张:%s\n",ptInfo->iComModel2==eVerCom?"竖直合成":(ptInfo->iComModel2==eHorCom?"水平合成":(ptInfo->iComModel2==eMatCom?"田字合成":"不合成")));
					printf("\t3张:%s\n",ptInfo->iComModel3==eVerCom?"竖直合成":(ptInfo->iComModel3==eHorCom?"水平合成":(ptInfo->iComModel3==eMatCom?"田字合成":"不合成")));
					printf("\t4张:%s\n",ptInfo->iComModel4==eVerCom?"竖直合成":(ptInfo->iComModel4==eHorCom?"水平合成":(ptInfo->iComModel4==eMatCom?"田字合成":"不合成")));
					break;
				}
			case ANALY_CMD_EVENT_IMG_OSD:
				{
					int iEvent = *((int *)(tMqBuf.data));
					ImgOSDT * ptInfo = (ImgOSDT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventImgOSD(iEvent, ptInfo);
					printf("[图片OSD] Event:%d\n",iEvent);
					printf("\t位置: %s\n",
						ptInfo->iPosModel==eEachUp   ? "单张上方" : (
						ptInfo->iPosModel==eEachIn   ? "单张内部" : (
						ptInfo->iPosModel==eEachDown ? "单张下方" : (
						ptInfo->iPosModel==eComUp    ? "合成上方" : (
						ptInfo->iPosModel==eComIn    ? "合成内部" : "合成下方")))));
					printf("\t边距: 左=%d%% 上=%d%%\n",ptInfo->iLeft,ptInfo->iTop);
					printf("\t字体: 大小=%d 颜色: %02X %02X %02X",ptInfo->iFontSize, ptInfo->tFontColor.r, ptInfo->tFontColor.g, ptInfo->tFontColor.b);
					printf("\t背景: 颜色=%02X %02X %02X\n",ptInfo->tBackColor.r, ptInfo->tBackColor.g, ptInfo->tBackColor.b);
					printf("\t内容: Flag=%04X", ptInfo->iContentFlag);
					printf(" 日期时间(%s)",(ptInfo->iContentFlag&OSD_TIME)?"O":"X");
					printf(" 时间毫秒(%s)",(ptInfo->iContentFlag&OSD_MESC)?"O":"X");
					printf(" 设备编号(%s)",(ptInfo->iContentFlag&OSD_DEVID)?"O":"X");
					printf(" 违法地点(%s)",(ptInfo->iContentFlag&OSD_PLACE)?"O":"X");
					printf(" 违法行为(%s)",(ptInfo->iContentFlag&OSD_EVENT)?"O":"X");
					printf(" 违法代码(%s)",(ptInfo->iContentFlag&OSD_CODE)?"O":"X");
					printf(" 号牌号码(%s)",(ptInfo->iContentFlag&OSD_PLATE)?"O":"X");
					printf(" 号牌颜色(%s)",(ptInfo->iContentFlag&OSD_PLATE_COLOR)?"O":"X");
					printf(" 场景方向(%s)",(ptInfo->iContentFlag&OSD_DIRECTION)?"O":"X");
					printf(" 防伪水印(%s)",(ptInfo->iContentFlag&OSD_WATERMARK)?"O":"X");
					printf("\n");
					break;
				}
			case ANALY_CMD_EVENT_IMG_QUALITY:
				{
					int iEvent = *((int *)(tMqBuf.data));
					ImgQualityT * ptInfo = (ImgQualityT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventImgQuality(iEvent, ptInfo);
					printf("[图片质量] Event:%d\n",iEvent);
					printf("\t宽度限制:%d\n\t高度限制:%d\n\t大小限制:%dKB\n",
						ptInfo->iWidthLimit, ptInfo->iHeightLimit, ptInfo->iSizeLimit);
					break;
				}
			case ANALY_CMD_EVENT_REC:
				{
					int iEvent = *((int *)(tMqBuf.data));
					RecEvidenceT * ptInfo = (RecEvidenceT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventRecEvidence(iEvent, ptInfo);
					printf("[录像取证] Event:%d\n",iEvent);
					printf("\t开关:%s\n",ptInfo->bEnable?"开启":"关闭");
					printf("\t格式:%s\n",ptInfo->iFileFormat==REC_FILE_MP4?"MP4":"AVI");
					printf("\t模式:%s\n",ptInfo->iRecModel==eRecWhole?"完整":"拼接");
					printf("\t预录:%ds\n",ptInfo->iPreTime);
					printf("\t延后:%ds\n",ptInfo->iAftTime);
					printf("\t最短:%ds\n",ptInfo->iMinTime);
					break;
				}
			case ANALY_CMD_EVENT_CODE:
				{
					int iEvent = *((int *)(tMqBuf.data));
					EventCodeT * ptInfo = (EventCodeT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventCode(iEvent, ptInfo);
					printf("[事件代码] Event:%d\n",iEvent);
					printf("\t代码:%s 描述:%s\n", ptInfo->szCode, ptInfo->szDesc);
					break;
				}
			case ANALY_CMD_EVENT_NAMING:
				{
					int iEvent = *((int *)(tMqBuf.data));
					FileNamingT * ptInfo = (FileNamingT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventFileNaming(iEvent, ptInfo);
					printf("[事件命名] Event:%d TimeModel:%d\n",iEvent, ptInfo->iTimeModel);
					printf("\t违法图片:%s\n",ptInfo->szIllegalImg);
					printf("\t违法录像:%s\n",ptInfo->szIllegalRec);
					printf("\t事件图片:%s\n",ptInfo->szEventImg);
					printf("\t时间录像:%s\n",ptInfo->szEventRec);
					break;
				}
			default:
				{
					LOG_WARNS_FMT("Unknown CMD, %d!",tMqBuf.lCmd);
					break;
				}
			}
		}
	}

	AnalyDispatcher::UnInitialize();
	MQ_Analy_Close();
	LogUninit();
	return 0;
}

