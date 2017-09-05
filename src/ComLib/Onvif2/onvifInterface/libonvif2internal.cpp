#include "libonvif2internal.h"
#include "libonvif2com.h"
#include "wsdd.h"
#include "wsaapi.h"
#include "wsseapi.h"
#include "soapRemoteDiscoveryBindingProxy.h"
#include "soapDeviceBindingProxy.h"
#include "soapMediaBindingProxy.h"
#include "soapPTZBindingProxy.h"
#include "soapImagingBindingProxy.h"
#include "soapwsddProxy.h"

int OnvifInternal::m_iDebugModel  = ONVIF_DEBUG_CLOSE;
int OnvifInternal::m_lConnTimeOut = 2;
int OnvifInternal::m_lSendTimeOut = 2;
int OnvifInternal::m_lRecvTimeOut = 2;

void OnvifInternal::SetDebugModel(int Model/* = ONVIF_DEBUG_CLOSE*/)
{
	m_iDebugModel = Model;
}

void OnvifInternal::SetTimeOut(int SendTimeOut/* = 2*/, int RecvTimeOut/* = 2*/, int ConnTimeOut/* = 2*/)
{
	m_lConnTimeOut = ConnTimeOut;
	m_lSendTimeOut = SendTimeOut;
	m_lRecvTimeOut = RecvTimeOut;
}


void OnvifInternal::PrintD(const char *format, ...)
{
	char buffer[4096] = {0};

	if(m_iDebugModel != ONVIF_DEBUG_CLOSE && format)
	{
		va_list argptr;
		va_start(argptr, format);
		vsprintf(buffer, format, argptr);
		va_end(argptr);

		printf("%s",buffer);
	}
}

