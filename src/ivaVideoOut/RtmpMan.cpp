#include "RtmpMan.h"
#include "H264Man.h"
#include <assert.h>
#include "oal_log.h"
#include "oal_file.h"
#include "oal_time.h"

static int s_iChnID[MAX_CHANNEL_NUM];

void * RtmpOutputThread(void *arg)
{
	if (arg == NULL)
	{
		return NULL;
	}

	int iChnID = *((int *)arg);
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return NULL;
	}

	H264_Queue_Node * ptDataNode = (H264_Queue_Node*)malloc(sizeof(H264_Queue_Node));
	assert(ptDataNode);

	LOG_DEBUG_FMT("Channel%d Output Thread running",iChnID);

	RtmpServer tSvrInfo = {0};

	while(!RtmpMan::GetInstance()->m_bExitThread)
	{
		// 是否可以推流
		if (RtmpMan::GetInstance()->CanIDoNow(iChnID) == false)
		{
			RtmpMan::GetInstance()->DisConnectRtmpSvr(iChnID);
			sleep(1);
			continue;
		}

		// 连接RTMP流媒体服务器
		RtmpServer tNewSvrInfo = {0};
		RtmpMan::GetInstance()->GetServerInfo(iChnID, tNewSvrInfo);
		if (memcmp(&tNewSvrInfo, &tSvrInfo, sizeof(RtmpServer)) != 0)
		{
			memcpy(&tSvrInfo, &tNewSvrInfo, sizeof(RtmpServer));
			RtmpMan::GetInstance()->DisConnectRtmpSvr(iChnID);
			LOG_DEBUG_FMT("RTMP Server -> %s", tSvrInfo.szUrl);
		}

		if (!RtmpMan::GetInstance()->IsConnectedRtmpSvr(iChnID))
		{
			RtmpMan::GetInstance()->ConnectRtmpSvr(iChnID);
		}

		if (!RtmpMan::GetInstance()->IsConnectedRtmpSvr(iChnID))
		{
			sleep(1);
			continue;
		}

		// 取一帧数据
		memset(ptDataNode, 0, sizeof(H264_Queue_Node));
		int iRet = H264Man::GetInstance()->GetNewFrameData(iChnID, ptDataNode);
		if(iRet != 0)
		{
			printf("%s:%d %s  GetNewFrameData = false\n",__FILE__, __LINE__, __FUNCTION__);
			usleep(10*1000);
			continue;
		}

		// 推送数据
		RtmpMan::GetInstance()->SendH264Frame(iChnID, ptDataNode);
	}

	free(ptDataNode);
	ptDataNode = NULL;

	return NULL;
}

RtmpMan* RtmpMan::m_pInstance = NULL;

RtmpMan* RtmpMan::GetInstance()
{
	return m_pInstance;
}

int RtmpMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new RtmpMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void RtmpMan::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

RtmpMan::RtmpMan()
{
	pthread_mutex_init(&m_tMutex, NULL);
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_bEnable[i] = false;
		memset(m_tRtmpSvr+i, 0, sizeof(RtmpServer));

		m_ptRtmpStream[i] = NULL;
	}
	m_bExitThread = false;
}

RtmpMan::~RtmpMan()
{
	m_bExitThread = true;

	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		pthread_join(m_output_thread[i],NULL);

		if (m_ptRtmpStream[i])
		{
			delete m_ptRtmpStream[i];
			m_ptRtmpStream[i] = NULL;
		}
	}
	pthread_mutex_destroy(&m_tMutex);
}

int RtmpMan::Run()
{
	m_bExitThread = false;
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		s_iChnID[i] = i;

		if(pthread_create(&m_output_thread[i], NULL, RtmpOutputThread, s_iChnID+i))
		{
			LOG_ERROR_FMT("Channel%d RtmpOutputThread create failed", i);
		}
	}

	return 0;
}

int RtmpMan::SetStrategy( const VoStrategy * ptInfo )
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

int RtmpMan::SetServerInfo( int iChnID, const RtmpServer * ptInfo )
{
	if(ptInfo == NULL || iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(m_tRtmpSvr+iChnID, ptInfo, sizeof(RtmpServer));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int RtmpMan::GetServerInfo( int iChnID, RtmpServer & tInfo )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&tInfo, m_tRtmpSvr+iChnID, sizeof(RtmpServer));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int RtmpMan::SetEnable( int iChnID, bool bEnable )
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

int RtmpMan::Reset( int iChnID )
{
	DisConnectRtmpSvr(iChnID);
	return 0;
}

bool RtmpMan::CanIDoNow( int iChnID )
{
	bool bEnable = false;
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return false;
	}

	if(m_tStrategy.iProtocol != VO_PROTOCOL_RTMP)
	{
		return false;
	}

	if (strlen(m_tRtmpSvr[iChnID].szUrl) < 1)
	{
		return false;
	}

	pthread_mutex_lock(&m_tMutex);
	bEnable = m_bEnable[iChnID];
	pthread_mutex_unlock(&m_tMutex);

	return bEnable;
}

void RtmpMan::ConnectRtmpSvr( int iChnID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return;
	}

	if (m_ptRtmpStream[iChnID])
	{
		LOG_ERROR_FMT("Channel%d Rtmp Stream is not null", iChnID);
		return;
	}

	m_ptRtmpStream[iChnID] = new CRTMPStream;
	if (m_ptRtmpStream[iChnID] == NULL)
	{
		LOG_ERROR_FMT("Channel%d new Rtmp Stream failed", iChnID);
		return;
	}

	if ( !m_ptRtmpStream[iChnID]->Connect(m_tRtmpSvr[iChnID].szUrl))
	{
		delete m_ptRtmpStream[iChnID];
		m_ptRtmpStream[iChnID] = NULL;
	}
}

void RtmpMan::DisConnectRtmpSvr( int iChnID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return;
	}

	if (m_ptRtmpStream[iChnID])
	{
		delete m_ptRtmpStream[iChnID];
		m_ptRtmpStream[iChnID] = NULL;
	}
}

bool RtmpMan::IsConnectedRtmpSvr( int iChnID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return false;
	}

	if (m_ptRtmpStream[iChnID] == NULL)
	{
		return false;
	}

	return m_ptRtmpStream[iChnID]->IsConnected();
}

int RtmpMan::SendH264Frame( int iChnID, H264_Queue_Node * ptNode )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if (m_ptRtmpStream[iChnID] == NULL)
	{
		return -1;
	}

	NaluUnit tNalu;
	tNalu.type = ((*(ptNode->frame_data+4))&0x1F);
	tNalu.size = ptNode->h264_info.data_size-4;
	tNalu.data = ptNode->frame_data+4;
	return m_ptRtmpStream[iChnID]->SendH264Frame(tNalu);
}

