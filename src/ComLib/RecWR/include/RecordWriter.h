#ifndef __RECORD_WRITER_H__
#define __RECORD_WRITER_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WR_RECORD_API
	#ifndef WIN32
		#ifdef __cplusplus
			#define WR_RECORD_API extern "C"
		#else
			#define WR_RECORD_API
		#endif
	#else
		#ifdef WRITERECORDFILE_EXPORTS
			#define WR_RECORD_API extern  "C"   __declspec(dllexport)
		#else
			#define WR_RECORD_API extern  "C"   __declspec(dllimport)
		#endif
	#endif
#endif

typedef void * WRHandle;

WR_RECORD_API WRHandle WR_NewHandle(int iType);

WR_RECORD_API const char * WR_FileSuffix(WRHandle handle);

WR_RECORD_API int WR_OpenFile(WRHandle handle, const char* filename);

WR_RECORD_API int WR_SetVideoTrack(WRHandle handle, const unsigned char* sps, int spslen,
						  const unsigned char* pps, int ppslen,
						  int width, int height, int rate);

WR_RECORD_API int  WR_WriteVideoSample(WRHandle handle,unsigned char* data,int datasize,bool keyframe);

WR_RECORD_API void WR_CloseFile(WRHandle handle);

WR_RECORD_API void WR_ReleaseHandle(WRHandle handle);

#ifdef __cplusplus
}
#endif

#endif//__RECORD_WRITER_H__
