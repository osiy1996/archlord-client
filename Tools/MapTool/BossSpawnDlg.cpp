// BossSpawnDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "BossSpawnDlg.h"
#include "MyEngine.h"

// CBossSpawnDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CBossSpawnDlg, CDialog)

CBossSpawnDlg::CBossSpawnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBossSpawnDlg::IDD, pParent)
{
	m_pSelectCharacter	=	NULL;
    
	ZeroMemory( m_pArrayNumTexture , sizeof(m_pArrayNumTexture) );
}

CBossSpawnDlg::~CBossSpawnDlg( VOID )
{
	for( INT i = 0 ; i < PATH_TEXTURENUM_COUNT ; ++i )
	{
		if( m_pArrayNumTexture[i] )
		{
			RwTextureDestroy( m_pArrayNumTexture[i] );
			m_pArrayNumTexture[i]		=	NULL;
		}
	}

}

void CBossSpawnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SPAWNLIST, m_listboxSpawn);
	DDX_Control(pDX, IDC_LIST_PATHLIST, m_listboxPath);
	DDX_Control(pDX, IDC_STATIC_CHARACTER, m_staticCharacter);
	DDX_Control(pDX, IDC_STATIC_SPAWN, m_staticSpawn);
	DDX_Control(pDX, IDC_STATIC_POSITION, m_staticPosition);
	DDX_Control(pDX, IDC_STATIC_SPAWN_COUNT, m_staticPathCount);
	DDX_Control(pDX, IDC_CHECK_PATHSET, m_checkPathSet);
	DDX_Control(pDX, IDC_RADIO_PATHLOOP, m_radioPathType);
}


BEGIN_MESSAGE_MAP(CBossSpawnDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CBossSpawnDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, &CBossSpawnDlg::OnBnClickedButtonReload)
	ON_BN_CLICKED(IDC_BUTTON_ALLSAVE, &CBossSpawnDlg::OnBnClickedButtonAllsave)
	ON_BN_CLICKED(IDC_RADIO_PATHLOOP, &CBossSpawnDlg::OnBnClickedRadioPathloop)
	ON_BN_CLICKED(IDC_RADIO_PATHREVERSE, &CBossSpawnDlg::OnBnClickedRadioPathreverse)
	ON_LBN_SELCHANGE(IDC_LIST_SPAWNLIST, &CBossSpawnDlg::OnLbnSelchangeListSpawnlist)
END_MESSAGE_MAP()


// CBossSpawnDlg 메시지 처리기입니다.


VOID	CBossSpawnDlg::AddSpawnName( const CString& strSpawnName )
{
	m_listboxSpawn.AddString( strSpawnName );
}

INT		CBossSpawnDlg::GetCurSel( VOID )
{
	return m_listboxSpawn.GetCurSel();
}

VOID	CBossSpawnDlg::SetSelectBoss( const char* strSpawnName , const char* strCharacterName , AuPOS vPos )
{
	CString		strPosition;

	m_staticSpawn.SetWindowText( strSpawnName );
	m_staticCharacter.SetWindowText( strCharacterName );

	strPosition.Format( "%d , %d" , (INT)vPos.x , (INT)vPos.z );
	m_staticPosition.SetWindowText( strPosition );

	UpdatePathListBox();
}

VOID	CBossSpawnDlg::GetText( INT nSel , CString& strText )
{
	m_listboxSpawn.GetText( nSel , strText );
}

STMonsterPathInfoPtr	CBossSpawnDlg::CreateBossSpawnPath( CString& strBossName , ePathType eType /* = E_PATH_LOOP */ )
{
	return m_MonsterPath.CreatePathInfo( strBossName.GetString() , eType );
}

BOOL	CBossSpawnDlg::DestroyBossSpawnPath( STMonsterPathInfoPtr pBossPath )
{
	if( !pBossPath )
		return FALSE;

	return m_MonsterPath.DestroyPathInfo( pBossPath );
}

BOOL	CBossSpawnDlg::DestroyBossSpawnPath( CString& strBossName )
{
	STMonsterPathInfoPtr		pMonsterPath	=	GetMonsterSpawnData( strBossName );

	return DestroyBossSpawnPath( pMonsterPath );
}

