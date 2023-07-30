/*
* Copyright (c) 2005,上海盛大网络计费平台部
* All rights reserved.
* 
* 文件名称：bsipfun.h
* 摘    要：各接口函数定义
* 当前版本：1.4
* 作    者：李闻
* 完成日期：2005年9月6日
* 修改内容：在1.3的版本上进行修改，删除弃用的函数
*           添加寄售交易使用的函数
* 修改时间：2005年9月6日
*
* 修改内容：添加Notify消息汇总接口
* 修改时间：2005年10月13日
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
	
	/// 处理道具下发
	BSIP_API_TYPE int SendNotifyC2SRequest(				const GSBsipNotifyC2SReqDef *req			);
	BSIP_API_TYPE int SendNotifyS2CResponse(			const GSBsipNotifyS2CResDef *res			);
	
#ifdef ITEMLOG_GATHER
    //该接口写本地日志，没有回调
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
