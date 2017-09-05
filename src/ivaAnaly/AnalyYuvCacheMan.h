#ifndef __ANALY_YUV_CACHE_H__
#define __ANALY_YUV_CACHE_H__
#include "capacity.h"
#include "oal_time.h"
#include "pthread.h"
#include "yuv_queue.h"

typedef struct _YuvCacheNode
{
	time_t          tNow;
	int             iMsec;
	Yuv_Queue_Node  tYuvData;
}YuvCacheNode;

typedef struct _YuvCacheStruct
{
	pthread_mutex_t tMutex;
	YuvCacheNode  tYuvNode;
	int iStat;
	int iReadCnt;
	int iWriteCnt;
}YuvCacheStruct;

class AnalyYuvMan
{
public:
	static AnalyYuvMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
public:
	void Run();
	bool m_bExitThread;

	int WriteYuvNodeToCache(int iChnID, const Yuv_Queue_Node * pNode);
	int ReadYuvNodeFromCache(int iChnID, YuvCacheNode * pNode);
private:
	AnalyYuvMan();
	~AnalyYuvMan();
	static AnalyYuvMan* m_pInstance;

	pthread_t	 m_yuv_cache_thread[MAX_CHANNEL_NUM];
	YuvCacheStruct m_tNewYuvNode[MAX_CHANNEL_NUM];
};


#endif