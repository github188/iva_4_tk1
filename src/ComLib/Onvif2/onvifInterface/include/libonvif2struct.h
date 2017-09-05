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


// �豸���ֺ��NVT��Ϣ
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
	long UserLevel; // 0-����Ա 1-�����û� 2-һ���û� 3-�����û� 4-��չ�û�
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


typedef struct// ����ռ�
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

typedef struct// �ٶȿռ�
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
	char Name[128];				// �ڵ�����
	char token[TOKEN_LENGTH];	// �ڵ���

	PositionSpaceT tAbsPositionSpace;	// Absolute֧�ֵ�Position�ռ�
	PositionSpaceT tRelTranslationSpace;// Relative֧�ֵ�Translation�ռ�
	PositionSpaceT tConVelocitySpace;	// Continuous֧�ֵ�Velocity�ռ�
	SpeedSpaceT    tSpeedSapce;			// ֧�ֵ��ٶȿռ�

	int	MaxPresetsNum;			// ���Ԥ��λ��
	int	HomeSupported;			// �Ƿ�֧�ֻع��
	int FixedHomePosition;		// �Ƿ�֧�ֻع��

	//int AuxCmdSize;				// ֧�ֵĸ����������
	//char AuxCmds[10][256];		// ֧�ֵĸ�������
}PTZNode;

typedef list<PTZNode> LIST_PTZNode;


typedef struct
{
	char Name[128];
	char token[TOKEN_LENGTH];
	int  UseCount;

	IntRect Bounds;
	char SourceToken[TOKEN_LENGTH];// ��Ӧ��Video Source
}VideoSourceConfig;

typedef struct
{
	char Name[128];
	char token[TOKEN_LENGTH];
	int  UseCount;

	int   Encoding;			// �������� JPEG = 0, MPEG4 = 1, H264 = 2
	int   Width;			// �ֱ��ʿ�
	int   Height;			// �ֱ��ʸ�
	long  SessionTimeOut;	// �ػ���ʱʱ��
	int   Framerate;		// ����
	int   Bitrate;			// ����
	float Quality;			// ͼ������
	int   EncodingInterval; // ������
	int   GovLength;		// I֡���
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

	// Video source configuration����ƵԴ���ã�
	VideoSourceConfig VideoSourceCfg;

	// Audio source configuration����ƵԴ���ã�
	//====

	// Video encoder configuration����Ƶ���������ã�
	VideoEncoderConfig VideoEncoderCfg;

	// Audio encoder configuration����Ƶ���������ã�
	//====

	// PTZ configuration��PTZ���ã�
	PTZConfig PTZCfg;

	// Video analytics configuration����Ƶ�������ã�
	// Metadata configuration����ý�����ã�
	// Audio output configuration����Ƶ������ã�
	// Audio decoder configuration����Ƶ���������ã�
}ProfileInfo;

typedef list<ProfileInfo> LIST_Profile;


typedef struct
{
	long      Mode;			// �ع�ģʽ 0-�Զ� 1-�ֶ�
	long      Priority;		// �ع����ȼ� 0-������ 1-֡��
	FloatRect Window;

	float MinExposureTime;	// ��С����
	float MaxExposureTime;	// ������
	float MinGain;			// ��С����
	float MaxGain;			// �������
	float MinIris;			// ��С��Ȧ
	float MaxIris;			// ����Ȧ

	float ExposureTime;		// ����
	float Gain;				// ����
	float Iris;				// ��Ȧ
}ExposureConfig;

typedef struct
{
	long  Mode;				//�۽�ģʽ 0-�Զ� 1-�ֶ�
	float NearLimit;
	float FarLimit;
	float DefaultSpeed;
}FocusConfig;

typedef struct
{
	float Brightness;			// ����
	float ColorSaturation;		// ɫ�ʱ��Ͷ�
	float Contrast;				// �Աȶ�
	float Sharpness;			// ���

	ExposureConfig Exposure;	// �ع�
	FocusConfig    Focus;		// �۽�
	
	long  IrCutFilterMode;		// ˫�˹�Ƭ�л���(��ҹ�л�) 0-�� 1-�� 2-�Զ�

	long  BackLightMode;		// ����ģʽ 0-�� 1-��
	float BackLightLevel;		// ���⼶��

	long  WhiteBalanceMode;		// ��ƽ��
	float CrGain;
	float CbGain;

	long  WideDynamicMode;		// ��̬ 0-�� 1-��
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
	char IP[IP_LENGTH];//IP��ַ
	char UserName[USERNAME_LENGTH];//�û���������������
	char Pass[PASSWD_LENGTH];//���룬����������

	char ServiceURI[URI_LENGTH];
	char Name[256];
	char Location[256];
	char Hardware[256];

	char OnvifVersion[ONVIF_VERSION_LENGTH];//onvif version
	char DeviceURI[URI_LENGTH];		//onvif device webservice URI
	char ImagingURI[URI_LENGTH];	//onvif imaging webservice URI
	char MediaURI[URI_LENGTH];		//onvif media webservice URI
	char PTZURI[URI_LENGTH];		//onvif PTZ webservice URI

	char Manufacturer[MANUFACYURER_LENGTH];//������
	char Model[MODEL_LENGTH];//ģ��
	char FirmwareVersion[FIRMWARE_VERSION_LENGTH];//�̼��汾
	char SerialNumber[DEVICE_SERIAL_NUMBER_LENGTH];//���к�
	char HardwareId[HARDWARE_ID_LENGTH];//Ӳ���汾

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
