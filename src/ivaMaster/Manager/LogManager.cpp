#include "LogManager.h"
#include "mq_database.h"
#include "oal_log.h"
#include "oal_unicode.h"
#include "WriteExcel.h"
#include <assert.h>
#include "capacity.h"
#include "oal_time.h"
#include "oal_file.h"

#define  LOGEXPORT_DIR		"LogExport"

LogManager* LogManager::m_pInstance = NULL;

LogManager::LogManager()
{
	ClearExportDir();
}

LogManager::~LogManager()
{
}

LogManager* LogManager::GetInstance()
{
	return m_pInstance;
}

int LogManager::Initialize()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new LogManager();
		assert(m_pInstance);
	}
	return 0;
}

void LogManager::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

int LogManager::QueryLog( const LogQueryCond & tCond, LogQueryResult &tResult )
{
	memset(&tResult, 0, sizeof(LogQueryResult));

	string sqlSelect = "select ROWID,CREATETIME,TYPE,USER,ADDR,CONTENT from LogTable";
	string sqlSelectCout = "select count(*) from LogTable";

	int iWhereCnt = 0;
	char szWhere[4][512] = {{0}};
	if (tCond.tStart > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CREATETIME >= %ld", tCond.tStart);
	}

	if (tCond.tEnd > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CREATETIME <= %ld", tCond.tEnd);
	}

	if (tCond.iType >= OP_LOG_USR && tCond.iType <= OP_LOG_ALG)
	{
		sprintf(szWhere[iWhereCnt++], "TYPE = %d", tCond.iType);
	}

	if (strlen(tCond.szLike) > 0)
	{
		sprintf(szWhere[iWhereCnt++], "(USER like '%%%s%%' or FROM like '%%%s%%' or CONTENT like '%%%s%%')", 
			tCond.szLike, tCond.szLike, tCond.szLike);
	}

	string sqlWhere = "";
	for (int i = 0; i < iWhereCnt; i++)
	{
		if (i == 0)
		{
			sqlWhere += " where ";
			sqlWhere += szWhere[i];
		}
		else
		{
			sqlWhere += " and ";
			sqlWhere += szWhere[i];
		}
	}

	string sqlOrder = " order by ROWID desc";

	char szLimit[512] = {0};
	sprintf(szLimit, " limit %d offset %d", tCond.iPageSize, tCond.iPageSize*(tCond.iPageNo-1));
	string sqlLimit = szLimit;

	DbQueryResult tDbResult = {0};
	string sqlContent = sqlSelect + sqlWhere + sqlOrder + sqlLimit + ";";
	//printf("%s %d sqlContent=%s\n", __FILE__, __LINE__,sqlContent.c_str());

	int iRet = MQ_DataBase_Select(MSG_TYPE_MASTER, sqlContent.c_str(), &tDbResult);
	if (iRet < 0)
	{
		LOG_DEBUG_FMT("Select Database failed");
		return -1;
	}

	int nRow = tDbResult.iRow;
	int nCol = tDbResult.iCol;
	//printf("%s %d nRow=%d nCol = %d\n", __FILE__, __LINE__,nRow, nCol);
	if (nCol != 6 && nRow > 0)
	{
		LOG_DEBUG_FMT("Only need 6 columns, nCol = %d",nCol);
		return -1;
	}
	if (nRow > MAX_LOG_PAGE_SIZE)
	{
		nRow = MAX_LOG_PAGE_SIZE;
	}
	if (nRow > tCond.iPageSize)
	{
		nRow = tCond.iPageSize;
	}

	tResult.iCurRecNum = nRow;

	char *pData = (char *)(tDbResult.data);

	for (int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			int len = *((int *)pData);
			pData = (char *)pData + sizeof(int);

			if (len > 1023)
			{
				LOG_ERROR_FMT("table cell size is too long.\n");
				return -1;
			}

			char buf[1024];
			memcpy(buf, pData, len);
			buf[len] = 0;

			//ROWID,CREATETIME,TYPE,USER,ADDR,CONTENT
			if (j == 0)
			{
				tResult.tRecords[i].iRowID = atoi(buf);
			}
			else if (j == 1)
			{
				tResult.tRecords[i].tCreate = atol(buf);
			}
			else if (j == 2)
			{
				tResult.tRecords[i].iType = atoi(buf);
			}
			else if (j == 3)
			{
				strcpy(tResult.tRecords[i].szUser,buf);
			}
			else if (j == 4)
			{
				strcpy(tResult.tRecords[i].szFrom,buf);
			}
			else if (j == 5)
			{
				strcpy(tResult.tRecords[i].szContent,buf);
			}

			pData = pData + len;
		}
	}

	// 查询总数量
	string sqlCount = sqlSelectCout + sqlWhere + ";";
	tResult.iTotalRecNum = MQ_DataBase_Select_Count(MSG_TYPE_MASTER, sqlCount.c_str());
	return 0;
}

