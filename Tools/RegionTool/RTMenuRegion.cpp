// RTMenuRegion.cpp : implementation file
//

#include "stdafx.h"
#include "regiontool.h"
#include "RegionToolDlg.h"
#include "RegionMenuDlg.h"
#include "RTMenuRegion.h"

#include "TemplateEditDlg.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString	g_strLastLockOwnerName;

static const char g_strINIRegionTemplateSection		[]	= "%d"			;
static const char g_strINIRegionTemplateKeyName		[]	= "Name"		;
static const char g_strINIRegionTemplateKeyPriority	[]	= "Priority"	;
static const char g_strINIRegionTemplateKeyType		[]	= "Type"		;
static const char g_strINIRegionTemplateKeyComment	[]	= "Comment"		;

static const char g_strINIRegionElementSection		[]	= "%d"			;
static const char g_strINIRegionElementIndex		[]	= "Index"		;
static const char g_strINIRegionElementKeyKind		[]	= "Kind"		;
static const char g_strINIRegionElementKeyStartX	[]	= "StartX"		;
static const char g_strINIRegionElementKeyStartZ	[]	= "StartZ"		;
static const char g_strINIRegionElementKeyEndX		[]	= "EndX"		;
static const char g_strINIRegionElementKeyEndZ		[]	= "EndZ"		;

/////////////////////////////////////////////////////////////////////////////
// CRTMenuRegion

char *	CRTMenuRegion::GetMenuName()
{
	static char _strName[] = "Region";
	return ( char * ) _strName;
}


CRTMenuRegion::CRTMenuRegion()
{
	m_nCurrentTemplate	= REGION_NO_TEMPLATE	;
	m_pSelectedElement	= NULL	;
	m_nRangeDragMode	= MODE_NONE	;

	m_bChangedRegion	= FALSE;
	m_bChangedTemplate	= FALSE;
}

CRTMenuRegion::~CRTMenuRegion()
{
}

BOOL CRTMenuRegion::CBProgress( char * pStr , int nPos , int nMax , void * pData )
{
	CProgressDlg	*pDlg = ( CProgressDlg * ) pData;

	pDlg->SetProgress	( nPos );
	pDlg->SetTarget		( nMax );

	return TRUE;
}

BOOL CRTMenuRegion::OnLoadData		()
{
	g_pcsApmMap->LoadTemplate( "Ini\\" REGIONTEMPLATEFILE );

	UpdateList();

	// ������ (2005-06-07 ���� 5:19:53) : 
	// �ڷ���Ʈ �����̸� ����Ʈ ����Ÿ�� �ε���.
	if( g_bTeleportOnlyMode )
	{
		// ����Ʈ ����Ÿ �ε�.
		g_pcsApmMap->SetLoadingMode( TRUE , FALSE );
		g_pcsApmMap->Init();	// ����Ÿ �ʱ�ȭ.

		CProgressDlg	dlg;
		dlg.StartProgress( "����Ʈ ����Ÿ �ε���" , 100 , this );
		g_pcsApmMap->LoadAll( TRUE , FALSE , CBProgress , this );
		dlg.EndProgress();
	}
	else
	{
		// ���� ���� �ε�..
		Load( "RegionTool\\" REGIONFILE );
	}

	m_bChangedTemplate	= FALSE;

	// ����� ����
	g_pcsApmMap->LoadWorldMap( "Ini\\" WORLDMAPTEMPLATE );

	// �ɰ��� ���� �б�
	if( g_pcsAgpmItem && !g_pcsAgpmItem->StreamReadTemplates( "INI\\ItemTemplateEntry.ini", NULL, FALSE ) )
	{
		::MessageBox( NULL , "������ ���ø� �б⿡ �����߽��ϴ�. ���ø��� ����� �ε���� �ʾ��� �� �ֽ��ϴ�." , "����" , MB_ICONERROR | MB_OK );
	}

	UpdateList();

	return TRUE;
}

