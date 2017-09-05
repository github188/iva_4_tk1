#include "isapi_analysis.h"
#include "oal_unicode.h"
#include "oal_time.h"
#include "oal_regexp.h"
#include "mq_analysis.h"
#include "../Manager/AnalyManager.h"
#include "../Profile/AnalyProfile.h"
#include "../Manager/UserManager.h"
#include "../Manager/LogManager.h"
#include "../Profile/SystemProfile.h"

static unsigned char  CharToByte(char ch)
{
	unsigned char ret = 0;
	if(ch >= '0' && ch <= '9')
	{
		ret = ch - '0';
	}
	else if(ch >= 'A' && ch <= 'F')
	{
		ret = ch - 'A' + 10;
	}
	else if(ch >= 'a' && ch <= 'f')
	{
		ret = ch - 'a' + 10;
	}
	return ret;
}

static void StrToRGB(const char * str, RGBColorT &tColor)
{
	// str == #RRGGBB 16进制
	tColor.r = 0x00;
	tColor.g = 0x00;
	tColor.b = 0x00;

	if(str && strlen(str) >= 7)
	{
		unsigned char R1,R2,G1,G2,B1,B2;
		R1 = CharToByte(*(str+1));
		R2 = CharToByte(*(str+2));
		G1 = CharToByte(*(str+3));
		G2 = CharToByte(*(str+4));
		B1 = CharToByte(*(str+5));
		B2 = CharToByte(*(str+6));

		tColor.r = R1*16 + R2;
		tColor.g = G1*16 + G2;
		tColor.b = B1*16 + B2;
	}
}

namespace ISAPI
{
	namespace Analysis
	{
		void Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jAnalysisCap;
			jAnalysisCap["maxPlanInDay"] = MAX_ANALY_PLAN_NUM;
			jAnalysisCap["maxSceneNum"] = MAX_SCENE_NUM;
			jAnalysisCap["maxAnalysisEvent"] = eIT_Event_Max;

			bool bSupportCalibrate = false;
			bool bSupportShieldArea = false;
			bool bSupportAnalyArea = false;
			bool bSupportPolyline = false;

			JsonElement jEventList;
			for (int i = 0; i < eIT_Event_Max; i++)
			{
				EventDef tDef;
				Global_EventDef(i, &tDef);
				JsonElement jEventDef;
				jEventDef["eventid"] = i+1;

				char szNameUtf8[256] = {0};
				GB2312_To_UTF8(tDef.szName, strlen(tDef.szName),szNameUtf8, sizeof(szNameUtf8));
				jEventDef["name"] = szNameUtf8;
				jEventDef["supportEvent"] = tDef.bSupportEvent;
				JsonElement jEventImgScope;
				{
					jEventImgScope["min"] = tDef.tEventImageNum.iMin;
					jEventImgScope["max"] = tDef.tEventImageNum.iMax;
				}
				jEventDef["eventImgNum"] = jEventImgScope;

				jEventDef["supportIllegal"] = tDef.bSupportIllegal;
				JsonElement jEviModels;
				for (int i = 0; i < tDef.iEviModelNum; i++)
				{
					JsonElement jEviModel;
					jEviModel["modelid"]  = i+1;

					char szNameUtf8[256] = {0};
					GB2312_To_UTF8(tDef.tIllegalModelDef[i].szName, strlen(tDef.tIllegalModelDef[i].szName),szNameUtf8, sizeof(szNameUtf8));
					jEviModel["name"] = szNameUtf8;
					JsonElement jEventImgScope;
					{
						jEventImgScope["min"] = tDef.tIllegalModelDef[i].tImageNumScope.iMin;
						jEventImgScope["max"] = tDef.tIllegalModelDef[i].tImageNumScope.iMax;
					}
					jEviModel["imgNum"] = jEventImgScope;

					jEviModels.append(jEviModel);
				}
				jEventDef["evidenceModel"] = jEviModels;
				
				JsonElement jThresholds;
				for (int i = 0; i < tDef.iThresholdNum; i++)
				{
					JsonElement jThreshold;
					jThreshold["which"] = tDef.tThresholdDef[0].iWhich;

					char szNameUtf8[256] = {0};
					GB2312_To_UTF8(tDef.tThresholdDef[i].szName, strlen(tDef.tThresholdDef[0].szName),szNameUtf8, sizeof(szNameUtf8));
					jThreshold["name"] = szNameUtf8;

					char szUnitUtf8[256] = {0};
					GB2312_To_UTF8(tDef.tThresholdDef[i].szUnit, strlen(tDef.tThresholdDef[0].szUnit),szUnitUtf8, sizeof(szUnitUtf8));
					jThreshold["unit"] = szUnitUtf8;

					JsonElement jValueScope;
					{
						jValueScope["min"] = tDef.tThresholdDef[i].tScope.iMin;
						jValueScope["max"] = tDef.tThresholdDef[i].tScope.iMax;
					}
					jThreshold["valuescope"] = jValueScope;

					jThresholds.append(jThreshold);
				}
				jEventDef["threshold"] = jThresholds;

				if (tDef.bSupportCalibrate)	bSupportCalibrate = true;
				if (tDef.bSupportShieldArea)bSupportShieldArea = true;
				if (tDef.bSupportAnalyArea)	bSupportAnalyArea = true;
				if (tDef.bSupportPolyline)	bSupportPolyline = true;
				jEventList.append(jEventDef);
			}
			jAnalysisCap["eventList"] = jEventList;
			jAnalysisCap["supportCalibrate"] = bSupportCalibrate;
			jAnalysisCap["supportShieldArea"] = bSupportShieldArea;
			jAnalysisCap["supportAnalyArea"] = bSupportAnalyArea;
			jAnalysisCap["supportPolyline"] = bSupportPolyline;

			jRespRoot["analysisCap"] = jAnalysisCap;

			WriteJsonResultSuccess(jRespRoot);
		}
		
		void Version_GET( const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot )
		{
			char szVer[128] = {0};
			AnalyManager::GetInstance()->GetAnalyVersion(szVer);
			jRespRoot["analysisVer"] = szVer;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Status_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			jRespRoot["analysisStatus"] = AnalyManager::GetInstance()->GetAnalyStatus(iChnID-1);

			WriteJsonResultSuccess(jRespRoot);
		}

		void Switch_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			jRespRoot["switch"] = AnalyProfile::GetInstance()->AlgIsRunning(iChnID-1);

