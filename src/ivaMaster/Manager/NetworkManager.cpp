#include "NetworkManager.h"
#include <errno.h>
#include <string>
#include "oal_typedef.h"
#include "oal_regexp.h"
#include "oal_log.h"
#include "oal_file.h"
#include "oal_time.h"
#include "../Profile/SystemProfile.h"
#ifdef WIN32
#include <winsock2.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif
using namespace std;
#include "oal_log.h"
#include "oal_file.h"

#define DNS_CFG_FILE   			"/etc/resolv.conf"
#define DNS_CFG_FILE_TEMP 		"/etc/~resolv.conf"

#define ADDR_CFG_FILE   		"/etc/network/interfaces"
#define ADDR_CFG_FILE_TEMP   	"/etc/network/~interfaces"

static void * Daemon4GThread(void * p)
{
	int iRet = -1;
	FourGCFG theCfgUsed = {false, -1};

	LOG_DEBUG_FMT("4G Daemon Thread is running...");
	while(NetworkManager::GetInstance()->m_bExitThread == false)
	{
		FourGCFG theCfgNow;
		SystemProfile::GetInstance()->Get4GInfo(&theCfgNow);
		if (memcmp(&theCfgNow, &theCfgUsed, sizeof(FourGCFG)) != 0)
		{
			NetworkManager::GetInstance()->PPP_4G_OFF();
			memcpy(&theCfgUsed, &theCfgNow, sizeof(FourGCFG));
		}
		
		FourGStat theStat;
		NetworkManager::GetInstance()->PPP_4G_Stat(&theStat);

		if (theStat.bOpen != theStat.bOn)
		{
			if (theStat.bOpen)
			{
				NetworkManager::GetInstance()->PPP_4G_ON(theCfgUsed.iTelecomID);
			}
			else
			{
				NetworkManager::GetInstance()->PPP_4G_OFF();
			}
		}

		sleep(1);
	}

	pthread_exit(p);
	return p;
}

NetworkManager* NetworkManager::m_pInstance = NULL;

NetworkManager::NetworkManager()
{
	for (int i = 0; i < MAX_INTERFACE_NUM; i++)
	{
		memset(&m_tNetworkInfo[i], 0, sizeof(IF_NETWORK_INFO));
		strcpy(m_tNetworkInfo[i].szLocalIP, "0.0.0.0");
		strcpy(m_tNetworkInfo[i].szNetMask, "0.0.0.0");
		strcpy(m_tNetworkInfo[i].szGateway, "0.0.0.0");
		strcpy(m_tNetworkInfo[i].szMacAddr, "00.00.00.00.00.00");
		m_bIfConnect[i] = true;
	}

	strcpy(m_szDNSAddr1, "0.0.0.0");
	strcpy(m_szDNSAddr2, "0.0.0.0");

	pthread_rwlock_init(&m_rwLock, NULL);
}

NetworkManager::~NetworkManager()
{
	m_bExitThread = true;
	pthread_join(m_4g_thread, NULL);

	pthread_rwlock_destroy(&m_rwLock);
}

NetworkManager* NetworkManager::GetInstance()
{
	return m_pInstance;
}

int NetworkManager::Initialize()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new NetworkManager();
		m_pInstance->Run();
	}

	pthread_rwlock_wrlock(&m_pInstance->m_rwLock);
	m_pInstance->ReadNetworkInfo();
	m_pInstance->ReadDNSInfo();
	m_pInstance->ReadMacAddr();
	pthread_rwlock_unlock(&m_pInstance->m_rwLock);

	printf("===================Network=========================\n");
	for (int i = 0; i < MAX_INTERFACE_NUM; i++)
	{
		IF_NETWORK_INFO tNetwork;
		m_pInstance->GetNetworkInfo(i,&tNetwork);
		printf("eth%d\n",i);
		printf("IPV4: %s\n", tNetwork.szLocalIP);
		printf("MASK: %s\n", tNetwork.szNetMask);
		printf("GATE: %s\n", tNetwork.szGateway);
		printf("MAC : %s\n", tNetwork.szMacAddr);
	}

	char szDNSAddr1[IPV4_LEN] = {0};
	char szDNSAddr2[IPV4_LEN] = {0};
	m_pInstance->GetDNS(szDNSAddr1,szDNSAddr2);
	printf("DNS1: %s\n", szDNSAddr1);
	printf("DNS2: %s\n", szDNSAddr2);
	printf("===================================================\n");

	return 0;
}

void NetworkManager::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}


