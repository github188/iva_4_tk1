#ifndef _SYSTEM_MAINTAIN_H_
#define _SYSTEM_MAINTAIN_H_
#include "oal_typedef.h"
#include <string>
#include "pthread.h"
#include "oal_queue.h"
using namespace std;

// ϵͳ�Զ���������
#define MAX_REBOOT_TIME_SIZE 5
typedef struct _TimerRebootCfg
{
	bool bEnable;						// ����
	u16 uUseNum;                        // �Զ�����ʱ��ʹ�ø���
	u32 uTimes[MAX_REBOOT_TIME_SIZE];   // �Զ�����ʱ�� ��0�㿪ʼ������
}TimerRebootCfg;

enum 
{
	SYN_TIME_NONE     = 0, // ��ͬ��ʱ��
	SYN_TIME_NTP      = 1, // NTP��ʱ
	SYN_TIME_VIDEOIN  = 2, // ����ƵԴͬ��
	SYN_TIME_PLATFORM = 3, // ��ƽ̨ͬ��
	SYN_TIME_GPS      = 4  // ��GPSͬ��
};

typedef struct _SYN_TIME_CFG
{
	int  iSynType;		// ʱ��ͬ����ʽ
	u32  uInterval;		// ͬ�����
	int  iSynChnID;		// ͬ������Ƶͨ��
	char szNtpAddr[128];  // NTP������
}SynTimeCFG;

#define REBOOT_TIME_USE  10


typedef struct _DelayCmdNode
{
	time_t tCrateTime;	// ����ʱ��
	int    iExpiryTime;	// ����ʱ�䳤
	char   cmd[1024];
}DelayCmdNode;

typedef struct _DelayCmdList
{
	DelayCmdNode tCmd;
	struct _DelayCmdList * ptNext;
}DelayCmdList;

class SystemMaintain
{
public:
	static SystemMaintain* GetInstance();
	static int Initialize();
	static void UnInitialize();

	int Run();
	bool m_bExitThread;

public:

	int SetTimerReboot2System(TimerRebootCfg *ptInfo);

	int Reboot();

	int FactoryRestore();

	int SetSystemTime(const char * pszTime);

	int SetSystemSynTime(SynTimeCFG *ptInfo);
	
	int TestNtpServer(const char *pszNtpSvr);

	int CreateExportFilePath( char *pszFilePath );
	int ExportProfiles( string &filepath );
	int ImportProfiles( const char *pszTempFilePath );
	int ClearExportDir();

	int Upgrade( const char *pszTempFilePath );

	string GetDeviceSerial(){return m_strSerial;}

	int AddDelayCmd( const char * cmd , int iExpiryTime);
	int CheckDelayCmdExpiry();

private:
	SystemMaintain();	
	~SystemMaintain();
	static SystemMaintain* m_pInstance;

	string m_strSerial;
	void InitSerialNumber();

private:
	pthread_t m_syn_thread;

	DelayCmdList *m_hDelayCmdList;
	pthread_mutex_t  m_mutex;
	pthread_t m_delayed_thread;

};
#endif //_SYSTEM_MAINTAIN_H_
