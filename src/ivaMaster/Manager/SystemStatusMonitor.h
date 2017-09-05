#ifndef __SYSTEM_STATUS_MONITOR__
#define __SYSTEM_STATUS_MONITOR__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "oal_typedef.h"
#include "pthread.h"
#ifndef WIN32
#include "sensors/sensors.h"
#include "sensors/error.h"
#endif

using namespace std;

typedef enum _ChipType
{
	E_CPU=0,
	E_GPU,
	E_MEM,
	E_BOARD
}ChipTypeE;

typedef struct _SysStatus
{
	double dbCpuUsage; // cpu使用率
	double dbCpuTemp;  // cpu温度

	double dbMemUsage; // 内存使用率

	double dbBoardTemp;// 主板温度
}SysStatus;


class SystemStatusMonitor
{
public:
	static SystemStatusMonitor* GetInstance();
	static int Initialize();
	static void UnInitialize();
	void Run();

public:
	int GetStatus(SysStatus &tStatus);
	u32 GetUptime();
	unsigned long m_start_tick;

public:
	void SetCpuOccupy(double fNewValue);
	bool m_bRefresh;
	pthread_t m_thread_cpucal;

private:
	double GetSensorsTemp(ChipTypeE eType);
	volatile double m_dbCpuUsage;

private:
	SystemStatusMonitor();
	~SystemStatusMonitor();
	static SystemStatusMonitor* m_pInstance;
};

#endif//__SYSTEM_STATUS_MONITOR__
