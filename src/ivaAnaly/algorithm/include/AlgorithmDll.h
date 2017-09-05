#ifndef __ALGORITHM_DLL__
#define __ALGORITHM_DLL__

#include "AlgorithmList.h"


#ifdef __cplusplus
extern "C" {
#endif

//与算法库传递数据
int algExchange(int ichannel, int idtype, void * pParam);

//处理一帧图像
int algProcess(int ichannel, unsigned char * pFrame, int iddata, void * pResData);

#ifdef __cplusplus
	}
#endif

#endif
