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
#include <vector>
namespace XingCloud
{
    namespace XA
    {
        
        
        class localCacheEvent
        {
        public:
            char *appID;
            char *userID;
            bool isInternal;
            bool isWriteTofile;
            //bool isSend;
            unsigned short eventSize;
            unsigned int   fileposition;
            cJSON *jsonEvent;
            localCacheEvent(){appID = userID = NULL;isWriteTofile=false;eventSize=0; fileposition=0;}
            ~localCacheEvent(){if(appID!=NULL)delete appID;if(userID!=NULL)delete userID;}
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
            
            void    handleTrackLogin(cJSON  *login);
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
            
            static  void    WriteMemoryDataToFile(int dataNumber);
            static  void    MoveFilePositon();
            
            static cJSON*   getGenSignedParamsObject(const char *appId,const char *userId,int timestamp);
            
            static void     readyForSendInternalData();
            static void     sendInternalEventData(cJSON *internalStatArray,int eventNumber);
            static void     sendGeneralEventData(const char *appID,const char *userID,cJSON *generalStatArray,int eventNumber);
            static void     handleEventData();
            static void     sendHeartbeatEventData();
           
            static cJSON*   quitEventData();
          
            static void     handleSendSucess(int sendDataNumber);
            static void     handleSendFailed(int sendDataNumber);
        
            static Mutex    filePointMutex;
            static Mutex    eventCacheMutex;
            
            static int      currentFilePosition;
            static int      lastFilePosition;
            static int      lastEventSize;
            static int      currentEventSize;
            static bool     isDeviceCache;
            static FILE    *localCache;
            static char    *docfilePath;
            static char    *uid;
            
            static int      sendEventNumber;
            static std::vector<localCacheEvent> eventMemoryCache;
            
            static unsigned int curValidPosition;

            
        private:
            unsigned int pause_time;
            static unsigned int idle_time;
        };
    }
}


#endif
