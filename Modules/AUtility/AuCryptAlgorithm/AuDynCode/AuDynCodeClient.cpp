// AuDynCode.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.06.08.

#ifdef _AREA_CHINA_

#ifdef _USE_GPK_

#pragma comment(lib, "GPKitClt.lib")

#include "AuDynCode.h"
#include <windows.h>
#include "GPKitClt.h"

AuDynCode g_DynCode;

void DYNCODE_CTX::init()
{
	nCodeIndex = 0;
}

AuDynCode::AuDynCode()
{
	_InitGPK();
}

AuDynCode::~AuDynCode()
{
	Cleanup();
}

BOOL AuDynCode::_InitGPK()
{
#ifdef _DEBUG	
	m_pGPKClt = SGPK::GPKStart("http://10.95.3.31/gpk","AC"); //���� �׼���.
#else
	m_pGPKClt = SGPK::GPKStart("http://area.ac.sdo.com/gpk","AC");
#endif
	if(NULL == m_pGPKClt)
		ExitProcess( 1 );
	return TRUE;
}

void AuDynCode::Cleanup()
{
	if(m_pGPKClt)
		m_pGPKClt->Release();
}

void AuDynCode::Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone)
{
	if(key)
	{
		m_pGPKClt->SetDynCode(key, lKeySize);
	}
}

// Encrypt �� pOutput �� pInput �� ����, lSize �� �״���̴�.
DWORD AuDynCode::Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pInput || lSize < 0)
		return 0;

	if(m_pGPKClt->Encode(pInput, lSize))
	{
		memcpy(pOutput, pInput, lSize);
		return lSize;
	}
	else
		return 0;
}

// Decrypt �� pOutput �� pInput �� ����, lSize �� �״���̴�.
void AuDynCode::Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pInput || lSize < 0)
		return;

	memcpy(pOutput, pInput, lSize);
}

// Output �� ����.
DWORD AuDynCode::GetOutputSize(DWORD lInputSize)
{
	return lInputSize;
}

// AuCryptAlgorithmBase ���� ��ӹ��� �� �ƴ� DynCode �� Ưȭ�� �Լ�
INT32 AuDynCode::GetClientDynCode(void* pctx, const BYTE** ppCode)
{
	if(!pctx)
		return 0;

	DYNCODE_CTX& ctx = *static_cast<DYNCODE_CTX*>(pctx);

	return 0;
}

#endif//_USE_GPK_


#endif//_AREA_CHINA_