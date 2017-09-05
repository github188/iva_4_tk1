#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../QueryDb.h"
#include "oal_log.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "oal_time.h"


static int fill_xml_body(xmlDocPtr doc, const AnalyDbRecord *pData)
{
	int iRet = -1;
	if (doc == NULL)
	{
		return -1;
	}

	do 
	{
		xmlNodePtr root = xmlNewNode(NULL, (const xmlChar*)("root"));
		if (root == NULL) break;

		xmlDocSetRootElement(doc, root);
		{
			xmlNodePtr head = xmlNewNode(NULL, (const xmlChar*)("head"));
			if (head == NULL) break;

			xmlAddChild(root, head);
			{
				xmlNewChild(head, NULL, (const xmlChar*)("version"), (const xmlChar*)("1"));
			}

			xmlNodePtr body = xmlNewNode(NULL, (const xmlChar*)("body"));
			if (body == NULL) break;

			xmlAddChild(root, body);
			{
				xmlNewChild(body, NULL, (const xmlChar*)("sn"), (const xmlChar*)(pData->szDevID));
					
				xmlChar xcDataType[4];
				xmlStrPrintf(xcDataType, sizeof(xcDataType)/sizeof(xmlChar), "%d", pData->iDataType);
				xmlNewChild(body, NULL, (const xmlChar*)("datetype"), xcDataType);

				xmlNewChild(body, NULL, (const xmlChar*)("where"), (const xmlChar*)(pData->szPlaceName));
				char szCreateTime[40] = {0};
				TimeFormatString(pData->tCreate, szCreateTime, sizeof(szCreateTime), eYMDHMS2);
				xmlNewChild(body, NULL, (const xmlChar*)("when"), (const xmlChar*)(szCreateTime));

				xmlChar xcChanNo[4];
				xmlStrPrintf(xcChanNo, sizeof(xcChanNo)/sizeof(xmlChar), "%d", pData->iChnID);
				xmlNewChild(body, NULL, (const xmlChar*)("chnid"), xcChanNo);

				xmlChar xcSceneNo[4];
				xmlStrPrintf(xcSceneNo, sizeof(xcSceneNo)/sizeof(xmlChar), "%d", pData->iSceneID);
				xmlNewChild(body, NULL, (const xmlChar*)("sceneid"), xcSceneNo);

				xmlNewChild(body, NULL, (const xmlChar*)("illegalcode"), (const xmlChar*)(pData->szCode));
				xmlNewChild(body, NULL, (const xmlChar*)("illegaldesc"), (const xmlChar*)(pData->szDesc));

				//vehicle车辆识别信息节点
				xmlNodePtr vehicle = xmlNewNode(NULL, (const xmlChar*)("vehicle"));
				if (vehicle == NULL) break;

				xmlAddChild(body, vehicle);
				{
					xmlChar xcCarType[4];
					xmlStrPrintf(xcCarType, sizeof(xcCarType)/sizeof(xmlChar), "%02d", pData->iCarType);

					xmlNewChild(vehicle, NULL, (const xmlChar*)("cartype"),  xcCarType);
					xmlNewChild(vehicle, NULL, (const xmlChar*)("carcolor"), (const xmlChar*)("Z"));
					xmlNewChild(vehicle, NULL, (const xmlChar*)("carbrand"), (const xmlChar*)("0"));

					xmlChar xcPlateType[4];
					xmlStrPrintf(xcPlateType, sizeof(xcPlateType)/sizeof(xmlChar), "%02d", pData->iPlateType);
					xmlNewChild(vehicle, NULL, (const xmlChar*)("platetype"), xcPlateType);
					
					xmlChar xcPlateColor[4];
					xmlStrPrintf(xcPlateColor, sizeof(xcPlateColor)/sizeof(xmlChar), "%d", pData->iPlateColor);
					xmlNewChild(vehicle, NULL, (const xmlChar*)("platecolor"), xcPlateColor);

					xmlNewChild(vehicle, NULL, (const xmlChar*)("platenumber"), (const xmlChar*)(pData->szPlate));

					xmlNodePtr plateposition = xmlNewNode(NULL, (const xmlChar*)("plateposition"));
					if (plateposition == NULL) break;

					xmlAddChild(vehicle, plateposition);
					{
						xmlChar xcX[4];
						xmlStrPrintf(xcX, sizeof(xcX)/sizeof(xmlChar), "%d", pData->tPlatePos.x);
						xmlNewChild(plateposition, NULL, (const xmlChar*)("x"), xcX);

						xmlChar xcY[4];
						xmlStrPrintf(xcY, sizeof(xcY)/sizeof(xmlChar), "%d", pData->tPlatePos.y);
						xmlNewChild(plateposition, NULL, (const xmlChar*)("y"), xcY);

						xmlChar xcW[4];
						xmlStrPrintf(xcW, sizeof(xcW)/sizeof(xmlChar), "%d", pData->tPlatePos.w);
						xmlNewChild(plateposition, NULL, (const xmlChar*)("w"), xcW);

						xmlChar xcH[4];
						xmlStrPrintf(xcH, sizeof(xcH)/sizeof(xmlChar), "%d", pData->tPlatePos.h);
						xmlNewChild(plateposition, NULL, (const xmlChar*)("h"), xcH);
					}
				}

				// snap抓拍信息节点
				xmlNodePtr snap = xmlNewNode(NULL, (const xmlChar*)("snap"));
				if (snap == NULL) break;

				xmlChar xcSnapCnt[4];
				xmlStrPrintf(xcSnapCnt, sizeof(xcSnapCnt)/sizeof(xmlChar), "%d", pData->iSnapNum);
				xmlNewProp(snap, (const xmlChar*)("cnt"), xcSnapCnt);

				xmlAddChild(body, snap);
				{
					for (int i = 0; i <  pData->iSnapNum; i++)
					{
						xmlNodePtr snapinfo = xmlNewNode(NULL, (const xmlChar*)("snapinfo"));
						if (snapinfo == NULL) break;
						xmlAddChild(snap, snapinfo);
						{
							xmlNewChild(snapinfo, NULL, (const xmlChar*)("time"), (const xmlChar*)(pData->szSnapTime[i]));
							
							xmlChar xcSnapType[4];
							xmlStrPrintf(xcSnapType, sizeof(xcSnapType)/sizeof(xmlChar), "%d", pData->iSnapType[i]);
							xmlNewChild(snapinfo, NULL, (const xmlChar*)("snaptype"), xcSnapType);

							xmlNodePtr position = xmlNewNode(NULL, (const xmlChar*)("position"));
							if (position == NULL) break;

							xmlAddChild(snapinfo, position);
							{
								xmlChar xcX[4];
								xmlStrPrintf(xcX, sizeof(xcX)/sizeof(xmlChar), "%d", pData->tSnapPos[i].x);
								xmlNewChild(position, NULL, (const xmlChar*)("x"), xcX);

								xmlChar xcY[4];
								xmlStrPrintf(xcY, sizeof(xcY)/sizeof(xmlChar), "%d", pData->tSnapPos[i].y);
								xmlNewChild(position, NULL, (const xmlChar*)("y"), xcY);

								xmlChar xcW[4];
								xmlStrPrintf(xcW, sizeof(xcW)/sizeof(xmlChar), "%d", pData->tSnapPos[i].w);
								xmlNewChild(position, NULL, (const xmlChar*)("w"), xcW);

								xmlChar xcH[4];
								xmlStrPrintf(xcH, sizeof(xcH)/sizeof(xmlChar), "%d", pData->tSnapPos[i].h);
								xmlNewChild(position, NULL, (const xmlChar*)("h"), xcH);
							}
						}
					}
				}

				// image图片信息节点
				xmlNodePtr image = xmlNewNode(NULL, (const xmlChar*)("image"));
				if (image == NULL) break;

				xmlChar xcImageCnt[4];
				xmlStrPrintf(xcImageCnt, sizeof(xcImageCnt)/sizeof(xmlChar), "%d", pData->iImgNum);
				xmlNewProp(image, (const xmlChar*)("cnt"), xcImageCnt);

				xmlAddChild(body, image);
				{
					xmlChar compose[16];
					xmlStrPrintf(compose, sizeof(compose)/sizeof(xmlChar), "%s", (pData->iImgNum!=pData->iSnapNum?"true":"false"));
					xmlNewChild(image, NULL, (const xmlChar*)("compose"), compose);

					for (int i = 0; i <  pData->iImgNum; i++)
					{
						xmlNewChild(image, NULL, (const xmlChar*)("filename"), (const xmlChar*)(pData->szImgPath[i]));
					}
				}

				// record录像信息节点
				xmlNodePtr record = xmlNewNode(NULL, (const xmlChar*)("record"));
				if (record == NULL) break;

				xmlChar xcRecordCnt[4];
				xmlStrPrintf(xcRecordCnt, sizeof(xcRecordCnt)/sizeof(xmlChar), "%d", pData->iRecNum);
				xmlNewProp(record, (const xmlChar*)("cnt"), xcRecordCnt);

				xmlAddChild(body, record);
				{
					for (int i = 0; i <  pData->iRecNum; i++)
					{
						xmlNewChild(record, NULL, (const xmlChar*)("filename"), (const xmlChar*)(pData->szRecPath[i]));
					}
				}
			}
		}
		iRet = 0;
	} while (0);
	
	return iRet;
}

