#include "application.h"
#include "mq_master.h"
#include "mq_videoin.h"
#include "mq_decode.h"
#include "mq_record.h"
#include "mq_onvif.h"
#include "mq_analysis.h"
#include "mq_upload.h"
#include "mq_database.h"
#include "mq_videoout.h"
#include "oal_log.h"
#include "oal_time.h"
#include "oal_unicode.h"
#include "ISAPI/isapi_handler.h"
#include "ISAPI/isapi_platform.h"
#include "Profile/SystemProfile.h"
#include "Profile/AnalyProfile.h"
#include "Manager/UserManager.h"
#include "Manager/NetworkManager.h"
#include "Manager/SystemStatusMonitor.h"
#include "Manager/DeviceManager.h"
#include "Manager/SoftAuthManager.h"
#include "Manager/VideoInManager.h"
#include "Manager/AlarmManager.h"
#include "Manager/LogManager.h"
#include "Manager/TempFileCleanup.h"
#include "Manager/AnalyManager.h"
#include "Manager/UploadManager.h"
//#include "Manager/NmsManager.h"

void Application::init()
{
	LogInit("ivaMaster", 10*1024*1024);// 日志最多10M

	// 初始化所有消息队列
	MQ_Database_Init();
	MQ_Upload_Init();
	MQ_Analy_Init();
	MQ_Onvif_Init();
	MQ_Record_Init();
	MQ_VideoOut_Init();
	MQ_Decode_Init();
	MQ_VideoIn_Init();
	m_iMasterMqid = MQ_Master_Init();
	if(m_iMasterMqid < 0)
	{
		LOG_ERROR_FMT("MQ_Master_Init error");
		return;
	}

	// 初始化各个管理模块
	SoftAuthManager::Initialize();
	TempFileCleanUp::Initialize();
	SystemProfile::Initialize();
	AnalyProfile::Initialize();
	UserManager::Initialize();
	NetworkManager::Initialize();
	SystemMaintain::Initialize();
	SystemStatusMonitor::Initialize();
	DeviceManager::Initialize();
	VideoInManager::Initialize();
	AlarmManager::Initialize();
	LogManager::Initialize();
	AnalyManager::Initialize();
	RecManager::Initialize();
	UploadManager::Initialize();
	//NmsManager::Initialize();
}

