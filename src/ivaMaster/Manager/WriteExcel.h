#include "LogManager.h"
#include "AlarmManager.h"
#ifndef __WRITE_EXCEL_H__
#define __WRITE_EXCEL_H__


/*****************************************************************************
* ��������: WriteLog2Excel
* ��������: ��־��¼������Excel
* �������: ptList-��¼ iCnt-��¼���� ps8SavePath-����·��
* �������: ��
* ��������: �ɹ�SUCCESS ʧ��FAILURE
******************************************************************************/
int WriteLog2Excel( const LogRecord* ptList, int iCnt, const char *ps8SavePath );

/*****************************************************************************
* ��������: WriteAlarm2Excel
* ��������: �澯��¼������Excel
* �������: ptList-��¼ iCnt-��¼���� ps8SavePath-����·��
* �������: ��
* ��������: �ɹ�SUCCESS ʧ��FAILURE
******************************************************************************/
int WriteAlarm2Excel(const AlarmRecord* ptList, int iCnt, const char *ps8SavePath);

#endif// __WRITE_EXCEL_H__

