/*!**********************************************************************
*\brief     �����¼������㷨���ò����ӿ�
*\details   ALG_IVR_Struct.h
*\author    jaya
*\version   v1.0.0
*\date      2015��/12��/15��
*\pre       
*\bug       
*\warning   
*\copyright jaya    
*/
/************************************************************************/
#ifndef _IVR_ALG_CONFIG_
#define _IVR_ALG_CONFIG_

#include "AlgorithmInput.h"

// ����õ��ĺ�
#define ALG_MAX_EVENT_COUNT         			 20	// һ֡ͼ������20���澯�¼�
#define ALG_MAX_SINGLE_EVENT_IMAGE_COUNT         4	// ���¼����ͼ�����
#define ALG_MAX_SNAP_CNT						 2	// Υͣ�����������
#define ALG_MAX_GRAPH_RESULTS				     20	//һ֡ͼ�������ʾ��������


#ifdef __cplusplus
	extern "C" {
#endif


typedef  enum 
{
	PLATE_COLOR_BLACK  = 0,// ��ɫ����
	PLATE_COLOR_BLUE   = 1,// ��ɫ����
	PLATE_COLOR_YELLOW = 2,// ��ɫ����
	PLATE_COLOR_WHITE  = 3,// ��ɫ����
	PLATE_COLOR_GREEN  = 4 // ��ɫ����
}EPlateColor;

/*!������ɫ�ṹ��*/
typedef   enum
{
	CCT_UNKNOWN=-1,        /*!<��ɫδ֪*/
	CCT_WHITE,             /*!<��ɫ*/
	CCT_BLACK,             /*!<��ɫ*/
	CCT_RED,               /*!<��ɫ*/
	CCT_YELLOW,            /*!<��ɫ*/
	CCT_BLUE,              /*!<��ɫ*/
	CCT_GREEN,             /*!<��ɫ*/
	CCT_GRAY,              /*!<��ɫ*/
	CCT_ARGENTINE,         /*!<��ɫ*/
	CCT_CAMBRIGE_BLUE,     /*!<������*/
	CCT_CHAMPAGNE,         /*!<����ɫ*/
}ECarColorType;


typedef struct
{
	EPlateColor    ePlateColor;		// ������ɫ
	ECarColorType  eCarColor;		// ������ɫ
	char 	       as8platenum[16]; // ���ƺ���
	ALG_Rect       tPlateRect;		// ����λ��
	float  	       platerealty;		// �������Ŷ�
}tLPRRes;

typedef  struct _tImageInfo
{
	EImageDataType   eImgDataType;	// ͼ������
	int              s32ImageWidth;	// ͼ��Ŀ�
	int              s32ImageHeight;// ͼ��ĸ�
	ALG_Rect         tObjRect;     	// Ŀ����ο� 
	struct timeval   d64TimeTick;  	// ͼ��ʱ��
	unsigned char    *pImageData;  	// ͼ������ͷ��ַ
}tImageInfo;

typedef  struct _tOneSnapInfo
{
	struct timeval  d64TimeBegin;	// ץ�Ŀ�ʼʱ��
	struct timeval  d64TimeEnd; 	// ץ�Ľ���ʱ��
}tOneSnapInfo;

typedef  struct _tSnapInfo
{
	int s32SnapCnt;				// ����ץ�Ĵ��� 1/2
	tOneSnapInfo tSnapInfos[ALG_MAX_SNAP_CNT];
}tSnapInfo;

typedef   struct _tAlgRes
{
	ALG_AlarmType eAlarmType;		// �¼�����
	int           s32EventID;		// �¼�ID���͸���Ŀ���Ŀ��ID��һ�µ�
	int           bNeedAbondon;		// ��Ҫ�����ü�¼
	int           s32ChannleIndex;	// ͨ�����
	int           s32PresetIndex;	// Ԥ��λ���
	tLPRRes       tLprRes;			// ����ʶ����
	int           s32LaneIndex;		// �������
	int           s32ImageCount;	// ͼƬ����
	tImageInfo    tImg[ALG_MAX_SINGLE_EVENT_IMAGE_COUNT];
	int			  bisEvent;			// ����Υ�����¼���¼  false Υ�� true �¼�
	tSnapInfo     tSnap;			// ץ����Ϣ
}tAlgRes;

/*!�㷨����б�*/
typedef  struct _tAlgResArray
{
	int       s32ResCount;					// �㷨�������
	tAlgRes   astResArray[ALG_MAX_EVENT_COUNT];	// һ֡ͼ������20���澯�¼�
	ALG_Rect  tRect;						// ��ǰ֡��Ŀ���ֻ�е�ץ�ĵ�ʱ�򣬲Żḳֵ
}tAlgResArray;


//�ؼ�����
typedef struct  _tAlgGraphResult
{
	tArea  tRes[ALG_MAX_GRAPH_RESULTS];
	int	   color[ALG_MAX_GRAPH_RESULTS];
	int    closed[ALG_MAX_GRAPH_RESULTS];
	int    ShowResNum;
}tAlgGraphResult;

#ifdef __cplusplus
	}
#endif

#endif	// _IVR_ALG_CONFIG_

