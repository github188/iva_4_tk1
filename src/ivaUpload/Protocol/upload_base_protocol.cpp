#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "upload_base_protocol.h"
#include "xml_com.h"
#include <string>
#include <exception>
#include <sys/stat.h>
#include <fcntl.h>
#include "oal_log.h"
#include "oal_file.h"
#include "oal_base64.h"
#include "oal_unicode.h"
#ifndef WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif


UploadBaseProtocol::UploadBaseProtocol()
{
}

UploadBaseProtocol::~UploadBaseProtocol()
{
	ShutDown();
}

int UploadBaseProtocol::GenXmlOne(const char *file_name, const AnalyDbRecord *pData)
{
	return create_xml_file(file_name,pData);
}

int UploadBaseProtocol::ShutDown()
{
	return 0;
}

size_t http_resp_data_writer(void* data, size_t size, size_t nmemb, void* content)
{
	long totalSize = size*nmemb;
	std::string* symbolBuffer = (std::string*)content;
	if(symbolBuffer)
	{
		symbolBuffer->append((char *)data, ((char*)data)+totalSize);
	}
	return totalSize;
}


const char * LocalIPAddr()
{
	static char s_IPAddr[32] = {0}; 

#ifndef WIN32
	int sockfd;
	struct ifreq ifr;
	struct sockaddr_in sin;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		return s_IPAddr;
	}

	//get eth0 ip	
	strcpy(ifr.ifr_name, "eth0");
	if(ioctl(sockfd,SIOCGIFADDR,&ifr) < 0)
	{
		close(sockfd);
		return s_IPAddr;
	}

	memcpy(&sin,&ifr.ifr_addr,sizeof(sin));
	sprintf(s_IPAddr,"%s",inet_ntoa(sin.sin_addr));
	close(sockfd);
#endif
	return s_IPAddr;
}

int File2HttpBoaUrl(const char * file_path, char *url)
{
	if(file_path == NULL || url == NULL)
	{
		return -1;
	}

	char file_path_2312[512] = {0};
	UTF8_To_GB2312((char *)file_path, strlen(file_path), file_path_2312, sizeof(file_path_2312));
	if (OAL_FileExist(file_path_2312) != SUCCESS)
	{
		return UPLOAD_RET_NO_FILE;
	}

	if(strlen(file_path) > 0)
	{
		if(strstr(file_path,"/mnt/"))
		{
			sprintf(url,"http://%s%s",LocalIPAddr(),file_path+4);
		}
		else
		{
			sprintf(url,"http://%s%s",LocalIPAddr(),file_path);
		}
	}

	return 0;
}

int PackageFile2Base64Buf(const char * file_path, char ** base64_str)
{
	if(file_path == NULL || base64_str == NULL)
	{
		return -1;
	}

	char file_path_2312[512] = {0};
	UTF8_To_GB2312((char *)file_path, strlen(file_path), file_path_2312, sizeof(file_path_2312));
	long file_size = OAL_FileSize(file_path_2312);
	if (file_size < 1)
	{
		LOG_DEBUG_FMT("file_size error!\n");
		return UPLOAD_RET_NO_FILE;
	}

	printf("file_path_2312:%s, file_size=%ld\n", file_path_2312, file_size);

	char * filebuffer = new char [file_size+1];
	if(filebuffer == NULL)
	{
		LOG_DEBUG_FMT("new filebuffer %l error!\n", file_size);
		return -1;
	}

	FILE *fp = fopen(file_path_2312, "rb+");
	if(fp == NULL)
	{
		delete [] filebuffer;
		LOG_DEBUG_FMT("fopen %s error!" ,file_path_2312);
		return UPLOAD_RET_NO_FILE;
	}

	fseek(fp ,0 ,SEEK_SET);
	int iLeft = file_size;
	int iOk = 0;
	while(iLeft > 0)
	{
		int cnt = iLeft;
		if(iLeft > 1024)
		{
			cnt = 1024;
		}

		int readedcnt = fread(filebuffer+iOk ,1 ,cnt ,fp);
		if(readedcnt <= 0)
		{
			break;
		}
		iLeft -= readedcnt;
		iOk += readedcnt;
	}
	fclose(fp);

	int encodelen = Base64encode_len(file_size);
	char * encodebuffer = new char [encodelen+1];
	if(encodebuffer == NULL)
	{
		LOG_DEBUG_FMT("new encodebuffer %l error!\n", encodelen);
		delete [] filebuffer;
		return -1;
	}

	Base64encode(encodebuffer,filebuffer, file_size);
	encodebuffer[encodelen] = '\0';
	
	*base64_str = encodebuffer;

	delete [] filebuffer;

	return 0;
}

