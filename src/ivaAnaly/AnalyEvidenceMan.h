#ifndef __ANALY_EVIDENCE_H__
#define __ANALY_EVIDENCE_H__
#include "oal_queue.h"
#include "mq_analysis.h"
#include "capacity.h"
#include "AlgorithmOutput.h"
#include "opencv2/opencv.hpp"

#define MAX_FULL_RECORD_SECS (600)// 超过该时间不提供完成的取证录像，转为拼接


typedef struct _AnalyData
{
	int     iID;
	int 	iChnID;
	int 	iSceneID;
	time_t  tCreateTime;
	int     tCreateMsec;
	int 	iDateType;
	int 	iEventType;

	char 	szPlate[PLATE_LEN];
	int 	iPlateType;
	int 	iPlateColor;
	int 	iPlateConf;
	int     iPlateIndex;
	RectT 	tPlatePos;

	int 	iCarType;
	int 	iCarColor;
	int 	iCarBrand;
	int 	iCarSpeed;

	int 	iPicNum;
	time_t  tPicTime[MAX_IMG_NUM];
	int     tPicMsec[MAX_IMG_NUM];
	int     iPicType[MAX_IMG_NUM];
	RectT 	tPicPos[MAX_IMG_NUM];
	int 	iPicWidth;
	int 	iPicHeight;

	int 	iPicNumFinal;
	char 	szPicRootPath[256];
	char 	szPicFile[MAX_IMG_NUM][256];

	char 	szRecRootPath[256];
	char 	szRecFile[256];

	// 临时信息
	tSnapInfo tSnap;
	IplImage *pIplImage[MAX_IMG_NUM];
}AnalyData;


typedef struct _tagNameInfo
{
	time_t _time;
	int    _msec;
	char   _IP[16];
	char   _ID[64];
	char   _JK[256];
	char   _JKD[256];
	char   _PN[16];
	int    _PC;
	char   _WFL[1024];
	char   _WFD[16];
}AlgNameInfo;

#define FILE_JPG  0
#define FILE_MP4  1
#define FILE_AVI  2
#define FILE_FLV  2

AnalyData * NewEvidenceNode();
void FreeEvidenceNode(AnalyData * pData);

class AnalyEvidenceMan
{
public:
	static AnalyEvidenceMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
public:
	void Run();
	bool m_bExitThread;

	int SetFrameInfo(int iChnID, int iFrameRate, int iFrameWidth, int iFrameHeight);
	int GetFrameInfo(int iChnID, int &iFrameRate, int &iFrameWidth, int &iFrameHeight);

	int Push2ImgEvidenceNode(AnalyData * pData);
	AnalyData * PopImgEvidenceNode();

	int Push2RecEvidenceNode(AnalyData * pData);
	AnalyData * PopRecEvidenceNode();

	int Push2DataBase(AnalyData * pData);

	int CreateImgFileName(AnalyData * pData);
	int DoImageEvidence(AnalyData * pData);

	int CreateRecFileName(AnalyData * pData);
	int DoRecordEvidence(AnalyData * pData);

private:
	AnalyEvidenceMan();
	~AnalyEvidenceMan();
	static AnalyEvidenceMan* m_pInstance;
private:
	pthread_t m_h264_thread[MAX_CHANNEL_NUM];
	pthread_t m_img_thread[MAX_CHANNEL_NUM];
	pthread_t m_rec_thread[MAX_CHANNEL_NUM];

	int m_frame_rate[MAX_CHANNEL_NUM];
	int m_frame_width[MAX_CHANNEL_NUM];
	int m_frame_height[MAX_CHANNEL_NUM];

	TQueHndl m_hImgEviQue;
	TQueHndl m_hRecEviQue;
};

IplImage * ImageInfo2IplImage(const tImageInfo *ptImgInfo);

#endif