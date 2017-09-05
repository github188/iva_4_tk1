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
	M_HIK		= 0,	// ��������
	M_DH		= 1,	// �󻪿Ƽ�
	M_TIANDY	= 2,	// ���ΰҵ
	M_UNIVIEW	= 3		// ���ӿƼ�
};

enum
{
	STREAM_TYPE_MAIN = 0,	// ������
	STREAM_TYPE_SUB  = 1	// ������
};

typedef struct _tagViSource
{
	bool bEnable;			// ʹ�ܱ��
	char szIP[IPV4_LEN];    // ���IP
	char szUser[NAME_LEN];  // ����û���
	char szPass[PWD_LEN];   // �������
	int  iManuCode;			// ���̱���
	int  iStreamType;       // ��������
}VISource;

typedef struct _OnvifTimeSyn
{
	bool bEnable;      // ʹ��
	u32  uInterval;    // ͬ�����
	int  iSynChnID;	   // ͬ������Ƶͨ��
}OnvifTimeSyn;

enum 
{
	CLICK_ZOOM_PRIVATE = 0,	// ˽��Э��
	CLICK_ZOOM_PROXY   = 1	// ����
};

typedef struct _ClickZoomProxy
{
	int  iModel;			// ����ģʽ
	char szAddr[IPV4_LEN];	// �����ַ
	int	 iPort;				// ����˿�
}ClickZoomProxy;

#define MIN_PRESET_ID	1
#define MAX_PRESET_ID	20

enum
{
	PTZ_MOVE_LEFT		= 0,	// ��
	PTZ_MOVE_LEFT_UP	= 1,	// ����
	PTZ_MOVE_UP			= 2,	// ��
	PTZ_MOVE_RIGHT_UP	= 3,	// ����
	PTZ_MOVE_RIGHT		= 4,	// ��
	PTZ_MOVE_RIGHT_DOWN = 5,	// ����
	PTZ_MOVE_DOWN		= 6,	// ��
	PTZ_MOVE_LEFT_DOWN	= 7,	// ����
	PTZ_MOVE_ZOOM_IN	= 8,	// �Ŵ�
	PTZ_MOVE_ZOOM_OUT	= 9		// ��С
};

enum
{
	APERTURE_INCREASE = 0,	// ��Ȧ����
	APERTURE_DECREASE = 1	// ��Ȧ��С
};

enum
{
	FOCUS_NEAR = 0,	// �����
	FOCUS_FAR  = 1	// ����Զ
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
	PRESET_GOTO = 0, // ����Ԥ��λ
	PRESET_SET  = 1, // ����Ԥ��λ
	PRESET_DEL  = 2	 // ɾ��Ԥ��λ
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


// �ͻ���+����˶���Ҫ���õĽӿ�
MQ_API int MQ_Onvif_Init();
MQ_API int MQ_Onvif_Close();

// �ͻ��˵��õĽӿ�
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

// ����˵��õĽӿ�
MQ_API int MQ_Onvif_Answer_Presets( long lDes, int iRet, const PresetArray *ptPresetArray );
MQ_API int MQ_Onvif_Answer_AbsPos( long lDes, int iRet, const AbsPosition *ptPos );

#ifdef __cplusplus
};
#endif

#endif

