#include "PreviewServer.h"
#include <assert.h>
#include "oal_log.h"
#ifdef WIN32
#include <winsock2.h>
typedef int socklen_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#include "oal_time.h"

void *PreviewPushThread(void *arg);


#define CLIENT_INFO_STATE_UNUSED	0
#define CLIENT_INFO_STATE_USED		1

#define EVENT_PREVIEW_SEND_TYPE_ALG_EVENT			0
#define EVENT_PREVIEW_SEND_TYPE_VIDEO_ANALYSE_OSD	2


PreviewClientMan* PreviewClientMan::m_pInstance = NULL;

PreviewClientMan* PreviewClientMan::GetInstance()
{
	return m_pInstance;
}

int PreviewClientMan::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new PreviewClientMan();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void PreviewClientMan::UnInitialize()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

PreviewClientMan::PreviewClientMan()
{
	pthread_mutex_init(&m_tMutex, NULL);
	memset(&m_tClientInfo, 0, sizeof(m_tClientInfo));
	m_bExitThread = false;
}

PreviewClientMan::~PreviewClientMan()
{
	m_bExitThread = true;

	pthread_join(m_tPerviewThread,NULL);
	pthread_mutex_destroy(&m_tMutex);
}

int PreviewClientMan::Run()
{
	m_bExitThread = false;

	if (pthread_create(&m_tPerviewThread, NULL, PreviewPushThread, NULL) != 0){
		LOG_ERROR_FMT("Event preview thread creates failed.");
	}
	return 0;
}

int PreviewClientMan::GetAvailableSlot(int sockfd)
{
	int iSlot = -1;

	pthread_mutex_lock(&m_tMutex);
	for (int i = 0; i < MAX_PREVIEW_CLIENT; i++)
	{
		if (m_tClientInfo[i].state == CLIENT_INFO_STATE_UNUSED)
		{
			m_tClientInfo[i].sock  = sockfd;
			m_tClientInfo[i].state = CLIENT_INFO_STATE_USED;
			iSlot = i;
			break;
		}
	}
	pthread_mutex_unlock(&m_tMutex);

	return iSlot;
}

int PreviewClientMan::CloseSlot(int iSlotIndex)
{
	if (iSlotIndex < 0 || iSlotIndex > MAX_PREVIEW_CLIENT-1)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	
	ClientInfo *pCI = m_tClientInfo + iSlotIndex;
	if (pCI->state == CLIENT_INFO_STATE_UNUSED)
	{
		LOG_DEBUG_FMT("slot %d is unused.", iSlotIndex);
	}

	if(pCI->sock > 0)
	{
#ifdef WIN32
		closesocket(pCI->sock);
#else
		close(pCI->sock);
#endif // WIN32
	}

	memset(pCI, 0, sizeof(ClientInfo));
	LOG_DEBUG_FMT("close client info slot at %d.", iSlotIndex);

	pthread_mutex_unlock(&m_tMutex);
	return 0;
}

ClientInfo *PreviewClientMan::GetSlot(int iSlotIndex)
{
	if (iSlotIndex < 0 || iSlotIndex > MAX_PREVIEW_CLIENT-1)
	{
		return NULL;
	}

	return m_tClientInfo + iSlotIndex;
}

int PreviewClientMan::SendMsg2AllClient(int iChnID, void *buf, int len)
{
	pthread_mutex_lock(&m_tMutex);
	for (int i = 0; i < MAX_PREVIEW_CLIENT; i++)
	{
		if (m_tClientInfo[i].state == CLIENT_INFO_STATE_USED && 
			m_tClientInfo[i].sock > 0 &&
			m_tClientInfo[i].chn == iChnID)
		{
			if (send(m_tClientInfo[i].sock, (const char *)buf, len, 0) == -1)
			{
				pthread_cancel(m_tClientInfo[i].thread);

#ifdef WIN32
				closesocket(m_tClientInfo[i].sock);
#else
				close(m_tClientInfo[i].sock);
#endif // WIN32

				memset(&m_tClientInfo[i], 0, sizeof(ClientInfo));
				LOG_DEBUG_FMT("close client info slot at %d.", i);
			}
			//return 1;
		}
	}
	pthread_mutex_unlock(&m_tMutex);
	return 1;
}

int PreviewClientMan::SetSlotClient(int iSlotIndex, int iCmdType, int iChnID, pthread_t thread)
{
	if (iSlotIndex < 0 || iSlotIndex > MAX_PREVIEW_CLIENT-1)
	{
		return -1;
	}

	pthread_mutex_lock(&m_tMutex);
	m_tClientInfo[iSlotIndex].cmd = iCmdType;
	m_tClientInfo[iSlotIndex].chn = iChnID;
	m_tClientInfo[iSlotIndex].thread = thread;
	pthread_mutex_unlock(&m_tMutex);
	return 1;
}

