
#include "AgcmUIMainSystemGridTooltip.h"
#include <AuMD5EnCrypt/AuMD5Encrypt.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <fstream>

AgcmUIMainSystemGridTooltip::AgcmUIMainSystemGridTooltip( VOID )
{

}

AgcmUIMainSystemGridTooltip::~AgcmUIMainSystemGridTooltip( VOID )
{
	Destroy();
}

#ifdef _AREA_GLOBAL_
BOOL	AgcmUIMainSystemGridTooltip::Initialize( AgcmUIManager2* pUIManager, char* pFileName, BOOL bEncrypt )
{
	if( !pUIManager ) return FALSE;
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	if( bEncrypt )
	{
		AuMD5Encrypt	MD5;
		ifstream		InFileStream;
		ofstream		OutFileStream;
		size_t			nSize		=	0;
		CHAR*			pFileBuffer	=	NULL;

		// 1 - ������ �о �޸𸮷� �ҷ��´�
		InFileStream.open( pFileName, ios_base::in | ios_base::binary );
		if( !InFileStream )
			return FALSE;

		InFileStream.seekg( 0 , ios_base::end );
		nSize		=	(size_t)InFileStream.tellg();
		pFileBuffer	=	new CHAR [ nSize ];
		ZeroMemory( pFileBuffer , nSize );

		InFileStream.seekg( 0 , ios_base::beg );
		InFileStream.read( pFileBuffer , nSize );
		InFileStream.close();
		// 1 - END


		// 2 - �޸��� ������ ��ȣȭ �Ѵ�
		if( !MD5.DecryptString( "1111", pFileBuffer, nSize ) )
		{
			DEF_SAFEDELETE( pFileBuffer );
			return FALSE;
		}
		// 2 - END


		// 3 - ��ȣȭ �� ������ ���Ϸ� ����
		OutFileStream.open( "INI\\SystemGridTooltipTemp.XML" , ios_base::out | ios_base::binary );
		if( !OutFileStream )
		{
			DEF_SAFEDELETE( pFileBuffer );
			return FALSE;
		}

		OutFileStream.write( pFileBuffer , nSize );
		OutFileStream.close();
		DEF_SAFEDELETE( pFileBuffer );
		// 3 - END

		// Read
		if( !LoadSystemGridTooltipXML("INI\\SystemGridTooltipTemp.XML") )
		{
			::DeleteFile( "INI\\SystemGridTooltipTemp.XML" );
			return FALSE;
		}

		// �ӽ÷� ���� ������ �����
		::DeleteFile( "INI\\SystemGridTooltipTemp.XML" );
	}

	else
	{
		if( !LoadSystemGridTooltipXML( pFileName ) )
			return FALSE;
	}

	m_pcsAgcmUIManager2	=	pUIManager;

	m_csSystemGridToolTip.m_Property.bTopmost	=	TRUE;
	pUIManager->AddWindow( (AgcWindow*)&m_csSystemGridToolTip );

	m_csSystemGridToolTip.ShowWindow( FALSE );

	return TRUE;
}
#else
BOOL	AgcmUIMainSystemGridTooltip::Initialize( BOOL bEncrypt )
{
	if( bEncrypt )
	{
		AuMD5Encrypt	MD5;
		ifstream		InFileStream;
		ofstream		OutFileStream;
		INT				nSize		=	0;
		CHAR*			pFileBuffer	=	NULL;

		// 1 - ������ �о �޸𸮷� �ҷ��´�
		InFileStream.open( "INI\\SystemGridTooltip.XML" , ios_base::in | ios_base::binary );
		if( !InFileStream )
			return FALSE;

		InFileStream.seekg( 0 , ios_base::end );
		nSize		=	InFileStream.tellg();
		pFileBuffer	=	new CHAR [ nSize ];
		ZeroMemory( pFileBuffer , nSize );

		InFileStream.seekg( 0 , ios_base::beg );
		InFileStream.read( pFileBuffer , nSize );
		InFileStream.close();
		// 1 - END


		// 2 - �޸��� ������ ��ȣȭ �Ѵ�
		if( !MD5.DecryptString( "1111", pFileBuffer, nSize ) )
		{
			DEF_SAFEDELETE( pFileBuffer );
			return FALSE;
		}
		// 2 - END


		// 3 - ��ȣȭ �� ������ ���Ϸ� ����
		OutFileStream.open( "INI\\SystemGridTooltipTemp.XML" , ios_base::out | ios_base::binary );
		if( !OutFileStream )
		{
			DEF_SAFEDELETE( pFileBuffer );
			return FALSE;
		}

		OutFileStream.write( pFileBuffer , nSize );
		OutFileStream.close();
		DEF_SAFEDELETE( pFileBuffer );
		// 3 - END

		// Read
		if( !LoadSystemGridTooltipXML("INI\\SystemGridTooltipTemp.XML") )
		{
			::DeleteFile( "INI\\SystemGridTooltipTemp.XML" );
			return FALSE;
		}

		// �ӽ÷� ���� ������ �����
		::DeleteFile( "INI\\SystemGridTooltipTemp.XML" );
	}

	else
	{
		if( !LoadSystemGridTooltipXML( "INI\\SystemGridTooltip.XML" ) )
			return FALSE;
	}

	m_csSystemGridToolTip.m_Property.bTopmost	=	TRUE;

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( pcmUIManager )
	{
		pcmUIManager->AddWindow( (AgcWindow*)&m_csSystemGridToolTip );
	}

	m_csSystemGridToolTip.ShowWindow( FALSE );

	return TRUE;
}
#endif

