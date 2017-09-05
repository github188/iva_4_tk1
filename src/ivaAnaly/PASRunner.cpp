#include "PASRunner.h"
#include "AnalyCfgMan.h"
#include "AlgorithmDll.h"
#include "AnalyDispatcher.h"
#include "oal_time.h"
#include "oal_unicode.h"
#include "AnalyRealTime.h"
#include "AnalyPtzCtrl.h"
#include <string.h>

static void ShowIVREventEvidenceConf(const char *event,const tEventEvidenceConf *ptConf)
{
	if(ptConf && event)
	{
		char msg[1024] = {0};
		sprintf(msg,"[%s] 算法%s 事件%s 抓拍%d张 ", event, ptConf->bEnable?"开启":"关闭", ptConf->bEvent?"开启":"关闭", ptConf->u32SnapCnt);

		for(int i = 0; i < ptConf->u32SnapCnt; i++)
		{
			sprintf(msg + strlen(msg), "(%d)%s ", ptConf->eSnapType[i],ptConf->eSnapType[i] == FULL_SHOT?"全景":(ptConf->eSnapType[i] == CLOSE_UP?"近景":"特写"));
			if(ptConf->eEventType == AT_ILLEGAL_STOP_CAR && i < ptConf->u32SnapCnt-1)
			{
				sprintf(msg + strlen(msg), "%ds ", ptConf->u32Interval[i]);
			}
		}

		LOG_DEBUG_FMT("%s",msg);
	}
}

static void ShowPresetList(tPresetList *ptList)
{
	if(ptList == NULL)
	{
		return;
	}

	LOG_DEBUG_FMT("==============================");
	LOG_DEBUG_FMT("预置位个数: %d", ptList->PresetNum);
	for (int i = 0; i < ptList->PresetNum; i++)
	{
		LOG_DEBUG_FMT("-%d----------------------------",i);
		LOG_DEBUG_FMT("最短停留时间: %.0f", ptList->MinStopTime[i]);
		LOG_DEBUG_FMT("预置位编号: %d", ptList->SinglePreset[i].s32PresetId);
		LOG_DEBUG_FMT("违停时间: %us", ptList->SinglePreset[i].u32IllegeStopTime);
		LOG_DEBUG_FMT("重复抓拍时间: %us", ptList->SinglePreset[i].u32PressLineTime);
		LOG_DEBUG_FMT("最大目标: %.0f", ptList->SinglePreset[i].fMaxObjSize);
		LOG_DEBUG_FMT("最小目标: %.0f", ptList->SinglePreset[i].fMinObjSize);
		LOG_DEBUG_FMT("屏蔽区域: %u", ptList->SinglePreset[i].tSheildConf.u32AreaCount);
		for (int j = 0; j < ptList->SinglePreset[i].tSheildConf.u32AreaCount; j++)
		{
			char szArea[512] = {0};
			sprintf(szArea,"%d: ", j+1);
			for (int k = 0; k < ptList->SinglePreset[i].tSheildConf.stArea[j].u32PointNum; k++)
			{
				sprintf(szArea+strlen(szArea), "(%d %d)",
					ptList->SinglePreset[i].tSheildConf.stArea[j].stPoint[k].x,
					ptList->SinglePreset[i].tSheildConf.stArea[j].stPoint[k].y);
			}
			LOG_DEBUG_FMT("\t%s",szArea);
		}
		LOG_DEBUG_FMT("分析区域: %u", ptList->SinglePreset[i].tParkArea.u32AreaCount);
		for (int j = 0; j < ptList->SinglePreset[i].tParkArea.u32AreaCount; j++)
		{
			char szArea[512] = {0};
			sprintf(szArea,"%d: ", j+1);
			for (int k = 0; k < ptList->SinglePreset[i].tParkArea.stArea[j].u32PointNum; k++)
			{
				sprintf(szArea+strlen(szArea), "(%d %d)",
					ptList->SinglePreset[i].tParkArea.stArea[j].stPoint[k].x,
					ptList->SinglePreset[i].tParkArea.stArea[j].stPoint[k].y);
			}
			LOG_DEBUG_FMT("\t%s",szArea);
		}

		ShowIVREventEvidenceConf("违停取证:",ptList->EventSetting[i].pEventConfList);
	}
	LOG_DEBUG_FMT("==============================");
}

PASRunner::PASRunner(int iChnID):BaseRunner(iChnID)
{
	memset(&m_tPreset, 0, sizeof(m_tPreset));
	memset(&m_tImgMakeup, 0, sizeof(m_tImgMakeup));
	memset(&m_tEviModel, 0, sizeof(m_tEviModel));
	memset(&m_tLprCfg, 0, sizeof(m_tLprCfg));
	memset(&m_tCtrl, 0, sizeof(m_tCtrl));
	memset(&m_tValidPresets, 0, sizeof(m_tValidPresets));
	m_bCreateAlg = false;
}

