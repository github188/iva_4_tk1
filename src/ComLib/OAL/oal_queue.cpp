/***************************************************************
模块 ： 缓存队列                       
文件 ： oal_queue.cpp
作者 ： Ripple Lee(libozjw@163.com)
版本 ： V1.0
日期 ： 2017-01-18
*****************************************************************/
#include "include/oal_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 队列句柄 */
typedef struct _TQueue
{
	u32 curRd;      /* 当前队列读索引值   */
	u32 curWr;      /* 当前队列写索引值   */
	s32 maxlen;     /* 当前队列最大节点数 */
	s32 count;      /* 当前队列节点数量   */

	s32 *queue;     /* 当前队列节点列表   */

	pthread_mutex_t lock;   /* 当前队列互斥锁   */
	pthread_cond_t  condRd; /* 当前队列读信号量 */
	pthread_cond_t  condWr; /* 当前队列写信号量 */

} TQueue;

OAL_API TQueHndl QueCreate(u32 maxLen)
{
	TQueue * hndl = (TQueue *)malloc(sizeof(TQueue));
	if(hndl == NULL)
	{
		return NULL;
	}

	hndl->curRd = 0;
	hndl->curWr = 0;
	hndl->count = 0;
	hndl->maxlen= maxLen;
	hndl->queue = (s32 *)malloc(sizeof(s32)*hndl->maxlen);

	if(hndl->queue==NULL) 
	{
		hndl->maxlen = 0;
		free(hndl);
		return NULL;
	}

	pthread_mutex_init(&hndl->lock, NULL);
	pthread_cond_init(&hndl->condRd, NULL);
	pthread_cond_init(&hndl->condWr, NULL);
	return hndl;
}

OAL_API int QueDelete(TQueHndl hndl)
{
	TQueue * pQueue = (TQueue*)hndl;
	if(hndl == NULL)
	{
		return FAILURE;
	}

	pthread_mutex_lock(&pQueue->lock);
	if(pQueue->queue != NULL)
	{
		free(pQueue->queue);
	}
	pthread_mutex_unlock(&pQueue->lock);

	pthread_cond_destroy(&pQueue->condRd);
	pthread_cond_destroy(&pQueue->condWr);
	pthread_mutex_destroy(&pQueue->lock);

	return SUCCESS;
}

OAL_API int QueSize( TQueHndl hndl )
{
	TQueue * pQueue = (TQueue*)hndl;
	if(hndl == NULL)
	{
		return FAILURE;
	}

	return pQueue->count;
}

OAL_API int QueEmpty(TQueHndl hndl)
{
	TQueue * pQueue = (TQueue*)hndl;
	if(hndl == NULL)
	{
		return FAILURE;
	}

	pthread_mutex_lock(&pQueue->lock);
	pQueue->count = 0;
	pthread_mutex_unlock(&pQueue->lock);
	return SUCCESS;
}

OAL_API int QuePush(TQueHndl hndl, void *value, s32 timeout)
{
	int status = FAILURE;
	TQueue * pQueue = (TQueue*)hndl;
	if(hndl == NULL || value == NULL)
	{
		return FAILURE;
	}

	pthread_mutex_lock(&(pQueue->lock));

	while(1) 
	{
		if( pQueue->count < pQueue->maxlen ) 
		{
			pQueue->queue[pQueue->curWr] = (s32)(int *)value;
			pQueue->curWr = (pQueue->curWr+1)%pQueue->maxlen;
			pQueue->count++;
			status = SUCCESS;
			pthread_cond_signal(&pQueue->condRd);
			break;
		}
		else 
		{
			if(timeout == TIMEOUT_NONE)
			{
				break;
			}
			else if (timeout == TIMEOUT_FOREVER)
			{
				status = pthread_cond_wait(&pQueue->condWr, &pQueue->lock);
			}
			else
			{
				struct timespec to = {0};
				if(timeout < 0)
					to.tv_sec += 2;
				else
					to.tv_sec += timeout;

				status = pthread_cond_timedwait(&pQueue->condWr, &pQueue->lock, &to);
				if(status != 0)
				{
					status = FAILURE;
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&pQueue->lock);

	if(status!= SUCCESS)
	{
		LOG_DEBUG_FMT("QuePut error,count = %d, maxlen :%d", pQueue->count, pQueue->maxlen);
	}
	return status;
}


OAL_API void* QuePop(TQueHndl hndl, s32 timeout)
{
	int status = FAILURE;
	void * value = NULL;

	TQueue * pQueue = (TQueue*)hndl;
	if(hndl == NULL)
	{
		return NULL;
	}

	pthread_mutex_lock(&pQueue->lock);
	while(1) 
	{
		if(pQueue->count > 0 ) 
		{
			value = (void *)pQueue->queue[pQueue->curRd];

			pQueue->curRd = (pQueue->curRd+1)%pQueue->maxlen;
			pQueue->count--;
			pthread_cond_signal(&pQueue->condWr);
			break;
		}
		else 
		{
			if(timeout == TIMEOUT_NONE)
			{
				break;
			}
			else if (timeout == TIMEOUT_FOREVER)
			{
				status = pthread_cond_wait(&pQueue->condRd, &pQueue->lock);
			}
			else
			{
				struct timespec to = {0};
				if(timeout < 0)
					to.tv_sec += 2;
				else
					to.tv_sec += timeout;

				status = pthread_cond_timedwait(&pQueue->condRd, &pQueue->lock, &to);
				if(status != 0)
				{
					value = NULL;
					break;
				}
			}
		}
	}
	pthread_mutex_unlock(&pQueue->lock);

	return value;
}

#ifdef __cplusplus
}
#endif
