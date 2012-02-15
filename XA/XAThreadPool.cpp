//
//  XAThreadPool.cpp
//  XA
//
//  Created by mini xingcloud on 12-2-15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#include "XAThreadPool.h"
namespace XingCloud
{
    namespace XAThreadPool
    {
        ThreadPool* ExecuteTask::thread_pool_=0;
        XATask::XATask(TaskCallback task,void *param,TaskGroup *taskGroup):Task(taskGroup)
        {
            this->task=task;
            this->param=param;
        }
        void XATask::execute()
        {
            (task)(param);
        }
        void ExecuteTask::addTask(XATask* urltask)
        {
            thread_pool_->addTask(urltask);
        }
    }/*XingCloud*/
}/*XAThreadPool*/
