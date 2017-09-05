#ifndef __ALG_OSD_H__
#define __ALG_OSD_H__
#include "cvxText.h"  

#ifdef __cplusplus
extern "C" {
#endif

#define OSD_LINE_GAP		8		// 行距
#define MAX_OSD_CHAR_NUM 	(1024*2)
#define MAX_OSD_LINE_NUM	40		// 行数

typedef struct 
{
	int  iLineCnt;//行数
	char szContent[MAX_OSD_LINE_NUM][MAX_OSD_CHAR_NUM];
}OsdContent;

// 生成OSD字符串，返回UTF8编码
int CreateImgOsdString( int iChnID, int iScene, int iEvent, 
					   time_t tTime,int iMsec,const char * pszPlate, int iPlateColor, 
					   int iContentFlag, int iWidthLimit, OsdContent* ptOSD );

// 透明叠加OSD
void OSD_Put2Img(IplImage* pImage, int x, int y,OsdContent * ptOsd,CvScalar txtColor, int txtHeight);

// 填充背景
void OSD_FillBackground(IplImage* pImage, int x, int y,int w,int h, CvScalar bkColor);

void GenWaterMark(char * wm);


#ifdef __cplusplus
};
#endif

#endif



