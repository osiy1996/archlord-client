// MaterialEditDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ModelTool.h"
#include "MaterialEditDlg.h"


// CMaterialEditDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMaterialEditDlg, CDialog)

CMaterialEditDlg::CMaterialEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMaterialEditDlg::IDD, pParent)
{

}

CMaterialEditDlg::~CMaterialEditDlg()
{
}

void CMaterialEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, m_SliderCtrl[0]);
	DDX_Control(pDX, IDC_SLIDER2, m_SliderCtrl[1]);
	DDX_Control(pDX, IDC_SLIDER3, m_SliderCtrl[2]);
	DDX_Control(pDX, IDC_SLIDER4, m_SliderCtrl[3]);
	DDX_Control(pDX, IDC_SLIDER5, m_SliderCtrl[4]);
	DDX_Control(pDX, IDC_SLIDER6, m_SliderCtrl[5]);
	DDX_Control(pDX, IDC_SLIDER7, m_SliderCtrl[6]);
	DDX_Control(pDX, IDC_SLIDER8, m_SliderCtrl[7]);
	DDX_Control(pDX, IDC_STATIC_TEXT1, m_StaticText[0]);
	DDX_Control(pDX, IDC_STATIC_TEXT2, m_StaticText[1]);
	DDX_Control(pDX, IDC_STATIC_TEXT3, m_StaticText[2]);
	DDX_Control(pDX, IDC_STATIC_TEXT4, m_StaticText[3]);
	DDX_Control(pDX, IDC_STATIC_TEXT5, m_StaticText[4]);
	DDX_Control(pDX, IDC_STATIC_TEXT6, m_StaticText[5]);
	DDX_Control(pDX, IDC_STATIC_TEXT7, m_StaticText[6]);
	DDX_Control(pDX, IDC_STATIC_TEXT8, m_StaticText[7]);
	DDX_Control(pDX, IDC_LIST_APPLY_FXNAME, m_ListBoxMaterial);
	DDX_Control(pDX, IDC_LIST_ATOMIC_NAME, m_listAtomicName);
	DDX_Control(pDX, IDC_LIST_MATERIAL_NAME, m_listMaterialName);
	DDX_Control(pDX, IDC_STATIC_NAME, m_staticClumpName);
	DDX_Control(pDX, IDC_COMBO_FX, m_comboFX);
}


BEGIN_MESSAGE_MAP(CMaterialEditDlg, CDialog)
	ON_BN_CLICKED(ID_ADD, &CMaterialEditDlg::OnBnClickedAdd)
	ON_BN_CLICKED(ID_DELETE, &CMaterialEditDlg::OnBnClickedDelete)
	ON_LBN_SELCHANGE(IDC_LIST_ATOMIC_NAME, &CMaterialEditDlg::OnLbnSelchangeListAtomicName)
	ON_LBN_SELCHANGE(IDC_LIST_MATERIAL_NAME, &CMaterialEditDlg::OnLbnSelchangeListMaterialName)
END_MESSAGE_MAP()


// CMaterialEditDlg 메시지 처리기입니다.

BOOL CMaterialEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HANDLE				hHandle;
	WIN32_FIND_DATA		csFindData;
	CHAR				strSearchFolder[ MAX_PATH ];
	INT					nCount			=	0;

	sprintf_s( strSearchFolder, MAX_PATH , "EFFECT\\*.*" );
	hHandle = FindFirstFile( strSearchFolder, &csFindData );

	while ( 1 )
	{
		if( FindNextFile( hHandle, &csFindData ) == FALSE )
			break;

		//폴더인가?
		if( csFindData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL )
		{
			csFindData.cFileName;
			m_comboFX.InsertString( nCount++ , csFindData.cFileName );
		}
	}

	AllDisableSliderWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL	CMaterialEditDlg::SetSliderWindow( INT nIndex , CONST CString& strSliderName , INT nMin , INT nMax , BOOL bShowWindow )
{
	if( nIndex < 0 || nIndex >= MAX_SLIDER_COUNT )
		return FALSE;

	m_StaticText[nIndex].SetWindowText( strSliderName );

	m_SliderCtrl[nIndex].SetRangeMin( nMin );
	m_SliderCtrl[nIndex].SetRangeMax( nMax );

	if( bShowWindow )
	{
		m_StaticText[nIndex].ShowWindow( SW_SHOW );
		m_SliderCtrl[nIndex].ShowWindow( SW_SHOW );
	}

	return TRUE;
}