BOOL CRTMenuRegion::OnSaveData		()
{
	if( m_bChangedTemplate )
	{
		g_pcsApmMap->SaveTemplate( "Ini\\" REGIONTEMPLATEFILE );
		m_bChangedTemplate = FALSE;
	}

	Save( "RegionTool\\" REGIONFILE );

	return TRUE;
}

BOOL	CRTMenuRegion::Create( CRect * pRect , CWnd * pParent )
{
	return CWnd::Create( NULL , NULL , WS_CHILD | WS_VISIBLE , *pRect , pParent , 1346 );
}


BEGIN_MESSAGE_MAP(CRTMenuRegion, CWnd)
	//{{AFX_MSG_MAP(CRTMenuRegion)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRTMenuRegion message handlers

void CRTMenuRegion::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );
		
	// Do not call CWnd::OnPaint() for painting messages
}

int CRTMenuRegion::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect	rect;
	GetClientRect( rect );

	CRect	rectList , rectButton;
	const int nButtonHeight = 20;

	rectList = rect;
	rectList.bottom -= ( nButtonHeight + 3 ) * 4 ;

	m_ctlTemplate.Create( WS_CHILD | WS_VISIBLE | LBS_SORT | WS_VSCROLL , rectList , this , IDC_MENUREGION_LIST );

	rectButton = rect;

	rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 0;
	rectButton.bottom	= rectButton.top + nButtonHeight;
	m_wndButtonEdit.Create( "Edit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rectButton , this,IDC_MENUREGION_BUTTON_EDIT);
	
	rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 1;
	rectButton.bottom	= rectButton.top + nButtonHeight;
	m_wndButtonAdd.Create( "Add", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rectButton , this,IDC_MENUREGION_BUTTON_ADD);
	
	rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 2;
	rectButton.bottom	= rectButton.top + nButtonHeight;
	m_wndButtonRemove.Create( "Remove", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rectButton , this,IDC_MENUREGION_BUTTON_REMOVE);
	
	rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 3;
	rectButton.bottom	= rectButton.top + nButtonHeight;
	m_wndButtonDelete.Create( "Delete Region", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON  , rectButton , this,IDC_MENUREGION_BUTTON_DELETE);
	
	// ����Ÿ����..

	UpdateList();
	
	return 0;
}

