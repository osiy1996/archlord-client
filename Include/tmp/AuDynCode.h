// AuDynCode.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.06.08.

//
// Wrapper for DynCode of China
//

#ifdef _AREA_CHINA_

#ifdef _USE_GPK_

#ifndef _AUDYNCODE_H_
#define _AUDYNCODE_H_

#include "AuCryptAlgorithmBase.h"

#include "GPKitClt.h"
#include "GPKitSvr.h"

// 서버에서만 사용하게 된다.
// 유저당 한개씩 암호화 알고리즘 인덱스값 저장용.
class DYNCODE_CTX : public BASE_CTX
{
public:
	int nCodeIndex;

	void init();
	DYNCODE_CTX() { init(); }
	~DYNCODE_CTX() { init(); }
};

// 암호화 복호화용 DynCode 유틸 클래스
class AuDynCode : public AuCryptAlgorithmBase
{
private:
	SGPK::IGPKCltDynCode* m_pGPKClt;
	SGPK::IGPKSvrDynCode* m_pGPKSvr;

	BOOL	_InitGPK();

public:
	AuDynCode();
	virtual ~AuDynCode();

	
	void	Cleanup();

	void	Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone = FALSE);
	DWORD	Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);
	void	Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);

	DWORD	GetOutputSize(DWORD lInputSize);

	// Specific for AuDynCode
	INT32	GetClientDynCode(void* pctx, const BYTE** ppCode);
};
extern AuDynCode g_DynCode;

#endif//_AUDYNCODE_H_

#endif//_USE_GPK_


#endif//_AREA_CHINA_