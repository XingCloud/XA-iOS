//
//  AppView.m
//  XAdemo
//
//  Created by Zhang Chuck on 2/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AppView.h"
#import "XA.h"
@implementation AppView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        
        UIButton *updateButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
        [updateButton addTarget:self action:@selector(updateButtonClick) forControlEvents:UIControlEventTouchUpInside];
        [updateButton setTitle:@"user.update" forState:UIControlStateNormal];
        updateButton.frame = CGRectMake(80.0, 20.0, 160.0, 40.0);
        [self addSubview:updateButton];
        
        UIButton *incrementButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
        [incrementButton addTarget:self action:@selector(incrementButtonClick) forControlEvents:UIControlEventTouchUpInside];
        [incrementButton setTitle:@"user.increment" forState:UIControlStateNormal];
        incrementButton.frame = CGRectMake(80.0, 70.0, 160.0, 40.0);
        [self addSubview:incrementButton];
        
        UIButton *loginButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
        [loginButton addTarget:self action:@selector(loginButtonClick) forControlEvents:UIControlEventTouchUpInside];
        [loginButton setTitle:@"user.login" forState:UIControlStateNormal];
        loginButton.frame = CGRectMake(80.0, 120.0, 160.0, 40.0);
        [self addSubview:loginButton];
        
        UIButton *payCompleteButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
        [payCompleteButton addTarget:self action:@selector(paycompleteButtonClick) forControlEvents:UIControlEventTouchUpInside];
        [payCompleteButton setTitle:@"pay.complete" forState:UIControlStateNormal];
        payCompleteButton.frame = CGRectMake(80.0, 170.0, 160.0, 40.0);
        [self addSubview:payCompleteButton];
        
        UIButton *countButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
        [countButton addTarget:self action:@selector(countButtonClick) forControlEvents:UIControlEventTouchUpInside];
        [countButton setTitle:@"count" forState:UIControlStateNormal];
        countButton.frame = CGRectMake(80.0, 220.0, 160.0, 40.0);
        [self addSubview:countButton];
        
        UIButton *milestoneButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
        [milestoneButton addTarget:self action:@selector(milestoneButtonClick) forControlEvents:UIControlEventTouchUpInside];
        [milestoneButton setTitle:@"milestone" forState:UIControlStateNormal];
        milestoneButton.frame = CGRectMake(80.0, 270.0, 160.0, 40.0);
        [self addSubview:milestoneButton];
        
        UIButton *tutorialButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
        [tutorialButton addTarget:self action:@selector(tutorialButtonClick) forControlEvents:UIControlEventTouchUpInside];
        [tutorialButton setTitle:@"tutorial" forState:UIControlStateNormal];
        tutorialButton.frame = CGRectMake(80.0, 320.0, 160.0, 40.0);
        [self addSubview:tutorialButton];
        
        UIButton *buyItemButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
        [buyItemButton addTarget:self action:@selector(buyitemButtonClick) forControlEvents:UIControlEventTouchUpInside];
        [buyItemButton setTitle:@"buy.item" forState:UIControlStateNormal];
        buyItemButton.frame = CGRectMake(80.0, 370.0, 160.0, 40.0);
        [self addSubview:buyItemButton];
    }
    return self;
}


-(void)updateButtonClick
{
    NSMutableDictionary *tris = [NSMutableDictionary dictionaryWithObjectsAndKeys:@"value1",@"key1",@"value2",@"key2",nil];//要指定一个nil
    [XA trackUserUpdate:tris];
}
-(void)incrementButtonClick
{
    NSMutableDictionary *tris = [NSMutableDictionary dictionaryWithObjectsAndKeys:@"value1",@"key1",@"value2",@"key2",nil];//要指定一个nil
    [XA trackUserIncrement:tris];
}
-(void)loginButtonClick
{
    
    NSString *userid=[NSString  stringWithFormat:@"gaomao=%d",1];
    [XA event:USER_LOGIN appId:@"chucktest@337_ar_xatest" userId:userid timestamp:222222222 params:@"{}"];
    
}
-(void)paycompleteButtonClick
{
    [XA trackPayService:@"transID" channel:@"appStore" gross:@"100" gcurrency:@"USD" vamount:@"10" vcurrentcy:@"ni"];
}
-(void)countButtonClick
{
    [XA trackCount:@"abc" level1:@"level1" level2:@"level2" level3: @"level3" level4:@"level4" level5:@"level5" count: 10];
}
-(void)milestoneButtonClick
{
    [XA trackMilestone:@"MILE STONE"];
}
-(void)tutorialButtonClick
{
    [XA trackTutorialService:@"3"  name:@"go" tutorial:@"to"];
}
-(void)buyitemButtonClick
{
    [XA trackBuyService:@"currency" payType:@"PAYTYPE" level1:@"level1" level2:@"level2" level3: @"level3" level4:@"level4" level5:@"level5" amount:11 number:12];
}


/*
 // Only override drawRect: if you perform custom drawing.
 // An empty implementation adversely affects performance during animation.
 - (void)drawRect:(CGRect)rect
 {
 // Drawing code
 }
 */

@end
