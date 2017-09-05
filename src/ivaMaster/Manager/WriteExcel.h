#include "LogManager.h"
#include "AlarmManager.h"
#ifndef __WRITE_EXCEL_H__
#define __WRITE_EXCEL_H__


/*****************************************************************************
* 函数名称: WriteLog2Excel
* 函数功能: 日志记录导出到Excel
* 输入参数: ptList-记录 iCnt-记录条数 ps8SavePath-保存路径
* 输出参数: 无
* 函数返回: 成功SUCCESS 失败FAILURE
******************************************************************************/
int WriteLog2Excel( const LogRecord* ptList, int iCnt, const char *ps8SavePath );

/*****************************************************************************
* 函数名称: WriteAlarm2Excel
* 函数功能: 告警记录导出到Excel
* 输入参数: ptList-记录 iCnt-记录条数 ps8SavePath-保存路径
* 输出参数: 无
* 函数返回: 成功SUCCESS 失败FAILURE
******************************************************************************/
int WriteAlarm2Excel(const AlarmRecord* ptList, int iCnt, const char *ps8SavePath);

#endif// __WRITE_EXCEL_H__