int NetworkManager::Run()
{
	m_bExitThread = false;
	pthread_create(&m_4g_thread, NULL, Daemon4GThread, this);
	return 0;
}

int NetworkManager::ReadNetworkInfo()
{
	//检查配置文件是否存在,若配置文件不存在，查看相应的临时配置是否存在，若存在，则采用临时文件替换
	if(access(ADDR_CFG_FILE, R_OK|W_OK) != 0)
	{
		if(access(ADDR_CFG_FILE_TEMP ,R_OK|W_OK) != 0)
		{
			LOG_ERROR_FMT("[ERROR] Network config file %s is not exist", ADDR_CFG_FILE);
			return -1;
		}
		
		rename(ADDR_CFG_FILE_TEMP, ADDR_CFG_FILE);
	}

	FILE* fp = fopen(ADDR_CFG_FILE, "r");
	if(NULL == fp)
	{
		LOG_ERROR_FMT("[ERROR] Open %s failed", ADDR_CFG_FILE);
		return -1;
	}

	int ethi = -1;
	char szBuffer[512] = {0};
	while(NULL != fgets(szBuffer, 511, fp))
	{
		//iface eth0 inet static
		int i = -1;
		int iCnt = sscanf(szBuffer,"iface eth%d ", &i);
		if(iCnt == 1 && i >=0 && i < MAX_INTERFACE_NUM)
		{
			ethi = i;
			continue;
		}

		if (ethi == -1)
		{
			continue;
		}

		string sentence = szBuffer;
		string::size_type pos = string::npos;
		pos = sentence.find("address");
		if(pos != string::npos)
		{
			string szAddr = sentence.substr(pos+7);
			string_trim(szAddr);
			strcpy(m_tNetworkInfo[ethi].szLocalIP, szAddr.c_str());
			continue;
		}

		pos = sentence.find("netmask");
		if(pos != string::npos)
		{
			string szAddr = sentence.substr(pos+7);
			string_trim(szAddr);
			strcpy(m_tNetworkInfo[ethi].szNetMask, szAddr.c_str());
			continue;
		}

		pos = sentence.find("gateway");
		if(pos != string::npos)
		{
			string szAddr = sentence.substr(pos+7);
			string_trim(szAddr);
			strcpy(m_tNetworkInfo[ethi].szGateway, szAddr.c_str());
			continue;
		}
	}
	
	fclose(fp);
	
	return  0;
}

int NetworkManager::ReadDNSInfo()
{
	//检查配置文件是否存在,若配置文件不存在，查看相应的临时配置是否存在，若存在，则采用临时文件替换
	if(access(DNS_CFG_FILE, R_OK|W_OK) != 0)
	{
		if(access(DNS_CFG_FILE_TEMP ,R_OK|W_OK) != 0)
		{
			LOG_ERROR_FMT("[ERROR] DNS config file %s is not exist\n", DNS_CFG_FILE);
			return -1;
		}
		
		rename(DNS_CFG_FILE_TEMP, DNS_CFG_FILE);
	}

	// 只读打开resolv.conf文件
	FILE* fp = fopen(DNS_CFG_FILE, "r");
	if(NULL == fp)
	{
		LOG_ERROR_FMT("[ERROR] Open %s failed\n", DNS_CFG_FILE);
		return -1;
	}

	int iFind = 0;
	char szBuffer[512] = {0};
	while(NULL != fgets(szBuffer, 511, fp))
	{
		string sentence = szBuffer;
		string::size_type pos = string::npos;
		pos = sentence.find("nameserver");
		if(pos != string::npos)
		{
			string szAddr = sentence.substr(pos+10);
			string_trim(szAddr);
			if (iFind == 0)
			{
				strcpy(m_szDNSAddr1, szAddr.c_str());
				iFind++;
			}
			else
			{
				strcpy(m_szDNSAddr2, szAddr.c_str());
			}
			continue;
		}
	}
	
	fclose(fp);
	
	return 0;

}

