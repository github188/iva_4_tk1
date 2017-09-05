#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <exception>
#include "oal_log.h"
#include "oal_unicode.h"
#include "oal_time.h"
#include "oal_file.h"
#include "../UploadMan.h"
#include "upload_http_com.h"
#include "xml_com.h"
#include <curl/curl.h>
#include "safe_json.h"
#include "capacity.h"
#include "oal_base64.h"
#include "mq_master.h"
#include "ftp.h"
#include "upload_ftp_com.h"


#define MAX_POST_WAIT_GAP 120 // 两次POST失败间最大等待120秒


static void *http_heart_to_server_thread_func(void *args)
{
	UploadByHttpCommon * ptHttp = (UploadByHttpCommon*)args;
	if (ptHttp == NULL)
	{
		return NULL;
	}

	bool bEnable = false;
	Platform tSvr = {0};
	LOG_DEBUG_FMT("Http thread is working...");

	while (!ptHttp->m_bExitThread)
	{
		Platform tNewSvr = {0};
		bEnable = UploadMan::GetInstance()->GetPlatformAndEnable(&tNewSvr);

		if (!bEnable)
		{
			ptHttp->TurnToUregistered();
			sleep(1);
			continue;
		}

		if (memcmp(&tNewSvr, &tSvr, sizeof(Platform))!=0)
		{
			ptHttp->TurnToUregistered();
			memcpy(&tSvr, &tNewSvr, sizeof(Platform));
		}

		if (strlen(tSvr.szPlatID) < 1 || strlen(tSvr.szAddr) < 1)
		{
			ptHttp->TurnToUregistered();
			sleep(1);
			continue;
		}
		
		// 一切条件都成熟了,还没有注册就开始注册
		if (!ptHttp->m_bRegistedOk)
		{
			ptHttp->CheckRegister();
		}

		if (!ptHttp->m_bRegistedOk)
		{
			sleep(1);
			continue;// 未注册成功
		}

		// 注册成功以后，检测是否需要发送状态
		ptHttp->CheckStatus();
		sleep(1);
	}

	return NULL;
}

ProtocolHandle UploadByHttpCommon::m_pInstance = NULL;

ProtocolHandle UploadByHttpCommon::GetInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance =  new UploadByHttpCommon();
		if (m_pInstance)
		{
			m_pInstance->InitSever();
		}
	}

	return m_pInstance;
}

int UploadByHttpCommon::InitSever()
{
	m_bExitThread = false;
	if(pthread_create(&m_heart_thread, NULL, http_heart_to_server_thread_func, this))
	{
		LOG_ERROR_FMT("http_heart_to_server_thread_func create failed");
	}
}

UploadByHttpCommon::UploadByHttpCommon(void)
{
	m_bExitThread = false;
	m_bRegistedOk = false;

	m_last_post_time = 0;
	m_need_wait_secs = 0;

	m_bNeedUploadAlarm = false;
	memset(&m_tUpOption, 0, sizeof(m_tUpOption));
}

UploadByHttpCommon::~UploadByHttpCommon(void)
{
	m_bExitThread = true;
	m_bRegistedOk = false;
	pthread_join(m_heart_thread,NULL);
}

int UploadByHttpCommon::PostSomething(std::string &szUrl, std::string &szContent, std::string &szResp)
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
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
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

int UploadByHttpCommon::UploadData(const AnalyDbRecord *pData)
{
	int iRet = -1;
	if (pData == NULL)
	{
		return -1;
	}

	Platform  tPlatform = {0};
	if (!UploadMan::GetInstance()->GetPlatformAndEnable(&tPlatform))
	{
		return -1;
	}

	char szUploadUrl[512] = {0};
	sprintf(szUploadUrl, "%s/upload?sn=%s&timestamp=%ld000", tPlatform.szAddr, tPlatform.szPlatID, time(NULL));
	std::string szUrl  = szUploadUrl;

	std::string szContent = "";
	iRet = PackeUploadDataJson(pData, szContent);
	if (iRet != 0)
	{
		return iRet;
	}

	std::string szResp  = "";
	iRet = -1;
	if (PostSomething(szUrl, szContent, szResp) == 0)
	{
		iRet = DoUploadResp(szResp);
	}

	if (iRet != 0)
	{
		TurnToUregistered();
	}

	return iRet;
}

int UploadByHttpCommon::UploadAlarm( const AlarmDbRecord *pData )
{
	int iRet = -1;

	if (pData == NULL)
	{
		return -1;
	}

	Platform  tPlatform = {0};
	if (!UploadMan::GetInstance()->GetPlatformAndEnable(&tPlatform))
	{
		return -1;
	}

	char szAlarmUrl[512] = {0};
	sprintf(szAlarmUrl, "%s/alarm?sn=%s&timestamp=%ld000", tPlatform.szAddr, tPlatform.szPlatID, time(NULL));
	std::string szUrl  = szAlarmUrl;

	JsonElement jContent;
	jContent["type"] = pData->iType;
	jContent["level"] = pData->iLevel;
	char szTime[40] = {0};
	TimeFormatString(pData->tCreate, szTime, sizeof(szTime), eYMDHMS2);
	jContent["time"] = szTime;
	jContent["desc"] = pData->szContent;

	Json::FastWriter jWriter;
	std::string szContent  = jWriter.write(jContent);

	std::string szResp  = "";

	if (PostSomething(szUrl, szContent, szResp) == 0)
	{
		iRet = DoUploadResp(szResp);
	}

	if (iRet != 0)
	{
		TurnToUregistered();
	}

	return iRet;
}

