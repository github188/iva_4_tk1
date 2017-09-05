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
	// ���ӵ�RTMP Server
	bool Connect(const char* url);
    // �Ͽ�����
	void Close();
	// �Ƿ��Ѿ�����
	bool IsConnected();
    // ����MetaData
	bool SendMetadata(LPRTMPMetadata lpMetaData);
    // ����H264����֡
	bool SendH264Packet(unsigned char *data,unsigned int size,bool bIsKeyFrame,unsigned int nTimeStamp);
	// ����H264�ļ�
	bool SendH264Frame( NaluUnit &naluUnit );
private:
	// ��������
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