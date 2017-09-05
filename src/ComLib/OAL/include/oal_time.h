/***************************************************************
ģ�� �� ʱ�����ͷ�ļ�                        
�ļ� �� oal_time.h
���� �� Ripple Lee(libozjw@163.com)
�汾 �� V1.0
���� �� 2017-01-18
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
* ��������: TimeFormatString
* ��������: �����ڸ�ʽ�����ַ���
* �������: t-ʱ�� eFormat-��ʽ������
* �������: pString����ʱ���ָ��
* ��������: �ɹ���SUCCESS ʧ��FAILURE
******************************************************************************/
OAL_API int TimeFormatString(time_t t, char *pString, int iLen, TimeFormatE eFormat);

/*****************************************************************************
* ��������: String2Time
* ��������: ����ת��Ϊtime_t
* �������: pString����ʱ���ָ�� eFormat-��ʽ������
* �������: 
* ��������: �ɹ���time_tʱ�� ʧ��FAILURE
******************************************************************************/
OAL_API time_t String2Time(const char *pString, TimeFormatE eFormat);

/*******************************************************************************
* ��������: CaculateWeekDay
* ��������: ����������
* �������: s32Year: �� s32Month: �� s32Day: ��
* �������: ��
* ��������: ������(0-6)
********************************************************************************/
OAL_API int CaculateWeekDay(int s32Year, int s32Month, int s32Day);

/*******************************************************************************
* ��������: GetWeekDay
* ��������: ��ȡ������
* �������: t
* �������: ��
* ��������: 0������ 1-6 ����һ/��
********************************************************************************/
OAL_API int GetWeekDay(time_t t);

/*******************************************************************************
* ��������: GetWeekDay
* ��������: ��ȡ������
* �������: t
* �������: ��
* ��������: 0-1440������
********************************************************************************/
OAL_API u32 GetSecTime(time_t t);

#ifdef WIN32 // һЩwindowû�е�ͨ�ú���
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