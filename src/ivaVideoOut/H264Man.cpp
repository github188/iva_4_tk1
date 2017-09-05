#include "H264Man.h"
#include <assert.h>
#include "oal_log.h"
#include "oal_file.h"
#include "oal_time.h"

H264Man* H264Man::m_pInstance = NULL;

H264Man* H264Man::GetInstance()
{
	return m_pInstance;
}

int H264Man::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new H264Man();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void H264Man::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

H264Man::H264Man()
{
}

H264Man::~H264Man()
{
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		H264_QueueClose(m_tH264SQ+i);
	}
}

int H264Man::Run()
{
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		// 打开共享队列
		m_tH264SQ[i].iChnID = i;
		m_tH264SQ[i].service_type = H264_QUEUE_STREAM_SERVICE;
		int iRet = H264_QueueOpen(&m_tH264SQ[i]);
		if (iRet != 0)
		{
			LOG_ERROR_FMT("Channel%d H264_QueueOpen failed", i);
		}
		LOG_DEBUG_FMT("Channel%d H264_QueueOpen OK", i);
	}

	return 0;
}

int H264Man::GetNewFrameData( int iChnID, H264_Queue_Node * ptNode )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM || ptNode == NULL)
	{
		return -1;
	}

	return H264_QueueGetFull(m_tH264SQ+iChnID, &(ptNode->h264_info), ptNode->frame_data);
}

int H264Man::GetNewFrameData2( int iChnID, H264_Data_Header &h264_info, unsigned char * frame_data )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM || frame_data == NULL)
	{
		return -1;
	}

	return H264_QueueGetFull(m_tH264SQ+iChnID, &h264_info, frame_data);

}
