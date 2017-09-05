#ifndef __PAS_RUNNER_H_
#define __PAS_RUNNER_H_
#include "BaseRunner.h"
#include "AnalyCfgMan.h"

class PASRunner : public BaseRunner
{
public:
	PASRunner(int iChnID);
	~PASRunner();

public:
	void ResetCfg();

public:
	virtual void CheckPreset();
	virtual void ProcessFrame(YuvCacheNode * ptYuvNode);
	virtual void ResetAnaly();
	virtual int  CollectAlgResult(int iChnID, const void * ptAlgRes, AnalyData *ptData);
	virtual void ManualSnap(ManualSnapT *ptInfo);

public:
	virtual void CreateAlgChannel();
	virtual void DestroyAlgChannel();

private:
	void ProcessTracks(const tAlgResArray * ptAlgResult);

private:
	tSinglePresetConf m_tPreset;
	EvidenceModelT m_tEviModel;
	ImgMakeupT m_tImgMakeup;
	LPRCfg m_tLprCfg;
	DispatchCtrlT m_tCtrl;

	ValidPresetSet m_tValidPresets;
	bool m_bCreateAlg;
};

void TrackColor2RGB(int iColor, RGBColorT * ptRGB);

#endif
