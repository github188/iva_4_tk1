#include "RecordMan.h"
#include <assert.h>
#include "oal_log.h"
#include "oal_time.h"
#include "RecordFile.h"
#include "oal_file.h"

static int s_iChnID[MAX_CHANNEL_NUM];

RecordMan* RecordMan::m_pInstance = NULL;

RecordMan* RecordMan::GetInstance()
{
	return m_pInstance;
}

int RecordMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new RecordMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void RecordMan::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

RecordMan::RecordMan()
{
	pthread_mutex_init(&m_tMutex, NULL);
	m_tAuthStat.eAuthType = eProhibit;
	m_tAuthStat.tTimeOut  = 0;

	m_tStrategy.iFileFormat = REC_FILE_MP4;
	m_tStrategy.iPacketMode = REC_PACKET_SIZE;
	m_tStrategy.iPacketSize = 50;
	m_tStrategy.iPacketTime = 5;
	strcpy(m_szRootPath, DEFAULT_ROOT_PATH);
	m_tFull.bFull = true;
	m_tFull.iHow2DO = eFull2StopSave;

	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_bEnable[i] = false;
		memset(m_tPlans+i, 0, sizeof(RecPlan));

		m_iStatus[i] = REC_UNINITIALIZED;
		memset(m_tH264Info+i, 0, sizeof(H264_INFO));

		m_hdlOpenFileQue[i]  = QueCreate(10);
		m_hdlCloseFileQue[i] = QueCreate(10);
	}
	m_bExitThread = false;

	// 清空临时文件夹
	char cmd[128] = {0};
	sprintf(cmd, "rm -fr %s%s/%s/*", MOUNT_POINT_ROM, IDENTIFY_DIR, RECORD_TEMP_DIR);
	system(cmd);

	sprintf(cmd, "rm -fr %s/%s/%s/*", MOUNT_POINT_USB, IDENTIFY_DIR, RECORD_TEMP_DIR);
	system(cmd);

	sprintf(cmd, "rm -fr %s/%s/%s/*", MOUNT_POINT_HDD, IDENTIFY_DIR, RECORD_TEMP_DIR);
	system(cmd);

	sprintf(cmd, "rm -fr %s/%s/%s/*", MOUNT_POINT_SD, IDENTIFY_DIR, RECORD_TEMP_DIR);
	system(cmd);
}

RecordMan::~RecordMan()
{
	m_bExitThread = true;

	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		pthread_join(m_init_thread[i],NULL);
		pthread_join(m_create_thread[i],NULL);
		pthread_join(m_write_thread[i],NULL);
		pthread_join(m_close_thread[i],NULL);

		// 清空已经创建的临时文件
		RecFileNode* pFileNode = RecordMan::GetInstance()->GetNewFileNode(i);
		while (pFileNode)
		{
			WR_CloseFile(pFileNode->ptWriter);
			WR_ReleaseHandle(pFileNode->ptWriter);
			delete pFileNode;

			pFileNode = RecordMan::GetInstance()->GetNewFileNode(i);
		}

		// 清空还未关闭的文件
		pFileNode = RecordMan::GetInstance()->GetCloseFileNode(i);
		while (pFileNode)
		{
			WR_CloseFile(pFileNode->ptWriter);
			WR_ReleaseHandle(pFileNode->ptWriter);
			delete pFileNode;

			pFileNode = RecordMan::GetInstance()->GetCloseFileNode(i);
		}

		QueDelete(&m_hdlOpenFileQue[i]);
		QueDelete(&m_hdlCloseFileQue[i]);

		H264_QueueClose(m_tH264SQ+i);
	}
	pthread_mutex_destroy(&m_tMutex);
}

