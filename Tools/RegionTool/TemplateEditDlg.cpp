// TemplateEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "regiontool.h"
#include "TemplateEditDlg.h"
#include "RegionToolDlg.h"
#include "WorldMapSelectDlg.h"
#include "SelectSkySetDlg.h"
#include "RegionSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateEditDlg dialog


CTemplateEditDlg::CTemplateEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTemplateEditDlg::IDD, pParent)
	, m_bRidable		(TRUE)
	, m_bPet			(TRUE)
	, m_bItemWangBok	(TRUE)
	, m_bItemPotion		(TRUE)
	, m_bItemResurrect	(TRUE)
	, m_bDisableMinimap	(FALSE)
	, m_bJail			(FALSE)
	, m_bCharacterBlock (FALSE)
	, m_bItemPositonType2(FALSE)
	, m_bItemPositonGuild(FALSE)
	, m_bZoneLoading	(FALSE)
	, m_strWorldMap(_T(""))
	, m_strSkySet(_T(""))
	, m_fSrcX(0)
	, m_fSrcZ(0)
	, m_fSrcHeight(0)
	, m_nRadius(0)
	, m_fDstX(0)
	, m_fDstZ(0)
	, m_nVDistance(12)
	, m_nLevelLimit( 0 )
	, m_nLevelMin( 0 )
	, m_pTemplate( NULL )
	, m_nParentIndex(-1)
	, m_bRecallItemUse(FALSE)
	, m_fResurrecX(0)
	, m_fResurrecZ(0)
	, m_bVitalPotionUse(FALSE)
	, m_bCurePotionUse(FALSE)
	, m_bRecoveryPotionUse(FALSE)
{
	//{{AFX_DATA_INIT(CTemplateEditDlg)
	m_strComment	= _T("여기다가는 주석을..");
	m_nIndex		= 0		;
	m_strName		= _T("여기엔 이름을..");
//	m_nPriority		= 0		;
	//}}AFX_DATA_INIT

	m_nType		= ApmMap::FT_FIELD	;
	m_nSafety	= ApmMap::ST_SAFE	;
	m_nWorldMapIndex	= -1;	// 지도 없음.
	m_nDefaultSkySet	= 13;


	InitSectionString();
	
}

VOID CTemplateEditDlg::InitSectionString( VOID )
{
	strcpy_s( m_arrStrDisableItemTypeName[ AGPMITEM_SECTION_TYPE_UNABLE_USE_EPIC_ZONE ] , MAX_PATH , _T("UNABLE_EPICITEM(1)") );
}


void CTemplateEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTemplateEditDlg)
	DDX_Control(pDX, IDC_TYPE, m_ctlTypeCombo);
	DDX_Text(pDX, IDC_COMMENT, m_strComment);
	DDX_Text(pDX, IDC_INDEX, m_nIndex);
	DDV_MinMaxInt(pDX, m_nIndex, 0, 65535);
	DDX_Text(pDX, IDC_NAME, m_strName);
	//	DDX_Slider(pDX, IDC_PRIORITY, m_nPriority);
	DDX_Control(pDX, IDC_SAFETY, m_ctlSafety);
	DDX_Check(pDX, IDC_RIDABLE, m_bRidable);
	DDX_Check(pDX, IDC_PET, m_bPet);
	DDX_Check(pDX, IDC_ITEMTELEPORT, m_bItemWangBok);
	DDX_Check(pDX, IDC_ITEMPOTION, m_bItemPotion);
	DDX_Check(pDX, IDC_ITEMRESURRECT, m_bItemResurrect);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_DISABLEMINIMAP, m_bDisableMinimap);
	DDX_Check(pDX, IDC_JAIL, m_bJail);
	DDX_Check(pDX, IDC_CHARACTER_BLOCK, m_bCharacterBlock);
	DDX_Check(pDX, IDC_ITEM_POTION_TYPE2, m_bItemPositonType2);
	DDX_Check(pDX, IDC_ITEM_POTION_GUILD, m_bItemPositonGuild);
	DDX_Text(pDX, IDC_WORLDMAP, m_strWorldMap);
	DDX_Text(pDX, IDC_DEFAULTSKYSET, m_strSkySet);
	DDX_Check(pDX, IDC_ZONELOADING, m_bZoneLoading);
	DDX_Text(pDX, IDC_SRC_X, m_fSrcX);
	DDX_Text(pDX, IDC_SRC_Z, m_fSrcZ);
	DDX_Text(pDX, IDC_SRC_HEIGHT, m_fSrcHeight);
	DDX_Text(pDX, IDC_RADIUS, m_nRadius);
	DDX_Text(pDX, IDC_DST_X, m_fDstX);
	DDX_Text(pDX, IDC_DST_Z, m_fDstZ);
	DDX_Text(pDX, IDC_VDISTANCE, m_nVDistance);
	DDX_Text(pDX, IDC_LEVELLIMIT, m_nLevelLimit);
	DDX_Text(pDX, IDC_LEVELMIN, m_nLevelMin);
	DDX_Text(pDX, IDC_PARENTINDEX, m_nParentIndex);
	DDX_Control(pDX, IDC_LIST_ITEMTYPE, m_listboxDisableItemType);
	DDX_Control(pDX, IDC_COMBO_ITEMTYPE, m_comboDisableItemTypeList);
	DDX_Check(pDX, IDC_CALLITEM_NOTUSE, m_bRecallItemUse);
	DDX_Text(pDX, IDC_EDIT_RESURRECTION_X, m_fResurrecX);
	DDX_Text(pDX, IDC_EDIT_RESURRECTION_Z, m_fResurrecZ);
	DDX_Check(pDX, IDC_ITEM_POTION_VITAL, m_bVitalPotionUse);
	DDX_Check(pDX, IDC_ITEM_POTION_CURE, m_bCurePotionUse);
	DDX_Check(pDX, IDC_ITEM_POTION_RECOVERY, m_bRecoveryPotionUse);
}


