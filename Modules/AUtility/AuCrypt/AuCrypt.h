// AuCrypt.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.07.

//
// AuCryptCell �� BYTE �������� Encrypt/Decrypt �������̽��� �����Ѵ�.
// ���� �˰����� AuCryptManager �� �ִ�.
//
// AuCryptActor �� Encrypt/Decrypt �� ���� �ʿ��� ������ �ν��Ͻ�ȭ �ȴ�.
//

#ifndef _AUCRYPT_H_
#define _AUCRYPT_H_

#include <ApBase/ApBase.h>
#include <AuCryptAlgorithm/AuCryptAlgorithm.h>

// ��ȣȭ�� ������ ���� �⺻���� Ŭ����
// Remove Template. 2006.06.13. steeple
//template <class KeyInfo, INT32 lKeyInfoSize = sizeof(KeyInfo)>

class AuCryptCell
{
public:
	typedef BASE_CTX*		KeyInfo;	// KeyInfo is Pointer Type

private:
	KeyInfo m_KeyInfo;

	INT32 m_lKeyInfoSize;

public:
	AuCryptCell();
	virtual ~AuCryptCell();

	void SetKey(KeyInfo key) { m_KeyInfo = key; }

	void SetKeyInfoSize(INT32 lSize) { m_lKeyInfoSize = lSize; }
	INT32 GetKeyInfoSize() { return m_lKeyInfoSize; }

	//KeyInfo& GetKeyRef() { return m_KeyInfo; }
	//KeyInfo* GetKeyPtr() { return &m_KeyInfo; }

	KeyInfo GetKeyPtr() { return m_KeyInfo; }
	KeyInfo GetKey()	{ return m_KeyInfo; }	// Enable by KeyInfo is Pointer Type

	void CleanUp() { if(m_KeyInfo) m_KeyInfo->init(); }
};

// public, private �� ������ �ְ�, ���� ���¿� ���� ������ ���� ��.
class AuCryptActor
{
public:
//#ifdef _AREA_CHINA_
//	typedef AuCryptCell<DYNCODE_CTX>		PublicKeyInfo;
//	typedef AuCryptCell<DYNCODE_CTX>		PrivateKeyInfo;		// �ϴ� Private �� BlowFish
//#else
//	typedef AuCryptCell<BLOWFISH_CTX>		PublicKeyInfo;
//	typedef AuCryptCell<BLOWFISH_CTX>		PrivateKeyInfo;		// �ϴ� Private �� BlowFish
//#endif
//
private:
	eAuCryptType m_eCryptType;

	AuCryptCell m_PublicKey;
	AuCryptCell m_PrivateKey;			

public:
	AuCryptActor();
	virtual ~AuCryptActor();

	// �Ʒ��� �⺻���� Ÿ���� �̷��ٴ� ���̰�, �����δ� public/private ��� ��� �����ϴ�.
	// ó�� ����ÿ��� �ϳ��� �ǰ� �Ϸ������� �߰��� �ٲ���. None �̳� �ƴϳĸ� üũ�ϸ� �ȴ�.
	void SetCryptType(eAuCryptType eType) { m_eCryptType = eType; }
	eAuCryptType GetCryptType() { return m_eCryptType; }

	AuCryptCell& GetPublicKeyRef() { return m_PublicKey; }
	AuCryptCell* GetPublicKeyPtr() { return &m_PublicKey; }

	AuCryptCell& GetPrivateKeyRef() { return m_PrivateKey; }
	AuCryptCell* GetPrivateKeyPtr() { return &m_PrivateKey; }

	void CleanUpPublic() { m_PublicKey.CleanUp(); }
	void CleanUpPrivate() { m_PrivateKey.CleanUp(); }

	void Initialize() { CleanUpPublic(); CleanUpPrivate(); SetCryptType(AUCRYPT_TYPE_NONE); }
};

#endif //_AUCrypt_H_
