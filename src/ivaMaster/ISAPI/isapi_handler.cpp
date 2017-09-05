#include <stdarg.h>
#include "isapi_handler.h"
#include "isapi_json.h"
#include "isapi_system.h"
#include "isapi_videoin.h"
#include "isapi_security.h"
#include "isapi_record.h"
#include "isapi_analysis.h"
#include "isapi_alarm.h"
#include "isapi_log.h"
#include "isapi_upload.h"
#include "isapi_notify.h"
#include "isapi_videoout.h"
#include "oal_base64.h"
#include "oal_unicode.h"
#include "oal_log.h"
#include "oal_file.h"
#include "../Manager/UserManager.h"
#include "../Manager/LogManager.h"
#include "../Manager/AlarmManager.h"

using namespace ISAPI;

using namespace std;

ISAPI_HANDLE g_ISAPI_List[] = 
{
	// System
	{"/ISAPI/System/Capabilities",	System::Capabilities_GET,	NULL,					NULL,NULL},
	{"/ISAPI/System/Status",		System::Status_GET, 		NULL,					NULL,NULL},

	// System/Device
	{"/ISAPI/System/DeviceInfo",	System::Deviceinfo_GET, 	System::Deviceinfo_PUT,	NULL,NULL},

	// System/Time
	{"/ISAPI/System/Time/Capabilities",	System::Time_Capabilities_GET,	NULL,							NULL,NULL},
	{"/ISAPI/System/Time/Status",		System::Time_Status_GET, 		System::Time_Status_PUT,		NULL,NULL},
	{"/ISAPI/System/Time/SynMechanism",	System::Time_SynMechanism_GET,	System::Time_SynMechanism_PUT,	NULL,NULL},
	{"/ISAPI/System/Time/CheckNtp",		NULL,							System::Time_CheckNtp_PUT,		NULL,NULL},

	// System/GPS
	{"/ISAPI/System/GPS/Capabilities",	System::GPS_Capabilities_GET,	NULL,							NULL,NULL},
	{"/ISAPI/System/GPS/Status",		System::GPS_Status_GET,			System::GPS_Status_PUT,			NULL,NULL},

	// System/Network
	{"/ISAPI/System/Network/Capabilities",	System::Network_Capabilities_GET,	NULL,							NULL,NULL},
	{"/ISAPI/System/Network/Status",		System::Network_Status_GET,			NULL,							NULL,NULL},
	{"/ISAPI/System/Network/Interfaces",	System::Network_Interfaces_GET,		System::Network_Interfaces_PUT,	NULL,NULL},
	{"/ISAPI/System/Network/DNS",			System::Network_DNS_GET,			System::Network_DNS_PUT,		NULL,NULL},
	// System/Network/WlanHot
	{"/ISAPI/System/Network/WlanHot",		System::Network_WlanHot_GET,		System::Network_WlanHot_PUT,	NULL,NULL},

	{"/ISAPI/System/Network/4G",			System::Network_4G_GET,				System::Network_4G_PUT,			NULL,NULL},

	// System/Store
	{"/ISAPI/System/Store/Capabilities",	System::Store_Capabilities_GET,	NULL,							NULL,NULL},
	{"/ISAPI/System/Store/Status",			System::Store_Status_GET,		NULL,							NULL,NULL},
	{"/ISAPI/System/Store/Disks",			System::Store_Disks_GET,   		System::Store_Disks_PUT,		NULL,NULL},
	{"/ISAPI/System/Store/FormatDisk",		NULL,							System::Store_FormatDisk_PUT,	NULL,NULL},
	{"/ISAPI/System/Store/FullStrategy",	System::Store_FullStrategy_GET, System::Store_FullStrategy_PUT,	NULL,NULL},

	// System/Maintain
	{"/ISAPI/System/Maintain/Capabilities",		System::Maintain_Capabilities_GET,	NULL,							NULL,NULL},
	{"/ISAPI/System/Maintain/Reboot",			NULL,								System::Maintain_Reboot_PUT,	NULL,NULL},
	{"/ISAPI/System/Maintain/RebootEx",			NULL,								System::Maintain_Reboot_PUT,	NULL,NULL},
	{"/ISAPI/System/Maintain/TimerReboot",		System::Maintain_TimerReboot_GET,	System::Maintain_TimerReboot_PUT,NULL,NULL},
	{"/ISAPI/System/Maintain/FactoryRestore",	NULL,								System::Maintain_FactoryRestore_PUT,NULL,NULL},
	{"/ISAPI/System/Maintain/Profile",			System::Maintain_Profile_GET,NULL,	System::Maintain_Profile_POST,NULL},
	{"/ISAPI/System/Maintain/Upgrade",			NULL,	NULL,						System::Maintain_Upgrade_POST,NULL},
	{"/ISAPI/System/Maintain/UpgradeEx",		NULL,	NULL,						System::Maintain_Upgrade_POST,NULL},

	// System/SoftAuth
	{"/ISAPI/System/SoftAuth",	System::SoftAuth_GET,	System::SoftAuth_PUT,NULL, System::SoftAuth_DELETE},

	// VideoIn
	{"/ISAPI/VideoIn/Capabilities",	VideoIn::Capabilities_GET,	NULL,NULL,NULL},
	{"/ISAPI/VideoIn/Status",		VideoIn::Status_GET,		NULL,NULL,NULL},
	{"/ISAPI/VideoIn/Source",		VideoIn::Source_GET,		VideoIn::Source_PUT,	VideoIn::Source_POST,VideoIn::Source_DELETE},
	{"/ISAPI/VideoIn/Stream",		VideoIn::Stream_GET,	    NULL,NULL,NULL},

	// VideoOut
	{"/ISAPI/VideoOut/Capabilities",VideoOut::Capabilities_GET,	NULL,NULL,NULL},
	{"/ISAPI/VideoOut/Strategy", 	VideoOut::Strategy_GET,		VideoOut::Strategy_PUT,	  NULL,NULL},
	{"/ISAPI/VideoOut/RtmpServer", 	VideoOut::RtmpServer_GET,	VideoOut::RtmpServer_PUT, NULL,NULL},
	{"/ISAPI/VideoOut/Switch", 		VideoOut::Switch_GET,		VideoOut::Switch_PUT,	  NULL,NULL},

	// VideoIn/PTZ
	{"/ISAPI/VideoIn/PTZ/Capabilities",		VideoIn::PTZ_Capabilities_GET,	NULL,NULL,NULL},
	{"/ISAPI/VideoIn/PTZ/PresetList", 		VideoIn::PTZ_PresetList_GET,	NULL,NULL,NULL},
	{"/ISAPI/VideoIn/PTZ/Preset", 			VideoIn::PTZ_Preset_GET,		VideoIn::PTZ_Preset_PUT, 	VideoIn::PTZ_Preset_POST,	VideoIn::PTZ_Preset_DELETE},
	{"/ISAPI/VideoIn/PTZ/Move",				NULL,							VideoIn::PTZ_Move_PUT,		NULL,						VideoIn::PTZ_Move_DELETE},
	{"/ISAPI/VideoIn/PTZ/MoveEx",			NULL,							VideoIn::PTZ_Move_PUT,		NULL,						VideoIn::PTZ_Move_DELETE},
	{"/ISAPI/VideoIn/PTZ/ClickZoomProxy",	VideoIn::PTZ_ClickZoomProxy_GET,VideoIn::PTZ_ClickZoomProxy_PUT,NULL,NULL},
	{"/ISAPI/VideoIn/PTZ/ClickZoom",		NULL,							VideoIn::PTZ_ClickZoom_PUT,		NULL,NULL},

	// Security
	{"/ISAPI/Security/Capabilities",Security::Capabilities_GET,	NULL,NULL,NULL},
	{"/ISAPI/Security/Users",		Security::Users_GET,		NULL,NULL,NULL},
	{"/ISAPI/Security/User", 		Security::User_GET,			Security::User_PUT,	Security::User_POST, Security::User_DELETE},
	{"/ISAPI/Security/UserCheck", 	NULL,						Security::UserCheck_PUT,NULL,NULL},

	// Record
	{"/ISAPI/Record/Capabilities",	Record::Capabilities_GET,	NULL,NULL,NULL},
	{"/ISAPI/Record/Status",		Record::Status_GET,			NULL,NULL,NULL},
	{"/ISAPI/Record/Strategy", 		Record::Strategy_GET,		Record::Strategy_PUT,	NULL,NULL},
	{"/ISAPI/Record/Plan", 			Record::Plan_GET,			Record::Plan_PUT,		NULL,NULL},
	{"/ISAPI/Record/Switch", 		Record::Switch_GET,			Record::Switch_PUT,		NULL,NULL},
	{"/ISAPI/Record/Query",			NULL,						Record::Query_PUT,		NULL,NULL},
	{"/ISAPI/Record/Operate",		NULL,						Record::Operate_PUT,	NULL,Record::Operate_DELETE},

	// Analysis
	{"/ISAPI/Analysis/Capabilities",	Analysis::Capabilities_GET,	NULL,NULL,NULL},
	{"/ISAPI/Analysis/Version",			Analysis::Version_GET,		NULL,NULL,NULL},
	{"/ISAPI/Analysis/Status",			Analysis::Status_GET,		NULL,NULL,NULL},
	{"/ISAPI/Analysis/Switch", 			Analysis::Switch_GET,		Analysis::Switch_PUT,	NULL,NULL},
	{"/ISAPI/Analysis/LPR", 			Analysis::LPR_GET,			Analysis::LPR_PUT,		NULL,NULL},
	{"/ISAPI/Analysis/Dispatch", 		Analysis::Dispatch_GET, 	Analysis::Dispatch_PUT,	NULL,NULL},
	{"/ISAPI/Analysis/Locations", 		Analysis::Locations_GET,	Analysis::Locations_PUT,	NULL,NULL},
	{"/ISAPI/Analysis/Plan", 			Analysis::Plan_GET, 		Analysis::Plan_PUT,		NULL,NULL},
	{"/ISAPI/Analysis/PlanSwitch", 		Analysis::PlanSwitch_GET, 	Analysis::PlanSwitch_PUT,NULL,NULL},
	{"/ISAPI/Analysis/ManualSnap", 		NULL, 						Analysis::ManualSnap_PUT,NULL,NULL},

	// Analysis/Scene
	{"/ISAPI/Analysis/Scene",			Analysis::Scene_GET,			Analysis::Scene_PUT,			NULL,NULL},
	{"/ISAPI/Analysis/Scene/Rule", 		Analysis::Scene_Rule_GET, 		Analysis::Scene_Rule_PUT,		NULL,NULL},

	// Analysis/Evidence
	{"/ISAPI/Analysis/Evidence/Determine",  Analysis::Evidence_Determine_GET, 	Analysis::Evidence_Determine_PUT,	NULL,NULL},
	{"/ISAPI/Analysis/Evidence/Model", 		Analysis::Evidence_Model_GET, 		Analysis::Evidence_Model_PUT,		NULL,NULL},
	{"/ISAPI/Analysis/Evidence/ImgMakeup", 	Analysis::Evidence_ImgMakeup_GET, 	Analysis::Evidence_ImgMakeup_PUT,	NULL,NULL},
	{"/ISAPI/Analysis/Evidence/ImgCompose",	Analysis::Evidence_ImgCompose_GET,	Analysis::Evidence_ImgCompose_PUT,	NULL,NULL},
	{"/ISAPI/Analysis/Evidence/ImgOSD",		Analysis::Evidence_ImgOSD_GET, 		Analysis::Evidence_ImgOSD_PUT,		NULL,NULL},
	{"/ISAPI/Analysis/Evidence/ImgQuality",	Analysis::Evidence_ImgQuality_GET, 	Analysis::Evidence_ImgQuality_PUT,	NULL,NULL},
	{"/ISAPI/Analysis/Evidence/Record",		Analysis::Evidence_Record_GET, 		Analysis::Evidence_Record_PUT,		NULL,NULL},
	{"/ISAPI/Analysis/Evidence/Code", 		Analysis::Evidence_Code_GET, 		Analysis::Evidence_Code_PUT,		NULL,NULL},
	{"/ISAPI/Analysis/Evidence/FileNaming",	Analysis::Evidence_FileNaming_GET,	Analysis::Evidence_FileNaming_PUT,	NULL,NULL},

	// Analysis/Results
	{"/ISAPI/Analysis/Results/Query", 	NULL,Analysis::Results_Query_PUT,NULL,NULL},
	{"/ISAPI/Analysis/Results/Operate",	NULL,Analysis::Results_Operate_PUT, NULL,Analysis::Results_Operate_DELETE},

	// Alarm
	{"/ISAPI/Alarm/Strategy",	Alarm::Strategy_GET, Alarm::Strategy_PUT,	NULL,NULL},
	{"/ISAPI/Alarm/Query",		NULL,	Alarm::Query_PUT,				NULL,NULL},
	{"/ISAPI/Alarm/Export",		NULL,	Alarm::Export_PUT,				NULL,NULL},
	{"/ISAPI/Alarm/Operate",	NULL,NULL,NULL,		 Alarm::Operate_DELETE},

	// Log
	{"/ISAPI/Log/Strategy",		Log::Strategy_GET, 	Log::Strategy_PUT,	NULL,NULL},
	{"/ISAPI/Log/Query",		NULL,	Log::Query_PUT,			NULL,NULL},
	{"/ISAPI/Log/Export",		NULL,	Log::Export_PUT,				NULL,NULL},
	{"/ISAPI/Log/Operate",		NULL,NULL,NULL,		Log::Operate_DELETE},

	// Upload
	{"/ISAPI/Upload/Capabilities",Upload::Capabilities_GET, 	NULL,					NULL,NULL},
	{"/ISAPI/Upload/Status",	Upload::Status_GET, 		NULL,					NULL,NULL},
	{"/ISAPI/Upload/Strategy",	Upload::Strategy_GET, 		Upload::Strategy_PUT,	NULL,NULL},
	{"/ISAPI/Upload/FtpAdvance",Upload::FtpAdvance_GET, 	Upload::FtpAdvance_PUT,	NULL,NULL},
	{"/ISAPI/Upload/Switch",	Upload::Switch_GET, 		Upload::Switch_PUT,	    NULL,NULL},
	
	// Notify
	{"/ISAPI/Notify/Capabilities",Notify::Capabilities_GET, 	NULL,					NULL,NULL},
	{"/ISAPI/Notify/Strategy",	Notify::Strategy_GET, 		Notify::Strategy_PUT,	NULL,NULL},
	{"/ISAPI/Notify/Switch",	Notify::Switch_GET, 		Notify::Switch_PUT,	    NULL,NULL},

	{"/ISAPI/END/Here",NULL,NULL,NULL,NULL},
};

