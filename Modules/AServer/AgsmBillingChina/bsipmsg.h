/*
* Copyright (c) 2005,上海盛大网络计费平台部
* All rights reserved.
*
* 文件名称：bsipmsg.h
* 摘    要：各接口消息体定义（client与GS间信息交互）
* 当前版本：1.4.2
* 作    者：李闻
* 完成日期：2005年9月6日
*
* 修改内容：1.4版本变更该文件
* 修改时间：2005年9月6日
*
* 修改内容：剥离宏定义
* 修改时间：2005年9月21日
*
* 修改内容：Author接口添加account_id字段
*           Account接口添加account_id、map_id字段
* 修改时间：2005年10月8日
*
* 修改内容：添加Notify消息汇总接口
* 修改时间：2005年10月13日
*
* 修改内容：添加AccountLock\AccountUnlock\CosignLock
*               \ConsignDeposit\ConsignUnlock消息中的item_id等字段
* 修改时间：2005年11月25日
*
* 修改内容：添加AccountLock\AccountUnlock\CosignLock
*               \ConsignDeposit\ConsignUnlock消息中的item_num、item_price等字段
* 修改时间：2005年11月28日
*
* 修改内容：增加了 item_sync 消息 和 gs_route 消息
* 修改时间：2006年8月16日

* 修改内容：增加了　多人对战 消息
* 修改时间：2006年11月20日
*
* 修改内容：所有char1改为int；ptid和sdid合并为userid；
*           废除部分多于字段；修改部分字段名；调整字段顺序
* 修改时间：2007-2-1 17:19
*
* 修改内容：增加ConsignTransfer消息，废除原寄售消息，支持一次预冻多次解冻，
*           修改道具信息为变长数据，支持传世传奇
* 修改时间：2007-5-3 9:25
*
* 修改内容：道具信息结构更改，
*           修改individual字段名为appendix，128字节，所有响应均添加该字段
* 修改时间：2007-5-3 9:25
*
* 修改内容：增加EkeyBind消息
* 修改时间：2007-6-19 15:54
*
* 修改内容：增加AccountExchangelock、unlock消息，和GoldConsume消息极其雷同
* 修改时间：2009-1-14 张立春
*/

/*
*bsip msg body ,
*/
#ifndef _BSIP_MSG_H_
#define _BSIP_MSG_H_

#define BSIP_VERSION 0X00010600

#include "msgmacro.h"

/*------------------------------消息体定义------------------------------*/
/*-------------------------------时长计费-------------------------------*/
/*---------------------------------授权---------------------------------*/
typedef struct GSBsipAuthorReq
{
    int  user_type;                      //用户类型
    int  status;                         //用户状态
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];   //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];       //数据包发送时间
}GSBsipAuthorReqDef;

typedef struct GSBsipAuthorRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的点数、秒数或天数(对应pay_type)
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char begin_time[MAX_TIME_LEN+1];     //生效时间（保留字段）
    char end_time[MAX_TIME_LEN+1];       //失效时间（保留字段）
}GSBsipAuthorResDef;

/*---------------------------------扣费---------------------------------*/
typedef struct GSBsipAccountReq
{
    int  pay_type;                       //付费类型
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  amount;                         //消费数量
    int  discount;                       //计费折扣率
    int  map_id;                         //地图id
    int  responsable;                    //是否需要响应标志
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char begin_time[MAX_TIME_LEN+1];     //上线时间
    char end_time[MAX_TIME_LEN+1];       //下线时间
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];       //数据包发送时间
}GSBsipAccountReqDef;

typedef struct GSBsipAccountRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的点数、秒数或天数(对应pay_type)
    int  map_id;                         //地图id
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
}GSBsipAccountResDef;

/*-------------------------------按次计费-------------------------------*/
/*-------------------------------账户认证-------------------------------*/
typedef struct GSBsipAccountAuthenReq
{
    int  user_type;                      //用户类型
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];   //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];       //数据包发送时间
    int  indication_authen;               //认证标志
    int  pay_type;                        //指定仅仅认证这一种余额
}GSBsipAccountAuthenReqDef;

