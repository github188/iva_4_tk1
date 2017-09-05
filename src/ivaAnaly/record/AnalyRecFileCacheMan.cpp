#include <fcntl.h>
#include <sys/types.h>
#include "oal_time.h"
#include "oal_file.h"
#include <assert.h>
#include "AnalyH264CacheMan.h"
#include "AnalyRecFileCacheMan.h"

void *AlgRecFileTimeOutFxn(void* param)
{
	printf("AlgRecFileTimeOutFxn is Running...\n");
	
	while(AnalyRecFileCacheMan::GetInstance()->m_bExitThread == false)
	{
		sleep(5);// 5秒执行一次
		AnalyRecFileCacheMan::GetInstance()->CheckTimeOut();
	}
	
	pthread_exit(param);
	return NULL;
}

void *AlgRecFileDoMissionFxn(void* param)
{
	int iRet = -1;

	printf("AlgRecFileDoMissionFxn is Running...\n");

	while(AnalyRecFileCacheMan::GetInstance()->m_bExitThread == false)
	{
		RecFileMission * ptMisson = (RecFileMission*)QuePop(AnalyRecFileCacheMan::GetInstance()->m_ptMissionList,TIMEOUT_FOREVER);
		if(ptMisson)
		{
			RecMission tRM = {0};
			sprintf(tRM.szPathFile,"/tmp/rec%d_%d_%ld_%d.dat",
				ptMisson->iFileID, ptMisson->iChnID,
				ptMisson->begin, ptMisson->secs);

			tRM.iCnt = 1;
			tRM.begin[0] = ptMisson->begin;
			tRM.secs[0]  = ptMisson->secs;
			iRet = AnalyH264CacheMan::GetInstance()->GenRecFile(ptMisson->iChnID, &tRM);
			if(iRet == 0)
			{
				// 加入到文件管理链表
				RecFile * pNewRecFile = (RecFile *)malloc(sizeof(RecFile));
				if(pNewRecFile)
				{
					pNewRecFile->create_time = time(NULL);
					strcpy(pNewRecFile->path,tRM.szPathFile);
					memcpy(&(pNewRecFile->misson),ptMisson,sizeof(RecFileMission));
					pNewRecFile->next = NULL;

					if(AnalyRecFileCacheMan::GetInstance()->AddRecFile(pNewRecFile)!=0)
					{
						free(pNewRecFile);
						pNewRecFile = NULL;
						printf("============AddRecFile failed ID:%d begin:%ld+%d\n",ptMisson->iFileID,ptMisson->begin,ptMisson->secs);
					}
				}
				else
				{
					printf("============malloc failed ID:%d begin:%ld+%d\n",ptMisson->iFileID,ptMisson->begin,ptMisson->secs);
				}
			}
			else if(iRet == 1)
			{
				if(QuePush(AnalyRecFileCacheMan::GetInstance()->m_ptMissionList,ptMisson,TIMEOUT_FOREVER)!= 0)
				{
					free(ptMisson);
					ptMisson = NULL;
				}
				usleep(100*1000);
				continue;
			}
			else
			{
				printf("============GenRecFile failed ID:%d begin:%ld+%d\n",ptMisson->iFileID,ptMisson->begin,ptMisson->secs);
			}
			free(ptMisson);
			ptMisson = NULL;
		}
		else
		{
			usleep(10*1000);
		}
	}
	pthread_exit(param);
	return NULL;
}

AnalyRecFileCacheMan* AnalyRecFileCacheMan::m_pInstance = NULL;

AnalyRecFileCacheMan* AnalyRecFileCacheMan::GetInstance()
{
	return m_pInstance;
}

int AnalyRecFileCacheMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new AnalyRecFileCacheMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void AnalyRecFileCacheMan::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

AnalyRecFileCacheMan::AnalyRecFileCacheMan()
{
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_iTimeOutSecs[i] = 3600;// 默认一小时
	}
	
	pthread_mutex_init(&m_hndlSem, NULL);
	m_ptRecFilelist = NULL;
	m_ptMissionList = QueCreate(20);
	FreeAllRecFile();
}

int AnalyRecFileCacheMan::Run()
{
	m_bExitThread = false;
	if(pthread_create(&m_file_create_thread, NULL, AlgRecFileTimeOutFxn, NULL))
	{
		printf("AlgRecFileTimeOutFxn Thread create failed\n");
	}

	if(pthread_create(&m_file_timeout_thread, NULL, AlgRecFileDoMissionFxn, NULL))
	{
		printf("AlgRecFileDoMissionFxn Thread create failed\n");
	}
	return 0;
}