BEGIN_MESSAGE_MAP(CTemplateEditDlg, CDialog)
	//{{AFX_MSG_MAP(CTemplateEditDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SELECTWORLDMAP, OnBnClickedSelectworldmap)
	ON_BN_CLICKED(IDC_SELECTSKYSET, OnBnClickedSelectskyset)
	ON_BN_CLICKED(IDC_ZONELOADING, OnBnClickedZoneloading)
	ON_BN_CLICKED(IDC_CHOICEREGION, &CTemplateEditDlg::OnBnClickedChoiceregion)
	ON_BN_CLICKED(IDC_BUTTON_ITEMADD, &CTemplateEditDlg::OnBnClickedButtonItemadd)
	ON_BN_CLICKED(IDC_BUTTON_ITEMDEL, &CTemplateEditDlg::OnBnClickedButtonItemdel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateEditDlg message handlers

BOOL CTemplateEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

/*
	CSliderCtrl * pWnd = ( CSliderCtrl * ) GetDlgItem( IDC_PRIORITY );
	pWnd->SetRange( 0 , 9 );
	pWnd->SetPos( m_nPriority);
*/	

	UpdateData( FALSE );

	m_ctlTypeCombo	.SetCurSel( m_nType );
	m_ctlSafety		.SetCurSel( m_nSafety );

	// 컨트롤 정리..
	OnBnClickedZoneloading();

	// 월드맵 스트링 설정
	UpdateWorldMap();

	// Disable Item Type 설정
	InitDisableItemType();

	SetSectionType( m_pTemplate->nUnableItemSectionNum );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTemplateEditDlg::OnOK() 
{
	m_nType		= m_ctlTypeCombo.GetCurSel();
	m_nSafety	= m_ctlSafety	.GetCurSel();

	if( m_pTemplate )
	{
		UpdateData( TRUE );

		m_pTemplate->nIndex		= m_nIndex		;
		strcpy( m_pTemplate->pStrName , ( LPCTSTR ) m_strName	);
//		m_pTemplate->nPriority	= m_nPriority	;
		m_pTemplate->nParentIndex = m_nParentIndex;
		strcpy( m_pTemplate->pStrComment , ( LPCTSTR ) m_strComment	);

		m_pTemplate->nUnableItemSectionNum	= GetSectionNum();


		// 마고자 (2005-08-24 오후 3:55:36) : 
		// 리젼 속성 여기에..
		m_pTemplate->ti.stType.uFieldType		= m_nType			;
		m_pTemplate->ti.stType.uSafetyType		= m_nSafety			;
		m_pTemplate->ti.stType.bRidable			= m_bRidable		;
		m_pTemplate->ti.stType.bPet				= m_bPet			;
		m_pTemplate->ti.stType.bItemWangbok		= m_bItemWangBok	;
		m_pTemplate->ti.stType.bItemPotion		= m_bItemPotion		;
		m_pTemplate->ti.stType.bRecallItem		= m_bRecallItemUse	;
		m_pTemplate->ti.stType.bVitalPotion		= m_bVitalPotionUse ;
		m_pTemplate->ti.stType.bCurePotion		= m_bCurePotionUse	;
		m_pTemplate->ti.stType.bRecoveryPotion	= m_bRecoveryPotionUse ;
		m_pTemplate->ti.stType.bItemResurrect	= m_bItemResurrect	;
		m_pTemplate->ti.stType.bDisableMinimap	= m_bDisableMinimap	;
		m_pTemplate->ti.stType.bJail			= m_bJail			;
		m_pTemplate->ti.stType.bCharacterBlock	= m_bCharacterBlock	;
		m_pTemplate->ti.stType.bZoneLoadArea	= m_bZoneLoading	;
		m_pTemplate->ti.stType.bItemPotion2		= m_bItemPositonType2;
		m_pTemplate->ti.stType.bItemPotionGuild	= m_bItemPositonGuild;
		
		m_pTemplate->nWorldMapIndex				= m_nWorldMapIndex	;
		m_pTemplate->nSkyIndex					= m_nDefaultSkySet	;

		m_pTemplate->zi.fSrcX				= m_fSrcX		;
		m_pTemplate->zi.fSrcZ				= m_fSrcZ		;
		m_pTemplate->zi.fHeightOffset		= m_fSrcHeight	;
		m_pTemplate->zi.fDstX				= m_fDstX		;
		m_pTemplate->zi.fDstZ				= m_fDstZ		;
		m_pTemplate->zi.nRadius				= m_nRadius		;

		m_pTemplate->fVisibleDistance			= ( FLOAT ) ( m_nVDistance * 100 );
		m_pTemplate->nLevelLimit			= m_nLevelLimit	;
		m_pTemplate->nLevelMin				= m_nLevelMin	;

		m_pTemplate->stResurrectionPos.x	= m_fResurrecX	;
		m_pTemplate->stResurrectionPos.z	= m_fResurrecZ	;
	}


	CDialog::OnOK();
}

INT  CTemplateEditDlg::GetSectionType( const char* szName )
{
	if( !strcmp( m_arrStrDisableItemTypeName[AGPMITEM_SECTION_TYPE_UNABLE_USE_EPIC_ZONE] , szName ) )
		return AGPMITEM_SECTION_TYPE_UNABLE_USE_EPIC_ZONE;

	return 0;
	
}

INT	 CTemplateEditDlg::GetSectionNum( VOID )
{
	INT		nCount			=	m_listboxDisableItemType.GetCount();
	INT		nReturnResult	=	AGPMITEM_SECTION_TYPE_NONE;
	CString	strTemp;

	for( INT i = 0 ; i < nCount ; ++i )
	{
		m_listboxDisableItemType.GetText( i , strTemp );

		nReturnResult	|=	GetSectionType( strTemp );
	}

	return nReturnResult;
}

void CTemplateEditDlg::OnBnClickedSelectworldmap()
{
	// 여기에 월드맵 에디터 를 추가함.
	// 현재 템플릿에서의 월드맵이 리스팅 돼고..
	// 원하는 월드맵을 추가할 수 있고 ,
	// 삭제할 수도 있다.
	// 그리고 연결돼는 아이템도 여기에서 설정한다.
	CWorldMapSelectDlg	dlg;
	dlg.m_nMapIndex = m_nWorldMapIndex;
	if( IDOK == dlg.DoModal() )
	{
		m_nWorldMapIndex = dlg.m_nMapIndex;

		// 월드맵 복사
		g_pcsApmMap->m_arrayWorldMap = dlg.m_arrayWorldMap;
		UpdateWorldMap();
	}
}

void	CTemplateEditDlg::UpdateWorldMap()
{
	// 월드맵 스트링 설정
	ApmMap::WorldMap * pWorld;
	pWorld = g_pcsApmMap->GetWorldMapInfo( m_nWorldMapIndex );
	if( pWorld )
	{
		m_strWorldMap.Format( "(%02d) %s" , m_nWorldMapIndex , pWorld->strComment );
	}
	else
	{
		// 어라 지도가 없네..
		m_strWorldMap.Format( "(%02d) %s" , m_nWorldMapIndex , "에러발생!!" );
		MessageBox( "지도이름 얻기 실패했어요, 이미 템플릿에서 삭제 돼었을 가능성이 높음" );
	}

	{
		AuList< AgpdSkySet * >	* pList = g_pcsAgpmEventNature->GetSkySetList();
		AuNode< AgpdSkySet * >	* pNode	= pList->GetHeadNode();
		AgpdSkySet				* pSkySet	;

		CString					str;
		while( pNode )
		{
			pSkySet	= pNode->GetData();

			if( pSkySet->m_nIndex == m_nDefaultSkySet )
			{
				m_strSkySet.Format( SKY_TEMPLATE_FORMAT , pSkySet->m_nIndex , pSkySet->m_strName );
				break;
			}
			pList->GetNext( pNode );
		}

		if( NULL == pNode )
		{
			m_strSkySet.Format( SKY_TEMPLATE_FORMAT , m_nDefaultSkySet , "스카이셋 에러!" );
		}
	}

	UpdateData( FALSE );
}

VOID CTemplateEditDlg::InitDisableItemType( VOID )
{

	m_comboDisableItemTypeList.ResetContent();
	m_listboxDisableItemType.ResetContent();
	
	for( INT i = 1 ; i < AGPMITEM_SECTION_TYPE_MAX  ; ++i )
	{

		m_comboDisableItemTypeList.AddString( m_arrStrDisableItemTypeName[i] );

	}

}

void CTemplateEditDlg::OnBnClickedSelectskyset()
{
	CSelectSkySetDlg	dlg;

	if( IDOK == dlg.DoModal() )
	{
		m_nDefaultSkySet	= dlg.m_nItemID;

		UpdateWorldMap();
	}
}

void CTemplateEditDlg::OnBnClickedZoneloading()
{
	UpdateData( TRUE );

	GetDlgItem( IDC_SRC_X		)->EnableWindow( m_bZoneLoading );
	GetDlgItem( IDC_SRC_Z		)->EnableWindow( m_bZoneLoading );
	GetDlgItem( IDC_SRC_HEIGHT	)->EnableWindow( m_bZoneLoading );
	GetDlgItem( IDC_RADIUS		)->EnableWindow( m_bZoneLoading );
	GetDlgItem( IDC_DST_X		)->EnableWindow( m_bZoneLoading );
	GetDlgItem( IDC_DST_Z		)->EnableWindow( m_bZoneLoading );
}

void	CTemplateEditDlg::SetTemplate( ApmMap::RegionTemplate * pTemplate )
{

	m_pTemplate = pTemplate;

	m_nIndex		= pTemplate->nIndex		;
	m_strName		= pTemplate->pStrName	;
//	m_nPriority		= pTemplate->nPriority	;
	m_nParentIndex  = pTemplate->nParentIndex;
	m_strComment	= pTemplate->pStrComment;

	m_nType				= pTemplate->ti.stType.uFieldType		;
	m_nSafety			= pTemplate->ti.stType.uSafetyType		;
	m_bRidable			= pTemplate->ti.stType.bRidable			;
	m_bPet				= pTemplate->ti.stType.bPet				;
	m_bItemWangBok		= pTemplate->ti.stType.bItemWangbok		;
	m_bItemPotion		= pTemplate->ti.stType.bItemPotion		;
	m_bRecallItemUse	= pTemplate->ti.stType.bRecallItem		;
	m_bVitalPotionUse	= pTemplate->ti.stType.bVitalPotion		;
	m_bCurePotionUse	= pTemplate->ti.stType.bCurePotion		;
	m_bRecoveryPotionUse= pTemplate->ti.stType.bRecoveryPotion	;
	m_bItemResurrect	= pTemplate->ti.stType.bItemResurrect	;
	m_bDisableMinimap	= pTemplate->ti.stType.bDisableMinimap	;
	m_bJail				= pTemplate->ti.stType.bJail			;
	m_bCharacterBlock	= pTemplate->ti.stType.bCharacterBlock	;
	m_bZoneLoading		= pTemplate->ti.stType.bZoneLoadArea	;
	m_bItemPositonType2	= pTemplate->ti.stType.bItemPotion2	;
	m_bItemPositonGuild	= pTemplate->ti.stType.bItemPotionGuild	;

	m_nWorldMapIndex= pTemplate->nWorldMapIndex				;
	m_nDefaultSkySet= pTemplate->nSkyIndex					;

	m_fSrcX			= pTemplate->zi.fSrcX				;
	m_fSrcZ			= pTemplate->zi.fSrcZ				;
	m_fSrcHeight	= pTemplate->zi.fHeightOffset		;
	m_fDstX			= pTemplate->zi.fDstX				;
	m_fDstZ			= pTemplate->zi.fDstZ				;
	m_nRadius		= pTemplate->zi.nRadius				;

	m_nVDistance	= ( int ) ( pTemplate->fVisibleDistance / 100.0f );
	m_nLevelLimit	= pTemplate->nLevelLimit			;
	m_nLevelMin		= pTemplate->nLevelMin				;

	m_fResurrecX	= pTemplate->stResurrectionPos.x	;
	m_fResurrecZ	= pTemplate->stResurrectionPos.z	;

}

void CTemplateEditDlg::OnBnClickedChoiceregion()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CRegionSelectDlg dlg;

	if( IDOK == dlg.DoModal() )
	{
		m_nParentIndex = atoi(dlg.m_strSelectParentIndex);

		UpdateData(FALSE);
	}

}

