#include "DeviceManager.h"
#include <stdio.h>
#include <errno.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <parted/parted.h>
#endif
#include "oal_time.h"
#include "oal_log.h"
#include "oal_file.h"
#include "../Profile/SystemProfile.h"

const char * PrintDiskType(int type)
{
	switch (type)
	{
	case DISK_ROM:
		return "ROM";
	case DISK_SD:
		return "SD";
	case DISK_USB:
		return "USB";
	case DISK_HDD:
		return "HDD";
	}
	return "UNKNOWN";
}

const char * PrintSize(u64 sizeB)
{
	static char buffer[5][256] = {0};
	static int si = -1;
	si++;
	si %= 5;

	if(sizeB < 1024)
		sprintf(buffer[si], "%lld B", sizeB);
	else if(sizeB < 1024*1024)
		sprintf(buffer[si], "%.2f KB", sizeB/1024.0f);
	else if(sizeB < 1024*1024*1024)
		sprintf(buffer[si], "%.2f MB", sizeB/1024.0f/1024.0f);
	else
		sprintf(buffer[si], "%.2f GB", sizeB/1024.0f/1024.0f/1024.0f);

	return buffer[si];
}

static const char * GetRigthMountPoint(int type)
{
	switch (type)
	{
	case DISK_ROM:
		return MOUNT_POINT_ROM;
	case DISK_SD:
		return MOUNT_POINT_SD;
	case DISK_USB:
		return MOUNT_POINT_USB;
	case DISK_HDD:
		return MOUNT_POINT_HDD;
	}
	return NULL;
}

int GetDiskType(const char * devpath)
{
	struct stat buf;
	char link[512] = {0};
	char link_path[512]={0};
#ifndef WIN32
	if(devpath== NULL || lstat(devpath,&buf) < 0)
	{
		return DISK_UNKNOWN;
	}

	if( S_ISBLK(buf.st_mode))
	{
		sprintf(link_path,"/sys/dev/block/%d:%d",major(buf.st_rdev),minor(buf.st_rdev));
		//printf("%s linkpath:%s\n",devpath,link_path);
		if( access(link_path, F_OK) >= 0 )
		{
			int ret = readlink(link_path,link,512);
			//printf("ret= %d,link:%s\n",ret, link);
			if(ret == 0 ) return DISK_UNKNOWN;
			string strlink = link;
			if(strlink.find("usb",0) != string::npos)
			{
				return DISK_USB;
			}
			else if(strlink.find("ata",0) != string::npos)
			{
				return DISK_HDD;
			}	
			else if(strlink.find("mmc",0) != string::npos)
			{
				if(strstr(devpath, ROM_DEVICE))
				{
					return DISK_ROM;
				}
				else
				{
					return DISK_SD;
				}
			}
		}
	}

#endif
	return DISK_UNKNOWN;
}

int GetRealMountPoint(const char *devpath, char *mntpoint)
{
	int ret = -1;

	if(devpath == NULL || mntpoint == NULL)
	{
		return -1;
	}

#ifndef WIN32   
	char cmd[1024] = {0};
	sprintf(cmd, "lsblk -ln -o MOUNTPOINT %s 2>&1",devpath);
	FILE* pipe = popen(cmd, "r");
	if (NULL == pipe)
	{
		return ret;
	}

	char retLine[1024] = {0};
	while (NULL != fgets(retLine, sizeof(retLine) - 1, pipe))
	{
		if (retLine[0] == '/')
		{
			ret = 0;
			string mntpt = retLine;
			string_trim(mntpt);
			strcpy(mntpoint, mntpt.c_str());
		}
	}

	pclose(pipe);
#endif

	return ret;
}

static const char * PrintDeviceType(int type)
{
#ifndef WIN32   
	switch (type)
	{
	case PED_DEVICE_UNKNOWN:
		return "UNKNOWN";
	case PED_DEVICE_SCSI:
		return "SCSI";
	case PED_DEVICE_IDE:
		return "IDE";
	case PED_DEVICE_DAC960:
		return "DAC960";
	case PED_DEVICE_CPQARRAY:
		return "CPQARRAY";
	case PED_DEVICE_FILE:
		return "FILE";
	case PED_DEVICE_ATARAID:
		return "ATARAID";
	case PED_DEVICE_I2O:
		return "I2O";
	case PED_DEVICE_UBD:
		return "UBD";
	case PED_DEVICE_DASD:
		return "DASD";
	case PED_DEVICE_VIODASD:
		return "VIODASD";
	case PED_DEVICE_SX8:
		return "SX8";
	case PED_DEVICE_DM:
		return "DM";
	case PED_DEVICE_XVD :
		return "XVD";
	case PED_DEVICE_SDMMC:
		return "SDMMC";
	case PED_DEVICE_VIRTBLK:
		return "VIRTBLK";
	case PED_DEVICE_AOE :
		return "AOE";
	case PED_DEVICE_MD :
		return "MD";
	case PED_DEVICE_LOOP:
		return "LOOP";
	}
#endif
	return "UNKNOWN";
}

static void GetMountPointSize(const char* mntpoint, u64 &totalsize, u64 &freesize)
{
	totalsize = 0;
	freesize = 0;

	if(mntpoint == NULL)
	{
		printf("%s %d\n", __FILE__, __LINE__);
		return;
	}

	if(strlen(mntpoint) < 1)
	{
		printf("%s %d\n", __FILE__, __LINE__);
		return;
	}

#ifndef WIN32
	struct statfs sfs;
	memset(&sfs,0,sizeof(struct statfs));
	if(statfs(mntpoint, &sfs) != 0)
	{
		printf("%s %d\n", __FILE__, __LINE__);
		return;
	}

// 	printf("path:%s\n块大小:%d 块总数:%lu 可用块数:%lu 非超级用户可获取的块数:%lu\n文件结点总数:%lu 可用文件结点数:%lu\n文件名的最大长度:%d\n",
// 		mntpoint,
// 		sfs.f_bsize, sfs.f_blocks, sfs.f_bfree, sfs.f_bavail,
// 		sfs.f_files, sfs.f_ffree, 
// 		sfs.f_namelen);

	totalsize = ((u64)sfs.f_bsize * (u64)sfs.f_blocks);
	freesize = ((u64)sfs.f_bsize * (u64)sfs.f_bfree);
#endif
}

static int umountDevice(const char *source)
{
	int ret = 0;

	if(source == NULL)
	{
		return -1;
	}

#ifndef WIN32
    char cmd[1024] = {0};
    sprintf(cmd, "umount %s 2>&1", source);
	LOG_DEBUG_FMT("umount %s", source);

	FILE* pipe = popen(cmd, "r");
	if (NULL == pipe)
	{
		return -1;
	}

    char retLine[1024] = {0};
	while (NULL != fgets(retLine, sizeof(retLine) - 1, pipe))
	{
		LOG_DEBUG_FMT("%s", retLine);
		ret = -1;
	}
	pclose(pipe);
#endif

	return ret;
}

