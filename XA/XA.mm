//
//  XA.m
//  XA
//
//  Created by Zhang Chuck on 2/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "XA.h"
#include "XADataManager.h"
@interface XA ()
@property (nonatomic, readwrite, assign) id internal;
@end

@implementation XA
@synthesize internal=__internal;

struct XAWrapOpaque
{
public:
	XAWrapOpaque(){wrap=new XingCloud::XA::XADataManager();};
    ~XAWrapOpaque(){delete wrap;wrap=NULL;};
	XingCloud::XA::XADataManager* wrap;
};

#pragma mark internal
static XA* sharedXA;
static NSString* VERSION = @"1.0";
static NSTimer *  heartbeatTimer;
static NSTimer*   eventTimer;
+ (XA*)sharedXA
{
    @synchronized(self)
    {
        if (sharedXA == nil)
        {
            [[self alloc] init];
        }
    }
    
    return sharedXA;
}

+ (id)allocWithZone:(NSZone *)zone
{
    @synchronized(self)
    {
        if(sharedXA == nil)
        {
            sharedXA = [super allocWithZone:zone];
            return sharedXA;
        }
    }
    
    return nil;
}

- (id)init
{
    self = [super init];
    if (self) {
        self.internal = (id)(new XAWrapOpaque());
    }
    return self;
}

- (void)dealloc
{
    delete (XAWrapOpaque*)__internal;
    __internal=NULL;
    [super dealloc];
}
#pragma mark XA Timer
+ (void)handleEventTimer
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->handleEventTimer();
}
+ (void)handleHeartbeatTimer
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->handleHeartbeatTimer();
}
#pragma mark XA basics
+ (NSString *)getVersion
{
    return VERSION;
}
+ (NSString *)getUserId
{
    return  [NSString stringWithFormat:@"%s",XingCloud::XA::XADataProxy::uid];
}

+ (void)setCrashReportEnabled:(BOOL)value
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->setCrashReportEnabled(value);
}
+ (void)setAdsChannel:(NSString*)adsChannel
{
   ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->setAdsChannel([adsChannel cStringUsingEncoding:NSUTF8StringEncoding]);
}
+ (void)setLogEnabled:(BOOL)value
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->setLogEnabled(value);
}
+ (void)setDelegate:(id <XADelegate>)delegate
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->setAppID([[delegate appId] cStringUsingEncoding:NSUTF8StringEncoding]);
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->setChannelID([[delegate channelId] cStringUsingEncoding:NSUTF8StringEncoding]);
}
+ (void)setReportPolicy:(XAReportPolicy)rp
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->setReportPolicy(rp);
    
}

