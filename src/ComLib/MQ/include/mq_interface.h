#ifndef __MQ_INTERFACE_H__
#define __MQ_INTERFACE_H__

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#ifndef WIN32
#include <sys/ipc.h>
#include <sys/msg.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MQ_API
	#ifndef WIN32
		#ifdef __cplusplus
			#define MQ_API extern "C"
		#else
			#define MQ_API
		#endif
	#else
		#ifdef MQ_EXPORTS
			#define MQ_API extern "C" __declspec(dllexport)
		#else
			#define MQ_API extern "C" __declspec(dllimport)
		#endif
	#endif
#endif


#define MSG_DATA_SIZE		(1024*8)
#define MSG_HEADER_SIZE		(4*sizeof(long))

typedef struct _MQBuf{
	long lDst;
	long lSrc;
	long lCmd;
	long lLen;
	unsigned char data[MSG_DATA_SIZE];
}MQ_BUF;

// 消息队列key
enum{
	MASTER_KEY	= 0x6000,	// ivaMaster监听的消息
	VIDEOIN_KEY	= 0x6001,	// ivaVideoIn监听的消息
	VIDEOOUT_KEY= 0x6002,	// ivaVideoOut监听的消息
	DECODE_KEY	= 0x6003,	// ivaDecode监听的消息
	RECORD_KEY	= 0x6004,	// ivaRecord监听的消息
	ANALY_KEY	= 0x6005,	// ivaAnaly监听的消息
	DATABASE_KEY= 0x6006,	// ivaDatabase监听的消息
	ONVIF_KEY	= 0x6007,	// ivaOnvif监听的消息
	UPLOAD_KEY	= 0x6008	// ivaUpload监听的消息
};

#define START_CMD		0x1000
#define MASTER_CMD		START_CMD+100
#define VIDEOIN_CMD		MASTER_CMD+100
#define VIDEOOUT_CMD	VIDEOIN_CMD+100
#define DECODE_CMD		VIDEOOUT_CMD+100
#define RECORD_CMD		DECODE_CMD+100
#define ANALY_CMD		RECORD_CMD+100
#define DATABASE_CMD	ANALY_CMD+100
#define ONVIF_CMD		DATABASE_CMD+100
#define UPLOAD_CMD		ONVIF_CMD+100

enum{
	MSG_TYPE_MASTER = 0x5000,
	MSG_TYPE_VIDEOIN, 
	MSG_TYPE_VIDEOOUT, 
	MSG_TYPE_DECODE, 
	MSG_TYPE_RECORD, 
	MSG_TYPE_ANALY,
	MSG_TYPE_DATABASE,
	MSG_TYPE_ONVIF,
	MSG_TYPE_UPLOAD,
	MSG_TYPE_BOA,
	MSG_TYPE_END
};

/*****************************************************************************
* 函数名称: Msg_Init
* 函数功能: 初始化消息队列
* 输入参数: msgKey-消息队列表示
* 输出参数: 无
* 函数返回: 成功，则返回消息队列ID，失败，则返回-1
******************************************************************************/
MQ_API int Msg_Init( int msgKey );

/*****************************************************************************
* 函数名称: Msg_Send
* 函数功能: 向消息队列写入数据
* 输入参数: qid-消息队列id pData-写入的数据 size-数据长度
* 输出参数: 无
* 函数返回: 成功，则返回0，失败，则返回-1
******************************************************************************/
MQ_API int Msg_Send( int qid, void *pData , int size );

/*****************************************************************************
* 函数名称: Msg_Recv
* 函数功能: 从消息队列阻塞的读取数据
* 输入参数: qid-消息队列id msg_type-消息类型 pData-接收缓存 size-缓存长度
* 输出参数: 无
* 函数返回: 成功，则返回拷贝的实际字节数，失败，则返回-1
******************************************************************************/
MQ_API int Msg_Recv( int qid, int msg_type, void *pData , int size );

/*****************************************************************************
* 函数名称: Msg_Recv_NoWait
* 函数功能: 从消息队列非阻塞读取数据
* 输入参数: qid-消息队列id msg_type-消息类型 pData-接收缓存 size-缓存长度
* 输出参数: 无
* 函数返回: 成功，则返回拷贝的实际字节数，失败，则返回-1
******************************************************************************/
MQ_API int Msg_Recv_NoWait( int qid, int msg_type, void *pData , int size );

/*****************************************************************************
* 函数名称: Msg_Kill
* 函数功能: kill消息队列
* 输入参数: qid-消息队列id
* 输出参数: 无
* 函数返回: 成功，则返回0，失败，则返回-1
******************************************************************************/
MQ_API int Msg_Kill( int qid );

/*****************************************************************************
* 函数名称: Wait_Msg
* 函数功能: 用于在有先后顺序的进程间，等待前一个进程的启动消息
* 输入参数: qid-消息队列id msg_type-消息类型 lCmd-命令类型
* 输出参数: 无
* 函数返回: 成功，则返回0，失败，则返回-1
******************************************************************************/
MQ_API int Wait_Service_Start(int qid, long lDst, long lCmd);

/*****************************************************************************
* 函数名称: Start_Next_Service
* 函数功能: 用于在有先后顺序的进程间,通知下一个进程自己已经启动
* 输入参数: qid-消息队列id lDst-目标进程 lSrc-来源进程 lCmd-启动命令
* 输出参数: 无
* 函数返回: 成功，则返回0，失败，则返回-1
******************************************************************************/
MQ_API int Start_Next_Service(int qid, long lDst, long lSrc, long lCmd);

#ifdef __cplusplus
};
#endif

#endif