void CTemplateEditDlg::OnBnClickedButtonItemadd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	INT nSel		=	m_comboDisableItemTypeList.GetCurSel();

	// 선택된 셀이 없으면 리턴
	if( nSel == -1 )
		return;

	CString strSection;
	m_comboDisableItemTypeList.GetLBText( nSel , strSection );

	// 이미 추가된 타입이면 리턴
	if( CheckOverlapString( strSection ) )
		return;

	m_listboxDisableItemType.AddString( strSection );

}

BOOL	CTemplateEditDlg::CheckOverlapString( CString&	strData )
{
	INT			nCount		=	m_listboxDisableItemType.GetCount();
	CString		strTemp;

	for( INT i = 0 ; i < nCount ; ++i )
	{
		m_listboxDisableItemType.GetText( i , strTemp );

		if( !strData.Compare( strTemp ) )
			return TRUE;
	}

	return FALSE;

}

void CTemplateEditDlg::OnBnClickedButtonItemdel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	INT nSel		=	m_listboxDisableItemType.GetCurSel();

	if( nSel == -1 )
		return;

	CString strSection;
	m_listboxDisableItemType.DeleteString( nSel );
}


VOID	CTemplateEditDlg::SetSectionType( INT nSectionNum )
{
	if( nSectionNum & AGPMITEM_SECTION_TYPE_UNABLE_USE_EPIC_ZONE )
		m_listboxDisableItemType.AddString( m_arrStrDisableItemTypeName[AGPMITEM_SECTION_TYPE_UNABLE_USE_EPIC_ZONE] );
	
}