int PreviewClientMan::SendAnalyResultPreivew( AnalyResult * ptResult )
{
	if (ptResult == NULL)
	{
		return -1;
	}

	char sendBuf[1024 * 8] = {0};
	int *sendType = (int *)sendBuf;
	*sendType = EVENT_PREVIEW_SEND_TYPE_ALG_EVENT;
	int *jsonLen = (int *)(sendBuf + sizeof(int));
	char *jsonStr = sendBuf + sizeof(int) + sizeof(*jsonLen);

	sprintf(jsonStr, "{"
		"\"jsonrpc\":\"2.0\","
		"\"channelno\":%d,"
		"\"eventno\":%d,"
		"\"eventtype\":\"%s\","
		"\"platename\":\"%s\","
		"\"platecolor\":\"%s\","
		"\"snaptime\":\"%s\""
		"}",
		ptResult->iChnID, ptResult->iEventID, ptResult->szEventDesc, ptResult->szPlate,
		ptResult->szPlateColor, ptResult->szCreateTime);

	*jsonLen = strlen(jsonStr);

	SendMsg2AllClient(ptResult->iChnID, sendBuf, sizeof(*sendType) + sizeof(*jsonLen) + (*jsonLen));
	return 0;
}

#define MAX_ANALYSE_OSD_NUM				5		//最大分析水印个数
#define MAX_ANALYSE_OSD_POINT_NUM		10		//每个分析水印最大点个数

#pragma pack(push) //保存对齐状态
#pragma pack(1)
typedef struct tagAOSinglePoint
{
	int x;
	int y;
} AOSinglePoint;

typedef struct tagAOColorRGB
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} AOColorRGB;

typedef struct tagAOSingleUnitInfo
{
	int nNum;           //点的个数
	AOColorRGB stColor;
	AOSinglePoint stPoints[MAX_ANALYSE_OSD_POINT_NUM];
} AOSingleUnitInfo;

typedef struct tagAOAnalyseOSDInfo
{
	int channelNo;
	int nNum;            //算法分析水印个数
	AOSingleUnitInfo stOSDInfo[MAX_ANALYSE_OSD_NUM];
} AOAnalyseOSDInfo;
#pragma pack(pop)


int PreviewClientMan::SendAnalyTrackPreivew( int iChnID, AnalyTracks * ptTrack )
{
	if (ptTrack == NULL)
	{
		return -1;
	}

	char sendBuf[1024 * 8] = {0};
	int *sendType = (int *)sendBuf;
	*sendType = EVENT_PREVIEW_SEND_TYPE_VIDEO_ANALYSE_OSD;
	int *structLen = (int *)(sendBuf + sizeof(*sendType));
	*structLen = sizeof(AOAnalyseOSDInfo);

	AOAnalyseOSDInfo tTrackInfo = {0};
	tTrackInfo.channelNo = iChnID;
	tTrackInfo.nNum = ptTrack->iTrackNum;
	//printf("%s channelNo=%d nNum=%d\n",__FUNCTION__,tTrackInfo.channelNo,tTrackInfo.nNum);
	for (int i = 0; i < tTrackInfo.nNum && i < MAX_ANALYSE_OSD_NUM; i++)
	{
		tTrackInfo.stOSDInfo[i].nNum = ptTrack->atTracks[i].iPointNum;
		tTrackInfo.stOSDInfo[i].stColor.r = ptTrack->atTracks[i].tColor.r;
		tTrackInfo.stOSDInfo[i].stColor.g = ptTrack->atTracks[i].tColor.g;
		tTrackInfo.stOSDInfo[i].stColor.b = ptTrack->atTracks[i].tColor.b;
		for (int j = 0; j < tTrackInfo.stOSDInfo[i].nNum && j < MAX_ANALYSE_OSD_POINT_NUM; j++)
		{
			tTrackInfo.stOSDInfo[i].stPoints[j].x = ptTrack->atTracks[i].atPoints[j].x;
			tTrackInfo.stOSDInfo[i].stPoints[j].y = ptTrack->atTracks[i].atPoints[j].y;
		}
	}
	memcpy(sendBuf + sizeof(int) + sizeof(int), &tTrackInfo, sizeof(AOAnalyseOSDInfo));

	SendMsg2AllClient(iChnID, sendBuf, sizeof(int) + sizeof(int) + sizeof(AOAnalyseOSDInfo));
	return 0;
}

