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
	/*�쳣��¼��д���ļ��� */
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
		printf("WSAStartup error��");
		return 1;
	}
#else
	//signal(SIGINT,  &exception_dump);// �ж��ź�
	signal(SIGSEGV, &exception_dump);// �ֶδ���
	signal(SIGBUS,  &exception_dump);// ���ߴ���
	signal(SIGQUIT, &exception_dump);// �˳��ź�
	signal(SIGILL,  &exception_dump);// �Ƿ�ָ��
	signal(SIGTRAP, &exception_dump);// ����/�ϵ��ж�
	signal(SIGFPE,  &exception_dump);// �����쳣
	signal(SIGABRT, &exception_dump);// ����
	signal(SIGPIPE, SIG_IGN);		 // ���� �ܵ����� һ��socket��Զ�˹رջ����
	signal(SIGHUP,  SIG_IGN);		 // ���� �����ź�
#endif

    theApp.exec();

#ifdef WIN32
	WSACleanup();
#endif
	return 0;
}
