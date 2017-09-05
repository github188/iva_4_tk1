#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "oal_sem.h"
#include "oal_shm.h"
#include "oal_log.h"

#include "include/h264_queue.h"

#define H264_QUEUE_SHM_NAME		"/shm_h264"
#define H264_QUEUE_SHM_SIZE		sizeof(unsigned int) + H264_QUEUE_NUM*(sizeof(H264_Data_Header) + H264_QUEUE_FRAME_MAX)

#define H264_QUEUE_SEM_MUTEX				"sem_mutex_h264"

#define H264_QUEUE_SEM_SYN_DECODE_SERVICE	"sem_syn_h264_decode"
#define H264_QUEUE_SEM_SYN_RECORD_SERVICE	"sem_syn_h264_record"
#define H264_QUEUE_SEM_SYN_EVIDENCE_SERVICE	"sem_syn_h264_evidence"
#define H264_QUEUE_SEM_SYN_STREAM_SERVICE	"sem_syn_h264_stream"

#ifdef WIN32
#define OPEN_MODE		0
#define PROT_READ		0x01
#define PROT_WRITE		0x02
#else
#define OPEN_MODE		S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH
#endif

#define SHM_OPEN_FLAG_CREATE	O_RDWR|O_CREAT	//|O_EXCL
#define SHM_OPEN_FLAG_OPEN 		O_RDWR

#define SEM_OPEN_FLAG_CREATE	O_CREAT
#define SEM_OPEN_FLAG_OPEN		0

#define QUEUE_START_CODE		1

SQ_API int H264_QueueCreate(H264_Queue * ptQueue)
{
	if (ptQueue == NULL)
	{
		return -1;
	}
	int iChnID = ptQueue->iChnID;

	// 创建共享内存
	char szShmName[64] = {0};
	sprintf(szShmName, "%s_%d", H264_QUEUE_SHM_NAME, iChnID);
	ptQueue->shm_fd = oal_shm_create(szShmName, SHM_OPEN_FLAG_CREATE, OPEN_MODE);
	if(ptQueue->shm_fd < 0)
	{
		LOG_ERROR_FMT("channel%d oal_shm_create %s failed", iChnID, szShmName);
		return -1;
	}

	// 内存映射
	ptQueue->shm_addr = oal_shm_init(H264_QUEUE_SHM_SIZE, PROT_READ|PROT_WRITE, ptQueue->shm_fd);
	if(ptQueue->shm_addr == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_shm_init failed", iChnID);
		return -1;
	}
	ptQueue->queue = ptQueue->shm_addr;

	// 创建内存操作的互斥对象
	char szSemName[64] = {0};
	sprintf(szSemName, "%s_%d", H264_QUEUE_SEM_MUTEX, iChnID);
	ptQueue->sem_mutex = oal_sem_create(szSemName, SEM_OPEN_FLAG_CREATE, OPEN_MODE, 1);
	if(ptQueue->sem_mutex == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_sem_create failed", iChnID);
		return -1;
	}

	// 创建同步对象
	char szSynDecodeName[64] = {0};
	sprintf(szSynDecodeName, "%s_%d", H264_QUEUE_SEM_SYN_DECODE_SERVICE, iChnID);
	ptQueue->sem_syn_decode = oal_sem_create(szSynDecodeName, SEM_OPEN_FLAG_CREATE, OPEN_MODE,0);
	if(ptQueue->sem_syn_decode == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_sem_create sem_syn_decode failed", iChnID);
		return -1;
	}

	char szSynRecordName[64] = {0};
	sprintf(szSynRecordName, "%s_%d", H264_QUEUE_SEM_SYN_RECORD_SERVICE, iChnID);
	ptQueue->sem_syn_record = oal_sem_create(szSynRecordName, SEM_OPEN_FLAG_CREATE, OPEN_MODE,0);
	if(ptQueue->sem_syn_record == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_sem_create sem_syn_record failed", iChnID);
		return -1;
	}

	char szSynEvidenceName[64] = {0};
	sprintf(szSynEvidenceName, "%s_%d", H264_QUEUE_SEM_SYN_EVIDENCE_SERVICE, iChnID);
	ptQueue->sem_syn_evidence = oal_sem_create(szSynEvidenceName, SEM_OPEN_FLAG_CREATE, OPEN_MODE,0);
	if(ptQueue->sem_syn_evidence == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_sem_create sem_syn_evidence failed", iChnID);
		return -1;
	}

	char szSynStreamName[64] = {0};
	sprintf(szSynStreamName, "%s_%d", H264_QUEUE_SEM_SYN_STREAM_SERVICE, iChnID);
	ptQueue->sem_syn_stream = oal_sem_create(szSynStreamName, SEM_OPEN_FLAG_CREATE, OPEN_MODE,0);
	if(ptQueue->sem_syn_stream == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_sem_create sem_syn_stream failed", iChnID);
		return -1;
	}


	// 下一个写入位置 初始化为第0个
	ptQueue->wr_pos = 0;

	// 还没有写入I帧，先赋值为0
	*((unsigned int *)ptQueue->queue) = 0;

	return 0;
}

