
#ifndef FRT_APP_Mutex_h
#define FRT_APP_Mutex_h

//-----------------------------------------------------------------------------
//
//	class Mutex, class Lock
//
//	Class Mutex is a wrapper for a system-dependent mutual exclusion
//	mechanism.  Actual locking and unlocking of a Mutex object must
//	be performed using an instance of a Lock (defined below).
//
//	Class lock provides safe locking and unlocking of mutexes even in
//	the presence of C++ exceptions.  Constructing a Lock object locks
//	the mutex; destroying the Lock unlocks the mutex.
//
//	Lock objects are not themselves thread-safe.  You should never
//	share a Lock object among multiple threads.
//
//	Typical usage:
//    
//	    Mutex mtx;	// Create a Mutex object that is visible
//	    		//to multiple threads
//
//	    ...		// create some threads
//
//	    // Then, within each thread, construct a critical section like so:
//
//	    {
//		Lock lock (mtx);	// Lock constructor locks the mutex
//		...			// do some computation on shared data
//	    }				// leaving the block unlocks the mutex
//
//-----------------------------------------------------------------------------

#include "config.h"

class Lock;

class Mutex
{
public:
    
    Mutex ();
    virtual ~Mutex ();
    
private:
    
    void	lock () const;
    void	unlock () const;
    
#if defined WIN32
	mutable CRITICAL_SECTION _mutex;
#elif HAVE_PTHREAD
	mutable pthread_mutex_t _mutex;
#endif
    
    void operator = (const Mutex& M);	// not implemented
    Mutex (const Mutex& M);		// not implemented
    
    friend class Lock;
};


class Lock
{
public:
    
    Lock (const Mutex& m, bool autoLock = true):
	_mutex (m),
	_locked (false)
    {
        if (autoLock)
        {
            _mutex.lock();
            _locked = true;
        }
    }
    
    ~Lock ()
    {
        if (_locked)
            _mutex.unlock();
    }
    
    void acquire ()
    {
        _mutex.lock();
        _locked = true;
    }
    
    void release ()
    {
        _mutex.unlock();
        _locked = false;
    }
    
    bool locked ()
    {
        return _locked;
    }
    
private:
    
    const Mutex &	_mutex;
    bool		_locked;
};

#endif
