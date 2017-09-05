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

// ���ļ������˲�Ʒ��һЩ��������

#define MAX_CHANNEL_NUM  1	// ���ͨ����
#define MAX_SCENE_NUM    8	// 1��ͨ����󳡾���

#define PAS_EDITION			5	// ͣ������ץ�İ汾park analysis system
#define FSS_EDITION			6	// ���ٲ��ذ汾fast supervise system

#define TIME_SYN_PRECISION	2	// ʱ��ͬ������

#define MOUNT_POINT_ROM		"/"				// �����޸�
#define MOUNT_POINT_USB		"/mnt/usb"		// USB���ص�
#define MOUNT_POINT_HDD 	"/mnt/hdd"		// Ӳ�̹��ص�
#define MOUNT_POINT_SD 		"/mnt/sd"		// SD�����ص�

#define IDENTIFY_DIR		"ivadata"		// ���ݴ洢�ı�ʶ�ļ���
#define RECORD_TEMP_DIR		"rectmp"		// ¼����ʱ�ļ���
#define RECORD_DIR			"record"		// ¼���ļ���
#define RESULT_EVENT_DIR	"event"
#define RESULT_ILLEGAL_DIR	"illegal"
#define RESULT_TOLL_DIR		"toll"

#define DOWNLOAD_DIR		"download"		// �������صĸ�Ŀ¼

#define DEFAULT_ROOT_PATH	"/ivadata"

// ¼��MP/IDENTIFY_DIR/RECORD_DIR/YYYYMMDD/******.***
// ȡ֤��MP/IDENTIFY_DIR/RESULT_***_DIR/YYYYMMDD/******.***
// ������


#define DEFAULT_BIN_PATH	"/opt/iva/"


// Υ�����ͱ�
enum
{
	eIT_Illegal_Park = 0,	// Υ��ͣ��
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
	char szName[128];				// ����ȡ֤ģʽ��ʲô
	ValueScopeDef tImageNumScope;	// ����ȡ֤ģʽ֧�ֵ�ץ��������Χ
}EvidenceModelDef;

#define  MAX_THRESHOLD_NUM	2
typedef struct _ThresholdDef
{
	int  iWhich;		// ��һ��ֵ 0-iValue1, 1-iValue2, 2-fValue1, 3-fValue2
	char szName[64];	// �����ֵ��ʲô
	char szUnit[64];	// ��λ��ʲô
	ValueScopeDef tScope;	// ȡֵ��Χ
}ThresholdDef;

typedef struct _EventDef
{
	char szName[128];				// ��ʲô����
	bool bSupportEvent;				// �Ƿ�֧���¼�
	ValueScopeDef tEventImageNum;	// �¼�ץ��ͼƬ������Χ

	bool bSupportIllegal;			// �Ƿ�֧��Υ��
	int  iEviModelNum;				// ֧�ּ���ȡ֤ģʽ
	EvidenceModelDef tIllegalModelDef[MAX_EVIMODEL_NUM];// ����ȡ֤ģʽ�Ķ���

	int  iThresholdNum;				// �ж�Υ����Ҫ������ֵ
	ThresholdDef  tThresholdDef[MAX_THRESHOLD_NUM];//��ֵ����

	bool bSupportCalibrate;		// �Ƿ���Ҫ�궨
	bool bSupportShieldArea;	// �Ƿ���Ҫ��������
	bool bSupportAnalyArea;		// �Ƿ���Ҫ��������
	bool bSupportPolyline;		// �Ƿ���Ҫ�ж��߶�
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