static int mountDevice(const char *source, const char *target)
{
	int ret = -1;

	if(source == NULL || target == NULL)
	{
		return -1;
	}

	char mkdircmd[1024] = {0};
	sprintf(mkdircmd, "%s/test.file", target);
	OAL_MakeDirByPath(mkdircmd);
    
#ifndef WIN32
    char cmd[1024] = {0};
    //sprintf(cmd, "mount %s %s 2>&1", source, target);
	sprintf(cmd, "mount -t ntfs-3g %s %s -o locale=zh_CN.gb2312 2>&1", source, target);

	FILE* pipe = popen(cmd, "r");
	if (NULL == pipe)
	{
		LOG_DEBUG_FMT("mount %s %s failed", source, target);
		return ret;
	}

	ret = 0;
    char retLine[1024] = {0};
	while (NULL != fgets(retLine, sizeof(retLine) - 1, pipe))
	{
		LOG_DEBUG_FMT("%s", retLine);
		ret = -1;
	}

	pclose(pipe);
#endif
	LOG_DEBUG_FMT("mount %s %s", source, target);

    return ret;
}

static int diskformat2ntfs(const char* dev)
{
	int ret = -1;

	if(dev == NULL)
	{
		return -1;
	}
    
#ifndef WIN32
	char cmd[1024] = {0};
    sprintf(cmd, "mkfs.ntfs -fF %s 2>&1", dev);
	LOG_DEBUG_FMT("mkfs.ntfs -fF %s", dev);

	FILE* pipe = popen(cmd, "r");
	if (NULL == pipe)
	{
		return ret;
	}

    char retLine[1024];
	while (NULL != fgets(retLine, sizeof(retLine) - 1, pipe))
	{
		LOG_DEBUG_FMT("%s", retLine);
		if (strstr(retLine, "successfully") != NULL)
		{
			ret = 0;
		}
	}

	pclose(pipe);
#endif

    return ret;
}

static int IsFileUsed(const char *fileName)
{
	int used = 0;
	if (fileName == NULL)
	{
		return 0;
	}

#ifndef WIN32
	char cmd[128];
	sprintf(cmd, "lsof | awk '{print $3}' | grep \"%s\"", fileName);

	FILE* pipe = popen(cmd, "r");
	if (NULL == pipe)
	{
		return used;
	}

	char *result = NULL;
	char retLine[1024] = {0};
	while ((result = fgets(retLine, 1024 - 1, pipe)) != NULL)
	{
		if (strcmp(retLine, fileName) == 0)
		{
			used = 1;
			break;
		}
	}

	pclose(pipe);
#endif
	return used;
}

static int CopyFile(const char *srcPath, const char *dstPath)
{
	char cmd[1024];
	char retLine[1024] = {0};
	int iRet = 0;
#ifndef WIN32

	sprintf(cmd, "cp -f \"%s\" \"%s\" 2>&1; sync", srcPath, dstPath);
	//printf("%s:%d %s\n",__FILE__, __LINE__, cmd);
	FILE* pipe = popen(cmd, "r");
	if (NULL == pipe)
	{
		return -1;
	}

	char *result;
	if ((result = fgets(retLine, 1024 - 1, pipe)) != NULL)
	{
		printf("cp failed. [%s].\n", cmd);
		printf("%s\n", retLine);
		iRet = -1;
	}

	pclose(pipe);
#endif

	return iRet;
}

//#define PRINT_PROBE
static void ProbeDisk(DiskAllProbe * pProbeDisk)
{
	if(pProbeDisk == NULL)
	{
		return;
	}
	
	memset(pProbeDisk, 0, sizeof(DiskAllProbe));
	
#ifndef WIN32
	ped_device_probe_all();
	
	PedDevice *dev = NULL;
	while((dev = ped_device_get_next(dev)))
	{
#ifdef PRINT_PROBE
		printf("====PedDevice======\n");
		printf("=mode: %s\n", dev->model);
		printf("=path: %s\n", dev->path);
		printf("=type: %s\n", PrintDeviceType(dev->type));
		printf("=size: %s\n", PrintSize(dev->sector_size*dev->length));
		//printf("=size: %s(phy)\n", PrintSize(dev->phys_sector_size*dev->length));
		printf("=USB : %s\n", PrintDiskType(GetDiskType(dev->path)));
		//printf("=logical sector size:%lld\n",dev->sector_size);
		//printf("=physical sector size:%lld\n",dev->phys_sector_size);
		//printf("=device length (LBA):%lld\n",dev->length);
		//printf("=opened number:%d\n",dev->open_count);
		//printf("=read_only:%d\n",dev->read_only);
		//printf("=external_mode:%d\n",dev->external_mode);
		//printf("=dirty:%d\n",dev->dirty);
		//printf("=boot_dirty:%d\n",dev->boot_dirty);
		//printf("=hw_geom: cylinders=%d = heads=%d sectors=%d\n",
		//	dev->hw_geom.cylinders,dev->hw_geom.heads,dev->hw_geom.sectors);
		//printf("=bios_geom: cylinders=%d = heads=%d sectors=%d\n",
		//	dev->bios_geom.cylinders,dev->bios_geom.heads,dev->bios_geom.sectors);
#endif
		if(pProbeDisk->disk_cnt >= MAX_DEVICE_NUM)
		{
			LOG_ERROR_FMT("MAX_DEVICE_NUM = %d", MAX_DEVICE_NUM);
			break;
		}
		
		// 磁盘类型
		if(dev->type == PED_DEVICE_SDMMC && strcmp(dev->path, ROM_DEVICE) == 0)
		{
			//continue;// 忽略系统自身的EMMC
		}
		else if(dev->type == PED_DEVICE_SDMMC && strcmp(dev->path, INVALID_DEVICE) == 0)
		{
			continue;// 忽略一个4M的系统自身的EMMC
		}

		if(dev->sector_size * dev->length == 0)
		{
			continue;// 忽略容量为0
		}

		// 获取分区信息
		PedDisk* disk = ped_disk_new(dev);
		if(disk == NULL)
		{
			continue;// 忽略未分区
		}

		int i = pProbeDisk->disk_cnt;
		pProbeDisk->disk_cnt++;
		
		DEVICE * pDev = &(pProbeDisk->disk[i]);
		strcpy(pDev->path,	dev->path);
		strcpy(pDev->model, dev->model);
		pDev->disktype  = GetDiskType(dev->path);
		pDev->totalsize = dev->sector_size * dev->length;
		pDev->part_cnt  = 0;

		for (PedPartition*	part = disk->part_list; part; part = ped_disk_next_partition (disk, part))
		{
			if (!ped_partition_is_active (part))
			{
				continue;// 忽略非active分区
			}

			if(pDev->part_cnt >= MAX_PATITION_NUM)
			{
				LOG_ERROR_FMT("MAX_PATITION_NUM = %d", MAX_PATITION_NUM);
				break;
			}

			// 如果是扩展分区，查找扩展分区之下的逻辑分区
			if (part->type & PED_PARTITION_EXTENDED)
			{
				for (PedPartition *log_part = part->part_list; log_part; log_part = log_part->next)
				{
					if (log_part->type & PED_PARTITION_METADATA)
						continue;// 忽略扩展分区之下的非逻辑分区
					else if (log_part->type & PED_PARTITION_FREESPACE)
						continue;// 忽略扩展分区之下的非逻辑分区
					else if (log_part->type & PED_PARTITION_EXTENDED)
						continue;// 忽略扩展分区之下的非逻辑分区
					else if (log_part->type & PED_PARTITION_LOGICAL)
						;
					else
						continue;// 忽略扩展分区之下的非逻辑分区

					if(log_part->geom.length * dev->length == 0)
					{
						continue;// 忽略容量为0
					}

					if(pDev->part_cnt >= MAX_PATITION_NUM)
					{
						LOG_ERROR_FMT("MAX_PATITION_NUM = %d", MAX_PATITION_NUM);
						break;
					}
					
					int j = pDev->part_cnt;
					pDev->part_cnt++;
					
					PARTITION * pPart = &(pDev->part[j]);
					pPart->disktype = pDev->disktype;
					strncpy(pPart->path,   ped_partition_get_path(log_part), sizeof(pPart->path) -1);
					strncpy(pPart->type,   ped_partition_type_get_name(log_part->type), sizeof(pPart->type) -1);
					strncpy(pPart->fstype, (log_part->fs_type ? log_part->fs_type->name : "UNKNOWN"), sizeof(pPart->fstype) -1);
					pPart->totalsize = log_part->geom.length * dev->sector_size;
					GetRealMountPoint(pPart->path, pPart->mountpoint);


#ifdef PRINT_PROBE
					printf("=PARTITION:%d\n",j);
					printf("==path:%s\n",pPart->path);
					printf("==type:%s\n",pPart->type);
					printf("==file:%s\n",pPart->fstype);
					printf("==size:%s\n",PrintSize(pPart->totalsize));
					printf("==mount:%s\n",pPart->mountpoint);
#endif
				}
			}
			else
			{
				if (!ped_partition_is_active (part))
				{
					continue;// 忽略非有效分区
				}

				if(part->geom.length * dev->length == 0)
				{
					continue;// 忽略容量为0
				}

				int j = pDev->part_cnt;
				pDev->part_cnt++;
				
				PARTITION * pPart = &(pDev->part[j]);
				pPart->disktype = pDev->disktype;
				strncpy(pPart->path,   ped_partition_get_path(part), sizeof(pPart->path) -1);
				strncpy(pPart->type,   ped_partition_type_get_name(part->type), sizeof(pPart->type) -1);
				strncpy(pPart->fstype, (part->fs_type ? part->fs_type->name : "UNKNOWN"), sizeof(pPart->fstype) -1);
				pPart->totalsize = part->geom.length * dev->sector_size;

				GetRealMountPoint(pPart->path, pPart->mountpoint);

#ifdef PRINT_PROBE
				printf("=PARTITION:%d\n",j);
				printf("==path:%s\n",pPart->path);
				printf("==type:%s\n",pPart->type);
				printf("==file:%s\n",pPart->fstype);
				printf("==size:%s\n",PrintSize(pPart->totalsize));
				printf("==mount:%s\n",pPart->mountpoint);
#endif
			}
		}
	}
#endif
}

