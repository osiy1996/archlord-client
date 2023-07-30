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
*				\ConsignDeposit\ConsignUnlock��Ϣ�е�item_id���ֶ�
* �޸�ʱ�䣺2005��11��25��
*
* �޸����ݣ����AccountLock\AccountUnlock\CosignLock
*				\ConsignDeposit\ConsignUnlock��Ϣ�е�item_num��item_price���ֶ�
* �޸�ʱ�䣺2005��11��28��
*/

/*
*bsip msg body ,
*/
#ifndef _BSIP_MSG_H_
#define _BSIP_MSG_H_

#include "bsipmacro.h"

/*------------------------------��Ϣ�嶨��------------------------------*/
/*-------------------------------ʱ���Ʒ�-------------------------------*/
/*---------------------------------��Ȩ---------------------------------*/
typedef struct GSBsipAuthorReq
{
	char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	char 		sess_id[MAX_SESSID_LEN+1];		//�Ựid                
	char		user_type;						//�û�����   
	char		status;							//�û�״̬
	char		unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id              
	char  		ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ
	int			reserve_int;					//�����ֶΣ����ã�
	char		reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�
}GSBsipAuthorReqDef;

typedef struct GSBsipAuthorRes
{
	int			result; 						//���ؽ��                               
	char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�                         
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�                       
	char 		sess_id[MAX_SESSID_LEN+1];		//�Ựid                                 
	char  		pay_type;						//��������                               
	char		unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id                               
	int			balance;						//ʣ��ĵ�������������������Ӧpay_type�� 
	char  		begin_time[MAX_TIME_LEN+1];		//��Чʱ�䣨�����ֶΣ�                   
	char  		end_time[MAX_TIME_LEN+1];		//ʧЧʱ�䣨�����ֶΣ�                   
	char  		ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ 
	int			reserve_int;					//�����ֶΣ����ã�
	char		reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�                    
}GSBsipAuthorResDef;

/*---------------------------------�۷�---------------------------------*/
typedef struct GSBsipAccountReq
{
	char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�        
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	char 		sess_id[MAX_SESSID_LEN+1];		//�Ựid                
	char  		pay_type;						//��������              
	char  		app_type;     					//�������              
	char 		status;							//�û�״̬              
	char		unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id              
	char 		couple_type;					//�������              
	int			amount;							//��������              
	int			discount;  						//�Ʒ��ۿ���                                       
	int 		item_id;						//��Ϸ���߱��          
	char  		begin_time[MAX_TIME_LEN+1];		//����ʱ��              
	char  		end_time[MAX_TIME_LEN+1];		//����ʱ��     
	char		log_time[MAX_TIME_LEN+1];		//����ʱ��         
	char  		ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ    
	char		responsable;					//�Ƿ���Ҫ��Ӧ��־ 
	int         map_id;							//��ͼid            
	int			reserve_int;					//�����ֶΣ����ã�
	char		reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�           
}GSBsipAccountReqDef;             
                                                                                
typedef struct GSBsipAccountRes
{
	int			result;							//���ؽ��                               
	char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�                         
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�                       
	char 		sess_id[MAX_SESSID_LEN+1];		//�Ựid                                 
	char  		pay_type;						//��������                               
	char		unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id                               
	int			balance;					    //ʣ��ĵ�������������������Ӧpay_type�� 
	int			pt_balance;						//ʣ���PT��                             
	char  		ip[IP_ADDRESS_LEN+1];		    //��Ϸ�ͻ��˵�IP��ַ 
	int         map_id;							//��ͼid                           
	int			reserve_int;					//�����ֶΣ����ã�
	char		reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�                             
}GSBsipAccountResDef;

/*-------------------------------���μƷ�-------------------------------*/
/*-------------------------------�˻���֤-------------------------------*/
typedef struct GSBsipAccountAuthenReq
{
	char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	char 		sess_id[MAX_SESSID_LEN+1];		//�Ựid                
	char		user_type;						//�û�����              
	char		unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id              
	char  		ip[IP_ADDRESS_LEN+1];				//��Ϸ�ͻ��˵�IP��ַ
	int			reserve_int;					//�����ֶΣ����ã�
	char		reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�
}GSBsipAccountAuthenReqDef;

