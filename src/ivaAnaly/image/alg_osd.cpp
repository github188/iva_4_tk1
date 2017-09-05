#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "alg_osd.h"
#include "AlgorithmDll.h"
#include <openssl/md5.h>  
#ifndef WIN32
#include <uuid/uuid.h>
#endif
#include "oal_time.h"
#include "oal_unicode.h"
#include "../AnalyCfgMan.h"
#include "opencv2/core/core_c.h"


void MD5EncodeString(const char * uuid,char * wm)
{
	unsigned char md5[16]={0};  
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx, uuid, strlen(uuid));  
	MD5_Final(md5,&ctx);

	for(int i=4;i<12;i++)
	{
		//if((i*md5[i]+i)%3==0)
		//{
			sprintf(wm+strlen(wm),"%02x",md5[i]);
		//}
		//else
		//{
		//	sprintf(wm+strlen(wm),"%02X",md5[i]);
		//}
	}
	wm[16] = '\0';
}

void GenWaterMark(char * wm)
{
	char szuuid[128] = {0};
#ifdef WIN32
	sprintf(szuuid, "111111111111");  
#else
	uuid_t uu;
	uuid_generate(uu);
	uuid_unparse(uu, szuuid);
#endif
	MD5EncodeString(szuuid, wm);
}

void AddOsdContent(OsdContent* ptOSD,char *pContent,int iWidth)
{
	if (NULL == ptOSD || NULL == pContent)
	{
		return;
	}

	char *pContentInRow = NULL;

	if(ptOSD->iLineCnt < 1 || ptOSD->iLineCnt > MAX_OSD_LINE_NUM)
	{
		memset(ptOSD,0,sizeof(OsdContent));
		pContentInRow = ptOSD->szContent[0];
		ptOSD->iLineCnt = 1;
	}
	else
	{
		pContentInRow = ptOSD->szContent[ptOSD->iLineCnt-1];

		int iOldWidth = GetOsdTextWidth(pContentInRow);
		int iNewWidth = GetOsdTextWidth(pContent);
		int iTolWidth = iWidth;
		//printf("iOldWidth %d + iNewWidth %d > iTolWidth %d\n",iOldWidth, iNewWidth, iTolWidth);
		if (iOldWidth + iNewWidth > iTolWidth && iOldWidth != 0)// iOldWidth != 0 ���һ������Ҳװ���£��Ǿ�ֻ��װ������ ������
		{
			// ����
			if (ptOSD->iLineCnt >= MAX_OSD_LINE_NUM)
			{
				printf("No more Osd Line\n");
				return;
			}
			else
			{
				ptOSD->iLineCnt++;
				pContentInRow = ptOSD->szContent[ptOSD->iLineCnt-1];
			}
		}
	}

	// ���
	if (strlen(pContentInRow) == 0)
	{
		sprintf(pContentInRow,"%s",pContent);
	}
	else
	{
		sprintf(pContentInRow + strlen(pContentInRow)," %s",pContent);
	}
}


