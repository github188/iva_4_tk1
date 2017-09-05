#ifndef __TEMP_FILE_CLEANUP_H__
#define __TEMP_FILE_CLEANUP_H__

#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"

#define EXPORT_FILE_TIME_OUT 600// �����ļ���ʱʱ��

typedef struct _TempFileNode
{
	time_t tCrateTime;	// ����ʱ��
	int    iExpiryTime;	// ����ʱ�䳤
	char   szFilePath[256];
}TempFileNode;

typedef struct _TempFileList
{
	TempFileNode tFile;
	struct _TempFileList * ptNext;
}TempFileList;

class TempFileCleanUp
{
public:
	static TempFileCleanUp* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	TempFileCleanUp();
	~TempFileCleanUp();

public:
	int Run();

	int AddNode(const TempFileNode & tFile);

	bool m_bExitThread;
	int CheckExpiry();

private:

	TempFileList   *m_ptFileList;

	pthread_mutex_t  m_mutex;
	pthread_t 		 m_expiry_thread;

private:
	static TempFileCleanUp* m_pInstance;
};



#endif //__TEMP_FILE_CLEANUP_H__

