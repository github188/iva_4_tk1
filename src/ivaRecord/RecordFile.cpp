#include "RecordFile.h"
#include "RecordMan.h"
#include "oal_time.h"
#include "oal_file.h"
#include "mq_database.h"
#include <assert.h>
#include "mq_master.h"

static int GetVideoH264Info(int iChnID, H264_INFO *ptInfo)
{
	if (ptInfo == NULL || iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return -1;
	}

	H264_Queue_Node * ptDataNode = (H264_Queue_Node*)malloc(sizeof(H264_Queue_Node));
	assert(ptDataNode);

	bool bFindSPS = false;
	bool bFindPPS = false;
	LOG_DEBUG_FMT("Channel%d Wait to find H264 info",iChnID);
	while(RecordMan::GetInstance()->GetChnRecStat(iChnID) == REC_GETING_INFO)
	{
		memset(ptDataNode, 0, sizeof(H264_Queue_Node));
		int iRet = RecordMan::GetInstance()->GetNewFrameData(iChnID, ptDataNode);
		//LOG_DEBUG_FMT("GetNewFrameData %d",iRet);
		if(iRet == 0)
		{
			// SPS
			if((*(ptDataNode->frame_data + 4)&0x1F) == 0x07)
			{
				ptInfo->iSpsSize = ptDataNode->h264_info.data_size - 4;
				memcpy(ptInfo->uSpsData, ptDataNode->frame_data + 4, ptInfo->iSpsSize);

				bFindSPS = true;
				LOG_INFOS_FMT("Channel%d find SPS info",iChnID);
			}

			// PPS
			if((*(ptDataNode->frame_data + 4)&0x1F) == 0x08)
			{
				ptInfo->iPpsSize = ptDataNode->h264_info.data_size - 4;
				memcpy(ptInfo->uPpsData, ptDataNode->frame_data + 4, ptInfo->iPpsSize);

				bFindPPS = true;
				LOG_INFOS_FMT("Channel%d find PPS info",iChnID);

				ptInfo->iFrameRate = ptDataNode->h264_info.frame_rate;
				ptInfo->iWidth     = ptDataNode->h264_info.width;
				ptInfo->iHeight    = ptDataNode->h264_info.height;
			}

			if (bFindPPS && bFindSPS)
			{
				break;
			}
		}		
	}

	free(ptDataNode);
	ptDataNode = NULL;

	LOG_INFOS_FMT("Channel%d find PPS %s, find SPS %s",iChnID,bFindPPS?"OK":"error",bFindSPS?"OK":"error");

	return ((bFindSPS && bFindPPS)?0:1);
}

void *RecordInitThread(void *arg)
{
	if (arg == NULL)
	{
		return NULL;
	}

	int iChnID = *((int *)arg);
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return NULL;
	}

	LOG_DEBUG_FMT("Channel%d Init Thread running",iChnID);

	while (!RecordMan::GetInstance()->m_bExitThread)
	{
		if (RecordMan::GetInstance()->GetChnRecStat(iChnID) == REC_NO_H264INFO)
		{
			// 开始获取PPS/SPS
			RecordMan::GetInstance()->SetChnRecStat(iChnID, REC_GETING_INFO);
			H264_INFO tInfo = {0};
			int iRet = GetVideoH264Info(iChnID, &tInfo);
			if (iRet == 0)
			{
				RecordMan::GetInstance()->SetH264Info(iChnID, &tInfo);
			}
			else
			{
				//RecordMan::GetInstance()->SetChnRecStat(iChnID, REC_NO_H264INFO);
			}
		}

		sleep(1);
	}

	return NULL;
}

void * FileCreateThread(void *arg)
{
	if (arg == NULL)
	{
		return NULL;
	}

	int iChnID = *((int *)arg);
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return NULL;
	}

	LOG_DEBUG_FMT("Channel%d Create Thread running",iChnID);

	while(!RecordMan::GetInstance()->m_bExitThread)
	{
		// 预先创建的文件
		RecordMan::GetInstance()->CreateNewFileNode(iChnID);

		sleep(1);
	}

	return NULL;
}

