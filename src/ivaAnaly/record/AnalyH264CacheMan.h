#ifndef __ALG_RECORD_H__
#define __ALG_RECORD_H__

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "h264_queue.h"
#include "oal_queue.h"
#include "pthread.h"
#include "capacity.h"
#include "RecordWriter.h"


#define REC_CUT_MAX 2// 最多两段
typedef struct _RecMission
{
	int    iID;
	char   szPathFile[512];
	int	   iFileFormat;
	int    iCnt;
	time_t begin[REC_CUT_MAX];
	int    secs[REC_CUT_MAX];
}RecMission;

typedef struct _FrameHead
{
	time_t  t;
	int     size;
	int     frame_type;
}FrameHead;

typedef struct _AlgH264Frame
{
	FrameHead     frame_head;
	unsigned char frame_data[H264_QUEUE_FRAME_MAX];
}AlgH264Frame;

typedef struct _RingBuffer
{
	unsigned char * m_pData;
	long m_pos_r;
	long m_pos_w;
	time_t m_t1;
	time_t m_t2;
	pthread_mutex_t m_hndlSem;
}RingBuffer;

class AnalyH264CacheMan
{
public:
	static AnalyH264CacheMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
	
public:
	int Run();
	int GenRecEvidence(int iChnID, RecMission * ptMission);

	int GenRecFile(int iChnID, const RecMission * mission);
	bool GenRecEvidenceByRecFile(int iChnID, const char * recfile, WRHandle pVideoWriter);
	
public:
	int PushFrameToCache(int iChnID, AlgH264Frame * ptFrame);// 写入缓冲队列
	AlgH264Frame * PopFrameFromCache(int iChnID);

	int WriteFrameToBuffer(int iChnID, AlgH264Frame * ptFrame);// 写入环形缓存

	bool m_bExitThread;
		
private:
	AnalyH264CacheMan();
	~AnalyH264CacheMan();

private:
	TQueHndl m_tH264CacheList[MAX_CHANNEL_NUM];// 缓存队列 第一级
	RingBuffer m_tRingBuf[MAX_CHANNEL_NUM];// 环形缓存 第二级

private:
	static AnalyH264CacheMan* m_pInstance;

	pthread_t m_H264Cache_thread[MAX_CHANNEL_NUM];
	
private:
	int GetFrameHead(int iChnID, FrameHead* ptFrameHead, long lReadPos);
	int MoveReadPos(int iChnID, long lSize);
	
	// 准备可写空间
	int PrepareWriteSize(int iChnID, long lSize);
};


#endif // __ALG_RECORD_H__

