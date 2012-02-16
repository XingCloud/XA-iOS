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
            
            if(servicesEnable.crashReportEnable)
            {
                //cJSON * visitJson=cJSON_CreateObject();
            }
            xaDataProxy.handleApplicationLaunch(visitJson,SystemInfo::getSystemInfo(getTimestamp()),NULL);
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
            cJSON *countJson=cJSON_CreateObject();
            char temp[64]={0};
            sprintf(temp,"%u",getTimestamp());
            cJSON_AddItemToObject(countJson,"timestamp",cJSON_CreateString(temp));
            cJSON_AddItemToObject(countJson,"eventName",cJSON_CreateString("count"));
            
            cJSON * countParams=cJSON_CreateObject();
            cJSON_AddItemToObject(countParams,"type",cJSON_CreateString(action));
            cJSON_AddItemToObject(countParams,"level_1",cJSON_CreateString(level1));
            cJSON_AddItemToObject(countParams,"level_2",cJSON_CreateString(level2));
            cJSON_AddItemToObject(countParams,"level_3",cJSON_CreateString(level3));
            cJSON_AddItemToObject(countParams,"level_4",cJSON_CreateString(level4));
            cJSON_AddItemToObject(countParams,"level_5",cJSON_CreateString(level5));
            memset(temp,0,64);
            sprintf(temp,"%d",count);
            cJSON_AddItemToObject(countParams,"amount",cJSON_CreateString(temp));
            
            cJSON_AddItemToObject(countJson,"params",countParams);
            xaDataProxy.handleTrackCount(countJson);
        }
        void    XADataManager::trackMilestone(const char *milestoneName)
        {
            cJSON *milestoneJson=cJSON_CreateObject();
            char temp[64]={0};
            sprintf(temp,"%u",getTimestamp());
            cJSON_AddItemToObject(milestoneJson,"timestamp",cJSON_CreateString(temp));
            cJSON_AddItemToObject(milestoneJson,"eventName",cJSON_CreateString("milestone"));
            
            cJSON * milestoneParams=cJSON_CreateObject();
            cJSON_AddItemToObject(milestoneParams,"milestone_name",cJSON_CreateString(milestoneName));
            
            
            cJSON_AddItemToObject(milestoneJson,"params",milestoneParams);
            xaDataProxy.handleTrackMilestone(milestoneJson);
            
        }
        void    XADataManager::trackTransaction(int event,const char *orderId,const char *cost,const char *money,const char *category,const char *name)
        {
            cJSON *transJson=cJSON_CreateObject();
            cJSON_AddItemToObject(transJson,"eventName",cJSON_CreateString("pay.complete"));
            
            cJSON * transParams=cJSON_CreateObject();
            cJSON_AddItemToObject(transParams,"trans_id",cJSON_CreateString(orderId));
            cJSON_AddItemToObject(transParams,"gross",cJSON_CreateString(cost));
            //cJSON_AddItemToObject(transParams,"",cJSON_CreateString(tutorial));
            
            char temp[64]={0};
            sprintf(temp,"%u",getTimestamp());
            cJSON_AddItemToObject(transJson,"timestamp",cJSON_CreateString(temp));
            
            cJSON_AddItemToObject(transJson,"params",transParams);
            xaDataProxy.handleTrackTransaction(transJson);
        }
        void    XADataManager::trackTutorialService(const char *index,const char *name,const char *tutorial)
        {
            cJSON *tutorialJson=cJSON_CreateObject();
            char temp[64]={0};
            sprintf(temp,"%u",getTimestamp());
            cJSON_AddItemToObject(tutorialJson,"timestamp",cJSON_CreateString(temp));
            cJSON_AddItemToObject(tutorialJson,"eventName",cJSON_CreateString("tutorial"));
            
            cJSON * tutorialParams=cJSON_CreateObject();
            cJSON_AddItemToObject(tutorialParams,"index",cJSON_CreateString(index));
            cJSON_AddItemToObject(tutorialParams,"step_name",cJSON_CreateString(name));
            cJSON_AddItemToObject(tutorialParams,"tid",cJSON_CreateString(tutorial));
            
            cJSON_AddItemToObject(tutorialJson,"data",tutorialParams);
            xaDataProxy.handleTrackTutorialService(tutorialJson);
            

        }
        void    XADataManager::trackBuyService(const char *currency,const char *payType,const char *level1,const char *level2,const char *level3,const char *level4,const char *level5,int                             amount,int number)
        {
            cJSON *buyJson=cJSON_CreateObject();
            char temp[64]={0};
            sprintf(temp,"%u",getTimestamp());
            cJSON_AddItemToObject(buyJson,"timestamp",cJSON_CreateString(temp));
            cJSON_AddItemToObject(buyJson,"eventName",cJSON_CreateString("buy.item"));
            
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
            //number TODO
            cJSON_AddItemToObject(buyJson,"params",buyParams);
            xaDataProxy.handleTrackBuyService(buyJson);
            
            
        }
        void    XADataManager::generalEvent(int event,const char *appId,const char *userId,int timestamp,const char *params)
        {
            cJSON *generalRootJson=cJSON_CreateObject();
            cJSON *signedJson=cJSON_CreateObject();
            cJSON_AddItemToObject(signedJson,"appid",cJSON_CreateString(appId));
            cJSON_AddItemToObject(signedJson,"uid",cJSON_CreateString(userId));
            char temp[64]={0};
            sprintf(temp,"%u",getTimestamp());
            cJSON_AddItemToObject(signedJson,"timestamp",cJSON_CreateString(temp));
            
            cJSON *statsJson=cJSON_CreateArray();
            cJSON *statsObjectJson=cJSON_CreateObject();
            memset(temp,0,64);
            switch(event)
            {
                case 0:
                    strcpy(temp,"user.update");
                    break;
                case 1:
                    strcpy(temp,"user.increment");
                    break;
                case 2:
                    strcpy(temp,"user.visit");
                    break;
                case 3:
                    strcpy(temp,"user.heartbeat");
                    break;
                case 4:
                    strcpy(temp,"user.login");
                    break;
                case 5:
                    strcpy(temp,"user.error");
                    break;
                case 6:
                    strcpy(temp,"user.quit");
                    break;
                case 7:
                    strcpy(temp,"pay.complete");
                    break;
                case 8:
                    strcpy(temp,"buy.item");
                    break;
                case 9:
                    strcpy(temp,"count");
                    break;
                case 10:
                    strcpy(temp,"milestone");
                    break;
                case 11:
                    strcpy(temp,"tutorial");
                    break;
                case 12:
                    strcpy(temp,"page.view");//not support 
                default:
                    
                    break;
            }
            cJSON_AddItemToObject(statsObjectJson,"eventName",cJSON_CreateString(temp));
            
            cJSON *statsParams=cJSON_CreateObject();
            cJSON_AddItemToObject(statsParams,"params",cJSON_CreateString(params));
            cJSON_AddItemToObject(statsObjectJson,"params",statsParams);
            memset(temp,0,64);
            sprintf(temp,"%d",timestamp);
            cJSON_AddItemToObject(statsObjectJson,"timestamp",cJSON_CreateString(temp));
            
            cJSON_AddItemToArray(statsJson,statsObjectJson);
            
            cJSON_AddItemToObject(generalRootJson,"signedParams",signedJson);
            cJSON_AddItemToObject(generalRootJson,"stats",statsJson);
            
            xaDataProxy.handleGeneralEvent(generalRootJson);
        }

    }
}