typedef struct GSBsipAccountAuthenRes
{
	int			result; 						//���ؽ��                               
	char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�                         
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�                       
	char 		sess_id[MAX_SESSID_LEN+1];		//�Ựid                                 
	char		unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id                               
	char  		pay_type1;						//��������                               	
	int			balance1;						//ʣ��ĵ�������������������Ӧpay_type�� 	
	char  		pay_type2;						//��������                               	
	int			balance2;						//ʣ��ĵ�������������������Ӧpay_type�� 	
	char  		pay_type3;						//��������                               	
	int			balance3;						//ʣ��ĵ�������������������Ӧpay_type�� 	
	char  		pay_type4;						//��������                               	
	int			balance4;						//ʣ��ĵ�������������������Ӧpay_type�� 	
	char  		pay_type5;						//��������                               	
	int			balance5;						//ʣ��ĵ�������������������Ӧpay_type�� 	
	char  		begin_time[MAX_TIME_LEN+1];		//��Чʱ�䣨�����ֶΣ�                   
	char  		end_time[MAX_TIME_LEN+1];		//ʧЧʱ�䣨�����ֶΣ�                   
	char  		ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ 
	int			reserve_int;					//�����ֶΣ����ã�
	char		reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�                    
}GSBsipAccountAuthenResDef;



typedef struct GSBsipAccountLockReq
{
	char			sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�        
	char			pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	char 			sess_id[MAX_SESSID_LEN+1];		//�Ựid                
	char  			pay_type;						//��������              
	char  			app_type;     					//�������              
	char			status;							//�û�״̬              
	char 			couple_type;					//�������
	unsigned short	item_num;						//��Ч����������              
	int				discount;  						//�Ʒ��ۿ���                     
	char  			order_id[MAX_ORDERID_LEN+1];	//���ݰ�id              
	char  			ip[IP_ADDRESS_LEN+1];		    //��Ϸ�ͻ��˵�IP��ַ    
	int				lock_period;					//lock���ʱ�� 
	char  			log_time[MAX_TIME_LEN+1];		//���ݰ�����ʱ��
	int 			item0_id;						//��Ϸ���߱�� 
	unsigned short	item0_num;						//��Ϸ��������    
	int				item0_price;					//��Ϸ���ߵ���
	int 			item1_id;						//��Ϸ���߱�� 
	unsigned short	item1_num;						//��Ϸ��������  
	int				item1_price;					//��Ϸ���ߵ���
	int 			item2_id;						//��Ϸ���߱�� 
	unsigned short	item2_num;						//��Ϸ�������� 
	int				item2_price;					//��Ϸ���ߵ��� 
	int 			item3_id;						//��Ϸ���߱�� 
	unsigned short	item3_num;						//��Ϸ��������
	int				item3_price;					//��Ϸ���ߵ���  
	int 			item4_id;						//��Ϸ���߱�� 
	unsigned short	item4_num;						//��Ϸ��������  
	int				item4_price;					//��Ϸ���ߵ���
}GSBsipAccountLockReqDef;     


                                                                                                                                 
typedef struct GSBsipAccountLockRes
{
	int				result;							//���ؽ��          
	char			sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�    
	char			pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�  
	char 			sess_id[MAX_SESSID_LEN+1];		//�Ựid            
	char  			pay_type;						//��������          
	int				pt_balance;						//ʣ���PT�� 
	unsigned short	item_num;						//��Ч����������
	char  			order_id[MAX_ORDERID_LEN+1];	//���ݰ�id          
	char  			ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ
	char  		log_time[MAX_TIME_LEN+1];			//���ݰ�����ʱ��
	int 			item0_id;						//��Ϸ���߱�� 
	unsigned short	item0_num;						//��Ϸ��������    
	int				item0_price;					//��Ϸ���ߵ���
	int 			item1_id;						//��Ϸ���߱�� 
	unsigned short	item1_num;						//��Ϸ��������  
	int				item1_price;					//��Ϸ���ߵ���
	int 			item2_id;						//��Ϸ���߱�� 
	unsigned short	item2_num;						//��Ϸ�������� 
	int				item2_price;					//��Ϸ���ߵ��� 
	int 			item3_id;						//��Ϸ���߱�� 
	unsigned short	item3_num;						//��Ϸ��������
	int				item3_price;					//��Ϸ���ߵ���  
	int 			item4_id;						//��Ϸ���߱�� 
	unsigned short	item4_num;						//��Ϸ��������  
	int				item4_price;					//��Ϸ���ߵ���
}GSBsipAccountLockResDef;

