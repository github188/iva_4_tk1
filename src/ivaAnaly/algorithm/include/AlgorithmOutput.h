/*!**********************************************************************
*\brief     城市事件分析算法设置参数接口
*\details   ALG_IVR_Struct.h
*\author    jaya
*\version   v1.0.0
*\date      2015年/12月/15日
*\pre       
*\bug       
*\warning   
*\copyright jaya    
*/
/************************************************************************/
#ifndef _IVR_ALG_CONFIG_
#define _IVR_ALG_CONFIG_

#include "AlgorithmInput.h"

// 输出用到的宏
#define ALG_MAX_EVENT_COUNT         			 20	// 一帧图像的最多20个告警事件
#define ALG_MAX_SINGLE_EVENT_IMAGE_COUNT         4	// 单事件最大图像个数
#define ALG_MAX_SNAP_CNT						 2	// 违停最多拉近次数
#define ALG_MAX_GRAPH_RESULTS				     20	//一帧图像最多显示的折线数


#ifdef __cplusplus
	extern "C" {
#endif


typedef  enum 
{
	PLATE_COLOR_BLACK  = 0,// 黑色车牌
	PLATE_COLOR_BLUE   = 1,// 蓝色车牌
	PLATE_COLOR_YELLOW = 2,// 黄色车牌
	PLATE_COLOR_WHITE  = 3,// 白色车牌
	PLATE_COLOR_GREEN  = 4 // 绿色车牌
}EPlateColor;

/*!车身颜色结构体*/
typedef   enum
{
	CCT_UNKNOWN=-1,        /*!<颜色未知*/
	CCT_WHITE,             /*!<白色*/
	CCT_BLACK,             /*!<黑色*/
	CCT_RED,               /*!<红色*/
	CCT_YELLOW,            /*!<黄色*/
	CCT_BLUE,              /*!<蓝色*/
	CCT_GREEN,             /*!<绿色*/
	CCT_GRAY,              /*!<灰色*/
	CCT_ARGENTINE,         /*!<银色*/
	CCT_CAMBRIGE_BLUE,     /*!<剑桥蓝*/
	CCT_CHAMPAGNE,         /*!<香槟色*/
}ECarColorType;


typedef struct
{
	EPlateColor    ePlateColor;		// 车牌颜色
	ECarColorType  eCarColor;		// 车身颜色
	char 	       as8platenum[16]; // 车牌号码
	ALG_Rect       tPlateRect;		// 车牌位置
	float  	       platerealty;		// 车牌置信度
}tLPRRes;

typedef  struct _tImageInfo
{
	EImageDataType   eImgDataType;	// 图像类型
	int              s32ImageWidth;	// 图像的宽
	int              s32ImageHeight;// 图像的高
	ALG_Rect         tObjRect;     	// 目标矩形框 
	struct timeval   d64TimeTick;  	// 图像时间
	unsigned char    *pImageData;  	// 图像数据头地址
}tImageInfo;

typedef  struct _tOneSnapInfo
{
	struct timeval  d64TimeBegin;	// 抓拍开始时间
	struct timeval  d64TimeEnd; 	// 抓拍结束时间
}tOneSnapInfo;

typedef  struct _tSnapInfo
{
	int s32SnapCnt;				// 拉近抓拍次数 1/2
	tOneSnapInfo tSnapInfos[ALG_MAX_SNAP_CNT];
}tSnapInfo;

typedef   struct _tAlgRes
{
	ALG_AlarmType eAlarmType;		// 事件类型
	int           s32EventID;		// 事件ID，和跟踪目标的目标ID是一致的
	int           bNeedAbondon;		// 需要丢弃该记录
	int           s32ChannleIndex;	// 通道编号
	int           s32PresetIndex;	// 预置位编号
	tLPRRes       tLprRes;			// 车牌识别结果
	int           s32LaneIndex;		// 车道编号
	int           s32ImageCount;	// 图片数量
	tImageInfo    tImg[ALG_MAX_SINGLE_EVENT_IMAGE_COUNT];
	int			  bisEvent;			// 区分违法和事件记录  false 违法 true 事件
	tSnapInfo     tSnap;			// 抓拍信息
}tAlgRes;

/*!算法结果列表*/
typedef  struct _tAlgResArray
{
	int       s32ResCount;					// 算法结果个数
	tAlgRes   astResArray[ALG_MAX_EVENT_COUNT];	// 一帧图像的最多20个告警事件
	ALG_Rect  tRect;						// 当前帧的目标框，只有当抓拍的时候，才会赋值
}tAlgResArray;


//控件绘制
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

