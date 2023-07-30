/*
* Copyright (c) 2005,�Ϻ�ʢ������Ʒ�ƽ̨��
* All rights reserved.
*
* �ļ����ƣ�bsipmsg.h
* ժ    Ҫ�����ӿ���Ϣ�嶨�壨client��GS����Ϣ������
* ��ǰ�汾��1.4.2
* ��    �ߣ�����
* ������ڣ�2005��9��6��
*
* �޸����ݣ�1.4�汾������ļ�
* �޸�ʱ�䣺2005��9��6��
*
* �޸����ݣ�����궨��
* �޸�ʱ�䣺2005��9��21��
*
* �޸����ݣ�Author�ӿ����account_id�ֶ�
*           Account�ӿ����account_id��map_id�ֶ�
* �޸�ʱ�䣺2005��10��8��
*
* �޸����ݣ����Notify��Ϣ���ܽӿ�
* �޸�ʱ�䣺2005��10��13��
*
* �޸����ݣ����AccountLock\AccountUnlock\CosignLock
*               \ConsignDeposit\ConsignUnlock��Ϣ�е�item_id���ֶ�
* �޸�ʱ�䣺2005��11��25��
*
* �޸����ݣ����AccountLock\AccountUnlock\CosignLock
*               \ConsignDeposit\ConsignUnlock��Ϣ�е�item_num��item_price���ֶ�
* �޸�ʱ�䣺2005��11��28��
*
* �޸����ݣ������� item_sync ��Ϣ �� gs_route ��Ϣ
* �޸�ʱ�䣺2006��8��16��

* �޸����ݣ������ˡ����˶�ս ��Ϣ
* �޸�ʱ�䣺2006��11��20��
*
* �޸����ݣ�����char1��Ϊint��ptid��sdid�ϲ�Ϊuserid��
*           �ϳ����ֶ����ֶΣ��޸Ĳ����ֶ����������ֶ�˳��
* �޸�ʱ�䣺2007-2-1 17:19
*
* �޸����ݣ�����ConsignTransfer��Ϣ���ϳ�ԭ������Ϣ��֧��һ��Ԥ����νⶳ��
*           �޸ĵ�����ϢΪ�䳤���ݣ�֧�ִ�������
* �޸�ʱ�䣺2007-5-3 9:25
*
* �޸����ݣ�������Ϣ�ṹ���ģ�
*           �޸�individual�ֶ���Ϊappendix��128�ֽڣ�������Ӧ����Ӹ��ֶ�
* �޸�ʱ�䣺2007-5-3 9:25
*
* �޸����ݣ�����EkeyBind��Ϣ
* �޸�ʱ�䣺2007-6-19 15:54
*
* �޸����ݣ�����AccountExchangelock��unlock��Ϣ����GoldConsume��Ϣ������ͬ
* �޸�ʱ�䣺2009-1-14 ������
*/

/*
*bsip msg body ,
*/
#ifndef _BSIP_MSG_H_
#define _BSIP_MSG_H_

#define BSIP_VERSION 0X00010600

#include "msgmacro.h"

/*------------------------------��Ϣ�嶨��------------------------------*/
/*-------------------------------ʱ���Ʒ�-------------------------------*/
/*---------------------------------��Ȩ---------------------------------*/
typedef struct GSBsipAuthorReq
{
    int  user_type;                      //�û�����
    int  status;                         //�û�״̬
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];   //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
}GSBsipAuthorReqDef;

typedef struct GSBsipAuthorRes
{
    int  result;                         //���ؽ��
    int  pay_type;                       //��������
    int  balance;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    char begin_time[MAX_TIME_LEN+1];     //��Чʱ�䣨�����ֶΣ�
    char end_time[MAX_TIME_LEN+1];       //ʧЧʱ�䣨�����ֶΣ�
}GSBsipAuthorResDef;

/*---------------------------------�۷�---------------------------------*/
typedef struct GSBsipAccountReq
{
    int  pay_type;                       //��������
    int  app_type;                       //�������
    int  status;                         //�û�״̬
    int  couple_type;                    //�������
    int  amount;                         //��������
    int  discount;                       //�Ʒ��ۿ���
    int  map_id;                         //��ͼid
    int  responsable;                    //�Ƿ���Ҫ��Ӧ��־
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char begin_time[MAX_TIME_LEN+1];     //����ʱ��
    char end_time[MAX_TIME_LEN+1];       //����ʱ��
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
}GSBsipAccountReqDef;

