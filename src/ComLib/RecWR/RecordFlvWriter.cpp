#include "RecordFlvWriter.h"
#include "oal_log.h"
#include <stdlib.h>
#include <string.h>

RecordFlvWriter::RecordFlvWriter()
	: RecordBaseWriter()
{
	m_flv = NULL;
	m_duration_pos = 0;
	m_filesize_pos = 0;
	m_bitrate_pos = 0;

	m_dts = 0;
	m_cts = 0;

	m_iWriteframeCnt = 0;
	m_rate = 25;

	m_iWriteStat = 0;
}

RecordFlvWriter::~RecordFlvWriter()
{
	CloseFile();
}

int RecordFlvWriter::CreateNewFile(const char* filename)
{
	if (m_flv)
	{
		return -1;
	}

    do
    {
        if(filename == NULL)
        {
            break;
        }

		m_flv = flv_create_writer(filename);
		if( !m_flv )
		{
			 break;
		}

		if( set_flv_header() < 0 )
		{
			break;
		}
		m_iWriteStat = 1;// 已经写入了头
		return 0;
    }while(0);

	close_file();

    return -1;
}

int RecordFlvWriter::SetVideoTrack(const unsigned char* sps, int spslen, const unsigned char* pps, int ppslen, int width,int height, int rate)
{
	if (m_flv == NULL || sps == NULL || rate <= 0)
		return -1;

	if (m_iWriteStat < 1)
		return -1;

	if( set_script_tag(width, height, rate) < 0 )
		return -1;

	if( write_h264_headers(sps, spslen, pps, ppslen) < 0 )
		return -1;

	m_cts = 0;
	m_dts = 0;
	m_rate = rate;
	m_iWriteframeCnt = 0;
	m_iWriteStat = 2;// 已经写入了Script

	return 0;
}

int RecordFlvWriter::WriteVideoSample(unsigned char* data, int datasize, bool keyframe)
{
	if (m_flv == NULL)
		return -1;

    if(data == NULL || datasize <=0)
        return -1;

	if (m_iWriteStat < 2)
		return -1;

	if (!m_bWriteKeyFrame && !keyframe)
		return 0;// 第一帧写入关键帧

	if (keyframe)
		m_bWriteKeyFrame = true;

	// 跳过SPS和PPS帧
	if ((*(data+4)&0x1F) == 0x07 || (*(data+4)&0x1F) == 0x08 )
		return 0;

	if( write_h264_frame(data+4, datasize-4, keyframe) < 0 )
		return -1;

	m_iWriteStat = 3;// 已经写入了Video
	m_iWriteframeCnt++;
	return 0;
}

void RecordFlvWriter::CloseFile()
{
	if (m_flv == NULL)
		return;

	close_file();
}

int RecordFlvWriter::set_flv_header()
{
	if (m_flv == NULL)
		return -1;

	{// FLV Header
		// 3字节 格式签名 必须是“FLV”3个大写字母
		x264_put_tag( m_flv, "FLV" );

		// 1字节 版本
		x264_put_byte( m_flv, 0x01 );

		// 1字节 类型标志位 1表示只有视频，5表示有视频和音频
		x264_put_byte( m_flv, 1 );

		// 4字节 文件体偏移量 版本1中＝9
		x264_put_be32( m_flv, 9 );
	}

	// PreviousTagSize = 0
	x264_put_be32( m_flv, 0 );

	return flv_flush_data( m_flv );
}

