#include "AnalyYuvCacheMan.h"
#include <assert.h>
#include <stdio.h>
#include "yuv_queue.h"
#include "oal_log.h"
#include "oal_time.h"
#ifdef WIN32
#include <winsock.h>
#endif // WIN32


static int g_iChnID[MAX_CHANNEL_NUM] ={0};

void* YuvCacheThreadFxn(void* param)
{
	Yuv_Queue 		 tYuvQueue = {0};
	Yuv_Queue_Node * ptYuvNode = NULL;

	if(param == NULL)
	{
		return NULL;
	}

	int iChnID = *((int *)param);
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return NULL;
	}

	// 打开YUV共享内存队列
	tYuvQueue.iChnID = iChnID;
	int iRet = Yuv_QueueOpen(&tYuvQueue);
	if(iRet != 0)
	{
		LOG_ERROR_FMT("Channel%d Yuv_QueueOpen failed",iChnID);
		return NULL;
	}

	LOG_DEBUG_FMT("Channel%u Yuv_QueueOpen OK",iChnID);

	while(!AnalyYuvMan::GetInstance()->m_bExitThread)
	{
		if(ptYuvNode)
		{
			// 清空YUV队列
			Yuv_QueuePutEmpty(&tYuvQueue);
			ptYuvNode = NULL;
		}

		// 从YUV队列中抽取一帧数据
		ptYuvNode = Yuv_QueueGetFull(&tYuvQueue);
		if(ptYuvNode == NULL)
		{
			usleep(10*1000);
			continue;
		}

		//printf("Yuv_QueueGetFull frame_id:%d timestamp:%llu\n", ptYuvNode->yuv_info.frame_id, ptYuvNode->yuv_info.timestamp);
		AnalyYuvMan::GetInstance()->WriteYuvNodeToCache(iChnID, ptYuvNode);
	}

	// 关闭YUV队列
	Yuv_QueueClose(&tYuvQueue);

	pthread_exit(NULL);
	return NULL;
}

AnalyYuvMan* AnalyYuvMan::m_pInstance = NULL;

AnalyYuvMan* AnalyYuvMan::GetInstance()
{
	return m_pInstance;
}

int AnalyYuvMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new AnalyYuvMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void AnalyYuvMan::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void AnalyYuvMan::Run()
{
	m_bExitThread = false;
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		g_iChnID[i] = i;
		if(pthread_create(&m_yuv_cache_thread[i], NULL, YuvCacheThreadFxn, &g_iChnID[i]))
		{
			LOG_ERROR_FMT("Channel%d YuvCache Thread create failed", i);
		}
	}
}

AnalyYuvMan::AnalyYuvMan()
{
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		pthread_mutex_init(&(m_tNewYuvNode[i].tMutex), NULL);
		memset(&(m_tNewYuvNode[i].tYuvNode), 0, sizeof(Yuv_Queue_Node));
		m_tNewYuvNode[i].iStat = 0;
		m_tNewYuvNode[i].iReadCnt = 0;
		m_tNewYuvNode[i].iWriteCnt = 0;
	}
}

AnalyYuvMan::~AnalyYuvMan()
{
	m_bExitThread = true;
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		pthread_join(m_yuv_cache_thread[i], NULL);
		pthread_mutex_destroy(&(m_tNewYuvNode[i].tMutex));
	}
}

int AnalyYuvMan::WriteYuvNodeToCache(int iChnID, const Yuv_Queue_Node * pNode)
{
	if(pNode == NULL)
	{
		return 0;
	}

	pthread_mutex_lock(&m_tNewYuvNode[iChnID].tMutex);
	memcpy(&(m_tNewYuvNode[iChnID].tYuvNode.tYuvData), pNode, sizeof(Yuv_Queue_Node));
	struct timeval _tv;
	gettimeofday(&_tv, NULL);
	m_tNewYuvNode[iChnID].tYuvNode.tNow  = _tv.tv_sec;
	m_tNewYuvNode[iChnID].tYuvNode.iMsec = _tv.tv_usec/1000;
	m_tNewYuvNode[iChnID].iStat = 1;
	m_tNewYuvNode[iChnID].iWriteCnt++;
	pthread_mutex_unlock(&m_tNewYuvNode[iChnID].tMutex);
	return 0;
}

int AnalyYuvMan::ReadYuvNodeFromCache(int iChnID, YuvCacheNode * pNode)
{
	int iRet = -1;
	if(pNode == NULL || iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tNewYuvNode[iChnID].tMutex);
	if(m_tNewYuvNode[iChnID].iStat > 0)
	{
		memcpy(pNode, &(m_tNewYuvNode[iChnID].tYuvNode),sizeof(YuvCacheNode));
		m_tNewYuvNode[iChnID].iStat = 0;
		iRet = 0;
		m_tNewYuvNode[iChnID].iReadCnt++;

		if (m_tNewYuvNode[iChnID].iWriteCnt%500==0 && m_tNewYuvNode[iChnID].iWriteCnt > 0)
		{
			printf("YUV::Write:%d Read:%d\n", m_tNewYuvNode[iChnID].iWriteCnt,m_tNewYuvNode[iChnID].iReadCnt);
		}
	}
	else
	{
		//printf("m_tNewYuvNode[iChnID].iStat = 0\n");
	}
	pthread_mutex_unlock(&m_tNewYuvNode[iChnID].tMutex);

	return iRet;
}
