#ifndef TIANDY3DPOSTION_H
#define TIANDY3DPOSTION_H

#include <iostream>
#include <sstream>
#include "capacity.h"

using namespace std;


class Tiandy3DPosition
{
private:
	Tiandy3DPosition();
	~Tiandy3DPosition();
public:
	static Tiandy3DPosition* GetInstance();
    void SetDevInfo(int iChnID, string ip, string username, string passwd);
    int  ClickZoomIn(int iChnID, int X0,int Y0, int X1,int Y1);
private:
	static Tiandy3DPosition* m_instance;
    string m_ip[MAX_CHANNEL_NUM];
    string m_username[MAX_CHANNEL_NUM];
    string m_passwd[MAX_CHANNEL_NUM];
	int m_iLogonID[MAX_CHANNEL_NUM];
};

#endif // TIANDY3DPOSTION_H