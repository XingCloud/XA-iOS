//
//  XADataManager.cpp
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//

#include "XADataManager.h"
#include "XASystemInfo.h"
#include<sys/time.h>
namespace XingCloud 
{
    namespace  XA
    {
        XADataManager::XADataManager()
        {
            channelID=NULL;
            appID=NULL;
            uid=new char[32];
            SystemInfo::getDeviceID(uid);
        }
        XADataManager::~XADataManager()
        {
            delete uid;
            if(channelID!=NULL)
                delete channelID;
            if(appID!=NULL)
                delete appID;
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
            static int sys_time = 0;
            
            struct timeval tv;
            gettimeofday(&tv, NULL);
            if(!sys_time)
            {
                sys_time = tv.tv_sec;
                return  0;
            }
            int interval=tv.tv_sec-sys_time;
            sys_time=tv.tv_sec;
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
            localCache=fopen(cacheDir,"wb+");
            if(feof(localCache))
            {//本地文件不存在，发送update事件
                xaDataProxy.handleApplicationLaunch(visitJson,SystemInfo::getSystemInfo(getTimestamp()),NULL);
            }
            else
            {//本地文件存在，不发送update事件
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
        void    XADataManager::trackTransaction(int event,const char *orderId,const char *cost,const char *money,const char *category,const char *name)
        {
//            cJSON *transJson=cJSON_CreateObject();
//            cJSON_AddItemToObject(transJson,"eventName",cJSON_CreateString("pay.complete"));
//            
              cJSON * transParams=cJSON_CreateObject();
//            cJSON_AddItemToObject(transParams,"trans_id",cJSON_CreateString(orderId));
//            cJSON_AddItemToObject(transParams,"gross",cJSON_CreateString(cost));
//            //cJSON_AddItemToObject(transParams,"",cJSON_CreateString(tutorial));
//            
//            char temp[64]={0};
//            sprintf(temp,"%u",getTimestamp());
//            cJSON_AddItemToObject(transJson,"timestamp",cJSON_CreateString(temp));
//            
//            cJSON_AddItemToObject(transJson,"params",transParams);
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
        void    XAPRINT(const char *fmt,...)
        {
            char sprint_buf[128];
            
            va_list args;
            
            va_start(args, fmt);
            
            vsprintf(sprint_buf, fmt, args);
            
            va_end(args);
#ifdef IOS
            NSLog(@"%s",sprint_buf);
#endif
            
        }

    }
}