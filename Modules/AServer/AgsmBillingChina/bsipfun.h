/*
* Copyright (c) 2005,�Ϻ�ʢ������Ʒ�ƽ̨��
* All rights reserved.
* 
* �ļ����ƣ�bsipfun.h
* ժ    Ҫ�����ӿں�������
* ��ǰ�汾��1.4
* ��    �ߣ�����
* ������ڣ�2005��9��6��
* �޸����ݣ���1.3�İ汾�Ͻ����޸ģ�ɾ�����õĺ���
*           ��Ӽ��۽���ʹ�õĺ���
* �޸�ʱ�䣺2005��9��6��
*
* �޸����ݣ����Notify��Ϣ���ܽӿ�
* �޸�ʱ�䣺2005��10��13��
*/

#ifndef _BSIP_FUN_H_
#define _BSIP_FUN_H_ 

#ifdef WIN32
	#define BSIP_API_TYPE _declspec(dllexport)
#else
	#define BSIP_API_TYPE
#endif

#define AWARD_NUMBER_LEN16_TYPE 1
#define AWARD_NUMBER_LEN30_TYPE 2

#include "bsipmsg.h"

typedef struct
{
	void (*CallbackAuthorRes)(GSBsipAuthorResDef* );  
	void (*CallbackAccountRes)(GSBsipAccountResDef* );
	void (*CallbackAccountAuthenRes)(GSBsipAccountAuthenResDef* );			
	void (*CallbackAccountLockRes)(GSBsipAccountLockResDef* );  
	void (*CallbackAccountUnlockRes)(GSBsipAccountUnlockResDef* ); 	
	void (*CallbackAccountLockExRes)(GSBsipAccountLockExResDef* );  
	void (*CallbackAccountUnlockExRes)(GSBsipAccountUnlockExResDef* ); 	
	void (*CallbackAwardAuthenRes)(GSBsipAwardAuthenResDef* );
	void (*CallbackAwardAckRes)(GSBsipAwardAckResDef* );	
	void (*CallbackConsignTransferRes)(GSBsipConsignTransferResDef* );
#if BSIP_VERSION < 0X00010400
	void (*CallbackGoldDepositReq)(GSBsipGoldDepositReqDef* );
#endif
	void (*CallbackGoldConsumeLockRes)(GSBsipGoldConsumeLockResDef* );
	void (*CallbackGoldConsumeUnlockRes)(GSBsipGoldConsumeUnlockResDef* );
	void (*CallbackTokenDepositRes)(GSBsipTokenDepositResDef *p );
	void (*CallbackItemSyncReq)( GSBsipItemSyncReqDef *req );
	void (*CallbackEkeyBindRes)(GSBsipEkeyBindResDef* );  
	void (*CallbackNotifyS2CReq)(GSBsipNotifyS2CReqDef* );
	void (*CallbackNotifyC2SRes)(GSBsipNotifyC2SResDef* );
	void (*CallbackAccountExchangeLockRes)(GSBsipAccountExchangeLockResDef* );
	void (*CallbackAccountExchangeUnlockRes)(GSBsipAccountExchangeUnlockResDef* );
}GSCallbackFuncDef;

#ifdef __cplusplus
extern "C" 
{
#endif
	BSIP_API_TYPE int GSInitialize(GSCallbackFuncDef *callbackFun, const char *configfilename,int client_type);
	BSIP_API_TYPE int Uninitialize(int client_type);	
	BSIP_API_TYPE int SendAuthorRequest(				const GSBsipAuthorReqDef *req			);
	BSIP_API_TYPE int SendAccountRequest(				const GSBsipAccountReqDef *req			);
	BSIP_API_TYPE int SendAccountAuthenRequest(			const GSBsipAccountAuthenReqDef *req	);
	BSIP_API_TYPE int SendEkeyBindRequest(				const GSBsipEkeyBindReqDef *req		);
	BSIP_API_TYPE int SendAccountLockRequest(			const GSBsipAccountLockReqDef *req		);
	BSIP_API_TYPE int SendAccountUnlockRequest(			const GSBsipAccountUnlockReqDef *req	);	
	BSIP_API_TYPE int SendAccountLockExRequest(			const GSBsipAccountLockExReqDef *req	);
	BSIP_API_TYPE int SendAccountUnlockExRequest(		const GSBsipAccountUnlockExReqDef *req	);
	BSIP_API_TYPE int SendAwardAuthenRequest(			const GSBsipAwardAuthenReqDef *req		);
	BSIP_API_TYPE int SendAwardAck(						const GSBsipAwardAckDef *req			);
	BSIP_API_TYPE int SendConsignTransferRequest(		const GSBsipConsignTransferReqDef *req	);
#if BSIP_VERSION < 0X00010400
	BSIP_API_TYPE int SendGoldDepositResponse(			const GSBsipGoldDepositResDef *res		);
#endif
	BSIP_API_TYPE int SendGoldConsumeLockRequest(		const GSBsipGoldConsumeLockReqDef *req	);	
	BSIP_API_TYPE int SendGoldConsumeUnlockRequest(		const GSBsipGoldConsumeUnlockReqDef *req);	

	BSIP_API_TYPE int SendAccountExchangeLockRequest(		const GSBsipAccountExchangeLockReqDef *req	);	
BSIP_API_TYPE int SendAccountExchangeUnlockRequest(		const GSBsipAccountExchangeUnlockReqDef *req);	
	
    BSIP_API_TYPE int SendTokenDepositRequest(	        const GSBsipTokenDepositReqDef *req     );
	BSIP_API_TYPE int SendItemSyncResponse(				const GSBsipItemSyncResDef *res			);
	
	/// ��������·�
	BSIP_API_TYPE int SendNotifyC2SRequest(				const GSBsipNotifyC2SReqDef *req			);
	BSIP_API_TYPE int SendNotifyS2CResponse(			const GSBsipNotifyS2CResDef *res			);
	
#ifdef ITEMLOG_GATHER
    //�ýӿ�д������־��û�лص�
    BSIP_API_TYPE int SendItemLogMsg(                   const GSBsipItemLogMsgDef *req          );
#endif
	BSIP_API_TYPE int GetNewId(char *id);
	BSIP_API_TYPE int GetNewIdByParam(int intareaId,int intGroupId, char *id);	

	/// type = AWARD_NUMBER_LEN16_TYPE 
	/// type = AWARD_NUMBER_LEN30_TYPE
	BSIP_API_TYPE int GetAwardNumber(char *id,int type);

#ifdef __cplusplus
}
#endif

#endif
