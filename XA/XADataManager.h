//
//  XADataManager.h
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef XA_XADataManager_h
#define XA_XADataManager_h
namespace XingCloud
{
    namespace XA
    {
        typedef unsigned char uint8_t;
        typedef struct serviceEnable
        {
            bool crashReportEnable;
            bool logPrintEnable;
            bool heartbeatEnable;
        }ServicesEnable;
        
        typedef enum {
            USER_UPDATE,
            USER_INCREMENT,
            USER_VISIT,
            USER_HEARTBEAT,
            USER_LOGIN,
            USER_ERROR,
            USER_QUIT,
            PAY_COMPLETE,
            BUY_ITEM,
            COUNT,
            MILESTONE,
            TUTORIAL,
            PAGE_VIEW
        } XAEventType;

        class XADataManager
        {
        public:
            XADataManager();
            bool    sendDataBuffer(const char buffer);
            void    sendCacheBuffer();
            void    sendErrorBuffer();
            
            void    setLogEnabled(bool value);
            void    setCrashReportEnabled(bool value);
            void    setHeartbeatEnabled(bool value);
            void    setReportPolicy(short reportPolice);
        private:
            uint8_t *sendBuffer;
            uint8_t *errorBuffer;
            uint8_t *cacheBuffer;
            ServicesEnable servicesEnable;
            short reportPolice;
            uint8_t* realloc();
        };
    }
}


#endif
