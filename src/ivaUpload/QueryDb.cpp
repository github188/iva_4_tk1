#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "QueryDb.h"
#include "mq_database.h"
#include "oal_log.h"

static int get_feild_value_int(const char *msg_data, int &int_value)
{
    char buf_temp[256] = {0};
	int len = -1;
	
	if(msg_data == NULL)
	{
		return -1;
	}

	const char * data = msg_data;
	
    len = *((int *)data);
	if(len < 0 || len > MSG_DATA_SIZE)
	{
		return -1;
	}
	
    data += sizeof(int);
	
    memcpy(buf_temp, data, len);
	
    int_value = atoi(buf_temp);

	return len;
}

static int get_feild_value_str(const char *msg_data,char *str_value,int str_len)
{
	int len = -1;
	
	if(msg_data == NULL || str_value == NULL)
	{
		return -1;
	}

	const char * data = msg_data;
	
    len = *((int *)data);
	if(len < 0 || len > MSG_DATA_SIZE)
	{
		return -1;
	}
	
    data += sizeof(int);

	if(len < str_len)
    {
    	memcpy(str_value, data, len);

	}
	else
	{
    	memcpy(str_value, data, str_len-1);

		str_value[str_len-1] = '\0';
	}
	
	return len;
}

#define GetFeildValueInt(data, feild) \
{\
	int len = get_feild_value_int(data, (feild));\
	if(len < 0) return cnt;\
	data += sizeof(int) + len;\
	cnt++;\
}\

#define GetFeildValueTime(data, feild) \
{\
	int iValue = 0;\
	int len = get_feild_value_int(data, iValue);\
	if(len < 0) return cnt;\
	data += sizeof(int) + len;\
	cnt++;\
	feild = (time_t)iValue;\
}\

#define GetFeildValueString(data, feild) \
{\
	int len = get_feild_value_str(data,feild,sizeof(feild));\
	if(len < 0) return cnt;\
	data += sizeof(int) + len;\
	cnt++;\
}\

static int get_table_value(const char *msg_data, AnalyDbRecord *pData)
{
    if((NULL == msg_data) || (NULL == pData))
    {
        return -1;
    }
	
	const char * data = msg_data;
	int cnt = 0;

	/*ROWID,CREATETIME,CREATEMESC,"
	"DATATYPE,EVENTID,CHNID,SCENEID,"
	"PLATE,PLATECOLOR,PLATETYPE,"
	"PLATEPIC,PLATEPOS,CONFIDENCE,"
	"CARTYPE,CARCOLOR,BRAND,SPEED,"
	"SNAPNUM,PICNUM,PICPATH,"
	"PIC1NAME,PIC1TIME,PIC1POS,SNAP1TYPE,"
	"PIC2NAME,PIC2TIME,PIC2POS,SNAP2TYPE,"
	"PIC3NAME,PIC3TIME,PIC3POS,SNAP3TYPE,"
	"PIC4NAME,PIC4TIME,PIC4POS,SNAP4TYPE,"
	"RECNUM,RECPATH,REC1NAME,REC2NAME,"
	"NETNO,REGNO,DEVNO,MANNO,"
	"AREAID,AREANAME,MONID,MONNAME,CROSSID,CROSSNAME,"		
	"SCENENAME,DIRECTION,"
	"CODE,DESC"
	"STATUS*/
	GetFeildValueInt(data, pData->iRowID);
	GetFeildValueTime(data, pData->tCreate);
	GetFeildValueTime(data, pData->tCreateMesc);
	GetFeildValueInt(data, pData->iDataType);
	GetFeildValueInt(data, pData->iEventID);
	GetFeildValueInt(data, pData->iChnID);
	GetFeildValueInt(data, pData->iSceneID);
	GetFeildValueString(data, pData->szPlate);
	GetFeildValueInt(data, pData->iPlateColor);
	GetFeildValueInt(data, pData->iPlateType);
	GetFeildValueInt(data, pData->iPlateImgIndex);

	char szPlatePos[256] = {0};
	GetFeildValueString(data, szPlatePos);

	sscanf(szPlatePos, "%d,%d,%d,%d", 
		&pData->tPlatePos.x, &pData->tPlatePos.y,
		&pData->tPlatePos.w, &pData->tPlatePos.h);

	GetFeildValueInt(data, pData->iPlateConfidence);
	GetFeildValueInt(data, pData->iCarType);
	GetFeildValueInt(data, pData->iCarColor);
	GetFeildValueInt(data, pData->iCarBrand);
	GetFeildValueInt(data, pData->iCarSpeed);
	GetFeildValueInt(data, pData->iSnapNum);
	GetFeildValueInt(data, pData->iImgNum);
	GetFeildValueString(data, pData->szImgRoot);

	for (int i = 0; i < MAX_IMG_NUM; i++)
	{
		GetFeildValueString(data, pData->szImgPath[i]);
		GetFeildValueString(data, pData->szSnapTime[i]);

		char szPos[256] = {0};
		GetFeildValueString(data, szPos);

		sscanf(szPos, "%d,%d,%d,%d", 
			&pData->tSnapPos[i].x, &pData->tSnapPos[i].y,
			&pData->tSnapPos[i].w, &pData->tSnapPos[i].h);

		GetFeildValueInt(data, pData->iSnapType[i]);
	}

	GetFeildValueInt(data, pData->iRecNum);
	GetFeildValueString(data, pData->szRecRoot);

	for (int i = 0; i < MAX_REC_NUM; i++)
	{
		GetFeildValueString(data, pData->szRecPath[i]);
	}

	GetFeildValueString(data, pData->szNetID);
	GetFeildValueString(data, pData->szRegID);
	GetFeildValueString(data, pData->szDevID);
	GetFeildValueString(data, pData->szManID);

	GetFeildValueString(data, pData->szAreaID);
	GetFeildValueString(data, pData->szAreaName);
	GetFeildValueString(data, pData->szMonitoryID);
	GetFeildValueString(data, pData->szMonitoryName);
	GetFeildValueString(data, pData->szPlaceID);
	GetFeildValueString(data, pData->szPlaceName);

	GetFeildValueString(data, pData->szSceneName);
	GetFeildValueInt(data, pData->iDirection);

	GetFeildValueString(data, pData->szCode);
	GetFeildValueString(data, pData->szDesc);
 
	GetFeildValueInt(data, pData->iStatus);
	return cnt;
}

