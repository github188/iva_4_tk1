#ifndef __RTSP_MAN_H__
#define __RTSP_MAN_H__

#include "capacity.h"
#include "h264_queue.h"
#include "pthread.h"
#include "mq_videoout.h"
#include "RTSPStream.h"

class RtspMan
{
public:
	static RtspMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	RtspMan();
	~RtspMan();

public:
	int Run();

	int SetStrategy(const VoStrategy * ptInfo);
	int SetEnable(int iChnID, bool bEnable);

	bool CanIDoNow(int iChnID);

	int Serve();
public:
	char m_bExitThread;

private:
	VoStrategy      m_tStrategy;
	bool			m_bEnable[MAX_CHANNEL_NUM];
	pthread_mutex_t m_tMutex;

	pthread_t		m_output_thread;

private:
	static RtspMan* m_pInstance;
};

#endif //__RTSP_MAN_H__


