#ifndef __LOG_MANAGER_H__
#define __LOG_MANAGER_H__
#include <string>
using namespace std;

enum{
	OP_LOG_USR = 1,//�û�����
	OP_LOG_MTN = 2,//ϵͳά��
	OP_LOG_SYS = 3,//ϵͳ����
	OP_LOG_VIN = 4,//��Ƶ����
	OP_LOG_REC = 5,//��Ƶ¼��
	OP_LOG_ALG = 6,//�����㷨
	OP_LOG_OUT = 7 //��Ƶ���
};

#define MIN_OP_LOG_TYPE 0
#define MAX_OP_LOG_TYPE OP_LOG_ALG

#define MAX_LOG_CONTENT_LEN	250	//��Ϣ���ݳ���

#define MAX_LOG_PAGE_SIZE	50//һ�β�ѯһҳ����¼��

typedef struct _LogQueryCond
{
	int iPageNo;
	int iPageSize;

	int iType;
	time_t tStart;
	time_t tEnd;
	char szLike[32];
}LogQueryCond;

typedef struct _LogRecord
{
	int iRowID;
	int iType;
	time_t tCreate;
	char szContent[MAX_LOG_CONTENT_LEN];
	char szUser[64];
	char szFrom[128];
}LogRecord;

typedef struct _LogQueryResult
{
	int iCurRecNum;//��¼��
	int iTotalRecNum;
	LogRecord tRecords[MAX_LOG_PAGE_SIZE];
}LogQueryResult;


class LogManager
{
public:
	static LogManager* GetInstance();
	static int Initialize();
	static void UnInitialize();

	int QueryLog( const LogQueryCond & tCond, LogQueryResult &tResult );
	int ExportLog( const LogQueryCond & tCond, string &filepath );
	int DeleteLog( int iRowID );
	int WriteOpLog(int iType, const char * user, const char * remote, const char * msg);

	int ClearExportDir();
	int CreateExportFilePath(char *pszFilePath);
private:
	static LogManager* m_pInstance;
	LogManager();
	~LogManager();
};



#endif//__LOG_MANAGER_H__
