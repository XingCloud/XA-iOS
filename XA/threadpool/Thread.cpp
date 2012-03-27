#include "Thread.h"
#include<stdio.h>
bool
supportsThreads ()
{
#if (defined WIN32) || HAVE_PTHREAD
	return true;
#else
    return false;
#endif
}

#if HAVE_PTHREAD
extern "C"
{
    typedef void * (* Start) (void *);
}
#endif

#if defined WIN32
unsigned __stdcall
#elif HAVE_PTHREAD
void
#endif
threadLoop (void * t)
{
    (reinterpret_cast<Thread*>(t))->run();
#if defined WIN32
    _endthreadex (0);
    return 0;
#endif
}

Thread::Thread ()
{
#if (defined WIN32) || HAVE_PTHREAD
#else
    printf("%s\n", "Threads not supported on this platform");
#endif
}


Thread::~Thread ()
{
#if defined WIN32
    DWORD status = ::WaitForSingleObject (_thread, INFINITE);
    assert (status ==  WAIT_OBJECT_0);
    bool ok = ::CloseHandle (_thread) != FALSE;
    assert (ok);
#elif HAVE_PTHREAD
    if(int error = ::pthread_join (_thread, 0))
    //if(int error = ::pthread_cancel(_thread))
        printf("%s (%d)\n", "pthread_join() was not successful",error);
#else
    printf("%s\n", "Threads not supported on this platform");
#endif
}


void
Thread::start ()
{
#if defined WIN32
    unsigned id;
    _thread = (HANDLE)::_beginthreadex (0, 0, &threadLoop, this, 0, &id);
    
    if (_thread == 0)
        printf("%s\n", "Cannot create new thread");
#elif HAVE_PTHREAD
    if (int error = ::pthread_create (&_thread, 0, Start (threadLoop), this))
        printf("%s (%d)\n", "Cannot create new thread", error);
#else
    printf("%s\n", "Threads not supported on this platform");
#endif
}