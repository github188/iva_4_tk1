#include "AnalyPlanTurns.h"
#include <stdio.h>
#include <string.h> 
#include <assert.h>
#include <time.h>
#include "oal_time.h"
#include "AnalyCfgMan.h"
#include "oal_log.h"

void * ExecuteCruise(void * p)
{
	if(p == NULL)
	{
		return NULL;
	}
	
	CCruiseExecutor * pExe = (CCruiseExecutor *)p;
	SceneCruiseT *ptSC = &(pExe->m_tSceneCruise);

	if(pExe->m_tSceneCruise.iCnt < 1)
	{
		printf("ExecuteCruise Cnt:%d Quit\n", pExe->m_tSceneCruise.iCnt);
		return NULL;
	}

	int    iCur   = -1;	// 目前执行到那个场景,从0开始
	time_t tBegin = 0;	// 目前这个场景开始的时间,从现在开始
	char szNow[40] = {0};

	LOG_DEBUG_FMT("ExecuteCruise Thread is running...");
	printf("ExecuteCruise Cnt:%d\n", pExe->m_tSceneCruise.iCnt);
	for (int i = 0; i < pExe->m_tSceneCruise.iCnt; i++)
	{
		printf("ExecuteCruise %d: SceneID=%d HoldTime=%d\n", i, pExe->m_tSceneCruise.SceneNodes[i].iSceneID, pExe->m_tSceneCruise.SceneNodes[i].iHoldTime);
	}

	while(!pExe->m_bExitThread)
	{
		// 一秒执行一次
		sleep(1);

		if (pExe->m_iCur < 0)
		{
			iCur   = 0;
			tBegin = time(NULL);
			pExe->m_iCur = iCur;

			TimeFormatString(tBegin, szNow, sizeof(szNow), eYMDHMS1);
			LOG_DEBUG_FMT("[SceneCruise] scene %d begin at %s hold %d secs",
				ptSC->SceneNodes[iCur].iSceneID,szNow,
				ptSC->SceneNodes[iCur].iHoldTime);
		}

		time_t tNow = time(NULL);
		if(tNow - tBegin > ptSC->SceneNodes[iCur].iHoldTime)
		{
			TimeFormatString(tNow, szNow, sizeof(szNow), eYMDHMS1);
			LOG_DEBUG_FMT("[SceneCruise] scene %d end at %s",
				ptSC->SceneNodes[iCur].iSceneID,szNow);
			
			iCur++;
			if(iCur >= ptSC->iCnt || iCur >= MAX_CRUISE_SCENE_NUM)
			{
				iCur = 0;
			}
			tBegin = tNow;
			pExe->m_iCur = iCur;

			TimeFormatString(tBegin, szNow, sizeof(szNow), eYMDHMS1);
			LOG_DEBUG_FMT("[SceneCruise] scene %d begin at %s hold %d secs",
				ptSC->SceneNodes[iCur].iSceneID,szNow,
				ptSC->SceneNodes[iCur].iHoldTime);
		}
	}

	LOG_DEBUG_FMT("[SceneCruise] quit");
	pExe->m_iCur = -1;
	pthread_exit(p);

	return NULL;
}

CCruiseExecutor::CCruiseExecutor(const SceneCruiseT * ptSC)
{
	m_iCur = -1;
	if (ptSC)
	{
		memcpy(&m_tSceneCruise, ptSC, sizeof(SceneCruiseT));
	}
}

CCruiseExecutor::~CCruiseExecutor()
{
	m_bExitThread = true;
	pthread_join(m_run_thread,NULL);
}

// 获取当前运行的场景
void CCruiseExecutor::Run()
{
	m_bExitThread = false;
	pthread_create(&m_run_thread, NULL, ExecuteCruise, this);
}

int CCruiseExecutor::GetCurRunScene()
{
	if(m_iCur < 0 )
	{
		return -1;
	}
	return m_tSceneCruise.SceneNodes[m_iCur].iSceneID;
}

