#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <curl/curl.h>
#include <string>
#include <exception>
#include "oal_time.h"
#include "oal_log.h"
#include "mq_upload.h"

static int pack_weixin_json(const AnalyNotify *pData, char *out_buf)
{
	if((NULL == pData) || (NULL == out_buf))
	{
		return -1;
	}

	char tm_buf[60] = {0};
	TimeFormatString(pData->tCreateTime, tm_buf, sizeof(tm_buf), eYMDHMS1);
	sprintf(out_buf,"{"
		"\"licencePlate\":\"%s\","
		"\"illegalPlace\":\"%s\","
		"\"illegalTime\":\"%s\"}",
		pData->szPlate, pData->szPlace, tm_buf);

	printf("[WX_JSON]:%s\n",out_buf);
	return 0;
}

int Notify2WeiXin( const char *server_url, const AnalyNotify *pData )
{
	CURLcode         res = CURLE_READ_ERROR;
	CURL            *curl = NULL;
	curl_slist      *plist = NULL;

	if((NULL == server_url) || (NULL == pData))
	{
		printf("input error!\n");
		return -1;
	}

	printf("Notify2WeiXin %s\n" ,server_url);

	// 生成XML
	char json_buf[512] = {0};
	memset(json_buf ,0 ,sizeof(json_buf));
	pack_weixin_json(pData, json_buf);
	try
	{
		curl_global_init(CURL_GLOBAL_ALL);

		curl = curl_easy_init();
		if(curl == NULL)
		{
			LOG_ERROR_FMT("%s ,URL:%s!",curl_easy_strerror(res) ,server_url);
			res = CURLE_FAILED_INIT;
			goto UPLOAD_ERR;
		}

		//curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &RecvData);
		curl_easy_setopt(curl, CURLOPT_URL, server_url);

		// 设置超时时间为5秒
		//curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

		// 设置http发送的内容类型为JSON	
		plist = curl_slist_append(plist, "Content-Type:application/json;charset=UTF-8");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, plist);

		// 设置要POST的JSON
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_buf);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
		{
			LOG_ERROR_FMT("%s ,URL:%s!",curl_easy_strerror(res) ,server_url);
		}
		else
		{
			long retcode = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
			if (retcode != 200)
			{
				LOG_WARNS_FMT("RESPONSE_CODE: %ld ,URL:%s!", retcode, server_url);
				res = CURLE_UNSUPPORTED_PROTOCOL;
			}
		}

UPLOAD_ERR:

		if(plist)
		{
			curl_slist_free_all(plist);
			plist = NULL;
		}
		if(curl)
		{
			curl_easy_cleanup(curl);
			curl = NULL;
		}
		curl_global_cleanup();
	}
	catch (std::exception &ex)
	{
		LOG_ERROR_FMT("curl exception %s.", ex.what());
	}

	return res;
}





