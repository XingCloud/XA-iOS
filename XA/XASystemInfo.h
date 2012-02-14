//
//  XASystemInfo.h
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//



@interface XASystemInfo : NSObject
{
    char *systemInfo;
    int index;
}
+(void) getNetType;
+(void) getSimOperator;
+(void) getCUPInfo;
+(void) getPhoneType;
+(void) getNetOperator;
+(void) getDeviceModel;
+(void) getCountryISO;
+(void) iOSVersion;
+(void) getResolution;
+(void) initSystemInfo;
+(void) getXAtagname;
//update 时候发
@end