/*---------------------------------�ⶳ---------------------------------*/
typedef struct GSBsipAccountUnlockReq
{
	char			sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�        
	char			pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	char 			confirm;						//ȷ�Ͽ۳�              
	char 			sess_id[MAX_SESSID_LEN+1];		//�Ựid                
	char  			pay_type;						//��������              
	char  			app_type;     					//�������              
	char 			status;							//�û�״̬              
	char 			couple_type;					//�������              
	unsigned short	item_num;						//��Ч����������           
	int				discount;  						//�Ʒ��ۿ���            
	char  			order_id[MAX_ORDERID_LEN+1];	//���ݰ�id                     
	char  			ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ    
	char			responsable;					//�Ƿ���Ҫ��Ӧ��־ 
	char  			log_time[MAX_TIME_LEN+1];		//���ݰ�����ʱ��
	int 			item0_id;						//��Ϸ���߱�� 
	unsigned short	item0_num;						//��Ϸ��������    
	int				item0_price;					//��Ϸ���ߵ���
	int 			item1_id;						//��Ϸ���߱�� 
	unsigned short	item1_num;						//��Ϸ��������  
	int				item1_price;					//��Ϸ���ߵ���
	int 			item2_id;						//��Ϸ���߱�� 
	unsigned short	item2_num;						//��Ϸ�������� 
	int				item2_price;					//��Ϸ���ߵ��� 
	int 			item3_id;						//��Ϸ���߱�� 
	unsigned short	item3_num;						//��Ϸ��������
	int				item3_price;					//��Ϸ���ߵ���  
	int 			item4_id;						//��Ϸ���߱�� 
	unsigned short	item4_num;						//��Ϸ��������  
	int				item4_price;					//��Ϸ���ߵ���     
}GSBsipAccountUnlockReqDef;      
                             
                                                                                                                               
typedef struct GSBsipAccountUnlockRes                             
{                                                                 
	int			result;							//���ؽ��          
	char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�    
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�  
	char 		sess_id[MAX_SESSID_LEN+1];		//�Ựid            
	char  		pay_type;						//��������          
	int			pt_balance;						//ʣ���PT��        
	char  		order_id[MAX_ORDERID_LEN+1];	//���ݰ�id          
	char  		ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ
}GSBsipAccountUnlockResDef;


/*---------------------------------Ԥ����չ---------------------------------*/
typedef struct GSBsipAccountLockExReq
{    
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺ�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char pay_type1;                      //��������
    char pay_type2;                      //��������
    char pay_type3;                      //��������
    char pay_type4;                      //��������
    char pay_type5;                      //��������
    int	 balance1;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance2;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance3;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance4;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance5;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
    char app_type;                       //�������
    char status;                         //�û�״̬
    char couple_type;                    //�������
    unsigned short item_num;             //��Ч����������
    int  discount;                       //�Ʒ��ۿ���
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id 
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
    int  lock_period;                    //lock���ʱ��
    char log_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
    int  item0_id;                       //��Ϸ���߱��
    int  item1_id;                       //��Ϸ���߱��
    int  item2_id;                       //��Ϸ���߱��
    int  item3_id;                       //��Ϸ���߱��
    int  item4_id;                       //��Ϸ���߱��
    unsigned short item0_num;            //��Ϸ��������
    unsigned short item1_num;            //��Ϸ��������
    unsigned short item2_num;            //��Ϸ��������
    unsigned short item3_num;            //��Ϸ��������
    unsigned short item4_num;            //��Ϸ��������
    int  item0_price;                    //��Ϸ���ߵ���
    int  item1_price;                    //��Ϸ���ߵ���
    int  item2_price;                    //��Ϸ���ߵ���
    int  item3_price;                    //��Ϸ���ߵ���
    int  item4_price;                    //��Ϸ���ߵ���
}GSBsipAccountLockExReqDef;
          
typedef struct GSBsipAccountLockExRes
{        
    int  result;                         //���ؽ��
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺ�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid    
    char pay_type1;                      //��������
    char pay_type2;                      //��������
    char pay_type3;                      //��������
    char pay_type4;                      //��������
    char pay_type5;                      //��������
    int	 balance1;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance2;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance3;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance4;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance5;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
    unsigned short item_num;             //��Ч����������
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
    int  item0_id;                       //��Ϸ���߱��
    int  item1_id;                       //��Ϸ���߱��
    int  item2_id;                       //��Ϸ���߱��
    int  item3_id;                       //��Ϸ���߱��
    int  item4_id;                       //��Ϸ���߱��
    unsigned short item0_num;            //��Ϸ��������
    unsigned short item1_num;            //��Ϸ��������
    unsigned short item2_num;            //��Ϸ��������
    unsigned short item3_num;            //��Ϸ��������
    unsigned short item4_num;            //��Ϸ��������
    int  item0_price;                    //��Ϸ���ߵ���
    int  item1_price;                    //��Ϸ���ߵ���
    int  item2_price;                    //��Ϸ���ߵ���
    int  item3_price;                    //��Ϸ���ߵ���
    int  item4_price;                    //��Ϸ���ߵ���
}GSBsipAccountLockExResDef;

