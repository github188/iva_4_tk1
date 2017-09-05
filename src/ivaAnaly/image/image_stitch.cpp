#include <stdio.h>
#include <stdlib.h>
#include "image_stitch.h"
#include <sys/stat.h>
#include "oal_time.h"
#include "oal_file.h"
#include "../AnalyCfgMan.h"

#define JPEG_COM_TIMES  20

bool IsSameColor(RGBColorT color1, RGBColorT color2 )
{
	return ((color1.r == color2.r) && (color1.g == color2.g) && (color1.b == color2.b));
}


static double SaveImageQuality(IplImage *img, const char *file_name, int img_quality)
{
	int quality[3];
	quality[0] = CV_IMWRITE_JPEG_QUALITY;
	quality[1] = img_quality;
	quality[2] = 0;

	cvSaveImage(file_name,img,quality);
	usleep(50*1000);

	long lBSize = OAL_FileSize(file_name);
	double dbSizeK = lBSize/1024.0;
	return dbSizeK;
}

int SaveImageWithLimit(IplImage *img,const char *file_name, int img_size, int img_quality, int max_times)
{
	int iTryTimes = 0;
	int iStep = img_quality/max_times;
	int use_quality = img_quality;

	while(iTryTimes < max_times && use_quality >= 0)
	{
		if(iTryTimes != 0)
		{
			remove(file_name);
			usleep(50*1000);
		}

		double file_size = SaveImageQuality(img,file_name,use_quality);

		if(file_size < (double)img_size || img_size == 0)
		{
			printf("%d %s, quality=%d, %.1fkb\n",iTryTimes,file_name, use_quality,file_size);
			return 0;
		}

		iTryTimes++;
		use_quality -= iStep;
	}
	return 0;
}

static int Image_Zoom_And_Copy2_Dst(IplImage *srcImg, IplImage *dstImg, int dstX, int dstY,int dstW, int dstH )
{
	if(srcImg == NULL  || dstImg == NULL)
	{
		return -1;
	}

	if( dstX + dstW > dstImg->width || dstY + dstH > dstImg->height)
	{
		LOG_ERROR_FMT("dstX(%d) + dstW(%d) > dstImg->width(%d) || dstY(%d) + dstH(%d) > dstImg->height(%d)",dstX,dstW,dstImg->width,dstY,dstH,dstImg->height);
		return -1;
	}

	IplImage *zoomImg = NULL;

	// 缩放
	if(srcImg->width != dstW || srcImg->height != dstH)
	{
		zoomImg = cvCreateImage(cvSize(dstW, dstH), srcImg->depth, srcImg->nChannels);
		if (zoomImg)
		{
			cvZero(zoomImg);
			cvResize(srcImg, zoomImg, CV_INTER_AREA);// 缩放到目标图像
			srcImg = zoomImg;//使用缩放后的图像作为源
		}
	}

	// 拷贝
	cvSetImageROI(dstImg, cvRect(dstX, dstY, srcImg->width, srcImg->height));
	cvCopy(srcImg, dstImg);
	cvResetImageROI(dstImg);

	// 释放中间图像
	if(zoomImg)
	{
		cvReleaseImage(&zoomImg);
		zoomImg	= NULL;
	}

	return 0;
}

