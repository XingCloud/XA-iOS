//
//  XASystemInfo.m
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "XASystemInfo.h"
#import <UIKit/UIKit.h>
#include <sys/utsname.h>
#include <string.h>
static char *my_strcpy(char *jsonKey,const char *jsonValue)
{
    strcpy([XASystemInfo instance]->systemInfo,jsonKey);
    ([XASystemInfo instance]->systemInfo) --;
    strcpy([XASystemInfo instance]->systemInfo,jsonValue);
    return 0;
}
@implementation XASystemInfo
static XASystemInfo *instance=nil;

+ (XASystemInfo*)instance
{
    @synchronized(self)
    {
        if (instance == nil)
        {
            [[self alloc] init];
        }
    }
    
    return instance;
}
+ (id)allocWithZone:(NSZone *)zone
{
    @synchronized(self)
    {
        if(instance == nil)
        {
            instance = [super allocWithZone:zone];
            return instance;
        }
    }
    
    return nil;
}

- (id)init
{
    self = [super init];
    if (self) {
        systemInfo = new char[512];
    }
    return self;
}

- (void)dealloc
{
    delete systemInfo;
    [super dealloc];
}

+(void) getNetType
{
    
}
+(void) getSimOperator
{
    
}
+(void) getCUPInfo
{
    struct utsname cpuinfo;
    cpuinfo.machine;
}
+(void) getPhoneType
{
    
}
+(void) getNetOperator
{
    
}
+(void) getDeviceModel
{
    
}
+(void) getCountryISO
{
    
}
+(void) iOSVersion
{
    [[[UIDevice currentDevice] systemVersion] cStringUsingEncoding:NSUTF8StringEncoding];
}
@end
