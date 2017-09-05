#ifndef __ALG_BASE_H__
#define __ALG_BASE_H__
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "yuv_queue.h"
#include "AlgorithmOutput.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "oal_queue.h"
#include "pthread.h"
#include "mq_analysis.h"
#include "mq_upload.h"
#include "capacity.h"
#include "AnalyPlanTurns.h"
#include "BaseRunner.h"


// Ëã·¨µ÷¶È
class AnalyDispatcher
{
public:
	static AnalyDispatcher* GetInstance();
	static int Initialize();
	static void UnInitialize();
public:
	void Run();
	int ManualSnap(int iChnID, ManualSnapT *ptInfo);

private:
	AnalyDispatcher();
	~AnalyDispatcher();
	static AnalyDispatcher* m_pInstance;

private:
	BaseRunner *m_ptRunner[MAX_CHANNEL_NUM];
};

BaseRunner * RunnerFactoryCreate(int iChnID);
#endif
