#include "isapi_handler.h"
#ifndef __ISAPI_LOG_H__
#define __ISAPI_LOG_H__

namespace ISAPI
{
	namespace Log
	{
		void Strategy_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
		void Strategy_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
		void Query_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
		void Export_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
		void Operate_DELETE(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot);
	}
}
#endif


