#include "isapi_videoin.h"
#include "capacity.h"
#include "oal_unicode.h"
#include "oal_time.h"
#include "mq_onvif.h"
#include "../Manager/UserManager.h"
#include "../Manager/VideoInManager.h"
#include "../Profile/AnalyProfile.h"
#include "../Profile/SystemProfile.h"
#include "../Manager/LogManager.h"


namespace ISAPI
{
	namespace VideoIn
	{
		void Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jVICap;
			jVICap["maxChannelNum"] = MAX_CHANNEL_NUM;
			
			JsonElement jManuArray;
			for(int i = 0; i < sizeof(g_tSupportMan)/sizeof(VIManufacturer); ++i)
			{
				char szCnName2312[256]= {0};
				GB2312_To_UTF8(g_tSupportMan[i].szCnName,strlen(g_tSupportMan[i].szCnName), szCnName2312, sizeof(szCnName2312));

				JsonElement jVIMan;
				jVIMan["mancode"] = g_tSupportMan[i].iCode;
				jVIMan["enname"] = g_tSupportMan[i].szEnName;
				jVIMan["cnname"] = szCnName2312;
				jManuArray.append(jVIMan);
			}

			JsonElement jSolutionArray;
			for(int i = 0; i < sizeof(g_tSupportSolution)/sizeof(VISolution); ++i)
			{
				JsonElement jVISolution;
				jVISolution["refer"] = g_tSupportSolution[i].szReferred;
				jVISolution["width"] = g_tSupportSolution[i].iWidth;
				jVISolution["height"] = g_tSupportSolution[i].iHeight;
				jSolutionArray.append(jVISolution);
			}

			jVICap["supportManufacturer"] = jManuArray;
			jVICap["supportSolution"] = jSolutionArray;
			jRespRoot["vedioinCap"] = jVICap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Status_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			JsonElement jTempError;
			bool bGetOne = QueryGetNumberValue(tReqHead.szQuery, "chnid", iChnID, jTempError);

			JsonElement jStatusArray;
			for(int i = 0; i < MAX_CHANNEL_NUM; i++)
			{
				if (bGetOne && iChnID != i + 1)
				{
					continue;
				}

				JsonElement jChnStatus;
				jChnStatus["chnid"] = i + 1;
				jChnStatus["streamStat"] = VideoInManager::GetInstance()->GetChnStatus(i);
				jStatusArray.append(jChnStatus);
			}

			jRespRoot["videoinStatus"] = jStatusArray;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Source_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			JsonElement jTempError;
			bool bGetOne = QueryGetNumberValue(tReqHead.szQuery, "chnid", iChnID,jTempError);

			bool bFind = false;
			JsonElement jSourceArray;
			for(int i = 0; i < MAX_CHANNEL_NUM; ++i)
			{
				if (bGetOne && iChnID != i + 1)
				{
					continue;
				}
				bFind = true;

				VISource tSource = {0};
				VideoInManager::GetInstance()->GetChnVideoSource(i, tSource);

				JsonElement jChnSrc;
				jChnSrc["chnid"] = i + 1;

				char szChnName[10] = {0};
				sprintf(szChnName, "CH%d", i+1);
				JsonElement jSrc;
				jSrc["chnname"] = szChnName;
				jSrc["enable"] = tSource.bEnable;
				jSrc["cameraip"] = tSource.szIP;
				jSrc["username"] = tSource.szUser;
				jSrc["password"] = tSource.szPass;
				jSrc["mancode"] = tSource.iManuCode;
				jSrc["streamtype"] = tSource.iStreamType;
				jChnSrc["videosource"] = jSrc;

				jSourceArray.append(jChnSrc);
			}

			if (bFind)
			{
				jRespRoot["sourcelist"] = jSourceArray;
				WriteJsonResultSuccess(jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_NOCFG,jRespRoot);
			}
		}

		void Source_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iQueryChnID = -1;
			JsonElement jTempError;
			bool bPutOne = QueryGetNumberValue(tReqHead.szQuery, "chnid", iQueryChnID,jTempError);

