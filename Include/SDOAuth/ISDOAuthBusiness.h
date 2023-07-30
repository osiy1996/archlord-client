#ifndef _I_SDOA_AUTH_BUSINESS_H_
#define _I_SDOA_AUTH_BUSINESS_H_
#include "ISDOAuthCallBack.h"

typedef enum
{
	EM_SDO_UserInfo_FirstNeedFullInfo=13001,
	EM_SDO_UserInfo_FullInfoUrl=13002,
	EM_SDO_UserInfo_SecondNeedFullInfo=13003,
}EUserInfoKey;

interface ISDOAuthBusiness
{
	SNDAMETHOD(int)  Init(const char* pConfig)=0;

	SNDAMETHOD(void) SetCallBack(SSDOAuthCallBack * pCbObj)=0;

	SNDAMETHOD(int)  AsyncGetUserInfo(int nUserIdType,const char *szUserId, int *pKeyId, int nKeyNum, 
		SKeyValue *pAdditional, int nAdditional,unsigned int *dwRpcId)=0;

	SNDAMETHOD(int)  SyncGetUserInfo(int nUserIdType,const char *szUserId, int *pKeyId, int nKeyNum, 
		SKeyValue *pAdditional, int nAdditional, SKeyValue ** ppKeyValue, int *nKeyValueNum)=0;
	SNDAMETHOD(void)  SyncFreeResult(SKeyValue * pResult, int nKeyValueNum) = 0;
	
};
SNDAAPI(ISDOAuthBusiness*) CreateSDOAuthBusinessInstance();
#endif

