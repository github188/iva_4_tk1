#include "isapi_upload.h"
#include "mq_upload.h"
#include "../Profile/SystemProfile.h"
#include "../Manager/LogManager.h"
#include "../Manager/UploadManager.h"
#include "oal_unicode.h"

namespace ISAPI
{
	namespace Upload
	{
		void Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jUploadCap;
			jUploadCap["isSupportFtp"] = true;
			jUploadCap["isSupportPlatform"] = true;
			jRespRoot["uploadCap"] = jUploadCap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Status_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			UploadStatus tStatus;
			UploadManager::GetInstance()->GetUploadStatus(&tStatus);
			JsonElement jUploadStatus;
			jUploadStatus["isUploading"] = tStatus.bUploading;
			jUploadStatus["serverStatus"] = tStatus.iServerStat;
			jUploadStatus["remaining"] = tStatus.bRemaining;
			jRespRoot["uploadStatus"] = jUploadStatus;
			WriteJsonResultSuccess(jRespRoot);
		}

		void Strategy_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			UploadStrategy tStrategy = {0};
			SystemProfile::GetInstance()->GetUploadSvrInfo(&tStrategy);

			JsonElement jUpload;
			jUpload["uploadway"] = tStrategy.iModel;

			JsonElement jFtp;
			jFtp["address"] = tStrategy.tFtpInfo.szAddr;
			jFtp["port"] = tStrategy.tFtpInfo.iPort;
			jFtp["user"] = tStrategy.tFtpInfo.szUser;
			jFtp["password"] = tStrategy.tFtpInfo.szPass;
			jUpload["ftp"] = jFtp;
			
			JsonElement jPlat;
			jPlat["address"] = tStrategy.tPlatform.szAddr;
			jPlat["port"] = tStrategy.tPlatform.iPort;
			jPlat["id"] = tStrategy.tPlatform.szPlatID;
			jUpload["platform"] = jPlat;

			JsonElement jMqtt;
			jMqtt["address"] = tStrategy.tMqttSvr.szAddr;
			jMqtt["port"] = tStrategy.tMqttSvr.iPort;
			jMqtt["id"] = tStrategy.tMqttSvr.szPlatID;
			jUpload["mqtt"] = jMqtt;

			JsonElement jBehavior;
			jBehavior["deletesuccess"] = tStrategy.bDelOk;
			jUpload["behavior"] = jBehavior;
			
			jRespRoot["upload"] = jUpload;
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

			UploadStrategy tStrategy = {0};

			JsonElement jUpload;
			if(!JsonChildAsObj(jReqRoot, "upload", jUpload, jRespRoot))return;

			if(!JsonChildAsIntWithBound(jUpload, "uploadway", tStrategy.iModel, UPLOAD_TO_FTP, UPLOAD_TO_MQTT, jRespRoot))return;

			JsonElement jFtp;
			if(!JsonChildAsObj(jUpload, "ftp", jFtp, jRespRoot))return;
			if(!JsonChildAsBuffer(jFtp, "address", tStrategy.tFtpInfo.szAddr, sizeof(tStrategy.tFtpInfo.szAddr), jRespRoot))return;
			if(!JsonChildAsInt(jFtp, "port", tStrategy.tFtpInfo.iPort, jRespRoot))return;
			if(!JsonChildAsBuffer(jFtp, "user", tStrategy.tFtpInfo.szUser, sizeof(tStrategy.tFtpInfo.szUser), jRespRoot))return;
			if(!JsonChildAsBuffer(jFtp, "password", tStrategy.tFtpInfo.szPass, sizeof(tStrategy.tFtpInfo.szPass), jRespRoot))return;

			JsonElement jPlat;
			if(!JsonChildAsObj(jUpload, "platform", jPlat, jRespRoot))return;
			if(!JsonChildAsBuffer(jPlat, "address", tStrategy.tPlatform.szAddr, sizeof(tStrategy.tPlatform.szAddr), jRespRoot))return;
			if(!JsonChildAsInt(jPlat, "port", tStrategy.tPlatform.iPort, jRespRoot))return;
			if(!JsonChildAsBuffer(jPlat, "id", tStrategy.tPlatform.szPlatID, sizeof(tStrategy.tPlatform.szPlatID), jRespRoot))return;

			JsonElement jMqtt;
			if(!JsonChildAsObj(jUpload, "mqtt", jMqtt, jRespRoot))return;
			if(!JsonChildAsBuffer(jMqtt, "address", tStrategy.tMqttSvr.szAddr, sizeof(tStrategy.tMqttSvr.szAddr), jRespRoot))return;
			if(!JsonChildAsInt(jMqtt, "port", tStrategy.tMqttSvr.iPort, jRespRoot))return;
			if(!JsonChildAsBuffer(jMqtt, "id", tStrategy.tMqttSvr.szPlatID, sizeof(tStrategy.tMqttSvr.szPlatID), jRespRoot))return;

			JsonElement jBehavior;
			if(!JsonChildAsObj(jUpload, "behavior", jBehavior, jRespRoot))return;
			if(!JsonChildAsBool(jBehavior, "deletesuccess", tStrategy.bDelOk, jRespRoot))return;

