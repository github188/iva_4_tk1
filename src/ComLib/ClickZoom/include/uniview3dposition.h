#ifndef UNIVIEW3DPOSTION_H
#define UNIVIEW3DPOSTION_H

#include <iostream>
#include <sstream>
#include "capacity.h"

using namespace std;


class Uniview3DPosition
{
private:
	Uniview3DPosition(){};
	~Uniview3DPosition(){};
public:
	static Uniview3DPosition* GetInstance();
    void SetDevInfo(int iChnID, string ip, string username, string passwd);
    int  ClickZoomIn(int iChnID, int X0,int Y0, int X1,int Y1);
private:
	static Uniview3DPosition* m_instance;
    string m_ip[MAX_CHANNEL_NUM];
    string m_username[MAX_CHANNEL_NUM];
    string m_passwd[MAX_CHANNEL_NUM];
};

#endif // UNIVIEW3DPOSTION_H