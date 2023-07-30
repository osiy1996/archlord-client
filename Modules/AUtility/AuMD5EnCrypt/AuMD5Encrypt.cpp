#include <AuMD5EnCrypt/AuMD5EnCrypt.h>

#ifdef _AP_MEMORY_TRACKER_
#include <ApMemoryTracker/ApMemoryTracker.h>
#endif

char *g_pstrCryptContainer = "MyKeyContainer";  

AuMD5Encrypt::AuMD5Encrypt()
{
	Initialize();
}

AuMD5Encrypt::~AuMD5Encrypt()
{
	CleanUp();
}

bool AuMD5Encrypt::Initialize()
{
	bool			bResult;

	bResult = true;

	CryptSetProvider( MS_DEF_PROV, PROV_RSA_FULL );

	if(!CryptAcquireContext( &m_hCryptProv, NULL, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT ) )
	{
		DWORD			lError;
		lError = GetLastError();

		FILE		*file;

		file = fopen( "errorcode.txt", "ab" );
		
		if( file != NULL )
		{
			fwrite( &lError, 1, sizeof( lError ), file );

			fclose( file );
		}

		bResult = false;
		return bResult;
	}

	return bResult;
}

bool AuMD5Encrypt::CleanUp()
{
	if(m_hCryptProv) 
	   CryptReleaseContext(m_hCryptProv, 0);

	return true;
}

//-----------------------------------------------------------------------
//

std::string AuMD5Encrypt::GetMD5Hash( char const * hashString )
{
	//-----------------------------------------------------------------------
	//

	struct MD5Hash
	{
		MD5Hash( HCRYPTPROV & algorythm ) : algorythm(algorythm), hash_handle(0)
		{
			//Hash를 만든다.
			if( !CryptCreateHash( algorythm, CALG_MD5, 0, 0, &hash_handle) )
				hash_handle = 0;
		}

		~MD5Hash() {
			if( hash_handle )
				CryptDestroyHash( hash_handle );
		}

		std::string operator()( char const * hashString )
		{
			std::string result;

			if( !hashString )
				result;

			BYTE md5Hash[16] = {0,};

			DWORD hashLen = sizeof(md5Hash);

			if( !hash_handle )
				return result;

			if( CryptHashData( hash_handle, (BYTE*)hashString, (DWORD)strlen(hashString), 0 ) )
				CryptGetHashParam( hash_handle, HP_HASHVAL, md5Hash, &hashLen, 0  );

			if( hashLen <= 16 )
			{
				char tmp[256] = {0,};
				for( DWORD i=0; i<hashLen; ++i )
				{
					sprintf_s( tmp, "%02X", md5Hash[i] );
					result += tmp;
				}
			}

			return result;
		}

		HCRYPTPROV & algorythm;
		HCRYPTKEY hash_handle;
	};

	//-----------------------------------------------------------------------
	//

	return MD5Hash( m_hCryptProv )( hashString );

	//-----------------------------------------------------------------------
}

//-----------------------------------------------------------------------
//

bool AuMD5Encrypt::EncryptString( char *pstrHashString, char *pstrData, unsigned long istrSize )
{
	HCRYPTKEY		hKey; 
	HCRYPTHASH		hHash; 

	bool			bResult;

	bResult = true;

	//Hash를 만든다.
	if(!CryptCreateHash( m_hCryptProv, CALG_MD5, 0, 0, &hHash))
	{
		bResult = false;
		return bResult;
	}  

	//Hash Object에 Hash Data를 넣는다.
	if(!CryptHashData( hHash, (BYTE *)pstrHashString, (DWORD)strlen(pstrHashString), 0))
	{
		bResult = false;
		return bResult;
	}

	//Key를 뽑아낸다.
	if(!CryptDeriveKey( m_hCryptProv, ENCRYPT_ALGORITHM, hHash, KEYLENGTH, &hKey))
	{
		bResult = false;
		return bResult;
	}

	//Key를 뽑아냈으니 Hash OBJ를 초기화한다.
	CryptDestroyHash(hHash); 
	hHash = 0;

    //Key를 이용해 pstrData를 Encoding한다.
	//더이상 Encoding할게 없으니 Final인자를 TRUE로....
	if(!CryptEncrypt( hKey, 0, FALSE, 0, (unsigned char *)pstrData, &istrSize, istrSize))
	{ 
		bResult = false;
		return bResult;
	}

	//Key를 더이상 사용할 일은 없으므로 없앤다.
	CryptDestroyKey( hKey );

	return bResult;
}

bool AuMD5Encrypt::DecryptString( char *pstrHashString, char *pstrData, unsigned long istrSize )
{
	HCRYPTKEY		hKey; 
	HCRYPTHASH		hHash; 

	bool			bResult;

	bResult = true;

	//Hash를 만든다.
	if(!CryptCreateHash( m_hCryptProv, CALG_MD5, 0, 0, &hHash))
	{
		bResult = false;
		return bResult;
	}  

	//Hash Object에 Hash Data를 넣는다.
	if(!CryptHashData( hHash, (BYTE *)pstrHashString, (DWORD)strlen(pstrHashString), 0))
	{
		bResult = false;
		return bResult;
	}

	//Key를 뽑아낸다.
	if(!CryptDeriveKey( m_hCryptProv, ENCRYPT_ALGORITHM, hHash, KEYLENGTH, &hKey))
	{
		bResult = false;
		return bResult;
	}

	//Key를 뽑아냈으니 Hash OBJ를 초기화한다.
	CryptDestroyHash(hHash); 
	hHash = 0;

	//더이상 Encoding할게 없으니 Final인자를 TRUE로....
	if(!CryptDecrypt( hKey, 0, FALSE, 0, (unsigned char *)pstrData, &istrSize))
	{ 
		bResult = false;
		return bResult;
	} 

	//Key를 더이상 사용할 일은 없으므로 없앤다.
	CryptDestroyKey( hKey );

	return bResult;
}

char *AuMD5Encrypt::GetRandomHashString( int iStringSize )
{
	char			*pstrHashString;

	pstrHashString = NULL;

	if( (0 < iStringSize) && (iStringSize<100000) )
	{
		pstrHashString = new char[iStringSize];
		memset( pstrHashString, 0, iStringSize );

		if( pstrHashString != NULL )
		{
			srand( timeGetTime() );

			int		limit1 = iStringSize-1;

			for( int i=0; i<limit1; ++i )
			{
				// 0이 나오지 못하게 한다.
				pstrHashString[i] = (char)((rand()%255) + 1);
			}
		}
	}

	return pstrHashString;
}
