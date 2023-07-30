#ifdef _AREA_JAPAN_
#include <windows.h>
#include <stdio.h>
#include "AuJapaneseClientAuth.h"
#include "HanAuthForClient.h"

CJapaneseClientAuth g_jAuth;

CJapaneseClientAuth::CJapaneseClientAuth()
:	m_autoLogin( true )
{
	m_gamestring.resize( SIZE_GAMESTRING );
	m_memberID.resize( 64 );
	m_authString.resize( SIZE_AUTHSTRING );
	ZeroMemory(m_BillNo,32);
}

bool CJapaneseClientAuth::Init( char* cmdLine )
{
	_snprintf_s( &m_gamestring[0], SIZE_GAMESTRING, _TRUNCATE, "%s", GetSecondParam( cmdLine ) );

	if ( !strncmp( &m_gamestring[0], "no_auto_login", 32 ) )
		m_autoLogin = false;
	else
	{
		int authResult = HanAuthInitGameString( &m_gamestring[0] );

		if ( HAN_AUTHCLI_OK == authResult )
		{
			authResult = HanAuthForClientGameString( &m_gamestring[0] );

			if ( HAN_AUTHCLI_OK == authResult )
			{
				authResult = HanAuthGetId( &m_gamestring[0], &m_memberID[0], 128 );
			}
		}

		if ( HAN_AUTHCLI_OK != authResult )
		{
			MessageBox( NULL, HanAuthErrorPrint( authResult ), "Hangame Authentication", MB_OK );
			return false;
		}
	}
	return true;
}

char* CJapaneseClientAuth::HanAuthErrorPrint( int errorCode )
{
	switch ( errorCode )
	{
		case HAN_AUTHCLI_ARGUMENT_INVALID:
			return "?�B���ꂽ���q������������܂���B";
		case HAN_AUTHCLI_INITED_ALREADY:
			return "�N���C�A���g���f��?�������łɏ���������Ă��܂��B";
		case HAN_AUTHCLI_INITED_NOT:
			return "�N���C�A���g���f��?��������������Ă��܂���B";
		case HAN_AUTHCLI_INITED_FAIL:
			return "���f��?���������Ɏ��s���܂����B";
		case HAN_AUTHCLI_AUTHHTTP_INITFAIL:
			return "HTTP�������Ɏ��s���܂����B";
		case HAN_AUTHCLI_AUTHHTTP_CONNFAIL:
			return "HTTP�A�N�Z�X�Ɏ��s���܂����B";
		case HAN_AUTHCLI_REFRESHHTTP_INITFAIL:
			return "HTTP������(refresh)�Ɏ��s���܂����B";
		case HAN_AUTHCLI_REFRESHHTTP_CONNFAIL:
			return "HTTP�A�N�Z�X(refresh)�Ɏ��s���܂����B";
		case HAN_AUTHCLI_NOT_IMPLEMENTED:
			return "�܂���������ĂȂ�??�ł��B";
		case HAN_AUTHCLI_AUTHHTTP_OPENREQFAIL:
			return "HTTP���N�G�X�g�I?�v���Ɏ��s���܂����B";
		case HAN_AUTHCLI_AUTHHTTP_SENDREQFAIL:
			return "���N�G�X�gSend�Ɏ��s���܂����B";
		case HAN_AUTHCLI_COOKIE_SETFAIL:
			return "�N�b�L?�icookie�j�Z�b�e�B���O�Ɏ��s���܂����B";
		case HAN_AUTHCLI_GAMESTRING_IDINVALID:
			return "?�B���ꂽ�Q??�X�g�����O������������܂���B";
		case HAN_AUTHCLI_GAMESTRING_USNINVALID:
			return "USN������������܂���B";
		case HAN_AUTHCLI_GAMESTRING_GAMEIDINVALID:
			return "GAMEID������������܂���B";
		case HAN_AUTHCLI_GAMESTRING_SERVICECODEINVALID:
			return "servicecode������������܂���B";
		//case HAN_AUTHCLI_GAMESTRING_VALUENOTEXIST:
		//	return "gamestring�����݂��܂���B";
		case HAN_AUTHCLI_DLL_UNEXPECTED:
			return "?�z����Ă��Ȃ��G��?���������܂����B";
		case HAN_AUTHCLI_PARAM_INVALID:
			return "�E�F�u�T?�o?��?�B���ꂽ���q���K�؂ł͂���܂���B";
		case HAN_AUTHCLI_IPADDR_INVALID:
			return "IP�A�h���X���Ԉ���Ă��܂��B";
		case HAN_AUTHCLI_MEMBERID_INVALID:
			return "�g�p��ID���Ԉ���Ă��܂��B";
		case HAN_AUTHCLI_PASSWORD_INCORRECT:
			return "�g�p�҃p�X��?�h���Ԉ���Ă��܂��B";
		case HAN_AUTHCLI_PASSWORD_MISMATCHOVER:
			return "�p�X��?�h����?�X�񐔐������I?�o?���܂����B";
		case HAN_AUTHCLI_MEMBERID_NOTEXIST:
			return "���݂��Ȃ��g�p�҂ł��B";
		case HAN_AUTHCLI_SYSTEM_ERROR:
			return "System Error.";
		case HAN_AUTHCLI_COOKIE_SETERROR:
			return "Wrong Cookie";
		default:
			return "?�z����Ă��Ȃ��G��?���������܂����B";
	}
}

char* CJapaneseClientAuth::GetSecondParam( char* szCmdLine, int length )
{
	bool quote = false;
	int  index = 0;

	int loop = std::min<int>( length, (int) strlen(szCmdLine) );

	for ( ; index < length; ++index )
	{
		if ( szCmdLine[index] == 0 )
			break;

		if ( szCmdLine[index] == '\"' )
			quote = !quote;

		if ( !quote && szCmdLine[index] == ' ' )
			break;
	}

	if ( length > index )
		return ( szCmdLine + index + 1 );
	else
		return 0;
}

void CJapaneseClientAuth::Refresh()
{
	HanAuthRefresh( &m_memberID[0] );
}

bool CJapaneseClientAuth::UpdateGamestring( char* newGamestring )
{
	int authResult = UpdateGameString( &m_gamestring[0], newGamestring, SIZE_GAMESTRING );

	if ( HAN_AUTHCLI_OK == authResult ) 
		return true;
	
	HanAuthErrorPrint( authResult );

	return false;
}

const bool CJapaneseClientAuth::IsRealService( void )
{
	bool bResult = true;

	int serviceType = 0;

	HanAuthGetServiceTypeCode( &m_gamestring[0], &serviceType );

	bResult = (serviceType == SERVICE_REAL);

	return bResult;
}

const char* CJapaneseClientAuth::GetAuthString( void )
{
	::GetAuthString( &m_authString[0], static_cast<int>(m_authString.size()) );
	return &m_authString[0];
}
#endif //_AREA_JAPAN_