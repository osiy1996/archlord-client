#include <AuZpackLoader/AuZpackLoader.h>
#include <exception>
#include <set>
#include <algorithm>
#include <stdio.h>
#include <AuStringConv/AuStringConv.h>

//////////////////////////////////////////////////////////////////////////////
// dll 에서 사용할 메모리풀

namespace ZpackUtil
{
	typedef umtl::static_pool::string string;
	typedef umtl::static_pool::wstring wstring;

	//-----------------------------------------------------------------------
	// 실행파일 리버싱으로 패스워드 문자열을 후킹하는것을 방지하기위해
	// 패스워드를 암호화

	inline void encrypt( wstring & src )
	{
		if( src.empty() )
			return;

		wchar_t * str = (wchar_t*)src.c_str();

		wchar_t len = (wchar_t)src.length();

		for( size_t i=0; i<src.length(); ++i )
		{
			str[i] = ~str[i];
			str[i] ^= len;
		}

		char * tmp = (char*)str;
		std::reverse( tmp, tmp+src.length() );
		std::reverse( tmp+src.length(), tmp+src.length()*2 );
	}

	inline void decrypt( wstring & src )
	{
		if( src.empty() )
			return;

		wchar_t * str = (wchar_t*)src.c_str();

		wchar_t len = (wchar_t)src.length();

		char * tmp = (char*)str;
		std::reverse( tmp, tmp+src.length() );
		std::reverse( tmp+src.length(), tmp+src.length()*2 );

		for( size_t i=0; i<src.length(); ++i )
		{
			str[i] ^= len;
			str[i] = ~str[i];
		}
	}

	inline string decrypt( wchar_t const * s )
	{
		if( !s ) return "";

		wstring src = s;

		decrypt(src);

		char result[1024] = {0,};

		WideCharToMultiByte( CP_ACP, 0, src.c_str(), (int)src.length(), result, sizeof(result), 0, 0 );

		return result;
	}

	//-----------------------------------------------------------------------
	//
	/*
	struct mat_t
	{
		typedef float elem_type;

		elem_type a, b, c, d, end;

		mat_t() : a(0), b(0), c(0), d(0), end(0) {}

		inline mat_t& operator!()	{
			elem_type discriminant = a*d - b*c;

			if( discriminant != 0 )
			{
				elem_type t = a;
				a = d;
				d = t;

				b = -b;
				c = -c;

				*this *= (1/discriminant);
			}

			return *this;
		}

		inline mat_t & operator*=(elem_type v)	{
			a *= v;	b *= v;	c *= v;	d *= v;
			return *this;
		}

		std::string str()
		{
			char s[5];

			s[0] = (char)floor(a+0.5f);
			s[1] = (char)floor(b+0.5f);
			s[2] = (char)floor(c+0.5f);
			s[3] = (char)floor(d+0.5f);
			s[4] = 0;

			return s;
		}
	};

	//-----------------------------------------------------------------------
	// 어셈블리 리버싱으로 dll 이름과 함수이름을 체크하여
	// 호출시점에 패스워드를 후킹하는것을 방지하기 위해
	// dll 이름과 로딩할 함수이름들을 암호화

	inline std::wstring StrEncoding( std::wstring str )
	{
		if( str.empty() )
			return str;

		typedef mat_t::elem_type elem_type;

		size_t bufLen = str.length() * sizeof( elem_type ) + 1;

		std::wstring result( bufLen, 0 );

		wchar_t * buf = (wchar_t*)result.c_str();

		for( size_t i=0; i<(str.length()/4); ++i )
		{
			mat_t m;

			m.a = (elem_type)str[i*4+0];
			m.b = (elem_type)str[i*4+1];
			m.c = (elem_type)str[i*4+2];
			m.d = (elem_type)str[i*4+3];

			m = !m;

			elem_type * fbuf = (elem_type*)buf;

			fbuf[i*4+0] = m.a;
			fbuf[i*4+1] = m.b;
			fbuf[i*4+2] = m.c;
			fbuf[i*4+3] = m.d;
		}

		for( size_t i=0; i<(str.length()%4); ++i )
		{
			elem_type * fbuf = (elem_type*)buf;
			fbuf[ str.length() - 1 - i ] = str[ str.length() - 1 - i ];
		}

		for( size_t i=0; i<result.length(); ++i )
		{
			result[i] ^= (wchar_t)result.length();
			result[i] = ~result[i];
		}

		return result;
	}

	inline std::string StrDecoding( std::wstring str )
	{
		if( str.empty() )
			return "";

		for( size_t i=0; i<str.length(); ++i )
		{
			str[i] ^= (wchar_t)str.length();
			str[i] = ~str[i];
		}

		typedef mat_t::elem_type elem_type;

		size_t d = (str.length()/sizeof(elem_type));
		size_t n = d%4;

		size_t bufLen = d + n;

		std::wstring result( bufLen, 0 );
		wchar_t * buf = (wchar_t*)result.c_str();

		elem_type * s = (elem_type*)str.c_str();

		for( size_t i=0; i<(d/4); ++i )
		{
			mat_t m;

			m.a = s[i*4+0];
			m.b = s[i*4+1];
			m.c = s[i*4+2];
			m.d = s[i*4+3];

			m = !m;

			memcpy_s( buf+i*4, 8, m.str().c_str(), 8 );
		}

		for( size_t i=0; i < n; ++i )
			buf[d-n+i] = (wchar_t)s[d-n+i];

		//return result;

		char r[1024] = {0,};

		wchar_t const * tt= result.c_str();

		WideCharToMultiByte( CP_ACP, 0, result.c_str(), -1, r, sizeof(r), 0, 0 );

		return r;
	}
	*/

