#ifndef __AUMD5ENCRYPT_H__
#define __AUMD5ENCRYPT_H__


#include <Windows.h>
#include <wincrypt.h>
#define KEYLENGTH  CRYPT_CREATE_SALT

#define ENCRYPT_ALGORITHM CALG_RC4 


#ifdef _AREA_CHINA_
	#define MD5_HASH_KEY_STRING						"Protect_No_666_HevensDoor"
#else
	#define MD5_HASH_KEY_STRING						"1111"
#endif



#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AuMD5EnCryptD" )
#else
#pragma comment ( lib , "AuMD5EnCrypt" )
#endif
#endif

#include <string>
#include <stdio.h>

class AuMD5Encrypt
{
private:
	HCRYPTPROV		m_hCryptProv; 

	bool Initialize();
	bool CleanUp();

public:
	AuMD5Encrypt();
	~AuMD5Encrypt();
	bool EncryptString( char *pstrHashString, char *pstrData, unsigned long istrSize );
	bool DecryptString( char *pstrHashString, char *pstrData, unsigned long istrSize );
	char *GetRandomHashString( int iStringSize );

	std::string GetMD5Hash( char const * hashString );
};

#endif