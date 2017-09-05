#include "isapi_handler.h"
#ifndef __ISAPI_VIDEOOUT_H__
#define __ISAPI_VIDEOOUT_H__

namespace ISAPI
{
	namespace VideoOut
	{
		void Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
		void Strategy_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
		void Strategy_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
		void RtmpServer_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
		void RtmpServer_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
		void Switch_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
		void Switch_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
	}
}
#endif
