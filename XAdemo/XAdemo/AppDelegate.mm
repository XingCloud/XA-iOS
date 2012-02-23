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
    
    [XA setDelegate:self];
    [XA setLogEnabled:YES];
    [XA setHeartbeatEnabled:YES];
    [XA setReportPolicy:REALTIME];//REALTIME,DEFAULT,BATCH 
    [XA applicationDidLaunched];
//    NSMutableDictionary *tris = [NSMutableDictionary dictionaryWithObjectsAndKeys:@"value1",@"key1",@"value2",@"key2",@"value3",nil];//要指定一个nil
//    [XA trackUserUpdate:tris];
//    [XA trackUserIncrement:@"maoxianInc"];
    
    
    [XA trackMilestone:@"MILE STONE"];
//    [XA trackCount:@"abc" level1:@"level1" level2:@"level2" level3: @"level3" level4:@"level4" level5:@"level5" count: 10];
//    [XA trackBuyService:@"currency" payType:@"PAYTYPE" level1:@"level1" level2:@"level2" level3: @"level3" level4:@"level4" level5:@"level5" amount:11 number:12];
//    [XA trackPayService:@"transID" channel:@"appStore" gross:@"100" gcurrency:@"USD" vamount:@"10" vcurrentcy:@"ni"];
//    [XA trackTutorialService:@"3"  name:@"go" tutorial:@"to"];
//    for(int i=0;i<2;i++)
//    {
//        NSString *userid=[NSString  stringWithFormat:@"gaomao=%d",i];
//        [XA event:USER_LOGIN appId:@"chucktest@337_ar_xatest" userId:userid timestamp:222222222 params:@"null"];
//    }
    
    
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    [XA applicationDidEnterBackground];
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
     If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
     */
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    [XA applicationWillEnterForeground];
    /*
     Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
     */
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [XA applicationWillTerminate];
    /*
     Called when the application is about to terminate.
     Save data if appropriate.
     See also applicationDidEnterBackground:.
     */
}
- (NSString *)appId
{
    return @"chucktest@337_ar_xatest";
}
- (NSString *)channelId
{
    return @"appStore";
}
@end
