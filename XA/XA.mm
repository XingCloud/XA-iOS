//
//  XA.m
//  XA
//
//  Created by Zhang Chuck on 2/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "XA.h"
#include "XADataManager.h"
@implementation XA

#pragma mark internal
static XA* sharedXA;
static NSString* VERSION = @"1.0";
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
        __internal = new XingCloud::XA::XADataManager();
    }
    return self;
}

- (void)dealloc
{
    delete (XingCloud::XA::XADataManager*)__internal;
    [super dealloc];
}
#pragma mark XA basics
+ (NSString *)getVersion
{
    return VERSION;
}
+ (NSString *)getUserId
{
    return @"";
}

+ (void)setCrashReportEnabled:(BOOL)value
{
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->setCrashReportEnabled(value);
}
+ (void)setLogEnabled:(BOOL)value
{
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->setLogEnabled(value);
}
+ (void)setDelegate:(id <XADelegate>)delegate
{
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->setAppID([[delegate appId] cStringUsingEncoding:NSUTF8StringEncoding]);
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->setChannelID([[delegate channelId] cStringUsingEncoding:NSUTF8StringEncoding]);
}
+ (void)setReportPolicy:(XAReportPolicy)rp
{
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->setReportPolicy(rp);
}

#pragma mark XA life circle
+ (void)applicationDidLaunched
{
    //发送update,view,error事件
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->applicationLaunch();
}
+ (void)applicationWillTerminate
{
    //发送quit事件
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->applicationTerminate();
}
+ (void)setHeartbeatEnabled:(BOOL)value
{
    //定时发送事件
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->setHeartbeatEnabled(value);
}
+ (void)applicationDidBecomeActive
{
    //重启定时器
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->applicationResume();
}
+ (void)applicationWillResignActive
{
    //暂停定时器
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->applicationPause();
}
#pragma mark XA events
+ (void)event:(XAEventType)eventType appId:(NSString*)appId userId:(NSString*)uid timestamp:(NSInteger)timestamp params:(NSString*)params;
{
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->generalEvent(eventType, 
                                                                               [appId cStringUsingEncoding:NSUTF8StringEncoding],
                                                                               [uid cStringUsingEncoding:NSUTF8StringEncoding],timestamp,
                                                                               [params cStringUsingEncoding:NSUTF8StringEncoding] );
}
+ (void)trackCount:(NSString*)action level1:(NSString*)level1 level2:(NSString*)level2 level3:(NSString*)level3 level4:(NSString*)level4 level5:(NSString*)level5 count:(NSInteger)count
{
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->trackCount([action cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                             [level1 cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                             [level2 cStringUsingEncoding:NSUTF8StringEncoding],
                                                                             [level3 cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                             [level4 cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                             [level5 cStringUsingEncoding:NSUTF8StringEncoding], count);
}
+ (void)trackPayService:(NSString*)trans_id channel:(NSString*)channel gross:(NSString*)gross gcurrency:(NSString*)gcurrency vamount:(NSString*)vamount vcurrentcy:(NSString*)vcurrentcy
{
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->trackTransaction([trans_id cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                   [channel cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                                   [gross cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                                   [gcurrency cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                   [vamount cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                   [vcurrentcy cStringUsingEncoding:NSUTF8StringEncoding]);
}
+ (void)trackMilestone:(NSString*)milestoneName
{
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->trackMilestone(  [milestoneName cStringUsingEncoding:NSUTF8StringEncoding]);
}
+ (void)trackPageview
{
    //((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))
}
+ (void)trackTutorialService:(NSString*)index name:(NSString*)name tutorial:(NSString*)tutorial
{
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->trackTutorialService([index cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                       [name cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                       [tutorial cStringUsingEncoding:NSUTF8StringEncoding]);
}
//@param event       XAEventType::BUY_ITEM
+ (void)trackBuyService:(NSString*)currency payType:(NSString*)payType level1:(NSString*)level1 level2:(NSString*)level2 level3:(NSString*)level3 level4:(NSString*)level4 level5:(NSString*)level5 amount:(int)amount number:(int)number
{
    ((XingCloud::XA::XADataManager*)([XA sharedXA]->__internal))->trackBuyService([currency cStringUsingEncoding:NSUTF8StringEncoding], 
                                                                                  [payType cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                  [level1 cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                  [level2 cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                  [level3 cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                  [level4 cStringUsingEncoding:NSUTF8StringEncoding],
                                                                                  [level5 cStringUsingEncoding:NSUTF8StringEncoding],amount,number);
}

@end
