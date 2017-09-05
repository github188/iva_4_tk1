#include "RTMPStream.h"
#include "oal_time.h"
#include "oal_log.h"
#include "librtmp/log.h"
#include "sps_decode.h"

enum
{
	FLV_CODECID_H264 = 7,
};

char * put_byte( char *output, uint8_t nVal )  
{  
	output[0] = nVal;  
	return output+1;  
}

char * put_be16(char *output, uint16_t nVal )  
{  
	output[1] = nVal & 0xff;  
	output[0] = nVal >> 8;  
	return output+2;  
}

char * put_be24(char *output,uint32_t nVal )  
{  
	output[2] = nVal & 0xff;  
	output[1] = nVal >> 8;  
	output[0] = nVal >> 16;  
	return output+3;  
}  

char * put_be32(char *output, uint32_t nVal )  
{  
	output[3] = nVal & 0xff;  
	output[2] = nVal >> 8;  
	output[1] = nVal >> 16;  
	output[0] = nVal >> 24;  
	return output+4;  
}  

char *  put_be64( char *output, uint64_t nVal )  
{  
	output=put_be32( output, nVal >> 32 );  
	output=put_be32( output, nVal );  
	return output;  
}

char * put_amf_string( char *c, const char *str )  
{  
	uint16_t len = strlen( str );  
	c=put_be16( c, len );  
	memcpy(c,str,len);  
	return c+len;  
}

char * put_amf_double( char *c, double d )  
{  
	*c++ = AMF_NUMBER;  /* type: Number */  
	{  
		unsigned char *ci, *co;  
		ci = (unsigned char *)&d;  
		co = (unsigned char *)c;  
		co[0] = ci[7];  
		co[1] = ci[6];  
		co[2] = ci[5];  
		co[3] = ci[4];  
		co[4] = ci[3];  
		co[5] = ci[2];  
		co[6] = ci[1];  
		co[7] = ci[0];  
	}  
	return c+8;  
}

CRTMPStream::CRTMPStream(void)
{
	m_pRtmp = NULL;
	memset(&m_metaData, 0, sizeof(RTMPMetadata));
	body = NULL;
	bodylen = 0;
	m_tick = 0;
	m_last_update = 0;

	//RTMP_LogSetLevel(RTMP_LOGALL);
	m_pRtmp = RTMP_Alloc();
	if(m_pRtmp)
	{
		RTMP_Init(m_pRtmp);
	}
}

CRTMPStream::~CRTMPStream(void)
{
	Close();
	if (body)
	{
		delete []body;
		body = NULL;
		bodylen = 0;
	}
}

bool CRTMPStream::Connect(const char* url)
{
	if (m_pRtmp == NULL)
	{
		return false;
	}

	if(RTMP_SetupURL(m_pRtmp, (char*)url)!=TRUE)
	{
		LOG_DEBUG_FMT("RTMP_SetupURL Error");
		return false;
	}
	RTMP_EnableWrite(m_pRtmp);
	if(RTMP_Connect(m_pRtmp, NULL)!=TRUE)
	{
		LOG_DEBUG_FMT("RTMP_Connect Error");
		return false;
	}
	LOG_DEBUG_FMT("RTMP_Connect OK");
	if(RTMP_ConnectStream(m_pRtmp,0)!=TRUE)
	{
		LOG_DEBUG_FMT("RTMP_ConnectStream Error");
		return false;
	}
	LOG_DEBUG_FMT("RTMP_ConnectStream OK");
	memset(&m_metaData, 0, sizeof(m_metaData));
	m_tick = 0;
	m_last_update = 0;
	return true;
}

void CRTMPStream::Close()
{
	if(m_pRtmp)
	{
		RTMP_Close(m_pRtmp);
		RTMP_Free(m_pRtmp);
		m_pRtmp = NULL;
	}
}

bool CRTMPStream::IsConnected()
{
	if (m_pRtmp == NULL)
	{
		return false;
	}

	if (RTMP_IsConnected(m_pRtmp))
		return true;
	else
		return false;
}

