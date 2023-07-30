#include <AuExcelTxtLib/AuExcelTxtLib.h>
#include "CNpcTextManager.h"

#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcModule/AgcEngine.h>




CNpcTextManager::CNpcTextManager( void )
{
}

CNpcTextManager::~CNpcTextManager( void )
{
}

BOOL CNpcTextManager::OnLoadNpcDialogText( char* pFileName, BOOL bIsEncrypt )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	AuExcelTxtLib ExcelFile;
	if( !ExcelFile.OpenExcelFile( pFileName, TRUE, bIsEncrypt ) )
	{
#ifdef _DEBUG
		char strDebug[ 256 ] = { 0, };
		sprintf_s( strDebug, sizeof( char ) * 256, "CNpcTextManager::OnLoadNpcDialogText(), Cannot load NpcDialog.txt, FileName = %s, Encryt = %s\n",
			pFileName, bIsEncrypt ? "TRUE" : "FALSE" );
		::OutputDebugString( strDebug );
#endif
		return FALSE;
	}

	int nMaxRow = ExcelFile.GetRow();
	int nMaxCol = ExcelFile.GetColumn();

	int nIndex = 0;

	for( int nRowCount = 0 ; nRowCount < nMaxRow ; nRowCount++ )
	{
		char* pBuffer = ExcelFile.GetData( 0, nRowCount );

		// '-' 이 오면 그 라인은 읽지 않는다.
		if( pBuffer && !strcmp( pBuffer, "-" ) ) continue;

		char* pIndex = ExcelFile.GetData( 0, nRowCount );
		char* pNpcTemplateName = ExcelFile.GetData( 1, nRowCount );
		char* pNpcID = ExcelFile.GetData( 2, nRowCount );
		char* pNpcName = ExcelFile.GetData( 3, nRowCount );

		if( pIndex && strlen( pIndex ) > 0 )
		{
			nIndex = atoi( pIndex );
			int nNpcID = atoi( pNpcID && strlen( pNpcID ) > 0 ? pNpcID : "0" );
			if( !OnAddNpc( nIndex, nNpcID, pNpcName ) )
			{
#ifdef _DEBUG
				char strDebug[ 256 ] = { 0, };
				sprintf_s( strDebug, sizeof( char ) * 256, "CNpcTextManager::OnLoadNpcDialogText(), Cannot add Npc, Index = %d, Name = %s\n",
					nIndex, pNpcName && strlen( pNpcName ) > 0 ? pNpcName : "Undefined" );
				::OutputDebugString( strDebug );
#endif
				return FALSE;
			}
		}

		char* pRateCommon = ExcelFile.GetData( 4, nRowCount );	// 공통확률
		char* pTextCommon = ExcelFile.GetData( 5, nRowCount );	// 공통대사

		OnAddNpcDialogText( nIndex, NpcText_Common, pTextCommon, pRateCommon );

		char* pTextBaloon = ExcelFile.GetData( 6, nRowCount );	// 말풍선대사
		char* pTextBaloonTimeMin = ExcelFile.GetData( 7, nRowCount );	// Min_Time
		char* pTextBaloonTimeMax = ExcelFile.GetData( 8, nRowCount );	// Max_Time

		OnAddNpcDialogText( nIndex, NpcText_Baloon, pTextBaloon, pRateCommon, pTextBaloonTimeMin, pTextBaloonTimeMax );

		char* pRateMurdererLevel1 = ExcelFile.GetData( 9, nRowCount );	// 악당레벨 1 확률
		char* pTextMurdererLevel1 = ExcelFile.GetData( 10, nRowCount );	// 악당레벨 1 대사

		OnAddNpcDialogText( nIndex, NpcText_MurdererLevel1, pTextMurdererLevel1, pRateMurdererLevel1 );

		char* pRateMurdererLevel2 = ExcelFile.GetData( 11, nRowCount );	// 악당레벨 2 확률
		char* pTextMurdererLevel2 = ExcelFile.GetData( 12, nRowCount );	// 악당레벨 2 대사

		OnAddNpcDialogText( nIndex, NpcText_MurdererLevel2, pTextMurdererLevel2, pRateMurdererLevel2 );

		char* pRateMurdererLevel3 = ExcelFile.GetData( 13, nRowCount );	// 악당레벨 3 확률
		char* pTextMurdererLevel3 = ExcelFile.GetData( 14, nRowCount );	// 악당레벨 3 대사

		OnAddNpcDialogText( nIndex, NpcText_MurdererLevel3, pTextMurdererLevel3, pRateMurdererLevel3 );
	}

	ExcelFile.CloseFile();
	return TRUE;
}