typedef struct GSBsipAccountAuthenRes
{
    int  result;                         //返回结果
    int  pay_type1;                      //付费类型
    int  balance1;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type2;                      //付费类型
    int  balance2;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type3;                      //付费类型
    int  balance3;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type4;                      //付费类型
    int  balance4;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type5;                      //付费类型
    int  balance5;                       //剩余的点数、秒数或天数(对应pay_type)
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char begin_time[MAX_TIME_LEN+1];     //生效时间（保留字段）
    char end_time[MAX_TIME_LEN+1];       //失效时间（保留字段）
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char appendix[128];
}GSBsipAccountAuthenResDef;

///  goods_info 的结构
typedef struct GSBsipGoods
{
	int serv_id_item;                   //道具属主（游戏）编号
	int area_id_item;                   //道具属主（游戏区）编号
	int group_id_item;                  //道具属主（游戏组）编号
    int item_id;                        //游戏道具编号
    int item_num;                       //游戏道具数量
	int item_amount;                    //该游戏道具的总价（非单价）
	int discount;
}GSBsipGoodsDef;

///  item_info 的结构
typedef struct GSBsipItem 
{ //因为这个结构直接操作与GS，所以不需要serviceid等信息
    int  item_id;                        //游戏道具编号
    int  item_num;                       //游戏道具数量
    char item_property[32];              //该游戏道具的属性
}GSBsipItemDef;

/*---------------------------------预冻---------------------------------*/
typedef struct GSBsipAccountLockReq
{
    int  pay_type;                       //付费类型
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  fee;                            //手续费
    int  lock_period;                    //lock相对时长
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  indication_lock;
    int  item_num;                       //有效道具种类数
    GSBsipGoodsDef goods_info[1];        //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 goods_info 的结构  字节数=
                                         //item_num * sizeof(GSBsipGoodsDef)
}GSBsipAccountLockReqDef;

typedef struct GSBsipAccountLockRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的PT币
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char award_num[MAX_AWARD_LEN+1];     //领奖号码
    char appendix[128];                  //附加信息
}GSBsipAccountLockResDef;

/*---------------------------------解冻---------------------------------*/
typedef struct GSBsipAccountUnlockReq
{
    int  pay_type;                       //付费类型
    int  amount;                   //解冻金额，可以是部分预冻金额，分多次解冻
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  confirm;                        //确认扣除
    int  responsable;                    //是否需要响应标志
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  indication_unlock;
    int  item_num;                       //有效道具种类数
    GSBsipGoodsDef goods_info[1];        //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 goods_info 的结构  字节数=
                                         //item_num * sizeof(GSBsipGoodsDef)
}GSBsipAccountUnlockReqDef;

typedef struct GSBsipAccountUnlockRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的未被冻结的可用余额
    int  balance_locked;                 //被冻结的未被解冻的剩余金额
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char appendix[128];                  //附加信息
}GSBsipAccountUnlockResDef;

/*---------------------------------寄售转帐---------------------------------*/
typedef struct GSBsipConsignTransferReq
{
    int  app_type;
    int  serv_id_player_dest;
    int  area_id_player_dest;            //区编号
    int  group_id_player_dest;           //组编号
    int  uid_type_dest;                  //买家帐号类型：1－pt帐户；2－数字id
    char user_id_dest[MAX_PTID_LEN+1];   //账号id（数字账号或pt账号）
    int  fee_dest;                       //得钱的人所要出的手续费
    int  pay_type;                       //付费的方式
    int  amount;                         //金额
    int  serv_id_player_src;             //游戏区编号
    int  area_id_player_src;             //游戏区编号
    int  group_id_player_src;            //表示组id
    int  uid_type_src;                   //帐号类型：1－pt帐户；2－数字id
    char user_id_src[MAX_PTID_LEN+1];    //账号id（数字账号或pt账号）
    int  fee_src;                        //付出钱的人所要出的手续费
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  indication_unlock;
    int  item_num;                       //有效道具种类数
    GSBsipGoodsDef goods_info[1];        //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 goods_info 的结构  字节数=
                                         //item_num * sizeof(GSBsipGoodsDef)
}GSBsipConsignTransferReqDef;

