//
//  XADataProxy.cpp
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
        Mutex   XADataProxy::fileMutex;
        char* XADataProxy::docfilePath=NULL;
        char *XADataProxy::uid=NULL;
        unsigned int XADataProxy::idle_time = 0;
        XADataProxy::XADataProxy()
        {
            idle_time = 0;
            pause_time = 0;
        }
        XADataProxy::~XADataProxy()
        {
            if(docfilePath!=NULL)
                delete docfilePath;
        }
        void  XADataProxy::sendHeartbeatEventData()
        {
            if(XADataManager::heartbeat)
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
                cJSON_AddItemToArray(statArray,statObject);
                cJSON_AddItemToObject(root,"stats",statArray);
                XASendData::getMethodSend(cJSON_PrintUnformatted(root));
                 //XASendData::postMethodSend(cJSON_PrintUnformatted(root));
            }
            if(SystemInfo::getNetType(NULL)!=0)
            {
                Lock lock(XASendData::postMutex);
                for(std::map<int,std::string>::iterator iter = XASendData::cache.begin();iter !=XASendData::cache.end();iter++)
                    XASendData::postMethodSend(iter->second.c_str());
            }
/**********************************just test usage*****************************************************/
//            cJSON *rootq=cJSON_CreateObject();
//            cJSON_AddItemToObject(rootq,"signedParams",XADataManager::getSignedParamsJsonObject());
//            cJSON *statArrayq=cJSON_CreateArray();
//            cJSON *statObjectq=cJSON_CreateObject();
//            cJSON_AddItemToObject(statObjectq,"eventName",cJSON_CreateString("user.quit"));
//            cJSON *statObjectParams=cJSON_CreateObject();
//            unsigned int duration_time=XADataManager::getTimer()- 0;
//            sprintf(temp,"%u",duration_time);
//            cJSON_AddItemToObject(statObjectParams,"duration_time",cJSON_CreateString(temp));
//            cJSON_AddItemToObject(statObjectParams,"is_mobile",cJSON_CreateString("true"));
//            
//            cJSON_AddItemToObject(statObjectq,"params",statObjectParams);
//            memset(temp,0,64);
//            sprintf(temp,"%u",XADataManager::getTimestamp());
//            cJSON_AddItemToObject(statObjectq,"timestamp",cJSON_CreateString(temp));
//            cJSON_AddItemToArray(statArrayq  ,statObjectq);
//            cJSON_AddItemToObject(root,"stats",statArrayq);
//            //quit 缓存下来
//            XASendData::postMethodSend(cJSON_PrintUnformatted(root));
            
            
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
            char appDir[512]={0};
            SystemInfo::getAppFileDir(appDir);
            docfilePath = new char[521];
            sprintf(docfilePath,"%s/appCache.log",appDir);
            
            localCache = fopen(docfilePath,"ab+");
            if(localCache==NULL)
            {
                XAPRINT("error  loacl Cache can not open "); 
                return ;
            }
//            long fileLength=ftell(localCache);
//            fseek(localCache,0L,SEEK_END);
//            ftell(localCache);
//            if((ftell(localCache) - fileLength)==0)
            fseek(localCache,0L,SEEK_SET);
            if(fgetc(localCache)==EOF)
            {//本地文件不存在，发送update事件
                uid=new char[64];
                memset(uid,0,64);
                SystemInfo::getDeviceID(uid);
                fclose(localCache);
                remove(docfilePath);
                //xaDataProxy.handleApplicationLaunch(visitJson,SystemInfo::getSystemInfo(getTimestamp()),NULL);
                handleInternalEvent(0,SystemInfo::getSystemInfo(XADataManager::getTimestamp()));
            }
            else
            {//本地文件存在，不发送update事件
                fseek(XADataProxy::localCache,0,SEEK_SET);
                uid=new char[64];
                memset(uid,0,64);
                 //send quit data
                fread(uid,63,1,XADataProxy::localCache);
                if(localCache!=NULL)
                {//sending  cache  event
                   
                    char temp[1024]={0};
                    int tempLength=0;
                    fread(&tempLength,sizeof(int),1,localCache);
                    fread(temp,tempLength,1,localCache);
                    XASendData::postMethodSend(temp);//send quit event
                    
                    tempLength=0;
                    fread(&tempLength,sizeof(int),1,localCache);
                    for(int i=0;i<tempLength;i++)
                    {
                        int length=0;
                        fread(&length,sizeof(int),1,localCache);
                        memset(temp,0,1024);
                        fread(temp,length,1,localCache);
                        XASendData::postMethodSend(temp);
                    }
                }
                
                fclose(localCache);
                localCache=NULL;
            
                remove(docfilePath);
                
            }