static bool StringIsValidDate(const char *str)  
{  
	if(str == NULL)
	{
		return false;
	}
	
	if(strlen(str) != 8/*strlen("20010101")*/)
	{
		return false;
	}

	if( str[0] < '0' || str[0] > '9' ||
		str[1] < '0' || str[1] > '9' ||
		str[2] < '0' || str[2] > '9' ||
		str[3] < '0' || str[3] > '9' ||
		str[4] < '0' || str[4] > '9' ||
		str[5] < '0' || str[5] > '9' ||
		str[6] < '0' || str[6] > '9' ||
		str[7] < '0' || str[7] > '9')
	{
		return false;
	}

	return true;
}

static bool DirIsEmpty(const char *dir_path)
{
	int num = 0;

#ifndef WIN32
	DIR *dirp = NULL;
	struct stat statbuf;

	if(dir_path == NULL)
	{
		return false;
	}

	dirp = opendir(dir_path);
	if(dirp == NULL)
	{
		return false;
	}

	//printf("%s,dir_path:%s\n",__FUNCTION__,dir_path);

	struct dirent *dir_info;
	while((dir_info = readdir(dirp)) != NULL)
	{
		//printf("%s,d_name:%s\n",__FUNCTION__, dir_info->d_name);

		if (dir_info->d_name[0] == '.')
		{
			continue;
		}
		else
		{
			num++;
			break;
		}
	}

	closedir(dirp);
	dirp = NULL;
#endif
	return (num > 0 ? false : true);  
}

static int GetOldDateDir(const char * path,char * old_dir, char *old_name)
{
	if(path == NULL || old_dir == NULL || old_name == NULL)
	{
		return -1;
	}

	int  iRmCnt = 0;
#ifndef WIN32

	DIR *dir = opendir(path);
	if(dir == NULL)
	{
		return -1;
	}
	//printf("%s,path:%s\n",__FUNCTION__,path);

	struct dirent *dir_info;
	while((dir_info = readdir(dir)) != NULL)
	{
		//printf("%s,d_name:%s\n",__FUNCTION__,dir_info->d_name);
		char file_path[256] = {0};
		memset(file_path,0,sizeof(file_path));
		strcpy(file_path, path);
		if(file_path[strlen(path) - 1] != '/')
		{
			strcat(file_path, "/");
		}
		strcat(file_path, dir_info->d_name);
		//printf("%s,file_path:%s\n",__FUNCTION__,file_path);

		if(StringIsValidDate(dir_info->d_name))
		{
			if(DirIsEmpty(file_path))
			{
				//printf("%s,file_path:%s is empty\n",__FUNCTION__,file_path);
				LOG_DEBUG_FMT("rm empty dir:%s",file_path);
				rmdir(file_path);
			}
			else
			{
				if(old_name[0] == '\0')
				{
					strcpy(old_name,dir_info->d_name);
					strcpy(old_dir,file_path);
				}
				else if(strcmp(old_name,dir_info->d_name)>0)
				{
					strcpy(old_name,dir_info->d_name);
					strcpy(old_dir,file_path);
				}
			}
		}
	}

	closedir(dir);
#endif
	return 0;
}

