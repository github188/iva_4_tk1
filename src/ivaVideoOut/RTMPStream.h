#ifndef __RTMP_STREAM_H__
#define __RTMP_STREAM_H__
#include "librtmp/rtmp.h"
#include <stdio.h>
#include "H264Man.h"


class CRTMPStream
{
public:
	CRTMPStream(void);
	~CRTMPStream(void);
public:
	// 连接到RTMP Server
	bool Connect(const char* url);
    // 断开连接
	void Close();
	// 是否已经连接
	bool IsConnected();
    // 发送MetaData
	bool SendMetadata(LPRTMPMetadata lpMetaData);
    // 发送H264数据帧
	bool SendH264Packet(unsigned char *data,unsigned int size,bool bIsKeyFrame,unsigned int nTimeStamp);
	// 发送H264文件
	bool SendH264Frame( NaluUnit &naluUnit );
private:
	// 发送数据
	int SendPacket(unsigned int nPacketType,unsigned char *data,unsigned int size,unsigned int nTimestamp);
private:
	RTMP* m_pRtmp;
	RTMPMetadata m_metaData;
	unsigned int m_tick;
	unsigned int m_last_update;

	unsigned char *body;
	int bodylen;
};

#endif//__RTMP_STREAM_H__