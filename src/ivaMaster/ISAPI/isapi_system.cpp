#include "isapi_system.h"
#include "oal_time.h"
#include "mq_analysis.h"
#include "../Profile/AnalyProfile.h"
#include "../Profile/SystemProfile.h"
#include "../Manager/SystemStatusMonitor.h"
#include "../Manager/SystemMaintain.h"
#include "../Manager/NetworkManager.h"
#include "../Manager/DeviceManager.h"
#include "../Manager/SoftAuthManager.h"
#include "../Manager/LogManager.h"
#include "../Manager/TempFileCleanup.h"


namespace ISAPI
{
	namespace System
	{
		void Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jSysCap;
			jSysCap["maxChannel"] = MAX_CHANNEL_NUM;
			jRespRoot["sysCap"] = jSysCap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Status_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			SysStatus tStatus = {0};
			SystemStatusMonitor::GetInstance()->GetStatus(tStatus);

			char szCPUUse[10], szMEMUse[10],szCPUTemp[10],szBoardTemp[10];
			sprintf(szCPUUse, "%.2f", tStatus.dbCpuUsage);
			sprintf(szMEMUse, "%.2f", tStatus.dbMemUsage);
			sprintf(szCPUTemp, "%.0f", tStatus.dbCpuTemp);
			sprintf(szBoardTemp, "%.0f", tStatus.dbBoardTemp);

			JsonElement jSysStatus;
			JsonElement jCpustat;
			jCpustat["usage"] = szCPUUse;
			jCpustat["temperature"] = szCPUTemp;

			JsonElement jMemstat;
			jMemstat["usage"] = szMEMUse;
			jMemstat["temperature"] = szBoardTemp;

			jSysStatus["cpustat"] = jCpustat;
			jSysStatus["memstat"] = jMemstat;
			jSysStatus["uptime"]  = SystemStatusMonitor::GetInstance()->GetUptime();

			jRespRoot["sysStatus"] = jSysStatus;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Deviceinfo_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			DeviceInfoT tDeviceInfo;
			memset(&tDeviceInfo,0,sizeof(DeviceInfoT));
			int iRet = SystemProfile::GetInstance()->GetDeviceInfo(&tDeviceInfo);

			if(iRet == 0)
			{
				JsonElement jDeviceInfo;
				jDeviceInfo["serial"] = SystemMaintain::GetInstance()->GetDeviceSerial();
				jDeviceInfo["devid"]  = tDeviceInfo.szDevID;
				jDeviceInfo["model"]  = Global_DeviceModel();
				jDeviceInfo["hwversion"] = Global_HardVersion();
				jDeviceInfo["swversion"] = Global_SoftVersion();

				jRespRoot["deviceinfo"] = jDeviceInfo;

				WriteJsonResultSuccess(jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}
		
		void Deviceinfo_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jDeviceInfo;
			if(!JsonChildAsObj(jReqRoot, "deviceinfo", jDeviceInfo, jRespRoot))return;

			DeviceInfoT tDeviceInfo;
			memset(&tDeviceInfo, 0, sizeof(DeviceInfoT));
			SystemProfile::GetInstance()->GetDeviceInfo(&tDeviceInfo);

			if(!JsonChildAsBuffer(jDeviceInfo, "devid", tDeviceInfo.szDevID, sizeof(tDeviceInfo.szDevID), jRespRoot))return;

			DeviceInfoT tOldCfg = {{0}};
			SystemProfile::GetInstance()->GetDeviceInfo(&tOldCfg);
			if (memcmp(&tOldCfg, &tDeviceInfo, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetDeviceInfo(&tDeviceInfo);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_SYS, "配置设备编号");

			MQ_Analy_DeviceInfo(&tDeviceInfo);
		}

		void Time_Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jTimeCap;
			jTimeCap["isSupportNtpSyn"] = true;
			jTimeCap["isSupportVideoSyn"] = true;
			jTimeCap["isSupportManualSyn"] = true;
			jTimeCap["isSupportPlatformSyn"] = false;
			jTimeCap["isSupportGpsSyn"] = false;
			jRespRoot["timeCap"] = jTimeCap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Time_Status_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			char szTime[20] = {0};
			TimeFormatString(time(NULL), szTime, sizeof(szTime), eYMDHMS1);

			jRespRoot["localtime"] = szTime;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Time_Status_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			// 2016-12-29 16:49:41
			char szTime[20] = {0};
			if (!JsonChildAsBuffer(jReqRoot, "localtime", szTime, sizeof(szTime), jRespRoot)) return;

			if (String2Time(szTime,eYMDHMS1) < 0)
			{
				WriteJsonResultError(ISAPI_FAILED_PARAMS, jRespRoot);
				return;
			}

			WriteDbLog(tReqHead, OP_LOG_SYS, "设置系统时间（前）");

			int iRet = SystemMaintain::GetInstance()->SetSystemTime(szTime);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteDbLog(tReqHead, OP_LOG_SYS, "设置系统时间（后）");

			WriteJsonResultSuccess(jRespRoot);
		}