static int RemoveOldestFile(const char * mountpoint)
{
	if (mountpoint == NULL)
	{
		return -1;
	}

	char szIvaDataDir[512] = {0};
	if(mountpoint[strlen(mountpoint) - 1] != '/')
		sprintf(szIvaDataDir, "%s/%s", mountpoint, IDENTIFY_DIR);
	else
		sprintf(szIvaDataDir, "%s%s", mountpoint, IDENTIFY_DIR);

	//printf("%s,rootdir:%s\n",__FUNCTION__,szIvaDataDir);
#ifndef WIN32

	// 获取根目录属性
	struct stat statbuf;
	if(lstat(szIvaDataDir, &statbuf) != 0)
	{
		LOG_ERROR_FMT("lstat %s failed,error:%s",szIvaDataDir,strerror(errno));
		return -1;
	}

	// 遍历 得到日期最早的文件夹
	DIR * dir = opendir(szIvaDataDir);
	if(dir == NULL)
	{
		LOG_ERROR_FMT("opendir %s failed", szIvaDataDir);
		return -1;
	}

	struct dirent *dir_info;
	char old_path[256] = {0};
	char old_name[256] = {0};
	while((dir_info = readdir(dir)) != NULL)
	{
		//printf("%s,d_name:%s\n",__FUNCTION__,dir_info->d_name);
		if( strcmp(dir_info->d_name, RECORD_DIR) == 0||
			strcmp(dir_info->d_name, RESULT_EVENT_DIR) == 0||
			strcmp(dir_info->d_name, RESULT_TOLL_DIR) == 0||
			strcmp(dir_info->d_name, RESULT_ILLEGAL_DIR) == 0)
		{
			char file_path[256] = {0};
			sprintf(file_path, "%s/%s", szIvaDataDir, dir_info->d_name);

			GetOldDateDir(file_path,old_path,old_name);
		}
	}

	closedir(dir);

	//printf("%s,oldest_path:%s\n",__FUNCTION__,old_path);

	// 删除最早的文件
	if(old_path[0] != '\0')
	{
		char cmd[128] = {0};
		char oldest_file[512] = {0};
		sprintf(cmd,"ls %s/* | head -n 1 ",old_path);

		//printf("cmd:%s\n",cmd);

		FILE *fp = popen(cmd, "r");
		if (fp == NULL)
		{
			LOG_DEBUG_FMT("Failed to popen command:%s\n", cmd);
			return -1;
		}

		if(fgets(oldest_file, sizeof(oldest_file)-1, fp) != NULL)
		{
			if(strlen(oldest_file) > 0)
			{
				if(oldest_file[strlen(oldest_file) - 1] == '\n')
				{
					oldest_file[strlen(oldest_file) - 1] = '\0';
				}
			}
		}
		pclose(fp);

		//printf("%s,oldest_file:%s\n",__FUNCTION__,oldest_file);
			
		if(access(oldest_file, F_OK) == 0)
		{
			int ret = unlink(oldest_file);
			LOG_DEBUG_FMT("unlink %s = %d", oldest_file,ret);
		}
		else
		{
			LOG_ERROR_FMT("access(%s) failed", oldest_file);
		}

		return 0;
	}
#endif
	return -1;
}

static void * DiskScanThread(void * p)
{
	DiskAllProbe tLastProbeDisk = {0};

	int iRet = -1;
	while(DeviceManager::GetInstance()->m_bExitThread == false)
	{
		DiskAllProbe tAllProbeDisk;
		memset(&tAllProbeDisk, 0, sizeof(DiskAllProbe));
		ProbeDisk(&tAllProbeDisk);

		DeviceManager::GetInstance()->SetAllDiskInfo(&tAllProbeDisk);

		DeviceManager::GetInstance()->CheckMountPoint();

		sleep(5);
	}

	pthread_exit(p);
	return p;
}

static int MoveOldestFile(const char * source_dir, const char * target_dir)
{
	if (source_dir == NULL || target_dir == NULL)
	{
		return -1;
	}

	char szIvaDataDir[512] = {0};
	if(source_dir[strlen(source_dir) - 1] != '/')
		sprintf(szIvaDataDir, "%s/%s", source_dir, IDENTIFY_DIR);
	else
		sprintf(szIvaDataDir, "%s%s", source_dir, IDENTIFY_DIR);

	char szDownloadDir[512] = {0};
	if(target_dir[strlen(target_dir) - 1] != '/')
		sprintf(szDownloadDir, "%s/%s", target_dir, DOWNLOAD_DIR);
	else
		sprintf(szDownloadDir, "%s%s", target_dir, DOWNLOAD_DIR);

#ifndef WIN32
	// 获取根目录属性
	struct stat statbuf;
	if(lstat(szIvaDataDir, &statbuf) != 0)
	{
		LOG_ERROR_FMT("lstat %s failed,error:%s",szIvaDataDir,strerror(errno));
		return -1;
	}

	// 遍历 得到日期最早的文件夹
	DIR * dir = opendir(szIvaDataDir);
	if(dir == NULL)
	{
		LOG_ERROR_FMT("opendir %s failed", szIvaDataDir);
		return -1;
	}

	struct dirent *dir_info;
	char old_path[256] = {0};
	char old_name[256] = {0};
	while((dir_info = readdir(dir)) != NULL)
	{
		//printf("%s,d_name:%s\n",__FUNCTION__,dir_info->d_name);
		if( strcmp(dir_info->d_name, RECORD_DIR) == 0||
			strcmp(dir_info->d_name, RESULT_EVENT_DIR) == 0||
			strcmp(dir_info->d_name, RESULT_TOLL_DIR) == 0||
			strcmp(dir_info->d_name, RESULT_ILLEGAL_DIR) == 0)
		{
			char file_path[256] = {0};
			sprintf(file_path, "%s/%s", szIvaDataDir, dir_info->d_name);

			GetOldDateDir(file_path, old_path, old_name);
		}
	}

	closedir(dir);

	//printf("%s,oldest_path:%s\n",__FUNCTION__,old_path);

	// 删除最早的文件
	if(old_path[0] != '\0')
	{
		char cmd[128] = {0};
		char oldest_file[512] = {0};
		sprintf(cmd,"ls %s/* | head -n 1 ",old_path);

		//printf("cmd:%s\n",cmd);

		FILE *fp = popen(cmd, "r");
		if (fp == NULL)
		{
			LOG_DEBUG_FMT("Failed to popen command:%s\n", cmd);
			return -1;
		}

		if(fgets(oldest_file, sizeof(oldest_file)-1, fp) != NULL)
		{
			if(strlen(oldest_file) > 0)
			{
				if(oldest_file[strlen(oldest_file) - 1] == '\n')
				{
					oldest_file[strlen(oldest_file) - 1] = '\0';
				}
			}
		}
		pclose(fp);

		//printf("%s,oldest_file:%s\n",__FUNCTION__,oldest_file);
		if (IsFileUsed(oldest_file))
		{
			return -1;
		}
		else
		{
			char szTargetFullPath[1024] = {0};
			sprintf(szTargetFullPath,"%s%s/", szDownloadDir, old_path+strlen(szIvaDataDir));
			OAL_MakeDirByPath(szTargetFullPath);

			if (CopyFile(oldest_file, szTargetFullPath) == 0)
			{
				unlink(oldest_file);
				LOG_DEBUG_FMT("MoveFile %s -> %s", oldest_file, szTargetFullPath);
				return 0;
			}
		}
	}
#endif
	return -1;
}

