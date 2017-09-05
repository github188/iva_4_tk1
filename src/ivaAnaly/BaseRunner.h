#ifndef __ANALY_BASE_RUNNER_H__
#define __ANALY_BASE_RUNNER_H__
#include "yuv_queue.h"
#include "pthread.h"
#include "AnalyYuvCacheMan.h"
#include "AnalyEvidenceMan.h"

class BaseRunner
{
public:
	BaseRunner(int iChnID);
	~BaseRunner();

	void Run();

public:
	void CheckSolution(int iWidth, int iHeight);
	virtual void ResetCfg();

public:
	virtual void CheckPreset() = 0;
	virtual void ProcessFrame(YuvCacheNode * ptYuvNode) = 0;
	virtual void ResetAnaly() = 0;
	virtual int  CollectAlgResult(int iChnID, const void * ptAlgRes, AnalyData *ptData) = 0;
	virtual void ManualSnap(ManualSnapT *ptInfo) = 0;

public:
	virtual void CreateAlgChannel() = 0;
	virtual void DestroyAlgChannel() = 0;

public:
	int m_iChnID;
	int m_iWidth;
	int m_iHeight;
	bool m_bExitThread;

private:

	pthread_t  m_run_thread;
};

#endif