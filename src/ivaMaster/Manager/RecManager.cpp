#include "RecManager.h"
#include "oal_log.h"
#include "oal_file.h"
#include "mq_master.h"
#include "mq_database.h"
#include <string>
#include "DeviceManager.h"
#include "SystemMaintain.h"
using namespace std;

RecManager* RecManager::m_pInstance = NULL;

RecManager::RecManager()
{
	pthread_mutex_init(&m_tMutex, NULL);
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_iAllStatus[i] = REC_STOP;
	}
}

RecManager::~RecManager()
{
	pthread_mutex_destroy(&m_tMutex);
}

RecManager* RecManager::GetInstance()
{
	return m_pInstance;
}

int RecManager::Initialize()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new RecManager();
	}
	return 0;
}

void RecManager::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

int RecManager::GetChnStatus( int iChnID )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	int iStat = 0;
	pthread_mutex_lock(&m_tMutex);
	iStat = m_iAllStatus[iChnID];
	pthread_mutex_unlock(&m_tMutex);

	return iStat;
}

int RecManager::SetChnStatus( int iChnID, int iStatus )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || iStatus < REC_STOP || iStatus > REC_ABNORMAL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	m_iAllStatus[iChnID] = iStatus;
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int RecManager::QueryRec( const RecQueryCond & tCond, RecQueryResult &tResult )
{
	memset(&tResult, 0, sizeof(RecQueryResult));

	string sqlSelect = "select ROWID,CHNID,TYPE,BEGINTIME,ENDTIME,FILEPATH from RecordTable";
	string sqlSelectCout = "select count(*) from RecordTable";

	int iWhereCnt = 0;
	char szWhere[4][512] = {{0}};

	if (tCond.iChnID > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CHNID = %d", tCond.iChnID-1);
	}

	if (tCond.tStart > 0)
	{
		sprintf(szWhere[iWhereCnt++], "ENDTIME >= %ld", tCond.tStart);
	}

	if (tCond.tEnd > 0)
	{
		sprintf(szWhere[iWhereCnt++], "BEGINTIME <= %ld", tCond.tEnd);
	}

	if (tCond.iType >= 0)
	{
		sprintf(szWhere[iWhereCnt++], "TYPE = %d", tCond.iType);
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

	string sqlOrder = " order by BEGINTIME desc";

	char szLimit[512] = {0};
	sprintf(szLimit, " limit %d offset %d", tCond.iPageSize, tCond.iPageSize*(tCond.iPageNo-1));
	string sqlLimit = szLimit;

	DbQueryResult tDbResult;
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
	if (nRow > MAX_REC_PAGE_SIZE)
	{
		nRow = MAX_REC_PAGE_SIZE;
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

			//ROWID,CHNID,TYPE,BEGINTIME,ENDTIME,FILEPATH
			if (j == 0)
			{
				tResult.tRecords[i].iRowID = atoi(buf);
			}
			else if (j == 1)
			{
				tResult.tRecords[i].iChnID = atoi(buf);
			}
			else if (j == 2)
			{
				tResult.tRecords[i].iType = atoi(buf);
			}
			else if (j == 3)
			{
				tResult.tRecords[i].tStart = atol(buf);
			}
			else if (j == 4)
			{
				tResult.tRecords[i].tEnd = atol(buf);
			}
			else if (j == 5)
			{
				strcpy(tResult.tRecords[i].szPath, buf);
			}

			pData = pData + len;
		}
	}

	// 查询总数量
	string sqlCount = sqlSelectCout + sqlWhere + ";";
	tResult.iTotalRecNum = MQ_DataBase_Select_Count(MSG_TYPE_MASTER, sqlCount.c_str());
	return 0;
}

int RecManager::SaveAsRec( int iRowID, int iDiskID )
{
	char szFilePath[256] = {0};

	if (iRowID >= 0 || (iDiskID != DISK_SD && iDiskID != DISK_USB))
	{
		int iRet = QueryRecFilePath(iRowID, szFilePath);
		if (iRet != 0)
		{
			return 0;
		}

		if (iDiskID == DISK_USB && strstr(szFilePath, MOUNT_POINT_USB)!=NULL)
		{
			return 0;
		}

		if (iDiskID == DISK_SD && strstr(szFilePath, MOUNT_POINT_SD)!=NULL)
		{
			return 0;
		}

		DiskShow tShow = {0};
		DeviceManager::GetInstance()->GetDiskShowByType(iDiskID, &tShow);
		if (tShow.iStat != DISK_STAT_OK)
		{
			return -2;
		}

		char szDistFilePath[256] = {0};
		if (iDiskID == DISK_USB)
			sprintf(szDistFilePath, "%s/saveas/", MOUNT_POINT_USB);
		else if (iDiskID == DISK_SD)
			sprintf(szDistFilePath, "%s/saveas/", MOUNT_POINT_SD);
			
		OAL_MakeDirByPath(szDistFilePath);

		char szCpCmd[1024] = {0};
		sprintf(szCpCmd, "cp %s %s", szFilePath, szDistFilePath);

		return SystemMaintain::GetInstance()->AddDelayCmd(szCpCmd, 1);
	}
	
	return -1;
}

int RecManager::DeleteRec( int iRowID )
{
	char szFilePath[256] = {0};

	char sqlDelete[128] = {0};
	if (iRowID >= 0)
	{
		int iRet = QueryRecFilePath(iRowID, szFilePath);
		if (iRet != 0)
		{
			return 0;
		}

		sprintf(sqlDelete,"delete from RecordTable where ROWID = %d", iRowID);
	}
	else
	{

		sprintf(sqlDelete,"delete from RecordTable");
	}

	int iRet = MQ_DataBase_Delete(MSG_TYPE_MASTER, sqlDelete);
	if (iRet < 0)
	{
		return -1;
	}
	else
	{
		if (iRowID >= 0)
		{
			OAL_DeleteFile(szFilePath);// 删除对应的文件
		}
		else
		{
			DeviceManager::GetInstance()->DeleteAllRecord();// 删除所有的录像文件
		}

		return 0;
	}
}

int RecManager::QueryRecFilePath( int iRowID, char * pszFilePath )
{
	if (pszFilePath == NULL)
	{
		return -1;
	}

	string sqlSelect = "select ROWID,FILEPATH from RecordTable";
	char szWhere[128] = {0};
	sprintf(szWhere, " where ROWID = %d", iRowID);

	DbQueryResult tDbResult;
	string sqlContent = sqlSelect + szWhere + ";";
	int iRet = MQ_DataBase_Select(MSG_TYPE_MASTER, sqlContent.c_str(), &tDbResult);
	if (iRet < 0)
	{
		LOG_DEBUG_FMT("Select Database failed");
		return -1;
	}

	int nRow = tDbResult.iRow;
	int nCol = tDbResult.iCol;
	if (nCol != 2 && nRow > 0)
	{
		LOG_DEBUG_FMT("Only need 2 columns, nCol = %d",nCol);
		return -1;
	}
	if (nRow != 1)
	{
		return -1;
	}

	char *pData = (char *)(tDbResult.data);

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

		//ROWID,FILEPATH
		if (j == 0)
		{
		}
		else if (j == 1)
		{
			strcpy(pszFilePath, buf);
		}

		pData = pData + len;
	}
	return 0;
}