typedef struct GSBsipAccountRes
{
    int  result;                         //���ؽ��
    int  pay_type;                       //��������
    int  balance;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  map_id;                         //��ͼid
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
}GSBsipAccountResDef;

/*-------------------------------���μƷ�-------------------------------*/
/*-------------------------------�˻���֤-------------------------------*/
typedef struct GSBsipAccountAuthenReq
{
    int  user_type;                      //�û�����
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];   //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
    int  indication_authen;               //��֤��־
    int  pay_type;                        //ָ��������֤��һ�����
}GSBsipAccountAuthenReqDef;

typedef struct GSBsipAccountAuthenRes
{
    int  result;                         //���ؽ��
    int  pay_type1;                      //��������
    int  balance1;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type2;                      //��������
    int  balance2;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type3;                      //��������
    int  balance3;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type4;                      //��������
    int  balance4;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type5;                      //��������
    int  balance5;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char begin_time[MAX_TIME_LEN+1];     //��Чʱ�䣨�����ֶΣ�
    char end_time[MAX_TIME_LEN+1];       //ʧЧʱ�䣨�����ֶΣ�
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    char appendix[128];
}GSBsipAccountAuthenResDef;

///  goods_info �Ľṹ
typedef struct GSBsipGoods
{
	int serv_id_item;                   //������������Ϸ�����
	int area_id_item;                   //������������Ϸ�������
	int group_id_item;                  //������������Ϸ�飩���
    int item_id;                        //��Ϸ���߱��
    int item_num;                       //��Ϸ��������
	int item_amount;                    //����Ϸ���ߵ��ܼۣ��ǵ��ۣ�
	int discount;
}GSBsipGoodsDef;

///  item_info �Ľṹ
typedef struct GSBsipItem 
{ //��Ϊ����ṹֱ�Ӳ�����GS�����Բ���Ҫserviceid����Ϣ
    int  item_id;                        //��Ϸ���߱��
    int  item_num;                       //��Ϸ��������
    char item_property[32];              //����Ϸ���ߵ�����
}GSBsipItemDef;

/*---------------------------------Ԥ��---------------------------------*/
typedef struct GSBsipAccountLockReq
{
    int  pay_type;                       //��������
    int  app_type;                       //�������
    int  status;                         //�û�״̬
    int  couple_type;                    //�������
    int  fee;                            //������
    int  lock_period;                    //lock���ʱ��
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
    int  indication_lock;
    int  item_num;                       //��Ч����������
    GSBsipGoodsDef goods_info[1];        //�䳤���飬Ϊ���ṩһ�������ĵ�ַ�ռ���׵�ַ
                                         //�� goods_info �Ľṹ  �ֽ���=
                                         //item_num * sizeof(GSBsipGoodsDef)
}GSBsipAccountLockReqDef;

typedef struct GSBsipAccountLockRes
{
    int  result;                         //���ؽ��
    int  pay_type;                       //��������
    int  balance;                        //ʣ���PT��
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char award_num[MAX_AWARD_LEN+1];     //�콱����
    char appendix[128];                  //������Ϣ
}GSBsipAccountLockResDef;

/*---------------------------------�ⶳ---------------------------------*/
typedef struct GSBsipAccountUnlockReq
{
    int  pay_type;                       //��������
    int  amount;                   //�ⶳ�������ǲ���Ԥ�����ֶ�νⶳ
    int  app_type;                       //�������
    int  status;                         //�û�״̬
    int  couple_type;                    //�������
    int  confirm;                        //ȷ�Ͽ۳�
    int  responsable;                    //�Ƿ���Ҫ��Ӧ��־
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
    int  indication_unlock;
    int  item_num;                       //��Ч����������
    GSBsipGoodsDef goods_info[1];        //�䳤���飬Ϊ���ṩһ�������ĵ�ַ�ռ���׵�ַ
                                         //�� goods_info �Ľṹ  �ֽ���=
                                         //item_num * sizeof(GSBsipGoodsDef)
}GSBsipAccountUnlockReqDef;

typedef struct GSBsipAccountUnlockRes
{
    int  result;                         //���ؽ��
    int  pay_type;                       //��������
    int  balance;                        //ʣ���δ������Ŀ������
    int  balance_locked;                 //�������δ���ⶳ��ʣ����
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char appendix[128];                  //������Ϣ
}GSBsipAccountUnlockResDef;