VOID	CMaterialEditDlg::DisableSliderWindow( INT nIndex )
{
	if( nIndex < 0 || nIndex >= MAX_SLIDER_COUNT )
		return;

	m_StaticText[nIndex].ShowWindow( SW_HIDE );
	m_SliderCtrl[nIndex].ShowWindow( SW_HIDE );

}

VOID	CMaterialEditDlg::AllDisableSliderWindow( VOID )
{
	for( INT i = 0 ; i < MAX_SLIDER_COUNT  ; ++i )
	{
		m_StaticText[i].ShowWindow( SW_HIDE );
		m_SliderCtrl[i].ShowWindow( SW_HIDE );
	}

}

void CMaterialEditDlg::OnOK()
{
	// Material 정보 저장
	// rws를 다시 저장한다


	CDialog::OnOK();
}

void CMaterialEditDlg::OnCancel()
{
	// Material 정보를 전으로 돌린다
	// rws 정보를 전으로 돌려놓는다


	CDialog::OnCancel();
}

INT	CMaterialEditDlg::GetEnableSliderCount( VOID )
{
	INT nTotalCount	=	0;
	

	for( INT i = 0 ; i < MAX_SLIDER_COUNT ; ++i )
	{
		if( m_SliderCtrl[i].IsWindowVisible() )
			++nTotalCount;
	}

	return nTotalCount;
}

BOOL CMaterialEditDlg::SetClump( RpClump* pClump )
{
	m_listAtomicName.ResetContent();

	if( !pClump )
		return FALSE;

	m_pClump	=	pClump;

	m_staticClumpName.SetWindowText( pClump->szName );

	RpAtomic*	parrAtomic[ 50 ];
	ZeroMemory( parrAtomic , sizeof(parrAtomic) );

	RpClumpForAllAtomics( pClump , CBAllAtomic , parrAtomic );
	
	RpAtomic*	pAtomic	=	pClump->atomicList;

	for( INT k = 0 ; parrAtomic[k] ; ++k )
	{
		if( !pAtomic->szName )
		{
			CString	str;
			str.Format( "Atomic - %d" , k );
			m_listAtomicName.InsertString( k , str );
		}

		RpGeometry*		pGeometry	=	RpAtomicGetGeometry( parrAtomic[k] );
		RpMaterial**	ppMaterial	=	pGeometry->matList.materials;
		for( INT i = 0 ; i < pGeometry->matList.numMaterials ; ++i )
		{
			DxEffect*	pEffect		=	RpMaterialD3DFxGetEffect( ppMaterial[i] );
			if( !pEffect )
				continue;
			
			if( pEffect->pSharedD3dXEffect->name )
			{

			}	
		}
	}

	return TRUE;
}

RpAtomic*	CMaterialEditDlg::CBAllAtomic( RpAtomic* pAtomic , PVOID pData )
{
	if( !pAtomic )
		return NULL;

	RpAtomic**	ppAtomic		=	static_cast<RpAtomic**>(pData);

	for( INT i = 0 ;  ; ++i )
	{
		if( !ppAtomic[i] )
		{
			ppAtomic[i]	=	pAtomic;
			break;
		}
	}
	
	return pAtomic;	
}

