#ifndef __RECORD_FLV_WRITER_H__
#define __RECORD_FLV_WRITER_H__

#include "RecordBaseWriter.h"
#include "flv_bytestream.h"

class RecordFlvWriter : public RecordBaseWriter
{
public:
    RecordFlvWriter();
    ~RecordFlvWriter();

public:
	const char * FileSuffix(){return "flv";};
    int CreateNewFile(const char* filename);
	int SetVideoTrack(const unsigned char* sps,int spslen,
					  const unsigned char* pps,int ppslen,
					  int width, int height,int rate);
    int WriteVideoSample(unsigned char* data, int datasize, bool keyframe);
    void CloseFile();

private:
	int set_flv_header();
	int set_script_tag( int width, int height, int rate);
	int write_h264_headers( const unsigned char* sps,int spslen,const unsigned char* pps,int ppslen );
	int write_h264_frame( unsigned char* data,int datasize,bool keyframe);
	int close_file();

	void rewrite_amf_double( FILE *fp, uint64_t position, double value );

private:
    flv_buffer *m_flv;
	uint64_t m_duration_pos;
	uint64_t m_filesize_pos;
	uint64_t m_bitrate_pos;

	int64_t m_dts; //上一次时间戳
	int64_t m_cts; //当前时间戳

	int64_t m_iWriteframeCnt;
	int  m_rate;
	int  m_iWriteStat;
};

#endif // __RECORD_FLV_WRITER_H__
