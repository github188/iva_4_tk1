#ifndef __MQ_MASTER_H__
#define __MQ_MASTER_H__

#include "mq_interface.h"
#include "mq_videoin.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	MASTER_CMD_START = MASTER_CMD,
	MASTER_CMD_ISAPI,		// BOA->MASTER
	MASTER_CMD_SET_RTSP,	// ONVIF->MASTER
	MASTER_CMD_SET_PTZCAP,	// ONVIF->MASTER
	MASTER_CMD_SET_SYSTIME,	// ONVIF->MASTER
	MASTER_CMD_VIDEOIN_STAT,// VIDEOIN->MASTER
	MASTER_CMD_RECORD_STAT, // RECORD->MASTER
	MASTER_CMD_ANALY_VERSION,	// ANALY->MASTER
	MASTER_CMD_ANALY_STAT,	// ANALY->MASTER
	MASTER_CMD_UPLOAD_STAT,	// UPLOAD->MASTER
	MASTER_CMD_GETALLSTAT,	// UPLOAD->MASTER
	MASTER_CMD_END
};

#define ISAPI_GET		1	// 获取
#define ISAPI_PUT		3	// 设置
#define ISAPI_POST		4	// 创建
#define ISAPI_DELETE	5	// 删除
typedef struct _ISAPI_REQ
{
	int  method;
	char szURI[128];
	char szAuth[128];
	char szRemoteIP[32];
	char szQuery[128];
}ISAPI_REQ;

typedef struct _ISAPI_RESP
{
	int  code;
}ISAPI_RESP;

enum 
{
	VIS_NOCFG     = 0,	// 视频源状态 无配置
	VIS_NORMAL	  = 1,	// 视频源状态 正常
	VIS_ABNORMAL  = 2	// 视频源状态 异常
};

enum 
{
	REC_STOP  	 = 0,	// 录像状态 没有录像
	REC_RUNNING  = 1,	// 录像状态 正在录像
	REC_ABNORMAL = 2	// 录像状态 异常
};

enum 
{
	ANALY_STOP     = 0,	// 算法状态 没有分析
	ANALY_RUNNING  = 1,	// 算法状态 正在分析
	ANALY_ABNORMAL = 2	// 算法状态 异常
};

typedef struct _PTZCap
{
	bool bSptPtz;
	bool bSptAbsMove;
	bool bSptClickZoom;
	int  iMaxPresetNum;
}PTZCap;

enum 
{
	UPLOAD_SVR_NO  = 0,	// 没有使用上传
	UPLOAD_SVR_CONN  = 1,	// 连接成功
	UPLOAD_SVR_DISCONN = 2	// 连接不成功
};

typedef struct _UploadStatus
{
	bool bUploading;
	int  iServerStat;
	bool bRemaining;
}UploadStatus;


MQ_API int MQ_Master_Init();
MQ_API int MQ_Master_Close();

// BOA调用
MQ_API int MQ_Master_ISAPI_Request(ISAPI_REQ* req, const char *req_json, ISAPI_RESP * resp, char *resp_json);

// MASTER回复BOA
MQ_API int MQ_Master_ISAPI_Response(ISAPI_RESP* resp, const char *resp_json);

// Onvif调用
MQ_API int MQ_Master_SetRtspInfo(int iChnID, int iStreamType, RtspInfo *pRtsp);
MQ_API int MQ_Master_SetPtzCap(int iChnID, PTZCap *pPtzCap);
MQ_API int MQ_Master_SetSysTime(time_t tTime);

// VideoIn调用
MQ_API int MQ_Master_SetVIStatus(int iChnID, int iStatus);

// Record调用
MQ_API int MQ_Master_SetRecStatus(int iChnID, int iStatus);

// ANALY调用
MQ_API int MQ_Master_SetAnalyVersion(const char * szVer);
MQ_API int MQ_Master_SetAnalyStatus(int iChnID, int iStatus);

// Upload调用
MQ_API int MQ_Master_SetUploadStatus(UploadStatus * ptStatus);
MQ_API int MQ_Master_GetAllStatus(char *resp_json);
MQ_API int MQ_Master_SetSysPlatTime(time_t tTime);

// MASTER回复Upload
MQ_API int MQ_Master_Response_AllStatus(const char *resp_json);


#ifdef __cplusplus
};
#endif

#endif//__MQ_MASTER_H__

