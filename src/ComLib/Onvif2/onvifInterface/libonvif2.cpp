#include "libonvif2.h"
#include "libonvif2com.h"
#include "libonvif2internal.h"
#ifdef WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
#include <math.h>
#include <iostream>
#include <map>
using namespace std;

typedef map<long, DeviceInfo*> DeviceMap;
DeviceMap g_DeviceMap;

static int GetProfileByToken(LIST_Profile ProfileList, const char *Token, ProfileInfo* Profile)
{
	if(Token == NULL || Profile == NULL)
	{
		return -1;
	}

	int result = -1;
	LIST_Profile::iterator iter = ProfileList.begin();
	for(;iter != ProfileList.end(); iter++)
	{
		if(strcmp(iter->token,Token)==0)
		{
			*Profile = *iter;
			result = 0;
			break;
		}
	}
	return result;
}

static int GetPtzNodeByToken(LIST_PTZNode PtzNodeList, const char *Token,PTZNode *PtzNode)
{
	if(Token == NULL || PtzNode == NULL)
	{
		return -1;
	}

	int result = -1;
	LIST_PTZNode::iterator iter = PtzNodeList.begin();
	for(;iter != PtzNodeList.end(); iter++)
	{
		if(strcmp(iter->token, Token)==0)
		{
			*PtzNode = *iter;
			result = 0;
			break;
		}
	}
	return result;
}

static int GetStreamUriByToken(LIST_StreamURI StreamURIList, const char *Token,StreamURI *streamuri)
{
	if(Token == NULL || streamuri == NULL)
	{
		return -1;
	}

	int result = -1;
	LIST_StreamURI::iterator iter = StreamURIList.begin();
	for(;iter != StreamURIList.end(); iter++)
	{
		if(strcmp(iter->ProfileToken, Token)==0)
		{
			*streamuri = *iter;
			result = 0;
			break;
		}
	}
	return result;
}

LIBONVIF2_API int libOnvif2Debug(int bOpen)
{
	if(bOpen)
	{
		OnvifInternal::SetDebugModel(ONVIF_DEBUG_OPEN);
	}
	else
	{
		OnvifInternal::SetDebugModel(ONVIF_DEBUG_CLOSE);
	}
	return 0;
}

LIBONVIF2_API int libOnvif2ProbeAllNVT(LIST_NVT * Devices)
{
	return OnvifInternal::ProbeAllDevices(Devices);
}

LIBONVIF2_API long libOnvif2ControlCreate( const char * IP)
{
	if( !Onvif2_IsValidIPV4Addr(IP) )
	{
        return -1;
	}

    char ServiceUrl[URI_LENGTH] = {0};
	NVTInfo nvtInfo = {0};
    int result = OnvifInternal::ProbeDevice(IP, &nvtInfo);
    if(result != 0)
    {
		result = OnvifInternal::ProbeDeviceByWsdd(IP, &nvtInfo);
	}

	// 解析是否有合法的设备服务地址
	OnvifScopes OScopes = {0};
	if(result == 0)
	{
		result = -1;
		LIST_MatchInfo::iterator lIter;
		for(lIter = nvtInfo.MatchInfos.begin();lIter != nvtInfo.MatchInfos.end(); ++lIter)
		{
			if(Onvif2_ParseUrlFromXAddrs(lIter->xAddrs, IP, ServiceUrl))
			{
				printf("ServiceURI:%s\n",lIter->xAddrs);
				result = 0;
				OnvifInternal::ParseDeviceScopes(lIter->Item,&OScopes);
				break;
			}
		}
	}
 
	if(result != 0)
    {
		printf("ProbeDevice %s failed\n",IP);
		sprintf(ServiceUrl,"http://%s/onvif/device_service",IP);
        printf("Use Defualt ServiceURI:%s\n",ServiceUrl);
    }

    long sessionid = (int)abs((int)inet_addr(IP));
	DeviceInfo* pDevideinfo = g_DeviceMap[sessionid];
    if(pDevideinfo == NULL)
    {
        pDevideinfo = new DeviceInfo;
        if(pDevideinfo ==NULL)
        {
			printf("Out of memory\n");
            return -1;
        }

        g_DeviceMap[sessionid] = pDevideinfo;
    }

	InitDeviceInfo(pDevideinfo);
	strcpy(pDevideinfo->IP, IP);
	strcpy(pDevideinfo->ServiceURI, ServiceUrl);
	strcpy(pDevideinfo->Name, OScopes.name);
	strcpy(pDevideinfo->Location, OScopes.location);
	strcpy(pDevideinfo->Hardware, OScopes.hardware);

    return sessionid;
}

