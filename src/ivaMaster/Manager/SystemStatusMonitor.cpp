#include "SystemStatusMonitor.h"
#include "DeviceManager.h"
#include "../Profile/SystemProfile.h"
#include "oal_log.h"
#include "oal_time.h"

using namespace std;

typedef struct stCPUPacket
{
	unsigned int user;
	unsigned int nice;
	unsigned int system;
	unsigned int idle;
}CPU_OCCUPY;

static void get_cpuoccupy (CPU_OCCUPY *cpust)
{
	if(cpust == NULL)
	{
		return;
	}

#ifndef WIN32
	FILE *fd = fopen ("/proc/stat", "r"); 
	if(NULL == fd)
	{
		LOG_ERROR_FMT("Open /proc/stat failed\n");
		return;
	}
	
	char buff[1024] = {0}; 
	char *res = fgets (buff, sizeof(buff), fd);
	if(res)
	{
		char name[48] = {0};
		unsigned int user = 0;
		unsigned int nice = 0;
		unsigned int system = 0;
		unsigned int idle = 0;
		int iOK = sscanf (buff, "%s %u %u %u %u", name, &user, &nice, &system, &idle);
		if(iOK == 5)
		{
			cpust->user = user;
			cpust->nice = nice;
			cpust->system = system;
			cpust->idle = idle;
		}
	}
	fclose(fd);
#endif
}

static double cal_cpuoccupy (CPU_OCCUPY *o, CPU_OCCUPY *n) 
{   
	if(o == NULL || n == NULL)
	{
		return 0;
	}
	
	unsigned long od, nd;    
	unsigned long id, sd;
	double cpu_use = 0;   

	od = (unsigned long) (o->user + o->nice + o->system +o->idle);//第一次(用户+优先级+系统+空闲)的时间再赋给od
	nd = (unsigned long) (n->user + n->nice + n->system +n->idle);//第二次(用户+优先级+系统+空闲)的时间再赋给nd

	id = (unsigned long) (n->user - o->user);    //用户第一次和第二次的时间之差再赋给id
	sd = (unsigned long) (n->system - o->system);//系统第一次和第二次的时间之差再赋给sd

	if((nd-od) != 0)
	{
		cpu_use = (sd + id) * 100.0/(nd - od); //((用户+系统)乖100)除(第一次和第二次的时间差)再赋给g_cpu_used
	}
	else
	{
		cpu_use = 0;
	}
	
	return cpu_use;
}


void* CpuOccupyThread(void* pParam)
{
	//pthread_detach(pthread_self());

	while(SystemStatusMonitor::GetInstance()->m_bRefresh)
	{
		CPU_OCCUPY cpu_stat1 = {0};
		memset(&cpu_stat1, 0, sizeof(CPU_OCCUPY));
		get_cpuoccupy(&cpu_stat1);

		sleep(3);

		CPU_OCCUPY cpu_stat2 = {0};
		memset(&cpu_stat2, 0, sizeof(CPU_OCCUPY));
		get_cpuoccupy(&cpu_stat2);

		double fNewValue = cal_cpuoccupy(&cpu_stat1, &cpu_stat2);
		SystemStatusMonitor::GetInstance()->SetCpuOccupy(fNewValue);
	}

	return NULL;
}

static double GetMemUsage()
{
	unsigned long lTotal  = 0; 
	unsigned long lFree   = 0;                        
	unsigned long lBuffer = 0;                       
	unsigned long lCached = 0;                      

#ifndef WIN32
	/*
	MemTotal:        1937888 kB
	MemFree:         1370552 kB
	Buffers:           50928 kB
	Cached:           164828 kB
	*/
	FILE* fd = fopen ("/proc/meminfo", "r"); 
	if(NULL == fd)
	{
		LOG_ERROR_FMT("Open /proc/meminfo failed\n");
		return 0;
	}

	int iFindNum = 0;
	char buff[256] = {0};   
	char *res = NULL;
	while((res = fgets (buff, sizeof(buff), fd)))
	{
		string content = buff;
		//printf("content: %s",content.c_str());
		
		string::size_type pos = string::npos;

		if((pos = content.find("MemTotal:")) != string::npos)
		{
			string strSize = content.substr(pos+strlen("MemTotal:"));
			string_trim(strSize);
			lTotal = atoi(strSize.c_str());
			//printf("lTotal: %ld\n",lTotal);
			iFindNum++;
		}
		else if((pos = content.find("MemFree:")) != string::npos)
		{
			string strSize = content.substr(pos+strlen("MemFree:"));
			string_trim(strSize);
			lFree = atoi(strSize.c_str());
			//printf("lFree: %ld\n",lFree);
			iFindNum++;
		}
		else if((pos = content.find("Buffers:")) != string::npos)
		{
			string strSize = content.substr(pos+strlen("Buffers:"));
			string_trim(strSize);
			lBuffer= atoi(strSize.c_str());
			//printf("lBuffer: %ld\n",lBuffer);
			iFindNum++;
		}
		else if((pos = content.find("Cached:")) != string::npos)
		{
			string strSize = content.substr(pos+strlen("Cached:"));
			string_trim(strSize);
			lCached= atoi(strSize.c_str());
			//printf("lCached: %ld\n",lCached);
			iFindNum++;
		}

		if(iFindNum >= 4)
		{
			break;
		}
	}

	fclose(fd);
#endif

	// MemFree=buffers+cached+free
	double frate = lTotal > 0 ? (lTotal - lFree - lBuffer - lCached) / lTotal : 0;
	return frate;
}


SystemStatusMonitor* SystemStatusMonitor::m_pInstance=NULL;

