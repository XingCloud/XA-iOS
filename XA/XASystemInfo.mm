//
//  XASystemInfo.m
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//

#import "XASystemInfo.h"
#import "Reachability.h"
#import <UIKit/UIKit.h>
#import <CoreTelephony/CTCarrier.h>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#include <sys/utsname.h>
#include <string.h>

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
static char *my_strcpy(const char *jsonKey,const char *jsonValue)
{
    
    [XASystemInfo instance]->systemInfo[[XASystemInfo instance]->index++]='\"';
    while(*(jsonKey))
    {
        [XASystemInfo instance]->systemInfo[[XASystemInfo instance]->index++]=*(jsonKey++);
    }
    [XASystemInfo instance]->systemInfo[[XASystemInfo instance]->index++]='\"';
    
    [XASystemInfo instance]->systemInfo[[XASystemInfo instance]->index++]=':';
    
    [XASystemInfo instance]->systemInfo[[XASystemInfo instance]->index++]='\"';
    while(*(jsonValue))
    {
        [XASystemInfo instance]->systemInfo[[XASystemInfo instance]->index++]=*(jsonValue++);
    }
    [XASystemInfo instance]->systemInfo[[XASystemInfo instance]->index++]='\"';
    
    [XASystemInfo instance]->systemInfo[[XASystemInfo instance]->index++]=',';
    return 0;
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
        systemInfo = new char[1024];
        index=0;
    }
    return self;
}

- (void)dealloc
{
    delete systemInfo;
    [super dealloc];
}
+(void) initSystemInfo
{
    [XASystemInfo instance]->systemInfo[[XASystemInfo instance]->index++]='{';
    [XASystemInfo iOSVersion];
    [XASystemInfo getCountryISO];
    [XASystemInfo getNetOperator];
    [XASystemInfo getDeviceModel];
    [XASystemInfo getPhoneType];
    [XASystemInfo getCUPInfo];
    [XASystemInfo getResolution];
    ([XASystemInfo instance]->index)--;
    [XASystemInfo instance]->systemInfo[[XASystemInfo instance]->index++]='}';
    [XASystemInfo instance]->systemInfo[[XASystemInfo instance]->index]='\0';
}
+(void) getNetType
{
    if([[Reachability reachabilityForLocalWiFi] currentReachabilityStatus] != NotReachable)
    {
        my_strcpy("netType","WIFI");
    }
    else if([[Reachability reachabilityForInternetConnection] currentReachabilityStatus] != NotReachable)
    {
        my_strcpy("netType","3G/GPRS");
    }
    else 
    {
        my_strcpy("netType","noconnection");
    }
}
+(void) getSimOperator
{
    
}
+(void) getCUPInfo
{
    struct utsname cpuInfo;
    my_strcpy("cpuInfo",cpuInfo.machine);
}
+(void) getResolution
{
    CGRect bound=[[UIScreen mainScreen] bounds];
    int scale = [[UIScreen mainScreen] scale];
    int width=(int)bound.size.width * scale;
    int height=bound.size.height * scale;
    char temp[20]={0};
    sprintf(temp,"%d*%d",width,height);
    my_strcpy("resolution",temp);
}
+(void) getPhoneType
{
    CTTelephonyNetworkInfo *netInfo = [[CTTelephonyNetworkInfo alloc] init];
    CTCarrier *carrier = [netInfo subscriberCellularProvider];
    switch([[carrier mobileNetworkCode] intValue])
    {
        case 00:
        case 02:
        case 07:
            my_strcpy("phoneType","GSM");//china mobile
            break;
        case 01:
        case 06:    
            my_strcpy("phoneType","WCDMA");//China Unicom
            break;
        case 03:
        case 05:    
            my_strcpy("phoneType","CDMA");//China telecom
            break;
        default:
            break;
    }
}
+(void) getNetOperator
{
    CTTelephonyNetworkInfo *netInfo = [[CTTelephonyNetworkInfo alloc] init];
    CTCarrier *carrier = [netInfo subscriberCellularProvider];
    my_strcpy("netOperator", [[carrier 	carrierName] cStringUsingEncoding:NSUTF8StringEncoding]);
    [netInfo release];
}
+(void) getDeviceModel
{
    my_strcpy("deviceModel",[[[UIDevice currentDevice] model] cStringUsingEncoding:NSUTF8StringEncoding]);
}
+(void) getCountryISO
{
    CTTelephonyNetworkInfo *netInfo = [[CTTelephonyNetworkInfo alloc] init];
    CTCarrier *carrier = [netInfo subscriberCellularProvider];
    my_strcpy("countryIso", [[carrier isoCountryCode] cStringUsingEncoding:NSUTF8StringEncoding]);
    [netInfo release];
}
+(void) iOSVersion
{
    my_strcpy("osVersion",[[[UIDevice currentDevice] systemVersion] cStringUsingEncoding:NSUTF8StringEncoding]);
}
+(void) getXAtagname
{
    
}
@end
