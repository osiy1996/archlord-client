// DungeonTemplateEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "MyEngine.h"
#include "MainFrm.h"
#include "ApUtil.h"
#include "DungeonTemplateEditDlg.h"
#include "DungeonWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMainFrame *		g_pMainFrame;
/////////////////////////////////////////////////////////////////////////////
// CDungeonTemplateEditDlg dialog

#define LAST_CONTROL	IDC_DOME1_2


CDungeonTemplateEditDlg::CDungeonTemplateEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDungeonTemplateEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDungeonTemplateEditDlg)
	m_strTemplateName = _T("");
	//}}AFX_DATA_INIT

	for( int i = 0 ; i < 12 ; i ++ )
		m_afHeight[ i ] = 0.0f;

	m_pTemplate	= NULL;
}


void CDungeonTemplateEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDungeonTemplateEditDlg)
	DDX_Control(pDX, IDC_OBJECTLIST, m_ctlTree);
	DDX_Text(pDX, IDC_TEMPLATE_NAME, m_strTemplateName);
	DDX_Text(pDX, IDC_HEIGHT_01, m_afHeight[ 0	] );
	DDX_Text(pDX, IDC_HEIGHT_02, m_afHeight[ 1	] );
	DDX_Text(pDX, IDC_HEIGHT_03, m_afHeight[ 2	] );
	DDX_Text(pDX, IDC_HEIGHT_04, m_afHeight[ 3	] );
	DDX_Text(pDX, IDC_HEIGHT_05, m_afHeight[ 4	] );
	DDX_Text(pDX, IDC_HEIGHT_06, m_afHeight[ 5	] );
	DDX_Text(pDX, IDC_HEIGHT_07, m_afHeight[ 6	] );
	DDX_Text(pDX, IDC_HEIGHT_08, m_afHeight[ 7	] );
	DDX_Text(pDX, IDC_HEIGHT_09, m_afHeight[ 8	] );
	DDX_Text(pDX, IDC_HEIGHT_10, m_afHeight[ 9	] );
	DDX_Text(pDX, IDC_HEIGHT_11, m_afHeight[ 10	] );
	DDX_Text(pDX, IDC_HEIGHT_12, m_afHeight[ 11	] );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDungeonTemplateEditDlg, CDialog)
	//{{AFX_MSG_MAP(CDungeonTemplateEditDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDungeonTemplateEditDlg message handlers

BOOL CDungeonTemplateEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if( !LoadCategory() )
	{
		MD_SetErrorMessage( "ī�װ����� �ε� ����" );
	}

	CopyTree( &g_pMainFrame->m_pTileList->m_pObjectWnd->m_wndTreeCtrl , TVI_ROOT , 0 );

	UpdateControls();

	m_strTemplateName = m_pTemplate->strName;

	for( int i = 0 ; i < 12 ; i ++ )
	{
		m_afHeight[ i ] = m_pTemplate->afSampleHeight[ i ] / 100.0f;
	}

	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDungeonTemplateEditDlg::CopyTree( CTreeCtrl * pTree , HTREEITEM root, int level)
{
	HTREEITEM child;
	stObjectEntry *pObject;

	if( root != TVI_ROOT )
	{
		pObject = (stObjectEntry *) pTree->GetItemData( root );

		ApdObjectTemplate *			pstApdObjectTemplate	= g_pcsApmObject->GetObjectTemplate(pObject->tid);

		if( pstApdObjectTemplate )
		{
			AgcdObjectTemplate *		pstAgcdObjectTemplate	= g_pcsAgcmObject->GetTemplateData(pstApdObjectTemplate);

			HTREEITEM category;
			category = SearchItemText( pstAgcdObjectTemplate->m_szCategory );
			if( !category )
				category = TVI_ROOT;
			
			HTREEITEM item;
			item = m_ctlTree.InsertItem( pObject->name , category);
			m_ctlTree.SetItemData(item, (DWORD) pObject);
		}
	}

	for( child = pTree->GetChildItem( root ) ; child ; child = pTree->GetNextSiblingItem( child ) )
	{
		CopyTree( pTree , child, level + 1 );
	}

	return TRUE;
}

