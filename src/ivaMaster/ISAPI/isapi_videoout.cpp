#include "isapi_videoout.h"
#include "capacity.h"
#include "oal_unicode.h"
#include "oal_time.h"
#include "../Profile/SystemProfile.h"
#include "../Manager/LogManager.h"
#include "mq_videoout.h"


namespace ISAPI
{
	namespace VideoOut
	{
		void Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jVOCap;
			jVOCap["supportRtsp"] = true;
			jVOCap["supportRtmp"] = true;
			jRespRoot["vediooutCap"] = jVOCap;
			WriteJsonResultSuccess(jRespRoot);
		}

		void Strategy_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			VoStrategy tStrategy = {0};
			SystemProfile::GetInstance()->GetVideoOutStrategy(&tStrategy);

			JsonElement jStrategy;
			jStrategy["protocol"] = tStrategy.iProtocol;

			jRespRoot["videooutStrategy"] = jStrategy;

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

			VoStrategy tStrategy = {0};

			JsonElement jStrategy;
			if(!JsonChildAsObj(jReqRoot, "videooutStrategy", jStrategy, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jStrategy, "protocol", tStrategy.iProtocol, VO_PROTOCOL_NONE, VO_PROTOCOL_RTMP, jRespRoot))return;

			VoStrategy tOldCfg  = {0};
			SystemProfile::GetInstance()->GetVideoOutStrategy(&tOldCfg);
			if (memcmp(&tOldCfg, &tStrategy, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetVideoOutStrategy(&tStrategy);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_OUT, "设置视频输出策略");

			MQ_VideoOut_SetStrategy(&tStrategy);
		}

		void RtmpServer_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			RtmpServer tSvr = {0};
			SystemProfile::GetInstance()->GetVideoOutRtmpSvr(iChnID-1, &tSvr);

			JsonElement jSvr;
			jSvr["addr"] = tSvr.szUrl;
			jRespRoot["rtmpserver"] = jSvr;
			WriteJsonResultSuccess(jRespRoot);
		}

		void RtmpServer_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			RtmpServer tSvr = {0};
			JsonElement jSvr;
			if(!JsonChildAsObj(jReqRoot, "rtmpserver", jSvr, jRespRoot))return;
			if(!JsonChildAsBuffer(jSvr, "addr", tSvr.szUrl, sizeof(tSvr.szUrl), jRespRoot))return;

			RtmpServer tOldCfg = {0};
			SystemProfile::GetInstance()->GetVideoOutRtmpSvr(iChnID-1, &tOldCfg);
			if (memcmp(&tOldCfg, &tSvr, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetVideoOutRtmpSvr(iChnID-1, &tSvr);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteDbLog(tReqHead, OP_LOG_OUT, "设置通道%d之RTMP服务器", iChnID);

			WriteJsonResultSuccess(jRespRoot);

			MQ_VideoOut_SetRtmpServer(iChnID-1, &tSvr);
		}

		void Switch_GET( const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot )
		{
			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			bool bEnable = false;
			SystemProfile::GetInstance()->GetVideoOutSwitch(iChnID-1, bEnable);

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

			int iChnID = -1;
			if (!QueryGetNumberValueWithBound(tReqHead.szQuery, "chnid", iChnID, 1, MAX_CHANNEL_NUM,jRespRoot)) return;

			bool bEnable = false;
			if (!JsonChildAsBool(jReqRoot, "switch", bEnable, jRespRoot))return;

			bool bEnableOld;
			SystemProfile::GetInstance()->GetVideoOutSwitch(iChnID-1, bEnableOld);
			if (bEnableOld == bEnable)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;
			}

			int iRet = SystemProfile::GetInstance()->SetVideoOutSwitch(iChnID-1, bEnable);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_REC, "%s通道%d录像",bEnable?"开启":"关闭",iChnID);

			MQ_VideoOut_SetEnable(iChnID-1, bEnable);
		}
	}
}
