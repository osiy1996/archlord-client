#include "AuNPGameLib.h"

#ifndef _WIN64 // 클라에서만 사용하는 클래스

#include <ApBase/MagDebug.h>
#include <AuIgnoringHelper/AuIgnoringHelper.h>
#include <AuStrTable/AuStrTable.h>
#include <AuParallel/thread.hpp>

//-----------------------------------------------------------------------
//

AuNPWrapper & GetNProtect()
{
	static AuNPWrapper inst;
	return inst;
}

//-----------------------------------------------------------------------
//

AuNPWrapper::AuNPWrapper()
	: m_pNPGameLib(NULL)
	, inited_(false)
{
	// 릴리즈 디버깅을 위해서 게임가드 스킵 설정파일을 추가
	if( AuIgnoringHelper::IgnoreToFile( "NotCheckGameGuard.arc" ) )
	{

		m_pNPGameLib = new AuNPBase();

	}
	else
	{

#if defined( _USE_NPROTECT_GAMEGUARD_ ) && !defined( _DEBUG )

		m_pNPGameLib = new AuNPGameLib();

#else

		m_pNPGameLib = new AuNPBase();

#endif

	}

	inited_ = m_pNPGameLib->Init() ? true : false;
}

//-----------------------------------------------------------------------
//

AuNPWrapper::~AuNPWrapper()
{
	if(m_pNPGameLib)
		delete m_pNPGameLib;
}

//-----------------------------------------------------------------------
//

bool AuNPWrapper::Init()
{
	return inited_;
}

//-----------------------------------------------------------------------
//

DWORD AuNPWrapper::Send(LPCTSTR lpszUserId)
{
	return m_pNPGameLib->Send( lpszUserId );
}

//-----------------------------------------------------------------------
//

CHAR* AuNPWrapper::GetMessage(DWORD dwResult)
{
	return m_pNPGameLib->GetMessage(dwResult);
}

//-----------------------------------------------------------------------
//

BOOL AuNPWrapper::NPGameMonCallback( DWORD dwMsg, DWORD dwArg )
{
	return m_pNPGameLib->NPGameMonCallback(dwMsg, dwArg);
}

//-----------------------------------------------------------------------
//

BOOL AuNPWrapper::Check()
{
	return m_pNPGameLib->Check();
}

//-----------------------------------------------------------------------
//

void AuNPWrapper::SetWndHandle( HWND hWnd )
{
	m_pNPGameLib->SetWndHandle(hWnd);
}

//-----------------------------------------------------------------------
//

void AuNPWrapper::Auth( PVOID pggAuthData )
{
	m_pNPGameLib->Auth(pggAuthData);
}

#ifdef _USE_NPROTECT_GAMEGUARD_

//-----------------------------------------------------------------------
//

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg)
{
	return GetNProtect().NPGameMonCallback(dwMsg, dwArg);
}

//-----------------------------------------------------------------------
//

AuNPGameLib::AuNPGameLib()
: m_pNPGameLib(NULL)
{
}

//-----------------------------------------------------------------------
//

AuNPGameLib::~AuNPGameLib()
{
	if( m_pNPGameLib )
		delete m_pNPGameLib;
}

//-----------------------------------------------------------------------
//

