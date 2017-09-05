#include "isapi_json.h"

void WriteJsonResult(int code, const char * desc, JsonElement & jRoot)
{
	JsonElement jResult;
	jResult[RPC_RESULT_CODE] = code;
	jResult[RPC_RESULT_DESC] = desc ? desc : "";
	jRoot[RPC_RESULT] = jResult;
}

void WriteJsonResultSuccess(JsonElement & jRoot)
{
	WriteJsonResult(0, "success", jRoot);
}

void WriteJsonResultError(const ErrorCode& errCode, JsonElement& jRoot)
{
	const char * desc = NULL;
	switch(errCode)
	{
	case ISAPI_SUCCESS:
		desc = "success";
		break;
	case ISAPI_FAILED_PARSING:
		desc = "json parse error";
		break;
	case ISAPI_FAILED_PARAMS:
		desc = "invalid params";
		break;
	case ISAPI_FAILED_PERMISSION:
		desc = "no permission";
		break;
	case ISAPI_FAILED_OPERATE:
		desc = "operation failed";
		break;
	case ISAPI_FAILED_EXISTING:
		desc = "existence";
		break;
	case ISAPI_FAILED_NOCFG:
		desc = "no config";
		break;
	case ISAPI_FAILED_ALG_RUNNING:
		desc = "analysis is running";
		break;
	case ISAPI_FAILED_ALG_STOP:
		desc = "analysis is not running";
		break;
	case ISAPI_FAILED_NO_DATA:
		desc = "no data";
		break;
	case ISAPI_FAILED_FILE:
		desc = "file error";
		break;
	default:
		desc = "unknown error";
		break;
	}
	
	WriteJsonResult(errCode, desc, jRoot);
}

bool JsonArrayAsObjAt(const JsonElement& jRoot, int i, JsonElement & jObj, JsonElement & jError)
{
	try
	{
		if(jRoot.isArray())
		{
			JsonElement jFind = jRoot[i];
			if(jFind.isObject())
			{
				jObj = jFind;
				return true;
			}
		}
	}
	catch (...)
	{
		;
	}

	WriteJsonResultError(ISAPI_FAILED_PARAMS, jError);
	
	return false;
}

bool JsonArrayAsIntAt(const JsonElement& jRoot, int i, int & iValue, JsonElement & jError)
{
	try
	{
		if(jRoot.isArray())
		{
			JsonElement jFind = jRoot[i];
			if(jFind.isInt())
			{
				iValue = jFind.asInt();
				return true;
			}
		}
	}
	catch (...)
	{
		;
	}

	WriteJsonResultError(ISAPI_FAILED_PARAMS, jError);
	
	return false;
}

bool JsonChildAsObj(const JsonElement& jRoot, const char * pszName, JsonElement & jObj, JsonElement & jError)
{
	if(pszName == NULL || strlen(pszName) < 1)
	{
		return false;
	}
	
	try
	{
		if(jRoot.isObject())
		{
			JsonElement jFind = jRoot[pszName];
			if(jFind.isObject())
			{
				jObj = jFind;
				return true;
			}
		}
	}
	catch (...)
	{
		;
	}

	string szMsg = "lack essential field [";
	szMsg += pszName;
	szMsg += "], type:Object.";
	WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg.c_str(), jError);
	
	return false;
}

bool JsonChildAsArray(const JsonElement& jRoot, const char * pszName, JsonElement& jArray, JsonElement& jError)
{
	if(pszName == NULL || strlen(pszName) < 1)
	{
		return false;
	}
	
	try
	{
		if(jRoot.isObject())
		{
			JsonElement jFind = jRoot[pszName];
			if(jFind.isArray())
			{
				jArray = jFind;
				return true;
			}
		}
	}
	catch (...)
	{
		;
	}

	string szMsg = "lack essential field [";
	szMsg += pszName;
	szMsg += "], type:Array.";
	WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg.c_str(), jError);
	
	return false;
	
}

bool JsonChildAsBool(const JsonElement& jRoot, const char * pszName, bool & bValue, JsonElement& jError)
{
	if(pszName == NULL || strlen(pszName) < 1)
	{
		return false;
	}
	
	try
	{
		if(jRoot.isObject())
		{
			JsonElement jFind = jRoot[pszName];
			if(jFind.isBool())
			{
				bValue = jFind.asBool();
				return true;
			}
		}
	}
	catch (...)
	{
		;
	}

	string szMsg = "lack essential field [";
	szMsg += pszName;
	szMsg += "], type:Boolean.";
	WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg.c_str(), jError);
	
	return false;
}

bool JsonChildAsDouble(const JsonElement& jRoot, const char * pszName, double & dbValue, JsonElement& jError)
{
	if(pszName == NULL || strlen(pszName) < 1)
	{
		return false;
	}
	
	try
	{
		if(jRoot.isObject())
		{
			JsonElement jFind = jRoot[pszName];
			if(jFind.isDouble())
			{
				dbValue = jFind.asDouble();
				return true;
			}
		}
	}
	catch (...)
	{
		;
	}

	string szMsg = "lack essential field [";
	szMsg += pszName;
	szMsg += "], type:Number.";
	WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg.c_str(), jError);
	
	return false;
}

