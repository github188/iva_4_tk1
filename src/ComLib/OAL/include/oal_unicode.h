/***************************************************************
模块 ： 字符转换相关                        
文件 ： oal_unicode.h
作者 ： Ripple Lee(libozjw@163.com)
版本 ： V1.0
日期 ： 2017-01-18
*****************************************************************/
#ifndef __OAL_UNICODE_H__
#define __OAL_UNICODE_H__
#include "oal_typedef.h"
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
* 函数名称: GB2312_To_UTF8
* 函数功能: 将字符串由GB2312转换为UTF8
* 输入参数: inbuf-输入字符串 inlen-输入字符串长度 outlen-输出字符串的长度
* 输出参数: outbuf，输出字符串
* 函数返回: 成功SUCCESS 失败FAILURE
******************************************************************************/
OAL_API int GB2312_To_UTF8(char *inbuf, int inlen, char *outbuf, int outlen);

/*****************************************************************************
* 函数名称: UTF8_To_GB2312
* 函数功能: 将字符串由UTF8转换为GB2312
* 输入参数: inbuf-输入字符串 inlen-输入字符串长度 outlen-输出字符串的长度
* 输出参数: outbuf，输出字符串
* 函数返回: 成功SUCCESS 失败FAILURE
******************************************************************************/
OAL_API int UTF8_To_GB2312(char *inbuf, int inlen, char *outbuf, int outlen);


#ifdef __cplusplus
}
#endif

#endif//__OAL_UNICODE_H__