			WriteJsonResultSuccess(jRespRoot);
		}

		void Switch_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			bool bOpen = false;
			if (!JsonChildAsBool(jReqRoot, "switch", bOpen,jRespRoot))return;
			
			bool bOpenOld;
			AnalyProfile::GetInstance()->GetChnAlgSwitch(iChnID-1, bOpenOld);
			if (bOpenOld == bOpen)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = AnalyProfile::GetInstance()->SetChnAlgSwitch(iChnID-1, bOpen);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "%s通道%d算法",bOpen?"开启":"关闭",iChnID);

			MQ_Analy_Chn_AnalyEnable(iChnID-1, bOpen);
		}

		void LPR_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			LPRCfg tCfg = {0};
			SystemProfile::GetInstance()->GetLprCfg(&tCfg);
			JsonElement jLpr;
			jLpr["defprov"]		 = tCfg.iDefProv;
			jLpr["hanzi"]		 = tCfg.iHanziConf;
			jLpr["minplatesize"] = tCfg.iMinSize;
			jLpr["duptime"]		 = tCfg.iDupTime;
			jLpr["dupsens"]		 = tCfg.iDupSens;
			jRespRoot["lpr"] = jLpr;

			WriteJsonResultSuccess(jRespRoot);
		}

		void LPR_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			LPRCfg tCfg = {0};
			JsonElement jLpr;
			if (!JsonChildAsObj(jReqRoot, "lpr", jLpr, jRespRoot))return;
			if (!JsonChildAsIntWithBound(jLpr, "defprov", tCfg.iDefProv, MIN_HANZI, MAX_HANZI, jRespRoot))return;
			if (!JsonChildAsIntWithBound(jLpr, "hanzi", tCfg.iHanziConf, 0, 100, jRespRoot))return;
			if (!JsonChildAsIntWithBound(jLpr, "minplatesize", tCfg.iMinSize, 20, 500, jRespRoot))return;
			if (!JsonChildAsIntWithBound(jLpr, "duptime", tCfg.iDupTime, 0, 1440, jRespRoot))return;
			if (!JsonChildAsIntWithBound(jLpr, "dupsens", tCfg.iDupSens, 0, 6, jRespRoot))return;
			
			LPRCfg tOldCfg  = {0};
			SystemProfile::GetInstance()->GetLprCfg(&tOldCfg);
			if (memcmp(&tOldCfg, &tCfg, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetLprCfg(&tCfg);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置车牌识别参数");

			MQ_Analy_LPR(&tCfg);
		}

		void Dispatch_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			DispatchCtrlT tCfg = {0};
			SystemProfile::GetInstance()->GetDispatch(&tCfg);
			JsonElement jDispatch;
			jDispatch["sensitivity"]  = tCfg.iSensitivity;
			jDispatch["priority"]	  = tCfg.iPriority;
			jDispatch["sceneminstay"] = tCfg.iSceneMinStay;
			jDispatch["repeattime"]   = tCfg.iRepeatTime;
			jDispatch["ptzlock"]	  = tCfg.bPtzLock;
			jDispatch["locktime"]  	  = tCfg.iLockTime;
			jRespRoot["dispatch"] = jDispatch;			

			WriteJsonResultSuccess(jRespRoot);
		}

		void Dispatch_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			DispatchCtrlT tCfg = {0};
			JsonElement jDispatch;
			if (!JsonChildAsObj(jReqRoot, "dispatch", jDispatch, jRespRoot))return;
			if (!JsonChildAsIntWithBound(jDispatch, "sensitivity", tCfg.iSensitivity, eSenFast, eSenAccuracy, jRespRoot))return;
			if (!JsonChildAsIntWithBound(jDispatch, "priority", tCfg.iPriority, ePriByScene, ePriByTarget, jRespRoot))return;
			if (!JsonChildAsIntWithBound(jDispatch, "sceneminstay", tCfg.iSceneMinStay, 0, 1800, jRespRoot))return;
			if (!JsonChildAsIntWithBound(jDispatch, "repeattime", tCfg.iRepeatTime, 0, 100000, jRespRoot))return;
			if (!JsonChildAsBool(jDispatch, "ptzlock", tCfg.bPtzLock, jRespRoot))return;
			if (!JsonChildAsIntWithBound(jDispatch, "locktime", tCfg.iLockTime, 0, 1800, jRespRoot))return;

			DispatchCtrlT tOldCfg  = {0};
			SystemProfile::GetInstance()->GetDispatch(&tOldCfg);
			if (memcmp(&tOldCfg, &tCfg, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetDispatch(&tCfg);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置算法调度参数");

			MQ_Analy_Dispatch(&tCfg);
		}

		void Locations_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			PlaceInfoT tInfo = {0};
			SystemProfile::GetInstance()->GetChnPlace(iChnID-1,&tInfo);
			
			PlaceInfoT tInfoUtf8 = {0};
			GB2312_To_UTF8(tInfo.szAreaName, strlen(tInfo.szAreaName),tInfoUtf8.szAreaName, sizeof(tInfoUtf8.szAreaName));
			GB2312_To_UTF8(tInfo.szAreaID, strlen(tInfo.szAreaID),tInfoUtf8.szAreaID, sizeof(tInfoUtf8.szAreaID));
			GB2312_To_UTF8(tInfo.szMonitoryName, strlen(tInfo.szMonitoryName),tInfoUtf8.szMonitoryName, sizeof(tInfoUtf8.szMonitoryName));
			GB2312_To_UTF8(tInfo.szMonitoryID, strlen(tInfo.szMonitoryID),tInfoUtf8.szMonitoryID, sizeof(tInfoUtf8.szMonitoryID));
			GB2312_To_UTF8(tInfo.szPlaceName, strlen(tInfo.szPlaceName),tInfoUtf8.szPlaceName, sizeof(tInfoUtf8.szPlaceName));
			GB2312_To_UTF8(tInfo.szPlaceID, strlen(tInfo.szPlaceID),tInfoUtf8.szPlaceID, sizeof(tInfoUtf8.szPlaceID));
			
			JsonElement jPlace;
			jPlace["areaname"] = tInfoUtf8.szAreaName;
			jPlace["areacode"] = tInfoUtf8.szAreaID;
			jPlace["monsitename"] = tInfoUtf8.szMonitoryName;
			jPlace["monsitecode"] = tInfoUtf8.szMonitoryID;
			jPlace["placename"] = tInfoUtf8.szPlaceName;
			jPlace["placecode"] = tInfoUtf8.szPlaceID;
			jRespRoot["locations"] = jPlace;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Locations_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;
			
			JsonElement jPlace;
			if(!JsonChildAsObj(jReqRoot, "locations", jPlace, jRespRoot))return;

			PlaceInfoT tInfoUtf8 = {0};
			if(!JsonChildAsBuffer(jPlace, "areaname", tInfoUtf8.szAreaName, sizeof(tInfoUtf8.szAreaName), jRespRoot))return;
			if(!JsonChildAsBuffer(jPlace, "areacode", tInfoUtf8.szAreaID, sizeof(tInfoUtf8.szAreaID), jRespRoot))return;
			if(!JsonChildAsBuffer(jPlace, "monsitename", tInfoUtf8.szMonitoryName, sizeof(tInfoUtf8.szMonitoryName), jRespRoot))return;
			if(!JsonChildAsBuffer(jPlace, "monsitecode", tInfoUtf8.szMonitoryID, sizeof(tInfoUtf8.szMonitoryID), jRespRoot))return;
			if(!JsonChildAsBuffer(jPlace, "placename", tInfoUtf8.szPlaceName, sizeof(tInfoUtf8.szPlaceName), jRespRoot))return;
			if(!JsonChildAsBuffer(jPlace, "placecode", tInfoUtf8.szPlaceID, sizeof(tInfoUtf8.szPlaceID), jRespRoot))return;

			PlaceInfoT tInfo = {0};
			UTF8_To_GB2312(tInfoUtf8.szAreaName, strlen(tInfoUtf8.szAreaName),tInfo.szAreaName, sizeof(tInfo.szAreaName));
			UTF8_To_GB2312(tInfoUtf8.szAreaID, strlen(tInfoUtf8.szAreaID),tInfo.szAreaID, sizeof(tInfo.szAreaID));
			UTF8_To_GB2312(tInfoUtf8.szMonitoryName, strlen(tInfoUtf8.szMonitoryName),tInfo.szMonitoryName, sizeof(tInfo.szMonitoryName));
			UTF8_To_GB2312(tInfoUtf8.szMonitoryID, strlen(tInfoUtf8.szMonitoryID),tInfo.szMonitoryID, sizeof(tInfo.szMonitoryID));
			UTF8_To_GB2312(tInfoUtf8.szPlaceName, strlen(tInfoUtf8.szPlaceName),tInfo.szPlaceName, sizeof(tInfo.szPlaceName));
			UTF8_To_GB2312(tInfoUtf8.szPlaceID, strlen(tInfoUtf8.szPlaceID),tInfo.szPlaceID, sizeof(tInfo.szPlaceID));

			PlaceInfoT tOldCfg  = {0};
			SystemProfile::GetInstance()->GetChnPlace(iChnID-1,&tOldCfg);
			if (memcmp(&tOldCfg, &tInfo, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetChnPlace(iChnID-1,&tInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置通道%d地点信息", iChnID);

			WriteJsonResultSuccess(jRespRoot);

			MQ_Analy_Chn_PlaceInfo(iChnID-1, &tInfo);
		}

		void Plan_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			AnalyPlan tPlan = {0};
			SystemProfile::GetInstance()->GetChnAnalyPlan(iChnID-1, &tPlan);

			JsonElement jMon;
			for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
			{
				JsonElement jTimeSlot;
				jTimeSlot["s"] = tPlan.tMon[i].iStartTime;
				jTimeSlot["e"] = tPlan.tMon[i].iEndTime;
				jMon.append(jTimeSlot);
			}

			JsonElement jTue;
			for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
			{
				JsonElement jTimeSlot;
				jTimeSlot["s"] = tPlan.tThu[i].iStartTime;
				jTimeSlot["e"] = tPlan.tThu[i].iEndTime;
				jTue.append(jTimeSlot);
			}

			JsonElement jWed;
			for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
			{
				JsonElement jTimeSlot;
				jTimeSlot["s"] = tPlan.tWed[i].iStartTime;
				jTimeSlot["e"] = tPlan.tWed[i].iEndTime;
				jWed.append(jTimeSlot);
			}

			JsonElement jThu;
			for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
			{
				JsonElement jTimeSlot;
				jTimeSlot["s"] = tPlan.tThu[i].iStartTime;
				jTimeSlot["e"] = tPlan.tThu[i].iEndTime;
				jThu.append(jTimeSlot);
			}

			JsonElement jFri;
			for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
			{
				JsonElement jTimeSlot;
				jTimeSlot["s"] = tPlan.tFri[i].iStartTime;
				jTimeSlot["e"] = tPlan.tFri[i].iEndTime;
				jFri.append(jTimeSlot);
			}

			JsonElement jSat;
			for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
			{
				JsonElement jTimeSlot;
				jTimeSlot["s"] = tPlan.tSat[i].iStartTime;
				jTimeSlot["e"] = tPlan.tSat[i].iEndTime;
				jSat.append(jTimeSlot);
			}

			JsonElement jSun;
			for(int i = 0; i < MAX_REC_PLAN_NUM; ++i)
			{
				JsonElement jTimeSlot;
				jTimeSlot["s"] = tPlan.tSun[i].iStartTime;
				jTimeSlot["e"] = tPlan.tSun[i].iEndTime;
				jSun.append(jTimeSlot);
			}

			JsonElement jAnalyPlan;
			jAnalyPlan["mon"] = jMon;
			jAnalyPlan["tue"] = jTue;
			jAnalyPlan["wed"] = jWed;
			jAnalyPlan["thu"] = jThu;
			jAnalyPlan["fri"] = jFri;
			jAnalyPlan["sat"] = jSat;
			jAnalyPlan["sun"] = jSun;

			jRespRoot["plan"] = jAnalyPlan;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Plan_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			AnalyPlan tPlan = {0};

			JsonElement jAnalyPlan;
			if(!JsonChildAsObj(jReqRoot, "analysisplan", jAnalyPlan, jRespRoot))return;

			JsonElement jMon;
			if(!JsonChildAsArray(jAnalyPlan, "mon", jMon, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jMon.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jMon, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tMon[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tMon[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jTue;
			if(!JsonChildAsArray(jAnalyPlan, "tue", jTue, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jTue.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jTue, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tTue[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tTue[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jWed;
			if(!JsonChildAsArray(jAnalyPlan, "wed", jWed, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jWed.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jWed, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tWed[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tWed[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jThu;
			if(!JsonChildAsArray(jAnalyPlan, "thu", jThu, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jThu.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jThu, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tThu[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tThu[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jFri;
			if(!JsonChildAsArray(jAnalyPlan, "fri", jFri, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jFri.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jFri, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tFri[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tFri[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jSat;
			if(!JsonChildAsArray(jAnalyPlan, "sat", jSat, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jSat.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jSat, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tSat[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tSat[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jSun;
			if(!JsonChildAsArray(jAnalyPlan, "sun", jSun, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jSun.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jSun, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tSun[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tSun[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			AnalyPlan tOldCfg  = {0};
			SystemProfile::GetInstance()->GetChnAnalyPlan(iChnID-1,&tOldCfg);
			if (memcmp(&tOldCfg, &tPlan, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetChnAnalyPlan(iChnID-1, &tPlan);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_REC, "设置通道%d分析计划",iChnID);

			MQ_Analy_Chn_AnalyPlan(iChnID-1, &tPlan);
		}

		void PlanSwitch_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			bool bEnable = false;
			SystemProfile::GetInstance()->GetChnAnalyPlanSwitch(iChnID-1, bEnable);

			jRespRoot["switch"] = bEnable;

			WriteJsonResultSuccess(jRespRoot);
		}

		void PlanSwitch_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			bool bEnable = false;
			if (!JsonChildAsBool(jReqRoot, "switch", bEnable, jRespRoot))return;

			bool bEnableOld;
			SystemProfile::GetInstance()->GetChnAnalyPlanSwitch(iChnID-1, bEnableOld);
			if (bEnableOld == bEnable)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetChnAnalyPlanSwitch(iChnID-1, bEnable);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "通道%d分析计划%s",iChnID,bEnable?"开启":"关闭");

			MQ_Analy_Chn_AnalyPlanEnable(iChnID-1, bEnable);
		}

		void Scene_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			int iSceneID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "sceneid", iSceneID, 1, MAX_SCENE_NUM,jRespRoot)) return;
			
			SceneInfoT tInfo = {0};
			SystemProfile::GetInstance()->GetChnSceneInfo(iChnID-1, iSceneID-1, &tInfo);

			char szSceneNameUtf8[256];
			GB2312_To_UTF8(tInfo.szSceneName,strlen(tInfo.szSceneName), szSceneNameUtf8, sizeof(szSceneNameUtf8));
			
			JsonElement jScene;
			jScene["presetid"]  = tInfo.iPresetID;
			jScene["scenename"] = szSceneNameUtf8;
			jScene["waittime"]  = tInfo.iStayTime;
			jScene["direction"] = tInfo.iDirection;
			jRespRoot["scene"] = jScene;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Scene_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			int iSceneID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "sceneid", iSceneID, 1, MAX_SCENE_NUM,jRespRoot)) return;

			SceneInfoT tInfo = {0};
			JsonElement jScene;
			if(!JsonChildAsObj(jReqRoot, "scene", jScene, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jScene, "presetid", tInfo.iPresetID, -1, MAX_PRESET_ID, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jScene, "waittime", tInfo.iStayTime, 0, 24*60*60, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jScene, "direction", tInfo.iDirection, e2UN, eBOTH, jRespRoot))return;

			char szSceneNameUtf8[256];
			if(!JsonChildAsBuffer(jScene, "scenename", szSceneNameUtf8, sizeof(szSceneNameUtf8), jRespRoot))return;
			UTF8_To_GB2312(szSceneNameUtf8, strlen(szSceneNameUtf8),tInfo.szSceneName,sizeof(tInfo.szSceneName));

			int iRet = SystemProfile::GetInstance()->SetChnSceneInfo(iChnID-1, iSceneID-1, &tInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			// 创建预置位
			if(tInfo.iPresetID > 0)
			{
				MQ_Onvif_Preset(MSG_TYPE_MASTER, iChnID-1, PRESET_SET, tInfo.iPresetID);

				usleep(1000*1000);

				AbsPosition tPos = {0};
				MQ_Onvif_Get_AbsPos(MSG_TYPE_MASTER, iChnID-1, &tPos);
				LOG_DEBUG_FMT("Channel%d Preset%d PTZ:%f %f %f",iChnID, tInfo.iPresetID, tPos.fPan, tPos.fTilt, tPos.fZoom);

				SystemProfile::GetInstance()->SetPresetPosition(iChnID-1, tInfo.iPresetID, &tPos);

				MQ_Analy_Chn_Preset_Pos(iChnID-1, tInfo.iPresetID, &tPos);
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置通道%d场景%d信息",iChnID, iSceneID);

			MQ_Analy_Chn_Scn_Info(iChnID-1, iSceneID-1, &tInfo);
		}

		void Scene_Rule_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			int iSceneID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "sceneid", iSceneID, 1, MAX_SCENE_NUM,jRespRoot)) return;

			AnalyRuleT tRule = {0};
			AnalyProfile::GetInstance()->GetChnAnalyScheme(iChnID-1, iSceneID-1, &tRule);

			JsonElement jRule;
			jRule["enablesize"] = (tRule.tTargetSize.bEnable!=0?true:false);

			JsonElement jTargetMin;
			JsonElement jMinPoints;
			for(int i = 0; i < 2; i++)
			{
				JsonElement jPoint;
				jPoint["x"] = tRule.tTargetSize.atMinPoints[i].x;
				jPoint["y"] = tRule.tTargetSize.atMinPoints[i].y;
				jMinPoints.append(jPoint);
			}
			jTargetMin["pointcnt"] = 2;
			jTargetMin["points"] = jMinPoints;
			jRule["minsize"] = jTargetMin;

			JsonElement jTargetMax;
			JsonElement jMaxPoints;
			for(int i = 0; i < 2; i++)
			{
				JsonElement jPoint;
				jPoint["x"] = tRule.tTargetSize.atMaxPoints[i].x;
				jPoint["y"] = tRule.tTargetSize.atMaxPoints[i].y;
				jMaxPoints.append(jPoint);
			}
			jTargetMax["pointcnt"] = 2;
			jTargetMax["points"] = jMaxPoints;
			jRule["maxsize"] = jTargetMax;

			JsonElement jCalibration;
			jRule["clblen"] = tRule.tCalib.length;
			jRule["clbwidth"] = tRule.tCalib.width;
			JsonElement jPoints;
			for(int i = 0; i < 4; i++)
			{
				JsonElement jPoint;
				jPoint["x"] = tRule.tCalib.atPoints[i].x;
				jPoint["y"] = tRule.tCalib.atPoints[i].y;
				jPoints.append(jPoint);
			}
			jCalibration["pointcnt"] = 4;
			jCalibration["points"] = jPoints;
			jRule["clbcoord"] = jCalibration;

			JsonElement jSheildArea;
			for(int i = 0; i < tRule.iSheildAreaNum; i++)
			{
				JsonElement jPoints;
				for(int j = 0; j < tRule.atSheildAreas[i].iPtNum; j++)
				{
					JsonElement jPoint;
					jPoint["x"] = tRule.atSheildAreas[i].atPoints[j].x;
					jPoint["y"] = tRule.atSheildAreas[i].atPoints[j].y;

					jPoints.append(jPoint);
				}

				JsonElement jArea;
				jArea["points"] = jPoints;
				jArea["pointcnt"] = tRule.atSheildAreas[i].iPtNum;

				jSheildArea.append(jArea);
			}
			jRule["sheildarea"] = jSheildArea;
			jRule["sheildareacnt"] = tRule.iSheildAreaNum;
		
			JsonElement jAnalyArea;
			for(int i = 0; i < tRule.iAnalyAreaNum; i++)
			{
				JsonElement jPoints;
				for(int j = 0; j < tRule.atAnalyAreas[i].iPtNum; j++)
				{
					JsonElement jPoint;
					jPoint["x"] = tRule.atAnalyAreas[i].atPoints[j].x;
					jPoint["y"] = tRule.atAnalyAreas[i].atPoints[j].y;

					jPoints.append(jPoint);
				}

				JsonElement jArea;
				jArea["points"] = jPoints;
				jArea["pointcnt"] = tRule.atAnalyAreas[i].iPtNum;

				jAnalyArea.append(jArea);
			}
			jRule["stoparea"] = jAnalyArea;
			jRule["stopareacnt"] = tRule.iAnalyAreaNum;

			JsonElement jPolyLine;
			for(int i = 0; i < tRule.iJudgeLineNum; i++)
			{
				JsonElement jPoints;
				for(int j = 0; j < tRule.atudgeLines[i].iPtNum; j++)
				{
					JsonElement jPoint;
					jPoint["x"] = tRule.atudgeLines[i].atPoints[j].x;
					jPoint["y"] = tRule.atudgeLines[i].atPoints[j].y;

					jPoints.append(jPoint);
				}

				JsonElement jLine;
				jLine["points"] = jPoints;
				jLine["pointcnt"] = tRule.atudgeLines[i].iPtNum;

				jPolyLine.append(jLine);
			}
			jRule["solidline"] = jPolyLine;
			jRule["solidlinecnt"] = tRule.iJudgeLineNum;

			jRespRoot["rule"] = jRule;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Scene_Rule_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			int iSceneID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "sceneid", iSceneID, 1, MAX_SCENE_NUM,jRespRoot)) return;

			AnalyRuleT tRule = {0};
			JsonElement jRule;
			if(!JsonChildAsObj(jReqRoot, "rule", jRule, jRespRoot))return;

			JsonElement jTempErr;
			bool bEnableTarget = false;
			JsonChildAsBool(jRule, "enablesize", bEnableTarget, jTempErr);
			tRule.tTargetSize.bEnable = bEnableTarget?1:0;

			JsonElement jTargetMinSize;
			if(JsonChildAsObj(jRule, "minsize", jTargetMinSize, jTempErr))
			{
				JsonElement jPoints;
				JsonChildAsArray(jTargetMinSize, "points", jPoints, jTempErr);
				for(int i = 0; i < MIN(jPoints.size(),2); i++)
				{
					JsonElement jPoint;
					JsonArrayAsObjAt(jPoints, i, jPoint, jTempErr);

					JsonChildAsInt(jPoint, "x", tRule.tTargetSize.atMinPoints[i].x, jTempErr);
					JsonChildAsInt(jPoint, "y", tRule.tTargetSize.atMinPoints[i].y, jTempErr);
				}
			}

			JsonElement jTargetMaxSize;
			if(JsonChildAsObj(jRule, "maxsize", jTargetMaxSize, jTempErr))
			{
				JsonElement jPoints;
				JsonChildAsArray(jTargetMaxSize, "points", jPoints, jTempErr);
				for(int i = 0; i < MIN(jPoints.size(),2); i++)
				{
					JsonElement jPoint;
					JsonArrayAsObjAt(jPoints, i, jPoint, jTempErr);

					JsonChildAsInt(jPoint, "x", tRule.tTargetSize.atMaxPoints[i].x, jTempErr);
					JsonChildAsInt(jPoint, "y", tRule.tTargetSize.atMaxPoints[i].y, jTempErr);
				}
			}

			if(!JsonChildAsFloat(jRule, "clblen", tRule.tCalib.length, jRespRoot))return;
			if(!JsonChildAsFloat(jRule, "clbwidth", tRule.tCalib.width, jRespRoot))return;
			
			JsonElement jCalibration;
			if(JsonChildAsObj(jRule, "clbcoord", jCalibration, jTempErr))
			{
				JsonElement jPoints;
				JsonChildAsArray(jCalibration, "points",jPoints,jTempErr);
				for(int i = 0; i < MIN(jPoints.size(),4); i++)
				{
					JsonElement jPoint;
					JsonArrayAsObjAt(jPoints, i, jPoint, jTempErr);

					JsonChildAsInt(jPoint, "x", tRule.tCalib.atPoints[i].x, jTempErr);
					JsonChildAsInt(jPoint, "y", tRule.tCalib.atPoints[i].y, jTempErr);
				}
			}

			JsonElement jSheildArea;
			if(JsonChildAsArray(jRule, "sheildarea", jSheildArea, jTempErr))
			{
				tRule.iSheildAreaNum = MIN(jSheildArea.size(),MAX_SHEILD_AREA_NUM);
				for(int i = 0; i < tRule.iSheildAreaNum; i++)
				{
					JsonElement jArea;
					JsonArrayAsObjAt(jSheildArea, i, jArea, jTempErr);

					JsonElement jPoints;
					JsonChildAsArray(jArea, "points", jPoints, jTempErr);
					tRule.atSheildAreas[i].iPtNum = MIN(jPoints.size(),AREA_MAX_POINT_NUM);
					for(int j = 0; j < tRule.atSheildAreas[i].iPtNum; j++)
					{
						JsonElement jPoint;
						JsonArrayAsObjAt(jPoints, j, jPoint, jTempErr);

						JsonChildAsInt(jPoint, "x", tRule.atSheildAreas[i].atPoints[j].x, jTempErr);
						JsonChildAsInt(jPoint, "y", tRule.atSheildAreas[i].atPoints[j].y, jTempErr);
					}					
				}
			}

			JsonElement jAnalyArea;
			if(JsonChildAsArray(jRule, "stoparea", jAnalyArea, jTempErr))
			{
				tRule.iAnalyAreaNum = MIN(jAnalyArea.size(),MAX_ANALY_AREA_NUM);
				for(int i = 0; i < tRule.iAnalyAreaNum; i++)
				{
					JsonElement jArea;
					JsonArrayAsObjAt(jAnalyArea, i, jArea, jTempErr);

					JsonElement jPoints;
					JsonChildAsArray(jArea, "points", jPoints, jTempErr);
					tRule.atAnalyAreas[i].iPtNum = MIN(jPoints.size(),AREA_MAX_POINT_NUM);
					for(int j = 0; j < tRule.atAnalyAreas[i].iPtNum; j++)
					{
						JsonElement jPoint;
						JsonArrayAsObjAt(jPoints, j, jPoint, jTempErr);

						JsonChildAsInt(jPoint, "x", tRule.atAnalyAreas[i].atPoints[j].x, jTempErr);
						JsonChildAsInt(jPoint, "y", tRule.atAnalyAreas[i].atPoints[j].y, jTempErr);
					}					
				}
			}
			JsonElement jPolyLine;
			if(JsonChildAsArray(jRule, "solidline", jPolyLine, jTempErr))
			{
				tRule.iJudgeLineNum = MIN(jPolyLine.size(),MAX_ANALY_AREA_NUM);
				for(int i = 0; i < tRule.iJudgeLineNum; i++)
				{
					JsonElement jArea;
					JsonArrayAsObjAt(jPolyLine, i, jArea, jTempErr);

					JsonElement jPoints;
					JsonChildAsArray(jArea, "points", jPoints, jTempErr);
					tRule.atudgeLines[i].iPtNum = MIN(jPoints.size(),AREA_MAX_POINT_NUM);
					for(int j = 0; j < tRule.atudgeLines[i].iPtNum; j++)
					{
						JsonElement jPoint;
						JsonArrayAsObjAt(jPoints, j, jPoint, jTempErr);

						JsonChildAsInt(jPoint, "x", tRule.atudgeLines[i].atPoints[j].x, jTempErr);
						JsonChildAsInt(jPoint, "y", tRule.atudgeLines[i].atPoints[j].y, jTempErr);
					}					
				}
			}
			
			AnalyRuleT tOldCfg  = {0};
			AnalyProfile::GetInstance()->GetChnAnalyScheme(iChnID-1, iSceneID-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tRule, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = AnalyProfile::GetInstance()->SetChnAnalyScheme(iChnID-1, iSceneID-1, &tRule);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置通道%d场景%d分析规则",iChnID, iSceneID);

			MQ_Analy_Chn_Scn_Rule(iChnID-1, iSceneID-1, &tRule);
		}

		void Evidence_Determine_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			DetermineT tInfo = {0};
			SystemProfile::GetInstance()->GetEventDetermine(iEvent-1, &tInfo);
		
			JsonElement jDetermine;
			jDetermine["value1"] = tInfo.iValue1;
			jDetermine["value2"] = tInfo.iValue2;
			jDetermine["fvalue1"] = tInfo.fValue1;
			jDetermine["fvalue2"] = tInfo.iValue2;
			jRespRoot["determine"] = jDetermine;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Evidence_Determine_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			EventDef tDef;
			Global_EventDef(iEvent-1, &tDef);

			DetermineT tInfo = {0};
			JsonElement jDetermine;
			if(!JsonChildAsObj(jReqRoot, "determine", jDetermine, jRespRoot))return;
			for (int i = 0; i < tDef.iThresholdNum; i++)
			{
				if (tDef.tThresholdDef[i].iWhich == 0)
				{
					if(!JsonChildAsIntWithBound(jDetermine, "value1", tInfo.iValue1,
						tDef.tThresholdDef[i].tScope.iMin,
						tDef.tThresholdDef[i].tScope.iMax,
						jRespRoot))
						return;
				}
				if (tDef.tThresholdDef[i].iWhich == 1)
				{
					if(!JsonChildAsIntWithBound(jDetermine, "value2", tInfo.iValue2,
						tDef.tThresholdDef[i].tScope.iMin,
						tDef.tThresholdDef[i].tScope.iMax,
						jRespRoot))
						return;
				}
				if (tDef.tThresholdDef[i].iWhich == 2)
				{
					if(!JsonChildAsDoubleWithBound(jDetermine, "fvalue1", tInfo.fValue1,
						tDef.tThresholdDef[i].tScope.iMin,
						tDef.tThresholdDef[i].tScope.iMax,
						jRespRoot))
						return;
				}
				if (tDef.tThresholdDef[i].iWhich == 3)
				{
					if(!JsonChildAsDoubleWithBound(jDetermine, "fvalue1", tInfo.fValue2,
						tDef.tThresholdDef[i].tScope.iMin,
						tDef.tThresholdDef[i].tScope.iMax,
						jRespRoot))
						return;
				}
			}

			DetermineT tOldCfg  = {0};
			SystemProfile::GetInstance()->GetEventDetermine(iEvent-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tInfo, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetEventDetermine(iEvent-1, &tInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置事件%d的判决条件",iEvent);

			MQ_Analy_Ent_Determine(iEvent-1, &tInfo);
		}

		void Evidence_Model_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			EvidenceModelT tInfo = {0};
			SystemProfile::GetInstance()->GetEventEvidenceModel(iEvent-1, &tInfo);
	
			JsonElement jModel;
			jModel["enableevent"] = tInfo.bEnableEvent;
			jModel["model"] = tInfo.iModel+1;
			jRespRoot["evidencemodel"] = jModel;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Evidence_Model_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			EvidenceModelT tInfo = {0};

			JsonElement jModel;
			if(!JsonChildAsObj(jReqRoot, "evidencemodel", jModel, jRespRoot))return;
			if(!JsonChildAsBool(jModel, "enableevent", tInfo.bEnableEvent, jRespRoot))return;

			EventDef tDef = {0};
			Global_EventDef(iEvent-1, &tDef);
			if(!JsonChildAsIntWithBound(jModel, "model", tInfo.iModel, 1, tDef.iEviModelNum, jRespRoot))return;
			tInfo.iModel -= 1;

			EvidenceModelT tOldCfg  = {0};
			SystemProfile::GetInstance()->GetEventEvidenceModel(iEvent-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tInfo, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetEventEvidenceModel(iEvent-1, &tInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置事件%d的取证模式",iEvent);

			MQ_Analy_Ent_EvidenceModel(iEvent-1, &tInfo);
		}

		void Evidence_ImgMakeup_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			EventDef tDef = {0};
			Global_EventDef(iEvent-1, &tDef);

			int iModel = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "modelid", iModel, 1, tDef.iEviModelNum,jRespRoot)) return;

			ImgMakeupT tInfo = {0};
			SystemProfile::GetInstance()->GetEventImgMakeUp(iEvent-1, iModel-1, &tInfo);

			JsonElement jMakeup;
			jMakeup["picNum"] = tInfo.iImgNum;
			JsonElement jSeqs;
			for (int i = 0; i < tInfo.iImgNum; i++)
			{
				JsonElement jSeq;
				jSeq["snaptype"] = tInfo.iSnapPos[i];
				jSeq["waittime"] = tInfo.iInterval[i];
				jSeqs.append(jSeq);
			}
			jMakeup["sequence"] = jSeqs;
			jRespRoot["makeup"] = jMakeup;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Evidence_ImgMakeup_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			EventDef tDef;
			Global_EventDef(iEvent-1, &tDef);

			int iModel = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "modelid", iModel, 1, tDef.iEviModelNum,jRespRoot)) return;

			ImgMakeupT tInfo = {0};
			JsonElement jMakeup;
			if(!JsonChildAsObj(jReqRoot, "makeup", jMakeup, jRespRoot))return;
		
			if(!JsonChildAsIntWithBound(jMakeup, "picNum", tInfo.iImgNum,
				tDef.tIllegalModelDef[iModel-1].tImageNumScope.iMin,
				tDef.tIllegalModelDef[iModel-1].tImageNumScope.iMax,
				jRespRoot))return;

			JsonElement jSeqs;
			if(!JsonChildAsArrayWithBound(jMakeup, "sequence", jSeqs, tInfo.iImgNum, tDef.tIllegalModelDef[iModel-1].tImageNumScope.iMax, jRespRoot))return;

			for (int i = 0; i < tInfo.iImgNum; i++)
			{
				JsonElement jSeq;
				if (!JsonArrayAsObjAt(jSeqs, i, jSeq, jRespRoot))return;
				
				if (!JsonChildAsIntWithBound(jSeq, "snaptype", tInfo.iSnapPos[i],eFull, eFeature,jRespRoot))return;
				if (!JsonChildAsIntWithBound(jSeq, "waittime", tInfo.iInterval[i],0, 600,jRespRoot))return;
			}

			ImgMakeupT tOldCfg  = {0};
			SystemProfile::GetInstance()->GetEventImgMakeUp(iEvent-1, iModel-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tInfo, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetEventImgMakeUp(iEvent-1, iModel-1, &tInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置事件%d %s模式的取证构成",iEvent,tDef.tIllegalModelDef[iModel-1].szName);

			MQ_Analy_Ent_ImgMakeup(iEvent-1, iModel-1, &tInfo);
		}

		void Evidence_ImgCompose_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			ImgComposeT tInfo = {0};
			SystemProfile::GetInstance()->GetEventImgCom(iEvent-1, &tInfo);

			JsonElement jCompose;
			jCompose["com2"] = tInfo.iComModel2;
			jCompose["com3"] = tInfo.iComModel3;
			jCompose["com4"] = tInfo.iComModel4;
			jRespRoot["compose"] = jCompose;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Evidence_ImgCompose_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			ImgComposeT tInfo = {0};
			JsonElement jCompose;
			if(!JsonChildAsObj(jReqRoot, "compose", jCompose, jRespRoot))return;

			if(!JsonChildAsIntWithBound(jCompose, "com2", tInfo.iComModel2, eNotCom, eHorCom, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jCompose, "com3", tInfo.iComModel3, eNotCom, eHorCom, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jCompose, "com4", tInfo.iComModel4, eNotCom, eMatCom, jRespRoot))return;

			ImgComposeT tOldCfg  = {0};
			SystemProfile::GetInstance()->GetEventImgCom(iEvent-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tInfo, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetEventImgCom(iEvent-1, &tInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置事件%d图片合成参数",iEvent);

			MQ_Analy_Ent_ImgCom(iEvent-1, &tInfo);
		}

		void Evidence_ImgOSD_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			ImgOSDT tInfo = {0};
			SystemProfile::GetInstance()->GetEventImgOsd(iEvent-1, &tInfo);

			JsonElement jOsd;
			jOsd["posmodel"] = tInfo.iPosModel;
			JsonElement jContent;
			jContent["osd_time"] = ((tInfo.iContentFlag&OSD_TIME)==OSD_TIME);
			jContent["osd_mesc"] = ((tInfo.iContentFlag&OSD_MESC)==OSD_MESC);
			jContent["osd_devid"] = ((tInfo.iContentFlag&OSD_DEVID)==OSD_DEVID);
			jContent["osd_place"] = ((tInfo.iContentFlag&OSD_PLACE)==OSD_PLACE);
			jContent["osd_event"] = ((tInfo.iContentFlag&OSD_EVENT)==OSD_EVENT);
			jContent["osd_code"] = ((tInfo.iContentFlag&OSD_CODE)==OSD_CODE);
			jContent["osd_plate"] = ((tInfo.iContentFlag&OSD_PLATE)==OSD_PLATE);
			jContent["osd_platecolor"] = ((tInfo.iContentFlag&OSD_PLATE_COLOR)==OSD_PLATE_COLOR);
			jContent["osd_direction"] = ((tInfo.iContentFlag&OSD_DIRECTION)==OSD_DIRECTION);
			jContent["osd_watermark"] = ((tInfo.iContentFlag&OSD_WATERMARK)==OSD_WATERMARK);
			jOsd["content"] = jContent;
			jOsd["left"] = tInfo.iLeft;
			jOsd["top"] = tInfo.iTop;
			jOsd["fontsize"] = tInfo.iFontSize;
			char szFontColor[8];
			sprintf(szFontColor, "#%02x%02x%02x", tInfo.tFontColor.r, tInfo.tFontColor.g, tInfo.tFontColor.b);
			char szBackColor[8];
			sprintf(szBackColor, "#%02x%02x%02x", tInfo.tBackColor.r, tInfo.tBackColor.g, tInfo.tBackColor.b);
			jOsd["fontcolor"] = szFontColor;
			jOsd["backcolor"] = szBackColor;
			jRespRoot["osd"] = jOsd;			

			WriteJsonResultSuccess(jRespRoot);
		}

		void Evidence_ImgOSD_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			ImgOSDT tInfo = {0};
			JsonElement jOsd;
			if(!JsonChildAsObj(jReqRoot, "osd", jOsd, jRespRoot))return;

			if(!JsonChildAsIntWithBound(jOsd, "posmodel", tInfo.iPosModel, eEachUp, eComDown, jRespRoot))return;

			JsonElement jContent;
			if(!JsonChildAsObj(jOsd, "content", jContent, jRespRoot))return;
			
			JsonElement jError;
			bool bEnable;
			tInfo.iContentFlag = 0;
			bEnable = false;
			JsonChildAsBool(jContent, "osd_time", bEnable, jError);
			tInfo.iContentFlag |= (bEnable?OSD_TIME:0);

			bEnable = false;
			JsonChildAsBool(jContent, "osd_mesc", bEnable, jError);
			tInfo.iContentFlag |= (bEnable?OSD_MESC:0);

			bEnable = false;
			JsonChildAsBool(jContent, "osd_devid", bEnable, jError);
			tInfo.iContentFlag |= (bEnable?OSD_DEVID:0);

			bEnable = false;
			JsonChildAsBool(jContent, "osd_place", bEnable, jError);
			tInfo.iContentFlag |= (bEnable?OSD_PLACE:0);

			bEnable = false;
			JsonChildAsBool(jContent, "osd_event", bEnable, jError);
			tInfo.iContentFlag |= (bEnable?OSD_EVENT:0);

			bEnable = false;
			JsonChildAsBool(jContent, "osd_code", bEnable, jError);
			tInfo.iContentFlag |= (bEnable?OSD_CODE:0);

			bEnable = false;
			JsonChildAsBool(jContent, "osd_plate", bEnable, jError);
			tInfo.iContentFlag |= (bEnable?OSD_PLATE:0);

			bEnable = false;
			JsonChildAsBool(jContent, "osd_platecolor", bEnable, jError);
			tInfo.iContentFlag |= (bEnable?OSD_PLATE_COLOR:0);

			bEnable = false;
			JsonChildAsBool(jContent, "osd_direction", bEnable, jError);
			tInfo.iContentFlag |= (bEnable?OSD_DIRECTION:0);

			bEnable = false;
			JsonChildAsBool(jContent, "osd_watermark", bEnable, jError);
			tInfo.iContentFlag |= (bEnable?OSD_WATERMARK:0);

			if(!JsonChildAsIntWithBound(jOsd, "left", tInfo.iLeft, 0, 100, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jOsd, "top", tInfo.iTop, 0, 100, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jOsd, "fontsize", tInfo.iFontSize, 32, 80, jRespRoot))return;
			char szFontColor[8];
			if(!JsonChildAsBuffer(jOsd, "fontcolor", szFontColor, sizeof(szFontColor), jRespRoot))return;
			char szBackColor[8];
			if(!JsonChildAsBuffer(jOsd, "backcolor", szBackColor, sizeof(szBackColor), jRespRoot))return;
			StrToRGB(szFontColor, tInfo.tFontColor);
			StrToRGB(szBackColor, tInfo.tBackColor);

			ImgOSDT tOldCfg  = {0};
			SystemProfile::GetInstance()->GetEventImgOsd(iEvent-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tInfo, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetEventImgOsd(iEvent-1, &tInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置事件%d图片OSD参数",iEvent);

			MQ_Analy_Ent_ImgOSD(iEvent-1, &tInfo);
		}

		void Evidence_ImgQuality_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			ImgQualityT tInfo = {0};
			SystemProfile::GetInstance()->GetEventImgQuality(iEvent-1, &tInfo);
		
			JsonElement jQuality;
			jQuality["sizelimit"] = tInfo.iSizeLimit;
			jQuality["widthlimit"] = tInfo.iWidthLimit;
			jQuality["heightlimit"] = tInfo.iHeightLimit;
			jRespRoot["quality"] = jQuality;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Evidence_ImgQuality_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			ImgQualityT tInfo = {0};
			JsonElement jQuality;
			if(!JsonChildAsObj(jReqRoot, "quality", jQuality, jRespRoot))return;

			if(!JsonChildAsIntWithBound(jQuality, "sizelimit", tInfo.iSizeLimit, 0, 10240, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jQuality, "widthlimit", tInfo.iWidthLimit, 0, 19200, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jQuality, "heightlimit", tInfo.iHeightLimit, 0, 10800, jRespRoot))return;

			ImgQualityT tOldCfg  = {0};
			SystemProfile::GetInstance()->GetEventImgQuality(iEvent-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tInfo, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetEventImgQuality(iEvent-1, &tInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置事件%d图片质量参数",iEvent);

			MQ_Analy_Ent_ImgQuality(iEvent-1, &tInfo);
		}

		void Evidence_Record_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			RecEvidenceT tInfo = {0};
			SystemProfile::GetInstance()->GetEventRec(iEvent-1, &tInfo);

			JsonElement jAnalyRecord;
			jAnalyRecord["enable"] = tInfo.bEnable;
			jAnalyRecord["model"] = tInfo.iRecModel;
			jAnalyRecord["fileformat"] = tInfo.iFileFormat;
			jAnalyRecord["pretime"] = tInfo.iPreTime;
			jAnalyRecord["afttime"] = tInfo.iAftTime;
			jAnalyRecord["mintime"] = tInfo.iMinTime;
			jRespRoot["analyrecord"] = jAnalyRecord;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Evidence_Record_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			RecEvidenceT tInfo = {0};
			JsonElement jAnalyRecord;
			if(!JsonChildAsObj(jReqRoot, "analyrecord", jAnalyRecord, jRespRoot))return;
			if(!JsonChildAsBool(jAnalyRecord, "enable", tInfo.bEnable, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jAnalyRecord, "model", tInfo.iRecModel, eRecWhole, eRecJoint, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jAnalyRecord, "fileformat", tInfo.iFileFormat, REC_FILE_MP4, REC_FILE_FLV, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jAnalyRecord, "pretime", tInfo.iPreTime, 0, 300, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jAnalyRecord, "afttime", tInfo.iAftTime, 0, 300, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jAnalyRecord, "mintime", tInfo.iMinTime, 0, 600, jRespRoot))return;

			RecEvidenceT tOldCfg  = {0};
			SystemProfile::GetInstance()->GetEventRec(iEvent-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tInfo, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetEventRec(iEvent-1, &tInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置事件%d录像取证参数",iEvent);

			MQ_Analy_Ent_RecEvidence(iEvent-1, &tInfo);
		}

		void Evidence_Code_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iEvent = -1;
			JsonElement jTempError;
			bool bGetOne = QueryGetNumberValue(tReqHead.szQuery, "eventid", iEvent, jTempError);

			JsonElement jCodeArray;
			for(int i = 0; i < eIT_Event_Max; ++i)
			{
				if (bGetOne && iEvent != i + 1)
				{
					continue;
				}
				EventCodeT tInfo = {0};
				SystemProfile::GetInstance()->GetEventCodeInfo(iEvent-1, &tInfo);
				
				JsonElement jCodeInfo;
				jCodeInfo["eventid"] = i + 1;
				JsonElement jCode;
				char szCodeUtf8[64] = {0};
				GB2312_To_UTF8(tInfo.szCode,strlen(tInfo.szCode), szCodeUtf8, sizeof(szCodeUtf8));
				jCode["code"] = szCodeUtf8;

				char szDescUtf8[1024] = {0};
				GB2312_To_UTF8(tInfo.szDesc,strlen(tInfo.szDesc), szDescUtf8, sizeof(szDescUtf8));
				jCode["desc"] = szDescUtf8;
				jCodeInfo["eventcode"] = jCode;
				jCodeArray.append(jCodeInfo);
			}

			jRespRoot["eventcodelist"] = jCodeArray;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Evidence_Code_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			JsonElement jCode;
			if(!JsonChildAsObj(jReqRoot, "eventcode", jCode, jRespRoot))return;
			char szCodeUtf8[64] = {0};
			if(!JsonChildAsBuffer(jCode, "code", szCodeUtf8, sizeof(szCodeUtf8), jRespRoot))return;
	
			char szDescUtf8[1024] = {0};
			if(!JsonChildAsBuffer(jCode, "desc", szDescUtf8, sizeof(szDescUtf8), jRespRoot))return;

			EventCodeT tInfo = {0};
			UTF8_To_GB2312(szCodeUtf8, strlen(szCodeUtf8), tInfo.szCode, sizeof(tInfo.szCode));
			UTF8_To_GB2312(szDescUtf8, strlen(szDescUtf8), tInfo.szDesc, sizeof(tInfo.szDesc));

			EventCodeT tOldCfg  = {0};
			SystemProfile::GetInstance()->GetEventCodeInfo(iEvent-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tInfo, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetEventCodeInfo(iEvent-1, &tInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置事件%d代码",iEvent);

			MQ_Analy_Ent_Code(iEvent-1, &tInfo);
		}

		void Evidence_FileNaming_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			FileNamingT tInfo = {0};
			SystemProfile::GetInstance()->GetEventFileNamingInfo(iEvent-1, &tInfo);

			JsonElement jFileNaming;

			jFileNaming["timemodel"] = tInfo.iTimeModel;

			char szImgUtf8[256] = {0};
			GB2312_To_UTF8(tInfo.szIllegalImg,strlen(tInfo.szIllegalImg), szImgUtf8, sizeof(szImgUtf8));
			jFileNaming["img"] = szImgUtf8;

			char szRecUtf8[256] = {0};
			GB2312_To_UTF8(tInfo.szIllegalRec,strlen(tInfo.szIllegalRec), szRecUtf8, sizeof(szRecUtf8));
			jFileNaming["rec"] = szRecUtf8;

			char szEImgUtf8[256] = {0};
			GB2312_To_UTF8(tInfo.szEventImg,strlen(tInfo.szEventImg), szEImgUtf8, sizeof(szEImgUtf8));
			jFileNaming["eventimg"] = szEImgUtf8;

			char szERecUtf8[256] = {0};
			GB2312_To_UTF8(tInfo.szEventRec,strlen(tInfo.szEventRec), szERecUtf8, sizeof(szERecUtf8));
			jFileNaming["eventrec"] = szERecUtf8;

			jRespRoot["filenaming"] = jFileNaming;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Evidence_FileNaming_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iEvent = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "eventid", iEvent, 1, eIT_Event_Max,jRespRoot)) return;

			FileNamingT tInfo = {0};

			JsonElement jFileNaming;
			if(!JsonChildAsObj(jReqRoot, "filenaming", jFileNaming, jRespRoot))return;

			if(!JsonChildAsIntWithBound(jFileNaming, "timemodel", tInfo.iTimeModel, eUseStartTime, eUseCreateTime, jRespRoot))return;

			char szImgUtf8[256] = {0};
			if(!JsonChildAsBuffer(jFileNaming, "img", szImgUtf8, sizeof(szImgUtf8), jRespRoot))return;

			char szRecUtf8[256] = {0};
			if(!JsonChildAsBuffer(jFileNaming, "rec", szRecUtf8, sizeof(szRecUtf8), jRespRoot))return;

			char szEImgUtf8[256] = {0};
			if(!JsonChildAsBuffer(jFileNaming, "eventimg", szEImgUtf8, sizeof(szEImgUtf8), jRespRoot))return;

			char szERecUtf8[256] = {0};
			if(!JsonChildAsBuffer(jFileNaming, "eventrec", szERecUtf8, sizeof(szERecUtf8), jRespRoot))return;

			UTF8_To_GB2312(szImgUtf8, strlen(szImgUtf8), tInfo.szIllegalImg, sizeof(tInfo.szIllegalImg));
			UTF8_To_GB2312(szRecUtf8, strlen(szRecUtf8), tInfo.szIllegalRec, sizeof(tInfo.szIllegalRec));
			UTF8_To_GB2312(szEImgUtf8, strlen(szEImgUtf8), tInfo.szEventImg, sizeof(tInfo.szEventImg));
			UTF8_To_GB2312(szERecUtf8, strlen(szERecUtf8), tInfo.szEventRec, sizeof(tInfo.szEventRec));

			FileNamingT tOldCfg  = {0};
			SystemProfile::GetInstance()->GetEventFileNamingInfo(iEvent-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tInfo, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetEventFileNamingInfo(iEvent-1, &tInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_ALG, "配置事件%d文件命名规则",iEvent);

			MQ_Analy_Ent_FileNaming(iEvent-1, &tInfo);
		}
	
		void ManualSnap_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			ManualSnapT  tSnap = {0};

			JsonElement jManualSnap;
			if(!JsonChildAsObj(jReqRoot, "manualsnap", jManualSnap, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jManualSnap, "eventid", tSnap.iEntID, 1,eIT_Event_Max,jRespRoot))return;

			JsonElement jArea;
			int iRigth, iBottom;
			if(!JsonChildAsObj(jManualSnap, "snapeara", jArea, jRespRoot))return;
			if(!JsonChildAsInt(jArea, "left",tSnap.tRect.x,jRespRoot))return;
			if(!JsonChildAsInt(jArea, "right",iRigth,jRespRoot))return;
			if(!JsonChildAsInt(jArea, "top",tSnap.tRect.y,jRespRoot))return;
			if(!JsonChildAsInt(jArea, "bottom",iBottom,jRespRoot))return;
			tSnap.tRect.w = iRigth - tSnap.tRect.x;
			tSnap.tRect.h = iBottom - tSnap.tRect.y;
			tSnap.iEntID  -= 1;

			EventDef tDef = {0};
			Global_EventDef(tSnap.iEntID, &tDef);
			WriteDbLog(tReqHead, OP_LOG_ALG, "手动抓拍%s",tDef.szName);

			MQ_Analy_Chn_ManualSnap(iChnID-1, &tSnap);

			WriteJsonResultSuccess(jRespRoot);
		}

		void Results_Query_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jQuery;
			if(!JsonChildAsObj(jReqRoot, "analysisquery", jQuery, jRespRoot))return;

			AnalyQueryCond tCond = {0};
			tCond.iChnID = 0;
			tCond.iEventID = 0;
			tCond.iDataType = 0;
			tCond.iPlateColor = 0;
			tCond.tStart = -1;
			tCond.tEnd = -1;
	
			string szStartTime = "", szEndTime = "",szPlate = "";
			JsonElement jTemp;
			JsonChildAsIntWithBound(jQuery, "chnid", tCond.iChnID, 0, MAX_CHANNEL_NUM, jTemp);
			JsonChildAsIntWithBound(jQuery, "eventid", tCond.iEventID, 0, eIT_Event_Max, jTemp);
			JsonChildAsIntWithBound(jQuery, "datetype", tCond.iDataType, 0, eTollData, jTemp);
			JsonChildAsIntWithBound(jQuery, "platecolor", tCond.iPlateColor, 0, 7, jTemp);
			JsonChildAsString(jQuery, "start", szStartTime, jTemp);
			JsonChildAsString(jQuery, "end",   szEndTime,   jTemp);
			JsonChildAsString(jQuery, "plate", szPlate,   jTemp);

			if (!szStartTime.empty())
			{
				tCond.tStart = String2Time(szStartTime.c_str(),eYMDHMS1);
				if (tCond.tStart <= 0)
				{
					WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
					return;
				}
			}

			if (!szEndTime.empty())
			{
				tCond.tEnd = String2Time(szEndTime.c_str(),eYMDHMS1);
				if (tCond.tEnd <= 0)
				{
					WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
					return;
				}
			}

			// 必须的查询条件
			if (!JsonChildAsInt(jQuery, "pageno", tCond.iPageNo, jRespRoot)) return;
			if (!JsonChildAsIntWithBound(jQuery, "pagesize", tCond.iPageSize, 1, MAX_ANALY_PAGE_SIZE, jRespRoot)) return;
			if (tCond.iPageNo < 0)
			{
				WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
				return;
			}

			AnalyQueryResult tResult = {0};
			int iRet = AnalyManager::GetInstance()->Query(tCond, tResult);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
				return;
			}

			int iPageCount = tResult.iTotalRecNum / tCond.iPageSize;
			if (tResult.iTotalRecNum % tCond.iPageSize != 0)
			{
				iPageCount++;
			}
			if (iPageCount == 0)
			{
				iPageCount = 1;
			}

			JsonElement jRecResult;
			jRecResult["reccnt"]  = tResult.iTotalRecNum;
			jRecResult["pageno"]  = tCond.iPageNo;
			jRecResult["pagecnt"] = iPageCount;

			JsonElement jRecList;
			for (int i = 0; i < tResult.iCurRecNum; i++)
			{
				char szTime[20] = {0};
				TimeFormatString(tResult.tRecords[i].tCreate, szTime, sizeof(szTime),eYMDHMS1);

				JsonElement jRecRec;
				jRecRec["rowid"] = tResult.tRecords[i].iRowID;
				jRecRec["chnid"] = tResult.tRecords[i].iChnID+1;
				jRecRec["eventid"] = tResult.tRecords[i].iEventID+1;
				jRecRec["datetype"]  = tResult.tRecords[i].iDataType;
				jRecRec["plate"]  = tResult.tRecords[i].szPlate;
				jRecRec["platecolor"]  = tResult.tRecords[i].iPlateColor+1;
				jRecRec["time"] = szTime;

				JsonElement jImgs;
				//for (int n = tResult.tRecords[i].iImgNum-1; n >=0 ; n--)
				for (int n = 0; n < tResult.tRecords[i].iImgNum; n++)
				{
					char szFilePath[512] = {0};
					sprintf(szFilePath, "%s%s", tResult.tRecords[i].szImgRoot,tResult.tRecords[i].szImgPath[n]);
					char szFileUrl[256] = {0};
					File2HttpUrl(szFilePath, szFileUrl, false);

					JsonElement jImgUrl;
					jImgUrl["url"] = szFileUrl;
					jImgs.append(jImgUrl);
				}
				jRecRec["imgs"] = jImgs;

				JsonElement jRecs;
				//for (int n = tResult.tRecords[i].iRecNum-1; n >=0 ; n--)
				for (int n = 0; n < tResult.tRecords[i].iRecNum; n++)
				{
					char szFilePath[512] = {0};
					sprintf(szFilePath, "%s%s", tResult.tRecords[i].szRecRoot,tResult.tRecords[i].szRecPath[n]);

					char szFileUrl[256] = {0};
					File2HttpUrl(szFilePath, szFileUrl, false);

					JsonElement jRecUrl;
					jRecUrl["url"] = szFileUrl;
					jRecs.append(jRecUrl);
				}
				jRecRec["recs"] = jRecs;

				jRecRec["status"] = tResult.tRecords[i].iStatus;
				jRecList.append(jRecRec);
			}
			jRecResult["analylist"] = jRecList;
			jRespRoot["analysisresult"] = jRecResult;
			WriteJsonResultSuccess(jRespRoot);
		}

		void Results_Operate_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iRowID = -1;
			if (!QueryGetNumberValue(tReqHead.szQuery, "rowid", iRowID,jRespRoot)) return;

			int iRet = AnalyManager::GetInstance()->ReUpload(iRowID);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);
			WriteDbLog(tReqHead, OP_LOG_ALG, "重传一条分析记录ID=%d",iRowID);
		}

		void Results_Operate_DELETE( const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot )
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iRowID = -1;
			JsonElement jTempError;
			bool bDelOne = QueryGetNumberValue(tReqHead.szQuery, "rowid", iRowID, jTempError);

			int iRet = AnalyManager::GetInstance()->Delete(bDelOne?iRowID:-1);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				WriteDbLog(tReqHead, OP_LOG_ALG, "删除%s分析记录",bDelOne?"一条":"所有");
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}
	}
}