LIBONVIF2_API int libOnvif2SetTimeOut(int ts, int tr, int tc)
{
	OnvifInternal::SetTimeOut(ts,tr,tc);
	return 0;
}


LIBONVIF2_API int libOnvif2ControlDelete(long hSession)
{
    DeviceInfo* pDevideinfo = g_DeviceMap[hSession];

    if(pDevideinfo != NULL)
    {
        delete pDevideinfo;
        pDevideinfo = NULL;

        g_DeviceMap.erase(hSession);
    }

	return 0;
}

LIBONVIF2_API int libOnvif2ControlCleanup()
{
    for(DeviceMap::iterator it = g_DeviceMap.begin();it != g_DeviceMap.end();it++)
    {
        DeviceInfo* pDevideinfo = it->second;

        if(pDevideinfo != NULL)
        {
            delete pDevideinfo;
            pDevideinfo = NULL;
        }
    }

    g_DeviceMap.clear();

    return 0;
}


LIBONVIF2_API int libOnvif2DeviceLogin(long hSession,const char* UserName,const char* Passwd)
{
	int result = -1;

    if(UserName == NULL || Passwd == NULL)
    {
        return -1;
    }

    DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        return -1;
    }

	if(strlen(pDevinfo->ServiceURI) < 1)
	{
		printf("ServiceURI is empty\n");
		return -1;
	}

	// 记录用户名密码
    strcpy(pDevinfo->UserName,UserName);
    strcpy(pDevinfo->Pass,Passwd);

	// 能力信息 Device Imaging Media PTZ的URI
	Capabilities caps = {0};
	result = OnvifInternal::GetCapabilities(pDevinfo->ServiceURI,pDevinfo->UserName,pDevinfo->Pass,&caps);
	if(result == 0)
    {
		strcpy(pDevinfo->OnvifVersion,caps.Dev_OnvifVersions);

		if (strlen(caps.Dev_XAddr) > 0)
			strcpy(pDevinfo->DeviceURI,caps.Dev_XAddr);
		else
			strcpy(pDevinfo->DeviceURI,pDevinfo->ServiceURI);

		if (strlen(caps.Image_XAddr) > 0)
			strcpy(pDevinfo->ImagingURI,caps.Image_XAddr);
		else
			strcpy(pDevinfo->ImagingURI,pDevinfo->ServiceURI);

		if (strlen(caps.Media_XAddr) > 0)
			strcpy(pDevinfo->MediaURI,caps.Media_XAddr);
		else
			strcpy(pDevinfo->MediaURI,pDevinfo->ServiceURI);

		if (strlen(caps.PTZ_XAddr) > 0)
			strcpy(pDevinfo->PTZURI,caps.PTZ_XAddr);
		else
			strcpy(pDevinfo->PTZURI,pDevinfo->ServiceURI);
    }
	else
	{
#if 0
		return -1;// 严重错误，不能再继续
#else
		// 统一使用ServiceURI
		printf("Use ServiceURI as DeviceURI ImagingURI MediaURI PTZURI...\n");
		strcpy(pDevinfo->DeviceURI,pDevinfo->ServiceURI);
		strcpy(pDevinfo->ImagingURI,pDevinfo->ServiceURI);
		strcpy(pDevinfo->MediaURI,pDevinfo->ServiceURI);
		strcpy(pDevinfo->PTZURI,pDevinfo->ServiceURI);

		//const char * p = strstr(pDevinfo->ServiceURI, "/device_service");// http://IP[:port]/onvif/device_service
		//char prefixtemp[URI_LENGTH] = {0};
		//if(p) strncpy(prefixtemp, pDevinfo->ServiceURI, (p-pDevinfo->ServiceURI));
		
		//printf("Use Defualt URI...\n");
		//sprintf(pDevinfo->DeviceURI,  "%s/device", prefixtemp);//IP[:port]/onvif/device
		//sprintf(pDevinfo->ImagingURI, "%s/imaging",prefixtemp);//IP[:port]/onvif/imaging
		//sprintf(pDevinfo->MediaURI,   "%s/media",  prefixtemp);//IP[:port]/onvif/media
		//sprintf(pDevinfo->PTZURI,     "%s/ptz",	   prefixtemp);//IP[:port]/onvif/ptz
#endif
	}

	// 设备信息
	DeviceInfomation info = {0};
	result = OnvifInternal::GetDeviceInformation(pDevinfo->DeviceURI,pDevinfo->UserName,pDevinfo->Pass,&info);
	if(result == 0)
    {
		strcpy(pDevinfo->Manufacturer,info.Manufacturer);
		strcpy(pDevinfo->Model,info.Model);
		strcpy(pDevinfo->FirmwareVersion,info.FirmwareVersion);
		strcpy(pDevinfo->SerialNumber,info.SerialNumber);
		strcpy(pDevinfo->HardwareId,info.HardwareId);
    }
	if(result == -2)
	{
		// 密码错误或者球机被锁定了
		return -2;
	}

	// Profile
	pDevinfo->ProfileList.clear();
	result = OnvifInternal::GetAllProfiles(pDevinfo->MediaURI,pDevinfo->UserName,pDevinfo->Pass,&(pDevinfo->ProfileList));
	if(result != 0)
	{
		printf("GetAllProfiles failed， can not do anything...\n");
		return -1;
	}

	// 获取每个Profile的视频播放地址
	pDevinfo->StreamURIList.clear();
	LIST_Profile::iterator iter = pDevinfo->ProfileList.begin();
	for(; iter != pDevinfo->ProfileList.end(); iter++)
	{
		StreamURI streamUrl = {0};
		if(OnvifInternal::GetStreamUri(pDevinfo->MediaURI,pDevinfo->UserName,pDevinfo->Pass,iter->token, &streamUrl) == 0)
		{
			// 给RTSP地址加上用户信息
			string strUri = streamUrl.RtspUri;
			string::size_type pos1 = strUri.find_first_of("@");
			if (pos1 == string::npos)
			{
				pos1 = strUri.find_first_of("rtsp://");
				if (pos1 != string::npos)
				{
					string strTemp = strUri.substr(pos1 + 7);
					sprintf(streamUrl.RtspUri,"rtsp://%s:%s@%s",
						pDevinfo->UserName,pDevinfo->Pass,strTemp.c_str());
				}
			}

			printf("Profile:%s StreamURI=%s\n",streamUrl.ProfileToken, streamUrl.RtspUri);
			pDevinfo->StreamURIList.push_back(streamUrl);
		}
	}
	
	// 获取所有VideoSource
	pDevinfo->VideoSourceList.clear();
	OnvifInternal::GetVideoSources(pDevinfo->MediaURI,pDevinfo->UserName,pDevinfo->Pass,
		&(pDevinfo->VideoSourceList));


	// 获取所有PTZNODE
	pDevinfo->PtzNodeList.clear();
	OnvifInternal::PTZ_GetNodes(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,
		&(pDevinfo->PtzNodeList));

	// 获取所有Preset
	pDevinfo->PresetList.clear();
	OnvifInternal::PTZ_GetPresets(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,
		pDevinfo->ProfileList.begin()->token, &(pDevinfo->PresetList));

	return 0;
}

