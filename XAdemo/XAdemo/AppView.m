//
//  AppView.m
//  XAdemo
//
//  Created by Zhang Chuck on 2/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AppView.h"

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
    
}
-(void)incrementButtonClick
{
    
}
-(void)loginButtonClick
{
    
}
-(void)paycompleteButtonClick
{
    
}
-(void)countButtonClick
{
    
}
-(void)milestoneButtonClick
{
    
}
-(void)tutorialButtonClick
{
    
}
-(void)buyitemButtonClick
{
    
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