int CRTMPStream::SendPacket(unsigned int nPacketType,unsigned char *data,unsigned int size,unsigned int nTimestamp)
{
	if(m_pRtmp == NULL)
	{
		return FALSE;
	}

	RTMPPacket packet;
	RTMPPacket_Reset(&packet);
	RTMPPacket_Alloc(&packet,size);

	packet.m_packetType = nPacketType;
	packet.m_nChannel = 0x04;  
	packet.m_headerType = RTMP_PACKET_SIZE_LARGE;  
	packet.m_nTimeStamp = nTimestamp;  
	packet.m_nInfoField2 = m_pRtmp->m_stream_id;
	packet.m_nBodySize = size;
	memcpy(packet.m_body,data,size);

	int nRet = RTMP_SendPacket(m_pRtmp,&packet,0);

	RTMPPacket_Free(&packet);

	return nRet;
}

bool CRTMPStream::SendMetadata(LPRTMPMetadata lpMetaData)
{
	if(lpMetaData == NULL)
	{
		return false;
	}
	char body[1024] = {0};;
    
    char * p = (char *)body;  
	p = put_byte(p, AMF_STRING );
	p = put_amf_string(p , "@setDataFrame" );

	p = put_byte( p, AMF_STRING );
	p = put_amf_string( p, "onMetaData" );

	p = put_byte(p, AMF_OBJECT );  
	p = put_amf_string( p, "copyright" );

	p = put_byte(p, AMF_STRING );  
	p = put_amf_string( p, "firehood" );  

	p = put_amf_string( p, "width");
	p = put_amf_double( p, lpMetaData->nWidth);

	p = put_amf_string( p, "height");
	p = put_amf_double( p, lpMetaData->nHeight);

	p = put_amf_string( p, "framerate" );
	p = put_amf_double( p, lpMetaData->nFrameRate); 

	p = put_amf_string( p, "videocodecid" );
	p = put_amf_double( p, FLV_CODECID_H264 );

	p = put_amf_string( p, "" );
	p = put_byte( p, AMF_OBJECT_END  );

	int index = p-body;

	int iRet = SendPacket(RTMP_PACKET_TYPE_INFO,(unsigned char*)body,p-body,0);
	LOG_DEBUG_FMT("SendPacket RTMP_PACKET_TYPE_INFO %d", iRet);

	int i = 0;
	body[i++] = 0x17; // 1:keyframe  7:AVC
	body[i++] = 0x00; // AVC sequence header

	body[i++] = 0x00;
	body[i++] = 0x00;
	body[i++] = 0x00; // fill in 0;

	// AVCDecoderConfigurationRecord.
	body[i++] = 0x01; // configurationVersion
	body[i++] = lpMetaData->Sps[1]; // AVCProfileIndication
	body[i++] = lpMetaData->Sps[2]; // profile_compatibility
	body[i++] = lpMetaData->Sps[3]; // AVCLevelIndication 
    body[i++] = 0xff; // lengthSizeMinusOne  

    // sps nums
	body[i++] = 0xE1; //&0x1f
	// sps data length
	body[i++] = lpMetaData->nSpsLen>>8;
	body[i++] = lpMetaData->nSpsLen&0xff;
	// sps data
	memcpy(&body[i],lpMetaData->Sps,lpMetaData->nSpsLen);
	i= i+lpMetaData->nSpsLen;

	// pps nums
	body[i++] = 0x01; //&0x1f
	// pps data length 
	body[i++] = lpMetaData->nPpsLen>>8;
	body[i++] = lpMetaData->nPpsLen&0xff;
	// sps data
	memcpy(&body[i],lpMetaData->Pps,lpMetaData->nPpsLen);
	i= i+lpMetaData->nPpsLen;

	iRet = SendPacket(RTMP_PACKET_TYPE_VIDEO,(unsigned char*)body,i,0);
	LOG_DEBUG_FMT("SendPacket RTMP_PACKET_TYPE_VIDEO %d", iRet);
	return (iRet==TRUE?true:false);
}