/*---------------------------------�ⶳ��չ---------------------------------*/
typedef struct GSBsipAccountUnlockExReq
{    
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺ�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺ�
    char confirm;                        //ȷ�Ͽ۳�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char pay_type1;                      //��������
    char pay_type2;                      //��������
    char pay_type3;                      //��������
    char pay_type4;                      //��������
    char pay_type5;                      //��������
    int	 balance1;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance2;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance3;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance4;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance5;						 //ʣ��ĵ�������������������Ӧpay_type�� 		
    char app_type;                       //�������
    char status;                         //�û�״̬
    char couple_type;                    //�������
    unsigned short item_num;             //��Ч����������
    int  discount;                       //�Ʒ��ۿ���
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id   
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
    char responsable;                    //�Ƿ���Ҫ��Ӧ��־
    char log_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
    int  item0_id;                       //��Ϸ���߱��
    int  item1_id;                       //��Ϸ���߱��
    int  item2_id;                       //��Ϸ���߱��
    int  item3_id;                       //��Ϸ���߱��
    int  item4_id;                       //��Ϸ���߱��
    unsigned short item0_num;            //��Ϸ��������
    unsigned short item1_num;            //��Ϸ��������
    unsigned short item2_num;            //��Ϸ��������
    unsigned short item3_num;            //��Ϸ��������
    unsigned short item4_num;            //��Ϸ��������
    int  item0_price;                    //��Ϸ���ߵ���
    int  item1_price;                    //��Ϸ���ߵ���
    int  item2_price;                    //��Ϸ���ߵ���
    int  item3_price;                    //��Ϸ���ߵ���
    int  item4_price;                    //��Ϸ���ߵ���
}GSBsipAccountUnlockExReqDef;

typedef struct GSBsipAccountUnlockExRes
{
    int  result;                         //���ؽ��
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺ�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid    
    char pay_type1;                      //��������
    char pay_type2;                      //��������
    char pay_type3;                      //��������
    char pay_type4;                      //��������
    char pay_type5;                      //��������
    int	 balance1;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance2;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance3;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance4;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
	int	 balance5;						 //ʣ��ĵ�������������������Ӧpay_type�� 	
    unsigned short item_num;             //��Ч����������
    char order_id[MAX_ORDERID_LEN+1];    //���ݰ�id 
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
}GSBsipAccountUnlockExResDef;


/*---------------------------------�콱---------------------------------*/
/*-------------------------------�콱��֤-------------------------------*/
typedef struct GSBsipAwardAuthenReq
{
	char		sd_id[MAX_SDID_LEN+1];					//�û��������ʺ�        
	char		pt_id[MAX_PTID_LEN+1];					//�û���PT�ַ��ʺ�      
	char 		sess_id[MAX_SESSID_LEN+1];				//�Ựid                
	char		user_type;								//�û�����              
	char 		couple_type;							//�������              
	char		unique_id[MAX_ORDERID_LEN+1];			//���ݰ�id              
	char  		ip[IP_ADDRESS_LEN+1];					//��Ϸ�ͻ��˵�IP��ַ    
	int			award_type;								//�콱����              
	char		award_num[MAX_AWARD_LEN+1];				//�콱����
	char		award_patchid[MAX_AWARDPATCH_LEN+1]; 	//�콱���κ� 
	int			reserve_int;					//�����ֶΣ����ã�
	char		reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�            
}GSBsipAwardAuthenReqDef;

typedef struct GSBsipAwardAuthenRes
{
	int  			result;							//���ؽ��          
	char			sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�    
	char			pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�  
	char 			sess_id[MAX_SESSID_LEN+1];  	//�Ựid            
	char 			couple_type;					//�������          
	char			unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id          
	char  			ip[IP_ADDRESS_LEN+1];       	//��Ϸ�ͻ��˵�IP��ַ
	char			award_num[MAX_AWARD_LEN+1];		//�콱����          
	char			award_sn[MAX_AWARDSN_LEN+1];	//�콱���к�        
	char			bind_user;						//��û����      
	int				award_type1;					//��Ʒ����          
	unsigned int	award_item_id1;					//��Ʒ���          
	unsigned int	award_item_count1;				//��Ʒ����          
	int				award_type2;					//��Ʒ����          
	unsigned int 	award_item_id2;					//��Ʒ���          
	unsigned int	award_item_count2;				//��Ʒ����          
	int				award_type3;					//��Ʒ����          
	unsigned int 	award_item_id3;					//��Ʒ���          
	unsigned int	award_item_count3;				//��Ʒ����          
	int				award_type4;					//��Ʒ����          
	unsigned int 	award_item_id4;					//��Ʒ���          
	unsigned int	award_item_count4;				//��Ʒ����          
	int				award_type5;					//��Ʒ����          
	unsigned int 	award_item_id5;					//��Ʒ���          
	unsigned int	award_item_count5;				//��Ʒ����   
	int				reserve_int;					//�����ֶΣ����ã�
	char			reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�       
}GSBsipAwardAuthenResDef;