LRESULT CRTMenuRegion::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	#define	CW_CLICKED_BUTTON( uID )	if( uID == LOWORD( wParam ) && BN_CLICKED == HIWORD( wParam ) )

	if( WM_COMMAND == message )
	{
		CW_CLICKED_BUTTON( IDC_MENUREGION_BUTTON_EDIT )
		{
			if( LockTemplate() )
			{
				// ���� �Ǿ��ִ��� Ȯ��..
				OnSelchangeTemplate();

				if( REGION_NO_TEMPLATE == GetCurrentTemplate() )
				{
					MessageBox( "���ø� �����ϰ� ������!" );
				}
				else
				{
					ApmMap::RegionTemplate * pTemplate = g_pcsApmMap->GetTemplate( GetCurrentTemplate() );

					ASSERT( NULL != pTemplate );

					CTemplateEditDlg	dlg;
					dlg.SetTemplate( pTemplate );
					
					if( IDOK == dlg.DoModal() )
					{
						m_bChangedTemplate	= TRUE;
				
						UpdateList();
					}
				}
			}
			else
			{
				MessageBox( "RegionTemplate.ini ������ ���� �����־��!" );
			}
		}
		CW_CLICKED_BUTTON( IDC_MENUREGION_BUTTON_ADD )
		{
			if( LockTemplate() )
			{
				CTemplateEditDlg	dlg;

				// �� ���ø� �ε��� ����..
				int nLast = 0;
				int i;
				for( i = 0 ; i < 256 ; ++ i )
				{
					if( NULL == g_pcsApmMap->GetTemplate( i ) )
					{
						nLast = i;
						break;
					}
				}

				if( i == 256 )
				{
					MessageBox( "���ø��� ������ ���̻� ���� �� �����. �����ڿ� �����ϼ��� -_-;;" );
				}
				else
				{

					ApmMap::RegionTemplate	stTemplate;
					stTemplate.nIndex = nLast;
					dlg.SetTemplate( &stTemplate );

					if( IDOK == dlg.DoModal() )
					{
						// ���ø� �߰�..		
						while( !g_pcsApmMap->AddTemplate( & stTemplate ) )
						{
							MessageBox( "����Ÿ�� ������ �־ �߰��ĺ���( Ư�� �ε��� )" );

							dlg.SetTemplate( &stTemplate );
							if( IDOK == dlg.DoModal() )
							{

							}
							else
							{
								break;
							}
						}

						UpdateList();
						m_bChangedTemplate = TRUE;
					}
				}
			}
			else
			{
				MessageBox( "RegionTemplate.ini ������ ���� �����־��!" );
			}

		}
		CW_CLICKED_BUTTON( IDC_MENUREGION_BUTTON_REMOVE )
		{
			if( LockTemplate() )
			{
				OnSelchangeTemplate();
				
				if( REGION_NO_TEMPLATE == GetCurrentTemplate() )
				{
					MessageBox( "���ø� �����ϰ� ������!" );
				}
				else
				{
					ApmMap::RegionTemplate * pTemplate = g_pcsApmMap->GetTemplate( GetCurrentTemplate() );

					ASSERT( NULL != pTemplate );

					CString	str;
					str.Format( "���� ���ø�'%s' �� ���� ���﷡��?" , pTemplate->pStrName );
					if( IDYES == MessageBox( str , "������" , MB_YESNOCANCEL ) )
					{
						g_pcsApmMap->RemoveTempate( pTemplate->nIndex );
						UpdateList();
						m_bChangedTemplate = TRUE;
					}
				}
			}
			else
			{
				MessageBox( "RegionTemplate.ini ������ ���� �����־��!" );
			}
		}
		CW_CLICKED_BUTTON( IDC_MENUREGION_BUTTON_DELETE )
		{
			OnDeleteKeyDown();			
		}
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

void	CRTMenuRegion::UpdateList()
{
	m_ctlTemplate.ResetContent();

	AuNode< ApmMap::RegionTemplate > * pNode = g_pcsApmMap->m_listTemplate.GetHeadNode();
	ApmMap::RegionTemplate * pTemplateInList;

	CString	str;

	while( pNode )
	{
		pTemplateInList	= & pNode->GetData();

		str.Format( SKY_TEMPLATE_FORMAT , pTemplateInList->nIndex , pTemplateInList->pStrName );

		m_ctlTemplate.AddString( str );

		pNode = pNode->GetNextNode();
	}
}

void CRTMenuRegion::OnSelchangeTemplate() 
{
	// TODO: Add your control notification handler code here
	int nSelection = m_ctlTemplate.GetCurSel();

	if( nSelection == LB_ERR ) return;

	CString	str;

	m_ctlTemplate.GetText( nSelection , str );

	int	nIndex = atoi( (LPCTSTR) str );

	SetCurrentTemplate( nIndex );
}

BOOL CRTMenuRegion::OnDeleteKeyDown()
{
	RemoveSelected();
	return TRUE;
}

void CRTMenuRegion::OnPaintOnMap		( CDC * pDC )
{

	BOOL	bControlState = FALSE;
	if( GetAsyncKeyState( VK_CONTROL ) < 0 )
		bControlState = TRUE;

	CRect	rect , rectMap;
	this->m_pParent->m_pMainDlg->m_ctlRegion.GetClientRect( rect );

	if( g_bTeleportOnlyMode )
	{

	}
	else
	{
		// ���� ǥ��..
		AuNode< ApmMap::RegionElement > * pNode = m_listElement.GetHeadNode();
		ApmMap::RegionElement * pElement;

		CPen	penRegion;
		penRegion.CreatePen( PS_SOLID , 2 , RGB( 87 , 25 , 255 )	);
		pDC->SelectObject( penRegion );
		pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );

		pDC->SetBkColor( RGB( 255 , 255 , 255 ) );
		while( pNode )
		{
			pElement	= & pNode->GetData();

			//if( pElement->nIndex < MAX_COLOR_SAMPLE )
			{
				penRegion.DeleteObject();
				penRegion.CreatePen( PS_SOLID , 2 , __GetColor( pElement->nIndex ) 	);
				pDC->SelectObject( penRegion );
			}

			/*else
			{
				penRegion.DeleteObject();
				penRegion.CreatePen( PS_SOLID , 2 , RGB( 87 , 25 , 255 )	);
				pDC->SelectObject( penRegion );
			}
			*/

			rectMap.SetRect(
				( INT32 ) ( pElement->nStartX * m_fScale) + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX,
				( INT32 ) ( pElement->nStartZ * m_fScale) + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ,
				( INT32 ) ( pElement->nEndX * m_fScale	) + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX ,
				( INT32 ) ( pElement->nEndZ * m_fScale	) + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ);

			if ( rect.right > rectMap.left && rectMap.right > rect.left)
			{
				if (rect.bottom > rectMap.top && rectMap.bottom > rect.top )
				{
					// ȭ�鿡 ���Եȴ�..
					// 

					if( bControlState )
					{
						pDC->FillSolidRect( rectMap , __GetColor( pElement->nIndex ) );
					}
					else
					{
						if( pNode == m_pSelectedElement )
							pDC->FillSolidRect( rectMap , RGB( 255 , 255 , 255 ) );
						else
							pDC->Rectangle( rectMap );
					}

					CString	str;
					
					ApmMap::RegionTemplate * pTemplate = g_pcsApmMap->GetTemplate( pElement->nIndex );
					if( pTemplate )
					{
						str.Format( "%d , %s" , pTemplate->nIndex , pTemplate->pStrName );
						pDC->DrawText( str , rectMap , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
					}
				}
			}

			pNode = pNode->GetNextNode();
		}


		// Selection ����..
		if( m_nRangeDragMode == MODE_RANGESELECTED )
		{
			CPen	penSelection;
			penSelection.CreatePen( PS_SOLID , 2 , RGB( 255, 128 , 128 )	);
			pDC->SelectObject( penSelection );
			pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );

			pDC->Rectangle(
				m_SelectedPosX1 + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX ,
				m_SelectedPosZ1 + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ ,
				m_SelectedPosX2 + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX ,
				m_SelectedPosZ2 + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ );
		}
	}
}