bool JsonChildAsFloat(const JsonElement& jRoot, const char * pszName, float & fValue, JsonElement& jError)
{
	double dbValue;
	bool bRet = JsonChildAsDouble(jRoot, pszName, dbValue, jError);
	if(bRet)
	{
		fValue = (float)dbValue;
	}
	return bRet;
}

bool JsonChildAsInt(const JsonElement& jRoot, const char * pszName, int & iValue, JsonElement& jError)
{
	if(pszName == NULL || strlen(pszName) < 1)
	{
		return false;
	}
	
	try
	{
		if(jRoot.isObject())
		{
			JsonElement jFind = jRoot[pszName];
			if(jFind.isInt())
			{
				iValue = jFind.asInt();
				return true;
			}
		}
	}
	catch (...)
	{
		;
	}

	string szMsg = "lack essential field [";
	szMsg += pszName;
	szMsg += "], type:Number.";
	WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg.c_str(), jError);
	
	return false;
}

bool JsonChildAsInt8(const JsonElement& jRoot, const char * pszName, char & uValue, JsonElement& jError)
{
	int i32Value;
	bool bRet = JsonChildAsInt(jRoot, pszName, i32Value, jError);
	if(bRet)
	{
		uValue = (char)i32Value;
	}
	return bRet;
}

bool JsonChildAsInt16(const JsonElement& jRoot, const char * pszName, short & uValue, JsonElement& jError)
{
	int i32Value;
	bool bRet = JsonChildAsInt(jRoot, pszName, i32Value, jError);
	if(bRet)
	{
		uValue = (short)i32Value;
	}
	return bRet;
}

bool JsonChildAsUInt(const JsonElement& jRoot, const char * pszName, unsigned int & uValue, JsonElement& jError)
{
	if(pszName == NULL || strlen(pszName) < 1)
	{
		return false;
	}
	
	try
	{
		if(jRoot.isObject())
		{
			JsonElement jFind = jRoot[pszName];
			if(jFind.isUInt())
			{
				uValue = jFind.asUInt();
				return true;
			}
		}
	}
	catch (...)
	{
		;
	}

	string szMsg = "lack essential field [";
	szMsg += pszName;
	szMsg += "], type:Number.";
	WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg.c_str(), jError);
	
	return false;
}

bool JsonChildAsUInt8(const JsonElement& jRoot, const char * pszName, unsigned char & uValue, JsonElement& jError)
{
	unsigned int u32Value;
	bool bRet = JsonChildAsUInt(jRoot, pszName, u32Value, jError);
	if(bRet)
	{
		uValue = (unsigned char)u32Value;
	}
	return bRet;
}

bool JsonChildAsUInt16(const JsonElement& jRoot, const char * pszName, unsigned short & uValue, JsonElement& jError)
{
	unsigned int u32Value;
	bool bRet = JsonChildAsUInt(jRoot, pszName, u32Value, jError);
	if(bRet)
	{
		uValue = (unsigned short)u32Value;
	}
	return bRet;
}

bool JsonChildAsBuffer(const JsonElement& jRoot, const char * pszName, char * pszValue, int len, JsonElement& jError)
{
	if(pszName == NULL || strlen(pszName) < 1 || pszValue == NULL)
	{
		return false;
	}
	
	try
	{
		if(jRoot.isObject())
		{
			JsonElement jFind = jRoot[pszName];
			if(jFind.isString())
			{
				std::string str = jFind.asString();

				if(str.length() > len-1)
				{
					char szMsg[128] = {0};
					sprintf(szMsg,"the field [%s], type:String [len < %d]",pszName,len);
					WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
					return false;
				}

				memset(pszValue,0,len);
				strcpy(pszValue, str.c_str());
				return true;
			}
		}
	}
	catch (...)
	{
		;
	}

	string szMsg = "lack essential field [";
	szMsg += pszName;
	szMsg += "], type:String.";
	WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg.c_str(), jError);
	
	return false;
}

bool JsonChildAsString(const JsonElement& jRoot, const char * pszName, string &szValue, JsonElement& jError)
{
	if(pszName == NULL || strlen(pszName) < 1 )
	{
		return false;
	}
	
	try
	{
		if(jRoot.isObject())
		{
			JsonElement jFind = jRoot[pszName];
			if(jFind.isString())
			{
				szValue = jFind.asString();
				return true;
			}
		}
	}
	catch (...)
	{
		;
	}

	string szMsg = "lack essential field [";
	szMsg += pszName;
	szMsg += "], type:String.";
	WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg.c_str(), jError);
	
	return false;
}