/*---------------------------------����ת��---------------------------------*/
typedef struct GSBsipConsignTransferReq
{
    int  app_type;
    int  serv_id_player_dest;
    int  area_id_player_dest;            //�����
    int  group_id_player_dest;           //����
    int  uid_type_dest;                  //����ʺ����ͣ�1��pt�ʻ���2������id
    char user_id_dest[MAX_PTID_LEN+1];   //�˺�id�������˺Ż�pt�˺ţ�
    int  fee_dest;                       //��Ǯ������Ҫ����������
    int  pay_type;                       //���ѵķ�ʽ
    int  amount;                         //���
    int  serv_id_player_src;             //��Ϸ�����
    int  area_id_player_src;             //��Ϸ�����
    int  group_id_player_src;            //��ʾ��id
    int  uid_type_src;                   //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id_src[MAX_PTID_LEN+1];    //�˺�id�������˺Ż�pt�˺ţ�
    int  fee_src;                        //����Ǯ������Ҫ����������
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //Ψһ��ʶ
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
    int  indication_unlock;
    int  item_num;                       //��Ч����������
    GSBsipGoodsDef goods_info[1];        //�䳤���飬Ϊ���ṩһ�������ĵ�ַ�ռ���׵�ַ
                                         //�� goods_info �Ľṹ  �ֽ���=
                                         //item_num * sizeof(GSBsipGoodsDef)
}GSBsipConsignTransferReqDef;

typedef struct GSBsipConsignTransferRes
{
    int  result;                         //���ؽ��
    int  pay_type;                       //���ѵķ�ʽ
    int  uid_type_dest;                  //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id_dest[MAX_PTID_LEN+1];   //����˺�id�������˺Ż�pt�˺ţ�
    int  balance_dest;                   //ʣ��PT��
    int  uid_type_src;                   //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id_src[MAX_PTID_LEN+1];    //����˺�id�������˺Ż�pt�˺ţ�
    int  balance_locked_src;             //��Ǯ����ʣ��Ԥ�������
    int  balance_src;                    //ʣ��PT��
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //Ψһ��ʶ
    char appendix[128];                  //������Ϣ
}GSBsipConsignTransferResDef;

/*---------------------------------Ԥ����չ---------------------------------*/
typedef struct GSBsipAccountLockExReq
{
    int  app_type;                       //�������
    int  status;                         //�û�״̬
    int  couple_type;                    //�������
    int  fee;
    int  lock_period;                    //lock���ʱ��
    int  pay_type1;                      //��������
    int  amount1;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type2;                      //��������
    int  amount2;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type3;                      //��������
    int  amount3;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type4;                      //��������
    int  amount4;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type5;                      //��������
    int  amount5;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
    int  item_num;                       //��Ч����������
    GSBsipGoodsDef goods_info[1];        //�䳤���飬Ϊ���ṩһ�������ĵ�ַ�ռ���׵�ַ
                                         //�� goods_info �Ľṹ  �ֽ���=
                                         //item_num * sizeof(GSBsipGoodsDef)
}GSBsipAccountLockExReqDef;

typedef struct GSBsipAccountLockExRes
{
    int  result;                         //���ؽ��
    int  pay_type1;                      //��������
    int  balance1;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type2;                      //��������
    int  balance2;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type3;                      //��������
    int  balance3;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type4;                      //��������
    int  balance4;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type5;                      //��������
    int  balance5;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char award_num[MAX_AWARD_LEN+1];     //�콱����
    char appendix[128];                  //������Ϣ
}GSBsipAccountLockExResDef;

/*---------------------------------�ⶳ��չ---------------------------------*/
typedef struct GSBsipAccountUnlockExReq
{
    int  app_type;                       //�������
    int  status;                         //�û�״̬
    int  couple_type;                    //�������
    int  confirm;                        //ȷ�Ͽ۳�
    int  pay_type1;                      //��������
    int  amount1;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type2;                      //��������
    int  amount2;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type3;                      //��������
    int  amount3;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type4;                      //��������
    int  amount4;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type5;                      //��������
    int  amount5;                        //ʣ��ĵ���������������(��Ӧpay_type)
    int  responsable;                    //�Ƿ���Ҫ��Ӧ��־
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
    int  item_num;                       //��Ч����������
    GSBsipGoodsDef goods_info[1];        //�䳤���飬Ϊ���ṩһ�������ĵ�ַ�ռ���׵�ַ
                                         //�� goods_info �Ľṹ  �ֽ���=
                                         //item_num * sizeof(GSBsipGoodsDef)
}GSBsipAccountUnlockExReqDef;

