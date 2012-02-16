//
//  XALifeCicle.cpp
//  XA
//
//  Created by mini xingcloud on 12-2-16.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#include "XADataProxy.h"
#include "XASystemInfo.h"
#include "XASendData.h"
namespace  XingCloud
{
    namespace XA
    {
        XADataProxy::XADataProxy()
        {
            
        }
        XADataProxy::~XADataProxy()
        {
            
        }
        void    XADataProxy::handleApplicationLaunch(cJSON *visitEvent,cJSON *updateEvent,cJSON *errorEvent)
        {
            
        }
        void    XADataProxy::handleApplicationTerminate(cJSON *quitEvent)
        {
            
        }
        void    XADataProxy::handleApplicationPause()
        {
            
        }
        void    XADataProxy::handleApplicationResume()
        {
            
        }
        
        void    XADataProxy::handleTrackCount(cJSON *countEvent)
        {
            
        }
        void    XADataProxy::handleTrackMilestone(cJSON *milestoneEvent)
        {
            
        }
        void    XADataProxy::handleTrackTransaction(cJSON *transactionEvent)
        {
            
        }
        void    XADataProxy::handleTrackTutorialService(cJSON *tutorialEvent)
        {
            
        }
        void    XADataProxy::handleTrackBuyService(cJSON *buyEvent)
        {
            
        }
        void    XADataProxy::handleGeneralEvent(cJSON *generalEvent)
        {
            const char *buffer = cJSON_PrintUnformatted(generalEvent);
        }
    }
}