#if 0
void PASRunner::CheckPreset()
{
	LPRCfg tTmpLpr = {0};
	int iRet = AnalyCfgMan::GetInstance()->GetLprCfg(&tTmpLpr);
	if(iRet == 0)
	{
		if(memcmp(&tTmpLpr, &m_tLprCfg, sizeof(LPRCfg)) != 0)
		{
			memcpy(&m_tLprCfg, &tTmpLpr, sizeof(LPRCfg));

			int iDefProv = 4998;
			if(m_tLprCfg.iHanziConf >= 60)
			{
				iDefProv = m_tLprCfg.iDefProv + 5000;
			}
			algExchange(m_iChnID, ALG_IVR_SetProvince, &iDefProv);
			//LOG_DEBUG_FMT("algExchange %d ALG_IVR_SetProvince %d", m_iChnID, iDefProv);

			int iMinSize = m_tLprCfg.iMinSize;
			algExchange(m_iChnID, ALG_IVR_SetPlateDiagonalLen, &iMinSize);
			//LOG_DEBUG_FMT("algExchange %d ALG_IVR_SetPlateDiagonalLen %d", m_iChnID, iMinSize);
		}
	}

	bool bPresetChanged = false;

	int iSceneID = AnalyPlanMan::GetInstance()->GetChnCurScene(m_iChnID);
	//LOG_DEBUG_FMT("m_iChnID: %d iSceneID: %d", m_iChnID,iSceneID);
	tSinglePresetConf tPresetCfg = {0};
	iRet = AnalyCfgMan::GetInstance()->GetChnScenePresetCfg(m_iChnID, iSceneID, &tPresetCfg);
	if(iRet == 0)
	{
		if(memcmp(&m_tPreset, &tPresetCfg, sizeof(m_tPreset)) != 0 )
		{
			memcpy(&m_tPreset, &tPresetCfg, sizeof(m_tPreset));

			bPresetChanged = true;
		}
	}

	bool bEvidenceChanged = false;
	EvidenceModelT tEviModel  = {0};
	AnalyCfgMan::GetInstance()->GetEventEvidenceModel(eIT_Illegal_Park, &tEviModel);
	ImgMakeupT tImgMakeup = {0};
	AnalyCfgMan::GetInstance()->GetEventImgMakeup(eIT_Illegal_Park, tEviModel.iModel, &tImgMakeup);
	if(memcmp(&tEviModel, &m_tEviModel, sizeof(EvidenceModelT)) != 0||
		memcmp(&tImgMakeup, &m_tImgMakeup, sizeof(ImgMakeupT)) != 0)
	{
		memcpy(&m_tEviModel, &tEviModel, sizeof(EvidenceModelT));
		memcpy(&m_tImgMakeup, &tImgMakeup, sizeof(ImgMakeupT));
		bEvidenceChanged = true;
	}

	if(bEvidenceChanged || bPresetChanged)
	{
		algExchange(m_iChnID, ALG_IVR_SetSinglePreset, &tPresetCfg);
		//LOG_DEBUG_FMT("algExchange %d ALG_IVR_SetSinglePreset", m_iChnID);
		LOG_DEBUG_FMT("预置位: %d", tPresetCfg.s32PresetId);
		LOG_DEBUG_FMT("违停时间: %ds", tPresetCfg.u32IllegeStopTime);
		LOG_DEBUG_FMT("标定区域: 长=%g, 宽=%g (%g %g)(%g %g)(%g %g)(%g %g)", 
			tPresetCfg.tCalibConf.fActualHeight, tPresetCfg.tCalibConf.fActualWidth,		 
			tPresetCfg.tCalibConf.tPtsInImage[0].x, tPresetCfg.tCalibConf.tPtsInImage[0].y,
			tPresetCfg.tCalibConf.tPtsInImage[1].x, tPresetCfg.tCalibConf.tPtsInImage[1].y,
			tPresetCfg.tCalibConf.tPtsInImage[2].x, tPresetCfg.tCalibConf.tPtsInImage[2].y,
			tPresetCfg.tCalibConf.tPtsInImage[3].x, tPresetCfg.tCalibConf.tPtsInImage[3].y);
		LOG_DEBUG_FMT("屏蔽区域: %d", tPresetCfg.tSheildConf.u32AreaCount);
		for (int i = 0; i < tPresetCfg.tSheildConf.u32AreaCount; i++)
		{
			char szArea[512] = {0};
			sprintf(szArea,"%d: ", i+1);
			for (int j = 0; j < tPresetCfg.tSheildConf.stArea[i].u32PointNum; j++)
			{
				sprintf(szArea+strlen(szArea), "(%d %d)",tPresetCfg.tSheildConf.stArea[i].stPoint[j].x,tPresetCfg.tSheildConf.stArea[i].stPoint[j].y);
			}
			LOG_DEBUG_FMT("%s",szArea);
		}
		LOG_DEBUG_FMT("分析区域: %d", tPresetCfg.tParkArea.u32AreaCount);
		for (int i = 0; i < tPresetCfg.tParkArea.u32AreaCount; i++)
		{
			char szArea[512] = {0};
			sprintf(szArea,"%d: ", i+1);
			for (int j = 0; j < tPresetCfg.tParkArea.stArea[i].u32PointNum; j++)
			{
				sprintf(szArea+strlen(szArea), "(%d %d)",tPresetCfg.tParkArea.stArea[i].stPoint[j].x,tPresetCfg.tParkArea.stArea[i].stPoint[j].y);
			}
			LOG_DEBUG_FMT("%s",szArea);
		}
	
		tEventSetting tEvent;
		tEvent.nEvent = 1;
		tEvent.pEventConfList = new tEventEvidenceConf[tEvent.nEvent];
		memset(tEvent.pEventConfList, 0 ,tEvent.nEvent * sizeof(tEventEvidenceConf));

		tEvent.pEventConfList[0].eEventType = AT_ILLEGAL_STOP_CAR;
		tEvent.pEventConfList[0].bEnable	= (tPresetCfg.u32FuncFlag & USE_PARK)?1:0;
		tEvent.pEventConfList[0].bEvent     = tEviModel.bEnableEvent?1:0;
		tEvent.pEventConfList[0].u32SnapCnt = tImgMakeup.iImgNum;
		for(int i = 0; i < tImgMakeup.iImgNum; i++)
		{
			tEvent.pEventConfList[0].eSnapType[i] = (ESnapType)(tImgMakeup.iSnapPos[i]);
			tEvent.pEventConfList[0].u32Interval[i] = tImgMakeup.iInterval[(i+1)%4];
		}
		int iStopTime = AnalyCfgMan::GetInstance()->GetIllegeStopTime();
		if(iStopTime >= MAX_FULL_RECORD_SECS && tEvent.pEventConfList[0].bEnable && tEvent.pEventConfList[0].bEvent == 0)
		{
			LOG_DEBUG_FMT("违停时间%d>%d, 强制打开事件取证",iStopTime,MAX_FULL_RECORD_SECS);
			tEvent.pEventConfList[0].bEvent = 1;
		}

		if(tEvent.pEventConfList[0].bEnable && AnalyCfgMan::GetInstance()->ChnNotifyIsOpen(m_iChnID))
		{
			LOG_DEBUG_FMT("%s","提醒开启, 强制打开事件取证");
			tEvent.pEventConfList[0].bEvent = 1;
		}

		algExchange(m_iChnID,ALG_IVR_SetEventEvidenceConf,&tEvent);
		LOG_DEBUG_FMT("algExchange %d ALG_IVR_SetEventEvidenceConf", m_iChnID);
		ShowIVREventEvidenceConf("违停",tEvent.pEventConfList);

		delete [] tEvent.pEventConfList;
	}
}
#else
void PASRunner::CheckPreset()
{
	if (!m_bCreateAlg)
	{
		return;
	}

	// 车牌识别参数
	LPRCfg tTmpLpr = {0};
	int iRet = AnalyCfgMan::GetInstance()->GetLprCfg(&tTmpLpr);
	if(iRet == 0)
	{
		if(memcmp(&tTmpLpr, &m_tLprCfg, sizeof(LPRCfg)) != 0)
		{
			memcpy(&m_tLprCfg, &tTmpLpr, sizeof(LPRCfg));

			int iDefProv = 4998;
			if(m_tLprCfg.iHanziConf >= 60)
			{
				iDefProv = m_tLprCfg.iDefProv + 5000;
			}
			algExchange(m_iChnID, ALG_IVR_SetProvince, &iDefProv);

			int iMinSize = m_tLprCfg.iMinSize;
			algExchange(m_iChnID, ALG_IVR_SetPlateDiagonalLen, &iMinSize);
		}
	}

	// 取证
	bool bEvidenceChanged = false;
	EvidenceModelT tEviModel  = {0};
	ImgMakeupT tImgMakeup = {0};
	AnalyCfgMan::GetInstance()->GetEventEvidenceModel(eIT_Illegal_Park, &tEviModel);
	AnalyCfgMan::GetInstance()->GetEventImgMakeup(eIT_Illegal_Park, tEviModel.iModel, &tImgMakeup);
	if(memcmp(&tEviModel, &m_tEviModel, sizeof(EvidenceModelT)) != 0||
		memcmp(&tImgMakeup, &m_tImgMakeup, sizeof(ImgMakeupT)) != 0)
	{
		memcpy(&m_tEviModel, &tEviModel, sizeof(EvidenceModelT));
		memcpy(&m_tImgMakeup, &tImgMakeup, sizeof(ImgMakeupT));
		bEvidenceChanged = true;
	}

	// 调度
	bool bCtrlChanged = false;
	DispatchCtrlT tCtrl;
	AnalyCfgMan::GetInstance()->GetDispatchCtrl(&tCtrl);
	if(memcmp(&tCtrl, &m_tCtrl, sizeof(DispatchCtrlT)) != 0)
	{
		memcpy(&m_tCtrl, &tCtrl, sizeof(DispatchCtrlT));
		bCtrlChanged = true;
	}

	if (tCtrl.iPriority == ePriByScene)// 场景优先
	{
		memset(&m_tValidPresets, 0, sizeof(ValidPresetSet));
		
		bool bPresetChanged = false;
		tSinglePresetConf tPresetCfg = {0};

		int iSceneID = AnalyPlanMan::GetInstance()->GetChnCurScene(m_iChnID);
		iRet = AnalyCfgMan::GetInstance()->GetChnScenePresetCfg(m_iChnID, iSceneID, &tPresetCfg);
		if(iRet == 0)
		{
			if(memcmp(&m_tPreset, &tPresetCfg, sizeof(m_tPreset)) != 0 )
			{
				memcpy(&m_tPreset, &tPresetCfg, sizeof(m_tPreset));

				bPresetChanged = true;
			}
		}

		if(bCtrlChanged || bEvidenceChanged || bPresetChanged)
		{
			tPresetList tList;
			memset(&tList, 0, sizeof(tPresetList));

			tList.PresetNum = 1;
			tList.MinStopTime[0] = (float)/*tCtrl.iSceneMinStay*/6000;
			memcpy(&tList.SinglePreset[0], &tPresetCfg, sizeof(tSinglePresetConf));
			tList.SinglePreset[0].u32PressLineTime = tCtrl.iRepeatTime;
			tList.EventSetting[0].nEvent = 1;
			tList.EventSetting[0].pEventConfList = new tEventEvidenceConf[1];
			if (tList.EventSetting[0].pEventConfList == NULL)return;
			memset(tList.EventSetting[0].pEventConfList, 0, 1 * sizeof(tEventEvidenceConf));
			tList.EventSetting[0].pEventConfList[0].eEventType = AT_ILLEGAL_STOP_CAR;
			tList.EventSetting[0].pEventConfList[0].bEnable = (tPresetCfg.u32FuncFlag & USE_PARK)?1:0;
			tList.EventSetting[0].pEventConfList[0].bEvent  = tEviModel.bEnableEvent?1:0;
			tList.EventSetting[0].pEventConfList[0].u32SnapCnt = tImgMakeup.iImgNum;
			for(int i = 0; i < tImgMakeup.iImgNum; i++)
			{
				tList.EventSetting[0].pEventConfList[0].eSnapType[i] = (ESnapType)(tImgMakeup.iSnapPos[i]);
				tList.EventSetting[0].pEventConfList[0].u32Interval[i] = tImgMakeup.iInterval[(i+1)%4];
			}

			// 强制事件取证
			if (tList.EventSetting[0].pEventConfList[0].bEnable && tList.EventSetting[0].pEventConfList[0].bEvent == 0)
			{
				if(AnalyCfgMan::GetInstance()->GetIllegeStopTime() >= MAX_FULL_RECORD_SECS)
				{
					LOG_DEBUG_FMT("判定时间太长, 强制打开事件取证");
					tList.EventSetting[0].pEventConfList[0].bEvent = 1;
				}

				if(AnalyCfgMan::GetInstance()->ChnNotifyIsOpen(m_iChnID))
				{
					LOG_DEBUG_FMT("提醒开启, 强制打开事件取证");
					tList.EventSetting[0].pEventConfList[0].bEvent = 1;
				}
			}

			algExchange(m_iChnID, ALG_IVR_SetPresetList, &tList);
			ShowPresetList(&tList);			

			delete [] tList.EventSetting[0].pEventConfList;
			tList.EventSetting[0].pEventConfList = NULL;
		}
	}
	else// 目标优先
	{
		memset(&m_tPreset, 0, sizeof(tSinglePresetConf));

		bool bPresetChanged = false;
		ValidPresetSet tAllPresetCfg;
		iRet = AnalyCfgMan::GetInstance()->GetChnAllPresetCfg(m_iChnID, &tAllPresetCfg);
		if(iRet == 0)
		{
			if(memcmp(&m_tValidPresets, &tAllPresetCfg, sizeof(m_tValidPresets)) != 0 )
			{
				memcpy(&m_tValidPresets, &tAllPresetCfg, sizeof(m_tValidPresets));

				bPresetChanged = true;
			}
		}

		if(bCtrlChanged || bEvidenceChanged || bPresetChanged)
		{
			tPresetList tList;
			memset(&tList, 0, sizeof(tPresetList));

			tList.PresetNum = tAllPresetCfg.iPresetNum;
			for (int i = 0; i < tList.PresetNum; i++)
			{
				tList.MinStopTime[i] = (float)tCtrl.iSceneMinStay;
				memcpy(&tList.SinglePreset[i], &tAllPresetCfg.SinglePreset[i], sizeof(tSinglePresetConf));
				tList.SinglePreset[i].u32PressLineTime = tCtrl.iRepeatTime;
				tList.EventSetting[i].nEvent = 1;
				tList.EventSetting[i].pEventConfList = new tEventEvidenceConf[1];
				if (tList.EventSetting[i].pEventConfList == NULL)
				{
					for (int j = 0; j < i; j++)
					{
						delete [] tList.EventSetting[j].pEventConfList;
						tList.EventSetting[j].pEventConfList = NULL;
					}
					return;
				}
				memset(tList.EventSetting[i].pEventConfList, 0, 1 * sizeof(tEventEvidenceConf));
				tList.EventSetting[i].pEventConfList[0].eEventType = AT_ILLEGAL_STOP_CAR;
				tList.EventSetting[i].pEventConfList[0].bEnable = (tAllPresetCfg.SinglePreset[i].u32FuncFlag & USE_PARK)?1:0;
				tList.EventSetting[i].pEventConfList[0].bEvent  = tEviModel.bEnableEvent?1:0;
				tList.EventSetting[i].pEventConfList[0].u32SnapCnt = tImgMakeup.iImgNum;
				for(int k = 0; k < tImgMakeup.iImgNum; k++)
				{
					tList.EventSetting[i].pEventConfList[0].eSnapType[k] = (ESnapType)(tImgMakeup.iSnapPos[k]);
					tList.EventSetting[i].pEventConfList[0].u32Interval[k] = tImgMakeup.iInterval[(k+1)%4];
				}

				// 强制事件取证
				if (tList.EventSetting[i].pEventConfList[0].bEnable && tList.EventSetting[i].pEventConfList[0].bEvent == 0)
				{
					if(AnalyCfgMan::GetInstance()->GetIllegeStopTime() >= MAX_FULL_RECORD_SECS)
					{
						//LOG_DEBUG_FMT("判定时间太长, 强制打开事件取证");
						tList.EventSetting[i].pEventConfList[0].bEvent = 1;
					}

					if(AnalyCfgMan::GetInstance()->ChnNotifyIsOpen(m_iChnID))
					{
						//LOG_DEBUG_FMT("提醒开启, 强制打开事件取证");
						tList.EventSetting[i].pEventConfList[0].bEvent = 1;
					}
				}
			}

			algExchange(m_iChnID, ALG_IVR_SetPresetList, &tList);
			ShowPresetList(&tList);			

			for (int j = 0; j < tList.PresetNum; j++)
			{
				delete [] tList.EventSetting[j].pEventConfList;
				tList.EventSetting[j].pEventConfList = NULL;
			}
		}
	}
}
#endif