static int My_DataBase_Search_Record(int iRowID, int iStatus, DbQueryResult *ptResult)
{
    if(ptResult == NULL)
    {
        return -1;
    }

    /* 此查询语句, 每次返回一条数据 */
	char szSelect[2048] = {0};
	sprintf(szSelect,"select ROWID,CREATETIME,CREATEMESC,"
					"DATATYPE,EVENTID,CHNID,SCENEID,"
					"PLATE,PLATECOLOR,PLATETYPE,"
					"PLATEPIC,PLATEPOS,CONFIDENCE,"
					"CARTYPE,CARCOLOR,BRAND,SPEED,"
					"SNAPNUM,PICNUM,PICPATH,"
					"PIC1NAME,PIC1TIME,PIC1POS,SNAP1TYPE,"
					"PIC2NAME,PIC2TIME,PIC2POS,SNAP2TYPE,"
					"PIC3NAME,PIC3TIME,PIC3POS,SNAP3TYPE,"
					"PIC4NAME,PIC4TIME,PIC4POS,SNAP4TYPE,"
					"RECNUM,RECPATH,REC1NAME,REC2NAME,"
					"NETNO,REGNO,DEVNO,MANNO,"
					"AREAID,AREANAME,MONID,MONNAME,CROSSID,CROSSNAME,"		
					"SCENENAME,DIRECTION,"
					"CODE,DESC,"
					"STATUS from AnalyTable "
					"where ROWID > %d and STATUS = %d "
					"order by CREATETIME limit 1;",
					iRowID, iStatus);

    return MQ_DataBase_Select(MSG_TYPE_UPLOAD, szSelect, ptResult);
}

static int My_DataBase_Result_Parse(const DbQueryResult *ptResult, AnalyDbRecord *ptEventData)
{
    
    if(ptResult == NULL || ptEventData == NULL)
    {
        return -1;
    }
    
	int NEED_COL = 55;
	int nRow = ptResult->iRow;
	int nCol = ptResult->iCol;
	//printf("2 %p ROW:%d COL:%d LEN:%d\n",ptResult, ptResult->iRow,ptResult->iCol,ptResult->iLen);

	if(nRow == 0)
	{
		return -2;// 无数据
	}

	if(nRow != 1 || nCol != NEED_COL)
	{
		LOG_ERROR_FMT("Row Need %d,is %d;Col Need %d,is %d\n",1,nRow,NEED_COL,nCol);
		return -1;
	}

	int iRet = get_table_value((const char *)ptResult->data, ptEventData);
    if(NEED_COL != iRet)
    {
		LOG_ERROR_FMT("Col Need %d,is %d\n",NEED_COL, iRet);
		return -1;
    }
    return 0;
}

int GetOneNeedUploadRecord(AnalyDbRecord* ptEventData)
{
	static int sRowID = 0;
	static int sInited = 0;
    if (NULL == ptEventData)
    {
        return -1;
    }

	DbQueryResult tDbRes;
	memset(&tDbRes, 0, sizeof(DbQueryResult));
	
	/* 每次查询一条记录 */
	int ret = My_DataBase_Search_Record(sRowID, eUnUp, &tDbRes);
	if (ret < 0)
	{
		return -1;
	}
	
	memset(ptEventData, 0, sizeof(AnalyDbRecord));
	ret = My_DataBase_Result_Parse(&tDbRes, ptEventData);
	if (ret == -2)
	{
		if(sRowID != 0 || sInited == 0)
		{
			LOG_DEBUG_FMT("No new data, sRowID = %d", sRowID);
			sInited = 1;
			sRowID = 0;
		}
		return -1;
	}
	else if (ret != 0)
	{
		printf("My_DataBase_Result_Parse failed,sRowID=%d\n",sRowID);
		sRowID++;// 跳过错误的记录
		return -1;
	}

	sInited = 1;
	sRowID++;
	return 0;
}

int UpdataRecordStatus(int iRowID, int iStatus)
{
	char sql[200] = {0};
	sprintf(sql,"update AnalyTable set STATUS = %d where ROWID = %d;", iStatus, iRowID);
	return MQ_DataBase_Update(MSG_TYPE_UPLOAD, sql);
}

