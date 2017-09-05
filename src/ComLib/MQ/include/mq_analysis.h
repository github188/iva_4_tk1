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
	ANALY_CMD_DEVICEINFO,		// MASTER->ANALY �豸��Ϣ
	ANALY_CMD_LICENSE,			// MASTER->ANALY ��Ȩ��Ϣ
	ANALY_CMD_ROOTPATH,			// MASTER->ANALY �洢���� Ŀ¼ ����
	ANALY_CMD_STORE_FULL,		// MASTER->ANALY �洢״̬

	ANALY_CMD_LPR,				// MASTER->ANALY ����ʶ�����
	ANALY_CMD_DISPATCH,			// MASTER->ANALY ���ȿ��Ʋ���

	ANALY_CMD_CHN_OPEN,			// MASTER->ANALY ����ͨ���㷨
	ANALY_CMD_CHN_PLAN,			// MASTER->ANALY ͨ�������ƻ�
	ANALY_CMD_CHN_PLAN_OPEN,	// MASTER->ANALY ����ͨ�������ƻ�
	ANALY_CMD_CHN_NOTIFY_OPEN,	// MASTER->ANALY ����ͨ��΢������
	ANALY_CMD_CHN_PLACE,		// MASTER->ANALY ͨ���ص���Ϣ
	ANALY_CMD_CHN_CLEAR,		// MASTER->ANALY ���ͨ����������
	ANALY_CMD_CHN_MANUAL,		// MASTER->ANALY ͨ���ֶ�ץ��

	ANALY_CMD_CHN_PRESET_POS,	// MASTER->ANALY ͨ��Ԥ��λ����

	ANALY_CMD_CHN_SCENE_INFO,	// MASTER->ANALY ͨ������������Ϣ
	ANALY_CMD_CHN_SCENE_RULE,	// MASTER->ANALY ͨ�������㷨����

	ANALY_CMD_EVENT_DETERMINE,	// MASTER->ANALY Υ���ж�
	ANALY_CMD_EVENT_EVIDENCE,	// MASTER->ANALY ȡ֤ģʽ
	ANALY_CMD_EVENT_IMG_MAKEUP,	// MASTER->ANALY ͼƬȡ֤֮ͼƬ���ɷ�ʽ
	ANALY_CMD_EVENT_IMG_COM,	// MASTER->ANALY ͼƬȡ֤֮ͼƬ�ϳɷ�ʽ
	ANALY_CMD_EVENT_IMG_OSD,	// MASTER->ANALY ͼƬȡ֤֮ͼƬOSD
	ANALY_CMD_EVENT_IMG_QUALITY,// MASTER->ANALY ͼƬȡ֤֮ͼƬ����
	ANALY_CMD_EVENT_REC,		// MASTER->ANALY ¼��ȡ֤

	ANALY_CMD_EVENT_CODE,		// MASTER->ANALY Υ������
	ANALY_CMD_EVENT_NAMING,		// MASTER->ANALY ȡ֤�ļ������淶

	ANALY_CMD_END
};

// �豸��Ϣ
typedef struct _tagDeviceInfo
{
	char szDevID[64];	// �豸���
	char szRegID[64];	// �豸ע���
	char szNetID[64];   // �豸�������
	char szManID[64];	// �豸���̱���
}DeviceInfoT;


// ʡ�ݱ����
enum
{
	eBeiJing 		= 0,	// ��
	eXinJiang		= 1,	// ��
	eNeiMengGu 		= 2,	// ��
	eYunNan 		= 3,	// ��
	eHeNan 			= 4,	// ԥ
	eLiaoNing 		= 5,	// ��
	eGuiZhou 		= 6,	// ��
	eTianJing 		= 7,	// ��
	eFuJian		 	= 8,	// ��
	eHeBei 			= 9,	// ��
	eShanXi1 		= 10,	// ��
	eJiLing 		= 11,	// ��
	eHeLongJiang 	= 12,	// ��
	eShangHai	 	= 13,	// ��
	eJiangSu	 	= 14,	// ��
	eZheJiang	 	= 15,	// ��
	eAnHui 			= 16,	// ��
	eJiangXi		= 17,	// ��
	eShangDong	 	= 18,	// ³
	eHuBei 			= 19,	// ��
	eHuNan 			= 20,	// ��
	eGuangDong 		= 21,	// ��
	eGuangxi 		= 22,	// ��
	eHaiNan 		= 23,	// ��
	eSiChuan 		= 24,	// ��
	eXiZang 		= 25,	// ��
	eShanXi3	 	= 26,	// ��
	eGansu 			= 27,	// ��
	eQingHai 		= 28,	// ��
	eNingXia 		= 29,	// ��
	eChongQin 		= 30	// ��
};
#define MIN_HANZI	eBeiJing
#define MAX_HANZI	eChongQin

// ����ʶ�����
typedef struct _tagLPR
{
	int iDefProv;	// Ĭ��ʡ�ݺ��֣���ʶ��������Ŷ�Ҫ��ʱʹ��
	int iHanziConf;	// �������Ŷ�Ҫ�� 0-100
	int iMinSize;	// ������С�ߴ�
	int iDupTime;	// ��ͬ�����ظ�����ʱ��
	int iDupSens;	// ���ٸ��ַ���ͬ����ͬ���� 0-6
}LPRCfg;

enum
{
	eSenFast		= 0,	// �����ٶȿ���
	eSenCompromise	= 1,	// �ٶ�׼ȷ����
	eSenAccuracy	= 2		// �������׼ȷ
};

