
#ifndef __NOTIFY_H__
#define __NOTIFY_H__
#include "mq_upload.h"
#include "pthread.h"
#include "oal_queue.h"

class NotifyMan
{
public:
	static NotifyMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	NotifyMan();
	~NotifyMan();
public:
	int Run();
	int SetServer(const NotifySvr * ptServer);
	int GetServerUrl( char * pszUrl );

	int SetEnable(bool bEnable);
	bool GetEnable();

	int AddNewNotify(const AnalyNotify * ptResult);
	AnalyNotify * GetNewNotify();
	int SendNewNotify(const AnalyNotify * ptResult);

	bool m_bExitThread;

private:
	int ClearAllNotify();

private:

	bool m_bEnable;
	NotifySvr m_tServer;
	TQueHndl m_hNotifyQue;
	pthread_mutex_t  m_tMutex;
	pthread_t m_tNotifyThread;

private:
	static NotifyMan* m_pInstance;
};

#endif//__NOTIFY_H__


