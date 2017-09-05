#include "UploadManager.h"


UploadManager* UploadManager::m_pInstance = NULL;

UploadManager* UploadManager::GetInstance()
{
	return m_pInstance;
}

int UploadManager::Initialize()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new UploadManager();
	}
	return 0;
}

void UploadManager::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

UploadManager::UploadManager(void)
{	
	pthread_mutex_init(&m_tMutex, NULL);

	m_tStatus.bRemaining = false;
	m_tStatus.bUploading = false;
	m_tStatus.iServerStat = UPLOAD_SVR_NO;
}


UploadManager::~UploadManager(void)
{
	pthread_mutex_destroy(&m_tMutex);
}


int UploadManager::SetUploadStatus( const UploadStatus *ptStatus )
{
	if (ptStatus == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tStatus,ptStatus,sizeof(UploadStatus));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int UploadManager::GetUploadStatus( UploadStatus *ptStatus )
{
	if (ptStatus == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(ptStatus, &m_tStatus, sizeof(UploadStatus));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}


