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
	_Unknown,	// δ֪����
	_Hikvision, // ��������
	_DaHua,		// �㽭��
	_Tiandy,	// ���ΰҵ
	_Uniview	// ���ӿƼ�
}MANUFACTURER_TYPE;

typedef enum
{
	_toLeft      = 0,	// ����
	_toLeftUp    = 1,	// ������
	_toUp        = 2,	// ����
	_toRightUp   = 3,	// ������
	_toRight     = 4,	// ����
	_toRightDown = 5,	// ������
	_toDown      = 6,	// ����
	_toLeftDown  = 7,	// ������
	_toZoomIn    = 8,	// �Ŵ�
	_toZoomOut   = 9	// ��С
}PTZ_ACTION;

typedef enum
{
	_toNear = 0,	// ��
	_toFar  = 1		// Զ
}FOCUS_ACTION;

typedef struct
{
	char ProfileToken[TOKEN_LENGTH];// Profile��ʶ
	char URI[URI_LENGTH];//rtsp�㲥����ַ
	int  width;
	int  height;
}StreamInfo;

typedef struct
{
	char ProfileToken[TOKEN_LENGTH];// Profile��ʶ
	char PTZNodeToken[TOKEN_LENGTH];// PTZNode��ʶ
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
	* ������Ϣ
	*
	* @ParamIn	bOpen 0-�رգ�Ĭ�ϣ� 1��
	* @Return ������
**/
LIBONVIF2_API int libOnvif2Debug(int bOpen);

/**
	* ���ó�ʱʱ�� �ڵ��ò�ͬ��APIǰ�����������
	*
	* @ParamIn	ts ���ͳ�ʱʱ��(when > 0, gives socket send timeout in seconds, < 0 in usec )
	* @ParamIn	tr ���ճ�ʱʱ��(when > 0, gives socket recv timeout in seconds, < 0 in usec )
	* @ParamIn	tc ���ӳ�ʱʱ��(when > 0, gives socket connect timeout in seconds, < 0 in usec )
	* @Return ������
**/
LIBONVIF2_API int libOnvif2SetTimeOut(int ts, int tr, int tc);

/**
	* ͨ��WSD�������е�onvif NVT�豸
	*
	* @ParamOut	
	* @Return ������
**/
LIBONVIF2_API int libOnvif2ProbeAllNVT(LIST_NVT * Devices);


/**
	* �����豸���ƾ��
	*
	* @ParamIn IP �豸IP��ַ
	* @ParamIn forwhat �豸��ʼ������
	* @Return �ɹ�-��� ʧ��-������
**/
LIBONVIF2_API long libOnvif2ControlCreate( const char * IP);

/**
	* ɾ���豸���ƾ��
	*
	* @ParamIn hSession �豸���ƾ��
	* @Return ������
**/
LIBONVIF2_API int libOnvif2ControlDelete(long hSession);

/**
	* ɾ�������豸���ƾ��
	*
	* @Return ������
**/
LIBONVIF2_API int libOnvif2ControlCleanup();

/**
    * �豸��½
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn UserName �û���
    * @ParamIn Pass ����
    * @Return ������
**/
LIBONVIF2_API int libOnvif2DeviceLogin(long hSession,const char* UserName,const char* Passwd);

/**
    * ����豸profile����
    *
    * @ParamIn hSession �豸���ƾ��
    * @Return ����
**/
LIBONVIF2_API int libOnvif2GetProfileSize(long hSession);


/**
    * ���豸��½�Ժ���øú����������豸�ļ������������ȡ��ѵ�Profile����
    *
    * @ParamIn hSession �豸���ƾ��
    * @Return ��ѵ�Profile����
**/
LIBONVIF2_API int libOnvif2ReportAfterLogin(long hSession);

/**
    * ��ȡʱ��
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamOut pszDateTime ���ʱ�� YYYY-MM-DD hh-mm-ss
    * @Return ������
**/
LIBONVIF2_API int libOnvif2GetDateTime(long hSession, char* pszDateTime);



/**
    * �豸�ǳ�
    *
    * @ParamIn hSession �豸���ƾ��
    * @Return ������
**/
LIBONVIF2_API int libOnvif2DeviceLogout(long hSession);

/**
    * ��ȡ�豸�ĳ����ͺ�
    *
    * @ParamIn hSession �豸���ƾ��
    * @Return ���̴���
**/
LIBONVIF2_API MANUFACTURER_TYPE libOnvif2GetManufacturer(long hSession);


/**
    * ����豸��Ƶ����Ϣ
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn index Profile��������Ĭ��ֵ0
    * @ParamOut Info
    * @Return ������
**/
LIBONVIF2_API int libOnvif2GetStreamInfo(long hSession, int index, StreamInfo * Info);


/**
    * ����豸��PTZ��Ϣ
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn index Profile��������Ĭ��ֵ0
    * @ParamOut Info
    * @Return ������
**/
LIBONVIF2_API int libOnvif2GetPTZInfo(long hSession, int index, PTZInfo * Info);


/**
    * ��̨���ƿ�ʼ
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn ProfileToken ��libOnvif2GetPTZInfo���ص�PTZInfo->ProfileToken��
    * @ParamIn Action ��������
    * @ParamIn Velocity �ٶ� 0-100
    * @Return ������
**/
LIBONVIF2_API int libOnvif2PTZStartMove(long hSession,const char * ProfileToken,
										PTZ_ACTION Action,int Velocity);


/**
    * ��̨����ֹͣ
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn ProfileToken ��libOnvif2GetPTZInfo���ص�PTZInfo->ProfileToken��
    * @Return ������
**/
LIBONVIF2_API int libOnvif2PTZStopMove(long hSession,const char * ProfileToken);

/**
    * ��ȡ��̨����λ��
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn ProfileToken ��libOnvif2GetPTZInfo���ص�PTZInfo->ProfileToken��
    * @ParamOut Position ���ص�����ֵ
    * @Return ������ IDLE = 0, MOVING = 1, UNKNOWN = 2
**/
LIBONVIF2_API int libOnvif2PTZGetAbsolutePosition(long hSession,const char * ProfileToken,
											  PTZAbsPosition *Position);

/**
    * ͨ������λ�õ���PTZ
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn ProfileToken ��libOnvif2GetPTZInfo���ص�PTZInfo->ProfileToken��
    * @ParamIn Position ����ֵ
    * @ParamIn Velocity ��������ƽ��ʱ���ٶ� 0-100
    * @ParamIn ZoomVelocity �Ŵ���С���ٶ� 0-100
    * @Return ������
**/
LIBONVIF2_API int libOnvif2PTZAbsoluteMove(long hSession,const char * ProfileToken,
									   const PTZAbsPosition *Position,int PanTiltVelocity, int ZoomVelocity);

/**
    * ���Ԥ�õ�
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn ProfileToken ��libOnvif2GetPTZInfo���ص�PTZInfo->ProfileToken��
    * @ParamOut presets Ԥ�õ�
    * @Return ������
**/
LIBONVIF2_API int libOnvif2GetPresets(long hSession,const char * ProfileToken,
									  LIST_Preset* presets);

/**
    * ����Ԥ�õ�
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn ProfileToken ��libOnvif2GetPTZInfo���ص�PTZInfo->ProfileToken��
    * @ParamIn PresetNo Ԥ�õ���
    * @Return ������
**/
LIBONVIF2_API int libOnvif2CreatePreset(long hSession,const char * ProfileToken,
										unsigned int PresetNo);

/**
    * ɾ��Ԥ�õ�
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn ProfileToken ��libOnvif2GetPTZInfo���ص�PTZInfo->ProfileToken��
    * @ParamIn PresetNo Ԥ�õ���
    * @Return ������
**/
LIBONVIF2_API int libOnvif2DelPreset(long hSession,const char * ProfileToken,
									unsigned int PresetNo);

/**
    * �ƶ���Ԥ�õ�
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn ProfileToken ��libOnvif2GetPTZInfo���ص�PTZInfo->ProfileToken��
    * @ParamIn PresetNo Ԥ�õ���
    * @Return ������
**/
LIBONVIF2_API int libOnvif2GotoPreset(long hSession,const char * ProfileToken,
									   unsigned int PresetNo);


/**
    * �Խ���ʼ
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn ProfileToken ��libOnvif2GetPTZInfo���ص�PTZInfo->ProfileToken��
    * @ParamIn Action ��������(0,�������1������Զ)
    * @ParamIn Velocity �Ŵ���С���ٶ� 0-100
    * @Return ������
**/
LIBONVIF2_API int libOnvif2FocusMove(long hSession,const char * ProfileToken,
									 FOCUS_ACTION Action,int Velocity);

/**
    * �Խ�ֹͣ
    *
    * @ParamIn hSession �豸���ƾ��
    * @ParamIn ProfileToken ��libOnvif2GetPTZInfo���ص�PTZInfo->ProfileToken��
    * @Return ������
**/
LIBONVIF2_API int libOnvif2FocusStop(long hSession,const char * ProfileToken);





#endif

