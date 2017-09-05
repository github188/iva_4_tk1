#include <fcntl.h>
#include <sys/types.h>
#include <assert.h>
#include "AnalyH264CacheMan.h"
#include "oal_time.h"
#include "AnalyRecFileCacheMan.h"
#include "../AnalyEvidenceMan.h"
#include "RecordWriter.h"

#define MEM_BUF_SIZE 		(1024*1024*200)
#define RING_BUF_CNT		MEM_BUF_SIZE

int g_iChnID[MAX_CHANNEL_NUM] = {0};

static bool TimeIsInMission(time_t t, const RecMission * m)
{
	bool bIn = false;
	for(int i = 0; i < m->iCnt; i++)
	{
		if(t >= m->begin[i] && t <= (m->begin[i]+m->secs[i]))
		{
			bIn = true;
			break;
		}
	}
	return bIn;
}

// 双缓存，解决在生成MP4时无法继续接受流的问题
void *H264Cache2RingBufferThread(void* param)
{
	if (param == NULL)
	{
		return NULL;
	}
	int iChnID = *(int *)param;

	printf("Channel%d H264Cache2RingBufferThread is Running...\n", iChnID);

	while(AnalyH264CacheMan::GetInstance()->m_bExitThread == false)
	{
		AlgH264Frame * ptFrame = AnalyH264CacheMan::GetInstance()->PopFrameFromCache(iChnID);
		if(ptFrame)
		{
			int iRet = AnalyH264CacheMan::GetInstance()->WriteFrameToBuffer(iChnID, ptFrame);
			if(iRet != 0)
			{
				printf("============PutFrame failed.size=%d\n",ptFrame->frame_head.size);
			}
			free(ptFrame);
			ptFrame = NULL;
		}
		else
		{
			usleep(10*1000);
		}
	}
	pthread_exit(param);
	return NULL;
}


AnalyH264CacheMan* AnalyH264CacheMan::m_pInstance = NULL;

AnalyH264CacheMan* AnalyH264CacheMan::GetInstance()
{
	return m_pInstance;
}

int AnalyH264CacheMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new AnalyH264CacheMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void AnalyH264CacheMan::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

AnalyH264CacheMan::AnalyH264CacheMan()
{
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		pthread_mutex_init(&m_tRingBuf[i].m_hndlSem,NULL);
		m_tRingBuf[i].m_t1 = time(NULL);
		m_tRingBuf[i].m_t2 = time(NULL);
		m_tRingBuf[i].m_pos_r = -1;
		m_tRingBuf[i].m_pos_w = 0;
		m_tRingBuf[i].m_pData = (unsigned char *)malloc(RING_BUF_CNT);
		assert(m_tRingBuf[i].m_pData);
		memset(m_tRingBuf[i].m_pData, 0, RING_BUF_CNT);

		m_tH264CacheList[i] = QueCreate(1000);
	}
}

AnalyH264CacheMan::~AnalyH264CacheMan()
{
	m_bExitThread = true;
	
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		pthread_join(m_H264Cache_thread[i], NULL);

		AlgH264Frame * ptFrame = PopFrameFromCache(i);
		while(ptFrame)
		{
			free(ptFrame);
			ptFrame = PopFrameFromCache(i);
		}
		QueDelete(m_tH264CacheList[i]);
		m_tH264CacheList[i] = NULL;

		// 释放一级缓存的残留
		pthread_mutex_lock(&m_tRingBuf[i].m_hndlSem);
		if(m_tRingBuf[i].m_pData)
		{
			free(m_tRingBuf[i].m_pData);
			m_tRingBuf[i].m_pData = NULL;
		}
		pthread_mutex_unlock(&m_tRingBuf[i].m_hndlSem);
		pthread_mutex_destroy(&m_tRingBuf[i].m_hndlSem);
	}
}