typedef struct GSBsipAccountUnlockExRes
{
    int  result;                         //���ؽ��
    int  pay_type1;                      //��������
    int  balance1;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type2;                      //��������
    int  balance2;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type3;                      //��������
    int  balance3;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type4;                      //��������
    int  balance4;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  pay_type5;                      //��������
    int  balance5;                       //ʣ��ĵ���������������(��Ӧpay_type)
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char appendix[128];                  //������Ϣ
}GSBsipAccountUnlockExResDef;

/*---------------------------------�콱---------------------------------*/
/*-------------------------------�콱��֤-------------------------------*/
typedef struct GSBsipAwardAuthenReq
{
    int  user_type;                      //�û�����
    int  couple_type;                    //�������
    int  award_type;                     //�콱����
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char award_num[MAX_AWARD_LEN+1];     //�콱����
    char award_patchid[MAX_AWARDPATCH_LEN+1]; //�콱���κ�
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
    char role_name[32+1];                //��ҵ�ǰ��ɫ��
    int  current_level;                  //��ҵ�ǰ��ɫ����
    int  award_level;                    //����콱����
    char ind_msg[128];                   //������Ϣ������
}GSBsipAwardAuthenReqDef;

typedef struct GSBsipAwardAuthenRes
{
    int  result;                         //���ؽ��
    int  couple_type;                    //�������
    int  bind_user;                      //��û����
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //��Ϸ�ͻ��˵�IP��ַ
    char award_num[MAX_AWARD_LEN+1];     //�콱����
    char appendix[128];                  //������Ϣ
    int  item_num;                       //��Ч����������
    GSBsipItemDef item_info[1];          //�䳤���飬Ϊ���ṩһ�������ĵ�ַ�ռ���׵�ַ
                                         //�� item_info �Ľṹ  �ֽ���=
                                         //item_num * sizeof(GSBsipItemDef)
}GSBsipAwardAuthenResDef;

/*-------------------------------�콱ȷ��-------------------------------*/
typedef struct GSBsipAwardAck
{
    int  confirm;                        //
    int  award_type;                     //�콱����
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char award_num[MAX_AWARD_LEN+1];     //�콱����
    char award_time[MAX_TIME_LEN+1];     //�����ɹ���ʧ��ʱ��
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
}GSBsipAwardAckDef;

typedef struct GSBsipAwardAckRes
{
    int  result;                         //���ؽ��
    int  couple_type;                    //�������
    int  award_type;                     //��Ʒ����
    int  pay_type;                       //�콱�ĳ�ֵ����
    int  balance;                        //��ֵ���û����
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    char appendix[128];                  //������Ϣ
}GSBsipAwardAckResDef;

#if BSIP_VERSION < 0X00010600

/*-------------------------------���۽���-------------------------------*/
/*-------------------------------����Ԥ��-------------------------------*/
typedef struct GSBsipConsignLockReq
{
    int  area_id;                        //�����
    int  group_id;                       //����
    int  pay_type;                       //��������
    int  app_type;                       //�������
    int  status;                         //�û�״̬
    int  couple_type;                    //�������
    int  lock_period;                    //lock���ʱ��
    int  discount;                       //�Ʒ��ۿ���
    int  item_num;                       //��Ч����������
    int  item1_id;                       //��Ϸ���߱��
    int  item1_num;                      //��Ϸ��������
    int  item1_price;                    //��Ϸ���ߵ���
    int  item2_id;                       //��Ϸ���߱��
    int  item2_num;                      //��Ϸ��������
    int  item2_price;                    //��Ϸ���ߵ���
    int  item3_id;                       //��Ϸ���߱��
    int  item3_num;                      //��Ϸ��������
    int  item3_price;                    //��Ϸ���ߵ���
    int  item4_id;                       //��Ϸ���߱��
    int  item4_num;                      //��Ϸ��������
    int  item4_price;                    //��Ϸ���ߵ���
    int  item5_id;                       //��Ϸ���߱��
    int  item5_num;                      //��Ϸ��������
    int  item5_price;                    //��Ϸ���ߵ���
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
    char log_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
}GSBsipConsignLockReqDef;

