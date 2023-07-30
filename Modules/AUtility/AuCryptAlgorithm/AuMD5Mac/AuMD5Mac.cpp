// AuMD5Mac.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.17.

#include "AuMD5Mac.h"

AuMD5Mac::AuMD5Mac()
{
	Startup();
}

AuMD5Mac::~AuMD5Mac()
{
	Cleanup();
}

void AuMD5Mac::Startup()
{
	CryptSetProvider(MS_DEF_PROV, PROV_RSA_FULL);

	if(!CryptAcquireContext(&m_hCryptProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
	{
		ASSERT(!"AuMD5Mac::Startup() CryptAcquireContext Failure!!!");
		return;
	}
}

void AuMD5Mac::Cleanup()
{
	if(m_hCryptProv) 
	   CryptReleaseContext(m_hCryptProv, 0);
}

void AuMD5Mac::Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone)
{
	if(!pctx || !key || lKeySize < 1)
		return;

	MD5MAC_CTX& ctx = *static_cast<MD5MAC_CTX*>(pctx);

	if(lKeySize != sizeof(ctx.m_HashByte))
	{
		ASSERT(!"�Ѿ�� KeySize �� sizeof(ctx.m_HashByte) �� ��ġ�� ���Ѵ�.");
		return;
	}

	if(bClone)
	{
		// �׳� Ű�� �����Ѵ�.
		memcpy(ctx.m_HashByte, key, lKeySize);
		return;
	}

	// Random Hash String �� ������ش�.
	memset(ctx.m_HashByte, 0, sizeof(ctx.m_HashByte));
	srand(timeGetTime());

	for(int i = 0; i < AUCRYPT_KEY_LENGTH_32BYTE; i++)
		ctx.m_HashByte[i] = (BYTE)(rand()%256);

	// �Էµ� Key �� ������� Hash ���� Ű�� �Ѵ�.
	// �Ѿ�� key �� ctx.m_HashByte ũ�⿡ ��ġ�� ���ϸ� ������ �����.
	memcpy(key, ctx.m_HashByte, sizeof(ctx.m_HashByte));
}

// MD5Mac �� pInput �� ���� ũ��� �׳� �ٲ�� ������, pOutput �� �׳� ī�����ش�.
DWORD AuMD5Mac::Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || !pOutput || lSize < 1)
		return 0;

	MD5MAC_CTX& ctx = *static_cast<MD5MAC_CTX*>(pctx);

	HCRYPTKEY		hKey; 
	HCRYPTHASH		hHash; 

	//Hash�� �����.
	if(!CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &hHash))
		return 0;

	//Hash Object�� Hash Data�� �ִ´�.
	if(!CryptHashData(hHash, ctx.m_HashByte, AUCRYPT_KEY_LENGTH_32BYTE, 0))
		return 0;

	//Key�� �̾Ƴ���.
	if(!CryptDeriveKey(m_hCryptProv, CALG_RC4, hHash, CRYPT_CREATE_SALT, &hKey))
		return 0;

	//Key�� �̾Ƴ����� Hash OBJ�� �ʱ�ȭ�Ѵ�.
	CryptDestroyHash(hHash);
	hHash = 0;

    //Key�� �̿��� pstrData�� Encoding�Ѵ�.
	if(!CryptEncrypt(hKey, 0, FALSE, 0, pInput, &lSize, lSize))
		return 0;

	//Key�� ���̻� ����� ���� �����Ƿ� ���ش�.
	CryptDestroyKey(hKey);

	// pOutput �� �־��ش�.
	memcpy(pOutput, pInput, lSize);
	return lSize;
}

void AuMD5Mac::Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize)
{
	if(!pctx || !pInput || !pOutput || lSize < 1)
		return;

	MD5MAC_CTX& ctx = *static_cast<MD5MAC_CTX*>(pctx);

	HCRYPTKEY		hKey; 
	HCRYPTHASH		hHash; 

	//Hash�� �����.
	if(!CryptCreateHash(m_hCryptProv, CALG_MD5, 0, 0, &hHash))
		return;

	//Hash Object�� Hash Data�� �ִ´�.
	if(!CryptHashData(hHash, ctx.m_HashByte, AUCRYPT_KEY_LENGTH_32BYTE, 0))
		return;

	//Key�� �̾Ƴ���.
	if(!CryptDeriveKey(m_hCryptProv, CALG_RC4, hHash, CRYPT_CREATE_SALT, &hKey))
		return;

	//Key�� �̾Ƴ����� Hash OBJ�� �ʱ�ȭ�Ѵ�.
	CryptDestroyHash(hHash); 
	hHash = 0;

	//���̻� Encoding�Ұ� ������ Final���ڸ� TRUE��....
	if(!CryptDecrypt(hKey, 0, FALSE, 0, pInput, &lSize))
		return;
	
	//Key�� ���̻� ����� ���� �����Ƿ� ���ش�.
	CryptDestroyKey(hKey);

	// Output �ʿ� Copy
	memcpy(pOutput, pInput, lSize);
}

// MD5Mac �� Input, Output Size �� ����.
DWORD AuMD5Mac::GetOutputSize(DWORD lInputSize)
{
	return lInputSize;
}
