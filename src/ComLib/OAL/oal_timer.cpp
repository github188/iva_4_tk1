#include "include/oal_timer.h"
#include "include/oal_log.h"
#ifdef WIN32
#include <winsock2.h>
#endif // WIN32

typedef struct TagTimer
{
	TimerProcFunPtr pvFunc; // 定时器处理函数
	FreeData        pvFree;	// 定时任务结束后释放用户数据
	void *			pvData; // 传给定时器处理函数的参数
	int			 iInterVal; // 定时周期            
	int			   iRepeat; // 重复方式            

	bool 		     bRun;  // 通过该标志可以在不重建定时器的情况下反复启停定时器
	int				iTick;	// 计数器            
	pthread_mutex_t tMutex;
}TTimerParam;

int g_s32TimerInitialize = 0;
int g_s32TimerOver = 0;
TTimerParam g_tTimerParam[MAX_TIMER_NUM];
pthread_t   g_hTimerThread;


/*定时器处理函数*/
void  *TimerThreadFunc(void *pData)
{
	struct timeval tTimeVal = {0};
	tTimeVal.tv_sec  = 1;
	tTimeVal.tv_usec = 0;

	while(1 != g_s32TimerOver)
	{
		if(select(0, NULL, NULL, NULL, &tTimeVal) == 0)
		{
			// 1秒定时到 扫描所有的定时器
			for(int i =0; i < MAX_TIMER_NUM; i++)
			{
				bool bDone = false;

				pthread_mutex_lock(&g_tTimerParam[i].tMutex);

				if( g_tTimerParam[i].pvFunc != NULL  &&
					g_tTimerParam[i].iTick > 0 &&
					g_tTimerParam[i].bRun)
				{
					g_tTimerParam[i].iTick -= 1;

					if(g_tTimerParam[i].iTick <= 0)
					{
						g_tTimerParam[i].pvFunc(g_tTimerParam[i].pvData);
						if (g_tTimerParam[i].iRepeat == eTimerOnce)
						{
							// 执行一次就退出
							bDone = true;
						}
						else
						{
							// 重复执行
							g_tTimerParam[i].iTick = g_tTimerParam[i].iInterVal;
						}
					}
				}

				pthread_mutex_unlock(&g_tTimerParam[i].tMutex);

				if (bDone)
				{
					TimerDelete(i);
				}
			}
		}
	}

	return (void *)0;
}


OAL_API int TimerInit(void)
{
	g_s32TimerOver = 0;

	if(g_s32TimerInitialize == 0)
	{
		for(int i = 0; i < MAX_TIMER_NUM; i++)
		{
			memset(&g_tTimerParam[i], 0, sizeof(TTimerParam));
			pthread_mutex_init(&g_tTimerParam[i].tMutex,NULL);
		}

		// 创建处理函数
		if(pthread_create(&g_hTimerThread, NULL, TimerThreadFunc, NULL))
		{
			LOG_ERROR_FMT("Timer Thread create failed");
			for(int i = 0; i < MAX_TIMER_NUM; i++)
			{
				pthread_mutex_destroy(&g_tTimerParam[i].tMutex);
			}
			return FAILURE;
		}
	}

	g_s32TimerInitialize++;
	return SUCCESS;
}

OAL_API int TimerUninit(void)
{
	if(g_s32TimerInitialize > 0)
	{
		g_s32TimerInitialize--;
		if(0 == g_s32TimerInitialize)
		{
			g_s32TimerOver = 1;
			for(int i = 0; i < MAX_TIMER_NUM; i++)
			{
				TimerDelete(i);

				pthread_mutex_destroy(&g_tTimerParam[i].tMutex);
			}

			pthread_join(g_hTimerThread, NULL);
		}
	}

	return SUCCESS;
}

