#ifndef __ALG_OSD_H__
#define __ALG_OSD_H__
#include "cvxText.h"  

#ifdef __cplusplus
extern "C" {
#endif

#define OSD_LINE_GAP		8		// �о�
#define MAX_OSD_CHAR_NUM 	(1024*2)
#define MAX_OSD_LINE_NUM	40		// ����

typedef struct 
{
	int  iLineCnt;//����
	char szContent[MAX_OSD_LINE_NUM][MAX_OSD_CHAR_NUM];
}OsdContent;

// ����OSD�ַ���������UTF8����
int CreateImgOsdString( int iChnID, int iScene, int iEvent, 
					   time_t tTime,int iMsec,const char * pszPlate, int iPlateColor, 
					   int iContentFlag, int iWidthLimit, OsdContent* ptOSD );

// ͸������OSD
void OSD_Put2Img(IplImage* pImage, int x, int y,OsdContent * ptOsd,CvScalar txtColor, int txtHeight);

// ��䱳��
void OSD_FillBackground(IplImage* pImage, int x, int y,int w,int h, CvScalar bkColor);

void GenWaterMark(char * wm);


#ifdef __cplusplus
};
#endif

#endif