SQ_API int H264_QueuePutFull(H264_Queue * ptQueue, const H264_Data_Header *ptHeader, const unsigned char * pFrameData)
{
	if (ptQueue == NULL || ptHeader == NULL || pFrameData == NULL)
	{
		return -1;
	}

	unsigned int *pIFramePos = (unsigned int *)ptQueue->queue;
	H264_Queue_Node * ptNodeHead = (H264_Queue_Node *)(pIFramePos + 1);

	oal_sem_wait(ptQueue->sem_mutex);

	// 得到要写入的节点位置
	if(ptQueue->wr_pos >= H264_QUEUE_NUM)	
		ptQueue->wr_pos = 0;

	H264_Queue_Node * ptWriteNode = ptNodeHead + ptQueue->wr_pos;

	// 写入帧头信息
	memcpy(&ptWriteNode->h264_info, ptHeader, sizeof(H264_Data_Header));

	// 如果是I帧 记录位置
	if(ptHeader->frame_type == I_FRAME)
		*pIFramePos = ptQueue->wr_pos;

	// 写入帧数据
	unsigned char * pData = ptWriteNode->frame_data;

#ifdef QUEUE_START_CODE	
	unsigned char start_code[4] = {0x00, 0x00, 0x00, 0x01};
	memcpy(pData, start_code, sizeof(start_code));
	ptWriteNode->h264_info.data_size += sizeof(start_code);
	pData += sizeof(start_code);
#endif

	memcpy(pData, pFrameData, ptHeader->data_size);

	// 要写入的节点位置向后移
	ptQueue->wr_pos++;

	oal_sem_post(ptQueue->sem_mutex);

	// 消费者们可以取数据了
	oal_sem_post(ptQueue->sem_syn_decode);
	oal_sem_post(ptQueue->sem_syn_record);
	oal_sem_post(ptQueue->sem_syn_evidence);
	oal_sem_post(ptQueue->sem_syn_stream);

	return 0;	
}