int AnalyH264CacheMan::Run()
{
	m_bExitThread = false;
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		g_iChnID[i] = i;
		if(pthread_create(&m_H264Cache_thread[i], NULL, H264Cache2RingBufferThread, &g_iChnID[i]))
		{
			printf("H264Cache2RingBuffer Thread create failed\n");
		}
	}

	return 0;
}

int AnalyH264CacheMan::GenRecEvidence( int iChnID, RecMission * ptMission )
{
	time_t tFind1=0,tFind2=0;
	int framecnt = 0;
	char buff1[100]={0},buff2[100]={0};
	
	if(ptMission == NULL || iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	time_t t1 = ptMission->begin[0];
	time_t t2 = ptMission->begin[ptMission->iCnt-1] + ptMission->secs[ptMission->iCnt-1];
	TimeFormatString(ptMission->begin[0],buff1,sizeof(buff1), eYMDHMS1);
	TimeFormatString(ptMission->begin[0]+ptMission->secs[0],buff2,sizeof(buff2),eYMDHMS1);
	LOG_DEBUG_FMT("[GEN  REC] from %s to %s,total %d secs",buff1,buff2,ptMission->secs[0]);
	if(ptMission->iCnt > 1)
	{
		TimeFormatString(ptMission->begin[1],buff1,sizeof(buff1),eYMDHMS1);
		TimeFormatString(ptMission->begin[1]+ptMission->secs[1],buff2,sizeof(buff2),eYMDHMS1);
		LOG_DEBUG_FMT("---------- from %s to %s,total %d secs",buff1,buff2,ptMission->secs[1]);
	}
	time_t m_t1 = m_tRingBuf[iChnID].m_t1;
	time_t m_t2 = m_tRingBuf[iChnID].m_t2;
	TimeFormatString(m_t1,buff1,sizeof(buff1), eYMDHMS1);
	TimeFormatString(m_t2,buff2,sizeof(buff2),eYMDHMS1);
	LOG_DEBUG_FMT("[RING BUF] Channel%d from %s to %s,total %dm:%ds", iChnID, buff1, buff2,(int)(m_t2-m_t1)/60,(int)(m_t2-m_t1)%60);

	if(t2 > m_t2)
	{
		if(time(NULL) > t2+10)
		{
			// 现在时刻已经过了结束时间10秒了，buffer还是不够，不好意思只能这样了，将就录点了
			LOG_WARNS_FMT("[BUF WAIT] %ds, but can not wait any more",(int)(t2-m_t2));
		}
		else
		{
			LOG_DEBUG_FMT("[BUF WAIT] %ds\n",(int)(t2-m_t2));
			return 1;// 等一会，缓存时间还没有到
		}
	}

	WRHandle pVideoWriter = WR_NewHandle(ptMission->iFileFormat);
	if(pVideoWriter == NULL)
	{
		LOG_ERROR_FMT("GetSuitableWriter failed.................");
		return -1;
	}
	
	int iRet = WR_OpenFile(pVideoWriter, ptMission->szPathFile);
	if(iRet != 0)
	{
		LOG_ERROR_FMT("VideoWriter createFile %s failed", ptMission->szPathFile);
		WR_ReleaseHandle(pVideoWriter);
		pVideoWriter = NULL;
		return -1;
	}
	//LOG_ERROR_FMT("VideoWriter createFile %s", ptMission->szPathFile);
	
	// 从环形缓存读取帧数据
	pthread_mutex_lock(&m_tRingBuf[iChnID].m_hndlSem);
	
	bool bWriteHeader = false;

	// 如果第一时间段的录像不在，从file中获取
	if(ptMission->iCnt > 1 && ptMission->begin[0]+ptMission->secs[0] < m_t1)
	{
		RecFile tRecFile = {0};
		iRet = AnalyRecFileCacheMan::GetInstance()->GetRecFile(iChnID, ptMission->iID, &tRecFile);
		if(iRet == 0)
		{
			bWriteHeader = GenRecEvidenceByRecFile(iChnID, tRecFile.path, pVideoWriter);
		}
	}

	long lReadPos = m_tRingBuf[iChnID].m_pos_r;
	while(1)
	{
		AlgH264Frame tFrame = {0};
		if(GetFrameHead(iChnID, &(tFrame.frame_head),lReadPos) != 0)
		{
			break;
		}

		int iNeedSize = sizeof(FrameHead)+tFrame.frame_head.size;
		if(tFrame.frame_head.t > t2)
		{
			break;
		}
		else if(TimeIsInMission(tFrame.frame_head.t, ptMission))
		{
			// 满足条件的帧，读取出完整数据
			int iTailSize = RING_BUF_CNT - lReadPos;
			if(iTailSize < iNeedSize)
			{
				memcpy(&tFrame, m_tRingBuf[iChnID].m_pData+lReadPos, iTailSize);
				memcpy(((unsigned char*)(&tFrame))+iTailSize, m_tRingBuf[iChnID].m_pData, iNeedSize-iTailSize);
			}
			else
			{
				memcpy(&tFrame, m_tRingBuf[iChnID].m_pData+lReadPos, iNeedSize);
			}

			if(tFind1==0)
			{
				tFind1 = tFrame.frame_head.t;
			}
			tFind2 = tFrame.frame_head.t;

			// 下一帧的位置
			lReadPos += iNeedSize;
			if(lReadPos >= RING_BUF_CNT)
			{
				lReadPos -= RING_BUF_CNT;
			}
	
			if(!bWriteHeader)
			{
				if((*(tFrame.frame_data + 4)&0x1F) == 0x07)
				{
					// 读取接下来的一帧，以便获得SPS PPS
					AlgH264Frame tFrame2 = {0};
					if(GetFrameHead(iChnID, &(tFrame2.frame_head),lReadPos) != 0)
					{
						break;
					}

					int iNeedSize2 = sizeof(FrameHead)+tFrame2.frame_head.size;
					int iTailSize2 = RING_BUF_CNT - lReadPos;
					if(iTailSize2 < iNeedSize2)
					{
						memcpy(&tFrame2, m_tRingBuf[iChnID].m_pData+lReadPos, iTailSize2);
						memcpy(((unsigned char*)(&tFrame2))+iTailSize2, m_tRingBuf[iChnID].m_pData, iNeedSize2-iTailSize2);
					}
					else
					{
						memcpy(&tFrame2, m_tRingBuf[iChnID].m_pData+lReadPos, iNeedSize2);
					}

					// 下一帧的位置
					lReadPos += iNeedSize2;
					if(lReadPos >= RING_BUF_CNT)
					{
						lReadPos -= RING_BUF_CNT;
					}

					if((*(tFrame2.frame_data + 4)&0x1F) == 0x08)
					{
						framecnt = 2;
						
						int iFrameRate, iFrameWidth, iFrameHeight;
						AnalyEvidenceMan::GetInstance()->GetFrameInfo(iChnID, iFrameRate, iFrameWidth, iFrameHeight);
						//printf("iFrameRate=%d, iFrameWidth=%d, iFrameHeight=%d\n",iFrameRate, iFrameWidth, iFrameHeight);
						
						WR_SetVideoTrack(pVideoWriter,tFrame.frame_data+4,tFrame.frame_head.size-4, tFrame2.frame_data+4, tFrame2.frame_head.size-4, iFrameWidth, iFrameHeight, iFrameRate);
						WR_WriteVideoSample(pVideoWriter,tFrame.frame_data, tFrame.frame_head.size, (tFrame.frame_head.frame_type==I_FRAME));
						WR_WriteVideoSample(pVideoWriter,tFrame2.frame_data, tFrame2.frame_head.size, (tFrame2.frame_head.frame_type==I_FRAME));
						
						bWriteHeader = true;
					}
				}
			}
			else
			{
				framecnt++;
				WR_WriteVideoSample(pVideoWriter,(tFrame.frame_data),tFrame.frame_head.size, tFrame.frame_head.frame_type==I_FRAME?true:false);
			}
		}
		else
		{
			// 下一帧的位置
			lReadPos += iNeedSize;
			if(lReadPos >= RING_BUF_CNT)
			{
				lReadPos -= RING_BUF_CNT;
			}
		}
	}
	pthread_mutex_unlock(&m_tRingBuf[iChnID].m_hndlSem);

	TimeFormatString(tFind1, buff1, sizeof(buff1), eYMDHMS1);
	TimeFormatString(tFind2, buff2, sizeof(buff2), eYMDHMS1);
	if (tFind2 == tFind1) tFind2 = tFind1 + 1;
	LOG_DEBUG_FMT("[GEN  REC] Done. framecnt=%d %dSecs %dFPS from %s to %s",framecnt,(int)(tFind2-tFind1),framecnt/(int)(tFind2-tFind1),buff1, buff2);

	WR_CloseFile(pVideoWriter);
	WR_ReleaseHandle(pVideoWriter);
	pVideoWriter = NULL;

	sleep(1);

	/*if(framecnt < 7 || tFind1 == 0 || tFind2-tFind1 < 3)
	{
		printf("[GEN  REC]Failed.Buffer is not Enough\n\n");

		//删除不成功的文件
		char cmd[128];
		memset(cmd,'\0',sizeof(cmd));
		sprintf(cmd,"rm -f %s", ptMission->szPathFile);
		system(cmd);
		printf("%s\n" ,cmd);
		return -1;
	}
	else*/
	{
		printf("[GEN  REC] %s OK!\n\n",ptMission->szPathFile);
		return 0;
	}
}

int AnalyH264CacheMan::GenRecFile(int iChnID, const RecMission * ptMission)
{
	time_t tFind1=0,tFind2=0;
	int framecnt = 0;
	char buff1[100]={0},buff2[100]={0};

	if(ptMission == NULL || iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	time_t t1 = ptMission->begin[0];
	time_t t2 = ptMission->begin[0] + ptMission->secs[0];
	TimeFormatString(ptMission->begin[0],buff1,sizeof(buff1), eYMDHMS1);
	TimeFormatString(ptMission->begin[0]+ptMission->secs[0],buff2,sizeof(buff2),eYMDHMS1);
	LOG_DEBUG_FMT("[GEN  REC] from %s to %s,total %d secs",buff1,buff2,ptMission->secs[0]);
	if(ptMission->iCnt > 1)
	{
		TimeFormatString(ptMission->begin[1],buff1,sizeof(buff1),eYMDHMS1);
		TimeFormatString(ptMission->begin[1]+ptMission->secs[1],buff2,sizeof(buff2),eYMDHMS1);
		LOG_DEBUG_FMT("---------- from %s to %s,total %d secs",buff1,buff2,ptMission->secs[1]);
	}

	time_t m_t1 = m_tRingBuf[iChnID].m_t1;
	time_t m_t2 = m_tRingBuf[iChnID].m_t2;
	TimeFormatString(m_t1, buff1, sizeof(buff1), eYMDHMS1);
	TimeFormatString(m_t2, buff2, sizeof(buff2), eYMDHMS1);
	LOG_DEBUG_FMT("[RING BUF] Channel%d from %s to %s,total %dm:%ds", iChnID, buff1, buff2,(int)(m_t2-m_t1)/60,(int)(m_t2-m_t1)%60);

	if(t2 > m_t2)
	{
		//================防止在延后录像时，由于视频流暂时断流引起的无限等待
		if(time(NULL) > t2+10)
		{
			// 现在时刻已经过了结束时间10秒了，buffer还是不够，不好意思只能这样了，将就录点了
			LOG_WARNS_FMT("[BUF WAIT] %ds, but can not wait any more",(int)(t2-m_t2));
		}
		else
			//================end
		{
			LOG_DEBUG_FMT("[BUF WAIT] %ds\n",(int)(t2-m_t2));
			return 1;// 等一会，缓存时间还没有到
		}
	}

	FILE *fp = fopen(ptMission->szPathFile, "wb");
	if (NULL == fp)
	{
		LOG_ERROR_FMT("open %s for writing failed.", ptMission->szPathFile);
		return -1;
	}

	// 从环形缓存读取帧数据
	pthread_mutex_lock(&m_tRingBuf[iChnID].m_hndlSem);

	bool bWriteHeader = false;
	long lReadPos = m_tRingBuf[iChnID].m_pos_r;
	while(1)
	{
		AlgH264Frame tFrame = {0};
		if(GetFrameHead(iChnID, &(tFrame.frame_head),lReadPos) != 0)
		{
			break;
		}

		int iNeedSize = sizeof(FrameHead)+tFrame.frame_head.size;
		if(tFrame.frame_head.t > t2)
		{
			break;
		}
		else if(TimeIsInMission(tFrame.frame_head.t, ptMission))
		{
			// 满足条件的帧，读取出完整数据
			int iTailSize = RING_BUF_CNT - lReadPos;
			if(iTailSize < iNeedSize)
			{
				memcpy(&tFrame, m_tRingBuf[iChnID].m_pData+lReadPos, iTailSize);
				memcpy(((unsigned char*)(&tFrame))+iTailSize, m_tRingBuf[iChnID].m_pData, iNeedSize-iTailSize);
			}
			else
			{
				memcpy(&tFrame, m_tRingBuf[iChnID].m_pData+lReadPos, iNeedSize);
			}

			// 下一帧的位置
			lReadPos += iNeedSize;
			if(lReadPos >= RING_BUF_CNT)
			{
				lReadPos -= RING_BUF_CNT;
			}

			if(!bWriteHeader)
			{
				if((*(tFrame.frame_data + 4)&0x1F) == 0x07)
				{
					// 读取接下来的一帧，以便获得SPS PPS
					AlgH264Frame tFrame2 = {0};
					if(GetFrameHead(iChnID, &(tFrame2.frame_head),lReadPos) != 0)
					{
						break;
					}

					int iNeedSize2 = sizeof(FrameHead)+tFrame2.frame_head.size;
					int iTailSize2 = RING_BUF_CNT - lReadPos;
					if(iTailSize2 < iNeedSize2)
					{
						memcpy(&tFrame2, m_tRingBuf[iChnID].m_pData+lReadPos, iTailSize2);
						memcpy(((unsigned char*)(&tFrame2))+iTailSize2, m_tRingBuf[iChnID].m_pData, iNeedSize2-iTailSize2);
					}
					else
					{
						memcpy(&tFrame2, m_tRingBuf[iChnID].m_pData+lReadPos, iNeedSize2);
					}

					// 下一帧的位置
					lReadPos += iNeedSize2;
					if(lReadPos >= RING_BUF_CNT)
					{
						lReadPos -= RING_BUF_CNT;
					}

					if((*(tFrame2.frame_data + 4)&0x1F) == 0x08)
					{
						tFind1 = tFrame2.frame_head.t;
						framecnt = 2;

						fwrite(&(tFrame.frame_head),sizeof(FrameHead),1,fp);
						fwrite(tFrame.frame_data,tFrame.frame_head.size,1,fp);
						fflush(fp);

						fwrite(&(tFrame2.frame_head),sizeof(FrameHead),1,fp);
						fwrite(tFrame2.frame_data,tFrame2.frame_head.size,1,fp);
						fflush(fp);

						bWriteHeader = true;
					}
				}
			}
			else
			{
				framecnt++;
				fwrite(&(tFrame.frame_head),sizeof(FrameHead),1,fp);
				fwrite(tFrame.frame_data,tFrame.frame_head.size,1,fp);
				fflush(fp);
				tFind2 = tFrame.frame_head.t;
			}
		}
		else
		{
			// 下一帧的位置
			lReadPos += iNeedSize;
			if(lReadPos >= RING_BUF_CNT)
			{
				lReadPos -= RING_BUF_CNT;
			}
		}
	}
	pthread_mutex_unlock(&m_tRingBuf[iChnID].m_hndlSem);

	TimeFormatString(tFind1,buff1,sizeof(buff1),eYMDHMS1);
	TimeFormatString(tFind2,buff2,sizeof(buff2),eYMDHMS1);
	LOG_DEBUG_FMT("[REC FILE] Done. framecnt=%d %dSecs from %s to %s",framecnt,(int)(tFind2-tFind1), buff1, buff2);

	fclose(fp);
	fp = NULL;

	LOG_DEBUG_FMT("[REC FILE] %s OK!\n",ptMission->szPathFile);
	return 0;
}

bool AnalyH264CacheMan::GenRecEvidenceByRecFile(int iChnID, const char * recfile, WRHandle pVideoWriter)
{
	if(recfile == NULL || pVideoWriter == NULL)
	{
		return false;
	}

	FILE *fp = fopen(recfile, "rb");
	if (NULL == fp)
	{
		printf("open %s for reading failed.\n", recfile);
		return false;
	}

	int cnt = 0;
	time_t t1=0, t2=0;
	bool bWriteHeader = false;
	while (1)
	{
		AlgH264Frame tFrame = {0};
		size_t size_head = fread ( &(tFrame.frame_head), sizeof(FrameHead), 1, fp) ;
		if(size_head != 1)
		{
			break;
		}

		size_t size_data = fread ( &(tFrame.frame_data), tFrame.frame_head.size, 1, fp) ;
		if(size_data != 1)
		{
			break;
		}

		if(t1 == 0)
		{
			t1=tFrame.frame_head.t;
		}
		t2 = tFrame.frame_head.t;

		cnt++;
		if(!bWriteHeader)
		{
			if((*(tFrame.frame_data + 4)&0x1F) == 0x07)
			{
				// 读取接下来的一帧，以便获得SPS PPS
				AlgH264Frame tFrame2 = {0};
				size_t size_head2 = fread ( &(tFrame2.frame_head), sizeof(FrameHead), 1, fp) ;
				if(size_head2 != 1)
				{
					break;
				}

				size_t size_data2 = fread ( &(tFrame2.frame_data), tFrame2.frame_head.size, 1, fp) ;
				if(size_data2 != 1)
				{
					break;
				}

				if((*(tFrame2.frame_data + 4)&0x1F)== 0x08)
				{
					int iFrameRate, iFrameWidth, iFrameHeight;
					AnalyEvidenceMan::GetInstance()->GetFrameInfo(iChnID, iFrameRate, iFrameWidth, iFrameHeight);
					WR_SetVideoTrack(pVideoWriter, (tFrame.frame_data)+4,tFrame.frame_head.size-4,(tFrame2.frame_data)+4, tFrame2.frame_head.size-4, iFrameWidth, iFrameHeight, iFrameRate);
					WR_WriteVideoSample(pVideoWriter, (tFrame.frame_data),  tFrame.frame_head.size, (tFrame.frame_head.frame_type==I_FRAME));
					WR_WriteVideoSample(pVideoWriter, (tFrame2.frame_data), tFrame2.frame_head.size, (tFrame2.frame_head.frame_type==I_FRAME));

					bWriteHeader = true;
				}
			}
		}
		else
		{
			WR_WriteVideoSample(pVideoWriter,(tFrame.frame_data),tFrame.frame_head.size, tFrame.frame_head.frame_type==I_FRAME?true:false);
		}
	}

	fclose(fp);
	fp = NULL;

	char buff1[50],buff2[50];
	TimeFormatString(t1,buff1,sizeof(buff1),eYMDHMS1);
	TimeFormatString(t2,buff2,sizeof(buff2),eYMDHMS1);
	printf("[GenRecEvidenceByRecFile] Done. framecnt=%d %dSecs from %s to %s\n",cnt,(int)(t2-t1),buff1,buff2);

	return bWriteHeader;
}

int AnalyH264CacheMan::PushFrameToCache( int iChnID, AlgH264Frame * ptFrame )
{
	if(ptFrame == NULL || iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	return QuePush(m_tH264CacheList[iChnID], ptFrame, TIMEOUT_NONE);
}

AlgH264Frame * AnalyH264CacheMan::PopFrameFromCache( int iChnID )
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return NULL;
	}

	return (AlgH264Frame*)QuePop(m_tH264CacheList[iChnID],TIMEOUT_NONE);
}

int AnalyH264CacheMan::WriteFrameToBuffer(int iChnID, AlgH264Frame * ptFrame)
{
	if(iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1 || ptFrame == NULL)
	{
		return -1;
	}

	if( m_tRingBuf[iChnID].m_pData == NULL || ptFrame->frame_head.size > sizeof(ptFrame->frame_data))
	{
		return -1;
	}

	pthread_mutex_lock(&m_tRingBuf[iChnID].m_hndlSem);

	// 准备写空间
	int iNeedSize = sizeof(FrameHead) + ptFrame->frame_head.size;
	if(PrepareWriteSize(iChnID, iNeedSize) != 0)
	{
		pthread_mutex_unlock(&m_tRingBuf[iChnID].m_hndlSem);
		return -1;
	}

	// 写入
	long iTailSize = RING_BUF_CNT - m_tRingBuf[iChnID].m_pos_w;
	if(iTailSize < iNeedSize)
	{
		memcpy(m_tRingBuf[iChnID].m_pData + m_tRingBuf[iChnID].m_pos_w, ptFrame, iTailSize);
		memcpy(m_tRingBuf[iChnID].m_pData, ((unsigned char*)ptFrame)+iTailSize, iNeedSize-iTailSize);
	}
	else
	{
		memcpy(m_tRingBuf[iChnID].m_pData+m_tRingBuf[iChnID].m_pos_w, ptFrame, iNeedSize);
	}
	
	char buff[40] = {0};
	if(m_tRingBuf[iChnID].m_pos_r < 0)
	{
		m_tRingBuf[iChnID].m_t1    = ptFrame->frame_head.t;
		m_tRingBuf[iChnID].m_pos_r = m_tRingBuf[iChnID].m_pos_w;// 应该只在第一针写入以后赋值一次
	}

	m_tRingBuf[iChnID].m_pos_w += iNeedSize;
	if(m_tRingBuf[iChnID].m_pos_w >= RING_BUF_CNT)
	{
		m_tRingBuf[iChnID].m_pos_w -= RING_BUF_CNT;
	}

	m_tRingBuf[iChnID].m_t2 = ptFrame->frame_head.t;
#if 0
	char buff1[40] = {0};
	char buff2[40] = {0};
	TimeFormatString(m_tRingBuf[iChnID].m_t1, buff1, 40, eYMDHMS2);
	TimeFormatString(m_tRingBuf[iChnID].m_t2, buff2, 40, eYMDHMS2);

	printf("[Chn%d RING BUF] from %s to %s,cache %dsecs ",iChnID, buff1, buff2,(int)(m_tRingBuf[iChnID].m_t2-m_tRingBuf[iChnID].m_t1));

	if(m_tRingBuf[iChnID].m_pos_r < m_tRingBuf[iChnID].m_pos_w)
	{
		printf("total %.2fM used %.2fM free %.2fM\n",RING_BUF_CNT/1024.0/1024.0,(m_tRingBuf[iChnID].m_pos_w-m_tRingBuf[iChnID].m_pos_r)/1024.0/1024.0,(RING_BUF_CNT-m_tRingBuf[iChnID].m_pos_w+m_tRingBuf[iChnID].m_pos_r)/1024.0/1024.0);
	}
	else
	{
		printf("total %.2fM  Used %.2fM Free %.2fM\n",RING_BUF_CNT/1024.0/1024.0,(RING_BUF_CNT-m_tRingBuf[iChnID].m_pos_r+m_tRingBuf[iChnID].m_pos_w)/1024.0/1024.0,(m_tRingBuf[iChnID].m_pos_r-m_tRingBuf[iChnID].m_pos_w)/1024.0/1024.0);
	}

#endif

	pthread_mutex_unlock(&m_tRingBuf[iChnID].m_hndlSem);
	return 0;
}

int AnalyH264CacheMan::GetFrameHead(int iChnID,FrameHead* ptFrameHead, long lReadPos)
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	if(ptFrameHead == NULL || m_tRingBuf[iChnID].m_pData == NULL || m_tRingBuf[iChnID].m_pos_r < 0 || lReadPos < 0 || lReadPos >= RING_BUF_CNT)
	{
		return -1;
	}
	
	if(m_tRingBuf[iChnID].m_pos_r < m_tRingBuf[iChnID].m_pos_w)
	{
		if(lReadPos < m_tRingBuf[iChnID].m_pos_r || lReadPos >= m_tRingBuf[iChnID].m_pos_w)
		{
			return -1;// 不在可读范围
		}
	}
	else
	{
		if(lReadPos < m_tRingBuf[iChnID].m_pos_r && lReadPos >= m_tRingBuf[iChnID].m_pos_w)
		{
			return -1;// 不在可读范围
		}
	}

	// 读取头部
	int iNeedSize = sizeof(FrameHead);
	int iTailSize = RING_BUF_CNT - lReadPos;
	if(iTailSize < iNeedSize)
	{
		memcpy(ptFrameHead, m_tRingBuf[iChnID].m_pData+lReadPos, iTailSize);
		memcpy(((unsigned char*)ptFrameHead)+iTailSize, m_tRingBuf[iChnID].m_pData, iNeedSize-iTailSize);
	}
	else
	{
		memcpy(ptFrameHead, m_tRingBuf[iChnID].m_pData+lReadPos, iNeedSize);
	}

	return 0;
}

