#include "AnalyEvidenceMan.h"
#include "h264_queue.h"
#include "record/AnalyH264CacheMan.h"
#include "image/image_stitch.h"
#include "oal_time.h"
#include "AnalyCfgMan.h"
#include "mq_database.h"
#include "mq_upload.h"
#include "oal_unicode.h"
#include "oal_file.h"
#ifndef WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif
#include "record/AnalyRecFileCacheMan.h"
#include <stdlib.h>

static int g_iChnID[MAX_CHANNEL_NUM] ={0};

const char * LocalIPAddr()
{
	static char s_IPAddr[32] = {0}; 

#ifndef WIN32
	int sockfd;
	struct ifreq ifr;
	struct sockaddr_in sin;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		return s_IPAddr;
	}

	//get eth0 ip	
	strcpy(ifr.ifr_name, "eth0");
	if(ioctl(sockfd,SIOCGIFADDR,&ifr) < 0)
	{
		close(sockfd);
		return s_IPAddr;
	}

	memcpy(&sin,&ifr.ifr_addr,sizeof(sin));
	sprintf(s_IPAddr,"%s",inet_ntoa(sin.sin_addr));
	close(sockfd);
#endif
	return s_IPAddr;
} 	

AnalyData * NewEvidenceNode()
{
	AnalyData *ptNew = new AnalyData;
	if (ptNew)
	{
		memset(ptNew, 0, sizeof(AnalyData));
	}
	return ptNew;
}

void FreeEvidenceNode( AnalyData * pData )
{
	if (pData)
	{
		for(int i = 0; i < MAX_IMG_NUM; i++)
		{
			if(pData->pIplImage[i])
			{
				cvReleaseImage(&pData->pIplImage[i]);
				pData->pIplImage[i] = NULL;
			}
		}

		delete pData;
	}
}

IplImage * ImageInfo2IplImage(const tImageInfo *ptImgInfo)
{
	if(ptImgInfo == NULL)
	{
		return NULL;
	}

	if(ptImgInfo->eImgDataType == IMAGE_DATA_TYPE_NV12)
	{
		cv::Mat matYUV(ptImgInfo->s32ImageHeight*3/2, ptImgInfo->s32ImageWidth, CV_8UC1, ptImgInfo->pImageData);
		cv::Mat matBGR;
		cv::cvtColor(matYUV, matBGR, cv::COLOR_YUV2BGR_NV12);

		//���
		IplImage imgTmp = matBGR;
		IplImage *img = cvCloneImage(&imgTmp);

		return img;
	}
	else if(ptImgInfo->eImgDataType == IMAGE_DATA_TYPE_YUV420)
	{
		cv::Mat matYUV(ptImgInfo->s32ImageHeight*3/2, ptImgInfo->s32ImageWidth, CV_8UC1, ptImgInfo->pImageData);
		cv::Mat matBGR;
		cv::cvtColor(matYUV, matBGR, cv::COLOR_YUV2BGR_I420);

		//���
		IplImage imgTmp = matBGR;
		IplImage *img = cvCloneImage(&imgTmp);

		return img;
	}
	else if(ptImgInfo->eImgDataType == IMAGE_DATA_TYPE_BGR)
	{
		cv::Mat matBGR(ptImgInfo->s32ImageHeight,ptImgInfo->s32ImageWidth, CV_8UC3,ptImgInfo->pImageData,ptImgInfo->s32ImageWidth*3);

		//���
		IplImage imgTmp = matBGR;
		IplImage *img = cvCloneImage(&imgTmp);

		return img;
	}
	return NULL;
}

char KeyWords[][8] = {
	{"%YYYY%"},	// 4λ��
	{"%YY%"},	// 2λ��
	{"%MM%"},	// 2λ��
	{"%DD%"},	// 2λ��
	{"%HH%"},	// 2λСʱ
	{"%mm%"},	// 2λ����
	{"%SS%"},	// 2λ����
	{"%XXX%"},	// 3λ����
	{"%IP%"},	// �豸IP
	{"%ID%"},	// �豸���
	{"%JK%"},	// ��ص�����
	{"%JKD%"},	// ��ص���
	{"%PN%"},	// ���ƺ���
	{"%PC%"},	// ������ɫ
	{"%WFL%"},	// Υ������
	{"%WFD%"}	// Υ������
	//{"%XH0%"},// ��0��ʼ���
	//{"%XH1%"}	// ��1��ʼ���
};
#define KEY_WORDS_NUM (sizeof(KeyWords)/8)

