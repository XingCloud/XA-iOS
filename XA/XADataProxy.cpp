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
#include "curl.h"
namespace  XingCloud
{
    namespace XA
    {
        
        XADataProxy* XADataProxy::m_instance = new XADataProxy();
        
        XADataProxy::XADataProxy()
        {
            idle_time = 0;
            pause_time = 0;
            idle_time = 0;
            sendEventNumber=0;
            lastReadyEventNumber=0;
            uid=NULL;
            docfilePath=NULL;
            m_instance =this;
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
                //XASendData::getMethodSend(cJSON_PrintUnformatted(root));
                
                XASendData::instance()->postMethodSend(cJSON_PrintUnformatted(root),0);
                cJSON_Delete(root);
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
            if(SystemInfo::getNetType(NULL)!=0)
            {
                int i = 0;
                for(std::vector<localCacheEvent>::iterator iter = eventMemoryCache.begin() ;i<eventNumber && iter !=eventMemoryCache.end() ;i++,iter++)
                {
                    (*iter).isWriteTofile =true;
                }

            }
            if (eventNumber==0) {
                cJSON_Delete(internalStatArray);
                return;
            }
            cJSON *Internalroot=cJSON_CreateObject();
            cJSON_AddItemToObject(Internalroot,"signedParams",XADataManager::getSignedParamsJsonObject());
            cJSON_AddItemToObject(Internalroot,"stats",internalStatArray);
            
            XASendData::instance()->postMethodSend(cJSON_PrintUnformatted(Internalroot),eventNumber);
            cJSON_Delete(Internalroot);
        }
        void  XADataProxy::sendGeneralEventData(const char *appID,const char *userID,cJSON *generalObject,int eventNumber)
        {
            if(SystemInfo::getNetType(NULL)!=0)
            {
                int i = 0;
                for(std::vector<localCacheEvent>::iterator iter = eventMemoryCache.begin() ;i<eventNumber && iter !=eventMemoryCache.end() ;i++,iter++)
                {
                    (*iter).isWriteTofile =true;
                }
                
            }
            cJSON *root=cJSON_CreateObject();
            if(appID ==NULL || userID == NULL)
            {
                cJSON_Delete(root);
                return ;
            }
            cJSON_AddItemToObject(root,"signedParams",getGenSignedParamsObject(appID,userID,XADataManager::getTimestamp()));
            cJSON *generalStatArray=cJSON_CreateArray();
            cJSON_AddItemToArray(generalStatArray,generalObject);
            cJSON_AddItemToObject(root,"stats",generalStatArray);
            
            XASendData::instance()->postMethodSend(cJSON_PrintUnformatted(root),eventNumber);
            cJSON_Delete(root);
        }
        void  XADataProxy::readyForSendData(int lastReadyEventNumber)
        {
            Lock lock(eventCacheMutex);
            
            cJSON *internalStatArray=cJSON_CreateArray();
            
            int internalEventNumber=0;
            for(std::vector<localCacheEvent>::iterator iter=getInstance()->eventMemoryCache.begin()+lastReadyEventNumber; iter!= getInstance()->eventMemoryCache.end() ;iter++)
            {
                //发送的个数
                //if(((*iter).appID==NULL ||(*iter).userID== NULL)||(!strcmp(XADataManager::appID,(*iter).appID) && !strcmp(uid,(*iter).userID)))
                if((*iter).isInternal)
                {
                    //XAPRINT(cJSON_PrintUnformatted((*iter).jsonEvent));
                    cJSON_AddItemToArray(internalStatArray,cJSON_Parse((*iter).jsonEvent));
                    if(++internalEventNumber >50)
                    {
                        sendInternalEventData(internalStatArray,50);
                        
                        internalEventNumber = 0;
                        internalStatArray=NULL;
                        internalStatArray = cJSON_CreateArray();
                    }
                }
                else
                {
                    if(internalEventNumber!=0)
                    {
                        sendInternalEventData(internalStatArray,internalEventNumber);
                        internalEventNumber=0;
                        internalStatArray=cJSON_CreateArray();
                    }
                    sendGeneralEventData((*iter).appID,(*iter).userID,cJSON_Parse((*iter).jsonEvent),1);
                }
            }
            if(internalEventNumber!=0)
            {
                sendInternalEventData(internalStatArray,internalEventNumber);
            }
            else
            {
                cJSON_Delete(internalStatArray);
            }
            
        }
        