void PASRunner::ProcessFrame(YuvCacheNode * ptYuvNode)
{
	if(ptYuvNode == NULL || !m_bCreateAlg)
	{
		return;
	}
	
	// 执行算法
	tAlgResArray tAlgResult = {0};
	//printf("algProcess frame_id:%d timestamp:%llu\n", ptYuvNode->tYuvData.yuv_info.frame_id, ptYuvNode->tYuvData.yuv_info.timestamp);
	algProcess(m_iChnID, ptYuvNode->tYuvData.yuv_data, ALG_RES_IVR, &tAlgResult);
	//printf("-----------------after algProcess\n");
	

	// 处理结果
	for (int i = 0; i < tAlgResult.s32ResCount; i++)
	{
		printf("the %d/%d result\n", i, tAlgResult.s32ResCount);
		
		AnalyData theNewData = {0};
		int iRet = CollectAlgResult(m_iChnID, &(tAlgResult.astResArray[i]), &theNewData);
		if(iRet != 0)
		{
			LOG_INFOS_FMT("CollectAlgResult failed");
			continue;
		}

		// 实时推送
		SendRealTimeAnalyPreview(&theNewData);

		// 微信推送
		SendRealTimeAnalyWechat(&theNewData);

		// 压入取证队列
		if(AnalyEvidenceMan::GetInstance()->Push2ImgEvidenceNode(&theNewData) != 0)
		{
			LOG_INFOS_FMT("AnalyEvidenceMan Push2ImgEvidenceNode failed");
			continue;
		}
	}

	// 分析轨迹
	ProcessTracks(&tAlgResult);
}

