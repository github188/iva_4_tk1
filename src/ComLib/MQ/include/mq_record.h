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
	REC_FILE_MP4 = 0,		// MP4格式
	REC_FILE_AVI = 1,		// AVI格式
	REC_FILE_FLV = 2,		// FLV格式

};

enum {
	REC_PACKET_SIZE = 0,	// 按大小打包
	REC_PACKET_TIME = 1		// 按时间长度打包
};


enum
{
	eProhibit  = 0,	// 禁用
	ePermanent = 1, // 永久
	eTemporary = 2	// 临时
};

// 软件授权
typedef struct _AuthStatusT
{
	int     eAuthType;
	time_t  tTimeOut;
}AuthStatusT;


// 录像策略
typedef struct _RecStrategy
{
	int iFileFormat;	// 录像文件格式
	int iPacketMode;	// 打包模式
	int iPacketSize;	// 打包大小
	int iPacketTime;	// 打包时长
}RecStrategy;

#define MAX_REC_PLAN_NUM 4             //每天最多录像计划个数

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
	eFull2CyclicCovering  = 0,	// 满盘循环覆盖
	eFull2StopAnaly  = 1,		// 满盘停止分析
	eFull2StopSave   = 2		// 满盘停止写入磁盘
};

// 存储满盘状态
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

