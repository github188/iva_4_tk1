#include <sys/stat.h>
#include <sys/types.h>  
#include <string.h>
#include <assert.h>
#include "StreamMan.h"
#include "librtspsource.h"
#include "oal_log.h"
#include "oal_time.h"
#include "mq_master.h"
#include "mq_decode.h"

const int g_iChnID[MAX_CHANNEL_NUM] = {0};// 用来标识回调的

void StreamCallback(int iStreamType, unsigned char* framedata, unsigned len, void* context, FrameInfo *info)
{
	int iChnID =  -1;

	if (context)
	{
		iChnID = *((int *)context);
	}

	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return;
	}

	static int s_frame_id[MAX_CHANNEL_NUM] = {0};
    
	// 视频数据
	if(2 == iStreamType)
    {
		if(len >= H264_QUEUE_FRAME_MAX)
		{
			printf(" ==========Frame is too big %d > %d============\n", len, H264_QUEUE_FRAME_MAX);
			return;
		}

		int width = 0, height = 0, fps = 25;
		StreamMan::GetInstance()->GetChnSolution(iChnID, width, height, fps);

		H264_Data_Header h264_info = {0};

		h264_info.frame_type = ((*framedata & 0x1F) == 0x05)?1:0;
		h264_info.timestamp  = info->pts;
		h264_info.frame_id   = s_frame_id[iChnID]++;
		h264_info.data_size  = len;
		h264_info.channel    = iChnID;
		h264_info.frame_rate = fps;
		h264_info.width      = width;
		h264_info.height     = height;

		H264_QueuePutFull(&(StreamMan::GetInstance()->m_tH264SQ[iChnID]), &h264_info, framedata);
		
		StreamMan::GetInstance()->RecordFrameInfo(&h264_info);
    }
}

void DisconnectCallback(void * context)
{
	if (context)
	{
		int iChnID = *((int *)context);
		if(iChnID >= 0 || iChnID < MAX_CHANNEL_NUM)
		{
			LOG_WARNS_FMT("Channel%d stream disconnected", iChnID);
			StreamMan::GetInstance()->m_bDisconnected[iChnID] = true;
		}
	}
}

static void * StreamManageThread(void * p)
{
	static RtspInfo sOldRtsp[MAX_CHANNEL_NUM] = {0};
	
	while(StreamMan::GetInstance()->m_bExitThread == false)
	{
		sleep(1);

		for (int i = 0; i < MAX_CHANNEL_NUM; i++)
		{
			RtspInfo tNewRtsp = {0};
			StreamMan::GetInstance()->GetChnRtspInfo(i, tNewRtsp);
			
			// 如果视频源配置改变,断掉原来的流
			if(memcmp(&sOldRtsp[i], &tNewRtsp, sizeof(RtspInfo)) != 0)
			{
				LOG_INFOS_FMT("Video RTSP Change ...");
				LOG_INFOS_FMT("%d * %d URL:%s",tNewRtsp.iWidth,tNewRtsp.iHeight,tNewRtsp.szUrl);
				memcpy(&sOldRtsp[i], &tNewRtsp,sizeof(RtspInfo));

				// 关闭原来的流
				StreamMan::GetInstance()->Stream_Close(i);

				sleep(1);
			}

			if (strlen(sOldRtsp[i].szUrl) > 0)
			{
				// 如果视频流断了，重连
				if(StreamMan::GetInstance()->m_bDisconnected[i])
				{
					StreamMan::GetInstance()->m_bDisconnected[i] = false;
					StreamMan::GetInstance()->Stream_Close(i);
					sleep(1);
				}

				if(StreamMan::GetInstance()->m_pVSource[i] == NULL)
				{
					int ret = StreamMan::GetInstance()->Stream_Init(i);
					if(ret == -2)
					{
						sleep(5);// 增加休眠5秒
					}
				}
			}
			else
			{
				// 视频源没有有效地址，就要关闭原来的流
				StreamMan::GetInstance()->Stream_Close(i);
			}
			sleep(1);
		}
	}
	pthread_exit(p);
	return p;
}