typedef struct GSBsipConsignTransferRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费的方式
    int  uid_type_dest;                  //帐号类型：1－pt帐户；2－数字id
    char user_id_dest[MAX_PTID_LEN+1];   //买家账号id（数字账号或pt账号）
    int  balance_dest;                   //剩余PT币
    int  uid_type_src;                   //帐号类型：1－pt帐户；2－数字id
    char user_id_src[MAX_PTID_LEN+1];    //买家账号id（数字账号或pt账号）
    int  balance_locked_src;             //付钱方所剩被预冻得余额
    int  balance_src;                    //剩余PT币
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识
    char appendix[128];                  //附加信息
}GSBsipConsignTransferResDef;

/*---------------------------------预冻扩展---------------------------------*/
typedef struct GSBsipAccountLockExReq
{
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  fee;
    int  lock_period;                    //lock相对时长
    int  pay_type1;                      //付费类型
    int  amount1;                        //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type2;                      //付费类型
    int  amount2;                        //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type3;                      //付费类型
    int  amount3;                        //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type4;                      //付费类型
    int  amount4;                        //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type5;                      //付费类型
    int  amount5;                        //剩余的点数、秒数或天数(对应pay_type)
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  item_num;                       //有效道具种类数
    GSBsipGoodsDef goods_info[1];        //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 goods_info 的结构  字节数=
                                         //item_num * sizeof(GSBsipGoodsDef)
}GSBsipAccountLockExReqDef;

typedef struct GSBsipAccountLockExRes
{
    int  result;                         //返回结果
    int  pay_type1;                      //付费类型
    int  balance1;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type2;                      //付费类型
    int  balance2;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type3;                      //付费类型
    int  balance3;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type4;                      //付费类型
    int  balance4;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type5;                      //付费类型
    int  balance5;                       //剩余的点数、秒数或天数(对应pay_type)
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char award_num[MAX_AWARD_LEN+1];     //领奖号码
    char appendix[128];                  //附加信息
}GSBsipAccountLockExResDef;

/*---------------------------------解冻扩展---------------------------------*/
typedef struct GSBsipAccountUnlockExReq
{
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  confirm;                        //确认扣除
    int  pay_type1;                      //付费类型
    int  amount1;                        //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type2;                      //付费类型
    int  amount2;                        //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type3;                      //付费类型
    int  amount3;                        //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type4;                      //付费类型
    int  amount4;                        //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type5;                      //付费类型
    int  amount5;                        //剩余的点数、秒数或天数(对应pay_type)
    int  responsable;                    //是否需要响应标志
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  item_num;                       //有效道具种类数
    GSBsipGoodsDef goods_info[1];        //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 goods_info 的结构  字节数=
                                         //item_num * sizeof(GSBsipGoodsDef)
}GSBsipAccountUnlockExReqDef;

typedef struct GSBsipAccountUnlockExRes
{
    int  result;                         //返回结果
    int  pay_type1;                      //付费类型
    int  balance1;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type2;                      //付费类型
    int  balance2;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type3;                      //付费类型
    int  balance3;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type4;                      //付费类型
    int  balance4;                       //剩余的点数、秒数或天数(对应pay_type)
    int  pay_type5;                      //付费类型
    int  balance5;                       //剩余的点数、秒数或天数(对应pay_type)
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char appendix[128];                  //附加信息
}GSBsipAccountUnlockExResDef;

/*---------------------------------领奖---------------------------------*/
/*-------------------------------领奖认证-------------------------------*/
typedef struct GSBsipAwardAuthenReq
{
    int  user_type;                      //用户类型
    int  couple_type;                    //耦合类型
    int  award_type;                     //领奖类型
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char award_num[MAX_AWARD_LEN+1];     //领奖号码
    char award_patchid[MAX_AWARDPATCH_LEN+1]; //领奖批次号
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    char role_name[32+1];                //玩家当前角色名
    int  current_level;                  //玩家当前角色级别
    int  award_level;                    //玩家领奖级别
    char ind_msg[128];                   //附加信息，保留
}GSBsipAwardAuthenReqDef;

typedef struct GSBsipAwardAuthenRes
{
    int  result;                         //返回结果
    int  couple_type;                    //耦合类型
    int  bind_user;                      //邦定用户标记
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //游戏客户端的IP地址
    char award_num[MAX_AWARD_LEN+1];     //领奖号码
    char appendix[128];                  //附加信息
    int  item_num;                       //有效道具种类数
    GSBsipItemDef item_info[1];          //变长数组，为了提供一个连续的地址空间的首地址
                                         //见 item_info 的结构  字节数=
                                         //item_num * sizeof(GSBsipItemDef)
}GSBsipAwardAuthenResDef;

