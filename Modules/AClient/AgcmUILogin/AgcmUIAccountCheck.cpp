#include "AgcmUIAccountCheck.h"
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcmUILogin/AgcmUILogin.h>
#include <AgcmWorld/AgcmWorld.h>
#include <AuStrTable/AuStrTable.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmUIControl/AcUIEdit.h>
#include <AgcmUIControl/AcUIButton.h>

#ifndef USE_MFC
	#include <AgcAuth/AgcAuth.h>
	#include <AuJapan/AuJapaneseClientAuth.h>

	#ifdef _AREA_KOREA_
		#include "AuHanAuth.h"
	#endif
#endif



AgcmUIAccountCheck::AgcmUIAccountCheck( void )
: m_pcmUILogin( NULL ), m_pChallengeString( NULL ), m_pcdUIUserDataKeyChallenge( NULL ),
	m_nEventKeyOpen( 0 ), m_nEventKeyClose( 0 ), m_nEventNotProtected( 0 ), m_nRetryCount( 0 )
{
	OnInitialize( NULL );
}

AgcmUIAccountCheck::~AgcmUIAccountCheck( void )
{
	OnInitialize( NULL );
}

BOOL AgcmUIAccountCheck::OnInitialize( void* pUILogin )
{
	if( !pUILogin ) return FALSE;
	m_pcmUILogin = pUILogin;
	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddEvent( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( pcmUIManager ) return FALSE;

	m_nEventKeyOpen = pcmUIManager->AddEvent( "LG_OpenEKey" );
	if( m_nEventKeyOpen < 0 ) return FALSE;

	m_nEventKeyClose = pcmUIManager->AddEvent( "LG_CloseEKey" );
	if( m_nEventKeyClose < 0 ) return FALSE;

	m_nEventNotProtected = pcmUIManager->AddEvent( "LG_NotIsProtected" );
	if( m_nEventNotProtected < 0 ) return FALSE;

	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddFunction( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	if( !pcmUIManager->AddFunction( this, "LG_IdPasswordOK",		CB_OnSubmit,	2, "ID_EDIT", "PASSWORD_EDIT" ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_IdPasswordCancel",	CB_OnCancel,	0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_IdPasswordEditTab",	CB_OnKeyTab,	2, "Pre_EDIT", "Next_EDIT" ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_EKeyOK",				CB_OnOpenKey,	1, "EKey_EDIT" ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_EKeyCancel",			CB_OnCloseKey,	0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_LoginToServer",		CB_OnSendLogin, 0 ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "LG_IdPasswordInit",		CB_OnClear,		2, "Id_Edit", "Password_Edit" ) ) return FALSE;
	if( !pcmUIManager->AddFunction( this, "EXIT_ApplicationQUIT",	CB_OnExitGame,	0 ) ) return FALSE;

	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddUserData( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	m_pcdUIUserDataKeyChallenge	= pcmUIManager->AddUserData( "EKEY_Challenge", m_pChallengeString, sizeof( CHAR* ), 5, AGCDUI_USERDATA_TYPE_STRING );
	if( !m_pcdUIUserDataKeyChallenge ) return FALSE;

	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddDisplay( void* pUIManager )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	if( !pcmUIManager->AddDisplay( this, "EKEY_Display_Challenge", 0, CB_OnDisplayChallenge, AGCDUI_USERDATA_TYPE_STRING ) ) return FALSE;
	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddCallBack( void* pLogin )
{
	AgcmLogin* pcmLogin = ( AgcmLogin* )pLogin;
	if( !pcmLogin ) return FALSE;

	if( !pcmLogin->SetCallbackEKeyActive( CB_OnActiveKey, this ) ) return FALSE;
	if( !pcmLogin->SetCallbackSignOnSuccess( CB_OnLoginOK, this ) ) return FALSE;

	if( !pcmLogin->SetCallbackConnectLoginServer( CB_OnConnect, this ) ) return FALSE;
	if( !pcmLogin->SetCallbackDisconnectLoginServer( CB_OnDisConnect, this ) ) return FALSE;

	return TRUE;
}

BOOL AgcmUIAccountCheck::OnAddLoginRetryCount( void* pUIManager )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pUIManager;
	if( !pcmUIManager ) return FALSE;

	++m_nRetryCount;
	if( m_nRetryCount < 5 )
	{
		pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_PASSWORD_INCORRECT ] );
	}
	else
	{
		pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_PASSWORD_FALIED ] );
	}

	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnClear( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcdUIControl* pEditAccount = ( AgcdUIControl* )pData1;
	AgcdUIControl* pEditPassword = ( AgcdUIControl* )pData2;
	if( !pEditAccount || !pEditPassword ) return FALSE;
	if( pEditAccount->m_lType != AcUIBase::TYPE_EDIT || pEditPassword->m_lType != AcUIBase::TYPE_EDIT ) return FALSE;

	( ( AcUIEdit* )pEditAccount->m_pcsBase )->ClearText();
	( ( AcUIEdit* )pEditPassword->m_pcsBase )->ClearText();

	if( g_pEngine )
	{
		g_pEngine->ReleaseMeActiveEdit( NULL );
	}

	( ( AcUIEdit* )pEditAccount->m_pcsBase )->SetMeActiveEdit();
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnSubmit( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
#ifndef USE_MFC
	#ifdef _AREA_JAPAN_
		if( g_jAuth.GetAutoLogin() )	
			return FALSE;
	#elif defined(_AREA_KOREA_)
		if( g_cHanAuthKor.IsExcute() ) 
			return FALSE;
	#elif !defined(_AREA_CHINA_)
		if( AuthManager().getProperty<bool>( "autologin" ) ) 
			return FALSE;
	#endif
#endif

	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmUIManager || !pcmLogin ) return FALSE;

	AgcdUIControl* pEditAccount = ( AgcdUIControl* )pData1;
	AgcdUIControl* pEditPassword = ( AgcdUIControl* )pData2;
	if( !pEditAccount || !pEditPassword ) return FALSE;
	if( pEditAccount->m_lType != AcUIBase::TYPE_EDIT || pEditPassword->m_lType != AcUIBase::TYPE_EDIT ) return FALSE;

	TCHAR* pTextAccount = ( CHAR* )( ( AcUIEdit* )pEditAccount->m_pcsBase )->GetText();
	TCHAR* pTextPassword = ( CHAR* )( ( AcUIEdit* )pEditPassword->m_pcsBase )->GetText();

	if( !pTextAccount || strlen( pTextAccount ) <= 0 ) return FALSE;
	if( !pTextPassword || strlen( pTextPassword ) <= 0 ) return FALSE;

	pcmLogin->SetIDPassword( pTextAccount, pTextPassword );
	pcmUILogin->ConnectLoginServer();

#ifdef _AREA_GLOBAL_
	(( AcUIButton* )pControl)->KillFocus();
#elif defined(_AREA_CHINA)
	if( g_pEngine )
	{
		g_pEngine->WaitingDialog( NULL, pcmUIManager->GetUIMessage( "LOGIN_WAITING" ) );
	}
#endif

	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnCancel( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnKeyTab( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcdUIControl* pEditAccount = ( AgcdUIControl* )pData1;
	AgcdUIControl* pEditPassword = ( AgcdUIControl* )pData2;
	if( !pEditAccount || !pEditPassword ) return FALSE;
	if( pEditAccount->m_lType != AcUIBase::TYPE_EDIT || pEditPassword->m_lType != AcUIBase::TYPE_EDIT ) return FALSE;

	( ( AcUIEdit* )( pEditAccount->m_pcsBase ) )->ReleaseMeActiveEdit();
	( ( AcUIEdit* )( pEditPassword->m_pcsBase ) )->SetMeActiveEdit();
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnSendLogin( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	pcmUILogin->ConnectLoginServer();
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnExitGame( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	RsEventHandler( rsQUITAPP, NULL );
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnOpenKey( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmLogin ) return FALSE;

	AgcdUIControl* pEdit = ( AgcdUIControl* )pData1;
	if( !pEdit || pEdit->m_lType != AcUIBase::TYPE_EDIT ) return FALSE;

	TCHAR* pKeyString = ( CHAR* )( ( AcUIEdit* )( pEdit->m_pcsBase ) )->GetText();
	pcmLogin->SendEKey( pKeyString, pcmLogin->m_nNID );

	( ( AcUIEdit* )( pEdit->m_pcsBase ) )->SetText( "" );
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnCloseKey( void* pClass, void* pData1, void* pData2, void* pData3, void* pData4, void* pData5, ApBase* pTarget, AgcdUIControl* pControl )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	pcmUIManager->ThrowEvent( pThis->m_nEventKeyClose );
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnConnect( void* pData, void* pClass, void* pCustomData )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

#ifndef USE_MFC
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmLogin ) return FALSE;

	if( !pData )
	{
		if( pcmLogin->GetLoginMode() == AGCMLOGIN_MODE_ID_PASSWORD_INPUT )
		{
			if( g_pEngine )
			{
				// Login���� �������� ���̻� �Ұ����ϸ� �����Ѵ�
				if( !pThis->ReConnectLoginServer() )
				{
					g_pEngine->WaitingDialogEnd();
					pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_CONNECT_FAILED_LOGIN_SERVER ] );

					if( g_eServiceArea == AP_SERVICE_AREA_JAPAN 
#ifdef _AREA_GLOBAL_
						|| g_eServiceArea == AP_SERVICE_AREA_GLOBAL
#elif defined(_AREA_KOREA_) 
						|| 	( g_eServiceArea == AP_SERVICE_AREA_KOREA && g_cHanAuthKor.IsExcute() ) 
#endif
						)
					{
						g_pEngine->OnTerminate();
					}
				}
			}
		}
	}
#endif

	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnDisConnect( void* pData, void* pClass, void* pCustomData )
{
	if( g_pEngine )
	{
		g_pEngine->WaitingDialogEnd();
	}

	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	BOOL* pbIsDestroyNormal = ( BOOL* )pCustomData;
	if( !pbIsDestroyNormal ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )pcmUILogin->GetModule( "AgcmCharacter" );
	if( !pcmCharacter ) return FALSE;

	if( !( *pbIsDestroyNormal ) && _tcslen( pcmCharacter->m_szGameServerAddress ) < 1 )
	{
		pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_DISCONNECTED_BY_SERVER ] );
	}

	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnActiveKey( void* pData, void* pClass, void* pCustomData )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	if( g_pEngine )
	{
		g_pEngine->WaitingDialogEnd();
	}

	pThis->m_pChallengeString = ( CHAR* )pData;
	pcmUIManager->SetUserDataRefresh( ( AgcdUIUserData* )pThis->GetUserDataKeyChallenge() );
	pcmUIManager->ThrowEvent( pThis->GetEKeyOpen() );
	return TRUE;
}

BOOL AgcmUIAccountCheck::CB_OnLoginOK( void* pData, void* pClass, void* pCustomData )
{
	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pcmUILogin->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcmLogin* pcmLogin = ( AgcmLogin* )pcmUILogin->GetModule( "AgcmLogin" );
	if( !pcmLogin ) return FALSE;

	AgcmWorld* pcmWorld = ( AgcmWorld* )pcmUILogin->GetModule( "AgcmWorld" );
	if( !pcmWorld ) return FALSE;
	if( !pcmWorld->SendPacketGetWorld( NULL, pcmLogin->m_nNID ) ) return FALSE;

	static bool bIsFirstCall = true;

	if( g_eServiceArea == AP_SERVICE_AREA_CHINA )
	{
		if( pcmLogin->m_lIsProtected == 0 && bIsFirstCall )
		{
			pcmLogin->m_lIsLimited = 0;
			pcmUIManager->ThrowEvent( pThis->GetENotProtected() );
		}
	}

	pcmWorld->SendPacketGetCharCount( pcmLogin->m_szAccount, pcmLogin->m_nNID );
	pcmUILogin->SetLoginMode( AGCMLOGIN_MODE_SERVER_SELECT );
	return pcmUIManager->ThrowEvent( pcmUILogin->m_lEvent[ AGCMUILOGIN_EVENT_ID_PASSWORD_INPUT_CLOSE ] );
}

BOOL AgcmUIAccountCheck::CB_OnDisplayChallenge( void* pClass, void* pData, AgcdUIDataType eType, INT32 nID, CHAR* pDisplay, INT32* pValue )
{
	if( !pDisplay ) return FALSE;

	AgcmUIAccountCheck* pThis = ( AgcmUIAccountCheck* )pClass;
	if( !pThis ) return FALSE;

	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pThis->m_pcmUILogin;
	if( !pcmUILogin ) return FALSE;

	if( pThis->GetChallengeString() )
	{
		INT32 nChallenge = atoi( pThis->GetChallengeString() );
		CHAR strTemp[ 12 ] = { 0, };

		if( nChallenge < 1000 )
		{
			strcpy( strTemp, "XXXX" );
		}
		else
		{
			sprintf( strTemp, "%d", nChallenge );
		}

		sprintf( pDisplay, ClientStr().GetStr( STI_INPUT_EKEY ), strTemp );
	}
	else
	{
		sprintf( pDisplay, "" );
	}

	return TRUE;
}