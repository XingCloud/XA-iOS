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
    COUNT,//累计事件数量
    MILESTONE,//定义一个目标,目标名
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
+ (void)applicationWillResignActive;
/**
 设置是否持续向行云数据分析平台发送心跳事件。该事件有助于获取PCU（峰值在线用户）数据
 @param value 是否开启
 */
+ (void)setHeartbeatEnabled:(BOOL)value;

#pragma mark XA events
/**
 统计游戏开发者需要定制的一些游戏中的计数点。
 @param action  自定义的游戏中的action名称，如buy,sell等
 @param level1  分类1
 @param level2  分类2
 @param level3  分类3
 @param level4  分类4
 @param level5  分类5
 @param count   影响的数值
 */
+ (void)trackCount:(NSString*)action level1:(NSString*)level1 level2:(NSString*)level2 level3:(NSString*)level3 level4:(NSString*)level4 level5:(NSString*)level5 count:(NSInteger)count;
/**
 记录在某段时间注册的用户，完成指定任务数量的百分比
 @param milestoneName 
 */
+ (void)trackMilestone:(NSString*)milestoneName;
/**
 游戏中的用户付费情况
 @param trans_id     交易号
 @param channel      第三方支付渠道的名称，例如paypal,google
 @param gross        支付金额
 @param gcurrency    货币类型，如USD
 @param vamount      游戏币的数量
 @param vcurrentcy   游戏币的单位
 */
+ (void)trackPayService:(NSString*)trans_id channel:(NSString*)channel gross:(NSString*)gross gcurrency:(NSString*)gcurrency vamount:(NSString*)vamount vcurrentcy:(NSString*)vcurrentcy;

+ (void)trackPageview;//TODO add api params&comment
/**
 记录新手引导步骤中，每步完成的数量，从而查看新手引导过程中的用户流失率
 @param event       事件的名称，XAEventType::TUTORIAL
 @parma index       教程步骤
 @param name        教程名称
 @param tutorial    值固定为tutorial
 */
+ (void)trackTutorialService:(NSString*)index name:(NSString*)name tutorial:(NSString*)tutorial;
/**
 记录某类币种结合用户所定义资源结构的收入和支出情况。分别统计出人数，次数和总和。
 @param currency    币种
 @param payType     收入或支出（只能取值为income/payout)
 @param level1      属性1
 @param level2      属性2
 @param level3      属性3
 @param level4      属性4
 @param level5      属性5
 @param count       交易数量
 */
+ (void)trackBuyService:(NSString*)currency payType:(NSString*)payType level1:(NSString*)level1 level2:(NSString*)level2 level3:(NSString*)level3 level4:(NSString*)level4 level5:(NSString*)level5 amount:(int)amount number:(int)number;
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