BOOL	CBossSpawnDlg::AddBossSpawnPath( STMonsterPathInfoPtr pBossPath , AuPOS vPos )
{
	if( !pBossPath )
		return FALSE;

	m_MonsterPath.AddMonsterPath( pBossPath , vPos );

	return TRUE;
}

BOOL	CBossSpawnDlg::AddBossSpawnPath( CString& strBossName , AuPOS vPos )
{
	STMonsterPathInfoPtr	pMonsterPath	=	GetMonsterSpawnData( strBossName );

	return AddBossSpawnPath( pMonsterPath , vPos );
}

BOOL	CBossSpawnDlg::SetBossSpawnPath( STMonsterPathInfoPtr pBossPath , INT nIndex , RwV3d vPos )
{
	if( !pBossPath )
		return FALSE;

	m_MonsterPath.EditMonsterPath( pBossPath , nIndex , vPos.x , vPos.z );

	return TRUE;
}

BOOL	CBossSpawnDlg::SetBossSpawnPath( CString& strBossName , INT nIndex , RwV3d vPos )
{
	STMonsterPathInfoPtr		pMonsterPath		=	GetMonsterSpawnData( strBossName );


	return SetBossSpawnPath( pMonsterPath , nIndex , vPos );

}

BOOL	CBossSpawnDlg::SetBossSpawnType( STMonsterPathInfoPtr pBossPath , ePathType eType )
{
	if( !pBossPath )
		return FALSE;

	return m_MonsterPath.EditPathType( pBossPath , eType );

}

BOOL	CBossSpawnDlg::SetBossSpawnType( CString& strBossName , ePathType eType )
{
	STMonsterPathInfoPtr		pMonsterPath		=	GetMonsterSpawnData( strBossName );
	return SetBossSpawnType( pMonsterPath , eType );
}

INT	CBossSpawnDlg::DelBossSpawnPath( STMonsterPathInfoPtr pBossPath , AuPOS vPos )
{
	if( !pBossPath )
		return FALSE;


	return m_MonsterPath.DelMonsterPath( pBossPath , vPos );
}

INT	CBossSpawnDlg::DelBossSpawnPath( CString& strBossName , AuPOS vPos )
{
	STMonsterPathInfoPtr		pMonsterPath		=	GetMonsterSpawnData( strBossName );
	RpClump*					pClump				=	RpClumpCreate();

	return DelBossSpawnPath( pMonsterPath , vPos );
}

AuPOS	CBossSpawnDlg::GetBossSpawnIndexPos( STMonsterPathInfoPtr pBossPath , INT nIndex )
{
	AuPOS		ResultPos;

	if( !pBossPath )
		return ResultPos;
 
	MonsterPathListIter	Iter	=	pBossPath->PathList.begin();
	for( ; Iter != pBossPath->PathList.end() ; ++Iter )
	{
		if( Iter->nPointIndex == nIndex )
		{
			ResultPos.x	=	Iter->nX;
			ResultPos.z	=	Iter->nZ;

			break;
		}
	}


	return ResultPos;
}

AuPOS	CBossSpawnDlg::GetBossSpawnIndexPos( CString& strBossName , INT nIndex )
{
	STMonsterPathInfoPtr	pBossPath		=	m_MonsterPath.GetMonsterPath( strBossName.GetString() );

	return GetBossSpawnIndexPos( pBossPath , nIndex );
}

INT		CBossSpawnDlg::GetBossSpawnPathCount( STMonsterPathInfoPtr pBossPath )
{
	if( !pBossPath )
		return 0;

	return pBossPath->nPointCount;
	
}

INT		CBossSpawnDlg::GetBossSpawnPathCount( CString& strBossName )
{
	STMonsterPathInfoPtr		pMonsterPath =	m_MonsterPath.GetMonsterPath( strBossName.GetString() );

	return GetBossSpawnPathCount( pMonsterPath );
}

STMonsterPathInfoPtr	CBossSpawnDlg::GetMonsterSpawnData( CString& strBossName )
{
	if( strBossName == "" )
		return NULL;

	return m_MonsterPath.GetMonsterPath( strBossName.GetString() );
}

