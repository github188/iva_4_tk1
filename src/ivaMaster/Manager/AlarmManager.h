#ifndef __ALARM_MANAGER_H__
#define __ALARM_MANAGER_H__
#include <string>
using namespace std;

typedef struct _tagAlarmStrategy
{
	int iSameInterval; // 相关警告间隔时间 分钟
	int iKeepDays;	//保存多少天
}AlarmStrategy;

#define MAX_ALARM_CONTENT_LEN	250	//消息内容长度

#define MAX_ALARM_PAGE_SIZE		50  //一次查询一页最多纪录数

enum
{
	eAlarm_Unknown_Type = 1999, //未知告警
};

enum
{
	eAlarm_Level_0 = 0, //轻微
	eAlarm_Level_1 = 1, //一般
	eAlarm_Level_2 = 2, //严重
	eAlarm_Level_3 = 3  //致命
};

typedef struct _AlarmQueryCond
{
	int iPageNo;
	int iPageSize;

	time_t tStart;
	time_t tEnd;
	char szLike[32];
}AlarmQueryCond;

typedef struct _AlarmRecord
{
	int iRowID;
	time_t tCreate;
	char szContent[MAX_ALARM_CONTENT_LEN];
}AlarmRecord;

typedef struct _AlarmQueryResult
{
	int iCurRecNum;//纪录数
	int iTotalRecNum;
	AlarmRecord tRecords[MAX_ALARM_PAGE_SIZE];
}AlarmQueryResult;

class AlarmManager
{
public:
	static AlarmManager* GetInstance();
	static int Initialize();
	static void UnInitialize();

	int QueryAlarm( const AlarmQueryCond & tCond, AlarmQueryResult &tResult );
	int ExportAlarm( const AlarmQueryCond & tCond, string &filepath );
	int DeleteAlarm( int iRowID );
	int WriteAlarm(const char * msg);

	int ClearExportDir();
	int CreateExportFilePath( char *pszFilePath );
private:
	static AlarmManager* m_pInstance;

private:
	AlarmManager();
	~AlarmManager();
};


#endif//__ALARM_MANAGER_H__
