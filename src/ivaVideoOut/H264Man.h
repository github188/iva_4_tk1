#ifndef __H264_MAN_H__
#define __H264_MAN_H__
#include "h264_queue.h"
#include "capacity.h"
#include "pthread.h"

// NALUµ¥Ôª
typedef struct _NaluUnit
{
	int type;
	int size;
	unsigned char *data;
}NaluUnit;

typedef struct _RTMPMetadata
{
	// video, must be h264 type
	unsigned int	nWidth;
	unsigned int	nHeight;
	unsigned int	nFrameRate;		// fps
	unsigned int	nVideoDataRate;	// bps
	unsigned int	nSpsLen;
	unsigned char	Sps[1024];
	unsigned int	nPpsLen;
	//unsigned char	Pps[1024];
	unsigned char	Pps[2048];
	// audio, must be aac type
	bool	        bHasAudio;
	unsigned int	nAudioSampleRate;
	unsigned int	nAudioSampleSize;
	unsigned int	nAudioChannels;
	char		    pAudioSpecCfg;
	unsigned int	nAudioSpecCfgLen;

} RTMPMetadata,*LPRTMPMetadata;

class H264Man
{
public:
	static H264Man* GetInstance();
	static int Initialize();
	static void UnInitialize();
private:
	H264Man();
	~H264Man();

public:
	int Run();

public:
	int GetNewFrameData(int iChnID, H264_Queue_Node * ptNode);
	int GetNewFrameData2(int iChnID, H264_Data_Header &h264_info, unsigned char * frame_data);

private:
	H264_Queue		m_tH264SQ[MAX_CHANNEL_NUM];

private:
	static H264Man* m_pInstance;
};

#endif //__H264_MAN_H__