typedef struct GSBsipConsignLockRes
{
    int  result;                         //���ؽ��
    int  area_id;                        //�����
    int  group_id;                       //����
    int  pay_type;                       //��������
    int  balance;                        //ʣ���PT��
    int  item_num;                       //��Ч����������
    int  item1_id;                       //��Ϸ���߱��
    int  item1_num;                      //��Ϸ��������
    int  item1_price;                    //��Ϸ���ߵ���
    int  item2_id;                       //��Ϸ���߱��
    int  item2_num;                      //��Ϸ��������
    int  item2_price;                    //��Ϸ���ߵ���
    int  item3_id;                       //��Ϸ���߱��
    int  item3_num;                      //��Ϸ��������
    int  item3_price;                    //��Ϸ���ߵ���
    int  item4_id;                       //��Ϸ���߱��
    int  item4_num;                      //��Ϸ��������
    int  item4_price;                    //��Ϸ���ߵ���
    int  item5_id;                       //��Ϸ���߱��
    int  item5_num;                      //��Ϸ��������
    int  item5_price;                    //��Ϸ���ߵ���
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
}GSBsipConsignLockResDef;

/*-------------------------------���۽ⶳ-------------------------------*/
typedef struct GSBsipConsignUnlockReq
{
    int  area_id;                        //�����
    int  group_id;                       //����
    int  pay_type;                       //��������
    int  app_type;                       //�������
    int  status;                         //�û�״̬
    int  couple_type;                    //�������
    int  discount;                       //�Ʒ��ۿ���
    int  confirm;                        //ȷ�Ͽ۳�
    int  responsable;                    //�Ƿ���Ҫ��Ӧ��־
    int  item_num;                       //��Ч����������
    int  item1_id;                       //��Ϸ���߱��
    int  item1_num;                      //��Ϸ��������
    int  item1_price;                    //��Ϸ���ߵ���
    int  item2_id;                       //��Ϸ���߱��
    int  item2_num;                      //��Ϸ��������
    int  item2_price;                    //��Ϸ���ߵ���
    int  item3_id;                       //��Ϸ���߱��
    int  item3_num;                      //��Ϸ��������
    int  item3_price;                    //��Ϸ���ߵ���
    int  item4_id;                       //��Ϸ���߱��
    int  item4_num;                      //��Ϸ��������
    int  item4_price;                    //��Ϸ���ߵ���
    int  item5_id;                       //��Ϸ���߱��
    int  item5_num;                      //��Ϸ��������
    int  item5_price;                    //��Ϸ���ߵ���
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
    char log_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
}GSBsipConsignUnlockReqDef;

typedef struct GSBsipConsignUnlockRes
{
    int  result;                         //���ؽ��
    int  area_id;                        //�����
    int  group_id;                       //����
    int  pay_type;                       //��������
    int  balance;                        //ʣ���PT��
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
}GSBsipConsignUnlockResDef;

/*-------------------------------���۳�ֵ-------------------------------*/
typedef struct GSBsipConsignDepositReq
{
    int  area_id;                        //�����
    int  group_id;                       //����
    int  user_type;                      //�û�����
    int  pay_type;                       //���ѵķ�ʽ
    int  amount;                         //Ԫ�����
    int  buyer_area_id;                  //��Ϸ�����
    int  buyer_group_id;                 //��ʾ��id
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    int  buyer_uid_type;                 //����ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char buyer_user_id[MAX_PTID_LEN+1];  //����˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //Ψһ��ʶ
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char patch_id[MAX_PATCHID_LEN+1];    //���κ�
    char ip[IP_ADDRESS_LEN+1];           //ip��Ϸ�ͻ��˵�ַ
    char log_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
}GSBsipConsignDepositReqDef;

typedef struct GSBsipConsignDepositRes
{
    int  result;                         //���ؽ��
    int  area_id;                        //�����
    int  group_id;                       //����
    int  buyer_area_id;                  //��Ϸ�����
    int  buyer_group_id;                 //��ʾ��id
    int  pay_type;                       //���ѵķ�ʽ
    int  balance;                        //ʣ��PT��
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    int  buyer_uid_type;                 //����ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char buyer_user_id[MAX_PTID_LEN+1];  //����˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //Ψһ��ʶ
    char context_id[MAX_ORDERID_LEN+1];  //����id
    char patch_id[MAX_PATCHID_LEN+1];    //���κ�
    char ip[IP_ADDRESS_LEN+1];           //ip��Ϸ�ͻ��˵�ַ
}GSBsipConsignDepositResDef;
#endif // BSIP_VERSION  < 0X00010600

