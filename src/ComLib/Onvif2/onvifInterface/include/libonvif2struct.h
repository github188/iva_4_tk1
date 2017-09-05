#ifndef _LIB_ONVIF2_STRUCT_H_
#define _LIB_ONVIF2_STRUCT_H_

#define MANUFACYURER_LENGTH 64
#define MODEL_LENGTH 32
#define FIRMWARE_VERSION_LENGTH 64
#define DEVICE_SERIAL_NUMBER_LENGTH 64
#define HARDWARE_ID_LENGTH 64
#define IP_LENGTH 64
#define MAC_LENGTH 64
#define DEVICENAME_LENGTH 64
#define LOCATION_LENGTH 64
#define URI_LENGTH 128
#define USERNAME_LENGTH 64
#define PASSWD_LENGTH 64
#define USER_SIZE 32
#define RESOLUTION_SIZE 32
#define PRESET_TOKEN_LENGTH 64
#define PRESET_NAME_LENGTH 64
#define PRESET_SIZE 512//32
#define ONVIF_VERSION_LENGTH 8
#define SOURCE_SIZE 32
#define STREAM_SIZE 8
#define STREAM_NAME_LENGTH 64
#define TOKEN_LENGTH 64

#include <list>
#include <map>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define USE_PTZ_SPACE


// 设备发现后的NVT信息
typedef struct
{	
	char xAddrs[URI_LENGTH];
	char Types[URI_LENGTH];
	char Item[URI_LENGTH*10];
    char MatchBy[URI_LENGTH];
	unsigned int MetadataVersion;
}MatchInfo;
typedef list<MatchInfo> LIST_MatchInfo;

typedef struct
{	
	char IP[IP_LENGTH];
	LIST_MatchInfo MatchInfos;
}NVTInfo;
typedef list<NVTInfo> LIST_NVT;

typedef struct
{
	char type[256];
	char Profile[256];
	char hardware[256];
	char name[256];
	char location[256];
}OnvifScopes;


typedef struct
{
	int x;
	int y;
	int width;
	int height;
}IntRect;

typedef struct
{
	float left;
	float right;
	float top;
	float bottom;
}FloatRect;

typedef struct
{
	float Min;
	float Max;
}FRange;


typedef struct
{
	char UserName[USERNAME_LENGTH];
	char PassWord[PASSWD_LENGTH];
	long UserLevel; // 0-管理员 1-操作用户 2-一般用户 3-匿名用户 4-扩展用户
}UserInfo;

typedef struct 
{
	long size;
	UserInfo Users[USER_SIZE];
}User;


typedef struct
{
#ifdef USE_PTZ_SPACE
	char  PT_Space[URI_LENGTH];
#endif
	float fP;
	float fT;

#ifdef USE_PTZ_SPACE
	char  Z_Space[URI_LENGTH];
#endif
	float fZ;
}PTZ_POSITION;

typedef struct
{
#ifdef USE_PTZ_SPACE
	char  PT_Space[URI_LENGTH];
#endif

	float P_Speed;
	float T_Speed;

#ifdef USE_PTZ_SPACE
	char  Z_Space[URI_LENGTH];
#endif

	float Z_Speed;
}PTZ_SPEED;


typedef struct// 坐标空间
{
#ifdef USE_PTZ_SPACE
	char   PT_URI[URI_LENGTH];
#endif
	FRange P_Range;
	FRange T_Range;

#ifdef USE_PTZ_SPACE
	char   Z_URI[URI_LENGTH];
#endif
	FRange Z_Range;
}PositionSpaceT;

typedef struct// 速度空间
{
#ifdef USE_PTZ_SPACE
	char   PT_URI[URI_LENGTH];
#endif
	FRange PT_Range;

#ifdef USE_PTZ_SPACE
	char   Z_URI[URI_LENGTH];
#endif
	FRange Z_Range;
}SpeedSpaceT;


typedef struct
{
	char Name[128];				// 节点名称
	char token[TOKEN_LENGTH];	// 节点句柄

	PositionSpaceT tAbsPositionSpace;	// Absolute支持的Position空间
	PositionSpaceT tRelTranslationSpace;// Relative支持的Translation空间
	PositionSpaceT tConVelocitySpace;	// Continuous支持的Velocity空间
	SpeedSpaceT    tSpeedSapce;			// 支持的速度空间

	int	MaxPresetsNum;			// 最大预置位数
	int	HomeSupported;			// 是否支持回归点
	int FixedHomePosition;		// 是否支持回归点

	//int AuxCmdSize;				// 支持的辅助命令个数
	//char AuxCmds[10][256];		// 支持的辅助命令
}PTZNode;

