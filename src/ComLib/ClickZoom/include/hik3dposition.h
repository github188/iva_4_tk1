#ifndef HIK3DPOSTION_H
#define HIK3DPOSTION_H

#include <iostream>
#include <sstream>
#include "capacity.h"

using namespace std;


class Hik3DPosition
{
private:
    Hik3DPosition(){};
    ~Hik3DPosition(){};
public:
    static Hik3DPosition* GetInstance();
    void SetDevInfo(int iChnID, string ip, string username, string passwd);
    int  ClickZoomIn(int iChnID, int X0,int Y0, int X1,int Y1);
private:
    static Hik3DPosition* m_instance;
    string m_ip[MAX_CHANNEL_NUM];
    string m_username[MAX_CHANNEL_NUM];
    string m_passwd[MAX_CHANNEL_NUM];
};

#endif // HIK3DPOSTION_H
