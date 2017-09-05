#ifndef __H264_QUEUE_H__
#define __H264_QUEUE_H__

#include <semaphore.h>
#include "comm_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define H264_QUEUE_NUM			20	// ����֡��
#define H264_QUEUE_FRAME_MAX	2*1024*1024	// һ֡������С

enum {
    H264_QUEUE_DECODE_SERVICE = 1,	// ���ڽ���
    H264_QUEUE_RECORD_SERVICE,		// ����¼��
    H264_QUEUE_EVIDENCE_SERVICE,	// ����ȡ֤¼��
	H264_QUEUE_STREAM_SERVICE		// ��������
};

enum {
	P_FRAME = 0,	// P֡
	I_FRAME = 1,	// I֡
	B_FRAME = 2		// B֡
};

typedef struct
{
	unsigned int		frame_id;
	int					channel;
	int					frame_rate;
	int					width;
	int					height;
	int					frame_type;
	unsigned long long  timestamp;
	unsigned int		data_size;
}H264_Data_Header;

typedef struct
{
 	H264_Data_Header h264_info;
	unsigned char	 frame_data[H264_QUEUE_FRAME_MAX];
}H264_Queue_Node;

typedef struct
{
	// ͨ����ʶ
	int		iChnID;

	// �����ڴ湫����������
	int 	shm_fd;			// �����ڴ��ļ�������
	void *	shm_addr;		// �ڴ�ӳ���Ժ���ڴ��׵�ַ
	void *	queue;			// == shm_addr, ʵ�ʲ�����ָ�� 4���ֽ�=���µ�һ��I֡λ�� + H264_Queue_Node����
	sem_t*	sem_mutex;		// �ڴ��������	

	// ��������
	sem_t*	sem_syn_decode;
	sem_t*	sem_syn_record;
	sem_t*	sem_syn_evidence;
	sem_t*	sem_syn_stream;
	unsigned int wr_pos;	// ��һ֡д��λ��

	// ��������
	int 	service_type;	// ����������
	sem_t*	sem_syn;		// �����ĸ�֮һ
	int 	rd_flag;		// �Ƿ��Ѿ���ȡ�����ݵı��
	unsigned int rd_frameid;// �Ѿ���ȡ��֡��
} H264_Queue;


// ���������˵��õĽӿ�
SQ_API int H264_QueueCreate(H264_Queue * ptQueue);
SQ_API int H264_QueuePutFull(H264_Queue * ptQueue, const H264_Data_Header *ptHeader, const unsigned char * pFrameData);
SQ_API int H264_QueueRelease(H264_Queue * ptQueue);

// �������Ѷ˵��õĽӿ�
SQ_API int H264_QueueOpen(H264_Queue * ptQueue);
SQ_API int H264_QueueGetFull(H264_Queue * ptQueue, H264_Data_Header *ptHeader, unsigned char * pFrameData);
SQ_API int H264_QueueClose(H264_Queue * ptQueue);


#ifdef __cplusplus
};
#endif

#endif

