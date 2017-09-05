/*
 *  JsonRpc-Cpp - JSON-RPC implementation.
 *  Copyright (C) 2008-2011 Sebastien Vincent <sebastien.vincent@cppextrem.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file jsonrpc_common.h
 * \brief Common functions and enumerations for JsonRpc-Cpp.
 * \author Sebastien Vincent
 */

#ifndef JSONRPC_COMMON_H
#define JSONRPC_COMMON_H

#ifndef _WIN32

#include <stdint.h>

#else

#ifndef _MSC_VER
#include <stdint.h>
#endif

#endif
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <json/json.h>
using namespace std;

typedef Json::Value JsonElement;

// һЩ�̶��Ĳ�������
#define RPC_RESULT			"result"
#define RPC_RESULT_CODE   	"code"
#define RPC_RESULT_DESC   	"description"

enum ErrorCode
{
	ISAPI_SUCCESS = 0,    //�ɹ�
	ISAPI_FAILED_PARSING  		= 1000, // JSON��������
    ISAPI_FAILED_PARAMS 		= 1001, // ��������
	ISAPI_FAILED_PERMISSION		= 1002, // �޲���Ȩ��
	ISAPI_FAILED_OPERATE		= 1003, // ����ʧ��     
	ISAPI_FAILED_EXISTING 		= 1004, // �Ѿ�����
	ISAPI_FAILED_NOCFG	     	= 1005,	// û����Ӧ������
	ISAPI_FAILED_ALG_RUNNING  	= 1006,	// �㷨��������
	ISAPI_FAILED_ALG_STOP     	= 1007,	// �㷨û������
	ISAPI_FAILED_NO_DATA     	= 1008,	// û�в�ѯ������
	ISAPI_FAILED_FILE	     	= 1009,	// �ļ�����
	ISAPI_FAILED_UNKNOWN		= 9999  // δ֪����
};

void WriteJsonResult(int code, const char * desc, JsonElement & jRoot);
void WriteJsonResultSuccess(JsonElement & jRoot);
void WriteJsonResultError(const ErrorCode& errCode, JsonElement& jRoot);


bool JsonArrayAsObjAt(const JsonElement& jRoot, int i, JsonElement & jObj, JsonElement & jError);
bool JsonArrayAsIntAt(const JsonElement& jRoot, int i, int & iValue, JsonElement & jError);
bool JsonChildAsObj(const JsonElement& jRoot, const char * pszName, JsonElement & jObj, JsonElement & jError);
bool JsonChildAsArray(const JsonElement& jRoot, const char * pszName, JsonElement& jArray, JsonElement& jError);
bool JsonChildAsBool(const JsonElement& jRoot, const char * pszName, bool & bValue, JsonElement& jError);
bool JsonChildAsDouble(const JsonElement& jRoot, const char * pszName, double & dbValue, JsonElement& jError);
bool JsonChildAsFloat(const JsonElement& jRoot, const char * pszName, float & fValue, JsonElement& jError);
bool JsonChildAsInt(const JsonElement& jRoot, const char * pszName, int & iValue, JsonElement& jError);
bool JsonChildAsInt8(const JsonElement& jRoot, const char * pszName, char & uValue, JsonElement& jError);
bool JsonChildAsInt16(const JsonElement& jRoot, const char * pszName, short & uValue, JsonElement& jError);
bool JsonChildAsUInt(const JsonElement& jRoot, const char * pszName, unsigned int & uValue, JsonElement& jError);
bool JsonChildAsUInt8(const JsonElement& jRoot, const char * pszName, unsigned char & uValue, JsonElement& jError);
bool JsonChildAsUInt16(const JsonElement& jRoot, const char * pszName, unsigned short & uValue, JsonElement& jError);
bool JsonChildAsBuffer(const JsonElement& jRoot, const char * pszName, char * pszValue, int len, JsonElement& jError);
bool JsonChildAsString(const JsonElement& jRoot, const char * pszName, string &szValue, JsonElement& jError);
bool JsonChildAsIntWithBound(const JsonElement& jRoot, const char * pszName, int & nValue, int nLower, int nUpper, JsonElement& jError);
bool JsonChildAsInt8WithBound(const JsonElement& jRoot, const char * pszName, char & nValue, char nLower, char nUpper, JsonElement& jError);
bool JsonChildAsInt16WithBound(const JsonElement& jRoot, const char * pszName, short & nValue, short nLower, short nUpper, JsonElement& jError);
bool JsonChildAsUIntWithBound(const JsonElement& jRoot, const char * pszName, unsigned int & uValue, unsigned int uLower, unsigned int uUpper, JsonElement& jError);
bool JsonChildAsUInt8WithBound(const JsonElement& jRoot, const char * pszName, unsigned char & uValue, unsigned char uLower, unsigned char uUpper, JsonElement& jError);
bool JsonChildAsUInt16WithBound(const JsonElement& jRoot, const char * pszName, unsigned short & uValue, unsigned short uLower, unsigned short uUpper, JsonElement& jError);
bool JsonChildAsFloatWithBound(const JsonElement& jRoot, const char * pszName, float & fValue, float fLower, float fUpper, JsonElement& jError);
bool JsonChildAsDoubleWithBound(const JsonElement& jRoot, const char * pszName, double & fValue, double fLower, double fUpper, JsonElement& jError);
bool JsonChildAsArrayWithBound(const JsonElement& jRoot, const char * pszName,JsonElement& arrayValue,int nMinCnt,int nMaxCnt,JsonElement& jError);


#endif /* JSONRPC_COMMON_H */