static void * DiskFullCheckThread(void * p)
{
	int iRet = -1;
	char szRootPath[512] = {0};
	StoreFullStatusT  tFullStat = {false, -1};

	while(DeviceManager::GetInstance()->m_bExitThread == false)
	{
		DiskFullStrategy tStrategy = {0};
		SystemProfile::GetInstance()->GetDiskStategyInfo(&tStrategy);

		// 检测满盘并处理
		DeviceManager::GetInstance()->CheckFullStrategy();

		// 存储路径是否改变
		const char * pszRoot = DeviceManager::GetInstance()->GetRootPath();
		if (strcmp(pszRoot, szRootPath) != 0)
		{
			char szRoot[512] = {0};
			if(pszRoot[strlen(pszRoot) - 1] != '/')
				sprintf(szRoot, "%s/%s", pszRoot, IDENTIFY_DIR);
			else
				sprintf(szRoot, "%s%s", pszRoot, IDENTIFY_DIR);

			MQ_Analy_RootPath(szRoot);
			MQ_Record_RootPath(szRoot);
			strcpy(szRootPath, pszRoot);
		}

		// 存储状态是否改变
		StoreFullStatusT  tNowStat = {0};
		tNowStat.bFull = DeviceManager::GetInstance()->PathIsFull(pszRoot);;
		tNowStat.iHow2DO = tStrategy.iHow2DO;
		if (tNowStat.bFull != tFullStat.bFull || 
			tNowStat.iHow2DO != tFullStat.iHow2DO)
		{
			tFullStat.bFull = tNowStat.bFull;
			tFullStat.iHow2DO = tNowStat.iHow2DO;
			MQ_Analy_FullStatus(&tFullStat);
			MQ_Record_FullStatus(&tFullStat);
			LOG_DEBUG_FMT("path:%s bFull:%s, How2DO:%d", pszRoot, 
				tFullStat.bFull?"YES":"NO",tFullStat.iHow2DO);
		}

		sleep(5);
	}

	pthread_exit(p);
	return p;
}

static void * DataAutoCopyThread(void * p)
{
	int iRet = -1;

	while(DeviceManager::GetInstance()->m_bExitThread == false)
	{
		// 检测满盘并处理
		iRet = DeviceManager::GetInstance()->DoDataCopy();
		if (iRet == 0)
		{
			usleep(10);
		}
		else
		{
			sleep(1);

		}
	}

	pthread_exit(p);
	return p;
}


DeviceManager* DeviceManager::m_pInstance = NULL;

int DeviceManager::Initialize()
{
	if( NULL == m_pInstance)
	{
		m_pInstance = new DeviceManager();
		m_pInstance->Run();
	}
	return (m_pInstance == NULL ? -1 : 0);
}

void DeviceManager::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

DeviceManager* DeviceManager::GetInstance()
{
	return m_pInstance;
}

DeviceManager::DeviceManager()
{
	pthread_mutex_init(&m_mutex,NULL);
	SystemProfile::GetInstance()->GetDiskUsageCfg(&m_tDiskUsage);
	memset(&m_tAllDisk, 0, sizeof(DiskAllProbe));
}

DeviceManager::~DeviceManager()
{
	m_bExitThread = true;
	pthread_join(m_scan_thread,NULL);
	pthread_join(m_fullcheck_thread,NULL);
	pthread_join(m_copy_thread,NULL);

	pthread_mutex_destroy(&m_mutex);
}

int DeviceManager::Run()
{
	m_bExitThread = false;
	pthread_create(&m_scan_thread, NULL, DiskScanThread, this);
	pthread_create(&m_fullcheck_thread, NULL, DiskFullCheckThread, this);
	pthread_create(&m_copy_thread, NULL, DataAutoCopyThread, this);
	return 0;
}

int DeviceManager::GetAllDiskShow(AllDiskShow* pInfo)
{
	if(pInfo == NULL)
	{
		return -1;
	}

	GetDiskShowByType(DISK_ROM, &pInfo->tRom);
	GetDiskShowByType(DISK_HDD, &pInfo->tHDD);
	GetDiskShowByType(DISK_SD,  &pInfo->tSD);
	GetDiskShowByType(DISK_USB, &pInfo->tUSB);
	return 0;
}

int DeviceManager::SetUsageType( int iDiskType, int iUseType )
{
	pthread_mutex_lock(&m_mutex);
	if (iDiskType == DISK_SD)
	{
		m_tDiskUsage.iSdUseType = iUseType;
	}
	else if (iDiskType == DISK_HDD)
	{
		m_tDiskUsage.iHddUseType = iUseType;
	}
	else if (iDiskType == DISK_USB)
	{
		m_tDiskUsage.iUsbUseType = iUseType;
	}
	pthread_mutex_unlock(&m_mutex);
	SystemProfile::GetInstance()->SetDiskUsageCfg(&m_tDiskUsage);
	return 0;
}

int DeviceManager::GetUsageType( int iDiskType )
{
	int iUseType = DISK_USE_DISABLE;
	pthread_mutex_lock(&m_mutex);
	if (iDiskType == DISK_SD)
	{
		iUseType = m_tDiskUsage.iSdUseType;
	}
	else if (iDiskType == DISK_HDD)
	{
		iUseType = m_tDiskUsage.iHddUseType;
	}
	else if (iDiskType == DISK_USB)
	{
		iUseType = m_tDiskUsage.iUsbUseType;
	}
	else if (iDiskType == DISK_ROM)
	{
		iUseType = DISK_USE_SYSTEM;
	}
	pthread_mutex_unlock(&m_mutex);

	return iUseType;
}

int DeviceManager::SetAllDiskInfo(const DiskAllProbe* pInfo)
{
	if(pInfo == NULL)
	{
		return -1;
	}
	
	if(memcmp(pInfo, &m_tAllDisk, sizeof(DiskAllProbe))!=0)
	{
		pthread_mutex_lock(&m_mutex);
		memcpy(&m_tAllDisk, pInfo, sizeof(DiskAllProbe));
		pthread_mutex_unlock(&m_mutex);
		
		for(int i = 0; i < m_tAllDisk.disk_cnt; i++)
		{
			DEVICE * dev = &(m_tAllDisk.disk[i]);
			printf("\n===================================================\n");
			printf("-DISK : %s\n", dev->path);
			printf("-MODE : %s\n", dev->model);
			printf("-TYPE : %s\n", PrintDiskType(dev->disktype));
			printf("-SIZE : %s\n", PrintSize(dev->totalsize));

			for(int j = 0; j < dev->part_cnt; j++)
			{
				printf("---------------------------------------------------\n");
				PARTITION *part = &(dev->part[j]);
				printf("|-PART  : %s\n", part->path);
				printf("|-TYPE  : %s\n", PrintDiskType(part->disktype));
				printf("|-PART  : %s\n", part->type);
				printf("|-FORMAT: %s\n", part->fstype);
				printf("|-SIZE  : %s\n", PrintSize(part->totalsize));
				printf("|-MOUNT : %s\n", part->mountpoint);
			}
			printf("===================================================\n");
		}
	}

	return 0;
}