		void Time_SynMechanism_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			SynTimeCFG tSynCfg = {0};
			SystemProfile::GetInstance()->GetSynTimeInfo(&tSynCfg);

			JsonElement jTimeSyn;
			jTimeSyn["syntype"] = tSynCfg.iSynType;
			jTimeSyn["syncycle"] = tSynCfg.uInterval;
			jTimeSyn["synchnid"] = tSynCfg.iSynChnID+1;
			jTimeSyn["ntpaddr"] = tSynCfg.szNtpAddr;

			jRespRoot["timeSyn"] = jTimeSyn;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Time_SynMechanism_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			JsonElement jTimeSyn;
			if(!JsonChildAsObj(jReqRoot, "timeSyn", jTimeSyn, jRespRoot))return;

			SynTimeCFG tSynCfg = {0};
			SystemProfile::GetInstance()->GetSynTimeInfo(&tSynCfg);

			if (!JsonChildAsIntWithBound(jTimeSyn, "syntype", tSynCfg.iSynType, SYN_TIME_NONE, SYN_TIME_GPS, jRespRoot)) return;
			if (!JsonChildAsUIntWithBound(jTimeSyn, "syncycle", tSynCfg.uInterval, 0, 1440, jRespRoot)) return;
			if (!JsonChildAsIntWithBound(jTimeSyn, "synchnid", tSynCfg.iSynChnID, 1, MAX_CHANNEL_NUM, jRespRoot) && tSynCfg.iSynType == SYN_TIME_VIDEOIN) return;
			if (!JsonChildAsBuffer(jTimeSyn, "ntpaddr", tSynCfg.szNtpAddr, sizeof(tSynCfg.szNtpAddr), jRespRoot) && tSynCfg.iSynType == SYN_TIME_NTP) return;
			tSynCfg.iSynChnID -= 1;

			SynTimeCFG tOldCfg = {0};
			SystemProfile::GetInstance()->GetSynTimeInfo(&tOldCfg);
			if (memcmp(&tOldCfg, &tSynCfg, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemMaintain::GetInstance()->SetSystemSynTime(&tSynCfg);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_SYS, "配置时间同步方式");
		}

		void Time_CheckNtp_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			char szNtpAddr[128] = {0};
			if (!JsonChildAsBuffer(jReqRoot, "ntpaddr", szNtpAddr, sizeof(szNtpAddr), jRespRoot)) return;