int Application::exec()
{
	MQ_BUF tMqBuf;
	LOG_INFOS_FMT("******************ivaMaster Start******************");
	while(1)
	{
		memset(&tMqBuf, 0, sizeof(MQ_BUF));

		int iRecvLen = Msg_Recv(m_iMasterMqid, MSG_TYPE_MASTER, &tMqBuf, sizeof(MQ_BUF));
		if(iRecvLen < 0)
		{
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				LOG_ERROR_FMT("Msg_Recv failed, %s",strerror(errno));
				break;
			}
		}		
		else
		{
			switch(tMqBuf.lCmd)
			{
			case MASTER_CMD_ISAPI:
				{
					ISAPI_REQ tReq = {0};
					memcpy(&tReq, tMqBuf.data, sizeof(ISAPI_REQ));

					LOG_DEBUG_FMT("[MASTER_CMD_ISAPI] METHOD:%s URI:%s Auth:%s Query:%s IP:%s",
						GetISAPIMethod(tReq.method),
						tReq.szURI,
						tReq.szAuth,
						tReq.szQuery,
						tReq.szRemoteIP);

					char  req_json[MSG_DATA_SIZE] = {0};
					if (tMqBuf.lLen-sizeof(ISAPI_REQ) > 0)
					{
						memcpy(req_json, tMqBuf.data+sizeof(ISAPI_REQ), tMqBuf.lLen-sizeof(ISAPI_REQ));
					}

					// 用户认证()
					bool bAuthPass = false;
					if (!IsNeedCheckAuthorization(tReq.szURI, tReq.method))
					//if (strcmp(tReq.szURI,"/ISAPI/Security/UserCheck") == 0 && tReq.method == ISAPI_PUT)
					{
						bAuthPass = true;//UserCheck_Get不需要Authorization
					}
					else
					{
						char szUserName[32] = {0};
						char szPassword[32] = {0};
						bAuthPass = AuthorizationParse(tReq.szAuth, szUserName, szPassword);
						//LOG_DEBUG_FMT("AuthorizationParse User:%s Pass:%s",szUserName,szPassword);

						if(bAuthPass)
						{
							int iRet = UserManager::GetInstance()->CheckUser(szUserName,szPassword);
							bAuthPass = (iRet == 0);
						}
					}

					ISAPI_RESP tResp;
					char  resp_json[MSG_DATA_SIZE] = {0};

					// 用户鉴权通过以后再处理
					if(bAuthPass)
					{
						ISAPI_HANDLE * handle = GetISAPIHandle(tReq.szURI);
						if(handle)
						{
							HANDLE_FUNC pFunc = GetISAPIHandleFunc(handle, tReq.method);
							if(pFunc)
							{
								bool bPraseOk = true;
								JsonElement jRoot;
								JsonElement jRespRoot;

								if (strlen(req_json) > 0)
								{
									string str_req_json = req_json;
									
									Json::Reader jReader;
									bPraseOk = jReader.parse(str_req_json, jRoot);
								}

								if(bPraseOk)
								{
									tResp.code = 200;//ok
									pFunc(tReq,jRoot,jRespRoot);// 总是返回200
								}
								else
								{
									tResp.code = 200;//ok
									WriteJsonResultError(ISAPI_FAILED_PARSING, jRespRoot);
								}

								Json::FastWriter jWriter;
								string str_resp_json  = jWriter.write(jRespRoot);
								strcpy(resp_json, str_resp_json.c_str());
							}
							else
							{
								tResp.code = 403;// forbidden
							}							
						}
						else
						{
							tResp.code = 400;// bad request
						}
					}
					else
					{
						tResp.code = 401;// unauthorized
					}
					char  resp_json_gb2312[MSG_DATA_SIZE] = {0};
					UTF8_To_GB2312(resp_json, strlen(resp_json),resp_json_gb2312, sizeof(resp_json_gb2312));
					LOG_DEBUG_FMT("Resp:code=%d json=%s", tResp.code, resp_json_gb2312);
					MQ_Master_ISAPI_Response(&tResp, resp_json);
					break;
				}
			case MASTER_CMD_SET_RTSP:	// ONVIF->MASTER
				{
					int * data = (int *)tMqBuf.data;
					int iChnID = data[0];
					int iStreamType = data[1];
					RtspInfo * ptRtsp = (RtspInfo *)(tMqBuf.data+2*sizeof(int));

					LOG_DEBUG_FMT("[MASTER_CMD_SET_RTSP] Channel:%d StreamType:%d %d*%d URI:%s",
						iChnID, iStreamType, ptRtsp->iWidth, ptRtsp->iHeight, ptRtsp->szUrl);

					VideoInManager::GetInstance()->SetChnRtsp(iChnID, iStreamType, ptRtsp);
					break;
				}
			case MASTER_CMD_SET_PTZCAP:
				{
					int iChnID = *((int *)tMqBuf.data);
					PTZCap * ptPtzCap = (PTZCap *)(tMqBuf.data+sizeof(int));

					LOG_DEBUG_FMT("[MASTER_CMD_SET_PTZCAP] Channel:%d SptPtz:%d SptAbsMove:%d SptClickZoom:%d MaxPresetNum:%d",
						iChnID, ptPtzCap->bSptPtz, ptPtzCap->bSptAbsMove, ptPtzCap->bSptClickZoom,ptPtzCap->iMaxPresetNum);

					VideoInManager::GetInstance()->SetChnPtzCap(iChnID, ptPtzCap);
					break;
				}
			case MASTER_CMD_SET_SYSTIME:
				{
					time_t tOnvifTime;
					memcpy(&tOnvifTime, tMqBuf.data, sizeof(time_t));

					char buf[24] = {0};
					TimeFormatString(tOnvifTime, buf, sizeof(buf), eYMDHMS1);
					LOG_DEBUG_FMT("[MASTER_CMD_SET_SYSTIME] Time:%s",buf);

					SystemMaintain::GetInstance()->SetSystemTime(buf);
					break;
				}
			case MASTER_CMD_VIDEOIN_STAT:
				{
					int * data = (int *)tMqBuf.data;
					int iChnID = data[0];
					int iStatus = data[1];

					LOG_DEBUG_FMT("[MASTER_CMD_VIDEOIN_STAT] Channel:%d Status:%d",
						iChnID, iStatus);

					VideoInManager::GetInstance()->SetChnStatus(iChnID, iStatus);

					break;
				}
			case MASTER_CMD_RECORD_STAT:
				{
					int * data = (int *)tMqBuf.data;
					int iChnID = data[0];
					int iStatus = data[1];

					LOG_DEBUG_FMT("[MASTER_CMD_RECORD_STAT] Channel:%d Status:%d",
						iChnID, iStatus);

					RecManager::GetInstance()->SetChnStatus(iChnID, iStatus);

					break;
				}
			case MASTER_CMD_ANALY_VERSION:
				{
					const char * ver = (const char *)tMqBuf.data;

					LOG_DEBUG_FMT("[MASTER_CMD_ANALY_VERSION] %s",ver);

					AnalyManager::GetInstance()->SetAnalyVersion(ver);

					break;
				}
			case MASTER_CMD_ANALY_STAT:
				{
					int * data = (int *)tMqBuf.data;
					int iChnID = data[0];
					int iStatus = data[1];

					LOG_DEBUG_FMT("[MASTER_CMD_ANALY_STAT] Channel:%d Status:%d",
						iChnID, iStatus);

					AnalyManager::GetInstance()->SetAnalyStatus(iChnID, iStatus);

					break;
				}
			case MASTER_CMD_UPLOAD_STAT:
				{
					const UploadStatus * data = (const UploadStatus *)tMqBuf.data;

					LOG_DEBUG_FMT("[MASTER_CMD_UPLOAD_STAT] Uploading:%d ServerStat:%d Remaining:%d",
						data->bUploading,data->iServerStat,data->bRemaining);

					UploadManager::GetInstance()->SetUploadStatus(data);
					break;
				}
			case MASTER_CMD_GETALLSTAT:
				{
					JsonElement jRespRoot;
					ISAPI::PlatformNS::AllStat_GET(jRespRoot);

					Json::FastWriter jWriter;
					string str_resp_json  = jWriter.write(jRespRoot);

					char  resp_json[MSG_DATA_SIZE] = {0};
					strcpy(resp_json, str_resp_json.c_str());
					MQ_Master_Response_AllStatus(resp_json);

					break;
				}
			default:
				break;
			}
		}
	}

	return 0;
}

void Application::quit(int returnCode)
{
	//NmsManager::UnInitialize();
	AnalyManager::UnInitialize();
	RecManager::UnInitialize();
	UploadManager::UnInitialize();
	LogManager::UnInitialize();
	AlarmManager::UnInitialize();
	VideoInManager::UnInitialize();
	DeviceManager::UnInitialize();
	SystemMaintain::UnInitialize();
	SystemStatusMonitor::UnInitialize();
	NetworkManager::UnInitialize();
	UserManager::UnInitialize();
	SystemProfile::UnInitialize();
	AnalyProfile::UnInitialize();
	TempFileCleanUp::UnInitialize();
	SoftAuthManager::UnInitialize();
	MQ_Master_Close();

	LogUninit();

	exit(returnCode);
}
