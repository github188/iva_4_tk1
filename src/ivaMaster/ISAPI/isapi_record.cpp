#include "isapi_record.h"
#include "mq_record.h"
#include "capacity.h"
#include "oal_time.h"
#include "oal_file.h"
#include "../Manager/RecManager.h"
#include "../Profile/SystemProfile.h"
#include "../Manager/LogManager.h"
#include "../Manager/DeviceManager.h"

namespace ISAPI
{
	namespace Record
	{
		void Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jRecCap;
			jRecCap["isSupportRecord"] = true;
			jRecCap["maxPlanInDay"] = MAX_REC_PLAN_NUM;
			JsonElement jRecFormats;

			JsonElement jRecFormatMp4;
			jRecFormatMp4["filetype"] = REC_FILE_MP4;
			jRecFormatMp4["filesuffix"] = "mp4";
			jRecFormats.append(jRecFormatMp4);

			JsonElement jRecFormatAvi;
			jRecFormatAvi["filetype"] = REC_FILE_AVI;
			jRecFormatAvi["filesuffix"] = "avi";
			jRecFormats.append(jRecFormatAvi);

			JsonElement jRecFormatFlv;
			jRecFormatFlv["filetype"] = REC_FILE_FLV;
			jRecFormatFlv["filesuffix"] = "flv";
			jRecFormats.append(jRecFormatFlv);

			jRecCap["supportfileformat"] = jRecFormats;
			jRespRoot["recordCap"] = jRecCap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Status_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			JsonElement jTempError;
			bool bGetOne = QueryGetNumberValue(tReqHead.szQuery, "chnid", iChnID, jTempError);

			JsonElement jStatusArray;
			for(int i = 0; i < MAX_CHANNEL_NUM; ++i)
			{
				if (bGetOne && iChnID != i + 1)
				{
					continue;
				}

				JsonElement jChnStatus;
				jChnStatus["chnid"] = i + 1;
				jChnStatus["recording"] = RecManager::GetInstance()->GetChnStatus(i);
				jStatusArray.append(jChnStatus);
			}

			jRespRoot["recordStatus"] = jStatusArray;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Strategy_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			RecStrategy tStrategy = {0};
			SystemProfile::GetInstance()->GetRecStrategy(&tStrategy);
	
			JsonElement jStrategy;
			jStrategy["fileformat"] = tStrategy.iFileFormat;
			jStrategy["packetmode"] = tStrategy.iPacketMode;
			jStrategy["packetsize"] = tStrategy.iPacketSize;
			jStrategy["packettime"] = tStrategy.iPacketTime;

			jRespRoot["recordStrategy"] = jStrategy;

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

			RecStrategy tStrategy = {0};

			JsonElement jStrategy;
			if(!JsonChildAsObj(jReqRoot, "recordStrategy", jStrategy, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jStrategy, "fileformat", tStrategy.iFileFormat, REC_FILE_MP4, REC_FILE_FLV, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jStrategy, "packetmode", tStrategy.iPacketMode, REC_PACKET_SIZE, REC_PACKET_TIME, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jStrategy, "packetsize", tStrategy.iPacketSize, 5, 500, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jStrategy, "packettime", tStrategy.iPacketTime, 2, 60, jRespRoot))return;

			RecStrategy tOldCfg  = {0};
			SystemProfile::GetInstance()->GetRecStrategy(&tOldCfg);
			if (memcmp(&tOldCfg, &tStrategy, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetRecStrategy(&tStrategy);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_REC, "设置录像策略");

			MQ_Record_SetStrategy(&tStrategy);
		}

		void Plan_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;
			
			RecPlan tPlan = {0};
			SystemProfile::GetInstance()->GetChnRecPlan(iChnID-1, &tPlan);

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
				
			JsonElement jRecordPlan;
			jRecordPlan["mon"] = jMon;
			jRecordPlan["tue"] = jTue;
			jRecordPlan["wed"] = jWed;
			jRecordPlan["thu"] = jThu;
			jRecordPlan["fri"] = jFri;
			jRecordPlan["sat"] = jSat;
			jRecordPlan["sun"] = jSun;

			jRespRoot["recordplan"] = jRecordPlan;

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

			RecPlan tPlan = {0};

			JsonElement jRecordPlan;
			if(!JsonChildAsObj(jReqRoot, "recordplan", jRecordPlan, jRespRoot))return;

