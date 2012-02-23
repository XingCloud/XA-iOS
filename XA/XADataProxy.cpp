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

        Mutex   XADataProxy::filePointMutex;
        Mutex   XADataProxy::eventCacheMutex;;
        char* XADataProxy::docfilePath=NULL;
        char *XADataProxy::uid=NULL;
        unsigned int XADataProxy::idle_time = 0;
        bool  XADataProxy::isDeviceCache = true;
        int  XADataProxy::lastFilePosition;
        
        int  XADataProxy::currentFilePosition;
        int  XADataProxy::lastEventSize=0;
        int  XADataProxy::currentEventSize=0;
        int  XADataProxy::sendEventNumber=0;
        unsigned int XADataProxy::curValidPosition;
        std::vector<localCacheEvent>  XADataProxy::eventMemoryCache;
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

        }
        void XADataProxy::sendInternalEventData(cJSON *internalStatArray,int eventNumber)
        {
            cJSON *Internalroot=cJSON_CreateObject();
            cJSON_AddItemToObject(Internalroot,"signedParams",XADataManager::getSignedParamsJsonObject());
            cJSON_AddItemToObject(Internalroot,"stats",internalStatArray);
           
            XASendData::postMethodSend(cJSON_PrintUnformatted(Internalroot),eventNumber);
        }
        void  XADataProxy::sendGeneralEventData(const char *appID,const char *userID,cJSON *generalObject,int eventNumber)
        {
            
            cJSON *root=cJSON_CreateObject();
            cJSON_AddItemToObject(root,"signedParams",getGenSignedParamsObject(appID,userID,XADataManager::getTimestamp()));
            cJSON *generalStatArray=cJSON_CreateArray();
            cJSON_AddItemToArray(generalStatArray,generalObject);
            cJSON_AddItemToObject(root,"stats",generalStatArray);
            
            XASendData::postMethodSend(cJSON_PrintUnformatted(root),eventNumber);
        }
        void  XADataProxy::readyForSendInternalData()
        {
            Lock lock(eventCacheMutex);
            cJSON *internalStatArray=cJSON_CreateArray();
            int internalEventNumber=0;
          
            for(std::vector<localCacheEvent>::iterator iter = eventMemoryCache.begin(); iter!= eventMemoryCache.end();)
            {
                
                //发送的个数
                if(! strcmp(XADataManager::appID,(*iter).appID) || !strcmp(uid,(*iter).userID))
                {
                    cJSON_AddItemToArray(internalStatArray,(*iter).jsonEvent);
                    if(++internalEventNumber >50)
                    {
                        sendInternalEventData(internalStatArray,50);
                      
                        internalEventNumber = 0;
                        delete internalStatArray;
                        internalStatArray=NULL;
                        internalStatArray = cJSON_CreateArray();
                    }
                }
                else
                {
                    sendGeneralEventData((*iter).appID,(*iter).userID,(*iter).jsonEvent,1);
                    
                }
                
            }
            
            if(internalEventNumber!=0)
            {
                sendInternalEventData(internalStatArray,internalEventNumber);
              
            }
        }

        void  XADataProxy::handleEventData()
        {//timer
            if(XADataManager::reportPolice==0)
                return ;
            if(XADataManager::reportPolice==3)
            {
                static int timebomb=0;
                timebomb+=20;
                if(timebomb==60)
                {
                    timebomb=0;
                
                    if(eventMemoryCache.size()!=0)
                        readyForSendInternalData();
                }

            }
            WriteMemoryDataToFile(eventMemoryCache.size());
        }
        void XADataProxy::WriteMemoryDataToFile(int dataNumber)
        {
            if(eventMemoryCache.size()==0)
            {
                return;
            }
            Lock lock(filePointMutex);
            localCache = fopen(docfilePath,"rb+");
            if(localCache==NULL)
            {
                XAPRINT("error  loacl Cache can not open "); 
                return ;
            }
            if(0!=fseek(localCache,curValidPosition,SEEK_SET))
            {
                XAPRINT("error  loacl Cache can not fseek "); 
                return ;
            }
            int i=0;
            for(std::vector<localCacheEvent>::iterator iter =eventMemoryCache.begin() ;iter !=eventMemoryCache.end() && i<dataNumber;i++)
            {
                const char *temp = cJSON_PrintUnformatted((*iter).jsonEvent);
                unsigned short length = strlen(temp);
                bool isWriteTofile=true;
                unsigned int postion=(*iter).fileposition = ftell(localCache);//数据的开始位置
                if((*iter).isInternal)
                {
                    fwrite(&isWriteTofile,1,1,localCache);// 是否设备文件
                    fwrite(&length,sizeof(unsigned short),1,localCache);//长度
                    fwrite(temp,length,1,localCache);//
                    
                    fwrite(&postion,sizeof(unsigned int),1,localCache);
                    bool b=true;
                    fwrite(&b,1,1,localCache);//类型
                    
                }
                else
                {
                    fwrite(&isWriteTofile,1,1,localCache);            //是否设备文件
                    unsigned short appIDLength = strlen((*iter).appID);
                    fwrite(&appIDLength,sizeof(unsigned short),1,localCache);//app length
                    fwrite((*iter).appID,appIDLength,1,localCache);//app content
                    
                    unsigned short userIDLength = strlen((*iter).userID);
                    fwrite(&userIDLength, sizeof(unsigned short), 1, localCache);//userLength
                    fwrite((*iter).userID, userIDLength, 1, localCache); //userID content
                    
                   
                    fwrite(&length,sizeof(unsigned short),1,localCache);
                    fwrite(temp,length,1,localCache);
                    
                    fwrite(&postion,sizeof(unsigned int),1,localCache);
                    bool b= false;
                    fwrite(&b,1,1,localCache);
                }
                eventMemoryCache.erase(iter++);

            }
            curValidPosition=ftell(localCache);
            if(0!=fseek(localCache,63,SEEK_SET))
            {
                XAPRINT("error  loacl Cache can not fseek "); 
                return ;
            }
            fwrite(&curValidPosition,sizeof(unsigned int),1,localCache);//存下当前指fp位置
            fclose(localCache);
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
                isDeviceCache =false;
                fseek(XADataProxy::localCache,0,SEEK_SET);
                uid=new char[64];
                memset(uid,0,64);
                SystemInfo::getDeviceID(uid);
                fwrite(uid,63,1,localCache);
                curValidPosition =sizeof(unsigned int)+63;
                fwrite(&curValidPosition,sizeof(unsigned int),1,localCache);
                fclose(localCache);

                handleGeneralEvent(0,NULL,NULL,XADataManager::getTimestamp(),SystemInfo::getSystemInfo(),true);
            }
            else
            {//本地文件存在，不发送update事件
                Lock lock(filePointMutex);
                
                fseek(XADataProxy::localCache,0,SEEK_SET);
                uid=new char[64];
                memset(uid,0,64);
                fread(uid,63,1,XADataProxy::localCache);
                fread(&curValidPosition,sizeof(unsigned int),1,localCache);
                
                unsigned int internalNumber=0;
                cJSON *internalStatArray=cJSON_CreateArray();
                fseek(localCache,curValidPosition,SEEK_SET);
                while(curValidPosition!=(63+sizeof(unsigned int)))
                {
                    char temp[1024]={0};
                    unsigned short tempLength=0;
                    
                    if(fseek(localCache,-1,SEEK_CUR)!=0)
                        XAPRINT("set failed");
                    bool b=false;//
                    fread(&b,1,1,localCache);
                    
                    fseek(localCache,-sizeof(unsigned int)-1,SEEK_CUR);
                    
                    fread(&curValidPosition,sizeof(unsigned int),1,localCache);
                    
                    fseek(localCache,curValidPosition,SEEK_SET);
                    if(b)
                    {
                        localCacheEvent lce;
                        bool isWriteTofile=false;
                        fread(&isWriteTofile,1,1,localCache);
                        fread(&tempLength,sizeof(unsigned short),1,localCache);
                        if(tempLength==0)
                        {
                            break;
                        }
                        fread(temp,tempLength,1,localCache);
                        if(strlen(temp)==0)
                        {
                            break;
                        }
                        cJSON *eventJson = cJSON_Parse(temp);
                        lce.isWriteTofile = true;
                        lce.isInternal = true;
                        lce.jsonEvent =eventJson;
                        lce.fileposition =curValidPosition;
                        eventMemoryCache.push_back(lce);
                        cJSON_AddItemToArray(internalStatArray,eventJson);
                        if(++internalNumber>=50)
                        {
                            sendInternalEventData(internalStatArray,50);
                            internalStatArray=cJSON_CreateArray();
                        }
                    }
                    else
                    {
                        
                        sendInternalEventData(internalStatArray,internalNumber);
                        internalNumber=0;
                        internalStatArray=cJSON_CreateArray();
                        
                        localCacheEvent lce;
                        bool isWriteTofile=false;
                        fread(&isWriteTofile,1,1,localCache);
                        
                        unsigned short appIDlen=0;
                        fread(&appIDlen,sizeof(unsigned short),1,localCache);
                        lce.appID = new char[appIDlen+1];
                        memset(lce.appID,0,appIDlen+1);
                        fread(lce.appID,appIDlen,1,localCache);
                        
                        unsigned short userIDlen=0;
                        fread(&userIDlen,sizeof(unsigned short),1,localCache);
                        lce.userID = new char[userIDlen+1];
                        memset(lce.userID,0,userIDlen+1);
                        fread(lce.userID,userIDlen,1,localCache);
                        
                        fread(&tempLength,sizeof(unsigned short),1,localCache);
                        if(tempLength==0)
                        {
                            break;
                        }
                        fread(temp,tempLength,1,localCache);
                        if(strlen(temp)==0)
                        {
                            break;
                        }
                        cJSON *eventJson = cJSON_Parse(temp);
                        
                        lce.isWriteTofile=true;
                        lce.isInternal =false;
                        lce.jsonEvent =eventJson;
                        lce.fileposition =curValidPosition;
                        
                        eventMemoryCache.push_back(lce);
                        
                        sendGeneralEventData(lce.appID, lce.userID,eventJson,1);
                       
                    }
                    
                }//while
                if(0!=fseek(localCache,63,SEEK_SET))
                {
                    XAPRINT("error  loacl Cache can not fseek "); 
                    return ;
                }
                fwrite(&curValidPosition,sizeof(unsigned int),1,localCache);//存下当前指fp位置
                fclose(localCache);
            }
                
            handleGeneralEvent(2,NULL,NULL,XADataManager::getTimestamp(),visitEvent,true);
            if(errorEvent!=NULL)
                handleGeneralEvent(5,NULL,NULL,XADataManager::getTimestamp(),errorEvent,true);
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
            localCache = fopen(docfilePath,"rb+");
            fseek(localCache,curValidPosition,SEEK_SET);
            
            if(localCache==NULL)
                return;
            char *quitData = cJSON_PrintUnformatted(quitEventData());
            XAPRINT(quitData);
            unsigned int positon =ftell(localCache);
            bool b=true;
            fwrite(&b, 1, 1, localCache);//isWriteToDevice
            unsigned short quitDataLength= strlen(quitData);
            fwrite(&quitDataLength, sizeof(unsigned short), 1, localCache);
            fwrite(quitData, quitDataLength, 1, localCache);
            
            
            fwrite(&positon,sizeof(unsigned int),1,localCache);
            fwrite(&b,1,1,localCache);//类型
            
            curValidPosition = ftell(localCache);
            if(0!=fseek(localCache,63,SEEK_SET))
            {
                XAPRINT("error  loacl Cache can not fseek "); 
                return ;
            }
            fwrite(&curValidPosition,sizeof(unsigned int),1,localCache);//存下当前指fp位置
           
            fclose(localCache);
            
            WriteMemoryDataToFile(eventMemoryCache.size());
            
            pause_time = XADataManager::getTimer();
        }
        void    XADataProxy::handleApplicationTerminate()
        {
                
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
            cJSON *encapEvent = encapsulateEvent(event,params);
            XAPRINT(cJSON_PrintUnformatted(encapEvent));
            //make cache 
            localCacheEvent lce ; 
            if(appId !=NULL)
            {
                int size =strlen(appId)+1;
                lce.appID = new char[size];
                memset(lce.appID,0,size);
                strcpy(lce.appID,appId);
            }
               
            lce.jsonEvent = encapEvent;
            if(userId!=NULL)
            {
                int size =strlen(userId)+1;
                lce.userID = new char[size];
                memset(lce.userID,0,size);
                strcpy(lce.userID,userId);
            }
                
            lce.isInternal = isInternal;
            
            eventMemoryCache.push_back(lce);
            
            handleEvent(event,timestamp,encapEvent,0);
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
               
                XASendData::postMethodSend(cJSON_PrintUnformatted(root),1);
            }
            else if(XADataManager::reportPolice==1)
            {//启动时批量发送
                
            }
            else if(XADataManager::reportPolice==3)
            {//定时定量发送
                
                if(eventMemoryCache.size()>=10)
                {
                    readyForSendInternalData();
                }
            }
            else
            {
                
            }
        }
        void XADataProxy::handleSendSucess(int sendDataNumber)
        {
            Lock lock(filePointMutex);
            if(eventMemoryCache[sendDataNumber-1].isWriteTofile)
            {
                localCache = fopen(docfilePath,"rb+");
                curValidPosition = eventMemoryCache[sendDataNumber-1].fileposition;
                if(0!=fseek(localCache,63,SEEK_SET))
                {
                    XAPRINT("error  loacl Cache can not fseek "); 
                    return ;
                }
                fwrite(&curValidPosition,sizeof(unsigned int),1,localCache);//存下当前指fp位置
                fseek(localCache,curValidPosition,SEEK_CUR);
                fclose(localCache);
               
            }
            else
            {
                std::vector<localCacheEvent>::iterator iter =eventMemoryCache.begin();
                int i=0;
                for(;i<sendDataNumber && iter!= eventMemoryCache.end();i++)
                {
                    eventMemoryCache.erase(iter++);
                }
            }
            
        }
        void XADataProxy::handleSendFailed(int sendDataNumber)
        {
            WriteMemoryDataToFile(sendDataNumber);
        }
        void    XADataProxy::MoveFilePositon()
        {
            curValidPosition = eventMemoryCache[sendEventNumber-1].fileposition;
//            if(0!=fseek(localCache,63,SEEK_SET))
//            {
//                XAPRINT("error  loacl Cache can not fseek "); 
//                return ;
//            }
//            fwrite(&curValidPosition,sizeof(unsigned int),1,localCache);//存下当前指fp位置
//            fseek(localCache,curValidPosition,SEEK_CUR);
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