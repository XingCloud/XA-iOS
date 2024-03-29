//
//  XASystemInfo.m
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//

#import "XASystemInfo.h"
#import "XAReachability.h"

#import <UIKit/UIKit.h>
#import <CoreTelephony/CTCarrier.h>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#include <sys/utsname.h>
#include <string.h>
#include "XADataManager.h"
int XingCloud::XA::SystemInfo::phoneType=0;

void    XAPRINT(const char *fmt,...)
{
    if(XingCloud::XA::XADataManager::logEnable==false)
    {
        return ;
    }
    if(strlen(fmt)>2048*4-1)
    {
        return ;
    }
    char sprint_buf[2048*4];
    
    va_list args;
    
    va_start(args, fmt);
    
    vsprintf(sprint_buf, fmt, args);
    
    va_end(args);
    //#ifdef IOS
    NSLog(@"%s\n\n",sprint_buf);
    //#endif
    
}
static void setHeartbeatTimer();
cJSON* XingCloud::XA::SystemInfo::getSystemInfo()
{
    char temp[64]={0};
    
//    cJSON * statsObject=cJSON_CreateObject();
//    cJSON_AddItemToObject(statsObject,"eventName",cJSON_CreateString("user.update"));
    
    cJSON * statsParams=cJSON_CreateObject();
   
    cJSON_AddItemToObject(statsParams,"is_mobile",cJSON_CreateString("true"));
    memset(temp,0,64);
    getNetType(temp);
    bool _isJailbroken = isJailbroken();
    if(_isJailbroken)
    {
        cJSON_AddItemToObject(statsParams,"isJailbroken",cJSON_CreateString("true"));
    }
    else
    {
        cJSON_AddItemToObject(statsParams,"isJailbroken",cJSON_CreateString("false"));
    }
    cJSON_AddItemToObject(statsParams,"netType",cJSON_CreateString(temp));
    
    memset(temp,0,64);
    getCPUInfo(temp);
    cJSON_AddItemToObject(statsParams,"cpuInfo",cJSON_CreateString(temp));
    
    memset(temp,0,64);
    getResolution(temp);
    cJSON_AddItemToObject(statsParams,"resolution",cJSON_CreateString(temp));
    
    memset(temp,0,64);
    getPhoneType(temp);
    cJSON_AddItemToObject(statsParams,"phoneType",cJSON_CreateString(temp));
    
    memset(temp,0,64);
    getNetOperator(temp);
    cJSON_AddItemToObject(statsParams,"netOperator",cJSON_CreateString(temp));
    
    memset(temp,0,64);
    getDeviceModel(temp);
    cJSON_AddItemToObject(statsParams,"deviceModel",cJSON_CreateString(temp));
    
    memset(temp,0,64);
    getCountryISO(temp);
    cJSON_AddItemToObject(statsParams,"countryIso",cJSON_CreateString(temp));
    
    memset(temp,0,64);
    iOSVersion(temp);
    cJSON_AddItemToObject(statsParams,"osVersion",cJSON_CreateString(temp));
    
    memset(temp,0,64);
    getXAtagname(temp);
    cJSON_AddItemToObject(statsParams,"XA_tagname",cJSON_CreateString(temp));
    
    
//    cJSON_AddItemToObject(statsObject,"params",statsParams);
//    memset(temp,0,64);
//    sprintf(temp,"%u",timestamp);
//    cJSON_AddItemToObject(statsObject,"timestamp",cJSON_CreateString(temp));//to confirm
    return statsParams;
}
void XingCloud::XA::SystemInfo::getAppFileDir(char* result)
{//document
//    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//    NSString *documentsDirectory = [paths objectAtIndex:0];
//    if(documentsDirectory!=nil)
//    {
//        const char* destDir = [documentsDirectory UTF8String];
//        strcpy(result, destDir);
//    }
    NSString *tempPath = NSTemporaryDirectory();
    if(tempPath!=nil)
    {
        const char* destDir = [tempPath UTF8String];
        strcpy(result, destDir);
    }
}
bool XingCloud::XA::SystemInfo::isJailbroken() 
{
    BOOL jailbroken = NO;
    NSString *cydiaPath = @"/Applications/Cydia.app";
    NSString *aptPath = @"/private/var/lib/apt/";
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:cydiaPath]) {
        jailbroken = YES;
    }
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:aptPath]) {
        jailbroken = YES;
    }
    return jailbroken;
}
int XingCloud::XA::SystemInfo::getNetType(char *source)
{
    if([[Reachability reachabilityForLocalWiFi] currentReachabilityStatus] != NotReachable)
    {
        if(source!=NULL)
            strcpy(source,"WIFI");
        return 1;
    }
    else if([[Reachability reachabilityForInternetConnection] currentReachabilityStatus] != NotReachable)
    {
        switch(phoneType)
        {
            case 00:
            case 02:
            case 07:
                if(source!=NULL)
                    strcpy(source,"GPRS");
                break;
            default:
                if(source!=NULL)
                    strcpy(source,"3G");
                break;
        }
        return 2;
    }
    else 
    {
        if(source!=NULL)
            strcpy(source,"noconnection");
        return 0;
    }
}
void XingCloud::XA::SystemInfo::getSimOperator(char *source)
{
    
}
void XingCloud::XA::SystemInfo::getCPUInfo(char *source)
{
    struct utsname cpuInfo;
    strcpy(source,cpuInfo.machine);
}
void XingCloud::XA::SystemInfo::getResolution(char *source)
{
    CGRect bound=[[UIScreen mainScreen] bounds];
    int scale = [[UIScreen mainScreen] scale];
    int width=(int)bound.size.width * scale;
    int height=bound.size.height * scale;
    char temp[20]={0};
    sprintf(temp,"%d*%d",width,height);
    strcpy(source,temp);
    //my_strcpy("resolution",temp);
}
void XingCloud::XA::SystemInfo::getPhoneType(char *source)
{
    CTTelephonyNetworkInfo *netInfo = [[CTTelephonyNetworkInfo alloc] init];
    CTCarrier *carrier = [netInfo subscriberCellularProvider];
    phoneType=[[carrier mobileNetworkCode] intValue];
    switch(phoneType)
    {
        case 00:
        case 02:
        case 07:
            //my_strcpy("phoneType","GSM");//china mobile
            strcpy(source,"GSM");
            break;
        case 01:
        case 06:
            strcpy(source,"WCDMA");
            //my_strcpy("phoneType","WCDMA");//China Unicom
            break;
        case 03:
        case 05: 
            strcpy(source,"CDMA");
            //my_strcpy("phoneType","CDMA");//China telecom
            break;
        default:
            break;
    }
    [netInfo release];
}
void XingCloud::XA::SystemInfo::getNetOperator(char *source)
{
    CTTelephonyNetworkInfo *netInfo = [[CTTelephonyNetworkInfo alloc] init];
    if(netInfo == nil)
        return ;
    CTCarrier *carrier = [netInfo subscriberCellularProvider];
    strcpy(source,[[carrier carrierName] cStringUsingEncoding:NSUTF8StringEncoding]);
  
    [netInfo release];
}
void XingCloud::XA::SystemInfo::getDeviceID(char *source)
{
    //UUID/UDID
    CFUUIDRef puuid = CFUUIDCreate( nil );  
    CFStringRef uuidString = CFUUIDCreateString( nil, puuid );  
    NSString * result = (NSString *)CFStringCreateCopy( NULL, uuidString);  
    CFRelease(puuid);  
    CFRelease(uuidString); 
    strcpy(source,[result cStringUsingEncoding:NSUTF8StringEncoding]);
    [result release];
}
void XingCloud::XA::SystemInfo::getDeviceModel(char *source)
{
    strcpy(source,[[[UIDevice currentDevice] model] cStringUsingEncoding:NSUTF8StringEncoding]);
   
}
void XingCloud::XA::SystemInfo::getCountryISO(char *source)
{
    CTTelephonyNetworkInfo *netInfo = [[CTTelephonyNetworkInfo alloc] init];
    if(netInfo == nil)
        return ;
    CTCarrier *carrier = [netInfo subscriberCellularProvider];
    strcpy(source,[[carrier isoCountryCode] cStringUsingEncoding:NSUTF8StringEncoding]);
    [netInfo release];
}
void XingCloud::XA::SystemInfo::iOSVersion(char *source)
{
    strcpy(source,[[[UIDevice currentDevice] systemVersion] cStringUsingEncoding:NSUTF8StringEncoding]);
}
void XingCloud::XA::SystemInfo::getXAtagname(char *source)
{
    //NSString *plistPath = [[NSBundle mainBundle] pathForResource:@"Data" ofType:@"plist"];
    NSDictionary* infoDict = [[NSBundle mainBundle] infoDictionary];
    NSString* version = [infoDict objectForKey:@"CFBundleVersion"];//版本号
    strcpy(source,[version cStringUsingEncoding:NSUTF8StringEncoding]);
}