			JsonElement jMon;
			if(!JsonChildAsArray(jRecordPlan, "mon", jMon, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jMon.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jMon, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tMon[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tMon[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jTue;
			if(!JsonChildAsArray(jRecordPlan, "tue", jTue, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jTue.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jTue, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tTue[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tTue[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jWed;
			if(!JsonChildAsArray(jRecordPlan, "wed", jWed, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jWed.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jWed, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tWed[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tWed[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jThu;
			if(!JsonChildAsArray(jRecordPlan, "thu", jThu, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jThu.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jThu, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tThu[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tThu[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jFri;
			if(!JsonChildAsArray(jRecordPlan, "fri", jFri, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jFri.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jFri, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tFri[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tFri[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jSat;
			if(!JsonChildAsArray(jRecordPlan, "sat", jSat, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jSat.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jSat, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tSat[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tSat[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			JsonElement jSun;
			if(!JsonChildAsArray(jRecordPlan, "sun", jSun, jRespRoot))return;
			for(int i = 0; i < MAX_REC_PLAN_NUM && i < (int)jSun.size(); ++i)
			{
				JsonElement jTimeSlot;
				if (!JsonArrayAsObjAt(jSun, i, jTimeSlot,jRespRoot))return;

				if(!JsonChildAsIntWithBound(jTimeSlot, "s", tPlan.tSun[i].iStartTime, 0, 86400, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jTimeSlot, "e", tPlan.tSun[i].iEndTime, 0, 86400, jRespRoot))return;
			}

			RecPlan tOldCfg  = {0};
			SystemProfile::GetInstance()->SetChnRecPlan(iChnID-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tPlan, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetChnRecPlan(iChnID-1, &tPlan);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_REC, "设置通道%d录像计划",iChnID);

			MQ_Record_SetPlan(iChnID-1, &tPlan);
		}

		void Switch_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			bool bEnable = false;
			SystemProfile::GetInstance()->GetChnRecPlanSwitch(iChnID-1, bEnable);

			jRespRoot["switch"] = bEnable;

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

			bool bEnable = false;
			if (!JsonChildAsBool(jReqRoot, "switch", bEnable, jRespRoot))return;

			bool bEnableOld;
			SystemProfile::GetInstance()->GetChnRecPlanSwitch(iChnID-1, bEnableOld);
			if (bEnableOld == bEnable)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;
			}

			int iRet = SystemProfile::GetInstance()->SetChnRecPlanSwitch(iChnID-1, bEnable);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_REC, "%s通道%d录像",bEnable?"开启":"关闭",iChnID);

			MQ_Record_SetEnable(iChnID-1, bEnable);
		}

		void Query_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jQuery;
			if(!JsonChildAsObj(jReqRoot, "recquery", jQuery, jRespRoot))return;

			RecQueryCond tCond = {0};
			tCond.iChnID = 0;
			tCond.iType = 0;
			tCond.tStart = -1;
			tCond.tEnd = -1;

			string szStartTime = "", szEndTime = "",szLike = "";
			JsonElement jTemp;
			JsonChildAsIntWithBound(jQuery, "chnid", tCond.iChnID, 0, MAX_CHANNEL_NUM, jTemp);
			JsonChildAsIntWithBound(jQuery, "type", tCond.iType, 0, REC_ROUTINE, jTemp);
			JsonChildAsString(jQuery, "start", szStartTime, jTemp);
			JsonChildAsString(jQuery, "end",   szEndTime,   jTemp);

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
			if (!JsonChildAsIntWithBound(jQuery, "pagesize", tCond.iPageSize, 1, MAX_REC_PAGE_SIZE, jRespRoot)) return;
			if (tCond.iPageNo < 0)
			{
				WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
				return;
			}

			RecQueryResult tResult = {0};
			int iRet = RecManager::GetInstance()->QueryRec(tCond, tResult);
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
				JsonElement jRecRec;

				jRecRec["rowid"] = tResult.tRecords[i].iRowID;
				jRecRec["chnid"] = tResult.tRecords[i].iChnID+1;
				jRecRec["type"]  = tResult.tRecords[i].iType;

				char szStartTime[20] = {0};
				TimeFormatString(tResult.tRecords[i].tStart, szStartTime, sizeof(szStartTime),eYMDHMS1);
				jRecRec["start"] = szStartTime;

				char szEndTime[20] = {0};
				TimeFormatString(tResult.tRecords[i].tEnd, szEndTime, sizeof(szEndTime),eYMDHMS1);
				jRecRec["end"] = szEndTime;

				char szFileUrl[256] = {0};
				File2HttpUrl(tResult.tRecords[i].szPath, szFileUrl, false);

				jRecRec["path"] = szFileUrl;
				long lSizeByte = OAL_FileSize(tResult.tRecords[i].szPath);
				jRecRec["size"] = PrintSize(((u64)lSizeByte));

				int iDiskType = DISK_ROM;
				if (strstr(tResult.tRecords[i].szPath, MOUNT_POINT_USB))
					iDiskType = DISK_USB;
				else if (strstr(tResult.tRecords[i].szPath, MOUNT_POINT_SD))
					iDiskType = DISK_SD;
				else if (strstr(tResult.tRecords[i].szPath, MOUNT_POINT_HDD))
					iDiskType = DISK_HDD;
				jRecRec["disktype"] = iDiskType;


				jRecList.append(jRecRec);
			}
			jRecResult["filelist"] = jRecList;
			jRespRoot["recresult"] = jRecResult;
			WriteJsonResultSuccess(jRespRoot);
		}

		void Operate_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iRowID = -1;
			if (!QueryGetNumberValue(tReqHead.szQuery, "rowid", iRowID, jRespRoot)) return;

			int iDiskID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "disktype", iDiskID, DISK_SD, DISK_USB,jRespRoot))return;

			int iRet = RecManager::GetInstance()->SaveAsRec(iRowID, iDiskID);
			if (iRet == -2)
			{
				WriteJsonResult(ISAPI_FAILED_OPERATE, iDiskID==DISK_USB?"The USB is not available!":"The SD card is not available!", jRespRoot);
				return;
			}
			else if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_REC, "录像%d另存到磁盘%d",iRowID, iDiskID);
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

			int iRet = RecManager::GetInstance()->DeleteRec(bDelOne?iRowID:-1);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				WriteDbLog(tReqHead, OP_LOG_SYS, "删除%s录像",bDelOne?"一条":"所有");
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}
	}
}