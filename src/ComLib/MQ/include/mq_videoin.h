#ifndef __MQ_VIDEOIN_H__
#define __MQ_VIDEOIN_H__

#include "mq_interface.h"
#include "oal_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	VI_CMD_START = VIDEOIN_CMD,
	VI_CMD_SET_RTSP,	// ONVIF->VIDEOIN
	VI_CMD_END
};

typedef struct _RtspInfo
{
	char szUrl[URI_LEN];
	int  iWidth;
	int  iHeight;
	int  iFps;
}RtspInfo;

MQ_API int MQ_VideoIn_Init();
MQ_API int MQ_VideoIn_Close();

// Onvifµ÷ÓÃ
MQ_API int MQ_VideoIn_SetRtspInfo(int iChnID, RtspInfo *pRtsp);

#ifdef __cplusplus
};
#endif

#endif	//__MQ_VIDEOIN_H__