ISAPI_NOAuthorization g_ISAPI_NOAUTH_List[] = 
{
	{"/ISAPI/Security/UserCheck",			false,	true,	false,	false},
	{"/ISAPI/System/Maintain/RebootEx",		false,	true,	false,	false},
	{"/ISAPI/System/Maintain/UpgradeEx",	false,	false,	true,	false},
	{"/ISAPI/VideoIn/PTZ/MoveEx",			false,	true,	false,	true },

	{"/ISAPI/END/Here",						true,	true,	true,	true },
};

ISAPI_HANDLE * GetISAPIHandle(const char * uri)
{
	if(uri)
	{
		for(int i = 0; i < sizeof(g_ISAPI_List)/sizeof(ISAPI_HANDLE); i++)
		{
			if(strcmp(g_ISAPI_List[i].uri, uri) == 0)
			{
				return g_ISAPI_List+i;
			}
		}
	}

	return NULL;
}

HANDLE_FUNC GetISAPIHandleFunc(ISAPI_HANDLE * handle, int method)
{
	if(handle == NULL)
	{
		return NULL;
	}

	if(method == ISAPI_GET)
	{
		return handle->get_handler;
	}
	else if(method == ISAPI_PUT)
	{
		return handle->put_handler;
	}
	else if(method == ISAPI_POST)
	{
		return handle->post_handler;
	}
	else if(method == ISAPI_DELETE)
	{
		return handle->delete_handler;
	}
	else
	{
		return NULL;
	}
}

