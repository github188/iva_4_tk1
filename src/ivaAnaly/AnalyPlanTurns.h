/**************************************************************************
# Copyright : JAYA	
# Name	    : PlanTurns.h
# Describe  : 分析计划与场景轮换
# Date      : 12/17/2015
# Author    : libo
**************************************************************************/
#ifndef __PLAN_TURNS__H__
#define __PLAN_TURNS__H__

#include "capacity.h"
#include "pthread.h"

// 场景巡航
typedef struct _SceneNodeT
{
	int iSceneID;	// 场景编号
	int iHoldTime;	// 停留时间
}SceneNodeT;

#define MAX_CRUISE_SCENE_NUM 20
typedef struct _SceneCruise
{
	int iCnt;
	SceneNodeT SceneNodes[MAX_CRUISE_SCENE_NUM];
}SceneCruiseT;

class CCruiseExecutor
{
public:
	CCruiseExecutor(const SceneCruiseT * ptSC);
	~CCruiseExecutor();
	void Run();
public:
	int GetCurRunScene();

	int  m_iCur;
	SceneCruiseT m_tSceneCruise;

	bool m_bExitThread;
	
private:
	pthread_t m_run_thread;
};

class CPlanExecutor
{
public:
	CPlanExecutor(int iChnNo);
	~CPlanExecutor();
	void Run();
public:
	int GetCurRunScene();

	CCruiseExecutor* m_ptCruiseExe;
	pthread_mutex_t m_tMutex;

	int m_iChnNo;
	bool m_bExitThread;

private:
	pthread_t m_run_thread;
};

class AnalyPlanMan
{
public:
	static AnalyPlanMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
public:
	void Run();
	int  GetChnCurScene(int iChnID);

private:
	AnalyPlanMan();
	~AnalyPlanMan();
	static AnalyPlanMan* m_pInstance;
	CPlanExecutor *m_ptPlanExe[MAX_CHANNEL_NUM];
};

#endif //__PLAN_TURNS__H__