/*-------------------------------领奖确认-------------------------------*/
typedef struct GSBsipAwardAck
{
    int  confirm;                        //
    int  award_type;                     //领奖类型
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char award_num[MAX_AWARD_LEN+1];     //领奖号码
    char award_time[MAX_TIME_LEN+1];     //发奖成功、失败时间
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
}GSBsipAwardAckDef;

typedef struct GSBsipAwardAckRes
{
    int  result;                         //返回结果
    int  couple_type;                    //耦合类型
    int  award_type;                     //奖品类型
    int  pay_type;                       //领奖的充值类型
    int  balance;                        //充值后用户余额
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char appendix[128];                  //附加信息
}GSBsipAwardAckResDef;

#if BSIP_VERSION < 0X00010600

/*-------------------------------寄售交易-------------------------------*/
/*-------------------------------寄售预冻-------------------------------*/
typedef struct GSBsipConsignLockReq
{
    int  area_id;                        //区编号
    int  group_id;                       //组编号
    int  pay_type;                       //付费类型
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  lock_period;                    //lock相对时长
    int  discount;                       //计费折扣率
    int  item_num;                       //有效道具种类数
    int  item1_id;                       //游戏道具编号
    int  item1_num;                      //游戏道具数量
    int  item1_price;                    //游戏道具单价
    int  item2_id;                       //游戏道具编号
    int  item2_num;                      //游戏道具数量
    int  item2_price;                    //游戏道具单价
    int  item3_id;                       //游戏道具编号
    int  item3_num;                      //游戏道具数量
    int  item3_price;                    //游戏道具单价
    int  item4_id;                       //游戏道具编号
    int  item4_num;                      //游戏道具数量
    int  item4_price;                    //游戏道具单价
    int  item5_id;                       //游戏道具编号
    int  item5_num;                      //游戏道具数量
    int  item5_price;                    //游戏道具单价
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
}GSBsipConsignLockReqDef;

typedef struct GSBsipConsignLockRes
{
    int  result;                         //返回结果
    int  area_id;                        //区编号
    int  group_id;                       //组编号
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的PT币
    int  item_num;                       //有效道具种类数
    int  item1_id;                       //游戏道具编号
    int  item1_num;                      //游戏道具数量
    int  item1_price;                    //游戏道具单价
    int  item2_id;                       //游戏道具编号
    int  item2_num;                      //游戏道具数量
    int  item2_price;                    //游戏道具单价
    int  item3_id;                       //游戏道具编号
    int  item3_num;                      //游戏道具数量
    int  item3_price;                    //游戏道具单价
    int  item4_id;                       //游戏道具编号
    int  item4_num;                      //游戏道具数量
    int  item4_price;                    //游戏道具单价
    int  item5_id;                       //游戏道具编号
    int  item5_num;                      //游戏道具数量
    int  item5_price;                    //游戏道具单价
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
}GSBsipConsignLockResDef;

/*-------------------------------寄售解冻-------------------------------*/
typedef struct GSBsipConsignUnlockReq
{
    int  area_id;                        //区编号
    int  group_id;                       //组编号
    int  pay_type;                       //付费类型
    int  app_type;                       //消费类别
    int  status;                         //用户状态
    int  couple_type;                    //耦合类型
    int  discount;                       //计费折扣率
    int  confirm;                        //确认扣除
    int  responsable;                    //是否需要响应标志
    int  item_num;                       //有效道具种类数
    int  item1_id;                       //游戏道具编号
    int  item1_num;                      //游戏道具数量
    int  item1_price;                    //游戏道具单价
    int  item2_id;                       //游戏道具编号
    int  item2_num;                      //游戏道具数量
    int  item2_price;                    //游戏道具单价
    int  item3_id;                       //游戏道具编号
    int  item3_num;                      //游戏道具数量
    int  item3_price;                    //游戏道具单价
    int  item4_id;                       //游戏道具编号
    int  item4_num;                      //游戏道具数量
    int  item4_price;                    //游戏道具单价
    int  item5_id;                       //游戏道具编号
    int  item5_num;                      //游戏道具数量
    int  item5_price;                    //游戏道具单价
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
}GSBsipConsignUnlockReqDef;

