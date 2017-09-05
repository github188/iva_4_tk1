/***************************************************************
模块 ： ini读取相关接口头文件                        
文件 ： oal_ini.h
作者 ： Ripple Lee(libozjw@163.com)
版本 ： V1.0
日期 ： 2017-01-18
*****************************************************************/
#ifndef __OAL_INI_H__
#define __OAL_INI_H__
#include "oal_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


	/*****************************************************************************
	* 函数名称: OAL_ReadIniString
	* 函数功能: 按字符串读取配置
	* 输入参数:[title]
	*         key=value
	* 输出参数: 无
	* 函数返回: 成功，则返回value，失败，则返回defaultValue
	******************************************************************************/
	OAL_API const char * OAL_ReadIniString(const char *title, const char *key, 
		const char *defaultValue, const char *fileName);

	/*****************************************************************************
	* 函数名称: OAL_ReadIniNumber
	* 函数功能: 按数字读取配置
	* 输入参数:[title]
	*         key=value
	* 输出参数: 无
	* 函数返回: 存在，则返回value，不存在，则返回defaultValue。
	******************************************************************************/
	OAL_API int OAL_ReadIniNumber(const char *title, const char *key, 
		int defaultValue, const char *fileName);


#ifdef __cplusplus
}
#endif


#endif