int DeviceManager::GetDiskShowByType(int iDiskType, DiskShow * pInfo)
{
	if (pInfo == NULL)
	{
		return -1;
	}

	PARTITION tPart = {0};
	int iRet = GetPartInfoByType(iDiskType, &tPart);
	if (iRet != 0)
	{
		// 错误-1 无磁盘-2 无分区-3
		strcpy(pInfo->szPath, "");
		strcpy(pInfo->szFormat, "UNKNOWN");
		pInfo->uTotalSize = 0;
		pInfo->uFreeSize = 0;
		pInfo->iUseType = GetUsageType(iDiskType);
		pInfo->iStat = (iRet==-3?DISK_STAT_UNAVAILABLE:DISK_STAT_NOTFOUND);
		return 0;
	}
	else
	{
		strcpy(pInfo->szPath, tPart.path);
		strcpy(pInfo->szFormat, tPart.fstype);
		pInfo->iUseType = GetUsageType(iDiskType);
		if (strlen(tPart.mountpoint)>0)
		{
			GetMountPointSize(tPart.mountpoint, pInfo->uTotalSize,pInfo->uFreeSize);
			pInfo->iStat = DISK_STAT_OK;
		}
		else
		{
			pInfo->uTotalSize = tPart.totalsize;
			pInfo->uFreeSize = 0;
			pInfo->iStat = DISK_STAT_UNMOUNT;
		}
		return 0;
	}
}

int DeviceManager::GetPartInfoByMount(const char * mount, PARTITION* pInfo)
{
	if(pInfo == NULL || mount == NULL || strlen(mount) < 1)
	{
		return -1;
	}

	int iRet = -1;
	pthread_mutex_lock(&m_mutex);
	for (int i = 0; i < m_tAllDisk.disk_cnt; i++)
	{
		for (int j = 0; j < m_tAllDisk.disk[i].part_cnt; j++)
		{
			if(strcmp(m_tAllDisk.disk[i].part[j].mountpoint, mount) == 0)
			{
				iRet = 0;
				memcpy(pInfo, &(m_tAllDisk.disk[i].part[j]), sizeof(PARTITION));
				break;
			}
		}

		if (iRet == 0)
		{
			break;
		}
	}
	pthread_mutex_unlock(&m_mutex);
	return iRet;
}

// 错误-1 无磁盘 -2 无分区 -3
int DeviceManager::GetPartInfoByType( int iDiskType, PARTITION* pInfo )
{
	if (pInfo == NULL)
	{
		return -1;
	}

	const char * pRightMP = GetRigthMountPoint(iDiskType);
	if (pRightMP == NULL)
	{
		return -1;
	}

	// 第一步 查找已经正确挂载的分区
	PARTITION tPart = {0};
	int iRet = GetPartInfoByMount(pRightMP, &tPart);
	if (iRet >= 0)
	{
		if (tPart.disktype != iDiskType)
		{
			LOG_ERROR_FMT("Device %s mount wrong path, Who Did It?",tPart.path);
			//return -1;
		}
		else
		{
			memcpy(pInfo, &tPart, sizeof(PARTITION));
			return 0;
		}
	}

	// 第二步 找出容量最大的NTFS分区
	int iBestD = -1, iBestP = -1;
	u64 uMaxSize = 0;
	pthread_mutex_lock(&m_mutex);
	for (int i = 0; i < m_tAllDisk.disk_cnt; i++)
	{
		if (m_tAllDisk.disk[i].disktype == iDiskType)
		{
			for (int j = 0; j < m_tAllDisk.disk[i].part_cnt; j++)
			{
				if (m_tAllDisk.disk[i].part[j].totalsize < (u64)50*(u64)1024*(u64)1024)
				{
					continue;// 小于50M的分区，不合适
				}

				if (strcmp(m_tAllDisk.disk[i].part[j].fstype, "ntfs") == 0)
				{
					if (uMaxSize < m_tAllDisk.disk[i].part[j].totalsize)
					{
						iBestD = i;
						iBestP = j;
					}
				}
			}
		}		
	}
	pthread_mutex_unlock(&m_mutex);
	if (iBestD >= 0 && iBestP >= 0)
	{
		memcpy(pInfo, &m_tAllDisk.disk[iBestD].part[iBestP], sizeof(PARTITION));
		return 0;
	}

	// 第三步 找出容量最大的可用分区
	iBestD = -1; iBestP = -1;
	uMaxSize = 0;
	pthread_mutex_lock(&m_mutex);
	for (int i = 0; i < m_tAllDisk.disk_cnt; i++)
	{
		if (m_tAllDisk.disk[i].disktype == iDiskType)
		{
			for (int j = 0; j < m_tAllDisk.disk[i].part_cnt; j++)
			{
				if (m_tAllDisk.disk[i].part[j].totalsize < (u64)50*(u64)1024*(u64)1024)
				{
					continue;// 小于50M的分区，不合适
				}

				if (uMaxSize < m_tAllDisk.disk[i].part[j].totalsize)
				{
					iBestD = i;
					iBestP = j;
				}
			}
		}		
	}
	pthread_mutex_unlock(&m_mutex);
	if (iBestD >= 0 && iBestP >= 0)
	{
		memcpy(pInfo, &m_tAllDisk.disk[iBestD].part[iBestP], sizeof(PARTITION));
		return 0;
	}

	// 第三步 找出容量最大相应的磁盘
	iBestD = -1;
	uMaxSize = 0;
	pthread_mutex_lock(&m_mutex);
	for (int i = 0; i < m_tAllDisk.disk_cnt; i++)
	{
		if (m_tAllDisk.disk[i].disktype == iDiskType)
		{
			if (m_tAllDisk.disk[i].totalsize < 50*1024*1024)
			{
				continue;// 小于50M的磁盘，不合适
			}

			if (uMaxSize < m_tAllDisk.disk[i].totalsize)
			{
				iBestD = i;
			}
		}		
	}
	pthread_mutex_unlock(&m_mutex);
	if (iBestD > 0)
	{
		return -3;
	}
	else
	{
		return -2;
	}
}

