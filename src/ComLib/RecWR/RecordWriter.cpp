#include "include/RecordWriter.h"
#include "mq_record.h"
#include "RecordBaseWriter.h"
#include "RecordAviWriter.h"
#include "RecordMp4Writer.h"
#include "RecordFlvWriter.h"

WR_RECORD_API WRHandle WR_NewHandle( int iType )
{
	RecordBaseWriter * ptWriter = NULL;
	switch (iType)
	{
	case REC_FILE_MP4:
		ptWriter  = new RecordMp4Writer();
		break;
	case REC_FILE_AVI:
		ptWriter =  new RecordAviWriter();
		break;
	case REC_FILE_FLV:
		ptWriter =  new RecordFlvWriter();
		break;
	default:
		break;
	}
	return (WRHandle)ptWriter;
}

WR_RECORD_API const char * WR_FileSuffix( WRHandle handle )
{
	RecordBaseWriter * ptWriter = (RecordBaseWriter*)handle;
	if (ptWriter)
	{
		return ptWriter->FileSuffix();
	}
	else
	{
		return NULL;
	}
}

WR_RECORD_API int WR_OpenFile( WRHandle handle, const char* filename)
{
	RecordBaseWriter * ptWriter = (RecordBaseWriter*)handle;
	if (ptWriter)
	{
		return ptWriter->CreateNewFile(filename);
	}
	else
	{
		return -1;
	}
}

WR_RECORD_API int WR_SetVideoTrack( WRHandle handle, const unsigned char* sps, int spslen, const unsigned char* pps, int ppslen, int width, int height, int rate )
{
	RecordBaseWriter * ptWriter = (RecordBaseWriter*)handle;
	if (ptWriter)
	{
		return ptWriter->SetVideoTrack(sps,spslen,pps,ppslen,width,height,rate);
	}
	else
	{
		return -1;
	}
}

WR_RECORD_API int WR_WriteVideoSample( WRHandle handle,unsigned char* data,int datasize,bool keyframe )
{
	RecordBaseWriter * ptWriter = (RecordBaseWriter*)handle;
	if (ptWriter)
	{
		return ptWriter->WriteVideoSample(data, datasize, keyframe);
	}
	else
	{
		return -1;
	}
}

WR_RECORD_API void WR_CloseFile( WRHandle handle )
{
	RecordBaseWriter * ptWriter = (RecordBaseWriter*)handle;
	if (ptWriter)
	{
		ptWriter->CloseFile();
	}
}

WR_RECORD_API void WR_ReleaseHandle( WRHandle handle )
{
	RecordBaseWriter * ptWriter = (RecordBaseWriter*)handle;
	if (ptWriter)
	{
		delete ptWriter;
	}
}