VOID	CBossSpawnDlg::UpdatePathListBox( VOID )
{
	CString		str;

	m_listboxPath.ResetContent();
	m_staticPathCount.SetWindowText( "0" );

	if( m_pSelectCharacter	)
	{
		STMonsterPathInfoPtr	pMonsterPath	=	m_MonsterPath.GetMonsterPath( m_pSelectCharacter->m_szID );


		if( pMonsterPath )
		{
			MonsterPathListIter	Iter	=	pMonsterPath->PathList.begin();
			for( ; Iter != pMonsterPath->PathList.end() ; ++Iter )
			{
				str.Format( "[%d]  <%d> <%d>" , Iter->nPointIndex , Iter->nX , Iter->nZ  );
				m_listboxPath.AddString( str );		

			}

			// 몬스터 포인트 갯수를 찍어준다
			str.Format( "%d" , pMonsterPath->nPointCount );
			m_staticPathCount.SetWindowText( str );

			// 몬스터 Path Type을 지정한다
			if( pMonsterPath->eType == E_PATH_LOOP )
				CheckRadioButton( IDC_RADIO_PATHLOOP , IDC_RADIO_PATHREVERSE , IDC_RADIO_PATHLOOP );
			else
				CheckRadioButton( IDC_RADIO_PATHLOOP , IDC_RADIO_PATHREVERSE , IDC_RADIO_PATHREVERSE );
		}

		// 화면에 렌더링 될 부분을 셋팅한다
		RenderUpdateBossPath( pMonsterPath );

	}
}

VOID	CBossSpawnDlg::RenderUpdateBossPath( STMonsterPathInfoPtr pBossPath )
{
	_ClearClumpList();

	if( !pBossPath )
		return;

	MonsterPathListIter	Iter	=	pBossPath->PathList.begin();
	for( ; Iter != pBossPath->PathList.end()  ; ++Iter )
	{
		_AddClump( Iter->nX , Iter->nZ );
	}

}

VOID	CBossSpawnDlg::RenderUpdateBossPath( CString& strBossName )
{
	STMonsterPathInfoPtr	pMonsterPath	=	m_MonsterPath.GetMonsterPath( strBossName.GetString() );		

	RenderUpdateBossPath( pMonsterPath );
}

BOOL	CBossSpawnDlg::_AddClump( INT nX , INT nZ )
{
	AuPOS	pos;
	INT		nCount;
	RwRGBA	ClearColor;

	pos.x	=	nX;
	pos.y	=	0;
	pos.z	=	nZ;

	ClearColor.alpha	=	255;
	ClearColor.blue		=	0;
	ClearColor.green	=	0;
	ClearColor.red		=	0;


	pos.y = AGCMMAP_THIS->GetHeight_Lowlevel( nX, nZ , SECTOR_MAX_HEIGHT);

	RpClump*		pClump	=	g_pcsAgcmObject->LoadClump( "waypoint.rws");
	if( !pClump )
		return FALSE;

	nCount	=	(INT)m_ClumpList.size();
	
	RpClumpForAllAtomics( pClump , CBClumpSetting , m_pArrayNumTexture[nCount] );
	
	RwFrameTranslate(RpClumpGetFrame(pClump), (RwV3d *)(&pos), rwCOMBINEREPLACE);
	g_pcsAgcmRender->AddClumpToWorld( pClump );

	m_ClumpList.push_back( pClump );

	return TRUE;
}

RpAtomic*	CBossSpawnDlg::CBClumpSetting( RpAtomic*	pAtomic , PVOID data )
{
	RpGeometry*	pGeometry	=	RpAtomicGetGeometry( pAtomic );
	RwTexture*	pTexture	=	(RwTexture*)data;

	RpGeometryForAllMaterials( pGeometry , CBossSpawnDlg::CBMaterialSetTexture , pTexture );

	return pAtomic;
}

BOOL	CBossSpawnDlg::_ClumpNumberSetting( RpClump* pClump , INT nNumber )
{
	if( !pClump )
		return FALSE;
	RwTexture*		pTexture		=	NULL;

	INT	nCount	=	RpClumpGetNumAtomics( pClump );

	RpAtomic*	pAtomic		=	pClump->atomicList->next;
	RpGeometry*	pGeometry	=	RpAtomicGetGeometry( pAtomic );
	RpMaterial*	pMaterial	=	RpGeometryGetMaterial( pGeometry , 0 );

	if( !pMaterial )
		return FALSE;

	

	pTexture		=	m_pArrayNumTexture[ nNumber ];
	RpMaterialSetTexture( pMaterial , pTexture );

	RpGeometryForAllMaterials( pGeometry , CBMaterialSetTexture , pTexture );

	return TRUE;
}