BOOL AuNPGameLib::Init()
{
	CHAR	szFileName[ MAX_PATH ];

	CHAR * localToken = "";

	switch( g_eServiceArea )
	{
	case AP_SERVICE_AREA_TAIWAN :
		localToken = "TW";
		break;
	case AP_SERVICE_AREA_GLOBAL :
		localToken = "GB";
		break;
	case AP_SERVICE_AREA_KOREA :
		localToken = "KR";
		break;
	case AP_SERVICE_AREA_JAPAN :
		localToken = "JP";
		break;
	}

	sprintf_s( szFileName, sizeof(szFileName), "Archlord%sTest", localToken );

	FILE*	pFile	=	fopen( (std::string(szFileName) + ".ini").c_str() , "rb" );
	if( pFile )
		sprintf_s( szFileName , MAX_PATH , "Archlord%sTest", localToken );
	else
		sprintf_s( szFileName , MAX_PATH , "Archlord%s", localToken );

	if( pFile )
		fclose( pFile );

	m_pNPGameLib = new CNPGameLib( szFileName );

	if(!m_pNPGameLib)
		return FALSE;

	DWORD dwResult = m_pNPGameLib->Init();
	if( dwResult != NPGAMEMON_SUCCESS)
	{
		CHAR strMessage[256] = { 0, };
		sprintf_s( strMessage, sizeof(strMessage), "%s", GetMessage(dwResult));
		MessageBox(NULL, strMessage, "GameGuard Error", MB_OK);
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------
//

CHAR* AuNPGameLib::GetMessage(DWORD dwResult)
{
	char* pMsg = 0;

	switch (dwResult)
	{
	case NPGAMEMON_ERROR_EXIST:
		pMsg = ClientStr().GetStr(STI_NP_EXIST);
		break;
	case NPGAMEMON_ERROR_GAME_EXIST:
		pMsg = ClientStr().GetStr(STI_NP_GAME_EXIST);
		break;
	case NPGAMEMON_ERROR_INIT:
		pMsg = ClientStr().GetStr(STI_NP_ERROR_INIT);
		break;
	case NPGAMEMON_ERROR_AUTH_GAMEGUARD:
	case NPGAMEMON_ERROR_NFOUND_GG:
	case NPGAMEMON_ERROR_AUTH_INI:
	case NPGAMEMON_ERROR_NFOUND_INI:
		pMsg = ClientStr().GetStr(STI_NP_NOT_FOUND);
		break;
	case NPGAMEMON_ERROR_CRYPTOAPI:
		pMsg = ClientStr().GetStr(STI_NP_CRYPTOAPI);
		break;
	case NPGAMEMON_ERROR_EXECUTE:
		pMsg = ClientStr().GetStr(STI_NP_EXECUTE_FAIL);
		break;
	case NPGAMEMON_ERROR_ILLEGAL_PRG:
		pMsg = ClientStr().GetStr(STI_NP_ILLEGAL_PRO);
		break;
	case NPGMUP_ERROR_ABORT:
		pMsg = ClientStr().GetStr(STI_NP_ABORT_UPDATE);
		break;
	case NPGMUP_ERROR_CONNECT:
		pMsg = ClientStr().GetStr(STI_NP_CONNECT_FAIL);
		break;
		//case HOOK_TIMEOUT:
		//	pMsg = ClientStr().GetStr(STI_NP_TIMEOUT);
		//	break;
	case NPGAMEMON_ERROR_GAMEGUARD:
		pMsg = ClientStr().GetStr(STI_NP_GAMEGUARD);
		break;
		//case NPGMUP_ERROR_PARAM:
		//	pMsg = ClientStr().GetStr(STI_NP_NO_INI);
		//	break;
		//case NPGMUP_ERROR_INIT:
		//	pMsg = ClientStr().GetStr(STI_NP_NPGMUP);
		//	break;
	case NPGMUP_ERROR_DOWNCFG:
		pMsg = ClientStr().GetStr(STI_NP_DOWNCFG);
		break;
	case NPGMUP_ERROR_AUTH:
		pMsg = ClientStr().GetStr(STI_NP_AUTH_FAIL);
		break;
	case NPGAMEMON_ERROR_NPSCAN:
		pMsg = ClientStr().GetStr(STI_NP_NPSCAN_FAIL);
		break;
	default:
		pMsg = ClientStr().GetStr(STI_NP_DEFAULT);
		break;
	}

	return pMsg;
}

//-----------------------------------------------------------------------
//

BOOL AuNPGameLib::Check()
{
	DWORD dwResult = m_pNPGameLib->Check();
	if(dwResult != NPGAMEMON_SUCCESS)
	{
		return FALSE;
	}

	return TRUE;
}

//-----------------------------------------------------------------------
//

DWORD AuNPGameLib::Send( LPCTSTR lpszUserId )
{
	return m_pNPGameLib->Send(lpszUserId);
}

//-----------------------------------------------------------------------
//

void AuNPGameLib::SetWndHandle( HWND hWnd )
{
	m_pNPGameLib->SetHwnd(hWnd);
}

//-----------------------------------------------------------------------
//

void AuNPGameLib::Auth( PVOID pggAuthData )
{
	GG_AUTH_DATA ggData;
	memcpy(&ggData, pggAuthData, sizeof(GG_AUTH_DATA));
	m_pNPGameLib->Auth2(&ggData);
}

//-----------------------------------------------------------------------
//

BOOL AuNPGameLib::NPGameMonCallback( DWORD dwMsg, DWORD dwArg )
{
	BOOL bReturn = FALSE;

	CHAR hackMsg[1024] = {0, };

	switch (dwMsg)
	{
	case NPGAMEMON_COMM_ERROR:
		{
			sprintf_s( hackMsg, sizeof(hackMsg), ClientStr().GetStr(STI_NP_EXECUTE_FAIL) );
			break;
		}
	case NPGAMEMON_COMM_CLOSE:
		{			
		} break;
	case NPGAMEMON_INIT_ERROR:
		{
			sprintf_s(hackMsg, sizeof(hackMsg), "%s : %lu", ClientStr().GetStr(STI_NP_INIT_ERROR), dwArg);
		} break;
	case NPGAMEMON_SPEEDHACK:
		{
			sprintf_s(hackMsg, sizeof(hackMsg), ClientStr().GetStr(STI_NP_SPEEDHACK));
		} break;
	case NPGAMEMON_GAMEHACK_KILLED:
		{
			sprintf_s(hackMsg, sizeof(hackMsg), "%s\r\n%s", ClientStr().GetStr(STI_NP_HACK_KILLED), "");
		} break;
	case NPGAMEMON_GAMEHACK_DETECT:
		{
			sprintf_s(hackMsg, sizeof(hackMsg), "%s\r\n%s", ClientStr().GetStr(STI_NP_HACK_DETECT), "");
		} break;
	case NPGAMEMON_GAMEHACK_DOUBT:
		{
			sprintf_s(hackMsg, sizeof(hackMsg), ClientStr().GetStr(STI_NP_HACK_DOUBT));
		} break;
	case NPGAMEMON_GAMEHACK_REPORT:
		{
			sprintf_s(hackMsg, sizeof(hackMsg), ClientStr().GetStr(STI_NP_ILLEGAL_PRO));
			//bReturn = TRUE;
		} break;
	case NPGAMEMON_CHECK_CSAUTH2:
		{
#ifndef USE_MFC
			NPGameMonSend(dwArg);
#endif
			bReturn = TRUE;
		} break;
	}

	if( !bReturn )
	{
		struct MsgThread {
			std::string msg;
			MsgThread( const char * m ) : msg( m ? m : "" ) {}
			void operator()() {
				MessageBox(NULL, msg.c_str(), "GameGuard", MB_OK);
			}
		};

		static parallel::thread< MsgThread > msgThread = MsgThread( hackMsg );

		//throw "detect gamehack";
	}

	return bReturn;
}

//-----------------------------------------------------------------------

#endif

#endif // !_WIN64