#if BSIP_VERSION < 0X00010400
/*-------------------------------Ԫ������-------------------------------*/
/*-------------------------------Ԫ����ֵ-------------------------------*/
typedef struct GSBsipGoldDepositReq
{
    int  user_type;                      //�û�����
    int  pay_type;                       //��������
    int  amount;                         //��ֵ���
    int  sync_flag;                      //��ֵ��Ϣ��־λ
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
}GSBsipGoldDepositReqDef;

typedef struct GSBsipGoldDepositRes
{
    int  result;                         //���ؽ��
    int  pay_type;                       //��������
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
}GSBsipGoldDepositResDef;
#endif // BSIP_VERSION  < 0X00010400

/*����ȯ����*/
typedef struct GSBsipTokenDepositReq
{
    int  pay_type;                       //��������
    int  amount;                         //��ֵ���
    int  item_id;                        //�������յĵ��߱��
    int  item_num;                       //�������յĵ�������
    int  item_price;                     //�������յĵ��ߵ���
    int  deposit_flag;                   //��ֵ��Ϣ��־λ 1-���򷵻� 2-���߻��� ����-����
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //order_id�����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char patch_id[MAX_PATCHID_LEN+1];    //���κ�
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
    int  valid_period;                   //��ֵ������Ч�ڣ����¡�
}GSBsipTokenDepositReqDef;

typedef struct GSBsipTokenDepositRes
{
    int  result;                         //���ؽ��
    int  pay_type;                       //��������
    int  balance;                        //ʣ��ĵ���ȯ
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //order_id�����id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char appendix[128];                  //������Ϣ
}GSBsipTokenDepositResDef;

/*-------------------------------Ԫ����ʱ��-------------------------------*/
typedef struct GSBsipGoldConsumeLockReq
{
    int  couple_type;                    //�������
    int  discount;                       //�Ʒ��ۿ���
    int  src_pay_type;                   //��������
    int  src_amount;                     //��������
    int  dest_pay_type;                  //�����ʱ������
    int  dest_amount;                    //�һ���ʱ������
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //���ݰ�id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
}GSBsipGoldConsumeLockReqDef;

typedef struct GSBsipGoldConsumeLockRes
{
    int  result;                         //���ؽ��
    int  src_pay_type;                   //��������
    int  src_balance;                    //�һ����ܵĵ�ȯ����
    int  dest_pay_type;                  //�����ʱ������
    int  dest_balance;                   //�һ����ܵ�ʱ������
    int  dest_amount;                    //�һ���ʱ������
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //���ݰ�id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    char begin_time[MAX_TIME_LEN+1];     //��Чʱ�� ��time_type Ϊ1 ʱ��Ч��
    char end_time[MAX_TIME_LEN+1];       //ʧЧʱ�� ��time_type Ϊ1 ʱ��Ч��
    char appendix[128];                  //������Ϣ
}GSBsipGoldConsumeLockResDef;

typedef struct GSBsipGoldConsumeUnlockReq
{
    int  confirm;                        //ȷ�Ͽ۳�
    int  couple_type;                    //�������
    int  discount;                       //�Ʒ��ۿ���
    int  src_pay_type;                   //��������
    int  src_amount;                     //��������
    int  dest_pay_type;                  //�����ʱ������
    int  dest_amount;                    //�һ���ʱ������
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //���ݰ�id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
}GSBsipGoldConsumeUnlockReqDef;

typedef struct GSBsipGoldConsumeUnlockRes
{
    int  result;                         //���ؽ��
    int  src_pay_type;                   //��������
    int  src_balance;                    //�һ����ܵĵ�ȯ����
    int  dest_pay_type;                  //�����ʱ������
    int  dest_balance;                   //�һ����ܵ�ʱ������
    int  dest_amount;                    //�һ���ʱ������
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //������id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    char begin_time[MAX_TIME_LEN+1];     //��Чʱ�� ��time_type Ϊ1 ʱ��Ч��
    char end_time[MAX_TIME_LEN+1];       //ʧЧʱ�� ��time_type Ϊ1 ʱ��Ч��
    char appendix[128];                  //������Ϣ
}GSBsipGoldConsumeUnlockResDef;

