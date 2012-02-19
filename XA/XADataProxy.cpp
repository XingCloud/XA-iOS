//
//  XALifeCicle.cpp
//  XA
//
//  Created by mini xingcloud on 12-2-16.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#include "XADataProxy.h"
#include "XASystemInfo.h"
#include "XASendData.h"
#include "XADataManager.h"
#include <vector>
namespace  XingCloud
{
    namespace XA
    {
        typedef struct generalEvent
        {
            char appID[64];
            char userID[64];
            cJSON *event;
        }generalEventJSON;
        FILE   *XADataProxy::localCache;
        std::vector<cJSON*> internalEventCache;
        std::vector<generalEventJSON> generalEventCache;
        Mutex   XADataProxy::internalMutex;
        Mutex   XADataProxy::generalMutex;
        XADataProxy::XADataProxy()
        {
            idle_time = 0;
            pause_time = 0;
        }
        XADataProxy::~XADataProxy()
        {
        }
        void  XADataProxy::sendHeartbeatEventData()
        {
            cJSON *root=cJSON_CreateObject();
            cJSON_AddItemToObject(root,"signedParams",XADataManager::getSignedParamsJsonObject());
            cJSON *statArray=cJSON_CreateArray();
            cJSON *statObject=cJSON_CreateObject();
            cJSON_AddItemToObject(statObject,"eventName",cJSON_CreateString("user.heartbeat"));
            cJSON_AddItemToObject(statObject,"params",cJSON_CreateObject());
            char temp[64]={0};
            sprintf(temp,"%u",XADataManager::getTimestamp());
            cJSON_AddItemToObject(statObject,"timestamp",cJSON_CreateString(temp));
            cJSON_AddItemToObject(root,"stats",statArray);
            XASendData::getMethodSend(cJSON_PrintUnformatted(root));
        }
        void  XADataProxy::sendInternalEventData()
        {
            cJSON *root=cJSON_CreateObject();
            cJSON_AddItemToObject(root,"signedParams",XADataManager::getSignedParamsJsonObject());
            cJSON *statArray=cJSON_CreateArray();
            
            Lock lock(internalMutex);
            for(int i=0;i<internalEventCache.size();i++)
                cJSON_AddItemToArray(statArray,internalEventCache[i]);
            cJSON_AddItemToObject(root,"stats",statArray);
            XASendData::postMethodSend(cJSON_PrintUnformatted(root));
            generalEventCache.clear();
        }
        void  XADataProxy::sendGeneralEventData()
        {
            Lock  lock(generalMutex);
            for(int i=0;i<generalEventCache.size();i++)
            {
                cJSON *root=cJSON_CreateObject();
                cJSON_AddItemToObject(root,"signedParams",getGenSignedParamsObject(generalEventCache[i].appID,generalEventCache[i].userID,XADataManager::getTimestamp()));
                cJSON *statsJson=cJSON_CreateArray();
                cJSON_AddItemToArray(statsJson,generalEventCache[i].event);
                cJSON_AddItemToObject(root,"stats",statsJson);
                XASendData::postMethodSend(cJSON_PrintUnformatted(root));
            }
            generalEventCache.clear();
            
        }
        void    XADataProxy::handleApplicationLaunch(cJSON *visitEvent,cJSON *updateEvent,cJSON *errorEvent)
        {
            if(localCache!=NULL)
            {
                int cacheLength=0;
                fread(&cacheLength,sizeof(int),1,localCache);
                for(int i=0;i<cacheLength;i++)
                {
                    int length=0;
                    fread(&length,sizeof(int),1,localCache);
                    char temp[1024]={0};
                    fread(temp,length,1,localCache);
                    XASendData::postMethodSend(temp);
                }
            }

            handleInternalEvent(2,(visitEvent));
            if(updateEvent!=NULL)
                handleInternalEvent(0,(updateEvent));
            if(errorEvent!=NULL)
                handleInternalEvent(5,(errorEvent));
            
            if(XADataManager::reportPolice==1)
            {
                XADataProxy::sendInternalEventData();
                XADataProxy::sendGeneralEventData();
            }
        }
        void    XADataProxy::handleApplicationTerminate(cJSON *quitEvent)
        {
            if(localCache!=NULL)
            {
                int cacheLength=XASendData::cache.size();
                fwrite(&cacheLength,sizeof(int),1,localCache);
                for(int i=0;i<XASendData::cache.size();i++)
                {
                    const char *data = XASendData::cache[i].c_str();
                    int length=strlen(data);
                    fwrite(&length,sizeof(int),1,localCache);
                    fwrite(data,length,1,localCache);
                }
            }
            //send quit event 
            char temp[64]={0};
            cJSON *root=cJSON_CreateObject();
            cJSON_AddItemToObject(root,"signedParams",XADataManager::getSignedParamsJsonObject());
            cJSON *statArray=cJSON_CreateArray();
            cJSON *statObject=cJSON_CreateObject();
            cJSON_AddItemToObject(statObject,"eventName",cJSON_CreateString("user.quit"));
            unsigned int duration_time=XADataManager::getTimer()- idle_time;
            sprintf(temp,"%u",duration_time);
            cJSON_AddItemToObject(statObject,"params",cJSON_CreateString(temp));
            memset(temp,0,64);
            sprintf(temp,"%u",XADataManager::getTimestamp());
            cJSON_AddItemToObject(statObject,"timestamp",cJSON_CreateString(temp));
            cJSON_AddItemToObject(root,"stats",statArray);
            XASendData::getMethodSend(cJSON_PrintUnformatted(root));
        }
        void    XADataProxy::handleApplicationPause()
        {
            pause_time = XADataManager::getTimer();
        }
        void    XADataProxy::handleApplicationResume()
        {
            idle_time +=( XADataManager::getTimer()- pause_time);
        }
        void    XADataProxy::handleTrackCount(cJSON *countEvent)
        {
            handleInternalEvent(9,(countEvent));
        }
        void    XADataProxy::handleTrackMilestone(cJSON *milestoneEvent)
        {
            handleInternalEvent(10,(milestoneEvent));
        }
        void    XADataProxy::handleTrackTransaction(cJSON *transactionEvent)
        {
            handleInternalEvent(7,(transactionEvent));
        }
        void    XADataProxy::handleTrackTutorialService(cJSON *tutorialEvent)
        {
            handleInternalEvent(11,(tutorialEvent));
        }
        void    XADataProxy::handleTrackBuyService(cJSON *buyEvent)
        {
            handleInternalEvent(8,(buyEvent));
        }
        cJSON*    XADataProxy::encapsulateEvent(int event,cJSON *params)
        {
            char temp[64]={0};
            eventString(event,temp);
            cJSON  *eventJson=cJSON_CreateObject();
            cJSON_AddItemToObject(eventJson,"eventName",cJSON_CreateString(temp));
            cJSON_AddItemToObject(eventJson,"params",params);
            memset(temp,0,64);
            sprintf(temp,"%u",XADataManager::getTimestamp());
            cJSON_AddItemToObject(eventJson,"timestamp",cJSON_CreateString(temp));
            return eventJson;
        }
        void    XADataProxy::handleInternalEvent(int event,cJSON *params)
        {
            XAPRINT(cJSON_PrintUnformatted(params));
            if(XADataManager::reportPolice==0)
            {//实时发送
                cJSON *root=cJSON_CreateObject();
                cJSON_AddItemToObject(root,"signedParams",XADataManager::getSignedParamsJsonObject());
                cJSON *statArray=cJSON_CreateArray();
                cJSON_AddItemToArray(statArray,encapsulateEvent(event,params));
                cJSON_AddItemToObject(root,"stats",statArray);
                XASendData::postMethodSend(cJSON_PrintUnformatted(root));
            }
            else if(XADataManager::reportPolice==1)
            {//启动时批量发送
                
            }
            else if(XADataManager::reportPolice==3)
            {//定时定量发送
                Lock lock(internalMutex);
                internalEventCache.push_back(encapsulateEvent(event,params));
                if(internalEventCache.size()>=10)
                {
                    sendInternalEventData();
                }
            }
            else
            {
                
            }
        }
        cJSON *XADataProxy::getGenSignedParamsObject(const char *appId,const char *userId,int timestamp)
        {
            
            cJSON *signedJson=cJSON_CreateObject();
            cJSON_AddItemToObject(signedJson,"appid",cJSON_CreateString(appId));
            cJSON_AddItemToObject(signedJson,"uid",cJSON_CreateString(userId));
            char temp[64]={0};
            sprintf(temp,"%u",timestamp);
            cJSON_AddItemToObject(signedJson,"timestamp",cJSON_CreateString(temp));
            
            return signedJson;
        }
       
