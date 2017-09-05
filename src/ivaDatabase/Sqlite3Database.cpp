#include "Sqlite3Database.h"
#include <assert.h>

#define DB_Path "iva.db3"

Sqlite3Database* Sqlite3Database::m_pInstance = NULL;

Sqlite3Database* Sqlite3Database::GetInstance()
{
	return m_pInstance;
}

int Sqlite3Database::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new Sqlite3Database();
		if (m_pInstance == NULL)
		{
			return -1;
		}

		int iRet = m_pInstance->OpenDB();
		if (iRet != 0)
		{
			Sqlite3Database::UnInitialize();
			return -1;
		}
		int iRet1 = m_pInstance->CreateRecordTable();
		int iRet2 = m_pInstance->CreateAnalyTable();
		int iRet3 = m_pInstance->CreateLogTable();
		int iRet4 = m_pInstance->CreateAlarmTable();
		if (iRet1 != 0 || iRet2 != 0 || iRet3 != 0 || iRet4 != 0)
		{
			Sqlite3Database::UnInitialize();
			return -1;
		}
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void Sqlite3Database::UnInitialize()
{
	if (m_pInstance)
	{
		m_pInstance->CloseDB();
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

Sqlite3Database::Sqlite3Database()
{
	pthread_mutex_init(&m_mutex, NULL);
	m_pDb = NULL;
}

Sqlite3Database::~Sqlite3Database()
{
	pthread_mutex_destroy(&m_mutex);
}

int Sqlite3Database::OpenDB()
{
	sqlite3 *db = NULL;
	int iRet = sqlite3_open(DB_Path, &db);

	if( iRet != SQLITE_OK )
	{
		LOG_ERROR_FMT("open database %s error:%s.\n", DB_Path, sqlite3_errmsg(db));
		return -1;
	}
#if 0	
	else
	{
		iRet = sqlite3_exec(db, "PRAGMA synchronous = OFF;", 0,0,0);
		if( iRet!= SQLITE_OK )		
		{
			LOG_ERROR_FMT("error status:%d error sql:%s !\n",iRet,"PRAGMA synchronous = NORMAL;");
		}
	}
#endif

	pthread_mutex_lock(&m_mutex);
	m_pDb = db;
	pthread_mutex_unlock(&m_mutex);
	return 0;
}

int Sqlite3Database::CreateAnalyTable()
{
	if (m_pDb == NULL )
	{
		return -1;
	}

	char szCreateSql[2048] = {0};
	sprintf(szCreateSql,"CREATE TABLE IF NOT EXISTS AnalyTable("
		"CREATETIME NUMERIC,CREATEMESC NUMERIC,"
		"DATATYPE INTEGER,EVENTID INTEGER,CHNID INTEGER,SCENEID INTEGER,"
		"PLATE TEXT,PLATECOLOR INTEGER,PLATETYPE INTEGER,"
		"PLATEPIC INTEGER,PLATEPOS TEXT,CONFIDENCE INTEGER,"
		"CARTYPE INTEGER,CARCOLOR INTEGER,BRAND INTEGER,"
		"SPEED INTEGER,"
		"SNAPNUM INTEGER,PICNUM INTEGER,PICPATH TEXT,"
		"PIC1NAME TEXT,PIC1TIME TEXT,PIC1POS TEXT,SNAP1TYPE INTEGER,"
		"PIC2NAME TEXT,PIC2TIME TEXT,PIC2POS TEXT,SNAP2TYPE INTEGER,"
		"PIC3NAME TEXT,PIC3TIME TEXT,PIC3POS TEXT,SNAP3TYPE INTEGER,"
		"PIC4NAME TEXT,PIC4TIME TEXT,PIC4POS TEXT,SNAP4TYPE INTEGER,"
		"RECNUM INTEGER,RECPATH TEXT,"
		"REC1NAME TEXT,REC2NAME TEXT,"
		"NETNO TEXT,REGNO TEXT,DEVNO TEXT,MANNO TEXT,"
		"AREAID TEXT,AREANAME TEXT,MONID TEXT,MONNAME TEXT,CROSSID TEXT,CROSSNAME TEXT,"		
		"SCENENAME TEXT,DIRECTION NUMERIC,"
		"CODE TEXT,DESC TEXT,"
		"STATUS INTEGER);");

	char *pErrMsg = NULL;
	int iRet = sqlite3_exec(m_pDb, szCreateSql, NULL, NULL, &pErrMsg);
	if(iRet != SQLITE_OK )
	{
		LOG_ERROR_FMT("CREATE AnalyTable failed, %s", pErrMsg?pErrMsg:"");
	}
	return iRet;
}

int Sqlite3Database::CreateLogTable()
{
	if (m_pDb == NULL )
	{
		return -1;
	}

	char szCreateSql[2048] = {0};
	sprintf(szCreateSql,"CREATE TABLE IF NOT EXISTS LogTable(TYPE INTEGER,CREATETIME NUMERIC,CONTENT TEXT,USER TEXT,ADDR TEXT);");

	char *pErrMsg = NULL;
	int iRet = sqlite3_exec(m_pDb, szCreateSql, NULL, NULL, &pErrMsg);
	if(iRet != SQLITE_OK )
	{
		LOG_ERROR_FMT("CREATE LogTable failed, %s", pErrMsg?pErrMsg:"");
	}
	return iRet;
}

int Sqlite3Database::CreateAlarmTable()
{
	if (m_pDb == NULL )
	{
		return -1;
	}

	char szCreateSql[2048] = {0};
	sprintf(szCreateSql,"CREATE TABLE IF NOT EXISTS AlarmTable(TYPE INTEGER,LEVEL INTEGER,CREATETIME NUMERIC,CONTENT TEXT,STATUS INTEGER);");

	char *pErrMsg = NULL;
	int iRet = sqlite3_exec(m_pDb, szCreateSql, NULL, NULL, &pErrMsg);
	if(iRet != SQLITE_OK )
	{
		LOG_ERROR_FMT("CREATE AlarmTable failed, %s", pErrMsg?pErrMsg:"");
	}
	return iRet;
}

int Sqlite3Database::CreateRecordTable()
{
	if (m_pDb == NULL )
	{
		return -1;
	}

	char szCreateSql[2048] = {0};
	sprintf(szCreateSql,"CREATE TABLE IF NOT EXISTS RecordTable(FILEPATH TEXT,BEGINTIME NUMERIC,ENDTIME NUMERIC,CHNID INTEGER,TYPE INTEGER,STATUS INTEGER);");

	char *pErrMsg = NULL;
	int iRet = sqlite3_exec(m_pDb, szCreateSql, NULL, NULL, &pErrMsg);
	if(iRet != SQLITE_OK )
	{
		LOG_ERROR_FMT("CREATE RecordTable failed, %s", pErrMsg?pErrMsg:"");
	}
	return iRet;
}

int Sqlite3Database::CloseDB()
{
	if (m_pDb == NULL )
	{
		return 0;
	}

	if( sqlite3_close(m_pDb) != SQLITE_OK )
	{
		LOG_ERROR_FMT("close database error.\n");
		return -1;
	}
	else
	{
		m_pDb = NULL;
		return 0;
	}
}	

int Sqlite3Database::Excute( const char *sql )
{
	if (m_pDb == NULL || sql == NULL)
	{
		return -1;
	}

	LOG_DEBUG_FMT("[SQL] %s", sql);

	char *pErrMsg = NULL;
	int iRet = sqlite3_exec(m_pDb, sql, NULL, NULL, &pErrMsg);
	if(iRet != SQLITE_OK )
	{
		LOG_ERROR_FMT("sqlite3_exec error: result:%d msg:%s",iRet, pErrMsg?pErrMsg:"");
	}
	return iRet;
}

int Sqlite3Database::Select_Count( const char *sql )
{
	if (m_pDb == NULL || sql == NULL)
	{
		return -1;
	}

	int iCnt = 0;

	sqlite3_stmt *stmt = NULL;
	int iRet = sqlite3_prepare_v2(m_pDb, sql, -1, &stmt, NULL); 
	if(iRet != SQLITE_OK )
	{
		LOG_ERROR_FMT("sqlite3_prepare_v2 error: result:%d",iRet);
		LOG_ERROR_FMT("sql:%s", sql);
		return 0;
	}
	else
	{
		while(sqlite3_step(stmt) == SQLITE_ROW)
		{
			iCnt = sqlite3_column_int(stmt, 0);
		}
	}

	if(stmt)
	{
		sqlite3_finalize(stmt);
	}

	return iCnt;
}

int Sqlite3Database::Select( const char *sql, DbQueryResult & tResult )
{
	if (m_pDb == NULL || sql == NULL)
	{
		return -1;
	}

	memset(&tResult, 0, sizeof(DbQueryResult));

	char **dbResult = NULL;
	char * pErrMsg = NULL;
	int iRow = 0;
	int iCol = 0;
	//LOG_DEBUG_FMT("sql=%s",sql);
	int iRet = sqlite3_get_table(m_pDb, sql, &dbResult, &iRow, &iCol, &pErrMsg );

	if(iRet != SQLITE_OK)
	{
		LOG_ERROR_FMT("sqlite3_get_table error: result:%d msg:%s",iRet, pErrMsg?pErrMsg:"");
		LOG_ERROR_FMT("sql:%s", sql);
		iRet = -1;
	}
	else if (dbResult == NULL || *dbResult == NULL)
	{
		//LOG_WARNS_FMT("sqlite3_get_table no result iRow=%d iCol=%d",iRow, iCol);
		tResult.iRow = iRow;
		tResult.iCol = iCol;
		return 0;
	}
	else
	{
		//LOG_DEBUG_FMT("iRow = %d, iCol = %d ",iRow, iCol);
		tResult.iRow = iRow;
		tResult.iCol = iCol;
		int iReadLen = 0;
		char * pdate = (char *)(tResult.data);

		if(iRow > 0)
		{
			for(int i = 0; i < iRow; i++)
			{
				bool berror = false;

				for(int j = 0; j < iCol; j++)
				{
					char * pvalue = dbResult[iCol*(i+1)+j];
					int len = 0;
					if(pvalue)
					{
						len = strlen(pvalue);
					}

					if(iReadLen + sizeof(int) + len < sizeof(tResult.data))
					{
						*(int *)pdate = len;
						pdate += sizeof(int);

						if (pvalue)
						{
							strcpy(pdate, pvalue);
							pdate += len;
						}

						iReadLen += sizeof(int) + len;
					}
					else
					{
						LOG_ERROR_FMT("iReadLen=%d sizeof(tResult.data)=%d",iReadLen,sizeof(tResult.data));
						berror = true;
						break;
					}
				}

				if(berror)
				{
					tResult.iRow = i;// 只能取这么多行
					LOG_WARNS_FMT("ERROR, total %d rows, but only read %d rows\n",iRow, tResult.iRow);
					break;
				}
			}
		}
		tResult.iLen = iReadLen;
	}

	if (dbResult)
	{
		sqlite3_free_table( dbResult );
	}

	return 0;
}

