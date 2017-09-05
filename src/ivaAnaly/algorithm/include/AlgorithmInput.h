/*!**********************************************************************
*\brief     智能分析算法库基础结构体声明头文件
*\details   
*\author    Xuyongxi
*\version    v1.0
*\date      2015 年 12 月 9 日
*\pre       
*\bug       
*\warning   
*\copyright JAYA Tech  
*/
/************************************************************************/
#ifndef __ALG_COM_STRUCT_H__
#define __ALG_COM_STRUCT_H__

#include <time.h>
#ifdef WIN32
#include <winsock2.h>
#endif

#ifdef __cplusplus
	extern "C" {
#endif

// 输入用到的宏
#define  ALG_MAX_CHANNEL_NUM    	 1	// 最大通道数
#define  ALG_MAX_SCENE_NUM     		16	// 1个通道最大场景数
#define  ALG_MAX_SHEILD_AREA_NUM     8  // 1个场景最大屏蔽区域的个数
#define  ALG_MAX_PARK_AREA_NUM       8  // 1个场景最大违停区域的个数
#define  ALG_MAX_LANE_NUM      	     8  // 1个场景最大车道个数
#define  ALG_MAX_SOLID_LINE_NUM      8  // 1个场景最大实线个数
#define  ALG_MAX_UTURN_LINE_NUM      8  // 1个场景最大禁止掉头线个数
#define  ALG_MAX_CHANGE_LINE_NUM     8  // 1个场景最大禁止变道线个数
#define  ALG_MAX_POINT_NUM		  	10  // 1个区域或者折线中最多的点数
#define  ALG_MAX_SNAP_NUM			 4	// 1个事件最大抓拍张数
#define  ALG_MAX_PRESET_NUMBER      16  // 最多预置位个数


typedef  enum 
{
	IMAGE_DATA_TYPE_YUV420 = 0,
	IMAGE_DATA_TYPE_NV12   = 1,
	IMAGE_DATA_TYPE_BGR    = 2,
	IMAGE_DATA_TYPE_UNKNOW
}EImageDataType;


/*!整形类型的2维点坐标*/
typedef struct
{
	int x;
	int y;
}ALG_Point;

/*!浮点类型的2维点坐标*/
typedef struct
{
    float x;
    float y;
}ALG_PointF;

/*!浮点类型的矩形*/
typedef struct
{
	int x;
	int y;
	int w;
	int h;
}ALG_Rect;


/*车道类型*/
typedef enum
{
	LANE_TYPE_NORMAL,			// 普通车道
	LANE_TYPE_OVERTAKING_ROAD,	// 超车车道，大车不能在这个车道行驶
	LANE_TYPE_EMERGENCY_ROAD,	// 应急车道
	LANE_TYPE_BUS_PURPOSE,		// 公交车专用车道
	LANE_TYPE_NON_MOTOR_VEHICLE,// 非机动车道
	LANE_TYPE_LIGHT_VEHICLE,	// 小型车道
	LANE_TYPE_UNKONWN			// 车道类型未知
}ELaneType;


/*!单区域/多段线信息结构体*/
typedef struct _tArea
{
	unsigned int  u32PointNum;/*点的个数*/
	ALG_Point     stPoint[ALG_MAX_POINT_NUM];/*点集合*/
}tArea,tLine;

/*!屏蔽区配置结构体*/
typedef struct _tSheildAreaConf
{
	unsigned int u32AreaCount; /*区域的个数 */
	tArea        stArea[ALG_MAX_SHEILD_AREA_NUM];   /*屏蔽区的集合*/
}tSheildAreaConf;

/*!违停区配置结构体*/
typedef struct _tParkAreaConf
{
	unsigned int u32AreaCount; /*区域的个数 */
	tArea        stArea[ALG_MAX_PARK_AREA_NUM];   /*违停区的集合*/
}tParkAreaConf;

/*!标定参数结构体*/
typedef struct
{
	ALG_PointF tPtsInImage[4];
	float  fActualWidth;	// 矩形的实际宽，单位是m
	float  fActualHeight;	// 矩形的实际高，单位是m
}tCalibrationConf;

/*!单车道配置参数结构体*/
typedef struct
{
	int    	  s32LaneID;		// 车道编号 
	int       s32LaneType;		// 车道类型,类型参照ELaneType
	ALG_Point tPtLaneCor[4];	// 车道坐标，为4个点
	ALG_Point tDirectPt[2];		// 车道方向，tDirectPt[0]为起点，tDirectPt[1]为终点
}tSingleLaneConf;

/*!车道配置参数结构体*/
typedef struct _tLaneParamConf
{
	unsigned int     u32LaneNum;// 车道数目
	tSingleLaneConf  tOneLaneConf[ALG_MAX_LANE_NUM];// 车道配置结构体
}tLaneParamConf;

/*! 折线规则配置结构体*/
typedef struct _tSolidLineConf
{
	unsigned int  u32LineNum;/*!折线的个数*/
	tLine   	  tLineConf[ALG_MAX_SOLID_LINE_NUM];/*!折线规则集合*/
}tSolidLineConf;

//单个掉头/变道规则折线
typedef struct tagSingleUTurnLine
{
	unsigned int  PointNum;					// 折线上点的个数
	ALG_Point     Point[ALG_MAX_POINT_NUM];	// 点集合

	int LeftRightLaneNO[2];					// 当前折线左、右相邻两车道的编号
	int CrossAllowFlag;						// 0:双向不允许 1:仅[0]->[1]允许，[1]->[0]不允许  2:仅[1]->[0]允许，[0]->[1]不允许  3:双向允许
}tSingleUTurnLine;

typedef struct tagUTurnLineConf
{
	unsigned int		SingleUTurnNum;				     // 掉头/变道规则的个数
	tSingleUTurnLine	SingleUTurn[ALG_MAX_UTURN_LINE_NUM]; // 掉头/变道规则集合
}tUturnLineConf;

typedef enum
{
	FULL_SHOT,			// 全景
	CLOSE_UP,			// 近景
	CLOSE_SHOT			// 特写
}ESnapType;

// 告警类型
typedef enum 
{
	AT_NORMAL= -1,					// 正常状态
	AT_ILLEGAL_STOP_CAR,			// 违章停车          // WTP  IVR  HWD
	AT_REVERSE_DRIVING, 			// 逆行/倒车         //      IVR  HWD
	AT_CAR_ILLEGE_UTURN,			// 违章掉头		    //      IVR
	AT_CAR_PRESS_LINE,				// 压实线            //      IVR
	AT_ILLEGAL_OCCUPY_LANE, 		// 违章占道	        //      IVR  HWD	// 大车违章占道/机占非...等等都是它
	AT_ABANDONED_OBJECT,			// 出现抛洒物告警     // 	    HWD
	AT_PEDESTRIAN,    				// 出现行人告警       // 			HWD
	AT_HEAVE_TRAFFIC_CROWD,         // 交通拥堵          //           HWD
	AT_ILLEGAL_CHANGE_LANE,         // 违章变道          //      IVR
	AT_ONBOARD_OCR					// 车牌识别结果       //      IVR
}ALG_AlarmType;

// 事件取证
typedef struct
{
	ALG_AlarmType eEventType;	// 事件类型
	unsigned int  u32SnapCnt;  	// 抓拍数量 违停2-4 其他1-3
	ESnapType     eSnapType[ALG_MAX_SNAP_NUM];// 全景/特写/(近景)
	unsigned int  u32Interval[ALG_MAX_SNAP_NUM];// 间隔时间(违停专用)
	int		      bEnable;//是否取证
	int		      bEvent;//是否需要事件记录
}tEventEvidenceConf;

//事件配置
typedef struct  _tEventSetting
{
	tEventEvidenceConf *pEventConfList;
	int nEvent;
}tEventSetting;

typedef struct tagPresetAbsPtz
{
	float  pan;
	float  tilt;
	float  zoom;
	float  focallen;
}tPresetAbsPtz;

/*!单预置位算法配置结构体*/
typedef  struct tagSinglePresetConf
{
	unsigned int     u32ChannelID;		// 通道号
	int				 s32PresetId;		// 预置位编号
	tPresetAbsPtz	 tAbsPtz;			// 预置位PTZF值

	float			 fClickZoomInRate;	// 点击放大倍率
	float			 fClickZoomDistance;// 动态拉近预测距离距离
	unsigned int     u32FuncFlag;		// 功能开关标识 按位表示

	tCalibrationConf tCalibConf;		// 标定参数
	float            fMaxObjSize;		// 最大目标比例
	float            fMinObjSize;		// 最小目标比例

	tSheildAreaConf  tSheildConf;		// 屏蔽区域参数

	unsigned int     u32IllegeStopTime; // 判断为 违停     的最小时间，单位是s
	tParkAreaConf    tParkArea;			// 违停区域

	float            fReverseDriveDis;  // 判断为 逆行/倒车的最小行驶距离，单位是米
	float            fIllegeOccupyDis;  // 判断为 非法占道 的最小行驶拘留，单位是米
	tLaneParamConf   tLaneConf;			// 车道配置

	unsigned int     u32UTurnAngle;	    // 判断为 违法掉头 的最小角度，度
	tUturnLineConf	 tUTurnConf;        // 违法掉头规则配置

	unsigned int     u32PressLineTime;  // 判断为 压实线   的最小时间，单位是s
	float            fPressLineDis;     // 判断为 压实线   的最小行驶距离，单位是米
	tSolidLineConf   tSolidConf;		// 实线规则配置

	tUturnLineConf   LaneChangeConf;	// 违章变道的配置
}tSinglePresetConf;

//预置位切换列表
typedef struct _tPresetList
{
	//预置位个数
	int PresetNum;
	//每个预置位的配置
	tSinglePresetConf	SinglePreset[ALG_MAX_PRESET_NUMBER];
	//每个取证配置
	tEventSetting		EventSetting[ALG_MAX_PRESET_NUMBER];
	//预置位最短时间
	float				MinStopTime[ALG_MAX_PRESET_NUMBER];
}tPresetList;


// 功能开关
#define USE_PARK	0x0001	// 违停


// PTZ操作
typedef  enum
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NONE
} DIRECTION;