/*-------------------------------�콱ȷ��-------------------------------*/
typedef struct GSBsipAwardAck
{
	int  			result;							//���ؽ��              
	char			sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�        
	char			pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	char 			sess_id[MAX_SESSID_LEN+1];  	//�Ựid                
	char			unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id              
	char  			ip[IP_ADDRESS_LEN+1];         	//��Ϸ�ͻ��˵�IP��ַ    
	int				award_type;						//�콱����
	char			award_num[MAX_AWARD_LEN+1];		//�콱����              
	char			award_sn[MAX_AWARDSN_LEN+1];	//�콱���к�            
	char			award_time[MAX_TIME_LEN+1];		//�����ɹ���ʧ��ʱ�� 
	int				reserve_int;					//�����ֶΣ����ã�
	char			reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�   
}GSBsipAwardAckDef;

typedef struct GSBsipAwardAckRes
{
	int  			result;							//���ؽ��              
	char			sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�        
	char			pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	char 			sess_id[MAX_SESSID_LEN+1];  	//�Ựid     
	char			couple_type;					//�������
	char			unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id              
	char  			ip[IP_ADDRESS_LEN+1];         	//��Ϸ�ͻ��˵�IP��ַ    
	int				award_type;						//��Ʒ����
	char			pay_type;						//�콱�ĳ�ֵ����
	int				balance;						//��ֵ���û����	
}GSBsipAwardAckResDef;
/*-------------------------------���۽���-------------------------------*/
/*-------------------------------����Ԥ��-------------------------------*/


typedef struct GSBsipConsignLockReq
{
	char			sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�        
	char			pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	char 			sess_id[MAX_SESSID_LEN+1];		//�Ựid                
	char  			pay_type;						//��������
	int				group_id;						//����              
	char  			app_type;     					//�������              
	char			status;							//�û�״̬              
	char 			couple_type;					//�������              
	unsigned short	item_num;						//��Ч����������
	int				discount;  						//�Ʒ��ۿ���                 
	char  			order_id[MAX_ORDERID_LEN+1];	//���ݰ�id  
	int				area_id;						//�����            
	char  			ip[IP_ADDRESS_LEN+1];		    //��Ϸ�ͻ��˵�IP��ַ    
	int				lock_period;					//lock���ʱ�� 
	char  			log_time[MAX_TIME_LEN+1];		//���ݰ�����ʱ��
	int 			item0_id;						//��Ϸ���߱�� 
	unsigned short	item0_num;						//��Ϸ��������    
	int				item0_price;					//��Ϸ���ߵ���
	int 			item1_id;						//��Ϸ���߱�� 
	unsigned short	item1_num;						//��Ϸ��������  
	int				item1_price;					//��Ϸ���ߵ���
	int 			item2_id;						//��Ϸ���߱�� 
	unsigned short	item2_num;						//��Ϸ�������� 
	int				item2_price;					//��Ϸ���ߵ��� 
	int 			item3_id;						//��Ϸ���߱�� 
	unsigned short	item3_num;						//��Ϸ��������
	int				item3_price;					//��Ϸ���ߵ���  
	int 			item4_id;						//��Ϸ���߱�� 
	unsigned short	item4_num;						//��Ϸ��������  
	int				item4_price;					//��Ϸ���ߵ���    
}GSBsipConsignLockReqDef;     
                                                                                                                                 
typedef struct GSBsipConsignLockRes
{
	int				result;							//���ؽ��          
	char			sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�    
	char			pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�  
	char 			sess_id[MAX_SESSID_LEN+1];		//�Ựid            
	char  			pay_type;						//��������  
	int				group_id;						//����        
	int				pt_balance;						//ʣ���PT��        
	unsigned short	item_num;						//��Ч����������
	char  			order_id[MAX_ORDERID_LEN+1];	//���ݰ�id   
	int				area_id;						//�����       
	char  			ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ
	int 			item0_id;						//��Ϸ���߱�� 
	unsigned short	item0_num;						//��Ϸ��������    
	int				item0_price;					//��Ϸ���ߵ���
	int 			item1_id;						//��Ϸ���߱�� 
	unsigned short	item1_num;						//��Ϸ��������  
	int				item1_price;					//��Ϸ���ߵ���
	int 			item2_id;						//��Ϸ���߱�� 
	unsigned short	item2_num;						//��Ϸ�������� 
	int				item2_price;					//��Ϸ���ߵ��� 
	int 			item3_id;						//��Ϸ���߱�� 
	unsigned short	item3_num;						//��Ϸ��������
	int				item3_price;					//��Ϸ���ߵ���  
	int 			item4_id;						//��Ϸ���߱�� 
	unsigned short	item4_num;						//��Ϸ��������  
	int				item4_price;					//��Ϸ���ߵ���    
}GSBsipConsignLockResDef;