BOOL CRTMenuRegion::OnLButtonDownMap	( CPoint &pointOriginal , int x , int z )
{
	OnSelchangeTemplate();
	
	AuNode< ApmMap::RegionElement > * pNode = GetRegion( x , z , GetCurrentTemplate() );

	if( LockRegion() )
	{
		if( pNode )
		{
			// ����..
			m_pSelectedElement	= pNode;

			// �巡�׸��..
			int nOffsetX , nOffsetZ;

			nOffsetX = x;
			nOffsetZ = z;
			m_nRangeDragMode = MODE_REGIONMOVE;
			m_pointLastPress	= pointOriginal ;
		
			m_pParent->InvalidateRegionView();
		}
		else
		{
			if( GetCurrentTemplate() != REGION_NO_TEMPLATE )
			{
				m_pParent->SetCaptureRegionView();
			
				int nOffsetX , nOffsetZ;

				nOffsetX = x;
				nOffsetZ = z;

				// �巡�׸��..
				m_nRangeDragMode = MODE_RANGESELECTED;
				m_SelectedPosX1	 = m_SelectedPosX2 = nOffsetX;
				m_SelectedPosZ1	 = m_SelectedPosZ2 = nOffsetZ;
				m_pParent->InvalidateRegionView();
			}
			else
			{
				// do nothing..
			}
		}
	}
	else
	{
		CString	str;
		str.Format( "'%s'�������� Region.ini �� �����־��" , g_strLastLockOwnerName );
		MessageBox( str );
	}

	return TRUE;
}

