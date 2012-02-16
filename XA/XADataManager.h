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
#include "XADataProxy.h"
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
        
        class XADataManager
        {
        public:
            XADataManager();
            ~XADataManager();
            //life cicle
            void    applicationLaunch();
            void    applicationTerminate();
            void    applicationPause();
            void    applicationResume();
            
                   
            //XA events
            void    trackCount(const char *action,const char *level1,const char *level2,const char *level3,const char *level4,const char *level5,int count);
            void    trackMilestone(const char *milestoneName);
            void    trackTransaction(int event,const char *orderId,const char *cost,const char*money,const char *category,const char *name);
            void    trackTutorialService(const char *index,const char *name,const char *tutorial);
            void    trackBuyService(const char *currency,const char *payType,const char *level1,const char *level2,const char *level3,const char *level4,const char *level5,int                             amount,int number);
            void    generalEvent(int event,const char *appId,const char *userId,int timestamp,const char *params);
            
            //set arguments
            void    setLogEnabled(bool value);
            void    setCrashReportEnabled(bool value);
            void    setHeartbeatEnabled(bool value);
            void    setReportPolicy(short reportPolice);
            void    setChannelID(const char *value);
            void    setAppID(const char *value);
            
            static  cJSON* getSignedParamsJsonObject();
            static int     getTimer();
            static unsigned int     getTimestamp();
            static char *channelID;
            static char *appID;
            static char *uid;
            static short reportPolice;
            FILE   *localCache;
        private:
            XAStreamBuffer *sendBuffer;
            XAStreamBuffer *errorBuffer;
            XAStreamBuffer *cacheBuffer;
            ServicesEnable servicesEnable;
            
            XADataProxy   xaDataProxy;
            bool       firstInitApp;
        };
    }
}

#endif