bool UploadByHttpCommon::CanUploadData()
{
	return (m_bRegistedOk&&(m_tUpOption.type!=UpData_None));
}

bool UploadByHttpCommon::CanUploadAlarm()
{
	return (m_bRegistedOk&&m_bNeedUploadAlarm);
}

bool UploadByHttpCommon::CanRemoveFile()
{
	return (m_bRegistedOk&&(m_tUpOption.type==UpData_Data || m_tUpOption.type == UpData_Ftp));
}

void UploadByHttpCommon::TurnToUregistered()
{
	if (m_bRegistedOk)
	{
		LOG_INFOS_FMT("Turn To Uregistered!");
	}

	m_last_post_time = -1;
	m_need_wait_secs = 0;
	m_bRegistedOk =  false;
}

int UploadByHttpCommon::CheckRegister()
{
	if (m_bRegistedOk)
	{
		return 0;
	}

	// 等比数列式发送，失败+等待1秒+失败+等待2秒+失败+等待4秒+失败+等待8秒.....直到最大等待64秒
	if (m_last_post_time <= 0 || (m_last_post_time > 0 && m_last_post_time + m_need_wait_secs <= time(NULL)))
	{
		m_last_post_time = time(NULL);
		int iRet = PostRegister();
		if (iRet == 0)
		{
			m_need_wait_secs = 0;
			m_bRegistedOk = true;
			LOG_INFOS_FMT("Register OK!");
		}
		else
		{
			if (m_need_wait_secs < 1)
				m_need_wait_secs = 1;
			else if (m_need_wait_secs >= MAX_POST_WAIT_GAP)
				m_need_wait_secs = MAX_POST_WAIT_GAP;
			else
				m_need_wait_secs <<= 1;
			m_bRegistedOk = false;
			LOG_INFOS_FMT("Register Failed, Next Wait %d Secs!",m_need_wait_secs);
		}
	}
	else
	{
		return 0;// 再等等
	}
}

int UploadByHttpCommon::PostRegister()
{
	int iRet = -1;
	Platform  tPlatform = {0};
	if (!UploadMan::GetInstance()->GetPlatformAndEnable(&tPlatform))
	{
		return -1;
	}

	char szRegUrl[512] = {0};
	sprintf(szRegUrl, "%s/register?sn=%s&timestamp=%ld000", tPlatform.szAddr, tPlatform.szPlatID, time(NULL));
	std::string szUrl  = szRegUrl;

	JsonElement jContent;
	jContent["channelnum"] = MAX_CHANNEL_NUM;

	Json::FastWriter jWriter;
	std::string szContent  = jWriter.write(jContent);

	std::string szResp  = "";
	if (PostSomething(szUrl, szContent, szResp) == 0)
	{
		iRet = DoRegisterResp(szResp);
	}

	return iRet;
}

int UploadByHttpCommon::DoRegisterResp(std::string szResp)
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

	// 解析dataupload
	DataUploadOption tOption = {0};
	JsonElement jDataUpload;
	if (SafeJsonChildAsObj(jResp, "dataupload", jDataUpload))
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

	SafeJsonChildAsBool(jResp, "alarmupload", m_bNeedUploadAlarm);

	int type;
	FtpSvr tFtpSvr;
	bool bUploadImg;
	bool bUploadRec;

	LOG_DEBUG_FMT("上传数据  type:%d Image:%d Record:%d Ftp:(%s:%s@%s:%d)",tOption.type, tOption.bUploadImg,tOption.bUploadRec,
		tOption.tFtpSvr.szUser,tOption.tFtpSvr.szPass,tOption.tFtpSvr.szAddr,tOption.tFtpSvr.iPort);
	LOG_DEBUG_FMT("上传告警  %d",m_bNeedUploadAlarm);

	return 0;
}

int UploadByHttpCommon::CheckStatus()
{
	if (!m_bRegistedOk)
	{
		return -1;
	}

	// 等比数列式发送，失败+等待1秒+失败+等待2秒+失败+等待4秒+失败+等待8秒.....直到最大等待64秒
	if ( m_last_post_time <= 0 ||
		(m_last_post_time > 0 && m_last_post_time + m_need_wait_secs <= time(NULL)) ||
		 m_last_post_time > time(NULL))// 对时造成的时间提前
	{
		m_last_post_time = time(NULL);
		int iRet = PostStatus();
		if (iRet == 0)
		{
			m_need_wait_secs = NMS_HEART_CYCLE;
			LOG_INFOS_FMT("Status OK!");
		}
		else
		{
			m_need_wait_secs = 0;
			m_bRegistedOk = false;
			LOG_INFOS_FMT("Status Failed!");
		}
	}
	else
	{
		return 0;// 再等等
	}
}

