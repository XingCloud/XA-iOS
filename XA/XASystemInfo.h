//
//  XASystemInfo.h
//  XA
//
//  Created by mini xingcloud on 12-2-13.
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//
//
namespace XingCloud
{
    namespace XA
    {
        class SystemInfo
        {
        public:
            static void getNetType(char *source);
            static void getSimOperator(char *source);
            static void getCPUInfo(char *source);
            static void getPhoneType(char *source);
            static void getNetOperator(char *source);
            static void getDeviceModel(char *source);
            static void getCountryISO(char *source);
            static void iOSVersion(char *source);
            static void getResolution(char *source);
            static void initSystemInfo(char *source);
            static void getXAtagname(char *source);
            static void getDeviceID(char *source);
            static bool isJailbroken(char *source);
            
            static char *getSystemInfo(char *appID,int timesamp);
            
            static int phoneType;
        };
    }
}