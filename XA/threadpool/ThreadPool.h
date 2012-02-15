#ifndef FRT_APP_ThreadPool_h
#define FRT_APP_ThreadPool_h

//-----------------------------------------------------------------------------
//
//	class Task, class ThreadPool, class TaskGroup
//
//	Class ThreadPool manages a set of worker threads and accepts
//	tasks for processing.  Tasks added to the thread pool are
//	executed concurrently by the worker threads.  
//	
//	Class Thread provides an abstract interface for a task which
//	a ThreadPool works on.  Derived classes need to implement the
//	execute() function which performs the actual task.
//
//	Class TaskTroup allows synchronization on the completion of a set
//	of tasks.  Every task that is added to a ThreadPool belongs to a
//	single TaskGroup.  The destructor of the TaskGroup waits for all
//	tasks in the group to finish.
//
//	Note: if you plan to use the ThreadPool interface in your own
//	applications note that the implementation of the ThreadPool calls
//	operator delete on tasks as they complete.  If you define a custom
//	operator new for your tasks, for instance to use a custom heap,
//	then you must also write an appropriate operator delete.
//
//-----------------------------------------------------------------------------

class TaskGroup;
class Task;


class ThreadPool  
{
public:
    
    //-------------------------------------------------------
    // Constructor -- creates numThreads worker threads which
    // wait until a task is available. 
    //-------------------------------------------------------
    
    ThreadPool (unsigned numThreads = 0);
    
    
    //-----------------------------------------------------------
    // Destructor -- waits for all tasks to complete, joins all
    // the threads to the calling thread, and then destroys them.
    //-----------------------------------------------------------
    
    virtual ~ThreadPool ();
    
    
    //--------------------------------------------------------
    // Query and set the number of worker threads in the pool.
    //
    // Warning: never call setNumThreads from within a worker
    // thread as this will almost certainly cause a deadlock
    // or crash.
    //--------------------------------------------------------
    
    int		numThreads () const;
    void	setNumThreads (int count);
    
    
    //------------------------------------------------------------
    // Add a task for processing.  The ThreadPool can handle any
    // number of tasks regardless of the number of worker threads.
    // The tasks are first added onto a queue, and are executed
    // by threads as they become available, in FIFO order.
    //------------------------------------------------------------
    
    void addTask (Task* task);
    
    
    //-------------------------------------------
    // Access functions for the global threadpool
    //-------------------------------------------
    
    static ThreadPool&	globalThreadPool ();
    static void		addGlobalTask (Task* task);
    
    struct Data;
    
protected:
    
    Data *		_data;
};


class Task
{
public:
    
    Task (TaskGroup* g);
    virtual ~Task ();
    
    virtual void	execute () = 0;
    TaskGroup *		group();
    
protected:
    
    TaskGroup *		_group;
};


class TaskGroup
{
public:
    
    TaskGroup();
    ~TaskGroup();
    
    struct Data;
    Data* const		_data;
};

#endif
