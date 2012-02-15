#ifndef Semaphore_h
#define Semaphore_h

#include "config.h"

//-----------------------------------------------------------------------------
//
//	class Semaphore -- a wrapper class for
//	system-dependent counting semaphores
//
//-----------------------------------------------------------------------------
class Semaphore
{
public:
    
    Semaphore (unsigned int value = 0);
    virtual ~Semaphore();
    
    void	wait();
    bool	tryWait();
    void	post();
    int		value() const;
    
private:
    
#if defined WIN32
    
	mutable HANDLE _semaphore;
    
#elif HAVE_PTHREAD && !HAVE_POSIX_SEMAPHORES
    
	//
	// If the platform has Posix threads but no semapohores,
	// then we implement them ourselves using condition variables
	//
    
	struct sema_t
	{
	    unsigned int count;
	    unsigned long numWaiting;
	    pthread_mutex_t mutex;
	    pthread_cond_t nonZero;
	};
    
	mutable sema_t _semaphore;
    
#elif HAVE_PTHREAD && HAVE_POSIX_SEMAPHORES
    
	mutable sem_t _semaphore;
    
#endif
    
    void operator = (const Semaphore& s);	// not implemented
    Semaphore (const Semaphore& s);		// not implemented
};

#endif
