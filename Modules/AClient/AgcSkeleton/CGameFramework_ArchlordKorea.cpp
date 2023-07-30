#define _WINSOCKAPI_
#include "CGameFramework_ArchlordKorea.h"
#include "CGameFramework_Utility.h"


CGameFramework_ArchlordKorea::CGameFramework_ArchlordKorea( void )
{
}

CGameFramework_ArchlordKorea::~CGameFramework_ArchlordKorea( void )
{
}

BOOL CGameFramework_ArchlordKorea::OnGameStart( void )
{
	// Step 1. �������� �ʱ�ȭ ( rsINITIALIZE, psInitialize() );

	// Step 2. Ÿ��ī���� �ʱ�ȭ, ������ ���� �غ� ( InitApplication );

	// Step 3. ���� �Ķ���� �˻�, ���ӽ�Ʈ���� ��ġ�ڵ带 �˻��Ѵ�.
	if( !ParseCommandLine() ) return FALSE;

	// Step 4. ������ ����

	// Step 5. ������ ����Ű ��ŷ

	// Step 6. ������ ����̽� ����

	return TRUE;
}

BOOL CGameFramework_ArchlordKorea::OnGameLoop( void )
{
	return TRUE;
}

BOOL CGameFramework_ArchlordKorea::OnGameEnd( void )
{
	return TRUE;
}

BOOL CGameFramework_ArchlordKorea::ParseCommandLine( void )
{
	char* pCommand = ::GetCommandLine();
	if( !pCommand || strlen( pCommand ) <= 0 ) return FALSE;

	int nArgumentCount = 0;
	char** ppArguments = FrameworkUtil::CommandLineToArgv( &nArgumentCount );
	if( !ppArguments || nArgumentCount <= 0 ) return FALSE;

	// ��ġ�ڵ� �˻�
	if( !CheckPatchCode( nArgumentCount >= 2 ? ppArguments[ 1 ] : NULL ) )
	{
		::MessageBox( NULL, "Cannot run AlefClient.exe alone.", "Invalid Client running", MB_OK );
		return FALSE;
	}

	FrameworkUtil::DeleteCommandLine( ppArguments, nArgumentCount );
	return TRUE;
}
