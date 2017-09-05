
#ifndef __GLOBAL_TYPES_H__
#define __GLOBAL_TYPES_H__

#ifndef WIN32
typedef struct _RECT 
{ 
    int left; 
    int top; 
    int right; 
    int bottom; 
}RECT, *LPRECT;

typedef struct tagPOINT
{
    int  x;
    int  y;
}POINT, *LPPOINT;
#define __stdcall
#else
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#endif

/**************************************************************/
//�ص�YUV����
typedef struct tagDecAVInfo
{
    int iType;   // 0����Ƶ  1:��Ƶ 
    int iFormat;
    int iWidth;
    int iHeight;
    const unsigned char *pucData;
    int iDataLen;
}DecAVInfo, *PDecAVInfo;

typedef struct tagDecAVInfoEx
{
	DecAVInfo 	m_pInfo;

	//	����OtherInfoSize�жϺ���ľ�����Ϣ�ṹ��, 1:TimeStamp(unsigned int), 2:FrameRate(int)
	int			m_iOtherInfoSize;		
	void*		m_pvOtherInfo;
}DecAVInfoEx, *PDecAVInfoEx;

typedef void (*pfCBGetDecAV)(int _iID, const DecAVInfo* _pDecAVInfo, int _iUser);
/****************************************************************/

#define VCA_MAX_POLYGON_POINT_NUM		16				//	ÿ���������������������
#define VCA_MAX_POLYGON_POINT_NUMEX		32				//  Max Tripwire Points Number
#define VCA_MAX_POLYGON_NUM				8				//	ÿ��VCA��������������Ķ���θ���
#define MAX_AREA_NUM					4				//  Max Area Number for 300w Leave Detect

#endif
