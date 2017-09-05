#include "TempFileCleanup.h"
#include <assert.h>
#include "oal_time.h"
#include "oal_log.h"
#include "oal_file.h"

static void * CheckExpiryThread(void * p)
{
	while(TempFileCleanUp::GetInstance()->m_bExitThread == false)
	{
		TempFileCleanUp::GetInstance()->CheckExpiry();

		sleep(1);
	}

	pthread_exit(NULL);
	return NULL;
}

TempFileCleanUp* TempFileCleanUp::m_pInstance = NULL;

TempFileCleanUp* TempFileCleanUp::GetInstance()
{
	return m_pInstance;
}

int TempFileCleanUp::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new TempFileCleanUp();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void TempFileCleanUp::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

TempFileCleanUp::TempFileCleanUp()
{
	pthread_mutex_init(&m_mutex, NULL);

	m_bExitThread = false;
	m_ptFileList = NULL;
}

TempFileCleanUp::~TempFileCleanUp()
{
	m_bExitThread = true;
	pthread_join(m_expiry_thread,NULL);
}

int TempFileCleanUp::Run()
{
	m_bExitThread = false;
	pthread_create(&m_expiry_thread, NULL, CheckExpiryThread, this);
	return 0;
}

int TempFileCleanUp::AddNode( const TempFileNode & tFile )
{
	int iRet = -1;
	pthread_mutex_lock(&m_mutex);
	TempFileList * pNode = new TempFileList;
	if (pNode)
	{
		memcpy(&pNode->tFile, &tFile, sizeof(TempFileNode));
		pNode->ptNext = m_ptFileList;
		m_ptFileList = pNode;

		char time1[40] = {0};
		char time2[40] = {0};
		TimeFormatString(tFile.tCrateTime, time1, 40, eYMDHMS1);
		TimeFormatString(tFile.tCrateTime+tFile.iExpiryTime, time2, 40, eYMDHMS1);
		printf("AddNode %s, %s,to %s\n",tFile.szFilePath ,time1, time2);
	}
	pthread_mutex_unlock(&m_mutex);
	return 0;
}

int TempFileCleanUp::CheckExpiry()
{
	pthread_mutex_lock(&m_mutex);
	TempFileList * pLast = NULL;
	TempFileList * pDel = NULL;
	TempFileList * pNode = m_ptFileList;
	while (pNode)
	{
		if (time(NULL) > pNode->tFile.tCrateTime+pNode->tFile.iExpiryTime)
		{
			char szCreate[40] = {0};
			TimeFormatString(pNode->tFile.tCrateTime,szCreate, 40, eYMDHMS1);
			LOG_INFOS_FMT("File:%s,Create at %s,ExpiryTime=%d,Delete",pNode->tFile.szFilePath,szCreate, pNode->tFile.iExpiryTime);

			// 删除文件
			OAL_DeleteFile(pNode->tFile.szFilePath);

			// 从链表中删除
			if (pLast)
			{
				pLast->ptNext = pNode->ptNext;
			}
			else
			{
				m_ptFileList = m_ptFileList->ptNext;
			}

			// 释放节点
			pDel = pNode;
			pNode = pNode->ptNext;
			delete pDel;
			pDel = NULL;
			continue;
		}
		else
		{
			pLast = pNode;
			pNode = pNode->ptNext;
			continue;
		}
	}
	pthread_mutex_unlock(&m_mutex);
	return 0;
}