LIBONVIF2_API int libOnvif2GetProfileSize(long hSession)
{
	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }
	return pDevinfo->ProfileList.size();
}


LIBONVIF2_API int libOnvif2ReportAfterLogin(long hSession)
{
    DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }

	// 检查服务地址是否完善
	if(strlen(pDevinfo->DeviceURI) < 1)
	{
        //printf("[ERROR] DeviceURI = %s!\n",pDevinfo->DeviceURI);
		printf("严重错误 无法工作!\n");
		return -1;
	}

	if(strlen(pDevinfo->MediaURI) < 1)
	{
        //printf("[ERROR] MediaURI = %s!\n",pDevinfo->MediaURI);
 		printf("严重错误 无法获取视频!\n");
       return -1;
	}

	if(strlen(pDevinfo->PTZURI) < 1)
	{
        //printf("[ERROR] PTZURI = %s!\n",pDevinfo->PTZURI);
 		printf("严重错误 无法控制云台!\n");
        return -1;
	}

	if(strlen(pDevinfo->ImagingURI) < 1)
	{
        //printf("[ERROR] ImagingURI = %s!\n",pDevinfo->ImagingURI);
  		printf("一般错误 无法调节图像!\n");
		//return -1;
	}

	// 检查Profile
	if(pDevinfo->ProfileList.size() < 1)
	{
        printf("[ERROR] ProfileList.size = %d!\n",pDevinfo->ProfileList.size());
  		printf("严重错误 没有Profile,可能是用户名密码错误导致登录失败!\n");
		return -1;
	}

	// 找到最大分辨率的Profile作为最佳
	int iBestIndex = -1;
	int iMaxSolution = 0;
	LIST_Profile::iterator iBestIter;
	LIST_Profile::iterator iter = pDevinfo->ProfileList.begin();
	for(int i = 0;iter != pDevinfo->ProfileList.end(); iter++)
	{
		if(iter->VideoEncoderCfg.Encoding != 2)
		{
			printf("Profile%d: is not H264 %d!\n",i, iter->VideoEncoderCfg.Encoding);
			continue;
		}

		printf("Profile%d: %d X %d!\n",i, iter->VideoEncoderCfg.Width,iter->VideoEncoderCfg.Height);
		int iSolution = iter->VideoEncoderCfg.Width*iter->VideoEncoderCfg.Height;
		if(iSolution > iMaxSolution)
		{
			iBestIndex = i;
			iBestIter = iter;
			iMaxSolution = iSolution;
			//break;
		}
		i++;
	}

	if(iBestIndex == -1)
	{
  		printf("严重错误 没有有效分辨率的Profile!\n");
		return -1;
	}

	StreamInfo streamInfo = {0};
	int iret = libOnvif2GetStreamInfo(hSession, iBestIndex, &streamInfo);
	if(iret != 0 || strlen(streamInfo.URI) < 1)
	{
  		printf("严重错误 没有RTSP地址 不能获取视频流!\n");
		return -1;
	}

	PTZInfo ptznfo;
	iret = libOnvif2GetPTZInfo(hSession, iBestIndex, &ptznfo);
	if(iret != 0 || ptznfo.bSupport == false)
	{
  		printf("严重错误 不支持PTZ!\n");
		//return -1;
	}

	if(ptznfo.bSupportAbsMove == false)
	{
  		printf("警告 不支持绝对PTZ移动!\n");
	}

	if(iBestIter->VideoEncoderCfg.Framerate < 15)
	{
  		printf("警告 帧率%d < 15 过低!\n",iBestIter->VideoEncoderCfg.Framerate);
	}

	if(iBestIter->VideoEncoderCfg.Framerate > 30)
	{
  		printf("警告 帧率%d > 30 过高!\n",iBestIter->VideoEncoderCfg.Framerate);
	}

	if(iBestIter->VideoEncoderCfg.Bitrate < 512)
	{
  		printf("警告 码率%d < 512 过低!\n",iBestIter->VideoEncoderCfg.Bitrate);
	}

	if(iBestIter->VideoEncoderCfg.Bitrate > 4096)
	{
  		printf("警告 码率%d > 4096 过高!\n",iBestIter->VideoEncoderCfg.Bitrate);
	}

	if(iBestIter->VideoEncoderCfg.GovLength > 25)
	{
  		printf("警告 I帧间隔%d > 25 过于稀疏!\n",iBestIter->VideoEncoderCfg.GovLength);
	}

	printf("The Best Profile is %d, token is %s!\n",iBestIndex, iBestIter->token);

	return iBestIndex;
}

