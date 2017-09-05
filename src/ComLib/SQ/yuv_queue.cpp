#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include "oal_sem.h"
#include "oal_shm.h"
#include "oal_log.h"
#include "include/yuv_queue.h"


#define YUV_QUEUE_SHM_NAME		"/shm_yuv"

#define YUV_QUEUE_NODE_SIZE		(sizeof(Yuv_Data_Header) + YUV_QUEUE_FRAME_MAX)
#define YUV_QUEUE_SHM_SIZE		(YUV_QUEUE_NUM *sizeof(int)*2 + YUV_QUEUE_NUM * YUV_QUEUE_NODE_SIZE)

#define YUV_QUEUE_SEM_MUTEX			"sem_mutex_yuv"
#define YUV_QUEUE_SEM_SYN_SERVICE	"sem_syn_yuv"

#ifdef WIN32
#define OPEN_MODE		0
#define PROT_READ		0x01
#define PROT_WRITE		0x02
#else
#define OPEN_MODE		S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH
#endif // !WIN32


#define SHM_OPEN_FLAG_CREATE	O_RDWR|O_CREAT	//|O_EXCL
#define SHM_OPEN_FLAG_OPEN		O_RDWR

#define SEM_OPEN_FLAG_CREATE	O_RDWR|O_CREAT
#define SEM_OPEN_FLAG_OPEN		0

int Yuv_QueueCreate(Yuv_Queue * ptQueue)
{
	int iRet = -1;
	if (ptQueue == NULL)
	{
		return -1;
	}

	int iChnID = ptQueue->iChnID;

	// 创建共享内存
	char szShmName[64] = {0};
	sprintf(szShmName, "%s_%d", YUV_QUEUE_SHM_NAME, iChnID);
	ptQueue->shm_fd = oal_shm_create(szShmName, SHM_OPEN_FLAG_CREATE, OPEN_MODE);
	if(ptQueue->shm_fd < 0)
	{
		LOG_ERROR_FMT("channel%d oal_shm_create %s failed", iChnID, szShmName);
		return -1;
	}

	// 内存映射
	ptQueue->shm_addr = oal_shm_init(YUV_QUEUE_SHM_SIZE, PROT_READ|PROT_WRITE, ptQueue->shm_fd);
	if(ptQueue->shm_addr == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_shm_init failed", iChnID);
		return -1;
	}

	// 创建内存操作的互斥对象
	char szSemName[64] = {0};
	sprintf(szSemName, "%s_%d", YUV_QUEUE_SEM_MUTEX, iChnID);
	ptQueue->sem_mutex = oal_sem_create(szSemName, SEM_OPEN_FLAG_CREATE, OPEN_MODE, 1);
	if(ptQueue->sem_mutex == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_sem_create failed", iChnID);
		return -1;
	}

	// 创建同步对象
	char szSynName[64] = {0};
	sprintf(szSynName, "%s_%d", YUV_QUEUE_SEM_SYN_SERVICE, iChnID);
	ptQueue->sem_syn = oal_sem_create(szSynName, SEM_OPEN_FLAG_CREATE, OPEN_MODE,0);
	if(ptQueue->sem_syn == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_sem_create sem_syn failed", iChnID);
		return -1;
	}

	ptQueue->pFullFlag  = (int *)ptQueue->shm_addr;
	ptQueue->pEmptyFlag = ptQueue->pFullFlag + YUV_QUEUE_NUM;
	ptQueue->ptHeadNode = (Yuv_Queue_Node *)(ptQueue->pEmptyFlag + YUV_QUEUE_NUM);

	for(int i = 0; i < YUV_QUEUE_NUM; i++)
	{
		*(ptQueue->pFullFlag+i)  = 0;
		*(ptQueue->pEmptyFlag+i) = 1;
	}

	ptQueue->wr_num = -1;
	return 0;
}

