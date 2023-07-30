#include "CNpcText.h"
#include "stdio.h"



CNpcText::CNpcText( void )
{
	m_nNpcIndex = 0;
	m_nNPCID = 0;
	m_strNPCName = "";
}

CNpcText::~CNpcText( void )
{
}

BOOL CNpcText::OnSetNpcInfo( int nIndex, int nID, char* pName )
{
	m_nNpcIndex = nIndex;
	m_nNPCID = nID;
	m_strNPCName = pName ? pName : "UnKnown";
	return TRUE;
}

BOOL CNpcText::OnTextAdd( eNpcTextType eType, char* pText, int nRate, __int64 nTimeMin, __int64 nTimeMax )
{
	stNpcTextEntry* pEntry = m_mapNpcText.Get( eType );
	if( pEntry )
	{
		// 있으면 있는데다가 추가
		if( !pEntry->m_pText )
		{
			pEntry->m_pText = new CRandomText;
		}

		pEntry->m_pText->OnTextAdd( pText, nRate );

//#ifdef _DEBUG
//		char strDebug[ 1024 ] = { 0, };
//		sprintf_s( strDebug, sizeof( char ) * 1024, "__ CNpcText::OnTextAdd(), NpcIndex = %d, TextType = %d, Rate = %d, Text = %s\n",
//			m_nNpcIndex, ( int )eType, nRate, pText && strlen( pText ) > 0 ? pText : "NoText" );
//		::OutputDebugString( strDebug );
//#endif
	}
	else
	{
		// 없으면 새로 추가
		stNpcTextEntry NewEntry;

		NewEntry.m_eType = eType;
		NewEntry.m_pText = new CRandomText;
		if( !NewEntry.m_pText ) return FALSE;

		NewEntry.m_pText->OnTextAdd( pText, nRate );
		NewEntry.m_nViewTimeMin = nTimeMin;
		NewEntry.m_nViewTimeMax = nTimeMax;

//#ifdef _DEBUG
//		char strDebug[ 1024 ] = { 0, };
//		sprintf_s( strDebug, sizeof( char ) * 1024, "__ CNpcText::OnTextAdd(), NpcIndex = %d, TextType = %d, Rate = %d, Text = %s\n",
//			m_nNpcIndex, ( int )eType, nRate, pText && strlen( pText ) > 0 ? pText : "NoText" );
//		::OutputDebugString( strDebug );
//#endif

		m_mapNpcText.Add( eType, NewEntry );
	}

	return TRUE;
}

BOOL CNpcText::OnTextClear( void )
{
	int nTextCount = m_mapNpcText.GetSize();
	for( int nCount = 0 ; nCount < nTextCount ; nCount++ )
	{
		stNpcTextEntry* pEntry = m_mapNpcText.GetByIndex( nCount );
		if( pEntry && pEntry->m_pText )
		{
			delete pEntry->m_pText;
			pEntry->m_pText = NULL;
		}
	}

	m_mapNpcText.Clear();
	return TRUE;
}

char* CNpcText::GetNpcText( eNpcTextType eType )
{
	stNpcTextEntry* pEntry = m_mapNpcText.Get( eType );
	if( !pEntry || !pEntry->m_pText ) return NULL;
	return pEntry->m_pText->GetRandomText();
}