typedef struct GSBsipConsignUnlockRes
{
    int  result;                         //返回结果
    int  area_id;                        //区编号
    int  group_id;                       //组编号
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的PT币
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
}GSBsipConsignUnlockResDef;

/*-------------------------------寄售充值-------------------------------*/
typedef struct GSBsipConsignDepositReq
{
    int  area_id;                        //区编号
    int  group_id;                       //组编号
    int  user_type;                      //用户类型
    int  pay_type;                       //付费的方式
    int  amount;                         //元宝金额
    int  buyer_area_id;                  //游戏区编号
    int  buyer_group_id;                 //表示组id
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    int  buyer_uid_type;                 //买家帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char buyer_user_id[MAX_PTID_LEN+1];  //买家账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char patch_id[MAX_PATCHID_LEN+1];    //批次号
    char ip[IP_ADDRESS_LEN+1];           //ip游戏客户端地址
    char log_time[MAX_TIME_LEN+1];       //数据包发送时间
}GSBsipConsignDepositReqDef;

typedef struct GSBsipConsignDepositRes
{
    int  result;                         //返回结果
    int  area_id;                        //区编号
    int  group_id;                       //组编号
    int  buyer_area_id;                  //游戏区编号
    int  buyer_group_id;                 //表示组id
    int  pay_type;                       //付费的方式
    int  balance;                        //剩余PT币
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    int  buyer_uid_type;                 //买家帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char buyer_user_id[MAX_PTID_LEN+1];  //买家账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识
    char context_id[MAX_ORDERID_LEN+1];  //关联id
    char patch_id[MAX_PATCHID_LEN+1];    //批次号
    char ip[IP_ADDRESS_LEN+1];           //ip游戏客户端地址
}GSBsipConsignDepositResDef;
#endif // BSIP_VERSION  < 0X00010600

#if BSIP_VERSION < 0X00010400
/*-------------------------------元宝交易-------------------------------*/
/*-------------------------------元宝充值-------------------------------*/
typedef struct GSBsipGoldDepositReq
{
    int  user_type;                      //用户类型
    int  pay_type;                       //付费类型
    int  amount;                         //充值金额
    int  sync_flag;                      //充值消息标志位
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
}GSBsipGoldDepositReqDef;

typedef struct GSBsipGoldDepositRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费类型
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char ip[IP_ADDRESS_LEN+1];           //游戏客户端的IP地址
}GSBsipGoldDepositResDef;
#endif // BSIP_VERSION  < 0X00010400

/*抵用券交易*/
typedef struct GSBsipTokenDepositReq
{
    int  pay_type;                       //付费类型
    int  amount;                         //充值金额
    int  item_id;                        //购买或回收的道具编号
    int  item_num;                       //购买或回收的道具数量
    int  item_price;                     //购买或回收的道具单价
    int  deposit_flag;                   //充值消息标志位 1-购买返还 2-道具回收 其他-待定
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //order_id的相关id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char patch_id[MAX_PATCHID_LEN+1];    //批次号
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  valid_period;                   //充值数额有效期，“月”
}GSBsipTokenDepositReqDef;

typedef struct GSBsipTokenDepositRes
{
    int  result;                         //返回结果
    int  pay_type;                       //付费类型
    int  balance;                        //剩余的抵用券
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //order_id的相关id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char appendix[128];                  //附加信息
}GSBsipTokenDepositResDef;

/*-------------------------------元宝换时间-------------------------------*/
typedef struct GSBsipGoldConsumeLockReq
{
    int  couple_type;                    //耦合类型
    int  discount;                       //计费折扣率
    int  src_pay_type;                   //付费类型
    int  src_amount;                     //消费数量
    int  dest_pay_type;                  //充入的时长类型
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //数据包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
}GSBsipGoldConsumeLockReqDef;