LIBONVIF2_API int libOnvif2GetDateTime(long hSession, char* pszDateTime)
{
	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }

	DevDateTime theDateTime;
	int result = OnvifInternal::GetSystemDateAndTime(pDevinfo->DeviceURI,pDevinfo->UserName,pDevinfo->Pass,
												    &theDateTime);
	if(pszDateTime)
	{
		sprintf(pszDateTime, "%04ld-%02ld-%02ld %02ld:%02ld:%02ld", 
			theDateTime.year, theDateTime.month, theDateTime.day,
			theDateTime.hour, theDateTime.minute, theDateTime.second);
	}
	return result;

}

LIBONVIF2_API int libOnvif2DeviceLogout(long hSession)
{
    DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo != NULL)
    {
        memset(pDevinfo,0,sizeof(DeviceInfo));
    }
    else
    {
        return -1;
    }
    return 0;
}


LIBONVIF2_API MANUFACTURER_TYPE libOnvif2GetManufacturer(long hSession)
{
	MANUFACTURER_TYPE man = _Unknown;
    DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return man;
    }

	if( strstr(pDevinfo->Manufacturer,"hik") ||
		strstr(pDevinfo->Manufacturer,"Hik") ||
		strstr(pDevinfo->Manufacturer,"HIK") )
	{
		man = _Hikvision;
	}
	
	if( strstr(pDevinfo->Manufacturer,"dahua") ||
		strstr(pDevinfo->Manufacturer,"DaHua") ||
		strstr(pDevinfo->Manufacturer,"DAHUA") )
	{
		man = _DaHua;
	}

	if( strstr(pDevinfo->Manufacturer,"tiandy") ||
		strstr(pDevinfo->Manufacturer,"Tiandy") ||
		strstr(pDevinfo->Manufacturer,"TIANDY") )
	{
		man = _Tiandy;
	}
	if( strstr(pDevinfo->Manufacturer,"uniview") ||
		strstr(pDevinfo->Manufacturer,"Uniview") ||
		strstr(pDevinfo->Manufacturer,"UNIVIEW") )
	{
		man = _Uniview;
	}

	return man;
}