const char * GetISAPIMethod(int method)
{
	if(method == ISAPI_GET)
	{
		return "GET";
	}
	else if(method == ISAPI_PUT)
	{
		return "PUT";
	}
	else if(method == ISAPI_POST)
	{
		return "POST";
	}
	else if(method == ISAPI_DELETE)
	{
		return "DELETE";
	}
	else
	{
		return "UNKNOWN";
	}
}

bool IsNeedCheckAuthorization(const char * uri, int method)
{
	if(uri)
	{
		for(int i = 0; i < sizeof(g_ISAPI_NOAUTH_List)/sizeof(ISAPI_NOAuthorization); i++)
		{
			if(strcmp(g_ISAPI_NOAUTH_List[i].uri, uri) == 0)
			{
				if(method == ISAPI_GET)
				{
					return (!g_ISAPI_NOAUTH_List[i].bGet);
				}
				else if(method == ISAPI_PUT)
				{
					return (!g_ISAPI_NOAUTH_List[i].bPut);
				}
				else if(method == ISAPI_POST)
				{
					return (!g_ISAPI_NOAUTH_List[i].bPost);
				}
				else if(method == ISAPI_DELETE)
				{
					return (!g_ISAPI_NOAUTH_List[i].bDelete);
				}
			}
		}
	}
	return true;
}