#pragma mark XA life circle
+ (void)applicationDidLaunched
{
    //发送update,view,error事件
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->applicationLaunch();
    if(((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->reportPolice!=REALTIME)
    {
       
            eventTimer = [NSTimer scheduledTimerWithTimeInterval: 20  
                                                          target: self  
                                                        selector: @selector(handleEventTimer)  
                                                        userInfo: nil  
                                                         repeats: YES];
    }
    if(((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->heartbeat)
    {
        heartbeatTimer = [NSTimer scheduledTimerWithTimeInterval: 60*5  
                                                          target: self 
                                                        selector: @selector(handleHeartbeatTimer)  
                                                        userInfo: nil  
                                                         repeats: YES];
    }
    //always open ,also for cache event
//    heartbeatTimer = [NSTimer scheduledTimerWithTimeInterval: 45  
//                                                      target: self 
//                                                    selector: @selector(handleHeartbeatTimer)  
//                                                    userInfo: nil  
//                                                     repeats: YES];
    
}
+ (void)trackLogin:(NSString*)login
{
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->trackLogin([login cStringUsingEncoding:NSUTF8StringEncoding]);
}
+ (void)trackUserUpdate:(NSMutableDictionary*)userInfo
{
    NSEnumerator *enumerator = [userInfo keyEnumerator];
    
    NSString* key;
    cJSON * userUpdateParams=cJSON_CreateObject();
    cJSON_AddItemToObject(userUpdateParams,"is_mobile",cJSON_CreateString("true"));
    while ((key = [enumerator nextObject])) 
    {
        const char *tempKey =  [key cStringUsingEncoding:NSUTF8StringEncoding];
        const char *tempValue = [[userInfo  objectForKey:key] cStringUsingEncoding:NSUTF8StringEncoding];
        if(tempKey !=NULL && tempValue !=NULL)
        {
            cJSON_AddItemToObject(userUpdateParams,tempKey,cJSON_CreateString(tempValue));
        }
    }
     ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->trackUserUpdate(userUpdateParams);
}
+ (void)trackUserIncrement:(NSMutableDictionary*)userInfo;
{
    NSEnumerator *enumerator = [userInfo keyEnumerator];
    
    NSString* key;
    cJSON * userIncParams=cJSON_CreateObject();
    cJSON_AddItemToObject(userIncParams,"is_mobile",cJSON_CreateString("true"));
    while ((key = [enumerator nextObject])) 
    {
        const char *tempKey =  [key cStringUsingEncoding:NSUTF8StringEncoding];
        const char *tempValue = [[userInfo  objectForKey:key] cStringUsingEncoding:NSUTF8StringEncoding];
        if(tempKey !=NULL && tempValue !=NULL)
        {
            cJSON_AddItemToObject(userIncParams,tempKey,cJSON_CreateString(tempValue));
        }
    }
    
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->trackUserIncrement(userIncParams);
}
+ (void)applicationWillTerminate
{
    //发送quit事件
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->applicationTerminate();
}
+ (void)setHeartbeatEnabled:(BOOL)value
{
    //定时发送事件
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->setHeartbeatEnabled(value);
}
+ (void)applicationWillEnterForeground
{
    //重启定时器
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->applicationResume();
}
+ (void)applicationDidEnterBackground
{
    //暂停定时器
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->applicationPause();
}
#pragma mark XA events
+ (void)event:(XAEventType)eventType appId:(NSString*)appId userId:(NSString*)uid timestamp:(NSInteger)timestamp params:(NSString*)params;
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->generalEvent(eventType, 
                                                                               [appId cStringUsingEncoding:NSUTF8StringEncoding],
                                                                               [uid cStringUsingEncoding:NSUTF8StringEncoding],timestamp,
                                                                               [params cStringUsingEncoding:NSUTF8StringEncoding] );
}
+ (void)trackCount:(NSString*)action level1:(NSString*)level1 level2:(NSString*)level2 level3:(NSString*)level3 level4:(NSString*)level4 level5:(NSString*)level5 count:(NSInteger)count
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->trackCount([action cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                             [level1 cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                             [level2 cStringUsingEncoding:NSUTF8StringEncoding],
                                                                             [level3 cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                             [level4 cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                             [level5 cStringUsingEncoding:NSUTF8StringEncoding], count);
}
+ (void)trackPayService:(NSString*)trans_id channel:(NSString*)channel gross:(NSString*)gross gcurrency:(NSString*)gcurrency vamount:(NSString*)vamount vcurrentcy:(NSString*)vcurrentcy
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->trackTransaction([trans_id cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                   [channel cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                                   [gross cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                                   [gcurrency cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                   [vamount cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                   [vcurrentcy cStringUsingEncoding:NSUTF8StringEncoding]);
}
+ (void)trackMilestone:(NSString*)milestoneName
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->trackMilestone(  [milestoneName cStringUsingEncoding:NSUTF8StringEncoding]);
}
+ (void)trackPageview
{
    //((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap
}
+ (void)trackTutorialService:(NSString*)index name:(NSString*)name tutorial:(NSString*)tutorial
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->trackTutorialService([index cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                       [name cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                       [tutorial cStringUsingEncoding:NSUTF8StringEncoding]);
}
//@param event       XAEventType::BUY_ITEM
+ (void)trackBuyService:(NSString*)currency payType:(NSString*)payType level1:(NSString*)level1 level2:(NSString*)level2 level3:(NSString*)level3 level4:(NSString*)level4 level5:(NSString*)level5 amount:(int)amount number:(int)number
{
    ((XAWrapOpaque*)([XA sharedXA]->__internal))->wrap->trackBuyService([currency cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                                  [payType cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                  [level1 cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                  [level2 cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                  [level3 cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                  [level4 cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                  [level5 cStringUsingEncoding:NSUTF8StringEncoding],amount,number);
}

@end