LIBONVIF2_API int libOnvif2GetStreamInfo(long hSession, int index, StreamInfo * Info)
{
	if(Info == NULL)
    {
        return -1;
    }

	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }

	if(pDevinfo->ProfileList.size() < 1)
	{
        printf("Profile is empty!\n");
        return -1;
	}

	if(pDevinfo->ProfileList.size() <= index || index < 0)
	{
        printf("index is error,%d is not between (0 %d)!\n",index, pDevinfo->ProfileList.size());
        return -1;
	}

	LIST_Profile::iterator iter = pDevinfo->ProfileList.begin();
	for(int i = 0; i < index; i++)
	{
		iter++;
	}
	ProfileInfo Profile = *iter;

	StreamURI streamuri = {0};
	GetStreamUriByToken(pDevinfo->StreamURIList,iter->token, &streamuri);

	strcpy(Info->ProfileToken,Profile.token);
	strcpy(Info->URI,streamuri.RtspUri);
	Info->width  = Profile.VideoEncoderCfg.Width;
	Info->height = Profile.VideoEncoderCfg.Height;
	
	return 0;
}


LIBONVIF2_API int libOnvif2GetPTZInfo(long hSession, int index, PTZInfo * Info)
{
	if(Info == NULL)
    {
        return -1;
    }

	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }

	if(pDevinfo->ProfileList.size() < 1)
	{
        printf("Profile is empty!\n");
        return -1;
	}

	if(pDevinfo->ProfileList.size() <= index || index < 0)
	{
        printf("index is error,%d is not between (0 %d)!\n",index, pDevinfo->ProfileList.size());
        return -1;
	}

	LIST_Profile::iterator iter = pDevinfo->ProfileList.begin();
	for(int i = 0; i < index; i++)
	{
		iter++;
	}

	ProfileInfo Profile = *iter;
	strcpy(Info->ProfileToken,Profile.token);
	strcpy(Info->PTZNodeToken,Profile.PTZCfg.NodeToken);
	//Info->width  = Profile.VideoEncoderCfg.Width;
	//Info->height = Profile.VideoEncoderCfg.Height;

	PTZNode PtzNode;
	if(GetPtzNodeByToken(pDevinfo->PtzNodeList, Profile.PTZCfg.NodeToken,&PtzNode) == 0)
	{
		Info->bSupport  = 1;
		if(strlen(Profile.PTZCfg.DefAbsPTPostionSpace) > 0 && strlen(Profile.PTZCfg.DefAbsZPostionSpace) > 0)
		{
			PTZStatus status = {0};
			int result = OnvifInternal::PTZ_GetStatus(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,Profile.token,&status);
			Info->bSupportAbsMove = (result == -2 || (status.position.fP == 0 && status.position.fT == 0 && status.position.fP == 0)) ? 0 : 1;
		}
		else
		{
			Info->bSupportAbsMove = 0;
		}
		Info->bSupportHome    = PtzNode.HomeSupported;
		Info->MaxPresetsNum   = PtzNode.MaxPresetsNum;
	}
	else
	{
		Info->bSupport  = 0;
		Info->bSupportAbsMove = 0;
		Info->bSupportHome    = 0;
		Info->MaxPresetsNum   = 0;
	}

	
	return 0;
}



