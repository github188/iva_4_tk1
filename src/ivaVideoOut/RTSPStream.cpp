#include "RTSPStream.h"
#include "oal_log.h"
#include "H264VideoStreamFramer.hh"
#include "H264VideoRTPSink.hh"
#include "H264Man.h"
#include "RtspMan.h"
#include "sps_decode.h"

H264FramedSource::H264FramedSource( UsageEnvironment& env, int iChnID )
	: FramedSource(env)
{
	m_iChnID = iChnID;
	memset(&m_metaData, 0, sizeof(RTMPMetadata));
}

H264FramedSource::~H264FramedSource()
{
	LOG_DEBUG_FMT("~~~~Chnnel%d H264 Source Destroy!\n", m_iChnID);
}

H264FramedSource* H264FramedSource::createNew( UsageEnvironment& env, int iChnID )
{
	return new H264FramedSource(env, iChnID);
}

unsigned int H264FramedSource::maxFrameSize() const  
{  
	return MAX_FRAME_SIZES;  
} 

void H264FramedSource::doGetNextFrame()
{
	int ret = -1;

	fFrameSize = 0;
	fNumTruncatedBytes = 0;
	fDurationInMicroseconds = FRAME_TIME;//40000;

	if (RtspMan::GetInstance()->CanIDoNow(m_iChnID))
	{
		H264_Data_Header h264_info = {0};
		ret = H264Man::GetInstance()->GetNewFrameData2(m_iChnID, h264_info, fTo);
		if(ret == 0)
		{
			if (m_metaData.nSpsLen <= 0)
			{
				if((*(fTo + 4)& 0x1F) == 0x07)
				{
					LOG_DEBUG_FMT("Find SPS");
					m_metaData.nSpsLen = h264_info.data_size-4;
					memcpy(m_metaData.Sps, fTo+4, h264_info.data_size-4);
				}
				else
				{
					LOG_DEBUG_FMT("Wait SPS, ignore...");
				}
				goto NextTask;
			}

			if (m_metaData.nPpsLen <= 0)
			{
				if((*(fTo + 4)& 0x1F) == 0x08)
				{
					LOG_DEBUG_FMT("Find PPS");
					m_metaData.nPpsLen = h264_info.data_size-4;
					memcpy(m_metaData.Pps, fTo+4, h264_info.data_size-4);
				}
				else
				{
					LOG_DEBUG_FMT("Wait PPS, ignore...");
				}
				goto NextTask;
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
			}

			fFrameSize = h264_info.data_size;

			if( fFrameSize > fMaxSize)
			{
				LOG_DEBUG_FMT("~~~~~Channel%d streaming date_size:%d fMaxSize:%d",m_iChnID, fFrameSize, fMaxSize);
				fNumTruncatedBytes = fFrameSize - fMaxSize;  
				fFrameSize = fMaxSize;  
			}  
			else
			{  
				fNumTruncatedBytes = 0;
			}

			fPresentationTime.tv_sec  = h264_info.timestamp/1000;
			fPresentationTime.tv_usec = (h264_info.timestamp%1000)*1000;

			fDurationInMicroseconds = 1000000/(m_metaData.nFrameRate>0?m_metaData.nFrameRate:FRAME_RATE);//FRAME_TIME;//40000;
		}
	}

NextTask:

	nextTask() = envir().taskScheduler().scheduleDelayedTask( 0,(TaskFunc*)FramedSource::afterGetting, this);//表示延迟0秒后再执行 afterGetting 函数
	return;
}


H264MediaSubssion * H264MediaSubssion::createNew( UsageEnvironment& env, int iChnID, Boolean reuseFirstSource )
{
	return new H264MediaSubssion( env, iChnID, reuseFirstSource);
}

H264MediaSubssion::H264MediaSubssion( UsageEnvironment& env, int iChnID, Boolean reuseFirstSource )
	: OnDemandServerMediaSubsession(env,reuseFirstSource)
{
	m_iChnID = iChnID;
}

H264MediaSubssion::~H264MediaSubssion()
{
}

FramedSource* H264MediaSubssion::createNewStreamSource( unsigned clientSessionId, unsigned& estBitrate )
{
	/* Remain to do : assign estBitrate */
	estBitrate = 640000;//estBitrate; // kbps, estimate

	// Create the video source:
	H264FramedSource *LiveSource = H264FramedSource::createNew(envir(), m_iChnID);
	return H264VideoStreamFramer::createNew(envir(), LiveSource); 	
}

RTPSink* H264MediaSubssion::createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic, FramedSource* /*inputSource*/)
{
	printf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);
	return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
	//return H264VideoRTPSink::createNew(envir(), rtpGroupsock, 96, 0, "H264");
}

char const* H264MediaSubssion::sdpLines()  
{
	static char fSDPLines[] = 
		"m=video 0 RTP/AVP 96\r\n"
		"c=IN IP4 0.0.0.0\r\n"
		"b=AS:96\r\n"
		"a=rtpmap:96 H264/90000\r\n"
		"a=fmtp:96 packetization-mode=1;profile-level-id=000000;sprop-parameter-sets=H264\r\n"
		"a=control:track1\r\n";
	return fSDPLines;
}
