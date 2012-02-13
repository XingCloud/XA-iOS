//
//  XADataManager.cpp
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include "XADataManager.h"
namespace XingCloud 
{
    namespace  XA
    {
        XADataManager::XADataManager()
        {
            
        }
        bool    XADataManager::sendDataBuffer(const char buffer)
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
        uint8_t* XADataManager::realloc()
        {
            return 0;
        }
    }
}