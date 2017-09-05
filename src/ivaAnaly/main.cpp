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
	/*�쳣��¼��д���ļ��� */
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
	LogInit("ivaAnaly", 10*1024*1024);// ��־���10M

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

	// ��ʼ���㷨����
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
					printf("[�豸��Ϣ]\n");
					printf("\t������: %s\n", ptInfo->szNetID);
					printf("\tע���: %s\n", ptInfo->szRegID);
					printf("\t�豸��: %s\n", ptInfo->szDevID);
					printf("\t���̺�: %s\n", ptInfo->szManID);
					break;
				}
			case ANALY_CMD_LICENSE:
				{
					AuthStatusT * ptInfo = (AuthStatusT *)tMqBuf.data;
					AnalyCfgMan::GetInstance()->SetAuthInfo(ptInfo);
					printf("[��Ȩ��Ϣ] %s\n", ptInfo->eAuthType==eProhibit?"δ��Ȩ":(ptInfo->eAuthType==ePermanent?"������Ȩ":"��ʱ��Ȩ"));
					if(ptInfo->eAuthType==eTemporary)
					{
						char buf[40] = {0};
						TimeFormatString(ptInfo->tTimeOut, buf,40, eYMD1);
						printf("\t��Ȩ����: %s\n", buf);
					}
					break;
				}
			case ANALY_CMD_ROOTPATH:
				{
					const char * pszPath = (const char *)tMqBuf.data;
					AnalyCfgMan::GetInstance()->SetRootPath(pszPath);
					printf("[�洢Ŀ¼] %s\n", pszPath);
					break;
				}
			case ANALY_CMD_STORE_FULL:
				{
					StoreFullStatusT * ptInfo = (StoreFullStatusT *)tMqBuf.data;
					AnalyCfgMan::GetInstance()->SetStoreFull(ptInfo);
					printf("[�洢״̬] %s %s\n", ptInfo->bFull?"����":"δ��",
						ptInfo->iHow2DO==eFull2CyclicCovering?"ѭ������":(
						ptInfo->iHow2DO==eFull2StopAnaly?"ֹͣ����":"ֹͣд��"));
					break;
				}
			case ANALY_CMD_LPR:
				{
					LPRCfg * ptInfo = (LPRCfg *)tMqBuf.data;
					AnalyCfgMan::GetInstance()->SetLprCfg(ptInfo);
					printf("[����ʶ��]\n");
					printf("\tĬ��ʡ��:%s ���Ŷ�:%d%% ��С�ߴ�:%d\n", g_HANZI_LIST[ptInfo->iDefProv%31], ptInfo->iHanziConf, ptInfo->iMinSize);
					printf("\t�ظ����:%d �ظ�����:%d\n", ptInfo->iDupTime,ptInfo->iDupSens);
					break;
				}
			case ANALY_CMD_DISPATCH:
				{
					DispatchCtrlT * ptInfo = (DispatchCtrlT *)tMqBuf.data;
					AnalyCfgMan::GetInstance()->SetDispatchCtrl(ptInfo);

					printf("[���Ȳ���]\n");
					printf("\t������:%s\n", 
						ptInfo->iSensitivity==eSenFast?"����":(
						ptInfo->iSensitivity==eSenAccuracy?"׼ȷ":"ƽ��"));
					printf("\t���ȼ�:%s\n", 
						ptInfo->iPriority==ePriByScene?"��������":"Ŀ������");
					printf("\t��Сͣ��:%d\n", ptInfo->iSceneMinStay);
					printf("\t�ظ�ץ��:%d\n", ptInfo->iRepeatTime);
					break;
				}
			case ANALY_CMD_CHN_OPEN:
				{
					int iChnID = *((int *)(tMqBuf.data));
					bool bEnable = *((bool *)(tMqBuf.data+sizeof(int)));
					AnalyCfgMan::GetInstance()->SetChnAlgOpen(iChnID, bEnable);
					printf("[��������] CHN:%d %s\n",iChnID, bEnable?"����":"�ر�");
					break;
				}
			case ANALY_CMD_CHN_PLAN:
				{
					int iChnID = *((int *)(tMqBuf.data));
					AnalyPlan *ptInfo = (AnalyPlan *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetChnPlan(iChnID, ptInfo);
					printf("[�����ƻ�] CHN:%d\n",iChnID);
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
					printf("[�ƻ�����] CHN:%d %s\n", iChnID, bEnable?"����":"�ر�");
					break;
				}
			case ANALY_CMD_CHN_NOTIFY_OPEN:
				{
					int iChnID = *((int *)(tMqBuf.data));
					bool bEnable = *((bool *)(tMqBuf.data+sizeof(int)));
					AnalyCfgMan::GetInstance()->SetChnNotifyOpen(iChnID, bEnable);
					printf("[֪ͨ����] CHN:%d %s\n", iChnID, bEnable?"����":"�ر�");
					break;
				}
			case ANALY_CMD_CHN_PLACE:
				{
					int iChnID = *((int *)(tMqBuf.data));
					PlaceInfoT * ptInfo = (PlaceInfoT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetChnPlaceInfoT(iChnID, ptInfo);
					printf("[ͨ����Ϣ] CHN:%d\n",iChnID);
					printf("\t��������: %s\n", ptInfo->szAreaName);
					printf("\t������: %s\n", ptInfo->szAreaID);
					printf("\t�������: %s\n", ptInfo->szMonitoryName);
					printf("\t��ر��: %s\n", ptInfo->szMonitoryID);
					printf("\t�ص�����: %s\n", ptInfo->szPlaceName);
					printf("\t�ص���: %s\n", ptInfo->szPlaceID);
					break;
				}
			case ANALY_CMD_CHN_CLEAR:
				{
					int iChnID = *((int *)(tMqBuf.data));
					AnalyCfgMan::GetInstance()->ClearChnRuleAnaly(iChnID);
					printf("[�������] CHN:%d\n",iChnID);
					break;
				}
			case ANALY_CMD_CHN_MANUAL:
				{
					int iChnID = *((int *)(tMqBuf.data));
					ManualSnapT *ptInfo = (ManualSnapT *)(tMqBuf.data+sizeof(int));
					AnalyDispatcher::GetInstance()->ManualSnap(iChnID, ptInfo);
					printf("[�ֶ�ץ��] CHN:%d Event:%d Pos:(%d %d %d %d)\n",iChnID, ptInfo->iEntID, 
						ptInfo->tRect.x, ptInfo->tRect.y, ptInfo->tRect.w, ptInfo->tRect.h);
					break;
				}
			case ANALY_CMD_CHN_PRESET_POS:
				{
					int iChnID = *((int *)(tMqBuf.data));
					int iPresetID = *((int *)(tMqBuf.data+sizeof(int)));
					AbsPosition *ptInfo = (AbsPosition *)(tMqBuf.data+2*sizeof(int));
					AnalyCfgMan::GetInstance()->SetChnPresetPos(iChnID, iPresetID, ptInfo);
					printf("[Ԥ��λ����] CHN:%d, Preset:%d\n", iChnID, iPresetID);
					printf("\tPan:%g Tilt:%g Zoom:%g\n", ptInfo->fPan, ptInfo->fTilt, ptInfo->fZoom);
					break;
				}
			case ANALY_CMD_CHN_SCENE_INFO:
				{
					int iChnID = *((int *)(tMqBuf.data));
					int iSceneID = *((int *)(tMqBuf.data+sizeof(int)));
					SceneInfoT * ptInfo = (SceneInfoT *)(tMqBuf.data+2*sizeof(int));
					AnalyCfgMan::GetInstance()->SetChnSceneInfo(iChnID, iSceneID, ptInfo);
					printf("[������Ϣ] CHN:%d, SCE:%d\n", iChnID, iSceneID);
					printf("\tԤ��λ:%d\n", ptInfo->iPresetID);
					printf("\tʱ��:%ds\n", ptInfo->iStayTime);
					printf("\t����:%d\n", ptInfo->iDirection);
					printf("\t����:%s\n", ptInfo->szSceneName);
					break;
				}
			case ANALY_CMD_CHN_SCENE_RULE:
				{
					int iChnID = *((int *)(tMqBuf.data));
					int iSceneID = *((int *)(tMqBuf.data+sizeof(int)));
					AnalyRuleT * ptInfo = (AnalyRuleT *)(tMqBuf.data+2*sizeof(int));
					AnalyCfgMan::GetInstance()->SetChnSceneRule(iChnID, iSceneID, ptInfo);
					printf("[��������] CHN:%d, SCE:%d\n", iChnID, iSceneID);
					printf("\t�����궨:��%.2f ��%.2f ",ptInfo->tCalib.length,ptInfo->tCalib.width);
					printf("(%d,%d) (%d,%d) (%d,%d) (%d,%d)\n",
						ptInfo->tCalib.atPoints[0].x,ptInfo->tCalib.atPoints[0].y,
						ptInfo->tCalib.atPoints[1].x,ptInfo->tCalib.atPoints[1].y,
						ptInfo->tCalib.atPoints[2].x,ptInfo->tCalib.atPoints[2].y,
						ptInfo->tCalib.atPoints[3].x,ptInfo->tCalib.atPoints[3].y);

					printf("\tĿ���С:%s \n",ptInfo->tTargetSize.bEnable?"ʹ��":"����");
					printf("\t��СĿ��(%d,%d) (%d,%d) ���Ŀ��(%d,%d) (%d,%d)\n",
						ptInfo->tTargetSize.atMinPoints[0].x,ptInfo->tTargetSize.atMinPoints[0].y,
						ptInfo->tTargetSize.atMinPoints[1].x,ptInfo->tTargetSize.atMinPoints[1].y,
						ptInfo->tTargetSize.atMaxPoints[0].x,ptInfo->tTargetSize.atMaxPoints[0].y,
						ptInfo->tTargetSize.atMaxPoints[1].x,ptInfo->tTargetSize.atMaxPoints[1].y);

					printf("\t��������:%d\n",ptInfo->iAnalyAreaNum);
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

					printf("\t��������:%d\n",ptInfo->iSheildAreaNum);
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
					printf("[�о�����] Event:%d\n", iEvent);
					printf("\t%d %d %f %f\n", ptInfo->iValue1,ptInfo->iValue2,ptInfo->fValue1,ptInfo->fValue2);
					break;
				}
			case ANALY_CMD_EVENT_EVIDENCE:
				{
					int iEvent = *((int *)(tMqBuf.data));
					EvidenceModelT  * ptInfo = (EvidenceModelT  *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventEvidenceModel(iEvent, ptInfo);
					printf("[ȡ֤ģʽ] Event:%d\n",iEvent);
					printf("\tģʽ:%d\n",ptInfo->iModel);
					printf("\t�¼�:%s\n",ptInfo->bEnableEvent?"����":"�ر�");
					break;
				}
			case ANALY_CMD_EVENT_IMG_MAKEUP:
				{
					int iEvent = *((int *)(tMqBuf.data));
					int iEviModel = *((int *)(tMqBuf.data+sizeof(int)));
					ImgMakeupT * ptInfo = (ImgMakeupT *)(tMqBuf.data+2*sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventImgMakeup(iEvent, iEviModel, ptInfo);
					printf("[ͼƬȡ֤] Event:%d EviModel:%d\n", iEvent, iEviModel);
					printf("\tͼƬ����:%d��",ptInfo->iImgNum);
					for (int i = 0; i < ptInfo->iImgNum; i++)
					{
						if (i>0)
						{
							printf(" %d",ptInfo->iInterval[i]);
						}
						printf(" %s",ptInfo->iSnapPos[i]==eFull?"ȫ��":(ptInfo->iSnapPos[i]==eCloser?"����":"��д"));
					}
					printf("\n");
					break;
				}
			case ANALY_CMD_EVENT_IMG_COM:
				{
					int iEvent = *((int *)(tMqBuf.data));
					ImgComposeT * ptInfo = (ImgComposeT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventImgCompose(iEvent, ptInfo);
					printf("[ͼƬ�ϳ�] Event:%d\n",iEvent);
					printf("\t2��:%s\n",ptInfo->iComModel2==eVerCom?"��ֱ�ϳ�":(ptInfo->iComModel2==eHorCom?"ˮƽ�ϳ�":(ptInfo->iComModel2==eMatCom?"���ֺϳ�":"���ϳ�")));
					printf("\t3��:%s\n",ptInfo->iComModel3==eVerCom?"��ֱ�ϳ�":(ptInfo->iComModel3==eHorCom?"ˮƽ�ϳ�":(ptInfo->iComModel3==eMatCom?"���ֺϳ�":"���ϳ�")));
					printf("\t4��:%s\n",ptInfo->iComModel4==eVerCom?"��ֱ�ϳ�":(ptInfo->iComModel4==eHorCom?"ˮƽ�ϳ�":(ptInfo->iComModel4==eMatCom?"���ֺϳ�":"���ϳ�")));
					break;
				}
			case ANALY_CMD_EVENT_IMG_OSD:
				{
					int iEvent = *((int *)(tMqBuf.data));
					ImgOSDT * ptInfo = (ImgOSDT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventImgOSD(iEvent, ptInfo);
					printf("[ͼƬOSD] Event:%d\n",iEvent);
					printf("\tλ��: %s\n",
						ptInfo->iPosModel==eEachUp   ? "�����Ϸ�" : (
						ptInfo->iPosModel==eEachIn   ? "�����ڲ�" : (
						ptInfo->iPosModel==eEachDown ? "�����·�" : (
						ptInfo->iPosModel==eComUp    ? "�ϳ��Ϸ�" : (
						ptInfo->iPosModel==eComIn    ? "�ϳ��ڲ�" : "�ϳ��·�")))));
					printf("\t�߾�: ��=%d%% ��=%d%%\n",ptInfo->iLeft,ptInfo->iTop);
					printf("\t����: ��С=%d ��ɫ: %02X %02X %02X",ptInfo->iFontSize, ptInfo->tFontColor.r, ptInfo->tFontColor.g, ptInfo->tFontColor.b);
					printf("\t����: ��ɫ=%02X %02X %02X\n",ptInfo->tBackColor.r, ptInfo->tBackColor.g, ptInfo->tBackColor.b);
					printf("\t����: Flag=%04X", ptInfo->iContentFlag);
					printf(" ����ʱ��(%s)",(ptInfo->iContentFlag&OSD_TIME)?"O":"X");
					printf(" ʱ�����(%s)",(ptInfo->iContentFlag&OSD_MESC)?"O":"X");
					printf(" �豸���(%s)",(ptInfo->iContentFlag&OSD_DEVID)?"O":"X");
					printf(" Υ���ص�(%s)",(ptInfo->iContentFlag&OSD_PLACE)?"O":"X");
					printf(" Υ����Ϊ(%s)",(ptInfo->iContentFlag&OSD_EVENT)?"O":"X");
					printf(" Υ������(%s)",(ptInfo->iContentFlag&OSD_CODE)?"O":"X");
					printf(" ���ƺ���(%s)",(ptInfo->iContentFlag&OSD_PLATE)?"O":"X");
					printf(" ������ɫ(%s)",(ptInfo->iContentFlag&OSD_PLATE_COLOR)?"O":"X");
					printf(" ��������(%s)",(ptInfo->iContentFlag&OSD_DIRECTION)?"O":"X");
					printf(" ��αˮӡ(%s)",(ptInfo->iContentFlag&OSD_WATERMARK)?"O":"X");
					printf("\n");
					break;
				}
			case ANALY_CMD_EVENT_IMG_QUALITY:
				{
					int iEvent = *((int *)(tMqBuf.data));
					ImgQualityT * ptInfo = (ImgQualityT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventImgQuality(iEvent, ptInfo);
					printf("[ͼƬ����] Event:%d\n",iEvent);
					printf("\t�������:%d\n\t�߶�����:%d\n\t��С����:%dKB\n",
						ptInfo->iWidthLimit, ptInfo->iHeightLimit, ptInfo->iSizeLimit);
					break;
				}
			case ANALY_CMD_EVENT_REC:
				{
					int iEvent = *((int *)(tMqBuf.data));
					RecEvidenceT * ptInfo = (RecEvidenceT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventRecEvidence(iEvent, ptInfo);
					printf("[¼��ȡ֤] Event:%d\n",iEvent);
					printf("\t����:%s\n",ptInfo->bEnable?"����":"�ر�");
					printf("\t��ʽ:%s\n",ptInfo->iFileFormat==REC_FILE_MP4?"MP4":"AVI");
					printf("\tģʽ:%s\n",ptInfo->iRecModel==eRecWhole?"����":"ƴ��");
					printf("\tԤ¼:%ds\n",ptInfo->iPreTime);
					printf("\t�Ӻ�:%ds\n",ptInfo->iAftTime);
					printf("\t���:%ds\n",ptInfo->iMinTime);
					break;
				}
			case ANALY_CMD_EVENT_CODE:
				{
					int iEvent = *((int *)(tMqBuf.data));
					EventCodeT * ptInfo = (EventCodeT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventCode(iEvent, ptInfo);
					printf("[�¼�����] Event:%d\n",iEvent);
					printf("\t����:%s ����:%s\n", ptInfo->szCode, ptInfo->szDesc);
					break;
				}
			case ANALY_CMD_EVENT_NAMING:
				{
					int iEvent = *((int *)(tMqBuf.data));
					FileNamingT * ptInfo = (FileNamingT *)(tMqBuf.data+sizeof(int));
					AnalyCfgMan::GetInstance()->SetEventFileNaming(iEvent, ptInfo);
					printf("[�¼�����] Event:%d TimeModel:%d\n",iEvent, ptInfo->iTimeModel);
					printf("\tΥ��ͼƬ:%s\n",ptInfo->szIllegalImg);
					printf("\tΥ��¼��:%s\n",ptInfo->szIllegalRec);
					printf("\t�¼�ͼƬ:%s\n",ptInfo->szEventImg);
					printf("\tʱ��¼��:%s\n",ptInfo->szEventRec);
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

