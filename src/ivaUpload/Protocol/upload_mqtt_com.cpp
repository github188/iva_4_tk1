#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <exception>
#include "upload_mqtt_com.h"
#include "xml_com.h"
#include "../UploadMan.h"
#include "oal_log.h"
#include "oal_file.h"
#include "oal_unicode.h"
#include "oal_time.h"
#include "safe_json.h"
#include "capacity.h"
#include "oal_base64.h"
#include "mq_master.h"
#include "curl/curl.h"
#include "upload_ftp_com.h"

#define REMOTE_UPGRADE_NAME  "/ivadata/upgrade/remote_upgrade.tar.gz"
#define LOCAL_UPGRADE_NAME	 "/ivadata/upgrade/upgrade.tar.gz"

#define MESSAGE_SIZE  1024L

void MQTT_ConnectCallback(struct mosquitto *mosq, void *obj, int rc)
{
	UploadByMQTT * ptMqtt = (UploadByMQTT*)obj;
	if (rc == 0)
	{
		LOG_DEBUG_FMT("MQTT connection success");
		return;
	}
	else if (rc == 1)
	{
		LOG_DEBUG_FMT("MQTT connection refused (unacceptable protocol version)");
	}
	else if (rc == 2)
	{
		LOG_DEBUG_FMT("MQTT connection refused (identifier rejected)");
	}
	else if (rc == 3)
	{
		LOG_DEBUG_FMT("MQTT connection refused (broker unavailable)");
	}
	else
	{
		LOG_DEBUG_FMT("MQTT connection refused (unknown)");
	}
	ptMqtt->CloseConnect();
}

void MQTT_DisconnectCallback(struct mosquitto *mosq, void *obj, int rc)
{
	UploadByMQTT * ptMqtt = (UploadByMQTT*)obj;
	if (rc == 0)
	{
		LOG_DEBUG_FMT("MQTT the client has called <mosquitto_disconnect>");
		ptMqtt->CloseConnect();
		return;
	}
	else
	{
		LOG_DEBUG_FMT("MQTT disconnect is unexpected");
		ptMqtt->UnexpectedDisconnect();
	}
	//ptMqtt->CloseConnect();
}

void MQTT_PublishCallback(struct mosquitto * mosq, void *obj, int mid)
{
	LOG_DEBUG_FMT("MQTT Publish %d OK",mid);
}

void MQTT_SubscribeCallback(struct mosquitto * mosq, void *obj, int mid, int qos_count, const int *granted_qos)
{
	LOG_DEBUG_FMT("MQTT Subscribe %d OK, qos_count=%d", mid, qos_count);
}

void MQTT_MessageCallback(struct mosquitto * mosq, void *obj, const struct mosquitto_message *msg)
{
	UploadByMQTT * ptMqtt = (UploadByMQTT*)obj;
	ptMqtt->DoSubscibe(msg);
}

void MQTT_UnsubscribeCallback(struct mosquitto *mosq, void *obj, int mid)
{
	LOG_DEBUG_FMT("MQTT Unsubscribe %d OK", mid);
}

void MQTT_LogCallback(struct mosquitto *mosq, void *obj, int level, const char *str)
{
	if (str == NULL)
	{
		return;
	}
	UploadByMQTT * ptMqtt = (UploadByMQTT*)obj;

	switch (level)
	{
	case MOSQ_LOG_INFO:
	case MOSQ_LOG_NOTICE:
		LOG_INFOS_FMT("%s",str);
		break;
	case MOSQ_LOG_WARNING:
		LOG_WARNS_FMT("%s",str);
		break;
	case MOSQ_LOG_ERR:
		LOG_ERROR_FMT("%s",str);
		break;
	case MOSQ_LOG_DEBUG:
		LOG_DEBUG_FMT("%s",str);
		break;
	default:
		break;
	}
}

