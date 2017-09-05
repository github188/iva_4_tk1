#ifndef __REC_MANAGER_H__
#define __REC_MANAGER_H__
#include "oal_typedef.h"
#include "mq_record.h"
#include "pthread.h"
#include "capacity.h"


#define MAX_REC_PAGE_SIZE	50//一次查询一页最多纪录数

enum{
	REC_ROUTINE = 0 // 常规录像
};

typedef struct _RecQueryCond
{
	int iPageNo;
	int iPageSize;

	int iChnID;
	int iType;
	time_t tStart;
	time_t tEnd;
}RecQueryCond;

typedef struct _RecRecord
{
	int iRowID;
	int iType;
	int iChnID;
	time_t tStart;
	time_t tEnd;
	char szPath[215];
}RecRecord;

typedef struct _RecQueryResult
{
	int iCurRecNum;//纪录数
	int iTotalRecNum;
	RecRecord tRecords[MAX_REC_PAGE_SIZE];
}RecQueryResult;

class RecManager
{
public:
	static RecManager* GetInstance();
	static int Initialize();
	static void UnInitialize();

public:
	int GetChnStatus(int iChnID);
	int SetChnStatus(int iChnID, int iStatus);

	int QueryRec( const RecQueryCond & tCond, RecQueryResult &tResult );
	int SaveAsRec( int iRowID, int iDiskID );
	int DeleteRec( int iRowID );

private:
	RecManager();
	~RecManager();
	static RecManager* m_pInstance;

private:
	int QueryRecFilePath( int iRowID, char * pszFilePath );

private:
	RecStrategy m_tStrategy;
	int		    m_iAllStatus[MAX_CHANNEL_NUM];
	pthread_mutex_t m_tMutex;
};



#endif//__REC_MANAGER_H__
