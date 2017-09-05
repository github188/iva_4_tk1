#include "pthread.h"
#include "mq_master.h"
#ifndef __UPLOAD_MANAGER_H__
#define __UPLOAD_MANAGER_H__



class UploadManager
{
public:
	static UploadManager* GetInstance();
	static int Initialize();
	static void UnInitialize();

	int SetUploadStatus( const UploadStatus *ptStatus );
	int GetUploadStatus( UploadStatus *ptStatus );

private:
	UploadManager();
	~UploadManager();

private:
	static UploadManager* m_pInstance;

private:
	UploadStatus m_tStatus;
	pthread_mutex_t m_tMutex;
};


#endif//__UPLOAD_MANAGER_H__
