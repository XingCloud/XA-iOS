//
//  XAThreadPool.h
//  XA
//
//  Created by mini xingcloud on 12-2-15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#ifndef XATHREADPOOL_H_
#define XATHREADPOOL_H_
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

#endif
