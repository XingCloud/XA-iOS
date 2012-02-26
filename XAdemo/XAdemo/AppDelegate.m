//
//  AppDelegate.m
//  XADemo
//
//  Created by mini xingcloud on 12-2-20.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

@synthesize window = _window;

- (void)dealloc
{
    [_window release];
    [super dealloc];
}
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // Override point for customization after application launch.
    self.window.backgroundColor = [UIColor whiteColor];
        
    [self.window makeKeyAndVisible];
    
    [self.window addSubview:[[AppView alloc] initWithFrame:[self.window bounds]]]; 
    
    /**
     调用顺序
     1.设置delegate
     2.设置其他选项
     3.调用XA applicationDidLaunched
     */
    
    [XA setDelegate:self];
    
    [XA setLogEnabled:YES];
    [XA setHeartbeatEnabled:NO];
    [XA setReportPolicy:DEFAULT];//REALTIME,DEFAULT,BATCH 
    
    
    [XA applicationDidLaunched];

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{

}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    [XA applicationDidEnterBackground];
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    [XA applicationWillEnterForeground];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{

}
- (void)applicationWillTerminate:(UIApplication *)application
{
    [XA applicationWillTerminate];
}
/**
 实现delegate中的函数
 */
- (NSString *)appId
{
    return @"chucktest@337_ar_xatest";
}
- (NSString *)channelId
{
    return @"appStore";
}
@end
