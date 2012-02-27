//
//  XASendData.h
//  XA
//
//  Copyright (c) 2012 XingCloud.com All rights reserved.
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
        typedef struct postdata
        {
            char *data;
            int  sendEventNumber;
        }postData;
        class XASendData
        {
        public:
            //send data
            XASendData();
            ~XASendData();
            static bool    getMethodSend(const char *buffer);
            static bool    postMethodSend(const char *buffer,int dataNumber);
            
            //static std::vector<std::string> cache;
            //static std::map<int,std::string> cache;
            static Mutex   postMutex;
        private:
           // CURL *easy_handle;
            static TaskGroup  taskGroup;
            
        };
    }
}


#endif
