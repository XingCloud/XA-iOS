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
    //[__internal release];
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
    
}

+ (void)trackEvent:(NSString*)function action:(NSString*)action level1:(NSString*)level1 level2:(NSString*)level2 level3:(NSString*)level3 level4:(NSString*)level4 level5:(NSString*)level5 count:(NSInteger)count
{
    
}
+ (void)trackTransaction:(int)function :(NSString*)values
{
    
}
+ (void)trackPageview
{
    
}
+ (void)trackTutorialService
{
    
}
+ (void)trackBuyService
{
    
}

@end
