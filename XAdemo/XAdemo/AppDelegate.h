//
//  AppDelegate.h
//  XADemo
//
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//

#import <UIKit/UIKit.h>
#import "XA.h"
#import "AppView.h"
/**
 1.使用XA库之前需要包含XA.h，实现XADelegate,并调用setDelegate。
 2.在每个life circle(生命周期)处都需要调用对应的XA life circle(生命周期)函数
 3.在didFinishLaunching中设置需要的发送策略，心跳包等optional（选项）
 4.XA的application需要在设置完各种optional（选项）之后调用
 */
@interface AppDelegate : UIResponder <UIApplicationDelegate,XADelegate>

@property (strong, nonatomic) UIWindow *window;

@end