RpMaterial *	CBossSpawnDlg::CBMaterialSetTexture( RpMaterial *material, PVOID data )
{
	RwTexture*	pTexture		=	(RwTexture*)data;

	RpMaterialSetTexture( material , pTexture );


	return material;
}


BOOL	CBossSpawnDlg::_DelClump( RpClump* pClump )
{
	g_pcsAgcmRender->RemoveClumpFromWorld( pClump );
	return TRUE;
}

VOID	CBossSpawnDlg::_ClearClumpList( VOID )
{
	PathClumpListIter	Iter	=	m_ClumpList.begin();
	for( ; Iter != m_ClumpList.end() ; ++Iter )
	{
		_DelClump( (*Iter) );
	}

	m_ClumpList.clear();
}

void CBossSpawnDlg::OnBnClickedButtonSave()
{
	if( !m_pSelectCharacter )
		return;

	STMonsterPathInfoPtr	pMonsterPath	=	m_MonsterPath.GetMonsterPath( m_pSelectCharacter->m_szID );

	if( m_MonsterPath.OneSavePath( MONSTER_PATH_DIRECTORY , pMonsterPath ) )
		AfxMessageBox( "SUCCESS" );
	
	else
		AfxMessageBox( "FAILED" );

}

void CBossSpawnDlg::OnBnClickedButtonReload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	if( m_MonsterPath.LoadPath( MONSTER_PATH_DIRECTORY ) )
		AfxMessageBox( "SUCCESS" );
	else
		AfxMessageBox( "FAILED" );


	UpdatePathListBox();
}

void CBossSpawnDlg::OnBnClickedButtonAllsave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( m_MonsterPath.SavePath( MONSTER_PATH_DIRECTORY ) )
		AfxMessageBox( "SUCCESS" );
	
	else
		AfxMessageBox( "FAILED" );
}

void CBossSpawnDlg::OnBnClickedRadioPathloop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	STMonsterPathInfoPtr	pMonsterPath		=	m_MonsterPath.GetMonsterPath( m_pSelectCharacter->m_szID );
	if( !pMonsterPath )
		return;

	pMonsterPath->eType		=	E_PATH_LOOP;	
	
}

void CBossSpawnDlg::OnBnClickedRadioPathreverse()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	STMonsterPathInfoPtr	pMonsterPath		=	m_MonsterPath.GetMonsterPath( m_pSelectCharacter->m_szID );
	if( !pMonsterPath )
		return;

	pMonsterPath->eType		=	E_PATH_REVERSE;
}

BOOL	CBossSpawnDlg::OnInitNumber( VOID )
{

	RwRaster*		pTextureRaster	=	NULL;
	RwImage *		pImage			=   RwImageCreate(TEXTURE_NUMBER_SIZE, TEXTURE_NUMBER_SIZE, 32);
	m_pNumberImage					=	RwImageRead( "DM_Number.png" );

	if ( !RwImageAllocatePixels( pImage ) )
	{
		RwImageDestroy( pImage );
		return FALSE;
	}


	// 텍스쳐를 미리 100개 만든다
	for( INT i = 0 ; i < PATH_TEXTURENUM_COUNT ; ++i )
	{
		// Raster를 생성후 생성된 Image의 픽셀을 Raster에 적용
		pTextureRaster		=	RwRasterCreate( TEXTURE_NUMBER_SIZE , TEXTURE_NUMBER_SIZE , 0 , rwRASTERTYPETEXTURE | rwRASTERFORMAT8888 );
		if( pTextureRaster )
		{
			RwRGBA  *rpDestin = (RwRGBA*) pImage->cpPixels;

			int tx, ty;
			for( ty=0; ty < TEXTURE_NUMBER_SIZE; ++ty )
			{
				for( tx=0; tx < TEXTURE_NUMBER_SIZE; ++tx )
				{
					DWORD	tcolor = 0xffff00ff;
					RwRGBASetFromPixel(rpDestin,tcolor,rwRASTERFORMAT8888 );

					++rpDestin;
				}
			}

			m_pArrayNumTexture[i]	=	RwTextureCreate( pTextureRaster );
			RwTextureSetFilterMode(m_pArrayNumTexture[i],rwFILTERNEAREST);
			
			if( m_pArrayNumTexture[i] )
				_InitTexture( pImage , i );

			RwRasterSetFromImage( pTextureRaster , pImage );
		}

	}

	RwImageDestroy( pImage );

	return TRUE;
}