bool JsonChildAsIntWithBound(const JsonElement& jRoot, const char * pszName, int & nValue, int nLower, int nUpper, JsonElement& jError)
{
	if(JsonChildAsInt(jRoot,pszName,nValue,jError))
	{
		if(nValue < nLower || nValue > nUpper)
		{
			char szMsg[128] = {0};
			sprintf(szMsg,"the field [%s] type:Number [%d - %d]",pszName,nLower,nUpper);
			WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
			return false;
		}
		return true;
	}
	
	return false;	
}

bool JsonChildAsInt8WithBound(const JsonElement& jRoot, const char * pszName, char & nValue, char nLower, char nUpper, JsonElement& jError)
{
	if(JsonChildAsInt8(jRoot,pszName,nValue,jError))
	{
		if(nValue < nLower || nValue > nUpper)
		{
			char szMsg[128] = {0};
			sprintf(szMsg,"the field [%s] type:Number [%d - %d]",pszName,nLower,nUpper);
			WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
			return false;
		}
		return true;
	}
	
	return false;	
}

bool JsonChildAsInt16WithBound(const JsonElement& jRoot, const char * pszName, short & nValue, short nLower, short nUpper, JsonElement& jError)
{
	if(JsonChildAsInt16(jRoot,pszName,nValue,jError))
	{
		if(nValue < nLower || nValue > nUpper)
		{
			char szMsg[128] = {0};
			sprintf(szMsg,"the field [%s] type:Number [%d - %d]",pszName,nLower,nUpper);
			WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
			return false;
		}
		return true;
	}
	
	return false;	
}

bool JsonChildAsUIntWithBound(const JsonElement& jRoot, const char * pszName, unsigned int & uValue, unsigned int uLower, unsigned int uUpper, JsonElement& jError)
{
	if(JsonChildAsUInt(jRoot,pszName,uValue,jError))
	{
		if(uValue < uLower || uValue > uUpper)
		{
			char szMsg[128] = {0};
			sprintf(szMsg,"the field [%s] type:Number [%d - %d]",pszName,uLower,uUpper);
			WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
			return false;
		}
		return true;
	}
	return false;	
}

bool JsonChildAsUInt8WithBound(const JsonElement& jRoot, const char * pszName, unsigned char & uValue, unsigned char uLower, unsigned char uUpper, JsonElement& jError)
{
	if(JsonChildAsUInt8(jRoot,pszName,uValue,jError))
	{
		if(uValue < uLower || uValue > uUpper)
		{
			char szMsg[128] = {0};
			sprintf(szMsg,"the field [%s] type:Number [%d - %d]",pszName,uLower,uUpper);
			WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
			return false;
		}
		return true;
	}
	return false;	
}

bool JsonChildAsUInt16WithBound(const JsonElement& jRoot, const char * pszName, unsigned short & uValue, unsigned short uLower, unsigned short uUpper, JsonElement& jError)
{
	if(JsonChildAsUInt16(jRoot,pszName,uValue,jError))
	{
		if(uValue < uLower || uValue > uUpper)
		{
			char szMsg[128] = {0};
			sprintf(szMsg,"the field [%s] type:Number [%d - %d]",pszName,uLower,uUpper);
			WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
			return false;
		}
		return true;
	}
	return false;	
}

bool JsonChildAsFloatWithBound(const JsonElement& jRoot, const char * pszName, float & fValue, float fLower, float fUpper, JsonElement& jError)
{
	if(JsonChildAsFloat(jRoot,pszName,fValue,jError))
	{
		if(fValue < fLower || fValue > fUpper)
		{
			char szMsg[128] = {0};
			sprintf(szMsg,"the field  [%s] type:Number[%g - %g]",pszName,fLower,fUpper);
			WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
			return false;
		}
		return true;
	}
	return false;
}

bool JsonChildAsDoubleWithBound( const JsonElement& jRoot, const char * pszName, double & fValue, double fLower, double fUpper, JsonElement& jError )
{
	if(JsonChildAsDouble(jRoot,pszName,fValue,jError))
	{
		if(fValue < fLower || fValue > fUpper)
		{
			char szMsg[128] = {0};
			sprintf(szMsg,"the field  [%s] type:Number[%g - %g]",pszName,fLower,fUpper);
			WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
			return false;
		}
		return true;
	}
	return false;
}

bool JsonChildAsArrayWithBound(const JsonElement& jRoot, const char * pszName,JsonElement& arrayValue,int nMinCnt,int nMaxCnt,JsonElement& jError)
{
	if(JsonChildAsArray(jRoot,pszName,arrayValue,jError))
	{
		if(arrayValue.size() < nMinCnt || arrayValue.size() > nMaxCnt)
		{
			char szMsg[128] = {0};
			sprintf(szMsg,"the field [%s] type:Array [%d - %d]",pszName, nMinCnt,nMaxCnt);
			WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
			return false;
		}
		return true;
	}
	return false;
}