int OnvifInternal::ProbeAllDevices(LIST_NVT * Devices)
{
	if(Devices == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	char ProbeUrl[URI_LENGTH] = {0};
	sprintf(ProbeUrl,"soap.udp://239.255.255.250:3702");
	wsddProxy wsdd_proxy(ProbeUrl);
	
	PrintD("ProbeUrl : %s\n", ProbeUrl);  

	struct soap* soap = wsdd_proxy.soap;
	soap->recv_timeout = m_lRecvTimeOut; // 接收回应的超时时间
	soap->header = soap_new_SOAP_ENV__Header(soap);
	if(soap->header == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	soap->header->wsa__MessageID = soap_strdup(soap,soap_wsa_rand_uuid(soap));
	soap->header->wsa__To        = soap_strdup(soap,"urn:schemas-xmlsoap-org:ws:2005:04:discovery");
	soap->header->wsa__Action    = soap_strdup(soap,"http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe");

	struct wsdd__ProbeType *wsdd__Probe  = soap_new_wsdd__ProbeType(soap);
	if(wsdd__Probe == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	wsdd__Probe->Types = soap_strdup(soap,"tdn:NetworkVideoTransmitter");// 只发现局域网中NVT设备
	PrintD("ProbeType : %s\n", wsdd__Probe->Types);  
	wsdd__Probe->Scopes = soap_new_wsdd__ScopesType(soap);
	if(wsdd__Probe->Scopes == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	wsdd__Probe->Scopes->__item  = soap_strdup(soap,"onvif://www.onvif.org");
	wsdd__Probe->Scopes->MatchBy = soap_strdup(soap,"");
	PrintD("ScopesType : %s\n", wsdd__Probe->Scopes->__item);  

	// 发送Probe
	int result = wsdd_proxy.send_Probe(wsdd__Probe); 
	if (result != SOAP_OK)
	{
		PrintD("wsdd_proxy.send_Probe failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	while (result == SOAP_OK)  
	{  
		struct __wsdd__ProbeMatches resp;
		result = wsdd_proxy.recv_ProbeMatches(resp);  
		if (result == SOAP_OK)  
		{  
			// 成功接收某个设备的回应消息
			if(resp.wsdd__ProbeMatches && resp.wsdd__ProbeMatches->ProbeMatch)
			{
				NVTInfo NVT = {0};
				Onvif2_ParseIPFromUrl(resp.wsdd__ProbeMatches->ProbeMatch->XAddrs, NVT.IP);
				PrintD("==IP: %s\n", NVT.IP);  

				for(int i = 0; i < resp.wsdd__ProbeMatches->__sizeProbeMatch; i++)
				{
					MatchInfo tMatchInfo =	{0};
					struct wsdd__ProbeMatchType *ProbeMatch = resp.wsdd__ProbeMatches->ProbeMatch+i;
					strcpy(tMatchInfo.xAddrs, Onvif2_StringSafe(ProbeMatch->XAddrs));
					strcpy(tMatchInfo.Types,  Onvif2_StringSafe(ProbeMatch->Types));
					if(ProbeMatch->Scopes)
					{
						strcpy(tMatchInfo.Item,   Onvif2_StringSafe(ProbeMatch->Scopes->__item));
						strcpy(tMatchInfo.MatchBy, Onvif2_StringSafe(ProbeMatch->Scopes->MatchBy));
					}
					tMatchInfo.MetadataVersion = ProbeMatch->MetadataVersion;
					PrintD("====Match%d xAddress : %s\n",i, tMatchInfo.xAddrs);  
					PrintD("====Match%d Types    : %s\n",i, tMatchInfo.Types);  
					PrintD("====Match%d Item     : %s\n",i, tMatchInfo.Item);  
					//PrintD("====Match%d MatchBy  : %s\n",i, tMatchInfo.MatchBy);  
					//PrintD("====Match%d MetaVer  : %d\n",i, tMatchInfo.MetadataVersion);

					NVT.MatchInfos.push_back(tMatchInfo);
				}
			
				Devices->push_back(NVT);
			}
		}  
		else if (soap->error)  
		{  
			break;  
		}  
	}

	PrintD("Probe end! It has Searched %d devices! \n", Devices->size());  
	PrintD("[Leave %s]\n", __FUNCTION__); 

	return 0;
}

int OnvifInternal::ProbeDevice(const char* IP, NVTInfo* NvtInfo)
{
	if( !Onvif2_IsValidIPV4Addr(IP) || NvtInfo == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	char ProbeUrl[URI_LENGTH] = {0};
	sprintf(ProbeUrl,"soap.udp://%s:3702",IP);
	RemoteDiscoveryBindingProxy discovery_proxy(ProbeUrl);
	PrintD("ProbeUrl : %s\n", ProbeUrl);  

	struct soap* soap = discovery_proxy.soap;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->header = soap_new_SOAP_ENV__Header(soap);
	if(soap->header == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	soap->header->wsa__MessageID = soap_strdup(soap,soap_wsa_rand_uuid(soap));
	soap->header->wsa__To        = soap_strdup(soap,"urn:schemas-xmlsoap-org:ws:2005:04:discovery");
	soap->header->wsa__Action    = soap_strdup(soap,"http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe");

	wsdd__ProbeType tdn__Probe;
	soap_default_wsdd__ProbeType(soap,&tdn__Probe);
	
	tdn__Probe.Scopes = soap_new_wsdd__ScopesType(soap);
	if(tdn__Probe.Scopes == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tdn__Probe.Scopes->__item  = soap_strdup(soap,"");
	//tdn__Probe.Scopes->__item  = soap_strdup(soap,"onvif://www.onvif.org");
	//tdn__Probe.Scopes->MatchBy = soap_strdup(soap,"");

	wsdd__ProbeMatchesType tdn__ProbeResponse;
	soap_default_wsdd__ProbeMatchesType(soap, &tdn__ProbeResponse);

	do
	{
		tdn__Probe.Types = soap_strdup(soap,"");
		int result = discovery_proxy.Probe(tdn__Probe,tdn__ProbeResponse);
		if (result == SOAP_OK)
		{
			PrintD("Types : %s, OK\n", tdn__Probe.Types);  
			break;
		}
		PrintD("discovery_proxy.Probe failed, Types : \"%s\", errcode=%d\n",tdn__Probe.Types,result);
		soap_print_fault(soap, stderr);

		tdn__Probe.Types = soap_strdup(soap,"tdn:NetworkVideoTransmitter");
		result = discovery_proxy.Probe(tdn__Probe,tdn__ProbeResponse);
		if (result == SOAP_OK)
		{
			PrintD("Types : %s, OK\n", tdn__Probe.Types);  
			break;
		}
		PrintD("discovery_proxy.Probe failed, Types : \"%s\", errcode=%d\n",tdn__Probe.Types,result);
		soap_print_fault(soap, stderr);

		tdn__Probe.Types = soap_strdup(soap,"tds:Device");
		result = discovery_proxy.Probe(tdn__Probe,tdn__ProbeResponse);
		if (result == SOAP_OK)
		{
			PrintD("Types : %s, OK\n", tdn__Probe.Types);  
			break;
		}
		PrintD("discovery_proxy.Probe failed, Types : \"%s\", errcode=%d\n",tdn__Probe.Types,result,strerror(soap_errno));
		soap_print_fault(soap, stderr);
		return -1;
	}while(0);

	strcpy(NvtInfo->IP, Onvif2_StringSafe(IP));
	for(int i = 0; i < tdn__ProbeResponse.__sizeProbeMatch; i++)
	{
		MatchInfo tMatchInfo =	{0};
		struct wsdd__ProbeMatchType *ProbeMatch = tdn__ProbeResponse.ProbeMatch+i;
		strcpy(tMatchInfo.xAddrs, Onvif2_StringSafe(ProbeMatch->XAddrs));
		strcpy(tMatchInfo.Types,  Onvif2_StringSafe(ProbeMatch->Types));
		if(ProbeMatch->Scopes)
		{
			strcpy(tMatchInfo.Item,   Onvif2_StringSafe(ProbeMatch->Scopes->__item));
			strcpy(tMatchInfo.MatchBy, Onvif2_StringSafe(ProbeMatch->Scopes->MatchBy));
		}
		tMatchInfo.MetadataVersion = ProbeMatch->MetadataVersion;
		PrintD("====Match%d xAddress : %s\n",i, tMatchInfo.xAddrs);  
		PrintD("====Match%d Types    : %s\n",i, tMatchInfo.Types);  
		//PrintD("====Match%d MatchBy  : %s\n",i, tMatchInfo.MatchBy);  
		//PrintD("====Match%d MetaVer  : %d\n",i, tMatchInfo.MetadataVersion);
		PrintD("====Match%d Item     : %s\n",i, tMatchInfo.Item);  

		NvtInfo->MatchInfos.push_back(tMatchInfo);
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::ProbeDeviceByWsdd(const char* IP, NVTInfo* NvtInfo)
{
	if( !Onvif2_IsValidIPV4Addr(IP) || NvtInfo == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	char ProbeUrl[URI_LENGTH] = {0};
	sprintf(ProbeUrl,"soap.udp://%s:3702",IP);
	wsddProxy wsdd_proxy(ProbeUrl);
	PrintD("ProbeUrl : %s\n", ProbeUrl);  

	struct soap* soap = wsdd_proxy.soap;
	soap->recv_timeout = m_lRecvTimeOut; 
	soap->header = soap_new_SOAP_ENV__Header(soap);
	if(soap->header == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	soap->header->wsa__MessageID = soap_strdup(soap,soap_wsa_rand_uuid(soap));
	soap->header->wsa__To        = soap_strdup(soap,"urn:schemas-xmlsoap-org:ws:2005:04:discovery");
	soap->header->wsa__Action    = soap_strdup(soap,"http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe");

	struct wsdd__ProbeType *wsdd__Probe = soap_new_wsdd__ProbeType(soap);
	if(wsdd__Probe == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	wsdd__Probe->Types  = soap_strdup(soap,"tdn:NetworkVideoTransmitter");
	wsdd__Probe->Scopes = soap_new_wsdd__ScopesType(soap);
	if(wsdd__Probe->Scopes == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	wsdd__Probe->Scopes->__item  = soap_strdup(soap,"onvif://www.onvif.org");
	wsdd__Probe->Scopes->MatchBy = soap_strdup(soap,"");

	// 发送单播
	int result = wsdd_proxy.send_Probe(wsdd__Probe);
	if (result != SOAP_OK)
	{
		PrintD("wsdd_proxy.send_Probe failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}
	
	//接收ProbeMatches,成功返回0,否则-1
	struct __wsdd__ProbeMatches ProbeMatchesResponse;
	result = wsdd_proxy.recv_ProbeMatches(ProbeMatchesResponse);
	if(result != SOAP_OK)       
	{
		PrintD("wsdd_proxy.recv_ProbeMatches failed errcode=%d\n",result);
		return -1;
	}
	//读取服务端回应的Probematch消息
	strcpy(NvtInfo->IP, Onvif2_StringSafe(IP));
	if(ProbeMatchesResponse.wsdd__ProbeMatches)
	{
		for(int i = 0; i < ProbeMatchesResponse.wsdd__ProbeMatches->__sizeProbeMatch; i++)
		{
			MatchInfo tMatchInfo =	{0};
			struct wsdd__ProbeMatchType *ProbeMatch = ProbeMatchesResponse.wsdd__ProbeMatches->ProbeMatch+i;
			strcpy(tMatchInfo.xAddrs, Onvif2_StringSafe(ProbeMatch->XAddrs));
			strcpy(tMatchInfo.Types,  Onvif2_StringSafe(ProbeMatch->Types));
			if(ProbeMatch->Scopes)
			{
				strcpy(tMatchInfo.Item,   Onvif2_StringSafe(ProbeMatch->Scopes->__item));
				strcpy(tMatchInfo.MatchBy, Onvif2_StringSafe(ProbeMatch->Scopes->MatchBy));
			}
			tMatchInfo.MetadataVersion = ProbeMatch->MetadataVersion;
			PrintD("====Match%d xAddress : %s\n",i, tMatchInfo.xAddrs);  
			PrintD("====Match%d Types    : %s\n",i, tMatchInfo.Types);  
			PrintD("====Match%d Item     : %s\n",i, tMatchInfo.Item);  
			PrintD("====Match%d MatchBy  : %s\n",i, tMatchInfo.MatchBy);  
			PrintD("====Match%d MetaVer  : %d\n",i, tMatchInfo.MetadataVersion);

			NvtInfo->MatchInfos.push_back(tMatchInfo);
		}
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 

	return 0;
}

int OnvifInternal::ParseDeviceScopes(const char * StrScopes, OnvifScopes * Scopes)
{
	if(StrScopes == NULL || Scopes == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}

	string strItems = StrScopes;
	string onvif_prefix = "onvif://www.onvif.org/";
	string::size_type pos = strItems.find(onvif_prefix);
	while(pos >= 0 && pos != string::npos)
	{
		strItems = strItems.substr(pos);

		string OneItem = "";
		string::size_type pos2 = strItems.find(" ");
		if(pos2 > pos && pos2 != string::npos)
		{
			OneItem = strItems.substr(0, pos2);
			strItems = strItems.substr(pos2);
		}
		else
		{
			OneItem = strItems.substr(pos);
			strItems = "";
		}

		string szValue = "";
		string szType = "";
		string::size_type pos1;

		szType = "onvif://www.onvif.org/type/";
		pos1 = OneItem.find(szType);
		if(pos1 >= 0 && pos1 != string::npos)
		{
			szValue = OneItem.substr(pos1+szType.length());
			if(strlen(Scopes->type) > 0)
				strcat(Scopes->type,";");
			strcat(Scopes->type,szValue.c_str());
		}

		szType = "onvif://www.onvif.org/Profile/";
		pos1 = OneItem.find(szType);
		if(pos1 >= 0 && pos1 != string::npos)
		{
			szValue = OneItem.substr(pos1+szType.length());
			if(strlen(Scopes->Profile) > 0)
				strcat(Scopes->Profile,";");
			strcat(Scopes->Profile,szValue.c_str());
		}

		szType = "onvif://www.onvif.org/hardware/";
		pos1 = OneItem.find(szType);
		if(pos1 >= 0 && pos1 != string::npos)
		{
			szValue = OneItem.substr(pos1+szType.length());
			if(strlen(Scopes->hardware) > 0)
				strcat(Scopes->hardware,";");
			strcat(Scopes->hardware,szValue.c_str());
		}

		szType = "onvif://www.onvif.org/name/";
		pos1 = OneItem.find(szType);
		if(pos1 >= 0 && pos1 != string::npos)
		{
			szValue = OneItem.substr(pos1+szType.length());
			if(strlen(Scopes->name) > 0)
				strcat(Scopes->name,";");
			strcat(Scopes->name,szValue.c_str());
		}

		szType = "onvif://www.onvif.org/location/";
		pos1 = OneItem.find(szType);
		if(pos1 >= 0 && pos1 != string::npos)
		{
			szValue = OneItem.substr(pos1+szType.length());
			if(strlen(Scopes->location) > 0)
				strcat(Scopes->location,";");
			strcat(Scopes->location,szValue.c_str());
		}

		// 下一条
		pos = strItems.find(onvif_prefix);
	}
	return 0;
}

int OnvifInternal::GetWsdlUrl(const char* XAddrs, const char* UserName,const char* Password,
							  char * WsdlUrl)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || WsdlUrl == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetWsdlUrl * tds__GetWsdlUrl = soap_new__tds__GetWsdlUrl(soap);
	if(tds__GetWsdlUrl == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	_tds__GetWsdlUrlResponse tds__GetWsdlUrlResponse;

	int result = device_proxy.GetWsdlUrl(tds__GetWsdlUrl,tds__GetWsdlUrlResponse);
	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetWsdlUrl failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return false;
	}

	strcpy(WsdlUrl,Onvif2_StringSafe(tds__GetWsdlUrlResponse.WsdlUrl));
	PrintD("WsdlUrl:%s\n",WsdlUrl);
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetCapabilities(const char* XAddrs, const char* UserName,const char* Password,
								   Capabilities * Caps)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || Caps == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetCapabilities *tds__GetCapabilities = soap_new__tds__GetCapabilities(soap);
	if(tds__GetCapabilities == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
#if 0
	tds__GetCapabilities->__sizeCategory = 4;
	tds__GetCapabilities->Category = soap_new_tt__CapabilityCategory(soap,4);
	*(tds__GetCapabilities->Category+0) = tt__CapabilityCategory__Device;
	*(tds__GetCapabilities->Category+1) = tt__CapabilityCategory__Imaging;
	*(tds__GetCapabilities->Category+2) = tt__CapabilityCategory__Media;
	*(tds__GetCapabilities->Category+3) = tt__CapabilityCategory__PTZ;
#else
	tds__GetCapabilities->__sizeCategory = 1;
	tds__GetCapabilities->Category = soap_new_tt__CapabilityCategory(soap);
	*tds__GetCapabilities->Category = tt__CapabilityCategory__All;
#endif

	_tds__GetCapabilitiesResponse tds__GetCapabilitiesResponse;

	int result = device_proxy.GetCapabilities(tds__GetCapabilities,tds__GetCapabilitiesResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetCapabilities failed errcode=%d\n",result);
		soap_print_fault(soap, stderr);
		return -1;
	}

	if (tds__GetCapabilitiesResponse.Capabilities == NULL)
	{
		PrintD("Capabilities is NULL\n");
		return -1;
	}

	// [设备]	
	if (tds__GetCapabilitiesResponse.Capabilities->Device)
	{
		tt__DeviceCapabilities *Device = tds__GetCapabilitiesResponse.Capabilities->Device;

		PrintD("Device XAddr:%s\n",Onvif2_StringSafe(Device->XAddr));
		strcpy(Caps->Dev_XAddr,Onvif2_StringSafe(Device->XAddr));

		tt__SystemCapabilities *System = Device->System;
		if (System)
		{
			Caps->Dev_RemoteDiscovery = System->RemoteDiscovery;
			if (System->__sizeSupportedVersions > 0)
			{
				if (System->SupportedVersions && System->SupportedVersions[0])
				{
					sprintf(Caps->Dev_OnvifVersions,"%d.%02d",System->SupportedVersions[0]->Major,System->SupportedVersions[0]->Minor);
					PrintD("Device OnvifVersions:%s\n",Caps->Dev_OnvifVersions);
				}
			}
		}        
	}

	// [图像]
	if (tds__GetCapabilitiesResponse.Capabilities->Imaging)
	{
		tt__ImagingCapabilities *Imaging = tds__GetCapabilitiesResponse.Capabilities->Imaging;
		PrintD("Imaging XAddr:%s\n",Onvif2_StringSafe(Imaging->XAddr));
		strcpy(Caps->Image_XAddr,Onvif2_StringSafe(Imaging->XAddr));
	}
	
	// [媒体]
	if (tds__GetCapabilitiesResponse.Capabilities->Media)
	{
		tt__MediaCapabilities *Media = tds__GetCapabilitiesResponse.Capabilities->Media;
		PrintD("Media XAddr:%s\n",Onvif2_StringSafe(Media->XAddr));
		strcpy(Caps->Media_XAddr,Onvif2_StringSafe(Media->XAddr));
	}

	// [云台]
	if (tds__GetCapabilitiesResponse.Capabilities->PTZ)
	{
		tt__PTZCapabilities *PTZ = tds__GetCapabilitiesResponse.Capabilities->PTZ;
		PrintD("PTZ XAddr:%s\n",Onvif2_StringSafe(PTZ->XAddr));
		strcpy(Caps->PTZ_XAddr,Onvif2_StringSafe(PTZ->XAddr));
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetHostname(const char* XAddrs, const char* UserName,const char* Password,
							   Hostname * Host)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || Host == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetHostname *tds__GetHostname = soap_new__tds__GetHostname(soap);
	if(tds__GetHostname == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	_tds__GetHostnameResponse tds__GetHostnameResponse;

	int result = device_proxy.GetHostname(tds__GetHostname,tds__GetHostnameResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetHostname failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	if(tds__GetHostnameResponse.HostnameInformation)
	{
		Host->FromDHCP = tds__GetHostnameResponse.HostnameInformation->FromDHCP;
		strcpy(Host->Name,Onvif2_StringSafe(tds__GetHostnameResponse.HostnameInformation->Name));
	}

	PrintD("Hostname:%s\n",Host->Name);

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::SetHostname(const char* XAddrs, const char* UserName,const char* Password,
							   const char * HostName)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || HostName == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__SetHostname *tds__SetHostname = soap_new__tds__SetHostname(soap);
	if(tds__SetHostname == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tds__SetHostname->Name = soap_strdup(soap,HostName);

	_tds__SetHostnameResponse tds__SetHostnameResponse;

	int result = device_proxy.SetHostname(tds__SetHostname,tds__SetHostnameResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.SetHostname failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("SetHostname:%s\n",HostName);

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetDNS(const char* XAddrs, const char* UserName,const char* Password,
						  char * DNS)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || DNS == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetDNS *tds__GetDNS = soap_new__tds__GetDNS(soap);
	if(tds__GetDNS == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	_tds__GetDNSResponse tds__GetDNSResponse;

	int result = device_proxy.GetDNS(tds__GetDNS,tds__GetDNSResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetDNS failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	if (tds__GetDNSResponse.DNSInformation)
	{
		for (int i = 0; i < tds__GetDNSResponse.DNSInformation->__sizeDNSManual; i++)
		{
			if (tds__GetDNSResponse.DNSInformation->DNSManual[i])
			{
				sprintf(DNS, "%s",Onvif2_StringSafe(tds__GetDNSResponse.DNSInformation->DNSManual[i]->IPv4Address));
			}
		}
	}

	PrintD("DNS:%s\n",DNS);

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::SetDNS(const char* XAddrs, const char* UserName,const char* Password,
						  const char * DNS)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || DNS == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__SetDNS *tds__SetDNS = soap_new__tds__SetDNS(soap);
	if(tds__SetDNS == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	tt__IPAddress* DNSManual[1] = {NULL};
	DNSManual[0] = soap_new_tt__IPAddress(soap);
	if(DNSManual[0] == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	DNSManual[0]->IPv4Address = soap_strdup(soap,DNS);

	tds__SetDNS->__sizeDNSManual= 1;
	tds__SetDNS->DNSManual = DNSManual;

	_tds__SetDNSResponse tds__SetDNSResponse;

	int result = device_proxy.SetDNS(tds__SetDNS,tds__SetDNSResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.SetDNS failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("SetDNS:%s\n",DNS);

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetNTP(const char* XAddrs, const char* UserName,const char* Password,
						  char * NTP)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || NTP == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetNTP *tds__GetNTP = soap_new__tds__GetNTP(soap);
	if(tds__GetNTP == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	_tds__GetNTPResponse tds__GetNTPResponse;
	int result = device_proxy.GetNTP(tds__GetNTP,tds__GetNTPResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetNTP failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	if (tds__GetNTPResponse.NTPInformation)
	{
		for (int i = 0; i < tds__GetNTPResponse.NTPInformation->__sizeNTPManual; i++)
		{
			if (tds__GetNTPResponse.NTPInformation->NTPManual[i])
			{
				tt__NetworkHost * NetworkHost = tds__GetNTPResponse.NTPInformation->NTPManual[i];

				if(NetworkHost->Type == tt__NetworkHostType__DNS)
				{
					sprintf(NTP, "%s",Onvif2_StringSafe(NetworkHost->DNSname));
				}
				else if(NetworkHost->Type == tt__NetworkHostType__IPv4)
				{
					sprintf(NTP, "%s",Onvif2_StringSafe(NetworkHost->IPv4Address));
				}
				else if(NetworkHost->Type == tt__NetworkHostType__IPv6)
				{
					sprintf(NTP, "%s",Onvif2_StringSafe(NetworkHost->IPv6Address));
				}
			}
		}
	}

	PrintD("NTP:%s\n",NTP);

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::SetNTP(const char* XAddrs, const char* UserName,const char* Password,
						  const char * NTP)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || NTP == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__SetNTP *tds__SetNTP = soap_new__tds__SetNTP(soap);
	if(tds__SetNTP == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	tt__NetworkHost* NTPManual[1] = {NULL};
	NTPManual[0] = soap_new_tt__NetworkHost(soap);
	if(NTPManual[0] == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	NTPManual[0]->Type = tt__NetworkHostType__IPv4;
	NTPManual[0]->IPv4Address = soap_strdup(soap,NTP);

	tds__SetNTP->__sizeNTPManual = 1;
	tds__SetNTP->NTPManual = NTPManual;

	_tds__SetNTPResponse tds__SetNTPResponse;

	int result = device_proxy.SetNTP(tds__SetNTP,tds__SetNTPResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.SetNTP failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("SetNTP:%s\n",NTP);

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetNetworkInterfaces(const char* XAddrs, const char* UserName,const char* Password,
										NetWorkIF * ifs)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ifs == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetNetworkInterfaces *tds__GetNetworkInterfaces = soap_new__tds__GetNetworkInterfaces(soap);
	if(tds__GetNetworkInterfaces == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	_tds__GetNetworkInterfacesResponse tds__GetNetworkInterfacesResponse;
	int result = device_proxy.GetNetworkInterfaces(tds__GetNetworkInterfaces,tds__GetNetworkInterfacesResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetNetworkInterfaces failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	if( tds__GetNetworkInterfacesResponse.__sizeNetworkInterfaces > 0 &&
		tds__GetNetworkInterfacesResponse.NetworkInterfaces[0])
	{
		tt__NetworkInterface *Interface = tds__GetNetworkInterfacesResponse.NetworkInterfaces[0];

		NetWorkIF * ptIf = ifs;
		ptIf->Enabled = Interface->Enabled;
		strcpy(ptIf->InterfaceToken,Interface->token);
		PrintD("NetworkInterfaces \n");
		PrintD("==Enabled=%d Token=%s\n",Interface->Enabled,ptIf->InterfaceToken);

		if(Interface->Info)
		{
			strcpy(ptIf->Name, Onvif2_StringSafe(Interface->Info->Name));
			strcpy(ptIf->HwAddress, Onvif2_StringSafe(Interface->Info->HwAddress));
			ptIf->MTU = (Interface->Info->MTU ? *(Interface->Info->MTU) : 0 );
			PrintD("==Name      = %s\n",ptIf->Name);
			PrintD("==HwAddress = %s\n",ptIf->HwAddress);
			PrintD("==MTU       = %d\n",ptIf->MTU);
		}

		if(Interface->IPv4)
		{
			ptIf->IPv4Enabled = Interface->IPv4->Enabled;
			PrintD("==IPv4Enable= %d\n",ptIf->IPv4Enabled);
			if(Interface->IPv4->Config)
			{
				int PrefixLength = 0;
				if(Interface->IPv4->Config->__sizeManual > 0 && Interface->IPv4->Config->Manual[0])
				{
					strcpy(ptIf->IPv4Address, Onvif2_StringSafe(Interface->IPv4->Config->Manual[0]->Address));
					PrefixLength = Interface->IPv4->Config->Manual[0]->PrefixLength;
					PrintD("==IPv4Addr  = %d\n",ptIf->IPv4Address);
					PrintD("==PrefixLen = %d\n",PrefixLength);
					Onvif2_IPv4Prefixlen2Mask(PrefixLength, ptIf->IPv4Mask);
					PrintD("==IPv4Mask  = %s\n",ptIf->IPv4Mask);
				}
				else if(Interface->IPv4->Config->DHCP && Interface->IPv4->Config->FromDHCP)
				{
					strcpy(ptIf->IPv4Address, Onvif2_StringSafe(Interface->IPv4->Config->FromDHCP->Address));
					PrefixLength = Interface->IPv4->Config->FromDHCP->PrefixLength;
					PrintD("==IPv4Addr  = %d\n",ptIf->IPv4Address);
					PrintD("==PrefixLen = %d\n",PrefixLength);
					Onvif2_IPv4Prefixlen2Mask(PrefixLength, ptIf->IPv4Mask);
					PrintD("==IPv4Mask  = %s\n",ptIf->IPv4Mask);
				}
			}
		}
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::SetNetworkInterfaces(const char* XAddrs, const char* UserName,const char* Password,
										const NetWorkIF * ifs)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ifs == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	int prefixLength = Onvif2_IPv4Mask2Prefixlen(ifs->IPv4Mask);
	if (prefixLength < 0 && prefixLength > 32)
	{
		return -1;
	}

	bool bTrue = true;
	bool bFalse = false;

	_tds__SetNetworkInterfaces *tds__SetNetworkInterfaces = soap_new__tds__SetNetworkInterfaces(soap);
	if(tds__SetNetworkInterfaces == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	tt__NetworkInterfaceSetConfiguration *NetworkInterface = soap_new_tt__NetworkInterfaceSetConfiguration(soap);
	if(NetworkInterface == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	tt__IPv4NetworkInterfaceSetConfiguration *IPv4 = soap_new_tt__IPv4NetworkInterfaceSetConfiguration(soap);
	if(IPv4 == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	IPv4->DHCP = &bFalse;
	IPv4->Enabled = &bTrue;
	IPv4->__sizeManual = 1;

	tt__PrefixedIPv4Address * Manual[1] = {NULL};
	Manual[0] = soap_new_tt__PrefixedIPv4Address(soap);
	if(Manual[0] == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}	

	tds__SetNetworkInterfaces->InterfaceToken   = soap_strdup(soap,ifs->InterfaceToken);
	tds__SetNetworkInterfaces->NetworkInterface = NetworkInterface;
	{
		NetworkInterface->Enabled = ifs->Enabled ? &bTrue : &bFalse;
		NetworkInterface->MTU = soap_new_int(soap);
		*(NetworkInterface->MTU) = ifs->MTU;
		NetworkInterface->IPv4 = IPv4;
		{
			IPv4->DHCP = &bFalse;
			IPv4->Enabled = ifs->IPv4Enabled ? &bTrue : &bFalse;
			IPv4->__sizeManual = 1;
			IPv4->Manual = Manual;
			{
				Manual[0]->Address = soap_strdup(soap,ifs->IPv4Address);
				Manual[0]->PrefixLength = prefixLength;
			}
		}
	}

	_tds__SetNetworkInterfacesResponse tds__SetNetworkInterfacesResponse;

	int result = device_proxy.SetNetworkInterfaces(tds__SetNetworkInterfaces,tds__SetNetworkInterfacesResponse);
	if (result != SOAP_OK)
	{
		PrintD("device_proxy.SetNetworkInterfaces failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	// 修改IP重启设备
	OnvifInternal::SystemReboot(XAddrs, UserName, Password);
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetNetworkDefaultGateway(const char* XAddrs, const char* UserName,const char* Password,
											char * gateway)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || gateway == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetNetworkDefaultGateway *tds__GetNetworkDefaultGateway = soap_new__tds__GetNetworkDefaultGateway(soap);
	_tds__GetNetworkDefaultGatewayResponse tds__GetNetworkDefaultGatewayResponse;

	int result = device_proxy.GetNetworkDefaultGateway(tds__GetNetworkDefaultGateway,tds__GetNetworkDefaultGatewayResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetNetworkDefaultGateway failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	if (tds__GetNetworkDefaultGatewayResponse.NetworkGateway)
	{
		strcpy(gateway,Onvif2_StringSafe(*(tds__GetNetworkDefaultGatewayResponse.NetworkGateway->IPv4Address)));
	}

	PrintD("Gateway =%s\n",gateway);
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::SetNetworkDefaultGateway(const char* XAddrs, const char* UserName,const char* Password,
											const char * gateway)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || gateway == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__SetNetworkDefaultGateway* tds__SetNetworkDefaultGateway = soap_new__tds__SetNetworkDefaultGateway(soap);
	if(tds__SetNetworkDefaultGateway == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	char *IPv4Address[1] = {NULL};
	IPv4Address[0] = soap_strdup(soap, gateway);

	tds__SetNetworkDefaultGateway->__sizeIPv4Address = 1;
	tds__SetNetworkDefaultGateway->IPv4Address = IPv4Address;

	_tds__SetNetworkDefaultGatewayResponse tds__SetNetworkDefaultGatewayResponse;

	int result = device_proxy.SetNetworkDefaultGateway(tds__SetNetworkDefaultGateway,tds__SetNetworkDefaultGatewayResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.SetNetworkDefaultGateway failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}
	PrintD("SetGateway =%s\n",gateway);

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetNetworkProtocols(const char* XAddrs, const char* UserName,const char* Password,
									   NetworkProtocols * Protocols)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || Protocols == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetNetworkProtocols *tds__GetNetworkProtocols = soap_new__tds__GetNetworkProtocols(soap);
	_tds__GetNetworkProtocolsResponse tds__GetNetworkProtocolsResponse;

	int result = device_proxy.GetNetworkProtocols(tds__GetNetworkProtocols,tds__GetNetworkProtocolsResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetNetworkProtocols failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	for(int i = 0; i < tds__GetNetworkProtocolsResponse.__sizeNetworkProtocols; i++)
	{
		tt__NetworkProtocol *NetworkProtocols = tds__GetNetworkProtocolsResponse.NetworkProtocols[i];
		if (NetworkProtocols)
		{
			if (NetworkProtocols->Name == tt__NetworkProtocolType__RTSP)
			{
				Protocols->RTSP_Enabled = NetworkProtocols->Enabled;

				if (NetworkProtocols->Port)
				{
					Protocols->RTSP_Port = *(NetworkProtocols->Port);
				}
				PrintD("RTSP_Enabled = %d RTSP_Port = %d\n",Protocols->RTSP_Enabled,Protocols->RTSP_Port);
			}
			else if (NetworkProtocols->Name == tt__NetworkProtocolType__HTTP)
			{
				Protocols->HTTP_Enabled = NetworkProtocols->Enabled;

				if (NetworkProtocols->Port)
				{
					Protocols->HTTP_Port = *(NetworkProtocols->Port);
				}
				PrintD("HTTP_Enabled = %d HTTP_Port = %d\n",Protocols->HTTP_Enabled,Protocols->HTTP_Port);
			}
			else if (NetworkProtocols->Name == tt__NetworkProtocolType__HTTPS)
			{
				Protocols->HTTPS_Enabled = NetworkProtocols->Enabled;

				if (NetworkProtocols->Port)
				{
					Protocols->HTTPS_Port = *(NetworkProtocols->Port);
				}
				PrintD("HTTPS_Enabled = %d HTTPS_Port = %d\n",Protocols->HTTPS_Enabled,Protocols->HTTPS_Port);
			}
		}
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetDeviceInformation(const char* XAddrs, const char* UserName,const char* Password,
										DeviceInfomation * DevInfo)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || DevInfo == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetDeviceInformation *tds__GetDeviceInformation = soap_new__tds__GetDeviceInformation(soap);
	_tds__GetDeviceInformationResponse tds__GetDeviceInformationResponse;
	int result = device_proxy.GetDeviceInformation(tds__GetDeviceInformation,tds__GetDeviceInformationResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetDeviceInformation failed errcode=%d\n",result);
		soap_print_fault(soap, stderr);
		if(result == SOAP_FAULT)
		{
			if(soap->fault)
			{
				if(soap->fault->SOAP_ENV__Code)
				{
					if(soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode)
					{
						if(soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Value)
						{
							if(strstr(soap->fault->SOAP_ENV__Code->SOAP_ENV__Subcode->SOAP_ENV__Value,"NotAuthorized"))// 海康
							{
								return -2;
							}
						}
					}
				}
				if(soap->fault->SOAP_ENV__Reason)
				{
					if(soap->fault->SOAP_ENV__Reason->SOAP_ENV__Text)
					{
						if(strstr(soap->fault->SOAP_ENV__Reason->SOAP_ENV__Text,"registry error")|| // 天地伟业
							strstr(soap->fault->SOAP_ENV__Reason->SOAP_ENV__Text,"not authorized"))// 海康
						{
							return -2;
						}
					}
				}
			}
		}
		return -1;
	}

	strcpy(DevInfo->FirmwareVersion, Onvif2_StringSafe(tds__GetDeviceInformationResponse.FirmwareVersion));
	strcpy(DevInfo->HardwareId, Onvif2_StringSafe(tds__GetDeviceInformationResponse.HardwareId));
	strcpy(DevInfo->Manufacturer, Onvif2_StringSafe(tds__GetDeviceInformationResponse.Manufacturer));
	strcpy(DevInfo->Model, Onvif2_StringSafe(tds__GetDeviceInformationResponse.Model));
	strcpy(DevInfo->SerialNumber, Onvif2_StringSafe(tds__GetDeviceInformationResponse.SerialNumber));
	PrintD("FirmwareVersion = %s\n",DevInfo->FirmwareVersion);
	PrintD("HardwareId      = %s\n",DevInfo->HardwareId);
	PrintD("Manufacturer    = %s\n",DevInfo->Manufacturer);
	PrintD("Model           = %s\n",DevInfo->Model);
	PrintD("SerialNumber    = %s\n",DevInfo->SerialNumber);

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetSystemDateAndTime(const char* XAddrs, const char* UserName,const char* Password,
										DevDateTime * DevTime)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || DevTime == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetSystemDateAndTime *tds__GetSystemDateAndTime = soap_new__tds__GetSystemDateAndTime(soap);
	if(tds__GetSystemDateAndTime == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	_tds__GetSystemDateAndTimeResponse tds__GetSystemDateAndTimeResponse;

	int result = device_proxy.GetSystemDateAndTime(tds__GetSystemDateAndTime, tds__GetSystemDateAndTimeResponse);
	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetSystemDateAndTime failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	if(tds__GetSystemDateAndTimeResponse.SystemDateAndTime)
	{
		if(tds__GetSystemDateAndTimeResponse.SystemDateAndTime->LocalDateTime)
		{
			tt__DateTime *LocalDateTime = tds__GetSystemDateAndTimeResponse.SystemDateAndTime->LocalDateTime;
			if(LocalDateTime->Date && LocalDateTime->Time)
			{
				DevTime->year = LocalDateTime->Date->Year;
				DevTime->month = LocalDateTime->Date->Month;
				DevTime->day = LocalDateTime->Date->Day;
				DevTime->hour = LocalDateTime->Time->Hour;
				DevTime->minute = LocalDateTime->Time->Minute;
				DevTime->second = LocalDateTime->Time->Second;
				PrintD("DATATIME = %d-%d-%d %d:%d:%d\n",
					DevTime->year,DevTime->month,DevTime->day,
					DevTime->hour,DevTime->minute,DevTime->second);
			}
		}
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::SetSystemDateAndTime(const char* XAddrs, const char* UserName,const char* Password,
										const DevDateTime * DevTime)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || DevTime == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__SetSystemDateAndTime *tds__SetSystemDateAndTime = soap_new__tds__SetSystemDateAndTime(soap);
	if(tds__SetSystemDateAndTime == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	tds__SetSystemDateAndTime->TimeZone = soap_new_tt__TimeZone(soap);
	if(tds__SetSystemDateAndTime->TimeZone == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tds__SetSystemDateAndTime->TimeZone->TZ = soap_strdup(soap,"GMT+00:00");

	tds__SetSystemDateAndTime->UTCDateTime = soap_new_tt__DateTime(soap);
	if(tds__SetSystemDateAndTime->UTCDateTime == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tds__SetSystemDateAndTime->UTCDateTime->Date = soap_new_tt__Date(soap);
	if(tds__SetSystemDateAndTime->UTCDateTime->Date == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tds__SetSystemDateAndTime->UTCDateTime->Date->Year  = DevTime->year;
	tds__SetSystemDateAndTime->UTCDateTime->Date->Month = DevTime->month;
	tds__SetSystemDateAndTime->UTCDateTime->Date->Day   = DevTime->day;

	tds__SetSystemDateAndTime->UTCDateTime->Time = soap_new_tt__Time(soap);
	if(tds__SetSystemDateAndTime->UTCDateTime->Time == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tds__SetSystemDateAndTime->UTCDateTime->Time->Hour   = DevTime->hour;
	tds__SetSystemDateAndTime->UTCDateTime->Time->Minute = DevTime->minute;
	tds__SetSystemDateAndTime->UTCDateTime->Time->Second = DevTime->second;


	_tds__SetSystemDateAndTimeResponse tds__SetSystemDateAndTimeResponse;

	int result = device_proxy.SetSystemDateAndTime(tds__SetSystemDateAndTime, tds__SetSystemDateAndTimeResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.SetSystemDateAndTime failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::SystemReboot(const char* XAddrs, const char* UserName,const char* Password)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__SystemReboot *tds__SystemReboot = soap_new__tds__SystemReboot(soap);
	if(tds__SystemReboot == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	_tds__SystemRebootResponse tds__SystemRebootResponse;
	int result = device_proxy.SystemReboot(tds__SystemReboot,tds__SystemRebootResponse);

	if (result != SOAP_OK)
	{
		PrintD("device_proxy.SystemReboot failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetScopes(const char* XAddrs, const char* UserName,const char* Password)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetScopes  * tds__GetScopes = soap_new__tds__GetScopes(soap);
	if(tds__GetScopes == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	_tds__GetScopesResponse tds__GetScopesResponse;
	int result = device_proxy.GetScopes(tds__GetScopes,tds__GetScopesResponse);
	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetScopes failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	for(int i = 0; i < tds__GetScopesResponse.__sizeScopes; i++)
	{
		//PrintD("Scopes[%d] __item=%s\n",i, Onvif2_StringSafe(tds__GetScopesResponse.Scopes[i]->__item));
		PrintD("Scopes[%d] ScopeItem=%s\n",i, Onvif2_StringSafe(tds__GetScopesResponse.Scopes[i]->ScopeItem));
		//PrintD("Scopes[%d] ScopeDef=%d\n",i, tds__GetScopesResponse.Scopes[i]->ScopeDef);
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetAccessPolicy(const char* XAddrs, const char* UserName,const char* Password)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetAccessPolicy *tds__GetAccessPolicy = soap_new__tds__GetAccessPolicy(soap);
	if(tds__GetAccessPolicy == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	_tds__GetAccessPolicyResponse tds__GetAccessPolicyResponse;
	int result = device_proxy.GetAccessPolicy(tds__GetAccessPolicy,tds__GetAccessPolicyResponse);
	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetAccessPolicy failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	if(tds__GetAccessPolicyResponse.PolicyFile)
	{
		PrintD("xmime__contentType=%s\n",Onvif2_StringSafe(tds__GetAccessPolicyResponse.PolicyFile->xmime__contentType));
		PrintD("Data.id=%s\n",Onvif2_StringSafe(tds__GetAccessPolicyResponse.PolicyFile->Data.id));
		PrintD("Data.type=%s\n",Onvif2_StringSafe(tds__GetAccessPolicyResponse.PolicyFile->Data.type));
		PrintD("Data.options=%s\n",Onvif2_StringSafe(tds__GetAccessPolicyResponse.PolicyFile->Data.options));
		PrintD("Data.__size=%d\n",tds__GetAccessPolicyResponse.PolicyFile->Data.__size);
		PrintD("Data.id=%s\n",Onvif2_StringSafe(tds__GetAccessPolicyResponse.PolicyFile->Data.id));
		PrintD("Data.__ptr=");
		for(int i = 0; i < tds__GetAccessPolicyResponse.PolicyFile->Data.__size; i++)
		{
			PrintD("%X", *(tds__GetAccessPolicyResponse.PolicyFile->Data.__ptr+i));
		}
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetUsers(const char* XAddrs, const char* UserName,const char* Password,
							User * pUsers)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || pUsers == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__GetUsers *tds__GetUsers = soap_new__tds__GetUsers(soap);
	if(tds__GetUsers == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	_tds__GetUsersResponse tds__GetUsersResponse;
	int result = device_proxy.GetUsers(tds__GetUsers,tds__GetUsersResponse);
	if (result != SOAP_OK)
	{
		PrintD("device_proxy.GetUsers failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	pUsers->size = (USER_SIZE < tds__GetUsersResponse.__sizeUser) ? USER_SIZE : tds__GetUsersResponse.__sizeUser;
	for(int i = 0; i < pUsers->size; i++)
	{
		tt__User * pUser = tds__GetUsersResponse.User[i];
		if(pUser)
		{
			strcpy(pUsers->Users[i].UserName,Onvif2_StringSafe(pUser->Username));
			strcpy(pUsers->Users[i].PassWord,Onvif2_StringSafe(pUser->Password));
			pUsers->Users[i].UserLevel = pUser->UserLevel;
			PrintD("User[%d] Username  = %s\n",i, Onvif2_StringSafe(pUser->Username));
			PrintD("User[%d] Password  = %s\n",i, Onvif2_StringSafe(pUser->Password));
			PrintD("User[%d] UserLevel = %d\n",i, pUser->UserLevel);
		}
	}
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::CreateUsers(const char* XAddrs, const char* UserName,const char* Password,
							   const UserInfo * UserInfo)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || UserInfo == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__CreateUsers *tds__CreateUsers = soap_new__tds__CreateUsers(soap);
	if(tds__CreateUsers == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tds__CreateUsers->__sizeUser = 1;
	tt__User* User[1] = {NULL};
	tds__CreateUsers->User = User;

	User[0] = soap_new_tt__User(soap);
	if(User[0] == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	User[0]->Username = soap_strdup(soap, UserInfo->UserName);
	User[0]->Password = soap_strdup(soap, UserInfo->PassWord);
	User[0]->UserLevel = (tt__UserLevel)(UserInfo->UserLevel);

	_tds__CreateUsersResponse tds__CreateUsersResponse;
	int result = device_proxy.CreateUsers(tds__CreateUsers,tds__CreateUsersResponse);
	if (result != SOAP_OK)
	{
		PrintD("device_proxy.CreateUsers failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::DeleteUsers(const char* XAddrs, const char* UserName,const char* Password,
							   const UserInfo * UserInfo)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || UserInfo == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__DeleteUsers *tds__DeleteUsers = soap_new__tds__DeleteUsers(soap);
	if(tds__DeleteUsers == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tds__DeleteUsers->__sizeUsername = 1;
	char* Username[1] = {NULL};
	tds__DeleteUsers->Username = Username;
	Username[0] = soap_strdup(soap, UserInfo->UserName);

	_tds__DeleteUsersResponse tds__DeleteUsersResponse;
	int result = device_proxy.DeleteUsers(tds__DeleteUsers,tds__DeleteUsersResponse);
	if (result != SOAP_OK)
	{
		PrintD("device_proxy.DeleteUsers failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::SetUser(const char* XAddrs, const char* UserName,const char* Password,
						   const UserInfo * UserInfo)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || UserInfo == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	DeviceBindingProxy device_proxy(XAddrs);
	struct soap* soap = device_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tds__SetUser *tds__SetUser = soap_new__tds__SetUser(soap);
	if(tds__SetUser == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tds__SetUser->__sizeUser = 1;
	tt__User* User[1] = {NULL};
	tds__SetUser->User = User;
	User[0]->Username = soap_strdup(soap, UserInfo->UserName);
	User[0]->Password = soap_strdup(soap, UserInfo->PassWord);
	User[0]->UserLevel= (tt__UserLevel)(UserInfo->UserLevel);

	_tds__SetUserResponse tds__SetUserResponse;
	int result = device_proxy.SetUser(tds__SetUser,tds__SetUserResponse);
	if (result != SOAP_OK)
	{
		PrintD("device_proxy.DeleteUsers failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetAllProfiles(const char* MediaURI, const char* UserName,const char* Password,
								  LIST_Profile * ProfileList)
{
	if(MediaURI == NULL || UserName == NULL || Password == NULL || ProfileList == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	MediaBindingProxy media_proxy(MediaURI);
	struct soap* soap = media_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = 10;//m_lRecvTimeOut;
	soap->send_timeout = 10;//m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_trt__GetProfiles *trt__GetProfiles = soap_new__trt__GetProfiles(soap);
	if(trt__GetProfiles == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	_trt__GetProfilesResponse trt__GetProfilesResponse;
	int result = media_proxy.GetProfiles(trt__GetProfiles,trt__GetProfilesResponse);
	if (result != SOAP_OK)
	{
		PrintD("media_proxy.GetProfiles failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	for(int i = 0; i < trt__GetProfilesResponse.__sizeProfiles; i++)
	{
		tt__Profile *pProfiles = trt__GetProfilesResponse.Profiles[i];
		PrintD("===Profile%d==========================\n",i);

		if(pProfiles)
		{
			ProfileInfo Profile = {0};
			strcpy(Profile.token,Onvif2_StringSafe(pProfiles->token));
			strcpy(Profile.Name,Onvif2_StringSafe(pProfiles->Name));
			Profile.fixed = pProfiles->fixed ? (*pProfiles->fixed) : false;

			PrintD("=Name  :%s\n",Profile.Name);
			PrintD("=token :%s\n",Profile.token);
			PrintD("=fixed :%d\n",Profile.fixed);

			if(pProfiles->VideoSourceConfiguration)
			{
				PrintD("=VideoSource\n");
				tt__VideoSourceConfiguration *pVSConfig = pProfiles->VideoSourceConfiguration;

				strcpy(Profile.VideoSourceCfg.Name,Onvif2_StringSafe(pVSConfig->Name));
				strcpy(Profile.VideoSourceCfg.token,Onvif2_StringSafe(pVSConfig->token));
				Profile.VideoSourceCfg.UseCount = pVSConfig->UseCount;
				if(pVSConfig->Bounds)
				{
					Profile.VideoSourceCfg.Bounds.x = pVSConfig->Bounds->x;
					Profile.VideoSourceCfg.Bounds.y = pVSConfig->Bounds->y;
					Profile.VideoSourceCfg.Bounds.width  = pVSConfig->Bounds->width;
					Profile.VideoSourceCfg.Bounds.height = pVSConfig->Bounds->height;
				}

				PrintD(" =Name     :%s\n",Profile.VideoSourceCfg.Name);
				PrintD(" =token    :%s\n",Profile.VideoSourceCfg.token);
				PrintD(" =UseCount :%d\n",Profile.VideoSourceCfg.UseCount);
				PrintD(" =Bounds   :(%d, %d, %d, %d)\n",
					Profile.VideoSourceCfg.Bounds.x,
					Profile.VideoSourceCfg.Bounds.y,
					Profile.VideoSourceCfg.Bounds.width,
					Profile.VideoSourceCfg.Bounds.height);
				strcpy(Profile.VideoSourceCfg.SourceToken,Onvif2_StringSafe(pVSConfig->SourceToken));
				PrintD(" =SourceToken:%s\n",Profile.VideoSourceCfg.SourceToken);
			}

			if(pProfiles->VideoEncoderConfiguration)
			{
				PrintD("=VideoEncoder\n");
				tt__VideoEncoderConfiguration *pVEConfig = pProfiles->VideoEncoderConfiguration;

				strcpy(Profile.VideoEncoderCfg.Name,Onvif2_StringSafe(pVEConfig->Name));
				strcpy(Profile.VideoEncoderCfg.token,Onvif2_StringSafe(pVEConfig->token));
				Profile.VideoEncoderCfg.UseCount = pVEConfig->UseCount;
				PrintD(" =Name     :%s\n",Profile.VideoEncoderCfg.Name);
				PrintD(" =token    :%s\n",Profile.VideoEncoderCfg.token);
				PrintD(" =UseCount :%d\n",Profile.VideoEncoderCfg.UseCount);

				Profile.VideoEncoderCfg.Encoding = pVEConfig->Encoding;
				PrintD(" =Encoding :%d\n",Profile.VideoEncoderCfg.Encoding);
				if(pVEConfig->Resolution)
				{
					Profile.VideoEncoderCfg.Width  = pVEConfig->Resolution->Width;
					Profile.VideoEncoderCfg.Height = pVEConfig->Resolution->Height;
				}
				PrintD(" =Resolution:%d * %d\n",
						Profile.VideoEncoderCfg.Width,
						Profile.VideoEncoderCfg.Height);
				
				Profile.VideoEncoderCfg.SessionTimeOut = (long)(pVEConfig->SessionTimeout);
				PrintD(" =SessionTimeOut:%ld\n",Profile.VideoEncoderCfg.SessionTimeOut);

				Profile.VideoEncoderCfg.Quality = pVEConfig->Quality;
				PrintD(" =Quality:%f\n",Profile.VideoEncoderCfg.Quality);
				if(pVEConfig->RateControl)
				{
					Profile.VideoEncoderCfg.Framerate = pVEConfig->RateControl->FrameRateLimit;
					Profile.VideoEncoderCfg.EncodingInterval = pVEConfig->RateControl->EncodingInterval;
					Profile.VideoEncoderCfg.Bitrate = pVEConfig->RateControl->BitrateLimit;
					PrintD(" =FrameRate :%d\n",Profile.VideoEncoderCfg.Framerate);
					PrintD(" =Bitrate :%d\n",Profile.VideoEncoderCfg.Bitrate);
					PrintD(" =EncodingInterval :%d\n",Profile.VideoEncoderCfg.EncodingInterval);
				}
				
				if(Profile.VideoEncoderCfg.Encoding == tt__VideoEncoding__MPEG4 && pVEConfig->MPEG4)
				{
					Profile.VideoEncoderCfg.GovLength   = pVEConfig->MPEG4->GovLength;
					Profile.VideoEncoderCfg.ProfileType = pVEConfig->MPEG4->Mpeg4Profile;
					PrintD(" =MPEG4.GovLength   :%d\n",Profile.VideoEncoderCfg.GovLength);
					PrintD(" =MPEG4.ProfileType :%d\n",Profile.VideoEncoderCfg.ProfileType);
				}

				if(Profile.VideoEncoderCfg.Encoding == tt__VideoEncoding__H264 && pVEConfig->H264)
				{
					Profile.VideoEncoderCfg.GovLength   = pVEConfig->H264->GovLength;
					Profile.VideoEncoderCfg.ProfileType = pVEConfig->H264->H264Profile;
					PrintD(" =H264.GovLength   :%d\n",Profile.VideoEncoderCfg.GovLength);
					PrintD(" =H264.ProfileType :%d\n",Profile.VideoEncoderCfg.ProfileType);
				}
			}

			if(pProfiles->PTZConfiguration)
			{
				tt__PTZConfiguration *pPTZConfig = pProfiles->PTZConfiguration;

				PrintD("=PTZ\n");
				strcpy(Profile.PTZCfg.Name,Onvif2_StringSafe(pPTZConfig->Name));
				strcpy(Profile.PTZCfg.token,Onvif2_StringSafe(pPTZConfig->token));
				Profile.PTZCfg.UseCount = pPTZConfig->UseCount;
				PrintD(" =Name     :%s\n",Profile.PTZCfg.Name);
				PrintD(" =token    :%s\n",Profile.PTZCfg.token);
				PrintD(" =UseCount :%d\n",Profile.PTZCfg.UseCount);

				if(pPTZConfig->PanTiltLimits)
				{
					if(pPTZConfig->PanTiltLimits->Range)
					{
						if(pPTZConfig->PanTiltLimits->Range->XRange)
						{
							Profile.PTZCfg.PanLimits.Min = pPTZConfig->PanTiltLimits->Range->XRange->Min;
							Profile.PTZCfg.PanLimits.Max = pPTZConfig->PanTiltLimits->Range->XRange->Max;

							PrintD(" =Pan Limits: (%g %g)\n",
								Profile.PTZCfg.PanLimits.Min,
								Profile.PTZCfg.PanLimits.Max);
						}
						if(pPTZConfig->PanTiltLimits->Range->YRange)
						{
							Profile.PTZCfg.TiltLimits.Min = pPTZConfig->PanTiltLimits->Range->YRange->Min;
							Profile.PTZCfg.TiltLimits.Max = pPTZConfig->PanTiltLimits->Range->YRange->Max;

							PrintD(" =Tilt Limits: (%g %g)\n",
								Profile.PTZCfg.TiltLimits.Min,
								Profile.PTZCfg.TiltLimits.Max);
						}
					}
				}
				if(pPTZConfig->ZoomLimits)
				{
					if(pPTZConfig->ZoomLimits->Range)
					{
						if(pPTZConfig->ZoomLimits->Range->XRange)
						{
							Profile.PTZCfg.ZoomLimits.Min = pPTZConfig->ZoomLimits->Range->XRange->Min;
							Profile.PTZCfg.ZoomLimits.Max = pPTZConfig->ZoomLimits->Range->XRange->Max;

							PrintD(" =Zoom Limits: (%g %g)\n",
								Profile.PTZCfg.ZoomLimits.Min,
								Profile.PTZCfg.ZoomLimits.Max);
						}
					}
				}

				if(pPTZConfig->DefaultPTZSpeed)
				{
					if(pPTZConfig->DefaultPTZSpeed->PanTilt)
					{
						Profile.PTZCfg.DefPTZSpeed.P_Speed = pPTZConfig->DefaultPTZSpeed->PanTilt->x;
						Profile.PTZCfg.DefPTZSpeed.T_Speed = pPTZConfig->DefaultPTZSpeed->PanTilt->y;
					}
					if(pPTZConfig->DefaultPTZSpeed->Zoom)
					{
						Profile.PTZCfg.DefPTZSpeed.Z_Speed = pPTZConfig->DefaultPTZSpeed->Zoom->x;
					}
					PrintD(" =DefPTZSpeed : Pan=%f Tilt=%f Zoom=%f\n",
						Profile.PTZCfg.DefPTZSpeed.P_Speed,
						Profile.PTZCfg.DefPTZSpeed.T_Speed,
						Profile.PTZCfg.DefPTZSpeed.Z_Speed);
				}

				if(pPTZConfig->DefaultPTZTimeout)
				{
					Profile.PTZCfg.DefPTZTimeout = (long)(*(pPTZConfig->DefaultPTZTimeout));
					PrintD(" =DefPTZTimeout : %ld\n",Profile.PTZCfg.DefPTZTimeout);
				}

				strcpy(Profile.PTZCfg.DefAbsPTPostionSpace, Onvif2_StringSafe(pPTZConfig->DefaultAbsolutePantTiltPositionSpace));
				strcpy(Profile.PTZCfg.DefAbsZPostionSpace,  Onvif2_StringSafe(pPTZConfig->DefaultAbsoluteZoomPositionSpace));
				strcpy(Profile.PTZCfg.DefRelPTVelocitySpace, Onvif2_StringSafe(pPTZConfig->DefaultRelativePanTiltTranslationSpace));
				strcpy(Profile.PTZCfg.DefRelZVelocitySpace,  Onvif2_StringSafe(pPTZConfig->DefaultRelativeZoomTranslationSpace));
				strcpy(Profile.PTZCfg.DefConPTTranslationSpace, Onvif2_StringSafe(pPTZConfig->DefaultContinuousPanTiltVelocitySpace));
				strcpy(Profile.PTZCfg.DefConZTranslationSpace,  Onvif2_StringSafe(pPTZConfig->DefaultContinuousZoomVelocitySpace));
				PrintD(" =DefaultAbsolutePantTiltPositionSpace   :%s\n",Profile.PTZCfg.DefAbsPTPostionSpace);
				PrintD(" =DefaultAbsoluteZoomPositionSpace       :%s\n",Profile.PTZCfg.DefAbsZPostionSpace);
				PrintD(" =DefaultRelativePanTiltTranslationSpace :%s\n",Profile.PTZCfg.DefRelPTVelocitySpace);
				PrintD(" =DefaultRelativeZoomTranslationSpace    :%s\n",Profile.PTZCfg.DefRelZVelocitySpace);
				PrintD(" =DefaultContinuousPanTiltVelocitySpace  :%s\n",Profile.PTZCfg.DefConPTTranslationSpace);
				PrintD(" =DefaultContinuousZoomVelocitySpace     :%s\n",Profile.PTZCfg.DefConZTranslationSpace);
				
				strcpy(Profile.PTZCfg.NodeToken, Onvif2_StringSafe(pPTZConfig->NodeToken));
				PrintD(" =NodeToken :%s\n",Profile.PTZCfg.NodeToken);
			}

			ProfileList->push_back(Profile);
		}
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetVideoSources(const char* XAddrs, const char* UserName,const char* Password,
								   LIST_VideoSource *VideoSourceList)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || VideoSourceList == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	MediaBindingProxy media_proxy(XAddrs);
	struct soap* soap = media_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_trt__GetVideoSources *trt__GetVideoSources = soap_new__trt__GetVideoSources(soap);
	if(trt__GetVideoSources == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	_trt__GetVideoSourcesResponse trt__GetVideoSourcesResponse;
	int result = media_proxy.GetVideoSources(trt__GetVideoSources,trt__GetVideoSourcesResponse);
	if (result != SOAP_OK)
	{
		PrintD("media_proxy.GetVideoSources failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	for(int i = 0;  i < trt__GetVideoSourcesResponse.__sizeVideoSources; i++)
	{
		tt__VideoSource *pVideoSource = trt__GetVideoSourcesResponse.VideoSources[i];
		if(pVideoSource == NULL)
		{
			continue;
		}
		PrintD("===VideoSource%d==========================\n",i);
		VideoSource tVideoSource;
		strcpy(tVideoSource.token,Onvif2_StringSafe(pVideoSource->token));
		PrintD("=token :%s\n",tVideoSource.token);

		tVideoSource.Framerate = pVideoSource->Framerate;
		PrintD("=Framerate:%f\n",tVideoSource.Framerate);
		if(pVideoSource->Resolution)
		{
			tVideoSource.Width = pVideoSource->Resolution->Width;
			tVideoSource.Height = pVideoSource->Resolution->Height;
			PrintD("=Resolution:(%d*%d)\n",tVideoSource.Width,tVideoSource.Height);
		}

		if(pVideoSource->Imaging)
		{
			PrintD("=Imaging:\n");

			tVideoSource.ImagingCfg.Brightness      =  Onvif2_FloatSafe(pVideoSource->Imaging->Brightness);
			tVideoSource.ImagingCfg.ColorSaturation =  Onvif2_FloatSafe(pVideoSource->Imaging->ColorSaturation);
			tVideoSource.ImagingCfg.Contrast        =  Onvif2_FloatSafe(pVideoSource->Imaging->Contrast);
			tVideoSource.ImagingCfg.Sharpness       =  Onvif2_FloatSafe(pVideoSource->Imaging->Sharpness);
			PrintD("==Brightness : %f\n",tVideoSource.ImagingCfg.Brightness);
			PrintD("==Saturation : %f\n",tVideoSource.ImagingCfg.ColorSaturation);
			PrintD("==Contrast   : %f\n",tVideoSource.ImagingCfg.Contrast);
			PrintD("==Sharpness  : %f\n",tVideoSource.ImagingCfg.Sharpness);
			
			if(pVideoSource->Imaging->Exposure)
			{
				PrintD("==Exposure:\n");
				tVideoSource.ImagingCfg.Exposure.Mode     =  pVideoSource->Imaging->Exposure->Mode;
				tVideoSource.ImagingCfg.Exposure.Priority =  pVideoSource->Imaging->Exposure->Priority;
				PrintD("===Mode : %d\n",tVideoSource.ImagingCfg.Exposure.Mode );
				PrintD("===Priority : %d\n",tVideoSource.ImagingCfg.Exposure.Priority);
				if(pVideoSource->Imaging->Exposure->Window)
				{
					tVideoSource.ImagingCfg.Exposure.Window.left   = Onvif2_FloatSafe(pVideoSource->Imaging->Exposure->Window->left);
					tVideoSource.ImagingCfg.Exposure.Window.right  = Onvif2_FloatSafe(pVideoSource->Imaging->Exposure->Window->right);
					tVideoSource.ImagingCfg.Exposure.Window.top    = Onvif2_FloatSafe(pVideoSource->Imaging->Exposure->Window->top);
					tVideoSource.ImagingCfg.Exposure.Window.bottom = Onvif2_FloatSafe(pVideoSource->Imaging->Exposure->Window->bottom);
					PrintD("===Window  : （%f ,%f, %f, %f）\n",
						tVideoSource.ImagingCfg.Exposure.Window.left,
						tVideoSource.ImagingCfg.Exposure.Window.right,
						tVideoSource.ImagingCfg.Exposure.Window.top,
						tVideoSource.ImagingCfg.Exposure.Window.bottom);
				}

				tVideoSource.ImagingCfg.Exposure.MinExposureTime = pVideoSource->Imaging->Exposure->MinExposureTime;
				tVideoSource.ImagingCfg.Exposure.MaxExposureTime = pVideoSource->Imaging->Exposure->MaxExposureTime;
				tVideoSource.ImagingCfg.Exposure.MinGain = pVideoSource->Imaging->Exposure->MinGain;
				tVideoSource.ImagingCfg.Exposure.MaxGain = pVideoSource->Imaging->Exposure->MaxGain;
				tVideoSource.ImagingCfg.Exposure.MinIris = pVideoSource->Imaging->Exposure->MinIris;
				tVideoSource.ImagingCfg.Exposure.MaxIris = pVideoSource->Imaging->Exposure->MaxIris;

				tVideoSource.ImagingCfg.Exposure.ExposureTime = pVideoSource->Imaging->Exposure->ExposureTime;
				tVideoSource.ImagingCfg.Exposure.Gain = pVideoSource->Imaging->Exposure->Gain;
				tVideoSource.ImagingCfg.Exposure.Iris = pVideoSource->Imaging->Exposure->Iris;

				PrintD("===MinExposureTime : %f\n",tVideoSource.ImagingCfg.Exposure.MinExposureTime);
				PrintD("===MaxExposureTime : %f\n",tVideoSource.ImagingCfg.Exposure.MaxExposureTime);
				PrintD("===MinGain : %f\n",tVideoSource.ImagingCfg.Exposure.MinGain);
				PrintD("===MaxGain : %f\n",tVideoSource.ImagingCfg.Exposure.MaxGain);
				PrintD("===MinIris : %f\n",tVideoSource.ImagingCfg.Exposure.MinIris);
				PrintD("===MaxIris : %f\n",tVideoSource.ImagingCfg.Exposure.MaxIris);

				PrintD("===ExposureTime    : %f\n",tVideoSource.ImagingCfg.Exposure.ExposureTime);
				PrintD("===Gain    : %f\n",tVideoSource.ImagingCfg.Exposure.Gain);
				PrintD("===Iris    : %f\n",tVideoSource.ImagingCfg.Exposure.Iris);
			}

			if(pVideoSource->Imaging->Focus)
			{
				PrintD("==Focus:\n");
				tVideoSource.ImagingCfg.Focus.Mode = pVideoSource->Imaging->Focus->AutoFocusMode;
				tVideoSource.ImagingCfg.Focus.DefaultSpeed   = pVideoSource->Imaging->Focus->DefaultSpeed;
				tVideoSource.ImagingCfg.Focus.FarLimit       = pVideoSource->Imaging->Focus->FarLimit;
				tVideoSource.ImagingCfg.Focus.NearLimit      = pVideoSource->Imaging->Focus->NearLimit;
				PrintD("===Mode     : %d\n",tVideoSource.ImagingCfg.Focus.Mode);
				PrintD("===DefSpeed : %f\n",tVideoSource.ImagingCfg.Focus.DefaultSpeed);
				PrintD("===FarLimit : %f\n",tVideoSource.ImagingCfg.Focus.FarLimit);
				PrintD("===NearLimit: %f\n",tVideoSource.ImagingCfg.Focus.NearLimit);
			}
	
			tVideoSource.ImagingCfg.IrCutFilterMode = pVideoSource->Imaging->IrCutFilter ? (*(pVideoSource->Imaging->IrCutFilter)):0;
			PrintD("==IrCutFilterMode : %d\n",tVideoSource.ImagingCfg.IrCutFilterMode);

			if(pVideoSource->Imaging->BacklightCompensation)
			{
				tVideoSource.ImagingCfg.BackLightMode  =  pVideoSource->Imaging->BacklightCompensation->Mode;
				tVideoSource.ImagingCfg.BackLightLevel =  pVideoSource->Imaging->BacklightCompensation->Level;
				PrintD("==BackLightMode  = %d\n",tVideoSource.ImagingCfg.BackLightMode);
				PrintD("==BackLightLevel = %f\n",tVideoSource.ImagingCfg.BackLightLevel);
			}

			if(pVideoSource->Imaging->WhiteBalance)
			{
				tVideoSource.ImagingCfg.WhiteBalanceMode = pVideoSource->Imaging->WhiteBalance->Mode;
				tVideoSource.ImagingCfg.CrGain = pVideoSource->Imaging->WhiteBalance->CrGain;
				tVideoSource.ImagingCfg.CbGain = pVideoSource->Imaging->WhiteBalance->CbGain;
				PrintD("==WhiteBalanceMode : %d\n",tVideoSource.ImagingCfg.WhiteBalanceMode);
				PrintD("==CbGain : %f\n",tVideoSource.ImagingCfg.CbGain);
				PrintD("==CrGain : %f\n",tVideoSource.ImagingCfg.CrGain);
			}

			
			if(pVideoSource->Imaging->WideDynamicRange)
			{
				tVideoSource.ImagingCfg.WideDynamicMode = pVideoSource->Imaging->WideDynamicRange->Mode;
				tVideoSource.ImagingCfg.WideDynamicLevel = pVideoSource->Imaging->WideDynamicRange->Level;
				PrintD("==WideDynamicMode : %d\n",tVideoSource.ImagingCfg.WideDynamicMode);
				PrintD("==WideDynamicLevel : %f\n",tVideoSource.ImagingCfg.WideDynamicLevel);
			}

		}

		VideoSourceList->push_back(tVideoSource);
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetStreamUri(const char* XAddrs, const char* UserName,const char* Password,
								const char* ProfileToken, StreamURI *StreamUri)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL || StreamUri == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	MediaBindingProxy media_proxy(XAddrs);
	struct soap* soap = media_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_trt__GetStreamUri *trt__GetStreamUri = soap_new__trt__GetStreamUri(soap);
	if(trt__GetStreamUri == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	trt__GetStreamUri->ProfileToken = soap_strdup(soap, ProfileToken);
	trt__GetStreamUri->StreamSetup = soap_new_tt__StreamSetup(soap);// 流类型和传输规范
	if(trt__GetStreamUri->StreamSetup == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	trt__GetStreamUri->StreamSetup->Stream = tt__StreamType__RTP_Unicast;
    trt__GetStreamUri->StreamSetup->Transport = soap_new_tt__Transport(soap);
	if(trt__GetStreamUri->StreamSetup->Transport == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
    trt__GetStreamUri->StreamSetup->Transport->Protocol = tt__TransportProtocol__RTSP;

	_trt__GetStreamUriResponse trt__GetStreamUriResponse;
	int result = media_proxy.GetStreamUri(trt__GetStreamUri,trt__GetStreamUriResponse);
	if (result != SOAP_OK)
	{
		PrintD("media_proxy.GetStreamUri failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	if(trt__GetStreamUriResponse.MediaUri)
	{
		strcpy(StreamUri->ProfileToken,ProfileToken);
		strcpy(StreamUri->RtspUri,Onvif2_StringSafe(trt__GetStreamUriResponse.MediaUri->Uri));
		PrintD("StreamUri=%s\n",StreamUri->RtspUri);
		PrintD("InvalidAfterConnect=%d\n",trt__GetStreamUriResponse.MediaUri->InvalidAfterConnect);
		PrintD("InvalidAfterReboot=%d\n",trt__GetStreamUriResponse.MediaUri->InvalidAfterReboot);
		PrintD("Timeout=%lld\n",trt__GetStreamUriResponse.MediaUri->Timeout);
	}
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetSnapshotUri(const char* XAddrs, const char* UserName,const char* Password,
								  const char* ProfileToken, char *SnapshotUri)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || SnapshotUri == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	MediaBindingProxy media_proxy(XAddrs);
	struct soap* soap = media_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_trt__GetSnapshotUri *trt__GetSnapshotUri = soap_new__trt__GetSnapshotUri(soap);
	if(trt__GetSnapshotUri == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	trt__GetSnapshotUri->ProfileToken = soap_strdup(soap, ProfileToken);

	_trt__GetSnapshotUriResponse trt__GetSnapshotUriResponse;
	int result = media_proxy.GetSnapshotUri(trt__GetSnapshotUri,trt__GetSnapshotUriResponse);
	if (result != SOAP_OK)
	{
		PrintD("media_proxy.GetSnapshotUri failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	if(trt__GetSnapshotUriResponse.MediaUri)
	{
		strcpy(SnapshotUri,Onvif2_StringSafe(trt__GetSnapshotUriResponse.MediaUri->Uri));
		PrintD("SnapshotUri=%s\n",SnapshotUri);
		PrintD("InvalidAfterConnect=%d\n",trt__GetSnapshotUriResponse.MediaUri->InvalidAfterConnect);
		PrintD("InvalidAfterReboot=%d\n",trt__GetSnapshotUriResponse.MediaUri->InvalidAfterReboot);
		PrintD("Timeout=%lld\n",trt__GetSnapshotUriResponse.MediaUri->Timeout);
	}
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::AbsoluteFocusMove(const char* XAddrs, const char* UserName,const char* Password,
									 const char* VideoSourceToken, float Position, float Speed)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || VideoSourceToken == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	ImagingBindingProxy imaging_proxy(XAddrs);
	struct soap* soap = imaging_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_timg__Move *timg__Move = soap_new__timg__Move(soap);
	if(timg__Move == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	timg__Move->VideoSourceToken = soap_strdup(soap, VideoSourceToken);
	timg__Move->Focus = soap_new_tt__FocusMove(soap);
	if(timg__Move->Focus == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	timg__Move->Focus->Absolute = soap_new_tt__AbsoluteFocus(soap);
	if(timg__Move->Focus->Absolute == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	timg__Move->Focus->Absolute->Position = Position;
	timg__Move->Focus->Absolute->Speed = soap_new_float(soap);
	if(timg__Move->Focus->Absolute->Speed == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	*(timg__Move->Focus->Absolute->Speed) = Speed;

	_timg__MoveResponse timg__MoveResponse;
	int result = imaging_proxy.Move(timg__Move,timg__MoveResponse);
	if (result != SOAP_OK)
	{
		PrintD("imaging_proxy.Move failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::RelativeFocusMove(const char* XAddrs, const char* UserName,const char* Password,
									 const char* VideoSourceToken, float Distance, float Speed)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || VideoSourceToken == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	ImagingBindingProxy imaging_proxy(XAddrs);
	struct soap* soap = imaging_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_timg__Move *timg__Move = soap_new__timg__Move(soap);
	if(timg__Move == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	timg__Move->VideoSourceToken = soap_strdup(soap, VideoSourceToken);
	timg__Move->Focus = soap_new_tt__FocusMove(soap);
	if(timg__Move->Focus == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	timg__Move->Focus->Relative = soap_new_tt__RelativeFocus(soap);
	if(timg__Move->Focus->Relative == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	timg__Move->Focus->Relative->Distance = Distance;
	timg__Move->Focus->Relative->Speed = soap_new_float(soap);
	if(timg__Move->Focus->Absolute->Speed == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	*(timg__Move->Focus->Absolute->Speed) = Speed;

	_timg__MoveResponse timg__MoveResponse;
	int result = imaging_proxy.Move(timg__Move,timg__MoveResponse);
	if (result != SOAP_OK)
	{
		PrintD("imaging_proxy.Move failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::ContinuousFocusMove(const char* XAddrs, const char* UserName,const char* Password,
									   const char* VideoSourceToken, float Speed)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || VideoSourceToken == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	ImagingBindingProxy imaging_proxy(XAddrs);
	struct soap* soap = imaging_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_timg__Move *timg__Move = soap_new__timg__Move(soap);
	if(timg__Move == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	timg__Move->VideoSourceToken = soap_strdup(soap, VideoSourceToken);
	timg__Move->Focus = soap_new_tt__FocusMove(soap);
	if(timg__Move->Focus == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	timg__Move->Focus->Continuous = soap_new_tt__ContinuousFocus(soap);
	if(timg__Move->Focus->Continuous == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	timg__Move->Focus->Continuous->Speed = Speed;

	_timg__MoveResponse timg__MoveResponse;
	int result = imaging_proxy.Move(timg__Move,timg__MoveResponse);
	if (result != SOAP_OK)
	{
		PrintD("imaging_proxy.Move failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::FocusStop(const char* XAddrs, const char* UserName,const char* Password,
							 const char* VideoSourceToken)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || VideoSourceToken == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	ImagingBindingProxy imaging_proxy(XAddrs);
	struct soap* soap = imaging_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_timg__Stop *timg__Stop = soap_new__timg__Stop(soap);
	if(timg__Stop == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	timg__Stop->VideoSourceToken = soap_strdup(soap, VideoSourceToken);

	_timg__StopResponse timg__StopResponse;
	int result = imaging_proxy.Stop(timg__Stop,timg__StopResponse);
	if (result != SOAP_OK)
	{
		PrintD("imaging_proxy.Stop failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::GetImagingStatus(const char* XAddrs, const char* UserName,const char* Password,
									const char* VideoSourceToken, ImagingStatus * Status)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || VideoSourceToken == NULL || Status == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	ImagingBindingProxy imaging_proxy(XAddrs);
	struct soap* soap = imaging_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_timg__GetStatus *timg__GetStatus = soap_new__timg__GetStatus(soap);
	if(timg__GetStatus == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	timg__GetStatus->VideoSourceToken = soap_strdup(soap, VideoSourceToken);

	_timg__GetStatusResponse timg__GetStatusResponse;
	int result = imaging_proxy.GetStatus(timg__GetStatus,timg__GetStatusResponse);
	if (result != SOAP_OK)
	{
		PrintD("imaging_proxy.GetMoveOptions failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}
	if( timg__GetStatusResponse.Status)
	{
		tt__ImagingStatus20 *p = timg__GetStatusResponse.Status;
		if(p->FocusStatus20)
		{
			Status->Position = p->FocusStatus20->Position;
			Status->MoveStatus = p->FocusStatus20->MoveStatus;
			strcpy(Status->Error,Onvif2_StringSafe(p->FocusStatus20->Error));
			PrintD("Position   = %f\n",Status->Position);
			PrintD("MoveStatus = %d\n",Status->MoveStatus);
			PrintD("Error      = %s\n",Status->Error);
		}
	}
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::PTZ_GetNodes(const char* XAddrs, const char* UserName,const char* Password,
								LIST_PTZNode * PtzNodeList)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || PtzNodeList == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__GetNodes *tptz__GetNodes = soap_new__tptz__GetNodes(soap);
	if(tptz__GetNodes == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	_tptz__GetNodesResponse tptz__GetNodesResponse;
	int result = ptz_proxy.GetNodes(tptz__GetNodes,tptz__GetNodesResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.GetNodes failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	for(int i = 0;  i < tptz__GetNodesResponse.__sizePTZNode; i++)
	{
		tt__PTZNode *pNode = tptz__GetNodesResponse.PTZNode[i];
		if(pNode == NULL)
		{
			continue;
		}
		PrintD("===PTZNODE%d==========================\n",i);

		PTZNode PtzNode;
		strcpy(PtzNode.Name,Onvif2_StringSafe(pNode->Name));
		strcpy(PtzNode.token,Onvif2_StringSafe(pNode->token));
		PtzNode.MaxPresetsNum = pNode->MaximumNumberOfPresets;
		PtzNode.HomeSupported = pNode->HomeSupported?1:0;
		PtzNode.FixedHomePosition = pNode->FixedHomePosition?(*(pNode->FixedHomePosition)?1:0):0;
		PrintD("=Name  :%s\n",PtzNode.Name);
		PrintD("=token :%s\n",PtzNode.token);
		PrintD("=MaxPresetsNum  :%d\n",PtzNode.MaxPresetsNum);
		PrintD("=HomeSupported  :%d\n",PtzNode.HomeSupported);
		PrintD("=FixedHomePosition  :%d\n",PtzNode.FixedHomePosition);

		if(pNode->SupportedPTZSpaces)
		{
			int j = 0;
			if(pNode->SupportedPTZSpaces->__sizeAbsolutePanTiltPositionSpace > 0)
			//for(j = 0; j < pNode->SupportedPTZSpaces->__sizeAbsolutePanTiltPositionSpace; j++)
			{
				tt__Space2DDescription * pSpace = pNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace[j];
				if(pSpace)
				{
#ifdef USE_PTZ_SPACE
					strcpy(PtzNode.tAbsPositionSpace.PT_URI,Onvif2_StringSafe(pSpace->URI));
					PrintD("=AbsolutePanTiltPositionSpace URI:%s\n",PtzNode.tAbsPositionSpace.PT_URI);
#endif
					if(pSpace->XRange)
					{
						PtzNode.tAbsPositionSpace.P_Range.Min = pSpace->XRange->Min;
						PtzNode.tAbsPositionSpace.P_Range.Max = pSpace->XRange->Max;
						PrintD("=AbsolutePanTiltPositionSpace Pan Range: (%f  %f)\n",
							pSpace->XRange->Min,pSpace->XRange->Max);
					}
					if(pSpace->YRange)
					{
						PtzNode.tAbsPositionSpace.T_Range.Min = pSpace->YRange->Min;
						PtzNode.tAbsPositionSpace.T_Range.Max = pSpace->YRange->Max;
						PrintD("=AbsolutePanTiltPositionSpace Tilt Range: (%f  %f)\n",
							pSpace->YRange->Min,pSpace->YRange->Max);
					}
				}
			}

			if(pNode->SupportedPTZSpaces->__sizeAbsoluteZoomPositionSpace > 0)
			//for(j = 0; j < pNode->SupportedPTZSpaces->__sizeAbsoluteZoomPositionSpace; j++)
			{
				tt__Space1DDescription * pSpace = pNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace[j];
				if(pSpace)
				{
#ifdef USE_PTZ_SPACE
					strcpy(PtzNode.tAbsPositionSpace.Z_URI,Onvif2_StringSafe(pSpace->URI));
					PrintD("=AbsoluteZoomPositionSpace URI:%s\n",PtzNode.tAbsPositionSpace.Z_URI);
#endif
					if(pSpace->XRange)
					{
						PtzNode.tAbsPositionSpace.Z_Range.Min = pSpace->XRange->Min;
						PtzNode.tAbsPositionSpace.Z_Range.Max = pSpace->XRange->Max;
						PrintD("=AbsoluteZoomPositionSpace Zoom Range: (%f  %f)\n",
							pSpace->XRange->Min,pSpace->XRange->Max);
					}
				}
			}

			if(pNode->SupportedPTZSpaces->__sizeRelativePanTiltTranslationSpace > 0)
			//(j = 0; j < pNode->SupportedPTZSpaces->__sizeRelativePanTiltTranslationSpace; j++)
			{
				tt__Space2DDescription * pSpace = pNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace[j];
				if(pSpace)
				{
#ifdef USE_PTZ_SPACE
					strcpy(PtzNode.tRelTranslationSpace.PT_URI,Onvif2_StringSafe(pSpace->URI));
					PrintD("=RelativePanTiltTranslationSpace URI:%s\n",PtzNode.tRelTranslationSpace.PT_URI);
#endif
					if(pSpace->XRange)
					{
						PtzNode.tRelTranslationSpace.P_Range.Min = pSpace->XRange->Min;
						PtzNode.tRelTranslationSpace.P_Range.Max = pSpace->XRange->Max;
						PrintD("=RelativePanTiltTranslationSpace Pan Range: (%f  %f)\n",
							pSpace->XRange->Min,pSpace->XRange->Max);
					}
					if(pSpace->YRange)
					{
						PtzNode.tRelTranslationSpace.T_Range.Min = pSpace->YRange->Min;
						PtzNode.tRelTranslationSpace.T_Range.Max = pSpace->YRange->Max;
						PrintD("=RelativePanTiltTranslationSpace Tilt Range: (%f  %f)\n",
							pSpace->YRange->Min,pSpace->YRange->Max);
					}
				}
			}

			if(pNode->SupportedPTZSpaces->__sizeRelativeZoomTranslationSpace > 0)
			//for(j = 0; j < pNode->SupportedPTZSpaces->__sizeRelativeZoomTranslationSpace; j++)
			{
				tt__Space1DDescription * pSpace = pNode->SupportedPTZSpaces->RelativeZoomTranslationSpace[j];
				if(pSpace)
				{
#ifdef USE_PTZ_SPACE
					strcpy(PtzNode.tRelTranslationSpace.Z_URI,Onvif2_StringSafe(pSpace->URI));
					PrintD("=RelativeZoomTranslationSpace URI:%s\n",PtzNode.tRelTranslationSpace.Z_URI);
#endif
					if(pSpace->XRange)
					{
						PtzNode.tRelTranslationSpace.Z_Range.Min = pSpace->XRange->Min;
						PtzNode.tRelTranslationSpace.Z_Range.Max = pSpace->XRange->Max;
						PrintD("=RelativeZoomTranslationSpace Zoom Range: (%f  %f)\n",
							pSpace->XRange->Min,pSpace->XRange->Max);
					}
				}
			}

			if(pNode->SupportedPTZSpaces->__sizeContinuousPanTiltVelocitySpace > 0)
			//for(j = 0; j < pNode->SupportedPTZSpaces->__sizeContinuousPanTiltVelocitySpace; j++)
			{
				tt__Space2DDescription * pSpace = pNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace[j];
				if(pSpace)
				{
#ifdef USE_PTZ_SPACE
					strcpy(PtzNode.tConVelocitySpace.PT_URI,Onvif2_StringSafe(pSpace->URI));
					PrintD("=ContinuousPanTiltVelocitySpace URI:%s\n",PtzNode.tConVelocitySpace.PT_URI);
#endif
					if(pSpace->XRange)
					{
						PtzNode.tConVelocitySpace.P_Range.Min = pSpace->XRange->Min;
						PtzNode.tConVelocitySpace.P_Range.Max = pSpace->XRange->Max;
						PrintD("=ContinuousPanTiltVelocitySpace Pan Range: (%f  %f)\n",
							pSpace->XRange->Min,pSpace->XRange->Max);
					}
					if(pSpace->YRange)
					{
						PtzNode.tConVelocitySpace.T_Range.Min = pSpace->YRange->Min;
						PtzNode.tConVelocitySpace.T_Range.Max = pSpace->YRange->Max;
						PrintD("=ContinuousPanTiltVelocitySpace Tilt Range: (%f  %f)\n",
							pSpace->YRange->Min,pSpace->YRange->Max);
					}
				}
			}

			if(pNode->SupportedPTZSpaces->__sizeContinuousZoomVelocitySpace > 0)
			//for(j = 0; j < pNode->SupportedPTZSpaces->__sizeContinuousZoomVelocitySpace; j++)
			{
				tt__Space1DDescription * pSpace = pNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace[j];
				if(pSpace)
				{
#ifdef USE_PTZ_SPACE
					strcpy(PtzNode.tConVelocitySpace.Z_URI,Onvif2_StringSafe(pSpace->URI));
					PrintD("=ContinuousZoomVelocitySpace URI:%s\n",PtzNode.tConVelocitySpace.Z_URI);
#endif
					if(pSpace->XRange)
					{
						PtzNode.tConVelocitySpace.Z_Range.Min = pSpace->XRange->Min;
						PtzNode.tConVelocitySpace.Z_Range.Max = pSpace->XRange->Max;
						PrintD("=ContinuousZoomVelocitySpace Zoom Range: (%f  %f)\n",
							pSpace->XRange->Min,pSpace->XRange->Max);
					}
				}
			}

			if(pNode->SupportedPTZSpaces->__sizePanTiltSpeedSpace > 0)
			//for(j = 0; j < pNode->SupportedPTZSpaces->__sizePanTiltSpeedSpace; j++)
			{
				tt__Space1DDescription * pSpeed = pNode->SupportedPTZSpaces->PanTiltSpeedSpace[j];
				if(pSpeed)
				{
#ifdef USE_PTZ_SPACE
					strcpy(PtzNode.tSpeedSapce.PT_URI,Onvif2_StringSafe(pSpeed->URI));
					PrintD("=PanTiltSpeedSpace URI:%s\n",PtzNode.tSpeedSapce.PT_URI);
#endif
					if(pSpeed->XRange)
					{
						PtzNode.tSpeedSapce.PT_Range.Min = pSpeed->XRange->Min;
						PtzNode.tSpeedSapce.PT_Range.Max = pSpeed->XRange->Max;
						PrintD("=PanTiltSpeedSpace Range: (%f  %f)\n",
							pSpeed->XRange->Min,pSpeed->XRange->Max);
					}
				}
			}

			if(pNode->SupportedPTZSpaces->__sizeZoomSpeedSpace > 0)
			//for(int j = 0; j < pNode->SupportedPTZSpaces->__sizeZoomSpeedSpace; j++)
			{
				tt__Space1DDescription * pSpeed = pNode->SupportedPTZSpaces->ZoomSpeedSpace[j];
				if(pSpeed)
				{
#ifdef USE_PTZ_SPACE
					strcpy(PtzNode.tSpeedSapce.Z_URI,Onvif2_StringSafe(pSpeed->URI));
					PrintD("=ZoomSpeedSpace URI:%s\n",PtzNode.tSpeedSapce.Z_URI);
#endif
					if(pSpeed->XRange)
					{
						PtzNode.tSpeedSapce.Z_Range.Min = pSpeed->XRange->Min;
						PtzNode.tSpeedSapce.Z_Range.Max = pSpeed->XRange->Max;
						PrintD("=ZoomSpeedSpace Range: (%f  %f)\n",
							pSpeed->XRange->Min,pSpeed->XRange->Max);
					}
				}
			}
		}


		/*PtzNode.AuxCmdSize = pNode->__sizeAuxiliaryCommands;
		if(PtzNode.AuxCmdSize > 10)
		{
			PtzNode.AuxCmdSize = 10;
		}
		for(int j = 0;  j < PtzNode.AuxCmdSize; j++)
		{
			strcpy(PtzNode.AuxCmds[j],Onvif2_StringSafe(pNode->AuxiliaryCommands[j]));
			PrintD("=AuxCmds[%d]  :%s\n",j,PtzNode.AuxCmds[j]);
		}*/
		PtzNodeList->push_back(PtzNode);
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::PTZ_AbsoluteMove(const char* XAddrs, const char* UserName,const char* Password,
									const char* ProfileToken, PTZ_POSITION * Position, PTZ_SPEED * Speed)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL || Position == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__AbsoluteMove *tptz__AbsoluteMove = soap_new__tptz__AbsoluteMove(soap);
	if(tptz__AbsoluteMove == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__AbsoluteMove->ProfileToken = soap_strdup(soap,ProfileToken);
	tptz__AbsoluteMove->Position = soap_new_tt__PTZVector(soap);
	if(tptz__AbsoluteMove->Position == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__AbsoluteMove->Position->PanTilt = soap_new_tt__Vector2D(soap);
	if(tptz__AbsoluteMove->Position->PanTilt == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__AbsoluteMove->Position->Zoom = soap_new_tt__Vector1D(soap);
	if(tptz__AbsoluteMove->Position->Zoom == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

#ifdef USE_PTZ_SPACE
	tptz__AbsoluteMove->Position->PanTilt->space = soap_strdup(soap,Position->PT_Space);
#endif

	tptz__AbsoluteMove->Position->PanTilt->x = Position->fP;
	tptz__AbsoluteMove->Position->PanTilt->y = Position->fT;
#ifdef USE_PTZ_SPACE
	tptz__AbsoluteMove->Position->Zoom->space = soap_strdup(soap,Position->Z_Space);
#endif
	tptz__AbsoluteMove->Position->Zoom->x     = Position->fZ;

	if(Speed)
	{
		tptz__AbsoluteMove->Speed = soap_new_tt__PTZSpeed(soap);
		if(tptz__AbsoluteMove->Speed == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
		tptz__AbsoluteMove->Speed->PanTilt = soap_new_tt__Vector2D(soap);
		if(tptz__AbsoluteMove->Speed->PanTilt == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
		tptz__AbsoluteMove->Speed->Zoom = soap_new_tt__Vector1D(soap);
		if(tptz__AbsoluteMove->Speed->Zoom == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
#ifdef USE_PTZ_SPACE
		tptz__AbsoluteMove->Speed->PanTilt->space = soap_strdup(soap,Speed->PT_Space);
#endif
		tptz__AbsoluteMove->Speed->PanTilt->x = Speed->P_Speed;
		tptz__AbsoluteMove->Speed->PanTilt->y = Speed->T_Speed;
#ifdef USE_PTZ_SPACE
		tptz__AbsoluteMove->Speed->Zoom->space = soap_strdup(soap,Speed->Z_Space);
#endif
		tptz__AbsoluteMove->Speed->Zoom->x     = Speed->Z_Speed;
	}

	_tptz__AbsoluteMoveResponse tptz__AbsoluteMoveResponse;
	int result = ptz_proxy.AbsoluteMove(tptz__AbsoluteMove,tptz__AbsoluteMoveResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.AbsoluteMove failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::PTZ_RelativeMove(const char* XAddrs, const char* UserName,const char* Password,
									const char* ProfileToken, PTZ_POSITION * Translation, PTZ_SPEED * Speed)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL || Translation == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__RelativeMove *tptz__RelativeMove = soap_new__tptz__RelativeMove(soap);
	if(tptz__RelativeMove == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__RelativeMove->ProfileToken = soap_strdup(soap,ProfileToken);
	tptz__RelativeMove->Translation = soap_new_tt__PTZVector(soap);
	if(tptz__RelativeMove->Translation == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__RelativeMove->Translation->PanTilt = soap_new_tt__Vector2D(soap);
	if(tptz__RelativeMove->Translation->PanTilt == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__RelativeMove->Translation->Zoom = soap_new_tt__Vector1D(soap);
	if(tptz__RelativeMove->Translation->Zoom == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

#ifdef USE_PTZ_SPACE
	tptz__RelativeMove->Translation->PanTilt->space = soap_strdup(soap,Translation->PT_Space);
#endif
	tptz__RelativeMove->Translation->PanTilt->x = Translation->fP;
	tptz__RelativeMove->Translation->PanTilt->y = Translation->fT;
#ifdef USE_PTZ_SPACE
	tptz__RelativeMove->Translation->Zoom->space = soap_strdup(soap,Translation->Z_Space);
#endif
	tptz__RelativeMove->Translation->Zoom->x    = Translation->fZ;

	if(Speed)
	{
		tptz__RelativeMove->Speed = soap_new_tt__PTZSpeed(soap);
		if(tptz__RelativeMove->Speed == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
		tptz__RelativeMove->Speed->PanTilt = soap_new_tt__Vector2D(soap);
		if(tptz__RelativeMove->Speed->PanTilt == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
		tptz__RelativeMove->Speed->Zoom = soap_new_tt__Vector1D(soap);
		if(tptz__RelativeMove->Speed->Zoom == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
#ifdef USE_PTZ_SPACE
		tptz__RelativeMove->Speed->PanTilt->space = soap_strdup(soap,Speed->PT_Space);
#endif
		tptz__RelativeMove->Speed->PanTilt->x = Speed->P_Speed;
		tptz__RelativeMove->Speed->PanTilt->y = Speed->T_Speed;
#ifdef USE_PTZ_SPACE
		tptz__RelativeMove->Speed->Zoom->space = soap_strdup(soap,Speed->Z_Space);
#endif
		tptz__RelativeMove->Speed->Zoom->x     = Speed->Z_Speed;
	}

	_tptz__RelativeMoveResponse tptz__RelativeMoveResponse;
	int result = ptz_proxy.RelativeMove(tptz__RelativeMove,tptz__RelativeMoveResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.RelativeMove failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::PTZ_ContinuousMove(const char* XAddrs, const char* UserName,const char* Password,
									  const char* ProfileToken, PTZ_SPEED * Velocity, long time_out)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL || Velocity == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__ContinuousMove *tptz__ContinuousMove = soap_new__tptz__ContinuousMove(soap);
	if(tptz__ContinuousMove == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__ContinuousMove->ProfileToken = soap_strdup(soap,ProfileToken);
	tptz__ContinuousMove->Velocity = soap_new_tt__PTZSpeed(soap);
	if(tptz__ContinuousMove->Velocity == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}

	if( Velocity->P_Speed < -0.000001f || Velocity->P_Speed > 0.000001f ||
		Velocity->T_Speed < -0.000001f || Velocity->T_Speed > 0.000001f)
	{
		tptz__ContinuousMove->Velocity->PanTilt = soap_new_tt__Vector2D(soap);
		if(tptz__ContinuousMove->Velocity->PanTilt == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
#ifdef USE_PTZ_SPACE
		tptz__ContinuousMove->Velocity->PanTilt->space = soap_strdup(soap,Velocity->PT_Space);
#endif
		tptz__ContinuousMove->Velocity->PanTilt->x = Velocity->P_Speed;
		tptz__ContinuousMove->Velocity->PanTilt->y = Velocity->T_Speed;
	}

	if( Velocity->Z_Speed < -0.000001f || Velocity->Z_Speed > 0.000001f )
	{
		tptz__ContinuousMove->Velocity->Zoom = soap_new_tt__Vector1D(soap);
		if(tptz__ContinuousMove->Velocity->Zoom == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
#ifdef USE_PTZ_SPACE
		tptz__ContinuousMove->Velocity->Zoom->space = soap_strdup(soap,Velocity->Z_Space);
#endif
		tptz__ContinuousMove->Velocity->Zoom->x     = Velocity->Z_Speed;
	}

	if(time_out > 0)
	{
		tptz__ContinuousMove->Timeout = (LONG64 *)soap_malloc(soap, sizeof(LONG64));
		if(tptz__ContinuousMove->Timeout == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
		*(tptz__ContinuousMove->Timeout) = time_out;
	}

	_tptz__ContinuousMoveResponse tptz__ContinuousMoveResponse;

	int result = ptz_proxy.ContinuousMove(tptz__ContinuousMove,tptz__ContinuousMoveResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.ContinuousMove failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}
int OnvifInternal::PTZ_Stop(const char* XAddrs, const char* UserName,const char* Password, 
							const char* ProfileToken, bool bStopPanTilt, bool bStopZoom)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__Stop *tptz__Stop = soap_new__tptz__Stop(soap);
	if(tptz__Stop == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__Stop->ProfileToken = soap_strdup(soap,ProfileToken);
	tptz__Stop->PanTilt = soap_new_bool(soap);
	if(tptz__Stop->PanTilt == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	*(tptz__Stop->PanTilt) = bStopPanTilt;

	tptz__Stop->Zoom = soap_new_bool(soap);
	if(tptz__Stop->Zoom == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	*(tptz__Stop->Zoom) = bStopZoom;

	_tptz__StopResponse tptz__StopResponse;
	int result = ptz_proxy.Stop(tptz__Stop,tptz__StopResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.Stop failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::PTZ_GetStatus(const char* XAddrs, const char* UserName,const char* Password,
								 const char* ProfileToken, PTZStatus * status)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL || status == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__GetStatus *tptz__GetStatus = soap_new__tptz__GetStatus(soap);
	if(tptz__GetStatus == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__GetStatus->ProfileToken = soap_strdup(soap,ProfileToken);

	_tptz__GetStatusResponse tptz__GetStatusResponse;
	int result = ptz_proxy.GetStatus(tptz__GetStatus,tptz__GetStatusResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.GetStatus failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	if(tptz__GetStatusResponse.PTZStatus)
	{
		if(tptz__GetStatusResponse.PTZStatus->Position)
		{
			if(tptz__GetStatusResponse.PTZStatus->Position->PanTilt)
			{
				status->position.fP = tptz__GetStatusResponse.PTZStatus->Position->PanTilt->x;
				status->position.fT = tptz__GetStatusResponse.PTZStatus->Position->PanTilt->y;
#ifdef USE_PTZ_SPACE
				strcpy(status->position.PT_Space,Onvif2_StringSafe(tptz__GetStatusResponse.PTZStatus->Position->PanTilt->space));
				PrintD("POSITION PTSpace =%s\n",status->position.PT_Space);
#endif
				PrintD("POSITION Pan =%f\n",status->position.fP);
				PrintD("POSITION Tilt=%f\n",status->position.fT);
			}
			if(tptz__GetStatusResponse.PTZStatus->Position->Zoom)
			{
				status->position.fZ = tptz__GetStatusResponse.PTZStatus->Position->Zoom->x;
#ifdef USE_PTZ_SPACE
				strcpy(status->position.Z_Space,Onvif2_StringSafe(tptz__GetStatusResponse.PTZStatus->Position->Zoom->space));
				PrintD("POSITION ZSpace =%s\n",status->position.Z_Space);
#endif
				PrintD("POSITION Zoom=%f\n",status->position.fZ);
			}
		}

		if(tptz__GetStatusResponse.PTZStatus->MoveStatus)
		{
			if(tptz__GetStatusResponse.PTZStatus->MoveStatus->PanTilt)
			{
				status->PTMoveStatus = *(tptz__GetStatusResponse.PTZStatus->MoveStatus->PanTilt);
				PrintD("MoveStatus PanTilt=%d\n",status->PTMoveStatus);
			}
			if(tptz__GetStatusResponse.PTZStatus->MoveStatus->Zoom)
			{
				status->ZMoveStatus = *(tptz__GetStatusResponse.PTZStatus->MoveStatus->Zoom);
				PrintD("MoveStatus Zoom=%d\n",status->ZMoveStatus);
			}
		}

		//status->UtcTime = tptz__GetStatusResponse.PTZStatus->UtcTime;
		//PrintD("UtcTime = %l\n", status->UtcTime);

		//strcpy(status->Error,Onvif2_StringSafe(tptz__GetStatusResponse.PTZStatus->Error));
		//PrintD("Error = %s\n", status->Error);

		if(tptz__GetStatusResponse.PTZStatus->Position == NULL &&
			tptz__GetStatusResponse.PTZStatus->MoveStatus == NULL)
		{
			return -2;// 返回错误
		}
	}
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::PTZ_GetPresets(const char* XAddrs, const char* UserName,const char* Password, 
								  const char* ProfileToken, LIST_Preset *PresetList)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL || PresetList == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = 10;//m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__GetPresets *tptz__GetPresets = soap_new__tptz__GetPresets(soap);
	if(tptz__GetPresets == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__GetPresets->ProfileToken = soap_strdup(soap,ProfileToken);

	_tptz__GetPresetsResponse tptz__GetPresetsResponse;
	int result = ptz_proxy.GetPresets(tptz__GetPresets,tptz__GetPresetsResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.GetPresets failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	for(int i = 0; i < tptz__GetPresetsResponse.__sizePreset; i++)
	{
		tt__PTZPreset *pPreset = tptz__GetPresetsResponse.Preset[i];
		if(pPreset == NULL)
		{
			continue;
		}
		Preset preset = {0};
		strcpy(preset.Name,Onvif2_StringSafe(pPreset->Name));
		strcpy(preset.Token,Onvif2_StringSafe(pPreset->token));
		//strcpy(preset.Attribute,Onvif2_StringSafe(pPreset->__anyAttribute));
		//PrintD("PTZPreset%d Name=%s token=%s",i,preset.Name,preset.Token);
		if(pPreset->PTZPosition)
		{
			if(pPreset->PTZPosition->PanTilt)
			{
				preset.position.fP = pPreset->PTZPosition->PanTilt->x;
				preset.position.fT = pPreset->PTZPosition->PanTilt->y;
#ifdef USE_PTZ_SPACE
				strcpy(preset.position.PT_Space,Onvif2_StringSafe(pPreset->PTZPosition->PanTilt->space));
#endif
			}
			if(pPreset->PTZPosition->Zoom)
			{
				preset.position.fZ = pPreset->PTZPosition->Zoom->x;
#ifdef USE_PTZ_SPACE
				strcpy(preset.position.Z_Space,Onvif2_StringSafe(pPreset->PTZPosition->Zoom->space));
#endif
			}
			//PrintD("PTZPreset%d Position Pan=%f Tilt=%f Zoom=%f",i,
			//	preset.position.fP,	preset.position.fT,	preset.position.fZ);
		}
		//PrintD("\n");

		PresetList->push_front(preset);
	}
	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}


int OnvifInternal::PTZ_SetPreset(const char* XAddrs, const char* UserName,const char* Password,
								 const char* ProfileToken, const char* PresetName, char* PresetToken)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL ||
		PresetToken == NULL || PresetName == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__SetPreset *tptz__SetPreset = soap_new__tptz__SetPreset(soap);
	if(tptz__SetPreset == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__SetPreset->ProfileToken = soap_strdup(soap,ProfileToken);
	if(strlen(PresetToken) > 0)
	{
		tptz__SetPreset->PresetToken = soap_strdup(soap,PresetToken);
	}
	tptz__SetPreset->PresetName = soap_strdup(soap,PresetName);

	_tptz__SetPresetResponse tptz__SetPresetResponse;
	int result = ptz_proxy.SetPreset(tptz__SetPreset,tptz__SetPresetResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.SetPreset failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	strcpy(PresetToken,Onvif2_StringSafe(tptz__SetPresetResponse.PresetToken));
	PrintD("SetPreset OK, PresetToken=%s\n",PresetToken);

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::PTZ_GotoPreset(const char* XAddrs, const char* UserName,const char* Password,
								  const char* ProfileToken, const char* PresetToken, const PTZ_SPEED *speed)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL || PresetToken == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__GotoPreset *tptz__GotoPreset = soap_new__tptz__GotoPreset(soap);
	if(tptz__GotoPreset == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__GotoPreset->ProfileToken = soap_strdup(soap,ProfileToken);
	tptz__GotoPreset->PresetToken = soap_strdup(soap,PresetToken);
	if(speed)
	{
		tptz__GotoPreset->Speed = soap_new_tt__PTZSpeed(soap);
		if(tptz__GotoPreset->Speed == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
		tptz__GotoPreset->Speed->PanTilt = soap_new_tt__Vector2D(soap);
		if(tptz__GotoPreset->Speed->PanTilt == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
		tptz__GotoPreset->Speed->Zoom = soap_new_tt__Vector1D(soap);
		if(tptz__GotoPreset->Speed->Zoom == NULL)
		{
			printf("Out of memory\n");
			return -1;
		}
#ifdef USE_PTZ_SPACE
		tptz__GotoPreset->Speed->PanTilt->space = soap_strdup(soap,speed->PT_Space);
#endif
		tptz__GotoPreset->Speed->PanTilt->x = speed->P_Speed;
		tptz__GotoPreset->Speed->PanTilt->y = speed->T_Speed;
#ifdef USE_PTZ_SPACE
		tptz__GotoPreset->Speed->Zoom->space = soap_strdup(soap,speed->Z_Space);
#endif
		tptz__GotoPreset->Speed->Zoom->x     = speed->Z_Speed;
	}

	_tptz__GotoPresetResponse tptz__GotoPresetResponse;
	int result = ptz_proxy.GotoPreset(tptz__GotoPreset,tptz__GotoPresetResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.SetPreset failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::PTZ_RemovePreset(const char* XAddrs, const char* UserName,const char* Password,
									const char* ProfileToken, const char* PresetToken)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL || PresetToken == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__RemovePreset *tptz__RemovePreset = soap_new__tptz__RemovePreset(soap);
	if(tptz__RemovePreset == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__RemovePreset->ProfileToken = soap_strdup(soap,ProfileToken);
	tptz__RemovePreset->PresetToken  = soap_strdup(soap,PresetToken);

	_tptz__RemovePresetResponse tptz__RemovePresetResponse;

	int result = ptz_proxy.RemovePreset(tptz__RemovePreset,tptz__RemovePresetResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.RemovePreset failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal::PTZ_GotoHomePosition(const char* XAddrs, const char* UserName,const char* Password, 
										const char* ProfileToken, const PTZ_SPEED *speed)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__GotoHomePosition *tptz__GotoHomePosition = soap_new__tptz__GotoHomePosition(soap);
	if(tptz__GotoHomePosition == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__GotoHomePosition->ProfileToken = soap_strdup(soap,ProfileToken);
	tptz__GotoHomePosition->Speed = soap_new_tt__PTZSpeed(soap);
	if(tptz__GotoHomePosition->Speed == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__GotoHomePosition->Speed->PanTilt = soap_new_tt__Vector2D(soap);
	if(tptz__GotoHomePosition->Speed->PanTilt == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__GotoHomePosition->Speed->Zoom = soap_new_tt__Vector1D(soap);
	if(tptz__GotoHomePosition->Speed->Zoom == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
#ifdef USE_PTZ_SPACE
	tptz__GotoHomePosition->Speed->PanTilt->space = soap_strdup(soap,speed->PT_Space);
#endif
	tptz__GotoHomePosition->Speed->PanTilt->x = speed->P_Speed;
	tptz__GotoHomePosition->Speed->PanTilt->y = speed->T_Speed;
#ifdef USE_PTZ_SPACE
	tptz__GotoHomePosition->Speed->Zoom->space = soap_strdup(soap,speed->Z_Space);
#endif
	tptz__GotoHomePosition->Speed->Zoom->x     = speed->Z_Speed;

	_tptz__GotoHomePositionResponse tptz__GotoHomePositionResponse;
	int result = ptz_proxy.GotoHomePosition(tptz__GotoHomePosition,tptz__GotoHomePositionResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.GotoHomePosition failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

int OnvifInternal:: PTZ_SetHomePosition(const char* XAddrs, const char* UserName,const char* Password, 
										const char* ProfileToken)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__SetHomePosition *tptz__SetHomePosition = soap_new__tptz__SetHomePosition(soap);
	if(tptz__SetHomePosition == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__SetHomePosition->ProfileToken = soap_strdup(soap,ProfileToken);

	_tptz__SetHomePositionResponse tptz__SetHomePositionResponse;
	int result = ptz_proxy.SetHomePosition(tptz__SetHomePosition,tptz__SetHomePositionResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.SetHomePosition failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}

/*int OnvifInternal::PTZ_SendAuxiliaryCommand(const char* XAddrs, const char* UserName,const char* Password, 
											const char* ProfileToken, const char *AuxiliaryData)
{
	if(XAddrs == NULL || UserName == NULL || Password == NULL || ProfileToken == NULL || AuxiliaryData == NULL)
	{
		printf("ParamIn is error\n");
		return -1;
	}
	PrintD("\n[Enter %s]\n", __FUNCTION__); 

	PTZBindingProxy ptz_proxy(XAddrs);
	struct soap* soap = ptz_proxy.soap;
	soap->connect_timeout = m_lConnTimeOut;
	soap->recv_timeout = m_lRecvTimeOut;
	soap->send_timeout = m_lSendTimeOut;

	// 设置用户名+密码
	soap_wsse_add_UsernameTokenDigest(soap, NULL, UserName, Password);

	_tptz__SendAuxiliaryCommand *tptz__SendAuxiliaryCommand = soap_new__tptz__SendAuxiliaryCommand(soap);
	if(tptz__SendAuxiliaryCommand == NULL)
	{
		printf("Out of memory\n");
		return -1;
	}
	tptz__SendAuxiliaryCommand->ProfileToken  = soap_strdup(soap,ProfileToken);
	tptz__SendAuxiliaryCommand->AuxiliaryData = soap_strdup(soap,ProfileToken);

	_tptz__SendAuxiliaryCommandResponse tptz__SendAuxiliaryCommandResponse;
	int result = ptz_proxy.SendAuxiliaryCommand(tptz__SendAuxiliaryCommand, tptz__SendAuxiliaryCommandResponse);
	if (result != SOAP_OK)
	{
		PrintD("ptz_proxy.SendAuxiliaryCommand failed errcode=%d\n",result);
		soap_print_fault(soap, stderr); 
		return -1;
	}

	PrintD("[Leave %s]\n", __FUNCTION__); 
	return 0;
}*/