int RecordFlvWriter::set_script_tag( int width, int height, int rate)
{
	if (m_flv == NULL)
		return -1;

	int i_datasize_pos = 0;

	{//--TAG HEADER-------------------------------------------
		// 1字节 标签类型
		x264_put_byte( m_flv, FLV_TAG_TYPE_META );

		// 3字节 标签数据区大小
		i_datasize_pos = m_flv->d_cur;// written at end of encoding
		x264_put_be24( m_flv, 0 );

		// 3字节 时间戳
		x264_put_be24( m_flv, 0 );

		// 1字节 扩展时间戳
		x264_put_byte( m_flv, 0 );

		// 3字节 流ID ≡0(恒等于0)
		x264_put_be24( m_flv, 0 );
	}

	{//--TAG Data-------------------------------------------
		{//第一个AMF包
			// 1字节 AMF包类型，一般总是02，表示字符串
			x264_put_byte( m_flv, AMF_DATA_TYPE_STRING );
			
			// 2字节 包内容长度，一般总是00 0A
			x264_put_be16( m_flv, 0x000A );
			
			// 包内容 一般总是"onMetaData"
			x264_put_amf_string( m_flv, "onMetaData" );
		}

		{//第二个AMF包
			// 1字节 AMF包类型，一般总是08，表示ECMA数组
			x264_put_byte( m_flv, AMF_DATA_TYPE_MIXEDARRAY );
		
			// 4字节 ECMA数组元素个数
			x264_put_be32( m_flv, 7 );

			{// 数组元素 2字节名称长度+名称+值类型+值

				// 1 width 视频宽度
				x264_put_amf_string( m_flv, "width" );
				x264_put_amf_double( m_flv, (double)width );

				// 2 height 视频高度
				x264_put_amf_string( m_flv, "height" );
				x264_put_amf_double( m_flv, (double)height );

				// 3 framerate 视频帧率
				x264_put_amf_string( m_flv, "framerate" );    
				x264_put_amf_double( m_flv, (double)rate );

				// 4 videocodecid 视频编码方式
				x264_put_amf_string( m_flv, "videocodecid" );
				x264_put_amf_double( m_flv, (double)FLV_CODECID_H264 );

				// 5 duration 时长
				x264_put_amf_string( m_flv, "duration" );
				m_duration_pos = m_flv->d_cur + m_flv->d_total;
				x264_put_amf_double( m_flv, 0 ); // written at end of encoding

				// 6 filesize 文件大小
				x264_put_amf_string( m_flv, "filesize" );
				m_filesize_pos = m_flv->d_cur + m_flv->d_total;
				x264_put_amf_double( m_flv, 0 ); // written at end of encoding

				// 6 videodatarate 视频码率
				x264_put_amf_string( m_flv, "videodatarate" );
				m_bitrate_pos = m_flv->d_cur + m_flv->d_total;
				x264_put_amf_double( m_flv, 0 ); // written at end of encoding
			}

			//数组结束标志 ≡00 00 09
			x264_put_byte( m_flv, 0x00 );
			x264_put_byte( m_flv, 0x00 );
			x264_put_byte( m_flv, AMF_END_OF_OBJECT );
		}
	}

	// 回写datasize
	// 当前位置-起始位置-3(DataSize)-3(TimeStamp)-1(TimeStampEx)-3(StreamID)
    unsigned datasize = m_flv->d_cur - i_datasize_pos - 3 - 3 - 1 - 3;
    rewrite_amf_be24( m_flv, datasize, i_datasize_pos );

	// Previous Tag Size = 11 + DataSize
    x264_put_be32( m_flv, datasize + 11 );

	return flv_flush_data( m_flv );
}

int RecordFlvWriter::write_h264_headers( const unsigned char* sps,int spslen,const unsigned char* pps,int ppslen )
{
	if (m_flv == NULL || sps == NULL)
		return -1;

	int i_datasize_pos = 0;

	{//--TAG HEADER-------------------------------------------
		// 1字节 标签类型
		x264_put_byte( m_flv, FLV_TAG_TYPE_VIDEO );

		// 3字节 标签数据区大小
		i_datasize_pos = m_flv->d_cur;// written at end of encoding
		x264_put_be24( m_flv, 0 );

		// 3字节 时间戳
		x264_put_be24( m_flv, 0 );

		// 1字节 扩展时间戳
		x264_put_byte( m_flv, 0 );

		// 3字节 流ID ≡0(恒等于0)
		x264_put_be24( m_flv, 0 );
	}

	{//--TAG Data-------------------------------------------
		// 1字节 帧类型|编码ID
		x264_put_byte( m_flv, FLV_FRAME_KEY|FLV_CODECID_H264 );

		// 1字节 AVC序列头 =00
		x264_put_byte( m_flv, 0x00 );

		// 3字节 CTS(CompositionTime) = 0
		x264_put_be24( m_flv, 0x00 );

		{//AVCDecoderConfigurationRecord
			// 1字节 版本号 = 01
			x264_put_byte( m_flv, 0x01 );

			// 3字节 SPS profile level id
			x264_put_byte( m_flv, sps[1] ); // AVC Profile SPS[1]
			x264_put_byte( m_flv, sps[2] ); // profile_compatibility  SPS[2]
			x264_put_byte( m_flv, sps[3] ); // profile_compatibility  SPS[2]

			// 1字节 lengthSizeMinusOne = FF
			x264_put_byte( m_flv, 0xff );   // 6 bits reserved (111111) + 2 bits nal size length - 1 (11)

			// 1字节 numOfSequenceParameterSets
			x264_put_byte( m_flv, 0xe1 );   // 3 bits reserved (111) + 5 bits number of sps (00001)
		
			// 2字节 SPS长度
			x264_put_be16( m_flv, spslen );

			// SPS
			flv_append_data( m_flv, (uint8_t *)sps, spslen );
		
			if(pps)
			{
				// 1字节 PPS个数 后面循环存放最多31个PPS
				x264_put_byte( m_flv, 1 );

				// 2字节 PPS长度
				x264_put_be16( m_flv, ppslen );

				// PPS
				flv_append_data( m_flv, (uint8_t *)pps, ppslen );
			}
		}
	}

	// 回写datasize
	// 当前位置-起始位置-3(DataSize)-3(TimeStamp)-1(TimeStampEx)-3(StreamID)
	unsigned datasize = m_flv->d_cur - i_datasize_pos - 3 - 3 - 1 - 3;
	rewrite_amf_be24( m_flv, datasize, i_datasize_pos );

	// Previous Tag Size = 11 + DataSize
	x264_put_be32( m_flv, datasize + 11 );

	return flv_flush_data( m_flv );
}