			int iRet = SystemMaintain::GetInstance()->TestNtpServer(szNtpAddr);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
			}
		}

		void GPS_Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jGpsCap;
			jGpsCap["isSupportGPS"] = false;
			jGpsCap["isSupportTiming"] = false;
			jRespRoot["gpsCap"] = jGpsCap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void GPS_Status_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jGpsStatus;
			jGpsStatus["enable"] = false;
			jGpsStatus["locate"] = false;
			jGpsStatus["longitude"] = 0;
			jGpsStatus["latitude"] = 0;
			jGpsStatus["gpstime"] = "0000-00-00 00:00:00";
			jRespRoot["gpsStatus"] = jGpsStatus;

			WriteJsonResultSuccess(jRespRoot);
		}

		void GPS_Status_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			JsonElement jGpsStatus;
			if(!JsonChildAsObj(jReqRoot, "gpsStatus", jGpsStatus, jRespRoot))return;

			bool bEnable = false;
			if (!JsonChildAsBool(jGpsStatus, "enable", bEnable, jRespRoot)) return;

			//int iRet = GpsManager::GetInstance()->Enable(bEnable);
			//if(iRet == 0)
			//{
				WriteJsonResultSuccess(jRespRoot);
			//}
			//else
			//{
			//	WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
			//}
		}

		void Network_Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jNetworkCap;
			jNetworkCap["maxInterfaceNum"] = MAX_INTERFACE_NUM;
			jNetworkCap["isSupportWifi"] = true;
			jNetworkCap["isSupport4G"] = true;
			{
				JsonElement jTelecomArry;
				JsonElement j10086;
				j10086["telid"] = e10086;
				j10086["telname"] = "China Mobile";
				jTelecomArry.append(j10086);

				JsonElement j10010;
				j10010["telid"] = e10010;
				j10010["telname"] = "China Unicom";
				jTelecomArry.append(j10010);

// 				JsonElement j10000;
// 				j10000["telid"] = e10000;
// 				j10000["telname"] = "China Telecom";
// 				jTelecomArry.append(j10000);

				jNetworkCap["support4GTelecom"] = jTelecomArry;
			}
			jRespRoot["networkCap"] = jNetworkCap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Network_Status_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jNetworkStatus;
			JsonElement jInterfaceStatus;
			for (int i = 0; i < MAX_INTERFACE_NUM; i++)
			{
				char szIfName[8] = {0};
				sprintf(szIfName, "eth%d",i);

				JsonElement jIfStatus;
				jIfStatus["ifname"]  = szIfName;
				jIfStatus["connect"] = NetworkManager::GetInstance()->IfNetworkConnect(i);

				jInterfaceStatus.append(jIfStatus);
			}
			jNetworkStatus["ifStatus"] = jInterfaceStatus;

			JsonElement jWifiStatus;
			jWifiStatus["enable"] = false;
			jWifiStatus["connect"] = false;
			jWifiStatus["dbm"] = 0;
			jNetworkStatus["wifiStatus"] = jWifiStatus;
	
			FourGStat t4GStat = {false, false, 0};
			NetworkManager::GetInstance()->PPP_4G_Stat(&t4GStat);
			JsonElement j4GStatus;
			j4GStatus["enable"] = t4GStat.bOpen;
			j4GStatus["connect"] = t4GStat.bOn;
			j4GStatus["dbm"] = t4GStat.iDbm;
			jNetworkStatus["fourGStatus"] = j4GStatus;

			jRespRoot["networkStatus"] = jNetworkStatus;

			WriteJsonResultSuccess(jRespRoot);
		}
		
		void Network_Interfaces_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jIfs;
			for (int i = 0; i < MAX_INTERFACE_NUM; i++)
			{
				IF_NETWORK_INFO tNetworkInfo = {0};
				if (NetworkManager::GetInstance()->GetNetworkInfo(i, &tNetworkInfo) == 0)
				{
					char szIfName[8] = {0};
					sprintf(szIfName, "eth%d",i);

					JsonElement jIfOne;
					jIfOne["ifname"]  = szIfName;
					jIfOne["mac"]  = tNetworkInfo.szMacAddr;

					JsonElement jNetwork;
					jNetwork["address"] = tNetworkInfo.szLocalIP;
					jNetwork["netmask"] = tNetworkInfo.szNetMask;
					jNetwork["gateway"] = tNetworkInfo.szGateway;
					jIfOne["networkinfo"]  = jNetwork;

					jIfs.append(jIfOne);
				}
			}
			jRespRoot["interfaces"] = jIfs;

			WriteJsonResultSuccess(jRespRoot);
		}
		
		void Network_Interfaces_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}
			
			// ifname
			char szIfName[8] = {0};
			bool bFind = QueryGetStringValue(tReqHead.szQuery, "ifname",szIfName,sizeof(szIfName),jRespRoot);
			int i = 0;
			int iRet = 0;
			if (bFind)
			{
				i = -1;
				sscanf(szIfName, "eth%d", &i);

				if (i < 0 || i > MAX_INTERFACE_NUM-1)
				{
					WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
					return;
				}

				JsonElement jNetwork;
				IF_NETWORK_INFO tNetworkInfo = {0};
				if (!JsonChildAsObj(jReqRoot, "networkinfo", jNetwork, jRespRoot)) return;
				if (!JsonChildAsBuffer(jNetwork, "address", tNetworkInfo.szLocalIP, sizeof(tNetworkInfo.szLocalIP), jRespRoot)) return;
				if (!JsonChildAsBuffer(jNetwork, "netmask", tNetworkInfo.szNetMask, sizeof(tNetworkInfo.szNetMask), jRespRoot)) return;
				if (!JsonChildAsBuffer(jNetwork, "gateway", tNetworkInfo.szGateway, sizeof(tNetworkInfo.szGateway), jRespRoot)) return;

				IF_NETWORK_INFO tOldCfg = {0};
				NetworkManager::GetInstance()->GetNetworkInfo(i, &tOldCfg);
				if( strcmp(tOldCfg.szLocalIP, tNetworkInfo.szLocalIP) == 0 ||
					strcmp(tOldCfg.szNetMask, tNetworkInfo.szNetMask) == 0 ||
					strcmp(tOldCfg.szGateway, tNetworkInfo.szGateway) == 0)
				{
					WriteJsonResultSuccess(jRespRoot);
					return;
				}

				iRet = NetworkManager::GetInstance()->SetNetworkInfo(i, &tNetworkInfo);
				if(iRet == 0)
				{
					WriteDbLog(tReqHead, OP_LOG_SYS, "配置%s网络参数",szIfName);
				}
			}
			else
			{
				JsonElement jIfs;
				if (!JsonChildAsArrayWithBound(jReqRoot, "interfaces", jIfs, 1, MAX_INTERFACE_NUM, jRespRoot)) return;

				for (int i = 0; i < jIfs.size(); i++)
				{
					JsonElement jIfOne;
					if (!JsonArrayAsObjAt(jIfs, i, jIfOne, jRespRoot)) return;

					char szIfName[8] = {0};
					if (!JsonChildAsBuffer(jIfOne, "ifname", szIfName, sizeof(szIfName),jRespRoot))return;

					int ethi = -1;
					sscanf(szIfName, "eth%d", &ethi);

					if (ethi < 0 || ethi > MAX_INTERFACE_NUM-1)
					{
						WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
						return;
					}

					JsonElement jNetwork;
					IF_NETWORK_INFO tNetworkInfo = {0};
					if (!JsonChildAsObj(jIfOne, "networkinfo", jNetwork, jRespRoot)) return;
					if (!JsonChildAsBuffer(jNetwork, "address", tNetworkInfo.szLocalIP, sizeof(tNetworkInfo.szLocalIP), jRespRoot)) return;
					if (!JsonChildAsBuffer(jNetwork, "netmask", tNetworkInfo.szNetMask, sizeof(tNetworkInfo.szNetMask), jRespRoot)) return;
					if (!JsonChildAsBuffer(jNetwork, "gateway", tNetworkInfo.szGateway, sizeof(tNetworkInfo.szGateway), jRespRoot)) return;

					IF_NETWORK_INFO tOldCfg = {0};
					NetworkManager::GetInstance()->GetNetworkInfo(i, &tOldCfg);
					if( strcmp(tOldCfg.szLocalIP, tNetworkInfo.szLocalIP) == 0 ||
						strcmp(tOldCfg.szNetMask, tNetworkInfo.szNetMask) == 0 ||
						strcmp(tOldCfg.szGateway, tNetworkInfo.szGateway) == 0)
					{
						continue;
					}

					iRet = NetworkManager::GetInstance()->SetNetworkInfo(ethi,&tNetworkInfo);
					if (iRet != 0)
					{
						break;
					}
					else
					{
						WriteDbLog(tReqHead, OP_LOG_SYS, "配置%s网络参数",szIfName);
					}
				}
			}

			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
			}
		}

		void Network_DNS_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			char szMain[16] = {0};
			char szStandby[16] = {0};
			NetworkManager::GetInstance()->GetDNS(szMain, szStandby);

			JsonElement jDNS;
			jDNS["main"] = szMain;
			jDNS["standby"] = szStandby;
			jRespRoot["dns"] = jDNS;

			WriteJsonResultSuccess(jRespRoot);
		}
		
		void Network_DNS_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			char szMain[16] = {0};
			char szStandby[16] = {0};
			JsonElement jDNS;
			if (!JsonChildAsObj(jReqRoot, "dns", jDNS, jRespRoot)) return;
			if (!JsonChildAsBuffer(jDNS, "main", szMain, sizeof(szMain), jRespRoot)) return;
			if (!JsonChildAsBuffer(jDNS, "standby", szStandby, sizeof(szStandby), jRespRoot)) return;

			char szMainOld[16] = {0};
			char szStandbyOld[16] = {0};
			NetworkManager::GetInstance()->GetDNS(szMainOld, szStandbyOld);
			if (strcmp(szMainOld, szMain)==0 && strcmp(szStandbyOld, szStandby)==0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;
			}

			int iRet = NetworkManager::GetInstance()->SetDNS(szMain, szStandby);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteDbLog(tReqHead, OP_LOG_SYS, "配置DNS");

			WriteJsonResultSuccess(jRespRoot);
		}
		
		void Network_WlanHot_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			WlanHotCFG tWlanHot = {0};
			SystemProfile::GetInstance()->GetWlanHotInfo(&tWlanHot);

			JsonElement jWlanHot;
			jWlanHot["enable"] = tWlanHot.bEnable;
			jWlanHot["ssid"] = tWlanHot.szSSID;
			jWlanHot["password"] = tWlanHot.szPwd;
			jWlanHot["frequency"] = tWlanHot.iFrequency;
			jWlanHot["maxclient"] = tWlanHot.iMaxClient;
			jRespRoot["wlanhot"] = jWlanHot;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Network_WlanHot_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			JsonElement jWlanHot;
			if(!JsonChildAsObj(jReqRoot, "wlanhot", jWlanHot, jRespRoot))return;

			WlanHotCFG tWlanHot = {0};
			if(!JsonChildAsBool(jWlanHot, "enable", tWlanHot.bEnable, jRespRoot))return;
			if(!JsonChildAsBuffer(jWlanHot, "ssid", tWlanHot.szSSID, sizeof(tWlanHot.szSSID), jRespRoot))return;
			if(!JsonChildAsBuffer(jWlanHot, "password", tWlanHot.szPwd, sizeof(tWlanHot.szPwd), jRespRoot))return;
			if(!JsonChildAsIntWithBound(jWlanHot, "frequency", tWlanHot.iFrequency, e2_4GHz, e5GHz, jRespRoot))return;
			if(!JsonChildAsIntWithBound(jWlanHot, "maxclient", tWlanHot.iMaxClient, 1, 8, jRespRoot))return;

			WlanHotCFG tOldCfg = {0};
			SystemProfile::GetInstance()->GetWlanHotInfo(&tOldCfg);
			if (memcmp(&tOldCfg, &tWlanHot, sizeof(tOldCfg))==0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;
			}

			int iRet = SystemProfile::GetInstance()->SetWlanHotInfo(&tWlanHot);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteDbLog(tReqHead, OP_LOG_SYS, "配置WIFI热点");

			WriteJsonResultSuccess(jRespRoot);
		}

		void Network_4G_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			FourGCFG t4GCfg = {0};
			SystemProfile::GetInstance()->Get4GInfo(&t4GCfg);

			JsonElement j4G;
			j4G["enable"] = t4GCfg.bOpen;
			j4G["telid"] = t4GCfg.iTelecomID;
			jRespRoot["fourG"] = j4G;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Network_4G_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			JsonElement j4G;
			if(!JsonChildAsObj(jReqRoot, "fourG", j4G, jRespRoot))return;

			FourGCFG t4GCfg = {0};
			if(!JsonChildAsBool(j4G, "enable", t4GCfg.bOpen, jRespRoot))return;
			if(!JsonChildAsIntWithBound(j4G, "telid", t4GCfg.iTelecomID, e10086, e10000, jRespRoot))return;

			FourGCFG tOldCfg = {0};
			SystemProfile::GetInstance()->Get4GInfo(&tOldCfg);
			if (memcmp(&tOldCfg, &t4GCfg, sizeof(tOldCfg))==0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;
			}

			int iRet = SystemProfile::GetInstance()->Set4GInfo(&t4GCfg);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}

			WriteDbLog(tReqHead, OP_LOG_SYS, "配置4G");

			WriteJsonResultSuccess(jRespRoot);
		}

		void Store_Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jStroeCap;
			jStroeCap["maxDiskNum"] = 1;
			jStroeCap["isSupportSD"] = true;
			jStroeCap["isSupportUSB"] = true;
			jStroeCap["isSupportSATA"] = true;
			jRespRoot["stroeCap"] = jStroeCap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Store_Status_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			u64 uAllTotalSize = 0;
			u64 uAllFreeSize = 0;

			AllDiskShow tDisk;
			memset(&tDisk, 0, sizeof(AllDiskShow));
			DeviceManager::GetInstance()->GetAllDiskShow(&tDisk);

			uAllTotalSize += tDisk.tRom.uTotalSize;
			uAllFreeSize += tDisk.tRom.uFreeSize;
			uAllTotalSize += tDisk.tSD.uTotalSize;
			uAllFreeSize += tDisk.tSD.uFreeSize;
			uAllTotalSize += tDisk.tUSB.uTotalSize;
			uAllFreeSize += tDisk.tUSB.uFreeSize;
			uAllTotalSize += tDisk.tHDD.uTotalSize;
			uAllFreeSize += tDisk.tHDD.uFreeSize;

			int iRate = (uAllTotalSize > 0 ? (int)(100*uAllFreeSize/uAllTotalSize) : 0);
			JsonElement jStoreStatus;
			jStoreStatus["total"] = PrintSize(uAllTotalSize);
			jStoreStatus["freerate"]  = iRate;
			jRespRoot["storeStatus"] = jStoreStatus;
			WriteJsonResultSuccess(jRespRoot);
		}

		void Store_Disks_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			int iQDiskType = -1;
			JsonElement jError;
			bool bQuery = QueryGetNumberValue(tReqHead.szQuery, "disktype", iQDiskType, jError);
			if (bQuery && (iQDiskType < DISK_ROM || iQDiskType > DISK_HDD))
			{
				WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
				return;
			}

			AllDiskShow tDisk;
			memset(&tDisk, 0, sizeof(AllDiskShow));
			DeviceManager::GetInstance()->GetAllDiskShow(&tDisk);

			JsonElement jDiskArray;
			if ((bQuery && iQDiskType == DISK_ROM) || !bQuery)
			{
				JsonElement jDisk;
				jDisk["disktype"] = DISK_ROM;
				jDisk["total"]	 = PrintSize(tDisk.tRom.uTotalSize);
				jDisk["free"]	 = PrintSize(tDisk.tRom.uFreeSize);
				jDisk["format"]  = tDisk.tRom.szFormat;
				jDisk["usage"]	 = tDisk.tRom.iUseType;
				jDisk["stat"]	 = tDisk.tRom.iStat;
				jDiskArray.append(jDisk);
			}

			if ((bQuery && iQDiskType == DISK_SD) || !bQuery)
			{
				JsonElement jDisk;
				jDisk["disktype"]    = DISK_SD;
				jDisk["total"]	 = PrintSize(tDisk.tSD.uTotalSize);
				jDisk["free"]	 = PrintSize(tDisk.tSD.uFreeSize);
				jDisk["format"]  = tDisk.tSD.szFormat;
				jDisk["usage"]	 = tDisk.tSD.iUseType;
				jDisk["stat"]	 = tDisk.tSD.iStat;
				jDiskArray.append(jDisk);
			}

			if ((bQuery && iQDiskType == DISK_USB) || !bQuery)
			{
				JsonElement jDisk;
				jDisk["disktype"]    = DISK_USB;
				jDisk["total"]	 = PrintSize(tDisk.tUSB.uTotalSize);
				jDisk["free"]	 = PrintSize(tDisk.tUSB.uFreeSize);
				jDisk["format"]  = tDisk.tUSB.szFormat;
				jDisk["usage"]	 = tDisk.tUSB.iUseType;
				jDisk["stat"]	 = tDisk.tUSB.iStat;
				jDiskArray.append(jDisk);
			}

			if ((bQuery && iQDiskType == DISK_HDD) || !bQuery)
			{
				JsonElement jDisk;
				jDisk["disktype"]    = DISK_HDD;
				jDisk["total"]	 = PrintSize(tDisk.tHDD.uTotalSize);
				jDisk["free"]	 = PrintSize(tDisk.tHDD.uFreeSize);
				jDisk["format"]  = tDisk.tHDD.szFormat;
				jDisk["usage"]	 = tDisk.tHDD.iUseType;
				jDisk["stat"]	 = tDisk.tHDD.iStat;
				jDiskArray.append(jDisk);
			}
			
			jRespRoot["disks"] = jDiskArray;
			WriteJsonResultSuccess(jRespRoot);
		}

		void Store_Disks_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}
	
			int iQDiskType = -1;
			JsonElement jError;
			bool bQuery = QueryGetNumberValue(tReqHead.szQuery, "disktype", iQDiskType, jError);
			if (bQuery && (iQDiskType < DISK_SD || iQDiskType > DISK_HDD))
			{
				WriteJsonResultError(ISAPI_FAILED_PARAMS,jRespRoot);
				return;
			}

			int iRet = -1;
			if (bQuery)
			{
				int iUseType = DISK_USE_DISABLE;
				int iMaxType = DISK_USE_COPY;
				if(iQDiskType == DISK_HDD)
				{
					iMaxType = DISK_USE_STORE;
				}
				if (!JsonChildAsIntWithBound(jReqRoot, "usage", iUseType, DISK_USE_DISABLE, iMaxType, jRespRoot)) return;
				
				if (iUseType == DeviceManager::GetInstance()->GetUsageType(iQDiskType))
				{
					WriteJsonResultSuccess(jRespRoot);
					return;				
				}

				iRet = DeviceManager::GetInstance()->SetUsageType(iQDiskType, iUseType);
				if (iRet != 0)
				{
					WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
					return;
				}

				WriteDbLog(tReqHead, OP_LOG_SYS, "配置%s使用类型",PrintDiskType(iQDiskType));

				WriteJsonResultSuccess(jRespRoot);
			}
			else
			{
				JsonElement jArray;
				if (!JsonChildAsArray(jReqRoot, "disks", jArray, jRespRoot)) return;
				for(int i = 0; i < (int)jArray.size(); ++i)
				{
					JsonElement jDisk;
					if (!JsonArrayAsObjAt(jArray, i, jDisk, jRespRoot))return;

					if(!JsonChildAsIntWithBound(jDisk, "disktype", iQDiskType, DISK_SD, DISK_HDD, jRespRoot))return;
					int iUseType = DISK_USE_DISABLE;
					int iMaxType = DISK_USE_COPY;
					if(iQDiskType == DISK_HDD)
					{
						iMaxType = DISK_USE_STORE;
					}
					if (!JsonChildAsIntWithBound(jDisk, "usage", iUseType, DISK_USE_DISABLE, iMaxType, jRespRoot)) return;

					if (iUseType == DeviceManager::GetInstance()->GetUsageType(iQDiskType))
					{
						WriteJsonResultSuccess(jRespRoot);
						continue;				
					}

					iRet = DeviceManager::GetInstance()->SetUsageType(iQDiskType, iUseType);
					if (iRet != 0)
					{
						WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
						return;
					}

					WriteDbLog(tReqHead, OP_LOG_SYS, "配置%s使用类型",PrintDiskType(iQDiskType));
				}
			}

			WriteJsonResultSuccess(jRespRoot);
		}

		void Store_FormatDisk_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iQDiskType;
			JsonElement jError;
			if(!QueryGetNumberValueWithBound(tReqHead.szQuery, "disktype",iQDiskType, DISK_SD, DISK_HDD,jRespRoot))return;

			int iRet = DeviceManager::GetInstance()->FormatNtfs(iQDiskType);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_SYS, "格式化%s设备", PrintDiskType(iQDiskType));
		}

		void Store_FullStrategy_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			DiskFullStrategy tStrategy = {0};
			SystemProfile::GetInstance()->GetDiskStategyInfo(&tStrategy);
			JsonElement jStoreStrategy;
			jStoreStrategy["threshold"] = tStrategy.uMinSize;
			jStoreStrategy["strategy"]  = tStrategy.iHow2DO;
			jRespRoot["storeStrategy"]	= jStoreStrategy;
	
			WriteJsonResultSuccess(jRespRoot);
		}

		void Store_FullStrategy_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			DiskFullStrategy tStrategy = {0};
			JsonElement jStoreStrategy;
			if (!JsonChildAsObj(jReqRoot, "storeStrategy", jStoreStrategy, jRespRoot)) return;

			if (!JsonChildAsUInt(jStoreStrategy, "threshold", tStrategy.uMinSize, jRespRoot)) return;
			if (!JsonChildAsIntWithBound(jStoreStrategy, "strategy", tStrategy.iHow2DO, eFull2CyclicCovering, eFull2StopSave,jRespRoot)) return;

			DiskFullStrategy tOldCfg  = {0};
			SystemProfile::GetInstance()->GetDiskStategyInfo(&tOldCfg);
			if (memcmp(&tOldCfg, &tStrategy, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemProfile::GetInstance()->SetDiskStategyInfo(&tStrategy);
			if (iRet != 0)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_SYS, "设置满盘策略");
		}

		void Maintain_Capabilities_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			JsonElement jMaintainCap;
			jMaintainCap["maxRebootPoint"] = MAX_REBOOT_TIME_SIZE;
			jMaintainCap["isSupportUpgrade"]= true;
			jRespRoot["storeStrategy"]	= jMaintainCap;

			WriteJsonResultSuccess(jRespRoot);
		}

		void Maintain_Reboot_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			WriteDbLog(tReqHead, OP_LOG_MTN, "Reboot");

			int iRet = SystemMaintain::GetInstance()->Reboot();
			if(0 != iRet)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			else
			{
				JsonElement jReboot;
				jReboot["relogin"] = true;
				jReboot["waittime"] = REBOOT_TIME_USE;
				jRespRoot["reboot"] = jReboot;
				WriteJsonResultSuccess(jRespRoot);
			}
		}

		void Maintain_TimerReboot_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			TimerRebootCfg tReboot = {0};
			SystemProfile::GetInstance()->GetTimerRebootInfo(&tReboot);

			JsonElement jTimerReboot;
			jTimerReboot["enable"] = tReboot.bEnable;
			
			JsonElement jTimerArray;
			for(int i = 0; i < tReboot.uUseNum; ++i)
			{
				jTimerArray.append(tReboot.uTimes[i]);
			}
			jTimerReboot["timerlist"] = jTimerArray;
			jRespRoot["timerReboot"] = jTimerReboot;

			WriteJsonResultSuccess(jRespRoot);
		}
		
		void Maintain_TimerReboot_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}
			JsonElement jTimerReboot;
			if(!JsonChildAsObj(jReqRoot, "timerReboot", jTimerReboot, jRespRoot))return;

			TimerRebootCfg tAutoReboot = {0};

			if(!JsonChildAsBool(jTimerReboot, "enable", tAutoReboot.bEnable,jRespRoot))return;
			JsonElement jTimerArray;
			if(!JsonChildAsArrayWithBound(jTimerReboot,"timerlist", jTimerArray, 0, MAX_REBOOT_TIME_SIZE, jRespRoot))return;
			tAutoReboot.uUseNum = jTimerArray.size();

			for(int i = 0; i < tAutoReboot.uUseNum; ++i)
			{
				int iTime;
				if(!JsonArrayAsIntAt(jTimerArray, i, iTime, jRespRoot)) return;

				tAutoReboot.uTimes[i] = (u32)iTime;
			}

			TimerRebootCfg tOldCfg = {0};
			SystemProfile::GetInstance()->GetTimerRebootInfo(&tOldCfg);
			if (memcmp(&tOldCfg, &tAutoReboot, sizeof(tOldCfg)) == 0)
			{
				WriteJsonResultSuccess(jRespRoot);
				return;// 未修改
			}

			int iRet = SystemMaintain::GetInstance()->SetTimerReboot2System(&tAutoReboot);
			if(0 != iRet)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			
			WriteJsonResultSuccess(jRespRoot);
			WriteDbLog(tReqHead, OP_LOG_MTN, "配置定时重启");
		}

		void Maintain_FactoryRestore_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) != USER_ROLE_ADMIN)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iRet = SystemMaintain::GetInstance()->FactoryRestore();
			if(0 != iRet)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			
			JsonElement jReboot;
			jReboot["relogin"] = true;
			jReboot["waittime"] = REBOOT_TIME_USE;
			jRespRoot["reboot"] = jReboot;

			WriteJsonResultSuccess(jRespRoot);

			WriteDbLog(tReqHead, OP_LOG_MTN, "恢复出厂设置");
		}

		void Maintain_Profile_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			string filepath = "";
			int iRet = SystemMaintain::GetInstance()->ExportProfiles(filepath);
			if(iRet == 0)
			{
				char szTimeout[24] = {0};
				TimeFormatString(time(NULL)+EXPORT_FILE_TIME_OUT, szTimeout, 24, eYMDHMS1);

				char szFileUrl[256] = {0};
				File2HttpUrl(filepath.c_str(), szFileUrl, false);

				JsonElement jExport;
				jExport["filename"] = szFileUrl;
				jExport["expirytime"] = szTimeout;
				jRespRoot["export"] = jExport;
				WriteJsonResultSuccess(jRespRoot);

				WriteDbLog(tReqHead, OP_LOG_MTN, "导出配置");

				// 添加到临时文件管理队列
				TempFileNode tNode;
				tNode.tCrateTime = time(NULL);
				tNode.iExpiryTime = EXPORT_FILE_TIME_OUT;
				strcpy(tNode.szFilePath, filepath.c_str());
				TempFileCleanUp::GetInstance()->AddNode(tNode);
			}
			else if (iRet == -2)
			{
				WriteJsonResultError(ISAPI_FAILED_NO_DATA,jRespRoot);
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}

			WriteJsonResultSuccess(jRespRoot);
		}

		void Maintain_Profile_POST(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) != USER_ROLE_ADMIN)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			char szTempFile[512] = {0};
			JsonElement jError;
			if (!JsonChildAsBuffer(jReqRoot, "filepath", szTempFile, sizeof(szTempFile) ,jError))
			{
				WriteJsonResultError(ISAPI_FAILED_FILE, jRespRoot);
			}

			int iRet = SystemMaintain::GetInstance()->ImportProfiles(szTempFile);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);

				WriteDbLog(tReqHead, OP_LOG_MTN, "还原配置");
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void Maintain_Upgrade_POST(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 观察员没有权限
			if (RequestUserRole(tReqHead) == USER_ROLE_VIEWER)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			char szTempFile[512] = {0};
			JsonElement jError;
			if (!JsonChildAsBuffer(jReqRoot, "filepath", szTempFile, sizeof(szTempFile) ,jError))
			{
				WriteJsonResultError(ISAPI_FAILED_FILE, jRespRoot);
			}

			int iRet = SystemMaintain::GetInstance()->Upgrade(szTempFile);
			if(iRet == 0)
			{
				WriteJsonResultSuccess(jRespRoot);

				WriteDbLog(tReqHead, OP_LOG_MTN, "版本升级");
			}
			else
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE,jRespRoot);
			}
		}

		void SoftAuth_GET(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			SoftAuth tAuth = {0};
			SoftAuthManager::GetInstance()->GetAuthorization(&tAuth);
			
			char szTimeOut[40] = {0};
			TimeFormatString(tAuth.tStatus.tTimeOut, szTimeOut, sizeof(szTimeOut), eYMD1);
			JsonElement jSoftAuth;
			jSoftAuth["license"]  = tAuth.szLicense;
			jSoftAuth["authtype"] = tAuth.tStatus.eAuthType;
			jSoftAuth["timeout"]  = szTimeOut;
			jRespRoot["softauth"]	= jSoftAuth;

			WriteJsonResultSuccess(jRespRoot);
		}

		void SoftAuth_PUT(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 只有管理员有权限
			if (RequestUserRole(tReqHead) != USER_ROLE_ADMIN)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}
			char szLicense[40] = {0};
			if (!JsonChildAsBuffer(jReqRoot, "license",szLicense, sizeof(szLicense),jRespRoot))return;
			int iRet = SoftAuthManager::GetInstance()->DoAuthorization(szLicense);
			if(1 == iRet)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			else if(2 == iRet)
			{
				WriteJsonResultError(ISAPI_FAILED_EXISTING, jRespRoot);
				return;
			}
			else
			{
				WriteJsonResultSuccess(jRespRoot);
			}
		}

		void SoftAuth_DELETE(const ISAPI_REQ & tReqHead,const JsonElement& jReqRoot, JsonElement& jRespRoot)
		{
			// 只有管理员有权限
			if (RequestUserRole(tReqHead) != USER_ROLE_ADMIN)
			{
				WriteJsonResultError(ISAPI_FAILED_PERMISSION,jRespRoot);
				return;
			}

			int iRet = SoftAuthManager::GetInstance()->DelAuthorization();
			if(0 != iRet)
			{
				WriteJsonResultError(ISAPI_FAILED_OPERATE, jRespRoot);
				return;
			}
			else
			{
				WriteJsonResultSuccess(jRespRoot);
			}
		}
	}
}