void * PlanExecute(void * p)
{
	if(p == NULL)
	{
		return NULL;
	}

	CPlanExecutor * pExe = (CPlanExecutor *)p;

	pthread_mutex_lock(&pExe->m_tMutex);
	pExe->m_ptCruiseExe = NULL;
	pthread_mutex_unlock(&pExe->m_tMutex);

	int iChnID = pExe->m_iChnNo;
	
	int AlgRun  = -1;
	int UsePlan = -1;
	
	int InPlan    = -2;
	int PlanIndex = -2;
	LOG_DEBUG_FMT("Channel%d PlanExecute Thread is running...", iChnID);
	
	while(!pExe->m_bExitThread)
	{
		// 一秒执行一次
		sleep(1);

		if(!AnalyCfgMan::GetInstance()->ChnAlgIsOpen(iChnID))
		{
			// 停止正在进行的巡航
			InPlan = -1;
			PlanIndex = -2;
			pthread_mutex_lock(&pExe->m_tMutex);
			if(pExe->m_ptCruiseExe)
			{
				delete pExe->m_ptCruiseExe;
				pExe->m_ptCruiseExe = NULL;
			}
			pthread_mutex_unlock(&pExe->m_tMutex);

			// 算法停止
			if(AlgRun != 0)
			{
				LOG_DEBUG_FMT("Channel%d Stop Plan by colse anlysis...", iChnID);
				AlgRun = 0;
			}
			
			usleep(500*1000);
			continue;
		}

		if(AlgRun != 1)
		{
			LOG_DEBUG_FMT("Channel%d Begin Excute Analy...",iChnID);
			AlgRun = 1;
		}
		
		if(AnalyCfgMan::GetInstance()->ChnPlanIsOpen(iChnID))
		{
			if(UsePlan != 1)
			{
				LOG_DEBUG_FMT("Channel%d Excute Analy By Plan...",iChnID);
				UsePlan = 1;
			}
			
			// 按计划执行
			int iWeekDay = GetWeekDay(time(NULL));
			int iTimeNow = GetSecTime(time(NULL));
			int iIndex   = AnalyCfgMan::GetInstance()->ChnIsInPlan(iChnID,iWeekDay,iTimeNow);

			if(iIndex == -1)
			{
				// 停止正在进行的巡航
				PlanIndex = -2;
				pthread_mutex_lock(&pExe->m_tMutex);
				if(pExe->m_ptCruiseExe)
				{
					delete pExe->m_ptCruiseExe;
					pExe->m_ptCruiseExe = NULL;
				}
				pthread_mutex_unlock(&pExe->m_tMutex);
			
				if(InPlan != 0)
				{
					LOG_DEBUG_FMT("Channel%d Is Not In Plan...",iChnID);
					InPlan = 0;
				}

				// 计划未触发
				usleep(500*1000);
				continue;
			}

			if(InPlan != 1)
			{
				LOG_DEBUG_FMT("Channel%d Is In Plan...",iChnID);
				InPlan = 1;
			}

			// 更换了巡航时间段
			if(iIndex != PlanIndex)
			{
				LOG_DEBUG_FMT("Channel%d Do Cruise (weekday=%d timesection=%d)...",iChnID, iWeekDay, iIndex);

				// 停止正在进行的巡航
				pthread_mutex_lock(&pExe->m_tMutex);
				if(pExe->m_ptCruiseExe)
				{
					delete pExe->m_ptCruiseExe;
					pExe->m_ptCruiseExe = NULL;
				}
				pthread_mutex_unlock(&pExe->m_tMutex);
			
				// 创建新的巡航
				PlanIndex = iIndex;

				SceneCruiseT tSC = {0};
				AnalyCfgMan::GetInstance()->GetSceneGlobalCruise(iChnID,&tSC);

				pthread_mutex_lock(&pExe->m_tMutex);
				pExe->m_ptCruiseExe = new CCruiseExecutor(&tSC);
				assert(pExe->m_ptCruiseExe);
				pExe->m_ptCruiseExe->Run();
				pthread_mutex_unlock(&pExe->m_tMutex);
			}
		}
		else
		{
			// 全场景巡航
			if(UsePlan != 0)
			{
				LOG_DEBUG_FMT("Channel%d Excute Analy By AllScene...",iChnID);
				UsePlan = 0;
			}

			// 更换了巡航时间段
			if(-1 != PlanIndex)
			{
				LOG_DEBUG_FMT("Channel%d Do Cruise AllScene...",iChnID);

				// 停止正在进行的巡航
				pthread_mutex_lock(&pExe->m_tMutex);
				if(pExe->m_ptCruiseExe)
				{
					delete pExe->m_ptCruiseExe;
					pExe->m_ptCruiseExe = NULL;
				}
				pthread_mutex_unlock(&pExe->m_tMutex);
			
				// 创建新的巡航
				PlanIndex = -1;
				
				SceneCruiseT tSC = {0};
				AnalyCfgMan::GetInstance()->GetSceneGlobalCruise(iChnID,&tSC);

				pthread_mutex_lock(&pExe->m_tMutex);
				pExe->m_ptCruiseExe = new CCruiseExecutor(&tSC);
				assert(pExe->m_ptCruiseExe);
				pExe->m_ptCruiseExe->Run();
				pthread_mutex_unlock(&pExe->m_tMutex);
			}
		}
	}
	
	pthread_exit(p);
	return NULL;
}

