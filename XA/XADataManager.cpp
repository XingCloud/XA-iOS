//
//  XADataManager.cpp
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//

#include "XADataManager.h"
#include "XASystemInfo.h"
#include "XAThreadPool.h"
#include<sys/time.h>
namespace XingCloud 
{
    namespace  XA
    {
        char *XADataManager::channelID=NULL;
        char *XADataManager::appID=NULL;
        bool  XADataManager::heartbeat;
        short XADataManager::reportPolice=3;
        
        unsigned int  XADataManager::startTimer=0;
        XADataManager::XADataManager()
        {
            XingCloud::XAThreadPool::ExecuteTask::initThreadPool(2);
        }
        XADataManager::~XADataManager()
        {
            XingCloud::XAThreadPool::ExecuteTask::deleteThreadPool();
            if(channelID!=NULL)
                delete channelID;
            if(appID!=NULL)
                delete appID;
        }
        void    XADataManager::handleHeartbeatTimer()
        {
            xaDataProxy.sendHeartbeatEventData();
        }
        void    XADataManager::handleEventTimer()
        {
            xaDataProxy.sendInternalEventData();
            xaDataProxy.sendGeneralEventData();
        }
        cJSON* XADataManager::getSignedParamsJsonObject()
        {
            cJSON * signedParamsObject=cJSON_CreateObject();
            cJSON_AddItemToObject(signedParamsObject,"appid",cJSON_CreateString(appID));
            cJSON_AddItemToObject(signedParamsObject,"uid",cJSON_CreateString(XADataProxy::uid));
            char  temp[64]={0};
            sprintf(temp,"%u",getTimestamp());
            cJSON_AddItemToObject(signedParamsObject,"timestamp",cJSON_CreateString(temp));
            
            return signedParamsObject;
        }
        int     XADataManager::getTimer()//return second
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            if(!startTimer)
            {
                startTimer = tv.tv_sec;
                return  startTimer;
            }
            int interval=tv.tv_sec-startTimer;
            