bool CRTMPStream::SendH264Packet(unsigned char *data,unsigned int size,bool bIsKeyFrame,unsigned int nTimeStamp)
{
	if(data == NULL && size<11)
	{
		return false;
	}

	if (bodylen < size+9)
	{
		if (body)
		{
			delete []body;
			body = NULL;
			bodylen = 0;
		}

		body = new unsigned char[size+9];
		if (body == NULL)
		{
			return false;
		}
		bodylen = size+9;
	}

	int i = 0;
	if(bIsKeyFrame)
	{
		body[i++] = 0x17;// 1:Iframe  7:AVC
	}
	else
	{
		body[i++] = 0x27;// 2:Pframe  7:AVC
	}
	body[i++] = 0x01;// AVC NALU
	body[i++] = 0x00;
	body[i++] = 0x00;
	body[i++] = 0x00;

	// NALU size
	body[i++] = size>>24;
	body[i++] = size>>16;
	body[i++] = size>>8;
	body[i++] = size&0xff;

	// NALU data
	memcpy(&body[i],data,size);

	int iRet = SendPacket(RTMP_PACKET_TYPE_VIDEO, body,i+size,nTimeStamp)==TRUE?true:false;
	//LOG_DEBUG_FMT("SendPacket RTMP_PACKET_TYPE_VIDEO size:%d %d", size, iRet);

	return (iRet==TRUE?true:false);
}

bool CRTMPStream::SendH264Frame( NaluUnit &naluUnit )
{
	if (m_pRtmp == NULL)
	{
		return false;
	}

	//LOG_DEBUG_FMT("SendH264Frame %x %d", naluUnit.type, naluUnit.size);

	if (m_metaData.nSpsLen <= 0)
	{
		if (naluUnit.type == 0x07)
		{
			LOG_DEBUG_FMT("Find SPS");
			m_metaData.nSpsLen = naluUnit.size;
			memcpy(m_metaData.Sps,naluUnit.data,naluUnit.size);
			return true;
		}
		else
		{
			LOG_DEBUG_FMT("Wait SPS, ignore...");
			return true;
		}
	}

	if (m_metaData.nPpsLen <= 0)
	{
		if (naluUnit.type == 0x08)
		{
			LOG_DEBUG_FMT("Find PPS");
			m_metaData.nPpsLen = naluUnit.size;
			memcpy(m_metaData.Pps, naluUnit.data, naluUnit.size);
			return true;
		}
		else
		{
			LOG_DEBUG_FMT("Wait PPS, ignore...");
			return true;
		}
	}

	if (m_metaData.nWidth <= 0)
	{
		int width = 0,height = 0, fps=0;  
		h264_decode_sps(m_metaData.Sps, m_metaData.nSpsLen, width, height, fps);
		m_metaData.nWidth = width;
		m_metaData.nHeight = height;
		if(fps)
			m_metaData.nFrameRate = fps; 
		else
			m_metaData.nFrameRate = 25;
		LOG_DEBUG_FMT("h264_decode_sps width = %d, height = %d, fps = %d",width,height, fps);

		if (SendMetadata(&m_metaData) == false)
		{
			memset(&m_metaData, 0, sizeof(m_metaData));
			return false;
		}
	}

	if (m_metaData.nFrameRate <= 0)
	{
		return false;
	}

	unsigned int tick_gap = 1000/m_metaData.nFrameRate;

	if(naluUnit.type == 0x07 || naluUnit.type == 0x08)
	{
		//return 0;
	}

// 	uint32_t now = RTMP_GetTime();
// 	printf("%s:%d %s  RTMP_GetTime = %u\n",__FILE__, __LINE__, __FUNCTION__,now);
// 	if (tick_gap - now + m_last_update >0 && m_last_update > 0)
// 	{
// 		printf("%s:%d %s  usleep = %u *1000\n",__FILE__, __LINE__, __FUNCTION__,tick_gap - now + m_last_update);
// 		usleep((tick_gap-now+m_last_update)*1000);  
// 	}

	bool bKeyframe  = (naluUnit.type == 0x05) ? TRUE : FALSE;
	// ·¢ËÍH264Êý¾ÝÖ¡
	bool bRet = SendH264Packet(naluUnit.data, naluUnit.size, bKeyframe, m_tick);

// 	m_last_update = RTMP_GetTime();
// 	printf("%s:%d %s  RTMP_GetTime = %u\n",__FILE__, __LINE__, __FUNCTION__,m_last_update);
	m_tick += tick_gap;
	return bRet;  
}
