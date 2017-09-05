#include "SystemMaintain.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "oal_typedef.h"
#include <vector>
#include <string>
#include <assert.h>

#ifdef WIN32
#include <direct.h>
#else
#include <uuid/uuid.h>
#include <linux/rtc.h>
#include <sys/time.h>
#include <unistd.h>
#endif
#include "oal_log.h"
#include "oal_file.h"
#include "oal_time.h"
#include "capacity.h"
#include "../Profile/SystemProfile.h"

using namespace std;


#ifdef WIN32
#define DEVICE_SERIAL_FILE_NAME		"%appdata%/iva/SERIAL"
#else
#define DEVICE_SERIAL_FILE_NAME		"/etc/iva/SERIAL"
#endif

#define CRON_CONFIG_FILE_PATH		"/etc/crontab"
#define MAX_LINE_LEN 1024

#define PROFILE_EXPORT_DIR			"ProfileExport"

#define PROFILE_IMPORT_PREFIX		"profiles/"
#define UPGRADE_PREFIX				"iva/"

#define PROFILE_IMPORT_NAME			"/ivadata/profile/profile.tar.gz"
#define UPGRADE_NAME				"/ivadata/upgrade/upgrade.tar.gz"


bool CheckGzFileIsValid(const char *filename, const char* prefix)
{
	if(filename == NULL || strlen(filename) == 0)
	{
		LOG_ERROR_FMT("filename is empty");
		return false;
	}

	if(strlen(filename) < strlen(".tar.gz"))
	{
		LOG_ERROR_FMT("%s is not tar.gz file", filename);
		return false;
	}

	if(strcmp(filename+(strlen(filename)-strlen(".tar.gz")),".tar.gz")!=0)
	{
		LOG_ERROR_FMT("%s is not tar.gz file", filename);
		return false;
	}

	bool bOk = false;

#ifndef WIN32
	char cmd[MAX_LINE_LEN];
	sprintf(cmd, "tar tf \"%s\"", filename);
	//printf("%s %d:cmd:%s\n",__FILE__,__LINE__,cmd);
	FILE *pipe = popen(cmd, "r");
	if (NULL == pipe)
	{
		LOG_ERROR_FMT("Failed to open the file %s.",filename);
		return false;
	}

	if (prefix && strlen(prefix) > 0)
	{
		char retLine[MAX_LINE_LEN];
		while (NULL != fgets(retLine, MAX_LINE_LEN - 1, pipe))
		{
			//printf("%s\n",retLine);
			if (strlen(retLine) > strlen(prefix) &&
				memcmp(retLine, prefix, strlen(prefix)) == 0)
			{
				bOk = true;
				//break;
			}
		}
	}
	else
	{
		bOk = true;
	}

	pclose(pipe);
#endif
	if (!bOk)
	{
		LOG_ERROR_FMT("%s is not a valid file", filename);
	}

	return bOk;
}

void * SynTime2GPSThreadFunc(void * /*args*/)
{
	time_t sLastSysTime = 0;
	while(SystemMaintain::GetInstance()->m_bExitThread == false)
	{
		SynTimeCFG tSynCfg = {0};
		SystemProfile::GetInstance()->GetSynTimeInfo(&tSynCfg);
		if (tSynCfg.iSynType != SYN_TIME_GPS)
		{
			sleep(1);
			continue;
		}

		// 是否达到同步周期
		time_t tNow = time(NULL);
		if (tNow - sLastSysTime < tSynCfg.uInterval)
		{
			sleep(1);
			continue;
		}

		// 获取系统时间
		time_t tSynTime = 0;
		int iRet = -1;//GpsManager::GetInstance()->GetTime(tSynTime);
		if (iRet != 0)
		{
			LOG_DEBUG_FMT("Can not get GPS time");
			sleep(1);
			continue;
		}

		tNow = time(NULL);
		char buf1[24] = {0};
		char buf2[24] = {0};
		TimeFormatString(tSynTime, buf1, sizeof(buf1),eYMDHMS1);
		TimeFormatString(tNow, buf2, sizeof(buf2),eYMDHMS1);
		LOG_DEBUG_FMT("============TIME CHECK==========");
		LOG_DEBUG_FMT("Time in SynSrc = %s", buf1);
		LOG_DEBUG_FMT("Time in System = %s", buf2);

		time_t iDeta = (tNow > tSynTime) ? (tNow - tSynTime) : (tSynTime - tNow);
		if(iDeta >= TIME_SYN_PRECISION)
		{
			SystemMaintain::GetInstance()->SetSystemTime(buf1);
		}

		sleep(1);
	}

	return NULL;
}

