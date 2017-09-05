#ifndef __OSA_SHM_H__
#define __OSA_SHM_H__

#ifndef WIN32
#include <sys/mman.h>
#endif

#include <sys/stat.h>// 为权限常量
#include <fcntl.h>// 为 O_* 常量 	 
#include <semaphore.h>
#include "oal_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

OAL_API int   oal_shm_create(const char *name, int oflag, mode_t mode);
OAL_API int   oal_shm_open(const char *name, int oflag, mode_t mode);
OAL_API void *oal_shm_init(size_t shm_size,int prot,int fd);
OAL_API void *oal_shm_mmap(size_t shm_size,int prot,int fd);
OAL_API int   oal_shm_close(void *shm_addr,size_t shm_size,int fd);
OAL_API int   oal_shm_del(const char *name);


#ifdef __cplusplus
};
#endif

#endif
