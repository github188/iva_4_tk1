#include "isapi_alarm.h"
#include "oal_time.h"
#include "../Manager/AlarmManager.h"
#include "../Profile/SystemProfile.h"
#include "../Manager/LogManager.h"
#include "../Manager/TempFileCleanup.h"


namespace ISAPI
{
	namespace Alarm
	{
		void Strategy_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			AlarmStrategy tStrategy  = {0};
			SystemProfile::GetInstance()->GetAlarmStrategy(&tStrategy);

			JsonElement jAlarmStrategy;
			jAlarmStrategy["sameinterval"] = tStrategy.iSameInterval;
			jAlarmStrategy["keep"] = tStrategy.iKeepDays;
			jRespRoot["alarmStrategy"] = jAlarmStrategy;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Strategy_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			JsonElement jAlarmStrategy;
			if(!JsonChildAsObj(jReqRoot, "alarmStrategy", jAlarmStrategy, jRespRoot))return;
			
			AlarmStrategy tStrategy  = {0};
			if(!JsonChildAsInt(jAlarmStrategy,"sameinterval",tStrategy.iSameInterval,jRespRoot))return;
			if(!JsonChildAsInt(jAlarmStrategy,"keep",tStrategy.iKeepDays,jRespRoot))return;

			AlarmStrategy tOldCfg  = {0};
			SystemProfile::GetInstance()->GetAlarmStrategy(&tOldCfg);
			if (memcmp(&tOldCfg, &tStrategy, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetAlarmStrategy(&tStrategy);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				WriteDbLog(tReqHead, OP_LOG_SYS, "调整告警策略");
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void Query_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jQuery;
			if(!JsonChildAsObj(jReqRoot, "alarmquery", jQuery, jRespRoot))return;

			JsonElement jTemp;
			AlarmQueryCond tCond = {0};		
			tCond.tStart = -1;
			tCond.tEnd = -1;

			string szStartTime = "", szEndTime = "",szLike = "";
			JsonChildAsString(jQuery, "start", szStartTime, jTemp);
			JsonChildAsString(jQuery, "end",   szEndTime,   jTemp);
			JsonChildAsBuffer(jQuery, "like",  tCond.szLike, sizeof(tCond.szLike)-1,jTemp);

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
			if (!JsonChildAsIntWithBound(jQuery, "pagesize", tCond.iPageSize, 1, MAX_ALARM_PAGE_SIZE, jRespRoot)) return;
			if (tCond.iPageNo < 0)
			{
				WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
				return;
			}

			AlarmQueryResult tResult = {0};
			int iRet = AlarmManager::GetInstance()->QueryAlarm(tCond, tResult);
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

			JsonElement jAlarmResult;
			jAlarmResult["reccnt"]  = tResult.iTotalRecNum;
			jAlarmResult["pageno"]  = tCond.iPageNo;
			jAlarmResult["pagecnt"] = iPageCount;

			JsonElement jAlarmList;
			for (int i = 0; i < tResult.iCurRecNum; i++)
			{
				JsonElement jAlarmRec;
				jAlarmRec["rowid"] = tResult.tRecords[i].iRowID;

				char szCreateTime[20] = {0};
				TimeFormatString(tResult.tRecords[i].tCreate, szCreateTime, sizeof(szCreateTime),eYMDHMS1);
				jAlarmRec["time"] = szCreateTime;

				jAlarmRec["message"] = tResult.tRecords[i].szContent;
				jAlarmList.append(jAlarmRec);
			}
			jAlarmResult["alarmlist"] = jAlarmList;
			jRespRoot["alarmresult"] = jAlarmResult;
			WriteJsonResultSuccess(jRespRoot);
		}

		void Export_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			JsonElement jQuery;
			if(!JsonChildAsObj(jReqRoot, "alarmquery", jQuery, jRespRoot))return;

			JsonElement jTemp;
			AlarmQueryCond tCond = {0};		
			tCond.tStart = -1;
			tCond.tEnd = -1;

			string szStartTime = "", szEndTime = "",szLike = "";
			JsonChildAsString(jQuery, "start", szStartTime, jTemp);
			JsonChildAsString(jQuery, "end",   szEndTime,   jTemp);
			JsonChildAsBuffer(jQuery, "like",  tCond.szLike, sizeof(tCond.szLike)-1,jTemp);

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

			string filepath = "";
			int iRet = AlarmManager::GetInstance()->ExportAlarm(tCond,filepath);
			if(iRet == 0)
			{
				char szTimeout[24] = {0};
				TimeFormatString(time(NULL)+EXPORT_FILE_TIME_OUT, szTimeout, 24, eYMDHMS1);

				char szFileUrl[256] = {0};
				File2HttpUrl(filepath.c_str(), szFileUrl, false);

				JsonElement jExport;
				jExport["filename"] = szFileUrl;
				jExport["expirytime"] = szTimeout;
				jRespRoot["export"] = jExport;
				WriteJsonResultSuccess(jRespRoot);

				WriteDbLog(tReqHead, OP_LOG_SYS, "导出告警");

				// 添加到临时文件管理队列
				TempFileNode tNode;
				tNode.tCrateTime = time(NULL);
				tNode.iExpiryTime = EXPORT_FILE_TIME_OUT;
				strcpy(tNode.szFilePath, filepath.c_str());
				TempFileCleanUp::GetInstance()->AddNode(tNode);

			}
			else if (iRet == -2)
			{
				WriteJsonResultError(ISAPI_FAILED_NO_DATA,jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void Operate_DELETE(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
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

			int iRet = AlarmManager::GetInstance()->DeleteAlarm(bDelOne?iRowID:-1);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				WriteDbLog(tReqHead, OP_LOG_SYS, "删除%s告警纪录",bDelOne?"一条":"所有");
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}
	}
}