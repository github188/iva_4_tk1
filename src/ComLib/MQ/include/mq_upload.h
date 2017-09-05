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
	UPLOAD_TO_FTP	   = 0,	// 上传FTP
	UPLOAD_TO_PLATFORM = 1,	// 上传平台
	UPLOAD_TO_MQTT     = 2  // 上传MQTT
};

//FTP信息
typedef struct _FtpSvr
{
	char szAddr[128];		// 服务器地址
	int	 iPort;				// 端口
	char szUser[NAME_LEN];  // 用户名
	char szPass[PWD_LEN];   // 密码
}FtpSvr;

// 平台
typedef struct _Platform
{
	char szAddr[128];		// 服务器地址
	int	 iPort;				// 端口
	char szPlatID[64];		// 平台编号
}Platform;

// 数据上传
typedef struct _tagDataUpload
{
	int  iModel;	// 上传模式
	bool bDelOk;	// 上传成功以后删除记录
	FtpSvr   tFtpInfo;
	Platform tPlatform;
	Platform tMqttSvr;
}UploadStrategy;

#define FTP_FLAG_XML   0x000001	// 上传XML文件
#define FTP_FLAG_IMG   0x000002	// 上传图片文件
#define FTP_FLAG_REC   0x000004	// 上传录像文件

enum 
{
	eCharset_GB2312,
	eCharset_UTF8
};

//FTP高级参数
typedef struct _FtpAdvance
{
	int  iContentFlag;				// 上传内容标记位
	int  iCharSet;					// 字符集
	char szIllegalDirGrammar[512];  // 违法数据上传目标目录
	char szEventDirGrammar[512];	// 事件数据上传目标目录
	char szTollDirGrammar[512];		// 卡口数据上传目标目录
}FtpAdvance;

enum 
{
	eIllegaldata  = 1,	// 违法数据
	eEventData	  = 2,	// 事件数据
	eTollData	  = 3	// 卡口数据
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

#define MAX_TRACK_NUM		5		//最大分析水印个数
#define MAX_TRACK_POINT_NUM	10		//每个分析水印最大点个数
typedef struct _ATrack
{
	int iPointNum;
	RGBColorT tColor;
	PointT   atPoints[MAX_TRACK_POINT_NUM];
}ATrack;

typedef struct _AnalyTrack
{
	int iTrackNum;            //算法分析水印个数
	ATrack atTracks[MAX_TRACK_NUM];
}AnalyTracks;

typedef struct _NotifySvr
{
	char szAddr[128];		// 服务器地址
	int	 iPort;				// 端口
	bool bAttachImg;		// 是否附带图片
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