typedef struct GSBsipGoldConsumeLockRes
{
    int  result;                         //返回结果
    int  src_pay_type;                   //付费类型
    int  src_balance;                    //兑换后总的点券数量
    int  dest_pay_type;                  //充入的时长类型
    int  dest_balance;                   //兑换后总的时长数量
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //数据包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char begin_time[MAX_TIME_LEN+1];     //生效时间 （time_type 为1 时有效）
    char end_time[MAX_TIME_LEN+1];       //失效时间 （time_type 为1 时有效）
    char appendix[128];                  //附加信息
}GSBsipGoldConsumeLockResDef;

typedef struct GSBsipGoldConsumeUnlockReq
{
    int  confirm;                        //确认扣除
    int  couple_type;                    //耦合类型
    int  discount;                       //计费折扣率
    int  src_pay_type;                   //付费类型
    int  src_amount;                     //消费数量
    int  dest_pay_type;                  //充入的时长类型
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //数据包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
}GSBsipGoldConsumeUnlockReqDef;

typedef struct GSBsipGoldConsumeUnlockRes
{
    int  result;                         //返回结果
    int  src_pay_type;                   //付费类型
    int  src_balance;                    //兑换后总的点券数量
    int  dest_pay_type;                  //充入的时长类型
    int  dest_balance;                   //兑换后总的时长数量
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char begin_time[MAX_TIME_LEN+1];     //生效时间 （time_type 为1 时有效）
    char end_time[MAX_TIME_LEN+1];       //失效时间 （time_type 为1 时有效）
    char appendix[128];                  //附加信息
}GSBsipGoldConsumeUnlockResDef;

/*-------------------------------货币兑换-------------------------------*/
typedef struct GSBsipAccountExchangeLockReq
{
    int  couple_type;                    //耦合类型
    int  discount;                       //计费折扣率
    int  src_pay_type;                   //付费类型
    int  src_amount;                     //消费数量
    int  dest_pay_type;                  //充入的时长类型
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //数据包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  rate_decision_lock;             //1 转帐预冻(汇率由后台计算) 
                                         //2 直接转帐(汇率由后台计算) 
                                         //3 转帐预冻(汇率由前端计算) 
                                         //4 直接转帐(汇率由前端计算)
}GSBsipAccountExchangeLockReqDef;

typedef struct GSBsipAccountExchangeLockRes
{
    int  result;                         //返回结果
    int  src_pay_type;                   //付费类型
    int  src_balance;                    //兑换后总的点券数量
    int  dest_pay_type;                  //充入的时长类型
    int  dest_balance;                   //兑换后总的时长数量
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //数据包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char begin_time[MAX_TIME_LEN+1];     //生效时间 （time_type 为1 时有效）
    char end_time[MAX_TIME_LEN+1];       //失效时间 （time_type 为1 时有效）
    char appendix[128];                  //附加信息
}GSBsipAccountExchangeLockResDef;

typedef struct GSBsipAccountExchangeUnlockReq
{
    int  confirm;                        //确认扣除
    int  couple_type;                    //耦合类型
    int  discount;                       //计费折扣率
    int  src_pay_type;                   //付费类型
    int  src_amount;                     //消费数量
    int  dest_pay_type;                  //充入的时长类型
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //数据包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
    int  rate_decision_unlock;           //1 转帐预冻(汇率由后台计算) 
                                         //2 解冻(汇率由前端定)
}GSBsipAccountExchangeUnlockReqDef;

typedef struct GSBsipAccountExchangeUnlockRes
{
    int  result;                         //返回结果
    int  src_pay_type;                   //付费类型
    int  src_balance;                    //兑换后总的点券数量
    int  dest_pay_type;                  //充入的时长类型
    int  dest_balance;                   //兑换后总的时长数量
    int  dest_amount;                    //兑换的时长数量
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char context_id[MAX_ORDERID_LEN+1];  //关联包id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    char begin_time[MAX_TIME_LEN+1];     //生效时间 （time_type 为1 时有效）
    char end_time[MAX_TIME_LEN+1];       //失效时间 （time_type 为1 时有效）
    char appendix[128];                  //附加信息
}GSBsipAccountExchangeUnlockResDef;

/*-------------------------------发货系统-------------------------------*/
typedef struct GSBsipItemSyncReq
{
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识
    int  sync_times;                     //同步次数
    int  indication_sync;                //指令消息 预留
    char msg_sync[256];                  //同步消息
    char item_info[512];                 //道具信息，格式为：
                                         //道具编号1,数量1;道具编号2,数量2;道具编号3,数量3;……
                                         //不同道具间使用半角分号分隔，
                                         //道具编号和数量之间用半角逗号分隔
}GSBsipItemSyncReqDef;