HTREEITEM CDungeonTemplateEditDlg::SearchItemText(const char *strName, HTREEITEM root)
{
	CCategory *pstCategory;
	for( int i = 0 ; i < ( int ) m_listCategory.size(); i ++ )
	{
		pstCategory = &m_listCategory[ i ];
		if( !strcmp( pstCategory->str , strName ) )
		{
			return pstCategory->pos;
		}
	}

	/*
	ASSERT( NULL != strName		);

	HTREEITEM child	;
	HTREEITEM item	;

	if( root != TVI_ROOT && !strcmp( strName, m_ctlTree.GetItemText(root) ) )
		return root;

	for( child = m_ctlTree.GetChildItem( root ) ; child ; child = m_ctlTree.GetNextSiblingItem( child ) )
	{
		item = SearchItemText( strName, child );
		if( item )
			return item;
	}
	*/

	return NULL;
}

BOOL	CDungeonTemplateEditDlg::LoadCategory()
{
	char	filename[ 1024 ];
	wsprintf( filename , "%s\\%s" , ALEF_CURRENT_DIRECTORY , OBJECT_FILE );
	FILE	*pFile = fopen( filename , "rt" );

	ASSERT( NULL != pFile && "������Ʈ ��ũ��Ʈ ���� ���� ����" );

	if( pFile == NULL )
	{
		// ���Ͽ��� ����.
		return FALSE;
	}

	CGetArg2		arg;

	char			strBuffer[	1024	];
	//unsigned int	nRead = 0;

	int				count = 0;
	int				level;
	HTREEITEM		current[10];
	CCategory		stCategory;

	current[0] = NULL;

	while( fgets( strBuffer , 1024 , pFile ) )
	{
		// �о�����..

		arg.SetParam( strBuffer , "|\n" );

		if( arg.GetArgCount() < 2 )
		{
			// ���� �̻�
			continue;
		}

		level				= atoi( arg.GetParam( 0 ) )	;
		if( level >= 10 )
		{
			continue;
		}

		{
			unsigned long tid = 0;
			const char *name = arg.GetParam( 1 );
			const char *file = arg.GetArgCount() > 2 ? arg.GetParam( 2 ):NULL;
			RpClump *pClump = NULL;
			HTREEITEM entry = level ? current[level - 1]:TVI_ROOT;
			RwRGBA *pstColor = NULL;

			stObjectEntry *pObject = new stObjectEntry;
			char strName[256];
			HTREEITEM item;

			ASSERT( NULL != pObject );

			pObject->tid = tid;

			strncpy( pObject->name, name, 256 );
			pObject->name[255] = 0;

			strcpy(strName, pObject->name);

			pObject->file[0] = 0;
			//pObject->pClump = pClump;
			if (pstColor)
				pObject->stPreLitLum = *pstColor;

			if( file )
			{
				strncpy( pObject->file, file, 256 );
				pObject->file[255] = 0;

				strcat(strName, " ( ");
				strcat(strName, pObject->file);
				strcat(strName, " )");
			}

			item = m_ctlTree.InsertItem(strName, entry);
			m_ctlTree.SetItemData(item, (DWORD) pObject);

			current[ level ] = item;

			// ������ (2005-04-11 ���� 4:31:36) : ī�װ� �����ص�..
			stCategory.str	= strName;
			stCategory.pos	= current[ level ];
			m_listCategory.push_back( stCategory );

			m_listDelete.AddTail( ( void * ) pObject );
		}

		count ++;
	}

	fclose( pFile );

	return TRUE;
}

// â����.. ���ø��� ���ڷ� ����..
int		CDungeonTemplateEditDlg::DoModal( CDungeonTemplate * pTemplate )
{
	ASSERT( NULL != pTemplate );

	if( NULL == pTemplate )
		return IDCANCEL;

	// ���ø� ������ ����
	this->m_pTemplate = pTemplate;

	return CDialog::DoModal();
}

void CDungeonTemplateEditDlg::OnOK() 
{
	UpdateData( TRUE );
	m_pTemplate->strName	= m_strTemplateName;

	// ���� ����..
	float	afHeightTmp[ 12 ];
	float	fTmp;
	int i , j , nIndex ;

	for( i = 0 ; i < 12 ; i ++ )
	{
		afHeightTmp[ i ] = m_afHeight[ i ];
	}

	for( i = 0 ; i < 12 ; i ++ )
	{
		fTmp	= afHeightTmp[ 0 ]	; 
		nIndex	= i					;

		for( j = 1 ; j < 12 - i ; j ++ )
		{
			if( fTmp < afHeightTmp[ j ] )
			{
				fTmp	= afHeightTmp[ j ] ;
				nIndex	= j;
			}
		}
	
		m_afHeight[ i ] = fTmp;
		afHeightTmp[ nIndex ] = afHeightTmp[ 12 - i - 1 ];
	}

	for( i = 0 ; i < 12 ; i ++ )
	{
		m_pTemplate->afSampleHeight[ i ] = m_afHeight[ i ] * 100.0f;
	}
	
	CDialog::OnOK();
}