void PASRunner::ProcessTracks(const tAlgResArray * ptAlgResult)
{
	if (!m_bCreateAlg)
	{
		return;
	}

	tAlgGraphResult tNewTarget = {0};
	memset(&tNewTarget,0,sizeof(tNewTarget));
	algExchange(m_iChnID, ALG_IVR_GetGraphResult, &tNewTarget);

	AnalyTracks tAnalyseOSD = {0};
	memset(&tAnalyseOSD, 0, sizeof(AnalyTracks));
	//tAnalyseOSD.iChnID = m_iChnID;
	tAnalyseOSD.iTrackNum = tNewTarget.ShowResNum > MAX_TRACK_NUM ? MAX_TRACK_NUM : tNewTarget.ShowResNum;
	for(int i = 0; i < tAnalyseOSD.iTrackNum; i++)
	{
		TrackColor2RGB(tNewTarget.color[i], &(tAnalyseOSD.atTracks[i].tColor));

		tAnalyseOSD.atTracks[i].iPointNum = 0;
		for(int j = 0; j < tNewTarget.tRes[i].u32PointNum && j < MAX_TRACK_POINT_NUM; j++)
		{
			tAnalyseOSD.atTracks[i].atPoints[j].x = tNewTarget.tRes[i].stPoint[j].x;
			tAnalyseOSD.atTracks[i].atPoints[j].y = tNewTarget.tRes[i].stPoint[j].y;
			tAnalyseOSD.atTracks[i].iPointNum++;
		}

		int j = tAnalyseOSD.atTracks[i].iPointNum;
		if(tNewTarget.closed[i] && j > 0 && j < MAX_TRACK_POINT_NUM)
		{
			tAnalyseOSD.atTracks[i].iPointNum++;
			tAnalyseOSD.atTracks[i].atPoints[j].x = tNewTarget.tRes[i].stPoint[0].x;
			tAnalyseOSD.atTracks[i].atPoints[j].y = tNewTarget.tRes[i].stPoint[0].y;
		}
	}

	static int null_obj_cnt = 0;
	if(tAnalyseOSD.iTrackNum < 1)
	{
		null_obj_cnt++;
		if(null_obj_cnt < 30)return;// 连续多帧帧为空才显示，不然图像跳
	}
	else
	{
		null_obj_cnt = 0;
	}

	SendRealTimeAnalyTracks(m_iChnID, &tAnalyseOSD);
}

