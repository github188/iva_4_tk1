/***************************************************************
ģ�� �� ϵͳ��־�ӿ�ͷ�ļ�                        
�ļ� �� oal_log.h
���� �� Ripple Lee(libozjw@163.com)
�汾 �� V1.0
���� �� 2017-01-18
*****************************************************************/
#ifndef __OAL_LOG_H__
#define __OAL_LOG_H__
#include "oal_typedef.h"
#include "pthread.h"

#ifdef __cplusplus
extern "C" {
#endif


/*��־��Ϣ����*/
enum LogLevel
{
	LOG_LEVEL_ERROR = 1,		/*�����صĴ�����Ϣ����*/
	LOG_LEVEL_WARNS = 2,		/*������Ϣ����*/
	LOG_LEVEL_INFOS = 3,		/*�����Ϣ��Ϣ����*/
	LOG_LEVEL_DEBUG = 4,		/*������Ϣ����*/
	LOG_LEVEL_DUMPS = 5			/*����Ϣ���𣬱�ʾ�κο��������Ϣ*/
};

/*****************************************************************************
* ��������: LogInit
* ��������: ��ʼ��ϵͳ��־
* �������: ps8Name,��־�ļ��� u32FileSize��ϵͳ�ļ����ܴ�С
* �������: ��
* ��������: �ɹ����򷵻�SUCCESS��ʧ�ܣ��򷵻�FAILURE
******************************************************************************/
OAL_API int LogInit(const char *ps8Name, u32 u32FileSize);

/*****************************************************************************
* ��������: LogUninit
* ��������: ����ϵͳ��־
* �������: ��
* �������: ��
* ��������: ��
******************************************************************************/
OAL_API void LogUninit(void);

/*****************************************************************************
* ��������: LogTrace
* ��������: дϵͳ��־���ļ�
* �������: iLogLevel��ϵͳ��־����
* �������: ��
* ��������: �ɹ����򷵻�SUCCESS��ʧ�ܣ��򷵻�FAILURE
******************************************************************************/
OAL_API int LogTrace(int iLogLevel, const char *ps8Format, ... );

/*****************************************************************************
* ��������: LogConfig��
* ��������: ����Log��Ϣ����ļ���
* �������: iLogLevel���Ѿ��򿪵������ļ����ļ�ָ�롣
* �������: ��
* ��������: �ɹ����򷵻�SUCCESS��ʧ�ܣ��򷵻�FAILURE��
******************************************************************************/
OAL_API int LogConfig(int iLogLevel);


#define LOG_DEBUG_FMT(fmt,...) LogTrace(LOG_LEVEL_DEBUG, "(%s|%d): "fmt, __FILE__,  __LINE__, ##__VA_ARGS__);
#define LOG_INFOS_FMT(fmt,...) LogTrace(LOG_LEVEL_INFOS, "(%s|%d): "fmt, __FILE__,  __LINE__, ##__VA_ARGS__);
#define LOG_WARNS_FMT(fmt,...) LogTrace(LOG_LEVEL_WARNS, "(%s|%d): "fmt, __FILE__,  __LINE__, ##__VA_ARGS__);
#define LOG_ERROR_FMT(fmt,...) LogTrace(LOG_LEVEL_ERROR, "(%s|%d): "fmt, __FILE__,  __LINE__, ##__VA_ARGS__);


#ifdef __cplusplus
}
#endif


#endif


