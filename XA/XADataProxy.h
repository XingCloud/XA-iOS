//
//  XALifeCicle.h
//  XA
//
//  Created by mini xingcloud on 12-2-16.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef XA_XALifeCicle_h
#define XA_XALifeCicle_h
#include "cJSON.h"
#include "Mutex.h"
namespace XingCloud
{
    namespace XA
    {
        void* timerFunc(void *param);
        
        class XADataProxy
        {
        public:
            XADataProxy();
            ~XADataProxy();
            void    handleApplicationLaunch(cJSON *visitEvent,cJSON *updateEvent,cJSON *errorEvent);
            void    handleApplicationTerminate();
            void    handleApplicationPause();
            void    handleApplicationResume();
            
            void    handleTrackUserIncrement(cJSON *userInfo);
            void    handleTrackUserUpdate(cJSON *userInfo);
            void    handleTrackCount(cJSON *countEvent);
            void    handleTrackMilestone(cJSON *milestoneEvent);
            void    handleTrackTransaction(cJSON *transactionEvent);
            void    handleTrackTutorialService(cJSON *tutorialEvent);
            void    handleTrackBuyService(cJSON *buyEvent);       
            void    handleGeneralEvent(int event,const char *appId,const char *userId,int timestamp,const char *params);
            void    handleInternalEvent(int event,cJSON *params);
        
            void    eventString(int event,char *source);
            cJSON*  encapsulateEvent(int event,cJSON *params);
            static cJSON*  getGenSignedParamsObject(const char *appId,const char *userId,int timestamp);
            static void  sendInternalEventData();
            static void  sendGeneralEventData();
            static void  sendHeartbeatEventData();
            static cJSON*  quitEventData();
            
            static Mutex   internalMutex;
            static Mutex   generalMutex;
            static Mutex   fileMutex;
            static FILE   *localCache;
            static char   *docfilePath;
            static char   *uid;
        private:
            unsigned int pause_time;
            static unsigned int idle_time;
        };
    }
}


#endif
