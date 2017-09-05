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

#define ISAPI_GET		1	// ��ȡ
#define ISAPI_PUT		3	// ����
#define ISAPI_POST		4	// ����
#define ISAPI_DELETE	5	// ɾ��
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
	VIS_NOCFG     = 0,	// ��ƵԴ״̬ ������
	VIS_NORMAL	  = 1,	// ��ƵԴ״̬ ����
	VIS_ABNORMAL  = 2	// ��ƵԴ״̬ �쳣
};

enum 
{
	REC_STOP  	 = 0,	// ¼��״̬ û��¼��
	REC_RUNNING  = 1,	// ¼��״̬ ����¼��
	REC_ABNORMAL = 2	// ¼��״̬ �쳣
};

enum 
{
	ANALY_STOP     = 0,	// �㷨״̬ û�з���
	ANALY_RUNNING  = 1,	// �㷨״̬ ���ڷ���
	ANALY_ABNORMAL = 2	// �㷨״̬ �쳣
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
	UPLOAD_SVR_NO  = 0,	// û��ʹ���ϴ�
	UPLOAD_SVR_CONN  = 1,	// ���ӳɹ�
	UPLOAD_SVR_DISCONN = 2	// ���Ӳ��ɹ�
};

typedef struct _UploadStatus
{
	bool bUploading;
	int  iServerStat;
	bool bRemaining;
}UploadStatus;


MQ_API int MQ_Master_Init();
MQ_API int MQ_Master_Close();

// BOA����
MQ_API int MQ_Master_ISAPI_Request(ISAPI_REQ* req, const char *req_json, ISAPI_RESP * resp, char *resp_json);

// MASTER�ظ�BOA
MQ_API int MQ_Master_ISAPI_Response(ISAPI_RESP* resp, const char *resp_json);

// Onvif����
MQ_API int MQ_Master_SetRtspInfo(int iChnID, int iStreamType, RtspInfo *pRtsp);
MQ_API int MQ_Master_SetPtzCap(int iChnID, PTZCap *pPtzCap);
MQ_API int MQ_Master_SetSysTime(time_t tTime);

// VideoIn����
MQ_API int MQ_Master_SetVIStatus(int iChnID, int iStatus);

// Record����
MQ_API int MQ_Master_SetRecStatus(int iChnID, int iStatus);

// ANALY����
MQ_API int MQ_Master_SetAnalyVersion(const char * szVer);
MQ_API int MQ_Master_SetAnalyStatus(int iChnID, int iStatus);

// Upload����
MQ_API int MQ_Master_SetUploadStatus(UploadStatus * ptStatus);
MQ_API int MQ_Master_GetAllStatus(char *resp_json);
MQ_API int MQ_Master_SetSysPlatTime(time_t tTime);

// MASTER�ظ�Upload
MQ_API int MQ_Master_Response_AllStatus(const char *resp_json);


#ifdef __cplusplus
};
#endif

#endif//__MQ_MASTER_H__