void PASRunner::ResetAnaly()
{
	if (!m_bCreateAlg)
	{
		return;
	}

	algExchange(m_iChnID, ALG_IVR_ResetAlg, NULL);
	LOG_DEBUG_FMT("Channel%d ALG_IVR_ResetAlg",m_iChnID);
}

void PASRunner::ResetCfg()
{
	memset(&m_tPreset, 0, sizeof(m_tPreset));
	memset(&m_tImgMakeup, 0, sizeof(m_tImgMakeup));
	memset(&m_tEviModel, 0, sizeof(m_tEviModel));
	memset(&m_tLprCfg, 0, sizeof(m_tLprCfg));
	memset(&m_tCtrl, 0, sizeof(m_tCtrl));
	memset(&m_tValidPresets, 0, sizeof(m_tValidPresets));
	LOG_DEBUG_FMT("Channel%d PASRunner::ResetCfg",m_iChnID);
}

void PASRunner::DestroyAlgChannel()
{
	algExchange(m_iChnID, ALG_IVR_DestroyChannel, NULL);
	LOG_DEBUG_FMT("Channel%d ALG_IVR_DestroyChannel",m_iChnID);

	ResetCfg();

	m_bCreateAlg = false;
}

void PASRunner::CreateAlgChannel()
{
	tImageInfo tsetimginfo;
	memset(&tsetimginfo,0,sizeof(tImageInfo));
	tsetimginfo.s32ImageWidth  = m_iWidth;
	tsetimginfo.s32ImageHeight = m_iHeight;
	tsetimginfo.eImgDataType   = IMAGE_DATA_TYPE_NV12;
	algExchange(m_iChnID, ALG_IVR_CreateChannel, &tsetimginfo);
	LOG_DEBUG_FMT("Channel%d ALG_IVR_CreateChannel %d X %d",m_iChnID,m_iWidth,m_iHeight);

	algExchange(m_iChnID, ALG_IVR_SetControlerHandle, &g_tPtzCtrler);
	LOG_DEBUG_FMT("Channel%d ALG_IVR_SetControlerHandle",m_iChnID);

	m_bCreateAlg = true;
}

