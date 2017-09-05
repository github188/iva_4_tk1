/**************************************************************************
# Copyright : JAYA	
# Name	    : ASInterface.cpp
# Describe  : 分析规则接口实现
# Date      : 12/2/2015
# Author    : libo
**************************************************************************/
#include "AnalyProfile.h"
#include "AnalyXml.h"
#include <string.h>
#include "oal_regexp.h"
#include "oal_log.h"
#include "oal_file.h"
#include "capacity.h"
#include "oal_time.h"



AnalyProfile* AnalyProfile::m_pInstance = NULL;

AnalyProfile* AnalyProfile::GetInstance()
{
	return m_pInstance;
}

int AnalyProfile::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new AnalyProfile();
		assert(m_pInstance);

		m_pInstance->ReadFromXmlFile();
		m_pInstance->SendInitCfg2Other();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void AnalyProfile::UnInitialize()
{
	SafeDeleteS(m_pInstance);
}

void AnalyProfile::SendInitCfg2Other()
{
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		for (int j = 0; j < MAX_SCENE_NUM; j++)
		{

			AnalyRuleT tSceneRule = {0};
			GetChnAnalyScheme(i, j, &tSceneRule);
			MQ_Analy_Chn_Scn_Rule(i, j, &tSceneRule);
			usleep(10000);
		}

		bool bEnable = false;
		GetChnAlgSwitch(i, bEnable);
		MQ_Analy_Chn_AnalyEnable(i, bEnable);
		usleep(10000);
	}
}

bool AnalyProfile::AlgIsRunning(int iChnID)
{
	bool bRuning = false;
	GetChnAlgSwitch(iChnID, bRuning);
	return bRuning;
}

AnalyProfile::AnalyProfile( const string& strDir /*= ALG_CFG_SAVE_PATH*/,const string& strFileName /*= ALG_CFG_FILE_NAME*/ )
{
	m_ptAs = NULL;
	m_strDir = strDir;
	m_strFileName = strFileName;
	pthread_rwlock_init(&m_Lock,NULL);
}

AnalyProfile::~AnalyProfile(void)
{
	pthread_rwlock_wrlock(&m_Lock);
	SafeDeleteS(m_ptAs);
	pthread_rwlock_unlock(&m_Lock);

	pthread_rwlock_destroy(&m_Lock);
}

int AnalyProfile::UseDefualtAnalyset()
{
	AnalySet * ptAs = NULL;
	ptAs = new AnalySet();
	assert(ptAs);

	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		ASChannel * ptChn = NULL;
		ptChn = new ASChannel();
		assert(ptChn);

		ptAs->Add(ptChn);

		ptChn->m_iChnID = i;
		ptChn->m_bOpenAlg = false;
		ptChn->m_ptSceneSet = NULL;
	}
	
	pthread_rwlock_wrlock(&m_Lock);
	SafeDeleteS(m_ptAs);
	m_ptAs = ptAs;
	pthread_rwlock_unlock(&m_Lock);

	return 0;
}

int AnalyProfile::DeleteChn( int iChnID )
{
	pthread_rwlock_wrlock(&m_Lock);
	ASChannel * ptChn = GetASChannelPtrByID(iChnID);
	if (ptChn)
	{
		SafeDeleteS(ptChn->m_ptSceneSet);
		ptChn->m_bOpenAlg = false;
	}
	pthread_rwlock_unlock(&m_Lock);

	SaveToXmlFile();
	return 0;
}