int Image_Stitch_None(AnalyData * ptData, OsdContent *pOsd)
{
	ImgOSDT tImgOsd = {0};
	AnalyCfgMan::GetInstance()->GetEventImgOSD(ptData->iEventType, &tImgOsd);

	ImgQualityT tImgQua = {0};
	AnalyCfgMan::GetInstance()->GetEventImgQuality(ptData->iEventType, &tImgQua);

	CvScalar txtColor = CV_RGB(tImgOsd.tFontColor.r, tImgOsd.tFontColor.g, tImgOsd.tFontColor.b);
	CvScalar bkColor  = CV_RGB(tImgOsd.tBackColor.r, tImgOsd.tBackColor.g, tImgOsd.tBackColor.b);
	int bkHeight = tImgOsd.iFontSize + 2 * OSD_LINE_GAP + (pOsd->iLineCnt - 1) * (tImgOsd.iFontSize + OSD_LINE_GAP);

	int iOutWidth = (tImgQua.iWidthLimit > 0 ? tImgQua.iWidthLimit : ptData->iPicWidth);
	int iOutHeight = (tImgQua.iHeightLimit > 0 ? tImgQua.iHeightLimit : ptData->iPicHeight);
	int iDepth = ptData->pIplImage[0]->depth;
	int iChans = ptData->pIplImage[0]->nChannels;

	switch(tImgOsd.iPosModel)
	{
	case eEachIn:
	case eComIn:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth,iOutHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}

			for (int i = 0; i < ptData->iPicNum; i++)
			{
				// 填充原始图像
				cvZero(dstImg);
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, 0, 0, iOutWidth, iOutHeight);

				// 填充背景
				int iStartX = iOutWidth * tImgOsd.iLeft / 100;
				int iStartY = iOutHeight * tImgOsd.iTop / 100;
				if (!IsSameColor(tImgOsd.tFontColor, tImgOsd.tBackColor))
				{
					OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth-iStartX, bkHeight, bkColor);
				}

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);

				// 保存文件
				char szPathFile[512] = {0};
				sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[i]);
				SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);
			}

			cvReleaseImage(&dstImg);
			dstImg = NULL;

			break;
		}
	case eEachDown:
	case eComDown:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth,iOutHeight+bkHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}

			for (int i = 0; i < ptData->iPicNum; i++)
			{
				// 填充原始图像
				cvZero(dstImg);
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, 0, 0, iOutWidth, iOutHeight);

				// 填充背景
				int iStartX = 0;
				int iStartY = iOutHeight;
				OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);

				// 保存文件
				char szPathFile[512] = {0};
				sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[i]);
				SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);
			}

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eEachUp:
	case eComUp:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth,iOutHeight+bkHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}

			for (int i = 0; i < ptData->iPicNum; i++)
			{
				// 填充原始图像
				cvZero(dstImg);
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, 0, bkHeight, iOutWidth, iOutHeight);

				// 填充背景
				int iStartX = 0;
				int iStartY = 0;
				OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);

				// 保存文件
				char szPathFile[512] = {0};
				sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[i]);
				SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);
			}

			cvReleaseImage(&dstImg);
			dstImg = NULL;

			break;
		}
	default:
		break;
	}

	return 0;
}