bool AuthorizationParse(const char * content, char *name, char *password)
{
	if(content == NULL || name == NULL || password == NULL)
	{
		return false;
	}

	string str = content;
	string::size_type pos = str.find("Basic");
	if(pos == string::npos)
	{
		return false;
	}

	str = str.substr(pos+5);
	
	// 去掉前缀空格
	str.erase(0, str.find_first_not_of(" "));

	// 查找结束空格
	pos = str.find(" ");
	if(pos != string::npos)
	{
		str = str.substr(0, pos);
	}

	// 去除后缀空格、回车换行
	str.erase(str.find_last_not_of(" ") + 1);  
	str.erase(str.find_last_not_of("\r") + 1);  
	str.erase(str.find_last_not_of("\n") + 1);

	if(str.empty())
	{
		return false;
	}

	// BASE64解码
	int iSrcLen = Base64decode_len(str.c_str());
	char * pszSrc = new char[iSrcLen+1];
	if(pszSrc == NULL)
	{
		return false;
	}

	//LOG_DEBUG_FMT("Base64 Befor:%s",str.c_str());

	Base64decode(pszSrc,str.c_str());

	//LOG_DEBUG_FMT("Base64 After:%s",pszSrc);

	string szUserPass = pszSrc;
	delete []pszSrc;
	pszSrc = NULL;

	// 用户名：密码
	pos = szUserPass.find(":");
	if(pos != string::npos)
	{
		string szName = szUserPass.substr(0, pos);
		string szPassword = szUserPass.substr(pos+1);
		strcpy(name, szName.c_str());
		//LOG_DEBUG_FMT("User:%s",name);
		//LOG_DEBUG_FMT("Pass:%s",szPassword.c_str());
		
		// 对密码就行Base64加密
		Base64encode(password, szPassword.c_str(),szPassword.length());
		//LOG_DEBUG_FMT("Pass base64:%s",password);
		return true;
	}
	
	return false;
}
bool QueryGetStringValue(const char* content, const char* key, char * value, int len, JsonElement &jError)
{
	if(content == NULL || key == NULL || value == NULL)
	{
		return false;
	}

	char keyequal[128] = {0};
	sprintf(keyequal, "%s=", key);
	const char *p = strstr(content, keyequal);
	if (p == NULL)
	{
		char szMsg[128] = {0};
		sprintf(szMsg,"lack essential query field [%s]", key);
		WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
		return false;
	}

	p = p+strlen(keyequal);
	int i = 0;
	while (*p != '&' && *p != '\0' && *p != '\r' && *p != '\n')
	{
		value[i] = *p;
		i++;
		if (i >= len)
		{
			char szMsg[512] = {0};
			sprintf(szMsg,"the query field [%s] is too long,[ < %d]", key, len-1);
			WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
			return false;
		}
		p++;
	}
	value[i] = '\0';
	return true;
}

