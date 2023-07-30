#include "AgcmSocialAction.h"
#include <AgpmCharacter/AgpdCharacter.h>
#include <AuExcelTxtLib/AuExcelTxtLib.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgcmResourceLoader/AgcmResourceLoader.h>
#include <AgcmUIControl/AgcmUIControl.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <fstream>

//---------------------- SocialAction --------------------------
SocialAction::SocialAction( char* szIcon, char* szCommand, char* szTooltipTitle, char* szTooltipText ) : 
 pGridItem(NULL),
 pTexture(NULL)
{
	strcpy( this->szIcon, szIcon );
	strcpy( this->szCommand, szCommand );
	strcpy( this->szTooltipTitle, szTooltipTitle );
	strcpy( this->szTooltipText, szTooltipText );
}

AgcmSocialActon::AgcmSocialActon()
{
	m_vecInfo.reserve( SOCIALACTION_MAX );
	//m_vecInfo.push_back( new SocialAction("SocialGreeting.png", " ", "�λ�", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialCelebration.png", " ", "����", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialGratitude.png", " ", "����", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialEncouragement.png", " ", "�ݷ�", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialDisregard.png", " ", "����", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialDancing.png", " ", "���߱�", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialDoziness.png", " ", "����", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialStretch.png", " ", "������", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialLaugh.png", " ", "����", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialWeeping.png", " ", "���", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialPout.png", " ", "�г�", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialRage.png", " ", "�����", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialApology.png", " ", "���", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialToast.png", " ", "�ǹ�", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialCheer.png", " ", "ȯȣ", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialRush.png", " ", "����", " " ) );
	//m_vecInfo.push_back( new SocialAction("SocialSit.png", " ", "�ɱ�", " " ) );
}																  

AgcmSocialActon::~AgcmSocialActon()
{
	for( SocialActionVecItr Itr = m_vecInfo.begin(); Itr != m_vecInfo.end(); ++Itr )
	{
		if( (*Itr) )
		{
			delete (*Itr);
			(*Itr) = NULL;
		}
	}
	m_vecInfo.clear();
}

BOOL	AgcmSocialActon::Create()
{
	//Texture�� �ε��ؾ߰ڱ� ����
	m_pcsAgcmResourceLoader->SetTexturePath( "Texture\\UI\\BASE\\" );

	m_pcsAgpmGrid->Init( &m_cGrid, 1, 5, 4 );
	m_cGrid.m_lGridType = AGPDGRID_TYPE_SOCIALACTION;
	m_pcsAgpmGrid->Reset( &m_cGrid );

	int nCount = 0;
	for( SocialActionVecItr Itr = m_vecInfo.begin(); Itr != m_vecInfo.end(); ++Itr, ++nCount )
	{
		(*Itr)->pGridItem	= m_pcsAgpmGrid->CreateGridItem();
		(*Itr)->pTexture	= RwTextureRead( (*Itr)->szIcon, NULL );

		if( (*Itr)->pTexture )
		{
			RwTextureSetFilterMode( (*Itr)->pTexture, rwFILTERNEAREST );
			RwTextureSetAddressing( (*Itr)->pTexture, rwTEXTUREADDRESSCLAMP );
			(*m_pcsAgcmUIControl->GetAttachGridItemTextureData( (*Itr)->pGridItem )) = (*Itr)->pTexture;
		}

		(*Itr)->pGridItem->m_eType		= AGPDGRID_ITEM_TYPE_SOCIALACTION;
		(*Itr)->pGridItem->m_bMoveable	= TRUE;
		(*Itr)->pGridItem->m_lItemID	= nCount;
		int nRow = nCount / 4, nColum = nCount % 4;
		m_pcsAgpmGrid->Add( &m_cGrid, 0, nRow, nColum, (*Itr)->pGridItem, 1, 1 );
	}

	//Tooltip Setting
	m_cTooltip.m_Property.bTopmost = TRUE;
	m_pcsAgcmUIManager2->AddWindow( (AgcWindow*)&m_cTooltip );
	m_cTooltip.ShowWindow( FALSE );

	return TRUE;
}