typedef struct GSBsipItemSyncRes
{
    int  result;                         //同步结果：0成功；其他失败
    char order_id[MAX_ORDERID_LEN+1];    //唯一标识，与请求消息该字段一致
}GSBsipItemSyncResDef;

/*-------------------------------道具日志采集结构-------------------------------*/
typedef struct GSBsipItemLogMsg
{
    int  serv_id_player;				//道具的服务号
    int  area_id_player;				//道具的区号
    int  group_id_player;				//道具的组号
    char user_id[MAX_PTID_LEN+1];		//用户账户
    char role_name[32+1];               //玩家当前角色名
    int  item_flag;						//道具出入标志
    char context_id[MAX_ORDERID_LEN+1]; //关联id
    char item_uniqueid[32+1];			//道具在游戏中的唯一编号
    int  item_id;                       //游戏道具编号
    int  item_num;                      //游戏道具数量
    int  item_amount;                   //该游戏道具的总价
    char reserve[128];                  //备用字段用于日后添加新的信息
                                        //添加的信息如有多个域需用','分隔，没有时为空
}GSBsipItemLogMsgDef;

/*------------------------------密宝绑定---------------------------------*/
typedef struct GSBsipEkeyBindReq
{
    int  operation_type;                 //1表示密宝；其他待定
    int  indication_bind;                //1－绑定密宝；2－解除绑定密宝
    int  pay_type;
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //终端IP地址
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //数据包发送时间
}GSBsipEkeyBindReqDef;

typedef struct GSBsipEkeyBindRes
{
    int  result;                         //返回结果
    int  uid_type;                       //帐号类型：1－pt帐户；2－数字id
    char user_id[MAX_PTID_LEN+1];        //账号id（数字账号或pt账号）
    char sess_id[MAX_SESSID_LEN+1];      //会话id
    char order_id[MAX_ORDERID_LEN+1];    //数据包id
}GSBsipEkeyBindResDef;

//--------------------GSBsipNotifyS2CReqDef--------------------
typedef struct GSBsipNotifyS2CReq   //cbs发起消息
{
    int  serv_id_site;
    int  area_id_site;
    int  group_id_site;
    int  serv_id_src;         // 消息来源serviceid
    int  area_id_src;         // 消息来源areaid
    int  group_id_src;        // 消息来源groupid
    char order_id[MAX_ORDERID_LEN+1];
    int  req_times;           // 发送次数，用于客户端校验是否重复
    int  indication_s2c;   	  //消息指示,缺省为0
    int  req_len;             // req_info的长度
    char req_info[1];         // 最大512字节
}GSBsipNotifyS2CReqDef;       // cbs->gs

typedef struct GSBsipNotifyS2CRes
{
    int  result;
    char order_id[MAX_ORDERID_LEN+1];
}GSBsipNotifyS2CResDef;     // gs->cbs

//------------------GSBsipNotifyC2SReqDef----------------------
typedef struct GSBsipNotifyC2SReq  //gs发起消息
{
    int  serv_id_site;      //调用者的应用id号，见统一数字编码
    int  area_id_site;      //调用者的区id号，见统一数字编码
    int  group_id_site;     //调用者的组id号，见统一数字编码
    int  serv_id_dest;        // 消息传送目标
    int  area_id_dest;        // 消息传送目标
    int  group_id_dest;       // 消息传送目标
    char order_id[MAX_ORDERID_LEN+1];//数据包ID
    char order_id_relation[MAX_ORDERID_LEN+1];//g数据包ID
    int  indication_c2s;   	  //消息指示,1:用户领奖,2:主动奖品推送到GS
    int  interface_id;        //接口ID标识由道具系统指定
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];
    int  platform_type;
    int  req_len;             // req_info的长度
    char req_info[1];         // 最大512字节
}GSBsipNotifyC2SReqDef;    // gs->cbs

typedef struct GSBsipNotifyC2SRes
{
    int  result;
    char order_id[MAX_ORDERID_LEN+1];
}GSBsipNotifyC2SResDef;       // cbs->gs

#endif