int AlgNameFile(const AlgNameInfo *ptInfo, const char * grammar, int file_type, int index, char *file_name)
{
	if(ptInfo == NULL || grammar == NULL || file_name == NULL)
	{
		return -1;
	}

	strcpy(file_name, grammar);
	if (strlen(file_name) < 1)
	{
		strcpy(file_name, "%YYYY%-%MM%-%DD%_%HH%-%mm%-%SS%-%XXX%");
	}

	char KeyValue[KEY_WORDS_NUM][256] = {{0}};

	struct tm *local_time = localtime(&ptInfo->_time);
	sprintf(KeyValue[0], "%04d", local_time->tm_year+1900);	// 4λ��
	sprintf(KeyValue[1], "%02d", local_time->tm_year-100);	// 2λ��
	sprintf(KeyValue[2], "%02d", local_time->tm_mon+1);		// 2λ��
	sprintf(KeyValue[3], "%02d", local_time->tm_mday);		// 2λ��
	sprintf(KeyValue[4], "%02d", local_time->tm_hour);		// 2λСʱ
	sprintf(KeyValue[5], "%02d", local_time->tm_min);		// 2λ����
	sprintf(KeyValue[6], "%02d", local_time->tm_sec);		// 2λ����
	sprintf(KeyValue[7], "%03d", ptInfo->_msec);			// 3λ����
	sprintf(KeyValue[8], "%s",	 ptInfo->_IP);				// �豸IP
	sprintf(KeyValue[9], "%s",	 ptInfo->_ID);				// �豸���
	sprintf(KeyValue[10], "%s",	 ptInfo->_JKD);				// ��ص�����
	sprintf(KeyValue[11], "%s",	 ptInfo->_JK);				// ��ص���
	sprintf(KeyValue[12], "%s",	 ptInfo->_PN);				// ���ƺ���
	sprintf(KeyValue[13], "%d",	 ptInfo->_PC);				// ������ɫ
	sprintf(KeyValue[14], "%s",	 ptInfo->_WFL);				// Υ������
	sprintf(KeyValue[15], "%s",	 ptInfo->_WFD);				// Υ������
	//sprintf(KeyValue[16], "%d",	 index-1);					// ��0��ʼ���
	//sprintf(KeyValue[17], "%d",	 index);					// ��1��ʼ���

	for (int i = 0; i < KEY_WORDS_NUM; i++)
	{
		ReplaceKeyWord(file_name, KeyWords[i],KeyValue[i]);
	}
	
	// �����ļ���׺
	char szSuffix1[5] = {0};
	char szSuffix2[5] = {0};
	if(file_type == FILE_MP4)
	{
		strcpy(szSuffix1, ".mp4");
		strcpy(szSuffix2, ".MP4");
	}
	else if(file_type == FILE_AVI)
	{
		strcpy(szSuffix1, ".avi");
		strcpy(szSuffix2, ".AVI");
	}
	else if(file_type == FILE_FLV)
	{
		strcpy(szSuffix1, ".flv");
		strcpy(szSuffix2, ".FLV");
	}
	else
	{
		strcpy(szSuffix1, ".jpg");
		strcpy(szSuffix2, ".JPG");
	}

	if (strlen(file_name) > 4)
	{
		if (strncmp(file_name+strlen(file_name)-4, szSuffix1, 4) != 0&&
			strncmp(file_name+strlen(file_name)-4, szSuffix2, 4) != 0)
			strcat(file_name, szSuffix1);
	}
	else
	{
		strcat(file_name, szSuffix1);
	}

	// ���������
	int rdret = -1;
	do
	{
		long int iRd = random();
		char cRd[16] = {0};
		sprintf(cRd, "%ld", iRd%10);
		rdret = ReplaceKeyWord(file_name, "%RD%", cRd, 0);
	}while(rdret==0);


	// �������
	if (strstr(file_name, "%XH0%"))
	{
		char cXH0[16] = {0};
		sprintf(cXH0, "%d", index-1);
		ReplaceKeyWord(file_name, "%XH0%", cXH0);
	}
	else if (strstr(file_name, "%XH1%"))
	{
		char cXH1[16] = {0};
		sprintf(cXH1, "%d", index);
		ReplaceKeyWord(file_name, "%XH1%", cXH1);
	}
	else if (index > 1)
	{
		char cXH1[16] = {0};
		sprintf(cXH1, "%d", index);

		char szSuffix[5] = {0};
		strncpy(szSuffix, file_name+strlen(file_name)-4, 4);
		file_name[strlen(file_name)-4] = '\0';
		strcat(file_name,"-");
		strcat(file_name,cXH1);
		strcat(file_name,szSuffix);
	}
	return 0;
}

// Ϊ¼��ȡ֤����H264����
void *H264CaptureThreadFxn(void* param)
{
	if(param == NULL)
	{
		return NULL;
	}

	int iChnID = *((int *)(param));
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return NULL;
	}

	LOG_DEBUG_FMT("Channel%d H264Cache Thread is Running...",iChnID);

	H264_Queue tH264Queue = {0};
	tH264Queue.iChnID = iChnID;
	tH264Queue.service_type = H264_QUEUE_EVIDENCE_SERVICE;
	int iRet = H264_QueueOpen(&tH264Queue);
	if(iRet < 0)
	{
		LOG_ERROR_FMT("Channel%d H264_QueueOpen Failed...",iChnID);
		return NULL;
	}

	LOG_DEBUG_FMT("Channel%d H264_QueueOpen OK,shm_fd = %d...", iChnID,tH264Queue.shm_fd);
	H264_Queue_Node * ptDataNode = (H264_Queue_Node*)malloc(sizeof(H264_Queue_Node));
	assert(ptDataNode);

	while(!AnalyEvidenceMan::GetInstance()->m_bExitThread)
    {
		memset(ptDataNode, 0, sizeof(H264_Queue_Node));
		iRet = H264_QueueGetFull(&tH264Queue, &ptDataNode->h264_info, ptDataNode->frame_data);
		if(iRet == 0 && ptDataNode->h264_info.data_size < H264_QUEUE_FRAME_MAX)
		{
			AlgH264Frame * ptFrame = (AlgH264Frame *)malloc(sizeof(AlgH264Frame));
			if (ptFrame == NULL)
			{
				LOG_ERROR_FMT("malloc AlgH264Frame failed");
				continue;
			}

			ptFrame->frame_head.size = ptDataNode->h264_info.data_size;
			ptFrame->frame_head.t	 = time(NULL);// �Լ����ʱ��Ϊ׼
			ptFrame->frame_head.frame_type = ptDataNode->h264_info.frame_type;
			memcpy(ptFrame->frame_data, ptDataNode->frame_data, ptDataNode->h264_info.data_size);

			AnalyEvidenceMan::GetInstance()->SetFrameInfo(iChnID,
				ptDataNode->h264_info.frame_rate,
				ptDataNode->h264_info.width,
				ptDataNode->h264_info.height);

			if(AnalyH264CacheMan::GetInstance()->PushFrameToCache(iChnID, ptFrame) != 0)
			{
				free(ptFrame);
				ptFrame = NULL;
				printf("Channel%d Push frame failed:size:%d\n",iChnID, ptDataNode->h264_info.data_size);
			}
		}
		else
		{
			usleep(10*1000);
		}
	}

	H264_QueueClose(&tH264Queue);
	free(ptDataNode);
	ptDataNode = NULL;

	pthread_exit(NULL);

	return NULL;
}

