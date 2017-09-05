/*****************************************************************************
模块 ： 系统日志实现                   
文件 ： oal_log.cpp
作者 ： Ripple Lee(libozjw@163.com)
版本 ： V1.0
日期 ： 2017-01-18
******************************************************************************/
#include "include/oal_log.h"
#include "include/oal_time.h"
#include "include/oal_file.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define OAL_LOG_DEF_PATH "Logfiles\\%s.log"
#else
#define OAL_LOG_DEF_PATH "./Logfiles/%s.log"
#endif

#define	OAL_LOG_BUFFER_SIZE 	(1024*10)

/*系统日志结构体*/
typedef struct _OalLog
{
	u32     u32CurSize;   /*当前文件大小*/
	u32     u32FileSize;  /*总的日志文件的大小*/
	FILE   *pfLog;        /*指向日志文件的指针*/
	char    szLogFileName[256];  /*日志文件名*/
	pthread_mutex_t tMutex;
}OalLogMan;

/*全局变量*/
OalLogMan  g_tLogMan;

/*该值等于0表示Log模块没有被初始化 大于1表示Log模块已经被初始化*/
int g_s32LogInitialized = 0;

/*Log消息输出的级别，通过LogConfig函数进行设置*/
int g_s32CurLogLevel = LOG_LEVEL_DEBUG;

OAL_API int LogInit(const char *ps8Name, u32 u32FileSize)
{
	int iResult = -1;
	if(ps8Name == NULL)
	{
		return FAILURE;
	}

	if(g_s32LogInitialized == 0)
	{
		memset(g_tLogMan.szLogFileName, 0, sizeof(g_tLogMan.szLogFileName));
		g_tLogMan.pfLog       = NULL;
		g_tLogMan.u32CurSize  = 0;
		g_tLogMan.u32FileSize = u32FileSize;
		pthread_mutex_init(&g_tLogMan.tMutex, NULL);

		char szWelcome[128] = {0};
		char szTimeNow[16] = {0};
		TimeFormatString(0, szTimeNow, sizeof(szTimeNow), eYMDHMS2);
		sprintf(szWelcome, "\r\nWelcome to use Oal log system, now is:%s\r\n", szTimeNow);

		sprintf(g_tLogMan.szLogFileName, OAL_LOG_DEF_PATH, ps8Name);

		OAL_MakeDirByPath(g_tLogMan.szLogFileName);

		// 以读写的方式打开文件
		g_tLogMan.pfLog = fopen(g_tLogMan.szLogFileName, "r+");
		if(g_tLogMan.pfLog == NULL)
		{
			// 创建文件
			g_tLogMan.pfLog = fopen(g_tLogMan.szLogFileName, "a+");
			if (g_tLogMan.pfLog)
			{
				// 写入欢迎语句
				g_tLogMan.u32CurSize += fwrite(szWelcome, sizeof(char), strlen(szWelcome), g_tLogMan.pfLog);
				fflush(g_tLogMan.pfLog);
				fclose(g_tLogMan.pfLog);

				// 以读写的方式打开文件，写位置到文件尾
				g_tLogMan.pfLog = fopen(g_tLogMan.szLogFileName, "r+");
				fseek(g_tLogMan.pfLog, 0, SEEK_END);

				iResult = SUCCESS;
			}
			else
			{
				printf("----Create Log File:%s failed\n", g_tLogMan.szLogFileName);
			}
		}
		else
		{
			// 以读写的方式打开文件，写位置到文件尾
			fseek(g_tLogMan.pfLog, 0, SEEK_END);
			
			// 获取文件大小
			g_tLogMan.u32CurSize = ftell(g_tLogMan.pfLog);

			// 超过限制大小 从头开始写
			if (g_tLogMan.u32CurSize >= g_tLogMan.u32FileSize)
			{
				fseek(g_tLogMan.pfLog, 0, SEEK_SET);
				g_tLogMan.u32CurSize = 0;
			}

			// 写入欢迎语句
			g_tLogMan.u32CurSize += fwrite(szWelcome, sizeof(char), strlen(szWelcome), g_tLogMan.pfLog);
			fflush(g_tLogMan.pfLog);
	
			iResult = SUCCESS;
		}
	}

	g_s32LogInitialized++;

	return iResult;
}

