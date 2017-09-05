#ifndef __SQLITE3_DATABASE_H__
#define __SQLITE3_DATABASE_H__
#include "pthread.h"
#include "sqlite3.h"
#include "oal_log.h"
#include "mq_database.h"

class Sqlite3Database
{
public:
	static Sqlite3Database* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	Sqlite3Database();
	~Sqlite3Database();

	int OpenDB();
	int CreateAnalyTable();
	int CreateLogTable();
	int CreateAlarmTable();
	int CreateRecordTable();
	int CloseDB();

public:
	int Excute(const char *sql);
	int Select_Count(const char *sql);
	int Select(const char *sql, DbQueryResult & tResult);

private:
	pthread_mutex_t  m_mutex;
	sqlite3 * m_pDb;
	static Sqlite3Database* m_pInstance;
};
#endif //__SQLITE3_DATABASE_H__
