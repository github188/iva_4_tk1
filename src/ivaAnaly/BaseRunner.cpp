#include "BaseRunner.h"
#include "oal_log.h"
#include "oal_time.h"
#include "AnalyRealTime.h"
#include "AnalyCfgMan.h"
#include "AnalyYuvCacheMan.h"
#include "mq_master.h"

void* AnalyRunningThreadFxn(void* param)
{
	BaseRunner * ptRunner = (BaseRunner *)param;

	if(param == NULL)
	{
		return NULL;
	}

	int iChnID = ptRunner->m_iChnID;

	bool bAlgStoped = false;
	bool bProcessed = false;

	LOG_DEBUG_FMT("Analy running thread is running...");

	while(!ptRunner->m_bExitThread)
	{
		if(bAlgStoped)
		{
			// 清空显示结果
			SendRealTimeAnalyTracks(iChnID, NULL);
		}

		// 算法开启才可进行
		if(!AnalyCfgMan::GetInstance()->ChnAlgIsOpen(iChnID))
		{
			if(!bAlgStoped)
			{
				LOG_INFOS_FMT("Channel%d Stop to analy...", iChnID);
				ptRunner->ResetCfg();
				MQ_Master_SetAnalyStatus(iChnID,ANALY_STOP);
			}
			bAlgStoped = true;
			usleep(10*1000);
			continue;
		}

		if(AnalyPlanMan::GetInstance()->GetChnCurScene(iChnID) < 0)
		{
			// 未触发计划
			if(!bAlgStoped)
			{
				LOG_INFOS_FMT("Channel%d is not in plan, Stop Analy...", iChnID);
				ptRunner->ResetCfg();
				MQ_Master_SetAnalyStatus(iChnID,ANALY_STOP);
			}
			bAlgStoped = true;
			usleep(10*1000);
			continue;
		}

		// 是否磁盘满停止分析
		if(AnalyCfgMan::GetInstance()->IsNeedStopAnalyForFull())
		{
			if(!bAlgStoped)
			{
				LOG_INFOS_FMT("Channel%d Save Path is Full, Stop Analy...", iChnID);
				ptRunner->ResetCfg();
				MQ_Master_SetAnalyStatus(iChnID,ANALY_STOP);
			}
			bAlgStoped = true;
			usleep(10*1000);
			continue;
		}

		//printf("%s %d\n", __FILE__, __LINE__);
		// 从YUV队列中抽取一帧数据
		YuvCacheNode tYuvNode = {0};
		if(AnalyYuvMan::GetInstance()->ReadYuvNodeFromCache(iChnID, &tYuvNode) == 0)
		{
			// 检测视频源是否变化(会自动初始化算法模块)
			ptRunner->CheckSolution(tYuvNode.tYuvData.yuv_info.width,tYuvNode.tYuvData.yuv_info.height);

			// 如果关闭算法，重置参数
			if (bAlgStoped && bProcessed)
			{
				ptRunner->ResetAnaly();
			}

			if (bAlgStoped || !bProcessed)
			{
				MQ_Master_SetAnalyStatus(iChnID,ANALY_RUNNING);
			}

			// 检测算法预置位信息是否变化
			ptRunner->CheckPreset();

			// 执行算法(会自动处理结果)
			ptRunner->ProcessFrame(&tYuvNode);

// 			// 如果关闭算法，重置参数
// 			if (bAlgStoped && bProcessed)
// 			{
// 				ptRunner->ResetAnaly();
// 			}

			bAlgStoped = false;
			bProcessed = true;
			usleep(1000);
		}
	}

	pthread_exit(ptRunner);
	return NULL;
}

BaseRunner::BaseRunner(int iChnID)
{
	m_iChnID  = iChnID;
	m_iHeight = -1;
	m_iWidth  = -1;
	m_bExitThread = false;
}

BaseRunner::~BaseRunner()
{
	m_bExitThread = true;
	pthread_join(m_run_thread, NULL);
}

void BaseRunner::Run()
{
	m_bExitThread = false;

	if(pthread_create(&m_run_thread, NULL, AnalyRunningThreadFxn, this))
	{
		LOG_ERROR_FMT("AnalyRunningThreadFxn Thread create failed");
	}
}

void BaseRunner::CheckSolution( int iWidth, int iHeight )
{
	if( iWidth != m_iWidth || iHeight!= m_iHeight)
	{
		// 关闭旧的通道信息	
		if(m_iWidth != -1)
		{
			DestroyAlgChannel();
		}

		// 创建新的通道
		m_iWidth  = iWidth;
		m_iHeight = iHeight;
		CreateAlgChannel();
	}
}

void BaseRunner::ResetCfg()
{
	LOG_DEBUG_FMT("Channel%d BaseRunner::ResetCfg",m_iChnID);
	;//do nothing
}

