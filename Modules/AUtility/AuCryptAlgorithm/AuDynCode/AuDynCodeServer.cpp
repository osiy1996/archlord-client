// AuDynCode.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.06.08.

#ifdef _AREA_CHINA_

#ifdef _USE_GPK_

#ifdef _WIN64
	#pragma comment(lib, "GPKitSvr64.lib")
#else
	#ifdef _DEBUG
		#pragma comment(lib, "GPKitSvrD.lib")
	#else
		#pragma comment(lib, "GPKitSvr.lib")
	#endif
#endif

#include "AuDynCode.h"
#include <windows.h>
#include "GPKitSvr.h"

const CHAR* CLIENTBIN_PATH = "\\DynCodeBin64\\Client";
#ifdef _WIN64
	const CHAR* SERVERBIN_PATH = "\\DynCodeBin64\\Server";
#else
	const CHAR* SERVERBIN_PATH = "\\DynCodeBin32\\Server";
#endif
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

// ���� DynCode �� �ִ� �� ������.
// Static �̾ ������ ��. �ѹ��ϰ� ���� �״���� read �����̴�.
char* GetRootPath();

char* GetRootPath()
{
    static char szPath[MAX_PATH];
    static bool bFirstTime = true;

    if(bFirstTime)
    {
        bFirstTime = false;
        GetModuleFileName(NULL, szPath, sizeof(szPath));
        char *p = strrchr(szPath, '\\');
        *p = '\0';
    }

    return szPath;
}

BOOL AuDynCode::_InitGPK()
{
	m_pGPKSvr = SGPK::GPKCreateSvrDynCode();

    if(NULL == m_pGPKSvr)
		return FALSE;

    CHAR szServerDir[MAX_PATH];
    sprintf_s(szServerDir, "%s%s", GetRootPath(), SERVERBIN_PATH);

    CHAR szClientDir[MAX_PATH];
    sprintf_s(szClientDir, "%s%s", GetRootPath(), CLIENTBIN_PATH);

	int nBinCount;
    nBinCount = m_pGPKSvr->LoadBinary(szServerDir, szClientDir);
	if(nBinCount == 0)
	{
		ASSERT(nBinCount > 0);
	}

	return TRUE;
}

void AuDynCode::Cleanup()
{
	if(m_pGPKSvr)
	{
		m_pGPKSvr->Release();
		m_pGPKSvr = NULL;
	}
}

void AuDynCode::Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone)
{
}

// Encrypt �� pOutput �� pInput �� ����, lSize �� �״���̴�.
DWORD AuDynCode::Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || lSize < 1)
		return 0;
	memcpy(pOutput, pInput, lSize);
	return lSize;
}

// Decrypt �� pOutput �� pInput �� ����, lSize �� �״���̴�.
void AuDynCode::Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || lSize < 1)
		return;

	DYNCODE_CTX& ctx = *static_cast<DYNCODE_CTX*>(pctx);
	if(!m_pGPKSvr)
		return;
	if(m_pGPKSvr->Decode(pInput, lSize, ctx.nCodeIndex))
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

	if( m_pGPKSvr )
		ctx.nCodeIndex = m_pGPKSvr->GetRandIdx();
	return m_pGPKSvr->GetCltDynCode(ctx.nCodeIndex, ppCode);
}

#endif//_USE_GPK_


#endif//_AREA_CHINA_