RpMaterial*	CMaterialEditDlg::CBAllMaterial( RpMaterial* pMaterial , PVOID pData )
{
	if( !pMaterial )
		return NULL;

	DxEffect*	pEffect	=	RpMaterialD3DFxGetEffect( pMaterial );
	if( pEffect )
	{
		pEffect->pSharedD3dXEffect->name;
	}
	
	return pMaterial;
}

void CMaterialEditDlg::OnBnClickedAdd()
{
	INT			nCurSel		=	m_comboFX.GetCurSel();
	CString		strData;

	if( nCurSel == -1 )
		return;

	// combo박스에서 선택된 FX String을 구해온다
	m_comboFX.GetLBText( nCurSel , strData );


	nCurSel	=	m_listAtomicName.GetCurSel();

	if( nCurSel == -1 )
		return;

	RpAtomic*	pAtomic		=	GetIndexAtomic( nCurSel );
	if( !pAtomic )
		return;


}

void CMaterialEditDlg::OnBnClickedDelete()
{
	INT	nCurSel	=	m_listAtomicName.GetCurSel();

	if( nCurSel == -1 )
		return;

	RpAtomic*	pAtomic		=	GetIndexAtomic( nCurSel );
	if( !pAtomic )
		return;

	//RpAtomicFxDisable( pAtomic );


	CModelToolApp::GetInstance()->ReleaseAtomicFX( pAtomic );

}

void CMaterialEditDlg::OnLbnSelchangeListAtomicName()
{
	m_listMaterialName.ResetContent();

	// Atomic 선택 변경
	INT	nCurSel	=	m_listAtomicName.GetCurSel();

	if( nCurSel == -1 )
		return;

	RpAtomic*	pAtomic		=	GetIndexAtomic( nCurSel );
	if( !pAtomic )
		return;

	RpGeometry*		pGeometry	=	RpAtomicGetGeometry( pAtomic );
	RpMaterial**	ppMaterial	=	pGeometry->matList.materials;
	CString			strMatName;

	for( INT i = 0 ; i < pGeometry->matList.numMaterials ; ++i )
	{
		strMatName.Format( "Material - %d" , i );
		m_listMaterialName.InsertString( i , strMatName );
	}

}
void CMaterialEditDlg::OnLbnSelchangeListMaterialName()
{
	m_ListBoxMaterial.ResetContent();

	INT		nCurSel	=	m_listMaterialName.GetCurSel();

	if( nCurSel == -1 )
		return;

	RpMaterial*	pMaterial	=	GetIndexMaterial( nCurSel );
	if( !pMaterial )
		return;

	DxEffect*		pEffect		=	RpMaterialD3DFxGetEffect( pMaterial );
	if( pEffect )
	{
		m_ListBoxMaterial.InsertString( 0 , pEffect->pSharedD3dXEffect->name );
	}
	else
	{
		CString		strTemp( "Not" );
		m_ListBoxMaterial.InsertString( 0 , strTemp );
	}
}

RpAtomic*	CMaterialEditDlg::GetIndexAtomic( INT nIndex )
{
	RpAtomic*	pAtomic	=	m_pClump->atomicList;
	for( INT k = 0 ; pAtomic ; pAtomic	=	pAtomic->next , ++k )
	{
		if( k == nIndex )
			return pAtomic;
	}

	return NULL;
}

RpMaterial*	CMaterialEditDlg::GetIndexMaterial( INT nindex )
{

	INT	nCurSel	=	m_listAtomicName.GetCurSel();

	if( nCurSel == -1 )
		return NULL;

	RpAtomic*	pAtomic		=	GetIndexAtomic( nCurSel );
	if( !pAtomic )
		return NULL;

	RpGeometry*		pGeometry	=	RpAtomicGetGeometry( pAtomic );
	RpMaterial**	ppMaterial	=	pGeometry->matList.materials;
	for( INT i = 0 ; i < pGeometry->matList.numMaterials ; ++i )
	{
		if( nindex == i )
			return ppMaterial[i];
	}
}