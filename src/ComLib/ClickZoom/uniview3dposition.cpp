#include <stdlib.h>
#include "include/uniview3dposition.h"
#include <curl/curl.h>
#include <curl/easy.h>

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


Uniview3DPosition *Uniview3DPosition::m_instance = NULL;
Uniview3DPosition *Uniview3DPosition::GetInstance()
{
    if(m_instance != NULL)
    {
    	return m_instance;
    }

    try
    {
		m_instance = new Uniview3DPosition();
    }
    catch(exception &e)
    {
        cout<<e.what()<<endl;
    }

    return m_instance;
}


void Uniview3DPosition::SetDevInfo(int iChnID, string ip, string username, string passwd)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return;
	}

	m_ip[iChnID] = ip;
    m_username[iChnID] = username;
	m_passwd[iChnID] = passwd;
}

int Uniview3DPosition::ClickZoomIn(int iChnID, int X0, int Y0, int X1, int Y1)
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

	int midPointX = startpointX;
	int midPointY = startpointY;
	int lengthX = endpointX - startpointX;
	int lengthY = endpointY - startpointY;

	string requesturl = "http://" + m_ip[iChnID] + "/LAPI/V1.0/Channel/0/PTZ/AreaZoomIn";
    string userpasswd = m_username[iChnID] + ":" + m_passwd[iChnID];

	stringstream ss;
	ss << "{\"MidPointX\":" << midPointX;
	ss << ", \"MidPointY\":" << midPointY;
	ss << ", \"LengthX\":" << lengthX ;
	ss << ", \"LengthY\":" << lengthY;
	ss << ", \"Width\":" << 255;
	ss << ", \"Height\":" << 255 << "}";
	
 	string ptzdata = ss.str();
	//printf("----json:%s\n", ptzdata.c_str());

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

		headers = curl_slist_append(headers, "Content-Type: application/json");

	    curl_easy_setopt(curl, CURLOPT_URL,requesturl.c_str());
	    curl_easy_setopt(curl, CURLOPT_USERPWD,userpasswd.c_str());
	    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
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

