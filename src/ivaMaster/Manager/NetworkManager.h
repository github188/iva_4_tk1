#ifndef __NETWORK_INTERFACE__
#define __NETWORK_INTERFACE__
#include <pthread.h>
#include "oal_typedef.h"

#define MAX_INTERFACE_NUM 1

//������Ϣ����
typedef struct tagNetworkInfo
{
	char szMacAddr[MAC_LEN];    //mac��ַ
	char szLocalIP[IPV4_LEN];   //����ip
	char szNetMask[IPV4_LEN];   //��������
	char szGateway[IPV4_LEN];   //����
}IF_NETWORK_INFO;

enum 
{
	e10086,// �й��ƶ�
	e10010,// �й���ͨ
	e10000 // �й�����
};

typedef struct tagFourGCFG
{
	bool bOpen;
	int  iTelecomID;
}FourGCFG;

typedef struct tagFourGStat
{
	bool bOpen;
	bool bOn;
	int  iDbm;
}FourGStat;

enum 
{
	e2_4GHz,// 2.4GHz
	e5GHz	// 5GHz
};

typedef struct tagWlanHotCFG
{
	bool bEnable;
	char szSSID[128];
	char szPwd[128];
	int  iFrequency;
	int  iMaxClient;
}WlanHotCFG;


class NetworkManager
{
public:
	static NetworkManager* GetInstance();
	static int Initialize();
	static void UnInitialize();

public:
	int Run();

public:
	bool IfNetworkConnect(int ethi);
	int GetNetworkInfo(int ethi, IF_NETWORK_INFO* pInfo);
	int SetNetworkInfo(int ethi, const IF_NETWORK_INFO* pInfo);

	int GetDNS(char * pszMain, char *pszStandby);
	int SetDNS(const char * pszMain, const char *pszStandby);

private:
	NetworkManager();
	~NetworkManager();
	static NetworkManager* m_pInstance;

private:
	int ReadNetworkInfo(); // �������ж�ȡip�����������Լ�����
	int ReadDNSInfo();     // �������ж�ȡdns��Ϣ
	int ReadMacAddr();     // ��ȡmac��ַ

	int WriteNetworkInfo();
	int WriteDNSInfo();

	int CreateNewInterface(const char* szIP,const char* szMask,const char* szGateway);  //�����������ļ��������򴴽�
	int CreateNewResolve(const char* szDns);             //��dns���ò������򴴽�

	void DoActive(int ethi);//����������

public:
	bool m_bExitThread;

private:
	IF_NETWORK_INFO	 m_tNetworkInfo[MAX_INTERFACE_NUM];
	bool m_bIfConnect[MAX_INTERFACE_NUM];
	char m_szDNSAddr1[IPV4_LEN];   //DNS��ַ
	char m_szDNSAddr2[IPV4_LEN];   //DNS��ַ
	pthread_rwlock_t m_rwLock;

	pthread_t 	m_4g_thread;
public:
	int PPP_4G_ON(int iTelecomID);
	int PPP_4G_OFF();
	int PPP_4G_Stat(FourGStat * ptInfo);
};
#endif