int DeviceManager::CheckMountPoint()
{
	int iRet = 0;
	if (m_tDiskUsage.iUsbUseType == DISK_USE_STORE || m_tDiskUsage.iUsbUseType == DISK_USE_COPY)
	{
		// 检查挂载点上是否已经挂载 （其他设备挂载错了卸载，挂载对了通过）
		PARTITION tPartition = {0};
		iRet = GetPartInfoByMount(MOUNT_POINT_USB, &tPartition);
		if (iRet == 0)
		{
			if(tPartition.disktype != DISK_USB)
			{
				LOG_ERROR_FMT("The usb path %s is mounted by %s device", MOUNT_POINT_USB, PrintDiskType(tPartition.disktype));
				umountDevice(tPartition.path);
				return 0; //每次做件事，等重新调整了设备信息以后，会再加入这个函数
			}
			else
			{
				//LOG_DEBUG_FMT("USB device is mounted on %s, OK", MOUNT_POINT_USB);
			}
		}
		else
		{
			// 没有挂载，找出设备
			PARTITION tPart = {0};
			int iRet = GetPartInfoByType(DISK_USB, &tPart);
			if (iRet != 0)
			{
				LOG_DEBUG_FMT("USB device is not found");
			}
			else
			{
				if(strcmp(tPart.mountpoint, MOUNT_POINT_USB)==0)
				{
					// 已经正确挂载，这个分支不应该执行的
					//LOG_DEBUG_FMT("USB device is mounted on %s, OK", MOUNT_POINT_USB);
				}
				else if(strlen(tPart.mountpoint) > 0)
				{
					// 自己挂载错位置了，卸载
					LOG_ERROR_FMT("USB device is mounted on %s, not %s, Error", tPart.mountpoint,MOUNT_POINT_USB);
					umountDevice(tPart.path);
					return 0;//每次做件事，等重新调整了设备信息以后，会再加入这个函数
				}
				else
				{
					// 自己没挂载 挂载
					LOG_ERROR_FMT("USB device is unmounted, need %s, Error", MOUNT_POINT_USB);
					mountDevice(tPart.path, MOUNT_POINT_USB);
					return 0;//每次做件事，等重新调整了设备信息以后，会再加入这个函数
				}
			}
		}
	}

	if (m_tDiskUsage.iSdUseType == DISK_USE_STORE || m_tDiskUsage.iSdUseType == DISK_USE_COPY)
	{
		// 检查挂载点上是否已经挂载 （其他设备挂载错了卸载，挂载对了通过）
		PARTITION tPartition = {0};
		iRet = GetPartInfoByMount(MOUNT_POINT_SD, &tPartition);
		if (iRet == 0)
		{
			if(tPartition.disktype != DISK_SD)
			{
				LOG_ERROR_FMT("The SD path %s is mounted by %s device", MOUNT_POINT_SD, PrintDiskType(tPartition.disktype));
				umountDevice(tPartition.path);
				return 0; //每次做件事，等重新调整了设备信息以后，会再加入这个函数
			}
			else
			{
				//LOG_DEBUG_FMT("SD device is mounted on %s, OK", MOUNT_POINT_SD);
			}
		}
		else
		{
			// 没有挂载，找出设备
			PARTITION tPart = {0};
			int iRet = GetPartInfoByType(DISK_SD, &tPart);
			if (iRet != 0)
			{
				LOG_DEBUG_FMT("SD device is not found");
			}
			else
			{
				if(strcmp(tPart.mountpoint, MOUNT_POINT_SD)==0)
				{
					// 已经正确挂载，这个分支不应该执行的
					//LOG_DEBUG_FMT("USB device is mounted on %s, OK", MOUNT_POINT_SD);
				}
				else if(strlen(tPart.mountpoint) > 0)
				{
					// 自己挂载错位置了，卸载
					LOG_ERROR_FMT("SD device is mounted on %s, not %s, Error", tPart.mountpoint,MOUNT_POINT_SD);
					umountDevice(tPart.path);
					return 0;//每次做件事，等重新调整了设备信息以后，会再加入这个函数
				}
				else
				{
					// 自己没挂载 挂载
					LOG_ERROR_FMT("SD device is unmounted, need %s, Error", MOUNT_POINT_SD);
					mountDevice(tPart.path, MOUNT_POINT_SD);
					return 0;//每次做件事，等重新调整了设备信息以后，会再加入这个函数
				}
			}
		}
	}

	if (m_tDiskUsage.iHddUseType == DISK_USE_STORE)
	{
		// 检查挂载点上是否已经挂载 （其他设备挂载错了卸载，挂载对了通过）
		PARTITION tPartition = {0};
		iRet = GetPartInfoByMount(MOUNT_POINT_HDD, &tPartition);
		if (iRet == 0)
		{
			if(tPartition.disktype != DISK_HDD)
			{
				LOG_ERROR_FMT("The HDD path %s is mounted by %s device", MOUNT_POINT_HDD, PrintDiskType(tPartition.disktype));
				umountDevice(tPartition.path);
				return 0; //每次做件事，等重新调整了设备信息以后，会再加入这个函数
			}
			else
			{
				//LOG_DEBUG_FMT("HDD device is mounted on %s, OK", MOUNT_POINT_HDD);
			}
		}
		else
		{
			// 没有挂载，找出设备
			PARTITION tPart = {0};
			int iRet = GetPartInfoByType(DISK_HDD, &tPart);
			if (iRet != 0)
			{
				LOG_DEBUG_FMT("HDD device is not found");
			}
			else
			{
				if(strcmp(tPart.mountpoint, MOUNT_POINT_HDD)==0)
				{
					// 已经正确挂载，这个分支不应该执行的
					//LOG_DEBUG_FMT("HDD device is mounted on %s, OK", MOUNT_POINT_HDD);
				}
				else if(strlen(tPart.mountpoint) > 0)
				{
					// 自己挂载错位置了，卸载
					LOG_ERROR_FMT("HDD device is mounted on %s, not %s, Error", tPart.mountpoint,MOUNT_POINT_HDD);
					umountDevice(tPart.path);
					return 0;//每次做件事，等重新调整了设备信息以后，会再加入这个函数
				}
				else
				{
					// 自己没挂载 挂载
					LOG_ERROR_FMT("HDD device is unmounted, need %s, Error", MOUNT_POINT_HDD);
					mountDevice(tPart.path, MOUNT_POINT_HDD);
					return 0;//每次做件事，等重新调整了设备信息以后，会再加入这个函数
				}
			}
		}
	}

	return 0;
}

int DeviceManager::CheckFullStrategy()
{
	AllDiskShow tShow;
	GetAllDiskShow(&tShow);

	DiskFullStrategy tStrategy = {0};
	SystemProfile::GetInstance()->GetDiskStategyInfo(&tStrategy);

	// 按照USB->SD->HDD->ROM的顺序
	const char * pszRoot = NULL;
	if (tShow.tUSB.iUseType == DISK_USE_STORE && tShow.tUSB.iStat == DISK_STAT_OK)
	{
		pszRoot = GetRigthMountPoint(DISK_USB);
		if (PathIsFull(pszRoot) && tStrategy.iHow2DO == eFull2CyclicCovering)
		{
			if(RemoveOldestFile(pszRoot) != 0)
			{
				LOG_WARNS_FMT("DISK_USB is full,but can not remove anything...");
			}
		}
	}

	if (tShow.tSD.iUseType == DISK_USE_STORE && tShow.tSD.iStat == DISK_STAT_OK)
	{
		pszRoot = GetRigthMountPoint(DISK_SD);
		if (PathIsFull(pszRoot) && tStrategy.iHow2DO == eFull2CyclicCovering)
		{
			if(RemoveOldestFile(pszRoot) != 0)
			{
				LOG_WARNS_FMT("DISK_SD is full,but can not remove anything...");
			}
		}
	}

	if (tShow.tHDD.iUseType == DISK_USE_STORE && tShow.tHDD.iStat == DISK_STAT_OK)
	{
		pszRoot = GetRigthMountPoint(DISK_HDD);
		if (PathIsFull(pszRoot) && tStrategy.iHow2DO == eFull2CyclicCovering)
		{
			if(RemoveOldestFile(pszRoot) != 0)
			{
				LOG_WARNS_FMT("DISK_HDD is full,but can not remove anything...");
			}
		}
	}

	pszRoot = GetRigthMountPoint(DISK_ROM);
	if (PathIsFull(pszRoot) && tStrategy.iHow2DO == eFull2CyclicCovering)
	{
		if(RemoveOldestFile(pszRoot) != 0)
		{
			LOG_WARNS_FMT("DISK_ROM is full,but can not remove anything...");
		}
	}

	return 0;
}

