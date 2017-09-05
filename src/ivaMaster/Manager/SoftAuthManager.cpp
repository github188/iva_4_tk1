#include "SoftAuthManager.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif
#include "oal_base64.h"
#include "openssl/md5.h"
#include "oal_log.h"
#include <assert.h>
#include "oal_file.h"
#include "oal_time.h"


#define AUTH_LICENSE_FILE	"/etc/iva/LICENSE"


static int MD5_Process(unsigned char *pu8Data, int iLen, unsigned char * pu8Dst,int iB,int iE)
{ 
	if (pu8Data == NULL || pu8Dst == NULL)
	{
		return -1;
	}

	int ret = -1;
	MD5_CTX c;
	unsigned char md5[16]={0};
	MD5_Init(&c);
	MD5_Update(&c, pu8Data, iLen);  
	MD5_Final(md5,&c); 
	ret = 0;

	int i = 0;
	for (int j = iB; j< iE && j<16 && j > 0; j++)
	{
		pu8Dst[i++] = md5[j];
	}

	return ret;
}

SoftAuthManager* SoftAuthManager::m_pInstance=NULL;

SoftAuthManager::SoftAuthManager()
{
	m_tSAInfo.tStatus.eAuthType = eProhibit;
	m_tSAInfo.tStatus.tTimeOut = 0;
	memset(m_tSAInfo.szLicense,0,sizeof(m_tSAInfo.szLicense));

	AuthStatusT tStatus;
	tStatus.eAuthType = m_tSAInfo.tStatus.eAuthType;
	tStatus.tTimeOut = m_tSAInfo.tStatus.tTimeOut;

	int iRet = ReadFile(AUTH_LICENSE_FILE,NULL);
	if(iRet > 0 && iRet < 2048)
	{
		char * pstrLicense = new char [iRet+1];
		if(pstrLicense == NULL)
		{
			return;
		}

		ReadFile(AUTH_LICENSE_FILE,(unsigned char *)pstrLicense);
		pstrLicense[iRet] = '\0';
		
		iRet = DecodeLicense(pstrLicense, &tStatus);

		if(iRet == 0)
		{
			m_tSAInfo.tStatus.eAuthType = tStatus.eAuthType;
			m_tSAInfo.tStatus.tTimeOut  = tStatus.tTimeOut;
			strcpy(m_tSAInfo.szLicense, pstrLicense);
		}

		delete [] pstrLicense;
		pstrLicense = NULL;
	}

	// 通知ANALY与Record
	MQ_Analy_SoftwareLicense(&tStatus);
	usleep(10000);
	MQ_Record_SoftwareLicense(&tStatus);
}

SoftAuthManager::~SoftAuthManager()
{
}

SoftAuthManager* SoftAuthManager::GetInstance()
{
	return m_pInstance;
}

int SoftAuthManager::Initialize()
{
	if(m_pInstance)
	{
		return 0;
	}

	m_pInstance = new SoftAuthManager();
	assert(m_pInstance);

	return 0;
}

void SoftAuthManager::UnInitialize()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

int SoftAuthManager::DoAuthorization(const char * license)
{
	if(license == NULL)
	{
		return 1;
	}
	
	//0:成功 1:失败 2:已经授权过(请先删除旧的授权)
	if(m_tSAInfo.tStatus.eAuthType == ePermanent || m_tSAInfo.tStatus.eAuthType == eTemporary)
	{
		return 2;
	}

	// 验证授权
	AuthStatusT tStatus;
	int iRet = DecodeLicense(license,&tStatus);
	if(iRet != 0)
	{
		return 1;
	}

	// 保存LICENSE
	iRet = SaveFile(AUTH_LICENSE_FILE,(const unsigned char *)license,strlen(license));
	if(iRet != 0)
	{
		return 1;
	}

	m_tSAInfo.tStatus.eAuthType = tStatus.eAuthType;
	m_tSAInfo.tStatus.tTimeOut	= tStatus.tTimeOut;
	strcpy(m_tSAInfo.szLicense,license);

	// 通知ANALY与Record
	MQ_Analy_SoftwareLicense(&tStatus);
	MQ_Record_SoftwareLicense(&tStatus);
	return 0;
}

