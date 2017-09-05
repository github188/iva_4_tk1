#ifndef _SYSTEM_MAINTAIN_H_
#define _SYSTEM_MAINTAIN_H_
#include "oal_typedef.h"
#include <string>
#include "pthread.h"
#include "oal_queue.h"
using namespace std;

// 系统自动重启配置
#define MAX_REBOOT_TIME_SIZE 5
typedef struct _TimerRebootCfg
{
	bool bEnable;						// 开关
	u16 uUseNum;                        // 自动重启时间使用个数
	u32 uTimes[MAX_REBOOT_TIME_SIZE];   // 自动重启时间 从0点开始的秒数
}TimerRebootCfg;

enum 
{
	SYN_TIME_NONE     = 0, // 不同步时间
	SYN_TIME_NTP      = 1, // NTP对时
	SYN_TIME_VIDEOIN  = 2, // 与视频源同步
	SYN_TIME_PLATFORM = 3, // 与平台同步
	SYN_TIME_GPS      = 4  // 与GPS同步
};

typedef struct _SYN_TIME_CFG
{
	int  iSynType;		// 时间同步方式
	u32  uInterval;		// 同步间隔
	int  iSynChnID;		// 同步的视频通道
	char szNtpAddr[128];  // NTP服务器
}SynTimeCFG;

#define REBOOT_TIME_USE  10


typedef struct _DelayCmdNode
{
	time_t tCrateTime;	// 创建时间
	int    iExpiryTime;	// 持续时间长
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