void * mqtt_heart_to_server_thread_func(void * args)
{
	UploadByMQTT * ptMqtt = (UploadByMQTT*)args;
	if (ptMqtt == NULL)
	{
		return NULL;
	}

	bool bEnable = false;
	Platform tSvr = {0};
	LOG_DEBUG_FMT("Mqtt thread is working...");

	while (!ptMqtt->m_bExitThread)
	{
		Platform tNewSvr = {0};
		bEnable = UploadMan::GetInstance()->GetMqttAndEnable(&tNewSvr);

		if (!bEnable)
		{
			ptMqtt->CloseConnect();
			sleep(1);
			continue;
		}

		if (memcmp(&tNewSvr, &tSvr, sizeof(Platform))!=0)
		{
			ptMqtt->CloseConnect();
			memcpy(&tSvr, &tNewSvr, sizeof(Platform));
		}

		if (strlen(tSvr.szPlatID) < 1 || strlen(tSvr.szAddr) < 1)
		{
			ptMqtt->CloseConnect();
			sleep(1);
			continue;
		}

		// 一切条件都成熟了,还没有注册就开始注册
		if (!ptMqtt->IsConnected())
		{
			ptMqtt->Connect();
		}

		if (!ptMqtt->IsConnected())
		{
			sleep(1);
			continue;// 未注册成功
		}

		// 注册成功以后，检测是否需要发送状态
		ptMqtt->CheckRightNow();

		ptMqtt->PublishStatus();
		sleep(1);
	}

	return NULL;
}


ProtocolHandle UploadByMQTT::m_pInstance = NULL;

ProtocolHandle UploadByMQTT::GetInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance =  new UploadByMQTT();
		if (m_pInstance)
		{
			m_pInstance->InitSever();
		}
	}

	return m_pInstance;
}

int UploadByMQTT::InitSever()
{
	m_bExitThread = false;
	if(pthread_create(&m_heart_thread, NULL, mqtt_heart_to_server_thread_func, this))
	{
		LOG_ERROR_FMT("mqtt_heart_to_server_thread_func create failed");
	}
}

UploadByMQTT::UploadByMQTT(void)
{
	m_bExitThread = false;

	m_tLastHeart = 0;
	pthread_mutex_init(&m_tMutex, NULL);
	m_mqtt = NULL;
	memset(m_szSubTopic, 0, sizeof(m_szSubTopic));
	memset(m_szPubTopic, 0, sizeof(m_szPubTopic));
	mosquitto_lib_init();
	m_bRightNow = false;

	m_bNeedUploadAlarm = false;
	memset(&m_tUpOption, 0, sizeof(m_tUpOption));
	m_uploadaddr = "";
}

UploadByMQTT::~UploadByMQTT(void)
{
	m_bExitThread = true;
	pthread_join(m_heart_thread,NULL);
}

int UploadByMQTT::UploadData(const AnalyDbRecord *pData)
{
	int iRet = -1;
	if (pData == NULL)
	{
		return -1;
	}

	Platform  tPlatform = {0};
	if (!UploadMan::GetInstance()->GetMqttAndEnable(&tPlatform))
	{
		return -1;
	}

	char szAlarmUrl[512] = {0};
	sprintf(szAlarmUrl, "%s/upload?sn=%s&timestamp=%ld000", m_uploadaddr.c_str(), tPlatform.szPlatID, time(NULL));
	std::string szUrl  = szAlarmUrl;

	std::string szContent = "";
	iRet = PackeUploadDataJson(pData, szContent);
	if (iRet != 0)
	{
		return iRet;
	}

	std::string szResp  = "";
	iRet = -1;
	if (CurlPost(szUrl, szContent, szResp) == 0)
	{
		iRet = DoUploadResp(szResp);
	}

	return iRet;
}

bool UploadByMQTT::CanUploadData()
{
	if (IsConnected() && (m_tUpOption.type!=UpData_None))
	{
		return true;
	}
	return false;
}

bool UploadByMQTT::CanUploadAlarm()
{
	if (IsConnected() && (m_bNeedUploadAlarm))
	{
		return true;
	}
	return false;
}

bool UploadByMQTT::CanRemoveFile()
{
	if (IsConnected() && (m_tUpOption.type==UpData_Data || m_tUpOption.type == UpData_Ftp))
	{
		return true;
	}
	return false;
}

bool UploadByMQTT::IsConnected()
{
	return m_mqtt?true:false;
}

