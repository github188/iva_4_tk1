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
			// �����ʾ���
			SendRealTimeAnalyTracks(iChnID, NULL);
		}

		// �㷨�����ſɽ���
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
			// δ�����ƻ�
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

		// �Ƿ������ֹͣ����
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
		// ��YUV�����г�ȡһ֡����
		YuvCacheNode tYuvNode = {0};
		if(AnalyYuvMan::GetInstance()->ReadYuvNodeFromCache(iChnID, &tYuvNode) == 0)
		{
			// �����ƵԴ�Ƿ�仯(���Զ���ʼ���㷨ģ��)
			ptRunner->CheckSolution(tYuvNode.tYuvData.yuv_info.width,tYuvNode.tYuvData.yuv_info.height);

			// ����ر��㷨�����ò���
			if (bAlgStoped && bProcessed)
			{
				ptRunner->ResetAnaly();
			}

			if (bAlgStoped || !bProcessed)
			{
				MQ_Master_SetAnalyStatus(iChnID,ANALY_RUNNING);
			}

			// ����㷨Ԥ��λ��Ϣ�Ƿ�仯
			ptRunner->CheckPreset();

			// ִ���㷨(���Զ�������)
			ptRunner->ProcessFrame(&tYuvNode);

// 			// ����ر��㷨�����ò���
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
		// �رվɵ�ͨ����Ϣ	
		if(m_iWidth != -1)
		{
			DestroyAlgChannel();
		}

		// �����µ�ͨ��
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

