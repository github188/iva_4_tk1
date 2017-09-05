/**************************************************************************
# Copyright : JAYA	
# Name	    : ASInterface.h
# Describe  : ��������ӿ�ͷ�ļ�
# Date      : 12/2/2015
# Author    : libo
**************************************************************************/
#ifndef __AS_INTERFACE_H__
#define __AS_INTERFACE_H__

#include "AnalySet.h"
#include "pthread.h"
#include "mq_analysis.h"

#define ALG_CFG_SAVE_PATH  "../profiles/"  // �����ļ�Ŀ¼
#define ALG_CFG_FILE_NAME  "analy.xml"       // ��������

// �����������ýӿ���
class AnalyProfile
{
public:
	static AnalyProfile* GetInstance();
	static int Initialize();
	static void UnInitialize();

	void SendInitCfg2Other();

public:
	int ReadFromXmlFile();

	int SaveToXmlFile();

	bool AlgIsRunning(int iChnID);
	
	int GetChnAnalyScheme(int iChnID, int iScene, AnalyRuleT * ptRule);

	int SetChnAnalyScheme(int iChnID, int iScene, AnalyRuleT* ptRule);

	int GetChnAlgSwitch( int iChnID, bool &bUse );
	
	int SetChnAlgSwitch( int iChnID, bool bUse );
	
	int UseDefualtAnalyset();

	int DeleteChn(int iChnID);

private:
	AnalyProfile(const string& strDir = ALG_CFG_SAVE_PATH,const string& strFileName = ALG_CFG_FILE_NAME);
	~AnalyProfile();

	ASChannel * GetASChannelPtrByID(int iChnID);	
	ASScene*    GetASScenePtrByID( ASChannel * ptChn,int iSceneID );

private:

	string m_strDir;		// �����ļ�Ŀ¼
	string m_strFileName;	// �����ļ�����

	AnalySet * m_ptAs;		// ����
	pthread_rwlock_t m_Lock;// ���ö�д��

private:
	static AnalyProfile* m_pInstance;

};

#endif //__AS_INTERFACE_H__