int RecordMan::Run()
{
	m_bExitThread = false;
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		s_iChnID[i] = i;

		// 打开共享队列
		m_tH264SQ[i].iChnID = i;
		m_tH264SQ[i].service_type = H264_QUEUE_RECORD_SERVICE;
		int iRet = H264_QueueOpen(&m_tH264SQ[i]);
		if (iRet != 0)
		{
			LOG_ERROR_FMT("Channel%d H264_QueueOpen failed", i);
		}
		LOG_DEBUG_FMT("Channel%d H264_QueueOpen OK", i);

		if(pthread_create(&m_write_thread[i], NULL, FileCloseThread, s_iChnID+i))
		{
			LOG_ERROR_FMT("Channel%d FileCloseThread create failed", i);
		}

		if(pthread_create(&m_close_thread[i], NULL, FileWriteThread, s_iChnID+i))
		{
			LOG_ERROR_FMT("Channel%d FileWriteThread create failed", i);
		}

		if(pthread_create(&m_create_thread[i], NULL, FileCreateThread, s_iChnID+i))
		{
			LOG_ERROR_FMT("Channel%d FileCreateThread create failed", i);
		}

		if(pthread_create(&m_init_thread[i], NULL, RecordInitThread, s_iChnID+i))
		{
			LOG_ERROR_FMT("Channel%d RecordInitThread create failed", i);
		}
	}

	return 0;
}