			if (bPutOne)
			{
				if (iQueryChnID < 1 || iQueryChnID > MAX_CHANNEL_NUM)
				{
					WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
					return;
				}

				VISource tSource = {0};
				
				JsonElement jSrc;
				if(!JsonChildAsObj(jReqRoot, "videosource", jSrc, jRespRoot))return;
				if(!JsonChildAsBool(jSrc, "enable", tSource.bEnable, jRespRoot))return;
				if(!JsonChildAsBuffer(jSrc, "cameraip", tSource.szIP, sizeof(tSource.szIP), jRespRoot))return;
				if(!JsonChildAsBuffer(jSrc, "username", tSource.szUser, sizeof(tSource.szUser), jRespRoot))return;
				if(!JsonChildAsBuffer(jSrc, "password", tSource.szPass, sizeof(tSource.szPass), jRespRoot))return;
				if(!JsonChildAsIntWithBound(jSrc, "mancode", tSource.iManuCode, M_HIK, M_UNIVIEW, jRespRoot))return;
				if(!JsonChildAsIntWithBound(jSrc, "streamtype", tSource.iStreamType,STREAM_TYPE_MAIN, STREAM_TYPE_SUB, jRespRoot))return;

				VISource tOldCfg = {0};
				VideoInManager::GetInstance()->GetChnVideoSource(iQueryChnID-1, tOldCfg);
				if (memcmp(&tOldCfg, &tSource, sizeof(tOldCfg)) == 0)
				{
					WriteJsonResultSuccess(jRespRoot);
					return;// 未修改
				}

				int iRet = VideoInManager::GetInstance()->SetChnVideoSource(iQueryChnID-1, tSource);
				if (iRet != 0)
				{
					WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
					return;
				}

				WriteDbLog(tReqHead, OP_LOG_VIN, "设置通道%d视频输入",iQueryChnID);

			}
			else
			{
				JsonElement jSourceArray;
				if(!JsonChildAsArrayWithBound(jReqRoot, "sourcelist", jSourceArray, 1, MAX_CHANNEL_NUM, jRespRoot))return;

				for (int i = 0; i < (int)jSourceArray.size(); i++)
				{
					JsonElement jChnSrc;
					if(!JsonArrayAsObjAt(jSourceArray, i, jChnSrc, jRespRoot))return;

					int iChnID = -1;
					if(!JsonChildAsIntWithBound(jChnSrc, "chnid", iChnID, 1, MAX_CHANNEL_NUM, jRespRoot))return;
				
					VISource tSource = {0};

					JsonElement jSrc;
					if(!JsonChildAsObj(jChnSrc, "videosource", jSrc, jRespRoot))return;
					if(!JsonChildAsBool(jSrc, "enable", tSource.bEnable, jRespRoot))return;
					if(!JsonChildAsBuffer(jSrc, "cameraip", tSource.szIP, sizeof(tSource.szIP), jRespRoot))return;
					if(!JsonChildAsBuffer(jSrc, "username", tSource.szUser, sizeof(tSource.szUser), jRespRoot))return;
					if(!JsonChildAsBuffer(jSrc, "password", tSource.szPass, sizeof(tSource.szPass), jRespRoot))return;
					if(!JsonChildAsIntWithBound(jSrc, "mancode", tSource.iManuCode, M_HIK, M_UNIVIEW, jRespRoot))return;
					if(!JsonChildAsIntWithBound(jSrc, "streamtype", tSource.iStreamType,STREAM_TYPE_MAIN, STREAM_TYPE_SUB, jRespRoot))return;

					VISource tOldCfg = {0};
					VideoInManager::GetInstance()->GetChnVideoSource(iChnID-1, tOldCfg);
					if (memcmp(&tOldCfg, &tSource, sizeof(tOldCfg)) == 0)
					{
						continue;
					}

					int iRet = VideoInManager::GetInstance()->SetChnVideoSource(iChnID-1, tSource);
					if (iRet != 0)
					{
						WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
						return;
					}
					WriteDbLog(tReqHead, OP_LOG_VIN, "设置通道%d视频输入",iChnID);
				}
			}

