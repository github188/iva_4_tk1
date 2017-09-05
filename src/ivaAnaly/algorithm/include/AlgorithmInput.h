/*!**********************************************************************
*\brief     ���ܷ����㷨������ṹ������ͷ�ļ�
*\details   
*\author    Xuyongxi
*\version    v1.0
*\date      2015 �� 12 �� 9 ��
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

// �����õ��ĺ�
#define  ALG_MAX_CHANNEL_NUM    	 1	// ���ͨ����
#define  ALG_MAX_SCENE_NUM     		16	// 1��ͨ����󳡾���
#define  ALG_MAX_SHEILD_AREA_NUM     8  // 1�����������������ĸ���
#define  ALG_MAX_PARK_AREA_NUM       8  // 1���������Υͣ����ĸ���
#define  ALG_MAX_LANE_NUM      	     8  // 1��������󳵵�����
#define  ALG_MAX_SOLID_LINE_NUM      8  // 1���������ʵ�߸���
#define  ALG_MAX_UTURN_LINE_NUM      8  // 1����������ֹ��ͷ�߸���
#define  ALG_MAX_CHANGE_LINE_NUM     8  // 1����������ֹ����߸���
#define  ALG_MAX_POINT_NUM		  	10  // 1������������������ĵ���
#define  ALG_MAX_SNAP_NUM			 4	// 1���¼����ץ������
#define  ALG_MAX_PRESET_NUMBER      16  // ���Ԥ��λ����


typedef  enum 
{
	IMAGE_DATA_TYPE_YUV420 = 0,
	IMAGE_DATA_TYPE_NV12   = 1,
	IMAGE_DATA_TYPE_BGR    = 2,
	IMAGE_DATA_TYPE_UNKNOW
}EImageDataType;


/*!�������͵�2ά������*/
typedef struct
{
	int x;
	int y;
}ALG_Point;

/*!�������͵�2ά������*/
typedef struct
{
    float x;
    float y;
}ALG_PointF;

/*!�������͵ľ���*/
typedef struct
{
	int x;
	int y;
	int w;
	int h;
}ALG_Rect;


/*��������*/
typedef enum
{
	LANE_TYPE_NORMAL,			// ��ͨ����
	LANE_TYPE_OVERTAKING_ROAD,	// �����������󳵲��������������ʻ
	LANE_TYPE_EMERGENCY_ROAD,	// Ӧ������
	LANE_TYPE_BUS_PURPOSE,		// ������ר�ó���
	LANE_TYPE_NON_MOTOR_VEHICLE,// �ǻ�������
	LANE_TYPE_LIGHT_VEHICLE,	// С�ͳ���
	LANE_TYPE_UNKONWN			// ��������δ֪
}ELaneType;


/*!������/�������Ϣ�ṹ��*/
typedef struct _tArea
{
	unsigned int  u32PointNum;/*��ĸ���*/
	ALG_Point     stPoint[ALG_MAX_POINT_NUM];/*�㼯��*/
}tArea,tLine;

/*!���������ýṹ��*/
typedef struct _tSheildAreaConf
{
	unsigned int u32AreaCount; /*����ĸ��� */
	tArea        stArea[ALG_MAX_SHEILD_AREA_NUM];   /*�������ļ���*/
}tSheildAreaConf;

/*!Υͣ�����ýṹ��*/
typedef struct _tParkAreaConf
{
	unsigned int u32AreaCount; /*����ĸ��� */
	tArea        stArea[ALG_MAX_PARK_AREA_NUM];   /*Υͣ���ļ���*/
}tParkAreaConf;

/*!�궨�����ṹ��*/
typedef struct
{
	ALG_PointF tPtsInImage[4];
	float  fActualWidth;	// ���ε�ʵ�ʿ���λ��m
	float  fActualHeight;	// ���ε�ʵ�ʸߣ���λ��m
}tCalibrationConf;

/*!���������ò����ṹ��*/
typedef struct
{
	int    	  s32LaneID;		// ������� 
	int       s32LaneType;		// ��������,���Ͳ���ELaneType
	ALG_Point tPtLaneCor[4];	// �������꣬Ϊ4����
	ALG_Point tDirectPt[2];		// ��������tDirectPt[0]Ϊ��㣬tDirectPt[1]Ϊ�յ�
}tSingleLaneConf;

/*!�������ò����ṹ��*/
typedef struct _tLaneParamConf
{
	unsigned int     u32LaneNum;// ������Ŀ
	tSingleLaneConf  tOneLaneConf[ALG_MAX_LANE_NUM];// �������ýṹ��
}tLaneParamConf;

/*! ���߹������ýṹ��*/
typedef struct _tSolidLineConf
{
	unsigned int  u32LineNum;/*!���ߵĸ���*/
	tLine   	  tLineConf[ALG_MAX_SOLID_LINE_NUM];/*!���߹��򼯺�*/
}tSolidLineConf;

