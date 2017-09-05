#ifndef __DECODE_MAN_H__
#define __DECODE_MAN_H__
#include "capacity.h"
#include "mq_decode.h"
#include "pthread.h"
#include "omx_decode.h"
#include "h264_queue.h"
#include "yuv_queue.h"
#ifdef WIN32
#include <winsock2.h>
#endif

typedef struct _DecodePrintInfo
{
	struct timeval tStartTime;
	struct timeval tEndTime;
	unsigned int   uInputFrameCnt;
	unsigned int   uInputIFrameCnt;
	unsigned int   uOutputFrameCnt;
	unsigned int   uOutputFrameMissCnt;
}DecodePrintInfo;

class DecodeMan
{
public:
	static DecodeMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	DecodeMan();
	~DecodeMan();

public:
	int Run();

	int SetChnVideoInfo(int iChnID, DecodeVideoInfo * ptInfo);
	int GetChnVideoInfo(int iChnID, DecodeVideoInfo & tInfo);

	int GetNewFrameData(int iChnID, H264_Queue_Node * ptNode);

	Yuv_Queue_Node *GetEmptyYuvNode(int iChnID);
	int PutFullYuvNode(int iChnID);

	int Decode_Init(int iChnID);
	int Decode_Close(int iChnID);
	
#ifndef DECODE_OPENMAX
	int Decode_H264(int iChnID, H264_Queue_Node * ptH264Data);
#endif

	bool m_bExitThread;
	int PrintfCountInfo(int iChnID);
	int CountInputInfo(const H264_Data_Header *ptHeader);
	int CountOutputInfo(int iChnID);
	int CountOutputInfoMiss(int iChnID);

private:
	pthread_t 			m_count_thread;
	DecodePrintInfo  	m_tPrintInfo[MAX_CHANNEL_NUM];

#ifdef DECODE_OPENMAX
	OMXDecoder			m_tDecoder[MAX_CHANNEL_NUM];
#else
	FfmpegDecoder		m_tDecoder[MAX_CHANNEL_NUM];
	pthread_t           m_decode_thread[MAX_CHANNEL_NUM];
#endif
	DecodeVideoInfo m_tVideoInfo[MAX_CHANNEL_NUM];
	H264_Queue		m_tH264SQ[MAX_CHANNEL_NUM];
	Yuv_Queue		m_tYuvSQ[MAX_CHANNEL_NUM];
	pthread_mutex_t m_tMutex;

private:
	static DecodeMan* m_pInstance;
};

#endif //__DECODE_MAN_H__


