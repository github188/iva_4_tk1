#ifndef __YUV_QUEUE_H__
#define __YUV_QUEUE_H__

#include <semaphore.h>
#include "comm_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define YUV_QUEUE_NUM							10

#define FRAME_WIDTH 1920
#define FRAME_HIGH  1088

#define YUV_Y_SIZE								FRAME_WIDTH*FRAME_HIGH
#define YUV_U_SIZE								YUV_Y_SIZE/4
#define YUV_V_SIZE								YUV_Y_SIZE/4

#define YUV_QUEUE_FRAME_MAX						FRAME_WIDTH*FRAME_HIGH + FRAME_WIDTH*FRAME_HIGH/2




typedef struct
{
	unsigned int frame_id;
	int channel;
	int frame_rate;
	int width;
	int height;
	unsigned long long timestamp;
	unsigned int data_size;
}Yuv_Data_Header;

typedef struct
{	
 	Yuv_Data_Header yuv_info;
	unsigned char 	yuv_data[YUV_QUEUE_FRAME_MAX];
}Yuv_Queue_Node;

typedef struct
{
	// ͨ����ʶ
	int		iChnID;

	// �����ڴ湫����������
	int 	shm_fd;			// �����ڴ��ļ�������
	void  *	shm_addr;		// �ڴ�ӳ���Ժ���ڴ��׵�ַ
	sem_t *	sem_mutex;		// �ڴ��������	
	sem_t * sem_syn;		// ͬ������

	int  *  pFullFlag;		// == �����״̬ ��һ��YUV_QUEUE_NUM��int
	int  *  pEmptyFlag;		// == �����״̬ �ڶ���YUV_QUEUE_NUM��int
	Yuv_Queue_Node * ptHeadNode;// == ��ʼ�ڵ� 

	// ������ʹ��
	int	wr_num;				// д��λ��

	// ������ʹ��
	int	rd_num;				// ��ȡλ��
} Yuv_Queue;

// ���������˵��õĽӿ�
SQ_API int Yuv_QueueCreate(Yuv_Queue * ptQueue);
SQ_API Yuv_Queue_Node  *Yuv_QueueGetEmpty(Yuv_Queue * ptQueue);
SQ_API int Yuv_QueuePutFull(Yuv_Queue * ptQueue);
SQ_API int Yuv_QueueRelease(Yuv_Queue * ptQueue);

// �������Ѷ˵��õĽӿ�
SQ_API int Yuv_QueueOpen(Yuv_Queue * ptQueue);
SQ_API Yuv_Queue_Node  *Yuv_QueueGetFull(Yuv_Queue * ptQueue);
SQ_API int Yuv_QueuePutEmpty(Yuv_Queue * ptQueue);
SQ_API int Yuv_QueueClose(Yuv_Queue * ptQueue);

#ifdef __cplusplus
};
#endif

#endif