LRESULT CDungeonTemplateEditDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	int	nID = LOWORD(wParam);

	if( WM_COMMAND == message			&&
		BN_CLICKED == HIWORD(wParam)	&&
		LOWORD(wParam) >= IDC_FLOOR_0	&&
		LAST_CONTROL >= LOWORD(wParam)	)
	{
		// ��ư ����������..

		// ��� Ÿ������ ��..
		int nType = ( nID - IDC_FLOOR_0 ) / MAX_DUNGEON_INDEX_COUNT;

		ASSERT( nType < TBM_SELECT );
		
		// ���õ� ��Ʈ�� Ȯ��..
		
		stObjectEntry	*pObject;
		ApdObjectTemplate	*	pstApdObjectTemplate	;
		HTREEITEM		item;

		item = m_ctlTree.GetSelectedItem();
		if( !item )
		{
			// ���õ� �������� ������ �ش� ������ ����..
			int i;
			for( i = 0 ; i < MAX_DUNGEON_INDEX_COUNT ; ++ i )
			{
				if( this->m_pTemplate->uTemplateIndex[ nType ][ i ].uIndex == 0 )
				{
					break;
				}
			}

			if( i > 0 )
				this->m_pTemplate->uTemplateIndex[ nType ][ i - 1 ].uIndex = 0;
		}
		else
		{
			// �߰� �� �Ǵµ�...

			pObject = (stObjectEntry *) m_ctlTree.GetItemData( item );
			ASSERT( NULL != pObject );
			pstApdObjectTemplate = g_pcsApmObject->GetObjectTemplate(pObject->tid);

			if( NULL == pstApdObjectTemplate )
			{
				MessageBox( "�ش� �����ۿ� ���ø� ������ �����" );
			}
			else
			{
				// �ش� Ÿ�Կ� ��ġ�°� �ִ��� �˻�..

				BOOL	bJungBok = FALSE;
				int		i;

				for( i = 0 ; i < MAX_DUNGEON_INDEX_COUNT ; ++ i )
				{
					if( pstApdObjectTemplate->m_lID == this->m_pTemplate->uTemplateIndex[ nType ][ i ].uIndex )
					{
						bJungBok = TRUE;
						break;
					}
				}

				if( !bJungBok )
				{
					// �ߺ��� �ƴѰ�쿡�� �߰���..
					
					// ��ĭ�� �ִ� ��� �ű⿡ ����..
					for( i = 0 ; i < MAX_DUNGEON_INDEX_COUNT ; ++ i )
					{
						if( this->m_pTemplate->uTemplateIndex[ nType ][ i ].uIndex == 0 )
						{
							// ����!
							this->m_pTemplate->uTemplateIndex[ nType ][ i ].uIndex = pstApdObjectTemplate->m_lID;
							break;
						}
					}

					if( i == MAX_DUNGEON_INDEX_COUNT )
					{
						// �ֱ� ����..
						MessageBox( "�ε����� 4������ ����." );
					}
				}
			}
		}

		// ��Ʈ�� ������Ʈ..
		UpdateControls();

		m_ctlTree.SelectItem( NULL );
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}

void	CDungeonTemplateEditDlg::UpdateControls()
{
	CONTROL_UPDATE( IDC_FLOOR_0 );
	CONTROL_UPDATE( IDC_FLOOR_1 );
	CONTROL_UPDATE( IDC_FLOOR_2 );
	CONTROL_UPDATE( IDC_SLOPE_0 );
	CONTROL_UPDATE( IDC_SLOPE_1 );
	CONTROL_UPDATE( IDC_SLOPE_2 );
	CONTROL_UPDATE( IDC_STAIR_0 );
	CONTROL_UPDATE( IDC_STAIR_1 );
	CONTROL_UPDATE( IDC_STAIR_2 );
	CONTROL_UPDATE( IDC_WALL2_0	);
	CONTROL_UPDATE( IDC_WALL2_1	);
	CONTROL_UPDATE( IDC_WALL2_2	);
	CONTROL_UPDATE( IDC_WALL4_0	);
	CONTROL_UPDATE( IDC_WALL4_1	);
	CONTROL_UPDATE( IDC_WALL4_2 );
	CONTROL_UPDATE( IDC_FENCE_0	);
	CONTROL_UPDATE( IDC_FENCE_1 );
	CONTROL_UPDATE( IDC_FENCE_2 );
	CONTROL_UPDATE( IDC_PILLAR_0);
	CONTROL_UPDATE( IDC_PILLAR_1);
	CONTROL_UPDATE( IDC_PILLAR_2);

	CONTROL_UPDATE( IDC_DOME4_0	);
	CONTROL_UPDATE( IDC_DOME4_1	);
	CONTROL_UPDATE( IDC_DOME4_2	);

	CONTROL_UPDATE( IDC_DOME1_0	);
	CONTROL_UPDATE( IDC_DOME1_1	);
	CONTROL_UPDATE( IDC_DOME1_2	);

}

