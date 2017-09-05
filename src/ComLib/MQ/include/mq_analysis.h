#ifndef __MQ_ANALYSIS_H__
#define __MQ_ANALYSIS_H__

#include "mq_interface.h"
#include "mq_record.h"
#include "mq_onvif.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	ANALY_CMD_START = ANALY_CMD,
	ANALY_CMD_DEVICEINFO,		// MASTER->ANALY 设备信息
	ANALY_CMD_LICENSE,			// MASTER->ANALY 授权信息
	ANALY_CMD_ROOTPATH,			// MASTER->ANALY 存储策略 目录 覆盖
	ANALY_CMD_STORE_FULL,		// MASTER->ANALY 存储状态

	ANALY_CMD_LPR,				// MASTER->ANALY 车牌识别参数
	ANALY_CMD_DISPATCH,			// MASTER->ANALY 调度控制参数

	ANALY_CMD_CHN_OPEN,			// MASTER->ANALY 开启通道算法
	ANALY_CMD_CHN_PLAN,			// MASTER->ANALY 通道分析计划
	ANALY_CMD_CHN_PLAN_OPEN,	// MASTER->ANALY 开启通道分析计划
	ANALY_CMD_CHN_NOTIFY_OPEN,	// MASTER->ANALY 开启通道微信提醒
	ANALY_CMD_CHN_PLACE,		// MASTER->ANALY 通道地点信息
	ANALY_CMD_CHN_CLEAR,		// MASTER->ANALY 清空通道分析规则
	ANALY_CMD_CHN_MANUAL,		// MASTER->ANALY 通道手动抓拍

	ANALY_CMD_CHN_PRESET_POS,	// MASTER->ANALY 通道预置位坐标

	ANALY_CMD_CHN_SCENE_INFO,	// MASTER->ANALY 通道场景基本信息
	ANALY_CMD_CHN_SCENE_RULE,	// MASTER->ANALY 通道场景算法规则

	ANALY_CMD_EVENT_DETERMINE,	// MASTER->ANALY 违法判定
	ANALY_CMD_EVENT_EVIDENCE,	// MASTER->ANALY 取证模式
	ANALY_CMD_EVENT_IMG_MAKEUP,	// MASTER->ANALY 图片取证之图片构成方式
	ANALY_CMD_EVENT_IMG_COM,	// MASTER->ANALY 图片取证之图片合成方式
	ANALY_CMD_EVENT_IMG_OSD,	// MASTER->ANALY 图片取证之图片OSD
	ANALY_CMD_EVENT_IMG_QUALITY,// MASTER->ANALY 图片取证之图片质量
	ANALY_CMD_EVENT_REC,		// MASTER->ANALY 录像取证

	ANALY_CMD_EVENT_CODE,		// MASTER->ANALY 违法代码
	ANALY_CMD_EVENT_NAMING,		// MASTER->ANALY 取证文件命名规范

	ANALY_CMD_END
};

// 设备信息
typedef struct _tagDeviceInfo
{
	char szDevID[64];	// 设备编号
	char szRegID[64];	// 设备注册号
	char szNetID[64];   // 设备入网编号
	char szManID[64];	// 设备厂商编码
}DeviceInfoT;


