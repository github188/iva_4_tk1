#include <stdlib.h>
#include "include/dahua3dposition.h" 
#include <curl/curl.h>
#include <curl/easy.h>
#include <math.h>

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


DaHua3DPosition* DaHua3DPosition::m_instance = NULL;

DaHua3DPosition* DaHua3DPosition::GetInstance()
{
    if(m_instance != NULL)
    {
    	return m_instance;
    }

    try
    {
        m_instance = new DaHua3DPosition();
    }
    catch(exception &e)
    {
        cout<<e.what()<<endl;
    }

    return m_instance;
}


void DaHua3DPosition::SetDevInfo(int iChnID, string ip,string username,string passwd)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return;
	}

	m_ip[iChnID] = ip;
    m_username[iChnID] = username;
	m_passwd[iChnID] = passwd;
}


int DaHua3DPosition::ClickZoomIn(int iChnID, int X0, int Y0, int X1, int Y1)
{
	if(iChnID < 0 || iChnID >= MAX_CHANNEL_NUM)
	{
		return -1;
	}

	// 归一化到8192
    float left   = ((float)X0/1920)*8192;
   	float top    = ((float)Y0/1080)*8192;
    float right  = ((float)X1/1920)*8192;
    float bottom = ((float)Y1/1080)*8192;

	float arg1 = (left + right)/(2 * 8192);
	float arg2 = (top + bottom)/(2 * 8192);
	float maxlength = fabs(bottom - top) > fabs(right - left) ? (bottom - top) : (right - left);
	float zoomtemp = maxlength / 8192;
	if(fabs(zoomtemp) > 1)
	{
		zoomtemp = 1.0;
	}
	float arg3 = (right > left) ? zoomtemp * zoomtemp : (-1) * zoomtemp * zoomtemp;

    string userpasswd = m_username[iChnID] + ":" + m_passwd[iChnID];

 	stringstream ss;
	ss << "http://" << m_ip[iChnID] << "/cgi-bin/ptz.cgi?";
	ss << "action=moveDirectly";
	ss << "&channel=0";
	ss << "&arg1=" << arg1;
	ss << "&arg2=" << arg2;
	ss << "&arg3=" << arg3;

	string	requesturl = ss.str();
	//printf("---- cgi:%s\n",requesturl.c_str());
	

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
		
		curl_easy_setopt(curl, CURLOPT_URL,requesturl.c_str());
		curl_easy_setopt(curl, CURLOPT_USERPWD,userpasswd.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
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