BOOL CRTMenuRegion::OnLButtonUpMap		( CPoint &pointOriginal , int x , int z )
{
	m_pParent->ReleaseCaptureRegionView();

	if( m_nRangeDragMode == MODE_RANGESELECTED )
	{
		// ������ (2004-09-10 ���� 6:04:32) : 
		// ��ư�� ���� ó���ϴ� �༮��..
		int nX1 , nX2 , nZ1 , nZ2;

		// ���� ǥ����..
		if( m_SelectedPosX1 < m_SelectedPosX2 )
		{
			nX1 = m_SelectedPosX1;
			nX2 = m_SelectedPosX2;
		}
		else									
		{
			nX1 = m_SelectedPosX2;
			nX2 = m_SelectedPosX1;
		}
		
		if( m_SelectedPosZ1 < m_SelectedPosZ2 )
		{
			nZ1 = m_SelectedPosZ1;
			nZ2 = m_SelectedPosZ2;
		}
		else									
		{
			nZ1 = m_SelectedPosZ2;
			nZ2 = m_SelectedPosZ1;
		}

		// �����߰�..
		if( GetCurrentTemplate() != REGION_NO_TEMPLATE )
		{
			if( LockRegion() )
			{
				AddRegion( GetCurrentTemplate() , 
					( INT32 ) ( nX1 / m_fScale ), 
					( INT32 ) ( nZ1 / m_fScale ), 
					( INT32 ) ( nX2 / m_fScale ), 
					( INT32 ) ( nZ2 / m_fScale ) );
				m_pParent->InvalidateRegionView();
			}
			else
			{
				CString	str;
				str.Format( "'%s'�������� Region.ini �� �����־��" , g_strLastLockOwnerName );
				MessageBox( str );
				return FALSE;
			}
		}
		else
		{
			// .
			// �߰����� ����..

			MessageBox( "���ø�����~." );
		}

		m_nRangeDragMode = MODE_NONE;
	}

	if( m_nRangeDragMode == MODE_REGIONMOVE)
	{
		m_nRangeDragMode = MODE_NONE;	

		// �ش� ������Ʈ�� ����Ʈ ������ �̵�..
		if( LockRegion() )
		{
			if( m_pSelectedElement )
			{
				ApmMap::RegionElement stElement = m_pSelectedElement->GetData();
				m_listElement.RemoveNode( m_pSelectedElement );
				m_listElement.AddTail( stElement );
				m_pSelectedElement = m_listElement.GetTailNode();
				m_pParent->InvalidateRegionView();
			}
		}
	}

	return TRUE;
}

BOOL CRTMenuRegion::OnMouseMoveMap		( CPoint &pointOriginal , int x , int z )
{
	if( m_nRangeDragMode == MODE_RANGESELECTED )
	{
		// ���� ������..
		m_SelectedPosX2 = x;
		m_SelectedPosZ2 = z;
		m_pParent->InvalidateRegionView();
	}
	
	if( m_nRangeDragMode == MODE_REGIONMOVE )
	{
		// ���� ������..
		int	dx , dz;

		dx	= m_pointLastPress.x - pointOriginal.x ;
		dz	= m_pointLastPress.y - pointOriginal.y ;

		dx = ( INT32 ) ( dx / m_fScale );
		dz = ( INT32 ) ( dz / m_fScale );

		m_pointLastPress = pointOriginal;
		
		if( m_pSelectedElement )
		{
			ApmMap::RegionElement * pElement= & m_pSelectedElement->GetData();
			pElement->nStartX	-= dx;
			pElement->nStartZ	-= dz;
			pElement->nEndX		-= dx;
			pElement->nEndZ		-= dz;
		}

		m_pParent->InvalidateRegionView();
	}

	return TRUE;
}