int create_xml_file(const char *xmlpath, const AnalyDbRecord *pData)
{
	if (xmlpath == NULL || pData == NULL)
	{
		LOG_ERROR_FMT("input is NULL\n");
		return -1;
	}

	xmlDocPtr doc = NULL;
	int iRet = -1;

	do 
	{
		doc = xmlNewDoc((const xmlChar*)("1.0"));
		if (doc == NULL) break;

		iRet = fill_xml_body(doc, pData);
	} while (0);

	if (iRet == 0)
	{
		// 保存
		xmlSaveFormatFileEnc(xmlpath, doc, "UTF-8", 1);
	}

	// 释放
	xmlFreeDoc(doc);

	xmlCleanupParser();

	xmlMemoryDump();//debug memory for regression tests

	return iRet;
}

int create_xml_buff( char *buff, int len, const AnalyDbRecord *pData )
{
	if (buff == NULL || pData == NULL)
	{
		LOG_ERROR_FMT("input is NULL\n");
		return -1;
	}

	xmlDocPtr doc = NULL;
	int iRet = -1;

	do 
	{
		doc = xmlNewDoc((const xmlChar*)("1.0"));
		if (doc == NULL) break;

		doc->encoding = xmlStrdup((const xmlChar*)("UTF-8"));

		iRet = fill_xml_body(doc, pData);
	} while (0);

	if (iRet == 0)
	{
		xmlChar *xmlbuff = NULL;
		int buffersize = 0;
		xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
		if (xmlbuff)
		{
			printf("%s\n", (char *) xmlbuff);  
		}
		
		if (buffersize <= len)
		{
			memcpy(buff, xmlbuff, buffersize);
			iRet = 0;
		}
		else
		{
			printf("buffersize %d <= len %d \n", buffersize, len);  
			iRet = 1;
		}
  
		xmlFree(xmlbuff);  
	}

	// 释放
	xmlFreeDoc(doc);

	xmlCleanupParser();

	xmlMemoryDump();//debug memory for regression tests

	return iRet;
}

