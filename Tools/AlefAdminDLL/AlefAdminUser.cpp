// AlefAdminUser.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminUser.h"
#include "AgppAdmin.h"

#include "AgcmCharacter.h"
#include "ApmMap.h"
#include "AgpmAdmin.h"
#include "AlefAdminManager.h"

// AlefAdminUser

IMPLEMENT_DYNCREATE(AlefAdminUser, CFormView)

AlefAdminUser::AlefAdminUser()
	: CFormView(AlefAdminUser::IDD)
{

}

AlefAdminUser::~AlefAdminUser()
{
}

void AlefAdminUser::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTMAIN, m_ListMain);
	DDX_Control(pDX, IDC_LISTDUNGEON, m_listDungeon);
	DDX_Control(pDX, IDC_LISTBATTLE, m_listBattle);
	DDX_Control(pDX, IDC_LISTSPIRIT, m_listSpirit);
	DDX_Control(pDX, IDC_LISTVILLAGE, m_listVillage);
	DDX_Control(pDX, IDC_STATIC_MAINUSER, m_staticMainCount);
	DDX_Control(pDX, IDC_STATIC_DUNGEONUSER, m_staticDungeonCount);
	DDX_Control(pDX, IDC_STATIC_BATTLEUSER, m_staticBattleCount);
	DDX_Control(pDX, IDC_STATIC_SPIRITUSER, m_staticSpiritCount);
	DDX_Control(pDX, IDC_STATIC_VILLAGEUSER, m_staticVillageCount);
	DDX_Control(pDX, IDC_STATIC_ALLUSER, m_staticAllUser);
}

BEGIN_MESSAGE_MAP(AlefAdminUser, CFormView)
	ON_BN_CLICKED(IDC_USER_REQUEST, &AlefAdminUser::OnBnClickedUserRequest)
END_MESSAGE_MAP()


// AlefAdminUser �����Դϴ�.

#ifdef _DEBUG
void AlefAdminUser::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void AlefAdminUser::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// AlefAdminUser �޽��� ó�����Դϴ�.

void AlefAdminUser::OnBnClickedUserRequest()
{
	SendUserRequest();
}