BOOL	CRTMenuRegion::SetCurrentTemplate( int nIndex )
{
	if( !g_pcsApmMap->GetTemplate( nIndex ) )		return FALSE;
		
	m_nCurrentTemplate = nIndex;
	return TRUE;
}

BOOL						CRTMenuRegion::AddRegion( int nIndex , INT32 nStartX , INT32 nStartZ , INT32 nEndX , INT32 nEndZ , INT32 nKind )
{
	ApmMap::RegionElement	element;

	element.nIndex	= nIndex	;
	element.nStartX	= nStartX	;
	element.nStartZ	= nStartZ	;
	element.nEndX	= nEndX		;
	element.nEndZ	= nEndZ		;
	element.nKind	= nKind		;

	// �����ؼ� �־���ϴµ�..
	// ������...

	m_listElement.AddTail( element );
	return TRUE;
}

AuNode< ApmMap::RegionElement > *	CRTMenuRegion::GetRegion( INT32 nOffsetX , INT32 nOffsetZ , INT32 nSelectedTemplate )
{
	AuNode< ApmMap::RegionElement > * pNode = m_listElement.GetHeadNode();
	ApmMap::RegionElement * pElement;

	nOffsetX = ( INT32 ) ( ( nOffsetX ) / m_fScale );
	nOffsetZ = ( INT32 ) ( ( nOffsetZ ) / m_fScale );

	while( pNode )
	{
		pElement	= & pNode->GetData();

		if( pElement->nStartX <= nOffsetX && nOffsetX <= pElement->nEndX	&&
			pElement->nStartZ <= nOffsetZ && nOffsetZ <= pElement->nEndZ	&&
			nSelectedTemplate == pElement->nIndex )
		{
			return pNode;
		}

		pNode = pNode->GetNextNode();
	}

	return NULL;
	// �׷��� ����..
}

BOOL	CRTMenuRegion::RemoveRegion( AuNode< ApmMap::RegionElement > * pNode )
{
	ASSERT( NULL != pNode );

	if( NULL == pNode ) return FALSE;

	if( LockRegion() )
	{
		m_listElement.RemoveNode( pNode );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void	CRTMenuRegion::RemoveSelected()
{
	if( m_pSelectedElement )
	{
		RemoveRegion( m_pSelectedElement );
		m_pSelectedElement = NULL;

		m_pParent->InvalidateRegionView();
	}
}

BOOL	CRTMenuRegion::Save( char * pFileName , BOOL bEncryption )
{
	if( m_bChangedRegion )
	{
		AuIniManagerA	iniManager;
		iniManager.SetPath( pFileName );

		ApmMap::RegionElement * pElement;
		AuNode< ApmMap::RegionElement >	* pNode				= m_listElement.GetHeadNode();
		char	strSection [ 256 ];

		int	nSeq = 0;

		while( pNode )
		{
			pElement	= & pNode->GetData();

			wsprintf( strSection , g_strINIRegionElementSection , nSeq++ );

			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementIndex		, pElement->nIndex	);
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyKind		, pElement->nKind	);
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyStartX	, pElement->nStartX	);
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyStartZ	, pElement->nStartZ	);
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyEndX		, pElement->nEndX	);
			iniManager.SetValueI	( strSection , ( char * ) g_strINIRegionElementKeyEndZ		, pElement->nEndZ	);

			pNode = pNode->GetNextNode();
		}

		iniManager.WriteFile(0, bEncryption);

		m_bChangedRegion	= FALSE;

		/*
		// ���ҽ�Ű�� ���� ������Ʈ
		char strFilename[ FILENAME_MAX ];
		wsprintf( strFilename , RM_RK_DIRECTORY_BIN "%s" , pFileName );

		if( g_bTeleportOnlyMode )
		{
			return TRUE;
		}
		else
		{
			if( FileUpload( strFilename ) )
			{
				if( UnLock( strFilename ) )
				{
				}
				return TRUE;
			}
			else
			{
				CString	str;
				str.Format( "'%s' ���� ���ε� ����!" , pFileName );
				MessageBox( str , "Region Tool" , MB_ICONERROR | MB_OK );
				return FALSE;
			}
		}
		*/
	}

	return TRUE;
}