int AnalyProfile::ReadFromXmlFile()
{
	string strFile = m_strDir + m_strFileName;

	AnalySet * ptAs = NULL;

	// 读取xml配置
	ptAs = XmlParser::XmlParseASFile(strFile.c_str());
	if (ptAs == NULL)
	{
		if(m_ptAs == NULL)
		{
			// 使用默认配置
			UseDefualtAnalyset();
			LOG_INFOS_FMT("[ASInterface] Use defualt annlyset.......");
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		// 释放已经读取的配置
		pthread_rwlock_wrlock(&m_Lock);
		SafeDeleteS(m_ptAs);
		m_ptAs = ptAs;
		pthread_rwlock_unlock(&m_Lock);
		return 0;
	}
}

int AnalyProfile::SaveToXmlFile()
{
	int iRet = 0;

	string strPrefix = "~";
	string strFile = m_strDir + m_strFileName;
	string strFile_temp = m_strDir + strPrefix + m_strFileName;

	OAL_MakeDirByPath(strFile_temp.c_str());

	// 将配置保存到临时文件中
	pthread_rwlock_wrlock(&m_Lock);
	iRet = XmlParser::XmlSaveAsFile(m_ptAs, strFile_temp.c_str());
	pthread_rwlock_unlock(&m_Lock);

	if (iRet != 0)
	{
		return -1;
	}

	// 将临时文件保存为正式文件
	if( rename( strFile_temp.c_str(), strFile.c_str() ) != 0 )
	{
		return -1;
	}

	remove(strFile_temp.c_str());

	return 0;

}

int AnalyProfile::GetChnAnalyScheme( int iChnID, int iScene, AnalyRuleT * ptRule )
{
	if(ptRule == NULL)
	{
		return -1;
	}
	
	pthread_rwlock_wrlock(&m_Lock);

	// 通道
	ASChannel * ptChn = GetASChannelPtrByID(iChnID);
	if (ptChn == NULL)
	{
		pthread_rwlock_unlock(&m_Lock);
		return -1;
	}

	int width = 1920, height = 1080;
	int minsize = width/16;
	int maxsize = width/3;
	ptRule->tTargetSize.bEnable = 0;
	ptRule->tTargetSize.atMinPoints[0].x = width/2-minsize/2;
	ptRule->tTargetSize.atMinPoints[0].y = 50;
	ptRule->tTargetSize.atMinPoints[1].x = width/2+minsize/2;
	ptRule->tTargetSize.atMinPoints[1].y = 50+minsize;
	ptRule->tTargetSize.atMaxPoints[0].x = width/2-maxsize/2;
	ptRule->tTargetSize.atMaxPoints[0].y = height-maxsize-50;
	ptRule->tTargetSize.atMaxPoints[1].x = width/2+maxsize/2;
	ptRule->tTargetSize.atMaxPoints[1].y = height-50;
	
	// 场景
	ASScene * ptASScene = GetASScenePtrByID(ptChn,iScene);
	if(ptASScene == NULL)
	{
		pthread_rwlock_unlock(&m_Lock);
		return -1;
	}

	// 目标大小
	ASTargetFilter  * ptTargetFilter = ptASScene->m_pTargetFilter;
	if(ptTargetFilter)
	{
		ptRule->tTargetSize.bEnable = ptTargetFilter->m_bEnable;
		ASMutiPoint * ptMinPoints = ptTargetFilter->m_ptMinPoints;
		if(ptMinPoints)
		{
			for(int i = 0; i < MIN(2, ptMinPoints->GetCnt()); i++)
			{
				ASPoint *ptPoint = ptMinPoints->GetAt(i);
				assert(ptPoint);
				ptRule->tTargetSize.atMinPoints[i].x = ptPoint->m_iX;
				ptRule->tTargetSize.atMinPoints[i].y = ptPoint->m_iY;
			}
		}
		ASMutiPoint * ptMaxPoints = ptTargetFilter->m_ptMaxPoints;
		if(ptMaxPoints)
		{
			for(int i = 0; i < MIN(2, ptMaxPoints->GetCnt()); i++)
			{
				ASPoint *ptPoint = ptMaxPoints->GetAt(i);
				assert(ptPoint);
				ptRule->tTargetSize.atMaxPoints[i].x = ptPoint->m_iX;
				ptRule->tTargetSize.atMaxPoints[i].y = ptPoint->m_iY;
			}
		}
	}

	// 标定场景
	ASCalibration  * ptCalibration = ptASScene->m_ptCalibration;
	if(ptCalibration)
	{
		ptRule->tCalib.length = ptCalibration->m_fLength;
		ptRule->tCalib.width  = ptCalibration->m_fWidth;

		ASMutiPoint * ptPoints = ptCalibration->m_ptPoints;
		if(ptPoints)
		{
			for(int i = 0; i < MIN(4, ptPoints->GetCnt()); i++)
			{
				ASPoint *ptPoint = ptPoints->GetAt(i);
				assert(ptPoint);
				ptRule->tCalib.atPoints[i].x = ptPoint->m_iX;
				ptRule->tCalib.atPoints[i].y = ptPoint->m_iY;
			}
		}
	}

	// 屏蔽区域	
	ASMutiPointSet * ptShieldAreaSet = ptASScene->m_ptShieldAreaSet;
	if(ptShieldAreaSet)
	{
		ptRule->iSheildAreaNum =  MIN(ptShieldAreaSet->GetCnt(),MAX_SHEILD_AREA_NUM);
		for(int i = 0; i < ptRule->iSheildAreaNum; i++)
		{
			ASMutiPoint * ptPointSet = ptShieldAreaSet->GetAt(i);
			assert(ptPointSet);
			ptRule->atSheildAreas[i].iPtNum = MIN(ptPointSet->GetCnt(),AREA_MAX_POINT_NUM);
			for(int j = 0; j < ptRule->atSheildAreas[i].iPtNum; j++)
			{
				ASPoint * ptPoint = ptPointSet->GetAt(j);
				assert(ptPoint);
				ptRule->atSheildAreas[i].atPoints[j].x = ptPoint->m_iX;
				ptRule->atSheildAreas[i].atPoints[j].y = ptPoint->m_iY;
			}
		}
	}

	// 分析区域
	ASMutiPointSet * ptAnalyAreaSet = ptASScene->m_ptAnalyAreaSet;
	if(ptAnalyAreaSet)
	{
		ptRule->iAnalyAreaNum = MIN(ptAnalyAreaSet->GetCnt(),MAX_ANALY_AREA_NUM);
		for(int i = 0; i < ptRule->iAnalyAreaNum; i++)
		{
			ASMutiPoint * ptPointSet = ptAnalyAreaSet->GetAt(i);
			assert(ptPointSet);
			ptRule->atAnalyAreas[i].iPtNum = MIN(ptPointSet->GetCnt(),AREA_MAX_POINT_NUM);
			for(int j = 0; j < ptRule->atAnalyAreas[i].iPtNum; j++)
			{
				ASPoint * ptPoint = ptPointSet->GetAt(j);
				assert(ptPoint);
				ptRule->atAnalyAreas[i].atPoints[j].x = ptPoint->m_iX;
				ptRule->atAnalyAreas[i].atPoints[j].y = ptPoint->m_iY;
			}
		}
	}

	// 判定线
	ASMutiPointSet * ptJudgeLineSet = ptASScene->m_ptJudgeLineSet;
	if(ptJudgeLineSet)
	{
		ptRule->iJudgeLineNum = MIN(ptJudgeLineSet->GetCnt(),MAX_ANALY_AREA_NUM);
		for(int i = 0; i < ptRule->iJudgeLineNum; i++)
		{
			ASMutiPoint * ptPointSet = ptJudgeLineSet->GetAt(i);
			assert(ptPointSet);
			ptRule->atudgeLines[i].iPtNum = MIN(ptPointSet->GetCnt(),AREA_MAX_POINT_NUM);
			for(int j = 0; j < ptRule->atudgeLines[i].iPtNum; j++)
			{
				ASPoint * ptPoint = ptPointSet->GetAt(j);
				assert(ptPoint);
				ptRule->atudgeLines[i].atPoints[j].x = ptPoint->m_iX;
				ptRule->atudgeLines[i].atPoints[j].y = ptPoint->m_iY;
			}
		}
	}

	pthread_rwlock_unlock(&m_Lock);

	return 0;
}

int AnalyProfile::SetChnAnalyScheme( int iChnID, int iScene, AnalyRuleT * ptRule )
{
	if(ptRule == NULL)
	{
		return -1;
	}
	
	pthread_rwlock_wrlock(&m_Lock);

	// 根
	if(m_ptAs == NULL)
	{
		m_ptAs = new AnalySet();
		assert(m_ptAs);
	}

	// 通道
	ASChannel * ptChn = GetASChannelPtrByID(iChnID);
	if (ptChn == NULL)
	{
		ptChn = new ASChannel();
		assert(ptChn);

		ptChn->m_iChnID = iChnID;
		
		m_ptAs->Add(ptChn);// 添加到集合
	}

	// 场景
	ASScene * ptASScene = GetASScenePtrByID(ptChn,iScene);
	if(ptASScene == NULL)
	{
		ptASScene = new ASScene();
		assert(ptASScene);
	
		ptASScene->m_iScnID = iScene;

		if(ptChn->m_ptSceneSet == NULL)
		{
			ptChn->m_ptSceneSet = new ASSceneSet();
			assert(ptChn->m_ptSceneSet);
		}
		
		ptChn->m_ptSceneSet->Add(ptASScene);
	}

	// 释放以前的资源，就能增删改
	SafeDeleteS(ptASScene->m_pTargetFilter);

	// 标定场景
	ASTargetFilter * pTargetFilter = new ASTargetFilter();
	assert(pTargetFilter);
	ptASScene->m_pTargetFilter = pTargetFilter;

	pTargetFilter->m_bEnable = ptRule->tTargetSize.bEnable;
	ASMutiPoint * ptMinPoints = new ASMutiPoint();
	assert(ptMinPoints);
	pTargetFilter->m_ptMinPoints = ptMinPoints;
	for(int i = 0; i < 2; i++)
	{
		ASPoint *ptPoint = new ASPoint();
		assert(ptPoint);
		ptMinPoints->Add(ptPoint);

		ptPoint->m_iX = ptRule->tTargetSize.atMinPoints[i].x;
		ptPoint->m_iY = ptRule->tTargetSize.atMinPoints[i].y;
	}
	ASMutiPoint * ptMaxPoints = new ASMutiPoint();
	assert(ptMaxPoints);
	pTargetFilter->m_ptMaxPoints = ptMaxPoints;
	for(int i = 0; i < 2; i++)
	{
		ASPoint *ptPoint = new ASPoint();
		assert(ptPoint);
		ptMaxPoints->Add(ptPoint);

		ptPoint->m_iX = ptRule->tTargetSize.atMaxPoints[i].x;
		ptPoint->m_iY = ptRule->tTargetSize.atMaxPoints[i].y;
	}

	// 释放以前的资源，就能增删改
	SafeDeleteS(ptASScene->m_ptCalibration);

	// 标定场景
	ASCalibration * ptCalibration = new ASCalibration();
	assert(ptCalibration);
	ptASScene->m_ptCalibration = ptCalibration;

	ptCalibration->m_fLength = ptRule->tCalib.length;
	ptCalibration->m_fWidth  = ptRule->tCalib.width;

	ASMutiPoint * ptPoints = new ASMutiPoint();
	assert(ptPoints);
	ptCalibration->m_ptPoints = ptPoints;
	for(int i = 0; i < 4; i++)
	{
		ASPoint *ptPoint = new ASPoint();
		assert(ptPoint);
		ptPoints->Add(ptPoint);

		ptPoint->m_iX = ptRule->tCalib.atPoints[i].x;
		ptPoint->m_iY = ptRule->tCalib.atPoints[i].y;
	}

	// 屏蔽区域
	SafeDeleteS(ptASScene->m_ptShieldAreaSet);
	ASMutiPointSet * ptShieldAreaSet = new ASMutiPointSet();
	assert(ptShieldAreaSet);
	ptASScene->m_ptShieldAreaSet = ptShieldAreaSet;
	for(int i = 0; i < MIN(ptRule->iSheildAreaNum,MAX_SHEILD_AREA_NUM); i++)
	{
		ASMutiPoint * ptArea = new ASMutiPoint();
		assert(ptArea);
		ptShieldAreaSet->Add(ptArea);

		for(int j = 0; j < MIN(ptRule->atSheildAreas[i].iPtNum, AREA_MAX_POINT_NUM); j++)
		{
			ASPoint * ptPoint = new ASPoint();
			assert(ptPoint);
			ptArea->Add(ptPoint);

			ptPoint->m_iX = ptRule->atSheildAreas[i].atPoints[j].x;
			ptPoint->m_iY = ptRule->atSheildAreas[i].atPoints[j].y;
		}
	}
	
	// 分析区域
	SafeDeleteS(ptASScene->m_ptAnalyAreaSet);
	ASMutiPointSet * ptAnalyAreaSet = new ASMutiPointSet();
	assert(ptAnalyAreaSet);
	ptASScene->m_ptAnalyAreaSet = ptAnalyAreaSet;

	for(int i = 0; i < MIN(ptRule->iAnalyAreaNum, MAX_ANALY_AREA_NUM); i++)
	{
		ASMutiPoint * ptArea = new ASMutiPoint();
		assert(ptArea);
		ptAnalyAreaSet->Add(ptArea);

		for(int j = 0; j < MIN(ptRule->atAnalyAreas[i].iPtNum, AREA_MAX_POINT_NUM); j++)
		{
			ASPoint * ptPoint = new ASPoint();
			assert(ptPoint);
			ptArea->Add(ptPoint);

			ptPoint->m_iX = ptRule->atAnalyAreas[i].atPoints[j].x;
			ptPoint->m_iY = ptRule->atAnalyAreas[i].atPoints[j].y;
		}	
	}

	// 判定线
	SafeDeleteS(ptASScene->m_ptJudgeLineSet);
	ASMutiPointSet * ptJudgeLineSet = new ASMutiPointSet();
	assert(ptJudgeLineSet);
	ptASScene->m_ptJudgeLineSet = ptJudgeLineSet;

	for(int i = 0; i < MIN(ptRule->iJudgeLineNum, MAX_ANALY_AREA_NUM); i++)
	{
		ASMutiPoint * ptArea = new ASMutiPoint();
		assert(ptArea);
		ptJudgeLineSet->Add(ptArea);

		for(int j = 0; j < MIN(ptRule->atudgeLines[i].iPtNum, AREA_MAX_POINT_NUM); j++)
		{
			ASPoint * ptPoint = new ASPoint();
			assert(ptPoint);
			ptArea->Add(ptPoint);

			ptPoint->m_iX = ptRule->atudgeLines[i].atPoints[j].x;
			ptPoint->m_iY = ptRule->atudgeLines[i].atPoints[j].y;
		}	
	}
	
	pthread_rwlock_unlock(&m_Lock);

	SaveToXmlFile();

	return 0;
}

int AnalyProfile::GetChnAlgSwitch( int iChnID, bool &bUse )
{
	pthread_rwlock_wrlock(&m_Lock);
	
	// 通道
	ASChannel * ptChn = GetASChannelPtrByID(iChnID);
	if (ptChn == NULL)
	{
		bUse = false;
	}
	else
	{
		bUse = ptChn->m_bOpenAlg;
	}
	
	pthread_rwlock_unlock(&m_Lock);

	return 0;
}

int AnalyProfile::SetChnAlgSwitch( int iChnID, bool bUse )
{
	pthread_rwlock_wrlock(&m_Lock);
	
	// 通道
	ASChannel * ptChn = GetASChannelPtrByID(iChnID);
	if (ptChn == NULL)
	{
		pthread_rwlock_unlock(&m_Lock);
		return -1;
	}


	ptChn->m_bOpenAlg = bUse;
	
	pthread_rwlock_unlock(&m_Lock);
	
	SaveToXmlFile();

	return 0;
}

ASChannel * AnalyProfile::GetASChannelPtrByID( int iChnID )
{
	ASChannel * ptChnFind = NULL;

	if (m_ptAs)
	{
		for (int i = 0; i < m_ptAs->GetCnt(); i++)
		{
			ASChannel * ptChn = m_ptAs->GetAt(i);
			if (ptChn)
			{
				if (ptChn->m_iChnID == iChnID)
				{
					ptChnFind = ptChn;
					break;
				}
			}
		}
	}

	return ptChnFind;
}

ASScene* AnalyProfile::GetASScenePtrByID( ASChannel * ptChn,int iSceneID )
{
	ASScene * ptFind = NULL;

	if (ptChn)
	{
		if(ptChn->m_ptSceneSet)
		{
			
			for (int i = 0; i < ptChn->m_ptSceneSet->GetCnt(); i++)
			{
				ASScene * ptScene = ptChn->m_ptSceneSet->GetAt(i);
				if (ptScene)
				{
					if (ptScene->m_iScnID == iSceneID)
					{
						ptFind = ptScene;
						break;
					}
				}
			}
		}
	}

	return ptFind;
}



