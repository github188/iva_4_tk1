#ifndef __DEVICE_MANAGER_H__
#define __DEVICE_MANAGER_H__
#include "pthread.h"
#include "oal_typedef.h"

#define MAX_DISK_PATH 		512

#define ROM_DEVICE 			"/dev/mmcblk0"
#define INVALID_DEVICE		"/dev/mmcblk0rpmb"

enum 
{
	DISK_UNKNOWN = -1,
	DISK_ROM = 0,
	DISK_SD  = 1,
	DISK_USB = 2,
	DISK_HDD = 3
};

enum 
{
	DISK_STAT_OK		  = 0,	// 可用
	DISK_STAT_NOTFOUND	  = 1,	// 未发现磁盘
	DISK_STAT_UNAVAILABLE = 2,	// 发现磁盘,但是不可用（无有效分区）
	DISK_STAT_UNMOUNT     = 3	// 未挂载
};

enum 
{
	DISK_USE_SYSTEM		= 0,	// 系统
	DISK_USE_DISABLE	= 1,	// 不使用
	DISK_USE_STORE		= 2,	// 存储数据 都可以
	DISK_USE_COPY		= 3		// 离线拷贝 USB和SD可以
};

typedef struct _DiskUsageCfg
{
	int iUsbUseType;
	int iHddUseType;
	int iSdUseType;
}DiskUsageCfg;

typedef struct _DiskShow
{
	char szPath[MAX_DISK_PATH];		// 分区路径
	char szFormat[24];				// 格式化类型
	u64  uTotalSize;				// 磁盘总大小
	u64  uFreeSize;					// 磁盘剩余大小
	int	 iUseType;					// 使用类型
	int	 iStat;						// 使用状态
}DiskShow;

#define MAX_DEVICE_NUM 		8
#define MAX_PATITION_NUM 	32

typedef struct _AllDiskShow
{
	DiskShow tRom;
	DiskShow tSD;
	DiskShow tUSB;
	DiskShow tHDD;
}AllDiskShow;

typedef struct _tagPartitionInfo
{
    char path[MAX_DISK_PATH];		// 分区路径
	int  disktype; 					// 磁盘类型
    char type[16];					// 分区类型 主分区 扩展分区 逻辑分区
    char fstype[16];				// 分区格式化类型 FAT32  NTFS ...
    u64  totalsize;					// 分区大小 B
    char mountpoint[MAX_DISK_PATH];	// 挂载点
}PARTITION;

typedef struct _tagDeviceDisk
{
    char	  path[MAX_DISK_PATH]; 		// 磁盘的路径
    char 	  model[128]; 				// 磁盘的型号
    int  	  disktype; 				// 磁盘类型
	u64 	  totalsize;				// 磁盘总大小

    int 	  part_cnt;					// 分区个数
    PARTITION part[MAX_PATITION_NUM];	// 分区信息
}DEVICE;

typedef struct _tagDiskAllProbe
{
    int    disk_cnt;
    DEVICE disk[MAX_DEVICE_NUM];
}DiskAllProbe;

typedef struct _DiskFullStrategy
{
	u32 uMinSize;
	int iHow2DO;
}DiskFullStrategy;

class DeviceManager
{
public:
	//配置管理初始化
	static int Initialize(); 
	static void UnInitialize(); 

	//获取配置管理实例
	static DeviceManager* GetInstance();

public:

	int Run();
	
	int GetAllDiskShow(AllDiskShow* pInfo);
	int SetUsageType(int iDiskType, int iUseType);
	int GetUsageType(int iDiskType);

	int SetAllDiskInfo(const DiskAllProbe* pInfo);

	int GetDiskShowByType(int iDiskType, DiskShow * pInfo);

	int GetPartInfoByMount(const char * mount, PARTITION* pInfo);
	int GetPartInfoByType(int iDiskType, PARTITION* pInfo);

	int CheckMountPoint();
	int CheckFullStrategy();

	int DoDataCopy();

	const char * GetRootPath();

	bool PathIsFull(const char *pszPath);

	int FormatNtfs(int iDiskType);

	int DeleteAllRecord();

public:
	bool m_bExitThread;

private:
	pthread_mutex_t m_mutex;
	DiskUsageCfg    m_tDiskUsage;
	DiskAllProbe 	m_tAllDisk;

	pthread_t 	m_scan_thread;
	pthread_t 	m_fullcheck_thread;
	pthread_t 	m_copy_thread;
private:
	DeviceManager();
	~DeviceManager();
	static DeviceManager* m_pInstance;
};

const char * PrintDiskType(int type);
const char * PrintSize(u64 sizeB);

#endif // __DEVICE_MANAGER_H__
