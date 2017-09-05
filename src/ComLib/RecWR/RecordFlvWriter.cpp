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
		m_iWriteStat = 1;// �Ѿ�д����ͷ
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
	m_iWriteStat = 2;// �Ѿ�д����Script

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
		return 0;// ��һ֡д��ؼ�֡

	if (keyframe)
		m_bWriteKeyFrame = true;

	// ����SPS��PPS֡
	if ((*(data+4)&0x1F) == 0x07 || (*(data+4)&0x1F) == 0x08 )
		return 0;

	if( write_h264_frame(data+4, datasize-4, keyframe) < 0 )
		return -1;

	m_iWriteStat = 3;// �Ѿ�д����Video
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
		// 3�ֽ� ��ʽǩ�� �����ǡ�FLV��3����д��ĸ
		x264_put_tag( m_flv, "FLV" );

		// 1�ֽ� �汾
		x264_put_byte( m_flv, 0x01 );

		// 1�ֽ� ���ͱ�־λ 1��ʾֻ����Ƶ��5��ʾ����Ƶ����Ƶ
		x264_put_byte( m_flv, 1 );

		// 4�ֽ� �ļ���ƫ���� �汾1�У�9
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
		// 1�ֽ� ��ǩ����
		x264_put_byte( m_flv, FLV_TAG_TYPE_META );

		// 3�ֽ� ��ǩ��������С
		i_datasize_pos = m_flv->d_cur;// written at end of encoding
		x264_put_be24( m_flv, 0 );

		// 3�ֽ� ʱ���
		x264_put_be24( m_flv, 0 );

		// 1�ֽ� ��չʱ���
		x264_put_byte( m_flv, 0 );

		// 3�ֽ� ��ID ��0(�����0)
		x264_put_be24( m_flv, 0 );
	}

	{//--TAG Data-------------------------------------------
		{//��һ��AMF��
			// 1�ֽ� AMF�����ͣ�һ������02����ʾ�ַ���
			x264_put_byte( m_flv, AMF_DATA_TYPE_STRING );
			
			// 2�ֽ� �����ݳ��ȣ�һ������00 0A
			x264_put_be16( m_flv, 0x000A );
			
			// ������ һ������"onMetaData"
			x264_put_amf_string( m_flv, "onMetaData" );
		}

		{//�ڶ���AMF��
			// 1�ֽ� AMF�����ͣ�һ������08����ʾECMA����
			x264_put_byte( m_flv, AMF_DATA_TYPE_MIXEDARRAY );
		
			// 4�ֽ� ECMA����Ԫ�ظ���
			x264_put_be32( m_flv, 7 );

			{// ����Ԫ�� 2�ֽ����Ƴ���+����+ֵ����+ֵ

				// 1 width ��Ƶ���
				x264_put_amf_string( m_flv, "width" );
				x264_put_amf_double( m_flv, (double)width );

				// 2 height ��Ƶ�߶�
				x264_put_amf_string( m_flv, "height" );
				x264_put_amf_double( m_flv, (double)height );

				// 3 framerate ��Ƶ֡��
				x264_put_amf_string( m_flv, "framerate" );    
				x264_put_amf_double( m_flv, (double)rate );

				// 4 videocodecid ��Ƶ���뷽ʽ
				x264_put_amf_string( m_flv, "videocodecid" );
				x264_put_amf_double( m_flv, (double)FLV_CODECID_H264 );

				// 5 duration ʱ��
				x264_put_amf_string( m_flv, "duration" );
				m_duration_pos = m_flv->d_cur + m_flv->d_total;
				x264_put_amf_double( m_flv, 0 ); // written at end of encoding

				// 6 filesize �ļ���С
				x264_put_amf_string( m_flv, "filesize" );
				m_filesize_pos = m_flv->d_cur + m_flv->d_total;
				x264_put_amf_double( m_flv, 0 ); // written at end of encoding

				// 6 videodatarate ��Ƶ����
				x264_put_amf_string( m_flv, "videodatarate" );
				m_bitrate_pos = m_flv->d_cur + m_flv->d_total;
				x264_put_amf_double( m_flv, 0 ); // written at end of encoding
			}

			//���������־ ��00 00 09
			x264_put_byte( m_flv, 0x00 );
			x264_put_byte( m_flv, 0x00 );
			x264_put_byte( m_flv, AMF_END_OF_OBJECT );
		}
	}

	// ��дdatasize
	// ��ǰλ��-��ʼλ��-3(DataSize)-3(TimeStamp)-1(TimeStampEx)-3(StreamID)
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
		// 1�ֽ� ��ǩ����
		x264_put_byte( m_flv, FLV_TAG_TYPE_VIDEO );

		// 3�ֽ� ��ǩ��������С
		i_datasize_pos = m_flv->d_cur;// written at end of encoding
		x264_put_be24( m_flv, 0 );

		// 3�ֽ� ʱ���
		x264_put_be24( m_flv, 0 );

		// 1�ֽ� ��չʱ���
		x264_put_byte( m_flv, 0 );

		// 3�ֽ� ��ID ��0(�����0)
		x264_put_be24( m_flv, 0 );
	}

	{//--TAG Data-------------------------------------------
		// 1�ֽ� ֡����|����ID
		x264_put_byte( m_flv, FLV_FRAME_KEY|FLV_CODECID_H264 );

		// 1�ֽ� AVC����ͷ =00
		x264_put_byte( m_flv, 0x00 );

		// 3�ֽ� CTS(CompositionTime) = 0
		x264_put_be24( m_flv, 0x00 );

		{//AVCDecoderConfigurationRecord
			// 1�ֽ� �汾�� = 01
			x264_put_byte( m_flv, 0x01 );

			// 3�ֽ� SPS profile level id
			x264_put_byte( m_flv, sps[1] ); // AVC Profile SPS[1]
			x264_put_byte( m_flv, sps[2] ); // profile_compatibility  SPS[2]
			x264_put_byte( m_flv, sps[3] ); // profile_compatibility  SPS[2]

			// 1�ֽ� lengthSizeMinusOne = FF
			x264_put_byte( m_flv, 0xff );   // 6 bits reserved (111111) + 2 bits nal size length - 1 (11)

			// 1�ֽ� numOfSequenceParameterSets
			x264_put_byte( m_flv, 0xe1 );   // 3 bits reserved (111) + 5 bits number of sps (00001)
		
			// 2�ֽ� SPS����
			x264_put_be16( m_flv, spslen );

			// SPS
			flv_append_data( m_flv, (uint8_t *)sps, spslen );
		
			if(pps)
			{
				// 1�ֽ� PPS���� ����ѭ��������31��PPS
				x264_put_byte( m_flv, 1 );

				// 2�ֽ� PPS����
				x264_put_be16( m_flv, ppslen );

				// PPS
				flv_append_data( m_flv, (uint8_t *)pps, ppslen );
			}
		}
	}

	// ��дdatasize
	// ��ǰλ��-��ʼλ��-3(DataSize)-3(TimeStamp)-1(TimeStampEx)-3(StreamID)
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
		// 1�ֽ� ��ǩ����
		x264_put_byte( m_flv, FLV_TAG_TYPE_VIDEO );

		// 3�ֽ� ��ǩ��������С
		i_datasize_pos = m_flv->d_cur;// written at end of encoding
		x264_put_be24( m_flv, 0 );

		// 3�ֽ� ʱ���
		x264_put_be24( m_flv, m_cts );

		// 1�ֽ� ��չʱ���
		x264_put_byte( m_flv, m_cts >> 24 );

		// 3�ֽ� ��ID ��0(�����0)
		x264_put_be24( m_flv, 0 );
	}
	{//--TAG Data-------------------------------------------
		// 1�ֽ� ֡����|����ID
		if(keyframe)
			x264_put_byte( m_flv, FLV_FRAME_KEY|FLV_CODECID_H264);
		else
			x264_put_byte( m_flv, FLV_FRAME_INTER|FLV_CODECID_H264);

		// 1�ֽ� AVC NALU��Ԫ =1
		x264_put_byte( m_flv, 0x01 );

		// 3�ֽ� CTS(CompositionTime)
		x264_put_be24( m_flv, offset );

		{//AVCVIDEOPACKET

			// NALU��Ԫ�ĳ���
			x264_put_be32(m_flv, datasize);

			// NALU���ݣ�û���ĸ��ֽڵ�nalu��Ԫͷ
			flv_append_data( m_flv, data, datasize);
		}
	}

	// ��дdatasize
	// ��ǰλ��-��ʼλ��-3(DataSize)-3(TimeStamp)-1(TimeStampEx)-3(StreamID)
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
