
#include <stdio.h>
#include <string.h> 
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "UploadMan.h"
#include "oal_time.h"
#include "oal_file.h"
#include "mq_database.h"
#include "Protocol/Protocol_Factory.h"
#include "QueryDb.h"
#include "oal_log.h"
#include "oal_unicode.h"

void *UploadThread(void *arg);

UploadMan* UploadMan::m_pInstance = NULL;

UploadMan* UploadMan::GetInstance()
{
	return m_pInstance;
}

int UploadMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new UploadMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}


void UploadMan::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

UploadMan::UploadMan()
{
	pthread_mutex_init(&m_tMutex, NULL);

	pthread_mutex_lock(&m_tMutex);
	m_bEnable = false;
	memset(&m_tStrategy, 0, sizeof(m_tStrategy));
	memset(&m_tFtpAdvance, 0, sizeof(m_tFtpAdvance));
	pthread_mutex_unlock(&m_tMutex);
}

UploadMan::~UploadMan()
{
	m_bExitThread = true;

	pthread_join(m_tUploadTread,NULL);

	pthread_mutex_destroy(&m_tMutex);
}

int UploadMan::Run()
{
	m_bExitThread = false;
	if(pthread_create(&m_tUploadTread, NULL, UploadThread, NULL))
	{
		printf("Upload Data Thread create fail\n");
	}

	return 0;
}

