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

#ifndef SAFE_JSON_H
#define SAFE_JSON_H

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

bool SafeJsonArrayAsObjAt(const JsonElement& jRoot, int i, JsonElement & jObj);
bool SafeJsonArrayAsIntAt(const JsonElement& jRoot, int i, int & iValue);
bool SafeJsonChildAsObj(const JsonElement& jRoot, const char * pszName, JsonElement & jObj);
bool SafeJsonChildAsArray(const JsonElement& jRoot, const char * pszName, JsonElement& jArray);
bool SafeJsonChildAsBool(const JsonElement& jRoot, const char * pszName, bool & bValue);
bool SafeJsonChildAsDouble(const JsonElement& jRoot, const char * pszName, double & dbValue);
bool SafeJsonChildAsFloat(const JsonElement& jRoot, const char * pszName, float & fValue);
bool SafeJsonChildAsInt(const JsonElement& jRoot, const char * pszName, int & iValue);
bool SafeJsonChildAsInt8(const JsonElement& jRoot, const char * pszName, char & uValue);
bool SafeJsonChildAsInt16(const JsonElement& jRoot, const char * pszName, short & uValue);
bool SafeJsonChildAsUInt(const JsonElement& jRoot, const char * pszName, unsigned int & uValue);
bool SafeJsonChildAsUInt8(const JsonElement& jRoot, const char * pszName, unsigned char & uValue);
bool SafeJsonChildAsUInt16(const JsonElement& jRoot, const char * pszName, unsigned short & uValue);
bool SafeJsonChildAsBuffer(const JsonElement& jRoot, const char * pszName, char * pszValue, int len);
bool SafeJsonChildAsString(const JsonElement& jRoot, const char * pszName, string &szValue);
bool SafeJsonChildAsIntWithBound(const JsonElement& jRoot, const char * pszName, int & nValue, int nLower, int nUpper);
bool SafeJsonChildAsInt8WithBound(const JsonElement& jRoot, const char * pszName, char & nValue, char nLower, char nUpper);
bool SafeJsonChildAsInt16WithBound(const JsonElement& jRoot, const char * pszName, short & nValue, short nLower, short nUpper);
bool SafeJsonChildAsUIntWithBound(const JsonElement& jRoot, const char * pszName, unsigned int & uValue, unsigned int uLower, unsigned int uUpper);
bool SafeJsonChildAsUInt8WithBound(const JsonElement& jRoot, const char * pszName, unsigned char & uValue, unsigned char uLower, unsigned char uUpper);
bool SafeJsonChildAsUInt16WithBound(const JsonElement& jRoot, const char * pszName, unsigned short & uValue, unsigned short uLower, unsigned short uUpper);
bool SafeJsonChildAsFloatWithBound(const JsonElement& jRoot, const char * pszName, float & fValue, float fLower, float fUpper);
bool SafeJsonChildAsDoubleWithBound(const JsonElement& jRoot, const char * pszName, double & fValue, double fLower, double fUpper);
bool SafeJsonChildAsArrayWithBound(const JsonElement& jRoot, const char * pszName,JsonElement& arrayValue,int nMinCnt,int nMaxCnt);

bool SafeJsonChildAsResult(JsonElement & jRoot, int &code, string &desc);


#endif /* SAFE_JSON_H */

