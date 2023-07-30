#pragma once

#include "bsipfun.h"
#include "bsipmsg.h"

class CCallBackFunc  
{
public:
	CCallBackFunc();
	virtual ~CCallBackFunc();

public:
	static void PCallbackAuthorRes(GSBsipAuthorResDef* );  
	static void PCallbackAccountRes(GSBsipAccountResDef* );
	static void PCallbackAccountAuthenRes(GSBsipAccountAuthenResDef* );		
	static void PCallbackAccountLockRes(GSBsipAccountLockResDef* );  
	static void PCallbackAccountUnlockRes(GSBsipAccountUnlockResDef* ); 
	static void PCallbackAccountLockExRes(GSBsipAccountLockExResDef* );  
	static void PCallbackAccountUnlockExRes(GSBsipAccountUnlockExResDef* );	
	static void PCallbackAwardAuthenRes(GSBsipAwardAuthenResDef* );
	static void PCallbackAwardAckRes(GSBsipAwardAckResDef* );
	static void PCallbackGoldConsumeUnlockRes(GSBsipGoldConsumeUnlockResDef* );
};
