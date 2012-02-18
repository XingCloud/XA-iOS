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
        char *XADataManager::uid=NULL;
        short XADataManager::reportPolice=3;
        unsigned int  XADataManager::startTimer=0;
        XADataManager::XADataManager()
        {
            XingCloud::XAThreadPool::ExecuteTask::initThreadPool();
        }
        XADataManager::~XADataManager()
        {
            XingCloud::XAThreadPool::ExecuteTask::deleteThreadPool();
            delete uid;
            if(channelID!=NULL)
                delete channelID;
            if(appID!=NULL)
                delete appID;
              fclose(localCache);
        }
        void    XADataManager::handleHeartbeatTimer()
        {
            
        }
        void    XADataManager::handleEventTimer()
        {
            
        }
        cJSON* XADataManager::getSignedParamsJsonObject()
        {
            cJSON * signedParamsObject=cJSON_CreateObject();
            cJSON_AddItemToObject(signedParamsObject,"appid",cJSON_CreateString(appID));
            cJSON_AddItemToObject(signedParamsObject,"uid",cJSON_CreateString(uid));
            char  temp[64]={0};
            sprintf(temp,"%u",getTimestamp());
            cJSON_AddItemToObject(signedParamsObject,"timestamp",cJSON_CreateString(temp));
            return signedParamsObject;
        }
        int     XADataManager::getTimer()//return minute
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            if(!startTimer)
            {
                startTimer = tv.tv_sec;
                return  startTimer;
            }
            int interval=tv.tv_sec-startTimer;
            
            return interval/60;
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
            cJSON *visitJson=cJSON_CreateObject();
            cJSON_AddItemToObject(visitJson,"eventName",cJSON_CreateString("user.visit"));
            cJSON * visitParams=cJSON_CreateObject();
            cJSON_AddItemToObject(visitParams,"ref",cJSON_CreateString(channelID));
            cJSON_AddItemToObject(visitJson,"params",visitParams);
            char temp[64]={0};
            sprintf(temp,"%u",getTimestamp());
            cJSON_AddItemToObject(visitJson,"timestamp",cJSON_CreateString(temp));
            
            char appDir[512]={0};
            SystemInfo::getAppFileDir(appDir);
            char cacheDir[521]={0};
            sprintf(cacheDir,"%s/appCache.log",appDir);
            localCache=fopen(cacheDir,"ab+");
            if(fgetc(localCache)==EOF)
            {//本地文件不存在，发送update事件
                uid=new char[128];
                SystemInfo::getDeviceID(uid);
                fwrite(uid,127,1,localCache);
                xaDataProxy.handleApplicationLaunch(visitJson,NULL,NULL);
            
                //xaDataProxy.handleApplicationLaunch(visitJson,SystemInfo::getSystemInfo(getTimestamp()),NULL);
            }
            else
            {//本地文件存在，不发送update事件
                fseek(localCache,0,SEEK_SET);
                fread(uid,127,1,localCache);
                xaDataProxy.handleApplicationLaunch(visitJson,NULL,NULL);
            }
          
//          if(servicesEnable.crashReportEnable)
//          {
//              //cJSON * visitJson=cJSON_CreateObject();
//          }
            
        }
        void    XADataManager::applicationTerminate()
        {
            
        }
        void    XADataManager::applicationPause()
        {
            
        }
        void    XADataManager::applicationResume()
        {
            
        }
        //track  events
        void    XADataManager::trackCount(const char *action,const char *level1,const char *level2,const char *level3,const char *level4,const char *level5,int count)
        {
            
            cJSON * countParams=cJSON_CreateObject();
            cJSON_AddItemToObject(countParams,"type",cJSON_CreateString(action));
            cJSON_AddItemToObject(countParams,"level_1",cJSON_CreateString(level1));
            cJSON_AddItemToObject(countParams,"level_2",cJSON_CreateString(level2));
            cJSON_AddItemToObject(countParams,"level_3",cJSON_CreateString(level3));
            cJSON_AddItemToObject(countParams,"level_4",cJSON_CreateString(level4));
            cJSON_AddItemToObject(countParams,"level_5",cJSON_CreateString(level5));
           
            xaDataProxy.handleTrackCount(countParams);
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
            cJSON_AddItemToObject(tutorialParams,"index",cJSON_CreateString(index));
            cJSON_AddItemToObject(tutorialParams,"step_name",cJSON_CreateString(name));
            cJSON_AddItemToObject(tutorialParams,"tid",cJSON_CreateString(tutorial));
        
            xaDataProxy.handleTrackTutorialService(tutorialParams);
            

        }
        void    XADataManager::trackBuyService(const char *currency,const char *payType,const char *level1,const char *level2,const char *level3,const char *level4,const char *level5,int                             amount,int number)
        {

            char temp[64]={0};
            cJSON *buyParams=cJSON_CreateObject();
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