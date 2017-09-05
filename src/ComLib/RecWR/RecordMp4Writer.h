#ifndef __RECORD_MP4_WRITER_H__
#define __RECORD_MP4_WRITER_H__

#include "RecordBaseWriter.h"
#include <mp4v2/mp4v2.h>

class RecordMp4Writer : public RecordBaseWriter
{
public:
    RecordMp4Writer();
    ~RecordMp4Writer();

public:
	const char * FileSuffix(){return "mp4";};
    int CreateNewFile(const char* filename);
	int SetVideoTrack(const unsigned char* sps,int spslen,
					  const unsigned char* pps,int ppslen,
					  int width, int height,int rate);
    int WriteVideoSample(unsigned char* data,int datasize,bool keyframe);
    void CloseFile();
	
private:
    MP4FileHandle _filehandle;
    MP4TrackId	  _videoid;
};

#endif // __RECORD_MP4_WRITER_H__