// 省份编码表
enum
{
	eBeiJing 		= 0,	// 京
	eXinJiang		= 1,	// 新
	eNeiMengGu 		= 2,	// 蒙
	eYunNan 		= 3,	// 云
	eHeNan 			= 4,	// 豫
	eLiaoNing 		= 5,	// 辽
	eGuiZhou 		= 6,	// 贵
	eTianJing 		= 7,	// 津
	eFuJian		 	= 8,	// 闽
	eHeBei 			= 9,	// 冀
	eShanXi1 		= 10,	// 晋
	eJiLing 		= 11,	// 吉
	eHeLongJiang 	= 12,	// 黑
	eShangHai	 	= 13,	// 沪
	eJiangSu	 	= 14,	// 苏
	eZheJiang	 	= 15,	// 浙
	eAnHui 			= 16,	// 皖
	eJiangXi		= 17,	// 赣
	eShangDong	 	= 18,	// 鲁
	eHuBei 			= 19,	// 鄂
	eHuNan 			= 20,	// 湘
	eGuangDong 		= 21,	// 粤
	eGuangxi 		= 22,	// 桂
	eHaiNan 		= 23,	// 琼
	eSiChuan 		= 24,	// 川
	eXiZang 		= 25,	// 藏
	eShanXi3	 	= 26,	// 陕
	eGansu 			= 27,	// 甘
	eQingHai 		= 28,	// 青
	eNingXia 		= 29,	// 宁
	eChongQin 		= 30	// 渝
};
#define MIN_HANZI	eBeiJing
#define MAX_HANZI	eChongQin

// 车牌识别参数
typedef struct _tagLPR
{
	int iDefProv;	// 默认省份汉字，在识别低于置信度要求时使用
	int iHanziConf;	// 汉字置信度要求 0-100
	int iMinSize;	// 车牌最小尺寸
	int iDupTime;	// 相同车牌重复过滤时间
	int iDupSens;	// 多少个字符相同算相同车牌 0-6
}LPRCfg;

enum
{
	eSenFast		= 0,	// 分析速度快速
	eSenCompromise	= 1,	// 速度准确折中
	eSenAccuracy	= 2		// 分析结果准确
};

enum
{
	ePriByScene = 0,	// 场景优先
	ePriByTarget= 1		// 目标优先
};

// 调度控制
typedef struct _tagDispatchCtrl
{
	int  iSensitivity;	// 触发灵敏度 0-快 1-平衡 2-准
	int  iPriority;		// 场景切换优先级
	int  iSceneMinStay;	// 场景最小停留时间
	int  iRepeatTime;	// 重复抓拍时间
	bool bPtzLock;		// 是否锁定云台
	int  iLockTime;		// 锁定时间
}DispatchCtrlT;


#define MAX_ANALY_PLAN_NUM	4

typedef struct _AnalyPlan
{
	TIME_SLOT tMon[MAX_ANALY_PLAN_NUM];// 星期一
	TIME_SLOT tTue[MAX_ANALY_PLAN_NUM];// 星期二
	TIME_SLOT tWed[MAX_ANALY_PLAN_NUM];// 星期三
	TIME_SLOT tThu[MAX_ANALY_PLAN_NUM];// 星期四
	TIME_SLOT tFri[MAX_ANALY_PLAN_NUM];// 星期五
	TIME_SLOT tSat[MAX_ANALY_PLAN_NUM];// 星期六
	TIME_SLOT tSun[MAX_ANALY_PLAN_NUM];// 星期天
}AnalyPlan;

// 监控点信息
typedef struct _PlaceInfo
{
	char szAreaName[256];              // 区域名称
	char szAreaID[128];                // 区域编号
	char szPlaceName[256];             // 地点名称
	char szPlaceID[128];               // 地点编号
	char szMonitoryName[256];          // 监控点名称
	char szMonitoryID[128];            // 监控点编号
}PlaceInfoT;

typedef struct _Rect
{
	int x;
	int y;
	int w;
	int h;
}RectT;

typedef struct _ManuallySnap
{
	int   iEntID;	// 事件类型
	RectT tRect;	// 抓拍区域
}ManualSnapT;

enum
{
	e2UN  = 0,	// 未知
	eE2W  = 1,	// 东向西
	eW2E  = 2,	// 西向东
	eS2N  = 3,	// 南向北
	eN2S  = 4,	// 北向南
	eD2U  = 5,	// 上行
	eU2D  = 6,	// 下行
	eBOTH = 7	// 双向
};

typedef struct _SceneInfo
{
	int   iPresetID;
	char  szSceneName[256];
	int   iStayTime;
	int   iDirection;
}SceneInfoT;

