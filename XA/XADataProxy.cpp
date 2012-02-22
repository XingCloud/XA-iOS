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

namespace  XingCloud
{
    namespace XA
    {
        
        
        FILE   *XADataProxy::localCache;

        Mutex   XADataProxy::eventCacheMutex;
        Mutex   XADataProxy::fileMutex;
        char* XADataProxy::docfilePath=NULL;
        char *XADataProxy::uid=NULL;
        unsigned int XADataProxy::idle_time = 0;
        
        int  XADataProxy::lastFilePosition;
        
        int  XADataProxy::currentFilePosition;
        int  XADataProxy::lastEventSize=0;
        int  XADataProxy::currentEventSize=0;
        std::map<int,localcacheEvent*> XADataProxy::eventCache;
        
        XADataProxy::XADataProxy()
        {
            idle_time = 0;
            pause_time = 0;
        }
        XADataProxy::~XADataProxy()
        {
            if(docfilePath!=NULL)
                delete docfilePath;
            if(uid != NULL)
                delete uid;
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
//            if(SystemInfo::getNetType(NULL)!=0)
//            {
//                Lock lock(XASendData::postMutex);
//                for(std::map<int,std::string>::iterator iter = XASendData::cache.begin();iter !=XASendData::cache.end();iter++)
//                    XASendData::postMethodSend(iter->second.c_str());
//                XASendData::cache.clear();
//            }
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
        void XADataProxy::sendInternalEventData(cJSON *internalStatArray)
        {
            cJSON *Internalroot=cJSON_CreateObject();
            cJSON_AddItemToObject(Internalroot,"signedParams",XADataManager::getSignedParamsJsonObject());
            cJSON_AddItemToObject(Internalroot,"stats",internalStatArray);
            
            XASendData::postMethodSend(cJSON_PrintUnformatted(Internalroot));
        }
        void  XADataProxy::sendGeneralEventData(const char *appID,const char *userID,cJSON *generalObject)
        {
            cJSON *root=cJSON_CreateObject();
            cJSON_AddItemToObject(root,"signedParams",getGenSignedParamsObject(appID,userID,XADataManager::getTimestamp()));
            cJSON *generalStatArray=cJSON_CreateArray();
            cJSON_AddItemToArray(generalStatArray,generalObject);
            cJSON_AddItemToObject(root,"stats",generalStatArray);
            
            
             XASendData::postMethodSend(cJSON_PrintUnformatted(root));
        }
        void  XADataProxy::readyForSendInternalData()
        {
            Lock lock(eventCacheMutex);
            cJSON *internalStatArray=cJSON_CreateArray();
            
            int internalEventNumber=0;
            
            for(std::map<int,localcacheEvent*>::iterator iter = eventCache.begin(); iter!= eventCache.end();iter++)
            {
                if(! strcmp(XADataManager::appID,iter->second->appID) || !strcmp(uid,iter->second->userID))
                {
                    cJSON_AddItemToArray(internalStatArray,iter->second->jsonEvent);
                    if(++internalEventNumber >50)
                    {
                        sendInternalEventData(internalStatArray);
                        internalEventNumber = 0;
                        delete internalStatArray;
                        internalStatArray=NULL;
                        internalStatArray = cJSON_CreateArray();
                    }
                }
                else
                {
                    sendGeneralEventData(iter->second->appID,iter->second->userID,iter->second->jsonEvent);
                    
                    //                    if(!isAssign)
                    //                    {
                    //                        isAssign =true;
                    //                        strcpy(tempAppID,iter->second->appID);
                    //                        strcpy(tempUserID,iter->second->userID);
                    //                        cJSON_AddItemToArray(generalStatArray,iter->second->jsonEvent);
                    //                    }
                    //                    else
                    //                    {
                    //                        if(! strcmp(tempAppID,iter->second->appID) || !strcmp(tempAppID,iter->second->userID))
                    //                        {//the same to last appID,userID
                    //                            
                    //                            cJSON_AddItemToArray(generalStatArray,iter->second->jsonEvent);
                    //                            if(++generalEventNumber>50)
                    //                            {
                    //                                sendGeneralEventData(iter->second->appID,iter->second->userID,generalStatArray);
                    //                                generalEventNumber = 0;
                    //                                delete generalStatArray;
                    //                                generalStatArray = NULL;
                    //                                generalStatArray =cJSON_CreateArray();
                    //                            }
                    //                                
                    //                        }
                    //                        else
                    //                        {//如果appID,或者userID上次不一样，则发送上次的，缓存本次的
                    //                            sendGeneralEventData(tempAppID,tempUserID,generalStatArray);
                    //                           
                    //                            delete generalStatArray;
                    //                            generalStatArray= cJSON_CreateArray();
                    //                            strcpy(tempAppID,iter->second->appID);
                    //                            strcpy(tempUserID,iter->second->userID);
                    //                            cJSON_AddItemToArray(generalStatArray,iter->second->jsonEvent);
                    //                        }
                    //                    }
                    
                    
                }
            }
            
            if(internalEventNumber!=0)
                sendInternalEventData(internalStatArray);
            
        }
        void  XADataProxy::writeCacheToFile()
        {
            Lock lock(eventCacheMutex);
            lastEventSize = currentEventSize;
            if(eventCache.size() == 0)
                return ;
            localCache = fopen(docfilePath,"ab+");
            if(localCache==NULL)
                return;//容错
            lastFilePosition =ftell(localCache);
            for(std::map<int,localcacheEvent*>::iterator iter = eventCache.begin(); iter!= eventCache.end();iter++)
            {
                if(!iter->second->isWriteTofile)
                {
                    const char *temp = cJSON_PrintUnformatted(iter->second->jsonEvent);
                    unsigned short length = strlen(temp);
                    if(iter->second->isInternal)
                    {
                        bool b=true;
                        fwrite(&b,1,1,localCache);
                        fwrite(&length,sizeof(unsigned short),1,localCache);
                        fwrite(temp,length,1,localCache);
                    }
                    else
                    {
                        bool b= false;
                        fwrite(&b,1,1,localCache);
                        unsigned short appIDLength = strlen(iter->second->appID);
                        fwrite(&appIDLength,sizeof(unsigned short),1,localCache);
                        fwrite(iter->second->appID,appIDLength,1,localCache);
                        unsigned short userIDLength = strlen(iter->second->userID);
                        fwrite(&userIDLength, sizeof(unsigned short), 1, localCache);
                        fwrite(iter->second->userID, userIDLength, 1, localCache);
                        
                        fwrite(&length,sizeof(unsigned short),1,localCache);
                        fwrite(temp,length,1,localCache);
                    }
                    iter->second->isWriteTofile=true;
                }
            }
        
            currentEventSize = eventCache.size();
            
            currentFilePosition = ftell(localCache);
            fclose(localCache);
        }
        void  XADataProxy::handleEventData()
        {//timer
            if(XADataManager::reportPolice==3)
            {
                static int timebomb=0;
                timebomb+=20;
                if(timebomb==60)
                {
                    timebomb=0;
                
                    if(eventCache.size()!=0)
                        readyForSendInternalData();
                }

            }
            writeCacheToFile();
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
            fseek(XADataProxy::localCache,0,SEEK_SET);
            if(fgetc(localCache)==EOF)
            {//本地文件不存在，发送update事件
                fseek(XADataProxy::localCache,0,SEEK_SET);
                uid=new char[64];
                memset(uid,0,64);
                SystemInfo::getDeviceID(uid);
                fwrite(uid,63,1,localCache);
                lastFilePosition = currentFilePosition = ftell(localCache);
                fclose(localCache);

                handleGeneralEvent(0,NULL,NULL,XADataManager::getTimestamp(),SystemInfo::getSystemInfo(),true);
            }
            else
            {//本地文件存在，不发送update事件
                fseek(XADataProxy::localCache,0,SEEK_SET);
                uid=new char[64];
                memset(uid,0,64);
                fread(uid,63,1,XADataProxy::localCache);
                if(localCache!=NULL)
                {//sending  cache  event
                    char temp[1024]={0};
                    unsigned short tempLength=0;
                    cJSON *internalStatArray=cJSON_CreateArray();
                    int internalEventNumber=0;
                    while(fgetc(localCache)!=EOF)
                    {
                        if(fseek(localCache,-1,SEEK_CUR)!=0)
                            XAPRINT("set failed");
                        bool b=false;//
                        fread(&b,1,1,localCache);
                        memset(temp,0,1024);
                        tempLength=0;
                        if(b)
                        {
                            fread(&tempLength,sizeof(unsigned short),1,localCache);
                            fread(&temp,tempLength,1,localCache);
                            cJSON_AddItemToArray(internalStatArray,cJSON_Parse(temp));
                            if(++internalEventNumber>50)
                            {
                                sendInternalEventData(internalStatArray);
                                internalEventNumber = 0;
                                delete internalStatArray;
                                internalStatArray=NULL;
                                internalStatArray = cJSON_CreateArray();
                            }
                        }
                        else
                        {
                            unsigned short tempNumber=0;
                            fread(&tempNumber,sizeof(unsigned short),1,localCache);
                            char *appID = new char[tempNumber+1];
                            memset(appID,0,tempNumber+1);
                            fread(appID,tempNumber,1,localCache);
                            tempNumber =0;
                            fread(&tempNumber,sizeof(unsigned short),1,localCache);
                            char *userID = new char[tempNumber+1];
                            memset(userID,0,tempNumber+1);
                            fread(&tempLength,sizeof(unsigned short),1,localCache);
                            fread(&temp,tempLength,1,localCache);
                            sendGeneralEventData(appID, userID,cJSON_Parse(temp));
                        }
                    }
                    int currentPostion =ftell(localCache);
                    fseek(localCache,63,SEEK_SET);
                    lastFilePosition = currentFilePosition = ftell(localCache);//移到uid之后为本次程序运行初始位置
                    if((currentPostion - lastFilePosition)!=0)
                    {
                        char *initData = new char[currentPostion - lastFilePosition];
                        memset(initData,-1,currentPostion - lastFilePosition);
                        fwrite(initData,currentPostion - lastFilePosition , 1,localCache);
                    }
                    if(internalEventNumber!=0)
                        sendInternalEventData(internalStatArray);
                    fclose(localCache);
                }
                
            }
            
            handleGeneralEvent(2,NULL,NULL,XADataManager::getTimestamp(),visitEvent,true);
            if(errorEvent!=NULL)
                handleGeneralEvent(5,NULL,NULL,XADataManager::getTimestamp(),errorEvent,true);
            
        }
        void    XADataProxy::handleApplicationTerminate()
        {
            
        }
        cJSON*  XADataProxy::quitEventData()
        {
            char temp[64]={0};

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

            return statObject;
        }
        void    XADataProxy::handleApplicationPause()
        {
            localCache = fopen(docfilePath,"ab+");
            if(localCache==NULL)
                return;
            char *quitData = cJSON_PrintUnformatted(quitEventData());
            XAPRINT(quitData);
            unsigned short quitDataLength= strlen(quitData);
            bool b=true;
            fwrite(&b, 1, 1, localCache);
            fwrite(&quitDataLength, sizeof(unsigned short), 1, localCache);
            fwrite(quitData, quitDataLength, 1, localCache);
            lastFilePosition = ftell(localCache);
            fclose(localCache);
            
            writeCacheToFile();
            
            pause_time = XADataManager::getTimer();
        }
        
        void    XADataProxy::handleApplicationResume()
        {
            idle_time +=( XADataManager::getTimer()- pause_time);
        }
        void    XADataProxy::handleTrackCount(cJSON *countEvent)
        {
            handleGeneralEvent(9,NULL,NULL,XADataManager::getTimestamp(),countEvent,true);
        }
        void    XADataProxy::handleTrackUserIncrement(cJSON *userInfo)
        {
            handleGeneralEvent(1,NULL,NULL,XADataManager::getTimestamp(),userInfo,true);
        }
        void    XADataProxy::handleTrackUserUpdate(cJSON *userInfo)
        {
            handleGeneralEvent(0,NULL,NULL,XADataManager::getTimestamp(),userInfo,true);
        }
        void    XADataProxy::handleTrackMilestone(cJSON *milestoneEvent)
        {
            handleGeneralEvent(10,NULL,NULL,XADataManager::getTimestamp(),milestoneEvent,true);
        }
        void    XADataProxy::handleTrackTransaction(cJSON *transactionEvent)
        {
            handleGeneralEvent(7,NULL,NULL,XADataManager::getTimestamp(),transactionEvent,true);
        }
        void    XADataProxy::handleTrackTutorialService(cJSON *tutorialEvent)
        {
            handleGeneralEvent(11,NULL,NULL,XADataManager::getTimestamp(),tutorialEvent,true);
        }
        void    XADataProxy::handleTrackBuyService(cJSON *buyEvent)
        {
            handleGeneralEvent(8,NULL,NULL,XADataManager::getTimestamp(),buyEvent,true);
        }
        
        cJSON *XADataProxy::getGenSignedParamsObject(const char *appId,const char *userId,int timestamp)
        {//get general SignedParams
            
            cJSON *signedJson=cJSON_CreateObject();
            cJSON_AddItemToObject(signedJson,"appid",cJSON_CreateString(appId));
            cJSON_AddItemToObject(signedJson,"uid",cJSON_CreateString(userId));
            char temp[64]={0};
            sprintf(temp,"%u",timestamp);
            cJSON_AddItemToObject(signedJson,"timestamp",cJSON_CreateString(temp));
            
            return signedJson;
        }
       
        void    XADataProxy::handleGeneralEvent(int event,const char *appId,const char *userId,unsigned int timestamp,cJSON *params,bool isInternal)
        {
            Lock lock(eventCacheMutex);
            static int eventIndex = 0;
            cJSON *encapEvent = encapsulateEvent(event,params);
            //make cache 
            localcacheEvent *lce = new localcacheEvent[sizeof(localcacheEvent)];
            if(appId !=NULL)
                strcpy(lce->appID,appId);
            lce->jsonEvent = encapEvent;
            if(userId!=NULL)
                strcpy(lce->userID,userId);
            lce->isInternal = isInternal;
            eventCache[eventIndex] = lce;
            
            handleEvent(event,timestamp,encapEvent,eventIndex);
        }
        cJSON*    XADataProxy::encapsulateEvent(int event,cJSON *params)
        {
            char temp[64] = {0};
            eventString(event,temp);
            cJSON  *eventJson=cJSON_CreateObject();
            cJSON_AddItemToObject(eventJson,"eventName",cJSON_CreateString(temp));
            cJSON_AddItemToObject(eventJson,"params",params);
            memset(temp,0,64);
            sprintf(temp,"%u",XADataManager::getTimestamp());
            cJSON_AddItemToObject(eventJson,"timestamp",cJSON_CreateString(temp));
            return eventJson;
        }
        void    XADataProxy::handleEvent(int event,unsigned int timestamp,cJSON *encapsulateEvent,int eventIndex)
        {
            //XAPRINT(cJSON_PrintUnformatted(encapsulateEvent));
            
            if(XADataManager::reportPolice==0)
            {//实时发送
                cJSON *root = cJSON_CreateObject();
                cJSON_AddItemToObject(root,"signedParams",XADataManager::getSignedParamsJsonObject());
                cJSON *statArray = cJSON_CreateArray();
                cJSON_AddItemToArray(statArray,encapsulateEvent);
                cJSON_AddItemToObject(root,"stats",statArray);
                XASendData::postMethodSend(cJSON_PrintUnformatted(root));
            }
            else if(XADataManager::reportPolice==1)
            {//启动时批量发送
                
            }
            else if(XADataManager::reportPolice==3)
            {//定时定量发送
                
                if(eventCache.size()>=10)
                {
                    handleEventData();
                }
            }
            else
            {
                
            }
        }
        void XADataProxy::handleSendSucess()
        {
            Lock lock(eventCacheMutex);
            if(XADataManager::reportPolice==0)
            {//realtime
                eventCache.clear();
            }
            else if(XADataManager::reportPolice==1)
            {//启动时候发送
            }
            else if(XADataManager::reportPolice==3)
            {
                if(lastEventSize != currentEventSize)
                {
                    int size = currentFilePosition- lastFilePosition;
                    char *unKnowData = new char[size];
                    memset(unKnowData,-1,currentFilePosition-lastFilePosition);
                    localCache = fopen(docfilePath,"ab+");
                    fseek(localCache,lastFilePosition,SEEK_SET);
                    fwrite(unKnowData,size,1,localCache);
                    fclose(localCache);
                }
                std::map<int,localcacheEvent*>::iterator iterBegin =eventCache.find(lastEventSize);
                std::map<int,localcacheEvent*>::iterator iterEnd =eventCache.find(currentEventSize);
                eventCache.erase(iterBegin,iterEnd);
                
                if(eventCache.size() == currentEventSize)
                {   
                    eventCache.clear();
                }
            }

        }
        void XADataProxy::handleSendFailed()
        {
            if(XADataManager::reportPolice==0)
            {//
                writeCacheToFile();
            }
            else if(XADataManager::reportPolice==1)
            {
            }
            else if(XADataManager::reportPolice==3)
            {//DEFAULT
                writeCacheToFile();
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