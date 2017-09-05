#ifndef __CAPTURE_STREAM_MAN_H__
#define __CAPTURE_STREAM_MAN_H__
#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "capacity.h"
#include "mq_videoin.h"
#include "h264_queue.h"
#ifdef WIN32
#include <winsock2.h>
#endif

typedef struct _FramePrintInfo 
{
	struct timeval tStartTime;
	struct timeval tEndTime;
	unsigned int   uFrameCnt;
	unsigned int   uIFrameCnt;
	unsigned int   uMaxFrameSize;
	unsigned int   uAvgFrameSize;
}FramePrintInfo;

class StreamMan
{
public:
	static StreamMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	StreamMan();
	~StreamMan();

public:
	int Run();

	int SetChnRtspInfo(int iChnID, const RtspInfo* ptInfo);
	int GetChnRtspInfo(int iChnID, RtspInfo & tInfo);

	int Stream_Init(int iChnID);
	int Stream_Close(int iChnID);

	int GetChnSolution(int iChnID, int & width, int & height, int & fps);

	int PrintfCountInfo(int iChnID);
	int RecordFrameInfo(const H264_Data_Header *ptHeader);
public:
	bool 		 m_bExitThread;
	bool 		 m_bDisconnected[MAX_CHANNEL_NUM];
	H264_Queue	 m_tH264SQ[MAX_CHANNEL_NUM];
	void *		 m_pVSource[MAX_CHANNEL_NUM];
private:
	RtspInfo		m_tRtspInfo[MAX_CHANNEL_NUM];
	FramePrintInfo  m_tFrameCount[MAX_CHANNEL_NUM];
	pthread_t 		m_manage_thread;
	pthread_t 		m_count_thread;
	pthread_mutex_t m_tMutex;
private:
	static StreamMan* m_pInstance;
};



#endif //__CAPTURE_STREAM_MAN_H__