			WriteJsonResultSuccess(jRespRoot);
		}

		void Source_POST(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			VISource tSource = {0};
			JsonElement jSrc;
			if(!JsonChildAsObj(jReqRoot, "videosource", jSrc, jRespRoot))return;
			if(!JsonChildAsBool(jSrc, "enable", tSource.bEnable, jRespRoot))return;
			if(!JsonChildAsBuffer(jSrc, "cameraip", tSource.szIP, sizeof(tSource.szIP), jRespRoot))return;
			if(!JsonChildAsBuffer(jSrc, "username", tSource.szUser, sizeof(tSource.szUser), jRespRoot))return;
			if(!JsonChildAsBuffer(jSrc, "password", tSource.szPass, sizeof(tSource.szPass), jRespRoot))return;
			if(!JsonChildAsIntWithBound(jSrc, "mancode", tSource.iManuCode, M_HIK, M_UNIVIEW, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jSrc, "streamtype", tSource.iStreamType,STREAM_TYPE_MAIN, STREAM_TYPE_SUB, jRespRoot))return;
			int iRet = VideoInManager::GetInstance()->AddChnVideoSource(tSource);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);
			WriteDbLog(tReqHead, OP_LOG_VIN, "添加通道视频输入");
		}

		void Source_DELETE(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iQueryChnID = -1;
			JsonElement jTempError;
			bool bDelOne = QueryGetNumberValue(tReqHead.szQuery, "chnid", iQueryChnID,jTempError);
			for(int i = 0; i < MAX_CHANNEL_NUM; ++i)
			{
				if (bDelOne && iQueryChnID != i + 1)
				{
					continue;
				}

				int iRet = VideoInManager::GetInstance()->DisableChnVideoSource(i);
				if (iRet != 0)
				{
					WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
					return;
				}
				WriteDbLog(tReqHead, OP_LOG_VIN, "删除通道%d视频输入",i+1);
			}

			WriteJsonResultSuccess(jRespRoot);
		}

		void Stream_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			JsonElement jTempError;
			bool bGetOne = QueryGetNumberValue(tReqHead.szQuery, "chnid", iChnID, jTempError);

			JsonElement jStreamArray;
			for(int i = 0; i < MAX_CHANNEL_NUM; ++i)
			{
				if (bGetOne && iChnID != i + 1)
				{
					continue;
				}

				RtspInfo tMainRtsp = {0};
				RtspInfo tSubRtsp = {0};
				VideoInManager::GetInstance()->GetChnRtsp(i, STREAM_TYPE_MAIN, tMainRtsp);
				VideoInManager::GetInstance()->GetChnRtsp(i, STREAM_TYPE_SUB, tSubRtsp);

				JsonElement jChnStream;
				jChnStream["chnid"] = i + 1;

				JsonElement jMain;
				jMain["rtsp"] = tMainRtsp.szUrl;
				jMain["width"] = tMainRtsp.iWidth;
				jMain["height"] = tMainRtsp.iHeight;
				jChnStream["main"] = jMain;

				JsonElement jSub;
				jSub["rtsp"] = tSubRtsp.szUrl;
				jSub["width"] = tSubRtsp.iWidth;
				jSub["height"] = tSubRtsp.iHeight;
				jChnStream["sub"] = jSub;

				jStreamArray.append(jChnStream);
			}

			jRespRoot["streamlist"] = jStreamArray;

			WriteJsonResultSuccess(jRespRoot);
		}

		void PTZ_Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			PTZCap tPtzCap = {0};
			VideoInManager::GetInstance()->GetChnPtzCap( iChnID-1, tPtzCap );
			
			JsonElement jPtzCap;
			jPtzCap["isSupportPtz"] = tPtzCap.bSptPtz;
			jPtzCap["maxPresetNum"] = tPtzCap.iMaxPresetNum;
			jPtzCap["isSupportAbsMove"] = tPtzCap.bSptAbsMove;
			jPtzCap["isSupportClickZoom"] = tPtzCap.bSptClickZoom;
			jRespRoot["ptzCap"] = jPtzCap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void PTZ_PresetList_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			PresetArray tPresetArray;
			memset(&tPresetArray, 0, sizeof(PresetArray));

			VISource tSource = {0};
			VideoInManager::GetInstance()->GetChnVideoSource(iChnID-1, tSource);
			if (tSource.bEnable)
			{
				int iRet = MQ_Onvif_Get_Presets( MSG_TYPE_MASTER, iChnID-1, &tPresetArray );
				if (iRet != 0)
				{
					WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
					return;
				}
			}
			
			// sort
			for (int i = 0; i < tPresetArray.iPresetNum-1; i++)
			{
				for (int j = i + 1; j < tPresetArray.iPresetNum; j++)
				{
					if (tPresetArray.arPresets[i].iPresetID > tPresetArray.arPresets[j].iPresetID)
					{
						PresetNode temp = {0};
						temp.iPresetID = tPresetArray.arPresets[i].iPresetID;
						strcpy(temp.szName, tPresetArray.arPresets[i].szName);

						tPresetArray.arPresets[i].iPresetID = tPresetArray.arPresets[j].iPresetID;
						strcpy(tPresetArray.arPresets[i].szName,tPresetArray.arPresets[j].szName);

						tPresetArray.arPresets[j].iPresetID = temp.iPresetID;
						strcpy(tPresetArray.arPresets[j].szName,temp.szName);
					}
				}
			}

			JsonElement jPresetList;
			for (int i = 0; i < tPresetArray.iPresetNum; i++)
			{
				JsonElement jPreset;
				jPreset["presetid"] = tPresetArray.arPresets[i].iPresetID;
				jPreset["presetname"] = tPresetArray.arPresets[i].szName;
				jPresetList.append(jPreset);
			}
			jRespRoot["presetlist"] = jPresetList;
			WriteJsonResultSuccess(jRespRoot);
		}

		void PTZ_Preset_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			if (AnalyProfile::GetInstance()->AlgIsRunning(iChnID-1))
			{
				WriteJsonResultError(ISAPI_FAILED_ALG_RUNNING,jRespRoot);
				return;
			}

			int iPresetID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "presetid", iPresetID, 1, MAX_PRESET_ID,jRespRoot)) return;

			int iRet = MQ_Onvif_Preset(MSG_TYPE_MASTER,iChnID-1,PRESET_GOTO,iPresetID);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void PTZ_Preset_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			if (AnalyProfile::GetInstance()->AlgIsRunning(iChnID-1))
			{
				WriteJsonResultError(ISAPI_FAILED_ALG_RUNNING,jRespRoot);
				return;
			}

			int iPresetID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "presetid", iPresetID, 1, MAX_PRESET_ID,jRespRoot)) return;

			int iRet = MQ_Onvif_Preset(MSG_TYPE_MASTER,iChnID-1,PRESET_SET,iPresetID);
			if(iRet == 0)
			{
				usleep(1000*1000);

				AbsPosition tPos = {0};
				MQ_Onvif_Get_AbsPos(MSG_TYPE_MASTER, iChnID-1, &tPos);

				SystemProfile::GetInstance()->SetPresetPosition(iChnID-1, iPresetID, &tPos);

				WriteJsonResultSuccess(jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void PTZ_Preset_POST(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			if (AnalyProfile::GetInstance()->AlgIsRunning(iChnID-1))
			{
				WriteJsonResultError(ISAPI_FAILED_ALG_RUNNING,jRespRoot);
				return;
			}

			// 分配预置位
			PresetArray tPresetArray;
			int iRet = MQ_Onvif_Get_Presets( MSG_TYPE_MASTER, iChnID-1, &tPresetArray );
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			if (tPresetArray.iPresetNum >= MAX_PRESET_NUM)
			{
				WriteJsonResult(ISAPI_FAILED_OPERATE, "reach the preset number upper limit", jRespRoot);
				return;
			}

			int iMaxID = 0;
			for (int i = 0; i < tPresetArray.iPresetNum; i++)
			{
				if (tPresetArray.arPresets[i].iPresetID > iMaxID)
				{
					iMaxID = tPresetArray.arPresets[i].iPresetID;
				}
			}
			iMaxID += 1;

			iRet = MQ_Onvif_Preset(MSG_TYPE_MASTER,iChnID-1,PRESET_SET,iMaxID);
			if(iRet == 0)
			{
				usleep(1000*1000);

				AbsPosition tPos = {0};
				MQ_Onvif_Get_AbsPos(MSG_TYPE_MASTER, iChnID-1, &tPos);

				SystemProfile::GetInstance()->SetPresetPosition(iChnID-1, iMaxID, &tPos);

				WriteJsonResultSuccess(jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void PTZ_Preset_DELETE(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			if (AnalyProfile::GetInstance()->AlgIsRunning(iChnID-1))
			{
				WriteJsonResultError(ISAPI_FAILED_ALG_RUNNING,jRespRoot);
				return;
			}

			int iPresetID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "presetid", iPresetID, 1, MAX_PRESET_ID,jRespRoot)) return;

			int iRet = MQ_Onvif_Preset(MSG_TYPE_MASTER,iChnID-1,PRESET_DEL,iPresetID);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void PTZ_Move_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			if (AnalyProfile::GetInstance()->AlgIsRunning(iChnID-1))
			{
				WriteJsonResultError(ISAPI_FAILED_ALG_RUNNING,jRespRoot);
				return;
			}

			JsonElement jPtzMove;
			if (!JsonChildAsObj(jReqRoot, "ptzmove", jPtzMove, jRespRoot)) return;

			int iAction;
			if (!JsonChildAsIntWithBound(jPtzMove, "action", iAction, 0, 13, jRespRoot)) return;

			int iVelocity;
			if (!JsonChildAsIntWithBound(jPtzMove, "velocity", iVelocity, 0, 100, jRespRoot)) return;

			int iRet = -1;
			if(iAction >= 0 && iAction <= 9)
			{
				iRet = MQ_Onvif_Ptz_Move(MSG_TYPE_MASTER,iChnID-1,iAction,iVelocity);
			}
			else if(iAction >= 10 && iAction <= 11)
			{
				iRet = MQ_Onvif_Focus_Move(MSG_TYPE_MASTER,iChnID-1,iAction-10,iVelocity);
			}
			else if(iAction >= 12 && iAction <= 13)
			{
				iRet = MQ_Onvif_Aperture_Move(MSG_TYPE_MASTER,iChnID-1,iAction-12,iVelocity);
			}

			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void PTZ_Move_DELETE(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			if (AnalyProfile::GetInstance()->AlgIsRunning(iChnID-1))
			{
				WriteJsonResultError(ISAPI_FAILED_ALG_RUNNING,jRespRoot);
				return;
			}

			JsonElement jPtzMove;
			int iRet = -1;
			if (JsonChildAsObj(jReqRoot, "ptzmove", jPtzMove, jRespRoot)) 
			{
				int iAction;
				if (!JsonChildAsIntWithBound(jPtzMove, "action", iAction, 0, 13, jRespRoot)) return;

				if(iAction >= 0 && iAction <= 9)
				{
					iRet = MQ_Onvif_Ptz_Stop(MSG_TYPE_MASTER,iChnID-1);
				}
				else if(iAction >= 10 && iAction <= 11)
				{
					iRet = MQ_Onvif_Focus_Stop(MSG_TYPE_MASTER,iChnID-1);
				}
				else if(iAction >= 12 && iAction <= 13)
				{
					iRet = MQ_Onvif_Aperture_Stop(MSG_TYPE_MASTER,iChnID-1);
				}
			}
			else
			{
				iRet = MQ_Onvif_Ptz_Stop(MSG_TYPE_MASTER,iChnID-1);
				iRet = MQ_Onvif_Focus_Stop(MSG_TYPE_MASTER,iChnID-1);
				iRet = MQ_Onvif_Aperture_Stop(MSG_TYPE_MASTER,iChnID-1);
			}

			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void PTZ_ClickZoomProxy_GET( const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot )
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			ClickZoomProxy tProxy = {0};
			SystemProfile::GetInstance()->GetClickZoomProxy( iChnID-1, &tProxy );

			JsonElement jProxy;
			jProxy["model"] = tProxy.iModel;
			jProxy["proxyaddr"] = tProxy.szAddr;
			jProxy["proxyport"] = tProxy.iPort;
			jRespRoot["clickzoomproxy"] = jProxy;

			WriteJsonResultSuccess(jRespRoot);
		}

		void PTZ_ClickZoomProxy_PUT( const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot )
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			ClickZoomProxy tProxy = {0};

			JsonElement jProxy;
			if(!JsonChildAsObj(jReqRoot, "clickzoomproxy", jProxy, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jProxy, "model", tProxy.iModel, CLICK_ZOOM_PRIVATE, CLICK_ZOOM_PROXY ,jRespRoot))return;
			if(!JsonChildAsBuffer(jProxy, "proxyaddr", tProxy.szAddr, sizeof(tProxy.szAddr), jRespRoot))return;
			if(!JsonChildAsInt(jProxy, "proxyport", tProxy.iPort, jRespRoot))return;
			int iRet = SystemProfile::GetInstance()->SetClickZoomProxy(iChnID-1, &tProxy);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			// 发送给Onvif
			iRet = MQ_Onvif_Set_Proxy(MSG_TYPE_MASTER, iChnID-1, &tProxy);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);
		}

		void PTZ_ClickZoom_PUT( const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot )
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			ClickArea tArea = {0};

			JsonElement jArea;
			if(!JsonChildAsObj(jReqRoot, "clickarea", jArea, jRespRoot))return;
			if(!JsonChildAsInt(jArea, "left",   tArea.StartPoint.x, jRespRoot))return;
			if(!JsonChildAsInt(jArea, "right",  tArea.EndPoint.x, jRespRoot))return;
			if(!JsonChildAsInt(jArea, "top",    tArea.StartPoint.y, jRespRoot))return;
			if(!JsonChildAsInt(jArea, "bottom", tArea.EndPoint.y, jRespRoot))return;

			// 发送给Onvif
			int iRet = MQ_Onvif_ClickZoom(MSG_TYPE_MASTER, iChnID-1, &tArea);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);
		}
	}
}