int PASRunner::CollectAlgResult( int iChnID, const void * ptVoid, AnalyData *ptData )
{
	if(ptVoid == NULL || ptData == NULL)
	{
		return -1;
	}

	const tAlgRes * ptAlgRes = (const tAlgRes *)ptVoid;
	printf("@--------------------------------------@\n");
	printf("@----eAlarmType:%d\n",ptAlgRes->eAlarmType);
	printf("@----s32EventID:%d\n",ptAlgRes->s32EventID);
	printf("@----s32ChannleIndex:%d\n",ptAlgRes->s32ChannleIndex);
	printf("@----s32PresetIndex:%d\n",ptAlgRes->s32PresetIndex);
	printf("@----s32LaneIndex:%d\n",ptAlgRes->s32LaneIndex);
	printf("@----bisEvent:%d\n",ptAlgRes->bisEvent);
	printf("@----s32LaneIndex:%d\n",ptAlgRes->s32LaneIndex);
	printf("@----as8platenum:%s\n",ptAlgRes->tLprRes.as8platenum);
	printf("@----ePlateColor:%d\n",ptAlgRes->tLprRes.ePlateColor);
	printf("@----eCarColor:%d\n",ptAlgRes->tLprRes.eCarColor);
	printf("@----tPlateRect:%d %d %d %d\n",ptAlgRes->tLprRes.tPlateRect.x,	ptAlgRes->tLprRes.tPlateRect.y,
		ptAlgRes->tLprRes.tPlateRect.w,ptAlgRes->tLprRes.tPlateRect.h);
	printf("@----platerealty:%f\n",ptAlgRes->tLprRes.platerealty);
	printf("@----s32SnapCnt:%d\n",ptAlgRes->tSnap.s32SnapCnt);
	for (int i = 0; i < ptAlgRes->tSnap.s32SnapCnt; i++)
	{
		char szBuff1[128] = {0};
		char szBuff2[128] = {0};
		TimeFormatString(ptAlgRes->tSnap.tSnapInfos[i].d64TimeBegin.tv_sec, szBuff1, 128, eYMDHMS1);
		TimeFormatString(ptAlgRes->tSnap.tSnapInfos[i].d64TimeEnd.tv_sec, szBuff2, 128, eYMDHMS1);
		printf("@----Snap%d: %s-%s\n",i, szBuff1, szBuff2);
	}
	printf("@----s32ImageCount:%d\n",ptAlgRes->s32ImageCount);
	for (int i = 0; i < ptAlgRes->s32ImageCount; i++)
	{
		printf("@----%d eImgDataType:%d\n", i,ptAlgRes->tImg[i].eImgDataType);
		printf("@----%d s32ImageWidth:%d\n", i,ptAlgRes->tImg[i].s32ImageWidth);
		printf("@----%d s32ImageHeight:%d\n", i,ptAlgRes->tImg[i].s32ImageHeight);
		printf("@----%d tObjRect:%d %d %d %d\n",i,ptAlgRes->tImg[i].tObjRect.x, ptAlgRes->tImg[i].tObjRect.y,
			ptAlgRes->tImg[i].tObjRect.w,ptAlgRes->tImg[i].tObjRect.h);
		char szBuff1[128] = {0};
		TimeFormatString(ptAlgRes->tImg[i].d64TimeTick.tv_sec, szBuff1, 128, eYMDHMS1);
		printf("@----%d d64TimeTick: %s\n",i, szBuff1);
		printf("@----%d pImageData: %p\n",i, ptAlgRes->tImg[i].pImageData);
	}
	printf("@--------------------------------------@\n");

	ptData->iID = ptAlgRes->s32EventID;
	ptData->iChnID = m_iChnID;
	ptData->iSceneID = AnalyCfgMan::GetInstance()->GetChnSceneIdByPreset(m_iChnID, ptAlgRes->s32PresetIndex);
	ptData->iDateType = ptAlgRes->bisEvent ? eEventData : eIllegaldata;
	if(ptAlgRes->eAlarmType == AT_ILLEGAL_STOP_CAR)
	{
		ptData->iEventType = eIT_Illegal_Park;
	}
	else
	{
		LOG_DEBUG_FMT("unknown, ptAlgRes->eAlarmType = %d ...",ptAlgRes->eAlarmType);
		return -1;
	}

	strcpy(ptData->szPlate, ptAlgRes->tLprRes.as8platenum);

	// 无车牌过滤
	if(strlen(ptData->szPlate) == 0)
	{
		printf("无车牌过滤\n");
		return -1;
	}

	ptData->iPlateConf = (int)(ptAlgRes->tLprRes.platerealty);
	if(ptAlgRes->tLprRes.ePlateColor == PLATE_COLOR_BLACK)
		ptData->iPlateColor = 3;
	else if(ptAlgRes->tLprRes.ePlateColor == PLATE_COLOR_BLUE)
		ptData->iPlateColor = 2;
	else if(ptAlgRes->tLprRes.ePlateColor == PLATE_COLOR_YELLOW)
		ptData->iPlateColor = 1;
	else if(ptAlgRes->tLprRes.ePlateColor == PLATE_COLOR_WHITE)
		ptData->iPlateColor = 0;
	else
		ptData->iPlateColor = 4;
	//ptData->iPlateColor = (int)(ptAlgRes->tLprRes.ePlateColor);
	//ptData->iPlateType;
	//ptData->iPlateIndex;
	//ptData->tPlatePos;

	//ptData->iCarType;
	//ptData->iCarColor;
	//ptData->iCarBrand;
	//ptData->iCarSpeed;

	memcpy(&ptData->tSnap, &(ptAlgRes->tSnap), sizeof(tSnapInfo));

	int imgOk = 0;
	for(int i = 0; i < MAX_IMG_NUM && i < ALG_MAX_SNAP_NUM && ptAlgRes->tImg[i].pImageData; i++)
	{
		ptData->tPicTime[i] = ptAlgRes->tImg[i].d64TimeTick.tv_sec;
		ptData->tPicMsec[i] = ptAlgRes->tImg[i].d64TimeTick.tv_usec/1000%1000;
		ptData->tPicPos[i].x = ptAlgRes->tImg[i].tObjRect.x;
		ptData->tPicPos[i].y = ptAlgRes->tImg[i].tObjRect.y;
		ptData->tPicPos[i].w = ptAlgRes->tImg[i].tObjRect.w;
		ptData->tPicPos[i].h = ptAlgRes->tImg[i].tObjRect.h;
		ptData->iPicWidth  = ptAlgRes->tImg[i].s32ImageWidth;
		ptData->iPicHeight = ptAlgRes->tImg[i].s32ImageHeight;

		// 转换颜色空间
		ptData->pIplImage[i] = ImageInfo2IplImage(&(ptAlgRes->tImg[i]));
		if(ptData->pIplImage[i] == NULL)
		{
			printf("Img%d ImageInfo2IplImage failed\n",i);
			break;
		}

		imgOk++;
	}

	ptData->iPicNum = imgOk;
	if(ptData->iPicNum < 1)
	{
		printf("ptData->iPicNum < 1\n");
		return -1;
	}

	FileNamingT tNamingInfo = {0};
	AnalyCfgMan::GetInstance()->GetEventFileNaming(ptData->iEventType, &tNamingInfo);
	if (tNamingInfo.iTimeModel == eUseCreateTime)
	{
		ptData->tCreateTime = time(NULL);
		ptData->tCreateMsec = 0;
	}
	else if (tNamingInfo.iTimeModel == eUseStartTime)
	{
		ptData->tCreateTime = ptData->tPicTime[0];
		ptData->tCreateMsec = ptData->tPicMsec[0];
	}
	else
	{
		ptData->tCreateTime = ptData->tPicTime[ptData->iPicNum-1];
		ptData->tCreateMsec = ptData->tPicMsec[ptData->iPicNum-1];
	}
	//printf("%s %d\n", __FUNCTION__, __LINE__);

	return 0;
}

void PASRunner::ManualSnap( ManualSnapT *ptInfo )
{
	if (!m_bCreateAlg || !ptInfo)
	{
		return;
	}
	ALG_Rect rect;
	rect.x = ptInfo->tRect.x;
	rect.y = ptInfo->tRect.y;
	rect.w = ptInfo->tRect.w;
	rect.h = ptInfo->tRect.h;
	algExchange(m_iChnID, ALG_IVR_ManualIllegeStop,&rect);
	LOG_DEBUG_FMT("Channel%d ALG_IVR_ManualIllegeStop",m_iChnID);
}

void TrackColor2RGB(int iColor, RGBColorT * ptRGB)
{
	static RGBColorT sInitedRGB[6] = 
	{
		{0xA0,0x52,0x20},
		{0x00,0xff,0x00},
		{0xff,0x00,0x00},
		{0xEE,0xEE,0x00},
		{0x9B,0x30,0xFF},
		{0xCA,0xFF,0x70}
	};

	if (iColor < 0)
	{
		iColor = 0;
	}
	ptRGB->r = sInitedRGB[iColor%6].r;
	ptRGB->g = sInitedRGB[iColor%6].g;
	ptRGB->b = sInitedRGB[iColor%6].b;
}
