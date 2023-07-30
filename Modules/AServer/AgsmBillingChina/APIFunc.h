#pragma once

#include "bsipmsg.h"
#include "bsipfun.h"

typedef int (*FunGSInitialize)(GSCallbackFuncDef *callbackFun, const char *configfilename,int type);
typedef int (*FunUninitialize)( int type );	
typedef int (*FunSendAuthorRequest)(				const GSBsipAuthorReqDef *req			);
typedef int (*FunSendAccountRequest)(				const GSBsipAccountReqDef *req			);
typedef int (*FunSendAccountAuthenRequest)(			const GSBsipAccountAuthenReqDef *req	);
typedef int (*FunSendAccountLockRequest)(			const GSBsipAccountLockReqDef *req		);
typedef int (*FunSendAccountUnlockRequest)(			const GSBsipAccountUnlockReqDef *req	);	
typedef int (*FunSendAccountLockExRequest)(			const GSBsipAccountLockExReqDef *req	);
typedef int (*FunSendAccountUnlockExRequest)(		const GSBsipAccountUnlockExReqDef *req	);
typedef int (*FunSendAwardAuthenRequest)(			const GSBsipAwardAuthenReqDef *req		);
typedef int (*FunSendAwardAck)(						const GSBsipAwardAckDef *req			);

typedef int (*FunGetNewId)(char *id);
typedef int (*FunGetNewIdByParam)(int intareaId,int intGroupId, char *id);
