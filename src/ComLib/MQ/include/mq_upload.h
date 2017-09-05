#ifndef __MQ_UPLOAD_H__
#define __MQ_UPLOAD_H__

#include "mq_interface.h"
#include "oal_typedef.h"
#include "mq_analysis.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	UPLOAD_CMD_START = UPLOAD_CMD,
	UPLOAD_CMD_STRATEGY,			// MASTER->UPLOAD
	UPLOAD_CMD_ENABLE,				// MASTER->UPLOAD
	UPLOAD_CMD_FTPADV,				// MASTER->UPLOAD
	UPLOAD_CMD_RESULT_PREVIEW,		// ANALY->UPLOAD
	UPLOAD_CMD_TRACK_PREVIEW,		// ANALY->UPLOAD
	UPLOAD_CMD_NOTIFY_SERVER,		// ANALY->UPLOAD
	UPLOAD_CMD_NOTIFY_ENABLE,		// ANALY->UPLOAD
	UPLOAD_CMD_NOTIFY,				// ANALY->UPLOAD
	UPLOAD_CMD_END
};

enum 
{
	UPLOAD_TO_FTP	   = 0,	// �ϴ�FTP
	UPLOAD_TO_PLATFORM = 1,	// �ϴ�ƽ̨
	UPLOAD_TO_MQTT     = 2  // �ϴ�MQTT
};

//FTP��Ϣ
typedef struct _FtpSvr
{
	char szAddr[128];		// ��������ַ
	int	 iPort;				// �˿�
	char szUser[NAME_LEN];  // �û���
	char szPass[PWD_LEN];   // ����
}FtpSvr;

// ƽ̨
typedef struct _Platform
{
	char szAddr[128];		// ��������ַ
	int	 iPort;				// �˿�
	char szPlatID[64];		// ƽ̨���
}Platform;

// �����ϴ�
typedef struct _tagDataUpload
{
	int  iModel;	// �ϴ�ģʽ
	bool bDelOk;	// �ϴ��ɹ��Ժ�ɾ����¼
	FtpSvr   tFtpInfo;
	Platform tPlatform;
	Platform tMqttSvr;
}UploadStrategy;

#define FTP_FLAG_XML   0x000001	// �ϴ�XML�ļ�
#define FTP_FLAG_IMG   0x000002	// �ϴ�ͼƬ�ļ�
#define FTP_FLAG_REC   0x000004	// �ϴ�¼���ļ�

enum 
{
	eCharset_GB2312,
	eCharset_UTF8
};

//FTP�߼�����
typedef struct _FtpAdvance
{
	int  iContentFlag;				// �ϴ����ݱ��λ
	int  iCharSet;					// �ַ���
	char szIllegalDirGrammar[512];  // Υ�������ϴ�Ŀ��Ŀ¼
	char szEventDirGrammar[512];	// �¼������ϴ�Ŀ��Ŀ¼
	char szTollDirGrammar[512];		// ���������ϴ�Ŀ��Ŀ¼
}FtpAdvance;

enum 
{
	eIllegaldata  = 1,	// Υ������
	eEventData	  = 2,	// �¼�����
	eTollData	  = 3	// ��������
};

typedef struct _AnalyResult
{
	int  iChnID;
	int  iEventID;
	int  iDataType;
	char szEventDesc[1024];
	char szPlate[PLATE_LEN]; 
	char szPlateColor[16];
	char szCreateTime[20];
	char ImageFile[MAX_IMG_NUM][256];
}AnalyResult;

#define MAX_TRACK_NUM		5		//������ˮӡ����
#define MAX_TRACK_POINT_NUM	10		//ÿ������ˮӡ�������
typedef struct _ATrack
{
	int iPointNum;
	RGBColorT tColor;
	PointT   atPoints[MAX_TRACK_POINT_NUM];
}ATrack;

typedef struct _AnalyTrack
{
	int iTrackNum;            //�㷨����ˮӡ����
	ATrack atTracks[MAX_TRACK_NUM];
}AnalyTracks;

typedef struct _NotifySvr
{
	char szAddr[128];		// ��������ַ
	int	 iPort;				// �˿�
	bool bAttachImg;		// �Ƿ񸽴�ͼƬ
}NotifySvr;

typedef struct _AnalyNotify
{
	time_t tCreateTime;
	char szPlace[256];
	int  iEventType;
	int  iCarType;
	int  iCarColor;
	int  iCarBrand;
	int  iPlateType;
	int  iPlateColor; 
	char szPlate[PLATE_LEN]; 
	int  PlateConfidence;
	char ImageFile[256];
}AnalyNotify;

//#pragma pack(pop)

MQ_API int MQ_Upload_Init();
MQ_API int MQ_Upload_Close();
MQ_API int MQ_Upload_SetStrategy(const UploadStrategy * ptInfo);
MQ_API int MQ_Upload_Enable(bool bEnable);
MQ_API int MQ_Upload_SetFtpAdv(const FtpAdvance * ptInfo);
MQ_API int MQ_Upload_ResultPreview(const AnalyResult * ptInfo);
MQ_API int MQ_Upload_TrackPreview(int iChnID, const AnalyTracks * ptInfo);
MQ_API int MQ_Upload_Notify_SetServer(const NotifySvr * ptInfo);
MQ_API int MQ_Upload_Notify_Enable(bool bEnable);
MQ_API int MQ_Upload_Notify_New(const AnalyNotify * ptInfo);



#ifdef __cplusplus
};
#endif

#endif