        void  XADataProxy::handleEventTimer()
        {//timer
            if(XADataManager::reportPolice==0)
                return ;
            if(XADataManager::reportPolice==3)
            {//default 
                if(eventMemoryCache.size()>=10)
                {
                    if(lastReadyEventNumber<=0)
                    {
                        lastReadyEventNumber=eventMemoryCache.size();
                        readyForSendData(0);
                        return ;
                    }
                    else if(lastReadyEventNumber>=eventMemoryCache.size())
                    {
                        readyForSendData(lastReadyEventNumber);
                        return ;
                    }
                }
                static int timebomb=0;
                timebomb+=20;
                if(timebomb==60)
                {
                    timebomb=0;
                    
                    if(eventMemoryCache.size()!=0)
                    {
                        lastReadyEventNumber=eventMemoryCache.size();
                        readyForSendData(0);
                    }
                    
                }
                
            }
            else
            {//batch
                WriteMemoryDataToFile(getInstance()->eventMemoryCache.size());
            }
        }
        void XADataProxy::WriteMemoryDataToFile(int dataNumber)
        {
            if(getInstance()->eventMemoryCache.size()==0)
            {
                return;
            }
            Lock lockcache(eventCacheMutex);
            Lock lock(filePointMutex);
            localfilePoint = fopen(docfilePath,"rb+");
            if(localfilePoint==NULL)
            {
                //XAPRINT("error  loacl Cache can not open "); 
                return ;
            }
            if(0!=fseek(localfilePoint,curValidPosition,SEEK_SET))
            {
                //XAPRINT("error  loacl Cache can not fseek "); 
                return ;
            }
            int i=0;
            for(std::vector<localCacheEvent>::iterator iter =getInstance()->eventMemoryCache.begin() ;iter !=getInstance()->eventMemoryCache.end() && i<dataNumber;i++)
            {
                if((*iter).isWriteTofile)
                {
                    continue;
                }
                const char *temp = (*iter).jsonEvent;
                
                if(temp!=NULL)
                {
                    unsigned short length = strlen(temp);
                    bool isWriteTofile=true;
                    unsigned int postion=(*iter).fileposition = ftell(localfilePoint);//数据的开始位置
                    if((*iter).isInternal)
                    {
                        fwrite(&isWriteTofile,1,1,localfilePoint);// 是否设备文件
                        fwrite(&length,sizeof(unsigned short),1,localfilePoint);//长度
                        fwrite(temp,length,1,localfilePoint);//
                        
                        fwrite(&postion,sizeof(unsigned int),1,localfilePoint);
                        bool b=true;
                        fwrite(&b,1,1,localfilePoint);//类型
                        
                    }
                    else
                    {
                        fwrite(&isWriteTofile,1,1,localfilePoint);            //是否设备文件
                        unsigned short appIDLength = strlen((*iter).appID);
                        fwrite(&appIDLength,sizeof(unsigned short),1,localfilePoint);//app length
                        fwrite((*iter).appID,appIDLength,1,localfilePoint);//app content
                        
                        unsigned short userIDLength = strlen((*iter).userID);
                        fwrite(&userIDLength, sizeof(unsigned short), 1, localfilePoint);//userLength
                        fwrite((*iter).userID, userIDLength, 1, localfilePoint); //userID content
                        
                        
                        fwrite(&length,sizeof(unsigned short),1,localfilePoint);
                        fwrite(temp,length,1,localfilePoint);
                        
                        fwrite(&postion,sizeof(unsigned int),1,localfilePoint);
                        bool b= false;
                        fwrite(&b,1,1,localfilePoint);
                    }
                    iter=getInstance()->eventMemoryCache.erase(iter);
                }
                else
                {
                    ++iter;
                }
                lastReadyEventNumber--;
                
            }//for
            curValidPosition=ftell(localfilePoint);
            if(0!=fseek(localfilePoint,63,SEEK_SET))
            {
                //XAPRINT("error  loacl Cache can not fseek "); 
                return ;
            }
            fwrite(&curValidPosition,sizeof(unsigned int),1,localfilePoint);//存下当前指fp位置
            fclose(localfilePoint);
        }
        void    XADataProxy::handleApplicationLaunch(cJSON *visitEvent,cJSON *updateEvent,cJSON *errorEvent)
        {
            char appDir[512]={0};
            SystemInfo::getAppFileDir(appDir);
            
            docfilePath = new char[521];
            
            sprintf(docfilePath,"%s/appCache.log",appDir);
            
            
            localfilePoint = fopen(docfilePath,"ab+");
            if(localfilePoint==NULL)
            {
                //XAPRINT("error  loacl Cache can not open "); 
                return ;
            }
            fseek(localfilePoint,0,SEEK_SET);
            if(fgetc(localfilePoint)==EOF)
            {//本地文件不存在，发送update事件
                
                fseek(localfilePoint,0,SEEK_SET);
                uid=new char[64];
                memset(uid,0,64);
                SystemInfo::getDeviceID(uid);
                fwrite(uid,63,1,localfilePoint);
                curValidPosition =sizeof(unsigned int)+63;
                fwrite(&curValidPosition,sizeof(unsigned int),1,localfilePoint);
                fclose(localfilePoint);
                
                handleGeneralEvent(0,NULL,NULL,XADataManager::getTimestamp(),SystemInfo::getSystemInfo(),true);
            }
            else  if(SystemInfo::getNetType(NULL)!=0)
            {//本地文件存在，不发送update事件
                
                Lock lock(filePointMutex);
                
                fseek(localfilePoint,0,SEEK_SET);
                uid=new char[64];
                memset(uid,0,64);
                fread(uid,63,1,localfilePoint);
                fread(&curValidPosition,sizeof(unsigned int),1,localfilePoint);
                
                unsigned int internalNumber=0;
                cJSON *internalStatArray=cJSON_CreateArray();
                //cJSON *eventJson;
                while(curValidPosition!=(63+sizeof(unsigned int)))
                {
                    fseek(localfilePoint,curValidPosition,SEEK_SET);
                    char temp[1024*2]={0};
                    unsigned short tempLength=0;
                    
                    if(fseek(localfilePoint,-1,SEEK_CUR)!=0)
                        break;// XAPRINT("set failed");
                    bool b=false;//
                    fread(&b,1,1,localfilePoint);
                    
                    fseek(localfilePoint,-sizeof(unsigned int)-1,SEEK_CUR);
                    
                    fread(&curValidPosition,sizeof(unsigned int),1,localfilePoint);
                    
                    fseek(localfilePoint,curValidPosition,SEEK_SET);
                    if(b)
                    {
                        //localCacheEvent lce;
                        bool isWriteTofile=false;
                        fread(&isWriteTofile,1,1,localfilePoint);
                        fread(&tempLength,sizeof(unsigned short),1,localfilePoint);
                        if(tempLength==0)
                        {
                            break;
                        }
                        fread(temp,tempLength,1,localfilePoint);
                        if(strlen(temp)==0)
                        {
                            break;
                        }
                        //lce.isWriteTofile = true;
                        //lce.isInternal = true;
                        //if(tempLength<1024)
                        //  strcpy(lce.jsonEvent,temp);
                        //lce.fileposition =curValidPosition;
                        //eventMemoryCache.push_back(lce);
                        
                        cJSON_AddItemToArray(internalStatArray,cJSON_Parse(temp));
                        if(++internalNumber>=50)
                        {
                            sendInternalEventData(internalStatArray,internalNumber);
                            internalNumber=0;
                            internalStatArray=cJSON_CreateArray();
                        }
                    }
                    else
                    {
                        if(internalNumber!=0)
                        {
                            sendInternalEventData(internalStatArray,internalNumber);
                            internalNumber=0;
                            internalStatArray=cJSON_CreateArray();
                        }
                        
                        //localCacheEvent lce; 
                        bool isWriteTofile=false;
                        fread(&isWriteTofile,1,1,localfilePoint);
                        
                        unsigned short appIDlen=0;
                        fread(&appIDlen,sizeof(unsigned short),1,localfilePoint);
                        char *appID = (char*)malloc(appIDlen+1);
                        memset(appID,0,appIDlen+1);
                        fread(appID,appIDlen,1,localfilePoint);
                        
                        unsigned short userIDlen=0;
                        fread(&userIDlen,sizeof(unsigned short),1,localfilePoint);
                        
                        char *userID = (char*)malloc(userIDlen+1);
                        memset(userID, 0,userIDlen+1);
                        fread(userID,userIDlen,1,localfilePoint);
                        
                        fread(&tempLength,sizeof(unsigned short),1,localfilePoint);
                        if(tempLength==0)
                        {
                            break;
                        }
                        fread(temp,tempLength,1,localfilePoint);
                        if(strlen(temp)==0)
                        {
                            break;
                        }
                        //                    //lce.isWriteTofile=true;
                        //                    //lce.isInternal =false;
                        //                    if(tempLength<1024)
                        //                        strcpy(lce.jsonEvent,temp);
                        //                    lce.fileposition =curValidPosition;
                        //                    
                        //                    eventMemoryCache.push_back(lce);
                        
                        sendGeneralEventData(appID,userID,cJSON_Parse(temp),1);
                        free(appID);
                        free(userID);
                        
                        
                    }
                }//while
                if(0!=fseek(localfilePoint,63,SEEK_SET))
                {
                    XAPRINT("error  loacl Cache can not fseek "); 
                    fclose(localfilePoint);
                }
                else
                {
                    fwrite(&curValidPosition,sizeof(unsigned int),1,localfilePoint);//存下当前指fp位置
                    fclose(localfilePoint);
                    if(internalNumber>0&&internalNumber<100000)
                    {
                        sendInternalEventData(internalStatArray,internalNumber);
                    }
                    else
                    {
                        if(internalNumber==0)
                            cJSON_Delete(internalStatArray);
                    }
                }
               
            }
            else
            {
                fclose(localfilePoint);
            }
           
            cJSON * initParams=cJSON_CreateObject();
            handleGeneralEvent(13,NULL,NULL,XADataManager::getTimestamp(),initParams,true);
            
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
            
            localfilePoint = fopen(docfilePath,"rb+");
            fseek(localfilePoint,curValidPosition,SEEK_SET);
            
            if(localfilePoint==NULL)
                return;
            cJSON *quitJson = quitEventData();
            char *quitData = cJSON_PrintUnformatted(quitJson);
            cJSON_Delete(quitJson);
            
            XAPRINT(quitData);
            
            unsigned int positon =ftell(localfilePoint);
            bool b=true;
            fwrite(&b, 1, 1, localfilePoint);//isWriteToDevice
            unsigned short quitDataLength= strlen(quitData);
            fwrite(&quitDataLength, sizeof(unsigned short), 1, localfilePoint);
            fwrite(quitData, quitDataLength, 1, localfilePoint);
            
            fwrite(&positon,sizeof(unsigned int),1,localfilePoint);
            fwrite(&b,1,1,localfilePoint);//类型
            
            curValidPosition = ftell(localfilePoint);
            if(0!=fseek(localfilePoint,63,SEEK_SET))
            {
                //XAPRINT("error  loacl Cache can not fseek "); 
                return ;
            }
            fwrite(&curValidPosition,sizeof(unsigned int),1,localfilePoint);//存下当前指fp位置
            
            fclose(localfilePoint);
            free(quitData);
            
            if(SystemInfo::getNetType(NULL)==0)
            {
                int i =0 ;
                for(std::vector<localCacheEvent>::iterator iter = eventMemoryCache.begin() ;i<sendEventNumber && iter !=eventMemoryCache.end() ;i++)
                {
                    (*iter).isWriteTofile =false;
                    iter=eventMemoryCache.erase(iter);
                }

            }
            WriteMemoryDataToFile(eventMemoryCache.size());
            lastReadyEventNumber = 0;
            pause_time = XADataManager::getTimer();
        }
        void    XADataProxy::handleApplicationTerminate()
        {
            
        }
        void    XADataProxy::handleApplicationResume()
        {//senddata
            
            idle_time +=( XADataManager::getTimer()- pause_time);
            
            if(SystemInfo::getNetType(NULL)==0)
                return;
            Lock lock(filePointMutex);
            localfilePoint = fopen(docfilePath,"rb+");
            
            Lock loc(eventCacheMutex);
            
            if(localfilePoint==NULL)
            {
                //XAPRINT("error  loacl Cache can not open "); 
                return ;
            }
            
            fseek(localfilePoint,0,SEEK_SET);
            
            fread(uid,63,1,localfilePoint);
            
            fread(&curValidPosition,sizeof(unsigned int),1,localfilePoint);
            
            unsigned int internalNumber=0;
            cJSON *internalStatArray=cJSON_CreateArray();
            
            
            while(curValidPosition!=(63+sizeof(unsigned int)))
            {
                fseek(localfilePoint,curValidPosition,SEEK_SET);
                char temp[1024]={0};
                unsigned short tempLength=0;
                
                if(fseek(localfilePoint,-1,SEEK_CUR)!=0)
                    break;// XAPRINT("set failed");
                bool b=false;//
                fread(&b,1,1,localfilePoint);
                
                fseek(localfilePoint,-sizeof(unsigned int)-1,SEEK_CUR);
                
                fread(&curValidPosition,sizeof(unsigned int),1,localfilePoint);
                
                fseek(localfilePoint,curValidPosition,SEEK_SET);
                if(b)
                {
                    //localCacheEvent lce;
                    bool isWriteTofile=false;
                    fread(&isWriteTofile,1,1,localfilePoint);
                    fread(&tempLength,sizeof(unsigned short),1,localfilePoint);
                    if(tempLength==0)
                    {
                        break;
                    }
                    fread(temp,tempLength,1,localfilePoint);
                    if(strlen(temp)==0)
                    {
                        break;
                    }
                    //lce.isWriteTofile = true;
                    //lce.isInternal = true;
                    //if(tempLength<1024)
                    //  strcpy(lce.jsonEvent,temp);
                    //lce.fileposition =curValidPosition;
                    //eventMemoryCache.push_back(lce);
                    if(isWriteTofile)
                        continue;
                    cJSON_AddItemToArray(internalStatArray,cJSON_Parse(temp));
                    if(++internalNumber>=50)
                    {
                        sendInternalEventData(internalStatArray,internalNumber);
                        internalNumber=0;
                        internalStatArray=cJSON_CreateArray();
                    }
                }
                else
                {
                    if(internalNumber!=0)
                    {
                        sendInternalEventData(internalStatArray,internalNumber);
                        internalNumber=0;
                        internalStatArray=cJSON_CreateArray();
                    }
                    //localCacheEvent lce; 
                    bool isWriteTofile=false;
                    fread(&isWriteTofile,1,1,localfilePoint);
                    
                    unsigned short appIDlen=0;
                    fread(&appIDlen,sizeof(unsigned short),1,localfilePoint);
                    char *appID = (char*)malloc(appIDlen+1);
                    memset(appID,0,appIDlen+1);
                    fread(appID,appIDlen,1,localfilePoint);
                    
                    unsigned short userIDlen=0;
                    fread(&userIDlen,sizeof(unsigned short),1,localfilePoint);
                    
                    char *userID = (char*)malloc(userIDlen+1);
                    memset(userID, 0,userIDlen+1);
                    fread(userID,userIDlen,1,localfilePoint);
                    
                    fread(&tempLength,sizeof(unsigned short),1,localfilePoint);
                    if(tempLength==0)
                    {
                        break;
                    }
                    fread(temp,tempLength,1,localfilePoint);
                    if(strlen(temp)==0)
                    {
                        break;
                    }
                    //                    //lce.isWriteTofile=true;
                    //                    //lce.isInternal =false;
                    //                    if(tempLength<1024)
                    //                        strcpy(lce.jsonEvent,temp);
                    //                    lce.fileposition =curValidPosition;
                    //                    
                    //                    eventMemoryCache.push_back(lce);
                    if(isWriteTofile)
                    {
                        free(appID);
                        free(userID);
                        continue;
                    }
                    sendGeneralEventData(appID,userID,cJSON_Parse(temp),1);
                    
                    
                }
                
            }//while
            if(0!=fseek(localfilePoint,63,SEEK_SET))
            {
                XAPRINT("error  loacl Cache can not fseek "); 
                fclose(localfilePoint);
            }
            else
            {
                fwrite(&curValidPosition,sizeof(unsigned int),1,localfilePoint);//存下当前指fp位置
                fclose(localfilePoint);
                if(internalNumber>0&&internalNumber<100000)
                {
                    sendInternalEventData(internalStatArray,internalNumber);
                }
                else
                {
                    if(internalNumber==0)
                        cJSON_Delete(internalStatArray);
                }
            }
            
        }
        void    XADataProxy::handleTrackLogin(cJSON *login)
        {
            handleGeneralEvent(4,NULL,NULL,XADataManager::getTimestamp(),login,true);
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
        void    XADataProxy::handleGeneralEvent(int event,const char *appId,const char *userId,unsigned int timestamp,cJSON *params,bool isInternal)
        {
            Lock lock(eventCacheMutex);
            char temp[64] = {0};
            eventString(event,temp);
            cJSON  *encapEvent=cJSON_CreateObject();
            cJSON_AddItemToObject(encapEvent,"eventName",cJSON_CreateString(temp));
            cJSON_AddItemToObject(encapEvent,"params",params);
            memset(temp,0,64);
            sprintf(temp,"%u",XADataManager::getTimestamp());
            cJSON_AddItemToObject(encapEvent,"timestamp",cJSON_CreateString(temp));
            
            
            //make cache 
            localCacheEvent lce; 
            if(appId !=NULL&& strlen(appId)!=0)
            {
                strcpy(lce.appID,appId);
            }
            const char *json = cJSON_PrintUnformatted(encapEvent);
            int jsonSize = strlen(json);
            if(jsonSize>0&&jsonSize<1024)
                strcpy(lce.jsonEvent,json);
            
            free(const_cast<char*>(json));
            
            if(userId!=NULL&&strlen(userId)!=0)
            {
                strcpy(lce.userID,userId);
            }
            
            lce.isInternal = isInternal;
            lce.isWriteTofile = false;
            
            eventMemoryCache.push_back(lce);
            if(XADataManager::reportPolice!=0)
            {
                cJSON_Delete(encapEvent);
            }
            else
            {
                handleEvent(event,timestamp,encapEvent,0);
            }
            
            
        }
        void    XADataProxy::handleEvent(int event,unsigned int timestamp,cJSON *encapsulateEvent,int eventIndex)
        {
            
            if(XADataManager::reportPolice==0)
            {//实时发送
                cJSON *root = cJSON_CreateObject();
                cJSON_AddItemToObject(root,"signedParams",XADataManager::getSignedParamsJsonObject());
                cJSON *statArray = cJSON_CreateArray();
                cJSON_AddItemToArray(statArray,encapsulateEvent);
                cJSON_AddItemToObject(root,"stats",statArray);
                
                XASendData::instance()->postMethodSend(cJSON_PrintUnformatted(root),1);
                cJSON_Delete(root);
            }
            
            //            else if(XADataManager::reportPolice== 1 )
            //            {//启动时批量发送
            //                XAPRINT("............%d",XADataManager::reportPolice);
            //            }
            //            else if(XADataManager::reportPolice == 3)
            //            {//定时定量发送
            //                XAPRINT("............%d",XADataManager::reportPolice);
            //            }
            //            else
            //            {
            //                
            //            }
        }
        void XADataProxy::handleSendSucess(int sendDataNumber)
        {
            
            Lock loc(eventCacheMutex);
            if(eventMemoryCache.size()>=sendDataNumber)
            {
                //                if(eventMemoryCache[sendDataNumber-1].isWriteTofile)
                //                {
                //                    //                Lock lock(filePointMutex);
                //                    //                localfilePoint = fopen(docfilePath,"rb+");
                //                    //                curValidPosition = getInstance()->eventMemoryCache[sendDataNumber-1].fileposition;
                //                    //                if(0!=fseek(localfilePoint,63,SEEK_SET))
                //                    //                {
                //                    //                    XAPRINT("error  loacl Cache can not fseek "); 
                //                    //                    return ;
                //                    //                }
                //                    //                fwrite(&curValidPosition,sizeof(unsigned int),1,localfilePoint);//存下当前指fp位置
                //                    //                fseek(localfilePoint,curValidPosition,SEEK_CUR);
                //                    //                fclose(localfilePoint);
                //                    
                //                    if(eventMemoryCache.size()>=sendDataNumber)
                //                        eventMemoryCache.erase(eventMemoryCache.begin(),eventMemoryCache.begin()+sendDataNumber);//删除前sendDataNumber个元素
                //                    if(lastReadyEventNumber>=sendDataNumber)
                //                    {
                //                        lastReadyEventNumber -= sendDataNumber;
                //                    }
                //                }
                //                else
                //                {
                std::vector<localCacheEvent>::iterator iter =eventMemoryCache.begin();
                int i=0;
                for(;i<sendDataNumber && iter!= getInstance()->eventMemoryCache.end();i++)
                {
                    
                    iter = getInstance()->eventMemoryCache.erase(iter);
                    lastReadyEventNumber--;
                }
                //}
            }
        }
        void XADataProxy::handleSendFailed(int sendDataNumber)
        {
            if(eventMemoryCache.size()>=sendDataNumber)
                WriteMemoryDataToFile(sendDataNumber);
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
                    break;
                case 13:
                    strcpy(source,"xa.init");
                    break;
                default:
                    break;
            }
            
        }
    }
}