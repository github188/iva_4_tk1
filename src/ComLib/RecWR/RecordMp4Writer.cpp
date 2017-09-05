#include "RecordMp4Writer.h"
#include "oal_log.h"


RecordMp4Writer::RecordMp4Writer()
	: RecordBaseWriter()
{
	_filehandle = MP4_INVALID_FILE_HANDLE;
	_videoid    = MP4_INVALID_TRACK_ID;
}

RecordMp4Writer::~RecordMp4Writer()
{
	CloseFile();
}

int RecordMp4Writer::CreateNewFile(const char* filename)
{
    do
    {
        if(filename == NULL)
        {
            break;
        }

		_filehandle = MP4CreateEx(filename);

		if( _filehandle == MP4_INVALID_FILE_HANDLE )
        {
			LOG_ERROR_FMT("MP4Create file %s fialed.",filename); 
            break;
        }

        if(!MP4SetTimeScale(_filehandle, 90000))
        {
            break;
        }

        return 0;
    }while(0);

    return -1;
}

int RecordMp4Writer::SetVideoTrack(const unsigned char* sps,int spslen,const unsigned char* pps,int ppslen,int width,int height,int rate)
{
	if( _filehandle == MP4_INVALID_FILE_HANDLE )
	{
		return -1;
	}
	
    do
    {
        if(sps == NULL || spslen <= 0)
            break;

        if(pps == NULL || ppslen <=0)
            break;

        if(width <=0 || height <=0)
            break;

        if(rate <=0 )
            break;

		/*printf("SPS:");
		for(int i = 0; i < spslen; i++)
		{
			printf("%02X ",sps[i]);
		}
		printf("\n");
		printf("PPS:");
		for(int i = 0; i < ppslen; i++)
		{
			printf("%02X ",pps[i]);
		}
		printf("\n");*/

		//添加h264 track
        _videoid = MP4AddH264VideoTrack(_filehandle, 90000, 90000 / rate, width, height,sps[1],sps[2],sps[3],3);
        if (_videoid == MP4_INVALID_TRACK_ID)
        {
			printf("@@@@MP4AddH264VideoTrack INVALID\n");
            break;
        }
        MP4SetVideoProfileLevel(_filehandle, 0x7F);

        MP4AddH264SequenceParameterSet(_filehandle,_videoid, (uint8_t*)sps, spslen);
        MP4AddH264PictureParameterSet(_filehandle,_videoid, (uint8_t*)pps, ppslen);

		//添加aac音频 
		//MP4TrackId audio = MP4AddAudioTrack(_filehandle, 48000, 1024, MP4_MPEG4_AUDIO_TYPE); 
		//if (audio == MP4_INVALID_TRACK_ID) 
		//{ 
        //    break;
		//} 
		//MP4SetAudioProfileLevel(_filehandle, 0x2); 

        return 0;
    }while(0);

    return -1;
}

int RecordMp4Writer::WriteVideoSample(unsigned char* data, int datasize, bool keyframe)
{
	if( _filehandle == MP4_INVALID_FILE_HANDLE || _videoid == MP4_INVALID_TRACK_ID)
	{
		return -1;
	}

    if(data == NULL || datasize <=0)
    {
        return -1;
    }

	if (!m_bWriteKeyFrame && !keyframe)
		return 0;// 第一帧写入关键帧

	if (keyframe)
		m_bWriteKeyFrame = true;

	// 跳过SPS和PPS帧
	if ((*(data+4)&0x1F) == 0x07 || (*(data+4)&0x1F) == 0x08 )
		return 0;

 	char naldata[4];
	naldata[0] = data[0];
	naldata[1] = data[1];
	naldata[2] = data[2];
	naldata[3] = data[3];
	int nalsize = datasize-4;
	data[0] = (nalsize&0xff000000)>>24;
	data[1] = (nalsize&0x00ff0000)>>16;
	data[2] = (nalsize&0x0000ff00)>>8;
	data[3] = nalsize&0x000000ff;

	bool bOK = MP4WriteSample(_filehandle, _videoid, (uint8_t*)data,datasize, MP4_INVALID_DURATION, 0, keyframe);
	data[0] = naldata[0];
	data[1] = naldata[1];
	data[2] = naldata[2];
	data[3] = naldata[3];
	if(bOK)
    {
        return 0;
    }
    else
    {
    	printf("&&&&&&&&&&&&&&&&&&&MP4WriteSample failed.datasize=%d\n",datasize);
        return -1;
    }
}

void RecordMp4Writer::CloseFile()
{
	if( _filehandle == MP4_INVALID_FILE_HANDLE )
	{
		return;
	}
	
	MP4Close(_filehandle);
	_filehandle = MP4_INVALID_FILE_HANDLE;
}