Yuv_Queue_Node  *Yuv_QueueGetEmpty(Yuv_Queue * ptQueue)
{
	int iRet = -1;
	if (ptQueue == NULL)
	{
		return NULL;
	}

	Yuv_Queue_Node *ptEmptyNode = NULL;

	oal_sem_wait(ptQueue->sem_mutex);

	// 查找空闲位置
	int iWrPos = -1;
	for(int i = ptQueue->wr_num + 1; i < ptQueue->wr_num + 1 + YUV_QUEUE_NUM; i++)
	{
		int iPos = i;
		if (iPos >= YUV_QUEUE_NUM)
		{
			iPos -= YUV_QUEUE_NUM;
		}

		if(*(ptQueue->pEmptyFlag + iPos))
		{
			iWrPos = iPos;
			break;
		}
	}

	if(iWrPos >= 0)
	{
		ptEmptyNode = ptQueue->ptHeadNode + iWrPos;
		ptQueue->wr_num = iWrPos;
		//printf("---%s wr_num = %d.\n", __FUNCTION__,ptQueue->wr_num);
	}		
	else
	{
#ifdef QUEUE_DEBUG
		printf("---Yuv_QueueGetEmpty do not get empty buf.\n");
#endif
	}

	oal_sem_post(ptQueue->sem_mutex);

	return ptEmptyNode; 
}

int Yuv_QueuePutFull(Yuv_Queue * ptQueue)
{
	if (ptQueue == NULL || ptQueue->wr_num < 0 || ptQueue->wr_num > YUV_QUEUE_NUM-1)
	{
		return -1;
	}

	oal_sem_wait(ptQueue->sem_mutex);
	*(ptQueue->pFullFlag  + ptQueue->wr_num) = 1;
	*(ptQueue->pEmptyFlag + ptQueue->wr_num) = 0;
	//printf("---%s wr_num = %d -> FULL.\n", __FUNCTION__,ptQueue->wr_num);
	oal_sem_post(ptQueue->sem_mutex);

	oal_sem_post(ptQueue->sem_syn);

	return 0;	
}
int Yuv_QueueRelease(Yuv_Queue * ptQueue)
{
	int iRet  = -1;
	if (ptQueue == NULL)
	{
		return -1;
	}
	int iChnID = ptQueue->iChnID;

	iRet = oal_shm_close(ptQueue->shm_addr,YUV_QUEUE_SHM_SIZE,ptQueue->shm_fd);
	if(iRet < 0)
		return -1;

	char szShmName[64] = {0};
	sprintf(szShmName, "%s_%d", YUV_QUEUE_SHM_NAME, iChnID);
	iRet = oal_shm_del(szShmName);
	if(iRet < 0)
		return -1;

	iRet = oal_sem_close(ptQueue->sem_mutex);
	if(iRet < 0)
		return -1;

	iRet = oal_sem_close(ptQueue->sem_syn);
	if(iRet < 0)
		return -1;

	char szSemName[64] = {0};
	sprintf(szSemName, "%s_%d", YUV_QUEUE_SEM_MUTEX, iChnID);
	iRet = oal_sem_del(szSemName);
	if(iRet < 0)
		return -1;

	char szSynName[64] = {0};
	sprintf(szSynName, "%s_%d", YUV_QUEUE_SEM_SYN_SERVICE, iChnID);
	iRet = oal_sem_del(szSynName);

	return iRet;
}