BOOL AlefAdminUser::SendUserRequest( VOID )
{
	
	AgcmCharacter*	pCharacter	=	static_cast< AgcmCharacter* >(AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgcmCharacter" ));
	AgpmAdmin*		pAdmin		=	static_cast< AgpmAdmin*		>(AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmAdmin" ));
	PACKET_AGPMADMIN_USER_POSITION_REQUEST	request( pCharacter->GetSelfCharacter()->GetID() );

	UINT16			nLength		= request.unPacketLength + 1; // end guard byte

	PVOID pvPacketRaw = g_AuCircularBuffer.Alloc(nLength);		// ��Ŷ �յڷ� �������Ʈ�� ���δ�.

	if (!pvPacketRaw) return FALSE;

	CopyMemory(pvPacketRaw, &request, nLength);

	// set guard byte
	*((BYTE *) pvPacketRaw)					= APPACKET_FRONT_GUARD_BYTE;
	*((BYTE *) pvPacketRaw + nLength - 1)	= APPACKET_REAR_GUARD_BYTE;

	((PACKET_HEADER *) pvPacketRaw)->unPacketLength		= nLength;
	((PACKET_HEADER *) pvPacketRaw)->lOwnerID			= 0;
	((PACKET_HEADER *) pvPacketRaw)->Flag.Compressed	= 0;
	((PACKET_HEADER *) pvPacketRaw)->cType				= request.cType;

	return	pAdmin->SendPacket( pvPacketRaw , nLength );
}

void AlefAdminUser::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	AgpmAdmin*		pAdmin		=	static_cast< AgpmAdmin* >(AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmAdmin" ));

	m_ListMain.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_listBattle.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_listDungeon.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_listSpirit.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_listVillage.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_ListMain.InsertColumn( 0 , _T("�����̸�") , LVCFMT_CENTER , 150 );
	m_ListMain.InsertColumn( 1 , _T("������") , LVCFMT_CENTER , 80 );

	m_listBattle.InsertColumn( 0 , _T("�����̸�") , LVCFMT_CENTER , 150 );
	m_listBattle.InsertColumn( 1 , _T("������") , LVCFMT_CENTER , 80 );

	m_listDungeon.InsertColumn( 0 , _T("�����̸�") , LVCFMT_CENTER , 150 );
	m_listDungeon.InsertColumn( 1 , _T("������") , LVCFMT_CENTER , 80 );

	m_listSpirit.InsertColumn( 0 , _T("�����̸�") , LVCFMT_CENTER , 150 );
	m_listSpirit.InsertColumn( 1 , _T("������") , LVCFMT_CENTER , 80 );

	m_listVillage.InsertColumn( 0 , _T("�����̸�") , LVCFMT_CENTER , 150 );
	m_listVillage.InsertColumn( 1 , _T("������") , LVCFMT_CENTER , 80 );

	
	// Callback 
	pAdmin->SetCallbackUserPositionInfo( CBUserInfoUpdate , this );
}


BOOL	AlefAdminUser::CBUserInfoUpdate( PVOID pData , PVOID pClass , PVOID pCustData )
{
	AlefAdminUser*					pAdminUser	=	static_cast< AlefAdminUser* >(pClass);
	PACKET_AGPMADMIN_USER_POSITON *	pPacket		=	static_cast< PACKET_AGPMADMIN_USER_POSITON* >(pData);

	if( !pPacket || !pAdminUser )
		return FALSE;	

	pAdminUser->InitRegion();

	for( INT i = 0 ;  i < AGPMADMIN_MAX_REGION_INDEX ; ++i )
	{
		if( pPacket->arRegionIndex[ i ] == -1 )
			continue;

		pAdminUser->UpdateRegion( i , pPacket->arRegionIndex[i] );
	}

	pAdminUser->UpdateUserCount();
}

VOID	AlefAdminUser::InitRegion( VOID )
{
	m_ListMain.DeleteAllItems();
	m_listDungeon.DeleteAllItems();
	m_listBattle.DeleteAllItems();
	m_listSpirit.DeleteAllItems();
	m_listVillage.DeleteAllItems();

	m_staticMainCount.SetWindowText( _T("0") );
	m_staticDungeonCount.SetWindowText( _T("0") );
	m_staticBattleCount.SetWindowText( _T("0") );
	m_staticSpiritCount.SetWindowText( _T("0") );
	m_staticVillageCount.SetWindowText( _T("0") );
	m_staticAllUser.SetWindowText( _T("0") );

	ZeroMemory( m_arrRegionUserCount , sizeof(m_arrRegionUserCount) );
}

VOID	AlefAdminUser::UpdateRegion( INT nRegionIndex , INT nRegionUserCount )
{

	eRegionType		eType			=	GetRegionType( nRegionIndex );
	const char*		szRegionName	=	GetRegionName( nRegionIndex );
	INT				nItemCount		=	0;
	CString			strUserCount;
	
	// ���� �̸��� ��� ���� ���ߴٸ� ���� �����̴�
	if( !szRegionName )
		return;

	strUserCount.Format( "%d" , nRegionUserCount );
	
	switch( eType )
	{
	case REGION_MAIN:
		nItemCount	=	m_ListMain.GetItemCount();
		m_ListMain.InsertItem( nItemCount , szRegionName ); 
		m_ListMain.SetItemText( nItemCount , 1 , strUserCount );		
		break;

	case REGION_BATTLE:	
		nItemCount	=	m_listBattle.GetItemCount();
		m_listBattle.InsertItem( nItemCount , szRegionName ); 
		m_listBattle.SetItemText( nItemCount , 1 , strUserCount );		
		break;

	case REGION_VILLAGE:
		nItemCount	=	m_listVillage.GetItemCount();
		m_listVillage.InsertItem( nItemCount , szRegionName ); 
		m_listVillage.SetItemText( nItemCount , 1 , strUserCount );		
		break;

	case REGION_SPIRIT:	
		nItemCount	=	m_listSpirit.GetItemCount();
		m_listSpirit.InsertItem( nItemCount , szRegionName ); 
		m_listSpirit.SetItemText( nItemCount , 1 , strUserCount );		
		break;

	case REGION_DUNGEON:
		nItemCount	=	m_listDungeon.GetItemCount();
		m_listDungeon.InsertItem( nItemCount , szRegionName ); 
		m_listDungeon.SetItemText( nItemCount , 1 , strUserCount );		
		break;
	}

	AddRegionUserCount( eType , nRegionUserCount );
}

VOID AlefAdminUser::UpdateUserCount( VOID )
{
	CString			strTemp;
	INT				nAllUser	=	0;

	strTemp.Format( _T("%d") , m_arrRegionUserCount[ REGION_MAIN ] );
	m_staticMainCount.SetWindowText( strTemp );

	strTemp.Format( _T("%d") , m_arrRegionUserCount[ REGION_BATTLE ] );
	m_staticBattleCount.SetWindowText( strTemp );

	strTemp.Format( _T("%d") , m_arrRegionUserCount[ REGION_SPIRIT ] );
	m_staticSpiritCount.SetWindowText( strTemp );

	strTemp.Format( _T("%d") , m_arrRegionUserCount[ REGION_DUNGEON ] );
	m_staticDungeonCount.SetWindowText( strTemp );

	strTemp.Format( _T("%d") , m_arrRegionUserCount[ REGION_VILLAGE ] );
	m_staticVillageCount.SetWindowText( strTemp );

	for( INT i = 0 ; i < REGION_COUNT ; ++ i )
	{
		nAllUser	+=	m_arrRegionUserCount[ i ];
	}

	strTemp.Format( _T("%d") , nAllUser );
	m_staticAllUser.SetWindowText( strTemp );

}

const char* AlefAdminUser::GetRegionName( INT nRegionIndex )
{

	ApmMap*						pMap			=	static_cast< ApmMap* >(AlefAdminManager::Instance()->GetAdminModule()->GetModule( "ApmMap" ));
	ApmMap::RegionTemplate*		pRegionTemplate	=	pMap->GetTemplate( nRegionIndex );

	if( pRegionTemplate )
		return pRegionTemplate->pStrName;


	return NULL;
}

eRegionType AlefAdminUser::GetRegionType( INT nRegionIndex )
{
	ApmMap*						pMap			=	static_cast< ApmMap* >(AlefAdminManager::Instance()->GetAdminModule()->GetModule( "ApmMap" ));
	ApmMap::RegionTemplate*		pRegionTemplate	=	pMap->GetTemplate( nRegionIndex );

	if( pRegionTemplate )
	{
		if( pRegionTemplate->nParentIndex	==	110 )
			return REGION_BATTLE;

		return REGION_MAIN;
	}

	return REGION_COUNT;
}

VOID AlefAdminUser::AddRegionUserCount( eRegionType eType , INT nRegionUserCount  )
{
	m_arrRegionUserCount[ eType ] += nRegionUserCount;
}