SystemStatusMonitor::SystemStatusMonitor()
{
	m_dbCpuUsage  = 0.0;
	m_bRefresh = true;

	m_start_tick = GetTickCount();

#ifndef WIN32
	
	//printf("\n=====================Sensors=======================\n");
	sensors_init(NULL);
	//int nr = 0;
	//const sensors_chip_name * chip = sensors_get_detected_chips(NULL, &nr);
	//while(chip)
	//{
	//	char buf[256];
	//	if (sensors_snprintf_chip_name(buf, 256, chip)>0)
	//	{
	//		printf("SENSOR: %s\n", buf);
	//	}
		
	//	chip = sensors_get_detected_chips(NULL, &nr);
	//}
	/*
	SENSOR: CPU-therm-virtual-0
	SENSOR: GPU-therm-virtual-0
	SENSOR: MEM-therm-virtual-0
	SENSOR: PLL-therm-virtual-0
	SENSOR: Tboard_tegra-virtual-0
	SENSOR: Tdiode_tegra-virtual-0
	*/
	//printf("===================================================\n\n");
#endif
}

SystemStatusMonitor::~SystemStatusMonitor()
{
	m_bRefresh = false;
	pthread_join(m_thread_cpucal,NULL);
#ifndef WIN32
	sensors_cleanup();
#endif
}

SystemStatusMonitor* SystemStatusMonitor::GetInstance()
{
	return m_pInstance;
}

int SystemStatusMonitor::Initialize()
{
	int iRet = -1;

	if(NULL == m_pInstance)
	{
		m_pInstance = new SystemStatusMonitor;
		m_pInstance->Run();
	}
	return 0;
}

void SystemStatusMonitor::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void SystemStatusMonitor::Run()
{
	int iRet = pthread_create(&m_thread_cpucal, NULL, &CpuOccupyThread, NULL);
	if (0 != iRet)
	{
		LOG_ERROR_FMT("pthread CpuOccupyThread create fail, error=(\"%s\")\n", strerror(iRet));
	}
}

int SystemStatusMonitor::GetStatus( SysStatus &tStatus )
{
	tStatus.dbCpuUsage  = m_dbCpuUsage;
	tStatus.dbCpuTemp   = GetSensorsTemp(E_CPU);
	tStatus.dbMemUsage  = GetMemUsage();
	tStatus.dbBoardTemp = GetSensorsTemp(E_BOARD);
	return 0;
}

double SystemStatusMonitor::GetSensorsTemp( ChipTypeE eType )
{
#ifndef WIN32
	char szPrefix[32] = {0};
	switch(eType)
	{
	case E_CPU:
		strcpy(szPrefix,"CPU");
		break;
	case E_GPU:
		strcpy(szPrefix,"GPU");
		break;
	case E_MEM:
		strcpy(szPrefix,"MEM");
		break;
	case E_BOARD:
		strcpy(szPrefix,"Tboard");
		break;
	default:
		return 0.0;
	}

	// 查找到对应的传感器
	const sensors_chip_name *pSensorChip = NULL;
	int nr = 0;
	const sensors_chip_name * chip = sensors_get_detected_chips(NULL, &nr);
	while(chip)
	{
		char chip_name[256] = {0};
		sensors_snprintf_chip_name(chip_name, 256, chip);
		
		if(strncmp(szPrefix, chip_name, strlen(szPrefix)) == 0)
		{
			pSensorChip = chip;
			break;
		}
		
		chip = sensors_get_detected_chips(NULL, &nr);
	}

	if(pSensorChip == NULL)
	{
		return 0.0;
	}

	// 找到对应的feature
	const sensors_feature *pSensorFeature = NULL;
	nr = 0;
	const sensors_feature * feature = sensors_get_features(pSensorChip, &nr);
	while(feature)
	{
		if(SENSORS_FEATURE_TEMP == feature->type)
		{
			pSensorFeature = feature;
			break;
		}
		
		feature = sensors_get_features(pSensorChip, &nr);
	}

	if(pSensorFeature == NULL)
	{
		return 0.0;
	}

	// 找到对应的subfeature
	double val = 0.0;
	const sensors_subfeature *pSubFeature = NULL;
	pSubFeature = sensors_get_subfeature(pSensorChip, pSensorFeature, SENSORS_SUBFEATURE_TEMP_FAULT);
	if(pSubFeature)
	{
		if(sensors_get_value(pSensorChip, pSubFeature->number, &val) == 0)
		{
			return val;
		}
	}

	pSubFeature = sensors_get_subfeature(pSensorChip, pSensorFeature, SENSORS_SUBFEATURE_TEMP_INPUT);
	if(pSubFeature)
	{
		if(sensors_get_value(pSensorChip, pSubFeature->number, &val) == 0)
		{
			return val;
		}
	}
#endif
	return 0.0;
}

unsigned int SystemStatusMonitor::GetUptime()
{
	unsigned int sec = 0;

	unsigned long tick = GetTickCount();
	sec = tick / 1000 - m_start_tick / 1000;

//#ifndef WIN32
#if 0
	char cmd[2048];
	char retLine[2048];

	sprintf(cmd, "cat /proc/uptime");
	FILE* pipe = popen(cmd, "r");
	if (NULL == pipe)
		return 0;

	char *result = fgets(retLine, 2048 - 1, pipe);
	if (NULL != result)
	{
		sec = atoi(result);
	}

	pclose(pipe);
#endif
	return sec;
}

void SystemStatusMonitor::SetCpuOccupy( double fNewValue )
{
	m_dbCpuUsage = fNewValue;
}
