/***************************************************************
ģ�� �� ini��ȡ��ؽӿ�ͷ�ļ�                        
�ļ� �� oal_ini.h
���� �� Ripple Lee(libozjw@163.com)
�汾 �� V1.0
���� �� 2017-01-18
*****************************************************************/
#ifndef __OAL_INI_H__
#define __OAL_INI_H__
#include "oal_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


	/*****************************************************************************
	* ��������: OAL_ReadIniString
	* ��������: ���ַ�����ȡ����
	* �������:[title]
	*         key=value
	* �������: ��
	* ��������: �ɹ����򷵻�value��ʧ�ܣ��򷵻�defaultValue
	******************************************************************************/
	OAL_API const char * OAL_ReadIniString(const char *title, const char *key, 
		const char *defaultValue, const char *fileName);

	/*****************************************************************************
	* ��������: OAL_ReadIniNumber
	* ��������: �����ֶ�ȡ����
	* �������:[title]
	*         key=value
	* �������: ��
	* ��������: ���ڣ��򷵻�value�������ڣ��򷵻�defaultValue��
	******************************************************************************/
	OAL_API int OAL_ReadIniNumber(const char *title, const char *key, 
		int defaultValue, const char *fileName);


#ifdef __cplusplus
}
#endif


#endif
