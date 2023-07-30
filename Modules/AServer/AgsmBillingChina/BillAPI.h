#pragma once

#include <windows.h>
#include <string>

#include "bsipfun.h"
#include "APIFunc.h"
#include "CallbackFunc.h"

#define BSIP_ERROR -1
#define BSIP_OK 0

class CBillAPI  
{
public:
	CBillAPI();
	virtual ~CBillAPI();

private:
	std::string m_fileName;
	HINSTANCE	m_hDll;

public:
	GSCallbackFuncDef m_funCallBack;

public:
	int IntSockEnv();
	int LoadDll();
	int LoadDll(std::string fileName);
	int GetFunc();
	int SetCallBackFunc();
	int FreeDll();

public:
	FunGSInitialize							m_pGSInitialize;
	FunUninitialize                         m_pUninitialize;
	FunSendAuthorRequest                    m_pSendAuthorRequest;
	FunSendAccountRequest                   m_pSendAccountRequest;
	FunSendAccountAuthenRequest             m_pSendAccountAuthenRequest;
	FunSendAccountLockRequest               m_pSendAccountLockRequest;
	FunSendAccountUnlockRequest             m_pSendAccountUnlockRequest;
	FunSendAccountLockExRequest             m_pSendAccountLockExRequest;
	FunSendAccountUnlockExRequest           m_pSendAccountUnlockExRequest;
	FunSendAwardAuthenRequest               m_pSendAwardAuthenRequest;
	FunSendAwardAck                         m_pSendAwardAck;
	FunGetNewId                             m_pGetNewId;
	FunGetNewIdByParam                      m_pGetNewIdByParam;
};