int Image_Stitch_Horizontal(AnalyData * ptData, OsdContent *pOsd)
{
	ImgOSDT tImgOsd = {0};
	AnalyCfgMan::GetInstance()->GetEventImgOSD(ptData->iEventType, &tImgOsd);

	ImgQualityT tImgQua = {0};
	AnalyCfgMan::GetInstance()->GetEventImgQuality(ptData->iEventType, &tImgQua);

	CvScalar txtColor = CV_RGB(tImgOsd.tFontColor.r, tImgOsd.tFontColor.g, tImgOsd.tFontColor.b);
	CvScalar bkColor  = CV_RGB(tImgOsd.tBackColor.r, tImgOsd.tBackColor.g, tImgOsd.tBackColor.b);
	int bkHeight = tImgOsd.iFontSize + 2 * OSD_LINE_GAP + (pOsd->iLineCnt - 1) * (tImgOsd.iFontSize + OSD_LINE_GAP);

	int iOutWidth = (tImgQua.iWidthLimit > 0 ? tImgQua.iWidthLimit : ptData->iPicWidth);
	int iOutHeight = (tImgQua.iHeightLimit > 0 ? tImgQua.iHeightLimit : ptData->iPicHeight);
	int iDepth = ptData->pIplImage[0]->depth;
	int iChans = ptData->pIplImage[0]->nChannels;

	switch(tImgOsd.iPosModel)
	{
	case eEachIn:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*ptData->iPicNum,iOutHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			for (int i = 0; i < ptData->iPicNum; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, 0, iOutWidth, iOutHeight);
				
				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;

				// 填充背景
				int iStartX = i*iOutWidth + iOutWidth * tImgOsd.iLeft / 100;
				int iStartY = iOutHeight * tImgOsd.iTop / 100;
				if (!IsSameColor(tImgOsd.tFontColor, tImgOsd.tBackColor))
				{
					OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth-iStartX, bkHeight, bkColor);
				}

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eEachDown:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*ptData->iPicNum,iOutHeight+bkHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			for (int i = 0; i < ptData->iPicNum; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, 0, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;

				// 填充背景
				int iStartX = i*iOutWidth;
				int iStartY = iOutHeight;
				OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eEachUp:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*ptData->iPicNum,iOutHeight+bkHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			for (int i = 0; i < ptData->iPicNum; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, bkHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;
				ptData->tPicPos[i].y += bkHeight;

				// 填充背景
				int iStartX = i*iOutWidth;
				int iStartY = 0;
				OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eComIn:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*ptData->iPicNum,iOutHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			// 填充原始图像
			for (int i = 0; i < ptData->iPicNum; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, 0, iOutWidth, iOutHeight);
			
				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;
			}

			// 填充背景
			int iStartX = ptData->iPicNum * iOutWidth * tImgOsd.iLeft / 100;
			int iStartY = iOutHeight * tImgOsd.iTop / 100;
			if (!IsSameColor(tImgOsd.tFontColor, tImgOsd.tBackColor))
			{
				OSD_FillBackground(dstImg, iStartX, iStartY, ptData->iPicNum * iOutWidth - iStartX, bkHeight, bkColor);
			}

			// 填充OSD
			iStartX = iStartX + OSD_LINE_GAP;
			iStartY = iStartY + OSD_LINE_GAP;
			OSD_Put2Img(dstImg, iStartX, iStartY, pOsd, txtColor, tImgOsd.iFontSize);

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eComDown:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*ptData->iPicNum,iOutHeight+bkHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			// 填充原始图像
			for (int i = 0; i < ptData->iPicNum; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, 0, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;
			}

			// 填充背景
			int iStartX = 0;
			int iStartY = iOutHeight;
			OSD_FillBackground(dstImg, iStartX, iStartY, ptData->iPicNum*iOutWidth, bkHeight, bkColor);

			// 填充OSD
			iStartX = iStartX + OSD_LINE_GAP;
			iStartY = iStartY + OSD_LINE_GAP;
			OSD_Put2Img(dstImg, iStartX, iStartY, pOsd, txtColor, tImgOsd.iFontSize);

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eComUp:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*ptData->iPicNum,iOutHeight+bkHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			// 填充原始图像
			for (int i = 0; i < ptData->iPicNum; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, bkHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;
				ptData->tPicPos[i].y += bkHeight;
			}

			// 填充背景
			int iStartX = 0;
			int iStartY = 0;
			OSD_FillBackground(dstImg, iStartX, iStartY, ptData->iPicNum*iOutWidth, bkHeight, bkColor);

			// 填充OSD
			iStartX = iStartX + OSD_LINE_GAP;
			iStartY = iStartY + OSD_LINE_GAP;
			OSD_Put2Img(dstImg, iStartX, iStartY, pOsd, txtColor, tImgOsd.iFontSize);

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	default:
		break;
	}

	return 0;
}

