#ifndef __ALARM_MANAGER_H__
#define __ALARM_MANAGER_H__
#include <string>
using namespace std;

typedef struct _tagAlarmStrategy
{
	int iSameInterval; // ��ؾ�����ʱ�� ����
	int iKeepDays;	//���������
}AlarmStrategy;

#define MAX_ALARM_CONTENT_LEN	250	//��Ϣ���ݳ���

#define MAX_ALARM_PAGE_SIZE		50  //һ�β�ѯһҳ����¼��

enum
{
	eAlarm_Unknown_Type = 1999, //δ֪�澯
};

enum
{
	eAlarm_Level_0 = 0, //��΢
	eAlarm_Level_1 = 1, //һ��
	eAlarm_Level_2 = 2, //����
	eAlarm_Level_3 = 3  //����
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
	int iCurRecNum;//��¼��
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