static void * FrameCountThread(void * /*p*/)
{
	time_t tBegin = time(NULL);
	while(StreamMan::GetInstance()->m_bExitThread == false)
	{
		sleep(1);

		if (time(NULL) - tBegin >= 60)
		{
			for (int i = 0; i < MAX_CHANNEL_NUM; i++)
			{
				StreamMan::GetInstance()->PrintfCountInfo(i);
			}
			tBegin = time(NULL);
		}
	}

	pthread_exit(NULL);
	return NULL;
}

StreamMan* StreamMan::m_pInstance = NULL;

StreamMan* StreamMan::GetInstance()
{
	return m_pInstance;
}

int StreamMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new StreamMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void StreamMan::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

StreamMan::StreamMan()
{
	pthread_mutex_init(&m_tMutex, NULL);

	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_pVSource[i] = NULL;
		m_bDisconnected[i] = false;
		memset(m_tRtspInfo+i,0,sizeof(RtspInfo));
		memset(m_tFrameCount+i, 0, sizeof(FramePrintInfo));
	}
	m_bExitThread = false;
}

StreamMan::~StreamMan()
{
	m_bExitThread = true;
	pthread_join(m_manage_thread,NULL);
	pthread_join(m_count_thread,NULL);

	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		Stream_Close(i);

		H264_QueueRelease(m_tH264SQ+i);
	}
	pthread_mutex_destroy(&m_tMutex);
}

int StreamMan::Run()
{
	m_bExitThread = false;
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_tH264SQ[i].iChnID = i;
		int iRet = H264_QueueCreate(m_tH264SQ+i);
		if (iRet != 0)
		{
			LOG_ERROR_FMT("Channel%d H264 SQueue create failed");
		}
	}

	pthread_create(&m_manage_thread, NULL, StreamManageThread, this);
	pthread_create(&m_count_thread, NULL, FrameCountThread, this);
	
	return 0;
}

int StreamMan::SetChnRtspInfo( int iChnID, const RtspInfo* ptInfo )
{
	if(ptInfo == NULL || iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(m_tRtspInfo+iChnID, ptInfo, sizeof(RtspInfo));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int StreamMan::GetChnRtspInfo( int iChnID, RtspInfo & tInfo )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&tInfo, m_tRtspInfo+iChnID, sizeof(RtspInfo));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int StreamMan::Stream_Init(int iChnID)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_pVSource[iChnID])
	{
		return -1;// 已经有流了 直接返回
	}

	RtspInfo tRtsp = {0};
	GetChnRtspInfo(iChnID, tRtsp);
	if(strlen(tRtsp.szUrl) < 1)
	{
		return -1;// 没有地址 直接返回
	}

	// RTSP
	void *source = rtsp_source_new(StreamCallback, DisconnectCallback, (void *)(g_iChnID+iChnID));
	if(source == NULL)
	{
		printf("rtsp_source_new error!\n");
		return -1;
	}
	
	int iRet = -1;
    rtsp_source_open(source, tRtsp.szUrl, 8000);
	if(rtsp_source_is_opened(source))
	{
		iRet = rtsp_source_play(source);
	}
	else
	{
		iRet = -1;
	}

	if(iRet == -1)
	{
		LOG_ERROR_FMT("Channel%d Stream Init error.", iChnID);
		rtsp_source_close(source);
		rtsp_source_free(source);
		source = NULL;
	}
	else
	{
		LOG_INFOS_FMT("Channel%d Stream Init ok. url:%s", iChnID, tRtsp.szUrl);
		int w, h, fps = 25;
		rtsp_source_get_resolution(source, w, h, fps);
		if (w != m_tRtspInfo[iChnID].iWidth || 
			h != m_tRtspInfo[iChnID].iHeight ||
			fps != m_tRtspInfo[iChnID].iFps)
		{
			LOG_INFOS_FMT("Channel%d Solution:%d*%d->%d*%d FPS:%d", iChnID, 
				m_tRtspInfo[iChnID].iWidth,m_tRtspInfo[iChnID].iHeight,
				w, h, fps);

			m_tRtspInfo[iChnID].iWidth  = w;
			m_tRtspInfo[iChnID].iHeight = h;
			m_tRtspInfo[iChnID].iFps = fps;
		}

		// 通知MASTER状态
		MQ_Master_SetVIStatus(iChnID, VIS_NORMAL);

		// 通知DECODE状态
		DecodeVideoInfo tDecInfo;
		tDecInfo.iFrameRate = fps;
		tDecInfo.iWidth  = w;
		tDecInfo.iHeight = h;
		MQ_Decode_VideoInfo(iChnID, &tDecInfo);
	}

	pthread_mutex_lock(&m_tMutex);
	m_pVSource[iChnID] = source;
	pthread_mutex_unlock(&m_tMutex);

	return iRet;
}

