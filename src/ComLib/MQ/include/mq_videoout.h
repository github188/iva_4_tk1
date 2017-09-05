#ifndef __MQ_VIDEOOUT_H__
#define __MQ_VIDEOOUT_H__

#include "mq_interface.h"
#include "mq_record.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	VO_CMD_START = VIDEOOUT_CMD,
	VO_CMD_SET_STRATEGY,	// MASTER->VIDEOOUT
	VO_CMD_SET_RTMPSERVER,	// MASTER->VIDEOOUT
	VO_CMD_SET_ENABLE,		// MASTER->VIDEOOUT
	VO_CMD_SOURCE_CHANGED,	// MASTER->VIDEOOUT
	VO_CMD_END
};

enum {
	VO_PROTOCOL_NONE = 0,// 不输出
	VO_PROTOCOL_RTSP = 1,// RTSP输出
	VO_PROTOCOL_RTMP = 2 // RTMP输出
};

typedef struct _VoStrategy
{
	int iProtocol;	// 协议类型
}VoStrategy;

typedef struct _RtmpServer
{
	char szUrl[512];
}RtmpServer;


MQ_API int MQ_VideoOut_Init();
MQ_API int MQ_VideoOut_Close();
MQ_API int MQ_VideoOut_SetStrategy(const VoStrategy * ptStrategy);
MQ_API int MQ_VideoOut_SetRtmpServer(int iChnID, const RtmpServer * ptInfo);
MQ_API int MQ_VideoOut_SetEnable(int iChnID, bool bEnable);
MQ_API int MQ_VideoOut_SourceChange(int iChnID, const RecSrcStat *ptStat);
#ifdef __cplusplus
};
#endif

#endif//__MQ_RTMP_H__