int CreateImgOsdString( int iChnID, int iScene, int iEvent,
					   time_t tTime,int iMsec,const char * pszPlate, int iPlateColor, 
					   int iContentFlag, int iWidthLimit, OsdContent* ptOSD )
{
	if(ptOSD == NULL)
	{
		return -1;
	}

	OsdContent tGb2312Osd = {0};
	char content[MAX_OSD_CHAR_NUM] = {0};

	// �������������
	if((iContentFlag & OSD_TIME) == OSD_TIME)
	{
		char szTime[40] = {0};
		TimeFormatString(tTime, szTime, sizeof(szTime),eYMDHMS1);

		memset(content,0,sizeof(content));
		if ((iContentFlag & OSD_MESC) == OSD_MESC)
			sprintf(content,"ʱ��:%s.%03d",szTime, iMsec);
		else
			sprintf(content,"ʱ��:%s",szTime);

		AddOsdContent(&tGb2312Osd, content, iWidthLimit);
	}

	// �豸���
	if((iContentFlag & OSD_DEVID) == OSD_DEVID)
	{
		DeviceInfoT tDevInfo = {0};
		AnalyCfgMan::GetInstance()->GetDevInfo(&tDevInfo);

		memset(content,0,sizeof(content));
		sprintf(content,"�豸���:%s",tDevInfo.szDevID);

		AddOsdContent(&tGb2312Osd, content, iWidthLimit);
	}

	// Υ���ص�
	if((iContentFlag & OSD_PLACE) == OSD_PLACE)
	{
		PlaceInfoT tPlaceInfo = {0};
		AnalyCfgMan::GetInstance()->GetChnPlaceInfoT(iChnID, &tPlaceInfo);

		memset(content,0,sizeof(content));
		sprintf(content,"�ص�:%s",tPlaceInfo.szPlaceName);

		AddOsdContent(&tGb2312Osd, content, iWidthLimit);
	}

	// ����
	if((iContentFlag & OSD_DIRECTION) == OSD_DIRECTION)
	{
		SceneInfoT tScene = {0};
		AnalyCfgMan::GetInstance()->GetChnSceneInfo(iChnID, iScene, &tScene);

		memset(content,0,sizeof(content));
		if(tScene.iDirection == eE2W)
			sprintf(content,"����:������");
		else if(tScene.iDirection == eW2E)
			sprintf(content,"����:����");
		else if(tScene.iDirection == eS2N)
			sprintf(content,"����:����");
		else if(tScene.iDirection == eN2S)
			sprintf(content,"����:������");
		else if(tScene.iDirection == eD2U)
			sprintf(content,"����:����");
		else if(tScene.iDirection == eU2D)
			sprintf(content,"����:����");
		else if(tScene.iDirection == eBOTH)
			sprintf(content,"����:˫��");
		else
			sprintf(content,"����:����");

		AddOsdContent(&tGb2312Osd, content, iWidthLimit);
	}

	// Υ��
	if((iContentFlag & OSD_EVENT) == OSD_EVENT)
	{
		EventCodeT tCode = {0};
		AnalyCfgMan::GetInstance()->GetEventCode(iEvent,&tCode);

		memset(content,0,sizeof(content));
		sprintf(content,"��Ϊ:%s", tCode.szDesc);

		AddOsdContent(&tGb2312Osd, content, iWidthLimit);
	}

	// Υ������
	if((iContentFlag & OSD_CODE) == OSD_CODE)
	{
		EventCodeT tCode = {0};
		AnalyCfgMan::GetInstance()->GetEventCode(iEvent,&tCode);

		memset(content,0,sizeof(content));
		sprintf(content,"����:%s", tCode.szCode);

		AddOsdContent(&tGb2312Osd, content, iWidthLimit);
	}

	// ���ƺ���
	if((iContentFlag & OSD_PLATE) == OSD_PLATE)
	{
		memset(content,0,sizeof(content));
		if(pszPlate == NULL || strlen(pszPlate) < 1)
			sprintf(content,"���ƺ���:�޳���");
		else
			sprintf(content,"���ƺ���:%s", pszPlate);

		AddOsdContent(&tGb2312Osd, content, iWidthLimit);
	}

	// ������ɫ	
	if((iContentFlag & OSD_PLATE_COLOR) == OSD_PLATE_COLOR)
	{
		memset(content,0,sizeof(content));
		if(iPlateColor == 0)
			sprintf(content,"������ɫ:��ɫ");
		else if(iPlateColor == 1)
			sprintf(content,"������ɫ:��ɫ");
		else if(iPlateColor == 2)
			sprintf(content,"������ɫ:��ɫ");
		else if(iPlateColor == 3)
			sprintf(content,"������ɫ:��ɫ");
		else if(iPlateColor >= 4 && iPlateColor <= 10)
			sprintf(content,"������ɫ:����Դ");
		else
			sprintf(content,"������ɫ:����");

		AddOsdContent(&tGb2312Osd, content, iWidthLimit);
	}
	
	if((iContentFlag & OSD_WATERMARK) == OSD_WATERMARK)
	{
		char strWaterMark[18] ={0};
		GenWaterMark(strWaterMark);

		memset(content,0,sizeof(content));
		sprintf(content,"��α��:%s",strWaterMark);

		AddOsdContent(&tGb2312Osd, content, iWidthLimit);
	}

	ptOSD->iLineCnt = tGb2312Osd.iLineCnt;
	for(int i = 0; i < tGb2312Osd.iLineCnt; i++)
	{
		GB2312_To_UTF8(tGb2312Osd.szContent[i], strlen(tGb2312Osd.szContent[i]),
			ptOSD->szContent[i], MAX_OSD_CHAR_NUM);
	}

	return 0;
}

void OSD_Put2Img(IplImage* pImage, int x, int y, OsdContent * ptOsd, CvScalar txtColor, int txtHeight)
{
	int x0 = x;
	int y0 = y;
	for(int i = 0; i <  ptOsd->iLineCnt; i++)
	{
		PutOsdText(pImage, ptOsd->szContent[i], x0, y0 + txtHeight, txtColor);
		y0 += (txtHeight + OSD_LINE_GAP);
	}
}

void OSD_FillBackground(IplImage* pImage, int x, int y, int w,int h, CvScalar bkColor)
{
	// ��䱳��
	CvPoint *pts[1];
	CvPoint bkPts[4] = {{x,y},{x+w,y},{x+w,y+h},{x,y+h}};
	pts[0] = bkPts;
	int arr[1];  
	arr[0] = 4;
	cvFillPoly(pImage,pts,arr,1,bkColor);
}

