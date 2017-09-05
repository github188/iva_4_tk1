#ifndef __RTSP_STREAM_H__
#define __RTSP_STREAM_H__
#include "FramedSource.hh"
#include "h264_queue.h"
#include "OnDemandServerMediaSubsession.hh"
#include "H264Man.h"

#define FRAME_RATE 25
#define FRAME_TIME 1000000/FRAME_RATE
#define MAX_FRAME_SIZES 2*1024*1024

class H264FramedSource: public FramedSource
{
public:
	H264FramedSource(UsageEnvironment& env, int iChnID);
	virtual ~H264FramedSource();

	static H264FramedSource* createNew(UsageEnvironment& env, int iChnID);
	virtual void doGetNextFrame();

private:
	virtual unsigned int maxFrameSize() const;
	int m_iChnID;
	RTMPMetadata m_metaData;
};


class H264MediaSubssion: public OnDemandServerMediaSubsession
{
public:
	static H264MediaSubssion *createNew(UsageEnvironment& env, int iChnID, Boolean reuseFirstSource);

private:
	H264MediaSubssion(UsageEnvironment& env, int iChnID, Boolean reuseFirstSource);

	virtual ~H264MediaSubssion();

private: // redefined virtual functions
	int m_iChnID;

	virtual FramedSource* createNewStreamSource(unsigned clientSessionId, unsigned& estBitrate);

	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock, unsigned char rtpPayloadTypeIfDynamic,FramedSource* inputSource);

protected:
	virtual char const* sdpLines();
};


#endif //__RTSP_STREAM_H__