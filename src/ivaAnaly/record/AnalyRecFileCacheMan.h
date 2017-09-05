#ifndef __ALG_RECORD_FILE_H__
#define __ALG_RECORD_FILE_H__
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "oal_queue.h"
#include "capacity.h"

typedef struct 
{
	int    iChnID;
	int    iFileID;// Œ®“ª±Í ∂
	time_t begin;
	int    secs;
}RecFileMission;

typedef struct _tagRecFile
{
	RecFileMission misson;
	time_t create_time;
	char path[256];
	struct _tagRecFile * next;
}RecFile,* RecFileList;

class AnalyRecFileCacheMan
{
public:
	static AnalyRecFileCacheMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
	
public:
	int Run();
	int SetTimeOut(int iChnID, int iSecs);
	int CreateRecFile(const RecFileMission &tMisson);
	int GetRecFile(int iChnID, int iID, RecFile* pFile);
	bool m_bExitThread;
	TQueHndl  m_ptMissionList;
	
public:
	void CheckTimeOut();
	int  AddRecFile(RecFile* pFile);
	void FreeAllRecFile();
private:
	AnalyRecFileCacheMan();
	~AnalyRecFileCacheMan();

private:
	static AnalyRecFileCacheMan* m_pInstance;

	pthread_t m_file_create_thread;
	pthread_t m_file_timeout_thread;
	
private:
	RecFileList m_ptRecFilelist;
	pthread_mutex_t  m_hndlSem;
	
	int m_iTimeOutSecs[MAX_CHANNEL_NUM];
};



#endif // __ALG_RECORD_FILE_H__