int ReplaceKeyWord(char * str,const char * keywords, const char * value)
{
	char outstr[2048] = {0};
	char * p = strstr(str, keywords);
	if(p)
	{
		//copy 开始到p的位置
		memcpy(outstr,str,p-str);
		//copy 值
		memcpy(outstr+strlen(outstr),value,strlen(value));
		//copy 剩余的部分
		memcpy(outstr+strlen(outstr),p+strlen(keywords),strlen(str)-(p-str)-strlen(keywords));

		ReplaceKeyWord(outstr,keywords,value);

		// 拷贝回去
		strcpy(str,outstr);

		return 0;
	}
	else
	{
		return -1;
	}
}

int ReplaceKeyWordOnce(char * str,const char * keywords, const char * value)
{
	char outstr[2048] = {0};
	char * p = strstr(str, keywords);
	if(p)
	{
		//copy 开始到p的位置
		memcpy(outstr,str,p-str);
		//copy 值
		memcpy(outstr+strlen(outstr),value,strlen(value));
		//copy 剩余的部分
		memcpy(outstr+strlen(outstr),p+strlen(keywords),strlen(str)-(p-str)-strlen(keywords));

		//ReplaceKeyWord(outstr,keywords,value);

		// 拷贝回去
		strcpy(str,outstr);

		return 0;
	}
	else
	{
		return -1;
	}
}

int FtpSvrSubDir(const AnalyDbRecord *data, const char * grammar, char *sub_dir)
{
	if(data == NULL || grammar == NULL || sub_dir == NULL)
	{
		return -1;
	}

	// 4位年-%YYYY% 2位年-%YY% 2位月-%MM% 2位天-%DD% 2位小时%HH% 2位分钟%mm%
	// 设备IP-%IP% 设备编号-%ID% 违法代码-%WFD%
	strcpy(sub_dir,grammar);

	struct tm *local_time = localtime(&data->tCreate);
	if (local_time == NULL)
	{
		return -1;
	}

	char cYYYY[5] = {0};
	sprintf(cYYYY, "%04d", local_time->tm_year+1900);
	ReplaceKeyWord(sub_dir,"%YYYY%",cYYYY);

	char cYY[3] = {0};
	strncpy(cYY, cYYYY+2, 2);
	ReplaceKeyWord(sub_dir,"%YY%",cYY);

	char cMM[3] = {0};
	sprintf(cMM, "%02d", local_time->tm_mon+1);
	ReplaceKeyWord(sub_dir,"%MM%",cMM);

	char cDD[3] = {0};
	sprintf(cDD, "%02d", local_time->tm_mday);
	ReplaceKeyWord(sub_dir,"%DD%",cDD);

	char cHH[3] = {0};
	sprintf(cHH, "%02d", local_time->tm_hour);
	ReplaceKeyWord(sub_dir,"%HH%",cHH);

	char cmm[3] = {0};
	sprintf(cmm, "%02d", local_time->tm_min);
	ReplaceKeyWord(sub_dir,"%mm%",cmm);

	ReplaceKeyWord(sub_dir,"%IP%", LocalIPAddr());
	ReplaceKeyWord(sub_dir,"%ID%", data->szDevID);

	char cWFD[16] = {0};
	sprintf(cWFD, "%s", data->szCode);
	ReplaceKeyWord(sub_dir,"%WFD%",cWFD);
	return 0;
}

