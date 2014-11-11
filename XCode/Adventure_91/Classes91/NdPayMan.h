//
//  NdPayMan.h
//  Demo91
//
//  Created by Delle  on 14-3-4.
//  Copyright (c) 2014年 Delle . All rights reserved.
//

#import <Foundation/Foundation.h>
#import "OrderManDelegate.h"

@protocol PayManDelegate <NSObject>
@required
- (void)didPaid;
- (void)didPaidFailedWithErrorDescription:(NSString *)des;
@end


@interface NdPayMan : NSObject<OrderManDelegate>{
    BOOL _isInnerMode;
    id<PayManDelegate> _delegate;
}

@property (nonatomic, assign) BOOL isInnerMode;
@property (nonatomic, assign) id<PayManDelegate>delegate;

+ (NdPayMan *)sharedInstance;
+ (void)purgeSharedInstance;
/**
 *  支付，调用前先设置delegate
 *
 *  @param productId  产品的id
 *  @param userId     userId
 *  @param channelId  渠道号
 *  @param serverId   服务器ID
 *  @param currency   货币类型，"0"是人民币，"1"是美元
 *  @param createTime 角色创建时间
 */
- (void)doPayWithProductId:(NSString *)productId
                    userId:(NSString *)userId
                 channelId:(NSString *)channelId
                  serverId:(NSString *)serverId
                  currency:(NSString *)currency
         andUserCreateTime:(NSString *)createTime;
@end