bool QueryGetNumberValue(const char* content,const char* key, int & value, JsonElement &jError)
{
	char szValue[128] = {0};
	bool bOk = QueryGetStringValue(content, key, szValue, 128, jError);
	value = atoi(szValue);
	return bOk;
}

bool QueryGetNumberValueWithBound( const char* content, const char* key, int & value, int nLower, int nUpper, JsonElement &jError )
{
	if(QueryGetNumberValue(content,key,value,jError))
	{
		if(value < nLower || value > nUpper)
		{
			char szMsg[128] = {0};
			sprintf(szMsg,"the query field [%s] type:Number [%d - %d]", key, nLower,nUpper);
			WriteJsonResult(ISAPI_FAILED_PARAMS, szMsg, jError);
			return false;
		}
		return true;
	}
	return false;	
}

int RequestUserRole( const ISAPI_REQ & tReqHead )
{
	int iRole = USER_ROLE_VIEWER;// 默认只是观察者
	char szUserName[32] = {0};
	char szPassword[32] = {0};
	bool bAuthPass = AuthorizationParse(tReqHead.szAuth, szUserName, szPassword);
	if (bAuthPass)
	{
		USER_INFO tUser = {0};
		if(UserManager::GetInstance()->FindUser(szUserName, tUser) == 0)
		{
			return tUser.iRole;
		}
	}
	return iRole;
}

