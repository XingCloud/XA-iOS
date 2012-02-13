//
//  XASystemInfo.h
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//



@interface XASystemInfo : NSObject
{
    char *systemInfo;
}
+(void) getNetType;
+(void) getSimOperator;
+(void) getCUPInfo;
+(void) getPhoneType;
+(void) getNetOperator;
+(void) getDeviceModel;
+(void) getCountryISO;
+(void) iOSVersion;
//update 时候发
@end
