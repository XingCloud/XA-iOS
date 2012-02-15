//
//  XADataManager.cpp
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//

#include "XADataManager.h"
//#include "XASystemInfo.h"
#include "cJSON.h"
namespace XingCloud 
{
    namespace  XA
    {
        XADataManager::XADataManager()
        {
            channelID=NULL;
            appID=NULL;
        }
        XADataManager::~XADataManager()
        {
            if(channelID!=NULL)
                delete channelID;
            if(appID!=NULL)
                delete appID;
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
            //发送系统信息，user.update,user.view,user.error事件
            
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
            
        }
        void    XADataManager::trackMilestone(const char *milestoneName)
        {
            
        }
        void    XADataManager::trackTransaction(int event,const char *orderId,const char *cost,const char*money,const char *category,const char *name)
        {
            
        }
        void    XADataManager::trackTutorialService(const char *index,const char *name,const char *tutorial)
        {
            
        }
        void    XADataManager::trackBuyService(const char *currency,const char *payType,const char *level1,const char *level2,const char *level3,const char *level4,const char *level5,int                             amount)
        {
            
        }
        void    XADataManager::generalEvent(int event,const char *appId,const char *userId,int timestamp,const char *params)
        {
            
        }
//        uint8_t* XADataManager::realloc()
//        {
//            return 0;
//        }
    }
}