BOOL	CRTMenuRegion::Load( char * pFileName , BOOL bDecryption )
{
	AuIniManagerA	iniManager;

	iniManager.SetPath(	pFileName	);

	if( iniManager.ReadFile(0, bDecryption) )
	{
		int		nSectionCount	;

		int		nKeyIndex	;
		int		nKeyKind	;
		int		nKeyStartX	;
		int		nKeyStartZ	;
		int		nKeyEndX	;
		int		nKeyEndZ	;

		nSectionCount	= iniManager.GetNumSection();	

		// �ױ��� �߰���..

		ApmMap::RegionElement	stElement;

		for( int i = 0 ; i < nSectionCount ; ++i  )
		{
			nKeyIndex	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementIndex		);
			nKeyKind	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyKind	);
			nKeyStartX	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyStartX	);
			nKeyStartZ	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyStartZ	);
			nKeyEndX	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyEndX	);
			nKeyEndZ	= iniManager.FindKey( i , ( char * ) g_strINIRegionElementKeyEndZ	);

			stElement.nIndex		= atoi( iniManager.GetValue	( i , nKeyIndex		) );
			stElement.nKind			= atoi( iniManager.GetValue	( i , nKeyKind		) );

			stElement.nStartX		= atoi( iniManager.GetValue	( i , nKeyStartX	) );
			stElement.nStartZ		= atoi( iniManager.GetValue	( i , nKeyStartZ	) );
			stElement.nEndX			= atoi( iniManager.GetValue	( i , nKeyEndX		) );
			stElement.nEndZ			= atoi( iniManager.GetValue	( i , nKeyEndZ		) );

			AddRegion( &stElement );
		}

		m_bChangedRegion	= FALSE;
		
		return TRUE;
	}
	else
	{
		TRACE( "���ø� ������ ����!.\n" );
		return FALSE;
	}
}

BOOL	CRTMenuRegion::LockRegion()
{
	m_bChangedRegion = TRUE;		
	return TRUE;
	/*
	if( m_bChangedRegion || g_bTeleportOnlyMode ) return TRUE;

	char strFilename[ FILENAME_MAX ];
	wsprintf( strFilename , RM_RK_DIRECTORY_BIN "Regiontool\\Region.ini" );

	if( IsLock( strFilename ) )
	{
		CString	strOwner;
		GetLockOwner( strFilename , strOwner );
		g_strLastLockOwnerName = strOwner;

		if( strOwner == this->m_pParent->m_pMainDlg->m_strUserName )
		{
			// �̹� ���ϰ� �ִ°�.

			// �� �÷��� ����..
			m_bChangedRegion = TRUE;
			return TRUE;
		}
		else
		{
			// ���� ���� �־�.
			return FALSE;
		}
	}

	// ������ ������ ���ε� ��Ų��..
	if( IsExistFileToRemote( strFilename ) )
	{
		// �ֽ��������� Ȯ��..
		if( !IsLatestFile( strFilename ) )
		{
			if( FileDownLoad( strFilename ) )
			{
				// ���� �ޱ� ����..
			}
			else
			{
				MessageBox( "�ֽ����� �ޱ� ����!" );
				return FALSE;
			}
		}

		if( Lock( strFilename ) )
		{
			// �� �÷��� ����..
			m_bChangedRegion = TRUE;
			return TRUE;
		}
		else
		{
			// �� ����
			return FALSE;
		}
	}
	else
	{
		// ������ü�� �����ϱ�..
		// �׳� �� ����.
		// �� �÷��� ����..
		m_bChangedRegion = TRUE;
		return TRUE;
	}
	*/
}

