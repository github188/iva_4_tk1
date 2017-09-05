#include "RecordAviWriter.h"

#include <string>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
  //C99整数范围常量. [纯C程序可以不用, 而C++程序必须定义该宏.]
  #define  __STDC_LIMIT_MACROS

  //C99整数常量宏. [纯C程序可以不用, 而C++程序必须定义该宏.]
  #define  __STDC_CONSTANT_MACROS

  // for int64_t print using PRId64 format.
  #define __STDC_FORMAT_MACROS

  #ifdef _STDINT_H
  #undef _STDINT_H
  #endif
  #include <stdint.h>
#endif


#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

#ifdef __cplusplus
}
#endif

using namespace std;

RecordAviWriter::RecordAviWriter()
	: RecordBaseWriter()
{
	m_fc = NULL;
	m_h264VideoStream = NULL;
	m_isHeaderWritten = 0;
}

RecordAviWriter::~RecordAviWriter()
{
	CloseFile();
}

int RecordAviWriter::CreateNewFile(const char* filename)
{
	if (filename == NULL || m_fc)
	{
		return -1;
	}

	m_isHeaderWritten = 0;

	av_log_set_level(/*AV_LOG_DEBUG*/AV_LOG_QUIET);

	av_register_all();

	avformat_network_init();

	int iret = avformat_alloc_output_context2(&m_fc, NULL, "avi", filename);
	if (iret < 0)
	{
		char ssss[AV_ERROR_MAX_STRING_SIZE] = {0};
		printf("avformat_alloc_output_context2, %d %s\n",iret, av_make_error_string(ssss, AV_ERROR_MAX_STRING_SIZE ,iret));
		return -1;
	}

	m_h264VideoStream = avformat_new_stream(m_fc, NULL);
	if (m_h264VideoStream == NULL)
	{
		printf("avformat_new_stream failed\n");
		return -1;
	}

	return 0;
}

int RecordAviWriter::SetVideoTrack(const unsigned char* sps,int spslen,const unsigned char* pps,int ppslen,int width,int height,int rate)
{
	if (m_h264VideoStream == NULL || m_fc == NULL)
	{
		printf("m_h264VideoStream == NULL\n");
		return -1;
	}

	if (sps == NULL || pps == NULL)
	{
		printf("sps == NULL || pps == NULL\n");
		return -1;
	}

	AVCodecContext *c;
	c = m_h264VideoStream->codec;
	c->codec_type = AVMEDIA_TYPE_VIDEO;
	c->codec_id = AV_CODEC_ID_H264;
	c->width = width;
	c->height = height;
	c->time_base.den = rate;
	c->time_base.num = 1;
	c->ticks_per_frame = 2;

	unsigned char splitter[4] = {0, 0, 0, 1};
	int extraSize = 4 + spslen + 4 + ppslen + FF_INPUT_BUFFER_PADDING_SIZE;

	if (c->extradata) av_free(c->extradata);
	c->extradata = (unsigned char *)av_malloc(extraSize);
	memcpy(c->extradata, splitter, 4);
	memcpy(c->extradata + 4, sps, spslen);
	memcpy(c->extradata + 4 + spslen, splitter, 4);
	memcpy(c->extradata + 4 + spslen + 4, pps, ppslen);
	c->extradata_size = 4 + spslen + 4 + ppslen;

	int iret = avio_open(&m_fc->pb, m_fc->filename, 2);
	if (iret < 0)
	{
		char ssss[AV_ERROR_MAX_STRING_SIZE] = {0};
		printf("avio_open, %d %s\n",iret, av_make_error_string(ssss, AV_ERROR_MAX_STRING_SIZE ,iret));
		return -1;
	}

	if (avformat_write_header(m_fc, NULL) != 0)
	{
		char ssss[AV_ERROR_MAX_STRING_SIZE] = {0};
		printf("avformat_write_header, %d %s\n",iret, av_make_error_string(ssss, AV_ERROR_MAX_STRING_SIZE ,iret));
		return -1;
	}

	m_isHeaderWritten = 1;
	return 0;
}

int RecordAviWriter::WriteVideoSample(unsigned char* data, int datasize, bool keyframe)
{
	if (NULL == m_fc || NULL == m_h264VideoStream || NULL == data)
		return -1;

	if (!m_bWriteKeyFrame && !keyframe)
		return 0;// 第一帧写入关键帧

	if (keyframe)
		m_bWriteKeyFrame = true;

	// 跳过SPS和PPS帧
	//if ((*(data+4)&0x1F) == 0x07 || (*(data+4)&0x1F) == 0x08 )
	//	return 0;

	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.stream_index = m_h264VideoStream->index;
	pkt.data = data;
	pkt.size = datasize;
	pkt.flags = keyframe? AV_PKT_FLAG_KEY : 0;
	pkt.dts = pkt.pts = AV_NOPTS_VALUE;
	//pkt.pts = av_rescale_q((ptsInc++)*3000, m_h264VideoStream->codec->time_base,m_h264VideoStream->time_base);

	int iret = av_interleaved_write_frame(m_fc, &pkt);
	if (iret < 0)
	{
		char ssss[AV_ERROR_MAX_STRING_SIZE] = {0};
		printf("avformat_write_header, %d %s\n",iret, av_make_error_string(ssss, AV_ERROR_MAX_STRING_SIZE ,iret));
		return -1;
	}

	av_packet_unref(&pkt);
	return 0;
}

void RecordAviWriter::CloseFile()
{
	if (NULL == m_fc)
		return;

	if (m_isHeaderWritten)
		av_write_trailer(m_fc);

	avio_close(m_fc->pb);
	
	avformat_free_context(m_fc);
	
	m_fc = NULL;
}