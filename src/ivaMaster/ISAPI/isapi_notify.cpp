#include "isapi_notify.h"
#include "mq_upload.h"
#include "../Profile/SystemProfile.h"
#include "../Manager/LogManager.h"
#include "../Manager/UploadManager.h"

namespace ISAPI
{
	namespace Notify
	{
		void Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jNotifyCap;
			jNotifyCap["isSupportWeChat"] = true;
			jRespRoot["notifyCap"] = jNotifyCap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Strategy_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			NotifySvr tStrategy = {0};
			SystemProfile::GetInstance()->GetNotifySvrInfo(&tStrategy);
		
			JsonElement jNotify;
			jNotify["address"] = tStrategy.szAddr;
			jNotify["port"] = tStrategy.iPort;
			jNotify["attachimage"] = tStrategy.bAttachImg;

			jRespRoot["notify"] = jNotify;
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

			NotifySvr tStrategy = {0};

			JsonElement jNotify;
			if(!JsonChildAsObj(jReqRoot, "notify", jNotify, jRespRoot))return;

			if(!JsonChildAsBuffer(jNotify, "address", tStrategy.szAddr, sizeof(tStrategy.szAddr), jRespRoot))return;
			if(!JsonChildAsInt(jNotify, "port", tStrategy.iPort, jRespRoot))return;
			if(!JsonChildAsBool(jNotify, "attachimage", tStrategy.bAttachImg, jRespRoot))return;

			NotifySvr tOldCfg  = {0};
			SystemProfile::GetInstance()->GetNotifySvrInfo(&tOldCfg);
			if (memcmp(&tOldCfg, &tStrategy, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetNotifySvrInfo(&tStrategy);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			WriteJsonResultSuccess(jRespRoot);

			MQ_Upload_Notify_SetServer(&tStrategy);

			WriteDbLog(tReqHead, OP_LOG_SYS, "设置提醒策略");
		}

		void Switch_GET( const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot )
		{
			bool bEnable = false;
			SystemProfile::GetInstance()->GetNotifySwitch(bEnable);

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
			SystemProfile::GetInstance()->GetNotifySwitch(bEnableOld);
			if (bEnableOld == bEnable)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;
			}

			int iRet = SystemProfile::GetInstance()->SetNotifySwitch(bEnable);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			MQ_Upload_Notify_Enable(bEnable);

			WriteDbLog(tReqHead, OP_LOG_SYS, "提醒%s",bEnable?"开启":"关闭");
		}
	}
}
