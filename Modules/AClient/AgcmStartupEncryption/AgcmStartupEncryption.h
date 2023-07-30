// AghmStartupEncryption.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.11.

#ifndef _AGCMSTARTUPENCRYPTION_H_
#define _AGCMSTARTUPENCRYPTION_H_

#include <AgcModule/AgcModule.h>
#include <AuRandomNumberGenerator/AuRandomNumber.h>

//const int AGCMSTARTUPENCRYPTION_MAX_RANDOM_LENGTH		= 8;	// 8 의 배수 단위로 하자.

class AgcmStartupEncryption : public AgcModule
{
private:
	class AgpmStartupEncryption*	m_pcsAgpmStartupEncryption;
	class AgcmLogin*				m_pcsAgcmLogin;
	class AgcmReturnToLogin*		m_pcsAgcmReturnToLogin;
	class AgcmConnectManager*		m_pcsAgcmConnectManager;
	class AgcmCharacter*			m_pcsAgcmCharacter;

	MTRand			m_csRandom;

public:
	AgcmStartupEncryption();
	virtual ~AgcmStartupEncryption();

	BOOL OnAddModule();

	BOOL SendRequestPublic(INT16 nNID);
	BOOL SendEncryptedRandom(BYTE* pKey, INT32 nSize, INT16 nNID);

	static BOOL CBAlgorithmType(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBPublic(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBComplete(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBDynCodePublic(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDynCodePrivate(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL CBConnectLoginServer(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBConnectGameServer(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBReconnectLoginServer(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif//_AGCMSTARTUPENCRYPTION_H_