static int libOnvif2PTZStartMoveByTime(long hSession,const char * ProfileToken,
										PTZ_ACTION Action,int Velocity,int TimeOut)
{
	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }
	
	ProfileInfo Profile;
	if(GetProfileByToken(pDevinfo->ProfileList,ProfileToken,&Profile) != 0)
	{
        printf("Profile is empty!\n");
        return -1;
	}

	PTZNode PtzNode;
	if(GetPtzNodeByToken(pDevinfo->PtzNodeList, Profile.PTZCfg.NodeToken,&PtzNode) != 0)
	{
        printf("PTZNodeToken is empty!\n");
        return -1;
	}

	if(Velocity < 0) Velocity = 0;
	if(Velocity > 100) Velocity = 100;


	PTZ_SPEED speed = {0};
#ifdef USE_PTZ_SPACE
	strcpy(speed.PT_Space, PtzNode.tConVelocitySpace.PT_URI);
	strcpy(speed.Z_Space, PtzNode.tConVelocitySpace.Z_URI);
#endif

    float fVelocity  = Velocity/100.0f;

	switch(Action)
    {
	case _toLeft:
		speed.P_Speed = 0 - fVelocity;
        break;
	case _toLeftUp:
		speed.P_Speed = 0 - fVelocity;
		speed.T_Speed = fVelocity;
        break;
	case _toUp:
		speed.T_Speed = fVelocity;
        break;
	case _toRightUp:
		speed.P_Speed = fVelocity;
		speed.T_Speed = fVelocity;
        break;
	case _toRight:
		speed.P_Speed = fVelocity;
        break;
	case _toRightDown:
		speed.P_Speed = fVelocity;
		speed.T_Speed = 0 - fVelocity;
        break;
	case _toDown:
		speed.T_Speed = 0 - fVelocity;
        break;
	case _toLeftDown:
		speed.P_Speed = 0 - fVelocity;
		speed.T_Speed = 0 - fVelocity;
        break;
	case _toZoomOut:
		speed.Z_Speed = 0 - fVelocity;
        break;
	case _toZoomIn:
 		speed.Z_Speed = fVelocity;
        break;
	};

	int result = OnvifInternal::PTZ_ContinuousMove(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,
												   ProfileToken, &speed, TimeOut);
	return result;
}

LIBONVIF2_API int libOnvif2PTZStartMove(long hSession,const char * ProfileToken, 
										PTZ_ACTION Action, int Velocity)
{
	return libOnvif2PTZStartMoveByTime(hSession,ProfileToken,Action,Velocity,0);
}

LIBONVIF2_API int libOnvif2PTZStopMove(long hSession,const char * ProfileToken)
{
	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }
	
	ProfileInfo Profile;
	if(GetProfileByToken(pDevinfo->ProfileList,ProfileToken,&Profile) != 0)
	{
        printf("Profile is empty!\n");
        return -1;
	}

	PTZNode PtzNode;
	if(GetPtzNodeByToken(pDevinfo->PtzNodeList, Profile.PTZCfg.NodeToken,&PtzNode) != 0)
	{
        printf("PTZNodeToken is empty!\n");
        return -1;
	}

	int result = OnvifInternal::PTZ_Stop(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,
												   ProfileToken, true, true);
	return result;
}

LIBONVIF2_API int libOnvif2PTZGetAbsolutePosition(long hSession,const char * ProfileToken,
											  PTZAbsPosition *Position)
{
	if(Position == NULL)
	{
		return -1;
	}

	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }
	
	ProfileInfo Profile;
	if(GetProfileByToken(pDevinfo->ProfileList,ProfileToken,&Profile) != 0)
	{
        printf("Profile is empty!\n");
        return -1;
	}

	PTZNode PtzNode;
	if(GetPtzNodeByToken(pDevinfo->PtzNodeList, Profile.PTZCfg.NodeToken,&PtzNode) != 0)
	{
        printf("PTZNodeToken is empty!\n");
        return -1;
	}
  
	PTZStatus status = {0};
	int result = OnvifInternal::PTZ_GetStatus(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,
												   ProfileToken, &status);
	if(result != 0)
	{
		return result;
	}

	Position->_Pan  = status.position.fP;
	Position->_Tilt = status.position.fT;
	Position->_Zoom = status.position.fZ;

	if(status.PTMoveStatus != 0)
	{
        printf("Device is PT moving,now!\n");
		return status.PTMoveStatus;
	}
	if(status.ZMoveStatus != 0)
	{
        printf("Device is Zoom moving,now!\n");
		return status.ZMoveStatus;
	}

	return 0;
}