int UploadByHttpCommon::PostStatus()
{
	int iRet = -1;

	Platform  tPlatform = {0};
	if (!UploadMan::GetInstance()->GetPlatformAndEnable(&tPlatform))
	{
		return -1;
	}

	char szHeartUrl[512] = {0};
	sprintf(szHeartUrl, "%s/status?sn=%s&timestamp=%ld000", tPlatform.szAddr, tPlatform.szPlatID, time(NULL));
	std::string szUrl  = szHeartUrl;

	char szAllStat[1024] = {0};
	MQ_Master_GetAllStatus(szAllStat);
	std::string szContent  = szAllStat;

	std::string szResp  = "";

	if (PostSomething(szUrl, szContent, szResp) == 0)
	{
		iRet = DoStatusResp(szResp);
	}

	if (iRet != 0)
	{
		TurnToUregistered();
	}

	return iRet;
}

int UploadByHttpCommon::DoStatusResp( std::string szResp )
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

	// 解析timesyn
	JsonElement jTimesyn;
	if (SafeJsonChildAsObj(jResp, "timesyn", jTimesyn))
	{
		bool bEnable = false;
		SafeJsonChildAsBool(jTimesyn, "enable", bEnable);
		if (bEnable)
		{
			char buf1[24] = {0};
			if(SafeJsonChildAsBuffer(jTimesyn, "time", buf1, sizeof(buf1)))
			{
				time_t tPlatTime = String2Time(buf1, eYMDHMS1);
				if (tPlatTime < 0)//"%Y-%m-%d %H:%M:%S"
				{
					tPlatTime = String2Time(buf1, eYMDHMS2);//"%Y%m%d%H%M%S"
				}
				if (tPlatTime > 0)
				{
					time_t tNow = time(NULL);
					char buf2[24] = {0};
					TimeFormatString(tPlatTime, buf1, sizeof(buf1),eYMDHMS1);
					TimeFormatString(tNow, buf2, sizeof(buf2),eYMDHMS1);

					time_t iDeta = (tNow > tPlatTime) ? (tNow - tPlatTime) : (tPlatTime - tNow);
					if(iDeta >= TIME_SYN_PRECISION)
					{
						LOG_DEBUG_FMT("============TIME CHECK==========");
						LOG_DEBUG_FMT("Time in Platform = %s",buf1);
						LOG_DEBUG_FMT("Time in System   = %s",buf2);

						MQ_Master_SetSysPlatTime(tPlatTime);
					}
				}
				else
				{
					LOG_ERROR_FMT("Time Format Wrong, %s",buf1);
				}
			}
		}
	}
	return 0;
}

int UploadByHttpCommon::DoUploadResp( std::string szResp )
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

int UploadByHttpCommon::PackeUploadDataJson( const AnalyDbRecord *pData, std::string &szContent )
{
	int iRet = 0;
	char *img_base64_str[MAX_IMG_NUM] = {NULL};
	char *rec_base64_str[MAX_REC_NUM] = {NULL};
	Json::FastWriter jWriter;

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
		jVehicle["carcolor"] = "Z"/*pData->iCarColor*/;
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
				char szFileUrl[512] = {0};
				int ret = File2HttpBoaUrl(szFilePath, szFileUrl);
				if (ret != 0)
				{
					return ret;
				}

				jImgi["url"] = szFileUrl;
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
				iRet = PackageFile2Base64Buf(szFilePath, &(img_base64_str[i]));
				if ( iRet != 0)
				{
					goto ErrOut;
				}
				jImgi["metadata"] = img_base64_str[i];
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
				char szFileUrl[512] = {0};
				int ret = File2HttpBoaUrl(szFilePath, szFileUrl);
				if (ret != 0)
				{
					return ret;
				}

				jReci["url"] = szFileUrl;
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
				iRet = PackageFile2Base64Buf(szFilePath, &(rec_base64_str[i]));
				if ( iRet != 0)
				{
					goto ErrOut;
				}

				jReci["metadata"] = rec_base64_str[i];
			}

			jRecData.append(jReci);
		}
		jRecord["recdata"] = jRecData;
		jContent["record"] = jRecord;
	}

	szContent  = jWriter.write(jContent);

ErrOut:
	for (int i = 0; i< MAX_IMG_NUM; i++)
	{
		if (img_base64_str[i])
		{
			delete [] img_base64_str[i];
			img_base64_str[i] = NULL;
		}
	}
	for (int i = 0; i< MAX_REC_NUM; i++)
	{
		if (rec_base64_str[i])
		{
			delete [] rec_base64_str[i];
			rec_base64_str[i] = NULL;
		}
	}
	return iRet;
}