VOID	AgcmUIMainSystemGridTooltip::Destroy( VOID )
{

	SystemGridTooltipVecIter	Iter	=	m_vecSysGridTooltip.begin();
	for( ; Iter != m_vecSysGridTooltip.end() ; ++Iter )
	{
		DEF_SAFEDELETE( (*Iter) );
	}
	m_vecSysGridTooltip.clear();

}

BOOL	AgcmUIMainSystemGridTooltip::LoadSystemGridTooltipXML( IN CONST string& strFullPathName )
{
	Destroy();

#ifdef _AREA_GLOBAL_
	if( !m_XMLDocument.LoadFile( strFullPathName ) )
#else
	if( !m_XMLDocument.LoadFile( strFullPathName , TIXML_ENCODING_LEGACY) )
#endif
	{
		TRACE( "System Grid Tooltip Load Failed\n" );
		return FALSE;
	}

	AuXmlNode *pRootNode = m_XMLDocument.FirstChild("SystemGridTooltip");
	if(!pRootNode)
		return FALSE;

	m_XMLRootNode	=	pRootNode->FirstChild("Tooltip");
	if(!m_XMLRootNode)
		return FALSE;

	for( AuXmlNode *pNode = m_XMLRootNode ; pNode ; pNode = pNode->NextSibling() )
	{
		stSystemGridTooltip			*pSystemGridTooltip	=	NULL;
		AuXmlElement				*pElemName = pNode->FirstChildElement( "Name" );
		if(!pElemName)
			continue;

		pSystemGridTooltip		=	new	stSystemGridTooltip;
		if( !pSystemGridTooltip )
			continue;

		pSystemGridTooltip->strSectionName	=	pElemName->GetText();
		
		for( AuXmlElement *pElemLine = pNode->FirstChildElement( "Line") ; pElemLine ; pElemLine = pElemLine->NextSiblingElement("Line")  )
		{
			stTooltipLine			LineInfo;

			LineInfo.dwColor	=	_LineColor( pElemLine->Attribute( "Color" ) );

			if( pElemLine->GetText() )
				LineInfo.strLine	=	pElemLine->GetText();

			pSystemGridTooltip->listTooltipLine.push_back( LineInfo );
		}

		m_vecSysGridTooltip.push_back( pSystemGridTooltip );
	}

	m_XMLDocument.Clear();
	return TRUE;
}
#ifdef _AREA_GLOBAL_
BOOL	AgcmUIMainSystemGridTooltip::OpenSystemGridTooltip( IN CONST string& strSection, UINT nExpireTime )
#else
BOOL	AgcmUIMainSystemGridTooltip::OpenSystemGridTooltip( IN CONST string& strSection )
#endif
{

#ifdef _AREA_GLOBAL_
	m_csSystemGridToolTip.DeleteAllStringInfo();
	m_csSystemGridToolTip.MoveWindow((INT32) m_pcsAgcmUIManager2->m_v2dCurMousePos.x + 30, (INT32) m_pcsAgcmUIManager2->m_v2dCurMousePos.y + 30,
		m_csSystemGridToolTip.w, m_csSystemGridToolTip.h);
#else
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	m_csSystemGridToolTip.DeleteAllStringInfo();
	m_csSystemGridToolTip.MoveWindow( ( INT32 )pcmUIManager->m_v2dCurMousePos.x + 30, ( INT32 )pcmUIManager->m_v2dCurMousePos.y + 30,
		m_csSystemGridToolTip.w, m_csSystemGridToolTip.h);
#endif

	INT i	=	0;
	for( i = 0 ; ; ++i )
	{
		stTooltipLine*	pTooltipLine	=	_GetString( strSection , i );
		if( !pTooltipLine )
			break;

		m_csSystemGridToolTip.AddString( (CHAR*)pTooltipLine->strLine.c_str() , 12, pTooltipLine->dwColor );
		m_csSystemGridToolTip.AddNewLine(14);
	}
	
	// ������ ���� ���� ���ӳ����� �ٲ�� �����̴�.
	if( i == 0 )
	{
		m_csSystemGridToolTip.AddString( (CHAR*)strSection.c_str() , 12 );
		m_csSystemGridToolTip.AddNewLine( 14 );
	}

	m_csSystemGridToolTip.ShowWindow( TRUE );

	return TRUE;
}

