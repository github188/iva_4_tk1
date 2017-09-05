#include "NotifyMan.h"
#include <assert.h>
#include "oal_log.h"
#include "oal_time.h"
#include "oal_typedef.h"

int Notify2WeiXin( const char *server_url, const AnalyNotify *pData );

void *notify_thread_fxn(void *arg)
{
    int  iRet = -1;
	
	LOG_DEBUG_FMT("notify thread is working...");
	
	while(!NotifyMan::GetInstance()->m_bExitThread)
	{
		if(!NotifyMan::GetInstance()->GetEnable())
        {
			usleep(50*000);
			continue;
		}

        /*是否有新记录*/
		AnalyNotify * ptNewNotify = {0};
		ptNewNotify = NotifyMan::GetInstance()->GetNewNotify();
		if(ptNewNotify == NULL)
		{
			usleep(50*000);
			continue;
		}
	
		iRet = NotifyMan::GetInstance()->SendNewNotify(ptNewNotify);
		if(iRet == 0)
		{
			LOG_DEBUG_FMT("SendNewNotify OK!");
		}
		else
		{
			LOG_DEBUG_FMT("SendNewNotify failed!");
		}
		free(ptNewNotify);
		ptNewNotify = NULL;
	}
	return NULL;
}

NotifyMan* NotifyMan::m_pInstance = NULL;

NotifyMan* NotifyMan::GetInstance()
{
	return m_pInstance;
}

int NotifyMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new NotifyMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void NotifyMan::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

NotifyMan::NotifyMan()
{
	pthread_mutex_init(&m_tMutex, NULL);
	memset(&m_tServer, 0, sizeof(m_tServer));
	m_bEnable = false;
	m_bExitThread = false;
	m_hNotifyQue = QueCreate(5);
	assert(m_hNotifyQue);
}

NotifyMan::~NotifyMan()
{
	m_bExitThread = true;

	pthread_join(m_tNotifyThread,NULL);
	ClearAllNotify();
	pthread_mutex_destroy(&m_tMutex);
}

int NotifyMan::Run()
{
	m_bExitThread = false;

	if (pthread_create(&m_tNotifyThread, NULL, notify_thread_fxn, NULL) != 0){
		LOG_ERROR_FMT("Event preview thread creates failed.");
	}
	return 0;
}

int NotifyMan::SetServer( const NotifySvr * ptServer )
{
	if (ptServer == NULL)
	{
		return -1;
	}
	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tServer, ptServer, sizeof(ptServer));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int NotifyMan::GetServerUrl( char * pszUrl )
{
	if (pszUrl == NULL)
	{
		return -1;
	}
	pthread_mutex_lock(&m_tMutex);
	if(m_tServer.iPort == 0)
	{
		sprintf(pszUrl, "%s",m_tServer.szAddr);
	}
	else if (m_tServer.iPort == 80)
	{
		sprintf(pszUrl, "http://%s:%d/park_access/access.do",m_tServer.szAddr ,m_tServer.iPort);
	}
	else
	{
		sprintf(pszUrl, "http://%s/park_access/access.do",m_tServer.szAddr);
	}

	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int NotifyMan::SetEnable( bool bEnable )
{
	pthread_mutex_lock(&m_tMutex);
	m_bEnable = bEnable;
	pthread_mutex_unlock(&m_tMutex);
	if (!m_bEnable)
	{
		ClearAllNotify();
	}
	return 0;
}

bool NotifyMan::GetEnable()
{
	return m_bEnable;
}

int NotifyMan::AddNewNotify( const AnalyNotify * ptResult )
{
	if (ptResult == NULL)
	{
		return -1;
	}
	int iRet = -1;
	AnalyNotify *pNewNotify = (AnalyNotify*)malloc(sizeof(AnalyNotify));
	if (pNewNotify == NULL)
	{
		LOG_ERROR_FMT("malloc AnalyNotify failed");
		return -1;
	}

	memcpy(pNewNotify, ptResult, sizeof(AnalyNotify));
	iRet = QuePush(m_hNotifyQue, pNewNotify, TIMEOUT_NONE);
	if (iRet != 0)
	{
		free(pNewNotify);
		pNewNotify = NULL;
		LOG_ERROR_FMT("QuePush NewNotify failed");
		return -1;
	}
	return 0;
}

AnalyNotify * NotifyMan::GetNewNotify()
{
	return (AnalyNotify*)QuePop(m_hNotifyQue, TIMEOUT_NONE);
}

int NotifyMan::SendNewNotify( const AnalyNotify * ptResult )
{
	if (!m_bEnable)
	{
		return -1;
	}

	char szUrl[URI_LEN] = {0};
	GetServerUrl(szUrl);

	return Notify2WeiXin(szUrl, ptResult);
}

int NotifyMan::ClearAllNotify()
{
	AnalyNotify * p = GetNewNotify();
	while (p)
	{
		free(p);
		p = GetNewNotify();
	}
	return 0;
}