        void    XADataProxy::handleGeneralEvent(int event,const char *appId,const char *userId,int timestamp,const char *params)
        {
            //generate  evnenObject
            cJSON *paramJSON=cJSON_CreateObject();
            cJSON_AddItemToObject(paramJSON,"params",cJSON_CreateString(params));
            cJSON *generalEventJSONObject=encapsulateEvent(event,paramJSON);
            
            if(XADataManager::reportPolice==0)
            {//realtime
                cJSON *root=cJSON_CreateObject();
                cJSON_AddItemToObject(root,"signedParams",getGenSignedParamsObject(appId,userId,timestamp));
                cJSON *statsJson=cJSON_CreateArray();
                cJSON_AddItemToArray(statsJson,generalEventJSONObject);
                cJSON_AddItemToObject(root,"stats",statsJson);
                XASendData::postMethodSend(cJSON_PrintUnformatted(root));
            }
            else if(XADataManager::reportPolice==1)
            {
                
            }
            else if(XADataManager::reportPolice==3)
            {//定时定量发送
                Lock lock(generalMutex);
                generalEventJSON ge;
                ge.event=generalEventJSONObject;
                strcpy(ge.appID,appId);
                strcpy(ge.userID,userId);
                generalEventCache.push_back(ge);
                if(generalEventCache.size()>=10)
                {
                    sendGeneralEventData();
                }
            }
            else
            {
                //
            }
        }
        void    XADataProxy::eventString(int event,char *source)
        {
            switch(event)
            {
                case 0:
                    strcpy(source,"user.update");
                    break;
                case 1:
                    strcpy(source,"user.increment");
                    break;
                case 2:
                    strcpy(source,"user.visit");
                    break;
                case 3:
                    strcpy(source,"user.heartbeat");
                    break;
                case 4:
                    strcpy(source,"user.login");
                    break;
                case 5:
                    strcpy(source,"user.error");
                    break;
                case 6:
                    strcpy(source,"user.quit");
                    break;
                case 7:
                    strcpy(source,"pay.complete");
                    break;
                case 8:
                    strcpy(source,"buy.item");
                    break;
                case 9:
                    strcpy(source,"count");
                    break;
                case 10:
                    strcpy(source,"milestone");
                    break;
                case 11:
                    strcpy(source,"tutorial");
                    break;
                case 12:
                    strcpy(source,"page.view");//not support 
                default:
                    break;
            }

        }
    }
}