	//-----------------------------------------------------------------------
}

//////////////////////////////////////////////////////////////////////////////
//

AuZpackLoader::AuZpackLoader()
	: hDll_(0)
	, find_(0)
	, release_(0)
	, setAllocator_(0)
	, setTmpAllocator_(0)
	, folderInfo_(0)
	, clearCache_(0)
	, del_(0)
	, add_(0)
	, extract_(0)
{
	// Zpack.dll 로딩
	hDll_ = LoadLibrary( ZpackUtil::decrypt(L"ﾝﾕﾗﾆﾬﾚﾚﾒ￘").c_str() );

	if( hDll_ )
	{
		find_				 = (Find)			GetProcAddress( hDll_, ZpackUtil::decrypt( L"ﾝﾕﾗﾆﾬﾒﾘﾟﾰ" ).c_str() );
		release_			 = (Release)		GetProcAddress( hDll_, ZpackUtil::decrypt( L"ꇿ飿郿鋿菿꧿雿胿鋿雿鿿雿" ).c_str() );
		folderInfo_			 = (FolderInfo)		GetProcAddress( hDll_, ZpackUtil::decrypt(L"ﾜﾟﾶﾛﾓﾑﾀﾪﾟﾖﾞﾹﾂﾕﾔ").c_str() );
		clearCache_			 = (ClearCache)		GetProcAddress( hDll_, ZpackUtil::decrypt( L"ﾕﾜﾳﾛﾓﾑﾀﾪﾕﾘﾓﾑﾳﾂﾑ" ).c_str() );
		del_				 = (Del)			GetProcAddress( hDll_, ZpackUtil::decrypt( L"ﾰﾟﾗﾕﾄﾮﾑﾀﾑﾘﾑ" ).c_str() );
		add_				 = (Add)			GetProcAddress( hDll_, ZpackUtil::decrypt( L"铿雿蟿귿鏿鏿뛿鳿" ).c_str() );
		extract_			 = (Extract)		GetProcAddress( hDll_, ZpackUtil::decrypt( L"뛿飿郿鋿菿꧿蟿郿鋿臿蟿诿" ).c_str() );
	}


#ifdef _AREA_TAIWAN_
	SetPassword( L"ﾘﾃﾋﾆﾈﾃﾔﾅﾊﾝﾅﾏﾁﾋﾮﾜﾁ" ); // test
#endif
}

//////////////////////////////////////////////////////////////////////////////
//

AuZpackLoader::~AuZpackLoader()
{
	if( hDll_ )
		FreeLibrary( hDll_ );
}

//////////////////////////////////////////////////////////////////////////////
//

AuZpackLoader & AuZpackLoader::Instance()
{
	static AuZpackLoader inst;
	return inst;
}

//////////////////////////////////////////////////////////////////////////////
//

void AuZpackLoader::SetPassword( wchar_t const * password )
{
	password_ = password ? password : L"";
}

//////////////////////////////////////////////////////////////////////////////
//

umtl::static_pool::wstring AuZpackLoader::GetPassword()
{
	umtl::static_pool::wstring pw = password_;
	ZpackUtil::decrypt( pw );
	return pw;
}

//////////////////////////////////////////////////////////////////////////////
//

typedef std::set< std::wstring > ZpackList;

static ZpackList zpackList;

//static ZpackUtil::MemoryPool pool;

//////////////////////////////////////////////////////////////////////////////
//

void ReleaseZpackPool()
{
	std::for_each( zpackList.begin(), zpackList.end(), &ClearZpackCache );

	//pool.release();
}

//////////////////////////////////////////////////////////////////////////////
//