int UploadMan::SetUploadStrategy(const UploadStrategy* ptInfo)
{
	if(ptInfo == NULL)
	{
		return -1;
	}
	
	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tStrategy, ptInfo, sizeof(UploadStrategy));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int UploadMan::SetUploadEnable(bool bEnable)
{
	pthread_mutex_lock(&m_tMutex);
	m_bEnable = bEnable;
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int UploadMan::SetUploadFtpAdv( const FtpAdvance* ptInfo )
{
	if(ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tFtpAdvance, ptInfo, sizeof(FtpAdvance));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int UploadMan::GetFtpDirGrammar( int iDataType, char * pszDir )
{
	if (pszDir == NULL)
	{
		return -1;
	}

	if(iDataType == eIllegaldata)
	{
		strcpy(pszDir, m_tFtpAdvance.szIllegalDirGrammar);
	}
	else if(iDataType == eEventData)
	{
		strcpy(pszDir, m_tFtpAdvance.szEventDirGrammar);
	}
	else if(iDataType == eTollData)
	{
		strcpy(pszDir, m_tFtpAdvance.szTollDirGrammar);
	}
	else
	{
		return -1;
	}
	return 0;
}

bool UploadMan::FtpEnableUploadFile( int iFlag )
{
	return (m_tFtpAdvance.iContentFlag&iFlag)==iFlag?true:false;
}

bool UploadMan::FtpUploadFileCharsetByUtf8()
{
	return (m_tFtpAdvance.iCharSet==eCharset_UTF8);
}

ProtocolHandle UploadMan::GetUploadProtocolHandle()
{
	// ����Э������ֵ
	int protocol = -1;

	pthread_mutex_lock(&m_tMutex);
	if(m_bEnable)
	{
		if(m_tStrategy.iModel == UPLOAD_TO_PLATFORM)
		{
			protocol = PROTOCOL_HTTP_NEW;// ƽ̨
		}
		else if(m_tStrategy.iModel == UPLOAD_TO_MQTT)
		{
			protocol = PROTOCOL_MQTT;// MQTT
		}
		else//FTP
		{
			protocol = PROTOCOL_FTP_NEW;// FTP
		}
	}
	pthread_mutex_unlock(&m_tMutex);

	// �ӹ������ɳ�Э����
	ProtocolHandle handle = NULL;
	if (protocol != -1)
	{
		handle = UploadProtocolFactory::Product(protocol);
	}

	return handle;
}

bool UploadMan::NeedRemoveFileAfterUpload()
{
	bool bNeed = false;	
	pthread_mutex_lock(&m_tMutex);
	bNeed = m_tStrategy.bDelOk;
	pthread_mutex_unlock(&m_tMutex);

	return bNeed;
}

int UploadMan::GetUploadServerFtp(FtpSvr* ptFtp)
{
	if(ptFtp == NULL)
	{
		return -1;
	}
	pthread_mutex_lock(&m_tMutex);
	memcpy(ptFtp, &(m_tStrategy.tFtpInfo),sizeof(FtpSvr));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

bool UploadMan::GetPlatformAndEnable( Platform *ptSvr )
{
	if(ptSvr == NULL)
	{
		return false;
	}
	bool bEnable = false;
	pthread_mutex_lock(&m_tMutex);
	bEnable = (m_bEnable && m_tStrategy.iModel==UPLOAD_TO_PLATFORM)?true:false;
	memcpy(ptSvr, &(m_tStrategy.tPlatform),sizeof(Platform));
	pthread_mutex_unlock(&m_tMutex);
	return bEnable;
}

bool UploadMan::GetMqttAndEnable( Platform *ptSvr )
{
	if(ptSvr == NULL)
	{
		return false;
	}
	bool bEnable = false;
	pthread_mutex_lock(&m_tMutex);
	bEnable = (m_bEnable && m_tStrategy.iModel==UPLOAD_TO_MQTT)?true:false;
	memcpy(ptSvr, &(m_tStrategy.tMqttSvr),sizeof(Platform));
	pthread_mutex_unlock(&m_tMutex);
	return bEnable;
}

static void RemoveRecordAllFile(AnalyDbRecord *pData)
{
	// ɾ��ͼƬ�ļ�
	for (int i = 0; i < pData->iImgNum; i++)
	{
		char szFullFIleUtf8[512] = {0};
		sprintf(szFullFIleUtf8, "%s%s", pData->szImgRoot, pData->szImgPath[i]);
		char szFullFIleGbk[512] = {0};
		UTF8_To_GB2312(szFullFIleUtf8, strlen(szFullFIleUtf8), szFullFIleGbk, sizeof(szFullFIleGbk));
		LOG_DEBUG_FMT("Delete File:%s", szFullFIleGbk);

		OAL_DeleteFile(szFullFIleGbk);
	}

	// ɾ��¼���ļ�
	for (int i = 0; i < pData->iRecNum; i++)
	{
		char szFullFIleUtf8[512] = {0};
		sprintf(szFullFIleUtf8, "%s%s", pData->szRecRoot, pData->szRecPath[i]);
		char szFullFIleGbk[512] = {0};
		UTF8_To_GB2312(szFullFIleUtf8, strlen(szFullFIleUtf8), szFullFIleGbk, sizeof(szFullFIleGbk));
		LOG_DEBUG_FMT("Delete File:%s", szFullFIleGbk);

		OAL_DeleteFile(szFullFIleGbk);
	}
}

void *UploadThread(void *arg)
{
	LOG_DEBUG_FMT("upload thread is running");

	while(!UploadMan::GetInstance()->m_bExitThread)
	{
		// ��ȡЭ����
		ProtocolHandle ptHandle = UploadMan::GetInstance()->GetUploadProtocolHandle();
		if(ptHandle == NULL)
		{
			// û��ָ��Э�飬���ϴ�
			sleep(1);
			continue;
		}

		if(!ptHandle->CanUploadData())
		{
			//�豸�����ϴ�����
			sleep(1);
			continue;
		}

		// �����ݿ��ȡ��¼
		AnalyDbRecord tEventData = {0};
		int iRet = GetOneNeedUploadRecord(&tEventData);
		if(iRet != 0)
		{
			sleep(1);
			continue;
		}

		// �ϴ�һ����¼
		iRet = ptHandle->UploadData(&tEventData);
		if(UPLOAD_RET_OK == iRet)
		{
			UpdataRecordStatus(tEventData.iRowID, eUpOk);
			LOG_DEBUG_FMT("The record %d upload,[OK]!",tEventData.iRowID);
		}
		else if(UPLOAD_RET_NO_FILE == iRet)
		{
			UpdataRecordStatus(tEventData.iRowID, eUpErr); 
			LOG_DEBUG_FMT("The record %d upload,[Error,No File]!",tEventData.iRowID);
		}
		else
		{
			LOG_DEBUG_FMT("The record %d upload,[Error]!",tEventData.iRowID);

			// ����ϴ�ʧ�ܣ����п����Ƿ������޷����ӣ�1���Ժ�����
			sleep(1);
		}

		// �ƺ���
		if(UPLOAD_RET_OK == iRet || UPLOAD_RET_NO_FILE == iRet)
		{
			if( ptHandle->CanRemoveFile() &&
				UploadMan::GetInstance()->NeedRemoveFileAfterUpload())
			{
				RemoveRecordAllFile(&tEventData);
			}
		}

		usleep(50000);
	}
	return NULL;
}