SQ_API int H264_QueueRelease(H264_Queue * ptQueue)
{
	int iRet  = -1;
	if (ptQueue == NULL)
	{
		return -1;
	}
	int iChnID = ptQueue->iChnID;

	iRet = oal_shm_close(ptQueue->shm_addr, H264_QUEUE_SHM_SIZE, ptQueue->shm_fd);
	if(iRet < 0)
		return -1;

	char szShmName[64] = {0};
	sprintf(szShmName, "%s_%d", H264_QUEUE_SHM_NAME, iChnID);
	iRet = oal_shm_del(szShmName);
	if(iRet < 0)
		return -1;

	iRet = oal_sem_close(ptQueue->sem_mutex);
	if(iRet < 0)
		return -1;

	iRet = oal_sem_close(ptQueue->sem_syn_decode);
	if(iRet < 0)
		return -1;

	iRet = oal_sem_close(ptQueue->sem_syn_record);
	if(iRet < 0)
		return -1;

	iRet = oal_sem_close(ptQueue->sem_syn_evidence);
	if(iRet < 0)
		return -1;

	iRet = oal_sem_close(ptQueue->sem_syn_stream);
	if(iRet < 0)
		return -1;

	char szSemName[64] = {0};
	sprintf(szSemName, "%s_%d", H264_QUEUE_SEM_MUTEX, iChnID);
	iRet = oal_sem_del(szSemName);
	if(iRet < 0)
		return -1;

	char szSynDecodeName[64] = {0};
	sprintf(szSynDecodeName, "%s_%d", H264_QUEUE_SEM_SYN_DECODE_SERVICE, iChnID);
	iRet = oal_sem_del(szSynDecodeName);
	if(iRet < 0)
		return -1;

	char szSynRecordName[64] = {0};
	sprintf(szSynRecordName, "%s_%d", H264_QUEUE_SEM_SYN_RECORD_SERVICE, iChnID);
	iRet = oal_sem_del(szSynRecordName);
	if(iRet < 0)
		return -1;

	char szSynEvidenceName[64] = {0};
	sprintf(szSynEvidenceName, "%s_%d", H264_QUEUE_SEM_SYN_EVIDENCE_SERVICE, iChnID);
	iRet = oal_sem_del(szSynEvidenceName);
	if(iRet < 0)
		return -1;

	char szSynStreamName[64] = {0};
	sprintf(szSynStreamName, "%s_%d", H264_QUEUE_SEM_SYN_STREAM_SERVICE, iChnID);
	iRet = oal_sem_del(szSynStreamName);
	return iRet;
}

SQ_API int H264_QueueOpen(H264_Queue * ptQueue)
{
	if (ptQueue == NULL)
	{
		return -1;
	}

	int iChnID = ptQueue->iChnID;

	// 打开共享内存
	char szShmName[64] = {0};
	sprintf(szShmName, "%s_%d", H264_QUEUE_SHM_NAME, iChnID);
	ptQueue->shm_fd = oal_shm_open(szShmName, SHM_OPEN_FLAG_OPEN, OPEN_MODE);
	if(ptQueue->shm_fd < 0)
	{
		LOG_ERROR_FMT("channel%d oal_shm_open %s failed", iChnID, szShmName);
		return -1;
	}

	// 内存映射
	ptQueue->shm_addr = oal_shm_mmap(H264_QUEUE_SHM_SIZE,PROT_READ | PROT_WRITE,ptQueue->shm_fd);
	if(ptQueue->shm_addr == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_shm_mmap failed", iChnID);
		return -1;
	}
	ptQueue->queue = ptQueue->shm_addr;

	// 创建内存操作的互斥对象
	char szSemName[64] = {0};
	sprintf(szSemName, "%s_%d", H264_QUEUE_SEM_MUTEX, iChnID);
	ptQueue->sem_mutex = oal_sem_open(szSemName, SEM_OPEN_FLAG_OPEN);
	if(ptQueue->sem_mutex == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_sem_open failed", iChnID);
		return -1;
	}

	char szSynName[64] = {0};

	switch(ptQueue->service_type)
	{
	case H264_QUEUE_DECODE_SERVICE:
		{
			sprintf(szSynName, "%s_%d", H264_QUEUE_SEM_SYN_DECODE_SERVICE, iChnID);
			break;
		}
	case H264_QUEUE_RECORD_SERVICE:
		{
			sprintf(szSynName, "%s_%d", H264_QUEUE_SEM_SYN_RECORD_SERVICE, iChnID);
			break;
		}
	case H264_QUEUE_EVIDENCE_SERVICE:
		{
			sprintf(szSynName, "%s_%d", H264_QUEUE_SEM_SYN_EVIDENCE_SERVICE, iChnID);
			break;		
		}
	case H264_QUEUE_STREAM_SERVICE:
		{
			sprintf(szSynName, "%s_%d", H264_QUEUE_SEM_SYN_STREAM_SERVICE, iChnID);
			break;		
		}
	default:
		{
			LOG_ERROR_FMT("channel%d not support service type =%d", iChnID, ptQueue->service_type);
			return -1;
			break;
		}
	}
	ptQueue->sem_syn = oal_sem_open(szSynName, SEM_OPEN_FLAG_OPEN);
	if(ptQueue->sem_syn == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_sem_open %s failed", iChnID,szSynName);
		return -1;
	}

	ptQueue->rd_flag = 0;
	ptQueue->rd_frameid = 0;
	return 0;
}

