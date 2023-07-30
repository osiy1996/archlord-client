#pragma once

#include <AgcModule/AgcModule.h>

typedef enum	_AgcmReturnToLoginCB {
	AGCMRETURN_CB_REQUEST_FAILED			= 0,
	AGCMRETURN_CB_END_PROCESS,
	AGCMRETURN_CB_RECONNECT_LOGINSERVER,
} AgcmReturnToLoginCB;

class AgcmReturnToLogin : public AgcModule {
private:
	class AgpmCharacter			*m_pcsAgpmCharacter;
	class AgpmReturnToLogin		*m_pcsAgpmReturnToLogin;
	class AgcmCharacter			*m_pcsAgcmCharacter;
	class AgcmConnectManager	*m_pcsAgcmConnectManager;
	class AgcmLogin				*m_pcsAgcmLogin;

	INT32				m_lAuthKey;

public:
	AgcmReturnToLogin();
	virtual ~AgcmReturnToLogin();

	BOOL	OnAddModule();

	static BOOL	CBReceiveRequestFailed(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBReceiveAuthKey(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBEndProcess(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBSocketOnConnect(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	SendPacketRequest();
	BOOL	SendReconnectLoginServer();

	BOOL	SetCallbackRequestFailed(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallbackEndProcess(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL	SetCallBackReconnectLoginServer(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	INT32	GetAuthKey() { return m_lAuthKey; }

private:
	BOOL	ReconnectLoginServer(INT32 lAuthKey, CHAR *pszLoginServerAddr);
};