int Image_Stitch_Vertical(AnalyData * ptData, OsdContent *pOsd)
{
	ImgOSDT tImgOsd = {0};
	AnalyCfgMan::GetInstance()->GetEventImgOSD(ptData->iEventType, &tImgOsd);

	ImgQualityT tImgQua = {0};
	AnalyCfgMan::GetInstance()->GetEventImgQuality(ptData->iEventType, &tImgQua);

	CvScalar txtColor = CV_RGB(tImgOsd.tFontColor.r, tImgOsd.tFontColor.g, tImgOsd.tFontColor.b);
	CvScalar bkColor  = CV_RGB(tImgOsd.tBackColor.r, tImgOsd.tBackColor.g, tImgOsd.tBackColor.b);
	int bkHeight = tImgOsd.iFontSize + 2 * OSD_LINE_GAP + (pOsd->iLineCnt - 1) * (tImgOsd.iFontSize + OSD_LINE_GAP);

	int iOutWidth = (tImgQua.iWidthLimit > 0 ? tImgQua.iWidthLimit : ptData->iPicWidth);
	int iOutHeight = (tImgQua.iHeightLimit > 0 ? tImgQua.iHeightLimit : ptData->iPicHeight);
	int iDepth = ptData->pIplImage[0]->depth;
	int iChans = ptData->pIplImage[0]->nChannels;

	switch(tImgOsd.iPosModel)
	{
	case eEachIn:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth,iOutHeight*ptData->iPicNum), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			for (int i = 0; i < ptData->iPicNum; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, 0, i*iOutHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].y += i*iOutHeight;
				
				// 填充背景
				int iStartX = iOutWidth * tImgOsd.iLeft / 100;
				int iStartY = i*iOutHeight +iOutHeight * tImgOsd.iTop / 100;
				if (!IsSameColor(tImgOsd.tFontColor, tImgOsd.tBackColor))
				{
					OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth-iStartX, bkHeight, bkColor);
				}

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eEachDown:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth,(iOutHeight+bkHeight)*ptData->iPicNum), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			for (int i = 0; i < ptData->iPicNum; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, 0, i*(iOutHeight +bkHeight), iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].y += i*(iOutHeight +bkHeight);

				// 填充背景
				int iStartX = 0;
				int iStartY = i*(iOutHeight +bkHeight)+iOutHeight;
				OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eEachUp:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth,(iOutHeight+bkHeight)*ptData->iPicNum), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			for (int i = 0; i < ptData->iPicNum; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, 0, i*(iOutHeight +bkHeight)+bkHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].y += i*(iOutHeight +bkHeight)+bkHeight;

				// 填充背景
				int iStartX = 0;
				int iStartY = i*(iOutHeight +bkHeight);
				OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eComIn:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth,iOutHeight*ptData->iPicNum), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			// 填充原始图像
			for (int i = 0; i < ptData->iPicNum; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, 0, i*iOutHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].y += i*(iOutHeight);
			}

			// 填充背景
			int iStartX = iOutWidth * tImgOsd.iLeft / 100;
			int iStartY = ptData->iPicNum * iOutHeight * tImgOsd.iTop / 100;
			if (!IsSameColor(tImgOsd.tFontColor, tImgOsd.tBackColor))
			{
				OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth-iStartX, bkHeight, bkColor);
			}

			// 填充OSD
			iStartX = iStartX + OSD_LINE_GAP;
			iStartY = iStartY + OSD_LINE_GAP;
			OSD_Put2Img(dstImg, iStartX, iStartY, pOsd, txtColor, tImgOsd.iFontSize);

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eComDown:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth,iOutHeight*ptData->iPicNum + bkHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			// 填充原始图像
			for (int i = 0; i < ptData->iPicNum; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, 0, i*iOutHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].y += i*(iOutHeight);
			}

			// 填充背景
			int iStartX = 0;
			int iStartY = ptData->iPicNum*iOutHeight;
			OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

			// 填充OSD
			iStartX = iStartX + OSD_LINE_GAP;
			iStartY = iStartY + OSD_LINE_GAP;
			OSD_Put2Img(dstImg, iStartX, iStartY, pOsd, txtColor, tImgOsd.iFontSize);

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eComUp:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth,iOutHeight*ptData->iPicNum + bkHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			// 填充原始图像
			for (int i = 0; i < ptData->iPicNum; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, 0, bkHeight+i*iOutHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].y += bkHeight+i*(iOutHeight);
			}

			// 填充背景
			int iStartX = 0;
			int iStartY = 0;
			OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

			// 填充OSD
			iStartX = iStartX + OSD_LINE_GAP;
			iStartY = iStartY + OSD_LINE_GAP;
			OSD_Put2Img(dstImg, iStartX, iStartY, pOsd, txtColor, tImgOsd.iFontSize);

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	default:
		break;
	}

	return 0;
}