SQ_API int H264_QueueGetFull(H264_Queue * ptQueue, H264_Data_Header *ptHeader, unsigned char * pFrameData)
{
	int iRet = -1;
	if (ptQueue == NULL || ptHeader == NULL || pFrameData == NULL)
	{
		return -1;
	}

	unsigned int *pIFramePos = (unsigned int *)ptQueue->queue;
	H264_Queue_Node * ptNodeHead = (H264_Queue_Node *)(pIFramePos + 1);

	H264_Queue_Node * ptReadNode = NULL;

	oal_sem_wait(ptQueue->sem_syn);

	oal_sem_wait(ptQueue->sem_mutex);

	if(ptQueue->rd_flag)
	{
		// 读取帧号=上次的帧号+1
		unsigned int rd_frameid = ptQueue->rd_frameid + 1;

		// 根据帧号查找数据
		for(int i = 0; i < H264_QUEUE_NUM; i++)
		{
			H264_Queue_Node *ptNode = ptNodeHead + i;

			if(ptNode->h264_info.frame_id == rd_frameid)
			{
				ptReadNode = ptNode;
				break;
			}
		}
	}

	// 没有找到对应帧号的数据 直接取最后一个I帧
	if(ptReadNode == NULL)
	{
		ptReadNode = ptNodeHead + *pIFramePos;

		// 如果这帧不是I帧 或者比需要的帧号还小（以前的数据）就不取了
		if((ptReadNode->h264_info.frame_type != I_FRAME)||(ptReadNode->h264_info.frame_id < ptQueue->rd_frameid) )
		{
			ptReadNode = NULL;
		}
	}

	// 读出数据
	if(ptReadNode)
	{	
		memcpy(ptHeader, &ptReadNode->h264_info, sizeof(H264_Data_Header));
		memcpy(pFrameData, ptReadNode->frame_data, ptReadNode->h264_info.data_size); 

		// 记录本次读取的帧号
		ptQueue->rd_frameid = ptReadNode->h264_info.frame_id;

		// 表示已经读取到了数据
		if(ptQueue->rd_flag == 0)
			ptQueue->rd_flag = 1;

		iRet = 0;
	}
	else
	{
#ifdef QUEUE_DEBUG	
		LOG_DEBUG_FMT("No more new frame data.");
#endif
	}

	oal_sem_post(ptQueue->sem_mutex);

	return iRet;	
}

SQ_API int H264_QueueClose(H264_Queue * ptQueue)
{
	int iRet  = -1;
	if (ptQueue == NULL)
	{
		return -1;
	}

	iRet = oal_shm_close(ptQueue->shm_addr, H264_QUEUE_SHM_SIZE, ptQueue->shm_fd);
	if(iRet < 0)
		goto end;

	iRet = oal_sem_close(ptQueue->sem_mutex);
	if(iRet < 0)
		goto end;

	iRet = oal_sem_close(ptQueue->sem_syn);
	if(iRet < 0)
		goto end;

	ptQueue->rd_flag = 0;
	ptQueue->rd_frameid = 0;
	iRet = 0;
end:	
	return iRet;
}




