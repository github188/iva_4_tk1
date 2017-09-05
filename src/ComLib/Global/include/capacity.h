#ifndef __GLOBAL_CAPACITY_H__
#define __GLOBAL_CAPACITY_H__

#ifndef GLOBAL_API
	#ifndef WIN32
		#ifdef __cplusplus
			#define GLOBAL_API extern "C"
		#else
			#define GLOBAL_API
		#endif
	#else
		#ifdef GLOBAL_EXPORTS
			#define GLOBAL_API extern  "C"   __declspec(dllexport)
		#else
			#define GLOBAL_API extern  "C"   __declspec(dllimport)
		#endif
	#endif
#endif

// 本文件描述了产品的一些能力定义

#define MAX_CHANNEL_NUM  1	// 最大通道数
#define MAX_SCENE_NUM    8	// 1个通道最大场景数

#define PAS_EDITION			5	// 停车分析抓拍版本park analysis system
#define FSS_EDITION			6	// 快速布控版本fast supervise system

#define TIME_SYN_PRECISION	2	// 时间同步精度

#define MOUNT_POINT_ROM		"/"				// 不能修改
#define MOUNT_POINT_USB		"/mnt/usb"		// USB挂载点
#define MOUNT_POINT_HDD 	"/mnt/hdd"		// 硬盘挂载点
#define MOUNT_POINT_SD 		"/mnt/sd"		// SD卡挂载点

#define IDENTIFY_DIR		"ivadata"		// 数据存储的标识文件夹
#define RECORD_TEMP_DIR		"rectmp"		// 录像临时文件夹
#define RECORD_DIR			"record"		// 录像文件夹
#define RESULT_EVENT_DIR	"event"
#define RESULT_ILLEGAL_DIR	"illegal"
#define RESULT_TOLL_DIR		"toll"

#define DOWNLOAD_DIR		"download"		// 离线下载的根目录

#define DEFAULT_ROOT_PATH	"/ivadata"

// 录像：MP/IDENTIFY_DIR/RECORD_DIR/YYYYMMDD/******.***
// 取证：MP/IDENTIFY_DIR/RESULT_***_DIR/YYYYMMDD/******.***
// 导出：


#define DEFAULT_BIN_PATH	"/opt/iva/"


// 违法类型表
enum
{
	eIT_Illegal_Park = 0,	// 违法停车
	eIT_Event_Max
};

typedef struct _ValueScopeDef
{
	int  iMin;
	int  iMax;
}ValueScopeDef;

#define  MAX_EVIMODEL_NUM	2
typedef struct _EvidenceModelDef
{
	char szName[128];				// 这种取证模式叫什么
	ValueScopeDef tImageNumScope;	// 这种取证模式支持的抓拍张数范围
}EvidenceModelDef;

#define  MAX_THRESHOLD_NUM	2
typedef struct _ThresholdDef
{
	int  iWhich;		// 哪一个值 0-iValue1, 1-iValue2, 2-fValue1, 3-fValue2
	char szName[64];	// 这个阈值叫什么
	char szUnit[64];	// 单位是什么
	ValueScopeDef tScope;	// 取值范围
}ThresholdDef;

typedef struct _EventDef
{
	char szName[128];				// 叫什么名字
	bool bSupportEvent;				// 是否支持事件
	ValueScopeDef tEventImageNum;	// 事件抓拍图片数量范围

	bool bSupportIllegal;			// 是否支持违法
	int  iEviModelNum;				// 支持几种取证模式
	EvidenceModelDef tIllegalModelDef[MAX_EVIMODEL_NUM];// 各个取证模式的定义

	int  iThresholdNum;				// 判断违法需要几个阈值
	ThresholdDef  tThresholdDef[MAX_THRESHOLD_NUM];//阈值定义

	bool bSupportCalibrate;		// 是否需要标定
	bool bSupportShieldArea;	// 是否需要屏蔽区域
	bool bSupportAnalyArea;		// 是否需要分析区域
	bool bSupportPolyline;		// 是否需要判断线段
}EventDef;


GLOBAL_API int Global_BuilldEdition();
GLOBAL_API const char * Global_DeviceModel();
GLOBAL_API const char * Global_HardVersion();
GLOBAL_API const char * Global_SoftVersion();

GLOBAL_API int Global_EventDef(int iEvent, EventDef *ptDef);

GLOBAL_API void  Global_DefualtThreshold(int iEvent, int & iValue1, int & iValue2, double & fValue1, double & fValue2);
GLOBAL_API const char * Global_DefualtCode(int iEvent);
GLOBAL_API const char * Global_DefualtDesc(int iEvent);


#endif//__UTILITY_FUNCTION_H__