int UploadByMQTT::CurlPost(std::string &szUrl, std::string &szContent, std::string &szResp)
{
	int iRet = -1;

	try
	{
		CURLcode res = CURLE_READ_ERROR;
		CURL * curl  = NULL;
		curl_slist *headers = NULL;

		curl_global_init( CURL_GLOBAL_ALL );

		curl = curl_easy_init();
		if (NULL == curl)
		{
			LOG_ERROR_FMT("error: %s, URL:%s!", curl_easy_strerror(res), szUrl.c_str());
			res = CURLE_FAILED_INIT;
			goto ERR;
		}

		headers = curl_slist_append(headers, "Content-Type: application/json; charset=UTF-8");
		headers = curl_slist_append(headers, "Accept: application/json");

		curl_easy_setopt(curl, CURLOPT_URL, szUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, szContent.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_resp_data_writer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&szResp);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
		{
			LOG_ERROR_FMT("error: %s, URL:%s!", curl_easy_strerror(res), szUrl.c_str());
			goto ERR;
		}
		else
		{
			long lStatusCode = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &lStatusCode);
			if (lStatusCode != 200)
			{
				LOG_ERROR_FMT("Resp Status Code: %ld, URL:%s!", lStatusCode, szUrl.c_str());
				res = CURLE_UNSUPPORTED_PROTOCOL;
				goto ERR;
			}

			iRet = 0;
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
		LOG_ERROR_FMT("curl exception %s.", ex.what());
	}

	return iRet;
}

int UploadByMQTT::DoUploadResp( std::string szResp )
{
	Json::Reader jReader;
	JsonElement  jResp;
	if (!jReader.parse(szResp, jResp))
	{
		LOG_ERROR_FMT("Json::Reader parse: failed, %s!", szResp.c_str());
		return -1;
	}

	// 获取result
	int iCode = -1;
	std::string szDesc = "";
	if (!SafeJsonChildAsResult(jResp, iCode, szDesc))
	{
		LOG_ERROR_FMT("there is no \"result\" in resp!", szResp.c_str());
		return -1;
	}

	if (iCode != 0)
	{
		LOG_ERROR_FMT("code = %d, desc = %s!", iCode, szDesc.c_str());
		return -1;
	}

	return 0;
}

void UploadByMQTT::Connect()
{
	if (IsConnected())
	{
		//LOG_ERROR_FMT("Need Close Connect first...");
		return;
	}

	Platform tSvrInfo = {0};
	UploadMan::GetInstance()->GetMqttAndEnable(&tSvrInfo);

	char id[50] = {0};
#ifdef WIN32
	sprintf(id, "msgps_sub_%d", _getpid());
#else
	sprintf(id, "msgps_sub_%d", getpid());	
#endif
	m_mqtt = mosquitto_new(id, true, this);
	if (m_mqtt == NULL)
	{
		return;
	}

	mosquitto_connect_callback_set(m_mqtt, MQTT_ConnectCallback);
	mosquitto_disconnect_callback_set(m_mqtt, MQTT_DisconnectCallback);
	mosquitto_publish_callback_set(m_mqtt, MQTT_PublishCallback);
	mosquitto_message_callback_set(m_mqtt, MQTT_MessageCallback);
	mosquitto_subscribe_callback_set(m_mqtt, MQTT_SubscribeCallback);
	mosquitto_unsubscribe_callback_set(m_mqtt, MQTT_UnsubscribeCallback);
	mosquitto_log_callback_set(m_mqtt, MQTT_LogCallback);
	mosquitto_reconnect_delay_set(m_mqtt,3,30,true);

	int ret = mosquitto_connect(m_mqtt, tSvrInfo.szAddr, tSvrInfo.iPort, 600);
	if (ret == MOSQ_ERR_SUCCESS)
	{
		sprintf(m_szSubTopic, "/nms/%s", tSvrInfo.szPlatID);
		sprintf(m_szPubTopic, "/dev/%s", tSvrInfo.szPlatID);
		mosquitto_subscribe(m_mqtt, NULL, m_szSubTopic, 0);
		mosquitto_loop_start(m_mqtt);

		// 启动时发布
		PublishChannels();
		PublishVersion();
	}
	else
	{
		memset(m_szSubTopic, 0, sizeof(m_szSubTopic));
		memset(m_szPubTopic, 0, sizeof(m_szPubTopic));
		mosquitto_destroy(m_mqtt);
		m_mqtt = NULL;
	}
	m_tLastHeart = 0;
}

void UploadByMQTT::CloseConnect()
{
	if (!IsConnected())
	{
		//LOG_ERROR_FMT("Need Connect first...");
		return;
	}
	mosquitto_loop_stop(m_mqtt,true);
	mosquitto_unsubscribe(m_mqtt, NULL, m_szSubTopic);
	mosquitto_destroy(m_mqtt);
	m_mqtt = NULL;
	m_tLastHeart = 0;

	m_bNeedUploadAlarm = false;
	memset(&m_tUpOption, 0, sizeof(m_tUpOption));
	m_uploadaddr = "";
}

