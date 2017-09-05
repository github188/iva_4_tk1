/***************************************************************
ģ�� �� ������ʽ���ͷ�ļ�                        
�ļ� �� oal_regexp.h
���� �� Ripple Lee(libozjw@163.com)
�汾 �� V1.0
���� �� 2017-01-18
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
* ��������: IsIPv4Addr
* ��������: �ж��ַ����Ƿ��ǺϷ���IPv4��ַ
* �������: szAddr�� ҪУ����ַ���
* �������: ��
* ��������: �ɹ����򷵻�SUCCESS��ʧ�ܣ��򷵻�FAILURE
*******************************************************************************************/
OAL_API	int IsIPv4Addr(const char* szAddr);

#ifdef __cplusplus
}
#endif

#endif//__OAL_REG_EXP_H__