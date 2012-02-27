//
//  XAThreadPool.h
//  XA
//
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//
#include "ThreadPool.h"
namespace XingCloud
{
    namespace XAThreadPool
    {

        typedef unsigned int (*TaskCallback)(void*);

        class XATask :public Task
        {
        public:
            XATask(TaskCallback task, void *param,TaskGroup *taskGroup);
            void	execute ();

        private:
            TaskCallback task;
            void *param;
        };
        class ExecuteTask
        {
        public:
            static void initThreadPool(short n=0){thread_pool_=new ThreadPool(n);}
            static void addTask(XATask* urltask);
            static void deleteThreadPool(){delete thread_pool_;}
        private:
            static ThreadPool *thread_pool_;
        };
    }/*XingCloud*/
}/*XAThreadPool*/

