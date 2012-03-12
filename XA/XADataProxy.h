//
//  XALifeCicle.h
//  XA
//
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//

#ifndef XA_XALifeCicle_h
#define XA_XALifeCicle_h
#include "XAJSON.h"
#include "Mutex.h"
#include <vector>
namespace XingCloud
{
    namespace XA
    {
        class localCacheEvent
        {
        public:
            char appID[512];
            char userID[512];
            bool isInternal;//app ID, userID是否与内部相同
            bool isWriteTofile;
            unsigned short eventSize;
            unsigned int   fileposition;
            char jsonEvent[1024];
//            cJSON *jsonEvent;
//            localCacheEvent(const localCacheEvent &rhs)
//            {
//                if(rhs.appID !=NULL)
//                {
//                    int sizeAppID = strlen(rhs.appID)+1;
//                    this->appID= new char[sizeAppID];
//                    strcpy(this->appID,rhs.appID);
//                }
//                else
//                {
//                    this->appID = NULL;
//                }
//                if(rhs.userID!=NULL)
//                {
//                    int sizeUserID = strlen(rhs.userID)+1;
//                    this->userID = new char[sizeUserID];
//                    strcpy(this->userID,rhs.userID);
//                }
//                else
//                {
//                    this->userID =NULL;
//                }
//                this->isInternal = rhs.isInternal;
//                this->isWriteTofile =rhs.isWriteTofile;
//                this->eventSize =rhs.eventSize;
//                this->fileposition = rhs.fileposition;
//                this->jsonEvent = rhs.jsonEvent;
//            }
//            localCacheEvent& operator=(const localCacheEvent &rhs)
//            {
//                if(this != &rhs)
//                {
//                    if(rhs.appID !=NULL)
//                    {
//                        int sizeAppID = strlen(rhs.appID)+1;
//                        
//                        this->appID= new char[sizeAppID];
//                        strcpy(this->appID,rhs.appID);
//                    }
//                    else
//                    {
//                        this->appID = NULL;
//                    }
//                    if(rhs.userID!=NULL)
//                    {
//                        int sizeUserID = strlen(rhs.userID)+1;
//                        
//                        this->userID = new char[sizeUserID];
//                        strcpy(this->userID,rhs.userID);
//                    }
//                    else
//                    {
//                        this->userID =NULL;
//                    }
//                    this->isInternal = rhs.isInternal;
//                    this->isWriteTofile =rhs.isWriteTofile;
//                    this->eventSize =rhs.eventSize;
//                    this->fileposition = rhs.fileposition;
//                    this->jsonEvent = rhs.jsonEvent;
//                }
//                return  *this;
//            }
//            localCacheEvent(){appID = userID = NULL;isWriteTofile=false;eventSize=0; fileposition=0;}
//            ~localCacheEvent(){if(appID!=NULL){delete appID;appID=NULL;}if(userID!=NULL){delete userID;userID=NULL;}}
        };
        
        
        class XADataProxy
        {
        public:
            
            ~XADataProxy();
            static XADataProxy* getInstance(){return m_instance;}
            
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
            
            void    WriteMemoryDataToFile(int dataNumber);
            
            cJSON*   getGenSignedParamsObject(const char *appId,const char *userId,int timestamp);
            
            void     readyForSendData(int lastReadyEventNumber);
            void     sendInternalEventData(cJSON *internalStatArray,int eventNumber);
            void     sendGeneralEventData(const char *appID,const char *userID,cJSON *generalStatArray,int eventNumber);
            void     handleEventTimer();
             void    sendHeartbeatEventData();
           
            cJSON*   quitEventData();
          
            void     handleSendSucess(int sendDataNumber);
            void     handleSendFailed(int sendDataNumber);
        
            Mutex    filePointMutex;
            Mutex    eventCacheMutex;
            Mutex    timerMutex;
           
            
            FILE    *localfilePoint;
            char    *docfilePath;
            char    *uid;
            
            int      sendEventNumber;
            
            std::vector<localCacheEvent> eventMemoryCache;
            
            unsigned int curValidPosition;
            
            int lastReadyEventNumber;
            
        private:
            XADataProxy();
            unsigned int pause_time;
            unsigned int idle_time;
            static XADataProxy* m_instance;
        };
    }
}


#endif
