//
//  XADataManager.h
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//

#ifndef XA_XADataManager_h
#define XA_XADataManager_h
#include "XAStreamBuffer.h"
namespace XingCloud
{
    namespace XA
    {
        //
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
        typedef void CURL;
        class XADataManager
        {
        public:
            XADataManager();
            void    applicationLaunch();
            void    applicationTerminate();
            void    applicationPause();
            void    applicationResume();
            
            bool    sendDataBuffer(const char *buffer);
            void    sendCacheBuffer();
            void    sendErrorBuffer();
            
            void    setLogEnabled(bool value);
            void    setCrashReportEnabled(bool value);
            void    setHeartbeatEnabled(bool value);
            void    setReportPolicy(short reportPolice);
            void    setChannelID(const char *value);
            void    setAppID(const char *value);
        private:
            XAStreamBuffer *sendBuffer;
            XAStreamBuffer *errorBuffer;
            XAStreamBuffer *cacheBuffer;
            ServicesEnable servicesEnable;
            char *channelID;
            char *appID;
            short reportPolice;
            uint8_t* realloc();
        };
    }
}

#endif
