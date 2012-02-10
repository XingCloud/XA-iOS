//
//  XA.m
//  XA
//
//  Created by Zhang Chuck on 2/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "XA.h"
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
        __internal = [[NSMutableDictionary alloc] init];
    }
    return self;
}

- (void)dealloc
{
    [__internal release];
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
    [[XA sharedXA]->__internal setValue:[NSNumber numberWithBool:value] forKey:@"crashReportEnabled"];
}
+ (void)setLogEnabled:(BOOL)value
{
    [[XA sharedXA]->__internal setValue:[NSNumber numberWithBool:value] forKey:@"logEnabled"];
}
+ (void)setDelegate:(id <XADelegate>)delegate
{
    [[XA sharedXA]->__internal setValue:delegate forKey:@"delegate"];
}
+ (void)setReportPolicy:(XAReportPolicy)rp
{
    [[XA sharedXA]->__internal setValue:[NSNumber numberWithInt:rp] forKey:@"reportPolicy"];
}

#pragma mark XA life circle
+ (void)appDidLaunched
{
    
}
+ (void)appWillTerminate
{
    
}
+ (void)setHeartbeatEnabled:(BOOL)value
{
    
}

#pragma mark XA events
+ (void)event:(XAEventType)eventType appId:(NSString*)appId userId:(NSString*)uid timestamp:(NSInteger)timestamp params:(NSString*)params;
{
    
}

+ (void)trackEvent:(NSString*)function action:(NSString*)action level1:(NSString*)level1 level2:(NSString*)level2 level3:(NSString*)level3 level4:(NSString*)level4 level5:(NSString*)level5 count:(NSInteger)count
{
    
}
+ (void)trackTransaction
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