enum
{
	ePriByScene = 0,	// ��������
	ePriByTarget= 1		// Ŀ������
};

// ���ȿ���
typedef struct _tagDispatchCtrl
{
	int  iSensitivity;	// ���������� 0-�� 1-ƽ�� 2-׼
	int  iPriority;		// �����л����ȼ�
	int  iSceneMinStay;	// ������Сͣ��ʱ��
	int  iRepeatTime;	// �ظ�ץ��ʱ��
	bool bPtzLock;		// �Ƿ�������̨
	int  iLockTime;		// ����ʱ��
}DispatchCtrlT;


#define MAX_ANALY_PLAN_NUM	4

typedef struct _AnalyPlan
{
	TIME_SLOT tMon[MAX_ANALY_PLAN_NUM];// ����һ
	TIME_SLOT tTue[MAX_ANALY_PLAN_NUM];// ���ڶ�
	TIME_SLOT tWed[MAX_ANALY_PLAN_NUM];// ������
	TIME_SLOT tThu[MAX_ANALY_PLAN_NUM];// ������
	TIME_SLOT tFri[MAX_ANALY_PLAN_NUM];// ������
	TIME_SLOT tSat[MAX_ANALY_PLAN_NUM];// ������
	TIME_SLOT tSun[MAX_ANALY_PLAN_NUM];// ������
}AnalyPlan;

// ��ص���Ϣ
typedef struct _PlaceInfo
{
	char szAreaName[256];              // ��������
	char szAreaID[128];                // ������
	char szPlaceName[256];             // �ص�����
	char szPlaceID[128];               // �ص���
	char szMonitoryName[256];          // ��ص�����
	char szMonitoryID[128];            // ��ص���
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
	int   iEntID;	// �¼�����
	RectT tRect;	// ץ������
}ManualSnapT;

enum
{
	e2UN  = 0,	// δ֪
	eE2W  = 1,	// ������
	eW2E  = 2,	// ����
	eS2N  = 3,	// ����
	eN2S  = 4,	// ������
	eD2U  = 5,	// ����
	eU2D  = 6,	// ����
	eBOTH = 7	// ˫��
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

#define  AREA_MAX_POINT_NUM		 10 // �����������
#define  MAX_SHEILD_AREA_NUM     4  // �����������ĸ���
#define  MAX_ANALY_AREA_NUM      4  // ����������ĸ���

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
	eFull,		// ȫ��
	eCloser,	// ����
	eFeature	// ��д
};

#define  MAX_IMG_NUM	4	// 1����¼���ץ������
#define  MAX_REC_NUM	2	// 1����¼���¼�����

enum
{
	eNotCom = 0,	// ���ϳ�
	eVerCom = 1,    // ��ֱ�ϳ�
	eHorCom = 2,    // ˮƽ�ϳ�
	eMatCom = 3     // ���ֺϳ�
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
	eEachUp   = 0,	// ����ͼƬ�ϵ���
	eEachIn   = 1,  // ����ͼƬ�ڵ���
	eEachDown = 2,  // ����ͼƬ�µ���
	eComUp	  = 3,	// �ϳ�ͼƬ�ϵ���
	eComIn    = 4,	// �ϳ�ͼƬ�ڵ���
	eComDown  = 5   // �ϳ�ͼƬ�µ���
};

#define OSD_TIME		0x0001	// ����ʱ��
#define OSD_MESC		0x0002	// ʱ�����
#define OSD_DEVID		0x0004	// �豸���
#define OSD_PLACE		0x0008	// Υ���ص�
#define OSD_EVENT		0x0010	// Υ����Ϊ
#define OSD_CODE		0x0020	// Υ������
#define OSD_PLATE		0x0040	// ���ƺ���
#define OSD_PLATE_COLOR	0x0080	// ������ɫ
#define OSD_DIRECTION	0x0100	// ����
#define OSD_WATERMARK	0x8000	// ��αˮӡ
typedef struct _ImgOSD
{
	RGBColorT tBackColor;		// ������ɫ
	RGBColorT tFontColor;		// ������ɫ
	int		  iFontSize;		// �����С
	int       iContentFlag;		// ��������
	int		  iPosModel;		// ����λ��
	int		  iLeft;			// ��ʼ��߾�
	int		  iTop;				// ��ʼ�ϱ߾�
}ImgOSDT;

typedef struct _ImgQuality
{
	int iSizeLimit;		// ��С����
	int iWidthLimit;	// �������
	int iHeightLimit;	// �߶�����
}ImgQualityT;

enum 
{
	eRecWhole = 0,	// ȫ��¼��
	eRecJoint = 1	// ƴ��¼��
};

typedef struct _RecEvidence
{
	bool bEnable;
	int  iRecModel;		// ¼��ϳ�ģʽ
	int  iFileFormat;	// ¼���ļ���ʽ	REC_FILE_MP4/AVI
	int  iPreTime;		// Ԥ¼ʱ��
	int  iAftTime;		// �ӳ�ʱ��
	int  iMinTime;		// ���ʱ��
}RecEvidenceT;

typedef struct _EventCode
{
	char szCode[16];	// ���� ���磺1092 001 AB001...
	char szDesc[1024];	// ���� ���磺Υ��ͣ��...
}EventCodeT;

enum 
{
	eUseStartTime	= 0,	// ʹ�ÿ�ʼʱ��
	eUseEndTime		= 1,	// ʹ�ý���ʱ��
	eUseCreateTime	= 2		// ʹ�ô���ʱ��
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