void UploadByMQTT::UnexpectedDisconnect()
{
	m_tLastHeart = 0;
}

int UploadByMQTT::PublishChannels()
{
	if (!IsConnected())
	{
		return -1;
	}

	Platform tSvrInfo = {0};
	UploadMan::GetInstance()->GetMqttAndEnable(&tSvrInfo);

	char timestamp[40] = {0};
	sprintf(timestamp, "%ld000", time(NULL));

	JsonElement jMQMsg;
	jMQMsg["sn"] = tSvrInfo.szPlatID;
	jMQMsg["timestamp"] = timestamp;
	jMQMsg["method"] = "channel";
	JsonElement jBody;
	{
		jBody["channelnum"] = MAX_CHANNEL_NUM;
	}
	jMQMsg["body"] = jBody;

	Json::FastWriter JWriter;
	std::string szMqttMsg = JWriter.write(jMQMsg);
	LOG_DEBUG_FMT("%s:%s", __FUNCTION__, szMqttMsg.c_str());

	int ret = mosquitto_publish(m_mqtt, NULL, m_szPubTopic, szMqttMsg.length(), szMqttMsg.c_str(), 0, false);

	m_tLastHeart = 0;

	return ret;
}

int UploadByMQTT::PublishVersion()
{
	if (!IsConnected())
	{
		return -1;
	}

	Platform tSvrInfo = {0};
	UploadMan::GetInstance()->GetMqttAndEnable(&tSvrInfo);

	char timestamp[40] = {0};
	sprintf(timestamp, "%ld000", time(NULL));

	JsonElement jMQMsg;
	jMQMsg["sn"] = tSvrInfo.szPlatID;
	jMQMsg["timestamp"] = timestamp;
	jMQMsg["method"] = "version";
	JsonElement jBody;
	{
		jBody["model"] = Global_DeviceModel();
		jBody["hardware"] = Global_HardVersion();
		jBody["software"] = Global_SoftVersion();
	}
	jMQMsg["body"] = jBody;

	Json::FastWriter JWriter;
	std::string szMqttMsg = JWriter.write(jMQMsg);
	LOG_DEBUG_FMT("%s:%s", __FUNCTION__, szMqttMsg.c_str());

	return mosquitto_publish(m_mqtt, NULL, m_szPubTopic, szMqttMsg.length(), szMqttMsg.c_str(), 0, false);
}

int UploadByMQTT::PublishStatus()
{
	if (!IsConnected())
	{
		return -1;
	}

	if (m_tLastHeart > 1 && time(NULL) - m_tLastHeart < NMS_HEART_CYCLE && m_tLastHeart < time(NULL))
	{
		return 0;
	}

	Platform tSvrInfo = {0};
	UploadMan::GetInstance()->GetMqttAndEnable(&tSvrInfo);

	char timestamp[40] = {0};
	sprintf(timestamp, "%ld000", time(NULL));

	JsonElement jMQMsg;
	jMQMsg["sn"] = tSvrInfo.szPlatID;
	jMQMsg["timestamp"] = timestamp;
	jMQMsg["method"] = "status";
	JsonElement jBody;

	char szAllStat[1024] = {0};
	MQ_Master_GetAllStatus(szAllStat);
	std::string szContent  = szAllStat;

	Json::Reader jReader;
	jReader.parse(szContent, jBody);

	jMQMsg["body"] = jBody;

	Json::FastWriter JWriter;
	std::string szMqttMsg = JWriter.write(jMQMsg);

	m_tLastHeart = time(NULL);
	LOG_DEBUG_FMT("%s:%s", __FUNCTION__, szMqttMsg.c_str());

	return mosquitto_publish(m_mqtt, NULL, m_szPubTopic, szMqttMsg.length(), szMqttMsg.c_str(), 0, false);
}