//������ͷ/�����������
typedef struct tagSingleUTurnLine
{
	unsigned int  PointNum;					// �����ϵ�ĸ���
	ALG_Point     Point[ALG_MAX_POINT_NUM];	// �㼯��

	int LeftRightLaneNO[2];					// ��ǰ�������������������ı��
	int CrossAllowFlag;						// 0:˫������ 1:��[0]->[1]����[1]->[0]������  2:��[1]->[0]����[0]->[1]������  3:˫������
}tSingleUTurnLine;

typedef struct tagUTurnLineConf
{
	unsigned int		SingleUTurnNum;				     // ��ͷ/�������ĸ���
	tSingleUTurnLine	SingleUTurn[ALG_MAX_UTURN_LINE_NUM]; // ��ͷ/������򼯺�
}tUturnLineConf;

typedef enum
{
	FULL_SHOT,			// ȫ��
	CLOSE_UP,			// ����
	CLOSE_SHOT			// ��д
}ESnapType;

// �澯����
typedef enum 
{
	AT_NORMAL= -1,					// ����״̬
	AT_ILLEGAL_STOP_CAR,			// Υ��ͣ��          // WTP  IVR  HWD
	AT_REVERSE_DRIVING, 			// ����/����         //      IVR  HWD
	AT_CAR_ILLEGE_UTURN,			// Υ�µ�ͷ		    //      IVR
	AT_CAR_PRESS_LINE,				// ѹʵ��            //      IVR
	AT_ILLEGAL_OCCUPY_LANE, 		// Υ��ռ��	        //      IVR  HWD	// ��Υ��ռ��/��ռ��...�ȵȶ�����
	AT_ABANDONED_OBJECT,			// ����������澯     // 	    HWD
	AT_PEDESTRIAN,    				// �������˸澯       // 			HWD
	AT_HEAVE_TRAFFIC_CROWD,         // ��ͨӵ��          //           HWD
	AT_ILLEGAL_CHANGE_LANE,         // Υ�±��          //      IVR
	AT_ONBOARD_OCR					// ����ʶ����       //      IVR
}ALG_AlarmType;

// �¼�ȡ֤
typedef struct
{
	ALG_AlarmType eEventType;	// �¼�����
	unsigned int  u32SnapCnt;  	// ץ������ Υͣ2-4 ����1-3
	ESnapType     eSnapType[ALG_MAX_SNAP_NUM];// ȫ��/��д/(����)
	unsigned int  u32Interval[ALG_MAX_SNAP_NUM];// ���ʱ��(Υͣר��)
	int		      bEnable;//�Ƿ�ȡ֤
	int		      bEvent;//�Ƿ���Ҫ�¼���¼
}tEventEvidenceConf;

//�¼�����
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

/*!��Ԥ��λ�㷨���ýṹ��*/
typedef  struct tagSinglePresetConf
{
	unsigned int     u32ChannelID;		// ͨ����
	int				 s32PresetId;		// Ԥ��λ���
	tPresetAbsPtz	 tAbsPtz;			// Ԥ��λPTZFֵ

	float			 fClickZoomInRate;	// ����Ŵ���
	float			 fClickZoomDistance;// ��̬����Ԥ��������
	unsigned int     u32FuncFlag;		// ���ܿ��ر�ʶ ��λ��ʾ

	tCalibrationConf tCalibConf;		// �궨����
	float            fMaxObjSize;		// ���Ŀ�����
	float            fMinObjSize;		// ��СĿ�����

	tSheildAreaConf  tSheildConf;		// �����������

	unsigned int     u32IllegeStopTime; // �ж�Ϊ Υͣ     ����Сʱ�䣬��λ��s
	tParkAreaConf    tParkArea;			// Υͣ����

	float            fReverseDriveDis;  // �ж�Ϊ ����/��������С��ʻ���룬��λ����
	float            fIllegeOccupyDis;  // �ж�Ϊ �Ƿ�ռ�� ����С��ʻ��������λ����
	tLaneParamConf   tLaneConf;			// ��������

	unsigned int     u32UTurnAngle;	    // �ж�Ϊ Υ����ͷ ����С�Ƕȣ���
	tUturnLineConf	 tUTurnConf;        // Υ����ͷ��������

	unsigned int     u32PressLineTime;  // �ж�Ϊ ѹʵ��   ����Сʱ�䣬��λ��s
	float            fPressLineDis;     // �ж�Ϊ ѹʵ��   ����С��ʻ���룬��λ����
	tSolidLineConf   tSolidConf;		// ʵ�߹�������

	tUturnLineConf   LaneChangeConf;	// Υ�±��������
}tSinglePresetConf;

//Ԥ��λ�л��б�
typedef struct _tPresetList
{
	//Ԥ��λ����
	int PresetNum;
	//ÿ��Ԥ��λ������
	tSinglePresetConf	SinglePreset[ALG_MAX_PRESET_NUMBER];
	//ÿ��ȡ֤����
	tEventSetting		EventSetting[ALG_MAX_PRESET_NUMBER];
	//Ԥ��λ���ʱ��
	float				MinStopTime[ALG_MAX_PRESET_NUMBER];
}tPresetList;


// ���ܿ���
#define USE_PARK	0x0001	// Υͣ


// PTZ����
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



