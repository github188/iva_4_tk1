#ifndef __ALG_CFG_MAN_H__
#define __ALG_CFG_MAN_H__
#include "capacity.h"
#include "mq_analysis.h"
#include "AlgorithmInput.h"
#include "AlgorithmOutput.h"
#include "AnalyPlanTurns.h"
#include <map>
using namespace std;
typedef map<int, AbsPosition*> Preset2PosMap;

typedef struct _tagPLATE_BUFFER
{
	time_t t;
	char plate[16];
	struct _tagPLATE_BUFFER * next;
}PLATE_BUFFER;

typedef struct
{
	DetermineT		tDetermine;		// 判决条件
	EvidenceModelT	tEviModel;		// 取证模式
	ImgMakeupT		tImgMakeup1;	// 一次抓拍取证模式
	ImgMakeupT		tImgMakeup2;	// 二次抓拍取证模式
	ImgComposeT		tImgCom;		// 图片合成
	ImgOSDT		    tImgOsd;		// 图片OSD
	ImgQualityT		tImgQua;		// 图片质量
	RecEvidenceT    tRecCfg;		// 录像取证
	EventCodeT  	tCode;			// 违法代码
	FileNamingT     tNaming;		// 命名规则
}AlgEventCfg;

typedef struct
{
	SceneInfoT 	tInfo;		// 基本信息
	AnalyRuleT 	tRule;		// 分析规则
}AlgSceneCfg;

typedef struct
{
	bool 		bOpenAlg;					// 开启算法

	PlaceInfoT  tChnPlaceInfo;				// 通道地点信息
	AlgSceneCfg tSceneCfg[MAX_SCENE_NUM];	// 所有场景

	bool        bUsePlan;
	AnalyPlan   tRunPlan;					// 运行计划

	bool        bUseNotify;
}AlgChannelCfg;

typedef struct _tValidPresetSet
{
	int iPresetNum;
	tSinglePresetConf SinglePreset[MAX_SCENE_NUM];
}ValidPresetSet;

class AnalyCfgMan
{
public:
	static AnalyCfgMan* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	AnalyCfgMan();
	~AnalyCfgMan();

public://响应参数配置
	int SetAuthInfo(const AuthStatusT * ptInfo);
	int SetDevInfo(const DeviceInfoT * ptInfo);
	int SetRootPath(const char * pszRootPath);
	int SetStoreFull(const StoreFullStatusT * ptInfo);
	int SetLprCfg(const LPRCfg * ptInfo);
	int SetDispatchCtrl(const DispatchCtrlT * ptInfo);

	int SetChnAlgOpen(int iChnID, bool bOpen);
	int SetChnPlaceInfoT(int iChnID, const PlaceInfoT * ptInfo);
	int SetChnPlan(int iChnID, const AnalyPlan * ptInfo);
	int SetChnPlanOpen(int iChnID, bool bOpen);
	int SetChnNotifyOpen(int iChnID, bool bOpen);

	int SetChnPresetPos(int iChnID, int iPreset, const AbsPosition * ptInfo);

	int SetChnSceneInfo(int iChnID, int iScene, const SceneInfoT * ptInfo);
	int SetChnSceneRule(int iChnID, int iScene, const AnalyRuleT * ptInfo);

	int SetEventDetermine(int iEvent, const DetermineT * ptInfo);
	int SetEventEvidenceModel(int iEvent, const EvidenceModelT * ptInfo);
	int SetEventImgMakeup(int iEvent, int iEviModel, const ImgMakeupT * ptInfo);
	int SetEventImgCompose(int iEvent, const ImgComposeT * ptInfo);
	int SetEventImgOSD(int iEvent, const ImgOSDT * ptInfo);
	int SetEventImgQuality(int iEvent, const ImgQualityT * ptInfo);
	int SetEventRecEvidence(int iEvent, const RecEvidenceT * ptInfo);
	int SetEventCode(int iEvent, const EventCodeT * ptInfo);
	int SetEventFileNaming(int iEvent, const FileNamingT * ptInfo);

	int ClearChnRuleAnaly(int iChnID);

public:// 计划执行调用
	bool IsAuthorised();
	bool ChnAlgIsOpen(int iChnID);
	bool ChnPlanIsOpen(int iChnID);
	bool ChnNotifyIsOpen(int iChnID);
	bool IsNeedStopAnalyForFull();
	bool IsNeedStopWriteForFull();

	int GetDevInfo( DeviceInfoT * ptInfo);
	const char * GetRootPath();
	int GetLprCfg(LPRCfg * ptInfo);
	int GetDispatchCtrl(DispatchCtrlT * ptInfo);

	int ChnIsInPlan(int iChnID, int iWeekDay, int iTime);
	int GetChnPlaceInfoT(int iChnID, PlaceInfoT * ptInfo);

	int GetChnSceneInfo(int iChnID, int iScene, SceneInfoT * ptInfo);
	int GetChnSceneRule(int iChnID, int iScene, AnalyRuleT * ptInfo);

	int GetEventDetermine(int iEvent, DetermineT * ptInfo);
	int GetEventEvidenceModel(int iEvent, EvidenceModelT * ptInfo);
	int GetEventImgMakeup(int iEvent, int iEviModel, ImgMakeupT * ptInfo);
	int GetEventImgCompose(int iEvent, ImgComposeT * ptInfo);
	int GetEventImgOSD(int iEvent, ImgOSDT * ptInfo);
	int GetEventImgQuality(int iEvent, ImgQualityT * ptInfo);
	int GetEventRecEvidence(int iEvent, RecEvidenceT * ptInfo);
	int GetEventCode(int iEvent, EventCodeT * ptInfo);
	int GetEventFileNaming(int iEvent, FileNamingT * ptInfo);

public:
	int GetIllegeStopTime();
	int GetSceneGlobalCruise(int iChnID, SceneCruiseT *ptSceneCruise);

	int GetChnSceneIdByPreset(int iChnID, int iPresetID);
	int GetChnScenePresetCfg(int iChnID, int iSceneID, tSinglePresetConf* ptCfg);
	int GetChnAllPresetCfg(int iChnID, ValidPresetSet* ptCfg);

	int GetPresetPTZ(int iChnID, int iPresetID, AbsPosition * ptInfo);

public:
	bool SimilarPlateFilter(bool bEvent,char * plate);
	void InitPlateFilter(bool bEvent);
	void CheckDefualtHanZi(char * plate);
	bool PlateReasonableCheck(char * plate);//合理性检查

private:
	AuthStatusT      m_tAuthStat;
	DeviceInfoT		 m_tDevInfo;
	char             m_szRootPath[256];
	StoreFullStatusT m_tStoreFull;
	LPRCfg			 m_tLprCfg;
	DispatchCtrlT  	 m_tDispatchCtrl;
	AlgChannelCfg 	 m_tChnInfo[MAX_CHANNEL_NUM];
	AlgEventCfg		 m_tEventCfg[eIT_Event_Max];	// 所有事件规则
	Preset2PosMap    m_tPresetPosMap;
	pthread_mutex_t  m_tMutex;
	
	
	PLATE_BUFFER * m_pPlate1History;
	PLATE_BUFFER * m_pPlate2History;
	pthread_mutex_t m_tPlateHistoryMutex;

private:
	static AnalyCfgMan* m_pInstance;
};

int ReplaceKeyWord(char * str,const char * keywords, const char * value, int recursion=1);



#endif //__ALG_CFG_MAN_H__

