#include "AlarmManager.h"
#include "mq_database.h"
#include "oal_log.h"
#include "oal_unicode.h"
#include "WriteExcel.h"
#include <assert.h>
#include "capacity.h"
#include "oal_time.h"
#include "oal_file.h"

#define  ALARMEXPORT_DIR		"AlarmExport"

AlarmManager* AlarmManager::m_pInstance = NULL;

AlarmManager::AlarmManager()
{
	ClearExportDir();
}

AlarmManager::~AlarmManager()
{
}

AlarmManager* AlarmManager::GetInstance()
{
	return m_pInstance;
}

int AlarmManager::Initialize()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new AlarmManager();
		assert(m_pInstance);
	}
	return 0;
}

void AlarmManager::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

int AlarmManager::QueryAlarm( const AlarmQueryCond & tCond, AlarmQueryResult &tResult )
{
	memset(&tResult, 0, sizeof(AlarmQueryResult));

	string sqlSelect = "select ROWID,CREATETIME,CONTENT from AlarmTable";
	string sqlSelectCout = "select count(*) from AlarmTable";

	int iWhereCnt = 0;
	char szWhere[3][512] = {{0}};
	if (tCond.tStart > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CREATETIME >= %ld", tCond.tStart);
	}

	if (tCond.tEnd > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CREATETIME <= %ld", tCond.tEnd);
	}

	if (strlen(tCond.szLike) > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CONTENT like '%%%s%%'", tCond.szLike);
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
	int iRet = MQ_DataBase_Select(MSG_TYPE_MASTER, sqlContent.c_str(), &tDbResult);
	if (iRet < 0)
	{
		LOG_DEBUG_FMT("Select Database failed");
		return -1;
	}

	int nRow = tDbResult.iRow;
	int nCol = tDbResult.iCol;
	if (nCol != 6 && nRow > 0)
	{
		LOG_DEBUG_FMT("Only need 6 columns, nCol = %d",nCol);
		return -1;
	}
	if (nRow > MAX_ALARM_PAGE_SIZE)
	{
		nRow = MAX_ALARM_PAGE_SIZE;
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

			//ROWID,CREATETIME,CONTENT
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

int AlarmManager::ExportAlarm( const AlarmQueryCond & tCond, string &filepath )
{
	string sqlSelect = "select ROWID,CREATETIME,CONTENT from AlarmTable";
	string sqlSelectCout = "select count(*) from AlarmTable";

	int iWhereCnt = 0;
	char szWhere[3][512] = {{0}};
	if (tCond.tStart > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CREATETIME >= %ld", tCond.tStart);
	}

	if (tCond.tEnd > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CREATETIME <= %ld", tCond.tEnd);
	}

	if (strlen(tCond.szLike) > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CONTENT like '%%%s%%'", tCond.szLike);
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

	AlarmRecord * pAlarmRecordList = (AlarmRecord*)malloc(iTotalRec * sizeof(AlarmRecord));
	if (pAlarmRecordList == NULL)
	{
		LOG_ERROR_FMT("malloc error");
		return -1;
	}
	memset(pAlarmRecordList, 0, iTotalRec * sizeof(AlarmRecord));

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
			AlarmRecord * pRec = pAlarmRecordList + iRead;
			iRead++;
			for (int j = 0; j < nCol; j++)
			{
				int len = *((int *)pData);
				pData = (char *)pData + sizeof(int);

				char buf[1024] = {0};
				memcpy(buf, pData, len);
				buf[len] = 0;

				//ROWID,CREATETIME,CONTENT
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
					strcpy(pRec->szContent,buf);
				}

				pData = pData + len;
			}
		}
	}

	if (iRead < 1)
	{
		free(pAlarmRecordList);
		pAlarmRecordList = NULL;
		return -2;
	}

	// 生成EXCEL文件
	char szExcelFile[256] = {0};
	CreateExportFilePath(szExcelFile);
	int iRet = WriteAlarm2Excel(pAlarmRecordList, iRead, szExcelFile);
	free(pAlarmRecordList);
	pAlarmRecordList = NULL;

	filepath = szExcelFile;
	return iRet;

}

int AlarmManager::DeleteAlarm( int iRowID )
{
	char sqlDelete[128] = {0};
	if (iRowID >= 0)
	{
		sprintf(sqlDelete,"delete from AlarmTable where ROWID = %d", iRowID);
	}
	else
	{
		sprintf(sqlDelete,"delete from AlarmTable");
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

int AlarmManager::WriteAlarm( const char * msg )
{
	if (msg == NULL)
	{
		return  -1;
	}

	char szUtf8Msg[1024 * 2] = {0};
	GB2312_To_UTF8((char *)msg, strlen(msg), szUtf8Msg, sizeof(szUtf8Msg));

	time_t tNow = time(NULL);
	char sqlInsert[1024 * 2] = {0};
	sprintf(sqlInsert, "INSERT INTO AlarmTable(CREATETIME, CONTENT) VALUES(%ld, '%s')",
			tNow, szUtf8Msg);

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

int AlarmManager::ClearExportDir()
{
	char szDir[128] = {0};
	sprintf(szDir, "%s/%s/", DEFAULT_ROOT_PATH, ALARMEXPORT_DIR);

	char szCmd[128] = {0};
	sprintf(szCmd, "rm -f %s*", szDir);
	system(szCmd);
	return 0;
}

int AlarmManager::CreateExportFilePath( char *pszFilePath )
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
	
	sprintf(pszFilePath, "%s/%s/%s_%03d.xls", DEFAULT_ROOT_PATH, ALARMEXPORT_DIR,szTime,siIndex);
	
	OAL_MakeDirByPath(pszFilePath);

	return 0;

}

