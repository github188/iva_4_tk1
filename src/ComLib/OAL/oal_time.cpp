/***************************************************************
模块 ： 时间相关                     
文件 ： oal_time.cpp
作者 ： Ripple Lee(libozjw@163.com)
版本 ： V1.0
日期 ： 2017-01-18
*****************************************************************/
#include "include/oal_time.h"

#ifdef __cplusplus
extern "C" {
#endif

OAL_API int TimeFormatString(time_t t, char *pString, int iLen, TimeFormatE eFormat)
{
	if (pString == NULL)
	{
		return FAILURE;
	}

	time_t tIn = t;
	if(tIn <= 0)
	{
		tIn = time(NULL);
	}

	struct tm *local_time = localtime(&tIn);
	if (local_time == NULL)
	{
		return FAILURE;
	}

	switch (eFormat)
	{
	case eYMDHMS1:
		sprintf(pString, "%04d-%02d-%02d %02d:%02d:%02d", 
			local_time->tm_year+1900, local_time->tm_mon+1, local_time->tm_mday,
			local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
		break;
	case eYMDHMS2:
		sprintf(pString, "%04d%02d%02d%02d%02d%02d", 
			local_time->tm_year+1900, local_time->tm_mon+1, local_time->tm_mday,
			local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
		break;
	case eYMDHMS3:
		sprintf(pString, "%04d-%02d-%02d_%02d:%02d:%02d", 
			local_time->tm_year+1900, local_time->tm_mon+1, local_time->tm_mday,
			local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
		break;
	case eYMD1:
		sprintf(pString, "%04d-%02d-%02d", 
			local_time->tm_year+1900, local_time->tm_mon+1, local_time->tm_mday);
		break;
	case eYMD2:
		sprintf(pString, "%04d%02d%02d", 
			local_time->tm_year+1900, local_time->tm_mon+1, local_time->tm_mday);
		break;
	case eHMS1:
		sprintf(pString, "%02d:%02d:%02d", 
			local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
		break;
	case eHMS2:
		sprintf(pString, "%02d%02d%02d", 
			local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
		break;
	default:
		sprintf(pString, "%04d-%02d-%02d %02d:%02d:%02d", 
			local_time->tm_year+1900, local_time->tm_mon+1, local_time->tm_mday,
			local_time->tm_hour, local_time->tm_min, local_time->tm_sec);
		break;
	}

	return SUCCESS;
}

OAL_API time_t String2Time( const char *pString, TimeFormatE eFormat )
{
	if (pString == NULL)
	{
		return -1;
	}

	int Y = 0,M = 0,D = 0,h = 0,m = 0,s = 0;
	int iCnt = 0;
	switch (eFormat)
	{
	case eYMDHMS1:
		iCnt = sscanf(pString,"%d-%d-%d %d:%d:%d",&Y,&M,&D,&h,&m,&s);
		if (iCnt != 6)
		{
			iCnt = sscanf(pString,"%04d-%02d-%02d %02d:%02d:%02d",&Y,&M,&D,&h,&m,&s);
		}
		if (iCnt != 6)
		{
			return -1;
		}
		break;
	case eYMDHMS2:
		iCnt = sscanf(pString,"%d%d%d%d%d%d",&Y,&M,&D,&h,&m,&s);
		if (iCnt != 6)
		{
			iCnt = sscanf(pString,"%04d%02d%02d%02d%02d%02d",&Y,&M,&D,&h,&m,&s);
		}
		if (iCnt != 6)
		{
			return -1;
		}
		break;
	case eYMDHMS3:
		iCnt = sscanf(pString,"%d-%d-%d_%d-%d-%d",&Y,&M,&D,&h,&m,&s);
		if (iCnt != 6)
		{
			iCnt = sscanf(pString,"%04d-%02d-%02d_%02d-%02d-%02d",&Y,&M,&D,&h,&m,&s);
		}
		if (iCnt != 6)
		{
			return -1;
		}
		break;
	case eYMD1:
		iCnt = sscanf(pString,"%d-%d-%d",&Y,&M,&D);
		if (iCnt != 3)
		{
			iCnt = sscanf(pString,"%04d-%02d-%02d",&Y,&M,&D);
		}
		if (iCnt != 3)
		{
			return -1;
		}
		break;
	default:
		iCnt = sscanf(pString,"%d-%d-%d %d:%d:%d",&Y,&M,&D,&h,&m,&s);
		if (iCnt != 6)
		{
			iCnt = sscanf(pString,"%04d-%02d-%02d %02d:%02d:%02d",&Y,&M,&D,&h,&m,&s);
		}
		if (iCnt != 6)
		{
			return -1;
		}
		break;
	}

	if (M < 1 || M > 12 ||
		D < 1 || D > 31 ||
		h < 0 || h > 23 ||
		m < 0 || m > 59 ||
		s < 0 || s > 59)
	{
		return -1;
	}

	struct tm t;
	t.tm_year = Y-1900;
	t.tm_mon  = M-1;
	t.tm_mday = D;
	t.tm_hour = h;
	t.tm_min  = m;
	t.tm_sec  = s;

	return mktime(&t);
}

OAL_API int CaculateWeekDay(int s32Year, int s32Month, int s32Day)
{
	if(s32Month == 1 || s32Month == 2)
	{
		s32Month += 12;
		s32Year--;
	}
	return (s32Day + 2 * s32Month + 3 * (s32Month + 1) / 5 + s32Year + s32Year / 4 - s32Year / 100 + s32Year / 400) % 7;
}

OAL_API int GetWeekDay( time_t t )
{
	struct tm *local_time = localtime(&t);
	if (local_time == NULL)
	{
		return FAILURE;
	}

	int iWeekDay = local_time->tm_wday;
	return iWeekDay;
}

OAL_API u32 GetSecTime( time_t t )
{
	struct tm *local_time = localtime(&t);
	if (local_time == NULL)
	{
		return FAILURE;
	}

	u32 iTimeNow = local_time->tm_hour * 60 * 60 + local_time->tm_min * 60 + local_time->tm_sec;
	return iTimeNow;
}

#ifdef WIN32
#include <windows.h>
OAL_API unsigned sleep(unsigned seconds)
{
	Sleep(seconds*1000);
	return 0;
}

OAL_API int usleep(unsigned usec)
{
	Sleep(usec/1000);
	return 0;
}

OAL_API int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;

	GetLocalTime(&wtm);
	tm.tm_year     = wtm.wYear - 1900;
	tm.tm_mon     = wtm.wMonth - 1;
	tm.tm_mday     = wtm.wDay;
	tm.tm_hour     = wtm.wHour;
	tm.tm_min     = wtm.wMinute;
	tm.tm_sec     = wtm.wSecond;
	tm. tm_isdst    = -1;
	clock = mktime(&tm);
	tp->tv_sec = (long)clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;

	return (0);
}
#else
OAL_API unsigned long GetTickCount()
{
	struct timespec ts;  

	clock_gettime(CLOCK_MONOTONIC, &ts);  

	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);  
}

#endif


#ifdef __cplusplus
}
#endif