BYTE * GetZpackStream( wchar_t * zpackFilename, const wchar_t * file, size_t & size )
{
	if( !zpackFilename || !file || !AuZpackLoader::Instance().find_ )
		return 0;

	umtl::static_pool::wstring filename = file;

	//-----------------------------------------------------------------------
	// 절대경로인경우 상대경로로 바꾸고 불필요한 문자 제거
	/*
	static std::string curDir;
	if( curDir.empty() )
	{
	char t[1024] = {0,};
	GetCurrentDirectory( 1024 , t );
	curDir = t;
	}

	size_t idx = tmp.find( curDir );

	if( idx == 0 )
	{
	tmp.erase( idx, curDir.length() );
	}
	*/

	while( !filename.empty() && (filename[0] == '.' || filename[0] == '\\' || filename[0] == '/') )
	{
		filename.erase( 0, 1 );
	}

	unsigned int s = 0;

	umtl::static_pool::wstring pw = AuZpackLoader::Instance().GetPassword();

	unsigned char * result = AuZpackLoader::Instance().find_( zpackFilename, (wchar_t *)filename.c_str(), s, (wchar_t*)pw.c_str() );

	size = s;

	return result;
}

BYTE * GetZpackStream( wchar_t * zpackFilename, const char * file, size_t & size )
{
	if( !zpackFilename || !file )
		return 0;

	AuZpackLoader & loader = AuZpackLoader::Instance();

	if( !loader.find_ )
		return 0;

	//-----------------------------------------------------------------------
	//

	umtl::static_pool::string tmp = file;

	wchar_t filename[1024] = {0,};

	if( !tmp.empty() )
	{
		//-----------------------------------------------------------------------
		// 멀티바이트를 유니코드로

		if( !tmp.empty() )
			MultiByteToWideChar(949, MB_PRECOMPOSED, tmp.c_str(), (int)tmp.length(), filename, sizeof(filename) / sizeof(wchar_t) );
	}

	return GetZpackStream( zpackFilename, &(filename[0]), size );
}

//////////////////////////////////////////////////////////////////////////////
//

void ReleaseZpackStream( wchar_t * zpackFileName, void * stream )
{
	if( zpackFileName && stream )
	{
		if( AuZpackLoader::Instance().release_ )
			AuZpackLoader::Instance().release_( zpackFileName, (wchar_t*)stream );
	}
}

//////////////////////////////////////////////////////////////////////////////
//

void ClearZpackCache( std::wstring zpackFileName )
{
	if( !zpackFileName.empty() )
	{
		if( AuZpackLoader::Instance().clearCache_ )
			AuZpackLoader::Instance().clearCache_( const_cast<wchar_t *>( zpackFileName.c_str() ) );
	}
}

void ClearZpackCacheAll()
{
	ClearZpackCache( L"character.zp" );
	ClearZpackCache( L"effect.zp" );
	ClearZpackCache( L"ini.zp" );
	ClearZpackCache( L"object.zp" );
	ClearZpackCache( L"texture.zp" );
	ClearZpackCache( L"world.zp" );

	ClearZpackCache( L"tEtc.zp" );
	ClearZpackCache( L"tWorld.zp" );
	ClearZpackCache( L"tObject.zp" );
	ClearZpackCache( L"tCharacter1.zp" );
	ClearZpackCache( L"tCharacter2.zp" );
	ClearZpackCache( L"tMinimap.zp" );
	ClearZpackCache( L"tSkill.zp" );
	ClearZpackCache( L"tWorldMap.zp" );
	ClearZpackCache( L"tUI.zp" );
	ClearZpackCache( L"tBase.zp" );
	ClearZpackCache( L"tEffect.zp" );

	umtl::clear();
}

//////////////////////////////////////////////////////////////////////////////
//

wchar_t const * GetZpackFolderInfo( wchar_t * zpackFileName, const char * folderPath )
{
	wchar_t const * result = 0;

	if( zpackFileName )
	{
		if( AuZpackLoader::Instance().folderInfo_ )
		{
			umtl::static_pool::string tmp = folderPath;

			wchar_t path[1024] = {0,};

			if( !tmp.empty() )
			{
				while( !tmp.empty() && (tmp[0] == '.' || tmp[0] == '\\' || tmp[0] == '/') )
				{
					tmp.erase( 0, 1 );
				}

				//-----------------------------------------------------------------------
				// 멀티바이트를 유니코드로

				if( !tmp.empty() )
					MultiByteToWideChar(CP_ACP, MB_COMPOSITE, tmp.c_str(), (int)tmp.length(), path, sizeof(path) / sizeof(wchar_t) );
			}

			umtl::static_pool::wstring pw = AuZpackLoader::Instance().GetPassword();

			result = AuZpackLoader::Instance().folderInfo_( zpackFileName, path, pw.c_str() );
		}
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////