AnalyRecFileCacheMan::~AnalyRecFileCacheMan()
{
	m_bExitThread = true;
	
	pthread_join(m_file_create_thread,NULL);
	pthread_join(m_file_timeout_thread,NULL);
	
	// 释放未完成的任务
	RecFileMission * ptMssion = (RecFileMission *)QuePop(m_ptMissionList,TIMEOUT_FOREVER);
	while(ptMssion)
    {
		free(ptMssion);
		ptMssion = (RecFileMission *)QuePop(m_ptMissionList,TIMEOUT_FOREVER);
	}

	FreeAllRecFile();
}

void AnalyRecFileCacheMan::FreeAllRecFile()
{
	pthread_mutex_lock(&m_hndlSem);
	while(m_ptRecFilelist)
	{
		m_ptRecFilelist = m_ptRecFilelist->next;
		RecFile* pFile= m_ptRecFilelist;
		free(pFile);
		pFile = NULL;
	}
	pthread_mutex_unlock(&m_hndlSem);
	
	// 删除历史文件
	char cmd[128] = {0};
	sprintf(cmd,"rm -f /tmp/rec*.dat");
	system(cmd);
	//printf("%s\n" ,cmd);
}

int AnalyRecFileCacheMan::SetTimeOut(int iChnID, int iSecs)
{
	if (iChnID < 0 || iChnID > MAX_CHANNEL_NUM - 1)
	{
		return -1;
	}

	pthread_mutex_lock(&m_hndlSem);
	m_iTimeOutSecs[iChnID] = iSecs;
	pthread_mutex_unlock(&m_hndlSem);
	return 0;
}

int AnalyRecFileCacheMan::CreateRecFile(const RecFileMission &tMisson)
{
	RecFileMission * ptNewMisson = (RecFileMission *)malloc(sizeof(RecFileMission));
	if(ptNewMisson==NULL)
	{
		return -1;
	}

	memcpy(ptNewMisson, &tMisson, sizeof(RecFileMission));

	if(QuePush(m_ptMissionList, ptNewMisson, TIMEOUT_FOREVER)!= 0)
	{
		free(ptNewMisson);
		ptNewMisson = NULL;
		return -1;
	}

	return 0;
}

void AnalyRecFileCacheMan::CheckTimeOut()
{
	pthread_mutex_lock(&m_hndlSem);
	RecFile* pFile = m_ptRecFilelist;
	RecFile* pFileLast = NULL;
	time_t tNow = time(NULL);
	while(pFile)
	{
		int i = pFile->misson.iChnID;
		if (i < 0 || i > MAX_CHANNEL_NUM - 1)
		{
			i = 0;
		}

		if(tNow - pFile->create_time >  m_iTimeOutSecs[i])
		{
			LOG_DEBUG_FMT("Channel%d RecFile%d is deleted for been not used in %d secs",
				pFile->misson.iChnID,
				pFile->misson.iFileID,
				m_iTimeOutSecs[i]);
			
			RecFile* pFileDel = pFile;

			// 从链表中删除
			if(pFileLast == NULL)
				m_ptRecFilelist = pFile->next;
			else
				pFileLast->next = pFile->next;
			
			pFile = pFile->next;

			// 删除文件
			OAL_DeleteFile(pFileDel->path);

			// 删除节点
			free(pFileDel);
			pFileDel = NULL;
		}
		else
		{
			pFileLast = pFile;
			pFile = pFile->next;
		}
	}
	
	pthread_mutex_unlock(&m_hndlSem);
}

int AnalyRecFileCacheMan::AddRecFile(RecFile* pFile)
{
	if(pFile == NULL)
	{
		return -1;
	}
	
	pthread_mutex_lock(&m_hndlSem);
	pFile->next = m_ptRecFilelist;
	m_ptRecFilelist = pFile;
	pthread_mutex_unlock(&m_hndlSem);

	return 0;
}

int AnalyRecFileCacheMan::GetRecFile(int iChnID, int iID, RecFile* pFile)
{
	int iRet = -1;
	if(pFile == NULL)
	{
		return -1;
	}
	
	pthread_mutex_lock(&m_hndlSem);
	RecFile* pFileNode = m_ptRecFilelist;
	while(pFileNode)
	{
		if(pFileNode->misson.iFileID == iID && pFileNode->misson.iChnID == iChnID)
		{
			iRet = 0;
			printf("Find Channel%d RecFile%d ->%s\n", iChnID, iID, pFileNode->path);
			memcpy(pFile, pFileNode, sizeof(RecFile));
			break;
		}
	
		pFile = pFile->next;
	}
	pthread_mutex_unlock(&m_hndlSem);
	return iRet;
}