int LogManager::ExportLog( const LogQueryCond & tCond, string &filepath )
{
	string sqlSelect = "select ROWID,CREATETIME,TYPE,USER,ADDR,CONTENT from LogTable";
	string sqlSelectCout = "select count(*) from LogTable";

	int iWhereCnt = 0;
	char szWhere[4][512] = {{0}};
	if (tCond.tStart > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CREATETIME >= %ld", tCond.tStart);
	}

	if (tCond.tEnd > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CREATETIME <= %ld", tCond.tEnd);
	}

	if (tCond.iType >= OP_LOG_USR && tCond.iType <= OP_LOG_ALG)
	{
		sprintf(szWhere[iWhereCnt++], "TYPE = %d", tCond.iType);
	}

	if (strlen(tCond.szLike) > 0)
	{
		sprintf(szWhere[iWhereCnt++], "(USER like '%%%s%%' or FROM like '%%%s%%' or CONTENT like '%%%s%%')", 
			tCond.szLike, tCond.szLike, tCond.szLike);
	}

	string sqlWhere = "";
	for (int i = 0; i < iWhereCnt; i++)
	{
		if (i == 0)
		{
			sqlWhere += " where ";
			sqlWhere += szWhere[i];
		}
		else
		{
			sqlWhere += " and ";
			sqlWhere += szWhere[i];
		}
	}

	string sqlOrder = " order by ROWID desc";

	string sqlCount = sqlSelectCout + sqlWhere + ";";
	int iTotalRec = MQ_DataBase_Select_Count(MSG_TYPE_MASTER, sqlCount.c_str());
	if (iTotalRec < 1)
	{
		return -2;
	}

	LogRecord * pLogRecordList = (LogRecord *)malloc(iTotalRec * sizeof(LogRecord));
	if (pLogRecordList == NULL)
	{
		LOG_ERROR_FMT("malloc error");
		return -1;
	}
	memset(pLogRecordList, 0, iTotalRec * sizeof(LogRecord));

	int iPageNo = 0;
	int iRead = 0;
	while (iRead < iTotalRec && iPageNo < (iTotalRec/20+1))
	{
		char szLimit[512] = {0};
		sprintf(szLimit, " limit 20 offset %d", 20*iPageNo);
		string sqlLimit = szLimit;

		iPageNo++;

		string sqlContent = sqlSelect + sqlWhere + sqlOrder + sqlLimit + ";";
		//printf("%s %d sqlContent=%s\n", __FILE__, __LINE__,sqlContent.c_str());

		DbQueryResult tDbResult = {0};
		int iRet = MQ_DataBase_Select(MSG_TYPE_MASTER, sqlContent.c_str(), &tDbResult);
		if (iRet < 0)
		{
			break;
		}

		int nRow = tDbResult.iRow;
		int nCol = tDbResult.iCol;
		//printf("%s %d nRow=%d nCol=%d\n", __FILE__, __LINE__,nRow, nCol);
		if (nCol != 6 || nRow < 1)
		{
			break;
		}
		if (nRow > MAX_LOG_PAGE_SIZE)
		{
			nRow = MAX_LOG_PAGE_SIZE;
		}

		char *pData = (char *)(tDbResult.data);
		for (int i = 0; i < nRow; i++)
		{
			LogRecord * pRec = pLogRecordList + iRead;
			iRead++;
			for (int j = 0; j < nCol; j++)
			{
				int len = *((int *)pData);
				pData = (char *)pData + sizeof(int);

				char buf[1024] = {0};
				memcpy(buf, pData, len);
				buf[len] = 0;

				//ROWID,CREATETIME,TYPE,USER,ADDR,CONTENT
				if (j == 0)
				{
					pRec->iRowID = atoi(buf);
				}
				else if (j == 1)
				{
					pRec->tCreate = atol(buf);
				}
				else if (j == 2)
				{
					pRec->iType = atoi(buf);
				}
				else if (j == 3)
				{
					strcpy(pRec->szUser,buf);
				}
				else if (j == 4)
				{
					strcpy(pRec->szFrom,buf);
				}
				else if (j == 5)
				{
					strcpy(pRec->szContent,buf);
				}

				pData = pData + len;
			}
		}
	}

	if (iRead < 1)
	{
		free(pLogRecordList);
		pLogRecordList = NULL;
		return -2;
	}

	// 生成EXCEL文件
	char szExcelFile[256] = {0};
	CreateExportFilePath(szExcelFile);
	int iRet = WriteLog2Excel(pLogRecordList, iRead, szExcelFile);
	free(pLogRecordList);
	pLogRecordList = NULL;

	filepath = szExcelFile;
	return iRet;
}