static void * CheckDelayCmdExpiryThread(void * /*p*/)
{
	while(SystemMaintain::GetInstance()->m_bExitThread == false)
	{
		SystemMaintain::GetInstance()->CheckDelayCmdExpiry();

		sleep(1);
	}

	pthread_exit(NULL);
	return NULL;
}

// 读取CronTab中所有的行，排除filterOutIfContain
static int getCronTabLines(vector<string> &cronLines, const char *filterOutIfContain)
{
	char line[MAX_LINE_LEN];

	FILE *fp = fopen(CRON_CONFIG_FILE_PATH, "r");
	if (NULL == fp)
	{
		LOG_ERROR_FMT("open %s for reading failed.", CRON_CONFIG_FILE_PATH);
		return -1;
	}

	while (NULL != fgets(line, MAX_LINE_LEN, fp))
	{
		if (filterOutIfContain && strstr(line, filterOutIfContain))
		{
			continue;
		}
		
		cronLines.push_back(line);
	}
	
	fclose(fp);

	return 0;
}

static int setCronTabLines(vector<string> &cronLines)
{
	FILE *fp = fopen(CRON_CONFIG_FILE_PATH, "w+");
	if (NULL == fp)
	{
		LOG_ERROR_FMT("open %s for writing failed.\n", CRON_CONFIG_FILE_PATH);
		return -1;
	}

	for (vector<string>::iterator it = cronLines.begin(); it != cronLines.end(); it++)
	{
		fputs(it->c_str(), fp);
	}
		
	fflush(fp);
	fclose(fp);

	return 0;
}

SystemMaintain* SystemMaintain::m_pInstance = NULL;

SystemMaintain::SystemMaintain()
{
	m_strSerial = "";
	InitSerialNumber();

	// 创建定时任务
	TimerRebootCfg tRebootCfg = {0};
	SystemProfile::GetInstance()->GetTimerRebootInfo(&tRebootCfg);
	SetTimerReboot2System(&tRebootCfg);

	SynTimeCFG tSynCfg = {0};
	SystemProfile::GetInstance()->GetSynTimeInfo(&tSynCfg);
	SetSystemSynTime(&tSynCfg);

	m_bExitThread = false;

	pthread_mutex_init(&m_mutex, NULL);
	m_hDelayCmdList = NULL;

	ClearExportDir();
}

SystemMaintain::~SystemMaintain()
{
	m_bExitThread = true;
	pthread_join(m_syn_thread,NULL);
	pthread_join(m_delayed_thread,NULL);
}

SystemMaintain* SystemMaintain::GetInstance()
{
	return m_pInstance;
}

int SystemMaintain::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new SystemMaintain();
		assert(m_pInstance);
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void SystemMaintain::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

int SystemMaintain::Run()
{
	m_bExitThread = false;
	if(pthread_create(&m_syn_thread, NULL, SynTime2GPSThreadFunc, NULL))
	{
		LOG_ERROR_FMT("SynTime2GPSThreadFunc create failed");
	}

	if(pthread_create(&m_delayed_thread, NULL, CheckDelayCmdExpiryThread, NULL))
	{
		LOG_ERROR_FMT("DelayedCmdThreadFunc create failed");
	}

	return 0;
}