int WriteDbLog(const ISAPI_REQ & tReqHead, int iType, const char *format, ...)
{
	char szUserName[32] = {0};
	char szPassword[32] = {0};
	AuthorizationParse(tReqHead.szAuth, szUserName, szPassword);

	char szLogMsg[1024 * 2] = {0};
	va_list argptr;  
	va_start(argptr, format);
	vsprintf(szLogMsg, format, argptr);
	va_end(argptr);

	return LogManager::GetInstance()->WriteOpLog(iType, szUserName, tReqHead.szRemoteIP, szLogMsg);
}

int WriteAlarm( const char *format, ... )
{
	char szAlarmMsg[1024 * 2] = {0};
	va_list argptr;  
	va_start(argptr, format);
	vsprintf(szAlarmMsg, format, argptr);
	va_end(argptr);

	return AlarmManager::GetInstance()->WriteAlarm(szAlarmMsg);
}

int File2HttpUrl(const char * file_path, char *url, bool bUploaded)
{
	if(file_path == NULL || url == NULL)
	{
		return -1;
	}

	char file_path_2312[512] = {0};
	UTF8_To_GB2312((char *)file_path,strlen(file_path), file_path_2312, sizeof(file_path_2312));
	bool bAccess = (OAL_FileExist(file_path_2312)==0?true:false);

	if(strlen(file_path) > 0)
	{
		if(bUploaded && !bAccess)
		{
			sprintf(url,"%s","images/uploaded.png");
		}
		else if(strstr(file_path,"/mnt/"))
		{
			sprintf(url,"%s",file_path+5);
		}
		else
		{
			sprintf(url,"%s",file_path+1);
		}
	}
	return 0;
}