/*-------------------------------���۽ⶳ-------------------------------*/
typedef struct GSBsipConsignUnlockReq
{
	char			sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�        
	char			pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	char 			confirm;						//ȷ�Ͽ۳�              
	char 			sess_id[MAX_SESSID_LEN+1];		//�Ựid                
	char  			pay_type;						//��������    
	int				group_id;						//����          
	char  			app_type;     					//�������              
	char 			status;							//�û�״̬              
	char 			couple_type;					//�������              
	unsigned short	item_num;						//��Ч����������
	int				discount;  						//�Ʒ��ۿ���            
	char  			order_id[MAX_ORDERID_LEN+1];	//���ݰ�id               
	int				area_id;						//�����        
	char  			ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ    
	char			responsable;					//�Ƿ���Ҫ��Ӧ��־
	char  			log_time[MAX_TIME_LEN+1];		//���ݰ�����ʱ�� 
	int 			item0_id;						//��Ϸ���߱�� 
	unsigned short	item0_num;						//��Ϸ��������    
	int				item0_price;					//��Ϸ���ߵ���
	int 			item1_id;						//��Ϸ���߱�� 
	unsigned short	item1_num;						//��Ϸ��������  
	int				item1_price;					//��Ϸ���ߵ���
	int 			item2_id;						//��Ϸ���߱�� 
	unsigned short	item2_num;						//��Ϸ�������� 
	int				item2_price;					//��Ϸ���ߵ��� 
	int 			item3_id;						//��Ϸ���߱�� 
	unsigned short	item3_num;						//��Ϸ��������
	int				item3_price;					//��Ϸ���ߵ���  
	int 			item4_id;						//��Ϸ���߱�� 
	unsigned short	item4_num;						//��Ϸ��������  
	int				item4_price;					//��Ϸ���ߵ���   
}GSBsipConsignUnlockReqDef;  

                                                                                                                               
typedef struct GSBsipConsignUnlockRes
{
	int			result;							//���ؽ��          
	char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�    
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�  
	char 		sess_id[MAX_SESSID_LEN+1];		//�Ựid            
	char  		pay_type;						//��������  
	int			group_id;						//����        
	int			pt_balance;						//ʣ���PT��        
	char  		order_id[MAX_ORDERID_LEN+1];	//���ݰ�id    
	int			area_id;						//�����      
	char  		ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ
}GSBsipConsignUnlockResDef;



/*-------------------------------���۳�ֵ-------------------------------*/
typedef struct GSBsipConsignDepositReq
{
	char    	sd_id[MAX_SDID_LEN+1];          //�û��������ʺţ�ȫΪ���֡�                                                                              
	char    	pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺţ�����ȫΪ����                                                                          
	char    	sess_id[MAX_SESSID_LEN+1];      //�Ựid
	int			amount;							//Ԫ�����                                                                                                
	char    	pay_type;                       //���ѵķ�ʽ
	char		patch_id[MAX_PATCHID_LEN+1];	//���κ�
	char    	order_id[MAX_ORDERID_LEN+1];    //Ψһ��ʶ
	char    	user_type;                      //�û�����
	int			group_id;						//����
	int			area_id;						//�����
	char    	ip[IP_ADDRESS_LEN+1];           //ip��Ϸ�ͻ��˵�ַ
	char    	buyer_sd_id[MAX_SDID_LEN+1];    //��ҵ������ʺţ�ȫΪ���֡�                       
	char    	buyer_pt_id[MAX_PTID_LEN+1];    //��ҵ�PT�ַ��ʺţ�����ȫΪ����                      
	int			buyer_group_id;					//��ʾ��id 
	int			buyer_area_id;					//��Ϸ�����
	char  		log_time[MAX_TIME_LEN+1];		//���ݰ�����ʱ��      
}GSBsipConsignDepositReqDef;

typedef struct GSBsipConsignDepositRes
{
	int			result;							//���ؽ��
	char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺţ�ȫΪ���֡�
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺţ�����ȫΪ����
	char    	sess_id[MAX_SESSID_LEN+1];      //�Ựid
	char		pay_type;						//���ѵķ�ʽ
	char		patch_id[MAX_PATCHID_LEN+1];	//���κ�
	int			group_id;						//����
	char		order_id[MAX_ORDERID_LEN+1];	//Ψһ��ʶ
	int			area_id;						//�����
	char		ip[IP_ADDRESS_LEN+1];			//ip��Ϸ�ͻ��˵�ַ
	char    	buyer_sd_id[MAX_SDID_LEN+1];    //��ҵ������ʺţ�ȫΪ���֡�                       
	char    	buyer_pt_id[MAX_PTID_LEN+1];    //��ҵ�PT�ַ��ʺţ�����ȫΪ���� 
	int			buyer_group_id;					//��ʾ��id 
	int			buyer_area_id;					//��Ϸ�����          
	int			pt_balance;						//ʣ��PT��   

}GSBsipConsignDepositResDef;

/*-------------------------------Ԫ������-------------------------------*/
/*-------------------------------Ԫ����ֵ-------------------------------*/
typedef struct GSBsipGoldDepositReq
{
	char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�        
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	int			amount;							//��ֵ���              
	char		pay_type;						//��������              
	char		unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id              
	char		user_type;						//�û�����              
	char		ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ
	char		sync_flag;						//��ֵ��Ϣ��־λ   
	int			reserve_int;					//�����ֶΣ����ã�
	char		reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã� 
}GSBsipGoldDepositReqDef;