/*****************************************just test to usage************************************************/
//            uid=new char[64];
//            memset(uid,0,64);
//            SystemInfo::getDeviceID(uid);
//            handleInternalEvent(0,SystemInfo::getSystemInfo(XADataManager::getTimestamp()));
            handleInternalEvent(2,(visitEvent));
  
            if(errorEvent!=NULL)
                handleInternalEvent(5,(errorEvent));
            
            if(XADataManager::reportPolice==1)
            {
                XADataProxy::sendInternalEventData();
                XADataProxy::sendGeneralEventData();
            }
        }
        void    XADataProxy::handleApplicationTerminate()
        {
//            localCache = fopen(docfilePath,"ab+");
//            fwrite(uid,63,1,localCache);
//           
//            if(localCache!=NULL)
//            {
//                int cacheLength=XASendData::cache.size();
//                fwrite(&cacheLength,sizeof(int),1,localCache);
//                for(int i=0;i<XASendData::cache.size();i++)
//                {
//                    const char *data = XASendData::cache[i].c_str();
//                    int length=strlen(data);
//                    fwrite(&length,sizeof(int),1,localCache);
//                    fwrite(data,length,1,localCache);
//                }
//            }
//            fclose(localCache);
//            //send quit event 
//            char temp[64]={0};
//            cJSON *root=cJSON_CreateObject();
//            cJSON_AddItemToObject(root,"signedParams",XADataManager::getSignedParamsJsonObject());
//            cJSON *statArray=cJSON_CreateArray();
//            cJSON *statObject=cJSON_CreateObject();
//            cJSON_AddItemToObject(statObject,"eventName",cJSON_CreateString("user.quit"));
//            
//            cJSON *statObjectParams=cJSON_CreateObject();
//            unsigned int duration_time=XADataManager::getTimer()- idle_time;
//            sprintf(temp,"%u",duration_time);
//            cJSON_AddItemToObject(statObjectParams,"duration_time",cJSON_CreateString(temp));
//            cJSON_AddItemToObject(statObjectParams,"is_mobile",cJSON_CreateString("true"));
//            
//            cJSON_AddItemToObject(statObject,"params",statObjectParams);
//            memset(temp,0,64);
//            sprintf(temp,"%u",XADataManager::getTimestamp());
//            cJSON_AddItemToObject(statObject,"timestamp",cJSON_CreateString(temp));
//            cJSON_AddItemToObject(root,"stats",statArray);
//            XASendData::getMethodSend(cJSON_PrintUnformatted(root));
        }
        cJSON*  XADataProxy::quitEventData()
        {
            char temp[64]={0};
            cJSON *root=cJSON_CreateObject();
            cJSON_AddItemToObject(root,"signedParams",XADataManager::getSignedParamsJsonObject());
            cJSON *statArray=cJSON_CreateArray();
            cJSON *statObject=cJSON_CreateObject();
            cJSON_AddItemToObject(statObject,"eventName",cJSON_CreateString("user.quit"));
            cJSON *statObjectParams=cJSON_CreateObject();
            unsigned int duration_time=XADataManager::getTimer()- idle_time;
            sprintf(temp,"%u",duration_time);
            cJSON_AddItemToObject(statObjectParams,"duration_time",cJSON_CreateString(temp));
            cJSON_AddItemToObject(statObjectParams,"is_mobile",cJSON_CreateString("true"));
            
            cJSON_AddItemToObject(statObject,"params",statObjectParams);
            memset(temp,0,64);
            sprintf(temp,"%u",XADataManager::getTimestamp());
            cJSON_AddItemToObject(statObject,"timestamp",cJSON_CreateString(temp));
            cJSON_AddItemToArray(statArray,statObject);
            cJSON_AddItemToObject(root,"stats",statArray);
            return root;
        }
        void    XADataProxy::handleApplicationPause()
        {
            localCache = fopen(docfilePath,"ab+");
            char *quitData = cJSON_PrintUnformatted(quitEventData());
            //XAPRINT(quitData);
            int quitDataLength= strlen(quitData);
            if(localCache!=NULL)
            {
                fwrite(uid,63,1,localCache);//write uid
                
                fwrite(&quitDataLength,sizeof(int),1,localCache);
                fwrite(quitData,quitDataLength,1,localCache);
                
                int cacheLength=XASendData::cache.size();//write cache number
                fwrite(&cacheLength,sizeof(int),1,localCache);
                
                for(int i=0;i<XASendData::cache.size();i++)
                {
                    const char *data = XASendData::cache[i].c_str();
                    int length=strlen(data);
                    fwrite(&length,sizeof(int),1,localCache);
                    fwrite(data,length,1,localCache);
                }
            }
            fclose(localCache);
            
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
        void    XADataProxy::handleTrackUserIncrement(cJSON *userInfo)
        {
            handleInternalEvent(1,(userInfo));
        }
        void    XADataProxy::handleTrackUserUpdate(cJSON *userInfo)
        {
            handleInternalEvent(0,(userInfo));
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
            //XAPRINT(cJSON_PrintUnformatted(params));
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
                Lock lock(internalMutex);
                internalEventCache.push_back(encapsulateEvent(event,params));
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
                Lock lock(generalMutex);
                generalEventJSON ge;
                ge.event=generalEventJSONObject;
                strcpy(ge.appID,appId);
                strcpy(ge.userID,userId);
                generalEventCache.push_back(ge);
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