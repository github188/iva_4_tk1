#ifndef LIBONVIF2_INTERNAL_H
#define LIBONVIF2_INTERNAL_H

#include "libonvif2struct.h"
#include <time.h>

typedef struct
{
	// [设备]
	char Dev_XAddr[URI_LENGTH];		// 设备的服务地址
	bool Dev_RemoteDiscovery;		// 表明设备支持远程发现
	char Dev_OnvifVersions[ONVIF_VERSION_LENGTH];	// 设备所能支持ONVIF标准规范的版本清单
	char Image_XAddr[URI_LENGTH];	// 图像服务地址
	char Media_XAddr[URI_LENGTH];	// 媒体服务地址
	char PTZ_XAddr[URI_LENGTH];		// 云台服务的地址
}Capabilities;

typedef struct
{
	bool FromDHCP;			// 主机名是通过DHCP(动态主机分配协议)获得
	char Name[IP_LENGTH];	// 主机名
}Hostname;

typedef struct
{
	char InterfaceToken[TOKEN_LENGTH];

	bool Enabled;

	char Name[256];
	char HwAddress[256];
	int  MTU;

	bool IPv4Enabled;
	char IPv4Address[IP_LENGTH];
	char IPv4Mask[IP_LENGTH];
}NetWorkIF;

typedef struct
{
	bool HTTP_Enabled;
	int  HTTP_Port;

	bool HTTPS_Enabled;
	int  HTTPS_Port;

	bool RTSP_Enabled;
	int  RTSP_Port;
}NetworkProtocols;

typedef struct
{
	char Manufacturer[256];
	char Model[256];
	char FirmwareVersion[256];
	char SerialNumber[256];
	char HardwareId[256];
}DeviceInfomation;

typedef struct
{
	long year;
	long month;
	long day;
	long hour;
	long minute;
	long second;
}DevDateTime;


typedef struct
{
	float Position;
	long  MoveStatus;// IDLE = 0, MOVING = 1, UNKNOWN = 2
	char  Error[256];
}ImagingStatus;

typedef struct
{
	PTZ_POSITION position;
	long  PTMoveStatus;// IDLE = 0, MOVING = 1, UNKNOWN = 2
	long  ZMoveStatus;// IDLE = 0, MOVING = 1, UNKNOWN = 2
	char  Error[256];
	time_t UtcTime;
}PTZStatus;



#define ONVIF_DEBUG_OPEN  1
#define ONVIF_DEBUG_CLOSE 0

class OnvifInternal
{
public:
	OnvifInternal(){}
	~OnvifInternal(){}

public:
	// 设置调试模式
	static void SetDebugModel(int Model = ONVIF_DEBUG_CLOSE);

	// 设置超时时间 > 0 in seconds, < 0 in usec
	static void SetTimeOut(int SendTimeOut = 2, int RecvTimeOut = 2, int ConnTimeOut = 2);

private:
	static void PrintD(const char *format, ...);
	static int m_iDebugModel;
	static int m_lConnTimeOut;
	static int m_lSendTimeOut;
	static int m_lRecvTimeOut;

public:// [设备发现]

	// 设备发现
	static int ProbeAllDevices(LIST_NVT * Devices);

	// 指定设备发现 获取设备服务地址或设备地址
	static int ProbeDevice(const char* IP, NVTInfo* NvtInfo);

	// 指定设备发现 获取设备服务地址或设备地址 wsdd方式
	static int ProbeDeviceByWsdd(const char* IP, NVTInfo* NvtInfo);

	// 解析NVTInfo的Scopes
	static int ParseDeviceScopes(const char * StrScopes, OnvifScopes * Scopes);

public:// [设备管理-功能]

	// 获取WSDL的URL 设备必须能返回一个用于下载WSDL和概要信息的URL
	static int GetWsdlUrl(const char* XAddrs, const char* UserName,const char* Password,
		char * WsdlUrl);

	// 交换的功能
	static int GetCapabilities(const char* XAddrs, const char* UserName,const char* Password,
		Capabilities * Caps);

public:// [设备管理-网络]

	// 主机名
	static int GetHostname(const char* XAddrs, const char* UserName,const char* Password,
		Hostname * Host);
	static int SetHostname(const char* XAddrs, const char* UserName,const char* Password,
		const char * HostName);

	// DNS
	static int GetDNS(const char* XAddrs, const char* UserName,const char* Password,
		char * DNS);
	static int SetDNS(const char* XAddrs, const char* UserName,const char* Password,
		const char * DNS);

	// NTP
	static int GetNTP(const char* XAddrs, const char* UserName,const char* Password,
		char * NTP);
	static int SetNTP(const char* XAddrs, const char* UserName,const char* Password,
		const char * NTP);

	// 网络接口
	static int GetNetworkInterfaces(const char* XAddrs, const char* UserName,const char* Password,
		NetWorkIF * ifs);
	static int SetNetworkInterfaces(const char* XAddrs, const char* UserName,const char* Password
		, const NetWorkIF * ifs);
	static int GetNetworkDefaultGateway(const char* XAddrs, const char* UserName,const char* Password,
		char * gateway);
	static int SetNetworkDefaultGateway(const char* XAddrs, const char* UserName,const char* Password,
		const char * gateway);