typedef struct GSBsipGoldDepositRes
{
	int			result;							//���ؽ��              
	char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�        
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	char		pay_type;						//��������              
	char		unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id              
	char		ip[IP_ADDRESS_LEN+1];			//��Ϸ�ͻ��˵�IP��ַ  
	int			reserve_int;					//�����ֶΣ����ã�
	char		reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�  
}GSBsipGoldDepositResDef;

/*-------------------------------Ԫ����ʱ��-------------------------------*/
typedef struct GSBsipGoldConsumeLockReq
{    
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺ�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char pay_type;                       //��������
    char couple_type;                    //�������
    int  amount;                         //��������
    int  discount;                       //�Ʒ��ۿ���
    char time_type;                      //�����ʱ������
    int  time_amount;                    //�һ���ʱ������
    char unique_id[MAX_ORDERID_LEN+1];   //���ݰ�id 
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
    char log_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
}GSBsipGoldConsumeLockReqDef;

typedef struct GSBsipGoldConsumeLockRes
{
    int  result;                         //���ؽ��
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺ�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char pay_type;                       //��������
    char time_type;                      //�����ʱ������
    int  time_amount;                    //�һ���ʱ������
    int  balance;                        //�һ����ܵ�ʱ������
    int  pt_balance;                     //�һ����ܵĵ�ȯ����
    char unique_id[MAX_ORDERID_LEN+1];   //���ݰ�id    
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
    char begin_time[MAX_TIME_LEN+1];     //��Чʱ�� ��time_type Ϊ1 ʱ��Ч��
    char end_time[MAX_TIME_LEN+1];       //ʧЧʱ�� ��time_type Ϊ1 ʱ��Ч��
}GSBsipGoldConsumeLockResDef;

typedef struct GSBsipGoldConsumeUnlockReq
{    
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺ�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char confirm;                        //ȷ�Ͽ۳�
    char pay_type;                       //��������
    char couple_type;                    //�������
    int  amount;                         //��������
    int  discount;                       //�Ʒ��ۿ���
    char time_type;                      //�����ʱ������
    int  time_amount;                    //�һ���ʱ������
    char unique_id[MAX_ORDERID_LEN+1];   //���ݰ�id
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
    char log_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
}GSBsipGoldConsumeUnlockReqDef;

typedef struct GSBsipGoldConsumeUnlockRes
{
    int  result;                         //���ؽ��
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺ�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺ�
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char pay_type;                       //��������
    char time_type;                      //�����ʱ������
    int  time_amount;                    //�һ���ʱ������
    int  balance;                        //�һ����ܵ�ʱ������
    int  pt_balance;                     //�һ����ܵĵ�ȯ����
    char unique_id[MAX_ORDERID_LEN+1];   //���ݰ�id
    char ip[IP_ADDRESS_LEN+1];           //��Ϸ�ͻ��˵�IP��ַ
    char begin_time[MAX_TIME_LEN+1];     //��Чʱ�� ��time_type Ϊ1 ʱ��Ч��
    char end_time[MAX_TIME_LEN+1];       //ʧЧʱ�� ��time_type Ϊ1 ʱ��Ч��
}GSBsipGoldConsumeUnlockResDef;

/*-------------------------------���ֵ-------------------------------*/
/*-------------------------------PT�ҳ�ֵ-------------------------------*/
typedef struct GSBsipDepositReq
{    
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺţ�ȫΪ���֡�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺţ�����ȫΪ����
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    int  item_id;                        //��Ϸ���߱��
    unsigned short item_num;             //��Ϸ��������
    char pay_type;                       //���ѵķ�ʽ
    int  amount;                         //Ԫ�����
    char unique_id[MAX_ORDERID_LEN+1];    //Ψһ��ʶ
    char user_type;                      //�û�����
    char ip[IP_ADDRESS_LEN+1];           //ip��Ϸ�ͻ��˵�ַ
    char log_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
}GSBsipDepositReqDef;

typedef struct GSBsipDepositRes
{
    int  result;                         //���ؽ�� 
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺţ�ȫΪ���֡�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺţ�����ȫΪ����
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    int  item_id;                        //��Ϸ���߱��
    unsigned short item_num;             //��Ϸ��������
    char pay_type;                       //���ѵķ�ʽ
    int  balance;                        //Ԫ�����
    char unique_id[MAX_ORDERID_LEN+1];   //Ψһ��ʶ    
    char ip[IP_ADDRESS_LEN+1];           //ip��Ϸ�ͻ��˵�ַ
}GSBsipDepositResDef;

