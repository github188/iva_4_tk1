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
	// 通道标识
	int		iChnID;

	// 共享内存公共交换部分
	int 	shm_fd;			// 共享内存文件描述符
	void  *	shm_addr;		// 内存映射以后的内存首地址
	sem_t *	sem_mutex;		// 内存操作互斥	
	sem_t * sem_syn;		// 同步对象

	int  *  pFullFlag;		// == 标记满状态 第一组YUV_QUEUE_NUM个int
	int  *  pEmptyFlag;		// == 标记闲状态 第二组YUV_QUEUE_NUM个int
	Yuv_Queue_Node * ptHeadNode;// == 开始节点 

	// 生产者使用
	int	wr_num;				// 写入位置

	// 消费者使用
	int	rd_num;				// 读取位置
} Yuv_Queue;

// 数据生产端调用的接口
SQ_API int Yuv_QueueCreate(Yuv_Queue * ptQueue);
SQ_API Yuv_Queue_Node  *Yuv_QueueGetEmpty(Yuv_Queue * ptQueue);
SQ_API int Yuv_QueuePutFull(Yuv_Queue * ptQueue);
SQ_API int Yuv_QueueRelease(Yuv_Queue * ptQueue);

// 数据消费端调用的接口
SQ_API int Yuv_QueueOpen(Yuv_Queue * ptQueue);
SQ_API Yuv_Queue_Node  *Yuv_QueueGetFull(Yuv_Queue * ptQueue);
SQ_API int Yuv_QueuePutEmpty(Yuv_Queue * ptQueue);
SQ_API int Yuv_QueueClose(Yuv_Queue * ptQueue);

#ifdef __cplusplus
};
#endif

#endif