// ͼ��ȡ֤�߳�
void* ImgEvidenceThreadFxn(void* param)
{
	if(param == NULL)
	{
		return NULL;
	}

	int iChnID = *((int *)(param));
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return NULL;
	}

	LOG_DEBUG_FMT("Channel%d Img Evidence Thread is Running...",iChnID);

	while(!AnalyEvidenceMan::GetInstance()->m_bExitThread)
	{
		AnalyData * ptData = AnalyEvidenceMan::GetInstance()->PopImgEvidenceNode();
		if(ptData == NULL)
		{
			usleep(100*1000);
			continue;
		}

		//printf("%s %d\n", __FUNCTION__, __LINE__);

		// δ��Ȩ������κν��
		if(!AnalyCfgMan::GetInstance()->IsAuthorised())
		{
			LOG_INFOS_FMT("No license information,Stop Write File...");
			FreeEvidenceNode(ptData);
			ptData = NULL;
			continue;
		}

		if(AnalyCfgMan::GetInstance()->IsNeedStopWriteForFull())
		{
			LOG_INFOS_FMT("Save Path is Full,Stop Write File...");
			FreeEvidenceNode(ptData);
			ptData = NULL;
			continue;
		}

		// �ռ�ͼƬ֤��
		if (AnalyEvidenceMan::GetInstance()->DoImageEvidence(ptData) != 0)
		{
			LOG_ERROR_FMT("DoImageEvidence failed...");
			FreeEvidenceNode(ptData);
			ptData = NULL;
			continue;
		}

		// �첽�ռ�¼��֤��
		if (AnalyEvidenceMan::GetInstance()->Push2RecEvidenceNode(ptData) != 0)
		{
			// д���ݿ�
			if (AnalyEvidenceMan::GetInstance()->Push2DataBase(ptData) != 0)
			{
				LOG_ERROR_FMT("Push2DataBase failed...");
			}

			FreeEvidenceNode(ptData);
			ptData = NULL;
		}
	}

	pthread_exit(NULL);

	return NULL;
}

// ¼��ȡ֤�߳�
void *RecEvidenceThreadFxn(void* param)
{
	if(param == NULL)
	{
		return NULL;
	}

	int iChnID = *((int *)(param));
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return NULL;
	}

	LOG_DEBUG_FMT("Channel%d Rec Evidence Thread is Running...",iChnID);

	while(!AnalyEvidenceMan::GetInstance()->m_bExitThread)
	{
		AnalyData * ptData = AnalyEvidenceMan::GetInstance()->PopRecEvidenceNode();
		if(ptData == NULL)
		{
			usleep(100*1000);
			continue;
		}

		// ����¼��
		int iRet = AnalyEvidenceMan::GetInstance()->DoRecordEvidence(ptData);
		if( iRet == 1)
		{
			// ʱ��δ�� ���·��������б�
			int iRet = AnalyEvidenceMan::GetInstance()->Push2RecEvidenceNode(ptData);
			if (iRet != 0)
			{
				LOG_ERROR_FMT("Channel%d Push2RecEvidenceNode failed...",iChnID);
				FreeEvidenceNode(ptData);
				ptData = NULL;
			}

			usleep(500*1000);
			continue;
		}
		else if (iRet == 0)
		{
			// ¼�����ɳɹ� д���ݿ�
			int iRet = AnalyEvidenceMan::GetInstance()->Push2DataBase(ptData);
			if (iRet != 0)
			{
				LOG_ERROR_FMT("Channel%d Push2DataBase failed...",iChnID);
			}
		}
		else
		{
			LOG_ERROR_FMT("Channel%d DoRecordEvidence failed...",iChnID);
		}
		
		// �ͷŽڵ���Դ
		FreeEvidenceNode(ptData);
		ptData = NULL;
	}

	pthread_exit(NULL);
	return NULL;
}


AnalyEvidenceMan* AnalyEvidenceMan::m_pInstance = NULL;

AnalyEvidenceMan* AnalyEvidenceMan::GetInstance()
{
	return m_pInstance;
}

int AnalyEvidenceMan::Initialize()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new AnalyEvidenceMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return 0;
}

void AnalyEvidenceMan::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

AnalyEvidenceMan::AnalyEvidenceMan()
{
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_frame_rate[i] = 25;
		m_frame_width[i] = 1920;
		m_frame_height[i] = 1080;
	}

	m_hImgEviQue = QueCreate(10);
	m_hRecEviQue = QueCreate(10);

	AnalyH264CacheMan::Initialize();
	AnalyRecFileCacheMan::Initialize();
}

AnalyEvidenceMan::~AnalyEvidenceMan()
{
	m_bExitThread = false;
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		pthread_join(m_h264_thread[i],NULL);
		pthread_join(m_img_thread[i],NULL);
		pthread_join(m_rec_thread[i],NULL);
	}

	QueDelete(m_hImgEviQue);
	QueDelete(m_hRecEviQue);

	AnalyRecFileCacheMan::UnInitialize();
	AnalyH264CacheMan::UnInitialize();
}