	// 网络协议
	static int GetNetworkProtocols(const char* XAddrs, const char* UserName,const char* Password,
		NetworkProtocols * Protocols);
	//static int SetNetworkProtocols(const char* XAddrs, const char* UserName,const char* Password, const NetworkProtocols * Protocols);

public:// [设备管理-系统]

	// 获取设备信息
	static int GetDeviceInformation(const char* XAddrs, const char* UserName,const char* Password,
		DeviceInfomation * DevInfo);

	// 获取系统日期以及时间
	static int GetSystemDateAndTime(const char* XAddrs, const char* UserName,const char* Password,
		DevDateTime * DevTime);
	static int SetSystemDateAndTime(const char* XAddrs, const char* UserName,const char* Password,
		const DevDateTime * DevTime);

	// 系统重启
	static int SystemReboot(const char* XAddrs, const char* UserName,const char* Password);

	// 获取参数范围
	static int GetScopes(const char* XAddrs, const char* UserName,const char* Password);

public:// [设备管理-安全]

	// 获取访问策略
	static int GetAccessPolicy(const char* XAddrs, const char* UserName,const char* Password);

	// 用户
	static int GetUsers(const char* XAddrs, const char* UserName,const char* Password,
		User * pUsers);
	static int CreateUsers(const char* XAddrs, const char* UserName,const char* Password,
		const UserInfo * UserInfo);
	static int DeleteUsers(const char* XAddrs, const char* UserName,const char* Password,
		const UserInfo * UserInfo);
	static int SetUser(const char* XAddrs, const char* UserName,const char* Password,
		const UserInfo * UserInfo);

public:// [媒体]

	// 媒体文件Profile
	static int GetAllProfiles(const char* MediaURI, const char* UserName,const char* Password,
		LIST_Profile * ProfileList);

	// 获取视频源集
	static int GetVideoSources(const char* XAddrs, const char* UserName,const char* Password,
		LIST_VideoSource *VideoSourceList);

	//请求一个URI，该URI可以被用来启用一个使用RTSP作为控制协议的实时媒体流
	static int GetStreamUri(const char* XAddrs, const char* UserName,const char* Password,
		const char* ProfileToken, StreamURI *StreamUri);

	//从NVT获得JPEG 快照，URI可通过一个HTTP GET操作获取JPEG图像。图像编码总是JPEG的编码设置
	static int GetSnapshotUri(const char* XAddrs, const char* UserName,const char* Password,
		const char* ProfileToken, char *SnapshotUri);

public:// [图像]

	// 调焦距
	static int AbsoluteFocusMove(const char* XAddrs, const char* UserName,const char* Password,
		const char* VideoSourceToken, float Position, float Speed);
	static int RelativeFocusMove(const char* XAddrs, const char* UserName,const char* Password,
		const char* VideoSourceToken, float Distance, float Speed);
	static int ContinuousFocusMove(const char* XAddrs, const char* UserName,const char* Password,
		const char* VideoSourceToken, float Speed);
	static int FocusStop(const char* XAddrs, const char* UserName,const char* Password,
		const char* VideoSourceToken);

	// 获取图像设备状态。如果设备支持焦点移动控制。焦点位置，移动状态（运行中，空闲或未知）和错误信息。
	static int GetImagingStatus(const char* XAddrs, const char* UserName,const char* Password,
		const char* VideoSourceToken, ImagingStatus * Status);

public:// PTZ

	// 获取所有节点
	static int PTZ_GetNodes(const char* XAddrs, const char* UserName,const char* Password, 
		LIST_PTZNode * ptzNodeSet);


	// 绝对的移动
	static int PTZ_AbsoluteMove(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, PTZ_POSITION * Position, PTZ_SPEED * Speed);

	// 相对移动
	static int PTZ_RelativeMove(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, PTZ_POSITION * Translation, PTZ_SPEED * Speed);

	// 连续移动time_out>0有效 <=0 无效
	static int PTZ_ContinuousMove(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, PTZ_SPEED * Velocity, long time_out);

	// 停止
	static int PTZ_Stop(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, bool bStopPanTilt, bool bStopZoom);

	// 读取状态
	static int PTZ_GetStatus(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, PTZStatus * status);

	// 读取所有预设值
	static int PTZ_GetPresets(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, LIST_Preset *presetset);

	// 设置预设值
	// strlen(PresetToken)=0 创建一个新的预设。假如创建成功，响应就会反馈一个唯一的标识符。
	// strlen(PresetToken)>0 通过相应的预设标识符，预设可以被重新设定
	static int PTZ_SetPreset(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, const char* PresetName, char* PresetToken);

	// 调用预设
	static int PTZ_GotoPreset(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, const char* PresetToken, const PTZ_SPEED *speed);

	// 移除预设
	static int PTZ_RemovePreset(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, const char* PresetToken);

	// 转到归位点
	static int PTZ_GotoHomePosition(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, const PTZ_SPEED *speed);

	// 设置归位点
	static int PTZ_SetHomePosition(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken);

	// 发送辅助命令
	//static int PTZ_SendAuxiliaryCommand(const char* XAddrs, const char* UserName,const char* Password, 
	//	const char* ProfileToken, const char *AuxiliaryData);

};

#endif