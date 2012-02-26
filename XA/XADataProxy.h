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
            bool isInternal;//app ID, userID是否与内部相同
            bool isWriteTofile;
            unsigned short eventSize;
            unsigned int   fileposition;
            cJSON *jsonEvent;
            localCacheEvent(const localCacheEvent &rhs)
            {
                if(rhs.appID !=NULL)
                {
                    int sizeAppID = strlen(rhs.appID)+1;
                    this->appID= new char[sizeAppID];
                    strcpy(this->appID,rhs.appID);
                }
                else
                {
                    this->appID = NULL;
                }
                if(rhs.userID!=NULL)
                {
                    int sizeUserID = strlen(rhs.userID)+1;
                    this->userID = new char[sizeUserID];
                    strcpy(this->userID,rhs.userID);
                }
                else
                {
                    this->userID =NULL;
                }
                this->isInternal = rhs.isInternal;
                this->isWriteTofile =rhs.isWriteTofile;
                this->eventSize =rhs.eventSize;
                this->fileposition = rhs.fileposition;
                this->jsonEvent = rhs.jsonEvent;
            }
            localCacheEvent& operator=(const localCacheEvent &rhs)
            {
                if(this != &rhs)
                {
                    if(rhs.appID !=NULL)
                    {
                        int sizeAppID = strlen(rhs.appID)+1;
                        if(appID !=NULL)
                        {
                            delete appID;
                        }
                        this->appID= new char[sizeAppID];
                        strcpy(this->appID,rhs.appID);
                    }
                    else
                    {
                        this->appID = NULL;
                    }
                    if(rhs.userID!=NULL)
                    {
                        int sizeUserID = strlen(rhs.userID)+1;
                        if(userID !=NULL)
                        {
                            delete userID;
                        }
                        this->userID = new char[sizeUserID];
                        strcpy(this->userID,rhs.userID);
                    }
                    else
                    {
                        this->userID =NULL;
                    }
                    this->isInternal = rhs.isInternal;
                    this->isWriteTofile =rhs.isWriteTofile;
                    this->eventSize =rhs.eventSize;
                    this->fileposition = rhs.fileposition;
                    this->jsonEvent = rhs.jsonEvent;
                }
                return  *this;
            }
            localCacheEvent(){appID = userID = NULL;isWriteTofile=false;eventSize=0; fileposition=0;}
            ~localCacheEvent(){if(appID!=NULL){delete appID;appID=NULL;}if(userID!=NULL){delete userID;userID=NULL;}}
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
            
            static cJSON*   getGenSignedParamsObject(const char *appId,const char *userId,int timestamp);
            
            static void     readyForSendData();
            static void     sendInternalEventData(cJSON *internalStatArray,int eventNumber);
            static void     sendGeneralEventData(const char *appID,const char *userID,cJSON *generalStatArray,int eventNumber);
            static void     handleEventData();
            static void     sendHeartbeatEventData();
           
            static cJSON*   quitEventData();
          
            static void     handleSendSucess(int sendDataNumber);
            static void     handleSendFailed(int sendDataNumber);
        
            static Mutex    filePointMutex;
            static Mutex    eventCacheMutex;
            
           
            
            static FILE    *localfilePoint;
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