void AnalyEvidenceMan::Run()
{
	m_bExitThread = false;
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		g_iChnID[i] = i;
		if(pthread_create(&m_h264_thread[i], NULL, H264CaptureThreadFxn, g_iChnID+i))
		{
			LOG_ERROR_FMT("Channel%d H264 Capture Thread create failed", i);
		}

		if(pthread_create(&m_img_thread[i], NULL, ImgEvidenceThreadFxn, g_iChnID+i))
		{
			LOG_ERROR_FMT("Channel%d Image Evidence Thread create failed", i);
		}

		if(pthread_create(&m_rec_thread[i], NULL, RecEvidenceThreadFxn, g_iChnID+i))
		{
			LOG_ERROR_FMT("Channel%d Record Evidence Thread create failed", i);
		}
	}
}

int AnalyEvidenceMan::SetFrameInfo( int iChnID, int iFrameRate, int iFrameWidth, int iFrameHeight )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM -1)
	{
		return -1;
	}
	m_frame_rate[iChnID] = iFrameRate;
	m_frame_width[iChnID] = iFrameWidth;
	m_frame_height[iChnID] = iFrameHeight;
	return 0;
}

int AnalyEvidenceMan::GetFrameInfo( int iChnID, int &iFrameRate, int &iFrameWidth, int &iFrameHeight )
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM -1)
	{
		return -1;
	}
	iFrameRate = m_frame_rate[iChnID];
	iFrameWidth = m_frame_width[iChnID];
	iFrameHeight = m_frame_height[iChnID];
	return 0;
}

int AnalyEvidenceMan::Push2ImgEvidenceNode( AnalyData * pData )
{
	if (pData == NULL)
	{
		return -1;
	}

	AnalyData * ptNew = NewEvidenceNode();
	if (ptNew == NULL)
	{
		return -1;
	}

	//printf("%s %d\n", __FUNCTION__, __LINE__);
	memcpy(ptNew, pData, sizeof(AnalyData));
	return QuePush(m_hImgEviQue, ptNew, TIMEOUT_NONE);
}

AnalyData * AnalyEvidenceMan::PopImgEvidenceNode()
{
	return (AnalyData *)QuePop(m_hImgEviQue, TIMEOUT_FOREVER);
}

int AnalyEvidenceMan::Push2RecEvidenceNode( AnalyData * pData )
{
	if (pData == NULL)
	{
		return -1;
	}

	AnalyData * ptNew = NewEvidenceNode();
	if (ptNew == NULL)
	{
		return 1;
	}
	memcpy(ptNew, pData, sizeof(AnalyData));
	return QuePush(m_hRecEviQue, ptNew, TIMEOUT_NONE);
}

AnalyData * AnalyEvidenceMan::PopRecEvidenceNode()
{
	return (AnalyData *)QuePop(m_hRecEviQue, TIMEOUT_FOREVER);
}