BOOL	CBossSpawnDlg::_InitTexture( RwImage* pImageTarget , INT nNumber )
{
	if( !pImageTarget )
		return FALSE;

	RwRGBA		ClearColor;
	INT			nCount		=	0;
	INT			nFirst		=	0;
	INT			nLast		=	0;
	INT			nX			=	0;
	INT			nZ			=	0;
	
	nFirst		=	nNumber / 10;
	nLast		=	nNumber % 10;

	// 텍스쳐에 0이 마지막에 있어서 모든 숫자들이 하나씩 앞으로 당겨진다
	// 0은 맨 뒤에 있으니까 9
	if( nFirst	== 0 )
		nFirst	=	9;
	else
		nFirst	-=	1;

	if( nLast	== 0 )
		nLast	=	9;
	else
		nLast	-=	1;

	// 텍스쳐를 적용한다
	_NumberDraw( pImageTarget , nFirst , nLast );


	return TRUE;
}

VOID	CBossSpawnDlg::_NumberDraw( RwImage* pImageTarget , INT nFirst , INT nLast )
{
	if( !pImageTarget )
		return;

	if( !m_pNumberImage )
		return;

	INT			nTexHeight		=	m_pNumberImage->height;
	INT			nTexWidth		=	m_pNumberImage->width;

	INT			nTexHeightDest	=	pImageTarget->height;
	INT			nTexWidthDest	=	pImageTarget->width;

	RwRGBA*		cpPixelSrc		=	(RwRGBA*)m_pNumberImage->cpPixels;
	RwRGBA*		cpPixelDest		=	(RwRGBA*)pImageTarget->cpPixels;

	INT			nStartWidth		=	nFirst * TEXT_NUMBER_WIDTH;
	INT			nEndWidth		=	nFirst * TEXT_NUMBER_WIDTH + TEXT_NUMBER_WIDTH;

	// 첫번째 숫자를 드로잉 한다
	for( INT i = 0 ; i < nTexHeight  ; ++i )
	{
		for( INT j = 0 ; j < nTexWidth ; ++j )
		{
			UINT32	nPixel	=		*(UINT32*)&cpPixelSrc[ i*nTexWidth + j ];
			if( nStartWidth <= j && nEndWidth >= j )
			{
				if(	cpPixelSrc[ i*nTexWidth + j ].alpha > 0 )
				{
					RwRGBASetFromPixel( &cpPixelDest[ nTexWidthDest*(9+i) + (j-nStartWidth+1) ] , nPixel , rwRASTERFORMAT8888 );
				}
			}
		}
	}

	nStartWidth		=	nLast * TEXT_NUMBER_WIDTH;
	nEndWidth		=	nLast * TEXT_NUMBER_WIDTH + TEXT_NUMBER_WIDTH;

	// 두번째 숫자를 드로잉 한다
	for( INT i = 0 ; i < nTexHeight  ; ++i )
	{
		for( INT j = 0 ; j < nTexWidth ; ++j )
		{
			UINT32	nPixel	=		*(UINT32*)&cpPixelSrc[ i*nTexWidth + j ];
			if( nStartWidth <= j && nEndWidth >= j )
			{
				if(	cpPixelSrc[ i*nTexWidth + j ].alpha > 0 )
				{
					RwRGBASetFromPixel( &cpPixelDest[ nTexWidthDest*(9+i) + (j-nStartWidth+26) ] , nPixel , rwRASTERFORMAT8888 );
				}
			}
		}
	}

}

BOOL	CBossSpawnDlg::OnInitDialog( VOID )
{
	CDialog::OnInitDialog();

	OnInitNumber();

	return TRUE;
}
void CBossSpawnDlg::OnLbnSelchangeListSpawnlist()
{
	m_checkPathSet.SetCheck( FALSE );
}
