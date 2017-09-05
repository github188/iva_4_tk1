#ifndef __COMM_QUEUE_H__
#define __COMM_QUEUE_H__


#ifdef __cplusplus
extern "C" {
#endif

#ifndef SQ_API
	#ifndef WIN32
		#ifdef __cplusplus
			#define SQ_API extern "C"
		#else
			#define SQ_API
		#endif
	#else
		#ifdef SQ_EXPORTS
			#define SQ_API extern  "C"   __declspec(dllexport)
		#else
			#define SQ_API extern  "C"   __declspec(dllimport)
		#endif
	#endif
#endif


#ifdef __cplusplus
};
#endif

#endif


