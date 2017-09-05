#ifndef __RECORD_BASE_WRITER_H__
#define __RECORD_BASE_WRITER_H__

class  RecordBaseWriter
{
public:
	RecordBaseWriter();
	~RecordBaseWriter();
	
	virtual const char * FileSuffix() = 0;

    virtual int CreateNewFile(const char* filename) = 0;

    virtual int SetVideoTrack( const unsigned char* sps, int spslen,
							   const unsigned char* pps, int ppslen,
							   int width, int height, int rate) = 0;

    virtual int WriteVideoSample(unsigned char* data,int datasize,bool keyframe) = 0;

    virtual void CloseFile() = 0;
	
public:
	bool m_bWriteKeyFrame;//第一帧必须写入关键帧
};


#endif //__RECORD_BASE_WRITER_H__