OAL_API TTimer TimerCreate( int iInterval, int iRepeat, TimerProcFunPtr pvFunc, void *pvData, FreeData pvFree )
{
	TTimer tTimerID = 0;

	if(NULL == pvFunc || iInterval < 1)
	{
		return  FAILURE;
	}

	if(g_s32TimerInitialize > 0)
	{
		for(int i = 1; i < MAX_TIMER_NUM; i++)
		{
			pthread_mutex_lock(&g_tTimerParam[i].tMutex);
			if(NULL == g_tTimerParam[i].pvFunc)
			{
				g_tTimerParam[i].pvFunc = pvFunc;
				g_tTimerParam[i].pvData = pvData;
				g_tTimerParam[i].pvFree = pvFree;
				g_tTimerParam[i].iInterVal = iInterval;
				g_tTimerParam[i].iRepeat = (iRepeat==eTimerOnce?eTimerOnce:eTimerRepeat);

				g_tTimerParam[i].bRun = false;// TimerStart以后开始
				g_tTimerParam[i].iTick = iInterval;
				
				tTimerID = i;
				break;
			}
			pthread_mutex_unlock(&g_tTimerParam[i].tMutex);

		}
	}
	else
	{
		tTimerID = FAILURE;
	}

	return tTimerID;
}

OAL_API int TimerDelete(TTimer tTimerID)
{
	if(g_s32TimerInitialize > 0)
	{
		if(tTimerID > 0 && tTimerID < MAX_TIMER_NUM)
		{
			TimerStop(tTimerID);

			pthread_mutex_lock(&g_tTimerParam[tTimerID].tMutex);
			
			g_tTimerParam[tTimerID].pvFunc = NULL;
			g_tTimerParam[tTimerID].iRepeat= eTimerRepeat;

			if(g_tTimerParam[tTimerID].pvData != NULL && g_tTimerParam[tTimerID].pvFree != NULL)
			{
				g_tTimerParam[tTimerID].pvFree(g_tTimerParam[tTimerID].pvData);
				g_tTimerParam[tTimerID].pvData = NULL;
				g_tTimerParam[tTimerID].pvFree = NULL;
			}

			pthread_mutex_unlock(&g_tTimerParam[tTimerID].tMutex);

			return SUCCESS;
		}
	}

	return FAILURE;
}

OAL_API int TimerStart(TTimer tTimerID)
{
	if(g_s32TimerInitialize > 0)
	{
		if((tTimerID > 0) && (tTimerID < MAX_TIMER_NUM) )
		{
			pthread_mutex_lock(&g_tTimerParam[tTimerID].tMutex);

			g_tTimerParam[tTimerID].bRun  = true;
			g_tTimerParam[tTimerID].iTick = g_tTimerParam[tTimerID].iInterVal;

			pthread_mutex_unlock(&g_tTimerParam[tTimerID].tMutex);
			return SUCCESS;
		}
	}

	return FAILURE;
}

OAL_API int TimerStop(TTimer tTimerID)
{
	//s32 s32Result = 0;

	if(g_s32TimerInitialize > 0)
	{
		if((tTimerID >= 0) && (tTimerID < MAX_TIMER_NUM) )
		{
			pthread_mutex_lock(&g_tTimerParam[tTimerID].tMutex);

			g_tTimerParam[tTimerID].bRun = false;
			g_tTimerParam[tTimerID].iTick= 0;

			pthread_mutex_unlock(&g_tTimerParam[tTimerID].tMutex);
			return SUCCESS;
		}
	}
	return FAILURE;
}

OAL_API int TimerReset(TTimer tTimerID, int iInterval)
{
	if (iInterval < 1)
	{
		LOG_ERROR_FMT("iInterval < 1");
		return FAILURE;
	}

	if(g_s32TimerInitialize > 0)
	{
		if((tTimerID > 0) && (tTimerID < MAX_TIMER_NUM) )
		{
			pthread_mutex_lock(&g_tTimerParam[tTimerID].tMutex);
			
			g_tTimerParam[tTimerID].bRun = true;
			g_tTimerParam[tTimerID].iInterVal = iInterval;
			g_tTimerParam[tTimerID].iTick = iInterval;
			
			pthread_mutex_unlock(&g_tTimerParam[tTimerID].tMutex);
			return SUCCESS;
		}
	}
	return FAILURE;
}










