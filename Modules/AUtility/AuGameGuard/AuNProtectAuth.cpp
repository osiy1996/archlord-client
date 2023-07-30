#include "AuNProtectAuth.h"

#ifdef _USE_NPROTECT_GAMEGUARD_

#include <windows.h>
#include <stdio.h>

//-----------------------------------------------------------------------
//
//


NProtectAuth::NProtectAuth( void ) {}

//-----------------------------------------------------------------------
//
//


NProtectAuth::~NProtectAuth( void )
{
	CleanupGameguardAuth();
}

//-----------------------------------------------------------------------
//
//


bool NProtectAuth::Init( void )
{
	// CS인증 모듈 내부에서 생성하는 타이머가 시스템의 특성에 의해서 동작하지 않는 경우가 있습니다.
	// 그런경우 3번째 인자를 true로 전달하고
	// 서버에서 직접 3시간 마다 체크하여 GGAuthUpdateTimer() 함수를 수동으로 호출하여 알고리즘이 변경되도록 해주어야 합니다.
	DWORD ret = InitGameguardAuth("", 200, false, 0x03);

	if ( ERROR_SUCCESS != ret )
	{
		char tmp[1024];
		sprintf_s( tmp, sizeof(tmp), "Fail[%d] InitGameguardAuth()", ret );
		MessageBoxA( 0, tmp, "NProtect Error", 0 );

		return false;
	}

	SetUpdateCondition( 30, 50 );
	return true;
}

//-----------------------------------------------------------------------
//
//


bool NProtectAuth::ServerToClient(CCSAuth2 & auth2, GG_AUTH_DATA & ggData)
{
	if ( ERROR_SUCCESS == auth2.GetAuthQuery() )
	{
		ggData = auth2.m_AuthQuery;
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------
//
// Error 레벨의 로그와 Debug 레벨의 로그가 전달되는 함수입니다.
// Debug 레벨은 전달량이 많아 서버에 부하를 일으킬 수 있습니다.
//


GGAUTHS_API void NpLog( int mode, char* msg )
{
	if( !msg )
		return;

	//if( mode & (NPLOG_DEBUG | NPLOG_ERROR)) //Select log mode.
	if( mode & (NPLOG_ERROR))
		printf("%s\n", msg);
}


//-----------------------------------------------------------------------
//
// CS 인증 라이브러리의 CS인증 버전 및 프로토콜이 업데이트 되었을 때 호출되는 콜백 함수입니다.
//


GGAUTHS_API void GGAuthUpdateCallback( PGG_UPREPORT report )
{
	if( !report )
		return;

	printf("GGAuth version update [%s] : [%ld] -> [%ld] \n", 
		(report->nType == 1) ? "GameGuard Ver" : "Protocol Num", 
		report->dwBefore, report->dwNext);
}

//-----------------------------------------------------------------------

#endif