typedef struct _PointDouble
{
	int x;
	int y;
}PointT;

#define  AREA_MAX_POINT_NUM		 10 // 区域的最大点数
#define  MAX_SHEILD_AREA_NUM     4  // 最多屏蔽区域的个数
#define  MAX_ANALY_AREA_NUM      4  // 最多分析区域的个数

typedef struct _Area
{
	int	   iPtNum;
	PointT atPoints[AREA_MAX_POINT_NUM];
}AreaT,LineT;

typedef struct _Calib
{
	float length;
	float width;
	PointT  atPoints[4];
}CalibT;

typedef struct _TargetSize
{
	int bEnable;
	PointT atMinPoints[2];
	PointT atMaxPoints[2];
}TargetSizeT;

typedef struct _AnalyRule
{
	CalibT tCalib;

	TargetSizeT tTargetSize;

	int   iSheildAreaNum;
	AreaT atSheildAreas[MAX_SHEILD_AREA_NUM];

	int   iAnalyAreaNum;
	AreaT atAnalyAreas[MAX_ANALY_AREA_NUM];

	int   iJudgeLineNum;
	LineT atudgeLines[MAX_ANALY_AREA_NUM];
}AnalyRuleT;

typedef struct _DetermineThreshold
{
	int     iValue1;
	int     iValue2;
	double  fValue1;
	double  fValue2;
}DetermineT;

typedef struct _EvidenceModel
{
	bool   bEnableEvent;
	int    iModel;
}EvidenceModelT;

enum
{
	eFull,		// 全景
	eCloser,	// 近景
	eFeature	// 特写
};

#define  MAX_IMG_NUM	4	// 1条记录最大抓拍张数
#define  MAX_REC_NUM	2	// 1条记录最大录像段数

enum
{
	eNotCom = 0,	// 不合成
	eVerCom = 1,    // 竖直合成
	eHorCom = 2,    // 水平合成
	eMatCom = 3     // 田字合成
};

typedef struct _ImgMakeup
{
	int iImgNum;
	int iSnapPos[MAX_IMG_NUM];
	int iInterval[MAX_IMG_NUM];
}ImgMakeupT;

typedef struct _ImgCompose
{
	int iComModel2;
	int iComModel3;
	int iComModel4;
}ImgComposeT;

typedef struct _RGBColor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}RGBColorT;

enum
{
	eEachUp   = 0,	// 单张图片上叠加
	eEachIn   = 1,  // 单张图片内叠加
	eEachDown = 2,  // 单张图片下叠加
	eComUp	  = 3,	// 合成图片上叠加
	eComIn    = 4,	// 合成图片内叠加
	eComDown  = 5   // 合成图片下叠加
};

#define OSD_TIME		0x0001	// 日期时间
#define OSD_MESC		0x0002	// 时间毫秒
#define OSD_DEVID		0x0004	// 设备编号
#define OSD_PLACE		0x0008	// 违法地点
#define OSD_EVENT		0x0010	// 违法行为
#define OSD_CODE		0x0020	// 违法代码
#define OSD_PLATE		0x0040	// 号牌号码
#define OSD_PLATE_COLOR	0x0080	// 号牌颜色
#define OSD_DIRECTION	0x0100	// 方向
#define OSD_WATERMARK	0x8000	// 防伪水印
typedef struct _ImgOSD
{
	RGBColorT tBackColor;		// 背景颜色
	RGBColorT tFontColor;		// 字体颜色
	int		  iFontSize;		// 字体大小
	int       iContentFlag;		// 叠加内容
	int		  iPosModel;		// 叠加位置
	int		  iLeft;			// 起始左边距
	int		  iTop;				// 起始上边距
}ImgOSDT;

typedef struct _ImgQuality
{
	int iSizeLimit;		// 大小限制
	int iWidthLimit;	// 宽度限制
	int iHeightLimit;	// 高度限制
}ImgQualityT;

enum 
{
	eRecWhole = 0,	// 全整录像
	eRecJoint = 1	// 拼接录像
};