int RecordFlvWriter::write_h264_frame( unsigned char* data, int datasize, bool keyframe )
{
	if (m_flv == NULL || data == NULL)
		return -1;

	int i_datasize_pos = 0;
	int64_t offset = m_cts - m_dts;
	m_dts = m_cts;

	{//--TAG HEADER-------------------------------------------
		// 1字节 标签类型
		x264_put_byte( m_flv, FLV_TAG_TYPE_VIDEO );

		// 3字节 标签数据区大小
		i_datasize_pos = m_flv->d_cur;// written at end of encoding
		x264_put_be24( m_flv, 0 );

		// 3字节 时间戳
		x264_put_be24( m_flv, m_cts );

		// 1字节 扩展时间戳
		x264_put_byte( m_flv, m_cts >> 24 );

		// 3字节 流ID ≡0(恒等于0)
		x264_put_be24( m_flv, 0 );
	}
	{//--TAG Data-------------------------------------------
		// 1字节 帧类型|编码ID
		if(keyframe)
			x264_put_byte( m_flv, FLV_FRAME_KEY|FLV_CODECID_H264);
		else
			x264_put_byte( m_flv, FLV_FRAME_INTER|FLV_CODECID_H264);

		// 1字节 AVC NALU单元 =1
		x264_put_byte( m_flv, 0x01 );

		// 3字节 CTS(CompositionTime)
		x264_put_be24( m_flv, offset );

		{//AVCVIDEOPACKET

			// NALU单元的长度
			x264_put_be32(m_flv, datasize);

			// NALU数据，没有四个字节的nalu单元头
			flv_append_data( m_flv, data, datasize);
		}
	}

	// 回写datasize
	// 当前位置-起始位置-3(DataSize)-3(TimeStamp)-1(TimeStampEx)-3(StreamID)
	unsigned _datasize = m_flv->d_cur - i_datasize_pos - 3 - 3 - 1 - 3;
	rewrite_amf_be24( m_flv, _datasize, i_datasize_pos );

	// Previous Tag Size = 11 + DataSize
	x264_put_be32( m_flv, _datasize + 11 );

	m_cts += 1000/(m_rate>0?m_rate:25);

	return flv_flush_data( m_flv );
}

void RecordFlvWriter::rewrite_amf_double( FILE *fp, uint64_t position, double value )
{
    uint64_t x = endian_fix64( dbl2int( value ) );
    fseek( fp, position, SEEK_SET );
    fwrite( &x, 8, 1, fp );
}

int RecordFlvWriter::close_file()
{
	if (m_flv == NULL)
		return -1;

	flv_flush_data( m_flv );

    if(( m_flv->fp ))
    {
		if (m_iWriteframeCnt > 0)
		{
			uint64_t filesize = ftell( m_flv->fp );

			double total_duration = (double)m_iWriteframeCnt / (m_rate>0?m_rate:25);

			rewrite_amf_double( m_flv->fp, m_duration_pos, total_duration );
			rewrite_amf_double( m_flv->fp, m_filesize_pos, filesize );
			rewrite_amf_double( m_flv->fp, m_bitrate_pos,  filesize * 8 / ( total_duration * 1000 ) );
		}
    }

    flv_destroy_writer( m_flv );
	m_flv = NULL;
    return 0;
}
