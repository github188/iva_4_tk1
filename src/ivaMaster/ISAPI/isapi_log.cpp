#include "isapi_log.h"
#include "oal_time.h"
#include "../Profile/SystemProfile.h"
#include "../Manager/LogManager.h"
#include "../Manager/TempFileCleanup.h"


namespace ISAPI
{
	namespace Log
	{
		void Strategy_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iOpLogMax;
			SystemProfile::GetInstance()->GetOperateLogLimit(iOpLogMax);

			JsonElement jLogStrategy;
			jLogStrategy["oplogkeep"] = iOpLogMax;
			jRespRoot["logStrategy"] = jLogStrategy;

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

			JsonElement jLogStrategy;
			if(!JsonChildAsObj(jReqRoot, "logStrategy", jLogStrategy, jRespRoot))return;

			int iOpLogMax;
			if(!JsonChildAsInt(jLogStrategy,"oplogkeep",  iOpLogMax,   jRespRoot))return;

			int iOldCfg;
			SystemProfile::GetInstance()->GetOperateLogLimit(iOldCfg);
			if (iOldCfg ==  iOpLogMax)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetOperateLogLimit(iOpLogMax);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				WriteDbLog(tReqHead, OP_LOG_SYS, "调整日志上限:%d",iOpLogMax);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void Query_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jQuery;
			if(!JsonChildAsObj(jReqRoot, "logquery", jQuery, jRespRoot))return;

			JsonElement jTemp;
			LogQueryCond tCond = {0};		
			tCond.iType = 0;
			tCond.tStart = -1;
			tCond.tEnd = -1;

			string szStartTime = "", szEndTime = "",szLike = "";
			JsonChildAsIntWithBound(jQuery, "type", tCond.iType, MIN_OP_LOG_TYPE, MAX_OP_LOG_TYPE, jTemp);
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
			if (!JsonChildAsIntWithBound(jQuery, "pagesize", tCond.iPageSize, 1, MAX_LOG_PAGE_SIZE, jRespRoot)) return;
			if (tCond.iPageNo < 0)
			{
				WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
				return;
			}

			LogQueryResult tResult = {0};
			int iRet = LogManager::GetInstance()->QueryLog(tCond, tResult);
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
			
			JsonElement jLogResult;
			jLogResult["reccnt"]  = tResult.iTotalRecNum;
			jLogResult["pageno"]  = tCond.iPageNo;
			jLogResult["pagecnt"] = iPageCount;

			JsonElement jLogList;
			for (int i = 0; i < tResult.iCurRecNum; i++)
			{
				JsonElement jLogRec;
				jLogRec["rowid"] = tResult.tRecords[i].iRowID;
				jLogRec["type"] = tResult.tRecords[i].iType;
				
				char szCreateTime[20] = {0};
				TimeFormatString(tResult.tRecords[i].tCreate, szCreateTime, sizeof(szCreateTime),eYMDHMS1);
				jLogRec["time"] = szCreateTime;
				
				jLogRec["message"] = tResult.tRecords[i].szContent;
				jLogRec["user"] = tResult.tRecords[i].szUser;
				jLogRec["addr"] = tResult.tRecords[i].szFrom;
				jLogList.append(jLogRec);
			}
			jLogResult["loglist"] = jLogList;
			jRespRoot["logresult"] = jLogResult;
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
			if(!JsonChildAsObj(jReqRoot, "logquery", jQuery, jRespRoot))return;

			JsonElement jTemp;
			LogQueryCond tCond = {0};		
			tCond.iType = 0;
			tCond.tStart = -1;
			tCond.tEnd = -1;

			string szStartTime = "", szEndTime = "",szLike = "";
			JsonChildAsIntWithBound(jQuery, "type", tCond.iType, MIN_OP_LOG_TYPE, MAX_OP_LOG_TYPE, jTemp);
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
			int iRet = LogManager::GetInstance()->ExportLog(tCond,filepath);
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

				WriteDbLog(tReqHead, OP_LOG_SYS, "导出日志");

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
			
			int iRet = LogManager::GetInstance()->DeleteLog(bDelOne?iRowID:-1);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				WriteDbLog(tReqHead, OP_LOG_SYS, "删除%s日志纪录",bDelOne?"一条":"所有");
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}
	}
}
