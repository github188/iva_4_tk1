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
	DISK_STAT_OK		  = 0,	// ����
	DISK_STAT_NOTFOUND	  = 1,	// δ���ִ���
	DISK_STAT_UNAVAILABLE = 2,	// ���ִ���,���ǲ����ã�����Ч������
	DISK_STAT_UNMOUNT     = 3	// δ����
};

enum 
{
	DISK_USE_SYSTEM		= 0,	// ϵͳ
	DISK_USE_DISABLE	= 1,	// ��ʹ��
	DISK_USE_STORE		= 2,	// �洢���� ������
	DISK_USE_COPY		= 3		// ���߿��� USB��SD����
};

typedef struct _DiskUsageCfg
{
	int iUsbUseType;
	int iHddUseType;
	int iSdUseType;
}DiskUsageCfg;

typedef struct _DiskShow
{
	char szPath[MAX_DISK_PATH];		// ����·��
	char szFormat[24];				// ��ʽ������
	u64  uTotalSize;				// �����ܴ�С
	u64  uFreeSize;					// ����ʣ���С
	int	 iUseType;					// ʹ������
	int	 iStat;						// ʹ��״̬
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
    char path[MAX_DISK_PATH];		// ����·��
	int  disktype; 					// ��������
    char type[16];					// �������� ������ ��չ���� �߼�����
    char fstype[16];				// ������ʽ������ FAT32  NTFS ...
    u64  totalsize;					// ������С B
    char mountpoint[MAX_DISK_PATH];	// ���ص�
}PARTITION;

typedef struct _tagDeviceDisk
{
    char	  path[MAX_DISK_PATH]; 		// ���̵�·��
    char 	  model[128]; 				// ���̵��ͺ�
    int  	  disktype; 				// ��������
	u64 	  totalsize;				// �����ܴ�С

    int 	  part_cnt;					// ��������
    PARTITION part[MAX_PATITION_NUM];	// ������Ϣ
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
	//���ù����ʼ��
	static int Initialize(); 
	static void UnInitialize(); 

	//��ȡ���ù���ʵ��
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
