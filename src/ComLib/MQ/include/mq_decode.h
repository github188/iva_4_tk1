#ifndef __MQ_DECODE_H__
#define __MQ_DECODE_H__

#include "mq_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	DECODE_CMD_START = DECODE_CMD,
	DECODE_CMD_VIDEO_INFO,	// VIDEOIN->DECODE
	DECODE_CMD_END
};

typedef struct _DecodeVideoInfo
{
	int  iFrameRate;
	int  iWidth;
	int  iHeight;
}DecodeVideoInfo;

MQ_API int MQ_Decode_Init();
MQ_API int MQ_Decode_Close();
MQ_API int MQ_Decode_VideoInfo(int iChnID, DecodeVideoInfo* ptInfo);


//int Capture_To_Decode_Video_Info(void *buf ,int len);

#ifdef __cplusplus
};
#endif

#endif // __MQ_DECODE_H__