int SoftAuthManager::GetAuthorization(SoftAuth * pInfo)
{
	if(pInfo)
	{
		pInfo->tStatus.eAuthType = m_tSAInfo.tStatus.eAuthType;
		pInfo->tStatus.tTimeOut  = m_tSAInfo.tStatus.tTimeOut;
		strcpy(pInfo->szLicense,m_tSAInfo.szLicense);
	}
	return 0;
}

int SoftAuthManager::DelAuthorization()
{
	OAL_DeleteFile(AUTH_LICENSE_FILE);
	m_tSAInfo.tStatus.eAuthType = eProhibit;
	m_tSAInfo.tStatus.tTimeOut	= 0;
	memset(m_tSAInfo.szLicense,0,sizeof(m_tSAInfo.szLicense));

	// 通知ANALY与Record
	MQ_Analy_SoftwareLicense(&m_tSAInfo.tStatus);
	MQ_Record_SoftwareLicense(&m_tSAInfo.tStatus);
	return 0;
}

int SoftAuthManager::DecodeLicense(const char * pstrLicense, AuthStatusT * ptInfo)
{
	unsigned char MAC[6] = {0x00,0x00,0x00,0x00,0x00,0x00};
	int iRet = ReadMacAddr(MAC);
	if(iRet != 0)
	{
		LOG_ERROR_FMT("ReadMacAddr failed\n");
		return -1;
	}
	
	// 明文校验部分 0xFF 0x23 0x45 0x67 6位MAC CEYE  0x76 0x54 0x32 0xFF 2位类型 4位长度
 	unsigned char cleartext[20] = {0xFF,0x23,0x45,0x67,0x00,0x00,0x00,0x00,0x00,0x00,'C','E','Y','E',
		0x76,0x54,0x32,0xFF,0xFA,0x00};
 	
	for (int i = 0; i < 6; i++)
 	{
 		cleartext[4+i] = MAC[i];
 	}

	// MD5加密
	unsigned char u8license1[24] = {0};
	unsigned char u8license2[24] = {0};
	MD5_Process(cleartext,20,u8license1,3,12);
	cleartext[19] = 0x01;
	MD5_Process(cleartext,20,u8license2,3,12);

	// BASE64解码
	int resize = Base64decode_len(pstrLicense);
	if(resize != 13)
	{
		LOG_ERROR_FMT("len=%d error",resize);
		return -1;
	}
	char temp[14] = {0};
	Base64decode(temp,pstrLicense);

	if(memcmp(temp,u8license1,9) == 0)
	{
		ptInfo->eAuthType = ePermanent;
		ptInfo->tTimeOut = 0;
		LOG_INFOS_FMT("ePermanent");
	}
	else if(memcmp(temp,u8license2,9) == 0)
	{
		ptInfo->eAuthType = eTemporary;
		int y = temp[9]+2001;
		int m = temp[10]-7;
		int d = temp[11]-9;
		struct tm t;
		t.tm_year=y-1900;
		t.tm_mon=m-1;
		t.tm_mday=d;
		t.tm_hour=23;
		t.tm_min=59;
		t.tm_sec=59;
		ptInfo->tTimeOut  = mktime(&t);
		LOG_INFOS_FMT("eTemporary To %04d-%02d-%02d",y,m,d);
	}
	else
	{
		ptInfo->eAuthType = eProhibit;
		ptInfo->tTimeOut = 0;
		LOG_INFOS_FMT("eProhibit");
		return -1;
	}
	return 0;
}

