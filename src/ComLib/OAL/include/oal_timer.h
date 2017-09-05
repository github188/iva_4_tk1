/***************************************************************
模块 ：定时任务相关头文件                        
文件 ： oal_timer.h
作者 ： Ripple Lee(libozjw@163.com)
版本 ： V1.0
日期 ： 2017-01-18
*****************************************************************/
#ifndef _OAL_TIMER_H_
#define _OAL_TIMER_H_
#include "oal_typedef.h"

#ifdef WIN32
#pragma comment(lib, "WS2_32")	// 链接到WS2_32.lib
#else
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#endif
#include <string.h>
#include "pthread.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TIMER_NUM      100   /*定义最大定时器数目*/

typedef void (* TimerProcFunPtr)(void *);
typedef void (* FreeData)(void *);

enum
{
	eTimerOnce   = 1,	// 执行一次的定时器
	eTimerRepeat = 2	// 循环执行的定时器
};

typedef int TTimer; /*定时器句柄类型*/



/*****************************************************************************
* 函数名称: TimerInit
* 函数功能: 定时器初始化
* 输入参数: 无
* 输出参数: 无
* 函数返回: 成功，则返回SUCCESS，失败，则返回FAILURE
******************************************************************************/
OAL_API int TimerInit(void);

/*****************************************************************************
* 函数名称: TimerUninit
* 函数功能: 释放定时器相关资源
* 输入参数: 无
* 输出参数: 无
* 函数返回: 成功，则返回SUCCESS，失败，则返回FAILURE
******************************************************************************/
OAL_API int TimerUninit(void);

/*****************************************************************************
* 函数名称: TimerCreate
* 函数功能: 创建定时器
* 输入参数: iInterval: 定时器时间间隔 单位秒
*		   iRepeat：重复类型 eTimerOnce/eTimerRepeat
*		   pvFunc：函数指针，执行定时器执行函数
*		   pvData：传给定时器执行函数的相关参数
*		   pvFree：参数释放函数
* 输出参数: 无
* 函数返回: 成功，则返回定时器ID，失败，则返回FAILURE
******************************************************************************/
OAL_API TTimer TimerCreate(int iInterval, int iRepeat, 
						   TimerProcFunPtr pvFunc, void *pvData, FreeData pvFree);

/*****************************************************************************
* 函数名称: TimerDelete
* 函数功能: 删除定时器
* 输入参数: tTimerID: 定时器ID，由TimerCreate函数返回
* 输出参数: 无
* 函数返回: 成功，则返回SUCCESS，失败，则返回FAILURE
******************************************************************************/
OAL_API int TimerDelete(TTimer tTimerID);

/*****************************************************************************
* 函数名称: TimerStart
* 函数功能: 启动定时器
* 输入参数: tTimerID: 定时器ID，由TimerCreate函数返回
* 输出参数: 无
* 函数返回: 成功，则返回SUCCESS，失败，则返回FAILURE
******************************************************************************/
OAL_API int TimerStart(TTimer tTimerID);

/*****************************************************************************
* 函数名称: TimerStop
* 函数功能: 停止定时器
* 输入参数: tTimerID: 定时器ID，由TimerCreate函数返回
* 输出参数: 无
* 函数返回: 成功，则返回SUCCESS，失败，则返回FAILURE
******************************************************************************/
OAL_API int TimerStop(TTimer tTimerID);

/*****************************************************************************
* 函数名称: TimerReset
* 函数功能: 更新 对应的定时器的是时钟
* 输入参数: tTimerID: 定时器ID，由TimerCreate函数返回
*		   iInterval: 定时器时间间隔 单位秒
* 输出参数: 无
* 函数返回: 成功，则返回SUCCESS，失败，则返回FAILURE
******************************************************************************/
OAL_API int TimerReset(TTimer tTimerID, int iInterval);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif 

