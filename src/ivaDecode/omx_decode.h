#ifndef OMX_DECODE_H
#define OMX_DECODE_H

#include <string.h>
#include <stdio.h>

//#define SAVE_YUV 		1

#ifdef DECODE_OPENMAX
#include "OMX_Types.h"
#include "OMX_Component.h"


#define MAX_INPUT_BUFFERS	10
#define MAX_OUTPUT_BUFFERS	10

#define NOTSET_U8		 ((OMX_U8)0xDE)
#define NOTSET_U16		 ((OMX_U16)0xDEDE)
#define NOTSET_U32		 ((OMX_U32)0xDEDEDEDE)
#define INIT_PARAM(_X_)  (memset(&(_X_), NOTSET_U8, sizeof(_X_)), ((_X_).nSize = sizeof (_X_)), (_X_).nVersion = vOMX)


typedef struct _OMXDecoder
{
	int  iChnID;

#ifdef SAVE_YUV
	FILE *fp_yuv;
#endif

	OMX_HANDLETYPE	pHandle;
    OMX_BUFFERHEADERTYPE * pInputBufPtr[MAX_INPUT_BUFFERS];
    OMX_BUFFERHEADERTYPE * pIOutputBufPtr[MAX_OUTPUT_BUFFERS];
    OMX_PARAM_PORTDEFINITIONTYPE tInputPortDef;
    OMX_PARAM_PORTDEFINITIONTYPE tOutputPortDef;
}OMXDecoder;

#else
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
  //C99整数范围常量. [纯C程序可以不用, 而C++程序必须定义该宏.]
  #define  __STDC_LIMIT_MACROS

  //C99整数常量宏. [纯C程序可以不用, 而C++程序必须定义该宏.]
  #define  __STDC_CONSTANT_MACROS

  // for int64_t print using PRId64 format.
  #define __STDC_FORMAT_MACROS

  #ifdef _STDINT_H
  #undef _STDINT_H
  #endif
  #include <stdint.h>
#endif


#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

typedef struct _FfmpegDecoder
{
	int  iChnID;
	
#ifdef SAVE_YUV
	FILE *fp_yuv;
#endif

	AVCodec  *ptAvCodec;
	AVCodecContext *ptAvCodecContext;
	AVPacket avpkt;
	AVFrame * ptAvFrame;
	int      findsps;
	int      findpps;
}FfmpegDecoder;


#ifdef __cplusplus
}
#endif

#endif


#endif 
