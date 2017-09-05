#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "include/oal_sem.h"

OAL_API sem_t *oal_sem_create(const char *name, int oflag, mode_t mode,unsigned int value)
{
	sem_t  *sem =  NULL;

#ifndef WIN32
	sem_unlink(name);

	sem = sem_open(name,oflag,mode,value);

	if(sem == SEM_FAILED)
		perror("oal_sem_create sem_open");

	/*int sval;
	sem_getvalue(sem, &sval);
	printf("sem %s value= %d\n",name,sval);*/
#endif

	return sem;
}


OAL_API sem_t *oal_sem_open(const char *name, int oflag)
{
	sem_t  *sem =  NULL;

#ifndef WIN32
	
	sem = sem_open(name,oflag);

	if(sem == SEM_FAILED)
		perror("sem_open");
#endif
	return sem;
}

OAL_API int oal_sem_wait(sem_t *sem)
{	
	int ret = -1;

#ifndef WIN32
	
	ret = sem_wait(sem);
	
    if(ret < 0)
        perror("oal_sem_wait");
#endif

    return ret;
}

OAL_API int oal_sem_post(sem_t *sem)
{
	int ret = -1;
	
#ifndef WIN32

	ret = sem_post(sem);

    if(ret < 0)
        perror("oal_sem_post");
#endif
       
    return ret;
}

OAL_API int oal_sem_close(sem_t *sem)
{	
	int ret = -1;

#ifndef WIN32
	ret = sem_close(sem);

    if(ret < 0)
        perror("sem_close");
#endif

    return ret;
}

OAL_API int oal_sem_del(const char *name)
{	
	int ret = -1;

#ifndef WIN32
	ret = sem_unlink(name);

	if(ret < 0)
		perror("sem_unlink");
#endif

    return ret;
}

