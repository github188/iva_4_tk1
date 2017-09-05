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

// ��Ϣ����key
enum{
	MASTER_KEY	= 0x6000,	// ivaMaster��������Ϣ
	VIDEOIN_KEY	= 0x6001,	// ivaVideoIn��������Ϣ
	VIDEOOUT_KEY= 0x6002,	// ivaVideoOut��������Ϣ
	DECODE_KEY	= 0x6003,	// ivaDecode��������Ϣ
	RECORD_KEY	= 0x6004,	// ivaRecord��������Ϣ
	ANALY_KEY	= 0x6005,	// ivaAnaly��������Ϣ
	DATABASE_KEY= 0x6006,	// ivaDatabase��������Ϣ
	ONVIF_KEY	= 0x6007,	// ivaOnvif��������Ϣ
	UPLOAD_KEY	= 0x6008	// ivaUpload��������Ϣ
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
* ��������: Msg_Init
* ��������: ��ʼ����Ϣ����
* �������: msgKey-��Ϣ���б�ʾ
* �������: ��
* ��������: �ɹ����򷵻���Ϣ����ID��ʧ�ܣ��򷵻�-1
******************************************************************************/
MQ_API int Msg_Init( int msgKey );

/*****************************************************************************
* ��������: Msg_Send
* ��������: ����Ϣ����д������
* �������: qid-��Ϣ����id pData-д������� size-���ݳ���
* �������: ��
* ��������: �ɹ����򷵻�0��ʧ�ܣ��򷵻�-1
******************************************************************************/
MQ_API int Msg_Send( int qid, void *pData , int size );

/*****************************************************************************
* ��������: Msg_Recv
* ��������: ����Ϣ���������Ķ�ȡ����
* �������: qid-��Ϣ����id msg_type-��Ϣ���� pData-���ջ��� size-���泤��
* �������: ��
* ��������: �ɹ����򷵻ؿ�����ʵ���ֽ�����ʧ�ܣ��򷵻�-1
******************************************************************************/
MQ_API int Msg_Recv( int qid, int msg_type, void *pData , int size );

/*****************************************************************************
* ��������: Msg_Recv_NoWait
* ��������: ����Ϣ���з�������ȡ����
* �������: qid-��Ϣ����id msg_type-��Ϣ���� pData-���ջ��� size-���泤��
* �������: ��
* ��������: �ɹ����򷵻ؿ�����ʵ���ֽ�����ʧ�ܣ��򷵻�-1
******************************************************************************/
MQ_API int Msg_Recv_NoWait( int qid, int msg_type, void *pData , int size );

/*****************************************************************************
* ��������: Msg_Kill
* ��������: kill��Ϣ����
* �������: qid-��Ϣ����id
* �������: ��
* ��������: �ɹ����򷵻�0��ʧ�ܣ��򷵻�-1
******************************************************************************/
MQ_API int Msg_Kill( int qid );

/*****************************************************************************
* ��������: Wait_Msg
* ��������: ���������Ⱥ�˳��Ľ��̼䣬�ȴ�ǰһ�����̵�������Ϣ
* �������: qid-��Ϣ����id msg_type-��Ϣ���� lCmd-��������
* �������: ��
* ��������: �ɹ����򷵻�0��ʧ�ܣ��򷵻�-1
******************************************************************************/
MQ_API int Wait_Service_Start(int qid, long lDst, long lCmd);

/*****************************************************************************
* ��������: Start_Next_Service
* ��������: ���������Ⱥ�˳��Ľ��̼�,֪ͨ��һ�������Լ��Ѿ�����
* �������: qid-��Ϣ����id lDst-Ŀ����� lSrc-��Դ���� lCmd-��������
* �������: ��
* ��������: �ɹ����򷵻�0��ʧ�ܣ��򷵻�-1
******************************************************************************/
MQ_API int Start_Next_Service(int qid, long lDst, long lSrc, long lCmd);

#ifdef __cplusplus
};
#endif

#endif

