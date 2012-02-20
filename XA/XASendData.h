//
//  XASendData.h
//  XA
//
//  Created by mini xingcloud on 12-2-15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef XA_XASendData_h
#define XA_XASendData_h
#include "XAThreadPool.h"
#include "Mutex.h"
#include <string>
#include <vector>
#include <map>
namespace XingCloud
{
    namespace XA
    {
        //typedef void CURL;
        
        class XASendData
        {
        public:
            //send data
            XASendData();
            ~XASendData();
            static bool    getMethodSend(const char *buffer);
            static bool    postMethodSend( char *buffer);
            static std::vector<std::string> cache;
            static Mutex   postMutex;
        private:
           // CURL *easy_handle;
            static TaskGroup  taskGroup;
            
        };
    }
}


#endif
