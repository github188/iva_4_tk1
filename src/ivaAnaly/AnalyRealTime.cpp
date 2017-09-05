#include "AnalyRealTime.h"
#include "oal_time.h"
#include "oal_unicode.h"
#include "capacity.h"
#include "AnalyCfgMan.h"

void SendRealTimeAnalyPreview(const AnalyData * ptData)
{
	static int s_iEventId[MAX_CHANNEL_NUM] = {0};
	if(ptData == NULL)
	{
		return;
	}

	AnalyResult  tResult = {0};
	tResult.iChnID = ptData->iChnID;
	tResult.iEventID = s_iEventId[ptData->iChnID]++;//ptData->iID;
	printf("[Send to Preview]:Channel%d, %d",tResult.iChnID,tResult.iEventID);

	EventCodeT tCode = {0};
	AnalyCfgMan::GetInstance()->GetEventCode(ptData->iEventType, &tCode);
	GB2312_To_UTF8(tCode.szDesc,strlen(tCode.szDesc), tResult.szEventDesc, sizeof(tResult.szEventDesc));

	if(strlen(ptData->szPlate) > 0)
	{
		GB2312_To_UTF8((char *)(ptData->szPlate), strlen(ptData->szPlate), tResult.szPlate, sizeof(tResult.szPlate));
		printf(" %s", ptData->szPlate);
	}
	else
	{
		GB2312_To_UTF8((char *)"无车牌", strlen("无车牌"), tResult.szPlate, sizeof(tResult.szPlate));
		printf(" %s", "无车牌");
	}

	if(ptData->iPlateColor == 3)
	{
		GB2312_To_UTF8((char *)"黑色",strlen("黑色"), tResult.szPlateColor,16);
		printf(" %s","黑色");
	}
	else if(ptData->iPlateColor == 2)
	{
		GB2312_To_UTF8((char *)"蓝色",strlen("蓝色"), tResult.szPlateColor,16);
		printf(" %s","蓝色");
	}
	else if(ptData->iPlateColor == 1)
	{
		GB2312_To_UTF8((char *)"黄色",strlen("黄色"), tResult.szPlateColor,16);
		printf(" %s","黄色");
	}
	else if(ptData->iPlateColor == 0)
	{
		GB2312_To_UTF8((char *)"白色",strlen("白色"), tResult.szPlateColor,16);
		printf(" %s","白色");
	}
	else if(ptData->iPlateColor >= 4 && ptData->iPlateColor <= 10)
	{
		GB2312_To_UTF8((char *)"新能源",strlen("新能源"), tResult.szPlateColor,16);
	}
	else
	{
		GB2312_To_UTF8((char *)"其他",strlen("其他"), tResult.szPlateColor, 16);
		printf(" %s","其他");
	}

	TimeFormatString(ptData->tCreateTime, tResult.szCreateTime, sizeof(tResult.szCreateTime), eYMDHMS1);
	printf(" %s\n", tResult.szCreateTime);

	MQ_Upload_ResultPreview(&tResult);
}

void SendRealTimeAnalyTracks(int iChnID, const AnalyTracks * ptTracks)
{
	static AnalyTracks s_tOSD[MAX_CHANNEL_NUM] = {{0}};
	static AnalyTracks s_tNULLOSD = {0};
	static time_t s_lastTime[MAX_CHANNEL_NUM] = {time(NULL)};
	time_t nowTime = time(NULL);

	if(ptTracks == NULL)
	{
		ptTracks = &s_tNULLOSD;
	}

	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return;
	}

	if( (memcmp(&s_tOSD[iChnID], ptTracks, sizeof(AnalyTracks)) != 0) || 
		(nowTime - s_lastTime[iChnID] > 1 && ptTracks != &s_tNULLOSD))
	{
		memcpy(&s_tOSD[iChnID], ptTracks, sizeof(AnalyTracks));
		s_lastTime[iChnID] = nowTime;
		MQ_Upload_TrackPreview(iChnID, ptTracks);
	}
}

void SendRealTimeAnalyWechat(const AnalyData * ptData)
{
	if(ptData == NULL)
	{
		return;
	}

	if(!AnalyCfgMan::GetInstance()->ChnNotifyIsOpen(ptData->iChnID))
	{
		return;
	}

	if(ptData->iDateType == eEventData && ptData->iEventType == eIT_Illegal_Park)
	{
		PlaceInfoT  tMonitory = {0};
		AnalyCfgMan::GetInstance()->GetChnPlaceInfoT(ptData->iChnID, &tMonitory);

		AnalyNotify tNewNotify = {0};
		tNewNotify.tCreateTime = time(NULL);
		GB2312_To_UTF8((char *)(ptData->szPlate), strlen(ptData->szPlate), tNewNotify.szPlate, sizeof(tNewNotify.szPlate));
		GB2312_To_UTF8(tMonitory.szPlaceName, strlen(tMonitory.szPlaceName),tNewNotify.szPlace,sizeof(tNewNotify.szPlace));

		char bufer[36] = {0};
		TimeFormatString(tNewNotify.tCreateTime , bufer, sizeof(bufer), eYMDHMS1);
		printf("[Notify to WeChat]:%s %s\n",bufer, ptData->szPlate);

		MQ_Upload_Notify_New(&tNewNotify);
	}
}