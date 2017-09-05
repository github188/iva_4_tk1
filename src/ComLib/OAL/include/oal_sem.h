#ifndef __OSA_SEM_H__
#define __OSA_SEM_H__

#include <fcntl.h>	// Ϊ O_* ���� 
#include <sys/stat.h>// ΪȨ�޳��� 
#include <semaphore.h>
#include "oal_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

OAL_API sem_t *oal_sem_create(const char *name, int oflag, mode_t mode,unsigned int value);
OAL_API sem_t *oal_sem_open(const char *name, int oflag);
OAL_API int oal_sem_wait(sem_t *sem);
OAL_API int oal_sem_post(sem_t *sem);
OAL_API int oal_sem_close(sem_t *sem);
OAL_API int oal_sem_del(const char *name);

#ifdef __cplusplus
};
#endif

#endif