typedef int (* Func_PTZ_Stop)(int iPort);
typedef int (* Func_PTZ_Move)(int iPort,DIRECTION dx, unsigned char speedx, DIRECTION dy, unsigned char  speedy, int stopInterval);
typedef int (* Func_PTZ_ZoomIn)(int iPort, int stopInterval);
typedef int (* Func_PTZ_ZoomOut)(int iPort, int stopInterval);
typedef int (* Func_PTZ_ClickZoomIn)(int iPort, int x, int y, float z, int w, int h, float speedx, float speedy);
typedef int (* Func_PTZ_SetPreset)(int iPort, int  number);
typedef int (* Func_PTZ_ToPreset)(int iPort, int  number);
typedef int (* Func_PTZ_DelPreset)(int iPort, int  number);
typedef int (* Func_PTZ_GetAbsPTZ)(int iPort, float * pfP, float *pfT, float *pfZ);
typedef int (* Func_PTZ_SetAbsPTZ)(int iPort, float fP, float fT, float fZ);


typedef  struct tagPTZController
{
	int                  iPort;
	Func_PTZ_Stop        Stop;
	Func_PTZ_Move        Move;
	Func_PTZ_ZoomIn      ZoomIn;
	Func_PTZ_ZoomOut     ZoomOut;
	Func_PTZ_ClickZoomIn ClickZoomIn;
	Func_PTZ_SetPreset   SetPreset;
	Func_PTZ_ToPreset    GotoPreset;
	Func_PTZ_DelPreset   DeletePreset;
	Func_PTZ_GetAbsPTZ   GetAbsPTZ;
	Func_PTZ_SetAbsPTZ   SetAbsPTZ;
}PTZController;


#ifdef __cplusplus
	}
#endif

#endif //endif  #define __ALG_COM_STRUCT_H__



