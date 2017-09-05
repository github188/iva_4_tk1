#include "AnalyManager.h"
#include "mq_database.h"
#include "mq_master.h"
#include "oal_log.h"

AnalyManager* AnalyManager::m_pInstance = NULL;

AnalyManager::AnalyManager()
{
	pthread_mutex_init(&m_tMutex, NULL);
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_iAllStatus[i] = ANALY_STOP;
	}
}

AnalyManager::~AnalyManager()
{
	pthread_mutex_destroy(&m_tMutex);
}

AnalyManager* AnalyManager::GetInstance()
{
	return m_pInstance;
}

int AnalyManager::Initialize()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new AnalyManager();
	}
	return 0;
}

void AnalyManager::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

int AnalyManager::SetAnalyVersion( const char * ver )
{

}

int AnalyManager::GetAnalyVersion( char * ver )
{

}

int AnalyManager::SetAnalyStatus( int iChnID, int iStatus )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || iStatus < ANALY_STOP || iStatus > ANALY_ABNORMAL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	m_iAllStatus[iChnID] = iStatus;
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int AnalyManager::GetAnalyStatus( int iChnID )
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

int AnalyManager::Query( const AnalyQueryCond & tCond, AnalyQueryResult &tResult )
{
	memset(&tResult, 0, sizeof(AnalyQueryResult));

	string sqlSelect = "select ROWID,CHNID,EVENTID,DATATYPE,PLATE,PLATECOLOR,CREATETIME,"
					   "PICNUM,PICPATH,PIC1NAME,PIC2NAME,PIC3NAME,PIC4NAME,"
					   "RECNUM,RECPATH,REC1NAME,REC2NAME,"
					   "STATUS from AnalyTable";
	string sqlSelectCout = "select count(*) from AnalyTable";

	int iWhereCnt = 0;
	char szWhere[7][512] = {{0}};

	if (tCond.iChnID > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CHNID = %d", tCond.iChnID-1);
	}

	if (tCond.iEventID > 0)
	{
		sprintf(szWhere[iWhereCnt++], "EVENTID = %d", tCond.iEventID-1);
	}

	if (tCond.iDataType > 0)
	{
		sprintf(szWhere[iWhereCnt++], "DATATYPE = %d", tCond.iDataType);
	}

	if (tCond.iPlateColor > 0)
	{
		sprintf(szWhere[iWhereCnt++], "PLATECOLOR = %d", tCond.iPlateColor-1);
	}

	if (tCond.tStart > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CREATETIME >= %ld", tCond.tStart);
	}

	if (tCond.tEnd > 0)
	{
		sprintf(szWhere[iWhereCnt++], "CREATETIME <= %ld", tCond.tEnd);
	}

	if (strlen(tCond.szPlate) > 0)
	{
		sprintf(szWhere[iWhereCnt++], "PLATE like '%%%s%%'", tCond.szPlate);
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
	if (nCol != 18 && nRow > 0)
	{
		LOG_DEBUG_FMT("Only need 18 columns, nCol = %d",nCol);
		return -1;
	}
	if (nRow > MAX_ANALY_PAGE_SIZE)
	{
		nRow = MAX_ANALY_PAGE_SIZE;
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

			//ROWID,CHNID,EVENTID,DATATYPE,PLATE,PLATECOLOR,CREATETIME
			//PICNUM,PICPATH,PIC1NAME,PIC2NAME,PIC3NAME,PIC4NAME,
			//RECNUM,RECPATH,REC1NAME,REC2NAME,STATUS 
			if (j == 0)//ROWID
			{
				tResult.tRecords[i].iRowID = atoi(buf);
			}
			else if (j == 1)//CHNID
			{
				tResult.tRecords[i].iChnID = atoi(buf);
			}
			else if (j == 2)//EVENTID
			{
				tResult.tRecords[i].iEventID = atoi(buf);
			}
			else if (j == 3)//DATATYPE
			{
				tResult.tRecords[i].iDataType = atoi(buf);
			}
			else if (j == 4)//PLATE
			{
				strcpy(tResult.tRecords[i].szPlate, buf);
			}
			else if (j == 5)//PLATECOLOR
			{
				tResult.tRecords[i].iPlateColor = atoi(buf);
			}
			else if (j == 6)//CREATETIME
			{
				tResult.tRecords[i].tCreate = atol(buf);
			}
			else if (j == 7)//PICNUM
			{
				tResult.tRecords[i].iImgNum = atoi(buf);
			}
			else if (j == 8)//PICPATH
			{
				strcpy(tResult.tRecords[i].szImgRoot, buf);
			}
			else if (j == 9)//PIC1NAME
			{
				strcpy(tResult.tRecords[i].szImgPath[0], buf);
			}
			else if (j == 10)//PIC2NAME
			{
				strcpy(tResult.tRecords[i].szImgPath[1], buf);
			}
			else if (j == 11)//PIC3NAME
			{
				strcpy(tResult.tRecords[i].szImgPath[2], buf);
			}
			else if (j == 12)//PIC4NAME
			{
				strcpy(tResult.tRecords[i].szImgPath[3], buf);
			}
			else if (j == 13)//RECNUM
			{
				tResult.tRecords[i].iRecNum = atoi(buf);
			}
			else if (j == 14)//RECPATH
			{
				strcpy(tResult.tRecords[i].szRecRoot, buf);
			}
			else if (j == 15)//REC1NAME
			{
				strcpy(tResult.tRecords[i].szRecPath[0], buf);
			}
			else if (j == 16)//REC2NAME
			{
				strcpy(tResult.tRecords[i].szRecPath[1], buf);
			}
			else if (j == 17)//STATUS
			{
				tResult.tRecords[i].iStatus = atoi(buf);
			}
			pData = pData + len;
		}
	}

	// 查询总数量
	string sqlCount = sqlSelectCout + sqlWhere + ";";
	tResult.iTotalRecNum = MQ_DataBase_Select_Count(MSG_TYPE_MASTER, sqlCount.c_str());
	return 0;
}

int AnalyManager::ReUpload( int iRowID )
{
	char sqlReUpload[128] = {0};
	sprintf(sqlReUpload,"update AnalyTable set STATUS = %d where ROWID = %d;", eUnUp, iRowID);

	int iRet = MQ_DataBase_Delete(MSG_TYPE_MASTER, sqlReUpload);
	if (iRet < 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int AnalyManager::Delete( int iRowID )
{
	char sqlDelete[128] = {0};
	if (iRowID >= 0)
	{
		sprintf(sqlDelete,"delete from AnalyTable where ROWID = %d", iRowID);
	}
	else
	{
		sprintf(sqlDelete,"delete from AnalyTable");
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