int SoftAuthManager::ReadFile(const char * pstrFilePath, unsigned char * buffer)
{
	if(pstrFilePath == NULL)
	{
		return -1;
	}
	
	FILE* fd = NULL;
	fd = fopen(pstrFilePath,"r");
	if(fd == NULL)
	{
		LOG_ERROR_FMT("fopen %s error",pstrFilePath);
		return -1;
	}

	fseek(fd,0L,SEEK_END); 
	int flen = ftell(fd); 
	if(flen <= 0)
	{
		fclose(fd);
		LOG_ERROR_FMT("file %s is empty",pstrFilePath);
		return -1;
	}
	fseek(fd,0L,SEEK_SET);

	if(buffer)
	{
		if(fread (buffer, flen, 1, fd) != 1)
		{
			fclose(fd);
			return -1;
		}
	}
	
	fclose(fd);
	fd = NULL;
	return flen;
}

int SoftAuthManager::SaveFile(const char * pstrFilePath, const unsigned char * buffer, int flen)
{
	if(pstrFilePath == NULL || buffer == NULL || flen < 1)
	{
		return -1;
	}

	OAL_MakeDirByPath(pstrFilePath);
	
	FILE* fd = NULL;
	fd = fopen(pstrFilePath,"wb+");
	if(fd == NULL)
	{
		LOG_ERROR_FMT("fopen %s error\n",pstrFilePath);
		return -1;
	}

	if(fwrite (buffer, flen, 1, fd) != 1)
	{
		fclose(fd);
		return -1;
	}
	
	fclose(fd);
	fd = NULL;
	return 0;
}

#ifndef WIN32	
int SoftAuthManager::ReadMacAddr(unsigned char * pu8Mac)
{
	if(pu8Mac == NULL)
	{
		return -1;
	}

	int nSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(nSocket< 0)
	{
		return -1;
	}

	struct ifreq ifr;
	memset(&ifr,0,sizeof(ifr));
	strcpy(ifr.ifr_name, "eth0");
	if (ioctl(nSocket, SIOCGIFHWADDR, &ifr)< 0)
	{   
		close(nSocket);
		return -1;
	}   
	close(nSocket);
	nSocket = -1;

	struct sockaddr sMac;
	memcpy(&sMac,&ifr.ifr_addr,sizeof(sMac));
	memcpy(pu8Mac,sMac.sa_data,6);
	return 0;
}
#else
#include  <httpext.h>  
#include  <windef.h>
#include  <Nb30.h>
#pragma comment(lib,"netapi32.lib")  
typedef struct _ASTAT_   
{
	ADAPTER_STATUS  adapt; 
	NAME_BUFFER  NameBuff[30];   
}ASTAT,  *PASTAT;   
 
int SoftAuthManager::ReadMacAddr(unsigned char * pu8Mac)
{
	if(pu8Mac == NULL)
	{
		return -1;
	}
	NCB ncb;   
   
	ASTAT Adapter;   
	LANA_ENUM lana_enum;

	UCHAR uRetCode;   
	memset(&ncb, 0, sizeof(ncb));   
	memset(&lana_enum, 0, sizeof(lana_enum));   

	ncb.ncb_command = NCBENUM;   
	ncb.ncb_buffer = (unsigned char *)&lana_enum;   
	ncb.ncb_length = sizeof(LANA_ENUM);   
	uRetCode = Netbios(&ncb);   
	if(uRetCode != NRC_GOODRET)   
		return uRetCode;   

	for(int lana=0; lana<lana_enum.length; lana++)   
	{
		ncb.ncb_command = NCBRESET; 
		ncb.ncb_lana_num = lana_enum.lana[lana]; 
		uRetCode = Netbios(&ncb);  
		if(uRetCode == NRC_GOODRET) 
			break; 
	} 
	if(uRetCode != NRC_GOODRET)
		return uRetCode;   

	memset(&ncb, 0, sizeof(ncb)); 
	ncb.ncb_command = NCBASTAT; 
	ncb.ncb_lana_num = lana_enum.lana[0];
	strcpy((char*)ncb.ncb_callname, "*"); 
	ncb.ncb_buffer = (unsigned char *)&Adapter;
	ncb.ncb_length = sizeof(Adapter);
	uRetCode = Netbios(&ncb); 
	if(uRetCode != NRC_GOODRET)  
		return uRetCode;

	memcpy(pu8Mac,Adapter.adapt.adapter_address,6);

	return 0;  
}
#endif


