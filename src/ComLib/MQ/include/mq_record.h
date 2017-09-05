#ifndef __MQ_RECORD_H__
#define __MQ_RECORD_H__

#include "mq_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	RECORD_CMD_START = RECORD_CMD,
	RECORD_CMD_LICENSE,			// MASTER->RECORD
	RECORD_CMD_SOURCE_CHANGED,	// MASTER->RECORD
	RECORD_CMD_SET_STRATEGY,	// MASTER->RECORD
	RECORD_CMD_SET_PLAN,		// MASTER->RECORD
	RECORD_CMD_SET_ENABLE,		// MASTER->RECORD
	RECORD_CMD_SET_ROOTPATH,	// MASTER->RECORD
	RECORD_CMD_SET_FULLSTAT,	// MASTER->RECORD
	RECORD_CMD_END
};

typedef struct _RecSource
{
	bool bEnable;
	bool bChanged;
}RecSrcStat;

enum {
	REC_FILE_MP4 = 0,		// MP4��ʽ
	REC_FILE_AVI = 1,		// AVI��ʽ
	REC_FILE_FLV = 2,		// FLV��ʽ

};

enum {
	REC_PACKET_SIZE = 0,	// ����С���
	REC_PACKET_TIME = 1		// ��ʱ�䳤�ȴ��
};


enum
{
	eProhibit  = 0,	// ����
	ePermanent = 1, // ����
	eTemporary = 2	// ��ʱ
};

// �����Ȩ
typedef struct _AuthStatusT
{
	int     eAuthType;
	time_t  tTimeOut;
}AuthStatusT;


// ¼�����
typedef struct _RecStrategy
{
	int iFileFormat;	// ¼���ļ���ʽ
	int iPacketMode;	// ���ģʽ
	int iPacketSize;	// �����С
	int iPacketTime;	// ���ʱ��
}RecStrategy;

#define MAX_REC_PLAN_NUM 4             //ÿ�����¼��ƻ�����

typedef struct _tagTimeSlot
{
	int iStartTime;
	int iEndTime;
}TIME_SLOT;

typedef struct _RecPlan
{
	TIME_SLOT tMon[MAX_REC_PLAN_NUM];
	TIME_SLOT tTue[MAX_REC_PLAN_NUM];
	TIME_SLOT tWed[MAX_REC_PLAN_NUM];
	TIME_SLOT tThu[MAX_REC_PLAN_NUM];
	TIME_SLOT tFri[MAX_REC_PLAN_NUM];
	TIME_SLOT tSat[MAX_REC_PLAN_NUM];
	TIME_SLOT tSun[MAX_REC_PLAN_NUM];
}RecPlan;

enum
{
	eFull2CyclicCovering  = 0,	// ����ѭ������
	eFull2StopAnaly  = 1,		// ����ֹͣ����
	eFull2StopSave   = 2		// ����ֹͣд�����
};

// �洢����״̬
typedef struct _tagAnalyStroeStrategy
{
	bool  bFull;
	int   iHow2DO;
}StoreFullStatusT;

MQ_API int MQ_Record_Init();
MQ_API int MQ_Record_Close();
MQ_API int MQ_Record_SoftwareLicense(const AuthStatusT * ptInfo);
MQ_API int MQ_Record_SourceChange(int iChnID, const RecSrcStat *ptStat);
MQ_API int MQ_Record_SetStrategy(const RecStrategy * ptStrategy);
MQ_API int MQ_Record_SetPlan(int iChnID, const RecPlan *ptPlan);
MQ_API int MQ_Record_SetEnable(int iChnID, bool bEnable);
MQ_API int MQ_Record_RootPath(const char * pszRoot);
MQ_API int MQ_Record_FullStatus(const StoreFullStatusT * ptInfo);

#ifdef __cplusplus
};
#endif

#endif//__MQ_RECORD_H__

