/***************************************************************
模块 ： 正则表达式相关头文件                        
文件 ： oal_regexp.h
作者 ： Ripple Lee(libozjw@163.com)
版本 ： V1.0
日期 ： 2017-01-18
*****************************************************************/
#ifndef __OAL_REG_EXP_H__
#define __OAL_REG_EXP_H__
#include "oal_typedef.h"
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MIN
#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

/******************************************************************************************
* 函数名称: IsIPv4Addr
* 函数功能: 判断字符串是否是合法的IPv4地址
* 输入参数: szAddr： 要校验的字符串
* 输出参数: 无
* 函数返回: 成功，则返回SUCCESS，失败，则返回FAILURE
*******************************************************************************************/
OAL_API	int IsIPv4Addr(const char* szAddr);

#ifdef __cplusplus
}
#endif

#endif//__OAL_REG_EXP_H__