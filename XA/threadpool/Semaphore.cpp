#include "Semaphore.h"
#include<stdio.h>
#if defined WIN32
string
errorString ()
{
    LPSTR messageBuffer;
    DWORD bufferLength;
    string message;
    
    //
    // Call FormatMessage() to allow for message 
    // text to be acquired from the system.
    //
    
    if (bufferLength = FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                       FORMAT_MESSAGE_IGNORE_INSERTS |
                                       FORMAT_MESSAGE_FROM_SYSTEM,
                                       0,
                                       GetLastError (),
                                       MAKELANGID (LANG_NEUTRAL,
                                                   SUBLANG_DEFAULT),
                                       (LPSTR) &messageBuffer,
                                       0,
                                       NULL))
    {
        message = messageBuffer;
        LocalFree (messageBuffer);
    }
    
    return message;
}
#endif

Semaphore::Semaphore (unsigned int value)
{
#if defined WIN32
    if ((_semaphore = ::CreateSemaphore (0, value, 0x7fffffff, 0)) == 0)
        printf("Could not create semaphore (%s)\n", errorString().c_str());
#elif HAVE_PTHREAD && HAVE_POSIX_SEMAPHORES
    if (::sem_init (&_semaphore, 0, value))
        printf("%s\n", "Cannot initialize semaphore");
#elif HAVE_PTHREAD && !HAVE_POSIX_SEMAPHORES
    if (int error = ::pthread_mutex_init (&_semaphore.mutex, 0))
        printf("%s (%d)\n", "Cannot initialize mutex", error);
    
    if (int error = ::pthread_cond_init (&_semaphore.nonZero, 0))
        printf("%s (%d)\n", "Cannot initialize condition variable",error);
    
    _semaphore.count = value;
    _semaphore.numWaiting = 0;
#endif
}


Semaphore::~Semaphore()
{
#if defined WIN32
    bool ok = ::CloseHandle (_semaphore) != FALSE;
    assert (ok);
#elif HAVE_PTHREAD && HAVE_POSIX_SEMAPHORES
    int error = ::sem_destroy (&_semaphore);
    assert (error == 0);
#elif HAVE_PTHREAD && !HAVE_POSIX_SEMAPHORES
    int error;
    if((error = ::pthread_cond_destroy (&_semaphore.nonZero)))
        printf("%s (%d)\n", "Cannot de-initialize condition variable",error);
    if((error = ::pthread_mutex_destroy (&_semaphore.mutex)))
        printf("%s (%d)\n", "Cannot de-initialize mutex", error);
#endif
}


void
Semaphore::wait()
{
#if defined WIN32
    if (::WaitForSingleObject (_semaphore, INFINITE) != WAIT_OBJECT_0)
        printf("Could not wait on semaphore (%s)\n", errorString().c_str());
#elif HAVE_PTHREAD && HAVE_POSIX_SEMAPHORES
    ::sem_wait (&_semaphore);
#elif HAVE_PTHREAD && !HAVE_POSIX_SEMAPHORES
    ::pthread_mutex_lock (&_semaphore.mutex);
    
    _semaphore.numWaiting++;
    
    while (_semaphore.count == 0)
    {
        if (int error = ::pthread_cond_wait (&_semaphore.nonZero,
                                             &_semaphore.mutex))
        {
            ::pthread_mutex_unlock (&_semaphore.mutex);
            
            printf("Cannot wait on condition variable (%d)\n",error);
        }
    }
    
    _semaphore.numWaiting--;
    _semaphore.count--;
    
    ::pthread_mutex_unlock (&_semaphore.mutex);
#endif
}


bool
Semaphore::tryWait()
{
#if defined WIN32
    return ::WaitForSingleObject (_semaphore, 0) == WAIT_OBJECT_0;
#elif HAVE_PTHREAD && HAVE_POSIX_SEMAPHORES
    return sem_trywait (&_semaphore) == 0;
#elif HAVE_PTHREAD && !HAVE_POSIX_SEMAPHORES
    ::pthread_mutex_lock (&_semaphore.mutex);
    
    if (_semaphore.count == 0)
    {
        ::pthread_mutex_unlock (&_semaphore.mutex);
        return false;
    }
    else
    {
        _semaphore.count--;
        ::pthread_mutex_unlock (&_semaphore.mutex);
        return true;
    }
#endif
}


void
Semaphore::post()
{
#if defined WIN32
    if (!::ReleaseSemaphore (_semaphore, 1, 0))
        printf("Could not post on semaphore (%s)\n",errorString().c_str());
#elif HAVE_PTHREAD && HAVE_POSIX_SEMAPHORES
    if (::sem_post (&_semaphore))
        printf("%s\n", "Post operation on semaphore failed");
#elif HAVE_PTHREAD && !HAVE_POSIX_SEMAPHORES
    ::pthread_mutex_lock (&_semaphore.mutex);
    
    if (_semaphore.numWaiting > 0)
    {
        if (int error = ::pthread_cond_signal (&_semaphore.nonZero))
        {
            ::pthread_mutex_unlock (&_semaphore.mutex);
            
            printf("Cannot signal condition variable (%d)\n",error);
        }
    }
    
    _semaphore.count++;
    ::pthread_mutex_unlock (&_semaphore.mutex);
#endif
}


int
Semaphore::value() const
{
#if defined WIN32
    LONG v = -1;
    
    if (!::ReleaseSemaphore (_semaphore, 0, &v) || v < 0)
        printf("Could not get value of semaphore (%s)\n", errorString().c_str());
    
    return v;
#elif HAVE_PTHREAD && HAVE_POSIX_SEMAPHORES
    int v;
    
    if (::sem_getvalue (&_semaphore, &v))
        printf("Cannot read semaphore value\n");
#elif HAVE_PTHREAD && !HAVE_POSIX_SEMAPHORES
    ::pthread_mutex_lock (&_semaphore.mutex);
    int v = _semaphore.count;
    ::pthread_mutex_unlock (&_semaphore.mutex);
#endif
    return v;
}