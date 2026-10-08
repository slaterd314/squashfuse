#ifndef PTHREAD_T_INCLUDED
#define PTHREAD_T_INCLUDED

#include <synchapi.h>

typedef struct {
	CRITICAL_SECTION Section;
} pthread_mutex_t;

typedef struct
{
	int type;
} pthread_mutexattr_t;


int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* mutexattr);
int pthread_mutex_destroy(pthread_mutex_t* mutex);
int pthread_mutex_lock(pthread_mutex_t* mutex);
int pthread_mutex_unlock(pthread_mutex_t* mutex);
int pthread_mutexattr_destroy(pthread_mutexattr_t* attr);
int pthread_mutexattr_init(pthread_mutexattr_t* attr);
int pthread_mutexattr_settype(pthread_mutexattr_t* attr, int type);

#endif // PTHREAD_T_INCLUDED