int UploadByMQTT::UploadAlarm(const AlarmDbRecord *pData)
{
	if (!IsConnected() || pData == NULL)
	{
		return -1;
	}

	Platform tSvrInfo = {0};
	UploadMan::GetInstance()->GetMqttAndEnable(&tSvrInfo);

	char timestamp[40] = {0};
	sprintf(timestamp, "%ld000", time(NULL));

	JsonElement jMQMsg;
	jMQMsg["sn"] = tSvrInfo.szPlatID;
	jMQMsg["timestamp"] = timestamp;
	jMQMsg["method"] = "alarm";
	JsonElement jBody;
	{
		jBody["type"] = pData->iType;
		jBody["level"] = pData->iLevel;
		char szTime[40] = {0};
		TimeFormatString(pData->tCreate, szTime, sizeof(szTime), eYMDHMS2);
		jBody["time"] = szTime;
		jBody["desc"] = pData->szContent;
	}
	jMQMsg["body"] = jBody;

	Json::FastWriter JWriter;
	std::string szMqttMsg = JWriter.write(jMQMsg);
	LOG_DEBUG_FMT("%s:%s", __FUNCTION__, szMqttMsg.c_str());

	return mosquitto_publish(m_mqtt, NULL, m_szPubTopic, szMqttMsg.length(), szMqttMsg.c_str(), 0, false);
}

void UploadByMQTT::CheckRightNow()
{
	if (m_bRightNow)
	{
		m_bRightNow = false;

		PublishChannels();
		PublishVersion();
		m_tLastHeart = 0;
		PublishStatus();
	}
}

int UploadByMQTT::PackeUploadDataJson( const AnalyDbRecord *pData, std::string &szContent )
{
	JsonElement jContent;
	jContent["datetype"] = pData->iDataType;
	jContent["where"] = pData->szPlaceName;

	char szWhen[40] = {0};
	TimeFormatString(pData->tCreate, szWhen, sizeof(szWhen), eYMDHMS1);
	jContent["when"] = szWhen;
	jContent["chnid"] = pData->iChnID;
	jContent["sceneid"] = pData->iSceneID;
	jContent["illegalcode"] = pData->szCode;
	jContent["illegaldesc"] = pData->szDesc;
	JsonElement jVehicle;
	{
		jVehicle["cartype"] = pData->iCarType;
		jVehicle["carcolor"] = "Z";//pData->iCarColor;
		jVehicle["carbrand"] = pData->iCarBrand;
		jVehicle["platetype"] = pData->iPlateType;
		jVehicle["platecolor"] = pData->iPlateColor;
		jVehicle["platenumber"] = pData->szPlate;
		JsonElement jPos;
		{
			jPos["x"] = pData->tPlatePos.x;
			jPos["y"] = pData->tPlatePos.y;
			jPos["w"] = pData->tPlatePos.w;
			jPos["h"] = pData->tPlatePos.h;
		}
		jVehicle["plateposition"] = jPos;
	}
	jContent["vehicle"] = jVehicle;

	JsonElement jSnap;
	{
		for (int i = 0; i < pData->iSnapNum; i++)
		{
			JsonElement jSnapi;
			jSnapi["time"] = pData->szSnapTime[i];
			jSnapi["snaptype"] = pData->iSnapType[i];
			JsonElement jPos;
			{
				jPos["x"] = pData->tSnapPos[i].x;
				jPos["y"] = pData->tSnapPos[i].y;
				jPos["w"] = pData->tSnapPos[i].w;
				jPos["h"] = pData->tSnapPos[i].h;
			}
			jSnapi["position"] = jPos;
			jSnap.append(jSnapi);
		}
	}
	jContent["snap"] = jSnap;

	if (m_tUpOption.bUploadImg)
	{
		JsonElement jImage;
		jImage["compose"] = (pData->iImgNum==pData->iSnapNum)?false:true;
		JsonElement jImageData;
		for (int i = 0; i < pData->iImgNum; i++)
		{
			char szFilePath[512] = {0};
			sprintf(szFilePath, "%s%s", pData->szImgRoot,pData->szImgPath[i]);

			JsonElement jImgi;
			if (m_tUpOption.type == UpData_Url)
			{
				jImgi["filename"] = szFilePath;
			}
			else if (m_tUpOption.type == UpData_Ftp)
			{
				char dst_dir_grammar_gbk[512] = {0};
				char dst_dir_grammar_utf8[512] = {0};
				char dst_dir_name_utf8[512] = {0};
				UploadMan::GetInstance()->GetFtpDirGrammar(pData->iDataType, dst_dir_grammar_gbk);
				GB2312_To_UTF8(dst_dir_grammar_gbk, strlen(dst_dir_grammar_gbk), dst_dir_grammar_utf8, sizeof(dst_dir_grammar_utf8));
				FtpSvrSubDir(pData, dst_dir_grammar_utf8, dst_dir_name_utf8);

				int ret = FtpUploadOneFile(&m_tUpOption.tFtpSvr,szFilePath, dst_dir_name_utf8, (char*)(pData->szImgPath[i]));
				if (ret != 0)
				{
					return ret;
				}

				char szFtppathUtf8[512] = {0};
				if (strlen(dst_dir_name_utf8)>0)
					sprintf(szFtppathUtf8, "%s/%s", dst_dir_name_utf8, pData->szImgPath[i]);
				else
					sprintf(szFtppathUtf8, "%s", pData->szImgPath[i]);

				jImgi["ftppath"] = szFtppathUtf8;
			}
			else if (m_tUpOption.type == UpData_Data)
			{
				char *base64_str = NULL;
				if (PackageFile2Base64Buf(szFilePath, &base64_str) != 0)
				{
					return -1;
				}
				jImgi["metadata"] = base64_str;
				delete []base64_str;
			}

			jImageData.append(jImgi);
		}
		jImage["imagedata"] = jImageData;

		jContent["image"] = jImage;
	}

	if (m_tUpOption.bUploadRec)
	{
		JsonElement jRecord;
		JsonElement jRecData;
		for (int i = 0; i < pData->iRecNum; i++)
		{
			char szFilePath[512] = {0};
			sprintf(szFilePath, "%s%s", pData->szRecRoot,pData->szRecPath[i]);

			JsonElement jReci;
			if (m_tUpOption.type == UpData_Url)
			{
				jReci["filename"] = szFilePath;
			}
			else if (m_tUpOption.type == UpData_Ftp)
			{
				char dst_dir_grammar_gbk[512] = {0};
				char dst_dir_grammar_utf8[512] = {0};
				char dst_dir_name_utf8[512] = {0};
				UploadMan::GetInstance()->GetFtpDirGrammar(pData->iDataType, dst_dir_grammar_gbk);
				GB2312_To_UTF8(dst_dir_grammar_gbk, strlen(dst_dir_grammar_gbk), dst_dir_grammar_utf8, sizeof(dst_dir_grammar_utf8));
				FtpSvrSubDir(pData, dst_dir_grammar_utf8, dst_dir_name_utf8);

				int ret = FtpUploadOneFile(&m_tUpOption.tFtpSvr,szFilePath, dst_dir_name_utf8, (char*)(pData->szRecPath[i]));
				if (ret != 0)
				{
					return ret;
				}

				char szFtppathUtf8[512] = {0};
				if (strlen(dst_dir_name_utf8)>0)
					sprintf(szFtppathUtf8, "%s/%s", dst_dir_name_utf8, pData->szRecPath[i]);
				else
					sprintf(szFtppathUtf8, "%s", pData->szRecPath[i]);

				jReci["ftppath"] = szFtppathUtf8;
			}
			else if (m_tUpOption.type == UpData_Data)
			{
				char *base64_str = NULL;
				if (PackageFile2Base64Buf(szFilePath, &base64_str) != 0)
				{
					return -1;
				}
				jReci["metadata"] = base64_str;
				delete []base64_str;
			}


			jRecData.append(jReci);
		}
		jRecord["recdata"] = jRecData;
		jContent["record"] = jRecord;
	}

	Json::FastWriter jWriter;
	szContent  = jWriter.write(jContent);
	return 0;
}