int Yuv_QueueOpen(Yuv_Queue * ptQueue)
{
	int iRet = -1;
	if (ptQueue == NULL)
	{
		return -1;
	}

	int iChnID = ptQueue->iChnID;

	// 打开共享内存
	char szShmName[64] = {0};
	sprintf(szShmName, "%s_%d", YUV_QUEUE_SHM_NAME, iChnID);
	ptQueue->shm_fd = oal_shm_open(szShmName, SHM_OPEN_FLAG_OPEN, OPEN_MODE);
	if(ptQueue->shm_fd < 0)
	{
		LOG_ERROR_FMT("channel%d oal_shm_open %s failed", iChnID, szShmName);
		return -1;
	}

	// 内存映射
	ptQueue->shm_addr = oal_shm_mmap(YUV_QUEUE_SHM_SIZE,PROT_READ | PROT_WRITE,ptQueue->shm_fd);
	if(ptQueue->shm_addr == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_shm_mmap failed", iChnID);
		return -1;
	}

	// 创建内存操作的互斥对象
	char szSemName[64] = {0};
	sprintf(szSemName, "%s_%d", YUV_QUEUE_SEM_MUTEX, iChnID);
	ptQueue->sem_mutex = oal_sem_open(szSemName, SEM_OPEN_FLAG_OPEN);
	if(ptQueue->sem_mutex == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_sem_open failed", iChnID);
		return -1;
	}

	char szSynName[64] = {0};
	sprintf(szSynName, "%s_%d", YUV_QUEUE_SEM_SYN_SERVICE, iChnID);
	ptQueue->sem_syn = oal_sem_open(szSynName, SEM_OPEN_FLAG_OPEN);
	if(ptQueue->sem_syn == NULL)
	{
		LOG_ERROR_FMT("channel%d oal_sem_open %s failed", iChnID,szSynName);
		return -1;
	}

	ptQueue->pFullFlag  = (int *)ptQueue->shm_addr;
	ptQueue->pEmptyFlag = ptQueue->pFullFlag + YUV_QUEUE_NUM;
	ptQueue->ptHeadNode = (Yuv_Queue_Node *)(ptQueue->pEmptyFlag + YUV_QUEUE_NUM);

	ptQueue->rd_num = -1;

	return 0;
}

Yuv_Queue_Node  *Yuv_QueueGetFull(Yuv_Queue * ptQueue)
{
	int iRet = -1;
	if (ptQueue == NULL)
	{
		return NULL;
	}

	Yuv_Queue_Node *ptFullNode = NULL;

	oal_sem_wait(ptQueue->sem_syn);

	oal_sem_wait(ptQueue->sem_mutex);

	int iRdPos = -1;
	for(int i = ptQueue->rd_num + 1; i < ptQueue->rd_num + 1 + YUV_QUEUE_NUM; i++)
	{
		int iPos = i;
		if (iPos >= YUV_QUEUE_NUM)
		{
			iPos -= YUV_QUEUE_NUM;
		}

		if(*(ptQueue->pFullFlag + iPos))
		{
			iRdPos = iPos;
			break;
		}
	}

	if(iRdPos >= 0)
	{
		ptFullNode = ptQueue->ptHeadNode + iRdPos;
		ptQueue->rd_num = iRdPos;
		//printf("---%s rd_num = %d.\n", __FUNCTION__,ptQueue->rd_num);
	}		
	else
	{
#ifdef QUEUE_DEBUG
		printf("---Yuv_QueueGetFull do not get new data.\n");
#endif
	}

	oal_sem_post(ptQueue->sem_mutex);

	return ptFullNode; 
}


int Yuv_QueuePutEmpty(Yuv_Queue * ptQueue)
{
	if (ptQueue == NULL || ptQueue->rd_num < 0 || ptQueue->rd_num > YUV_QUEUE_NUM-1)
	{
		return -1;
	}

	oal_sem_wait(ptQueue->sem_mutex);
	*(ptQueue->pFullFlag  + ptQueue->rd_num) = 0;
	*(ptQueue->pEmptyFlag + ptQueue->rd_num) = 1;
	//printf("---%s rd_num = %d -> EMPTY.\n", __FUNCTION__,ptQueue->rd_num);
	oal_sem_post(ptQueue->sem_mutex);

	return 0;	
}

int Yuv_QueueClose(Yuv_Queue * ptQueue)
{
	int iRet  = -1;

	iRet = oal_shm_close(ptQueue->shm_addr,YUV_QUEUE_SHM_SIZE,ptQueue->shm_fd);
	if(iRet < 0)
		goto end;

	iRet = oal_sem_close(ptQueue->sem_mutex);
	if(iRet < 0)
		goto end;

	iRet = oal_sem_close(ptQueue->sem_syn);

end:	
	return iRet;
}






