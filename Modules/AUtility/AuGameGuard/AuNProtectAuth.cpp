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
	// CS���� ��� ���ο��� �����ϴ� Ÿ�̸Ӱ� �ý����� Ư���� ���ؼ� �������� �ʴ� ��찡 �ֽ��ϴ�.
	// �׷���� 3��° ���ڸ� true�� �����ϰ�
	// �������� ���� 3�ð� ���� üũ�Ͽ� GGAuthUpdateTimer() �Լ��� �������� ȣ���Ͽ� �˰����� ����ǵ��� ���־�� �մϴ�.
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
// Error ������ �α׿� Debug ������ �αװ� ���޵Ǵ� �Լ��Դϴ�.
// Debug ������ ���޷��� ���� ������ ���ϸ� ����ų �� �ֽ��ϴ�.
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
// CS ���� ���̺귯���� CS���� ���� �� ���������� ������Ʈ �Ǿ��� �� ȣ��Ǵ� �ݹ� �Լ��Դϴ�.
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