#ifndef __VIDEOIN_MANAGER_H__
#define __VIDEOIN_MANAGER_H__
#include "oal_typedef.h"
#include "capacity.h"
#include "pthread.h"
#include "mq_onvif.h"
#include "mq_videoin.h"
#include "mq_master.h"

#include <string>
using namespace std;


typedef struct VideoInManufacturer
{
	int  iCode;// ±àÂë
	char szEnName[256];
	char szCnName[256];
}VIManufacturer;

typedef struct VideoInSolution
{
	char szReferred[256];
	int  iWidth;
	int  iHeight;
}VISolution;


extern VIManufacturer g_tSupportMan[4];
extern VISolution g_tSupportSolution[2];

class VideoInManager
{
public:
	static VideoInManager* GetInstance();
	static int Initialize();
	static void UnInitialize();

public:
	int GetChnStatus(int iChnID);
	int SetChnStatus(int iChnID, int iStatus);

	int GetChnVideoSource(int iChnID, VISource & tSource);
	int SetChnVideoSource(int iChnID, const VISource & tSource);
	int AddChnVideoSource(const VISource & tSource);
	int DisableChnVideoSource(int iChnID);

	int GetChnRtsp(int iChnID, int iStreamType, RtspInfo & tRtsp);
	int SetChnRtsp(int iChnID, int iStreamType, const RtspInfo * ptRtsp);

	int GetChnPtzCap(int iChnID, PTZCap & tPtzCap);
	int SetChnPtzCap(int iChnID, const PTZCap * ptPtzCap);

private:
	static VideoInManager* m_pInstance;

	VideoInManager();
	~VideoInManager();
private:
	VISource m_tAllSource[MAX_CHANNEL_NUM];
	int		 m_iAllStatus[MAX_CHANNEL_NUM];
	RtspInfo m_tAllMainRtsp[MAX_CHANNEL_NUM];
	RtspInfo m_tAllSubRtsp[MAX_CHANNEL_NUM];
	PTZCap   m_tAllPtzCap[MAX_CHANNEL_NUM];
	pthread_mutex_t m_tMutex;
};

#endif//__VIDEOIN_MANAGER_H__
