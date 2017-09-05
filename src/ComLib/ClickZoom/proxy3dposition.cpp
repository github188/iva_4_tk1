#include <stdlib.h>
#include "include/proxy3dposition.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include "mq_onvif.h"

static size_t http_data_writer(void* data, size_t size, size_t nmemb, void* content)
{
	long totalSize = size*nmemb;
	string* symbolBuffer = (string*)content;
	if(symbolBuffer)
	{
		symbolBuffer->append((char *)data, ((char*)data)+totalSize);
	}
	return totalSize;
}


Proxy3DPosition* Proxy3DPosition::m_instance = NULL;
Proxy3DPosition* Proxy3DPosition::GetInstance()
{
    if(m_instance != NULL)
    {
    	return m_instance;
    }

    try
    {
        m_instance = new Proxy3DPosition();
    }
    catch(exception &e)
    {
        cout<<e.what()<<endl;
    }

    return m_instance;
}

void Proxy3DPosition::SetDevInfo( int iChnID, string ip, string username, string passwd, int manufacturer )
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return;
	}

	m_ip[iChnID] = ip;
	m_username[iChnID] = username;
	m_passwd[iChnID] = passwd;

	if(manufacturer == M_HIK)
	{
		m_manufacturer[iChnID] = "hikvision";
		m_devPort[iChnID] = 8000;
	}
	else if(manufacturer == M_DH)
	{
		m_manufacturer[iChnID] = "dahua";
		m_devPort[iChnID] = 3000;
	}
	else if(manufacturer == M_TIANDY)
	{
		m_manufacturer[iChnID] = "tiandy";
		m_devPort[iChnID] = 3000;
	}
	else if(manufacturer == M_UNIVIEW)
	{
		m_manufacturer[iChnID] = "uniview";
		m_devPort[iChnID] = 0;
	}
	else
	{
		m_manufacturer[iChnID] = "hikvision";
		m_devPort[iChnID] = 8000;
	}
}

void Proxy3DPosition::SetProxyInfo(int iChnID,string proxyip, int proxyport)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return;
	}
    m_proxyip[iChnID] = proxyip;
	m_proxyport[iChnID] = proxyport;
}

int Proxy3DPosition::ClickZoomIn(int iChnID,int X0, int Y0, int X1, int Y1)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return -1;
	}

	// 归一化到255
    int startpointX = X0*255/1920;
   	int startpointY = Y0*255/1080;
    int endpointX   = X1*255/1920;
    int endpointY   = Y1*255/1080;

    stringstream ss0;
	ss0 << "http://" << m_proxyip[iChnID] << ":" << m_proxyport[iChnID] << "/Set3DZoom.cgi";

    string requesturl = ss0.str();
    string userpasswd = m_username[iChnID] + ":" + m_passwd[iChnID];

    stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	ss << "<SOAP-ENV:Envelope ";
		ss << "xmlns:SOAP-ENV=\"http://www.w3.org/2003/05/soap-envelope\" ";
		ss << "xmlns:SOAP-ENC=\"http://www.w3.org/2003/05/soap-encoding\" ";
		ss << "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" ";
		ss << "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" ";
		ss << "xmlns:ns=\"urn:Position3D\">\n";
	ss << "<SOAP-ENV:Body>\n";
	ss << "<ns:SetPosition3D>\n";
	ss << "<setPosition3DRequest>\n";
	ss << "<startPoint>" << "<x>" << startpointX << "</x>" << "<y>" << startpointY << "</y>" << "</startPoint>\n";
	ss << "<endPoint>" << "<x>" << endpointX << "</x>" << "<y>" << endpointY << "</y>" << "</endPoint>\n";
	ss << "<devManufacturer>" << m_manufacturer[iChnID]<< "</devManufacturer>\n";
	ss << "<devIp>" << m_ip<< "</devIp>\n";
	ss << "<devUsername>" << m_username[iChnID]<< "</devUsername>\n";
	ss << "<devPasswd>" << m_passwd[iChnID]<< "</devPasswd>\n";
	ss << "<devPort>" << m_devPort[iChnID]<< "</devPort>\n";
	ss << "</setPosition3DRequest>\n";
	ss << "</ns:SetPosition3D>\n";
	ss << "</SOAP-ENV:Body>\n";
	ss << "</SOAP-ENV:Envelope>\n";
	
	
	string ptzdata = ss.str();
	//printf("---- xml:%s\n",ptzdata.c_str());

	try
	{
		CURLcode res = CURLE_READ_ERROR;
		CURL * curl  = NULL;
	    curl_slist *headers = NULL;
		string strResp  = "";

	    curl_global_init( CURL_GLOBAL_ALL );

	    curl = curl_easy_init();
	    if (NULL == curl)
	    {
			printf("error: %s ,URL:%s!\n",curl_easy_strerror(res) ,requesturl.c_str());
			res = CURLE_FAILED_INIT;
			goto ERR;
	    }

		headers = curl_slist_append(headers, "Content-Type: application/xml");

	    curl_easy_setopt(curl, CURLOPT_URL,requesturl.c_str());
	    curl_easy_setopt(curl, CURLOPT_USERPWD,userpasswd.c_str());
	    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ptzdata.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_data_writer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&strResp);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);//CURLOPT_TIMEOUT 	设置cURL允许执行的最长秒数。
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2);//CURLOPT_CONNECTTIMEOUT 	在发起连接前等待的时间，如果设置为0，则无限等待。 

	    res = curl_easy_perform(curl);

		if(res != CURLE_OK)
		{
			printf("error: %s ,URL:%s!\n",curl_easy_strerror(res) ,requesturl.c_str());
			goto ERR;
		}
		else
		{
			long retcode = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
			if (retcode != 200)
			{
				printf("RESPONSE_CODE: %ld ,URL:%s!\n",retcode, requesturl.c_str());
				res = CURLE_UNSUPPORTED_PROTOCOL;
				goto ERR;
			}

			string::size_type pos1,pos2;
			pos1 = strResp.find("<result>");
			pos2 = strResp.find("</result>");
			if(pos1 == string::npos || pos2 == string::npos)
			{
				printf("RESPONSE: %s!\n",strResp.c_str());
				res = CURLE_UNSUPPORTED_PROTOCOL;
				goto ERR;
			}

			string strCode = strResp.substr(pos1+12, pos2-pos1-12);

			int iRespCode = atoi(strCode.c_str());
			if(iRespCode != 0)
			{
				printf("RESPONSE result: %s!\n",strCode.c_str());
				res = CURLE_UNSUPPORTED_PROTOCOL;
				goto ERR;
			}
		}
		
ERR:

		if(headers)
		{
			curl_slist_free_all(headers);
			headers = NULL;
		}

		
		if(curl != NULL)
		{
			curl_easy_cleanup(curl);
		}
		
		curl_global_cleanup();
	}
	catch (std::exception &ex)
	{
		printf("curl exception %s.\n", ex.what());
	}

    return 0;
}

