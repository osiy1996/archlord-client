#include "CRandomText.h"



CRandomText::CRandomText( void )
{
}

CRandomText::~CRandomText( void )
{
}

BOOL CRandomText::OnTextAdd( char* pText, int nRate )
{
	if( !pText || strlen( pText ) <= 0 ) return FALSE;

	stRandomTextEntry NewEntry;

	NewEntry.m_strText = pText;
	NewEntry.m_nRate = nRate;

	m_vecText.Add( NewEntry );
	return TRUE;
}

char* CRandomText::GetRandomText( void )
{
	::srand( ::timeGetTime() );
	int nTargetIndex = rand() % 100;

	int nTextCount = m_vecText.GetSize();
	int nTotalRate = 0;

	for( int nCount = 0 ; nCount < nTextCount ; nCount++ )
	{
		stRandomTextEntry* pText = m_vecText.Get( nCount );
		if( pText )
		{
			nTotalRate += pText->m_nRate;
			if( nTotalRate >= nTargetIndex )
			{
				return ( char* )pText->m_strText.c_str();
			}
		}
	}

	return NULL;
}