int SystemMaintain::SetTimerReboot2System( TimerRebootCfg *ptInfo )
{
	if (ptInfo == NULL)
	{
		return -1;
	}
	
	int  iRet = SystemProfile::GetInstance()->SetTimerRebootInfo(ptInfo);
	if (iRet != 0)
	{
		LOG_ERROR_FMT("SetTimerRebootInfo failed");
		return -1;
	}

	std::vector<std::string> cronLines;
	iRet = getCronTabLines(cronLines, "reboot");
	if (iRet != 0)
	{
		LOG_ERROR_FMT("getCronTabLines reboot failed");
	}

	if (ptInfo->bEnable)
	{
		for (u16 i = 0; i < ptInfo->uUseNum; i++)
		{
			u32 uTimeSec = ptInfo->uTimes[i];
			int hou = (uTimeSec / 60) / 60;
			int min = (uTimeSec / 60) % 60;
			
			char dirbuf[512] = {0};
			char * pszRoot = NULL;
#ifdef WIN32
			pszRoot = _getcwd(dirbuf, sizeof(dirbuf));

#else
			pszRoot = getcwd(dirbuf, sizeof(dirbuf));
#endif
			if (pszRoot == NULL)
			{
				pszRoot = (char *)DEFAULT_BIN_PATH;
			}

			char line[MAX_LINE_LEN] = {0};
			sprintf(line, "%d %d * * * root cd %ssh && sh -x reboot.sh\n", min, hou, pszRoot);
			//LOG_DEBUG_FMT("%s",line);

			cronLines.push_back(line);
		}
	}

	iRet = setCronTabLines(cronLines);
	if (iRet != 0)
	{
		LOG_ERROR_FMT("setCronTabLines reboot failed");
	}
	return 0;
}

int SystemMaintain::Reboot()
{
#ifndef DISABLE_MAINTIAN
	return AddDelayCmd("reboot", 5);
#else
	return 0;
#endif
}

int SystemMaintain::FactoryRestore()
{
	TimerRebootCfg tTimerReboot = {0};
	SetTimerReboot2System(&tTimerReboot);
	
	SynTimeCFG tSynCfg = {0};
	SetSystemSynTime(&tSynCfg);

	char cmd[64];
	sprintf(cmd, "rm -fr %s*",CFG_DIR_PATH);
#ifndef DISABLE_MAINTIAN
	system(cmd);
#endif

	Reboot();
	return 0;
}

int SystemMaintain::SetSystemTime( const char * pszTime )
{
	if (pszTime == NULL)
	{
		return -1;
	}

	time_t tWhen = String2Time(pszTime, eYMDHMS1);
	if (tWhen < 0)
	{
		LOG_INFOS_FMT("format is error, %s",pszTime);
		return -1;
	}

	LOG_INFOS_FMT("[Set System Time]: %s",pszTime);

	char cmd[64];
	sprintf(cmd, "date -s \"%s\"", pszTime);
	system(cmd);

	// 同步到硬件时钟
	sprintf(cmd, "hwclock -w");
	system(cmd);
	return 0;
}

int SystemMaintain::SetSystemSynTime( SynTimeCFG *ptInfo )
{
	if (ptInfo == NULL)
	{
		return -1;
	}
	
	int  iRet = SystemProfile::GetInstance()->SetSynTimeInfo(ptInfo);
	if (iRet != 0)
	{
		LOG_ERROR_FMT("SetSynTimeInfo failed");
		return -1;
	}
	
	OnvifTimeSyn tOnvifTimeSyn;
	tOnvifTimeSyn.bEnable = (ptInfo->iSynType == SYN_TIME_VIDEOIN);
	tOnvifTimeSyn.iSynChnID = ptInfo->iSynChnID;
	tOnvifTimeSyn.uInterval = ptInfo->uInterval;
	MQ_Onvif_Set_TimeSyn(MSG_TYPE_MASTER,&tOnvifTimeSyn);

	std::vector<std::string> cronLines;
	iRet = getCronTabLines(cronLines, "ntpdate");
	if (iRet != 0)
	{
		LOG_ERROR_FMT("getCronTabLines ntpdate failed");
	}

	if (ptInfo->iSynType == SYN_TIME_NTP)
	{
		char line[MAX_LINE_LEN];
		sprintf(line, "*/%d * * * * root ntpdate %s\n", ptInfo->uInterval, ptInfo->szNtpAddr);
		cronLines.push_back(line);

		// 立即同步一次
		char cmd[MAX_LINE_LEN];
		sprintf(cmd, "ntpdate %s", ptInfo->szNtpAddr);
		system(cmd);
	}

	iRet = setCronTabLines(cronLines);
	if (iRet != 0)
	{
		LOG_ERROR_FMT("setCronTabLines ntpdate failed");
	}
	return 0;
}