typedef struct _RecEvidence
{
	bool bEnable;
	int  iRecModel;		// 录像合成模式
	int  iFileFormat;	// 录像文件格式	REC_FILE_MP4/AVI
	int  iPreTime;		// 预录时间
	int  iAftTime;		// 延迟时间
	int  iMinTime;		// 最短时间
}RecEvidenceT;

typedef struct _EventCode
{
	char szCode[16];	// 代码 例如：1092 001 AB001...
	char szDesc[1024];	// 描述 例如：违法停车...
}EventCodeT;

enum 
{
	eUseStartTime	= 0,	// 使用开始时间
	eUseEndTime		= 1,	// 使用结束时间
	eUseCreateTime	= 2		// 使用创建时间
};

typedef struct _FileNaming
{
	int  iTimeModel;
	char szIllegalImg[256];
	char szIllegalRec[256];
	char szEventImg[256];
	char szEventRec[256];
}FileNamingT;

MQ_API int MQ_Analy_Init();
MQ_API int MQ_Analy_Close();

MQ_API int MQ_Analy_DeviceInfo(const DeviceInfoT * ptInfo);
MQ_API int MQ_Analy_SoftwareLicense(const AuthStatusT * ptInfo);
MQ_API int MQ_Analy_RootPath(const char * pszRootPath);
MQ_API int MQ_Analy_FullStatus(const StoreFullStatusT * ptInfo);
MQ_API int MQ_Analy_LPR(const LPRCfg * ptInfo);
MQ_API int MQ_Analy_Dispatch(const DispatchCtrlT * ptInfo);
MQ_API int MQ_Analy_Chn_AnalyEnable(int iChnID, bool bEnable);
MQ_API int MQ_Analy_Chn_AnalyPlan(int iChnID, const AnalyPlan * ptInfo);
MQ_API int MQ_Analy_Chn_AnalyPlanEnable(int iChnID, bool bEnable);
MQ_API int MQ_Analy_Chn_AnalyNotifyEnable(int iChnID, bool bEnable);
MQ_API int MQ_Analy_Chn_PlaceInfo(int iChnID, const PlaceInfoT * ptInfo);
MQ_API int MQ_Analy_Chn_ClearAnaly(int iChnID);
MQ_API int MQ_Analy_Chn_ManualSnap(int iChnID, const ManualSnapT * ptInfo);

MQ_API int MQ_Analy_Chn_Preset_Pos(int iChnID, int iPresetID, const AbsPosition * ptInfo);

MQ_API int MQ_Analy_Chn_Scn_Info(int iChnID, int iScnID, const SceneInfoT * ptInfo);
MQ_API int MQ_Analy_Chn_Scn_Rule(int iChnID, int iScnID, const AnalyRuleT * ptInfo);

MQ_API int MQ_Analy_Ent_Determine(int iEntID, const DetermineT * ptInfo);
MQ_API int MQ_Analy_Ent_EvidenceModel(int iEntID, const EvidenceModelT * ptInfo);
MQ_API int MQ_Analy_Ent_ImgMakeup(int iEntID, int iModelType, const ImgMakeupT * ptInfo);
MQ_API int MQ_Analy_Ent_ImgCom(int iEntID, const ImgComposeT * ptInfo);
MQ_API int MQ_Analy_Ent_ImgOSD(int iEntID, const ImgOSDT * ptInfo);
MQ_API int MQ_Analy_Ent_ImgQuality(int iEntID, const ImgQualityT * ptInfo);
MQ_API int MQ_Analy_Ent_RecEvidence(int iEntID, const RecEvidenceT * ptInfo);
MQ_API int MQ_Analy_Ent_Code(int iEntID, const EventCodeT * ptInfo);
MQ_API int MQ_Analy_Ent_FileNaming(int iEntID, const FileNamingT * ptInfo);

#ifdef __cplusplus
};
#endif

#endif//__MQ_ANALYSIS_H__

