#ifndef __MQ_ONVIF_H__
#define __MQ_ONVIF_H__

#include "mq_interface.h"
#include "oal_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif


enum
{
	ONVIF_CMD_START = ONVIF_CMD,
	ONVIF_CMD_SET_DEV,
	ONVIF_CMD_SET_PROXY,
	ONVIF_CMD_SET_TIMESYN,
	ONVIF_CMD_PTZ_MOVE,
	ONVIF_CMD_PTZ_STOP,
	ONVIF_CMD_APERTURE_MOVE,
	ONVIF_CMD_APERTURE_STOP,
	ONVIF_CMD_FOCUS_MOVE,
	ONVIF_CMD_FOCUS_STOP,
	ONVIF_CMD_GET_PRESETS,
	ONVIF_CMD_PRESET,
	ONVIF_CMD_GET_ABS,
	ONVIF_CMD_ABS_MOVE,
	ONVIF_CMD_CLICKZOOM,
	ONVIF_CMD_GET_RTSP_URL,
	ONVIF_CMD_END
};

enum 
{
	M_HIK		= 0,	// 海康威视
	M_DH		= 1,	// 大华科技
	M_TIANDY	= 2,	// 天地伟业
	M_UNIVIEW	= 3		// 宇视科技
};

enum
{
	STREAM_TYPE_MAIN = 0,	// 主码流
	STREAM_TYPE_SUB  = 1	// 子码流
};

typedef struct _tagViSource
{
	bool bEnable;			// 使能标记
	char szIP[IPV4_LEN];    // 球机IP
	char szUser[NAME_LEN];  // 球机用户名
	char szPass[PWD_LEN];   // 球机密码
	int  iManuCode;			// 厂商编码
	int  iStreamType;       // 码流类型
}VISource;

typedef struct _OnvifTimeSyn
{
	bool bEnable;      // 使能
	u32  uInterval;    // 同步间隔
	int  iSynChnID;	   // 同步的视频通道
}OnvifTimeSyn;

enum 
{
	CLICK_ZOOM_PRIVATE = 0,	// 私有协议
	CLICK_ZOOM_PROXY   = 1	// 代理
};

typedef struct _ClickZoomProxy
{
	int  iModel;			// 代理模式
	char szAddr[IPV4_LEN];	// 代理地址
	int	 iPort;				// 代理端口
}ClickZoomProxy;

#define MIN_PRESET_ID	1
#define MAX_PRESET_ID	20

enum
{
	PTZ_MOVE_LEFT		= 0,	// 左
	PTZ_MOVE_LEFT_UP	= 1,	// 左上
	PTZ_MOVE_UP			= 2,	// 上
	PTZ_MOVE_RIGHT_UP	= 3,	// 右上
	PTZ_MOVE_RIGHT		= 4,	// 右
	PTZ_MOVE_RIGHT_DOWN = 5,	// 右下
	PTZ_MOVE_DOWN		= 6,	// 下
	PTZ_MOVE_LEFT_DOWN	= 7,	// 左下
	PTZ_MOVE_ZOOM_IN	= 8,	// 放大
	PTZ_MOVE_ZOOM_OUT	= 9		// 缩小
};

enum
{
	APERTURE_INCREASE = 0,	// 光圈增大
	APERTURE_DECREASE = 1	// 光圈减小
};

enum
{
	FOCUS_NEAR = 0,	// 焦距近
	FOCUS_FAR  = 1	// 焦距远
};

typedef struct _PresetNode
{
	int  iPresetID;
	char szName[16];
}PresetNode;

#define MAX_PRESET_NUM  512
typedef struct _PresetArray
{
	int  iPresetNum;
	PresetNode arPresets[MAX_PRESET_NUM];
}PresetArray;

enum
{
	PRESET_GOTO = 0, // 调用预置位
	PRESET_SET  = 1, // 设置预置位
	PRESET_DEL  = 2	 // 删除预置位
};

typedef struct _Point
{
	int x;
	int y;
}Point;

typedef struct _ClickArea
{
	Point StartPoint;
	Point EndPoint;
}ClickArea;

typedef struct _AbsPosition
{
	float fPan;		// [-1 1]
	float fTilt;	// [-1 1]
	float fZoom;	// [0 1]
}AbsPosition;


// 客户端+服务端都需要调用的接口
MQ_API int MQ_Onvif_Init();
MQ_API int MQ_Onvif_Close();

// 客户端调用的接口
MQ_API int MQ_Onvif_Set_VISource( long lSrc, int iChnID, const VISource *ptInfo );
MQ_API int MQ_Onvif_Set_Proxy( long lSrc, int iChnID, const ClickZoomProxy *ptInfo );
MQ_API int MQ_Onvif_Set_TimeSyn( long lSrc, const OnvifTimeSyn *ptInfo );
MQ_API int MQ_Onvif_Ptz_Move( long lSrc, int iChnID, int iAction, int iSpeed );
MQ_API int MQ_Onvif_Ptz_Stop( long lSrc, int iChnID );
MQ_API int MQ_Onvif_Aperture_Move( long lSrc, int iChnID, int iAction, int iSpeed );
MQ_API int MQ_Onvif_Aperture_Stop( long lSrc, int iChnID );
MQ_API int MQ_Onvif_Focus_Move( long lSrc, int iChnID, int iAction, int iSpeed );
MQ_API int MQ_Onvif_Focus_Stop( long lSrc, int iChnID );
MQ_API int MQ_Onvif_Get_Presets( long lSrc, int iChnID, PresetArray *ptPresetArray );
MQ_API int MQ_Onvif_Preset( long lSrc, int iChnID, int iAction, int iPresetID );
MQ_API int MQ_Onvif_Get_AbsPos( long lSrc, int iChnID, AbsPosition * ptPos );
MQ_API int MQ_Onvif_Abs_Move( long lSrc, int iChnID, const AbsPosition *ptPos );
MQ_API int MQ_Onvif_ClickZoom( long lSrc, int iChnID, const ClickArea *ptArea );

// 服务端调用的接口
MQ_API int MQ_Onvif_Answer_Presets( long lDes, int iRet, const PresetArray *ptPresetArray );
MQ_API int MQ_Onvif_Answer_AbsPos( long lDes, int iRet, const AbsPosition *ptPos );

#ifdef __cplusplus
};
#endif

#endif

