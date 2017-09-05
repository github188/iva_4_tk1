#ifndef PROXY3DPOSTION_H
#define PROXY3DPOSTION_H

#include <iostream>
#include <sstream>
#include "capacity.h"

using namespace std;

class Proxy3DPosition
{
private:
    Proxy3DPosition(){};
    ~Proxy3DPosition(){};
public:
    static Proxy3DPosition* GetInstance();
	void SetDevInfo(int iChnID, string ip, string username, string passwd, int manufacturer);
 	void SetProxyInfo(int iChnID, string proxyip, int proxyport);
    int  ClickZoomIn(int iChnID, int X0,int Y0, int X1,int Y1);
private:
    static Proxy3DPosition* m_instance;
    string m_ip[MAX_CHANNEL_NUM];
    string m_username[MAX_CHANNEL_NUM];
    string m_passwd[MAX_CHANNEL_NUM];
    string m_manufacturer[MAX_CHANNEL_NUM];
    int    m_devPort[MAX_CHANNEL_NUM];
    string m_proxyip[MAX_CHANNEL_NUM];
    int    m_proxyport[MAX_CHANNEL_NUM];
};

#endif // PROXY3DPOSTION_H
