#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
const char *g_pszNullString = "";

const char * Onvif2_StringSafe(const char * str)
{
	if( str == NULL )
	{
		return g_pszNullString;
	}
	else
	{
		return str;
	}
}

float Onvif2_FloatSafe(const float * pf)
{
	if( pf == NULL )
	{
		return 0.0f;
	}
	else
	{
		return *pf;
	}
}

void Onvif2_ParseIPFromUrl(const char * xaddrs, char * ip)
{
	if( xaddrs == NULL || ip == NULL )
	{
		return;
	}

	// http://192.168.1.227/onvif/device_service
	// http://192.168.1.224:8080/onvif/device_service
	const char * h  = "http://";
	const char * p = xaddrs;
	const char * q = strstr(p, h);
	if( q == NULL )
	{
		return;
	}

	p += strlen(h);
	q = strstr(p, ":");
	if (q == NULL)
	{
		q = strstr(p, "/");
	}

	if (q)
	{
		strncpy(ip, p, q-p);
	}
}

bool Onvif2_ParseUrlFromXAddrs(const char * xaddrs, const char * ip, char * url)
{
	//XAddrs = http://192.168.1.227/onvif/device_service http://[fe80::4619:b6ff:fe53:c48b]/onvif/device_service
	if( xaddrs == NULL || ip == NULL || url == NULL )
	{
		return false;
	}

	char szHead[40] = {0};
	sprintf(szHead, "http://%s", ip);
	const char * p = xaddrs;
	const char * q = strstr(p, szHead);
	if(q == NULL)
	{
		return false;
	}
	
	//p += strlen(szHead);
	q = strstr(p, " ");
	if (q == NULL)
	{
		strcpy(url, p);
	}
	else
	{
		strncpy(url, p, q-p);
	}

	return true;
}

bool Onvif2_IsValidIPV4Addr(const char *IP)
{
	if(IP)
    {
		return (inet_addr(IP) != INADDR_NONE);
	}
	else
	{
		return false;
	}
}

int Onvif2_IPv4Prefixlen2Mask(int prefixlen, char* mask)
{  
//    char tmp[16] = {0};
//    char* p = tmp;
    unsigned int ip_num = 0;
      
    if (mask == NULL)
	{
		return -1;
	}

    if (prefixlen < 0 ||prefixlen > 32)
	{
		return -1;
	}
      
    // fast...  
    if (prefixlen == 8)
	{
		strcpy(mask, "255.0.0.0");
		return 0;
	}
    if (prefixlen == 16)
	{
		strcpy(mask, "255.255.0.0");
		return 0;
	}

    if (prefixlen == 24)
	{
		strcpy(mask, "255.255.255.0");
		return 0;
	}

    if (prefixlen == 32)
	{
		strcpy(mask, "255.255.255.255");
		return 0;
	}
      
    // just in case  
    for (int i = prefixlen, j = 31; i > 0; i--, j--)
    {  
        //unsigned int tmp = (1<<j);
        //printf("%d tmp: %08x\n", i, tmp);
        ip_num += (1<<j);
    }  
    //printf("ip_num: %08x\n", ip_num);  
    sprintf(mask, "%hhu.%hhu.%hhu.%hhu", (ip_num>>24)&0xff, (ip_num>>16)&0xff, (ip_num>>8)&0xff, ip_num&0xff);
      
    return 0;  
}

int Onvif2_IPv4Mask2Prefixlen(const char* mask)
{
    int ret = 0;
    unsigned int ip_num = 0;
    unsigned char c1,c2,c3,c4;
    int cnt = 0;

	if(!Onvif2_IsValidIPV4Addr(mask))
	{
		return 0;
	}
      
    ret = sscanf(mask, "%hhu.%hhu.%hhu.%hhu", &c1, &c2, &c3, &c4);
	if (ret != 4)
	{
		return 0;
	}
    ip_num = c1<<24 | c2<<16 | c3<<8 | c4;
      
    // fast...  
    if (ip_num == 0xffffffff) return 32;
    if (ip_num == 0xffffff00) return 24;
    if (ip_num == 0xffff0000) return 16;
    if (ip_num == 0xff000000) return 6;
      
    // just in case  
    for (int i = 0; i < 32; i++)
    {
        //unsigned int tmp = (ip_num<<i);  
        //printf("%d tmp: %x\n", i+1, tmp);  
        if ((ip_num<<i) & 0x80000000)
            cnt++;
        else  
            break;
    }  

	//printf("cnt: %d\n", cnt);  
    return cnt;
}  