			UploadStrategy tOldCfg = {0};
			SystemProfile::GetInstance()->GetUploadSvrInfo(&tOldCfg);
			if (memcmp(&tOldCfg, &tStrategy, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetUploadSvrInfo(&tStrategy);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_SYS, "设置上传策略");

			MQ_Upload_SetStrategy(&tStrategy);
		}

		void Switch_GET( const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot )
		{
			bool bEnable = false;
			SystemProfile::GetInstance()->GetUploadSwitch(bEnable);

			jRespRoot["switch"] = bEnable;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Switch_PUT( const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot )
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			bool bEnable = false;
			if (!JsonChildAsBool(jReqRoot, "switch", bEnable, jRespRoot))return;

			bool bEnableOld;
			SystemProfile::GetInstance()->GetUploadSwitch(bEnableOld);
			if (bEnableOld == bEnable)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;
			}

			int iRet = SystemProfile::GetInstance()->SetUploadSwitch(bEnable);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_SYS, "上传%s",bEnable?"开启":"关闭");

			MQ_Upload_Enable(bEnable);
		}

		void FtpAdvance_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			FtpAdvance tAdv = {0};
			SystemProfile::GetInstance()->GetUploadFtpAdv(&tAdv);

			JsonElement jAdvance;
			jAdvance["textflag"] = (tAdv.iContentFlag&FTP_FLAG_XML)?true:false;
			jAdvance["imageflag"] = (tAdv.iContentFlag&FTP_FLAG_IMG)?true:false;
			jAdvance["recordflag"] = (tAdv.iContentFlag&FTP_FLAG_REC)?true:false;
			jAdvance["namecharset"] = tAdv.iCharSet;
			char szIllegalUtf8[512] = {0};
			char szEventUtf8[512] = {0};
			char szTollUtf8[512] = {0};
			GB2312_To_UTF8(tAdv.szIllegalDirGrammar,strlen(tAdv.szIllegalDirGrammar), szIllegalUtf8, sizeof(szIllegalUtf8));
			GB2312_To_UTF8(tAdv.szEventDirGrammar,strlen(tAdv.szEventDirGrammar), szEventUtf8, sizeof(szEventUtf8));
			GB2312_To_UTF8(tAdv.szTollDirGrammar,strlen(tAdv.szTollDirGrammar), szTollUtf8, sizeof(szTollUtf8));

			jAdvance["illegalgrammar"] = szIllegalUtf8;
			jAdvance["eventgrammar"] = szEventUtf8;
			jAdvance["tollgrammar"] = szTollUtf8;

			jRespRoot["ftpadvance"] = jAdvance;
			WriteJsonResultSuccess(jRespRoot);
		}

		void FtpAdvance_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			FtpAdvance tAdv = {0};

			JsonElement jAdvance;
			if(!JsonChildAsObj(jReqRoot, "ftpadvance", jAdvance, jRespRoot))return;

			bool bText = false, bImage = false, bRecord = false;
			if(!JsonChildAsBool(jAdvance, "textflag", bText, jRespRoot))return;
			if(!JsonChildAsBool(jAdvance, "imageflag", bImage, jRespRoot))return;
			if(!JsonChildAsBool(jAdvance, "recordflag", bRecord, jRespRoot))return;
			
			if (bText) tAdv.iContentFlag |= FTP_FLAG_XML;
			if (bImage) tAdv.iContentFlag |= FTP_FLAG_IMG;
			if (bRecord) tAdv.iContentFlag |= FTP_FLAG_REC;

			if(!JsonChildAsIntWithBound(jAdvance, "namecharset", tAdv.iCharSet, eCharset_GB2312, eCharset_UTF8, jRespRoot))return;

			char szIllegalUtf8[512] = {0};
			char szEventUtf8[512] = {0};
			char szTollUtf8[512] = {0};
			if(!JsonChildAsBuffer(jAdvance, "illegalgrammar", szIllegalUtf8, sizeof(szIllegalUtf8), jRespRoot))return;
			if(!JsonChildAsBuffer(jAdvance, "eventgrammar", szEventUtf8, sizeof(szEventUtf8), jRespRoot))return;
			if(!JsonChildAsBuffer(jAdvance, "tollgrammar", szTollUtf8, sizeof(szTollUtf8), jRespRoot))return;

			UTF8_To_GB2312(szIllegalUtf8, strlen(szIllegalUtf8), tAdv.szIllegalDirGrammar, sizeof(tAdv.szIllegalDirGrammar));
			UTF8_To_GB2312(szEventUtf8, strlen(szEventUtf8), tAdv.szEventDirGrammar, sizeof(tAdv.szEventDirGrammar));
			UTF8_To_GB2312(szTollUtf8, strlen(szTollUtf8), tAdv.szTollDirGrammar, sizeof(tAdv.szTollDirGrammar));

			FtpAdvance tOldCfg = {0};
			SystemProfile::GetInstance()->GetUploadFtpAdv(&tOldCfg);
			if (memcmp(&tOldCfg, &tAdv, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetUploadFtpAdv(&tAdv);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_SYS, "设置FTP高级参数");

			MQ_Upload_SetFtpAdv(&tAdv);
		}

	}
}