            return interval;
        }
        unsigned int    XADataManager::getTimestamp()
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            
            return   tv.tv_sec*(long int)1000+tv.tv_usec/(long int)1000;
        }
        void    XADataManager::setLogEnabled(bool value)
        {
            servicesEnable.logPrintEnable = value;
        }
        void    XADataManager::setCrashReportEnabled(bool value)
        {
            servicesEnable.crashReportEnable = value;
        }
        void    XADataManager::setHeartbeatEnabled(bool value)
        {
            servicesEnable.heartbeatEnable =value;
            heartbeat=value;
        }
        void    XADataManager::setReportPolicy(short reportPolice)
        {
            this->reportPolice = reportPolice;
        }
        void    XADataManager::setChannelID(const char *value)
        {
            channelID = new char[strlen(value)+1];
            strcpy(channelID,value);
        }
        void    XADataManager::setAppID(const char *value)
        {
            appID = new char[strlen(value)+1];
            strcpy(appID,value);
        } 
        //life cicle
        void    XADataManager::applicationLaunch()
        {
            //发送系统信息，user.update,user.visit,user.error事件
            startTimer=getTimer();
           
            cJSON * visitParams=cJSON_CreateObject();
            cJSON_AddItemToObject(visitParams,"is_mobile",cJSON_CreateString("true"));
            char temp[32]={0};
            sprintf(temp,"nonads=%s",XingCloud::XA::XADataManager::channelID);
            cJSON_AddItemToObject(visitParams,"ref",cJSON_CreateString(temp));
            xaDataProxy.handleApplicationLaunch(visitParams,NULL,NULL);
            
//          if(servicesEnable.crashReportEnable)
//          {
//              //cJSON * visitJson=cJSON_CreateObject();
//          }
            
        }
        void    XADataManager::applicationTerminate()
        {
            xaDataProxy.handleApplicationTerminate();
        }
        void    XADataManager::applicationPause()
        {
            xaDataProxy.handleApplicationPause();
        }
        void    XADataManager::applicationResume()
        {
            xaDataProxy.handleApplicationResume();
        }
        //track  events
        void    XADataManager::trackCount(const char *action,const char *level1,const char *level2,const char *level3,const char *level4,const char *level5,int count)
        {
            
            cJSON * countParams=cJSON_CreateObject();
            cJSON_AddItemToObject(countParams,"is_mobile",cJSON_CreateString("true"));
            cJSON_AddItemToObject(countParams,"type",cJSON_CreateString(action));
            cJSON_AddItemToObject(countParams,"level_1",cJSON_CreateString(level1));
            cJSON_AddItemToObject(countParams,"level_2",cJSON_CreateString(level2));
            cJSON_AddItemToObject(countParams,"level_3",cJSON_CreateString(level3));
            cJSON_AddItemToObject(countParams,"level_4",cJSON_CreateString(level4));
            cJSON_AddItemToObject(countParams,"level_5",cJSON_CreateString(level5));
            char temp[64]={0};
            sprintf(temp,"%d",count);
            cJSON_AddItemToObject(countParams,"count",cJSON_CreateString(temp));
           
            xaDataProxy.handleTrackCount(countParams);
        }
        void    XADataManager::trackUserIncrement(cJSON* userInfo)
        {
            xaDataProxy.handleTrackUserIncrement(userInfo);
        }
        void    XADataManager::trackUserUpdate(cJSON* userInfo)
        {
            xaDataProxy.handleTrackUserUpdate(userInfo);
        }
        void    XADataManager::trackMilestone(const char *milestoneName)
        {
            
            cJSON * milestoneParams=cJSON_CreateObject();
            cJSON_AddItemToObject(milestoneParams,"milestone_name",cJSON_CreateString(milestoneName));
            
            xaDataProxy.handleTrackMilestone(milestoneParams);
            
        }
        void    XADataManager::trackTransaction(const char *trans_id,const char *channel,const char*gross,const char *gcurrency,const char *vamount,const char *vcurrentcy)
        {
            cJSON * transParams=cJSON_CreateObject();
            cJSON_AddItemToObject(transParams,"is_mobile",cJSON_CreateString("true"));
            cJSON_AddItemToObject(transParams,"trans_id",cJSON_CreateString(trans_id));
            cJSON_AddItemToObject(transParams,"channel",cJSON_CreateString(channel));
            cJSON_AddItemToObject(transParams,"gross",cJSON_CreateString(gross));
            cJSON_AddItemToObject(transParams,"gcurrency",cJSON_CreateString(gcurrency));
            cJSON_AddItemToObject(transParams,"vamount",cJSON_CreateString(vamount));
            cJSON_AddItemToObject(transParams,"vcurrentcy",cJSON_CreateString(vcurrentcy));

            xaDataProxy.handleTrackTransaction(transParams);
        }
        void    XADataManager::trackTutorialService(const char *index,const char *name,const char *tutorial)
        {
        
            
            cJSON * tutorialParams=cJSON_CreateObject();
            cJSON_AddItemToObject(tutorialParams,"is_mobile",cJSON_CreateString("true"));
            cJSON_AddItemToObject(tutorialParams,"index",cJSON_CreateString(index));
            cJSON_AddItemToObject(tutorialParams,"step_name",cJSON_CreateString(name));
            cJSON_AddItemToObject(tutorialParams,"tid",cJSON_CreateString(tutorial));
        
            xaDataProxy.handleTrackTutorialService(tutorialParams);
            

        }
        void    XADataManager::trackBuyService(const char *currency,const char *payType,const char *level1,const char *level2,const char *level3,const char *level4,const char *level5,int                             amount,int number)
        {

            char temp[64]={0};
            cJSON *buyParams=cJSON_CreateObject();
            cJSON_AddItemToObject(buyParams,"is_mobile",cJSON_CreateString("true"));
            cJSON_AddItemToObject(buyParams,"resource",cJSON_CreateString(currency));
            cJSON_AddItemToObject(buyParams,"paytype",cJSON_CreateString(payType));
            cJSON_AddItemToObject(buyParams,"level_1",cJSON_CreateString(level1));
            cJSON_AddItemToObject(buyParams,"level_2",cJSON_CreateString(level2));
            cJSON_AddItemToObject(buyParams,"level_3",cJSON_CreateString(level3));
            cJSON_AddItemToObject(buyParams,"level_4",cJSON_CreateString(level4));
            cJSON_AddItemToObject(buyParams,"level_5",cJSON_CreateString(level5));
            memset(temp,0,64);
            sprintf(temp,"%d",amount);
            cJSON_AddItemToObject(buyParams,"amount",cJSON_CreateString(temp));
            memset(temp,0,64);
            sprintf(temp,"%d",number);
            cJSON_AddItemToObject(buyParams,"number",cJSON_CreateString(temp));
            
            xaDataProxy.handleTrackBuyService(buyParams);
        }
        void    XADataManager::generalEvent(int event,const char *appId,const char *userId,int timestamp,const char *params)
        {
                        
            xaDataProxy.handleGeneralEvent(event,appId,userId,timestamp,params);
        }
        
    }
}