void	CDungeonTemplateEditDlg::CONTROL_UPDATE( UINT32 uID )
{
	CButton	* pButton;
	CString	str;
	int		nTemplate;
	ApdObjectTemplate	*	pstApdObjectTemplate	;

	pButton		= ( CButton * ) GetDlgItem( uID );
	nTemplate	= this->m_pTemplate->uTemplateIndex[ ( uID - IDC_FLOOR_0 ) / MAX_DUNGEON_INDEX_COUNT ][ ( uID - IDC_FLOOR_0 ) % MAX_DUNGEON_INDEX_COUNT ].uIndex;
	pstApdObjectTemplate = g_pcsApmObject->GetObjectTemplate( nTemplate );
	if( nTemplate == 0 ) str = "���ڸ�";
	else
		if( NULL == pstApdObjectTemplate )
		{
			MessageBox( "���ø��� ������ �־��." );
			str = "����!";
		}
		else
		{
			str = pstApdObjectTemplate->m_szName;
		}
	pButton->SetWindowText( str );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDungeonTemplateSelectDlg dialog


CDungeonTemplateSelectDlg::CDungeonTemplateSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDungeonTemplateSelectDlg::IDD, pParent),
	m_bListUpdated( FALSE ),
	m_nTemplateID( 0 )
{
	//{{AFX_DATA_INIT(CDungeonTemplateSelectDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pDungeonWnd	= g_pMainFrame->m_pDungeonWnd;
}


void CDungeonTemplateSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDungeonTemplateSelectDlg)
	DDX_Control(pDX, IDC_DUNGEONTEMPLATELIST, m_ctlTemplateList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDungeonTemplateSelectDlg, CDialog)
	//{{AFX_MSG_MAP(CDungeonTemplateSelectDlg)
	ON_BN_CLICKED(IDC_EDITTEMPLATE, OnEdittemplate)
	ON_BN_CLICKED(IDC_ADDTEMPLATE, OnAddtemplate)
	ON_BN_CLICKED(IDC_DELETE_TEMPLATE, OnDeleteTemplate)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDungeonTemplateSelectDlg message handlers

void CDungeonTemplateSelectDlg::OnEdittemplate() 
{
	int	nIndex = m_ctlTemplateList.GetCurSel();
	if( LB_ERR == nIndex )
	{
		MessageBox( "�ƹ��͵� ���õȰ� ������.." );
		return;
	}

	CDungeonTemplateEditDlg dlg;
	CDungeonTemplate	* pTemplate;

	pTemplate = ( CDungeonTemplate * ) m_ctlTemplateList.GetItemDataPtr( nIndex );

	if( IDOK == dlg.DoModal( pTemplate ) )
	{
		//////////////////////////////////////////////////////////////////////////
		// ���� ó���Ұ� ����..
		// �ȿ��� ������ ����Ÿ�� �ٲٱ� ������..
		// ����Ʈ ��ȭ �÷��׸� üũ�صд�.

		SetListUpdate();
	}
}

void CDungeonTemplateSelectDlg::OnAddtemplate() 
{
	// ���ø� �߰�..
	CDungeonTemplate	* pTemplate;
	CDungeonTemplateEditDlg dlg;

	// �� ���ø� �ε��� ���..
	int	nLastIndex = 0;
	int nCount = m_ctlTemplateList.GetCount();
	for( int i = 0 ; i < nCount ; ++ i )
	{
		pTemplate = ( CDungeonTemplate * ) m_ctlTemplateList.GetItemDataPtr( i );
		ASSERT( NULL != pTemplate );

		if( ( INT32 ) pTemplate->uTID > nLastIndex )
		{
			nLastIndex = pTemplate->uTID;
		}
	}	

	pTemplate		= new CDungeonTemplate;
	pTemplate->uTID	= nLastIndex + 1 ;	// ������ �ε����� 1�� ���ؼ� �־ �ߺ�����..

	if( IDOK == dlg.DoModal( pTemplate ) )
	{
		// �߰���..
		int nIndex = AddTemplate( pTemplate );
		ASSERT( nIndex != LB_ERR );

		m_ctlTemplateList.SetCurSel( nIndex );
		SetListUpdate();
	}
	else
	{
		// ��� �����ϱ�.. �׳� ��������..
		delete pTemplate;
	}
}

