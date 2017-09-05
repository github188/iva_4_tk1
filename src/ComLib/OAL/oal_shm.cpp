#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "include/oal_shm.h"

OAL_API int oal_shm_create(const char *name, int oflag, mode_t mode)
{
	int ret = -1;
#ifndef WIN32
//	ret = shm_unlink(name);
	
	ret = shm_open(name,oflag, mode);

	if(ret < 0)
		perror("oal_shm_create shm_open");
#endif

	return ret;
}

OAL_API int oal_shm_open(const char *name, int oflag, mode_t mode)
{
	int ret = -1;

#ifndef WIN32
	ret = shm_open(name,oflag,mode);

	if(ret < 0)
		perror("oal_shm_open");
#endif

	return ret;
}

OAL_API void *oal_shm_init(size_t shm_size,int prot,int fd)
{
	int ret = -1;
	void *shm_addr = NULL;
	
#ifndef WIN32
	ret = ftruncate(fd,shm_size);
	if(ret < 0){
		perror("oal_shm_init ftruncate");
		goto end;
	}
	
	shm_addr = mmap(NULL, shm_size, prot, MAP_SHARED, fd, SEEK_SET);

	if (shm_addr == MAP_FAILED){
		perror("oal_shm_init mmap");
		return NULL ;
	}	
end:
#endif

	return shm_addr;
}

OAL_API void *oal_shm_mmap(size_t shm_size,int prot,int fd)
{
	void *shm_addr = NULL;

#ifndef WIN32
	shm_addr = mmap(NULL, shm_size, prot, MAP_SHARED, fd, SEEK_SET);

	if (shm_addr == MAP_FAILED){
		perror("oal_shm_mmap");
		return NULL ;
	}
#endif
	
	return shm_addr;
}

OAL_API int oal_shm_close(void *shm_addr,size_t shm_size,int fd)
{
	int ret  = -1;

#ifndef WIN32
	ret = close(fd);

    ret = munmap(shm_addr, shm_size);
	
 	if(ret < 0)
		perror("oal_shm_close");
#endif
	
	return ret;
}

OAL_API int oal_shm_del(const char *name)
{
	int ret  = -1;
	
#ifndef WIN32
    ret = shm_unlink(name);
	if(ret < 0)
		perror("oal_shm_del");
#endif
	
	return ret;
}

