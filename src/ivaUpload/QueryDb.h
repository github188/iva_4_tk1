#ifndef __QUERY_DATABASE_H__
#define __QUERY_DATABASE_H__

#include "mq_upload.h"

typedef struct 
{
	int	iRowID;
	time_t tCreate;
	int    tCreateMesc;

	int iDataType;
	int iEventID;
	int iChnID;
	int iSceneID;

	char szPlate[PLATE_LEN];
	int iPlateColor;
	int iPlateType;
	int iPlateImgIndex;
	RectT tPlatePos;
	int iPlateConfidence;

	int iCarType;
	int iCarColor;
	int iCarBrand;
	int iCarSpeed;

	int  iImgNum;
	char szImgRoot[128];
	char szImgPath[MAX_IMG_NUM][128];

	int   iSnapNum;
	char  szSnapTime[MAX_IMG_NUM][24];
	int   iSnapType[MAX_IMG_NUM];
	RectT tSnapPos[MAX_IMG_NUM];

	int iRecNum;
	char szRecRoot[128];
	char szRecPath[MAX_REC_NUM][128];

	char szDevID[64];
	char szRegID[64];
	char szNetID[64];
	char szManID[64];

	char szAreaName[256];
	char szAreaID[128];
	char szPlaceName[256];
	char szPlaceID[128];
	char szMonitoryName[256];
	char szMonitoryID[128];

	char  szSceneName[256];
	int   iDirection;

	char szCode[16];
	char szDesc[1024];

	int iStatus;
}AnalyDbRecord;

int GetOneNeedUploadRecord(AnalyDbRecord* ptEventData);

int UpdataRecordStatus(int iRowID, int iStatus);


typedef struct _AlarmDbRecord
{
	int iRowID;
	int iType;
	int iLevel;
	time_t tCreate;
	char szContent[250];
}AlarmDbRecord;


#endif // !__QUERY_DATABASE_H__