void	AgcmSocialActon::Destory()
{
	//Texture�� �� ������� �Ѵ�..
	for( SocialActionVecItr Itr = m_vecInfo.begin(); Itr != m_vecInfo.end(); ++Itr )
	{
		m_pcsAgpmGrid->DeleteGridItem( (*Itr)->pGridItem );
		delete (*Itr);
	}
	m_vecInfo.clear();

	m_pcsAgpmGrid->Clear( &m_cGrid, 1, 5, 4, 1, 1 );
	m_pcsAgpmGrid->Remove( &m_cGrid );
}

BOOL	AgcmSocialActon::Load( char* szFilename, BOOL bDecryption )
{
	if( !szFilename )				return FALSE;
	if( !strlen( szFilename ) )		return FALSE;
	
	AuExcelTxtLib	cExcel;
	if( !cExcel.OpenExcelFile( szFilename, TRUE, bDecryption ) )
		return FALSE;

	for( int nRow = 1; nRow < cExcel.GetRow(); ++nRow )
	{
		char* szIcon = cExcel.GetData( 1, nRow );
		char* szCommand = cExcel.GetData( 2, nRow );
		char* szTooltipTitle = cExcel.GetData( 2, nRow )+1;
		char* szTooltipText = cExcel.GetData( 3, nRow );

#ifdef _DEBUG
		char strDebug[ 256 ] = { 0, };
		sprintf_s( strDebug, sizeof( char ) * 256, " -- Add Social Action : IconFile = %s, Command = %s, TooltipTitle = %s, TooltipText = %s\n",
			szIcon, szCommand, szTooltipTitle, szTooltipText );
		OutputDebugString( strDebug );
#endif

		m_vecInfo.push_back( new SocialAction( szIcon, szCommand, szTooltipTitle, szTooltipText ) );
	}

	return Create();
}

const SocialAction* AgcmSocialActon::GetInfo(UINT32 nNum)
{
	return nNum < AGPDCHAR_SOCIAL_TYPE_SPECIAL1 ? m_vecInfo[nNum] : NULL;
}


void	AgcmSocialActon::SetModule( AgpmGrid* pAgpmGrid, AgcmResourceLoader* pResourceLoader, AgcmUIControl* pUIControl, AgcmUIManager2* pUIManager2 )
{
	m_pcsAgpmGrid				= pAgpmGrid;
	m_pcsAgcmResourceLoader		= pResourceLoader;
	m_pcsAgcmUIControl			= pUIControl;
	m_pcsAgcmUIManager2			= pUIManager2;
}

BOOL	AgcmSocialActon::OpenTooltip( int nIndex )
{
	const SocialAction* pInfo = GetInfo( nIndex );
	if( !pInfo )	return FALSE;

	int nX = (int)m_pcsAgcmUIManager2->m_v2dCurMousePos.x + 30;
	int nY = (int)m_pcsAgcmUIManager2->m_v2dCurMousePos.y + 30;
	m_cTooltip.MoveWindow( nX, nY, m_cTooltip.w, m_cTooltip.h );

	//Setting String
	m_cTooltip.AddString( (CHAR*)pInfo->szTooltipTitle, 14, 0xffffffff );
	m_cTooltip.AddNewLine( 14 );
	m_cTooltip.AddString( (CHAR*)pInfo->szTooltipText, 12, 0xffffffff );
	m_cTooltip.AddNewLine( 14 );

	m_cTooltip.ShowWindow( TRUE );

	return TRUE;
}

void	AgcmSocialActon::CloseTooltip()
{
	m_cTooltip.DeleteAllStringInfo();
	m_cTooltip.ShowWindow( FALSE );
}