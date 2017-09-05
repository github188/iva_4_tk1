#ifndef _INTERFACEH_
#define _INTERFACEH_

#include "NetClientTypes.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN 
#include "windows.h"
#else
#define __stdcall 
#endif

#ifdef __cplusplus
extern "C"{
#endif

	int __stdcall NetClient_Startup_V4(int _iServerPort/*=3000*/, int _iClientPort/*=6000*/, int _iWnd/*=0*/);

	int __stdcall NetClient_SetNotifyFunction_V4(MAIN_NOTIFY_V4        _MainNotify, 
		ALARM_NOTIFY_V4       _AlarmNotify,
		PARACHANGE_NOTIFY_V4  _ParaNotify,
		COMRECV_NOTIFY_V4     _ComNotify,
		PROXY_NOTIFY       _ProxyNotify);

	int __stdcall NetClient_StartRecv_V4(unsigned int* _uiRecvID, CLIENTINFO* _cltInfo, NVSDATA_NOTIFY _cbkDataArrive = 0,void* _iUserData = (void*)-1);

	int __stdcall NetClient_SetNotifyUserData_V4(int _iLogonID,void* _iUserData=(void*)-1);

	int __stdcall NetClient_SetComRecvNotify_V4(COMRECV_NOTIFY_V4 _comNotify);

	int __stdcall NetClient_GetHTTPPort_V4(int _iLogonID, int *_iPort); 

	int __stdcall NetClient_SetHTTPPort_V4(int _iLogonID, int _iPort); 

	int __stdcall NetClient_SetDomainParsePara_V4(int _iLogonID, int _iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,int _iPort1, int _iPort2); 

	int __stdcall NetClient_GetDomainParsePara_V4(int _iLogonID, int *_iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,int* _iPort1, int* _iPort2);

	int __stdcall NetClient_GetBitrateOnVideo_V4(unsigned int _ulConID, int* _piX, int* _piY, int* _piEnabled, char *_pcInfo);

	int __stdcall NetClient_SetDecCallBack_V4(unsigned int _ulConID, DECYUV_NOTIFY_V4 _cbkDecYUV, void* _iUserData);

	int __stdcall NetClient_RegisterDrawFun(unsigned int _ulConID, CBK_DRAW_FUNC _pfDrawFun, long _lUserData, void *_pCmd, int _iCmdLen);

/*#ifdef WIN32
	int __stdcall NetClient_SetPort( int _iServerPort, int _iClientPort );

	int __stdcall NetClient_Startup();

	int __stdcall NetClient_SetNotifyFunction( LOGON_NOTIFY _LogonNotify,ALARM_NOTIFY _AlarmNotify, PARACHANGE_NOTIFY _ParaNotify);

	int __stdcall NetClient_SetNotifyFunctionEx( MAIN_NOTIFY _cbkMainNotify, ALARM_NOTIFY_EX _cbkAlarmNotify, PARACHANGE_NOTIFY_EX _cbkParaChangeNotify, void* _pNotifyUserData );
	
	int __stdcall NetClient_SetMSGHandle( unsigned int _uiMessage, HWND _hWnd, unsigned int _uiParaMsg, unsigned int _uiAlarmMsg );

	int __stdcall NetClient_StartRecv(unsigned int	*_ulConID, CLIENTINFO*	_cltInfo, RECVDATA_NOTIFY	_cbkDataArrive);

	int __stdcall NetClient_StartRecvEx(unsigned int *_ulConID,CLIENTINFO* _cltInfo,RECVDATA_NOTIFY_EX _cbkDataNotifyEx, void* _lpUserData);

	int __stdcall NetClient_SetComRecvNotify(COMRECV_NOTIFY _comNotify); 

	int __stdcall NetClient_GetHTTPPort(int _iLogonID, unsigned short *_iPort); 

	int __stdcall NetClient_SetHTTPPort(int _iLogonID, unsigned short _iPort); 

	int __stdcall NetClient_SetDomainParsePara(int _iLogonID, int _iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,unsigned short _iPort1, unsigned short _iPort2); 

	int __stdcall NetClient_GetDomainParsePara(int _iLogonID, int *_iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,unsigned short* _iPort1, unsigned short* _iPort2);

	int __stdcall NetClient_GetBitrateOnVideo(unsigned int	_ulConID, int*	_iX, int* _iY, int* _iEnabled);

	int __stdcall NetClient_SetDecCallBack(unsigned int	_ulConID, DECYUV_NOTIFY	_cbkGetYUV, void* _pContext);

	int __stdcall NetClient_InterTalkStartEx(unsigned int * _uiConnID, int _iLogonID, RECVDATA_NOTIFY_EX _cbkDataArrive, void* _iUserData);
#else*/
int __stdcall NetClient_Startup(int _iServerPort/*=3000*/, int _iClientPort/*=6000*/, int _iWnd/*=0*/);

int __stdcall NetClient_SetNotifyFunction(MAIN_NOTIFY_V4        _MainNotify, 
										  ALARM_NOTIFY_V4       _AlarmNotify,
										  PARACHANGE_NOTIFY_V4  _ParaNotify,
										  COMRECV_NOTIFY_V4     _ComNotify,
										  PROXY_NOTIFY       _ProxyNotify);

int __stdcall NetClient_StartRecv(unsigned int* _uiRecvID, CLIENTINFO* _cltInfo, NVSDATA_NOTIFY _cbkDataArrive = 0,void* _iUserData = (void*)-1);

int __stdcall NetClient_SetNotifyUserData(int _iLogonID,void* _iUserData=(void*)-1);

int __stdcall NetClient_SetComRecvNotify(COMRECV_NOTIFY_V4 _comNotify);

int __stdcall NetClient_GetHTTPPort(int _iLogonID, int *_iPort); 

int __stdcall NetClient_SetHTTPPort(int _iLogonID, int _iPort); 

int __stdcall NetClient_SetDomainParsePara(int _iLogonID, int _iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,int _iPort1, int _iPort2); 

int __stdcall NetClient_GetDomainParsePara(int _iLogonID, int *_iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,int* _iPort1, int* _iPort2);

int __stdcall NetClient_GetBitrateOnVideo(unsigned int _ulConID, int* _piX, int* _piY, int* _piEnabled, char *_pcInfo);

int __stdcall NetClient_SetDecCallBack(unsigned int _ulID, pfCBGetDecAV _cbkGetYUV, void* _iUserData);

int __stdcall NetClient_SetDecCallBackEx(unsigned int _ulID, DECYUV_NOTIFY _cbkDecYUV, void* _iUserData);

int __stdcall NetClient_InterTalkStartEx(unsigned int * _uiConnID, int _iLogonID, NVSDATA_NOTIFY _cbkDataArrive, void* _iUserData);
//#endif

int __stdcall NetClient_Cleanup();

int __stdcall NetClient_GetVersion(SDK_VERSION *_ver);

int __stdcall NetClient_Logon(char* _cProxy,char* _cIP,char* _cUserName,
					char* _cPwd,char* _cProductID,int _iPort);

int __stdcall NetClient_LogonEx(char* _cProxy,char* _cIP,char* _cUserName,
							  char* _cPwd,char* _cProductID,int _iPort,char* _pcCharSet);

int __stdcall NetClient_Logoff(int _iLogonID);

int __stdcall NetClient_GetLogonStatus(int _iLogonID);

int __stdcall NetClient_StopRecv(unsigned int _uiRecvID);

unsigned int __stdcall NetClient_GetRecvID(int _iLogonID, int _iChannel, int _iStreamNO=MAIN_STREAM);

int __stdcall NetClient_GetInfoByConnectID(unsigned int _iConnectID,st_ConnectInfo* _stConnectInfo);  

int __stdcall NetClient_SetFullStreamNotify(unsigned int _uiRecvID,FULLFRAME_NOTIFY _cbkFullStream);  //����ԭʼ֡���ݻص�

int __stdcall NetClient_SetFullStreamNotify_V4(unsigned int _uiRecvID,FULLFRAME_NOTIFY_V4 _cbkFullStream, void* _iUserData);  //����ԭʼ֡���ݻص�

int __stdcall NetClient_GetCmdString(int _iLogonID,int _iType,int _iPara,CMDSTRING_NOTIFY _cbkCmdString,void* _pUserData = 0);

int __stdcall NetClient_GetDevInfo(int _iLogonID,ENCODERINFO *_pEncoderInfo);

int __stdcall NetClient_SendDataToServer(int _iLogonID,char *_cData,int _iLen);

int __stdcall NetClient_IsValidUser(int _iLogonID,char *_cUserName,char *_cPwd);

int __stdcall NetClient_SetInnerDataNotify(unsigned int _uiRecvID,INNER_DATA_NOTIFY _cbkNotify,void* _iUserData);

int __stdcall NetClient_SetWorkMode(int _iWorkMode);

int __stdcall NetClient_AddActiveServer(char *_cDevIP,char *_cFactoryID,int _iSocket, void *_pUserData);

int __stdcall NetClient_BindSocket(int _iLogonID,int _iChan,int _iSocket, void *_pUserData);

int __stdcall NetClient_PushData(int _iLogonID,int _iChan,char *_cData,int _iLen);

int __stdcall NetClient_DelActiveServer(int _iLogonID);

int __stdcall NetClient_StartCaptureData(unsigned long _ulID);

int __stdcall NetClient_StopCaptureData(unsigned long _ulID);

int __stdcall NetClient_GetVideoHeader(unsigned long _ulID,unsigned char *_ucHeader);

int __stdcall NetClient_SetRawFrameCallBack(unsigned int _ulConID, RAWFRAME_NOTIFY _cbkGetFrame, void* _pContext = 0);

int __stdcall NetClient_StartCaptureFile(unsigned int _uiRecvID, char* _pszFileName, int _iRecordFileType = 0);

int __stdcall NetClient_StopCaptureFile(unsigned int _uiRecvID);

int __stdcall NetClient_GetCaptureStatus(unsigned int	_ulConID);

int __stdcall NetClient_StartPlay(unsigned int _ulID, int _hWnd, RECT _rcShow, unsigned int _iDecflag = H264DEC_DECTWO);

int __stdcall NetClient_StopPlay(unsigned int _ulID);

int __stdcall NetClient_StopPlayEx(unsigned int _ulID, unsigned int _iParam);

int __stdcall NetClient_ResetPlayerWnd(unsigned int _ulID, int _hwnd);

int __stdcall NetClient_GetPlayingStatus(unsigned int _ulID);

int __stdcall NetClient_AdjustVideo(unsigned int _ulID, RECT _rctShow);

int __stdcall NetClient_AudioStart(unsigned int _ulID);

int __stdcall NetClient_AudioStop(unsigned int _ulID);

int __stdcall NetClient_SetLocalAudioVolume(int _iVolume);

int __stdcall NetClient_SetBufferNum(unsigned int _ulID, int _iNum);

int __stdcall NetClient_SetPlayDelay(unsigned long _ulID, int _iNum);

int __stdcall NetClient_GetChannelNum(int _iLogonID, int* _iChannelNum);

int __stdcall NetClient_GetAlarmPortNum(int _iLogonID, int* _iAlarmInPortNum, int* _iAlarmOutPortNum);

int __stdcall NetClient_GetLocalAlarmNum(int _iLogonID, int* _iLocalAlarmInNum, int* _iLocalAlarmOutNum);

int __stdcall NetClient_SetVideoPara(int _iLogonID, int _iChannelNum, STR_VideoParam* _strVideoParam);

int __stdcall NetClient_GetVideoPara(int _iLogonID, int _iChannelNum, STR_VideoParam* _strVideoParam);

int __stdcall NetClient_SetVideoparaSchedule(int _iLogonID, int _iChannelNum,STR_VideoParam* _strVideoParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_GetVideoparaSchedule(int _iLogonID, int _iChannelNum,STR_VideoParam* _strVideoParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_SetVideoQuality(int _iLogonID, int _iChannelNum, int _iVideoQuality, int _iStreamNO=LAN_VIDEO);

int __stdcall NetClient_GetVideoQuality(int _iLogonID,int _iChannelNum,int* _iVideoQuality, int _iStreamNO=LAN_VIDEO); 

int __stdcall NetClient_SetFrameRate(int _iLogonID, int _iChannelNum, int _iFrameRate, int _iStreamNO=LAN_VIDEO);

int __stdcall NetClient_GetFrameRate(int _iLogonID, int _iChannelNum, int* _iFrameRate, int _iStreamNO=LAN_VIDEO); 

int __stdcall NetClient_GetDecordFrameNum(unsigned int _ulConID); //ֹ��server�㣬δʵ��

int __stdcall NetClient_SetStreamType(int _iLogonID, int _iChannelNum, int _iStreamType, int _iStreamNO=LAN_VIDEO); 

int __stdcall NetClient_GetStreamType(int _iLogonID, int _iChannelNum, int* _iStreamType, int _iStreamNO=LAN_VIDEO);

int __stdcall NetClient_SetMotionAreaEnable (int _iLogonID,int _iChannelNum);

int __stdcall NetClient_SetMotionDetetionArea(int _iLogonID, int _iChannelNum, int _ix, int _iy,int _iEnabled);

int __stdcall NetClient_GetMotionDetetionArea(int _iLogonID, int _iChannelNum, int _ix, int _iy,int* _iEnabled);

int __stdcall NetClient_SetThreshold(int _iLogonID, int _iChannelNum,int _iThreshold);

int __stdcall NetClient_GetThreshold(int _iLogonID, int _iChannelNum,int* _iThreshold);

int __stdcall NetClient_SetMotionDetection(int _iLogonID, int _iChannelNum,int _iEnabled); 

int __stdcall NetClient_GetMotionDetection(int _iLogonID, int _iChannelNum,int* _iEnabled); 

int __stdcall NetClient_SetMotionDecLinkRec(int _iLogonID,int _iChannelNum,int _iEnable); 

int __stdcall NetClient_GetMotionDecLinkRec(int _iLogonID,int _iChannelNum,int* _iEnable);

int __stdcall NetClient_SetMotionDecLinkSnap(int _iLogonID,int _iChannelNum,int _iEnable);

int __stdcall NetClient_GetMotionDecLinkSnap(int _iLogonID,int _iChannelNum,int* _iEnable);

int __stdcall NetClient_SetMotionDecLinkSoundDisplay(int _iLogonID,int _iChannelNum,int _iSoundEnable,int _iDisplayEnable);

int __stdcall NetClient_GetMotionDecLinkSoundDisplay(int _iLogonID,int _iChannelNum,int *_iSoundEnable,int *_iDisplayEnable);

int __stdcall NetClient_SetMotionDecLinkOutport(int _iLogonID,int _iChannelNum,int _iOutportArray); 

int __stdcall NetClient_GetMotionDecLinkOutport(int _iLogonID,int _iChannelNum,int *_iOutportArray); 

int __stdcall NetClient_SetMotionDetectSchedule(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]); 

int __stdcall NetClient_GetMotionDetectSchedule(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]); 

int __stdcall NetClient_SetOsdDiaphaneity(int _iLogonID,int _iChannelNum,int _iDiaphaneity); 

int __stdcall NetClient_GetOsdDiaphaneity(int _iLogonID,int _iChannelNum,int *_iDiaphaneity); 

int __stdcall NetClient_SetOsdText(int _iLogonID, int _iChannelNum, unsigned char * _cTxtPtr, unsigned long _ulTextColor); 

int __stdcall NetClient_GetOsdText(int _iLogonID, int _iChannelNum, unsigned char* _cOSDText, unsigned long* _ulTextColor);

int __stdcall NetClient_SetOsdType(int _iLogonID, int _iChannelNum,int _iPositionX,int _iPositionY,int _iOSDType,int _iEnabled); 

int __stdcall NetClient_GetOsdType(int _iLogonID, int _iChannelNum, int _iOSDType, int* _iPositionX ,int* _iPositionY , int *_iEnabled); 

int __stdcall NetClient_SetDateFormat(int _iLogonID, int _iFormat, char _cSeparate);

int __stdcall NetClient_GetDateFormat(int _iLogonID, int *_iFormat, char *_cSeparate);

int __stdcall NetClient_SetOsdLOGO(int _iLogonID, int _iChannelNum, unsigned char* _cLogoFile, unsigned int _ulbkColor); // δʵ��

int __stdcall NetClient_SetAudioChannel(int _iLogonID, int _iChannelNum, int _iAudioChannel);

int __stdcall NetClient_GetAudioChannel(int _iLogonID, int _iChannelNum, int* _iAudioChannel);

int __stdcall NetClient_SetIpFilter(int _iLogonID, char* _cFilterIP,char* _cFilterMask);

int __stdcall NetClient_GetIpFilter(int _iLogonID, char* _cFilterIP,char* _cFilterMask);

int __stdcall NetClient_SetAlarmOutput(int _iLogonID, int _iAlarmInput, unsigned long _ulAlarmOutput);  

int __stdcall NetClient_GetAlarmOutput(int _iLogonID, int _iAlarmInput, unsigned long* _ulAlarmOutput);

int __stdcall NetClient_GetAlarmIPortState(int _iLogonID, int _iInPort,int *_iState);

int __stdcall NetClient_SetAlarmPortEnable(int _iLogonID,int _iInPort,int _iEnabled);

int __stdcall NetClient_GetAlarmPortEnable(int _iLogonID,int _iInPort,int *_iEnabled);

int __stdcall NetClient_SetInportAlarmLinkRec(int _iLogonID,int _iPortNo,int _iEnable);

int __stdcall NetClient_GetInportAlarmLinkRec(int _iLogonID,int _iPortNo,int* _iEnable); 

int __stdcall NetClient_SetInportAlarmLinkSnap(int _iLogonID,int _iPortNo,int _iEnable);

int __stdcall NetClient_GetInportAlarmLinkSnap(int _iLogonID,int _iPortNo,int* _iEnable);

int __stdcall NetClient_SetInportAlarmLinkPTZ(int _iLogonID,int _iPortNo,int _iLinkChannelNum,int _iLinkType,int _iActNum); 

int __stdcall NetClient_GetInportAlarmLinkPTZ(int _iLogonID,int _iPortNo,int _iLinkChannelNum,int *_iLinkType,int *_iActNum) ;

int __stdcall NetClient_SetInportAlarmLinkSoundDisplay(int _iLogonID,int _iPortNo,int _iSoundEnable,int _iDisplayEnable);

int __stdcall NetClient_GetInportAlarmLinkSoundDisplay(int _iLogonID,int _iPortNo,int *_iSoundEnable,int *_iDisplayEnable);

int __stdcall NetClient_SetInportAlmLinkOutport(int _iLogonID,int _iPortNo,int _iOutportArray);

int __stdcall NetClient_GetInportAlmLinkOutport(int _iLogonID,int _iPortNo,int *_iOutportArray);

int __stdcall NetClient_SetAlarmInMode(int _iLogonID, int _iPortNum, int _iLowOrHigh);

int __stdcall NetClient_GetAlarmInMode(int _iLogonID, int _iPortNum, int* _iLowOrHigh);

int __stdcall NetClient_SetAlarmOutMode(int _iLogonID, int _iPortNum, int _iLowOrHigh, int _iPulseWidth);

int __stdcall NetClient_GetAlarmOutMode(int _iLogonID, int _iPortNum, int* _iLowOrHigh, int* _iPulseWidth);

int __stdcall NetClient_SetInportAlarmSchedule(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_GetInportAlarmSchedule(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_SetOutportAlarmSchedule(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_GetOutportAlarmSchedule(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]); 

int __stdcall NetClient_SetOutportAlarmDelay(int _iLogonID,int _iPortNo,int _iClearType,int _iDelayTime);

int __stdcall NetClient_GetOutportAlarmDelay(int _iLogonID,int _iPortNo,int *_iClearType,int *_iDelayTime);

int __stdcall NetClient_SetInportEnable(int _iLogonID, int _iPortNo,int _iEnable);

int __stdcall NetClient_GetInportEnable(int _iLogonID, int _iPortNo,int *_iEnable); 

int __stdcall NetClient_SetOutportEnable(int _iLogonID, int _iPortNo,int _iEnable);

int __stdcall NetClient_GetOutportEnable(int _iLogonID, int _iPortNo,int* _iEnable);

int __stdcall NetClient_SetOutportState(int _iLogonID, int _iPort, int _iState);

int __stdcall NetClient_GetOutportState(int _iLogonID, int _iPort, int *_iState);

int __stdcall NetClient_SetAlmVdoCovThreshold(int _iLogonID, int _iChannelNum, int _iThreshold); 

int __stdcall NetClient_GetAlmVdoCovThreshold(int _iLogonID, int _iChannelNum, int* _iThreshold);

int __stdcall NetClient_SetAlmVideoCov(int _iLogonID, int _iChannelNum, int _iEnabled);

int __stdcall NetClient_GetAlmVideoCov(int _iLogonID, int _iChannelNum, int* _iEnabled); 

int __stdcall NetClient_SetDeviceType(int _iLogonID,int _iChannelNum,int _iComNo,int _iDevAddress,const char* _pcProtocol);

int __stdcall NetClient_GetDeviceType(int _iLogonID,int _iChannelNum,int* _iComPort,int* _iDevAddress ,char* _cDeviceType);

int __stdcall NetClient_SetComFormat(int _iLogonID,int _iComPort,char* _cDeviceType,char* _cComFormat,int _iWorkMode = 2 ); 

int __stdcall NetClient_GetComFormat(int _iLogonID, int _iCom, char* _cComFormat,int* _iWorkMode);

int __stdcall NetClient_GetAllSupportDeviceType(int _iLogonID, int* _iSumDeviceType, char* _cDeviceType);

int __stdcall NetClient_DeviceCtrl(int _iLogonID, int _iChannelNum,int _iActionType, int _iParam1,int _iParam2);

int __stdcall NetClient_DeviceCtrlEx(int _iLogonID,int _iChannelNum,int _iActionType,int _iParam1,int _iParam2,int _iControlType);

int __stdcall NetClient_ComSend(int _iLogonID, unsigned char* _ucBuf, int _iLength, int _iComNo);

int __stdcall NetClient_DevicePTZCtrl(int _iLogonID, int _iProtocolType, int _iActionType, int _iComNo, int _iAddress, int _iSpeed, int _iPresetNO);

int __stdcall NetClient_GetComPortCounts(int _iLogonID, int *_piComPortCounts, int *_piComPortEnabledStatus);

int __stdcall NetClient_SetAlarmVideoLost(int _iLogonID, int _iChannelNum,int _iEnabled);

int __stdcall NetClient_GetAlarmVideoLost(int _iLogonID, int _iChannelNum, int* _iEnabled); 

int __stdcall NetClient_SetVideoLostLinkPTZ(int _iLogonID,int _iChannelNum,int _iLinkChannelNum,int _iLinkType,int _iActNum);

int __stdcall NetClient_GetVideoLostLinkPTZ(int _iLogonID,int _iChannelNum,int _iLinkChannelNum,int *_iLinkType,int *_iActNum);

int __stdcall NetClient_SetVideoLostLinkSoundDisplay(int _iLogonID,int _iChannelNum,int _iSoundEnable,int _iDisplayEnable );

int __stdcall NetClient_GetVideoLostLinkSoundDisplay(int _iLogonID,int _iChannelNum,int *_iSoundEnable,int *_iDisplayEnable);

int __stdcall NetClient_GetAlarmVLostState(int _iLogonID, int _iChannel,int *_iState); 

int __stdcall NetClient_SetVideoLostLinkOutport(int _iLogonID,int _iChannelNum,int _iOutportArray);

int __stdcall NetClient_GetVideoLostLinkOutport(int _iLogonID,int _iChannelNum,int *_iOutportArray);

int __stdcall NetClient_SetVideoLostSchedule(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_GetVideoLostSchedule(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]); 

int __stdcall NetClient_SetVideoLostLinkRec(int _iLogonID,int _iChannelNum,int _iEnable);

int __stdcall NetClient_GetVideoLostLinkRec(int _iLogonID,int _iChannelNum,int* _iEnable); 

int __stdcall NetClient_SetVideoLostLinkSnap(int _iLogonID,int _iChannelNum,int _iEnable);

int __stdcall NetClient_GetVideoLostLinkSnap(int _iLogonID,int _iChannelNum,int* _iEnable);

int __stdcall NetClient_SetIFrameRate(int _iLogonID, int _iChannelNum, int _iFrameRate, int _iStreamNO=LAN_VIDEO);

int __stdcall NetClient_GetIFrameRate(int _iLogonID, int _iChannelNum, int* _iFrameRate, int _iStreamNO=LAN_VIDEO);

int __stdcall NetClient_ForceIFrame(int _iLogonID, int _iChannelNum, int _iStreamNO = LAN_VIDEO);

int __stdcall NetClient_SetTime(int _iLogonID, int _iyy, int _imo, int _idd, int _ihh, int _imi, int _iss);

int __stdcall NetClient_Reboot(int _iLogonID);

int __stdcall NetClient_RebootEx(int _iLogonID, int _iChannelNo);

int __stdcall NetClient_DefaultPara(int _iLogonID);

int __stdcall NetClient_DefaultParaEx(int _iLogonID, int _iType);

int __stdcall NetClient_GetServerVersion(int _iLogonID, char *_cVer);

int __stdcall NetClient_SetNVS(int _iLogonID, int _iVideo, int _iCheck);

int __stdcall NetClient_UpgradeProgram(int _iLogonID, char* _cFileName, PROUPGRADE_NOTIFY _UpgradeNotify); 

int __stdcall NetClient_UpgradeWebPage(int _iLogonID, char* _cFileName, WEBUPGRADE_NOTIFY _UpgradeWebNotify);

int __stdcall NetClient_GetUpgradePercent(int _iLogonID);

int __stdcall NetClient_GetUserNum(int _iLogonID,int* _iUserNum);

int __stdcall NetClient_GetUserInfo(int _iLogonID,int _iUserSerial,char *_cUserName,char* _cPassword,int* _iAuthority);

int __stdcall NetClient_GetCurUserInfo(int _iLogonID, char _cUserName[16], char _cPassword[16], int* _iAuthority);

int __stdcall NetClient_AddUser(int _iLogonID, char* _cUserName, char* _cPassword, int _iAuthority);

int __stdcall NetClient_DelUser(int _iLogonID, char* _cUserName);

int __stdcall NetClient_ModifyPwd(int _iLogonID, char* _cUserName, char* _cNewPwd);

int __stdcall NetClient_SetMaxConUser(int _iLogonID, int _iMaxConUser);

int __stdcall NetClient_GetMaxGetUser(int _iLogonID,int* _iMaxConUser);

int __stdcall NetClient_TalkStart(int _iLogonID, int _iUser = 0);

int __stdcall NetClient_TalkEnd(int _iLogonID);

int __stdcall NetClient_InputTalkingdata(unsigned char* _ucData, unsigned int	_iLen);

int __stdcall NetClient_GetTalkingState(int _iLogonID,int* _iTalkState);

int __stdcall NetClient_CapturePic(unsigned int _ulConID,unsigned char** ucData); 

int __stdcall NetClient_CaptureBmpPic(unsigned int _ulConID, char* _cFileName);

int __stdcall NetClient_ChangeSvrIP(int _iLogonID, char* _cNewSvrIP, char* _cMask, char* _cGateway, char* _cDNS); 

int __stdcall NetClient_GetIpProperty(int _iLogonID, char* _cMAC, char* _cMask, char* _cGateway, char* _cDNS);

int __stdcall NetClient_SetDHCPParam(int _iLogonID,int _iDHCP); 

int __stdcall NetClient_GetDHCPParam(int _iLogonID,int* _iDHCP);

int __stdcall NetClient_SetWifiDHCPParam(int _iLogonID,int _iDHCP); 

int __stdcall NetClient_GetWifiDHCPParam(int _iLogonID,int* _iDHCP);

int __stdcall NetClient_GetVideoCovArea(int _iLogonID, int _iChannelNum, RECT* _rect, int _iStreamNO = LAN_VIDEO);

int __stdcall NetClient_SetVideoCovArea(int _iLogonID, int _iChannelNum, RECT* _rect, int _iStreamNO = LAN_VIDEO);

int __stdcall NetClient_GetVideoSize(int _iLogonID, int _iChannelNum, int* _width, int * _height, int _iStreamNO=LAN_VIDEO);
 
int __stdcall NetClient_SetVideoSize(int _iLogonID, int _iChannelNum, int _iVideoSize, int _iStreamNO=LAN_VIDEO);
 
int __stdcall NetClient_GetVideoSizeEx(int _iLogonID, int _iChannelNum, int* _ivideoSize, int _iStreamNO=LAN_VIDEO);
 
int __stdcall NetClient_GetMaxMinorVideoSize(int _iLogonID, int* _iMinorVideoSize);

int __stdcall NetClient_BindInterface(int	_interface);

char* __stdcall NetClient_GetNetInterface(int	_interface);

int __stdcall NetClient_SetMaxKByteRate(int _iLogonID, int _iChannelNum, int _ibitRate, int _iStreamNO=LAN_VIDEO);

int __stdcall NetClient_GetMaxKByteRate(int _iLogonID, int _iChannelNum, int* _ibitRate, int _iStreamNO=LAN_VIDEO); 

int __stdcall NetClient_WriteUserData(int	_iLogonID,int _iOffset,unsigned char* _u8Buf,int _iLength);

int __stdcall NetClient_ReadUserData(int _iLogonID,int _iOffset,unsigned char* _u8Buf,int	_iLength);

int __stdcall NetClient_SetReducenoiseState(int _iLogonID, int _iChannelNum,int _iState);

int __stdcall NetClient_GetReducenoiseState(int _iLogonID,int _iChannelNum,int *_iState);

int __stdcall NetClient_DrawRectOnLocalVideo(unsigned int	_ulConID, RECT*	_rcRect, int _iCount);

int __stdcall NetClient_DrawTextOnVideo(int _iLogonID, int _iChannelNum, int _iX, int	_iY, char* _cText, int _iStore, int	_iStreamNO=MAIN_STREAM);

int __stdcall NetClient_GetTextOnVideo(int _iLogonID, int	_iChannelNum, int* _iX, int* _iY, char* _cText,int _iStreamNO);

int __stdcall NetClient_SetBothStreamSame(int _iLogonID, int _iChannelNum, int _iState);

int __stdcall NetClient_GetBothStreamSame(int _iLogonID, int _iChannelNum, int* _iState);

int __stdcall NetClient_ShowBitrateOnVideo(unsigned int _ulConID, int _iX, int _iY, int _iEnabled);

int __stdcall NetClient_SetPPPoEInfo(int _iLogonID,char* _cAccount, char* _cPassword, int	_iEnabled);

int __stdcall NetClient_GetPPPoEInfo(int _iLogonID,char* _cAccount, char* _cPassword, int *_iEnabled);

int __stdcall NetClient_CPUCheckEnabled(int _iEnabled, int _interval = 300);

int __stdcall NetClient_SetEncodeMode(int _iLogonID, int _iChannelNum, int _iStreamNO, int _iMode);

int __stdcall NetClient_GetEncodeMode(int _iLogonID, int _iChannelNum, int _iStreamNO, int* _iMode);

int __stdcall NetClient_SetPreferMode(int _iLogonID, int _iChannelNum, int _iStreamNO, int _iMode);

int __stdcall NetClient_GetPreferMode(int _iLogonID, int _iChannelNum, int _iStreamNO, int* _iMode);

int __stdcall NetClient_LogFileSetProperty(int _iLogonID, int _iLevel, int _iSize);

int __stdcall NetClient_LogFileGetProperty(int _iLogonID, int* _iLevel, int* _iSize);

int __stdcall NetClient_LogFileDownload(int _iLogonID); 

int __stdcall NetClient_LogFileClear(int _iLogonID); 

int __stdcall NetClient_LogFileGetDetails(int _iLogonID, char* _cBuf, int* _iLen); 

int __stdcall NetClient_GetVideoNPMode(int _iLogonID, VIDEO_NORM *_vMode);

int __stdcall NetClient_SetVideoNPMode(int _iLogonID, VIDEO_NORM _vMode);

int __stdcall NetClient_SetAudioEncoder(int _iLogonID, int _iChannel, AUDIO_ENCODER _aCoder);

int __stdcall NetClient_GetAudioEncoder(int _iLogonID, int _iChannel, AUDIO_ENCODER *_aCoder);

int __stdcall NetClient_NetFileQuery(int _iLogonID, PNVS_FILE_QUERY _fileQuery);

int __stdcall NetClient_NetFileSetRecordRule(int _iLogonID, int _iRule, int _iTimelen, int _iFreedisk ,int _iFileSize = 0 ); 

int __stdcall NetClient_NetFileGetRecordRule(int _iLogonID, int* _iRule, int *_iTimelen, int *_iFreedisk,int *_iFileSize);

int __stdcall NetClient_NetFileSetAlarmRule(int _iLogonID, int _iPreRecordEnable, int _iPreRecordTime, int _iDelayTime ,int _iDelayEnable ,int _iChannelNum = 0 );

int __stdcall NetClient_NetFileGetAlarmRule(int _iLogonID, int *_iPreEnable, int *_iPreTime, int *_iDelayTime ,int *_iDelayEnable,int _iChannelNum = 0 );

int __stdcall NetClient_NetFileSetAlarmState(int _iLogonID, int _iChannel, int _iState);

int __stdcall NetClient_NetFileGetAlarmState(int _iLogonID, int _iChannel, int *_iState);

int __stdcall NetClient_NetFileSetTaskState(int _iLogonID, int _iChannel, int _iState);

int __stdcall NetClient_NetFileGetTaskState(int _iLogonID, int _iChannel, int *_iState); 

int __stdcall NetClient_NetFileSetTaskSchedule(int _iLogonID, int _iChannel, int _iWeekday, NVS_SCHEDTIME *_Schedule[MAX_TIMESEGMENT]);

int __stdcall NetClient_NetFileGetTaskSchedule(int _iLogonID, int _iChannel, int _iWeekday, NVS_SCHEDTIME *_Schedule[MAX_TIMESEGMENT]); 

int __stdcall NetClient_NetFileSetTaskScheduleEx(int _iLogonID, int _iChannel, int _iWeekday, NVS_SCHEDTIME_Ex *_Schedule[MAX_TIMESEGMENT]);

int __stdcall NetClient_NetFileGetTaskScheduleEx(int _iLogonID, int _iChannel, int _iWeekday, NVS_SCHEDTIME_Ex *_Schedule[MAX_TIMESEGMENT]); 

int __stdcall NetClient_NetFileGetFileCount(int _iLogonID, int *_iTotalCount, int *_iCurrentCount); 

int __stdcall NetClient_NetFileRebuildIndexFile(int _iLogonID, int _iState);

int __stdcall NetClient_NetFileGetDiskInfo(int _iLogonID, PNVS_STORAGEDEV _storage);

int __stdcall NetClient_NetFileGetDiskInfoEx(int _iLogonID, PNVS_STORAGEDEV _storage, int _iSize);


int __stdcall NetClient_NetFileIsSupportStorage(int _iLogonID);

int __stdcall NetClient_NetFileDownloadFile(unsigned int *_ulID,
								  int _iLogonID, 
								  char* _cRemoteFilename, 
								  char* _cLocalFilename,
								  int	_iFlag = 0,
								  int	_iPosition = -1,
								  int	_Speed = 1); 

int __stdcall NetClient_NetFileDownloadFileEx(unsigned int* _ulConID,
									int _iLogonID, 
									char* _cRemoteFilename, 
									char* _cLocalFilename,
									int	_iFlag = 0,
									int	_iPosition = -1,
									int	_Speed = 1,
									NVSDATA_NOTIFY _cbkDataArrive = 0,
									void* _iUserData = 0);

int __stdcall NetClient_NetFileStopDownloadFile(unsigned int _ulID);

int __stdcall NetClient_NetFileGetDownloadPos(unsigned int _ulID, int *_iPos, int *_iDLSize);

int __stdcall NetClient_NetFileMountUSB(int _iLogonID, int _iState);

int __stdcall NetClient_NetFileGetRecordState(int _iLogonID, int _iChannel, RECORD_STATE *_iState);

int __stdcall NetClient_NetFileDelFile(int _iLogonID,const char *_pcFileName);

int __stdcall NetClient_DiskSetUsage(int _iLogonID,int _iDiskNo,int _iUsage);

int __stdcall NetClient_NetFileGetQueryfile(int _iLogonID, int _iFileIndex, PNVS_FILE_DATA _fileInfo); 

int __stdcall NetClient_DiskFormat(int _iLogonID,int _iDiskNo,int _iFsType); 

int __stdcall NetClient_DiskPart(int _iLogonID,int _iDiskNo,int _iPartNum,int _iFormatNow);

int __stdcall NetClient_NetFileManualRecord(int _iLogonID, int _iChannel, int _iState);

int __stdcall NetClient_NetFileMapStoreDevice(int _iLogonID,PNVS_NFS_DEV _storeDev);

int __stdcall NetClient_NetFileGetMapStoreDevice(int _iLogonID,PNVS_NFS_DEV _storeDev);

int __stdcall NetClient_NetFileGetUSBstate(int _iLogonID, int *_iState);

int __stdcall NetClient_NetFileSetExtendname(int _iLogonID, char *_cExtend);

int __stdcall NetClient_NetFileGetExtendname(int _iLogonID, char *_cExtend);

int __stdcall NetClient_ClearDisk(int	_iLogonID,int _iDiskNo);

int __stdcall NetClient_GetDownloadFailedFileName(int _iLogonID, int _iFileID, char *_pcFileName, int _iFileNameBufSize);

//3.51

//3.52
int __stdcall NetClient_SetMediaStreamClient(int _iLogonID, int _iChannel, char* _cClientIP, unsigned short _iClientPort, int _iStreamType);

int __stdcall NetClient_GetMediaStreamClient(int _iLogonID, int _iChannel, char* _cClientIP, unsigned short* _iClientPort, int* _iStreamType);

//3.53

int __stdcall NetClient_SetEmailAlarm(int _iLogonID, PSMTP_INFO _pSmtp); 

int __stdcall NetClient_GetEmailAlarm(int _iLogonID, PSMTP_INFO _pSmtp);

int __stdcall NetClient_SetEmailAlarmEnable(int _iLogonID, int _iChannel, int _iEnable); 

int __stdcall NetClient_GetEmailAlarmEnable(int _iLogonID, int _iChannel, int *_iEnable); 

//3.54
int __stdcall NetClient_SetScene(int _iLogonID, int _iChannel, int _iScene);

int __stdcall NetClient_GetScene(int _iLogonID, int _iChannel, int *_iScene);

int __stdcall NetClient_SetSensorFlip(int	_iLogonID, int _iChannel, int _iFlip);

int __stdcall NetClient_GetSensorFlip(int _iLogonID, int _iChannel, int *_iFlip);

int __stdcall NetClient_SetSensorMirror(int _iLogonID, int _iChannel, int _iMirror);

int __stdcall NetClient_GetSensorMirror(int _iLogonID, int _iChannel, int *_iMirror);

//3.55
int __stdcall NetClient_Snapshot(int _iLogonID, int _iChannel, int _iQvalue);

//3.56
int __stdcall NetClient_GetFactoryID(int _iLogonID, char* _cFactoryID);

//3.57
int __stdcall NetClient_SetWifiParam(int _iLogonID, NVS_WIFI_PARAM* _pWifiParam);

int __stdcall NetClient_GetWifiParam(int _iLogonID, NVS_WIFI_PARAM *_pWifiParam);

int __stdcall NetClient_WifiSearch(int _iLogonID);

int __stdcall NetClient_GetWifiSearchResult(int _iLogonID, WIFI_INFO** _pWifiInfo, int* _iWifiCount);

//3.58
int __stdcall NetClient_SetPrivacyProtect(int _iLogonID,int _iChannelNum,int _iEnabled);

int __stdcall NetClient_GetPrivacyProtect(int _iLogonID,int _iChannelNum,int* _iEnabled);

//3.59
int __stdcall NetClient_IYUVtoYV12(int _iWidth, int _iHeight, unsigned char* _YUV420);

//3.60
int __stdcall NetClient_GetDevType(int _iLogonID,int* _iDevType);

int __stdcall NetClient_GetProductType(int _iLogonID,int* _iType);

//3.61
int __stdcall NetClient_BackupKernel(int _iLogonID);

//3.62
int __stdcall NetClient_SetUPNPEnable(int	_iLogonID,int _iEnable);

int __stdcall NetClient_GetUPNPEnable(int _iLogonID,int *_iEnable);

//3.63
int __stdcall NetClient_GetSysInfo(int _iLogonID);

//3.64
int __stdcall NetClient_SetDDNSPara(int _iLogonID,char* _cDUserName,char* _cDPassword,char* _cDNvsName, char* _cDomain,int _iPort,int _iDEnable);

int __stdcall NetClient_GetDDNSPara(int _iLogonID,char* _cDUserName,char* _cDPassword,char* _cDNvsName, char* _cDomain,int* _iPort,int* _iDEnable);

//3.65
int __stdcall NetClient_SetFuncListArray(int _iLogonID,int _iEnableArray);

int __stdcall NetClient_GetFuncListArray(int _iLogonID,int *_iEnableArray);

//3.66
int __stdcall NetClient_SendStringToServer(int _iLogonID, char* _cMsg, int _iLen);

int __stdcall NetClient_ReceiveString(char* _cIpAddress,int* _iType,char* _cMsg,int* _iLen);

int __stdcall NetClient_SendStringToCenter(char* _cIpAddress, int _iServerPort,char* _cMsg, int _iLen);

//3.67
int __stdcall NetClient_SetVencType(int _iLogonID,int _iChannelNum,int _iType);

int __stdcall NetClient_GetVencType(int _iLogonID,int _iChannelNum,int *_iType);

//3.68
int __stdcall NetClient_SetComServer(int _iLogonID,char* _cComServer,int _iComServerPort);

int __stdcall NetClient_GetComServer(int _iLogonID,char* _cComServer,int* _iComServerPort);

//3.69
int __stdcall NetClient_Get3GDeviceStatus(int _iLogonID, int* _i3GDeviceType,int* _iStatus,int* _iIntensity,char* _pcIP,char* _pcStarttime);

int __stdcall NetClient_Set3GDialog(int _iLogonID, int _iStartType,int _iStopType,int _iDuration);

int __stdcall NetClient_Get3GDialog(int _iLogonID, int* _iStartType,int* _iStopType,int* _iDuration);

int __stdcall NetClient_Set3GMessage(int _iLogonID, char* _cNotify,char* _cPhoneNum1,char* _cPhoneNum2,char* _cPhoneNum3,char* _cPhoneNum4,char* _cPhoneNum5);

int __stdcall NetClient_Get3GMessage(int _iLogonID, char* _cNotify,char* _cPhoneNum1,char* _cPhoneNum2,char* _cPhoneNum3,char* _cPhoneNum4,char* _cPhoneNum5);

int __stdcall NetClient_Set3GTaskSchedule(int _iLogonID, int _iEnable, PNVS_SCHEDTIME _strScheduleParam);

int __stdcall NetClient_Get3GTaskSchedule(int _iLogonID, int* _iEnable, PNVS_SCHEDTIME _strScheduleParam);

int __stdcall NetClient_Set3GNotify(int _iLogonID, int _iType,char* _cMessage);

int __stdcall NetClient_Get3GNotify(int _iLogonID, int* _iType,char* _cMessage);

//3.70
int __stdcall NetClient_SetHDCamer(int _iLogonID, int _iChannel,int _iFuncNum,int _iValue);

int __stdcall NetClient_GetHDCamer(int _iLogonID, int _iChannel,int _iFuncNum,int* _iValue);

//3.71	
int __stdcall NetClient_SetAlarmServer(int _iLogonID,const char* _pcAlarmServer,int _iServerPort);

int __stdcall NetClient_GetAlarmServer(int _iLogonID,char* _pcAlarmServer,int* _iServerPort);


//==============================================================================================

int __stdcall NetClient_InterTalkStart(unsigned int * _uiConnID, int _iLogonID, int _iUserData );

int __stdcall NetClient_InterTalkEnd(unsigned int _uiConnID, bool _blStopTalk);

//==============================================================================================

int __stdcall NetClient_NetFileQueryEx(int _iLogonID, PNVS_FILE_QUERY _pfileQuery, int _iSizeOfQuery);

int __stdcall NetClient_ControlDeviceRecord(int _iLogonID, int _iChannelID, int _iRecordType, int _iAction);

//==============================================================================================

int __stdcall NetClient_NetFileDownloadByTimeSpan(unsigned int*	_ulConID, int _iLogonID, char* _pcLocalFile, 
					int _iChannelNO, unsigned int _uiFromSecond, unsigned int _uiToSecond, 
                                                  int _iFlag = 0, int _iPosition = -1,int _iSpeed = 1);

int __stdcall NetClient_NetFileDownloadByTimeSpanEx( unsigned int* _ulConID, int _iLogonID, char* _pcLocalFile, 
					int _iChannelNO, NVS_FILE_TIME *_pTimeBegin, NVS_FILE_TIME *_pTimeEnd, 
                                                     int _iFlag = 0, int _iPosition = -1,int _iSpeed = 1);

int __stdcall NetClient_NetFileDownloadByTimeSpanCallBack( unsigned int* _ulConID, 
                                                 int _iLogonID, 
												 char* _pcLocalFile,
												 int _iChannelNO,
                                                 NVS_FILE_TIME *_pTimeBegin, 
                                                 NVS_FILE_TIME *_pTimeEnd, 
												 NVSDATA_NOTIFY _cbkDataArrive,
                                                 int _iFlag = 0, 
                                                 int _iPosition = -1,
                                                 int _iSpeed = 1,
                                                 void* _iUserData = 0);                     // ʱ��������ļ������������ص����� add by lyh 2010.9.1

int __stdcall NetClient_NetLogQuery(int _iLogonID, PNVS_LOG_QUERY _logQuery);

int __stdcall NetClient_NetLogGetLogfile(int _iLogonID, int _iLogIndex, PNVS_LOG_DATA _pLogInfo);

int __stdcall NetClient_NetLogGetLogCount(int _iLogonID, int *_iTotalCount, int *_iCurrentCount);

int __stdcall NetClient_GetProtocolList(int _iLogonID, st_NVSProtocol* _pstNVSProtocol);

//===============================================================================================
int __stdcall NetClient_SetCHNPTZCRUISE(int _iLogonID,int _iChannelNum,int _iCruiseNo,int _iEnable,int _iCruiseNum,st_PTZCruise* _stPTZCruise);

int __stdcall NetClient_GetCHNPTZCRUISE(int _iLogonID,int _iChannelNum,int _iCruiseNo,int* _iEnable,int* _iCruiseNum,st_PTZCruise* _stPTZCruise);

int __stdcall NetClient_SetVIDEOCOVER_LINKRECORD(int _iLogonID,int _iChannelNum,int _iEnableByBits);

int __stdcall NetClient_GetVIDEOCOVER_LINKRECORD(int _iLogonID,int _iChannelNum,int* _iEnableByBits);

int __stdcall NetClient_SetVIDEOCOVER_LINKPTZ(int _iLogonID,int _iChannelNum,int _iLinkChannel,int _iLinkType,int _iNo);

int __stdcall NetClient_GetVIDEOCOVER_LINKPTZ(int _iLogonID,int _iChannelNum,int _iLinkChannel,int* _iLinkType,int* _iNo);

int __stdcall NetClient_GetAlarmVCoverState(int _iLogonID, int _iChannel,int *_iState);

int __stdcall NetClient_StopCaptureDate(unsigned long _ulID);

/*
 * ------------------------------------------------------------------------------------------------------------------ 
 * add by lyh, for 3520 
 */



int __stdcall NetClient_SetColorToGray(int _iLogonID,int _iChannelNum, int _iEnable);

int __stdcall NetClient_GetColorToGray(int _iLogonID,int _iChannelNum, int* _iEnable);

int __stdcall NetClient_SetCustomChannelName(int _iLogonID,int _iChannelNum,int _iChannelType, char* _cChannelName);

int __stdcall NetClient_GetCustomChannelName(int _iLogonID,int _iChannelNum,int _iChannelType, char* _cChannelName);

int __stdcall NetClient_SetCustomRecType(int _iLogonID,int _iRecType, char* _cRecTypeName);

int __stdcall NetClient_GetCustomRecType(int _iLogonID,int _iRecType, char* _cRecTypeName);

int __stdcall NetClient_ChangeSvrIPEx(int _iLogonID, char* _cNewSvrIP, char* _cMask, char* _cGateway, char* _cDNS, char* _cBackDNS);

int __stdcall NetClient_GetIpPropertyEx(int _iLogonID,char* _cMAC, char* _cMask, char* _cGateway, char* _cDNS, char* _cBackDNS);

int __stdcall NetClient_SetFTPUpdate(int _iLogonID,char* _cFtpAddr,char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword);

int __stdcall NetClient_GetFTPUpdate(int _iLogonID,char* _cFtpAddr,char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword);

int __stdcall NetClient_SetCHNPTZFormat(int _iLogonID,int _iChannelNum,char* _cComFormat);

int __stdcall NetClient_GetCHNPTZFormat(int _iLogonID,int _iChannelNum,char* _cComFormat);

int __stdcall NetClient_GetServerVersionEx(int _iLogonID, char *_cVer,char *_cUIVersion,char* _cSlaveVersion);

int __stdcall NetClient_GetOSDTypeColor(int _iLogonID, int _iChannelNum, int _iOSDType, int* _iColor);

int __stdcall NetClient_SetOSDTypeColor(int _iLogonID, int _iChannelNum, int _iOSDType, int _iColor);

int __stdcall NetClient_GetExceptionMsg(int _iLogonID, int _iExceptionTyep, int* _iState);

int __stdcall NetClient_SetNTPInfo(int _iLogonID, char* _NTPServer, unsigned short _iPort, int _iInterval);

int __stdcall NetClient_GetNTPInfo(int _iLogonID, char* _NTPServer, unsigned short *_iPort, int *_iInterval);

//20101202  -->hyq
//-----------------------------------------
/************************************************************************
*	Function:	Control CD-Burn (start or stop)
*	Parameters:	
		_iLogonID				LogonID (from __stdcall NetClient_Logon)
		_iChannel				Channel ID, could be set -1 if it will not be used any way
		_iCmd					CD-Burn command
		_lpCmdBuf				buffer to control
		_iBufSize				buffer size
************************************************************************/
int __stdcall NetClient_SetCDBurnConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_GetCDBurnConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);

/************************************************************************
*	Function:	Video Encrypt
*	Parameters:	
		_iLogonID				LogonID (from __stdcall NetClient_Logon)
		_iChannel				Channel ID, could be set -1 if it will not be used any way
		_lpCmdBuf				buffer of video encrypt
		_iBufSize				buffer size                                                                      
************************************************************************/
int __stdcall NetClient_SetVideoEncrypt(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_GetVideoEncrypt(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_SetVideoDecrypt(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_GetVideoDecrypt(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);

/************************************************************************
*	Funtion: ���ö໭��ϳ�ʹ��                                                                     
************************************************************************/
int __stdcall NetClient_SetPreRecEnable(int _iLogonID, int _iChannel, int _iEnable);
int __stdcall NetClient_GetPreRecEnable(int _iLogonID, int _iChannel, int *_piEnable);
//ʵ�ֶ໭��ϳɹ���
int __stdcall NetClient_SetVideoCombine(int _iLogonID, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetVideoCombine(int _iLogonID, void* _lpBuf, int _iBufSize);

//VCA  20110111-->hyq
int __stdcall NetClient_VCASetConfig(int _iLogonID, int _iVCACmdID, int _iChannel, void* _lpCmdBuf, int _iCmdBufLen);

int __stdcall NetClient_VCAGetConfig(int _iLogonID, int _iVCACmdID, int _iChannel, void* _lpCmdBuf, int _iCmdBufLen);

int __stdcall NetClient_VCARestart(int _iLogonID);

int __stdcall NetClient_VCARestartEx(int _iLogonID, int _iChannelNO);

int __stdcall NetClient_VCAGetAlarmInfo(int _iLogonID, int _iAlarmIndex, void* _lpBuf, int _iBufSize);

//Email Alarm ��չЭ��
int __stdcall NetClient_SetEmailAlarmEx(int _iLogonID, PSMTP_INFO _pSmtp, int _iSize);
int __stdcall NetClient_GetEmailAlarmEx(int	_iLogonID, PSMTP_INFO _pSmtp, int _iSize);
//FTP �Զ��ϴ�
int __stdcall NetClient_SetFTPUploadConfig(int _iLogonID, int _iCmd, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_GetFTPUploadConfig(int _iLogonID, int _iCmd, void* _lpCmdBuf, int _iBufSize);

//����3G DVR
/************************************************************************
*	Function:	Set 3G DVR configuration, such as POWERDELAY, SIMNUM, GPSINFO, GPSFILTER
*	Parameters:
		_iLogonID				LogonID (from __stdcall NetClient_Logon)
		_iCmd					3G CmdID
		_iChannel				Channel ID, could be set -1 if it will not be used any way
		_lpCmdBuf				buffer to get actual setting, should be &T3GDVRInfo
		_iBufSize				buffer size, should be sizeof(T3GDVRInfo)
************************************************************************/
int __stdcall NetClient_Set3GConfig(int _iLogonID, int _iCmd, int _iChannel, void* _lpValueBuf, int _iBufSize);
int __stdcall NetClient_Get3GConfig(int _iLogonID, int _iCmd, int _iChannel, void* _lpValueBuf, int _iBufSize);

/************************************************************************
*	����ͨ���������������ȡ                                                                     
************************************************************************/
int __stdcall NetClient_SetDigitalChannelConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_GetDigitalChannelConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);

/************************************************************************
*	����NVRǰ�˵�͸��ͨ��������NetClient_ComSend                                                                     
************************************************************************/
int __stdcall NetClient_DigitalChannelSend(int _iLogonID, int _iChannel, unsigned char* _ucBuf, int _iLength);

/************************************************************************
*	����ͨ������VideoMode (N/P)                                                                     
************************************************************************/
int __stdcall NetClient_SetVideoNPModeEx(int _iLogonID, int _iChannel, VIDEO_NORM _iNPMode);
int __stdcall NetClient_GetVideoNPModeEx(int _iLogonID, int _iChannel, VIDEO_NORM *_piNPMode);

/************************************************************************
*	�õ�����ͨ���ĸ������Լ��õ�ÿ��ͨ�������ԣ�����ͨ���򱾵�ͨ����                                               
************************************************************************/
int __stdcall NetClient_GetDigitalChannelNum(int _iLogonID, int *_piDigitChannelNum);
/***************************************************************
Function     ��	NetClient_SetChannelProperty
Description  ��	����ͨ������
Prameter(in) ��		_iLogonID��	
                    	��¼ID
					_iChannel��	
						ͨ����, ȡֵ��Χ��[0, ͨ����-1],ͨ������NetClient_GetChannelNum�����������
					_iCmd��
 						����ID
						GENERAL_CMD_CHANNEL_TYPE	    �õ����豸��ĳ��ͨ��������
					_lpBuf: 		
						������ָ��
                        ��Ӧ_iCmd ��GENERAL_CMD_CHANNEL_TYPEΪint�ͣ�
						ͨ�����ͣ� 1������ģ��ͨ����  2������ͨ����3���ϳ�ͨ��
                    _iBufSize��
					    ��������С������_iCmd����
return         ��    0����ȷ
                    -1��ʧ��
                    other������
others         ��	2013-04-10 
***************************************************************/
int __stdcall  NetClient_SetChannelProperty( int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize );
/***************************************************************
Function     ��	NetClient_GetChannelProperty
Description  ��	���ͨ������
Prameter(in) ��		_iLogonID��	
                    	��¼ID
					_iChannel��	
						ͨ����, ȡֵ��Χ��[0, ͨ����-1],ͨ������NetClient_GetChannelNum�����������
					_iCmd��
 						����ID
						GENERAL_CMD_GET_CHANNEL_TYPE	   �õ����豸��ĳ��ͨ��������(��ǰ��)
						GENERAL_CMD_CHANNEL_TYPE           �õ����豸��ĳ��ͨ��������(�ĺ��)
					_iBufSize��
					    ��������С������_iCmd����
Prameter(out): 		_lpBuf: 		
                        ������ָ��
                        ��Ӧ_iCmd ��GENERAL_CMD_GET_CHANNEL_TYPEΪint*�ͣ�
						��Ӧ_iCmd ��GENERAL_CMD_CHANNEL_TYPEΪint*�ͣ�
						ͨ�����ͣ� 1������ģ��ͨ����2������ͨ����3���ϳ�ͨ��
return         ��    0����ȷ
                    -1��ʧ��
                    other������
others         ��	2013-04-10 
***************************************************************/
int __stdcall NetClient_GetChannelProperty(int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize);

int	__stdcall NetClient_SetDeviceTimerReboot(int _iLogonID, int _iEnable, int _iInterval, int _iHour);
int	__stdcall NetClient_GetDeviceTimerReboot(int _iLogonID, int *_iEnable, int *_iInterval, int *_iHour);
int __stdcall NetClient_SetVideoCoverSchedule(int _iLogonID, int _iChannel, int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
int __stdcall NetClient_GetVideoCoverSchedule(int _iLogonID, int _iChannel, int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
int	__stdcall NetClient_SetCPUMEMAlarmThreshold(int _iLogonID, int _iCPUThreshold, int _iMEMThreshold);
int	__stdcall NetClient_GetCPUMEMAlarmThreshold(int _iLogonID, int *_iCPUThreshold, int *_iMEMThreshold);
int	__stdcall NetClient_SetDZInfo(int _iLogonID, DZ_INFO_PARAM *_pDZ_INFO);
int	__stdcall NetClient_GetDZInfo(int _iLogonID, DZ_INFO_PARAM *_pDZ_INFO);
int	__stdcall NetClient_SetPTZAutoBack(int _iLogonID, int _iChannel,int _iEnable,int _iPresetIndex,int _iIdleTime);
int	__stdcall NetClient_GetPTZAutoBack(int _iLogonID, int _iChannel,int *_iEnable,int *_iPresetIndex,int *_iIdleTime);
int	__stdcall NetClient_Set3GVPND(int _iLogonID, char* _cDialNumber,char* _cAccount,char* _cPassword);
int __stdcall NetClient_Get3GVPND(int _iLogonID, char* _cDialNumber,char* _cAccount,char* _cPassword);
//	HDCamera extend interface
int	__stdcall NetClient_SetHDCamerEx(int _iLogonID, int _iChannel, int _iFuncID, void* _lpBuf, int _iSize);
int	__stdcall NetClient_GetHDCamerEx(int _iLogonID, int _iChannel, int _iFuncID, void* _lpBuf, int _iSize);

//add 20110310-->hyq
int __stdcall NetClient_SetFTPUsage(int _iLogonID, char* _cFtpAddr, int _iPort, char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword,int _iUsage);
int __stdcall NetClient_GetFTPUsage(int _iLogonID, char* _cFtpAddr, int* _piPort, char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword,int* _piUsage);

//sip add by cmz 20110303
int __stdcall NetClient_SetChannelSipConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetChannelSipConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize);

//MaxVideoSize add by cmz 20110305
int __stdcall NetClient_GetMaxVideoSize(int _iLogonID, int* _iVideoSize); 

//BitRatePercent add by cmz 20110308
int __stdcall NetClient_SetBitRatePercent(int _iLogonID, int _iChannelNO, int _iPercent);
int __stdcall NetClient_GetBitRatePercent(int _iLogonID, int _iChannelNO, int* _piPercent);

//GetVideoParam add by cmz 20110308
int __stdcall NetClient_GetVideoParam(unsigned int _uiConnID, int *_piWidth, int *_piHeight, int *_piFrameRate);
//OSDAlpha
int __stdcall NetClient_SetOSDAlpha(int _iLogonID, int _iChannel,int _iAlpha);
int __stdcall NetClient_GetOSDAlpha(int _iLogonID, int _iChannel,int *_iAlpha);
//DeviceSetup add by cmz 20110308
int __stdcall NetClient_DeviceSetup(int _iLogonID, int _iFlag,const char *_pcSection,const char* _pcKeyword,const char* _pcValue);

int __stdcall NetClient_SetPlayerShowFrameMode(int _iLogonID,int _iChannelNum, unsigned int _uiShowFrameMode, int _iStreamNO);

int __stdcall NetClient_GetPlayerShowFrameMode(int _iLogonID,int _iChannelNum,int _iStreamNO);

int __stdcall NetClient_DrawRectOnLocalVideo(unsigned int _uiConID, RECT* _rcRect, int _iCount);

int __stdcall NetClient_DrawPolyOnLocalVideo(unsigned int _uiConnID, POINT* _pPointArray, int _iPointCount, int _iFlag);

int __stdcall NetClient_SendStringToServerEx(int _iLogonID,char* _cMsg,int _iLen, int _iFlag);

int __stdcall NetClient_SetNetFileDownloadFileCallBack(unsigned int _ulConID, RECV_DOWNLOADDATA_NOTIFY _cbkDataNotify, void* _lpUserData = 0);

int __stdcall NetClient_SetDataPackCallBack(unsigned int _ulConID, int _iCBType, void* _pvCallBack, void* _pvUserData);

int __stdcall NetClient_AddConnectionToNetWork(int _iSocket, void *_Connection, void *_NotifyFun);
int __stdcall NetClient_MallocConnection();
int __stdcall NetClient_FreeConnection(void *_pConnect);

/***************************************************************
Function     �� NetClient_NetFileSetChannelParam
Description  ��	����ͨ����ش洢����							
Prameter(in) ��	_iLogonID����½ID
				_iChannelNo: ͨ����
				_iCmd:ͨ����ش洢����
				_lpBuf: �洢���Զ�Ӧ�ṹ��
return       ��	0:	success
<0:	failed
others       ��	2012-06-27 16:20:00
***************************************************************/
int __stdcall NetClient_NetFileSetChannelParam(int _iLogonID,int _iChannelNo,int _iCmd, void* _lpBuf);

/***************************************************************
Function     �� NetClient_NetFileGetChannelParam
Description  ��	��ȡͨ����ش洢����							
Prameter(in) ��	_iLogonID����½ID
				_iChannelNo: ͨ����
				_iCmd:ͨ����ش洢����
Prameter(in out): _lpBuf: �洢���Զ�Ӧ�ṹ��
return       ��	0:	success
<0:	failed
others       ��	2012-06-27 16:20:00
***************************************************************/
int __stdcall NetClient_NetFileGetChannelParam(int _iLogonID,int _iChannelNo,int _iCmd, void* _lpBuf);

/***************************************************************
Function     ��NetClient_ShutDownDev
Description  ���ػ�
Prameter(in) ��_iLogonID     ��¼ID
               _iFlag        �ػ���־
Prameter(out)��NULL
return       ��0��    success
               other��fail
others       ��2012-02-24 11:28:20
***************************************************************/
int __stdcall NetClient_ShutDownDev(int _iLogonID, int _iFlag);

/*
���ܣ����ݵ�ǰӦ�ó�����Դ�ļ� �����豸�Ĺ̼�����������
*/
int __stdcall NetClient_BackupImage(int _iLogonID , int _iType);


/***************************************************************
Function     ��NetClient_SetLanParam
Description  ��������������
Prameter(in) ��_iLogonID	 ��¼ID
Prameter(in)��_iCmd			 ������ѡ��
Prameter(in)��_lpData		 ����ָ��
return       ��0�� success
other��fail
others       ��2012-02-08 
***************************************************************/
int __stdcall NetClient_SetLanParam(int _iLogonID, int _iCmd, void* _lpData);

/***************************************************************
Function     ��NetClient_GetLanParam
Description  �������������
Prameter(in) ��_iLogonID	 ��¼ID
Prameter(in)��_iCmd			 ��ȡ��ѡ��
Prameter(in out)��_lpData		 ����ָ��
return       ��0�� success
other��fail
others       ��2012-02-08 
***************************************************************/
int __stdcall NetClient_GetLanParam(int _iLogonID, int _iCmd, void* _lpData);

/************************************************************************/
//* ����ĳͨ��֧�ֵ���Ƶ��С�б�ÿ��ͨ����Ҫ���͸�Э��һ�Σ�                                                                     
//************************************************************************/
/***************************************************************
Function     ��		NetClient_GetVideoSzList
Description  ��		����ĳͨ��֧�ֵ���Ƶ��С�б�
Prameter(in) ��		_iLogonID����¼ID
					_iChannelNo��ͨ����
					_iStreamNo: ������
Prameter(out)��		_piList����Ӧ�ķֱ����б�
Prameter(in, out)�� _piLstCount ���б�Ԫ�ظ���-------------�������Σ�Ҳ����Σ������ʱ��ʾ��������С              
return       ��     0�� success
                    -1������������
                    other��fail
others       ��2012-02-01 
***************************************************************/
int __stdcall NetClient_GetVideoSzList( int _iLogonID, int _iChannel, int _iStreamNo, int* _piList, int*  _piLstCount );

//add
/***************************************************************
Function     ��	NetClient_SetAlarmConfig
Description  ��	ͨ�ñ������ýӿ�
Prameter(in) ��	_iLogonID����½ID
				_iAlarmType: ��������
				_iCmdLen:	��������
Prameter(in, out)��	_pvParam: ����ģ������Ļ���ָ�룬�ο�NetClientTypes.h
return       ��	0�� success
				<0:	failed
others       ��2012-2-24 9:48:39
***************************************************************/
int  __stdcall NetClient_SetAlarmConfig( int _iLogonID, int _iChannel, int _iAlarmType, int _iCmd, void* _pvCmdBuf);  //cb@120415

/***************************************************************
Function     �� NetClient_GetAlarmConfig
Description  ��	ͨ�ñ�����ȡ�ӿ�
Prameter(in) ��	_iLogonID����½ID
				_iAlarmType: ��������0
				_iCmdLen:	��������
Prameter(in, out)��	_pvParam: ����ģ��������ο�NetClientTypes.h
return       ��	0:	success
				<0:	failed
others       ��	2012-2-24 9:51:53
***************************************************************/
int  __stdcall NetClient_GetAlarmConfig(int _iLogonID, int _iChannel, int _iAlarmType, int _iCmd, void* _pvCmdBuf);


//CAUTION��ITS
/************************************************************************
*	��������������                                                            
************************************************************************/
/***************************************************************
Function     ��	NetClient_SetITSBlock
Description  ��	�����������������ýӿ�
Prameter(in) ��	_iLogonID����½ID
				_iBlockID: �����
Prameter(in, out)��	_iX:	X����
					_iY:    Y����
return       ��	0�� success
<0:	failed
others       ��2012-10-18����˵��
***************************************************************/
int __stdcall NetClient_SetITSBlock(int _iLogonID, int _iBlockID, int _iX, int _iY);

/***************************************************************
Function     ��	NetClient_GetITSBlock
Description  ��	���������������ȡ�ӿ�
Prameter(in) ��		_iLogonID����½ID
					_iBlockID: �����
Prameter(in, out)��	_iX:	X����
					_iY:    Y����
return       ��	0�� success
<0:	failed
others       ��2012-10-18����˵��
***************************************************************/
int __stdcall NetClient_GetITSBlock(int _iLogonID, int _iBlockID, int* _iX, int* _iY);

/************************************************************************
*	���ʱ���ͨ�ò�������                                                                
************************************************************************/
/***************************************************************
Function     ��	NetClient_SetHDTimeRangeParam
Description  ��	���ʱ���ͨ�ò������ýӿ�
Prameter(in) ��	_iLogonID����½ID
				_iChannel: ͨ����
				_iIndex��ʱ��� 0-7
                _iBufSize���ṹ���С
Prameter(in, out)��	_lpBuf�� �洢�����ṹ��ָ��			
return       ��	0�� success
<0:	failed
others       ��2012-10-18����˵��
***************************************************************/
int __stdcall NetClient_SetHDTimeRangeParam(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ��	NetClient_GetHDTimeRangeParam
Description  ��	���ʱ���ͨ�ò�����ȡ�ӿ�
Prameter(in) ��		_iLogonID����½ID
					_iChannel: ͨ����
					_iIndex��ʱ��� 0-7
					_iBufSize���ṹ���С
Prameter(in, out)��	_lpBuf�� �洢�����ṹ��ָ��
return       ��	0�� success
				<0:	failed
others       ��2012-10-18����˵��
***************************************************************/
int __stdcall NetClient_GetHDTimeRangeParam(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);

/************************************************************************
*	�����������ģ������                                                                
************************************************************************/
/***************************************************************
Function     ��	NetClient_SetHDTemplateName
Description  ��	���������ģ���������ýӿ�
Prameter(in) ��	_iLogonID����½ID
				_iTemplateID: ģ���� 0-7��֧�����8��ģ�塣
Prameter(in, out)��	_cTemplateName�� ģ�����ƣ����32���ַ�			
return       ��	0�� success
<0:	failed
others       ��2012-10-20����˵��
***************************************************************/
int __stdcall NetClient_SetHDTemplateName(int _iLogonID, int _iTemplateID, char* _cTemplateName);

/***************************************************************
Function     ��	NetClient_GetHDTemplateName
Description  ��	���������ģ�����ƻ�ȡ�ӿ�
Prameter(in) ��	_iLogonID����½ID
				_iTemplateID: ģ����0-7��֧�����8��ģ�塣
Prameter(in, out)��	_cTemplateName�� ģ�����ƣ����32���ַ�			
return       ��	0�� success
<0:	failed
others       ��2012-10-20����˵��
***************************************************************/
int __stdcall NetClient_GetHDTemplateName(int _iLogonID, int _iTemplateID,char* _cTemplateName);


/************************************************************************
*	�����������ʱ���ģ��ӳ��                                                                
************************************************************************/
/***************************************************************
Function     ��NetClient_GetHDTemplateMap
Description  ��ʱ�������������ģ��ӳ���ϵ��ȡ�ӿ�
Prameter(in) ��	_iLogonID����½id
				_iChannel��ͨ����
				_iBufSize���ṹ���С
Prameter(out)��_lpBuf��ʱ�����ģ��ӳ��
return       ��0��success
               other:failed
others       ��2012-10-21Add@cb
***************************************************************/
int __stdcall NetClient_SetHDTemplateMap(int _iLogonID, int _iChannel, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_GetHDTemplateMap
Description  ��ʱ�������������ģ��ӳ���ϵ��ȡ�ӿ�
Prameter(in) ��_iLogonID����½id
			   _iChannel��ͨ����
			   _iBufSize���ṹ���С
Prameter(out)��_lpBuf��ʱ�����ģ��ӳ��
return       ��0��success
               other:failed
others       ��2012-10-21Add@cb
***************************************************************/
int __stdcall NetClient_GetHDTemplateMap(int _iLogonID, int _iChannel, void* _lpBuf, int _iBufSize);

/************************************************************************
*	���ʱ�������ʹ��                                                                
************************************************************************/
/***************************************************************
Function     ��NetClient_SetITSTimeRange
Description  �����ʱ������ýӿ�
Prameter(in) ��	_iLogonID����½id
				_iChannel��ͨ����
				_iIndex��ʱ��α��
				_iBufSize���ṹ���С
				Prameter(out)��
				_lpBuf�����ʱ��νṹ��
return       ��0��success
               other:failed
others       ��2012-10-24Add@cb
***************************************************************/
int __stdcall NetClient_SetITSTimeRangeEnable(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_SetITSTimeRange
Description  �����ʱ��λ�ȡ�ӿ�
Prameter(in) ��	_iLogonID����½id
				_iChannel��ͨ����
				_iIndex��ʱ��α��
				_iBufSize���ṹ���С
				Prameter(out)��
				_lpBuf�����ʱ��νṹ��
return       ��0��success
               other:failed
others       ��2012-10-24Add@cb
***************************************************************/
int __stdcall NetClient_GetITSTimeRangeEnable(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);

/************************************************************************
*	���ʱ�������                                                                
************************************************************************/
/***************************************************************
Function     ��NetClient_SetITSTimeRange
Description  ��ʱ��β������ʹ�ܺ�ʱ��������ʹ�ܻ�ȡ�ӿ����ýӿ�
Prameter(in) ��	_iLogonID����½id
				_iIndex��ʱ��α��
				_iCmd������ţ��ڲ�����ʵ�ַ���
				_iBufSize���ṹ���С
				Prameter(out)��
				_lpBuf��
				Ϊʱ��β������ʹ��ʱ��
				bit0��ʹ�ò������1
				bit1��ʹ�ò������2
				bit2��ʹ�ò������3
				bit3��ʹ�ò������4
				bit4��ʹ�ò������5
				Ϊʱ��������ʹ�ܻ�ȡ�ӿ�ʱ��
                ȡ�Ƿ�ʹ�ܣ�
return       ��0��success
               other:failed
others       ��2012-10-24Add@cb
***************************************************************/
int __stdcall NetClient_SetITSTimeRange(int _iLogonID, int _iIndex, int _iCmd, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_GetITSTimeRange
Description  ��ʱ��β������ʹ�ܺ�ʱ��������ʹ�ܻ�ȡ�ӿ�
Prameter(in) ��	_iLogonID����½id
				_iIndex��ʱ��α��
				_iCmd������ţ��ڲ�����ʵ�ַ���
				_iBufSize���ṹ���С
				Prameter(out)��
				_lpBuf��
				Ϊʱ��β������ʹ��ʱ��
				bit0��ʹ�ò������1
				bit1��ʹ�ò������2
				bit2��ʹ�ò������3
				bit3��ʹ�ò������4
				bit4��ʹ�ò������5
				Ϊʱ��������ʹ�ܻ�ȡ�ӿ�ʱ��
				ȡ�Ƿ�ʹ�ܣ�
return       ��0��success
               other:failed
others       ��2012-10-24Add@cb
***************************************************************/
int __stdcall NetClient_GetITSTimeRange(int _iLogonID, int _iIndex, int _iCmd, void* _lpBuf, int _iBufSize);

/************************************************************************
*	������ģʽ                                                             
************************************************************************/
/***************************************************************
Function     ��NetClient_SetITSTimeRange
Description  ������������ģʽ���ýӿ�
Prameter(in) ��	_iLogonID����½id
				Prameter(out)��
				_iMode ��  ����������ģʽ
return       ��0��success
               other:failed
others       ��2012-10-25Add@cb
***************************************************************/
int __stdcall NetClient_SetITSDetectMode(int _iLogonID, int _iMode);

/***************************************************************
Function     ��NetClient_GetITSDetectMode
Description  ������������ģʽ���ýӿ�
Prameter(in) ��	_iLogonID����½id
				Prameter(out)��
				_iMode ��  ����������ģʽ
return       ��0��success
               other:failed
others       ��2012-10-25Add@cb
***************************************************************/
int __stdcall NetClient_GetITSDetectMode(int _iLogonID, int* _iMode);

/************************************************************************
*	�����Ȧ����ģʽ                                                           
************************************************************************/
/***************************************************************
Function     ��NetClient_SetITSLoopMode
Description  ����Ȧ����ģʽ���ýӿ�
Prameter(in) ��	_iLogonID    ����½id
				Prameter(out)��
				_iLoopMode   �������Ȧ����ģʽ
return       ��0��success
               other:failed
others       ��2012-10-26Add@cb
***************************************************************/
int __stdcall NetClient_SetITSLoopMode(int _iLogonID, int _iLoopMode);

/***************************************************************
Function     ��NetClient_GetITSLoopMode
Description  ����Ȧ����ģʽ��ȡ�ӿ�
Prameter(in) ��	_iLogonID    ����½id
				Prameter(out)��
				_iLoopMode   �������Ȧ����ģʽ
return       ��0��success
               other:failed
others       ��2012-10-26Add@cb
***************************************************************/
int __stdcall NetClient_GetITSLoopMode(int _iLogonID, int* _iLoopMode);

/************************************************************************
*	�����������                                                     
************************************************************************/
/***************************************************************
Function     ��NetClient_SetITSDeviceType
Description  ����������������ýӿ�
Prameter(in) ��	_iLogonID����½id
				Prameter(out)��
				_iDeviceType���豸����
return       ��0��success
               other:failed
others       ��2012-10-27Add@cb
***************************************************************/
int __stdcall NetClient_SetITSDeviceType(int _iLogonID, int _iDeviceType);

/***************************************************************
Function     ��NetClient_SetITSDeviceType
Description  ������������ͻ�ȡ�ӿ�
Prameter(in) ��	_iLogonID����½id
				Prameter(out)��
				_iDeviceType���豸����
return       ��0��success
               other:failed
others       ��2012-10-27Add@cb
***************************************************************/
int __stdcall NetClient_GetITSDeviceType(int _iLogonID, int* _iDeviceType);

/************************************************************************/
//*   ���ܽ�ͨһ�����Ŀ��������
/************************************************************************/
/***************************************************************
Function     ��NetClient_SetITSRoadwayParam
Description  �����ܽ�ͨһ�����Ŀ�����������ýӿ�
Prameter(in) ��	_iLogonID����½id
                _iCmd:     Э���������
				_iBufSize���ṹ���С
Prameter(out)��
				_lpBuf���ṹ��
return       ��0��success
               other:failed
others       ��2012-11-01Add@cb
***************************************************************/
int __stdcall NetClient_SetITSRoadwayParam(int _iLogonID, int _iCmd, void * _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_GetITSRoadwayParam
Description  �����ܽ�ͨһ�����Ŀ�����������ýӿ�
Prameter(in) ��	_iLogonID����½id
                _iCmd:     Э���������
				_iBufSize���ṹ���С
Prameter(out)��
				_lpBuf���ṹ��
return       ��0��success
               other:failed
others       ��2012-11-01Add@cb
***************************************************************/
int __stdcall NetClient_GetITSRoadwayParam(int _iLogonID, int _iCmd, void * _lpBuf, int _iBufSize);

/************************************************************************/
/*   ���ܽ�ͨһ�����Ŀ�����Ż�                                                                     */
/************************************************************************/
/***************************************************************
Function     ��NetClient_SetITSLicensePlateOptimize
Description  �����ܽ�ͨһ�����Ŀ�����Ż����ýӿ�
Prameter(in) ��	_iLogonID����½id
                _iCmd:     Э���������
				_iBufSize���ṹ���С
Prameter(out)��
				_lpBuf���ṹ��
return       ��0��success
               other:failed
others       ��2012-11-06Add@cb
***************************************************************/
int __stdcall NetClient_SetITSLicensePlateOptimize(int _iLogonID, int _iCmd, void * _lpBuf, int _iBufSize);
/***************************************************************
Function     ��NetClient_GetITSLicensePlateOptimize
Description  �����ܽ�ͨһ�����Ŀ�����Ż���ȡ�ӿ�
Prameter(in) ��	_iLogonID����½id
                _iCmd:     Э���������
				_iBufSize���ṹ���С
Prameter(out)��
				_lpBuf���ṹ��
return       ��0��success
               other:failed
others       ��2012-11-06Add@cb
***************************************************************/
int __stdcall NetClient_GetITSLicensePlateOptimize(int _iLogonID, int _iCmd, void * _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_SetITSExtraInfo
Description  ����ͨ�����ýӿ� 
Prameter(in) : _iLogonID:��¼ID

			  _iCmd ����
				ITS_ILLEGALPARK_CMD_SET	                //����Υ��ͣ������
				ITS_ILLEGALPARKPARM_CMD_SET			    //����Υ��ͣ���궨�������
				ITS_LICENSEOPTIMIZEOTHER_CMD_SET		//���ô��������ƺͲ����ų���
				ITS_OPTIM_CMD_SET						//���ó����Ż��׺���
				ITS_RECOPARAM_CMD_SET					//����ʶ���㷨����
				ITS_ROADWAYENABLE_CMD_SET				//���ó���
				ITS_CAMLOCATION_CMD_SET					//�����豸��Ϣ
				ITS_EXACTCONFIRM_CMD_SET                //��ȷ��λ����
				ITS_JPEGSIZEINFO_CMD_SET                //�������ץ�ķֱ���
			
			  _iChannel ��ʱδʹ��

			  _pvCmdBuf ���������
			   ���ݲ�ͬ������ ��Ӧ��ͬ�ṹ��
			   ITS_ILLEGALPARK_CMD_SET	         ʹ�� ITS_IllegalPark �ṹ��					    //����Υ��ͣ������
			   ITS_ILLEGALPARKPARM_CMD_SET		 ʹ�� ITS_IllegalParkParm �ṹ��				    //����Υ��ͣ���궨�������
			   ITS_LICENSEOPTIMIZEOTHER_CMD_SET	 ʹ�� ITS_LicenseOptimizeOther					//���ô��������ƺͲ����ų���
			   ITS_OPTIM_CMD_SET				 ʹ�� SetLicensePlateOptimizeFirstCharacter		//���ó����Ż��׺���
			   ITS_RECOPARAM_CMD_SET			 ʹ�� ITS_RecoParamEx							//����ʶ���㷨����
			   ITS_ROADWAYENABLE_CMD_SET		 ʹ�� TITSRoadwayInfo							//���ó���
			   ITS_CAMLOCATION_CMD_SET			 ʹ�� ITS_CamLocation							//�����豸��Ϣ
			   ITS_EXACTCONFIRM_CMD_SET          ʹ�� ITS_ExactConfirm                          //��ȷ��λ�ṹ��
			   ITS_JPEGSIZEINFO_CMD_SET          ʹ�� ITS_JPEGSizeInfo                          //�������ץ�ķֱ���      

		      _iCmdLen  sizeof(�����Ӧ�Ľṹ��)  ����
return       ��0��success
***************************************************************/
int __stdcall NetClient_SetITSExtraInfo(int _iLogonID, int _iCmd, int _iChannel, void* _pvCmdBuf, int _iCmdLen);

/***************************************************************
Function     ��NetClient_GetITSExtraInfo
Description  ����ͨ���ȡ�����ӿ�
Prameter(in): _iLogonID:��¼ID

_iCmd ����
ITS_ILLEGALPARK_CMD_GET	                //����Υ��ͣ������
ITS_ILLEGALPARKPARM_CMD_GET			    //����Υ��ͣ���궨�������
ITS_LICENSEOPTIMIZEOTHER_CMD_GET		//���ô��������ƺͲ����ų���
ITS_OPTIM_CMD_GET						//���ó����Ż��׺���
ITS_RECOPARAM_CMD_GET					//����ʶ���㷨����
ITS_ROADWAYENABLE_CMD_GET				//���ó���
ITS_CAMLOCATION_CMD_GET					//�����豸��Ϣ

_iChannel ��ʱδʹ��

_pvCmdBuf ���������
���ݲ�ͬ������ ��Ӧ��ͬ�ṹ��
ITS_ILLEGALPARK_CMD_GET	         ʹ�� ITS_IllegalPark �ṹ��					//����Υ��ͣ������
ITS_ILLEGALPARKPARM_CMD_GET		 ʹ�� ITS_IllegalParkParm �ṹ��				//����Υ��ͣ���궨�������
ITS_LICENSEOPTIMIZEOTHER_CMD_GET ʹ�� ITS_LicenseOptimizeOther					//���ô��������ƺͲ����ų���
ITS_OPTIM_CMD_GET				 ʹ�� SetLicensePlateOptimizeFirstCharacter		//���ó����Ż��׺���
ITS_RECOPARAM_CMD_GET			 ʹ�� ITS_RecoParamEx							//����ʶ���㷨����
ITS_ROADWAYENABLE_CMD_GET		 ʹ�� TITSRoadwayInfo							//���ó���
ITS_CAMLOCATION_CMD_GET			 ʹ�� ITS_CamLocation							//�����豸��Ϣ
ITS_EXACTCONFIRM_CMD_GET         ʹ�� ITS_ExactConfirm                          //��ȷ��λ�ṹ��

_iCmdLen 
sizeof(�����Ӧ�Ľṹ��)  ����
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_GetITSExtraInfo(int _iLogonID, int _iCmd, int _iChannel, void* _pvCmdBuf, int _iCmdLen);

// IPC 3MP referance
/***************************************************************
Function     ��NetClient_CheckDeviceState
Description  ������豸ʵʱ״̬
Prameter(in) ��_iLogonID��logon id
               _iChannelNo:ͨ����
			   _iType:��ѯ������ 0���������ʵʱ����ֵ
Prameter(out)��NULL
return       ��0: success
               other:fail
others       ��2011-11-28 16:14:09
***************************************************************/
int __stdcall NetClient_CheckDeviceState(int _iLogonID, int _iChannelNo, int _iType);

/***************************************************************
Function     ��NetClient_GetDeviceState
Description  ��������NetClient_CheckDeviceState���յ���Ӧ��Ϣ�������Ϣ���ص�ֵ������Ӧ����
Prameter(in) ��_iLogonID��logon id
			   _iChannelNo:ͨ����
			   _iType:��ѯ������ 0���������ʵʱ����ֵ
Prameter(out)��_iValue��ʵʱֵ
return       ��0: success
               other:fail
others       ��2011-11-29 11:21:37
***************************************************************/
int __stdcall NetClient_GetDeviceState(int _iLogonID, int _iChannelNo, int _iType, int *_iValue);

/***************************************************************
Function     ��NetClient_GetCameraCheckInfo
Description  ���õ���Ӧͨ���Ļ�����״̬���յ���������Ϣ�Ժ����
Prameter(in) ��_iLogonID��logon id
			   _iChannelNo:ͨ����
			   _iType: 0:������  1:��Ȧ���
Prameter(out)��_iValue��״ֵ̬
return       ��0�� success
               other��fail
others       ��2011-11-29 14:02:47
***************************************************************/
int __stdcall NetClient_GetCameraCheckInfo(int _iLogonID, int _iChannelNo, int _iType, int *_iValue);

/***************************************************************
Function     ��NetClient_CheckCamera
Description  ��check some information of the camera
Prameter(in) ��_iLogonID�� ��½id
			   _iChannelNo:channel no
			   _iType:     check type. 0:bad pixel  1:Iris check 
			   _iEnable��  start?��Reserverd��
Prameter(out)��NULL
return       ��0�� success
			   other��fail
others       ��2011-11-13 14:27:47
***************************************************************/
int __stdcall NetClient_CheckCamera(int _iLogonID, int _iChannelNo, int _iType, int _iEnable);

/***************************************************************
Function     ��NetClient_GetCharSet
Description  ���õ���ǰ�豸����ʹ�õ��ַ���
Prameter(in) ��_iLogonID����½id
Prameter(out)��_pcCharSet���ַ���
return       ��0��success
				other:failed
others       ��2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_GetCharSet(int _iLogonID, char* _pcCharSet);

/***************************************************************
Function     ��NetClient_SetTimeZone
Description  �������豸ʹ�õ�ʱ��
Prameter(in) ��_iLogonID����½ID
_iTimeZone:ʱ������Χ[-12,13]
Prameter(out)��NULL
return       ��0:success 
				other:failed
others       ��2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_SetTimeZone(int _iLogonID, int _iTimeZone);

/***************************************************************
Function     ��NetClient_GetTimeZone
Description  ���õ��豸ʹ�õ�ʱ��
Prameter(in) ��_iLogonID����½ID
Prameter(out)��_iTimeZone��ʱ������Χ[-12,13]
return       ��0��success
				other:failed
others       ��2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_GetTimeZone(int _iLogonID, int* _iTimeZone);

/***************************************************************
Function     ��NetClient_SetCurLanguage
Description  �������豸��ǰ����ʹ�õ�����
Prameter(in) ��_iLogonID����½ID 
_pcLanguage����������
Prameter(out)��NULL
return       ��0��success
				other:failed
others       ��2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_SetCurLanguage(int _iLogonID, char* _pcLanguage);

/***************************************************************
Function     ��NetClient_GetCurLanguage
Description  ���õ��豸��ǰ����ʹ�õ�����
Prameter(in) ��_iLogonID����½ID                
Prameter(out)��_pcLanguage����������
return       ��0��success 
				other:failed
others       ��2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_GetCurLanguage(int _iLogonID, char* _pcLanguage);

/***************************************************************
Function     ��NetClient_GetLanguageList
Description  ���õ��豸֧�ֵ������б�
Prameter(in) ��_iLogonID����½ID     
Prameter(out)��_pStrctLanguageList�������б�ṹ��
return       ��0��success
				other:failed
others       ��2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_GetLanguageList(int _iLogonID, st_NVSLanguageList* _pStrctLanguageList);

/***************************************************************
Function     ��NetClient_SetChannelEncodeProfile
Description  ������ͨ������Profile
Prameter(in) ��_iLogonID:��½ID
               _iChannelNum��ͨ����
			   _iStreamNo:���������Ǹ�����
			   _iLevel��Profile�ȼ���0-baseline  1-main  2-high��
Prameter(out)��NULL
return       ��0��success
               other��fail
others       ��2011-8-17 14:39:09
***************************************************************/
int __stdcall NetClient_SetChannelEncodeProfile(int _iLogonID, int _iChannelNum, int _iStreamNo, int _iLevel);

/***************************************************************
Function     ��NetClient_GetChannelEncodeProfile
Description  �����ͨ������Profile
Prameter(in) ��_iLogonID:��½ID
               _iChannelNum��ͨ����
			   _iStreamNo:���������Ǹ�����
Prameter(out)��_piLevel��Profile�ȼ���0-baseline  1-main  2-high��
return       ��0��success
               other:faile
others       ��2011-8-18 13:55:35
***************************************************************/
int __stdcall NetClient_GetChannelEncodeProfile(int _iLogonID, int _iChannelNum, int _iStreamNo, int* _piLevel);

/************************************************************************/
/* ��������        
_iClearType:	ALARM_TYPE_VIDEO_LOST		
				ALARM_TYPE_PORT_ALARM      	
				ALARM_TYPE_MOTION_DETECTION	
				ALARM_TYPE_VIDEO_COVER			
				ALARM_TYPE_EXCPETION			
				ALARM_TYPE_ALL	
				(See NetClientType.h)
return��		0��success
				other:failed

*/
/************************************************************************/
int __stdcall NetClient_SetAlarmClear(int _iLogonID, int _iChannelNo, int _iClearType);

/***************************************************************
Function     ��NetClient_SetExceptionHandleParam
Description  �������쳣�������
Prameter(in) ��_iLogonID: ��½id
               _iExceptionType���쳣���ͣ�0,��������1���޴��̣�2�����̶�д����3���Ƿ����ʣ�4��IP��ͻ��5�������жϣ�
			   _iFlag:����ʽ����λ������ bit0��ʹ�ܣ���Ϊ0�����κδ���ʽ������Ч��
										   bit1���Ƿ񵯳���ʾ�Ի���
										   bit2���Ƿ�������������
										   bit3���Ƿ�֪ͨ����ͻ��ˣ�
Prameter(out): NULL
return       ��0:success
               other:fail
others       ��2012/10/22 11:46:34
***************************************************************/
int __stdcall NetClient_SetExceptionHandleParam( int _iLogonID, int _iExceptionType, int _iFlag );

/***************************************************************
Function     ��NetClient_GetExceptionHandleParam
Description  ����ȡ�쳣�������
Prameter(in) ��_iLogonID: ��½id
               _iExceptionType���쳣���ͣ�0,��������1���޴��̣�2�����̶�д����3���Ƿ����ʣ�4��IP��ͻ��5�������жϣ�
			  
Prameter(out): _iFlag:����ʽ����λ������ bit0��ʹ�ܣ���Ϊ0�����κδ���ʽ������Ч��
							  bit1���Ƿ񵯳���ʾ�Ի���
							  bit2���Ƿ�������������
							  bit3���Ƿ�֪ͨ����ͻ��ˣ�
return       ��0:success
               other:fail
others       ��2012/10/22 11:46:34
***************************************************************/
int __stdcall NetClient_GetExceptionHandleParam( int _iLogonID, int _iExceptionType, int *_iFlag);

/************************************************************************/
/* ����/��ȡ������������      
_iAlarmKLinkType:	ALARMLINKTYPE_VIDEOLOST_LINKRECORD    
					ALARMLINKTYPE_VIDEOLOST_LINKSNAP   	  
					ALARMLINKTYPE_VIDEOLOST_LINKOUTPORT   
					ALARMLINKTYPE_MOTIONDETECT_LINKRECORD 
					ALARMLINKTYPE_MOTIONDETECT_LINKSNAP   
					ALARMLINKTYPE_MOTIONDETECT_LINKOUTPORT
					ALARMLINKTYPE_PORTALARM_LINKRECORD    
					ALARMLINKTYPE_PORTALARM_LINKSNAP      
					ALARMLINKTYPE_PORTALARM_LINKOUTPORT   
					(See NetClientType.h)
_ChannelNo:         _iAlarmKLinkType Ϊ ALARMLINKTYPE_PORTALARM_LINKRECORD��
										ALARMLINKTYPE_PORTALARM_LINKSNAP��
										ALARMLINKTYPE_PORTALARM_LINKOUTPORT ʱ�˲���Ϊ����˿ں�
					_iAlarmKLinkType Ϊ��������ʱ�˲���Ϊͨ����

_pParam��          ���������ĿǰΪint* _pParam[0]Ϊ1-32ͨ��ʹ���б�_pParam[1]Ϊ33-64ͨ��ʹ���б�

_iParamSize��      _pParam��С

return��		0��success
				other:failed

*/
/************************************************************************/
int __stdcall NetClient_SetAlarmLink_V1(int _iLogonID, int _iChannelNo, int _iAlarmLinkType, void * _pParam, int _iParamSize );
int __stdcall NetClient_GetAlarmLink_V1(int _iLogonID, int _iChannelNo, int _iAlarmLinkType, void * _pParam, int _iParamSize );

/***************************************************************
Function     ��NetClient_SetCameraParam
Description  ���������������ͨ��Э��								
Prameter(in): _iLogonID:��¼ID
_lpBuf���ṹ��TCameraParam��ָ��
_iBufSize���ṹ��Ĵ�С
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_SetCameraParam(int _iLogonID, void * _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_GetCameraParam
Description  ����ȡ���������ͨ��Э��
Prameter(in): _iLogonID:��¼ID
_iBufSize���ṹ��Ĵ�С
Prameter(in out): _lpBuf���ṹ��TCameraParam��ָ��
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_GetCameraParam(int _iLogonID, void * _lpBuf, int _iBufSize);

/************************************************************************/
//����ɫ�ʲ���   COLORPARA                                                                  
/************************************************************************/
int __stdcall NetClient_SetColorParam(int _iLogonID,int _iChannelNum, int _iColorToGradEnable, int _iDayRange, int _iNightRange);
int __stdcall NetClient_GetColorParam(int _iLogonID,int _iChannelNum, int *_iColorToGradEnable,  int *_iDayRange, int *_iNightRange);

int __stdcall NetClient_InnerMallocBlock(int _iNeedSize);
int __stdcall NetClient_InnerFreeBlock(void * _pBlock);
int __stdcall NetClient_InnerReferBlock(void * _pBlock);
int __stdcall NetClient_InnerReleaseBlock(void *_pBlock);
int __stdcall NetClient_SetJPEGQuality(int _iLogonID, int _iJpegQuality);
int __stdcall NetClient_GetJPEGQuality(int _iLogonID, int* _iJpegQuality);

/************************************************************************
*	�õ��豸�˵��������                                                           
************************************************************************/
int __stdcall NetClient_GetConnectInfo(int _iLogonID, void* _lpBuf, int _iSize);

/************************************************************************
*	ƽ̨�������б�                                                                     
************************************************************************/
int __stdcall NetClient_SetPlatformApp(int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetPlatformApp(int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);


/*
Function: Set manager servers infomation
Parameters: _iLogonID	    logonID
            _cRegSvrIP 	    Register Server IP
            _wdRegPort      Register Server listen port
            _cHeartSvrIP    Heartbeat server IP
            _wdHeartPort    Heartbeat server listen port
            _cAlarmSvrIP    Alarm server IP
            _wdAlarmPort    Alarm server listen port
Return value:  0             Success
          -1             Illegal IP address or not logon server
          -2             Use has no authority
          -3             Illegal sent parameter
*/
int __stdcall NetClient_SetManagerServersInfo(int _iLogonID, char* _cRegSvrIP, unsigned short _wdRegPort,
            char* _cHeartSvrIP, unsigned short _wdHeartPort, char* _cAlarmSvrIP, unsigned short _wdAlarmPort);

/*
Function: Get manager servers infomation
Parameters: _iLogonID	    logonID
            _cRegSvrIP 	    [OUT]Register Server IP
            _wdRegPort      [OUT]Register Server listen port
            _cHeartSvrIP    [OUT]Heartbeat server IP
            _wdHeartPort    [OUT]Heartbeat server listen port
            _cAlarmSvrIP    [OUT]Alarm server IP
            _wdAlarmPort    [OUT]Alarm server listen port
Return value:  0             Success
          -1             Illegal IP address or not logon server
          -2             Use has no authority
          -3             Illegal sent parameter
*/
int __stdcall NetClient_GetManagerServersInfo(int _iLogonID, char* _cRegSvrIP, unsigned short *_wdRegPort,
            char* _cHeartSvrIP, unsigned short *_wdHeartPort, char* _cAlarmSvrIP, unsigned short *_wdAlarmPort);

/*
Function: Set device's ID and name
Parameters: _iLogonID	    logonID
            _cDeviceID 	    NVS device ID, max length 32Bytes
            _cDeviceName    NVS device name, max length 32Bytes
Return value:  0            Success
          -1             Illegal IP address or not logon server
          -2             Use has no authority
          -3             Illegal sent parameter
*/
int __stdcall NetClient_SetDeviceID(int _iLogonID, char* _cDeviceID, char* _cDeviceName, unsigned short _iVspPort, char* _cAccessPass, unsigned short _iVapPort);

/*
Function: Get device's ID and name
Parameters: _iLogonID	    logonID
            _cDeviceID 	    [OUT]NVS device ID
            _cDeviceName    [OUT]NVS device name
Return value:  0            Success
          -1             Illegal IP address or not logon server
          -2             Use has no authority
          -3             Illegal sent parameter
*/
int __stdcall NetClient_GetDeviceID(int _iLogonID, char* _cDeviceID, char* _cDeviceName, unsigned short *_iVspPort, char* _cAccessPass, unsigned short *_iVapPort);

/***************************************************************
Function     �� NetClient_SetATMConfig
Description  ��	����ATM��ز���							
Prameter(in) ��	_iLogonID����½ID
				_iCmd: ������
				_lpBuf: �������Ӧ�Ľṹ��ָ��
return       ��	0:	success
<0:	failed
others       ��	2012-09-12 10:23:00
***************************************************************/
int __stdcall NetClient_SetATMConfig(int _iLogonID,int _iCmd, void* _lpBuf);

/***************************************************************
Function     �� NetClient_GetATMConfig
Description  ��	��ȡATM��ز���							
Prameter(in) ��	_iLogonID����½ID
				_iCmd: ������
Prameter(in out): _lpBuf: �������Ӧ�Ľṹ��ָ��
return       ��	0:	success
<0:	failed
others       ��	2012-09-12 10:23:00
***************************************************************/
int __stdcall NetClient_GetATMConfig(int _iLogonID,int _iCmd, void* _lpBuf);

/***************************************************************
Function     �� NetClient_ATMQueryFile
Description  ��	��ѯ¼���ļ�							
Prameter(in) ��	_iLogonID����½ID
				_iCmd: ������(ATM_QUERY_CMD_FIELD��ATM_QUERY_CMD_CARD)
				_lpBuf: �������Ӧ�Ľṹ��ָ��(ATM_FIELD_QUERY��ATM_CARD_QUERY)
return       ��	0:	success
<0:	failed
others       ��	2012-09-12 10:23:00
***************************************************************/
int __stdcall NetClient_ATMQueryFile(int _iLogonID, int _iCmd, void* _lpBuf);

/***************************************************************
Function     �� NetClient_ATMGetQueryFile
Description  ��	��ȡ¼���ļ���Ϣ							
Prameter(in) ��	_iLogonID����½ID
				_iFileIndex: �ļ�����
Prameter(out): _pFileData: ¼���ļ���Ϣ
return       ��	0:	success
<0:	failed
others       ��	2012-09-12 10:23:00
***************************************************************/
int __stdcall NetClient_ATMGetQueryFile(int	_iLogonID, int	_iFileIndex, ATM_FILE_DATA* _pFileData);

/***************************************************************
Function     �� NetClient_SetAudioSample
Description  ��	������Ƶ��������ز���							
Prameter(in) ��	_iLogonID����½ID
				_iChannel:ͨ����
				_iCmd: ������
				_lpBuf: �������Ӧ�Ľṹ��ָ��
return       ��	0:	success
<0:	failed
others       ��	2012-09-19 17:00:00
***************************************************************/
int __stdcall NetClient_SetAudioSample(int _iLogonID, int _iChannel,int _iCmd, void* _lpBuf);

/***************************************************************
Function     �� NetClient_GetAudioSample
Description  ��	�����Ƶ��������ز���							
Prameter(in) ��	_iLogonID����½ID
				_iChannel:ͨ����
				_iCmd: ������
Prameter(in out): _lpBuf: �������Ӧ�Ľṹ��ָ��
return       ��	0:	success
<0:	failed
others       ��	2012-09-19 17:00:00
***************************************************************/
int __stdcall NetClient_GetAudioSample(int _iLogonID, int _iChannel,int _iCmd, void* _lpBuf);

/***************************************************************
Function     ��NetClient_SetSystemTypeEx
Description  ������ϵͳ����								
Prameter(in) ��_iLogonID:��¼ID
_iType:ϵͳ����,0:����,3:���� 4:�羯					
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_SetSystemTypeEx( int _iLogonID, int _iCmd, void* _pvCmdBuf);

/***************************************************************
Function     ��NetClient_GetSystemTypeEx
Description  ����ȡϵͳ����								
Prameter(in) ��_iLogonID:��¼ID
Prameter(out) ��_piType:ϵͳ����,0:����,3:���� 4:�羯					
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_GetSystemTypeEx( int _iLogonID, int _iCmd, void* _pvCmdBuf);

/***************************************************************
Function     ��NetClient_SetHXListenPortInfo
Description  �����ü����˿�
Prameter(in): _iLogonID:��¼ID
_iVideoPort����Ƶ�����˿�
_iTalkPort���Խ������˿�
_iMsgPort���豸��ָ������˿�
Prameter(in out): _lpBuf���ṹ��TCameraParam��ָ��
return       ��0��success
other:faile
others       ��	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_SetHXListenPortInfo(int _iLogonID,int _iVideoPort,int _iTalkPort,int _iMsgPort);

/***************************************************************
Function     ��NetClient_GetHXListenPortInfo
Description  ����ü����˿�
Prameter(in): _iLogonID:��¼ID
Prameter(out):_iVideoPort����Ƶ�����˿�
_iTalkPort���Խ������˿�
_iMsgPort���豸��ָ������˿�
Prameter(in out): _lpBuf���ṹ��TCameraParam��ָ��
return       ��0��success
other:faile
others       ��	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_GetHXListenPortInfo(int _iLogonID,int* _iVideoPort,int* _iTalkPort,int* _iMsgPort);

/***************************************************************
Function     ��NetClient_SetVideoModeMethod
Description  ��������ƵԴ��ʽ�Ļ�ȡ��ʽ
Prameter(in) ��_iLogonID����¼ID
               _iChannel��ͨ����     ��ʱ��0XFFFF������ʾ�������豸��Ч��������ʾͨ����
               _iMethod����ȡ����   0���ֶ����ã�1������Ӧ��
Prameter(out)��NULL
return       ��0��   success
               other��fail
others       ��	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_SetVideoModeMethod(int _iLogonID, int _iChannel, VIDEO_METHOD  _iMethod);

/***************************************************************
Function     ��NetClient_GetVideoModeMethod
Description  ��������ƵԴ��ʽ�Ļ�ȡ��ʽ
Prameter(in) ��_iLogonID����¼ID
               _iChannel��ͨ����     ��ʱ��0XFFFF������ʾ�������豸��Ч��������ʾͨ����
Prameter(out)��*_piMethod����ȡ����   0���ֶ����ã�1������Ӧ��
return       ��0��   success
               other��fail
others       ��	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_GetVideoModeMethod(int _iLogonID, int _iChannel, VIDEO_METHOD  *_piMethod);

/***************************************************************
Function     ��NetClient_GetMonitorNum
Description  ����ü�������Ŀ
return       ��>=0����������Ŀ
               other��fail
others       ��	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_GetMonitorNum();

/***************************************************************
Function     ��NetClient_GetMonitorInfo
Description  ����ü�������Ϣ
Prameter(in) ��_uiIndex�����������
Prameter(out) ��_MonitorInfo����������Ϣ 
return       ��0��success
               other��fail
others       ��	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_GetMonitorInfo(unsigned int _uiIndex,MONITOR_INFO* _MonitorInfo);

/***************************************************************
Function     ��NetClient_ChangeMonitor
Description  ���޸ļ�������Ϣ
Prameter(in) ��_iLogonID����¼ID
			 _iChannelNum��ͨ���� 
			 _uiIndex�����������
			 _iStreamNO����������
return       ��0��success
               other��fail
others       ��	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_ChangeMonitor(int _iLogonID,int _iChannelNum,unsigned int _uiIndex, int _iStreamNO=MAIN_STREAM);

/***************************************************************
Function     ��NetClient_EZoomAdd
Description  ����ӵ��ӷŴ�
Prameter(in) ��_uiConnID������ID
			 _hWnd����ʾ��� 
			 _rctDisplay����ʾ����
			 _uiMonitorIndex�����������
return       ��>=0��EZoomID
               other��fail
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_EZoomAdd(unsigned int _uiConnID, int _hWnd, RECT _rctDisplay, unsigned int _uiMonitorIndex = 0);

/***************************************************************
Function     ��NetClient_EZoomSet
Description  �����õ��ӷŴ�
Prameter(in) ��_uiConnID������ID
			 _iEZoomID�����ӷŴ�ID 
			 _rctVideo����ʾ����
return       ��0��sucess
               other��fail
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_EZoomSet(unsigned int _uiConnID, int _iEZoomID, RECT _rctVideo);

/***************************************************************
Function     ��NetClient_EZoomReset
Description  �����³�ʼ�����ӷŴ�
Prameter(in) ��_uiConnID������ID
			 _iEZoomID�����ӷŴ�ID 
return       ��0��sucess
               other��fail
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_EZoomReset(unsigned int _uiConnID, int _iEZoomID);

/***************************************************************
Function     ��NetClient_EZoomRemove
Description  ��ɾ�����ӷŴ�
Prameter(in) ��_uiConnID������ID
			 _iEZoomID�����ӷŴ�ID 
return       ��0��sucess
               other��fail
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_EZoomRemove(unsigned int _uiConnID, int _iEZoomID);

/***************************************************************
Function     ��NetClient_SetPsPackCallBack
Description  ������PS����װ�Ļص�����
Prameter(in) ��_uiConnID������ID
			 _cbkPsPackNotify��PS���ص����� 
			 _pContext���û�����
return       ��0��sucess
               other��fail
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_SetPsPackCallBack(unsigned int _ulConID, PSPACK_NOTIFY _cbkPsPackNotify, void*	_pContext);

/***************************************************************
Function     ��NetClient_DCardInit
Description  ����ʼ�����뿨
return       ��>=0��Success, return hardware decode channel number in system, that is, the max video number supported by the system
			   -1��Failure
			   -10��Core program Decoder.out does not exist or illegal format
               other��fail
others       ��2012-11-12 11:23:00
Notes: The interface and NetClient_DCardRelease (Release system recode card)must be used
together; functions related to hardware decode must be used between them, every hardware
channel can decode 1 channel D1, or1 channel Half-D1, or 4 channels Cif.
***************************************************************/
int __stdcall   NetClient_DCardInit();

/***************************************************************
Function     ��NetClient_DCardSetShowMode
Description  ��Set show mode of decode channel
Prameter(in) ��_iCardChannel��Decode card channel number, from 0 to -1
			  _iShowMode��Show mode of the channel
return       ��=0��Success, return file size
				<0 ��Failure
				-1 ��Whether channel number is legal
				-2 ��Whether shoe mode is legal
				-3 ��Showing decode, mode can not be set
				-5 ��Decode card has not been initialized
others       ��2012-11-12 11:23:00
Notes:  The channel can only decode 1 channle video if set 1 channel show, up to 4 channels CIF
can be decoded if set 4 channels show
***************************************************************/
int __stdcall   NetClient_DCardSetShowMode(int	_iCardChannel, int	_iShowMode);

/***************************************************************
Function     ��NetClient_DCardGetShowMode
Description  ��Get show mode of decode channel
Prameter(in) ��_iCardChannel��Decode card channel number, from 0 to -1
Prameter(out)��_iShowMode��Show mode of the channel
return       ��=0��Success, return file size
				<0 ��Failure
				-1 ��Illegal channel
				-5 ��Decode card has not been initialized
others       ��2012-11-12 11:23:00
Notes:  The channel can only decode 1 channle video if set 1 channel show, up to 4 channels CIF
can be decoded if set 4 channels show
***************************************************************/
int __stdcall	NetClient_DCardGetShowMode(int _iCardChannel, int *_iShowMode);

/***************************************************************
Function     ��NetClient_DCardStartPlay
Description  ��Start decord output
Prameter(in) ��_uiConnID��Connection mark, returned parameter when connect video
			   _iCardChannel��Decode channel number, form 0 to -1
			   _iPos��Display position of the channel video, it wil be 0 if show
					   0: Up-left
					   1: Up-right
					   2: Down-left
					   3: Down-right
return       ��0��Success, return file size
				<0 Failure
				-1 Invalid connection mark _ulConID
				-3 Illagal channel number
				-4 Show mode has not been set
				-5 Decode card has not been initialized
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardStartPlay(unsigned int	_ulConID, int	_iCardChannel, int	_iPos);

/***************************************************************
Function     ��NetClient_DCardStopPlay
Description  ��Stop decode output
Prameter(in) ��_uiConnID��Connection mark, returned parameter when connect video
			   _iFlag��Stop param
return       ��0��Success, return file size
				<0 Failure
				-1 Invalid connection mark _ulConID
				-5 Decode card has not been initialized
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardStopPlay(unsigned int	_ulConID, int	_iFlag = 1);

/***************************************************************
Function     ��NetClient_DCardRelease
Description  ��Destroy system decode card
return       ��>=0��Success, return hardware decode channel number, that is the max video channel number supported by the system
				<0 Failure
				-1 Invalid connection mark _ulConID
				-5 Decode card has not been initialized
others       ��2012-11-12 11:23:00
Notes: The interface and NetClient_DCardRelease (Release system recode card)must be used together;
functions related to hardware decode must be used between them, every hardware channel can decode
1 channel D1, or1 channel Half-D1, or 4 channels Cif.
The three interfaces below are showed decoeded data to the users��NVS stream data used for the type show��
***************************************************************/
int __stdcall NetClient_DCardRelease();

/***************************************************************
Function     ��NetClient_DCardReInit
Description  ��Reinited one decode card
Prameter(in) ��_iCardChannel��Decode channel number, form 0 to -1
return       ��0��Success
				-1��Failure
				-3��Illegal parameters
				-10��Decode card need file:Decoder.out is illegal.
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardReInit(int	_iCardChannel);

/***************************************************************
Function     ��NetClient_DCardGetState
Description  ��Get the Decode card working state
Prameter(in) ��_iCardChannel��Decode channel number, form 0 to -1
Prameter(out) ��_iState��working state
return       ��0��Success
				-1��Failure
				-3��Illegal parameters
				-4��the decode card don't support this API
others       ��2012-11-12 11:23:00
Note��  Every time get the _iState will be diffirent.If get the same value in series 2 time, that mean the
card work unormal. can reinited card use API NetClient_DCardReInit
***************************************************************/
int __stdcall NetClient_DCardGetState(int	_iCardChannel, int*	_iState);

/***************************************************************
Function     ��NetClient_DCardStartPlayEx
Description  ��Start decord output
Prameter(in) ��_iId��Connection mark, returned parameter when connect video
			   _dParam��Decode param
return       ��0��Success, return file size
				<0 Failure
				-1 Invalid connection mark _ulConID
				-2 Illegal video frame
				-3 Illagal channel number
				-4 Show mode has not been set
				-5 Decode card has not been initialized
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardStartPlayEx(int*	_iId, DecoderParam*	_dParam);

/***************************************************************
Function     ��NetClient_DCardPutDataEx
Description  ��Send external data to decode output
Prameter(in) ��_iId��Connection mark, returned parameter when connect video
			   _cBuf��Index for data buffer
			   _iLen��Data length
return       ��0��Success, return file size
				<0��Failure
				-1��Invalid connection mark _ulConID
				-5��Decode card has not been initialized
others       ��2012-11-12 11:23:00
Notes: External data will pass the interface after decode, send data to decoder output
***************************************************************/
int __stdcall NetClient_DCardPutDataEx(int	_iId, char*	_cBuf, int	_iLen);

/***************************************************************
Function     ��NetClient_DCardStopPlayEx
Description  ��Stop external data decode output
Prameter(in) ��_iId��Connection mark, returned parameter when connect video
			   _iFlag��stop param
return       ��0��Success, return file size
				<0��Failure
				-1��Invalid connection mark _ulConID
				-5��Decode card has not been initialized
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardStopPlayEx(int	_iId, int	_iFlag = 1);

/***************************************************************
Function     ��NetClient_DCardStartPlayAudio
Description  ��Start decode card audio output
Prameter(in) ��_uiConnID��Connection mark, returned parameters when connect video
return       ��0��sucess
			  -1��Invalid connection mark _ulConID
			  -5��Decode card has not been initialized
               other��fail
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardStartPlayAudio(unsigned int _ulConID);

/***************************************************************
Function     ��NetClient_DCardStopPlayAudio
Description  ��Stop decode card audio output
Prameter(in) ��_uiConnID��Connection mark, returned parameters when connect video
return       ��0��sucess
			  -1��Invalid connection mark _ulConID
			  -5��Decode card has not been initialized
               other��fail
others       ��2012-11-12 11:23:00
***************************************************************/
int __stdcall	NetClient_DCardStopPlayAudio(unsigned int _ulConID);

/***************************************************************
Function     ��NetClient_SetEncryptSN
Description  �����ü������к�								
Prameter(in) ��_iLogonID:��¼ID
_iType:��������,0��ʶ���㷨��������,1�������������		
_cSN:�������к�
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_SetEncryptSN( int _iLogonID,int _iType,char* _cSN);

/***************************************************************
Function     ��NetClient_GetSNReg
Description  ����ȡע����										
Prameter(in) ��_iLogonID:��¼ID
Prameter(out) ��_piLockRet:ע����,"bit0��ʶ��,bit1�����ܽ�ͨ���ܹ�;
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_GetSNReg( int _iLogonID,int* _piLockRet);

/***************************************************************
Function     ��NetClient_GetComFormat_V1
Description  ����ȡ�������Ե���չ�ӿ�,����Э�����ƵĲ�ѯ
Prameter(in): _iLogonID:��¼ID
_iComPort�����ں�
Prameter(out): _cDeviceType:Э������
_cCommFormat:��������
_iWorkMode:���ڹ���ģʽ��1������Э��,2��͸��ͨ��,3:��ҵԤ��
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_GetComFormat_V1(int _iLogonID, int _iComPort,char* _cDeviceType,char* _cCommFormat,int* _iWorkMode);

/***************************************************************
Function     �� NetClient_SetComFormat_V2
Description  ��	���ô��ڸ�ʽ,��չ��iComType							
Prameter(in) ��	_iLogonID����½ID
Prameter(in) ��_pComFormat: ���ڸ�ʽ�������
return       ��	0:	success
<0:	failed
others       ��	2012-03-06 10:00:00
***************************************************************/
int __stdcall NetClient_SetComFormat_V2(int _iLogonID, COMFORMAT* _pComFormat);

/***************************************************************
Function     �� NetClient_GetComFormat_V2
Description  ��	��ȡ���ڸ�ʽ,��չ��iComType							
Prameter(in) ��	_iLogonID����½ID
Prameter(in out) ��_pComFormat: ���ڸ�ʽ�������
return       ��	0:	success
<0:	failed
others       ��	2012-03-06 10:00:00
***************************************************************/
int __stdcall NetClient_GetComFormat_V2(int _iLogonID, COMFORMAT* _pComFormat);

/***************************************************************
Function     ��NetClient_GetServerVersion_V1
Description  ��Get the version of plugin
Prameter(in) ��_iLogonID:       logon id       
Prameter(out)��_pstrctServerVer:the struct point of the server version 
return       ��0:success
               other:fail
others       ��2011-10-19 11:27:35
***************************************************************/
int __stdcall NetClient_GetServerVersion_V1(int _iLogonID, SERVER_VERSION *_pstrctServerVer);

/***************************************************************
Function     ��NetClient_InputTalkingdataEx
Description  �����ͶԽ����� ����ÿ�η���640�ֽ�����
Prameter(in): _iLogonID:��¼ID
_ucData���Խ�����
_iLen�� �Խ����ݳ���
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_InputTalkingdataEx(int _iLogonID, unsigned char* _ucData, unsigned int _iLen);

/***************************************************************
Function     ��TC_SetVerticalSync
Description  �����ô�ֱͬ��������������ֱͬ�����Է�ֹ���������ǻ����Ĵ�����Cpu
Prameter(in) ��_ulConID��Connection mark
_iFlag:ͬ����ʾ 0���ر� 1������
return       ��0�� success
other��fail
others       ��2011-12-07 13:20:00
***************************************************************/
int __stdcall NetClient_SetVerticalSync(unsigned int _ulConID,int _iFlag);

/***************************************************************
Function     ��TC_GetVerticalSync
Description  ����ô�ֱͬ������
Prameter(in) ��_ulConID��Connection mark
Prameter(out) ��_piFlag:ͬ����ʾ 0���ر� 1������
return       ��0�� success
other��fail
others       ��2011-12-07 13:20:00
***************************************************************/
int __stdcall NetClient_GetVerticalSync(unsigned int _ulConID,int* _piFlag);


/***************************************************************
Function     ��NetClient_SetLocalAudioVolumeEx
Description  ��������������չ�ӿ�
Prameter(in) ��_uiConID	 ���ͨ����Ƶ���Ӧ��ConnectID
Prameter(out)��_iVolume������ֵ
return       ��0�� success
other��fail
others       ��2011-12-08 19:32:20
***************************************************************/
int __stdcall NetClient_SetLocalAudioVolumeEx(unsigned int _ulConID, int _iVolume);

/***************************************************************
Function     ��NetClient_GetLocalAudioVolumeEx
Description  ����ȡ��������չ�ӿ�
Prameter(in) ��_uiConID	 ���ͨ����Ƶ���Ӧ��ConnectID
Prameter(out)��_iVolume������ֵ
return       ��0�� success
other��fail
others       ��2011-12-14 
***************************************************************/
int __stdcall NetClient_GetLocalAudioVolumeEx(unsigned int _ulConID, int *_iVolume);

/************************************************************************
*	Function:	��������ѻ������
*	Parameters:
		_uiConnID		ConnectID (from NetClient_StartRecv)
		_iPolygonIndex  Polygon Index(from NetClient_DrawPolyOnLocalVideo)
*	Return:
		>=0		Success
		<0		Error
************************************************************************/
int __stdcall NetClient_ClearPolyLocalVideo(unsigned int _uiConnID, int _iPolygonIndex);

/*
Function: Set video show mode
Parameters: 
_iLogonID       logonID
_iChannelNum    ͨ����
iOsdType		�ַ��������ͣ� 1��ͨ�����ƣ��ı��� 2��ʱ������ 3:logo��ɫ 4�������ַ� 5:��ͨר��
iSize: ���������С��1-5ѡ��
Return value:  
				0     Success
<0              Failed
*/
int __stdcall NetClient_SetOSDTypeFontSize(int _iLogonID,int _iChannelNum,int _iOSDType, int _iSize);

/*
Function: Get video show mode .
Parameters: 
_iLogonID       logonID
_iChannelNum    ͨ����
iOsdType		�ַ��������ͣ� 1��ͨ������ 2��ʱ������  3:logo��ɫ 4�������ַ� 5:��ͨר��
iSize			���������С��1-5ѡ��
Return value:
				0      Success
<0              Failed
*/
int __stdcall NetClient_GetOSDTypeFontSize(int _iLogonID,int _iChannelNum,int _iOSDType, int* _iSize);

/***************************************************************
Function     ��NetClient_SetImgDisposal
Description  ������ͼƬЧ��������					
Prameter(in) ��_iLogonID:��¼ID
_lpBuf:TImgDisposal�ṹ��ָ��
_iBufSize���ṹ���С
return       ��0��success
other:faile
***************************************************************/	
int __stdcall NetClient_SetImgDisposal( int _iLogonID,void * _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_GetImgDisposal
Description  ����ȡͼƬЧ��������					
Prameter(in) ��_iLogonID:��¼ID
_iBufSize���ṹ���С
Prameter(in out) ��_lpBuf:TImgDisposal�ṹ��ָ��
return       ��0��success
other:faile
***************************************************************/	
int __stdcall NetClient_GetImgDisposal( int _iLogonID,void * _lpBuf, int _iBufSize);

/************************************************************************/
/* ���þ���  
_uiConID     NetClient_StartRecv�ķ���ֵ

_iMuted      MUTED-����    NOT_MUTED-������  
*/
/************************************************************************/
int __stdcall NetClient_SetMuted(unsigned int _uiConID, int _iMuted);

/***************************************************************
Function     ��NetClient_SetPWMValue
Description  �����ú����PWM����ֵ
Prameter(in) ��_iLogonID     ��¼ID
               _iChannelNo   ͨ����
			   _iValue       PWMֵ
Prameter(out)��NULL
return       ��0��    success
			   other��fail
others       ��2012-02-10 11:28:20
***************************************************************/
int __stdcall NetClient_SetPWMValue(int _iLogonID, int _iChannelNo, int _iValue);   //0210

/***************************************************************
Function     ��NetClient_SetPWMValue
Description  ����ȡ�����PWM����ֵ
Prameter(in) ��_iLogonID     ��¼ID
			   _iChannelNo   ͨ����
			   _iValue       PWMֵ
Prameter(out)��NULL
return       ��0��    success
               other��fail
others       ��2012-02-10 11:28:20
***************************************************************/
int __stdcall NetClient_GetPWMValue(int _iLogonID, int _iChannelNo, int* _piValue); //0210

/***************************************************************
Function     ��NetClient_SetSystemType
Description  ������ϵͳ����								
Prameter(in) ��_iLogonID:��¼ID
_iType:ϵͳ����,0:����,3:���� 4:�羯					
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_SetSystemType( int _iLogonID,int _iType);

/***************************************************************
Function     ��NetClient_GetSystemType
Description  ����ȡϵͳ����								
Prameter(in) ��_iLogonID:��¼ID
Prameter(out) ��_piType:ϵͳ����,0:����,3:���� 4:�羯					
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_GetSystemType( int _iLogonID,int* _piType);

/************************************************************************
*	������Ƶ���ⴥ��ģʽ�л�ʱ�������������ʱץ��ʱ�䣨��Ҫ�����״                                                  
************************************************************************/
int __stdcall NetClient_SetITSSwitchTime(int _iLogonID, int _iSwitchTime, int _iDelayTime);
int __stdcall NetClient_GetITSSwitchTime(int _iLogonID, int* _iSwitchTime, int* _iDelayTime);

/************************************************************************
*	��ͨʶ���㷨����                                                                
************************************************************************/
int __stdcall NetClient_SetITSRecoParam(int _iLogonID, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetITSRecoParam(int _iLogonID, void* _lpBuf, int _iBufSize);

/************************************************************************
*	��ͨ����ҹ��ʱ���                                                                
************************************************************************/
int __stdcall NetClient_SetITSDayNight(int _iLogonID, int _iTimeSegment);
int __stdcall NetClient_GetITSDayNight(int _iLogonID, int* _iTimeSegment);

/************************************************************************
*	��ͨ���λ��                                                               
************************************************************************/
int __stdcall NetClient_SetITSCamLocation(int _iLogonID, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetITSCamLocation(int _iLogonID, void* _lpBuf, int _iBufSize);

/************************************************************************
*	��ͨ�������ģʽ                                                           
************************************************************************/
int __stdcall NetClient_SetITSWorkMode(int _iLogonID, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetITSWorkMode(int _iLogonID, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_SetWaterMarkEnable
Description  ������ˮӡʹ��	
Prameter(in) ��_iLogonID:��¼ID
_iChannelID:�������,���֧��4��������0-3
_iEnable��ʹ��,0�������ã�1������					
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_SetWaterMarkEnable( int _iLogonID,int _iChannelID,int _iEnable);

/***************************************************************
Function     ��NetClient_GetWaterMarkEnable
Description  ����ȡˮӡʹ��		
Prameter(in) ��_iLogonID:��¼ID
_iChannelID:�������,���֧��4��������0-3
Prameter(out) ��_piEnable��ʹ��,0�������ã�1������					
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_GetWaterMarkEnable( int _iLogonID,int _iChannelID,int* _piEnable);

/***************************************************************
Function     ��NetClient_SetITSLightInfo
Description  �������źŵƶ�Ӧ�Ĳ���								
Prameter(in): _iLogonID:��¼ID
_lpBuf���ṹ��TITSLightInfo��ָ��
_iBufSize���ṹ��Ĵ�С
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_SetITSLightInfo(int _iLogonID, void * _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_GetITSLightInfo
Description  ����ȡ�źŵƶ�Ӧ�Ĳ���
Prameter(in): _iLogonID:��¼ID
_iBufSize���ṹ��Ĵ�С
Prameter(in out): _lpBuf���ṹ��TITSLightInfo��ָ��
return       ��0��success
other:faile
***************************************************************/
int __stdcall NetClient_GetITSLightInfo(int _iLogonID, void * _lpBuf, int _iBufSize);

//cb@121205for8127
/***************************************************************
Function     ��NetClient_SetHardWare
Description  ������sensor��ѹ	
Prameter(in) ��_iLogonID:��¼ID
               _iCmd: �豸��������
			   _lpBuf:���ݻ���ָ��
			   _iBufSize:���ݻ����С

			   ���_lpBufΪ��sensor��ѹʱ��
               _iType:   �豸���ͣ�0��reserved;1��sensor;2:xxx
               _iMold��  �豸��������,0��reserved��1����ѹ��2������
			   _iValue�� ����ֵ�����ݲ�ͬ����ȡֵ��ͬ����ѹ��λ��V���ͻ�ֱ����SDK��V*100���ͻ���գ��豸�˳���100��ȡfloatֵ���õ�ѹ��
return       ��0��success
               other:faile
***************************************************************/
int __stdcall NetClient_SetHardWareParam( int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_GetHardWare
Description  ����ȡsensor��ѹ		
Prameter(in) ��	_iLogonID:��¼ID
				_iCmd: �豸��������
				_lpBuf:���ݻ���ָ��
				_iBufSize:���ݻ����С

                ���_lpBufΪ��sensor��ѹʱ��
				_iType:   �豸���ͣ�0��reserved;1��sensor;2:xxx
				_iMold��  �豸��������,0��reserved��1����ѹ��2������
				_iValue�� ����ֵ�����ݲ�ͬ����ȡֵ��ͬ����ѹ��λ��V���ͻ�ֱ����SDK��V*100���գ�SDK�˳���100��ȡfloatֵ�����ϲ㣻					
return       ��0��success
               other:faile
***************************************************************/
int __stdcall NetClient_GetHardWareParam( int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_SetDomeAdvParam
Description  ����������߼�����							
Prameter(in) ��_iLogonID:	��¼ID
			   _iChannelNO:	ͨ����
			   _iCmd:		��������
			   _pvCmdBuf��	����������ز���ָ��
			   _iCmdBufLen��
return       ��0��success
other		 :failed
***************************************************************/
int __stdcall NetClient_SetDomeAdvParam( int _iLogonID, int _iChannelNO, int _iCmd, void* _pvCmdBuf, int _iCmdBufLen);

/***************************************************************
Function     ��NetClient_SetDomeAdvParam
Description  ����ȡ����߼�����							
Prameter(in) ��_iLogonID:	��¼ID
			   _iChannelNO:	ͨ����
			   _iCmd:		��������
			   _pvCmdBuf��	����������ز���ָ��
			   _iCmdBufLen��
return       ��0��success
other		 :failed
***************************************************************/
int __stdcall NetClient_GetDomeAdvParam( int _iLogonID, int _iChannelNO, int _iCmd, void* _pvCmdBuf, int _iCmdBufLen);

/***************************************************************
Function     ��	NetClient_SetDiskGroup
Description  ��	����������Ϣ
Prameter(in) ��	_iLogonID����¼ID
				_lpBuf: �ṹ��(DISK_GROUP)ָ��
				_iDiskGroupNum:��������ĸ�������󲻳���8
return       ��  0����ȷ
				other������
others       ��	2013-03-26 
***************************************************************/
int __stdcall NetClient_SetDiskGroup( int _iLogonID,void* _lpBuf, int _iDiskGroupNum);

/***************************************************************
Function     ��	NetClient_GetDiskGroup
Description  ��	���������Ϣ
Prameter(in) ��	_iLogonID����¼ID 
				_iDiskGroupNum����ǰ�豸֧�ֵ����������
Prameter(out) ��	_lpBuf: �ṹ��(DISK_GROUP)ָ��         
return       ��  0����ȷ
				-1������������
				other������
others       ��	2013-03-26 
***************************************************************/
int __stdcall NetClient_GetDiskGroup( int _iLogonID,void* _lpBuf, int _iDiskGroupNum);

/***************************************************************
Function     ��	NetClient_SetDiskQuota
Description  ��	���ô������
Prameter(in) ��	_iLogonID����¼ID
				_iChannelNo��ͨ����
				_lpBuf: �ṹ��(DISK_QUOTA)ָ��
				_iBufSize:�ṹ���С          
return       ��    0����ȷ
				other������
others       ��	2013-03-26 
***************************************************************/
int __stdcall NetClient_SetDiskQuota( int _iLogonID, int _iChannel, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ��	NetClient_GetQuotaState
Description  ��	��ȡĳͨ���������״̬
Prameter(in) ��	_iLogonID����¼ID
				_iChannelNo��ͨ���ţ�����_iCmd������
				_iCmd: �����   
				CMD_DISK_QUOTA_MAX_USED  		������������С 
				CMD_DISK_QUOTA_TOTAL			�ܴ��̵Ĵ�С
				CMD_DISK_QUOTA_CURRENT_USED		���̵�ǰʹ�õ�����С
				_iBufSize:�ṹ���С 
Prameter(out)��	_lpBuf������_iCmd�������������˵��         
return       ��  0����ȷ
				-1������������
				other������
others       ��	2013-04-01  
***************************************************************/
int __stdcall NetClient_GetDiskQuotaState ( int _iLogonID, int _iChannelNo, int _iCmd, void* _lpBuf, int _iBufSize );

/***************************************************************
Function     ��		NetClient_ModifyUserAuthority
Description  ��		�޸��û�Ȩ��
Prameter(in) ��		_iLogonID����¼ID
_pcUserName���ǹ���Ա�û�������󲻳���32���ַ���
_lpBuf: �ṹ��(USER_AUTHORITY)ָ��
_iBufSize:�ṹ���С
return       ��     0�� success
-1������������
other��fail
others       ��		2013-03-18 
***************************************************************/
int __stdcall NetClient_ModifyUserAuthority( int _iLogonID, char* _pcUserName, void* _lpBuf, int _iBufSize );

/***************************************************************
Function     ��		NetClient_GetUserAuthority
Description  ��		����޸ĵ��û�Ȩ��
Prameter(in) ��		_iLogonID����¼ID
_pcUserName���ǹ���Ա�û�������󲻳���32���ַ���
_iBufSize:�ṹ���С
Prameter(out): 		_lpBuf: �ṹ��(USER_AUTHORITY)ָ��
return       ��     0�� success
-1������������
other��fail
others       ��		2013-03-18 
***************************************************************/
int __stdcall NetClient_GetUserAuthority( int _iLogonID, char* _pcUserName, void* _lpBuf, int _iBufSize );

/***************************************************************
Function     ��	NetClient_GetGroupAuthority
Description  ��	�����豸Ȩ������ȡ��Ӧ��Ȩ��
Prameter(in) ��	_iLogonID����¼ID
_iBufSize:�ṹ���С
Prameter(in,out) ��_lpBuf: �ṹ��(GROUP_AUTHORITY)ָ��,(in��Ȩ�����ı�ţ�out����ӦȨ��
return       �� 0����ȷ
-1������������
other������
others       ��	2013-03-26 
***************************************************************/
int __stdcall NetClient_GetGroupAuthority( int _iLogonID, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     �� NetClient_NetFileGetQueryfileEx
Description  ��	��ȡ¼���ļ���Ϣ							
Prameter(in) ��	_iLogonID����½ID
				_iFileIndex: �ļ�����
Prameter(in out): _pFileInfo: ¼���ļ���Ϣ�ṹ��ָ��
return       ��	0:	success
<0:	failed
others       ��	2013-03-27 10:20:00
***************************************************************/
int __stdcall NetClient_NetFileGetQueryfileEx(int _iLogonID,int _iFileIndex, PNVS_FILE_DATA_EX _pFileInfo);

/***************************************************************
Function     ��NetClient_NetFileLockFile
Description  ���ӽ����ļ�		
Prameter(in) ��	_iLogonID:��¼ID
				_cFileName: �ļ���
				_iLock:�ӽ���״̬
return       ��	0:	success
<0:	failed
others       ��	2013-03-27 10:20:00
***************************************************************/
int __stdcall NetClient_NetFileLockFile( int _iLogonID, char* _cFileName, int _iLock);

/***************************************************************
Function     ��NetClient_GetOsdTextEx
Description  ����ȡͨ�����Ƽ���ɫ		
Prameter(in out) ��	_iLogonID:��¼ID
					_iChannel: ͨ����
					_lpBuf:�ṹ��ָ��
					_iBufSize:�ṹ���С			
return       ��	0:	success
<0:	failed
others       ��	2013-03-14 14:20:00
***************************************************************/
int __stdcall NetClient_GetOsdTextEx(int _iLogonID,int _iChannel,void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_SetHolidayPlan
Description  �����ü��ռƻ���ز���		
Prameter(in) ��	_iLogonID:��¼ID
_iCmd: ������(HOLIDAY_PLAN_CMD_SCHEDULE)
_lpBuf: �������Ӧ�Ľṹ��ָ��(HOLIDAYPLAN_SCHEDULE)
_iBufSize:�ṹ���С		
return       ��	0:	success
<0:	failed
others       ��	2013-03-15 14:20:00
***************************************************************/
int __stdcall NetClient_SetHolidayPlan(int _iLogonID,int _iCmd,void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ��NetClient_GetHolidayPlan
Description  ����ü��ռƻ���ز���		
Prameter(in) ��		_iLogonID:��¼ID
_iCmd: ������(HOLIDAY_PLAN_CMD_SCHEDULE)
_iBufSize:�ṹ���С	
Prameter(in out) �� _lpBuf: �������Ӧ�Ľṹ��ָ��(HOLIDAYPLAN_SCHEDULE)
return       ��	0:	success
<0:	failed
others       ��	2013-03-15 14:20:00
***************************************************************/
int __stdcall NetClient_GetHolidayPlan(int _iLogonID,int _iCmd,void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ��	NetClient_SetCommonEnable
Description  ��	����ͨ��ҵ����ʹ�ܿ�����ر�
Prameter(in) ��	
_iLogonID��		��¼ID
_iEnableID: ����ID (����ֵ�ο�EnableID��Ӧ��)
_iChannelNo��	ͨ���ţ�����ʵ�����ȷ��������Ϣ�����ͨ���Ļ���ʹ��0x7FFFFFFF��������Ч������
_iEnableValue��ʹ��ֵ (1:ʹ�ܣ�0����ʹ��)       
return       ��    0����ȷ
other������
others       ��	2013-10-25 
***************************************************************/
int __stdcall NetClient_SetCommonEnable( int _iLogonID, int _iEnableID, int _iChannel, int _iEnableValue );

/***************************************************************
Function     ��	NetClient_GetCommonEnable
Description  ��	��ȡͨ��ҵ����ʹ�ܿ�����ر�
Prameter(in) ��	
_iLogonID��		��¼ID
_iEnableID: ����ID (����ֵ�ο�EnableID��Ӧ��)
_iChannelNo��	ͨ���ţ�����ʵ�����ȷ��������Ϣ�����ͨ���Ļ���ʹ��0x7FFFFFFF��������Ч������
Prameter(out) ��	_iEnableValue��ʹ��ֵ (1:ʹ�ܣ�0����ʹ��)       
return       ��    0����ȷ
other������
others       ��	2013-10-25 
***************************************************************/
int __stdcall NetClient_GetCommonEnable( int _iLogonID, int _iEnableID, int _iChannel, int* _iEnableValue );

/***************************************************************
Function     ��	NetClient_NetFileDownload
Description  ��	¼���ļ�����
Prameter(in out) :_ulConID������ID
Prameter(in) :	_iLogonID����¼ID
				_iCmd: �����   
				_lpBuf:����Ŷ�Ӧ�Ľṹ��ָ��
				_iBufSize:�ṹ���С     
return       �� 0����ȷ other������
others       ��	2013-04-17 
***************************************************************/
int __stdcall NetClient_NetFileDownload( unsigned int *_ulConID,int _iLogonID,int _iCmd, void* _lpBuf, int _iBufSize);


int __stdcall NetClient_Upgrade_V4(int _iLogonID, char* _cFileName, int _iUpgradeType, UPGRADE_NOTIFY_V4 _UpgradeNotify); 

/***************************************************************
Function     ��		NetClient_GetAudioCoderList
Description  ��		��ȡĳͨ��֧�ֵ���Ƶ������б�
Prameter(in) ��		_iLogonID����¼ID
					_iChannelNo��ͨ����
					_iStreamNo: ������
Prameter(out)��		_piList����Ӧ�ķֱ����б�
Prameter(in, out)�� _piLstCount ���б�Ԫ�ظ������������Σ�Ҳ����Σ������ʱ��ʾ��������С              
return       ��     0�� success
                    -1������������
                    other��fail
others       ��		2013-02-18 
***************************************************************/
int __stdcall NetClient_GetAudioCoderList( int _iLogonID, int _iChannel, int _iStreamNo, int* _piList, int* _piLstCount );

//�Զ�������ͨ��Э��(�ڲ�)�ӿ�˵��
/***************************************************************
Function ��	NetClient_InnerAutoTest
Description ��	ͨ���Զ������ýӿ�
Prameter(in) ��	_iLogonID�� ��½ID��ͨ��NetClient_Logon ����
				_iCmd:	    ������ 
				_pvTestBuf:  ���Բ���������ָ��
				_iBufSize:   ���Բ�����������С
return ��	   0��success
			   <0:	failed
others ��      13-05-29 11:48:39
***************************************************************/
int __stdcall NetClient_InnerAutoTest(int _iLogonID, int _iCmd, void* _pvTestBuf, int _iBufSize);

//add by wd @20130621 
/***************************************************************
Function     ��	  NetClient_SetJEPGSize
Description  ��	  �������ץ��ͼƬ�ֱ���
Prameter(in) ��	  _iLogonID:     ��¼ID
                  _iChannelNo:    ͨ����  0~n-1
                  _iWidth��         ץ��ͼƬ��---������ֵ
                  _iHeight:         ץ��ͼƬ��---�ߣ�����ֵ  
return       ��   0    ����ȷ
                  other������
***************************************************************/
int __stdcall NetClient_SetJEPGSize(int _iLogonID, int _iChannelNo, int _iWidth, int _iHeight);

/***************************************************************
Function     ��	NetClient_GetJEPGSize
Description  ��	��ȡ���ץ��ͼƬ�ֱ���
Prameter(in) ��	_iLogonID����¼ID
                _iChannelNo:  ͨ����  0~n-1

Prameter(out) ��	_iWidth:       ץ��ͼƬ��---������ֵ
                _iHeight:      ץ��ͼƬ��---�ߣ�����ֵ    
return       ��     0����ȷ
                other������
***************************************************************/
int __stdcall NetClient_GetJEPGSize(int _iLogonID, int _iChannelNo, int* _iWidth, int * _iHeight);

/***************************************************************
Function     ��NetClient_QueryDevStatus
Description  ����ѯ�豸״̬ (Э��ΪCMD*DEVSTATUS*iType)
Prameter(in) ��_iLogonID     ��¼ID
               _iType        �豸״̬  0-���
Prameter(out)��NULL
return         ��0  ��ȷ
                -1  ʧ��
***************************************************************/
int __stdcall NetClient_QueryDevStatus(int _iLogonID, int _iType);

/***************************************************************
Function          �� NetClient_GetDevStatus
Description      �� ��ȡ�豸״̬ 
Prameter(in)     �� _iLogonID     ��¼ID
       _iBufSize     �ṹ��Ĵ�С
Prameter(in/out)�� _ lpBuf        �ṹ��STR_DevStatus��ָ��
return         ��0  ��ȷ
                  -1 ʧ��
***************************************************************/
int __stdcall NetClient_GetDevStatus(int _iLogonID, void* _lpBuf, int _iBufSize);

//add end
/***************************************************************
Function          �� NetClient_GetHDTemplateIndex
Description		  �� ��ȡ����ģ���� 
Prameter(in)      �� _iLogonID     ��¼ID
					 _iChannelNo   ͨ���ţ�Ԥ����Ĭ��Ϊ0
Prameter(in/out)  �� _iTemplateIndex        ����ģ����
return            ��0  ��ȷ
                    -1 ʧ��
***************************************************************/
int __stdcall NetClient_GetHDTemplateIndex(int _iLogonID, int _iChannelNo /*= 0*/, int* _iTemplateIndex);

/***************************************************************
Function          �� NetClient_SetStreamInsertData
Description		  �� �����û��Զ�����������
Prameter(in)      �� _iLogonID     ��¼ID
                     _iType        0 ��������1��ʼ����ͬ��Ӧ�á�1��˫Ŀ�����֡ͬ�����
					 _pDataBuf	   �û����ݴ洢��
					 _iDataLen	   ���ݳ��ȣ�С��64�ֽ�	
return            ��0  ��ȷ
                    -1 ʧ��
***************************************************************/
int __stdcall NetClient_SetStreamInsertData(int _iLogonID, int _iChannelNo, int _iFlag, int _iType, void* _pDataBuf, int _iDataLen);

/***************************************************************
Function          �� NetClient_GetStreamInsertData
Description		  ��  ��ȡ�û��Զ���������������
Prameter(in)      �� _iLogonID     ��¼ID
                     _iType        0 ��������1��ʼ����ͬ��Ӧ�á�1��˫Ŀ�����֡ͬ�����
					 _pDataBuf	   �û����ݴ洢��
					 _iDataLen	   ���ݳ��ȣ�С��64�ֽ�	
return            ��0  ��ȷ
                    -1 ʧ��
***************************************************************/
int __stdcall NetClient_GetStreamInsertData(int _iLogonID, int _iChannelNo, int _iFlag, int _iType, void* _pDataBuf, int _iDataLen);

/***************************************************************
Function          ��  NetClient_GetOtherID
Description		  ��  ��ȡNVS OtherID��(ĿǰITS���ͨ����ID��ȡCPUID)��
Prameter(in)      �� _iLogonID		��¼ID
                     _iBufLen		����������
Prameter(out)		 _pcBuff		�洢OtherID�Ļ�������
return            ��0  ��ȷ
                    other������
***************************************************************/
int __stdcall NetClient_GetOtherID(int _iLogonID, void* _pcBuff, int _iBufLen);

/***************************************************************
Function		��	NetClient_SetDomePTZ
Description		��	����Ptz����
Prameter(in)	��	_iLogonID����¼ID
					_iChannelNo��ͨ����
					_lpBuf: �ṹ��(DOMEPTZ)ָ��
 					_iBufSize:�ṹ���С
return			��    0����ȷ
                    -1������
***************************************************************/
int __stdcall NetClient_SetDomePTZ(int _iLogonID, int _iChannelNo, void *_lpBuf, int _iBufSize);

/***************************************************************
Function		��	NetClient_GetDomePTZ
Description		��	����޸ĵ��û�Ȩ��
Prameter(in)	��	_iLogonID����¼ID
					_iChannelNo��ͨ����
					_lpBuf: �ṹ��(DOMEPTZ)ָ��
 					_iBufSize:�ṹ���С
return			��    0����ȷ
                    -1������
***************************************************************/
int __stdcall NetClient_GetDomePTZ(int _iLogonID, int _iChannelNo, void *_lpBuf, int _iBufSize);

/***************************************************************
Function		��	NetClient_GetUserDataInfo
Description		��	��õ�ǰ����ʵʱ�����û�����
Prameter(in)	��	_uiRecvID��NetClient_Startrecv���ص�ID
					_iFlag��GET_USERDATA_INFO_MIN~GET_USERDATA_INFO_MAX
					_pBuffer: ���ؽ���Ļ�����
 					_iSize:��������С
return			��    0����ȷ
                    -1������
***************************************************************/
int __stdcall NetClient_GetUserDataInfo(unsigned int _ulConID, int _iFlag, void*  _pBuffer, int _iSize);

/***************************************************************
Function     ��	NetClient_GetBroadcastMessage
Description  ��	��ȡͨ�ù㲥������Ϣ,�յ�WCM_BROADCAST_MSG��Ϣ�����
Prameter(in) ��	_iLogonID����¼ID
				_iBufSize:�ṹ��(BROADCAST_MSG)��С
Prameter(in out) ��_lpBuf: �ṹ��ָ�� 
return       �� 0����ȷ
				other������
others       ��	2013-08-20 
***************************************************************/
int __stdcall NetClient_GetBroadcastMessage( int _iLogonID,void *_lpBuf, int _iBufSize);

/***************************************************************
Function     ��	NetClient_GetModuleCapability
Description  ��	��ȡģ��֧�ֵĹ���
Prameter(in) ��	_iLogonID����¼ID
				_iModule��ģ����
Prameter(out) ��_uCaps: ģ����������λ�洢 
return       �� 0����ȷ
				other������
others       ��	2013-08-20 
***************************************************************/
int __stdcall NetClient_GetModuleCapability( int _iLogonID,int _iModule,unsigned int* _uCaps);

/***************************************************************
//	��׼���ݲ�֧�ִ˽ӿ�
Function     ��	NetClient_KeyboardCtrl
Description  ��	���ͼ��̿���Э��
Prameter(in) ��	_cIP���豸IP
			_iAction��������
			_iValue����������
return       �� 0����ȷ
			other������
***************************************************************/
int __stdcall NetClient_KeyboardCtrl(char* _pcIP, int _iAction,int _iValue);

/***************************************************************
Function     ��	NetClient_NetFileSetSchedule
Description  ��	��������¼��ģ�塢����¼�����ģ��
Prameter(in) ��	_iLogonID����¼ID
				_iChannel��ͨ����
				_iCmd:������(TTEMPLATE_CMD_SMAR��TTEMPLATE_CMD_VIDEO)
				_lpBuf: �ṹ��(SMAR_TTEMPLATE��VIDEO_TEMPLATE)ָ��
				_iBufSize:�ṹ���С
return       ��0����ȷ
				other������
others       ��	2013-09-06
***************************************************************/
int __stdcall NetClient_NetFileSetSchedule(int _iLogonID, int _iChannel,int _iCmd,void *_lpBuf, int _iBufSize);

/***************************************************************
Function     ��	NetClient_NetFileGetSchedule
Description  ��	�������¼��ģ�塢����¼�����ģ��
Prameter(in) ��	_iLogonID����¼ID
				_iChannel��ͨ����
				_iCmd:������(TTEMPLATE_CMD_SMAR��TTEMPLATE_CMD_VIDEO)
				_iBufSize:�ṹ���С
Prameter(in out) ��_lpBuf: �ṹ��(SMAR_TTEMPLATE��VIDEO_TEMPLATE)ָ��
return       ��0����ȷ
				other������
others       ��	2013-09-06
***************************************************************/
int __stdcall NetClient_NetFileGetSchedule(int _iLogonID, int _iChannel, int _iCmd, void *_lpBuf, int _iBufSize);

/***************************************************************
Function     ��	NetClient_SetDevConfig
Description  ��	�����豸��������Ϣ
Prameter(in) ��		[IN]	_iLogonID����¼ID
					[IN]    _iCommand���豸��������μ���������
                	[IN]	_iChannel��ͨ���ţ���������Ҫͨ���ţ��Ĳ�����Ч����ΪXFFFFFFFF����
					[IN]	_lpInBuffer���������ݻ�����ָ��
					[IN]	_iInBufferSize���������ݵĻ��������ȣ����ֽ�Ϊ��λ��
return       ��		0����ȷ
				other������
others       ��
***************************************************************/
int __stdcall NetClient_SetDevConfig(int _iLogonID, int _iCommand, int _iChannel, void *_lpInBuffer, int _iInBufferSize);

/***************************************************************
Function     ��	NetClient_GetDevConfig
Description  ��	��ȡ�豸��������Ϣ
Prameter(in) ��	[IN]_iLogonID����¼ID
				    [IN]	_iCommand���豸��������μ���������
                   	[IN]	_iChannel��ͨ���ţ���������Ҫͨ���ţ��Ĳ�����Ч����ΪXFFFFFFFF����
					[OUT]	_lpOutBuffer���������ݻ�����ָ��
					[IN]	_iOutBufferSize���������ݵĻ��������ȣ����ֽ�Ϊ��λ��
					[OUT]	_lpBytesReturned:ʵ���յ������ݳ���ָ�룬����ΪNULL
return       ��		0����ȷ
				other������
others       ��
***************************************************************/
int __stdcall NetClient_GetDevConfig(int _iLogonID, int _iCommand, int _iChannel, void *_lpOutBuffer, int _iOutBufferSize, int *_lpBytesReturned);

int __stdcall NetClient_SendCommand(int _iLogonID,  int _iCommand,  int _iChannel,  void* _pBuffer,  int _iBufferSize);
int __stdcall NetClient_RecvCommand(int _iLogonID,  int _iCommand,  int _iChannel,  void* _pBuffer,  int _iBufferSize);


/***************************************************************
Function     ��	NetClient_SetDevDiskConfig
Description  ��	�����豸�Ĵ���������Ϣ
Prameter(in) ��		[IN]	_iLogonID����¼ID
					[IN]    _iCommand���豸��������μ���������
                	[IN]	_iChannel��ͨ���ţ���������Ҫͨ���ţ��Ĳ�����Ч����ΪXFFFFFFFF
					[IN]	_lpInBuffer���������ݻ�����ָ��
					[IN]	_iInBufferSize���������ݵĻ��������ȣ����ֽ�Ϊ��λ��
return       ��		0����ȷ
				other������
others       ��
***************************************************************/
int __stdcall NetClient_SetDevDiskConfig (int _iLogonID, int _iCommand, int _iChannel, void *_lpInBuffer, int _iInBufferSize);

/***************************************************************
Function     ��	NetClient_GetDevDiskConfig
Description  ��	��ȡ�豸�Ĵ���������Ϣ
Prameter(in) ��	[IN]_iLogonID����¼ID
				    [IN]	_iCommand����������μ���������
                   	[IN]	_iChannel��ͨ���ţ���������Ҫͨ���ţ��Ĳ�����Ч����ΪXFFFFFFFF
					[OUT]	_lpInBuffer���������ݻ�����ָ��
					[IN]	_iInBufferSize���������ݵĻ��������ȣ����ֽ�Ϊ��λ��
					[OUT]	_lpBytesReturned:ʵ���յ������ݳ���ָ�룬����ΪNULL
return       ��		0����ȷ
				other������
others       ��
***************************************************************/
int __stdcall NetClient_GetDevDiskConfig (int _iLogonID, int _iCommand, int _iChannel, void *_lpOutBuffer, int _iOutBufferSize, int *_lpBytesReturned);


//TODO:����ģʽ�Ż������ӿ�

/***************************************************************
Function     ��	NetClient_StartServer
Description  ��	��������������
Prameter(in) ��		[IN]	_pcLocalIP�����ع���IP��ַ��ΪNULL��ʾSDK�Զ���ȡPC��ǰ��IP��ַ
				    [IN]	_iLocalPort�����ؼ����˿ںţ����û�����
return       ��		0����ȷ
				other������
others       ��2014-08-01
***************************************************************/
int __stdcall NetClient_StartServer(char *_pcLocalIP,  int _iLocalPort);

/***************************************************************
Function     ��	NetClient_DSMSetDirectoryServer
Description  ��	����Ŀ¼��������IP�ͼ����˿�
Prameter(in) ��		[IN]	_cpDServerIP1����Ŀ¼��������IP
				    [IN]	_iPort1����Ŀ¼�������ļ����˿�
					[IN]	_cpDServerIP2������Ŀ¼��������IP
					[IN]	_iPort2������Ŀ¼�������ļ����˿�
return       ��		0����ȷ
				other������
others       ��2014-08-01
***************************************************************/
int __stdcall NetClient_DSMSetDirectoryServer(char* _pcDServerIP1, int _iPort1, char* _pcDServerIP2, int _iPort2);

/***************************************************************
Function     ��	NetClient_Logon_V4
Description  ��	��¼ָ����������Ƶ��������Ŀ¼������
Prameter(in) ��		[IN]	_iLogonType:�������ͣ�0-����ģʽ��1-����ģʽ��2-��������
				    [IN]	_pvPara����¼�����ṹ��
                   	[IN]	_iInBufferSize���������ݵĻ��������ȣ����ֽ�Ϊ��λ��
return       ��		0����ȷ
				other������
others       ��2014-08-01
***************************************************************/
int __stdcall NetClient_Logon_V4(int _iLogonType, void *_pvPara, int _iInBufferSize);

/***************************************************************
Function     ��	NetClient_PlayBackControl
Description  ��	PlayBackControl
Prameter(in) ��		[IN]	_ulConID:Return value NetlClient_PlayBackByName or NetClient_PlayBackByFlag
				    [IN]	_iControlCode�� 
                   	[IN]	_pcInBuffer:
					[IN]	_iInLen:
					[IN]    _pcOutBuffer
					[IN]	_iOutLen
return       ��		connect iD
others       ��2015-09-15
***************************************************************/
int __stdcall NetClient_PlayBackControl(unsigned long _ulConID, int _iControlCode, void* _pcInBuffer, int _iInLen
										 , void* _pcOutBuffer, int *_iOutLen);
/***************************************************************
Function     ��	NetClient_StopPlayBack
Description  ��	StopPlayBack
Prameter(in) ��		[IN]	_ulConID:Return value NetlClient_PlayBackByName or NetClient_PlayBackByFlag
return       ��		connect iD
others       ��2015-09-15
***************************************************************/
int __stdcall NetClient_StopPlayBack(unsigned long _ulConID);

/***************************************************************
Function     ��	NetClient_PlayBack
Description  ��	PlayBackByFlag
Prameter(out):      [OUT]   _ulConID return value
Prameter(in) ��		[IN]	_iCmd:Play Back Flag 
				    [IN]	_PlayerParam��_PlayerParam
                   	[IN]	_hWnd:the handle of the play window
return       ��		connect iD
others       ��2015-09-14
***************************************************************/
int __stdcall NetClient_PlayBack(unsigned int* _ulConID, int _iCmd, PlayerParam* _PlayerParam, void* _hWnd);

int __stdcall NetClient_GetPseChInfo(int _iLogonID, void *_lpBuf, int _iBufSize);
int __stdcall NetClient_SetPseChProperty(int _iLogonID, int _iPseCh, void *_lpBuf, int _iBufSize);
int __stdcall NetClient_GetPseChProperty(int _iLogonID, int _iPseCh, void *_lpBuf, int _iBufSize);

//NVR channel talk zyb add 20141219
/***************************************************************
Function     ��	NetClient_ChannelTalkStart
Description  ��	������ǰ��nvrͨ����Ӧ�豸�ĶԽ�
Prameter(in) ��	[IN]_iLogonID����¼ID
				[IN]_iChannel��ͨ���ţ���0��ʼ
                [IN]_iUser��ȡֵ0����1��
                		0��sdk�ɼ����ݣ�����Ҫ����NetClient_InputChannelTalkingdata 
                        1:�ϲ���Ҫ����NetClient_InputChannelTalkingdata����pcm����
return       ��	0����ȷ
				other������
others       ��  ��
***************************************************************/
int __stdcall NetClient_ChannelTalkStart(int _iLogonID, int _iChannel, int _iUser);


/***************************************************************
Function     ��	NetClient_ChannelTalkEnd
Description  ��	������ǰ��nvrͨ����Ӧ�豸�ĶԽ�
Prameter(in) ��	[IN]_iLogonID����¼ID
				[IN]_iChannel��ͨ���ţ���0��ʼ
return       ��	0����ȷ.
				other������
others       �� ��
***************************************************************/
int __stdcall NetClient_ChannelTalkEnd(int _iLogonID, int _iChannel);

/***************************************************************
Function     ��	NetClient_InputChannelTalkingdata
Description  ��	������Ƶ����
Prameter(in) ��	[IN]_iLogonID����¼ID
				[IN]_iChannel��ͨ����[-1, ͨ����-1]��
                		-1������ǰ��nvr�Խ�ͨ��ʹ��ͬһ�����ݣ�Ŀǰ��֧��-1����
                [IN]_ucData���Խ�����
                [IN]_iLen:�Խ����ݳ���
return       ��	0����ȷ
				other������
others       ��   ��
***************************************************************/
int __stdcall NetClient_InputChannelTalkingdata(int _iLogonID, int _iChannel, unsigned char* _ucData, unsigned int _iLen); 

/***************************************************************
Function     ��	NetClient_ChannelTalkEnd
Description  ��	��ȡNVRͨ����Ӧǰ���豸�ĵ�ǰ�Խ�״̬
Prameter(in) ��	[IN]_iLogonID����¼ID
				[IN]_iChannel��ͨ���ţ���0��ʼ
				[OUT]_iTalkState:�Խ�״̬ 0��û�жԽ� 1���Ѿ�����ʼ�Խ� 2�����ڶԽ� 3���Ѿ�����ֹͣ�Խ� 4�������Զ��ָ��Խ�������
return       ��	0����ȷ.
				other������
others       ��	��
***************************************************************/
int __stdcall NetClient_GetChannelTalkingState(int _iLogonID, int _iChannel, int* _iTalkState);
//add end

/***************************************************************
Function     ��	NetClient_CapturePicture
Description  ��	ץ��ͼƬ
Prameter(in) ��	[IN] _ulConID������ID
				[IN] _iPicType��ͼƬ���� 1��BMP 2��JPG
				[IN] _cFileName: ͼƬ����·������
return       ��	0����ȷ
				other������
others       ��2015-03-05
***************************************************************/
int __stdcall NetClient_CapturePicture(unsigned int _ulConID, int _iPicType, char* _cFileName);

/***************************************************************
Function     ��	NetClient_CapturePicData
Description  ��	��ȡץ��ͼƬ����
Prameter(in) ��	[IN] _ulConID������ID
				[IN] _iPicType��ͼƬ���� 1��BMP 2��JPG
				[OUT] _pPicBuf: ͼƬ����
				[OUT] _iPicSize��ͼƬ���ݴ��롢������С
return       ��	0����ȷ
				other������
others       ��2015-09-21
***************************************************************/
int __stdcall NetClient_CapturePicData(unsigned int _ulConID, int _iPicType, char* _pPicBuf, int* _iPicSize);

int __stdcall NetClient_SetSDKWorkMode(int _iWorkMode);

/***************************************************************
Function     ��	NetClient_Query_V4
Description  ��	��ѯǰ���豸�ļ�
Prameter(in) ��	[IN] _iLogonID����¼ID
				[IN] _iCmd:		�豸��������
				[IN] _iChannel: ͨ����
				[IN] _pvCmdBuf: �������ݻ�����ָ��
				[IN] _iBufLen:  �������ݻ��������ݴ�С
return       ��	0����ȷ
				other������
others       ��2015-03-05
***************************************************************/
int __stdcall NetClient_Query_V4(int _iLogonID, int _iCmd, int _iChannel, void* _pvCmdBuf, int _iBufLen);

/***************************************************************
Function     :	NetClient_RebootDeviceByType
Description  :	Mutil reboot
Prameter(in) :	[IN] _iLogonID: Logon ID
				[IN] _iType:0-device, 1-GUI,2-APP
				[IN] _pvCmdBuf: input space
				[IN] _iBufLen: input space size
return       :	0:ok
				other:error
others       :2015-04-27
***************************************************************/
int __stdcall NetClient_RebootDeviceByType(int _iLogonID, int _iType, void* _pvCmdBuf, int _iBufLen);

/***************************************************************
Function     :	NetClient_StartDownload
Description  :	Start Download
Prameter(in) :	[IN] _iLogonID: Logon ID
				[IN] _iChannel: Channel Number
				[IN] _iDownloadMode:0-file, 1-time span
				[IN] _lpInBuf: input space
				[IN] _iInBufLen: input space size
				[OUT] _pulDownloadFd: output download fd
return       :	>=0:ok
				other:error
others       :2015-06-04
***************************************************************/
int	__stdcall NetClient_StartDownload(int _iLogonID, int _iChannel, int _iDownloadMode, void *_lpInBuf, int _iInBufLen
									  , unsigned long* _pulDownloadFd);
int	__stdcall NetClient_StopDownload(unsigned long _ulDownloadFd);

/***************************************************************
Function     :	NetClient_GetDownloadPos
Description  :	Get Download Pos
Prameter(in) :	[IN] _ulDownloadFd: download fd
				[OUT] _puiDownloadPos: output download pos
return       :	>=0:ok
				other:error
others       :2015-06-04
***************************************************************/
int	__stdcall NetClient_GetDownloadPos(unsigned long _ulDownloadFd, int* _puiDownloadPos);

/***************************************************************
Function     :	NetClient_ProxySend
Description  :	Proxy Send Protocol 
Prameter 	 :	[IN] _iProtocolType
				[IN] _blSend
return       :	>=0:ok             
other:error
others       :	2015.10.13 
***************************************************************/
int	__stdcall NetClient_ProxySend (int _iProtocolType, bool _blSend);


#ifdef __cplusplus
}
#endif
#endif