int StreamMan::Stream_Close(int iChnID)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if(m_pVSource[iChnID] == NULL)
	{
		return -1;// 已经没有有流了 直接返回
	}

	pthread_mutex_lock(&m_tMutex);
	if(m_pVSource[iChnID])
	{
		rtsp_source_close(m_pVSource[iChnID]);
		rtsp_source_free(m_pVSource[iChnID]);
		m_pVSource[iChnID] = NULL;
	}
	pthread_mutex_unlock(&m_tMutex);

	LOG_INFOS_FMT("Channel%d Stream Close ok.", iChnID);

	// 通知MASTER状态
	MQ_Master_SetVIStatus(iChnID, VIS_ABNORMAL);

	return 0;
}

int StreamMan::GetChnSolution( int iChnID, int & width, int & height, int & fps )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return -1;
	}

	width  = m_tRtspInfo[iChnID].iWidth;
	height = m_tRtspInfo[iChnID].iHeight;
	fps    = m_tRtspInfo[iChnID].iFps;
	return 0;
}

int StreamMan::PrintfCountInfo( int iChnID )
{
	if (m_tFrameCount[iChnID].uFrameCnt==0)
	{
		return -1;
	}

	gettimeofday(&m_tFrameCount[iChnID].tEndTime,NULL);

	long timeuse = (1000*m_tFrameCount[iChnID].tEndTime.tv_sec+ m_tFrameCount[iChnID].tEndTime.tv_usec/1000)
		- (1000*m_tFrameCount[iChnID].tStartTime.tv_sec+ m_tFrameCount[iChnID].tStartTime.tv_usec/1000);
	if (timeuse == 0)
	{
		return -1;
	}

	LOG_DEBUG_FMT("==========Channel %d Frame Info============", iChnID);
	LOG_DEBUG_FMT("Frame Count:%u | IFrame Count:%u | FPS:%u",
		m_tFrameCount[iChnID].uFrameCnt, m_tFrameCount[iChnID].uIFrameCnt,
		m_tFrameCount[iChnID].uFrameCnt/(timeuse/1000));
	LOG_DEBUG_FMT("TotalTime:%ld(s) | AvgTime:%ld(ms)",
		timeuse/1000, timeuse/m_tFrameCount[iChnID].uFrameCnt);
	LOG_DEBUG_FMT("MaxSize:%u | AvgSize:%u\n",
		m_tFrameCount[iChnID].uMaxFrameSize, m_tFrameCount[iChnID].uAvgFrameSize);

	m_tFrameCount[iChnID].uAvgFrameSize = 0;
	m_tFrameCount[iChnID].uFrameCnt = 0;
	m_tFrameCount[iChnID].uIFrameCnt = 0;

	return 0;
}

int StreamMan::RecordFrameInfo( const H264_Data_Header *ptHeader )
{
	if (ptHeader == NULL || ptHeader->channel <0 || ptHeader->channel>MAX_CHANNEL_NUM-1)
	{
		return -1;
	}
	int iChnID = ptHeader->channel;

	if(m_tFrameCount[iChnID].uFrameCnt == 0)
	{
		gettimeofday(&m_tFrameCount[iChnID].tStartTime,NULL);
	}

	m_tFrameCount[iChnID].uAvgFrameSize = 
		(m_tFrameCount[iChnID].uAvgFrameSize*m_tFrameCount[iChnID].uFrameCnt + ptHeader->data_size)/(m_tFrameCount[iChnID].uFrameCnt+1);

	m_tFrameCount[iChnID].uFrameCnt++;
	if (ptHeader->frame_type == I_FRAME)
	{
		m_tFrameCount[iChnID].uIFrameCnt++;
	}
	if (ptHeader->data_size > m_tFrameCount[iChnID].uMaxFrameSize)
	{
		m_tFrameCount[iChnID].uMaxFrameSize = ptHeader->data_size;
	}
	return 0;
}