int AnalyH264CacheMan::MoveReadPos(int iChnID, long lSize)
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	//必然会成功
	int iMovedSize = 0;
	FrameHead tFrameHead = {0};
	while(iMovedSize < lSize)
	{
		memset(&tFrameHead, 0, sizeof(FrameHead));
		int iRet = GetFrameHead(iChnID, &tFrameHead, m_tRingBuf[iChnID].m_pos_r);
		if(iRet != 0)
		{
			printf("正常运行的话，你不应该看到这句打印\n");
			return -1;
		}

		// 移动读指针
		int iFrameSize = sizeof(FrameHead) + tFrameHead.size;
		m_tRingBuf[iChnID].m_pos_r += iFrameSize;
		if(m_tRingBuf[iChnID].m_pos_r >= RING_BUF_CNT)
		{
			m_tRingBuf[iChnID].m_pos_r -= RING_BUF_CNT;
		}
		
		m_tRingBuf[iChnID].m_t1 = tFrameHead.t;

		iMovedSize += iFrameSize;
	}

	return 0;
}

int AnalyH264CacheMan::PrepareWriteSize(int iChnID, long lSize)
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM-1)
	{
		return -1;
	}

	int iRet = -1;
	
	// 在读写间留一个空白
	long lNeedSize = lSize + sizeof(FrameHead);
	
	if(lNeedSize >= RING_BUF_CNT)
	{
		return -1;
	}

	if(m_tRingBuf[iChnID].m_pos_r < 0)
	{
		return 0;
	}

	long lFreeSize = 0;

	if(m_tRingBuf[iChnID].m_pos_r < m_tRingBuf[iChnID].m_pos_w)
	{
		lFreeSize = (RING_BUF_CNT - m_tRingBuf[iChnID].m_pos_w) + (m_tRingBuf[iChnID].m_pos_r - 0);
	}
	else
	{
		lFreeSize = m_tRingBuf[iChnID].m_pos_r - m_tRingBuf[iChnID].m_pos_w;
	}

	
	if(lFreeSize >= lNeedSize)
	{
		return 0;// 空间充足
	}
	else
	{
		// 空间不足，需要覆盖掉旧数据
		long lNeedRelaseSize = lNeedSize - lFreeSize;
		return MoveReadPos(iChnID, lNeedRelaseSize);
	}
}

