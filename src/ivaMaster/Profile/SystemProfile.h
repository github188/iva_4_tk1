#ifndef __CFG_MANAGEMENT__
#define __CFG_MANAGEMENT__
#include "Profile4Ini.h"
#include "../Manager/UserManager.h"
#include "../Manager/SystemMaintain.h"
#include "../Manager/AlarmManager.h"
#include "../Manager/DeviceManager.h"
#include "../Manager/VideoInManager.h"
#include "../Manager/RecManager.h"
#include "../Manager/NetworkManager.h"

#include "mq_analysis.h"
#include "mq_upload.h"
#include "mq_videoout.h"

#define CFG_DIR_PATH      			"../profiles/"  			//�����ļ�Ŀ¼
#define IVA_CFG_FILE_NAME 			"iva.ini"  					//���������ļ�


class SystemProfile
{
public:
	static int Initialize(); 
	static void UnInitialize(); 
	static SystemProfile* GetInstance();

public:
	void SendInitCfg2Other();

public:
	
	// ʱ��ͬ������
	int GetSynTimeInfo(SynTimeCFG* pInfo);
	int SetSynTimeInfo(const SynTimeCFG* pInfo);

	// ϵͳ����������Ϣ��д
	int GetTimerRebootInfo(TimerRebootCfg* pInfo);
	int SetTimerRebootInfo(const TimerRebootCfg* pInfo);

	// ���̸��ǲ���������Ϣ��д
	int GetDiskStategyInfo(DiskFullStrategy* pInfo);
	int SetDiskStategyInfo(const DiskFullStrategy* pInfo);

	// ���̹���������Ϣ��д
	int GetDiskUsageCfg(DiskUsageCfg* pInfo);
	int SetDiskUsageCfg(const DiskUsageCfg* pInfo);

	// ��ƵԴ������Ϣ��д
	int GetVideoSourceInfo(int iChnID, VISource* pInfo);
	int SetVideoSourceInfo(int iChnID, const VISource* pInfo);

	// ����Ŵ����
	int GetClickZoomProxy(int iChnID, ClickZoomProxy* pInfo);
	int SetClickZoomProxy(int iChnID, const ClickZoomProxy* pInfo);

	// ��ȡ�û���Ϣ
	int GetUserInfo(int iUserID, USER_INFO* pInfo);
	int SetUserInfo(int iUserID, const USER_INFO* pInfo);

	// Operate Log
	int GetOperateLogLimit(int & iOpLogMax);
	int SetOperateLogLimit(const int & iOpLogMax);

	// Alarm
	int GetAlarmStrategy(AlarmStrategy * pInfo);
	int SetAlarmStrategy(const AlarmStrategy * pInfo);

	int Get4GInfo(FourGCFG* pInfo);
	int Set4GInfo(const FourGCFG* pInfo);

	int GetWlanHotInfo(WlanHotCFG* pInfo);
	int SetWlanHotInfo(const WlanHotCFG* pInfo);

public:// RECORD
	int GetRecStrategy(RecStrategy* pInfo);
	int SetRecStrategy(const RecStrategy* pInfo);
	int GetChnRecPlan(int iChnID, RecPlan* pInfo);
	int SetChnRecPlan(int iChnID, const RecPlan* pInfo);
	int GetChnRecPlanSwitch(int iChnID, bool & bEnable);
	int SetChnRecPlanSwitch(int iChnID, bool bEnable);

public:// VIDEOOUT
	int GetVideoOutStrategy(VoStrategy* pInfo);
	int SetVideoOutStrategy(const VoStrategy* pInfo);
	int GetVideoOutRtmpSvr(int iChnID, RtmpServer* pInfo);
	int SetVideoOutRtmpSvr(int iChnID, const RtmpServer* pInfo);
	int GetVideoOutSwitch(int iChnID, bool & bEnable);
	int SetVideoOutSwitch(int iChnID, bool bEnable);

public:// ANALY
	int GetDeviceInfo(DeviceInfoT* pInfo);
	int SetDeviceInfo(const DeviceInfoT* pInfo);

	int GetLprCfg(LPRCfg* pInfo);
	int SetLprCfg(const LPRCfg* pInfo);

	int GetDispatch(DispatchCtrlT* pInfo);
	int SetDispatch(const DispatchCtrlT* pInfo);

	int GetChnAnalyPlan(int iChnID, AnalyPlan* pInfo);
	int SetChnAnalyPlan(int iChnID, const AnalyPlan* pInfo);

	int GetChnAnalyPlanSwitch(int iChnID, bool & bEnable);
	int SetChnAnalyPlanSwitch(int iChnID, bool bEnable);

	int GetChnPlace(int iChnID, PlaceInfoT* pInfo);
	int SetChnPlace(int iChnID, const PlaceInfoT* pInfo);

	int DeleteChnSceneInfo(int iChnID);
	int GetChnSceneInfo(int iChnID, int iScnID, SceneInfoT* pInfo);
	int SetChnSceneInfo(int iChnID, int iScnID, const SceneInfoT* pInfo);

	int GetPresetPosition(int iChnID, int iPresetNo, AbsPosition* pInfo);
	int SetPresetPosition(int iChnID, int iPresetNo, const AbsPosition* pInfo);

	int GetEventDetermine(int iEvent, DetermineT* pInfo);
	int SetEventDetermine(int iEvent, const DetermineT* pInfo);

	int GetEventEvidenceModel(int iEvent, EvidenceModelT* pInfo);
	int SetEventEvidenceModel(int iEvent, const EvidenceModelT* pInfo);

	int GetEventImgMakeUp(int iEvent, int iModel, ImgMakeupT* pInfo);
	int SetEventImgMakeUp(int iEvent, int iModel, const ImgMakeupT* pInfo);

	int GetEventImgCom(int iEvent, ImgComposeT* pInfo);
	int SetEventImgCom(int iEvent, const ImgComposeT* pInfo);

	int GetEventImgOsd(int iEvent, ImgOSDT* pInfo);
	int SetEventImgOsd(int iEvent, const ImgOSDT* pInfo);
		
	int GetEventImgQuality(int iEvent, ImgQualityT* pInfo);
	int SetEventImgQuality(int iEvent, const ImgQualityT* pInfo);

	int GetEventRec(int iEvent, RecEvidenceT* pInfo);
	int SetEventRec(int iEvent, const RecEvidenceT* pInfo);

	int GetEventCodeInfo(int iEvent, EventCodeT* pInfo);
	int SetEventCodeInfo(int iEvent, const EventCodeT* pInfo);

	int GetEventFileNamingInfo(int iEvent, FileNamingT* pInfo);
	int SetEventFileNamingInfo(int iEvent, const FileNamingT* pInfo);

public:// UPLOAD
	int GetUploadSvrInfo(UploadStrategy* pInfo);
	int SetUploadSvrInfo(const UploadStrategy* pInfo);
	int GetUploadSwitch(bool & bEnable);
	int SetUploadSwitch(bool bEnable);
	int GetUploadFtpAdv(FtpAdvance* pInfo);
	int SetUploadFtpAdv(const FtpAdvance* pInfo);

public:// Notify
	int GetNotifySvrInfo(NotifySvr* pInfo);
	int SetNotifySvrInfo(const NotifySvr* pInfo);
	int GetNotifySwitch(bool & bEnable);
	int SetNotifySwitch(bool bEnable);

private:
	SystemProfile();
	~SystemProfile(){};

	static SystemProfile* m_pInstance;
	
	CProfileIni* m_pCfgFile;
};

#endif
