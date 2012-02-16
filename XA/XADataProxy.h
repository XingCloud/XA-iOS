//
//  XALifeCicle.h
//  XA
//
//  Created by mini xingcloud on 12-2-16.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef XA_XALifeCicle_h
#define XA_XALifeCicle_h
#include "cJSON.h"
#include <vector>
namespace XingCloud
{
    namespace XA
    {
        class XADataProxy
        {
        public:
            XADataProxy();
            ~XADataProxy();
            void    handleApplicationLaunch(cJSON *visitEvent,cJSON *updateEvent,cJSON *errorEvent);
            void    handleApplicationTerminate(cJSON *quitEvent);
            void    handleApplicationPause();
            void    handleApplicationResume();
            
            void    handleTrackCount(cJSON *countEvent);
            void    handleTrackMilestone(cJSON *milestoneEvent);
            void    handleTrackTransaction(cJSON *transactionEvent);
            void    handleTrackTutorialService(cJSON *tutorialEvent);
            void    handleTrackBuyService(cJSON *buyEvent);       
            void    handleGeneralEvent(cJSON *generalEvent);
        private:
            std::vector<char*> eventCounts;
        };
    }
}


#endif