/*****************************************************************************
* 函数名称: LogUninit
* 函数功能: 结束系统日志
* 输入参数: 无
* 输出参数: 无
* 函数返回: 无
******************************************************************************/
OAL_API void LogUninit(void)
{
	if(g_s32LogInitialized > 0)
	{
		g_s32LogInitialized--;
		if(g_s32LogInitialized == 0)
		{
			if(NULL != g_tLogMan.pfLog)
			{
				char szTimeNow[16] = {0};
				TimeFormatString(0, szTimeNow, sizeof(szTimeNow), eYMDHMS2);
				char  szSayByeBye[512] = {0};
				sprintf(szSayByeBye,  "\n---- Date Time: %s-- ------- END  ----- \n\n",szTimeNow);

				pthread_mutex_lock(&g_tLogMan.tMutex);
				
				fwrite(szSayByeBye, sizeof(char), strlen(szSayByeBye), g_tLogMan.pfLog);
				fflush(g_tLogMan.pfLog);
				fclose(g_tLogMan.pfLog);
				g_tLogMan.pfLog = NULL;

				pthread_mutex_unlock(&g_tLogMan.tMutex);
				pthread_mutex_destroy(&g_tLogMan.tMutex);
			}
		}
	}
}

OAL_API int LogTrace(int iLogLevel, const char *ps8Format, ... )
{
	int iRet = FAILURE;
	char szLogDateTime[32] = {0};
	char szLogType[8] = {0};
	char szLogBuf[OAL_LOG_BUFFER_SIZE] = {0};
	int  s32Len;
	int	 s32Temp;

	if(NULL == ps8Format)
	{
		return FAILURE; /*无消息格式*/
	}

	va_list  args;

	if(g_s32LogInitialized > 0)
	{
		pthread_mutex_lock(&g_tLogMan.tMutex);
		if(g_tLogMan.pfLog == NULL)
		{
			goto CleanUp;
		}

		/*生成日期时间字符串*/
		TimeFormatString(0, szLogDateTime, sizeof(szLogDateTime), eYMDHMS1);

		/*判断日志级别*/
		memset(szLogType, 0, sizeof(szLogType));
		switch(iLogLevel)
		{
		case LOG_LEVEL_ERROR:
			strcpy(szLogType, "ERROR");
			break;
		case LOG_LEVEL_WARNS:
			strcpy(szLogType, "WARNS");
			break;
		case LOG_LEVEL_INFOS:
			strcpy(szLogType, "INFOS");
			break;
		case LOG_LEVEL_DEBUG:
			strcpy(szLogType, "DEBUG");
			break;
		case LOG_LEVEL_DUMPS:
			strcpy(szLogType, "DUMPS");
			break;
		default:
			break;
		}

		s32Len = sprintf(szLogBuf, "%s %s ",szLogDateTime, szLogType);
		
		va_start(args, ps8Format);

		s32Temp = vsnprintf(szLogBuf + s32Len, OAL_LOG_BUFFER_SIZE - s32Len - 2, ps8Format, args);

		va_end(args);

		if(0 > s32Temp)
		{
			goto CleanUp;
		}

		s32Len += s32Temp;
		szLogBuf[s32Len++] = '\n';
		szLogBuf[s32Len] = '\0';

		/*将系统日志写到文件里，当前文件大小大于等于总文件大小*/
		if(g_tLogMan.u32FileSize <= g_tLogMan.u32CurSize)
		{
			/*写完一个文件之后，从开头再次开始写*/
			fseek(g_tLogMan.pfLog, 0, SEEK_SET);
			g_tLogMan.u32CurSize = 0;
		}

		/*将缓冲区的内容写入到文件里*/
		s32Temp = fwrite(szLogBuf, sizeof(u8), s32Len, g_tLogMan.pfLog);

		/*---通过级别控制调试信息的输出---*/
		if(g_s32CurLogLevel >= iLogLevel)
		{
			printf("%s",szLogBuf);
		}

		if(s32Len == s32Temp)
		{
			g_tLogMan.u32CurSize += s32Temp;
			fflush(g_tLogMan.pfLog);
		}
		else
		{
			fclose(g_tLogMan.pfLog);
			g_tLogMan.pfLog = NULL;
		}

		iRet = SUCCESS;

CleanUp:

		pthread_mutex_unlock(&g_tLogMan.tMutex);
	}
	else
	{
		printf("---Error (%s|%d) :Fail to call Trace, LogInit first!\n", __FILE__, __LINE__);
	}

	return iRet;
}

OAL_API int LogConfig(int iLogLevel)
{
	if(iLogLevel < LOG_LEVEL_ERROR || iLogLevel > LOG_LEVEL_DUMPS)
	{
		LogTrace(LOG_LEVEL_ERROR, "(%s|%d) Incorrect log level : %d!", __FILE__, __LINE__, iLogLevel);
		return FAILURE;
	}

	g_s32CurLogLevel = iLogLevel;

	return SUCCESS;
}

#ifdef __cplusplus
}
#endif