BOOL	CRTMenuRegion::LockTemplate()
{
	// 2004/12/31 ��� ��..
	return TRUE;

	/*
	if( m_bChangedTemplate ) return TRUE;

	char strFilename[ FILENAME_MAX ];
	wsprintf( strFilename , RM_RK_DIRECTORY_BIN "Regiontool\\ApmMap::RegionTemplate.ini" );

	if( IsLock( strFilename ) )
	{
		CString	strOwner;
		GetLockOwner( strFilename , strOwner );
		g_strLastLockOwnerName = strOwner;

		if( strOwner == this->m_pParent->m_pMainDlg->m_strUserName )
		{
			// �̹� ���ϰ� �ִ°�.

			// �� �÷��� ����..
			m_bChangedTemplate = TRUE;
			return TRUE;
		}
		else
		{
			// ���� ���� �־�.
			return FALSE;
		}
	}

	// ������ ������ ���ε� ��Ų��..
	if( IsExistFileToRemote( strFilename ) )
	{
		// �ֽ��������� Ȯ��..
		if( !IsLatestFile( strFilename ) )
		{
			if( FileDownLoad( strFilename ) )
			{
				// ���� �ޱ� ����..
			}
			else
			{
				MessageBox( "�ֽ����� �ޱ� ����!" );
				return FALSE;
			}
		}

		if( Lock( strFilename ) )
		{
			// �� �÷��� ����..
			m_bChangedTemplate = TRUE;
			return TRUE;
		}
		else
		{
			// �� ����
			return FALSE;
		}
	}
	else
	{
		// ������ü�� �����ϱ�..
		// �׳� �� ����.
		// �� �÷��� ����..
		m_bChangedTemplate = TRUE;
		return TRUE;
	}
	*/
}

void	CRTMenuRegion::SetWindowControlPosition()
{
	if( m_ctlTemplate		.GetSafeHwnd() &&
		m_wndButtonEdit		.GetSafeHwnd() &&
		m_wndButtonAdd		.GetSafeHwnd() &&
		m_wndButtonRemove	.GetSafeHwnd() &&
		m_wndButtonDelete	.GetSafeHwnd() )
	{
		CRect	rect;
		GetClientRect( rect );

		CRect	rectList , rectButton;
		const int nButtonHeight = 20;

		rectList = rect;
		rectList.bottom -= ( nButtonHeight + 3 ) * 4 ;

		m_ctlTemplate.MoveWindow( rectList );

		rectButton = rect;

		rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 0;
		rectButton.bottom	= rectButton.top + nButtonHeight;
		m_wndButtonEdit.MoveWindow( rectButton );
		
		rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 1;
		rectButton.bottom	= rectButton.top + nButtonHeight;
		m_wndButtonAdd.MoveWindow( rectButton );
		
		rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 2;
		rectButton.bottom	= rectButton.top + nButtonHeight;
		m_wndButtonRemove.MoveWindow( rectButton );
		
		rectButton.top		= rectList.bottom + 3 + ( nButtonHeight + 3 ) * 3;
		rectButton.bottom	= rectButton.top + nButtonHeight;
		m_wndButtonDelete.MoveWindow( rectButton );
	}
}

void CRTMenuRegion::OnSize(UINT nType, int cx, int cy)
{
	CRTMenuBase::OnSize(nType, cx, cy);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if( this->GetSafeHwnd() )
	{
		SetWindowControlPosition();
	}
}
