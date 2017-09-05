#include <stdio.h>
#include "application.h"

#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib, "WS2_32")
#else
#include <execinfo.h>
#include <signal.h>
#include "oal_time.h"
const char * PrintSigNo(int signo)
{
	switch (signo)
	{
	case SIGSEGV:
		return "SIGSEGV";
	case SIGBUS:
		return "SIGBUS";
	case SIGQUIT:
		return "SIGQUIT";
	case SIGILL:
		return "SIGILL";
	case SIGTRAP:
		return "SIGTRAP";
	case SIGFPE:
		return "SIGFPE";
	case SIGABRT:
		return "SIGABRT";
	default:
		return "UNKOWN";
	}
}
void exception_dump(int signo)
{
	/*异常记录会写入文件中 */
	FILE *fp = fopen("./Logfiles/master_execption.txt", "ab+");

	void *array[100];
	size_t size = backtrace (array, 100);
	char ** strings = backtrace_symbols(array, size);

	char szTime[40] = {0};
	TimeFormatString(time(NULL), szTime, 40, eYMDHMS1);

	char szExpMsg[512] = {0};
	sprintf(szExpMsg,"\r\n%s signal=%d:%s\r\n",	szTime, signo, PrintSigNo(signo));

	printf ("%s", szExpMsg);
	if(fp)
	{
		fwrite(szExpMsg, 1, strlen(szExpMsg), fp);
	}

	for (size_t i = 0; i < size; i++)
	{
		char szExpMsg[1024] = {0};
		sprintf(szExpMsg, "%s\r\n", strings[i]);
		printf ("%s", szExpMsg);
		if(fp)
		{
			fwrite(szExpMsg, 1, strlen(szExpMsg), fp);
		}
	}

	free(strings);

	if(fp)
	{
		fclose(fp);
	}
	exit(0);
}
#endif

int main(int argc,char** argv)
{
    Application theApp;
	theApp.init();

#ifdef WIN32
	WSADATA wd;
	if(WSAStartup(MAKEWORD(2,2),&wd)  != 0)
	{
		printf("WSAStartup error！");
		return 1;
	}
#else
	//signal(SIGINT,  &exception_dump);// 中断信号
	signal(SIGSEGV, &exception_dump);// 分段错误
	signal(SIGBUS,  &exception_dump);// 总线错误
	signal(SIGQUIT, &exception_dump);// 退出信号
	signal(SIGILL,  &exception_dump);// 非法指令
	signal(SIGTRAP, &exception_dump);// 跟踪/断点中断
	signal(SIGFPE,  &exception_dump);// 浮点异常
	signal(SIGABRT, &exception_dump);// 放弃
	signal(SIGPIPE, SIG_IGN);		 // 忽略 管道错误 一般socket被远端关闭会产生
	signal(SIGHUP,  SIG_IGN);		 // 忽略 挂起信号
#endif

    theApp.exec();

#ifdef WIN32
	WSACleanup();
#endif
	return 0;
}
