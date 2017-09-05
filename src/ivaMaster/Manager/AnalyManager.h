#ifndef __ANALY_MANAGER_H__
#define __ANALY_MANAGER_H__
#include <string>
#include "capacity.h"
#include "pthread.h"
#include "mq_analysis.h"
#include "oal_typedef.h"
using namespace std;


#define MAX_ANALY_PAGE_SIZE		20//一次查询一页最多纪录数

typedef struct _AnalyQueryCond
{
	int iPageNo;
	int iPageSize;

	int iChnID;
	int iEventID;
	int iDataType;
	int iPlateColor;
	time_t tStart;
	time_t tEnd;
	char szPlate[PLATE_LEN];
}AnalyQueryCond;

typedef struct _AnalyRecord
{
	int iRowID;

	time_t tCreate;

	int iChnID;
	int iEventID;
	int iDataType;

	int iPlateColor;
	char szPlate[PLATE_LEN];

	int iImgNum;
	char szImgRoot[256];
	char szImgPath[MAX_IMG_NUM][256];
	
	int iRecNum;
	char szRecRoot[256];
	char szRecPath[MAX_REC_NUM][256];

	int iStatus;
}AnalyRecord;

typedef struct _AnalyQueryResult
{
	int iCurRecNum;//纪录数
	int iTotalRecNum;
	AnalyRecord tRecords[MAX_ANALY_PAGE_SIZE];
}AnalyQueryResult;

class AnalyManager
{
public:
	static AnalyManager* GetInstance();
	static int Initialize();
	static void UnInitialize();

	int SetAnalyVersion(const char * ver);
	int GetAnalyVersion(char * ver);

	int SetAnalyStatus(int iChnID, int iStatus);
	int GetAnalyStatus(int iChnID);

public:
	int Query( const AnalyQueryCond & tCond, AnalyQueryResult &tResult );
	int ReUpload( int iRowID );
	int Delete( int iRowID );

private:
	AnalyManager();
	~AnalyManager();

private:
	static AnalyManager* m_pInstance;

private:
	int m_iAllStatus[MAX_CHANNEL_NUM];
	pthread_mutex_t m_tMutex;
};


#endif//__ANALY_MANAGER_H__
