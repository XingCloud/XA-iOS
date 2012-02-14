//
//  XADataManager.cpp
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//

#include "XADataManager.h"
#include "curl.h"
namespace XingCloud 
{
    namespace  XA
    {
        XADataManager::XADataManager()
        {
            channelID=NULL;
            appID=NULL;
        }
        bool    XADataManager::sendDataBuffer(const char *buffer)
        {
            
            return true;
        }
        void    XADataManager::sendCacheBuffer()
        {
            
        }
        void    XADataManager::sendErrorBuffer()
        {
            
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
        void    XADataManager::applicationResume()
        {
            
        }
        uint8_t* XADataManager::realloc()
        {
            return 0;
        }
    }
}