BOOL	AgcmUIMainSystemGridTooltip::CloseSystemGridTooltip( VOID )
{
	m_csSystemGridToolTip.ShowWindow( FALSE );
	m_csSystemGridToolTip.DeleteAllStringInfo();

	return TRUE;
}



stTooltipLine*	AgcmUIMainSystemGridTooltip::_GetString( IN CONST string& strSection , INT nIndex )
{

	SystemGridTooltipVecIter	Iter		=	m_vecSysGridTooltip.begin();
	for( ; Iter != m_vecSysGridTooltip.end() ; ++Iter )
	{
		if( !(*Iter)->strSectionName.compare( strSection ) )
		{
			TooltipLineListIter	LineIter	=	(*Iter)->listTooltipLine.begin();
			for( INT i = 0 ; LineIter != (*Iter)->listTooltipLine.end() ; ++LineIter , ++i )
			{
				if( i == nIndex )
					return &(*LineIter);
			}
		}
	}

	return NULL;
}

DWORD	AgcmUIMainSystemGridTooltip::_LineColor( CONST string&	strLineColor )
{
	CHAR	szHexString[ MAX_PATH ];
	ZeroMemory( szHexString , MAX_PATH );

	sprintf_s( szHexString , MAX_PATH , "%s" ,strLineColor.c_str() );
	if( !szHexString )
		return 0;

	DWORD	dwValue		=	0;
	DWORD	dwTotal		=	0;
	DWORD	dwMulti		=	0;
	INT		nLength		=	strlen( szHexString );

	for( INT i = nLength-1 ; i >= 0 ; --i )
	{
		dwMulti		=	1;

		// Ascii Code �� ���� ���� ���ڷ� �����
		if( szHexString[ i ] >= 'A' && szHexString[ i ] <= 'F' )
			dwValue		=	szHexString[ i ] - 55;

		else if( szHexString[ i ] >= 'a' && szHexString[ i ] <= 'f' )
			dwValue		=	szHexString[ i ] - 87;

		else if( szHexString[ i ] >= '0' && szHexString[ i ] <= '9' )
			dwValue		=	szHexString[ i ] - 48;

		// ���� �����ش�
		for( INT k = 0 ; k < nLength - 1 - i  ; ++k )
			dwMulti	*= 16;

		dwTotal +=	( dwValue * dwMulti );
	}

	return dwTotal;
}
#ifdef _AREA_GLOBAL_
std::string AgcmUIMainSystemGridTooltip::_MakeExpireTimeString( UINT32 nExpireTime )
{
	CTime Time( ( time_t )nExpireTime );
	struct tm TimeData;
	Time.GetGmtTm( &TimeData );

	char strBuffer[ 128 ] = { 0, };
	sprintf_s( strBuffer, sizeof( char ) * 128, "GST %d/%02d/%02d %02d:%02d", TimeData.tm_year + 1900, TimeData.tm_mon, TimeData.tm_mday, TimeData.tm_hour, TimeData.tm_min );

	std::string strTimeString = strBuffer;
	return strTimeString;
}
#endif