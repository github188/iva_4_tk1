/***************************************************************
模块 ： 正则表达式相关                       
文件 ： oal_regexp.cpp
作者 ： Ripple Lee(libozjw@163.com)
版本 ： V1.0
日期 ： 2017-01-18
*****************************************************************/
#include "include/oal_regexp.h"

#ifdef WIN32
#include <regex>
#else
#include <regex.h>
#endif
#include <errno.h>
#include <sstream>
using namespace std;


static int JudgeStringByRegex(const char * pszPattern, const char *pszContent)
{
	if(pszPattern == NULL || pszContent == NULL)
	{
		return FAILURE;
	}
#ifdef WIN32
	if (regex_match( pszContent, regex(pszPattern)))  
	{
		return SUCCESS;
	}
#else

	regex_t reg;
	regmatch_t match[5];

	int retval = regcomp(&reg, pszPattern, REG_EXTENDED | REG_NEWLINE);
	if (0==retval)
	{
		retval = regexec(&reg,pszContent,5,match,0);
	}

	regfree(&reg);
	if(0 == retval)
	{
		return SUCCESS;
	}
#endif

	return FAILURE;
}

OAL_API int IsIPv4Addr(const char* szAddr)
{
	const char * IPv4_PATTERN =  "^(([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5]).){3}([0-9]|[1-9][0-9]|1[0-9]{1,2}|2[0-4][0-9]|25[0-5])$";
	return (JudgeStringByRegex(IPv4_PATTERN,szAddr)==SUCCESS)?1:0;
}