int SystemMaintain::TestNtpServer( const char *pszNtpSvr )
{
	if (pszNtpSvr == NULL)
	{
		return -1;
	}

#ifndef WIN32
	char cmd[MAX_LINE_LEN];
	char retLine[MAX_LINE_LEN];
	int ret = 0;

	sprintf(cmd, "ntpdate %s", pszNtpSvr);
	FILE* pipe = popen(cmd, "r");
	if (NULL == pipe)
		return 0;

	char *result = fgets(retLine, MAX_LINE_LEN - 1, pipe);
	if (NULL != result && NULL != strstr(retLine, "offset"))
		ret = 1;

	pclose(pipe);
#endif
	return 0;
}

int SystemMaintain::CreateExportFilePath( char *pszFilePath )
{
	static int siIndex = 0;
	if (pszFilePath == NULL)
	{
		return -1;
	}
	siIndex++;
	siIndex %= 1000;
	char szTime[40] = {0};
	TimeFormatString(time(NULL), szTime, 40, eYMDHMS2);

	sprintf(pszFilePath, "%s/%s/%s_%03d.tar.gz", DEFAULT_ROOT_PATH, PROFILE_EXPORT_DIR,szTime,siIndex);

	OAL_MakeDirByPath(pszFilePath);

	return 0;
}

int SystemMaintain::ExportProfiles( string &filepath )
{
	char szTarFile[128] = {0};
	CreateExportFilePath(szTarFile);
	filepath = szTarFile;

	char cmd[512] = {0};
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "cd ../; tar -zcf %s profiles;cd bin",szTarFile);
	system(cmd);

	filepath = szTarFile;
	return 0;
}

int SystemMaintain::ImportProfiles( const char *pszTempFilePath )
{
	if (pszTempFilePath == NULL)
	{
		return -1;
	}
		
	// 删除旧文件
	OAL_MakeDirByPath(PROFILE_IMPORT_NAME);
#ifdef WIN32
	_unlink(PROFILE_IMPORT_NAME);
#else
	unlink(PROFILE_IMPORT_NAME);
#endif // WIN32


	// 拷贝新文件
	rename(pszTempFilePath, PROFILE_IMPORT_NAME);
	//return OAL_FileExist(PROFILE_IMPORT_NAME);
	if (!CheckGzFileIsValid(PROFILE_IMPORT_NAME, PROFILE_IMPORT_PREFIX))
	{
		//unlink(PROFILE_IMPORT_NAME);
		return 0;
	}
	return 0;
}

int SystemMaintain::ClearExportDir()
{
	char szDir[128] = {0};
	sprintf(szDir, "%s/%s/", DEFAULT_ROOT_PATH, PROFILE_EXPORT_DIR);

	char szCmd[128] = {0};
	sprintf(szCmd, "rm -f %s*", szDir);
	system(szCmd);
	return 0;
}

int SystemMaintain::Upgrade( const char *pszTempFilePath )
{
	if (pszTempFilePath == NULL)
	{
		return -1;
	}

	// 删除旧文件
	OAL_MakeDirByPath(UPGRADE_NAME);
#ifdef WIN32
	_unlink(UPGRADE_NAME);
#else
	unlink(UPGRADE_NAME);
#endif // WIN32

	// 拷贝新文件
	rename(pszTempFilePath, UPGRADE_NAME);
	//return OAL_FileExist(UPGRADE_NAME);
	if (!CheckGzFileIsValid(UPGRADE_NAME, UPGRADE_PREFIX))
	{
#ifdef WIN32
		_unlink(UPGRADE_NAME);
#else
		unlink(UPGRADE_NAME);
#endif // WIN32
		return -1;
	}
	return 0;
}

