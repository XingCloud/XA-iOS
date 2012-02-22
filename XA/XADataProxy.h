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
#include <map>
namespace XingCloud
{
    namespace XA
    {
        void* timerFunc(void *param);
        
        class localcacheEvent
        {
        public:
            char *appID;
            char *userID;
            bool isInternal;
            cJSON *jsonEvent;
            localcacheEvent(){appID = userID = NULL;}
            ~localcacheEvent(){if(appID!=NULL)delete appID;if(userID!=NULL)delete userID;/*cJSON_Delete(jsonEvent);*/}
        };
        
        
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
            void    handleGeneralEvent(int event,const char *appId,const char *userId,unsigned int timestamp,cJSON *params,bool isInternal=false);
            void    handleEvent(int event,unsigned int timestamp,cJSON *params,int eventIndex);
        
            void    eventString(int event,char *source);
            cJSON*  encapsulateEvent(int event,cJSON *params);
            
            static cJSON*   getGenSignedParamsObject(const char *appId,const char *userId,int timestamp);
            static void     readyForSendInternalData();
            static void     sendInternalEventData(cJSON *internalStatArray);
            static void     sendGeneralEventData(const char *appID,const char *userID,cJSON *generalStatArray);
            static void     handleEventData();
            static void     sendHeartbeatEventData();
            static void     writeCacheToFile();
            static cJSON*   quitEventData();
            
            static Mutex    internalMutex;
            static Mutex    generalMutex;
            static Mutex    fileMutex;
          
            static int      currentFilePosition;
            static int      lastFilePosition;
            static int      lastEventSize;
            static int      currentEventSize;
            
            static FILE    *localCache;
            static char    *docfilePath;
            static char    *uid;
            
            static std::map<int,localcacheEvent*> eventCache;
        private:
            unsigned int pause_time;
            static unsigned int idle_time;
        };
    }
}


#endif