int DeviceManager::DoDataCopy()
{
	AllDiskShow tShow;
	GetAllDiskShow(&tShow);
	u64 totalsize = 0;
	u64 freesize = 0;

	// 按照USB->SD的顺序检查
	const char * pszTargetDir = NULL;
	if (tShow.tUSB.iUseType == DISK_USE_COPY && tShow.tUSB.iStat == DISK_STAT_OK)
	{
		pszTargetDir = GetRigthMountPoint(DISK_USB);
		GetMountPointSize(pszTargetDir, totalsize, freesize);
		if (freesize < (u64)(5*1024*1024))
		{
			LOG_WARNS_FMT("The remaining space less than 5M on path %s , total:%s free:%s", 
				pszTargetDir,PrintSize(totalsize),PrintSize(freesize));
			pszTargetDir = NULL;
		}
	}

	if (pszTargetDir == NULL && tShow.tSD.iUseType == DISK_USE_COPY && tShow.tSD.iStat == DISK_STAT_OK)
	{
		pszTargetDir = GetRigthMountPoint(DISK_SD);
		GetMountPointSize(pszTargetDir, totalsize, freesize);
		if (freesize < (u64)(5*1024*1024))
		{
			LOG_WARNS_FMT("The remaining space less than 5M on path %s , total:%s free:%s", 
				pszTargetDir,PrintSize(totalsize),PrintSize(freesize));
			pszTargetDir = NULL;
		}
	}

	// 暂时不能用
	if (pszTargetDir == NULL)
	{
		return -1;
	}

	// 按照ROM->HDD->SD/USB的顺序拷贝
	const char * pszSourceDir = NULL;
	pszSourceDir = GetRigthMountPoint(DISK_ROM);
	if(MoveOldestFile(pszSourceDir, pszTargetDir) == 0)
	{
		return 0;
	}

	if (tShow.tHDD.iUseType == DISK_USE_STORE && tShow.tHDD.iStat == DISK_STAT_OK)
	{
		pszSourceDir = GetRigthMountPoint(DISK_HDD);
		if(MoveOldestFile(pszSourceDir, pszTargetDir) == 0)
		{
			return 0;
		}
	}

	if (tShow.tSD.iUseType == DISK_USE_STORE && tShow.tSD.iStat == DISK_STAT_OK)
	{
		pszSourceDir = GetRigthMountPoint(DISK_SD);
		if(MoveOldestFile(pszSourceDir, pszTargetDir) == 0)
		{
			return 0;
		}
	}

	if (tShow.tUSB.iUseType == DISK_USE_STORE && tShow.tUSB.iStat == DISK_STAT_OK)
	{
		pszSourceDir = GetRigthMountPoint(DISK_USB);
		if(MoveOldestFile(pszSourceDir, pszTargetDir) == 0)
		{
			return 0;
		}
	}

	return -1;
}

const char * DeviceManager::GetRootPath()
{
	AllDiskShow tShow;
	GetAllDiskShow(&tShow);

	// 按照USB->SD->HDD->ROM的顺序
	const char * pszRoot = NULL;
	if (tShow.tUSB.iUseType == DISK_USE_STORE && tShow.tUSB.iStat == DISK_STAT_OK)
	{
		pszRoot = GetRigthMountPoint(DISK_USB);
		if (!PathIsFull(pszRoot))
		{
			return pszRoot;
		}
	}

	if (tShow.tSD.iUseType == DISK_USE_STORE && tShow.tSD.iStat == DISK_STAT_OK)
	{
		pszRoot = GetRigthMountPoint(DISK_SD);
		if (!PathIsFull(pszRoot))
		{
			return pszRoot;
		}
	}

	if (tShow.tHDD.iUseType == DISK_USE_STORE && tShow.tHDD.iStat == DISK_STAT_OK)
	{
		pszRoot = GetRigthMountPoint(DISK_HDD);
		if (!PathIsFull(pszRoot))
		{
			return pszRoot;
		}
	}

	pszRoot = GetRigthMountPoint(DISK_ROM);
	return pszRoot;
}

bool DeviceManager::PathIsFull( const char *pszPath )
{
	if (pszPath == NULL)
	{
		return true;
	}

	DiskFullStrategy tStrategy = {0};
	SystemProfile::GetInstance()->GetDiskStategyInfo(&tStrategy);

	u64 totalsize;
	u64 freesize;
	GetMountPointSize(pszPath, totalsize, freesize);
	if (freesize < (u64)(tStrategy.uMinSize*1024*1024))
	{
		LOG_WARNS_FMT("path:%s is full, total:%s free:%s limit:%s", pszPath, 
			PrintSize(totalsize),
			PrintSize(freesize),
			PrintSize((u64)(tStrategy.uMinSize*1024*1024)));
		return true;
	}
	else
	{
// 		LOG_DEBUG_FMT("path:%s is not full, total:%s free:%s limit:%s", pszPath, 
// 			PrintSize(totalsize),
// 			PrintSize(freesize),
// 			PrintSize((u64)(tStrategy.uMinSize*1024*1024)));
		return false;
	}
}

int DeviceManager::FormatNtfs(int iDiskType)
{
	if (iDiskType < DISK_SD || iDiskType > DISK_HDD)
	{
		LOG_ERROR_FMT("%s disk is not support format", PrintDiskType(iDiskType));
		return -1;
	}

	return 0;

	PARTITION tPart = {0};
	if (GetPartInfoByType(iDiskType, &tPart) != 0)
	{
		LOG_ERROR_FMT("no %s disk to format", PrintDiskType(iDiskType));
		return -1;
	}
	
	// 对于已经挂载的分区，先卸载 然后格式化，再挂载
	if (strlen(tPart.mountpoint) > 0)
	{
		if (umountDevice(tPart.path) != 0)
		{
			LOG_WARNS_FMT("FORMAT %s failed, can not umount", tPart.path);
			//return -1;
		}			
	}

	// 格式化分区
	int iRet = diskformat2ntfs(tPart.path);
	if ( iRet != 0)
	{
		LOG_ERROR_FMT("FORMAT %s failed, can not format to ntfs", tPart.path);
	}

	// 对于已经挂载的分区，先卸载 然后格式化，再挂载
	if (strlen(tPart.mountpoint) > 0)
	{
		if (mountDevice(tPart.path, tPart.mountpoint) != 0)
		{
			LOG_WARNS_FMT("FORMAT %s, can't mount, err:[%s].", tPart.path, strerror(errno));
		}			
	}

	return iRet;
}

int DeviceManager::DeleteAllRecord()
{
	LOG_WARNS_FMT("DeleteAllRecord is not imcompleted");
	return 0;
}