void * FileWriteThread(void *arg)
{
	if (arg == NULL)
	{
		return NULL;
	}

	int iChnID = *((int *)arg);
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return NULL;
	}

	H264_Queue_Node * ptDataNode = (H264_Queue_Node*)malloc(sizeof(H264_Queue_Node));
	assert(ptDataNode);

	LOG_DEBUG_FMT("Channel%d Write Thread running",iChnID);

	bool bEnableed = false;

	while(!RecordMan::GetInstance()->m_bExitThread)
	{
		bool bNewEnable = RecordMan::GetInstance()->GetEnable(iChnID);
		if (bNewEnable != bEnableed)
		{
			bEnableed = bNewEnable;
			MQ_Master_SetRecStatus(iChnID, bEnableed?REC_RUNNING:REC_STOP);
		}

		if (RecordMan::GetInstance()->GetEnable(iChnID) == false)
		{
			sleep(1);
			continue;
		}

		// 获取已经预先创建的文件
		RecFileNode* pFileNode = RecordMan::GetInstance()->GetNewFileNode(iChnID);
		if (pFileNode == NULL)
		{
			sleep(1);
			continue;
		}

		RecStrategy tStrategy = {0};
		RecordMan::GetInstance()->GetStrategy(tStrategy);
		if (tStrategy.iFileFormat != pFileNode->iFileFormat)
		{
			// 格式改变
			LOG_DEBUG_FMT("Channel%d File %s, FileFormat is wrong,delete",iChnID, pFileNode->szTempFileName);
			WR_ReleaseHandle(pFileNode->ptWriter);
			delete pFileNode;
			sleep(1);
			continue;
		}

		// 生成文件正式名称 记录开始时间
		time_t tNow = time(NULL);
		char  szNow[40] = {0};
		TimeFormatString(tNow, szNow, sizeof(szNow), eYMDHMS2);

		char  szYYYMMDD[40] = {0};
		TimeFormatString(tNow, szYYYMMDD, sizeof(szYYYMMDD), eYMD2);

		const char * pRPath = RecordMan::GetInstance()->GetRootPath();
		sprintf(pFileNode->szRealFileName, "%s%s/%s/%s_%d.%s", pRPath, RECORD_DIR,szYYYMMDD, szNow, iChnID,
			WR_FileSuffix(pFileNode->ptWriter));

		OAL_MakeDirByPath(pFileNode->szRealFileName);

		LOG_DEBUG_FMT("Channel%d Begin Write File %s",iChnID, pFileNode->szTempFileName);
		pFileNode->tStart = tNow;
		
		// 开始写文件
		int iWiteFrameCnt = 0;
		int iWriteSize = 0;
		int iNeedSize  = 200;
		int iRate = 0;
		do 
		{
			// 强制退出，因为用户关闭了录像功能
			if (RecordMan::GetInstance()->GetEnable(iChnID) == false)
			{
				LOG_INFOS_FMT("GetEnable(iChnID) == false break");
				break;
			}

			RecStrategy tStrategy = {0};
			RecordMan::GetInstance()->GetStrategy(tStrategy);
			if (tStrategy.iFileFormat != pFileNode->iFileFormat)
			{
				LOG_INFOS_FMT("tStrategy.iFileFormat != pFileNode->iFileFormat break", tStrategy.iFileFormat, pFileNode->iFileFormat);
				break;
			}

			// 取出一帧数据
			memset(ptDataNode, 0, sizeof(H264_Queue_Node));
			int iRet = RecordMan::GetInstance()->GetNewFrameData(iChnID, ptDataNode);
			if(iRet != 0)
			{
				usleep(10*1000);
				continue;
			}

			// 写入文件
			WR_WriteVideoSample(pFileNode->ptWriter, ptDataNode->frame_data, ptDataNode->h264_info.data_size,
				(ptDataNode->h264_info.frame_type==I_FRAME));
			iWiteFrameCnt++;

			//printf("Write Frame (%s) %u\n", (ptDataNode->h264_info.frame_type==I_FRAME)?"I":"P",ptDataNode->h264_info.data_size);

			if (tStrategy.iPacketMode == REC_PACKET_SIZE)
			{
				if (tStrategy.iPacketSize <= 0)
				{
					tStrategy.iPacketSize = 200;
				}
				iWriteSize += ptDataNode->h264_info.data_size;
				iNeedSize  = tStrategy.iPacketSize*1024*1024;
			}
			else
			{
				if (tStrategy.iPacketTime <= 0)
				{
					tStrategy.iPacketTime = 5;
				}

				if ((int)(time(NULL) - pFileNode->tStart) < iWriteSize)//处理时间被恶意向前调整
				{
					break;
				}

				iWriteSize = (int)(time(NULL) - pFileNode->tStart);
				iNeedSize  = tStrategy.iPacketTime*60;
			}

			int iiiR = (int)(((float)(iWriteSize)*1.0/(float)(iNeedSize))*100);
			if (iiiR != iRate && iiiR%5==0)
			{
				iRate = iiiR;
				LOG_DEBUG_FMT("Channel%d Write File %s %%%02d",iChnID, pFileNode->szTempFileName,iRate);
			}

		} while (iWriteSize < iNeedSize);

		if (iWriteSize == 0)
		{
			LOG_WARNS_FMT("Channel%d End Write File %s,no data，NeedSize=%d",iChnID, pFileNode->szTempFileName,iNeedSize);
			WR_ReleaseHandle(pFileNode->ptWriter);
			delete pFileNode;
			pFileNode = NULL;
		}
		else if (iWiteFrameCnt < 25)
		{
			LOG_WARNS_FMT("Channel%d End Write File %s,Too few frames to write，NeedSize=%d",iChnID, pFileNode->szTempFileName,iNeedSize);
			WR_ReleaseHandle(pFileNode->ptWriter);
			delete pFileNode;
			pFileNode = NULL;
		}
		else
		{
			// 文件结束 放入到关闭链表中
			pFileNode->tEnd = time(NULL);
			LOG_DEBUG_FMT("Channel%d End Write File %s",iChnID, pFileNode->szTempFileName);

			if (RecordMan::GetInstance()->PutCloseFileNode(iChnID,pFileNode) != 0)
			{
				LOG_DEBUG_FMT("Channel%d %s PutCloseFileNode failed",iChnID, pFileNode->szTempFileName);
				WR_CloseFile(pFileNode->ptWriter);
				WR_ReleaseHandle(pFileNode->ptWriter);
				delete pFileNode;
				pFileNode = NULL;
			}
		}
	}

	free(ptDataNode);
	ptDataNode = NULL;

	return NULL;
}

