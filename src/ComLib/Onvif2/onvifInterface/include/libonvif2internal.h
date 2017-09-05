#ifndef LIBONVIF2_INTERNAL_H
#define LIBONVIF2_INTERNAL_H

#include "libonvif2struct.h"
#include <time.h>

typedef struct
{
	// [�豸]
	char Dev_XAddr[URI_LENGTH];		// �豸�ķ����ַ
	bool Dev_RemoteDiscovery;		// �����豸֧��Զ�̷���
	char Dev_OnvifVersions[ONVIF_VERSION_LENGTH];	// �豸����֧��ONVIF��׼�淶�İ汾�嵥
	char Image_XAddr[URI_LENGTH];	// ͼ������ַ
	char Media_XAddr[URI_LENGTH];	// ý������ַ
	char PTZ_XAddr[URI_LENGTH];		// ��̨����ĵ�ַ
}Capabilities;

typedef struct
{
	bool FromDHCP;			// ��������ͨ��DHCP(��̬��������Э��)���
	char Name[IP_LENGTH];	// ������
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
	// ���õ���ģʽ
	static void SetDebugModel(int Model = ONVIF_DEBUG_CLOSE);

	// ���ó�ʱʱ�� > 0 in seconds, < 0 in usec
	static void SetTimeOut(int SendTimeOut = 2, int RecvTimeOut = 2, int ConnTimeOut = 2);

private:
	static void PrintD(const char *format, ...);
	static int m_iDebugModel;
	static int m_lConnTimeOut;
	static int m_lSendTimeOut;
	static int m_lRecvTimeOut;

public:// [�豸����]

	// �豸����
	static int ProbeAllDevices(LIST_NVT * Devices);

	// ָ���豸���� ��ȡ�豸�����ַ���豸��ַ
	static int ProbeDevice(const char* IP, NVTInfo* NvtInfo);

	// ָ���豸���� ��ȡ�豸�����ַ���豸��ַ wsdd��ʽ
	static int ProbeDeviceByWsdd(const char* IP, NVTInfo* NvtInfo);

	// ����NVTInfo��Scopes
	static int ParseDeviceScopes(const char * StrScopes, OnvifScopes * Scopes);

public:// [�豸����-����]

	// ��ȡWSDL��URL �豸�����ܷ���һ����������WSDL�͸�Ҫ��Ϣ��URL
	static int GetWsdlUrl(const char* XAddrs, const char* UserName,const char* Password,
		char * WsdlUrl);

	// �����Ĺ���
	static int GetCapabilities(const char* XAddrs, const char* UserName,const char* Password,
		Capabilities * Caps);

public:// [�豸����-����]

	// ������
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

	// ����ӿ�
	static int GetNetworkInterfaces(const char* XAddrs, const char* UserName,const char* Password,
		NetWorkIF * ifs);
	static int SetNetworkInterfaces(const char* XAddrs, const char* UserName,const char* Password
		, const NetWorkIF * ifs);
	static int GetNetworkDefaultGateway(const char* XAddrs, const char* UserName,const char* Password,
		char * gateway);
	static int SetNetworkDefaultGateway(const char* XAddrs, const char* UserName,const char* Password,
		const char * gateway);

	// ����Э��
	static int GetNetworkProtocols(const char* XAddrs, const char* UserName,const char* Password,
		NetworkProtocols * Protocols);
	//static int SetNetworkProtocols(const char* XAddrs, const char* UserName,const char* Password, const NetworkProtocols * Protocols);

public:// [�豸����-ϵͳ]

	// ��ȡ�豸��Ϣ
	static int GetDeviceInformation(const char* XAddrs, const char* UserName,const char* Password,
		DeviceInfomation * DevInfo);

	// ��ȡϵͳ�����Լ�ʱ��
	static int GetSystemDateAndTime(const char* XAddrs, const char* UserName,const char* Password,
		DevDateTime * DevTime);
	static int SetSystemDateAndTime(const char* XAddrs, const char* UserName,const char* Password,
		const DevDateTime * DevTime);

	// ϵͳ����
	static int SystemReboot(const char* XAddrs, const char* UserName,const char* Password);

	// ��ȡ������Χ
	static int GetScopes(const char* XAddrs, const char* UserName,const char* Password);

public:// [�豸����-��ȫ]

	// ��ȡ���ʲ���
	static int GetAccessPolicy(const char* XAddrs, const char* UserName,const char* Password);

	// �û�
	static int GetUsers(const char* XAddrs, const char* UserName,const char* Password,
		User * pUsers);
	static int CreateUsers(const char* XAddrs, const char* UserName,const char* Password,
		const UserInfo * UserInfo);
	static int DeleteUsers(const char* XAddrs, const char* UserName,const char* Password,
		const UserInfo * UserInfo);
	static int SetUser(const char* XAddrs, const char* UserName,const char* Password,
		const UserInfo * UserInfo);

public:// [ý��]

	// ý���ļ�Profile
	static int GetAllProfiles(const char* MediaURI, const char* UserName,const char* Password,
		LIST_Profile * ProfileList);

	// ��ȡ��ƵԴ��
	static int GetVideoSources(const char* XAddrs, const char* UserName,const char* Password,
		LIST_VideoSource *VideoSourceList);

	//����һ��URI����URI���Ա���������һ��ʹ��RTSP��Ϊ����Э���ʵʱý����
	static int GetStreamUri(const char* XAddrs, const char* UserName,const char* Password,
		const char* ProfileToken, StreamURI *StreamUri);

	//��NVT���JPEG ���գ�URI��ͨ��һ��HTTP GET������ȡJPEGͼ��ͼ���������JPEG�ı�������
	static int GetSnapshotUri(const char* XAddrs, const char* UserName,const char* Password,
		const char* ProfileToken, char *SnapshotUri);

public:// [ͼ��]

	// ������
	static int AbsoluteFocusMove(const char* XAddrs, const char* UserName,const char* Password,
		const char* VideoSourceToken, float Position, float Speed);
	static int RelativeFocusMove(const char* XAddrs, const char* UserName,const char* Password,
		const char* VideoSourceToken, float Distance, float Speed);
	static int ContinuousFocusMove(const char* XAddrs, const char* UserName,const char* Password,
		const char* VideoSourceToken, float Speed);
	static int FocusStop(const char* XAddrs, const char* UserName,const char* Password,
		const char* VideoSourceToken);

	// ��ȡͼ���豸״̬������豸֧�ֽ����ƶ����ơ�����λ�ã��ƶ�״̬�������У����л�δ֪���ʹ�����Ϣ��
	static int GetImagingStatus(const char* XAddrs, const char* UserName,const char* Password,
		const char* VideoSourceToken, ImagingStatus * Status);

public:// PTZ

	// ��ȡ���нڵ�
	static int PTZ_GetNodes(const char* XAddrs, const char* UserName,const char* Password, 
		LIST_PTZNode * ptzNodeSet);


	// ���Ե��ƶ�
	static int PTZ_AbsoluteMove(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, PTZ_POSITION * Position, PTZ_SPEED * Speed);

	// ����ƶ�
	static int PTZ_RelativeMove(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, PTZ_POSITION * Translation, PTZ_SPEED * Speed);

	// �����ƶ�time_out>0��Ч <=0 ��Ч
	static int PTZ_ContinuousMove(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, PTZ_SPEED * Velocity, long time_out);

	// ֹͣ
	static int PTZ_Stop(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, bool bStopPanTilt, bool bStopZoom);

	// ��ȡ״̬
	static int PTZ_GetStatus(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, PTZStatus * status);

	// ��ȡ����Ԥ��ֵ
	static int PTZ_GetPresets(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, LIST_Preset *presetset);

	// ����Ԥ��ֵ
	// strlen(PresetToken)=0 ����һ���µ�Ԥ�衣���紴���ɹ�����Ӧ�ͻᷴ��һ��Ψһ�ı�ʶ����
	// strlen(PresetToken)>0 ͨ����Ӧ��Ԥ���ʶ����Ԥ����Ա������趨
	static int PTZ_SetPreset(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, const char* PresetName, char* PresetToken);

	// ����Ԥ��
	static int PTZ_GotoPreset(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, const char* PresetToken, const PTZ_SPEED *speed);

	// �Ƴ�Ԥ��
	static int PTZ_RemovePreset(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, const char* PresetToken);

	// ת����λ��
	static int PTZ_GotoHomePosition(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken, const PTZ_SPEED *speed);

	// ���ù�λ��
	static int PTZ_SetHomePosition(const char* XAddrs, const char* UserName,const char* Password, 
		const char* ProfileToken);

	// ���͸�������
	//static int PTZ_SendAuxiliaryCommand(const char* XAddrs, const char* UserName,const char* Password, 
	//	const char* ProfileToken, const char *AuxiliaryData);

};

#endif