/*-------------------------------���Ҷһ�-------------------------------*/
typedef struct GSBsipAccountExchangeLockReq
{
    int  couple_type;                    //�������
    int  discount;                       //�Ʒ��ۿ���
    int  src_pay_type;                   //��������
    int  src_amount;                     //��������
    int  dest_pay_type;                  //�����ʱ������
    int  dest_amount;                    //�һ���ʱ������
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //���ݰ�id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
    int  rate_decision_lock;             //1 ת��Ԥ��(�����ɺ�̨����) 
                                         //2 ֱ��ת��(�����ɺ�̨����) 
                                         //3 ת��Ԥ��(������ǰ�˼���) 
                                         //4 ֱ��ת��(������ǰ�˼���)
}GSBsipAccountExchangeLockReqDef;

typedef struct GSBsipAccountExchangeLockRes
{
    int  result;                         //���ؽ��
    int  src_pay_type;                   //��������
    int  src_balance;                    //�һ����ܵĵ�ȯ����
    int  dest_pay_type;                  //�����ʱ������
    int  dest_balance;                   //�һ����ܵ�ʱ������
    int  dest_amount;                    //�һ���ʱ������
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //���ݰ�id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    char begin_time[MAX_TIME_LEN+1];     //��Чʱ�� ��time_type Ϊ1 ʱ��Ч��
    char end_time[MAX_TIME_LEN+1];       //ʧЧʱ�� ��time_type Ϊ1 ʱ��Ч��
    char appendix[128];                  //������Ϣ
}GSBsipAccountExchangeLockResDef;

typedef struct GSBsipAccountExchangeUnlockReq
{
    int  confirm;                        //ȷ�Ͽ۳�
    int  couple_type;                    //�������
    int  discount;                       //�Ʒ��ۿ���
    int  src_pay_type;                   //��������
    int  src_amount;                     //��������
    int  dest_pay_type;                  //�����ʱ������
    int  dest_amount;                    //�һ���ʱ������
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //���ݰ�id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
    int  rate_decision_unlock;           //1 ת��Ԥ��(�����ɺ�̨����) 
                                         //2 �ⶳ(������ǰ�˶�)
}GSBsipAccountExchangeUnlockReqDef;

typedef struct GSBsipAccountExchangeUnlockRes
{
    int  result;                         //���ؽ��
    int  src_pay_type;                   //��������
    int  src_balance;                    //�һ����ܵĵ�ȯ����
    int  dest_pay_type;                  //�����ʱ������
    int  dest_balance;                   //�һ����ܵ�ʱ������
    int  dest_amount;                    //�һ���ʱ������
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char context_id[MAX_ORDERID_LEN+1];  //������id
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    char begin_time[MAX_TIME_LEN+1];     //��Чʱ�� ��time_type Ϊ1 ʱ��Ч��
    char end_time[MAX_TIME_LEN+1];       //ʧЧʱ�� ��time_type Ϊ1 ʱ��Ч��
    char appendix[128];                  //������Ϣ
}GSBsipAccountExchangeUnlockResDef;

/*-------------------------------����ϵͳ-------------------------------*/
typedef struct GSBsipItemSyncReq
{
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char order_id[MAX_ORDERID_LEN+1];    //Ψһ��ʶ
    int  sync_times;                     //ͬ������
    int  indication_sync;                //ָ����Ϣ Ԥ��
    char msg_sync[256];                  //ͬ����Ϣ
    char item_info[512];                 //������Ϣ����ʽΪ��
                                         //���߱��1,����1;���߱��2,����2;���߱��3,����3;����
                                         //��ͬ���߼�ʹ�ð�Ƿֺŷָ���
                                         //���߱�ź�����֮���ð�Ƕ��ŷָ�
}GSBsipItemSyncReqDef;

typedef struct GSBsipItemSyncRes
{
    int  result;                         //ͬ�������0�ɹ�������ʧ��
    char order_id[MAX_ORDERID_LEN+1];    //Ψһ��ʶ����������Ϣ���ֶ�һ��
}GSBsipItemSyncResDef;