void * FileCloseThread(void *arg)
{
	if (arg == NULL)
	{
		return NULL;
	}

	int iChnID = *((int *)arg);
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return NULL;
	}

	LOG_DEBUG_FMT("Channel%d Close Thread running",iChnID);

	while(!RecordMan::GetInstance()->m_bExitThread)
	{
		// 获取已经预先创建的文件
		RecFileNode* pFileNode = RecordMan::GetInstance()->GetCloseFileNode(iChnID);
		if (pFileNode == NULL)
		{
			sleep(1);
			continue;
		}

		// 关闭文件
		if (pFileNode->ptWriter)
		{
			WR_CloseFile(pFileNode->ptWriter);
			WR_ReleaseHandle(pFileNode->ptWriter);
			pFileNode->ptWriter = NULL;
#ifndef WIN32
			sync();
#endif
		}

		// 从临时文件重命名为正式文件
		int imvRet = rename(pFileNode->szTempFileName, pFileNode->szRealFileName);
		if (imvRet != 0)
		{
			pFileNode->iTryCnt++;
			if (pFileNode->iTryCnt > 3)
			{
				LOG_DEBUG_FMT("Channel%d Close File %s to %s,rename=%d", iChnID, pFileNode->szTempFileName, pFileNode->szRealFileName,imvRet);
				delete pFileNode;
				pFileNode = NULL;
				continue;
			}
			else
			{
				// 多试几次
				if (RecordMan::GetInstance()->PutCloseFileNode(iChnID,pFileNode) != 0)
				{
					LOG_DEBUG_FMT("Channel%d Close File %s PutCloseFileNode failed", iChnID, pFileNode->szTempFileName);
					delete pFileNode;
					pFileNode = NULL;
					continue;
				}
			}
		}

		// 写入数据库
		char szInsertSql[1024] = {0};
		sprintf(szInsertSql, "insert into RecordTable"
			"(FILEPATH,BEGINTIME,ENDTIME,CHNID,TYPE,STATUS)"
			"values('%s',%lu,%lu,%d,%d,%d);",
			pFileNode->szRealFileName,
			pFileNode->tStart, pFileNode->tEnd,
			iChnID, 0, 0);

		LOG_DEBUG_FMT("sql: %s", szInsertSql);

		int iRet = MQ_DataBase_Insert(MSG_TYPE_RECORD, szInsertSql);
		if (iRet != 0)
		{
			LOG_ERROR_FMT("Insert to db failed");
		}

		delete pFileNode;
		pFileNode = NULL;
	}	
	return NULL;
}