int SystemMaintain::AddDelayCmd( const char * cmd , int iExpiryTime)
{
	if (cmd == NULL)
	{
		return -1;
	}

	int iRet = -1;
	pthread_mutex_lock(&m_mutex);
	DelayCmdList * pNode = new DelayCmdList;
	if (pNode)
	{
		strcpy(pNode->tCmd.cmd, cmd);
		pNode->tCmd.tCrateTime = time(NULL);
		pNode->tCmd.iExpiryTime = iExpiryTime;
		pNode->ptNext = m_hDelayCmdList;
		m_hDelayCmdList = pNode;

		char time1[40] = {0};
		char time2[40] = {0};
		TimeFormatString(pNode->tCmd.tCrateTime, time1, 40, eYMDHMS1);
		TimeFormatString(pNode->tCmd.tCrateTime+pNode->tCmd.iExpiryTime, time2, 40, eYMDHMS1);
		printf("Add Delay Cmd %s, %s,to %s\n", cmd ,time1, time2);
		iRet = 0;
	}
	else
	{
		LOG_ERROR_FMT("new DelayCmdList failed");
	}
	pthread_mutex_unlock(&m_mutex);
	return iRet;
}

int SystemMaintain::CheckDelayCmdExpiry()
{
	pthread_mutex_lock(&m_mutex);
	DelayCmdList * pLast = NULL;
	DelayCmdList * pDel = NULL;
	DelayCmdList * pNode = m_hDelayCmdList;
	while (pNode)
	{
		if (time(NULL) > pNode->tCmd.tCrateTime+pNode->tCmd.iExpiryTime)
		{
			char szCreate[40] = {0};
			TimeFormatString(pNode->tCmd.tCrateTime,szCreate, 40, eYMDHMS1);
			LOG_INFOS_FMT("Cmd:%s,Create at %s,ExpiryTime=%d,Done",pNode->tCmd.cmd,szCreate, pNode->tCmd.iExpiryTime);

			// 执行命令
			system(pNode->tCmd.cmd);

			// 从链表中删除
			if (pLast)
			{
				pLast->ptNext = pNode->ptNext;
			}
			else
			{
				m_hDelayCmdList = m_hDelayCmdList->ptNext;
			}

			// 释放节点
			pDel = pNode;
			pNode = pNode->ptNext;
			delete pDel;
			pDel = NULL;
			continue;
		}
		else
		{
			pLast = pNode;
			pNode = pNode->ptNext;
			continue;
		}
	}
	pthread_mutex_unlock(&m_mutex);
	return 0;
}

void SystemMaintain::InitSerialNumber()
{
	// 读取文件
	FILE * fp = fopen(DEVICE_SERIAL_FILE_NAME, "r");
	if(fp)
	{
		char buf[512]={0};
		char *res = fgets(buf, sizeof(buf)-1, fp);

		fclose(fp);
		fp= NULL;

		if(strlen(buf) > 0 && res)
		{
			m_strSerial = buf;
			printf("SerialNumber: %s\n", m_strSerial.c_str());
			return;// 读取成功
		}
	}

	// 产生一个序列号
	char szuuid[128] = {0};

#ifdef WIN32
	sprintf(szuuid, "111111111111");  
#else
	uuid_t uu;
	uuid_generate(uu);
	uuid_unparse(uu, szuuid);
#endif

	m_strSerial = szuuid;
	printf("SerialNumber: %s\n", m_strSerial.c_str());

	OAL_MakeDirByPath(DEVICE_SERIAL_FILE_NAME);

	// 保存成序列号文件
	fp = fopen(DEVICE_SERIAL_FILE_NAME,"w+");
	if(NULL == fp)
	{
		LOG_ERROR_FMT("Open %s failed", DEVICE_SERIAL_FILE_NAME);
		return;
	}

	fputs(m_strSerial.c_str(), fp);
	fflush(fp);
	fclose(fp);

	return;
}
