#ifndef LIBONVIF2_H
#define LIBONVIF2_H

#ifndef LIBONVIF2_API
    #ifndef WIN32
		#define LIBONVIF2_API
    #else
        #ifdef ONVIF2_EXPORTS
            #define LIBONVIF2_API extern  "C"   __declspec(dllexport)
        #else
            #define LIBONVIF2_API extern  "C"   __declspec(dllimport)
        #endif
     #endif
#endif


#include "libonvif2struct.h"


typedef enum
{
	_Unknown,	// 未知厂商
	_Hikvision, // 海康威视
	_DaHua,		// 浙江大华
	_Tiandy,	// 天地伟业
	_Uniview	// 宇视科技
}MANUFACTURER_TYPE;

typedef enum
{
	_toLeft      = 0,	// 向左
	_toLeftUp    = 1,	// 向左上
	_toUp        = 2,	// 向上
	_toRightUp   = 3,	// 向右上
	_toRight     = 4,	// 向右
	_toRightDown = 5,	// 向右下
	_toDown      = 6,	// 向下
	_toLeftDown  = 7,	// 向左下
	_toZoomIn    = 8,	// 放大
	_toZoomOut   = 9	// 缩小
}PTZ_ACTION;

typedef enum
{
	_toNear = 0,	// 近
	_toFar  = 1		// 远
}FOCUS_ACTION;

typedef struct
{
	char ProfileToken[TOKEN_LENGTH];// Profile标识
	char URI[URI_LENGTH];//rtsp点播流地址
	int  width;
	int  height;
}StreamInfo;

typedef struct
{
	char ProfileToken[TOKEN_LENGTH];// Profile标识
	char PTZNodeToken[TOKEN_LENGTH];// PTZNode标识
	int  bSupport;
	int  bSupportAbsMove;
	int  MaxPresetsNum;
	int  bSupportHome;
	//int  width;
	//int  height;
}PTZInfo;

typedef struct
{
	float _Pan;	// [-1  1]
	float _Tilt;// [-1  1]
	float _Zoom;// [0   1]
}PTZAbsPosition;

#define ONVIF2_PRESET_NAME "IvaPst"


/**
	* 调试信息
	*
	* @ParamIn	bOpen 0-关闭（默认） 1打开
	* @Return 错误码
**/
LIBONVIF2_API int libOnvif2Debug(int bOpen);

/**
	* 设置超时时间 在调用不同的API前可以灵活设置
	*
	* @ParamIn	ts 发送超时时间(when > 0, gives socket send timeout in seconds, < 0 in usec )
	* @ParamIn	tr 接收超时时间(when > 0, gives socket recv timeout in seconds, < 0 in usec )
	* @ParamIn	tc 连接超时时间(when > 0, gives socket connect timeout in seconds, < 0 in usec )
	* @Return 错误码
**/
LIBONVIF2_API int libOnvif2SetTimeOut(int ts, int tr, int tc);

/**
	* 通过WSD发现所有的onvif NVT设备
	*
	* @ParamOut	
	* @Return 错误码
**/
LIBONVIF2_API int libOnvif2ProbeAllNVT(LIST_NVT * Devices);


/**
	* 创建设备控制句柄
	*
	* @ParamIn IP 设备IP地址
	* @ParamIn forwhat 设备初始化类型
	* @Return 成功-句柄 失败-错误码
**/
LIBONVIF2_API long libOnvif2ControlCreate( const char * IP);

/**
	* 删除设备控制句柄
	*
	* @ParamIn hSession 设备控制句柄
	* @Return 错误码
**/
LIBONVIF2_API int libOnvif2ControlDelete(long hSession);

/**
	* 删除所有设备控制句柄
	*
	* @Return 错误码
**/
LIBONVIF2_API int libOnvif2ControlCleanup();

/**
    * 设备登陆
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn UserName 用户名
    * @ParamIn Pass 密码
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2DeviceLogin(long hSession,const char* UserName,const char* Passwd);

/**
    * 获得设备profile个数
    *
    * @ParamIn hSession 设备控制句柄
    * @Return 个数
**/
LIBONVIF2_API int libOnvif2GetProfileSize(long hSession);


/**
    * 在设备登陆以后调用该函数，报告设备的兼容性情况，获取最佳的Profile索引
    *
    * @ParamIn hSession 设备控制句柄
    * @Return 最佳的Profile索引
**/
LIBONVIF2_API int libOnvif2ReportAfterLogin(long hSession);

