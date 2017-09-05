#ifndef __RECORD_MAN_H__
#define __RECORD_MAN_H__

#include "capacity.h"
#include "h264_queue.h"
#include "oal_queue.h"
#include "pthread.h"
#include "mq_record.h"
#include "mq_analysis.h"
#include "RecordWriter.h"

typedef struct _RecFileNode 
{
	int  iFileFormat;
	char szTempFileName[128];
	char szRealFileName[256];
	WRHandle ptWriter;

	time_t tStart;
	time_t tEnd;

	int  iTryCnt;
}RecFileNode;

#define TIME_SCALE	90000

typedef struct _H264_INFO 
{  
	int iFrameRate;
	int iWidth;
	int iHeight;

	int iSpsSize;  
	unsigned char uSpsData[128];

	int iPpsSize;
	unsigned char uPpsData[128];
}H264_INFO;

enum
{
	REC_UNINITIALIZED,	// δ��ʼ��״̬
	REC_NO_H264INFO,	// �Ѿ���ʼ��������û�л�ȡ��PPS/SPS
	REC_GETING_INFO,	// ���ڻ�ȡH264������Ϣ
	REC_READY			// ����¼����
};

class RecordMan
{
public:
	static RecordMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	RecordMan();
	~RecordMan();

public:
	int Run();

	int SetAuthInfo(const AuthStatusT * ptInfo);
	bool IsAuthorised();

	int GetChnRecStat(int iChnID);
	int SetChnRecStat(int iChnID, int iStatus);

	int SetStrategy(const RecStrategy * ptInfo);
	int GetStrategy(RecStrategy & tInfo);

	int SetPlan(int iChnID, const RecPlan * ptInfo);
	int GetPlan(int iChnID, RecPlan & tInfo);

	int SetEnable(int iChnID, bool bEnable);
	bool GetEnable(int iChnID);
	bool CheckPlanNow(int iChnID);

	int SetRootPath(const char *pszRootPath);
	const char * GetRootPath();

	int SetFullStatus(const StoreFullStatusT *ptInfo);
	bool IsNeedStopByFull();
	
	int SetH264Info(int iChnID, const H264_INFO *ptInfo);

public:
	int GetNewFrameData(int iChnID, H264_Queue_Node * ptNode);

	int CreateNewFileNode(int iChnID);

	RecFileNode * GetNewFileNode(int iChnID);
	int PutCloseFileNode(int iChnID, RecFileNode *ptNode);

	RecFileNode * GetCloseFileNode(int iChnID);

public:
	bool 		 m_bExitThread;

private:
	AuthStatusT     m_tAuthStat;
	RecStrategy     m_tStrategy;
	bool			m_bEnable[MAX_CHANNEL_NUM];
	RecPlan			m_tPlans[MAX_CHANNEL_NUM];

	char			m_szRootPath[256];
	StoreFullStatusT m_tFull;
	int             m_iStatus[MAX_CHANNEL_NUM];
	H264_INFO       m_tH264Info[MAX_CHANNEL_NUM];
	H264_Queue		m_tH264SQ[MAX_CHANNEL_NUM];
	pthread_mutex_t m_tMutex;

	pthread_t		m_init_thread[MAX_CHANNEL_NUM];// Ԥ������ȡ��Ƶ����SPS PPS
	pthread_t		m_create_thread[MAX_CHANNEL_NUM];// �ļ������߳�
	TQueHndl		m_hdlOpenFileQue[MAX_CHANNEL_NUM];
	pthread_t		m_write_thread[MAX_CHANNEL_NUM];// �ļ�д���߳�
	TQueHndl		m_hdlCloseFileQue[MAX_CHANNEL_NUM];
	pthread_t		m_close_thread[MAX_CHANNEL_NUM];// �ļ��ر��̣߳���¼�����֮�󣬽��к�����¼���ݿ�Ȳ���
private:
	static RecordMan* m_pInstance;
};

#endif //__RECORD_MAN_H__