/*-------------------------------������־�ɼ��ṹ-------------------------------*/
typedef struct GSBsipItemLogMsg
{
    int  serv_id_player;				//���ߵķ����
    int  area_id_player;				//���ߵ�����
    int  group_id_player;				//���ߵ����
    char user_id[MAX_PTID_LEN+1];		//�û��˻�
    char role_name[32+1];               //��ҵ�ǰ��ɫ��
    int  item_flag;						//���߳����־
    char context_id[MAX_ORDERID_LEN+1]; //����id
    char item_uniqueid[32+1];			//��������Ϸ�е�Ψһ���
    int  item_id;                       //��Ϸ���߱��
    int  item_num;                      //��Ϸ��������
    int  item_amount;                   //����Ϸ���ߵ��ܼ�
    char reserve[128];                  //�����ֶ������պ�����µ���Ϣ
                                        //��ӵ���Ϣ���ж��������','�ָ���û��ʱΪ��
}GSBsipItemLogMsgDef;

/*------------------------------�ܱ���---------------------------------*/
typedef struct GSBsipEkeyBindReq
{
    int  operation_type;                 //1��ʾ�ܱ�����������
    int  indication_bind;                //1�����ܱ���2��������ܱ�
    int  pay_type;
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    int  serv_id_player;
    int  area_id_player;
    int  group_id_player;
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];  //�ն�IP��ַ
    int  platform_type;
    char call_time[MAX_TIME_LEN+1];      //���ݰ�����ʱ��
}GSBsipEkeyBindReqDef;

typedef struct GSBsipEkeyBindRes
{
    int  result;                         //���ؽ��
    int  uid_type;                       //�ʺ����ͣ�1��pt�ʻ���2������id
    char user_id[MAX_PTID_LEN+1];        //�˺�id�������˺Ż�pt�˺ţ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
}GSBsipEkeyBindResDef;

//--------------------GSBsipNotifyS2CReqDef--------------------
typedef struct GSBsipNotifyS2CReq   //cbs������Ϣ
{
    int  serv_id_site;
    int  area_id_site;
    int  group_id_site;
    int  serv_id_src;         // ��Ϣ��Դserviceid
    int  area_id_src;         // ��Ϣ��Դareaid
    int  group_id_src;        // ��Ϣ��Դgroupid
    char order_id[MAX_ORDERID_LEN+1];
    int  req_times;           // ���ʹ��������ڿͻ���У���Ƿ��ظ�
    int  indication_s2c;   	  //��Ϣָʾ,ȱʡΪ0
    int  req_len;             // req_info�ĳ���
    char req_info[1];         // ���512�ֽ�
}GSBsipNotifyS2CReqDef;       // cbs->gs

typedef struct GSBsipNotifyS2CRes
{
    int  result;
    char order_id[MAX_ORDERID_LEN+1];
}GSBsipNotifyS2CResDef;     // gs->cbs

//------------------GSBsipNotifyC2SReqDef----------------------
typedef struct GSBsipNotifyC2SReq  //gs������Ϣ
{
    int  serv_id_site;      //�����ߵ�Ӧ��id�ţ���ͳһ���ֱ���
    int  area_id_site;      //�����ߵ���id�ţ���ͳһ���ֱ���
    int  group_id_site;     //�����ߵ���id�ţ���ͳһ���ֱ���
    int  serv_id_dest;        // ��Ϣ����Ŀ��
    int  area_id_dest;        // ��Ϣ����Ŀ��
    int  group_id_dest;       // ��Ϣ����Ŀ��
    char order_id[MAX_ORDERID_LEN+1];//���ݰ�ID
    char order_id_relation[MAX_ORDERID_LEN+1];//g���ݰ�ID
    int  indication_c2s;   	  //��Ϣָʾ,1:�û��콱,2:������Ʒ���͵�GS
    int  interface_id;        //�ӿ�ID��ʶ�ɵ���ϵͳָ��
    int  endpoint_type;
    char endpoint_ip[IP_ADDRESS_LEN+1];
    int  platform_type;
    int  req_len;             // req_info�ĳ���
    char req_info[1];         // ���512�ֽ�
}GSBsipNotifyC2SReqDef;    // gs->cbs

typedef struct GSBsipNotifyC2SRes
{
    int  result;
    char order_id[MAX_ORDERID_LEN+1];
}GSBsipNotifyC2SResDef;       // cbs->gs

#endif
