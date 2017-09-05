/***************************************************************
模块 ： 字符转换相关                        
文件 ： oal_unicode.cpp
作者 ： Ripple Lee(libozjw@163.com)
版本 ： V1.0
日期 ： 2017-01-18
*****************************************************************/
#include "include/oal_unicode.h"

#ifndef  WIN32

#define LIBICONV_PLUG
#include <iconv.h>

OAL_API int GB2312_To_UTF8(char *inbuf, int inlen, char *outbuf, int outlen)
{
	int iRet = SUCCESS;
	char **pin = &inbuf;
	char **pout = &outbuf;
	iconv_t cd;
	size_t iconsize;
	
	if(inbuf == NULL || outbuf == NULL)
	{
		return FAILURE;
	}

	cd = iconv_open("utf-8", "GBK");
	if(cd == (iconv_t)-1) 
	{
		perror("iconv_open GB2312->UTF-8 failed");
		strcpy(outbuf, inbuf);
		return FAILURE;
	}

	memset(outbuf, 0, outlen);
	iconsize = iconv(cd, pin, (size_t*)&inlen, pout, (size_t*)&outlen);
	if(iconsize == -1)
	{
		perror("iconv failed:\n");
		strcpy(outbuf, inbuf);
		iRet = FAILURE;
	}
	
	iconv_close(cd);

	return iRet;
}

OAL_API int UTF8_To_GB2312(char *inbuf, int inlen, char *outbuf, int outlen)
{
	int iRet = SUCCESS;
	char **pin = &inbuf;
	char **pout = &outbuf;
	iconv_t cd;
	size_t iconsize;
	
	if(inbuf == NULL || outbuf == NULL)
	{
		return FAILURE;
	}

	cd = iconv_open("GBK","utf-8");
	if(cd == (iconv_t)-1) 
	{
		perror("iconv_open UTF-8->GB2312 failed");
		strcpy(outbuf, inbuf);
		return FAILURE;
	}


	memset(outbuf, 0, outlen);
	iconsize = iconv(cd, pin, (size_t*)&inlen, pout, (size_t*)&outlen);
	if(iconsize == -1)
	{
		perror("iconv failed:\n");
		strcpy(outbuf, inbuf);
		iRet = FAILURE;
	}
	
	iconv_close(cd);

	return iRet;
}

#else
#include <wtypes.h>
OAL_API int GB2312_To_UTF8(char *inbuf, int inlen, char *outbuf, int outlen)
{
	if(inbuf == NULL || outbuf == NULL)
	{
		return FAILURE;
	}

	//获得临时变量的大小
	int iLen = MultiByteToWideChar(CP_ACP, 0, inbuf, -1, NULL, 0);
	WCHAR *strSrc = new WCHAR[iLen+1];
	iLen = MultiByteToWideChar(CP_ACP, 0, inbuf, -1, strSrc, iLen);
	iLen = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, outbuf, outlen, NULL, NULL);
	delete []strSrc;

	return iLen>0?SUCCESS:FAILURE;
}

OAL_API int UTF8_To_GB2312(char *inbuf, int inlen, char *outbuf, int outlen)
{
	if(inbuf == NULL || outbuf == NULL)
	{
		return FAILURE;
	}

	//获得临时变量的大小
	int iLen = MultiByteToWideChar(CP_UTF8, 0, inbuf, -1, NULL, 0);
	WCHAR *strSrc = new WCHAR[iLen+1];
	iLen = MultiByteToWideChar(CP_UTF8, 0, inbuf, -1, strSrc, iLen);
	iLen = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, outbuf, outlen, NULL, NULL);
	delete []strSrc;

	return iLen>0?SUCCESS:FAILURE;
}
#endif //  WIN32