typedef list<PTZNode> LIST_PTZNode;


typedef struct
{
	char Name[128];
	char token[TOKEN_LENGTH];
	int  UseCount;

	IntRect Bounds;
	char SourceToken[TOKEN_LENGTH];// 对应的Video Source
}VideoSourceConfig;

typedef struct
{
	char Name[128];
	char token[TOKEN_LENGTH];
	int  UseCount;

	int   Encoding;			// 编码类型 JPEG = 0, MPEG4 = 1, H264 = 2
	int   Width;			// 分辨率宽
	int   Height;			// 分辨率高
	long  SessionTimeOut;	// 回话超时时间
	int   Framerate;		// 幀率
	int   Bitrate;			// 码率
	float Quality;			// 图像质量
	int   EncodingInterval; // 编码间隔
	int   GovLength;		// I帧间隔
	int   ProfileType;		// {Mpeg4 SP=0, ASP=1}{H264 Baseline=0, Main=1,Extended=2,High=3)
}VideoEncoderConfig;


typedef struct
{
	char Name[128];
	char token[TOKEN_LENGTH];
	int  UseCount;

	FRange PanLimits;
	FRange TiltLimits;
	FRange ZoomLimits;

	PTZ_SPEED DefPTZSpeed;
	long  DefPTZTimeout;

	char DefAbsPTPostionSpace[URI_LENGTH];
	char DefAbsZPostionSpace[URI_LENGTH];
	char DefRelPTVelocitySpace[URI_LENGTH];
	char DefRelZVelocitySpace[URI_LENGTH];
	char DefConPTTranslationSpace[URI_LENGTH];
	char DefConZTranslationSpace[URI_LENGTH];
	char NodeToken[TOKEN_LENGTH];
}PTZConfig;

typedef struct
{
	char Name[128];
	char token[TOKEN_LENGTH];
	bool fixed;

	// Video source configuration（视频源配置）
	VideoSourceConfig VideoSourceCfg;

	// Audio source configuration（音频源配置）
	//====

	// Video encoder configuration（视频编码器配置）
	VideoEncoderConfig VideoEncoderCfg;

	// Audio encoder configuration（音频编码器配置）
	//====

	// PTZ configuration（PTZ配置）
	PTZConfig PTZCfg;

	// Video analytics configuration（视频分析配置）
	// Metadata configuration（流媒体配置）
	// Audio output configuration（音频输出配置）
	// Audio decoder configuration（音频解码器配置）
}ProfileInfo;

typedef list<ProfileInfo> LIST_Profile;


typedef struct
{
	long      Mode;			// 曝光模式 0-自动 1-手动
	long      Priority;		// 曝光优先级 0-低噪声 1-帧率
	FloatRect Window;

	float MinExposureTime;	// 最小快门
	float MaxExposureTime;	// 最大快门
	float MinGain;			// 最小增益
	float MaxGain;			// 最大增益
	float MinIris;			// 最小光圈
	float MaxIris;			// 最大光圈

	float ExposureTime;		// 快门
	float Gain;				// 增益
	float Iris;				// 光圈
}ExposureConfig;

typedef struct
{
	long  Mode;				//聚焦模式 0-自动 1-手动
	float NearLimit;
	float FarLimit;
	float DefaultSpeed;
}FocusConfig;

typedef struct
{
	float Brightness;			// 亮度
	float ColorSaturation;		// 色彩饱和度
	float Contrast;				// 对比度
	float Sharpness;			// 锐度

	ExposureConfig Exposure;	// 曝光
	FocusConfig    Focus;		// 聚焦
	
	long  IrCutFilterMode;		// 双滤光片切换器(日夜切换) 0-开 1-关 2-自动

	long  BackLightMode;		// 背光模式 0-关 1-开
	float BackLightLevel;		// 背光级别

	long  WhiteBalanceMode;		// 白平衡
	float CrGain;
	float CbGain;

	long  WideDynamicMode;		// 宽动态 0-关 1-开
	float WideDynamicLevel;
}ImagingConfig;

