/***************************************************************
模块 ： 时间相关头文件                        
文件 ： oal_time.h
作者 ： Ripple Lee(libozjw@163.com)
版本 ： V1.0
日期 ： 2017-01-18
*****************************************************************/
#ifndef __OAL_TIME_H__
#define __OAL_TIME_H__
#include "oal_typedef.h"
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum TimeFormatE{
	eYMDHMS1 = 0,	// "%Y-%m-%d %H:%M:%S"
	eYMDHMS2 = 1,	// "%Y%m%d%H%M%S"
	eYMDHMS3 = 2,	// "%Y-%m-%d_%H-%M-%S"
	eYMD1 = 3,		// "%Y-%m-%d"
	eYMD2 = 4,		// "%Y%m%d"
	eHMS1 = 5,		// "%H:%M:%S"
	eHMS2 = 6		// "%H%M%S"
};

/*****************************************************************************
* 函数名称: TimeFormatString
* 函数功能: 将日期格式化成字符串
* 输入参数: t-时间 eFormat-格式化类型
* 输出参数: pString，存时间的指针
* 函数返回: 成功，SUCCESS 失败FAILURE
******************************************************************************/
OAL_API int TimeFormatString(time_t t, char *pString, int iLen, TimeFormatE eFormat);

/*****************************************************************************
* 函数名称: String2Time
* 函数功能: 日期转换为time_t
* 输入参数: pString，存时间的指针 eFormat-格式化类型
* 输出参数: 
* 函数返回: 成功，time_t时间 失败FAILURE
******************************************************************************/
OAL_API time_t String2Time(const char *pString, TimeFormatE eFormat);

/*******************************************************************************
* 函数名称: CaculateWeekDay
* 函数功能: 计算星期数
* 输入参数: s32Year: 年 s32Month: 月 s32Day: 日
* 输出参数: 无
* 函数返回: 星期数(0-6)
********************************************************************************/
OAL_API int CaculateWeekDay(int s32Year, int s32Month, int s32Day);

/*******************************************************************************
* 函数名称: GetWeekDay
* 函数功能: 获取星期数
* 输入参数: t
* 输出参数: 无
* 函数返回: 0星期天 1-6 星期一/六
********************************************************************************/
OAL_API int GetWeekDay(time_t t);

/*******************************************************************************
* 函数名称: GetWeekDay
* 函数功能: 获取星期数
* 输入参数: t
* 输出参数: 无
* 函数返回: 0-1440的秒数
********************************************************************************/
OAL_API u32 GetSecTime(time_t t);

#ifdef WIN32 // 一些window没有的通用函数
OAL_API unsigned sleep(unsigned seconds);
OAL_API int usleep(unsigned usec);
OAL_API int gettimeofday(struct timeval *tp, void *tzp);
#else
OAL_API unsigned long GetTickCount();
#endif

#ifdef __cplusplus
}
#endif

#endif//__OAL_TIME_H__