BOOL CNpcTextManager::OnAddNpc( int nNpcIndex, int nNpcID, char* pNpcName )
{
	stNpcDialogEntry* pNpc = m_mapNpcText.Get( nNpcIndex );
	if( pNpc )
	{
		pNpc->m_nIndex = nNpcIndex;

		// 있으면 내부 인스턴스 포인터 체크해보고 없으면 생성
		if( !pNpc->m_pNpc )
		{
			pNpc->m_pNpc = new CNpcText;
			if( pNpc->m_pNpc )
			{
				pNpc->m_pNpc->OnSetNpcInfo( nNpcIndex, nNpcID, pNpcName );
			}
		}
	}
	else
	{
		// 없으면 물론 추가
		stNpcDialogEntry NewEntry;

		NewEntry.m_nIndex = nNpcIndex;
		NewEntry.m_pNpc = new CNpcText;
		if( NewEntry.m_pNpc )
		{
			NewEntry.m_pNpc->OnSetNpcInfo( nNpcIndex, nNpcID, pNpcName );
		}

//#ifdef _DEBUG
//		char strDebug[ 256 ] = { 0, };
//		sprintf_s( strDebug, sizeof( char ) * 256, "-- CNpcTextManager::OnAddNpc(), Index = %d, NpcName = %s\n",
//			nNpcIndex, pNpcName && strlen( pNpcName ) > 0 ? pNpcName : "Undefined" );
//		::OutputDebugString( strDebug );
//#endif

		m_mapNpcText.Add( nNpcIndex, NewEntry );
	}

	return TRUE;
}

BOOL CNpcTextManager::OnAddNpcDialogText( int nNpcIndex, eNpcTextType eType, char* pText, char* pRate, char* pTimeMin, char* pTimeMax )
{
	int nRate = pRate && strlen( pRate ) > 0 ? atoi( pRate ) : 0;

	__int64 nTimeMin = pTimeMin && strlen( pTimeMin ) > 0 ? _atoi64( pTimeMin ) : 0;
	__int64 nTimeMax = pTimeMax && strlen( pTimeMax ) > 0 ? _atoi64( pTimeMax ) : 0;

	return OnAddNpcDialogText( nNpcIndex, eType, pText, nRate, nTimeMin, nTimeMax );
}

BOOL CNpcTextManager::OnAddNpcDialogText( int nNpcIndex, eNpcTextType eType, char* pText, int nRate, __int64 nTimeMin, __int64 nTimeMax )
{
	stNpcDialogEntry* pNpc = m_mapNpcText.Get( nNpcIndex );
	if( !pNpc || !pNpc->m_pNpc ) return FALSE;
	return pNpc->m_pNpc->OnTextAdd( eType, pText, nRate, nTimeMin, nTimeMax );
}

char* CNpcTextManager::GetNpcDialogText( int nNpcIndex, void* pUserCharacter )
{
	stNpcDialogEntry* pNpc = m_mapNpcText.Get( nNpcIndex );
	if( !pNpc || !pNpc->m_pNpc ) return NULL;

	// 대화창을 연 캐릭터를 알수 없는 경우엔 공통 메세지로..
	AgpdCharacter* ppdViewCharacter = ( AgpdCharacter* )pUserCharacter;
	if( !ppdViewCharacter )
	{
		return pNpc->m_pNpc->GetNpcText( NpcText_Common );
	}

	// 대화창을 연 캐릭터가 악당레벨이 존재하면 악당레벨에 따라서..
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	if( !ppmCharacter ) return NULL;

	int nMurdererLevel = ppmCharacter->GetMurdererLevel( ppdViewCharacter );
	switch( nMurdererLevel )
	{
	case AGPMCHAR_MURDERER_LEVEL1_POINT :	return pNpc->m_pNpc->GetNpcText( NpcText_MurdererLevel1 );	break; 
	case AGPMCHAR_MURDERER_LEVEL2_POINT :	return pNpc->m_pNpc->GetNpcText( NpcText_MurdererLevel2 );	break; 
	case AGPMCHAR_MURDERER_LEVEL3_POINT :	return pNpc->m_pNpc->GetNpcText( NpcText_MurdererLevel3 );	break; 
	}

	// 여기까지 왔는데 아직 걸린게 없으면 Common 메세지를.. Baloon 메세지는 쓰는데가 없더라;; 왜 있는거지..;;
	return pNpc->m_pNpc->GetNpcText( NpcText_Common );
}

BOOL CNpcTextManager::OnClearNpcDialogText( void )
{
	int nNpcCount = m_mapNpcText.GetSize();
	for( int nCount = 0 ; nCount < nNpcCount ; nCount++ )
	{
		stNpcDialogEntry* pNpc = m_mapNpcText.GetByIndex( nCount );
		if( pNpc && pNpc->m_pNpc )
		{
			pNpc->m_pNpc->OnTextClear();

			delete pNpc->m_pNpc;
			pNpc->m_pNpc = NULL;
		}
	}

	m_mapNpcText.Clear();
	return TRUE;
}