typedef struct
{
	char token[TOKEN_LENGTH];

	float Framerate;
	int   Width;
	int   Height;

	ImagingConfig ImagingCfg;
}VideoSource;

typedef list<VideoSource> LIST_VideoSource;


typedef struct
{
	char Token[PRESET_TOKEN_LENGTH];
	char Name[PRESET_NAME_LENGTH];
	//char Attribute[PRESET_NAME_LENGTH];
	PTZ_POSITION position;
}Preset;
typedef list<Preset> LIST_Preset;

typedef struct
{
	char ProfileToken[TOKEN_LENGTH];
	char RtspUri[URI_LENGTH];
}StreamURI;
typedef list<StreamURI> LIST_StreamURI;

typedef struct
{
	char IP[IP_LENGTH];//IP地址
	char UserName[USERNAME_LENGTH];//用户名，需调用者填充
	char Pass[PASSWD_LENGTH];//密码，需调用者填充

	char ServiceURI[URI_LENGTH];
	char Name[256];
	char Location[256];
	char Hardware[256];

	char OnvifVersion[ONVIF_VERSION_LENGTH];//onvif version
	char DeviceURI[URI_LENGTH];		//onvif device webservice URI
	char ImagingURI[URI_LENGTH];	//onvif imaging webservice URI
	char MediaURI[URI_LENGTH];		//onvif media webservice URI
	char PTZURI[URI_LENGTH];		//onvif PTZ webservice URI

	char Manufacturer[MANUFACYURER_LENGTH];//制造商
	char Model[MODEL_LENGTH];//模型
	char FirmwareVersion[FIRMWARE_VERSION_LENGTH];//固件版本
	char SerialNumber[DEVICE_SERIAL_NUMBER_LENGTH];//序列号
	char HardwareId[HARDWARE_ID_LENGTH];//硬件版本

	LIST_Profile	 ProfileList;
	LIST_StreamURI   StreamURIList;
	LIST_VideoSource VideoSourceList;
	LIST_PTZNode	 PtzNodeList;
	LIST_Preset		 PresetList;
}DeviceInfo;

inline void InitDeviceInfo(DeviceInfo * ptDeviceInfo)
{
	if(ptDeviceInfo)
	{
		memset(ptDeviceInfo->IP, 0, sizeof(ptDeviceInfo->IP));
		memset(ptDeviceInfo->Name, 0, sizeof(ptDeviceInfo->Name));
		memset(ptDeviceInfo->Location, 0, sizeof(ptDeviceInfo->Location));
		memset(ptDeviceInfo->Hardware, 0, sizeof(ptDeviceInfo->Hardware));
		memset(ptDeviceInfo->UserName, 0, sizeof(ptDeviceInfo->UserName));
		memset(ptDeviceInfo->Pass, 0, sizeof(ptDeviceInfo->Pass));
		memset(ptDeviceInfo->OnvifVersion, 0, sizeof(ptDeviceInfo->OnvifVersion));
		memset(ptDeviceInfo->DeviceURI, 0, sizeof(ptDeviceInfo->DeviceURI));
		memset(ptDeviceInfo->ImagingURI, 0, sizeof(ptDeviceInfo->ImagingURI));
		memset(ptDeviceInfo->MediaURI, 0, sizeof(ptDeviceInfo->MediaURI));
		memset(ptDeviceInfo->PTZURI, 0, sizeof(ptDeviceInfo->PTZURI));
		memset(ptDeviceInfo->Manufacturer, 0, sizeof(ptDeviceInfo->Manufacturer));
		memset(ptDeviceInfo->Model, 0, sizeof(ptDeviceInfo->Model));
		memset(ptDeviceInfo->FirmwareVersion, 0, sizeof(ptDeviceInfo->FirmwareVersion));
		memset(ptDeviceInfo->SerialNumber, 0, sizeof(ptDeviceInfo->SerialNumber));
		memset(ptDeviceInfo->HardwareId, 0, sizeof(ptDeviceInfo->HardwareId));
		ptDeviceInfo->ProfileList.clear();
		ptDeviceInfo->StreamURIList.clear();
		ptDeviceInfo->VideoSourceList.clear();
		ptDeviceInfo->PtzNodeList.clear();
		ptDeviceInfo->PresetList.clear();
	}
}


#endif
