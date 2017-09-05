#ifndef __ANALY_NULL_RUNNER_H__
#define __ANALY_NULL_RUNNER_H__
#include "BaseRunner.h"

class NullRunner : public BaseRunner
{
public:
	NullRunner(int iChnID);
	~NullRunner();

public:
	virtual void CheckPreset();
	virtual void ProcessFrame(YuvCacheNode * ptYuvNode);
	virtual void ResetAnaly();
	virtual int  CollectAlgResult(int iChnID, const void * ptAlgRes, AnalyData *ptData);
	virtual void ManualSnap(ManualSnapT *ptInfo);

public:
	virtual void CreateAlgChannel();
	virtual void DestroyAlgChannel();

};

#endif//__ANALY_NULL_RUNNER_H__