LIBONVIF2_API int libOnvif2PTZAbsoluteMove(long hSession, const char * ProfileToken,
									   const PTZAbsPosition *Position,int PanTiltVelocity, int ZoomVelocity)
{
	if(Position == NULL)
	{
		return -1;
	}

	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }
	
	ProfileInfo Profile;
	if(GetProfileByToken(pDevinfo->ProfileList,ProfileToken,&Profile) != 0)
	{
        printf("Profile is empty!\n");
        return -1;
	}
  
	PTZNode PtzNode;
	if(GetPtzNodeByToken(pDevinfo->PtzNodeList, Profile.PTZCfg.NodeToken,&PtzNode) != 0)
	{
        printf("PTZNodeToken is empty!\n");
        return -1;
	}
  
	float fPanVelocity  = PanTiltVelocity/100.0f;
    float fTiltVelocity = PanTiltVelocity/100.0f;
    float fZoomVelocity = ZoomVelocity/100.0f;

	PTZ_POSITION pos = {0};
#ifdef USE_PTZ_SPACE
	strcpy(pos.PT_Space, PtzNode.tAbsPositionSpace.PT_URI);
	strcpy(pos.Z_Space,  PtzNode.tAbsPositionSpace.Z_URI);
#endif
	pos.fP = Position->_Pan;
	pos.fT = Position->_Tilt;
	pos.fZ = Position->_Zoom;

	PTZ_SPEED  speed = {0};
#ifdef USE_PTZ_SPACE
	strcpy(speed.PT_Space, PtzNode.tSpeedSapce.PT_URI);
	strcpy(speed.Z_Space,  PtzNode.tSpeedSapce.Z_URI);
#endif
	speed.P_Speed = fPanVelocity;
	speed.T_Speed = fTiltVelocity;
	speed.Z_Speed = fZoomVelocity;

	int result = OnvifInternal::PTZ_AbsoluteMove(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,
												   ProfileToken, &pos, &speed);
	return result;
}

LIBONVIF2_API int libOnvif2GetPresets(long hSession,const char * ProfileToken,
									  LIST_Preset* presets)
{
	if(presets == NULL)
	{
		return -1;
	}

	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }
	
	ProfileInfo Profile;
	if(GetProfileByToken(pDevinfo->ProfileList,ProfileToken,&Profile) != 0)
	{
        printf("Profile is empty!\n");
        return -1;
	}

	int result = OnvifInternal::PTZ_GetPresets(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,
												ProfileToken, presets);
	return result;
}

LIBONVIF2_API int libOnvif2CreatePreset(long hSession,const char * ProfileToken,
										unsigned int PresetNo)
{
	char presetName[PRESET_NAME_LENGTH] = {0};
	sprintf (presetName, "%s%d", ONVIF2_PRESET_NAME,PresetNo);

	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }
	
	ProfileInfo Profile;
	if(GetProfileByToken(pDevinfo->ProfileList,ProfileToken,&Profile) != 0)
	{
        printf("Profile is empty!\n");
        return -1;
	}

	libOnvif2DelPreset(hSession, ProfileToken,PresetNo);
	
	char presetToken[PRESET_NAME_LENGTH] = {0};
	int result = OnvifInternal::PTZ_SetPreset(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,ProfileToken,presetName,presetToken);
	if(result == 0)
	{
		pDevinfo->PresetList.clear();
		
		result = OnvifInternal::PTZ_GetPresets(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,
											pDevinfo->ProfileList.begin()->token, &(pDevinfo->PresetList));
	}
	return 0;
}

