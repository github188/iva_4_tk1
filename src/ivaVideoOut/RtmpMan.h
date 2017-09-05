#ifndef __RTMP_MAN_H__
#define __RTMP_MAN_H__

#include "capacity.h"
#include "h264_queue.h"
#include "pthread.h"
#include "mq_videoout.h"
#include "RTMPStream.h"

class RtmpMan
{
public:
	static RtmpMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	RtmpMan();
	~RtmpMan();

public:
	int Run();

	int SetStrategy(const VoStrategy * ptInfo);
	int SetServerInfo(int iChnID, const RtmpServer * ptInfo);
	int GetServerInfo(int iChnID, RtmpServer & tInfo);

	int SetEnable(int iChnID, bool bEnable);
	int Reset(int iChnID);

	bool CanIDoNow(int iChnID);

public:
	void ConnectRtmpSvr(int iChnID);
	void DisConnectRtmpSvr(int iChnID);
	bool IsConnectedRtmpSvr(int iChnID);
	int  SendH264Frame(int iChnID, H264_Queue_Node * ptNode);

public:
	bool m_bExitThread;

private:
	VoStrategy      m_tStrategy;
	bool			m_bEnable[MAX_CHANNEL_NUM];
	RtmpServer		m_tRtmpSvr[MAX_CHANNEL_NUM];
	pthread_mutex_t m_tMutex;

	pthread_t		m_output_thread[MAX_CHANNEL_NUM];
	CRTMPStream    *m_ptRtmpStream[MAX_CHANNEL_NUM];

private:
	static RtmpMan* m_pInstance;
};

#endif //__RTMP_MAN_H__


