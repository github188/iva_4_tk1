#ifndef __ANALY_REAL_TIME_H__
#define __ANALY_REAL_TIME_H__
#include "mq_upload.h"
#include "AnalyEvidenceMan.h"

void SendRealTimeAnalyPreview(const AnalyData * ptData);

void SendRealTimeAnalyTracks(int iChnID, const AnalyTracks * ptTracks);

void SendRealTimeAnalyWechat(const AnalyData * ptData);
#endif