int NetworkManager::ReadMacAddr()
{
	int nSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(nSocket < 0)
	{
		return -1;
	}

	for(int i = 0; i < MAX_INTERFACE_NUM; i++)
	{
#ifndef WIN32
		struct ifreq ifr;
		memset(&ifr,0,sizeof(ifreq));

		sprintf(ifr.ifr_name, "eth%d", i);
		if (ioctl(nSocket, SIOCGIFHWADDR, &ifr)< 0)   
		{   
			close(nSocket);
			continue;   
		}

		struct sockaddr sMac;   
		memcpy(&sMac,&ifr.ifr_addr,sizeof(sMac));   

		unsigned char mac[6]; 
		memcpy(mac, sMac.sa_data, sizeof(mac));   

		sprintf(m_tNetworkInfo[i].szMacAddr,"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
#endif
	}

#ifdef WIN32
	closesocket(nSocket);
#else
	close(nSocket);
#endif

	return 0;
}

bool NetworkManager::IfNetworkConnect( int ethi )
{
	if (ethi < 0 || ethi > MAX_INTERFACE_NUM-1)
	{
		return false;
	}
	return m_bIfConnect[ethi];
}

int NetworkManager::GetNetworkInfo(int ethi, IF_NETWORK_INFO* pInfo)
{
	if (pInfo == NULL || ethi < 0 || ethi > MAX_INTERFACE_NUM-1)
	{
		return -1;
	}
	
	pthread_rwlock_rdlock(&m_rwLock);
	memset(pInfo,0,sizeof(IF_NETWORK_INFO));
	memcpy(pInfo,&m_tNetworkInfo[ethi],sizeof(IF_NETWORK_INFO));
	pthread_rwlock_unlock(&m_rwLock);
	return 0;
}

int NetworkManager::SetNetworkInfo(int ethi, const IF_NETWORK_INFO* pInfo)
{
	if (pInfo == NULL || ethi < 0 || ethi > MAX_INTERFACE_NUM-1)
	{
		return -1;
	}

	if(!IsIPv4Addr(pInfo->szLocalIP))
	{
		LOG_ERROR_FMT("[illegal], ipaddr:%s\n", pInfo->szLocalIP);
		return -1;
	}

	if(!IsIPv4Addr(pInfo->szNetMask))
	{
		LOG_ERROR_FMT("[illegal], netmask:%s\n", pInfo->szNetMask);
		return -1;
	}

	if(!IsIPv4Addr(pInfo->szGateway))
	{
		LOG_ERROR_FMT("[illegal], gateway:%s\n", pInfo->szGateway);
		return -1;
	}


	// 配置IP 掩码 网关
	if( strcmp(m_tNetworkInfo[ethi].szLocalIP, pInfo->szLocalIP) != 0 ||
		strcmp(m_tNetworkInfo[ethi].szNetMask, pInfo->szNetMask) != 0 ||
		strcmp(m_tNetworkInfo[ethi].szGateway, pInfo->szGateway) != 0)
	{
		pthread_rwlock_wrlock(&m_rwLock);
		memcpy(m_tNetworkInfo[ethi].szLocalIP,pInfo->szLocalIP,sizeof(m_tNetworkInfo[ethi].szLocalIP));
		memcpy(m_tNetworkInfo[ethi].szNetMask,pInfo->szNetMask,sizeof(m_tNetworkInfo[ethi].szNetMask));
		memcpy(m_tNetworkInfo[ethi].szGateway,pInfo->szGateway,sizeof(m_tNetworkInfo[ethi].szGateway));
		pthread_rwlock_unlock(&m_rwLock);

		int iRet = WriteNetworkInfo();
		if(iRet<0)
		{
			return -1;
		}		

		DoActive(ethi);
	}
	
	return 0;
}

int NetworkManager::GetDNS( char * pszMain, char *pszStandby )
{
	if (pszMain)
	{
		strcpy(pszMain, m_szDNSAddr1);
	}
	if (pszStandby)
	{
		strcpy(pszStandby, m_szDNSAddr2);
	}
	return 0;
}

int NetworkManager::SetDNS( const char * pszMain, const char *pszStandby )
{
	char szDNSAddr1[IPV4_LEN];   //DNS地址
	char szDNSAddr2[IPV4_LEN];   //DNS地址

	if(!IsIPv4Addr(pszMain))
	{
		memset(szDNSAddr1, 0, sizeof(szDNSAddr1));
	}
	else
	{
		strcpy(szDNSAddr1, pszMain);
	}

	if(!IsIPv4Addr(pszStandby))
	{
		memset(szDNSAddr2, 0, sizeof(szDNSAddr2));
	}
	else
	{
		strcpy(szDNSAddr2, pszStandby);
	}

	// 配置DNS
	if(strcmp(m_szDNSAddr1, szDNSAddr1) != 0 || strcmp(m_szDNSAddr2, szDNSAddr2) != 0)
	{
		pthread_rwlock_wrlock(&m_rwLock);
		memcpy(m_szDNSAddr1, szDNSAddr1, sizeof(szDNSAddr1));
		memcpy(m_szDNSAddr2, szDNSAddr2, sizeof(szDNSAddr2));
		pthread_rwlock_unlock(&m_rwLock);

		int iRet = WriteDNSInfo();
		if(0 != iRet)
		{
			LOG_ERROR_FMT("Update dns info failed\n");
			return -1;
		}
	}

	return 0;
}

int NetworkManager::WriteNetworkInfo()
{
	string strSum="";
	string strTemp="";

	strSum+="auto lo\n";
	strSum+="iface lo inet loopback\n";
	for (int i = 0; i < MAX_INTERFACE_NUM; i++)
	{
		char szIface[6] = {0};
		sprintf(szIface, "eth%d", i);
		strTemp = szIface;
		strSum+="auto "+ strTemp + "\n";
		strSum+="iface "+ strTemp + " inet static\n";
		strTemp=m_tNetworkInfo[i].szLocalIP;
		strSum+="address "+strTemp+"\n";
		strTemp=m_tNetworkInfo[i].szNetMask;
		strSum+="netmask "+strTemp+"\n";
		strTemp=m_tNetworkInfo[i].szGateway;
		strSum+="gateway "+strTemp+"\n";
	}

	FILE* fp = fopen(ADDR_CFG_FILE_TEMP, "w+");
	if (NULL == fp)
	{
		LOG_ERROR_FMT("fopen file %s fail, error=(%d, \"%s\")\n", ADDR_CFG_FILE_TEMP, errno, strerror(errno));
		return -1;
	}
	fseek(fp,0,SEEK_SET);
	fputs(strSum.c_str(),fp);
	fflush(fp);
	fclose(fp);
	
	remove(ADDR_CFG_FILE);
	rename(ADDR_CFG_FILE_TEMP, ADDR_CFG_FILE);
	
	return 0;
}

int NetworkManager::WriteDNSInfo()
{
	string strSum="";
	string strTemp="";

	if (strlen(m_szDNSAddr1) > 0)
	{
		strTemp=m_szDNSAddr1;
		strSum+="nameserver "+strTemp+"\n";
	}
	if (strlen(m_szDNSAddr2) > 0)
	{
		strTemp=m_szDNSAddr2;
		strSum+="nameserver "+strTemp+"\n";
	}

	FILE* fp = fopen(DNS_CFG_FILE_TEMP, "w+");
	if (NULL == fp)
	{
		LOG_ERROR_FMT("fopen file (\"%s\") fail, error=(%d, \"%s\")\n", DNS_CFG_FILE_TEMP, errno, strerror(errno));
		return -1;
	}
	fseek(fp,0,SEEK_SET);
	fputs(strSum.c_str(),fp);
	fclose(fp);
	remove(DNS_CFG_FILE);
	rename(DNS_CFG_FILE_TEMP, DNS_CFG_FILE);
	return 0;
}

void NetworkManager::DoActive(int ethi)
{
	int ret = 0;
	char szcmd[30] = {0};
	sprintf(szcmd, "ifdown eth%d", ethi);
	ret = system(szcmd);

	sprintf(szcmd, "ifup eth%d", ethi);
	ret = system(szcmd);
	LOG_DEBUG_FMT("network eth%d restart,ret = %d",ethi, ret);
}

int NetworkManager::PPP_4G_ON( int iTelecomID )
{
	if (iTelecomID == e10086)
	{
		system("cd /etc/ppp/peers/ && pppd call dial_unicom &");
	}
	else if (iTelecomID == e10000)
	{
		system("cd /etc/ppp/peers/ && pppd call dial_china_mobile &");
	}
	else
	{
		LOG_WARNS_FMT("Unsupported Telecom Type");
	}
	return 0;
}

int NetworkManager::PPP_4G_OFF()
{
	system("cd /etc/ppp/peers/ && sh -x ppp-off");
	return 0;
}

int NetworkManager::PPP_4G_Stat( FourGStat * ptInfo )
{
	if (ptInfo == NULL)
	{
		return -1;
	}
	
	FourGCFG tCfgInfo = {0};
	SystemProfile::GetInstance()->Get4GInfo(&tCfgInfo);
	ptInfo->bOpen = tCfgInfo.bOpen;
	//printf("4G  ptInfo->bOpen == %d\n", ptInfo->bOpen);

	if(access("/var/run/ppp0.pid", F_OK) == 0)
	{
		ptInfo->bOn = true;
	}
	else
	{
		ptInfo->bOn = false;
	}
	//printf("4G  ptInfo->bOn == %d\n", ptInfo->bOn);

	return 0;
}