int AnalyEvidenceMan::Push2DataBase( AnalyData * pData )
{
	if(pData == NULL)
	{
		return -1;
	}
	char tmpBuf[40] = {0};

	switch(pData->iPlateColor)
	{
	case 3:
		if(strstr(pData->szPlate,"ʹ"))
			pData->iPlateType = 3;
		else if(strstr(pData->szPlate,"��"))
			pData->iPlateType = 4;
		else
			pData->iPlateType = 6;
		break;
	case 2:
		pData->iPlateType  = 2;
		break;
	case 1:
		if(strstr(pData->szPlate,"ѧ"))
			pData->iPlateType = 16;
		else if(strstr(pData->szPlate,"��"))
			pData->iPlateType = 15;
		else
			pData->iPlateType = 1;
		break;
	case 0:
		if(strstr(pData->szPlate,"��"))
			pData->iPlateType = 23;
		else if(strncmp(pData->szPlate,"WJ",2)==0)
			pData->iPlateType = 26 ;
		pData->iPlateType = 25;
		break;
	default:
		pData->iPlateType= 99;
		break;
	};

	char szPlatePos[64] = {0};
	if(pData->tPlatePos.x != 0 ||pData->tPlatePos.y != 0 || pData->tPlatePos.w != 0 || pData->tPlatePos.h != 0)
	{
		sprintf(szPlatePos, "%d,%d,%d,%d",
			pData->tPlatePos.x, pData->tPlatePos.y, pData->tPlatePos.w, pData->tPlatePos.h);
	}

	char szPicTime[MAX_IMG_NUM][64] = {0};
	char szPicPos[MAX_IMG_NUM][64] = {0};
	for (int i = 0; i < pData->iPicNum; i++)
	{
		if(pData->tPicTime[i] > 0)
		{
			TimeFormatString(pData->tPicTime[i], szPicTime[i], sizeof(szPicTime[i]), eYMDHMS2);
			sprintf(szPicTime[i]+strlen(szPicTime[i]),"%03d",pData->tPicMsec[i]);
		}
		if(pData->tPicPos[i].x != 0 || pData->tPicPos[i].y != 0 || pData->tPicPos[i].w != 0 || pData->tPicPos[i].h != 0)
		{
			sprintf(szPicPos[i], "%d,%d,%d,%d",
				pData->tPicPos[i].x, pData->tPicPos[i].y, pData->tPicPos[i].w, pData->tPicPos[i].h);
		}
	}

	DeviceInfoT tDev = {0};
	AnalyCfgMan::GetInstance()->GetDevInfo(&tDev);

	PlaceInfoT tMon = {0};
	AnalyCfgMan::GetInstance()->GetChnPlaceInfoT(pData->iChnID, &tMon);

	EventCodeT tCode = {0};
	AnalyCfgMan::GetInstance()->GetEventCode(pData->iEventType, &tCode);

	SceneInfoT tScene = {0};
	AnalyCfgMan::GetInstance()->GetChnSceneInfo(pData->iChnID, pData->iSceneID, &tScene);

	char szSql[6*1024] = {0};
	sprintf(szSql, "insert into AnalyTable("
		"CREATETIME,CREATEMESC,"
		"DATATYPE,EVENTID,CHNID,SCENEID,"
		"PLATE,PLATECOLOR,PLATETYPE,PLATEPIC,PLATEPOS,CONFIDENCE,"
		"CARTYPE,CARCOLOR,BRAND,SPEED,"
		"SNAPNUM,PICNUM,PICPATH,"
		"PIC1NAME,PIC1TIME,PIC1POS,SNAP1TYPE,"
		"PIC2NAME,PIC2TIME,PIC2POS,SNAP2TYPE,"
		"PIC3NAME,PIC3TIME,PIC3POS,SNAP3TYPE,"
		"PIC4NAME,PIC4TIME,PIC4POS,SNAP4TYPE,"
		"RECNUM,RECPATH,REC1NAME,REC2NAME,"
		"NETNO,REGNO,DEVNO,MANNO,"
		"AREAID,AREANAME,MONID,MONNAME,CROSSID,CROSSNAME,"		
		"SCENENAME,DIRECTION,"
		"CODE,DESC,"
		"STATUS)values("
		"%ld,%d,"
		"%d,%d,%d,%d,"
		"'%s',%d,%d,%d,'%s',%d,"
		"%d,%d,%d,%d,"
		"%d,%d,'%s',"
		"'%s','%s','%s',%d,"
		"'%s','%s','%s',%d,"
		"'%s','%s','%s',%d,"
		"'%s','%s','%s',%d,"
		"'%d','%s',"
		"'%s','%s',"
		"'%s','%s','%s','%s',"
		"'%s','%s','%s','%s','%s','%s',"
		"'%s',%d,"
		"'%s','%s',"
		"%d);",
		pData->tCreateTime,pData->tCreateMsec,
		pData->iDateType,pData->iEventType,pData->iChnID,pData->iSceneID,
		pData->szPlate,pData->iPlateColor,pData->iPlateType,pData->iPlateIndex,szPlatePos,pData->iPlateConf,
		pData->iCarType,pData->iCarColor,pData->iCarBrand,pData->iCarSpeed,
		pData->iPicNum,pData->iPicNumFinal,pData->szPicRootPath,
		pData->szPicFile[0],szPicTime[0],szPicPos[0],pData->iPicType[0],
		pData->szPicFile[1],szPicTime[1],szPicPos[1],pData->iPicType[1],
		pData->szPicFile[2],szPicTime[2],szPicPos[2],pData->iPicType[2],
		pData->szPicFile[3],szPicTime[3],szPicPos[3],pData->iPicType[3],
		1,pData->szRecRootPath,	pData->szRecFile, "",
		tDev.szNetID, tDev.szRegID, tDev.szDevID, tDev.szManID,
		tMon.szAreaID, tMon.szAreaName, tMon.szMonitoryID, tMon.szMonitoryName, tMon.szPlaceID, tMon.szPlaceName,
		tScene.szSceneName, tScene.iDirection,
		tCode.szCode, tCode.szDesc,
		eUnUp);

	char szSqlUtf8[6*1024] = {0};
	GB2312_To_UTF8(szSql, strlen(szSql),szSqlUtf8, sizeof(szSqlUtf8));
	return MQ_DataBase_Insert(MSG_TYPE_ANALY, szSqlUtf8);
}

int AnalyEvidenceMan::CreateImgFileName(AnalyData * pData)
{
	if(pData == NULL)
	{
		return -1;
	}

	DeviceInfoT tDev = {0};
	AnalyCfgMan::GetInstance()->GetDevInfo(&tDev);

	PlaceInfoT tMon = {0};
	AnalyCfgMan::GetInstance()->GetChnPlaceInfoT(pData->iChnID, &tMon);

	EventCodeT tCode = {0};
	AnalyCfgMan::GetInstance()->GetEventCode(pData->iEventType, &tCode);

	FileNamingT tNaming = {0};
	AnalyCfgMan::GetInstance()->GetEventFileNaming(pData->iEventType, &tNaming);

	AlgNameInfo tInfo;
	tInfo._time	= pData->tCreateTime;
	tInfo._msec = pData->tCreateMsec;
	tInfo._PC   = pData->iPlateColor;
	strcpy(tInfo._IP, LocalIPAddr());
	strcpy(tInfo._ID, tDev.szDevID);
	strcpy(tInfo._JK, tMon.szMonitoryName);
	strcpy(tInfo._JKD,tMon.szMonitoryID);
	strcpy(tInfo._PN, pData->szPlate);
	strcpy(tInfo._WFL,tCode.szDesc);
	strcpy(tInfo._WFD,tCode.szCode);

	char  szYYYMMDD[40] = {0};
	TimeFormatString(pData->tCreateTime, szYYYMMDD, sizeof(szYYYMMDD), eYMD2);

	const char * pszRoot = AnalyCfgMan::GetInstance()->GetRootPath();
	if(pData->iDateType == eEventData)
	{
		sprintf(pData->szPicRootPath, "%s/%s/%s/", pszRoot, RESULT_EVENT_DIR, szYYYMMDD);
	}
	else if(pData->iDateType == eTollData)
	{
		sprintf(pData->szPicRootPath, "%s/%s/%s/", pszRoot, RESULT_TOLL_DIR, szYYYMMDD);
	}
	else
	{
		sprintf(pData->szPicRootPath, "%s/%s/%s/", pszRoot, RESULT_ILLEGAL_DIR, szYYYMMDD);
	}
	OAL_MakeDirByPath(pData->szPicRootPath);

	for (int i = 0; i < pData->iPicNumFinal; i++)
	{
		if(pData->iDateType == eEventData)
		{
			AlgNameFile(&tInfo, tNaming.szEventImg, FILE_JPG, i+1, pData->szPicFile[i]);
		}
		else
		{
			AlgNameFile(&tInfo, tNaming.szIllegalImg, FILE_JPG, i+1, pData->szPicFile[i]);
		}

		//printf("pData->szPicFile[%d]= %s\n",i ,pData->szPicFile[i]);
	}

	return 0;
}

