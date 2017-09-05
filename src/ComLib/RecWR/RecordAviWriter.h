#ifndef __RECORD_AVI_WRITER_H__
#define __RECORD_AVI_WRITER_H__

#include "RecordBaseWriter.h"

struct AVFormatContext;
struct AVStream;

class RecordAviWriter : public RecordBaseWriter
{
public:
	RecordAviWriter();
	~RecordAviWriter();

public:
	const char * FileSuffix(){return "avi";}
	int CreateNewFile(const char* filename);
    int SetVideoTrack(const unsigned char* sps,int spslen,
					  const unsigned char* pps,int ppslen,
					  int width, int height,int rate);
    int WriteVideoSample(unsigned char* data,int datasize,bool keyframe);
    void CloseFile();

private:
	AVFormatContext *m_fc;
	AVStream *m_h264VideoStream;
	int m_isHeaderWritten;
};

#endif//__RECORD_AVI_WRITER_H__