int UploadByMQTT::UploadFile( char * filename )
{
	int iRet = -1;
	if (filename == NULL)
	{
		return -1;
	}

	Platform  tPlatform = {0};
	if (!UploadMan::GetInstance()->GetMqttAndEnable(&tPlatform))
	{
		return -1;
	}

	char szAlarmUrl[512] = {0};
	sprintf(szAlarmUrl, "%s/uploadfile?sn=%s&timestamp=%ld000", m_uploadaddr.c_str(), tPlatform.szPlatID, time(NULL));
	std::string szUrl  = szAlarmUrl;

	char *base64_str = NULL;
	if (PackageFile2Base64Buf(filename, &base64_str) != 0)
	{
		return -1;
	}
	JsonElement jContent;
	jContent["filename"] = filename;
	jContent["metadata"] = base64_str;

	Json::FastWriter jWriter;
	std::string szContent = jWriter.write(jContent);

	delete []base64_str;

	std::string szResp  = "";
	if (CurlPost(szUrl, szContent, szResp) == 0)
	{
		iRet = DoUploadResp(szResp);
	}

	return iRet;
}

int UploadByMQTT::DoSubscibe( const struct mosquitto_message *msg )
{
	if (msg == NULL || msg->payload == NULL ||
		msg->payloadlen < 1 || msg->payloadlen > MESSAGE_SIZE-1)
	{
		return 0;
	}

	char szBody[MESSAGE_SIZE] = {0};
	memcpy(szBody, msg->payload, msg->payloadlen);

	std::string strJson = (const char *)(msg->payload);

	Json::Reader jReader;
	JsonElement jMsg;
	if(!jReader.parse(strJson, jMsg))
	{
		LOG_DEBUG_FMT("%s JSON Parse failed", __FUNCTION__);
		LOG_DEBUG_FMT("%s", strJson.c_str());
		return -1;
	}

	char szSN[128] = {0};
	if(!SafeJsonChildAsBuffer(jMsg, "sn", szSN, sizeof(szSN)))
	{
		LOG_DEBUG_FMT("Missing SN...");
		return -1;
	}

	Platform tSvrInfo = {0};
	UploadMan::GetInstance()->GetMqttAndEnable(&tSvrInfo);
	if (strcmp(szSN, tSvrInfo.szPlatID) != 0)
	{
		LOG_DEBUG_FMT("Wrong SN (need:%s, real:%s)",tSvrInfo.szPlatID, szSN);
		return -1;
	}

	char szTS[128] = {0};
	if(!SafeJsonChildAsBuffer(jMsg, "timestamp", szTS, sizeof(szTS)))
	{
		LOG_DEBUG_FMT("Missing timestamp...");
		return -1;
	}

	// 检测时间戳
	// ...

	char szMethod[128] = {0};
	if(!SafeJsonChildAsBuffer(jMsg, "method", szMethod, sizeof(szMethod)))
	{
		LOG_DEBUG_FMT("Missing timestamp...");
		return -1;
	}

	JsonElement jBody;
	if(!SafeJsonChildAsObj(jMsg, "body", jBody))
	{
		LOG_DEBUG_FMT("Missing body...");
		return -1;
	}

	if (strcmp(szMethod, "release") == 0)// 发布新版本
	{
		char szVersion[128] = {0};
		if(!SafeJsonChildAsBuffer(jBody, "version", szVersion, sizeof(szVersion)))
		{
			LOG_DEBUG_FMT("Missing version...");
			return -1;
		}

		char szUrl[512] = {0};
		if(!SafeJsonChildAsBuffer(jBody, "downloadurl", szUrl, sizeof(szUrl)))
		{
			LOG_DEBUG_FMT("Missing downloadurl...");
			return -1;
		}

		return GetUpgradePack(szUrl);
	}
	else if (strcmp(szMethod, "upload") == 0)// 上传地址
	{
		DataUploadOption tOption = {0};
		JsonElement jDataUpload;
		if (SafeJsonChildAsObj(jBody, "dataupload", jDataUpload))
		{

			if (!SafeJsonChildAsIntWithBound(jDataUpload, "type", tOption.type, UpData_None, UpData_Ftp))
				tOption.type = UpData_None;

			SafeJsonChildAsBool(jDataUpload, "image", tOption.bUploadImg);
			SafeJsonChildAsBool(jDataUpload, "record", tOption.bUploadRec);

			if (tOption.type == UpData_Ftp)
			{
				JsonElement jFtp;
				if(!SafeJsonChildAsObj(jDataUpload, "ftpsvr", jFtp))
					tOption.type = UpData_None;
				else
				{
					if(!SafeJsonChildAsBuffer(jFtp, "addr", tOption.tFtpSvr.szAddr, sizeof(tOption.tFtpSvr.szAddr)))
						tOption.type = UpData_None;

					if(!SafeJsonChildAsInt(jFtp, "port", tOption.tFtpSvr.iPort))
						tOption.type = UpData_None;

					if(!SafeJsonChildAsBuffer(jFtp, "user", tOption.tFtpSvr.szUser, sizeof(tOption.tFtpSvr.szUser)))
						tOption.type = UpData_None;

					char codedPass[PWD_LEN] = {0};
					if(!SafeJsonChildAsBuffer(jFtp, "pass", codedPass, sizeof(codedPass)))
						tOption.type = UpData_None;
					else
						Base64decode(tOption.tFtpSvr.szPass, codedPass); 
				}
			}
		}
		memcpy(&m_tUpOption, &tOption, sizeof(DataUploadOption));

		SafeJsonChildAsBool(jBody, "alarmupload", m_bNeedUploadAlarm);
		if(!SafeJsonChildAsString(jBody, "uploadaddr", m_uploadaddr))
			tOption.type = UpData_None;

		LOG_DEBUG_FMT("上传数据  type:%d Image:%d Record:%d Ftp:(%s:%s@%s:%d)",tOption.type, tOption.bUploadImg,tOption.bUploadRec,
			tOption.tFtpSvr.szUser,tOption.tFtpSvr.szPass,tOption.tFtpSvr.szAddr,tOption.tFtpSvr.iPort);
		LOG_DEBUG_FMT("上传告警  %d",m_bNeedUploadAlarm);
		LOG_DEBUG_FMT("上传地址  %s",m_uploadaddr.c_str());

	}
	else if (strcmp(szMethod, "control") == 0)// 发布新版本
	{
		int iTODO = -1;
		if(!SafeJsonChildAsInt(jBody, "todo", iTODO))
		{
			LOG_DEBUG_FMT("Missing todo...");
			return -1;
		}

		//char szParam[512] = {0};
		//if(!JsonChildAsBuffer(jBody, "param", szParam, sizeof(szParam), jError))
		//{
		//	LOG_DEBUG_FMT("Missing param...");
		//	return -1;
		//}

		if (iTODO == 0)
		{
			LOG_DEBUG_FMT("Reboot by NMS...");
			system("reboot");
		}
		else if (iTODO == 1)
		{
			LOG_DEBUG_FMT("Immediately Report by NMS...");
			ReportRightNow();
		}
		else
		{
			LOG_DEBUG_FMT("Unknown todo type %d...",iTODO);
		}
	}
	else if (strcmp(szMethod, "uploadfile") == 0)// 上传文件
	{
		char szFileName[512] = {0};
		if(!SafeJsonChildAsBuffer(jBody, "filename", szFileName, sizeof(szFileName)))
		{
			LOG_DEBUG_FMT("Missing todo...");
			return -1;
		}
		LOG_DEBUG_FMT("Immediately Upload File %s by NMS...", szFileName);
		UploadFile(szFileName);
	}
	else
	{
		LOG_DEBUG_FMT("Unknown method...");
		return -1;
	}

	return 0;
}