int AnalyEvidenceMan::DoImageEvidence( AnalyData * pData )
{
	if (pData == NULL)
	{
		return -1;
	}

	if (pData->iPicNum < 1 || pData->iPicNum > MAX_IMG_NUM)
	{
		LOG_ERROR_FMT("Channel%d ID:%d PicNum Error", pData->iChnID, pData->iID);
		return -1;
	}
	//printf("-----------------------------\n");
	//printf("iPicNum:%d\n",pData->iPicNum);

	// ��ȡͼƬ�ϳ�����
	ImgComposeT tImgCom;
	AnalyCfgMan::GetInstance()->GetEventImgCompose(pData->iEventType, &tImgCom);
	//printf("2��:%s\n",tImgCom.iComModel2==eVerCom?"��ֱ�ϳ�":(tImgCom.iComModel2==eHorCom?"ˮƽ�ϳ�":(tImgCom.iComModel2==eMatCom?"���ֺϳ�":"���ϳ�")));
	//printf("3��:%s\n",tImgCom.iComModel3==eVerCom?"��ֱ�ϳ�":(tImgCom.iComModel3==eHorCom?"ˮƽ�ϳ�":(tImgCom.iComModel3==eMatCom?"���ֺϳ�":"���ϳ�")));
	//printf("4��:%s\n",tImgCom.iComModel4==eVerCom?"��ֱ�ϳ�":(tImgCom.iComModel4==eHorCom?"ˮƽ�ϳ�":(tImgCom.iComModel4==eMatCom?"���ֺϳ�":"���ϳ�")));

	ImgQualityT tImgQua;
	AnalyCfgMan::GetInstance()->GetEventImgQuality(pData->iEventType, &tImgQua);
	if (tImgQua.iWidthLimit < 100)
		tImgQua.iWidthLimit = pData->iPicWidth;
	if (tImgQua.iHeightLimit < 100)
		tImgQua.iHeightLimit = pData->iPicHeight;
	//printf("�������:%d\n�߶�����:%d\n��С����:%dKB\n",
	//	tImgQua.iWidthLimit, tImgQua.iHeightLimit, tImgQua.iSizeLimit);

	ImgOSDT tImgOsd;
	AnalyCfgMan::GetInstance()->GetEventImgOSD(pData->iEventType, &tImgOsd);
// 	printf("λ��: %s\n",
// 		tImgOsd.iPosModel==eEachUp   ? "�����Ϸ�" : (
// 		tImgOsd.iPosModel==eEachIn   ? "�����ڲ�" : (
// 		tImgOsd.iPosModel==eEachDown ? "�����·�" : (
// 		tImgOsd.iPosModel==eComUp    ? "�ϳ��Ϸ�" : (
// 		tImgOsd.iPosModel==eComIn    ? "�ϳ��ڲ�" : "�ϳ��·�")))));
// 	printf("�߾�: ��=%d%% ��=%d%%\n",tImgOsd.iLeft,tImgOsd.iTop);
// 	printf("����: ��С=%d ��ɫ: %02X %02X %02X",tImgOsd.iFontSize,tImgOsd.tFontColor.r, tImgOsd.tFontColor.g, tImgOsd.tFontColor.b);
// 	printf("����: ��ɫ=%02X %02X %02X\n",tImgOsd.tBackColor.r, tImgOsd.tBackColor.g,tImgOsd.tBackColor.b);
// 	printf("����: Flag=%04X", tImgOsd.iContentFlag);
// 	printf(" ����ʱ��(%s)",(tImgOsd.iContentFlag&OSD_TIME)?"O":"X");
// 	printf(" ʱ�����(%s)",(tImgOsd.iContentFlag&OSD_MESC)?"O":"X");
// 	printf(" �豸���(%s)",(tImgOsd.iContentFlag&OSD_DEVID)?"O":"X");
// 	printf(" Υ���ص�(%s)",(tImgOsd.iContentFlag&OSD_PLACE)?"O":"X");
// 	printf(" Υ����Ϊ(%s)",(tImgOsd.iContentFlag&OSD_EVENT)?"O":"X");
// 	printf(" Υ������(%s)",(tImgOsd.iContentFlag&OSD_CODE)?"O":"X");
// 	printf(" ���ƺ���(%s)",(tImgOsd.iContentFlag&OSD_PLATE)?"O":"X");
// 	printf(" ������ɫ(%s)",(tImgOsd.iContentFlag&OSD_PLATE_COLOR)?"O":"X");
// 	printf(" ��������(%s)",(tImgOsd.iContentFlag&OSD_DIRECTION)?"O":"X");
// 	printf(" ��αˮӡ(%s)",(tImgOsd.iContentFlag&OSD_WATERMARK)?"O":"X");
// 	printf("\n");
// 
// 	printf("-----------------------------\n");
	// ���������С
	if(tImgOsd.iFontSize > 1 && tImgOsd.iFontSize < 1000)
	{
		resetFont(tImgOsd.iFontSize);
	}

	// ����������ȡ�ϳɷ�ʽ
	int iComModel = eNotCom;
	if(pData->iPicNum == 2)
		iComModel = tImgCom.iComModel2;
	else if(pData->iPicNum == 3)
		iComModel = tImgCom.iComModel3;
	else if(pData->iPicNum == 4)
		iComModel = tImgCom.iComModel4;

	// ����OSD����
	OsdContent tOsdString[MAX_IMG_NUM] = {0};
	if(tImgOsd.iPosModel == eEachIn || tImgOsd.iPosModel == eEachDown || tImgOsd.iPosModel == eEachUp)
	{
		int iTotalWidth = tImgQua.iWidthLimit;
		int iContainerWidth = iTotalWidth - 2*OSD_LINE_GAP;
		if(tImgOsd.iPosModel == eEachIn)
			iContainerWidth -= tImgOsd.iLeft*iTotalWidth/100;

		for(int i = 0; i < pData->iPicNum; i++)
		{
			CreateImgOsdString(pData->iChnID, pData->iSceneID, pData->iEventType,
				pData->tPicTime[i], pData->tPicMsec[i],	pData->szPlate, pData->iPlateColor,
				tImgOsd.iContentFlag, iContainerWidth, &tOsdString[i]);
		}
	}
	else if(tImgOsd.iPosModel == eComIn || tImgOsd.iPosModel == eComDown || tImgOsd.iPosModel == eComUp)
	{
		int iTotalWidth = tImgQua.iWidthLimit;
		if(iComModel == eHorCom)
			iTotalWidth = tImgQua.iWidthLimit*pData->iPicNum;
		else if(iComModel == eMatCom)
			iTotalWidth = tImgQua.iWidthLimit*2;

		int iContainerWidth = iTotalWidth - 2*OSD_LINE_GAP;
		if(tImgOsd.iPosModel == eComIn)
			iContainerWidth -= tImgOsd.iLeft*iTotalWidth/100;

		CreateImgOsdString(pData->iChnID, pData->iSceneID, pData->iEventType,
			pData->tPicTime[0], pData->tPicMsec[0],	pData->szPlate, pData->iPlateColor,
			tImgOsd.iContentFlag, iContainerWidth, &tOsdString[0]);
	}

	// ����ͼƬ�ļ���
	if (iComModel == eNotCom)
	{
		pData->iPicNumFinal = pData->iPicNum;	
		CreateImgFileName(pData);
	}
	else
	{
		pData->iPicNumFinal = 1;
		CreateImgFileName(pData);
	}

	// �ϳ�&����OSD
	switch(iComModel)
	{
	case eNotCom:
		{
			Image_Stitch_None(pData, tOsdString);
			break;
		}
	case eVerCom:
		{
			Image_Stitch_Vertical(pData, tOsdString);
			break;
		}
	case eHorCom:
		{
			Image_Stitch_Horizontal(pData, tOsdString);
			break;
		}
	case eMatCom:
		{
			Image_Stitch_Matts(pData, tOsdString);
			break;
		}
	};

	// ͼƬȡ֤��ɾͿ����ͷ���Դ��
	for(int i = 0; i < MAX_IMG_NUM; i++)
	{
		if(pData->pIplImage[i])
		{
			cvReleaseImage(&pData->pIplImage[i]);
			pData->pIplImage[i] = NULL;
		}
	}

	return 0;
}

