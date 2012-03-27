//
//  XASystemInfo.h
//  XA
//
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//
//
#include "XAJSON.h"
extern void    XAPRINT(const char *fmt,...);

namespace XingCloud
{
    namespace XA
    {
        class SystemInfo
        {
        public:
            static int getNetType(char *source);
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
            static bool isJailbroken();
            static void getAppFileDir(char* result);
            static cJSON *getSystemInfo();
            
            static int phoneType;
        };
    }
}
