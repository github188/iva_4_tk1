#include "RtspMan.h"
#include "H264Man.h"
#include <assert.h>
#include "oal_log.h"
#include "oal_file.h"
#include "oal_time.h"
#include "MediaSink.hh"
#include "BasicUsageEnvironment.hh"
#include "RTSPServer.hh"


//static int s_iChnID[MAX_CHANNEL_NUM];

void * RtspOutputThread(void *arg)
{
	if (arg == NULL)
	{
		return NULL;
	}

	RtspMan * ptMan = (RtspMan*)arg;
	ptMan->Serve();
	return arg;
}

RtspMan* RtspMan::m_pInstance = NULL;

RtspMan* RtspMan::GetInstance()
{
	return m_pInstance;
}

int RtspMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new RtspMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void RtspMan::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

RtspMan::RtspMan()
{
	pthread_mutex_init(&m_tMutex, NULL);
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_bEnable[i] = false;
	}
	m_bExitThread = 0;
}

RtspMan::~RtspMan()
{
	m_bExitThread = 1;
	pthread_join(m_output_thread,NULL);

	pthread_mutex_destroy(&m_tMutex);
}

int RtspMan::Run()
{
	m_bExitThread = 0;

	//OutPacketBuffer::maxSize = MAX_FRAME_SIZES;

	if(pthread_create(&m_output_thread, NULL, RtspOutputThread, this))
	{
		LOG_ERROR_FMT("RtspOutputThread create failed");
	}

	return 0;
}

int RtspMan::SetStrategy( const VoStrategy * ptInfo )
{
	if(ptInfo == NULL)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tStrategy, ptInfo, sizeof(VoStrategy));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int RtspMan::SetEnable( int iChnID, bool bEnable )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	m_bEnable[iChnID] = bEnable;
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

bool RtspMan::CanIDoNow( int iChnID )
{
	bool bEnable = false;
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return false;
	}

	if(m_tStrategy.iProtocol != VO_PROTOCOL_RTSP)
	{
		return false;
	}

	pthread_mutex_lock(&m_tMutex);
	bEnable = m_bEnable[iChnID];
	pthread_mutex_unlock(&m_tMutex);

	return bEnable;
}

int RtspMan::Serve() 
{
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	assert(scheduler);
	UsageEnvironment* env    = BasicUsageEnvironment::createNew(*scheduler);
	assert(env);
	RTSPServer* rtspServer   = RTSPServer::createNew(*env, 554, NULL, 10);
	assert(rtspServer);
	OutPacketBuffer::maxSize = MAX_FRAME_SIZES;

	// 创建所有通道的服务
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		char szStreamName[128] = {0};
		sprintf(szStreamName, "h264/%d",i);

		ServerMediaSession* serverMediaSession = ServerMediaSession::createNew(*env, szStreamName, szStreamName);
		assert(serverMediaSession);
		ServerMediaSubsession* subsession = H264MediaSubssion::createNew(*env, i, True);
		assert(subsession);
		serverMediaSession->addSubsession(subsession);
		
		rtspServer->addServerMediaSession(serverMediaSession);

		char* url = rtspServer->rtspURL(serverMediaSession);
		if (url)
		{
			LOG_DEBUG_FMT("Play Channel%d stream using the URL:%s", i, url);
			delete[] url;
		}
	}

	if (/*rtspServer->setUpTunnelingOverHTTP(80) ||*/ 
		rtspServer->setUpTunnelingOverHTTP(8000) ||
		rtspServer->setUpTunnelingOverHTTP(8080))
	{
		LOG_DEBUG_FMT("We use port %d for optional RTSP-over-HTTP tunneling.",rtspServer->httpServerPortNum())
	}
	else
	{
		LOG_DEBUG_FMT("RTSP-over-HTTP tunneling is not available.");
	}

	env->taskScheduler().doEventLoop(&m_bExitThread); // does not return

	LOG_INFOS_FMT("RtspMan::Serve exit\n");

	return 0; // only to prevent compiler warning
}