/**
    * 获取时间
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamOut pszDateTime 输出时间 YYYY-MM-DD hh-mm-ss
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2GetDateTime(long hSession, char* pszDateTime);



/**
    * 设备登出
    *
    * @ParamIn hSession 设备控制句柄
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2DeviceLogout(long hSession);

/**
    * 获取设备的厂商型号
    *
    * @ParamIn hSession 设备控制句柄
    * @Return 厂商代码
**/
LIBONVIF2_API MANUFACTURER_TYPE libOnvif2GetManufacturer(long hSession);


/**
    * 获得设备视频流信息
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn index Profile的索引，默认值0
    * @ParamOut Info
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2GetStreamInfo(long hSession, int index, StreamInfo * Info);


/**
    * 获得设备的PTZ信息
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn index Profile的索引，默认值0
    * @ParamOut Info
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2GetPTZInfo(long hSession, int index, PTZInfo * Info);


/**
    * 云台控制开始
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn ProfileToken （libOnvif2GetPTZInfo返回的PTZInfo->ProfileToken）
    * @ParamIn Action 操作类型
    * @ParamIn Velocity 速度 0-100
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2PTZStartMove(long hSession,const char * ProfileToken,
										PTZ_ACTION Action,int Velocity);


/**
    * 云台控制停止
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn ProfileToken （libOnvif2GetPTZInfo返回的PTZInfo->ProfileToken）
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2PTZStopMove(long hSession,const char * ProfileToken);

/**
    * 获取云台绝对位置
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn ProfileToken （libOnvif2GetPTZInfo返回的PTZInfo->ProfileToken）
    * @ParamOut Position 返回的坐标值
    * @Return 错误码 IDLE = 0, MOVING = 1, UNKNOWN = 2
**/
LIBONVIF2_API int libOnvif2PTZGetAbsolutePosition(long hSession,const char * ProfileToken,
											  PTZAbsPosition *Position);

/**
    * 通过绝对位置调用PTZ
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn ProfileToken （libOnvif2GetPTZInfo返回的PTZInfo->ProfileToken）
    * @ParamIn Position 坐标值
    * @ParamIn Velocity 上下左右平移时的速度 0-100
    * @ParamIn ZoomVelocity 放大缩小的速度 0-100
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2PTZAbsoluteMove(long hSession,const char * ProfileToken,
									   const PTZAbsPosition *Position,int PanTiltVelocity, int ZoomVelocity);

/**
    * 获得预置点
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn ProfileToken （libOnvif2GetPTZInfo返回的PTZInfo->ProfileToken）
    * @ParamOut presets 预置点
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2GetPresets(long hSession,const char * ProfileToken,
									  LIST_Preset* presets);

/**
    * 创建预置点
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn ProfileToken （libOnvif2GetPTZInfo返回的PTZInfo->ProfileToken）
    * @ParamIn PresetNo 预置点编号
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2CreatePreset(long hSession,const char * ProfileToken,
										unsigned int PresetNo);

/**
    * 删除预置点
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn ProfileToken （libOnvif2GetPTZInfo返回的PTZInfo->ProfileToken）
    * @ParamIn PresetNo 预置点编号
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2DelPreset(long hSession,const char * ProfileToken,
									unsigned int PresetNo);

/**
    * 移动到预置点
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn ProfileToken （libOnvif2GetPTZInfo返回的PTZInfo->ProfileToken）
    * @ParamIn PresetNo 预置点编号
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2GotoPreset(long hSession,const char * ProfileToken,
									   unsigned int PresetNo);


/**
    * 对焦开始
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn ProfileToken （libOnvif2GetPTZInfo返回的PTZInfo->ProfileToken）
    * @ParamIn Action 操作类型(0,焦点近；1，焦点远)
    * @ParamIn Velocity 放大缩小的速度 0-100
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2FocusMove(long hSession,const char * ProfileToken,
									 FOCUS_ACTION Action,int Velocity);

/**
    * 对焦停止
    *
    * @ParamIn hSession 设备控制句柄
    * @ParamIn ProfileToken （libOnvif2GetPTZInfo返回的PTZInfo->ProfileToken）
    * @Return 错误码
**/
LIBONVIF2_API int libOnvif2FocusStop(long hSession,const char * ProfileToken);





#endif