// OK=len failed=-1 disconnect=0
static int recv_bytes(int sockfd, void *buf, int len)
{
	if (buf == NULL || len <= 0 || sockfd < 0)
	{
		return -1;
	}

	char *p = (char *)buf;
	int left = len;
	int recvlen = 0;
	while (1)
	{
		//printf("need recv %d \n",len);
		recvlen = recv(sockfd, p, left, 0);
		if(recvlen > 0)
		{  
			p += recvlen;
			left -= recvlen;
			if (left <= 0)
			{
				return len;
			}
		}
		else if (recvlen < 0 )
		{  
			if(errno == EINTR)
			{  
				;  
			}
			else
			{
				printf("socket disconnected!\n");   
				return 0;  
			}  
		}
		else if (recvlen == 0)
		{  
			printf("socket disconnected!\n");   
			return 0;  
		}
	}
	return 0;
}

// OK=4 failed=-1 disconnect=0
static int recv_int(int sockfd, int & ivalue)
{
	char buf[5] = {0};
	int iret = recv_bytes(sockfd, buf, 4);
	if ( iret != 4)
	{
		return iret;
	}

	ivalue = *((int *)buf);

	return iret;
}

void *ReceiveSubscriptionInfo(void *args)
{
	int index = *((int*)args);

	ClientInfo *pCI = PreviewClientMan::GetInstance()->GetSlot(index);
	if (pCI == NULL)
	{
		printf("ClientInfo = NULL, index = %d\n",index);
		return NULL;
	}

	do 
	{
		int cmd = 0;
		int chn = 0;

		if (recv_int(pCI->sock, cmd) != 4) break;
		if (recv_int(pCI->sock, chn) != 4) break;

		printf("ClientInfo %d, cmd = %d, chn= %d\n",index, cmd, chn);

		PreviewClientMan::GetInstance()->SetSlotClient(index, cmd, chn, pthread_self());

		int exitCmd = -1;
		recv_int(pCI->sock, exitCmd);
		printf("ClientInfo %d, exitCmd = %d\n",index, exitCmd);
	} while (0);

	PreviewClientMan::GetInstance()->CloseSlot(index);
	return NULL;
}

void *PreviewPushThread(void *arg)
{
	static int sIndex[MAX_PREVIEW_CLIENT]={0};
	for (int i = 0; i < MAX_PREVIEW_CLIENT; i++)
	{
		sIndex[i] = i;
	}
	int fdSvr = socket(AF_INET, SOCK_STREAM, 0);
	if(fdSvr < 0)
	{
		LOG_ERROR_FMT("socket Preview Push Server failed");
		return NULL;
	}

	struct sockaddr_in svrAddr;
	memset(&svrAddr, 0, sizeof(svrAddr));
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svrAddr.sin_port = htons(PREVIEW_SVR_PORT);

	int iReuseFlag = 1;
	if (setsockopt(fdSvr, SOL_SOCKET, SO_REUSEADDR, (const char*)&iReuseFlag, sizeof(int)) == -1)
	{
		LOG_ERROR_FMT("set reuse sock err: [%s].", strerror(errno));
	}

	LOG_DEBUG_FMT("Preview Push Thread Is Running");

	while(!PreviewClientMan::GetInstance()->m_bExitThread)
	{
		if (bind(fdSvr, (struct sockaddr *)&svrAddr, sizeof(svrAddr)) != -1)
		{
			break;
		}

		LOG_DEBUG_FMT("bind event preview sock err: [%s].", strerror(errno));
		sleep(1);
	}

	listen(fdSvr, MAX_PREVIEW_CLIENT);

	while(!PreviewClientMan::GetInstance()->m_bExitThread)
	{
		struct sockaddr_in clientAddress;
		int clientLen = sizeof(clientAddress);
		printf("wait preview client\n");
		int clientSock = accept(fdSvr, (struct sockaddr *)&clientAddress, (socklen_t *)&clientLen);
		if (-1 == clientSock)
		{
			printf("accept err: [%s].\n", strerror(errno));
			continue;
		}

		LOG_DEBUG_FMT("new preview client from (%s:%d)",inet_ntoa(clientAddress.sin_addr),ntohs(clientAddress.sin_port));

		int iClientIndex = PreviewClientMan::GetInstance()->GetAvailableSlot(clientSock);
		if (iClientIndex < 0)
		{
#ifdef WIN32
			closesocket(clientSock);
#else
			close(clientSock);
#endif // WIN32
			LOG_DEBUG_FMT("no available slot to preview client from (%s:%d)",inet_ntoa(clientAddress.sin_addr),ntohs(clientAddress.sin_port));
			continue;
		}

		// 创建线程接收订阅头，在接收成功以后才会加入到管理队列
		pthread_t receiveHeadThread;
		pthread_create(&receiveHeadThread, NULL, ReceiveSubscriptionInfo, (void *)(sIndex+iClientIndex));
	}

	shutdown(fdSvr, 2);

	return NULL;
}
