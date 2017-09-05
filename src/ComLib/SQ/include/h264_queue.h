#ifndef __H264_QUEUE_H__
#define __H264_QUEUE_H__

#include <semaphore.h>
#include "comm_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define H264_QUEUE_NUM			20	// 缓存帧数
#define H264_QUEUE_FRAME_MAX	2*1024*1024	// 一帧的最大大小

enum {
    H264_QUEUE_DECODE_SERVICE = 1,	// 用于解码
    H264_QUEUE_RECORD_SERVICE,		// 用于录像
    H264_QUEUE_EVIDENCE_SERVICE,	// 用于取证录像
	H264_QUEUE_STREAM_SERVICE		// 用于推流
};

enum {
	P_FRAME = 0,	// P帧
	I_FRAME = 1,	// I帧
	B_FRAME = 2		// B帧
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
	// 通道标识
	int		iChnID;

	// 共享内存公共交换部分
	int 	shm_fd;			// 共享内存文件描述符
	void *	shm_addr;		// 内存映射以后的内存首地址
	void *	queue;			// == shm_addr, 实际操作的指针 4个字节=最新的一个I帧位置 + H264_Queue_Node数组
	sem_t*	sem_mutex;		// 内存操作互斥	

	// 生产者用
	sem_t*	sem_syn_decode;
	sem_t*	sem_syn_record;
	sem_t*	sem_syn_evidence;
	sem_t*	sem_syn_stream;
	unsigned int wr_pos;	// 下一帧写入位置

	// 消费者用
	int 	service_type;	// 消费者类型
	sem_t*	sem_syn;		// 等于四个之一
	int 	rd_flag;		// 是否已经读取过数据的标记
	unsigned int rd_frameid;// 已经读取的帧号
} H264_Queue;


// 数据生产端调用的接口
SQ_API int H264_QueueCreate(H264_Queue * ptQueue);
SQ_API int H264_QueuePutFull(H264_Queue * ptQueue, const H264_Data_Header *ptHeader, const unsigned char * pFrameData);
SQ_API int H264_QueueRelease(H264_Queue * ptQueue);

// 数据消费端调用的接口
SQ_API int H264_QueueOpen(H264_Queue * ptQueue);
SQ_API int H264_QueueGetFull(H264_Queue * ptQueue, H264_Data_Header *ptHeader, unsigned char * pFrameData);
SQ_API int H264_QueueClose(H264_Queue * ptQueue);


#ifdef __cplusplus
};
#endif

#endif

