#include "AcArchlordInfo.h"
#include <AuMD5EnCrypt/AuMD5EnCrypt.h>
#include <fstream>

AcArchlordInfo::AcArchlordInfo( VOID )
{
}

AcArchlordInfo::~AcArchlordInfo( VOID )
{
	Destroy();
}

VOID	AcArchlordInfo::Destroy( VOID )
{
	VecLoginGroupIter	Iter	=	m_VecLoginGroup.begin();
	for( ; Iter != m_VecLoginGroup.end() ; ++Iter )
	{
		DEF_SAFEDELETE( (*Iter) );
	}
	m_VecLoginGroup.clear();
}

BOOL	AcArchlordInfo::LoadFile( BOOL bEncrypt )
{

	if( !_LoadArchlordInfoXML( "ArchlordTest.ini") )
	{
		if( !_LoadArchlordInfoXML( "Archlord.ini" ) )
		{
			AuMD5Encrypt	MD5;
			ifstream		InFileStream;
			ofstream		OutFileStream;
			INT				nSize		=	0;
			CHAR*			pFileBuffer	=	NULL;

			// 1 - ������ �о �޸𸮷� �ҷ��´�
			InFileStream.open( "Archlord.ini" , ios_base::in | ios_base::binary );
			if( !InFileStream )
				return FALSE;

			InFileStream.seekg( 0 , ios_base::end );
			nSize		=	(int)InFileStream.tellg();
			pFileBuffer	=	new CHAR [ nSize ];
			ZeroMemory( pFileBuffer , nSize );

			InFileStream.seekg( 0 , ios_base::beg );
			InFileStream.read( pFileBuffer , nSize );
			InFileStream.close();
			// 1 - END


			// 2 - �޸��� ������ ��ȣȭ �Ѵ�
			//if( !MD5.DecryptString( "1111", pFileBuffer, nSize ) )
			//{
			//	DEF_SAFEDELETE( pFileBuffer );
			//	return FALSE;
			//}
			// 2 - END


			// 3 - ��ȣȭ �� ������ ���Ϸ� ����
			OutFileStream.open( "ArchlordTemp.ini" , ios_base::out | ios_base::binary );
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
			if( !_LoadArchlordInfoXML("ArchlordTemp.ini") )
			{
				::DeleteFile( "ArchlordTemp.ini" );
				return FALSE;
			}

			// �ӽ÷� ���� ������ �����
			::DeleteFile( "ArchlordTemp.ini" );
		}

	}

	return TRUE;
}


BOOL	AcArchlordInfo::_LoadArchlordInfoXML( IN CONST string& strFileName )
{
	if( !m_XMLDocument.LoadFile( strFileName.c_str() , TIXML_ENCODING_LEGACY) )
	{
		return FALSE;
	}


	// Login Server ������ �д´�.
	AuXmlNode *pRootNode = m_XMLDocument.FirstChild("ArchlordInfo");
	if( pRootNode )
	{
		m_XMLRootNode	=	pRootNode->FirstChild("ServerGroup");
		for( AuXmlNode *pNode = m_XMLRootNode ; pNode ; pNode = pNode->NextSibling() )
		{
			stLoginGroup				*pLoginGroup	=	NULL;
			AuXmlElement				*pElemName		= pNode->FirstChildElement( "GroupName" );
			if(!pElemName)
				continue;

			pLoginGroup		=	new	stLoginGroup;
			if( !pLoginGroup )
				continue;

			pLoginGroup->m_strGroupName	=	pElemName->GetText();

			for( AuXmlElement *pElemLine = pNode->FirstChildElement( "IP") ; pElemLine ; pElemLine = pElemLine->NextSiblingElement("IP")  )
			{
				if( pElemLine->GetText() )
				{	
					stLoginServerInfo	LoginServerInfo;
					LoginServerInfo.m_bTryConnect		=	FALSE;
					LoginServerInfo.m_strServerIP		=	pElemLine->GetText();
					pLoginGroup->m_vecServerInfo.push_back( LoginServerInfo );
#ifdef _AREA_GLOBAL_

					pLoginGroup->m_eOper	=	E_LOGIN_CONNECT;
				}
			}

			AuXmlElement*	pElemLine	=	pNode->FirstChildElement( "MSG" );
			if( pElemLine )
			{
				if( pElemLine->GetText() )
				{
					pLoginGroup->m_eOper		=	E_LOGIN_MESSAGEBOX;
					pLoginGroup->m_strMessage	=	pElemLine->GetText();
#endif
				}
			}

			m_VecLoginGroup.push_back( pLoginGroup );

		}
	}

	// Patch Server������ �д´�
	pRootNode		=	m_XMLDocument.FirstChild( "PatchServerInfo" );
	if( pRootNode )
	{
		AuXmlElement	*pElemName	=	pRootNode->FirstChildElement( "IP" );
		for( AuXmlElement *pElemLine = pElemName ; pElemLine ; pElemLine = pElemLine->NextSiblingElement("IP") )
		{
			if( pElemLine->GetText() )
			{
				stLoginServerInfo		PatchServerInfo;
				PatchServerInfo.m_bTryConnect		=	FALSE;
				PatchServerInfo.m_strServerIP		=	pElemLine->GetText();
				m_VecPatchServer.push_back( PatchServerInfo );
			}
		}

	}

	m_XMLDocument.Clear();
	return TRUE;
}


stLoginGroup*	AcArchlordInfo::GetGroup( INT nIndex )
{
	if( m_VecLoginGroup.size()-1 < nIndex )
		return NULL;

	return m_VecLoginGroup[ nIndex ];
}


stLoginServerInfo*	AcArchlordInfo::GetPatchServer( INT nIndex )
{
	if( m_VecPatchServer.size()-1 < nIndex )
		return NULL;

	return &m_VecPatchServer[ nIndex ];
}
