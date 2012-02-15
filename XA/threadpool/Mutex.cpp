#include "Mutex.h"
#include<stdio.h>
Mutex::Mutex ()
{
#if defined WIN32
    ::InitializeCriticalSection (&_mutex);
#elif HAVE_PTHREAD
    if (int error = ::pthread_mutex_init (&_mutex, 0))
        printf("%s (%d)\n","Cannot initialize mutex", error);
#endif
}

Mutex::~Mutex ()
{
#if defined WIN32
    ::DeleteCriticalSection (&_mutex);
#elif HAVE_PTHREAD
    if(int error = ::pthread_mutex_destroy (&_mutex))
        printf("%s (%d)\n","Cannot de-initialize mutex", error);
#endif
}

void
Mutex::lock () const
{
#if defined WIN32
    ::EnterCriticalSection (&_mutex);
#elif HAVE_PTHREAD
    if (int error = ::pthread_mutex_lock (&_mutex))
        printf("%s (%d)\n", "Cannot lock mutex", error);
#endif
}


void
Mutex::unlock () const
{
#if defined WIN32
    ::LeaveCriticalSection (&_mutex);
#elif HAVE_PTHREAD
    if (int error = ::pthread_mutex_unlock (&_mutex))
        printf("%s (%d)\n", "Cannot unlock mutex", error);
#endif
}