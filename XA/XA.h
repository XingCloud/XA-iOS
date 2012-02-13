//
//  XA.h
//  XA
//
//  Copyright (c) 2012 XingCloud.com All rights reserved.
//

#import <Foundation/Foundation.h>

typedef enum {
    REALTIME = 0,   //实时发送
    BATCH = 1,      //启动时批量发送
    DEFAULT = 3     //定时定量发送
} XAReportPolicy;

typedef enum {
    USER_UPDATE = 0,
    USER_INCREMENT,
    USER_VISIT,
    USER_HEARTBEAT,
    USER_LOGIN,
    USER_ERROR,
    USER_QUIT,
    PAY_COMPLETE,
    BUY_ITEM,
    COUNT,
    MILESTONE,
    TUTORIAL,
    PAGE_VIEW
} XAEventType;

@protocol XADelegate;

@interface XA : NSObject {
    void* __internal;
}

#pragma mark XA basics
/**
 获取SDK版本号
 @return 版本号字符串
 */
+ (NSString *)getVersion;

/**
 获取用户Id
 @return 用户Id字符串
 */
+ (NSString *)getUserId;

/**
 设置是否让行云数据分析SDK捕捉和记录您的crash log
 @param value 默认值为YES。设置为NO，行云数据分析SDK将不再捕捉和记录您的crash log,需要在setDelegate方法之前调用
 @return void
 */
+ (void)setCrashReportEnabled:(BOOL)value;

/**
 方法设置是否打印sdk的log信息
 @param value 默认值为NO。设置为YES，行云数据分析SDK将会输出数据统计请求
 */
+ (void)setLogEnabled:(BOOL)value;

/**
 设置XA代理
 @param delegate 实现了XADelegate协议的实例
 @return void
 */
+ (void)setDelegate:(id <XADelegate>)delegate;

/**
 设置统计数据发送策略
 @param XAReportPolicy 发送统计信息的策略设置，可选的发送策略包括
 1.BATCH		:批量发送。每次发送的时机在App开启的时候进行发送
 2.REALTIME	:实时发送。每当有事件产生时，进行发送
 3.DEFAULT   :定时定量发送。默认间隔时间是1分钟，一次最大数量上10条
 @return void
 */
+ (void)setReportPolicy:(XAReportPolicy)rp;

#pragma mark XA life circle
/**
 请在app启动后调用该API
 */
+ (void)applicationDidLaunched;
/**
 请在app退出时调用该API
 */
+ (void)applicationWillTerminate;
/**
  请在app唤醒时调用该API
 */
+ (void)applicationDidBecomeActive;
/**
  请在app切换时调用该API
 */
+ (void)applicationDidEnterBackground;
/**
 设置是否持续向行云数据分析平台发送心跳事件。该事件有助于获取PCU（峰值在线用户）数据
 @param value 是否开启
 */
+ (void)setHeartbeatEnabled:(BOOL)value;

#pragma mark XA events
+ (void)trackEvent:(NSString*)function action:(NSString*)action level1:(NSString*)level1 level2:(NSString*)level2 level3:(NSString*)level3 level4:(NSString*)level4 level5:(NSString*)level5 count:(NSInteger)count;

+ (void)trackTransaction;//TODO add api params&comment
+ (void)trackPageview;//TODO add api params&comment
+ (void)trackTutorialService;//TODO add api params&comment
+ (void)trackBuyService;//TODO add api params&comment

/**
 通用的事件发送API
 @param eventType 事件类型
 @param appId app在行云平台上的唯一标识
 @param uid 用户uid
 @param timestamp 当前时间戳
 @param params 事件参数，数据格式为json字符串
 @return void
 */
+ (void)event:(XAEventType)eventType appId:(NSString*)appId userId:(NSString*)uid timestamp:(NSInteger)timestamp params:(NSString*)params;

@end


@protocol XADelegate <NSObject>
@required
/**
 @return 返回AppId，如果AppId错误，统计后台不会对log进行记录
 */
- (NSString *)appId;
@optional
/**
 @return 返回渠道名，例如@"AppStore"
 */
- (NSString *)channelId;
@end