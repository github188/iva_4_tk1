#include "safe_json.h"

bool SafeJsonArrayAsObjAt(const JsonElement& jRoot, int i, JsonElement & jObj)
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

	return false;
}

bool SafeJsonArrayAsIntAt(const JsonElement& jRoot, int i, int & iValue)
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
	
	return false;
}

bool SafeJsonChildAsObj(const JsonElement& jRoot, const char * pszName, JsonElement & jObj)
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
	return false;
}

bool SafeJsonChildAsArray(const JsonElement& jRoot, const char * pszName, JsonElement& jArray)
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

	return false;
}

bool SafeJsonChildAsBool(const JsonElement& jRoot, const char * pszName, bool & bValue)
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
	
	return false;
}

bool SafeJsonChildAsDouble(const JsonElement& jRoot, const char * pszName, double & dbValue)
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
	
	return false;
}

bool SafeJsonChildAsFloat(const JsonElement& jRoot, const char * pszName, float & fValue)
{
	double dbValue;
	bool bRet = SafeJsonChildAsDouble(jRoot, pszName, dbValue);
	if(bRet)
	{
		fValue = (float)dbValue;
	}
	return bRet;
}

bool SafeJsonChildAsInt(const JsonElement& jRoot, const char * pszName, int & iValue)
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
	
	return false;
}

bool SafeJsonChildAsInt8(const JsonElement& jRoot, const char * pszName, char & uValue)
{
	int i32Value;
	bool bRet = SafeJsonChildAsInt(jRoot, pszName, i32Value);
	if(bRet)
	{
		uValue = (char)i32Value;
	}
	return bRet;
}

bool SafeJsonChildAsInt16(const JsonElement& jRoot, const char * pszName, short & uValue)
{
	int i32Value;
	bool bRet = SafeJsonChildAsInt(jRoot, pszName, i32Value);
	if(bRet)
	{
		uValue = (short)i32Value;
	}
	return bRet;
}

bool SafeJsonChildAsUInt(const JsonElement& jRoot, const char * pszName, unsigned int & uValue)
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

	return false;
}

bool SafeJsonChildAsUInt8(const JsonElement& jRoot, const char * pszName, unsigned char & uValue)
{
	unsigned int u32Value;
	bool bRet = SafeJsonChildAsUInt(jRoot, pszName, u32Value);
	if(bRet)
	{
		uValue = (unsigned char)u32Value;
	}
	return bRet;
}

bool SafeJsonChildAsUInt16(const JsonElement& jRoot, const char * pszName, unsigned short & uValue)
{
	unsigned int u32Value;
	bool bRet = SafeJsonChildAsUInt(jRoot, pszName, u32Value);
	if(bRet)
	{
		uValue = (unsigned short)u32Value;
	}
	return bRet;
}

bool SafeJsonChildAsBuffer(const JsonElement& jRoot, const char * pszName, char * pszValue, int len)
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

	return false;
}

bool SafeJsonChildAsString(const JsonElement& jRoot, const char * pszName, string &szValue)
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

	return false;
}

bool SafeJsonChildAsIntWithBound(const JsonElement& jRoot, const char * pszName, int & nValue, int nLower, int nUpper)
{
	if(SafeJsonChildAsInt(jRoot,pszName,nValue))
	{
		if(nValue < nLower || nValue > nUpper)
		{
			return false;
		}
		return true;
	}
	
	return false;	
}

bool SafeJsonChildAsInt8WithBound(const JsonElement& jRoot, const char * pszName, char & nValue, char nLower, char nUpper)
{
	if(SafeJsonChildAsInt8(jRoot,pszName,nValue))
	{
		if(nValue < nLower || nValue > nUpper)
		{
			return false;
		}
		return true;
	}
	
	return false;	
}

bool SafeJsonChildAsInt16WithBound(const JsonElement& jRoot, const char * pszName, short & nValue, short nLower, short nUpper)
{
	if(SafeJsonChildAsInt16(jRoot,pszName,nValue))
	{
		if(nValue < nLower || nValue > nUpper)
		{
			return false;
		}
		return true;
	}
	
	return false;	
}

bool SafeJsonChildAsUIntWithBound(const JsonElement& jRoot, const char * pszName, unsigned int & uValue, unsigned int uLower, unsigned int uUpper)
{
	if(SafeJsonChildAsUInt(jRoot,pszName,uValue))
	{
		if(uValue < uLower || uValue > uUpper)
		{
			return false;
		}
		return true;
	}
	return false;	
}

bool SafeJsonChildAsUInt8WithBound(const JsonElement& jRoot, const char * pszName, unsigned char & uValue, unsigned char uLower, unsigned char uUpper)
{
	if(SafeJsonChildAsUInt8(jRoot,pszName,uValue))
	{
		if(uValue < uLower || uValue > uUpper)
		{
			return false;
		}
		return true;
	}
	return false;	
}

bool SafeJsonChildAsUInt16WithBound(const JsonElement& jRoot, const char * pszName, unsigned short & uValue, unsigned short uLower, unsigned short uUpper)
{
	if(SafeJsonChildAsUInt16(jRoot,pszName,uValue))
	{
		if(uValue < uLower || uValue > uUpper)
		{
			return false;
		}
		return true;
	}
	return false;	
}

bool SafeJsonChildAsFloatWithBound(const JsonElement& jRoot, const char * pszName, float & fValue, float fLower, float fUpper)
{
	if(SafeJsonChildAsFloat(jRoot,pszName,fValue))
	{
		if(fValue < fLower || fValue > fUpper)
		{
			return false;
		}
		return true;
	}
	return false;
}

bool SafeJsonChildAsDoubleWithBound( const JsonElement& jRoot, const char * pszName, double & fValue, double fLower, double fUpper )
{
	if(SafeJsonChildAsDouble(jRoot,pszName,fValue))
	{
		if(fValue < fLower || fValue > fUpper)
		{
			return false;
		}
		return true;
	}
	return false;
}

bool SafeJsonChildAsArrayWithBound(const JsonElement& jRoot, const char * pszName,JsonElement& arrayValue,int nMinCnt,int nMaxCnt)
{
	if(SafeJsonChildAsArray(jRoot,pszName,arrayValue))
	{
		if(arrayValue.size() < nMinCnt || arrayValue.size() > nMaxCnt)
		{
			return false;
		}
		return true;
	}
	return false;
}

bool SafeJsonChildAsResult( JsonElement & jRoot, int &code, string &desc )
{
	JsonElement jRes;
	if (!SafeJsonChildAsObj(jRoot, "result", jRes)) return false;
	if (!SafeJsonChildAsInt(jRes, "code", code)) return false;
	SafeJsonChildAsString(jRes, "description", desc);
	return true;
}