int AnalyEvidenceMan::CreateRecFileName( AnalyData * pData )
{
	if(pData == NULL)
	{
		return -1;
	}

	DeviceInfoT tDev = {0};
	AnalyCfgMan::GetInstance()->GetDevInfo(&tDev);

	PlaceInfoT tMon = {0};
	AnalyCfgMan::GetInstance()->GetChnPlaceInfoT(pData->iChnID, &tMon);

	EventCodeT tCode = {0};
	AnalyCfgMan::GetInstance()->GetEventCode(pData->iEventType, &tCode);

	RecEvidenceT tEvi = {0};
	AnalyCfgMan::GetInstance()->GetEventRecEvidence(pData->iEventType, &tEvi);

	FileNamingT tNaming = {0};
	AnalyCfgMan::GetInstance()->GetEventFileNaming(pData->iEventType, &tNaming);

	AlgNameInfo tInfo;
	tInfo._time	= pData->tCreateTime;
	tInfo._msec = pData->tCreateMsec;
	tInfo._PC   = pData->iPlateColor;
	strcpy(tInfo._IP, LocalIPAddr());
	strcpy(tInfo._ID, tDev.szDevID);
	strcpy(tInfo._JK, tMon.szMonitoryName);
	strcpy(tInfo._JKD,tMon.szMonitoryID);
	strcpy(tInfo._PN, pData->szPlate);
	strcpy(tInfo._WFL,tCode.szDesc);
	strcpy(tInfo._WFD,tCode.szCode);

	char  szYYYMMDD[40] = {0};
	TimeFormatString(pData->tCreateTime, szYYYMMDD, sizeof(szYYYMMDD), eYMD2);

	const char * pszRoot = AnalyCfgMan::GetInstance()->GetRootPath();
	if(pData->iDateType == eEventData)
	{
		sprintf(pData->szRecRootPath, "%s/%s/%s/", pszRoot, RESULT_EVENT_DIR, szYYYMMDD);
	}
	else if(pData->iDateType == eTollData)
	{
		sprintf(pData->szRecRootPath, "%s/%s/%s/", pszRoot, RESULT_TOLL_DIR, szYYYMMDD);
	}
	else
	{
		sprintf(pData->szRecRootPath, "%s/%s/%s/", pszRoot, RESULT_ILLEGAL_DIR, szYYYMMDD);
	}

	OAL_MakeDirByPath(pData->szRecRootPath);

	if(pData->iDateType == eEventData)
	{
		AlgNameFile(&tInfo, tNaming.szEventRec, tEvi.iFileFormat==REC_FILE_FLV? FILE_FLV:(tEvi.iFileFormat==REC_FILE_AVI? FILE_AVI:FILE_MP4), 1, pData->szRecFile);
	}
	else
	{
		AlgNameFile(&tInfo, tNaming.szIllegalRec, tEvi.iFileFormat==REC_FILE_FLV? FILE_FLV:(tEvi.iFileFormat==REC_FILE_AVI? FILE_AVI:FILE_MP4), 1, pData->szRecFile);
	}

	return 0;
}

