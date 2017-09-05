#ifndef _LIB_ONVIF2_COM_H_
#define _LIB_ONVIF2_COM_H_

const char * Onvif2_StringSafe(const char * str);
float Onvif2_FloatSafe(const float * pf);

void Onvif2_ParseIPFromUrl(const char * xaddrs, char * ip);

bool Onvif2_ParseUrlFromXAddrs(const char * xaddrs, const char * ip, char * url);

bool Onvif2_IsValidIPV4Addr(const char *str);

int Onvif2_IPv4Prefixlen2Mask(int prefixlen, char* mask);
int Onvif2_IPv4Mask2Prefixlen(const char* mask);

#endif