typedef struct GSBsipDepositAckReq
{    
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺţ�ȫΪ���֡�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺţ�����ȫΪ����
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    char confirm;                        //ȷ�ϳ�ֵ
    int  item_id;                        //��Ϸ���߱��
    unsigned short item_num;             //��Ϸ��������
    char pay_type;                       //���ѵķ�ʽ
    int  amount;                         //Ԫ�����
    char unique_id[MAX_ORDERID_LEN+1];    //Ψһ��ʶ
    char user_type;                      //�û�����    
    char ip[IP_ADDRESS_LEN+1];           //ip��Ϸ�ͻ��˵�ַ
    char log_time[MAX_TIME_LEN+1];       //���ݰ�����ʱ��
}GSBsipDepositAckReqDef;

typedef struct GSBsipDepositAckRes
{
    int  result;                         //���ؽ��
    char sd_id[MAX_SDID_LEN+1];          //�û��������ʺţ�ȫΪ���֡�
    char pt_id[MAX_PTID_LEN+1];          //�û���PT�ַ��ʺţ�����ȫΪ����
    char sess_id[MAX_SESSID_LEN+1];      //�Ựid
    int  item_id;                        //��Ϸ���߱��
    unsigned short item_num;             //��Ϸ��������
    char pay_type;                       //���ѵķ�ʽ
    int  balance;                        //Ԫ�����
    char unique_id[MAX_ORDERID_LEN+1];   //Ψһ��ʶ    
    char ip[IP_ADDRESS_LEN+1];           //ip��Ϸ�ͻ��˵�ַ
}GSBsipDepositAckResDef;
/*-------------------------------��Ϣ����-------------------------------*/
typedef struct GSBsipNotifyReq
{
	int			area_id;						//��Ϸ�����
	int			group_id;						//��ʾ��id
	char		unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id
	int			notify_type;					//��Ϣ����
	int			info_data;						//��Ϣ����--����
	char		info_content[MAX_CONTENT_LEN+1];//��Ϣ����--�ַ���
	char		ip[IP_ADDRESS_LEN+1];			//Login Server��IP��ַ
	int			reserve_int;					//�����ֶΣ����ã�
	char		reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�             
}GSBsipNotifyReqDef;

typedef struct GSBsipNotifyRes
{
	int			result;							//���ؽ��
	int			area_id;						//��Ϸ�����
	int			group_id;						//��ʾ��id
	char		unique_id[MAX_ORDERID_LEN+1];	//���ݰ�id
	int			notify_type;					//��Ϣ����
	int			info_data;						//��Ϣ����--����
	char		info_content[MAX_CONTENT_LEN+1];//��Ϣ����--�ַ���
	char		ip[IP_ADDRESS_LEN+1];			//Login Server��IP��ַ
	int			reserve_int;					//�����ֶΣ����ã�
	char		reserve_char[MAX_RESERVE_LEN+1];//�����ֶΣ����ã�             
}GSBsipNotifyResDef;


/*�Ʒ���������ṹ*/

typedef struct{
	char		sd_id[MAX_SDID_LEN+1];	//�û��������ʺ�
	char		pt_id[MAX_PTID_LEN+1];	//�û���PT�ַ��ʺ�
}GSBsipUser;

typedef struct{
    char		sd_id[MAX_SDID_LEN+1];			//�û��������ʺ�
	char		pt_id[MAX_PTID_LEN+1];			//�û���PT�ַ��ʺ�      
	char 		sess_id[MAX_SESSID_LEN+1];		//�Ựid 
	int 		map_id;							//��Ϸ��map_id
} GSBsipLoginMsg;

typedef struct{
		char	sess_id[MAX_SESSID_LEN+1];		//���ػỰid 
		int		balance;      					//�����û���ǰʱ�����
		char	pay_type;						//�����û���ǰ��������
}GSBsipSessionMsg;

/*�Ʒ�����֪ͨ��Ϣ����*/
#define ALERT_BALANCE_NOTIFY	0x00400000 //BE����,�������֪ͨ
#define ALERT_NOT_ENOUGH_BALANCE	0x00400001 //BE����,����,֪ͨ����
#define ALERT_AUTHOR_FAILURE	0x00400002 //BE����,AUTHORʧ��,������ʾ��msg��
#define ALERT_ACCOUNT_FAILURE	0x00400003 //BE����,ACCOUNTʧ��,������ʾ��msg��
#define ALERT_LOGIN_TIMEOUT	0x00400004 //BE����,�û����볬ʱ

typedef struct{
	char		sd_id[MAX_SDID_LEN+1];	//�û��������ʺ�
	char		pt_id[MAX_PTID_LEN+1];	//�û���PT�ַ��ʺ�
	unsigned int	msg_type;	/*֪ͨ��Ϣ����*/
	char	msg[256];	/*��Ϣ����*/
}GSBsipAlertMsg;



#endif