int AnalyEvidenceMan::DoRecordEvidence( AnalyData * pData )
{
	if (pData == NULL)
	{
		return -1;
	}

	int iRet = 0;
	char tmpBuf[128] ={0};

	// ��ȡ�ϳ�����
	RecEvidenceT tRecEvi;
	AnalyCfgMan::GetInstance()->GetEventRecEvidence(pData->iEventType, &tRecEvi);

	if(tRecEvi.bEnable == false)
	{
		printf("Record Evidence Closed\n");
		return 0;// ������¼��ȡ֤
	}

	CreateRecFileName(pData);

	// ¼���ļ�
	RecMission tRecMission;
	tRecMission.iID = pData->iID;
	sprintf(tRecMission.szPathFile, "%s%s", pData->szRecRootPath, pData->szRecFile);
	tRecMission.iFileFormat = tRecEvi.iFileFormat;
	tRecMission.iCnt = 1;
	tRecMission.begin[0] = pData->tPicTime[0] - tRecEvi.iPreTime;
	tRecMission.secs[0]  = (int)(tRecEvi.iPreTime+ (pData->tPicTime[pData->iPicNum-1] - pData->tPicTime[0]) + tRecEvi.iAftTime);
	if(tRecMission.secs[0] < tRecEvi.iMinTime)
	{
		tRecMission.begin[0] = tRecMission.begin[0] + tRecMission.secs[0] - tRecEvi.iMinTime/2;
		tRecMission.secs[0] = tRecEvi.iMinTime;
	}

	// Υͣ+����ץ��+ƴ��
	if(pData->iDateType == eIllegaldata && pData->iEventType == eIT_Illegal_Park)
	{
		// Υͣ�¼����� ǿ��ƴ��
		if(AnalyCfgMan::GetInstance()->GetIllegeStopTime() >= MAX_FULL_RECORD_SECS)
		{
			tRecEvi.iRecModel = eRecJoint;
		}
		if(tRecEvi.iRecModel == eRecJoint && pData->tSnap.s32SnapCnt == 2)
		{
			tRecEvi.iPreTime = tRecEvi.iPreTime < 2? 2:tRecEvi.iPreTime;
			tRecEvi.iAftTime = tRecEvi.iAftTime < 2? 2:tRecEvi.iAftTime;

			tRecMission.iCnt = 2;
			tRecMission.begin[0] = pData->tSnap.tSnapInfos[0].d64TimeBegin.tv_sec - tRecEvi.iPreTime;
			tRecMission.secs[0]  = tRecEvi.iPreTime + (pData->tSnap.tSnapInfos[0].d64TimeEnd.tv_sec - pData->tSnap.tSnapInfos[0].d64TimeBegin.tv_sec);
			if(tRecMission.secs[0] < 5) tRecMission.begin[0] = tRecMission.begin[0] + tRecMission.secs[0] - 5;

			if(pData->tSnap.tSnapInfos[1].d64TimeBegin.tv_sec < 100)
			{
				if(pData->tSnap.tSnapInfos[1].d64TimeEnd.tv_sec - pData->tSnap.tSnapInfos[0].d64TimeEnd.tv_sec > 10)
				{
					pData->tSnap.tSnapInfos[1].d64TimeBegin.tv_sec = pData->tSnap.tSnapInfos[1].d64TimeEnd.tv_sec - 10;
				}
				else
				{
					pData->tSnap.tSnapInfos[1].d64TimeBegin.tv_sec = pData->tSnap.tSnapInfos[0].d64TimeEnd.tv_sec;
				}
			}

			tRecMission.begin[1] = pData->tSnap.tSnapInfos[1].d64TimeBegin.tv_sec;
			tRecMission.secs[1]  = (pData->tSnap.tSnapInfos[1].d64TimeEnd.tv_sec - pData->tSnap.tSnapInfos[1].d64TimeBegin.tv_sec) + tRecEvi.iAftTime;
			if(tRecMission.secs[1] < 5) tRecMission.secs[1] = 5;
		}
	}

	printf("Channel%d %d ¼��ȡ֤ Ԥ¼:%d �Ӻ�:%d �ϳ�:%s ͼƬ%d��\n",pData->iChnID,
		pData->iID,
		tRecEvi.iPreTime,
		tRecEvi.iAftTime,
		tRecEvi.iRecModel== eRecJoint?"ƴ��":"����",
		pData->iPicNum);
	for(int i = 0; i < pData->iPicNum; i++)
	{
		TimeFormatString(pData->tPicTime[i],tmpBuf,sizeof(tmpBuf),eYMDHMS1);
		printf("ͼƬ%d:%s.%03d\n",i+1,tmpBuf, pData->tPicMsec[i]);
	}

	for(int i = 0; i < tRecMission.iCnt; i++)
	{
		TimeFormatString(tRecMission.begin[i],tmpBuf,sizeof(tmpBuf),eYMDHMS1);
		printf("¼��%d:%s + %dsecs\n",i+1,tmpBuf,tRecMission.secs[i]);
	}

	// ����¼��
	return AnalyH264CacheMan::GetInstance()->GenRecEvidence(pData->iChnID, &tRecMission);
}

