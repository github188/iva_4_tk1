#ifndef __ALGORITHM_DLL__
#define __ALGORITHM_DLL__

#include "AlgorithmList.h"


#ifdef __cplusplus
extern "C" {
#endif

//���㷨�⴫������
int algExchange(int ichannel, int idtype, void * pParam);

//����һ֡ͼ��
int algProcess(int ichannel, unsigned char * pFrame, int iddata, void * pResData);

#ifdef __cplusplus
	}
#endif

#endif
