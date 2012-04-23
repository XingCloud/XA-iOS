//
//  XASendData.h
//  XA
//
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//

#ifndef XA_XASendData_h
#define XA_XASendData_h
//#include "XAThreadPool.h"
#include "Thread.h"
#include "Mutex.h"
#include <string>
#include <list>
#include <map>
namespace XingCloud
{
    namespace XA
    {
        typedef void CURL;
        typedef struct postdata
        {
            char *data;
            int  sendEventNumber;
        }postData;
        class XASendData :Thread
        {
        public:
            //send data
            ~XASendData();
            static bool    getMethodSend(const char *buffer);
            bool    postMethodSend(const char *buffer,int dataNumber);
            
            unsigned int  postPerform(void *param);
            static Mutex   postMutex;
          
            Mutex   mCacheMutex;
            void	run () ;
            
            static XASendData *instance(){return m_pSendData;}
            
        private:
            std::list<postData*> m_cache;   
            XASendData();
            static XASendData *m_pSendData;
        };
    }
}


#endif