int Image_Stitch_Matts(AnalyData * ptData, OsdContent *pOsd)
{
	ImgOSDT tImgOsd = {0};
	AnalyCfgMan::GetInstance()->GetEventImgOSD(ptData->iEventType, &tImgOsd);

	ImgQualityT tImgQua = {0};
	AnalyCfgMan::GetInstance()->GetEventImgQuality(ptData->iEventType, &tImgQua);

	CvScalar txtColor = CV_RGB(tImgOsd.tFontColor.r, tImgOsd.tFontColor.g, tImgOsd.tFontColor.b);
	CvScalar bkColor  = CV_RGB(tImgOsd.tBackColor.r, tImgOsd.tBackColor.g, tImgOsd.tBackColor.b);

	int bkHeight = tImgOsd.iFontSize + 2 * OSD_LINE_GAP + (pOsd->iLineCnt - 1) * (tImgOsd.iFontSize + OSD_LINE_GAP);

	int iOutWidth = (tImgQua.iWidthLimit > 0 ? tImgQua.iWidthLimit : ptData->iPicWidth);
	int iOutHeight = (tImgQua.iHeightLimit > 0 ? tImgQua.iHeightLimit : ptData->iPicHeight);
	int iDepth = ptData->pIplImage[0]->depth;
	int iChans = ptData->pIplImage[0]->nChannels;

	switch(tImgOsd.iPosModel)
	{
	case eEachIn:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*2,iOutHeight*2), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			for (int i = 0; i < ptData->iPicNum && i < 2; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, 0, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;

				// 填充背景
				int iStartX = i*iOutWidth + iOutWidth * tImgOsd.iLeft / 100;
				int iStartY = iOutHeight * tImgOsd.iTop / 100;
				if (!IsSameColor(tImgOsd.tFontColor, tImgOsd.tBackColor))
				{
					OSD_FillBackground(dstImg, iStartX, iStartY, i*iOutWidth+iOutWidth-iStartX, bkHeight, bkColor);
				}

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			for (int i = 2; i < ptData->iPicNum && i < 4; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, (i-2)*iOutWidth, iOutHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += (i-2)*iOutWidth;
				ptData->tPicPos[i].y += iOutHeight;

				// 填充背景
				int iStartX = (i-2)*iOutWidth + iOutWidth * tImgOsd.iLeft / 100;
				int iStartY = iOutHeight + iOutHeight * tImgOsd.iTop / 100;
				if (!IsSameColor(tImgOsd.tFontColor, tImgOsd.tBackColor))
				{
					OSD_FillBackground(dstImg, iStartX, iStartY, (i-2)*iOutWidth+iOutWidth-iStartX, bkHeight, bkColor);
				}

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eEachDown:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*2,(iOutHeight+bkHeight)*2), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			for (int i = 0; i < ptData->iPicNum && i < 2; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, 0, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;

				// 填充背景
				int iStartX = i*iOutWidth;
				int iStartY = iOutHeight;
				OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			for (int i = 2; i < ptData->iPicNum && i < 4; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, (i-2)*iOutWidth, iOutHeight+bkHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += (i-2)*iOutWidth;
				ptData->tPicPos[i].y += iOutHeight+bkHeight;

				// 填充背景
				int iStartX = (i-2)*iOutWidth;
				int iStartY = iOutHeight + bkHeight + iOutHeight;
				OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eEachUp:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*2,(iOutHeight+bkHeight)*2), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			for (int i = 0; i < ptData->iPicNum && i < 2; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, bkHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;
				ptData->tPicPos[i].y += bkHeight;

				// 填充背景
				int iStartX = i*iOutWidth;
				int iStartY = 0;
				OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			for (int i = 2; i < ptData->iPicNum && i < 4; i++)
			{
				// 填充原始图像
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, (i-2)*iOutWidth, iOutHeight+bkHeight+bkHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += (i-2)*iOutWidth;
				ptData->tPicPos[i].y += iOutHeight+bkHeight;

				// 填充背景
				int iStartX = (i-2)*iOutWidth;
				int iStartY = iOutHeight + bkHeight;
				OSD_FillBackground(dstImg, iStartX, iStartY, iOutWidth, bkHeight, bkColor);

				// 填充OSD
				iStartX = iStartX + OSD_LINE_GAP;
				iStartY = iStartY + OSD_LINE_GAP;
				OSD_Put2Img(dstImg, iStartX, iStartY, pOsd+i, txtColor, tImgOsd.iFontSize);
			}

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eComIn:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*2,iOutHeight*2), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			// 填充原始图像
			for (int i = 0; i < ptData->iPicNum && i < 2; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, 0, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;
			}

			for (int i = 2; i < ptData->iPicNum && i < 4; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, (i-2)*iOutWidth, iOutHeight, iOutWidth, iOutHeight);
				
				// 合并后调整坐标
				ptData->tPicPos[i].x += (i-2)*iOutWidth;
				ptData->tPicPos[i].y += iOutHeight;
			}

			// 填充背景
			int iStartX = 2*iOutWidth * tImgOsd.iLeft / 100;
			int iStartY = 2*iOutHeight * tImgOsd.iTop / 100;
			if (!IsSameColor(tImgOsd.tFontColor, tImgOsd.tBackColor))
			{
				OSD_FillBackground(dstImg, iStartX, iStartY, 2*iOutWidth-iStartX, bkHeight, bkColor);
			}

			// 填充OSD
			iStartX = iStartX + OSD_LINE_GAP;
			iStartY = iStartY + OSD_LINE_GAP;
			OSD_Put2Img(dstImg, iStartX, iStartY, pOsd, txtColor, tImgOsd.iFontSize);

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eComDown:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*2,iOutHeight*2+bkHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			// 填充原始图像
			for (int i = 0; i < ptData->iPicNum && i < 2; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, 0, iOutWidth, iOutHeight);
				
				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;
			}

			for (int i = 2; i < ptData->iPicNum && i < 4; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, (i-2)*iOutWidth, iOutHeight+bkHeight, iOutWidth, iOutHeight);
				
				// 合并后调整坐标
				ptData->tPicPos[i].x += (i-2)*iOutWidth;
				ptData->tPicPos[i].y += iOutHeight+bkHeight;
			}

			// 填充背景
			int iStartX = 0;
			int iStartY = 2*iOutHeight;
			OSD_FillBackground(dstImg, iStartX, iStartY, 2*iOutWidth, bkHeight, bkColor);

			// 填充OSD
			iStartX = iStartX + OSD_LINE_GAP;
			iStartY = iStartY + OSD_LINE_GAP;
			OSD_Put2Img(dstImg, iStartX, iStartY, pOsd, txtColor, tImgOsd.iFontSize);

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	case eComUp:
		{
			IplImage *dstImg = cvCreateImage(cvSize(iOutWidth*2,iOutHeight*2+bkHeight), iDepth, iChans);
			if (dstImg == NULL)
			{
				LOG_ERROR_FMT("cvCreateImage failed");
				return -1;
			}
			cvZero(dstImg);

			// 填充原始图像
			for (int i = 0; i < ptData->iPicNum && i < 2; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, i*iOutWidth, bkHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += i*iOutWidth;
				ptData->tPicPos[i].y += bkHeight;
			}

			for (int i = 2; i < ptData->iPicNum && i < 4; i++)
			{
				Image_Zoom_And_Copy2_Dst(ptData->pIplImage[i], dstImg, (i-2)*iOutWidth, iOutHeight+bkHeight, iOutWidth, iOutHeight);

				// 合并后调整坐标
				ptData->tPicPos[i].x += (i-2)*iOutWidth;
				ptData->tPicPos[i].y += iOutHeight+bkHeight;
			}

			// 填充背景
			int iStartX = 0;
			int iStartY = 0;
			OSD_FillBackground(dstImg, iStartX, iStartY, 2*iOutWidth, bkHeight, bkColor);

			// 填充OSD
			iStartX = iStartX + OSD_LINE_GAP;
			iStartY = iStartY + OSD_LINE_GAP;
			OSD_Put2Img(dstImg, iStartX, iStartY, pOsd, txtColor, tImgOsd.iFontSize);

			// 保存文件
			char szPathFile[512] = {0};
			sprintf(szPathFile, "%s%s", ptData->szPicRootPath, ptData->szPicFile[0]);
			SaveImageWithLimit(dstImg, szPathFile, tImgQua.iSizeLimit, 90, JPEG_COM_TIMES);

			cvReleaseImage(&dstImg);
			dstImg = NULL;
			break;
		}
	default:
		break;
	}

	return 0;
}
