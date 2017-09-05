#include <stdlib.h>
#include "include/tiandy3dposition.h"
#include "nvssdk/NetSdkClient.h"
#include "nvssdk/NetClientTypes.h"
#include <stdio.h>
#include <string.h>


Tiandy3DPosition *Tiandy3DPosition::m_instance = NULL;
Tiandy3DPosition *Tiandy3DPosition::GetInstance()
{
    if(m_instance != NULL)
    {
    	return m_instance;
    }

    try
    {
		m_instance = new Tiandy3DPosition();
    }
    catch(exception &e)
    {
        cout<<e.what()<<endl;
    }

    return m_instance;
}

Tiandy3DPosition::Tiandy3DPosition()
{
	NetClient_Startup_V4(3000, 6000, 0);
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		m_iLogonID[i] = -1;
	}
}

Tiandy3DPosition::~Tiandy3DPosition()
{
	for (int i = 0; i < MAX_CHANNEL_NUM; i++)
	{
		if (m_iLogonID[i] > 0)
		{
			NetClient_Logoff(m_iLogonID[i]);
			m_iLogonID[i] = -1;
		}
	}

	NetClient_Cleanup();
}

void Tiandy3DPosition::SetDevInfo(int iChnID, string ip, string username, string passwd)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return;
	}

	if ( ip.compare(m_ip[iChnID]) != 0 || username.compare(m_username[iChnID]) || passwd.compare(m_passwd[iChnID]))
	{
		if (m_iLogonID[iChnID] > 0)
		{
			NetClient_Logoff(m_iLogonID[iChnID]);
			m_iLogonID[iChnID] = -1;
		}

		m_ip[iChnID] = ip;
		m_username[iChnID] = username;
		m_passwd[iChnID] = passwd;

		m_iLogonID[iChnID] = NetClient_Logon((char*)"", (char*)m_ip[iChnID].c_str(), (char*)m_username[iChnID].c_str(), (char*)m_passwd[iChnID].c_str(), (char*)"", 3000);
	}
}

int Tiandy3DPosition::ClickZoomIn(int iChnID, int X0, int Y0, int X1, int Y1)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return -1;
	}

	if (m_iLogonID[iChnID] < 0)
	{
		m_iLogonID[iChnID] = NetClient_Logon((char*)"", (char*)m_ip[iChnID].c_str(), (char*)m_username[iChnID].c_str(), (char*)m_passwd[iChnID].c_str(), (char*)"", 3000);
		if (m_iLogonID[iChnID] < 0)
		{
			printf("Login TianDy device ip=%s failed\n", m_ip[iChnID].c_str());
			return -1;
		}
		else
		{
			printf("Login TianDy device ip=%s OK, m_iLogonID = %d\n", m_ip[iChnID].c_str(), m_iLogonID[iChnID]);
		}
	}

	float videoW = 1920.00;
	float videoH = 1080.00;

	float localX = (float)X0;
	float localY = (float)Y0;
	float localW = (float)(X1 - X0);
	float localH = (float)(Y1 - Y0);

	float cxpoint = localX + localW/(float)2.00;
	float cypoint = localY + localH/(float)2.00;

	int L = (cxpoint < videoW/2) ? 1 : 0;
	int R = (cxpoint < videoW/2) ? 0 : 1;
	int U = (cypoint < videoH/2) ? 1 : 0;
	int D = (cypoint < videoH/2) ? 0 : 1;
	int W = 0;
	int T = 1;

	unsigned char cDecBuf[9] = {0};
	cDecBuf[0] = 0xf6;
	cDecBuf[1] = 8;
	cDecBuf[2] = 0;
	cDecBuf[3] = 0x52;
	cDecBuf[4] = (unsigned char)( R + L*2 + U*4 + D*8 + W*16 + T*32 );
	cDecBuf[5] = (unsigned char)( abs(cxpoint - (videoW/2) ) * 0x3f / (videoW/2) );
	cDecBuf[6] = (unsigned char)( abs(cypoint - (videoH/2) ) * 0x3f / (videoH/2) );
	cDecBuf[7] = (unsigned char)( localW * 0x3f / videoW);
	cDecBuf[8] = (cDecBuf[1]+cDecBuf[2]+cDecBuf[3]+cDecBuf[4]+cDecBuf[5]+cDecBuf[6]+cDecBuf[7]) & 0x7f;

	int ret = NetClient_ComSend(m_iLogonID[iChnID], cDecBuf, sizeof(cDecBuf), 0);

	if(ret != 0)
	{
		NetClient_Logoff(m_iLogonID[iChnID]);
		m_iLogonID[iChnID] = -1;
	}

	return -1;
}