int RecordMan::SetAuthInfo( const AuthStatusT * ptInfo )
{
	if(ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tAuthStat, ptInfo, sizeof(AuthStatusT));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

bool RecordMan::IsAuthorised()
{
	if(m_tAuthStat.eAuthType == eProhibit)
	{
		return false;
	}
	else if(m_tAuthStat.eAuthType == ePermanent)
	{
		return true;
	}
	else if(m_tAuthStat.eAuthType == eTemporary)
	{
		if(time(NULL) > m_tAuthStat.tTimeOut)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

int RecordMan::GetChnRecStat( int iChnID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return REC_UNINITIALIZED;
	}

	int iStatus = 0;
	pthread_mutex_lock(&m_tMutex);
	iStatus = m_iStatus[iChnID];
	pthread_mutex_unlock(&m_tMutex);
	return iStatus;
}

int RecordMan::SetChnRecStat( int iChnID, int iStatus )
{
	if( iChnID < 0 || iChnID >= MAX_CHANNEL_NUM ||
		iStatus < REC_UNINITIALIZED ||iStatus > REC_READY)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	m_iStatus[iChnID] = iStatus;
	pthread_mutex_unlock(&m_tMutex);

	LOG_DEBUG_FMT("Channel%d Status -> %s",iChnID,iStatus==REC_UNINITIALIZED?"UNINITIALIZED":
	(iStatus==REC_NO_H264INFO?"NO_H264INFO":
	(iStatus==REC_GETING_INFO?"GETING_INFO":"READY")));
	return 0;
}

int RecordMan::SetStrategy( const RecStrategy * ptInfo )
{
	if(ptInfo == NULL)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	bool bBreak = false;
	pthread_mutex_lock(&m_tMutex);
	bBreak = (ptInfo->iFileFormat != m_tStrategy.iFileFormat);
	memcpy(&m_tStrategy, ptInfo, sizeof(RecStrategy));
	pthread_mutex_unlock(&m_tMutex);
	
	if (bBreak)
	{
		for (int i = 0; i < MAX_CHANNEL_NUM; i++)
		{
			if (GetChnRecStat(i) ==  REC_READY)
			{
				SetChnRecStat(i, REC_NO_H264INFO);
			}
		}
	}
	return 0;
}

int RecordMan::GetStrategy( RecStrategy & tInfo )
{
	pthread_mutex_lock(&m_tMutex);
	memcpy(&tInfo, &m_tStrategy, sizeof(RecStrategy));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int RecordMan::SetPlan( int iChnID, const RecPlan * ptInfo )
{
	if(ptInfo == NULL || iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(m_tPlans+iChnID, ptInfo, sizeof(RecPlan));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int RecordMan::GetPlan( int iChnID, RecPlan & tInfo )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&tInfo, m_tPlans+iChnID, sizeof(RecPlan));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

int RecordMan::SetEnable( int iChnID, bool bEnable )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	m_bEnable[iChnID] = bEnable;
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

bool RecordMan::GetEnable( int iChnID )
{
	bool bEnable = false;
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return bEnable;
	}

	if (GetChnRecStat(iChnID) != REC_READY)
	{
		return false;
	}

	if (IsNeedStopByFull())
	{
		return false;
	}

	if (!IsAuthorised())
	{
		return false;
	}

	if (!CheckPlanNow(iChnID))
	{
		return false;
	}
	
	pthread_mutex_lock(&m_tMutex);
	bEnable = m_bEnable[iChnID];
	pthread_mutex_unlock(&m_tMutex);

	return bEnable;
}

bool RecordMan::CheckPlanNow( int iChnID )
{
	bool bEnable = false;
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return bEnable;
	}
	time_t tNow = time(NULL);
	int iWeekDay = GetWeekDay(tNow);
	int iSecTime = GetSecTime(tNow);

	TIME_SLOT * ptTimeSlots = NULL;
	pthread_mutex_lock(&m_tMutex);
	switch (iWeekDay)
	{
	case 0:
		//printf("Sun\n");
		ptTimeSlots = m_tPlans[iChnID].tSun;
		break;
	case 1:
		//printf("Mon\n");
		ptTimeSlots = m_tPlans[iChnID].tMon;
		break;
	case 2:
		//printf("Tue\n");
		ptTimeSlots = m_tPlans[iChnID].tTue;
		break;
	case 3:
		//printf("Wed\n");
		ptTimeSlots = m_tPlans[iChnID].tWed;
		break;
	case 4:
		//printf("Thu\n");
		ptTimeSlots = m_tPlans[iChnID].tThu;
		break;
	case 5:
		//printf("Fri\n");
		ptTimeSlots = m_tPlans[iChnID].tFri;
		break;
	case 6:
		//printf("Sat\n");
		ptTimeSlots = m_tPlans[iChnID].tSat;
		break;
	default:
		break;
	}

	if (ptTimeSlots == NULL)
	{
		pthread_mutex_unlock(&m_tMutex);
		return false;
	}

	for (int i = 0; i < MAX_REC_PLAN_NUM; i++)
	{
		TIME_SLOT *ptOneSlot = ptTimeSlots+i;
		//printf("Slot%d iStartTime:%d iEndTime:%d iSecTime:%d\n",i, ptOneSlot->iStartTime,ptOneSlot->iEndTime, iSecTime);
		if (iSecTime >= ptOneSlot->iStartTime && iSecTime <= ptOneSlot->iEndTime)
		{
			bEnable = true;
			break;
		}
	}
	pthread_mutex_unlock(&m_tMutex);

	return bEnable;
}

int RecordMan::SetRootPath( const char *pszRootPath )
{
	if(pszRootPath == NULL || strlen(pszRootPath) < 1)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	strcpy(m_szRootPath, pszRootPath);
#ifdef WIN32
	if (m_szRootPath[strlen(m_szRootPath)-1] != '\\')
	{
		strcat(m_szRootPath, "\\");
	}
#else
	if (m_szRootPath[strlen(m_szRootPath)-1] != '/')
	{
		strcat(m_szRootPath, "/");
	}
#endif
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

const char * RecordMan::GetRootPath()
{
	return m_szRootPath;
}

int RecordMan::SetFullStatus( const StoreFullStatusT *ptInfo )
{
	if(ptInfo == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(&m_tFull, ptInfo, sizeof(StoreFullStatusT));
	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

bool RecordMan::IsNeedStopByFull()
{
	if (m_tFull.bFull && m_tFull.iHow2DO != eFull2CyclicCovering)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int RecordMan::SetH264Info( int iChnID, const H264_INFO *ptInfo )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM || ptInfo == NULL)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if (GetChnRecStat(iChnID) != REC_GETING_INFO)
	{
		LOG_ERROR_FMT("not REC_GETING_INFO , can not to REC_READY");
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	memcpy(m_tH264Info+iChnID, ptInfo, sizeof(H264_INFO));
	pthread_mutex_unlock(&m_tMutex);

	// 可以录像了
	LOG_DEBUG_FMT("Channel%d is ready to record", iChnID);
	LOG_DEBUG_FMT("Channel%d H264: %d*%d frame rate:%d", iChnID, ptInfo->iWidth, ptInfo->iHeight, ptInfo->iFrameRate);
	SetChnRecStat(iChnID, REC_READY);

	return 0;
}

int RecordMan::GetNewFrameData( int iChnID, H264_Queue_Node * ptNode )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return -1;
	}

	return H264_QueueGetFull(m_tH264SQ+iChnID, &(ptNode->h264_info), ptNode->frame_data);
}

int RecordMan::CreateNewFileNode( int iChnID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	if (GetChnRecStat(iChnID) != REC_READY)
	{
		return -1;// 还不能创建文件
	}

	if (QueSize(m_hdlOpenFileQue[iChnID]) > 0)
	{
		return 0;
	}

	static bool bAuthOk = true;
	if (!IsAuthorised())
	{
		if (bAuthOk)
		{
			LOG_WARNS_FMT("unauthorised....");
			bAuthOk = false;
		}
		return 0;
	}
	bAuthOk = true;

	static bool bFull = false;
	if (IsNeedStopByFull())
	{
		if (!bFull)
		{
			LOG_WARNS_FMT("Stop By Full....");
			bFull = true;
		}
		return 0;
	}
	bFull = false;

	RecStrategy tStrategy = {0};
	GetStrategy(tStrategy);

	RecFileNode * ptNewNode = new RecFileNode;
	if (ptNewNode == NULL)
	{
		LOG_ERROR_FMT("new RecFileNode failed");
		return -1;
	}
	memset(ptNewNode, 0, sizeof(RecFileNode));
	ptNewNode->iFileFormat = tStrategy.iFileFormat;

	ptNewNode->ptWriter = WR_NewHandle(tStrategy.iFileFormat);
	if (ptNewNode->ptWriter == NULL)
	{
		LOG_ERROR_FMT("GetSuitableWriter %d failed", m_tStrategy.iFileFormat);
		delete ptNewNode;
		ptNewNode = NULL;
		return -1;
	}

	// 创建临时文件
	time_t tNow = time(NULL);
	char szTimeNow[24] = {0};
	TimeFormatString(tNow, szTimeNow, sizeof(szTimeNow),eYMDHMS2);

	sprintf(ptNewNode->szTempFileName, "%s%s/%s_%d.%s", m_szRootPath, RECORD_TEMP_DIR, szTimeNow, iChnID,
		WR_FileSuffix(ptNewNode->ptWriter));

	OAL_MakeDirByPath(ptNewNode->szTempFileName);

	int iRet = WR_OpenFile(ptNewNode->ptWriter, ptNewNode->szTempFileName);
	if (iRet != 0)
	{
		LOG_ERROR_FMT("CreateFile %s failed", ptNewNode->szTempFileName);
		WR_ReleaseHandle(ptNewNode->ptWriter);
		delete ptNewNode;
		ptNewNode = NULL;
		return -1;
	}

	WR_SetVideoTrack(ptNewNode->ptWriter,
		m_tH264Info[iChnID].uSpsData, m_tH264Info[iChnID].iSpsSize,
		m_tH264Info[iChnID].uPpsData, m_tH264Info[iChnID].iPpsSize,
		m_tH264Info[iChnID].iWidth, m_tH264Info[iChnID].iHeight,
		m_tH264Info[iChnID].iFrameRate);

	if (QuePush(m_hdlOpenFileQue[iChnID], ptNewNode, TIMEOUT_NONE) != SUCCESS)
	{
		LOG_ERROR_FMT("QuePush %s failed", ptNewNode->szTempFileName);
		WR_ReleaseHandle(ptNewNode->ptWriter);
		delete ptNewNode;
		ptNewNode = NULL;
		return -1;
	}

	LOG_DEBUG_FMT("Create new file %s", ptNewNode->szTempFileName);

	return 0;
}

RecFileNode * RecordMan::GetNewFileNode( int iChnID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return NULL;
	}

	RecFileNode * ptNode = NULL;
	ptNode = (RecFileNode*)QuePop(m_hdlOpenFileQue[iChnID],2);
	return ptNode;
}

int RecordMan::PutCloseFileNode( int iChnID, RecFileNode * ptNode)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM || ptNode == NULL)
	{
		LOG_ERROR_FMT("Input error");
		return -1;
	}

	return QuePush(m_hdlCloseFileQue[iChnID], ptNode, TIMEOUT_NONE);
}

RecFileNode * RecordMan::GetCloseFileNode( int iChnID )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		LOG_ERROR_FMT("Input error");
		return NULL;
	}

	RecFileNode * ptNode = NULL;
	ptNode = (RecFileNode*)QuePop(m_hdlCloseFileQue[iChnID],2);
	return ptNode;
}

