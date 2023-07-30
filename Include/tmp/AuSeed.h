// AuSeed.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.27.

//
// C++ Wrapper for Seed algorithm
//

// ���� �ּ����� �ۿԾ�����, ������ �������ȴ�. ��������.
// �̳��� ���̸� �޸� �� ����!!!!!
// Wrapping �ϱ⵵ ������ ������~~~

#ifndef _AUSEED_H_
#define _AUSEED_H_

#include "AuCryptAlgorithmBase.h"

//#include <stdlib.h>
//#include <string.h>
//#include <memory.h>

// �������� �� ����� Little_Endian
#undef BIG_ENDIAN
#define LITTLE_ENDIAN	1		// For x86 Compatility

//	rotate by using shift operations
#if defined(_MSC_VER)
	#define ROTL_DWORD(x, n) _lrotl((x), (n))
	#define ROTR_DWORD(x, n) _lrotr((x), (n))
#else
	#define ROTL_DWORD(x, n) ( (DWORD)((x) << (n)) | (DWORD)((x) >> (32-(n))) )
	#define ROTR_DWORD(x, n) ( (DWORD)((x) >> (n)) | (DWORD)((x) << (32-(n))) )
#endif

//	reverse the byte order of DWORD(DWORD:4-bytes integer) and WORD.
#define ENDIAN_REVERSE_DWORD(dwS)	( (ROTL_DWORD((dwS),  8) & 0x00ff00ff)	\
									 | (ROTL_DWORD((dwS), 24) & 0xff00ff00) )

//	move DWORD type to BYTE type and BYTE type to DWORD type
#if defined(BIG_ENDIAN)		////	Big-Endian machine
	#define BIG_B2D(B, D)		D = *(DWORD *)(B)
	#define BIG_D2B(D, B)		*(DWORD *)(B) = (DWORD)(D)
	#define LITTLE_B2D(B, D)	D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
#elif defined(LITTLE_ENDIAN)	////	Little-Endian machine
	#define BIG_B2D(B, D)		D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define BIG_D2B(D, B)		*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
	#define LITTLE_B2D(B, D)	D = *(DWORD *)(B)
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = (DWORD)(D)
#else
	#error ERROR : Invalid DataChangeType
#endif

//	���� �Ʒ��� 4�� ����� �����Ѵ�.
#define AI_ECB					1
#define AI_CBC					2
#define AI_OFB					3
#define AI_CFB					4
//	���� �Ʒ��� �� padding�� �����Ѵ�.
#define AI_NO_PADDING			1	//	Padding ����(�Է��� 16����Ʈ�� ���)
#define AI_PKCS_PADDING			2	//	padding�Ǵ� ����Ʈ ���� padding

//	SEED�� ���õ� �����
#define SEED_BLOCK_LEN			16		//	in BYTEs
#define SEED_USER_KEY_LEN		16		//	in BYTEs
#define SEED_NO_ROUNDS			16
#define SEED_NO_ROUNDKEY		(2*SEED_NO_ROUNDS)	//	in DWORDs

//	SEED..
class SEED_ALG_INFO : public BASE_CTX
{
public:
	DWORD		ModeID;						//	ECB or CBC
	DWORD		PadType;					//	��Ͼ�ȣ�� Padding type
	BYTE		IV[SEED_BLOCK_LEN];			//	Initial Vector
	BYTE		ChainVar[SEED_BLOCK_LEN];	//	Chaining Variable
	BYTE		Buffer[SEED_BLOCK_LEN];		//	Buffer for unfilled block
	DWORD		BufLen; 					//	Buffer�� ��ȿ ����Ʈ ��
	DWORD		RoundKey[SEED_NO_ROUNDKEY];	//	���� Ű�� DWORD ��

	void		init()
	{
		eAlgorithm = AUCRYPT_ALGORITHM_SEED;
		ModeID = 0;
		PadType = 0;
		memset(IV, 0, sizeof(IV));
		memset(ChainVar, 0, sizeof(ChainVar));
		memset(Buffer, 0, sizeof(Buffer));
		BufLen = 0;
		memset(RoundKey, 0, sizeof(RoundKey));
	}

	SEED_ALG_INFO() { init(); }
	~SEED_ALG_INFO() {;}
};

typedef SEED_ALG_INFO	SEED_CTX;	// �ٸ� �˰������� Naming ������ �߰�.
typedef DWORD			RET_VAL;


//	Error Code - �����ϰ�, ������ ����ؾ� ��.
#define CTR_SUCCESS					0
#define CTR_FATAL_ERROR				0x1001
#define CTR_INVALID_USERKEYLEN		0x1002	//	���Ű�� ���̰� ��������.
#define CTR_PAD_CHECK_ERROR			0x1003	//	
#define CTR_DATA_LEN_ERROR			0x1004	//	���� ���̰� ��������.
#define CTR_CIPHER_LEN_ERROR		0x1005	//	��ȣ���� ����� ����� �ƴ�.



// ���⼭ ���� C++ Wrapper
class AuSeed : public AuCryptAlgorithmBase
{
private:
	void	SEED_SetAlgInfo(DWORD ModeID, DWORD PadType, BYTE* IV, SEED_ALG_INFO* AlgInfo);		// AlgInfo �ʱ�ȭ
	RET_VAL SEED_KeySchedule(BYTE* UserKey, DWORD UserKeyLen, SEED_ALG_INFO *AlgInfo);			// Round Key ����

	// ��ȣȭ �Լ���.
	RET_VAL	SEED_EncInit(SEED_ALG_INFO* AlgInfo);
	RET_VAL	SEED_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL	SEED_EncFinal(SEED_ALG_INFO	*AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen);
	void	SEED_Encrypt(void* CipherKey, BYTE* Data);

	RET_VAL ECB_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL	CBC_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL	OFB_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL	CFB_EncUpdate(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD PlainTxtLen, BYTE* CipherTxt, DWORD* CipherTxtLen);

	RET_VAL ECB_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL CBC_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL OFB_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen);
	RET_VAL CFB_EncFinal(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD* CipherTxtLen);

	// ��ȣȭ �Լ���.
	RET_VAL	SEED_DecInit(SEED_ALG_INFO* AlgInfo);
	RET_VAL	SEED_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL	SEED_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen);
	void	SEED_Decrypt(void* CipherKey, BYTE* Data);

	RET_VAL ECB_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL CBC_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL OFB_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL CFB_DecUpdate(SEED_ALG_INFO* AlgInfo, BYTE* CipherTxt, DWORD CipherTxtLen, BYTE* PlainTxt, DWORD* PlainTxtLen);

	RET_VAL ECB_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL CBC_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL OFB_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen);
	RET_VAL CFB_DecFinal(SEED_ALG_INFO* AlgInfo, BYTE* PlainTxt, DWORD* PlainTxtLen);

	// Padding ����
	RET_VAL PaddSet(BYTE* pbOutBuffer, DWORD dRmdLen, DWORD dBlockLen, DWORD dPaddingType);
	RET_VAL PaddCheck(BYTE* pbOutBuffer, DWORD dBlockLen, DWORD dPaddingType);

public:
	AuSeed();
	virtual ~AuSeed();

	void	Initialize(void* pctx, BYTE* key, DWORD lKeySize, BOOL bClone = FALSE);
	DWORD	Encrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);
	void	Decrypt(void* pctx, BYTE* pInput, BYTE* pOutput, DWORD lSize);

	DWORD	GetOutputSize(DWORD lInputSize);
};

#endif	//_AUSEED_H_