int	CDungeonTemplateSelectDlg::AddTemplate( CDungeonTemplate * pTemplate )
{
	ASSERT( NULL != pTemplate );
	if( NULL == pTemplate ) return LB_ERR;

	// ���� ����Ʈ �ڽ��� �����..

	int nIndex;
	nIndex = m_ctlTemplateList.AddString( pTemplate->strName );

	if( nIndex == LB_ERR )
	{
		MessageBox( "�޽����ڽ� ���� ����" );
		return LB_ERR;
	}

	VERIFY( m_ctlTemplateList.SetItemDataPtr( nIndex , pTemplate ) );
	return nIndex;
}

void CDungeonTemplateSelectDlg::OnDeleteTemplate() 
{
	int	nIndex = m_ctlTemplateList.GetCurSel();
	if( LB_ERR == nIndex )
	{
		MessageBox( "�ƹ��͵� ���õȰ� ������.." );
		return;
	}

	// ���õ� ���ø��� ������ ..
	CDungeonTemplate	* pTemplate;
	pTemplate = ( CDungeonTemplate * ) m_ctlTemplateList.GetItemDataPtr( nIndex );

	if( IDYES == MessageBox( "���� ����ſ���?" , "���ø� ���� Ȯ��.." , MB_YESNO ) )
	{
		delete pTemplate;
		m_ctlTemplateList.DeleteString( nIndex );

		// ���� �۾�..
		SetListUpdate();
	}
	
}

BOOL CDungeonTemplateSelectDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	ASSERT( NULL != m_pDungeonWnd );
	if( NULL == m_pDungeonWnd )
		return FALSE;

	AuList< CDungeonTemplate > * pList = &m_pDungeonWnd->m_listTemplate	;
	AuNode< CDungeonTemplate > * pNode = pList->GetHeadNode()			;
	CDungeonTemplate * pTemplate;
	CDungeonTemplate * pTemplateCopy;
	INT32	nIndex;

	while( pNode )
	{
		pTemplate	= &pNode->GetData();

		pTemplateCopy = new CDungeonTemplate;

		// ����..
		pTemplateCopy->copy( *pTemplate );

		// ���ø��� ����Ʈ�� �߰���.

		nIndex = m_ctlTemplateList.AddString( pTemplateCopy->strName );
		m_ctlTemplateList.SetItemDataPtr( nIndex , ( void * ) pTemplateCopy );

		pNode = pNode->GetNextNode();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDungeonTemplateSelectDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	// ������ (2004-10-07 ���� 12:58:26) : 
	// �޸� ���� �۾�..

	int nCount = m_ctlTemplateList.GetCount();
	for( int i = 0 ; i < nCount ; ++ i )
	{
		delete m_ctlTemplateList.GetItemDataPtr( i );
	}	
}

void CDungeonTemplateSelectDlg::OnOK() 
{
	// ���� �ȳ༮�� �ִ��� Ȯ���Ѵ�..
	int	nIndex = m_ctlTemplateList.GetCurSel();
	if( LB_ERR == nIndex )
	{
		MessageBox( "�ƹ��͵� ���õȰ� ������.." );
		return;
	}

	// ���õ� ���ø��� ������ ..
	CDungeonTemplate	* pTemplate;
	pTemplate = ( CDungeonTemplate * ) m_ctlTemplateList.GetItemDataPtr( nIndex );
	m_nTemplateID	= pTemplate->uTID;

	if( GetListUpdated() )
	{
		// ����Ʈ ������Ʈ..

		ASSERT( NULL != g_pMainFrame->m_pDungeonWnd );

		g_pMainFrame->m_pDungeonWnd->m_listTemplate.RemoveAll();

		int nCount = m_ctlTemplateList.GetCount();
		for( int i = 0 ; i < nCount ; ++ i )
		{
			g_pMainFrame->m_pDungeonWnd->m_listTemplate.AddTail( 
				* ( ( CDungeonTemplate * ) m_ctlTemplateList.GetItemDataPtr( i ) ) );
		}
	}
	
	CDialog::OnOK();
}

void CDungeonTemplateEditDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// ������ (2004-10-08 ���� 11:30:31) : 
	// �޸� Ŭ����..

	AuNode< void * > * pNode = m_listDelete.GetHeadNode();
	while( pNode )
	{
		delete pNode->GetData();

		pNode = pNode->GetNextNode();
	}
}
