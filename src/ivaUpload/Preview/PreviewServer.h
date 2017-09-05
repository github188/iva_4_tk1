#ifndef __PREVIEW_SERVER_H__
#define __PREVIEW_SERVER_H__
#include "mq_upload.h"
#include "pthread.h"

#define PREVIEW_SVR_PORT		8081

#define MAX_PREVIEW_CLIENT		10

typedef struct ClientInfo
{
	int sock;
	int state;

	int cmd;
	int chn;
	pthread_t thread;
} ClientInfo;

class PreviewClientMan
{
public:
	static PreviewClientMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	PreviewClientMan();
	~PreviewClientMan();
public:
	int Run();

	int GetAvailableSlot(int sockfd);
	int CloseSlot(int iSlotIndex);
	ClientInfo *GetSlot(int iSlotIndex);

	int SendMsg2AllClient(int iChnID, void *buf, int len);

	int SetSlotClient(int iSlotIndex, int iCmdType, int iChnID, pthread_t thread);

public:
	int SendAnalyResultPreivew(AnalyResult * ptResult);
	int SendAnalyTrackPreivew(int iChnID, AnalyTracks * ptTrack);					

	bool m_bExitThread;
	pthread_t m_tPerviewThread;

private:
	ClientInfo m_tClientInfo[MAX_PREVIEW_CLIENT];
	pthread_mutex_t  m_tMutex;
private:
	static PreviewClientMan* m_pInstance;
};
#endif//__PREVIEW_SERVER_H__