int LogManager::DeleteLog( int iRowID )
{
	char sqlDelete[128] = {0};
	if (iRowID >= 0)
	{
		sprintf(sqlDelete,"delete from LogTable where ROWID = %d", iRowID);
	}
	else
	{
		sprintf(sqlDelete,"delete from LogTable");
	}

	int iRet = MQ_DataBase_Delete(MSG_TYPE_MASTER, sqlDelete);
	if (iRet < 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int LogManager::WriteOpLog( int iType, const char * user, const char * remote, const char * msg )
{
	if (msg == NULL)
	{
		return  -1;
	}

	char szUtf8User[256] = {0};
	if (user)
	{
		GB2312_To_UTF8((char *)user, strlen(user), szUtf8User, sizeof(szUtf8User));
	}
	else
	{
		strcpy(szUtf8User, "unknown");
	}

	if (remote == NULL)
	{
		remote = "unknown";
	}

	char szUtf8Msg[1024 * 2] = {0};
	GB2312_To_UTF8((char *)msg, strlen(msg), szUtf8Msg, sizeof(szUtf8Msg));

	time_t tNow = time(NULL);
	char sqlInsert[1024 * 2] = {0};
	sprintf(sqlInsert, "INSERT INTO LogTable(CREATETIME,TYPE,USER,ADDR,CONTENT) "
		"VALUES(%ld, %d, '%s', '%s', '%s')",
		tNow, iType, szUtf8User, remote, szUtf8Msg);

	int iRet = MQ_DataBase_Insert(MSG_TYPE_MASTER, sqlInsert);
	if (iRet < 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int LogManager::ClearExportDir()
{
	char szDir[128] = {0};
	sprintf(szDir, "%s/%s/", DEFAULT_ROOT_PATH, LOGEXPORT_DIR);

	char szCmd[128] = {0};
	sprintf(szCmd, "rm -f %s*", szDir);
	system(szCmd);
	return 0;
}

int LogManager::CreateExportFilePath( char *pszFilePath )
{
	static int siIndex = 0;
	if (pszFilePath == NULL)
	{
		return -1;
	}
	siIndex++;
	siIndex %= 1000;
	char szTime[40] = {0};
	TimeFormatString(time(NULL), szTime, 40, eYMDHMS2);
	
	sprintf(pszFilePath, "%s/%s/%s_%03d.xls", DEFAULT_ROOT_PATH, LOGEXPORT_DIR,szTime,siIndex);

	OAL_MakeDirByPath(pszFilePath);

	return 0;
}

