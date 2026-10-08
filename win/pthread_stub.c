#include <Windows.h>
#include "pthread.h"

#define LOCK_SUCCESS 0
#define LOCK_FAILURE (-1)


int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* mutexattr)
{
	int success = -1;
	if (mutex != NULL)
	{
		BOOL result = InitializeCriticalSectionAndSpinCount(&mutex->Section, 1000);
		if (result)
		{
			success = LOCK_SUCCESS;
		}
	}
	return success;

}

int pthread_mutex_destroy(pthread_mutex_t* mutex)
{
	if (mutex)
	{
		DeleteCriticalSection(&mutex->Section);
		return LOCK_SUCCESS;
	}
	return LOCK_FAILURE;
}

int pthread_mutex_lock(pthread_mutex_t* mutex)
{
	if (mutex)
	{
		EnterCriticalSection(&mutex->Section);
		return LOCK_SUCCESS;
	}
	return LOCK_FAILURE;
}

int pthread_mutex_unlock(pthread_mutex_t* mutex)
{
	if (mutex)
	{
		LeaveCriticalSection(&mutex->Section);
		return LOCK_SUCCESS;
	}
	return LOCK_FAILURE;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t* attr)
{
	if(attr)
	{
		attr->type = 0;
		return LOCK_SUCCESS;
	}
	return LOCK_FAILURE;
}

int pthread_mutexattr_init(pthread_mutexattr_t* attr)
{
	if(attr)
	{
		attr->type = 0;
		return LOCK_SUCCESS;
	}
	return LOCK_FAILURE;
}

int pthread_mutexattr_settype(pthread_mutexattr_t* attr, int type)
{
	if(attr)
	{
		attr->type = type;
		return LOCK_SUCCESS;
	}
	return LOCK_FAILURE;
}
