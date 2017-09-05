#ifndef DAHUA3DPOSTION_H
#define DAHUA3DPOSTION_H

#include <iostream>
#include <sstream>
#include "capacity.h"

using namespace std;


class DaHua3DPosition
{
private:
    DaHua3DPosition(){};
    ~DaHua3DPosition(){};
public:
    static DaHua3DPosition* GetInstance();
    void SetDevInfo(int iChnID, string ip, string username, string passwd);
    int  ClickZoomIn(int iChnID, int X0,int Y0, int X1,int Y1);
private:
    static DaHua3DPosition* m_instance;
    string m_ip[MAX_CHANNEL_NUM];
    string m_username[MAX_CHANNEL_NUM];
    string m_passwd[MAX_CHANNEL_NUM];
};

#endif // DAHUA3DPOSTION_H