int UploadByMQTT::GetUpgradePack(const char * pszUrl)
{
	CURL * curl = NULL;
	CURLcode res = CURLE_READ_ERROR;
	long retcode = 0;	

	if(access(REMOTE_UPGRADE_NAME, F_OK) == 0)
	{
		unlink(REMOTE_UPGRADE_NAME);
	}

	OAL_MakeDirByPath(REMOTE_UPGRADE_NAME);

	FILE *fp = NULL;	
	fp = fopen(REMOTE_UPGRADE_NAME, "wb");
	if(fp == NULL)
	{
		printf("%s:%d ,error: %s!\n",__FUNCTION__ ,__LINE__ ,strerror(errno));
		return -1;
	}

	try
	{
		curl_global_init(CURL_GLOBAL_ALL);

		curl = curl_easy_init();
		if(curl == NULL)
		{
			printf("%s:%d ,error: %s!\n",__FUNCTION__ ,__LINE__ ,curl_easy_strerror(res));
			res = CURLE_FAILED_INIT;
			goto ERR;
		}

		curl_easy_setopt(curl, CURLOPT_URL, pszUrl);
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
		{
			printf("%s:%d ,error: %s!\n",__FUNCTION__ ,__LINE__ ,curl_easy_strerror(res));
		}
		else
		{
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
			if (retcode != 200)
			{
				printf("%s:%d ,RESPONSE_CODE: %ld\n",__FUNCTION__ ,__LINE__ , retcode);
				res = CURLE_UNSUPPORTED_PROTOCOL;
			}
		}

ERR:		
		if(fp)
		{
			fflush(fp);
			fclose(fp);
			fp = NULL;
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
		printf("curl exception %s.\n", ex.what());
	}

	if (res != CURLE_OK)
	{
		unlink(REMOTE_UPGRADE_NAME);
		return res;
	}
	else
	{
		OAL_MakeDirByPath(LOCAL_UPGRADE_NAME);
		unlink(LOCAL_UPGRADE_NAME);

		// 拷贝新文件
		rename(REMOTE_UPGRADE_NAME, LOCAL_UPGRADE_NAME);
		system("reboot");

		return 0;
	}
}

void UploadByMQTT::ReportRightNow()
{
	m_bRightNow = true;
}