CPlanExecutor::CPlanExecutor(int iChnNo)
{
	m_iChnNo = iChnNo;
	m_bExitThread = false;
	pthread_mutex_init(&m_tMutex, NULL);
	
	pthread_mutex_lock(&m_tMutex);
	m_ptCruiseExe = NULL;
	pthread_mutex_unlock(&m_tMutex);
}

CPlanExecutor::~CPlanExecutor()
{
	m_bExitThread = true;
	pthread_join(m_run_thread,NULL);

	pthread_mutex_lock(&m_tMutex);
	if(m_ptCruiseExe)
	{
		delete m_ptCruiseExe;
		m_ptCruiseExe = NULL;
	}
	pthread_mutex_unlock(&m_tMutex);

	pthread_mutex_destroy(&m_tMutex);
}

void CPlanExecutor::Run()
{
	m_bExitThread = false;
	pthread_create(&m_run_thread, NULL, PlanExecute, this);
}

int CPlanExecutor::GetCurRunScene()
{
	int iRet = -1;
	
	pthread_mutex_lock(&m_tMutex);
	if(m_ptCruiseExe)
	{
		if (m_ptCruiseExe->m_iCur >=0 && m_ptCruiseExe->m_iCur < MAX_CRUISE_SCENE_NUM)
		{
			iRet = m_ptCruiseExe->m_tSceneCruise.SceneNodes[m_ptCruiseExe->m_iCur].iSceneID;
		}
	}

	pthread_mutex_unlock(&m_tMutex);

	return iRet;
}

AnalyPlanMan* AnalyPlanMan::m_pInstance = NULL;

AnalyPlanMan* AnalyPlanMan::GetInstance()
{
	return m_pInstance;
}

int AnalyPlanMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new AnalyPlanMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void AnalyPlanMan::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

AnalyPlanMan::AnalyPlanMan()
{
	for(int chn = 0; chn <  MAX_CHANNEL_NUM; chn++)
	{
		m_ptPlanExe[chn] = new CPlanExecutor(chn);
		assert(m_ptPlanExe[chn]);
	}
}

AnalyPlanMan::~AnalyPlanMan()
{
	for(int chn = 0; chn <  MAX_CHANNEL_NUM; chn++)
	{
		delete m_ptPlanExe[chn];
		m_ptPlanExe[chn] = NULL;
	}
}

void AnalyPlanMan::Run()
{
	for(int chn = 0; chn <  MAX_CHANNEL_NUM; chn++)
	{
		if (m_ptPlanExe[chn])
		{
			m_ptPlanExe[chn]->Run();
		}
	}
}

int AnalyPlanMan::GetChnCurScene( int iChnID )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM -1)
	{
		//printf("%s %d iChnID=%d\n",__FILE__, __LINE__, iChnID);
		return -1;
	}

	if (m_ptPlanExe[iChnID]==NULL)
	{
		//printf("%s %d iChnID=%d\n",__FILE__, __LINE__, iChnID);
		return -1;
	}
	else
	{
		return m_ptPlanExe[iChnID]->GetCurRunScene();
	}
}