LIBONVIF2_API int libOnvif2DelPreset(long hSession,const char * ProfileToken,
									unsigned int PresetNo)
{
	char presetName[PRESET_NAME_LENGTH] = {0};
	sprintf (presetName, "%s%d", ONVIF2_PRESET_NAME,PresetNo);

	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }
	
	ProfileInfo Profile;
	if(GetProfileByToken(pDevinfo->ProfileList,ProfileToken,&Profile) != 0)
	{
        printf("Profile is empty!\n");
        return -1;
	}

	bool bDel = false;
	LIST_Preset::iterator iter = pDevinfo->PresetList.begin();
	for(;iter != pDevinfo->PresetList.end(); iter++)
	{
		if(strcmp(iter->Name, presetName) == 0)
		{
			bDel = true;
			printf("DelPreset \'%s\' in %s.\n",presetName,iter->Token);
			OnvifInternal::PTZ_RemovePreset(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,ProfileToken,iter->Token);
		}
	}

	if(bDel)
	{
		pDevinfo->PresetList.clear();
			
		OnvifInternal::PTZ_GetPresets(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,
									pDevinfo->ProfileList.begin()->token, &(pDevinfo->PresetList));
	}
	else
	{
		printf("No \'%s\'to del\n",presetName);
	}

	return 0;
}

LIBONVIF2_API int libOnvif2GotoPreset(long hSession,const char * ProfileToken,
									   unsigned int PresetNo)
{
	char presetName[PRESET_NAME_LENGTH] = {0};
	sprintf (presetName, "%s%d", ONVIF2_PRESET_NAME,PresetNo);

	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }
	
	ProfileInfo Profile;
	if(GetProfileByToken(pDevinfo->ProfileList,ProfileToken,&Profile) != 0)
	{
        printf("Profile is empty!\n");
        return -1;
	}

	PTZNode PtzNode;
	if(GetPtzNodeByToken(pDevinfo->PtzNodeList, Profile.PTZCfg.NodeToken,&PtzNode) != 0)
	{
        printf("PTZNodeToken is empty!\n");
        return -1;
	}

	LIST_Preset::iterator iter = pDevinfo->PresetList.begin();
	for(;iter != pDevinfo->PresetList.end(); iter++)
	{
		if(strcmp(iter->Name, presetName) == 0)
		{
			PTZ_SPEED speed;
#ifdef USE_PTZ_SPACE
			strcpy(speed.PT_Space, PtzNode.tSpeedSapce.PT_URI);
			strcpy(speed.Z_Space, PtzNode.tSpeedSapce.Z_URI);
#endif
			speed.P_Speed = 1.0f;
			speed.T_Speed = 1.0f;
			speed.Z_Speed = 1.0f;

			printf("GotoPreset \'%s\' in %s.\n",presetName,iter->Token);
			OnvifInternal::PTZ_GotoPreset(pDevinfo->PTZURI,pDevinfo->UserName,pDevinfo->Pass,ProfileToken,iter->Token,&speed);
			break;
		}
	}

	if(iter==pDevinfo->PresetList.end())
	{
		printf("No \'%s\' to go.\n",presetName);
	}
	return 0;
}

LIBONVIF2_API int libOnvif2FocusMove(long hSession,const char * ProfileToken,
									 FOCUS_ACTION Action,int Velocity)
{
	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }
	
	ProfileInfo Profile;
	if(GetProfileByToken(pDevinfo->ProfileList,ProfileToken,&Profile) != 0)
	{
        printf("Profile is empty!\n");
        return -1;
	}

    float fVelocity = Velocity/100.0f;
	if(Action == _toNear)
    {
		fVelocity = 0 - fVelocity;
	}
	int result = OnvifInternal::ContinuousFocusMove(pDevinfo->ImagingURI,pDevinfo->UserName,pDevinfo->Pass,
													Profile.VideoSourceCfg.SourceToken, fVelocity);
	return result;
}

LIBONVIF2_API int libOnvif2FocusStop(long hSession,const char * ProfileToken)
{
	DeviceInfo* pDevinfo = g_DeviceMap[hSession];
    if(pDevinfo == NULL)
    {
        printf("This session=%ld  does not exist!\n",hSession);
        return -1;
    }
	
	ProfileInfo Profile;
	if(GetProfileByToken(pDevinfo->ProfileList,ProfileToken,&Profile) != 0)
	{
        printf("Profile is empty!\n");
        return -1;
	}

	int result = OnvifInternal::FocusStop(pDevinfo->ImagingURI,pDevinfo->UserName,pDevinfo->Pass,
										Profile.VideoSourceCfg.SourceToken);
	return result;

}


