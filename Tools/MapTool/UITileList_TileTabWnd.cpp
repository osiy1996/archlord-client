// UITileList_TileTabWnd.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MapTool.h"
#include "rwcore.h"
#include "rpworld.h"
//#include "AcuRpDWSector.h"
#include "MainWindow.h"
#include "UITileList_TileTabWnd.h"
#include "UITileList_EditTileDlg.h"
#include "MainFrm.h"
#include "ApUtil.h"


#include "TileEditCategoryDlg.h"
#include "TileNameEditDlg.h"
#include "ConvertTextureDlg.h"

#include "MyEngine.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern MainWindow			g_MainWindow	;
extern CMainFrame *			g_pMainFrame	;
extern MyEngine				g_MyEngine		;

#define	IDT_CURSOR	13034
/////////////////////////////////////////////////////////////////////////////
// CUITileList_TileTabWnd

CUITileList_TileTabWnd::CUITileList_TileTabWnd()
{
	m_pSelectedCategory	= NULL					;
	m_nSelectedLayer	= TILELAYER_BACKGROUND	;
	m_pSelectedTile		= NULL					;
}

CUITileList_TileTabWnd::~CUITileList_TileTabWnd()
{
}


BEGIN_MESSAGE_MAP(CUITileList_TileTabWnd, CWnd)
	//{{AFX_MSG_MAP(CUITileList_TileTabWnd)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(IDM_TILEWINDOW_ADDCATEGORY, OnTilewindowAddcategory)
	ON_COMMAND(IDM_TILEWINDOW_DELETE_CATEGORY, OnTilewindowDeleteCategory)
	ON_COMMAND(IDM_TILEWINDOW_EDIT_CATEGORY, OnTilewindowEditCategory)
	ON_WM_DROPFILES()
	ON_COMMAND(IDM_TILEWINDOW_EDIT_TILE, OnTilewindowEditTile)
	ON_COMMAND(IDM_TILEWINDOW_DELETE_TILE, OnTilewindowDeleteTile)
	ON_COMMAND(IDM_TILEPOPUP_CHANGETILE, OnTilewindowChangeTile)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUITileList_TileTabWnd message handlers

void CUITileList_TileTabWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );

	CDC	memDC;
	memDC.CreateCompatibleDC( &dc );

	// �ٴ�Ÿ��..;;
	m_MemDC.FillRect( rect , &m_backgroundbrush );

	m_MemDC.SetBkMode( TRANSPARENT );

// ������ (2004-05-30 ���� 4:47:28) : ���� ������ ���̾� �ϴ� ����..
//	// ���̾� ���
//	{
//		CRect	rectLayer;
//		rectLayer.left	= 0					;
//		rectLayer.top	= 0					;
//		rectLayer.right	= rect.Width()		;
//		rectLayer.bottom= TILELAYER_HEIGHT	;
//
//		memDC.SelectObject( m_aBitmapLayer[ m_nSelectedLayer ] );
//		m_MemDC.BitBlt( rectLayer.left , rectLayer.top , rectLayer.Width() , rectLayer.Height() , &memDC , 0 , 0 , SRCCOPY );
//	}

	// ī�װ� ���.
	{
		AuNode< stTileCategory * > *	pNode = m_listCategory.GetHeadNode();
		stTileCategory *				pRef			;
		int								count = 0		;
		CRect							rectCategory	;

		while( pNode )
		{
			pRef = pNode->GetData();

			// Rect ����.
			rectCategory.left	= 0										;
			rectCategory.top	= TILELAYER_HEIGHT + TILECATEGORY_HEIGHT * count			;
			rectCategory.right	= TILECATEGORY_WIDTH					;
			rectCategory.bottom	= TILELAYER_HEIGHT + TILECATEGORY_HEIGHT * ( count + 1 )	;

			// �ڽ�ǥ��.
			if( pRef == m_pSelectedCategory )
			{
				// ���É°�;
				/* dc.FillSolidRect	( rectCategory , TILECATEGORY_COLOR_SELECTEDBOX	);*/
				memDC.SelectObject( m_bitmapCategoryBoxSelected );
				m_MemDC.BitBlt( rectCategory.left , rectCategory.top , rectCategory.Width() , rectCategory.Height() , &memDC , 0 , 0 , SRCCOPY );

				m_MemDC.SetTextColor		( TILECATEGORY_COLOR_SELECTEDTEXT				);
				m_MemDC.DrawText			( pRef->name , rectCategory , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
			}
			else
			{
				// ���õ��� ������.
				/* dc.FillSolidRect	( rectCategory , TILECATEGORY_COLOR_DEFAULTBOX	);*/
				memDC.SelectObject( m_bitmapCategoryBoxUnselect );
				m_MemDC.BitBlt( rectCategory.left , rectCategory.top , rectCategory.Width() , rectCategory.Height() , &memDC , 0 , 0 , SRCCOPY );
				m_MemDC.SetTextColor		( TILECATEGORY_COLOR_DEFAULTTEXT				);
				m_MemDC.DrawText			( pRef->name , rectCategory , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
			}

			m_listCategory.GetNext( pNode );
			count++;
		}
	}

	// ī�װ� ���É°� ������
	// 
	if( m_pSelectedCategory )
	{
		// �����.. 
	
		AuNode< stTileInfo > * pNode = m_pSelectedCategory->list.GetHeadNode();
		stTileInfo	* pTileInfo;

		int		count		;
		int		line		;
		int		offset		;
		CRect	rectTileBox	;
		CRect	rectOutLine	;
		CRect	rectTmp		;

		count	= 0;
		line	= 0;
		offset	= 0;

		while( pNode )
		{
			pTileInfo = & pNode->GetData();

			// ���� ����.
			line	= count / TILECATEGORY_COUNTPERLINE;
			offset	= count % TILECATEGORY_COUNTPERLINE;

			// Rect ����.
			rectTileBox.SetRect(
				TILECATEGORY_WIDTH +	TILECATEGORY_TILEWMARGIN * ( 1 + offset	* 2	) + TILECATEGORY_TILEWIDTH	* ( offset		),
				TILELAYER_HEIGHT + 		TILECATEGORY_TILEHMARGIN * ( 1 + line	* 2	) + TILECATEGORY_TILEHEIGHT	* ( line		),
				TILECATEGORY_WIDTH +	TILECATEGORY_TILEWMARGIN * ( 1 + offset	* 2	) + TILECATEGORY_TILEWIDTH	* ( offset	+ 1	),
				TILELAYER_HEIGHT + 		TILECATEGORY_TILEHMARGIN * ( 1 + line	* 2	) + TILECATEGORY_TILEHEIGHT	* ( line	+ 1	)
				);

			// ���� ���.
			// �ƿ����� �ڽ��� ����..
			rectOutLine.left	= rectTileBox.left		- TILECATEGORY_TILEWMARGIN;
			rectOutLine.top		= rectTileBox.top		- TILECATEGORY_TILEHMARGIN;
			rectOutLine.right	= rectTileBox.right		+ TILECATEGORY_TILEWMARGIN;
			rectOutLine.bottom	= rectTileBox.bottom	+ TILECATEGORY_TILEHMARGIN;

			// ������ ǥ��.
			if( m_pSelectedTile == pTileInfo )
			{	
				static bool bToggle = false;
				if( bToggle )
				{
					memDC.SelectObject( m_bitmapTileSelect );
				}
				else
				{
					memDC.SelectObject( m_bitmapTileSelect2 );
				}
				bToggle = ! bToggle;
				m_MemDC.BitBlt( rectOutLine.left , rectOutLine.top , rectOutLine.Width() , rectOutLine.Height() , &memDC , 0 , 0 , SRCCOPY );

				m_rectCursor	= rectOutLine;

				// dc.FillSolidRect( rectOutLine , TILECATEGORY_COLOR_SELECTEDBOX );
			}
			else
			{
				// do nothing;
				AuNode< int > * pNode = g_pMainFrame->m_Document.m_listSelectedTileIndex.GetHeadNode();
				int	nTile;
				while( pNode )
				{
					nTile = pNode->GetData();

					if( pTileInfo->index == nTile )
					{
						memDC.SelectObject( m_bitmapTileSelect );
						m_MemDC.BitBlt( rectOutLine.left , rectOutLine.top , rectOutLine.Width() , rectOutLine.Height() , &memDC , 0 , 0 , SRCCOPY );

						m_rectCursor	= rectOutLine;
						break;
					}

					pNode = pNode->GetNextNode();
				}
			}

			// �̹��� ���
			// dc.FillSolidRect( rectTileBox , RGB( 255 , 255 , 255 ) );

			LoadBitmap( pTileInfo );
			if( pTileInfo->pBmp )
			{
				pTileInfo->pBmp->Draw( m_MemDC.GetSafeHdc() , rectTileBox.left , rectTileBox.top );
			}

			// �ؽ�Ʈ ���
			rectTmp			= rectOutLine;
			rectTmp.left	+= 1;
			rectTmp.top		+= 1;
			rectTmp.right	+= 1;
			rectTmp.bottom	+= 1;
			
			m_MemDC.SetTextColor	( TILECATEGORY_TILEINFOTEXTBKCOLOR );
			m_MemDC.DrawText		( pTileInfo->name , rectTmp , DT_LEFT | DT_TOP | DT_SINGLELINE );

			if( IsUsedTileName( pTileInfo ) )
				m_MemDC.SetTextColor	( TILECATEGORY_TILEINFOTEXTCOLOR_USED );
			else
				m_MemDC.SetTextColor	( TILECATEGORY_TILEINFOTEXTCOLOR_UNUSED );

			m_MemDC.DrawText		( pTileInfo->name , rectOutLine , DT_LEFT | DT_TOP | DT_SINGLELINE );

			m_pSelectedCategory->list.GetNext( pNode );
			count ++;
		}
	}
		
	// ���� ���м� �߱�.
	m_MemDC.MoveTo( TILECATEGORY_WIDTH , TILELAYER_HEIGHT);
	m_MemDC.LineTo( TILECATEGORY_WIDTH , rect.Height()	);
	

	// ������ (2004-05-29 ���� 10:16:52) : ����� ǥ��~
	CRect	rectDimension = rect;
	rectDimension.top = rectDimension.bottom - 20;
	m_MemDC.FillSolidRect( rectDimension , RGB( 211, 135, 231 ) );
	
	AuNode< int > * pNode = g_pMainFrame->m_Document.m_listSelectedTileIndex.GetHeadNode();
	if( pNode )
	{
		int * pData = &pNode->GetData();
		CString	str;
		
		AcTextureList::TextureInfo * pInfo = AGCMMAP_THIS->m_TextureList.GetTextureInfo( * pData );
		if( pInfo )
		{
			str.Format( "%s - Ÿ�� %dx%d ���~" , __RemovePath( pInfo->strFilename ) , GET_TEXTURE_DIMENSION( *pData ) , GET_TEXTURE_DIMENSION( *pData ) );
		}
		else
		{
			str.Format( "Ÿ�� %dx%d ���~" , GET_TEXTURE_DIMENSION( *pData ) , GET_TEXTURE_DIMENSION( *pData ) );
		}

		m_MemDC.SetTextColor( RGB( 255 , 255 , 255 ) );
		m_MemDC.DrawText( str , rectDimension , DT_VCENTER | DT_CENTER | DT_SINGLELINE );
	}

	dc.BitBlt( 0 , 0 , rect.Width() , rect.Height() , & m_MemDC , 0 , 0 , SRCCOPY );
	// Do not call CWnd::OnPaint() for painting messages
}

void CUITileList_TileTabWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( point.x < TILECATEGORY_WIDTH )
	{
		// �˾� �޴��� ����.

		// ī�װ��� Ÿ���̳� ����
		stTileCategory * pRef;
		pRef = GetCategoryFromPoint( point );
		if( pRef )
		{
			// ���..;
			m_pSelectedCategory = pRef;
			Invalidate( FALSE );

			CMenu *pMenu;
			CMenu menu;

			menu.LoadMenu(IDR_TILEMENU);
			pMenu = menu.GetSubMenu(0);

			ClientToScreen( &point );

			TrackPopupMenu( pMenu->GetSafeHmenu() , TPM_TOPALIGN | TPM_LEFTALIGN ,
				point.x , point.y , 0 , GetSafeHwnd() , NULL );
		}
	
	}
	else
	{
		stTileInfo * pTileInfo;
		pTileInfo = GetTileFromPoint( point );
		if( pTileInfo )
		{
			if( pTileInfo != m_pSelectedTile )
			{
				// ���߼���..

				// Ÿ�� ������ �ٲ������ ���������쿡�� �˷��ش�.
				g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( pTileInfo->index );
			}
			else
			{
				m_pSelectedTile = pTileInfo;

				// Ÿ�� ������ �ٲ������ ���������쿡�� �˷��ش�.
				g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll()				;
				g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( pTileInfo->index );
				g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= pTileInfo->applyalpha		;
			}

			Invalidate( FALSE );

			switch( GET_TEXTURE_TYPE( pTileInfo->index ) )
			{
			case TT_FLOORTILE	:
			case TT_ONE			:
			case TT_ALPHATILE	:
				{
					// FLOOR ���� �����Ѵ�.
					ChangeLayer( TILELAYER_BACKGROUND );
				}
				break;

			case TT_UPPERTILE	:
				{
					// Float ���� �����Ѵ�.
					ChangeLayer( TILELAYER_UPPER );
				}
				break;

			}

			CMenu *pMenu;
			CMenu menu;

			menu.LoadMenu(IDR_TILEMENU);
			pMenu = menu.GetSubMenu(1);

			ClientToScreen( &point );

			TrackPopupMenu( pMenu->GetSafeHmenu() , TPM_TOPALIGN | TPM_LEFTALIGN ,
				point.x , point.y , 0 , GetSafeHwnd() , NULL );
		}
	}

	
	CWnd::OnRButtonDown(nFlags, point);
}

int CUITileList_TileTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( NULL == AGCMMAP_THIS ) return 0;

	// ������ ����� ī�װ� ������ �ε���.
	LoadScript();
	
	// �з��� ī�װ� ������ �̿��ؼ� �ε��ž��ִ� Ÿ���� �з��Ѵ�.
	
	CreateTextureList( &AGCMMAP_THIS->m_TextureList );

	// ó�� ��ϵ� Ÿ���� �����Ѵ�.
	if( m_listCategory.GetHeadNode() )
		m_pSelectedCategory = m_listCategory.GetHeadNode()->GetData()	;
	else
		m_pSelectedCategory = NULL										;

	DragAcceptFiles();
	
	m_backgroundbitmap.LoadBitmap			( IDB_TILE_BACKTILE		);
	m_backgroundbrush.CreatePatternBrush	( & m_backgroundbitmap	);

	m_aBitmapLayer[ TILELAYER_BACKGROUND	].LoadBitmap( IDB_TILE_LAYER_BACKGROUND	);
	m_aBitmapLayer[ TILELAYER_TONGMAP		].LoadBitmap( IDB_TILE_LAYER_TONGMAP	);
	m_aBitmapLayer[ TILELAYER_ALPHATILEING	].LoadBitmap( IDB_TILE_LAYER_ALPHA		);
	m_aBitmapLayer[ TILELAYER_UPPER			].LoadBitmap( IDB_TILE_LAYER_UPPER		);
	
	m_bitmapCategoryBoxSelected.	LoadBitmap( IDB_TILE_CATEGORY_SELECTED	);
	m_bitmapCategoryBoxUnselect.	LoadBitmap( IDB_TILE_CATEGORY_UNSELECT	);
	m_bitmapTileSelect.				LoadBitmap( IDB_TILE_TILESELECT			);
	m_bitmapTileSelect2.			LoadBitmap( IDB_TILE_TILESELECT2		);

	SetTimer( IDT_CURSOR , 1500 , NULL );

	CDC	*pDC = GetDC();
	m_MemDC.CreateCompatibleDC( pDC );
	ReleaseDC( pDC );

	CFont	font;
	font.CreatePointFont( 90 , "����" );
	m_MemDC.SelectObject( font );

	return 0;
}

void CUITileList_TileTabWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect	rect;
	GetClientRect( rect );

// ������ (2004-05-30 ���� 4:48:04) : ������ ���̾� �ϴ� ����.
	// ���̾�� �ƳĴ� ����
//	if( point.y < TILELAYER_HEIGHT )
//	{
//		// ���̾� �����̴�.
//
//		int layer = point.x / ( rect.Width() / TILELAYER_COUNT );
//		ChangeLayer( layer );
//
//		return;
//	}


	// ī�װ��� Ÿ���̳� ����
	stTileCategory * pRef;
	pRef = GetCategoryFromPoint( point );
	if( pRef )
	{
		// ���..;
		m_pSelectedCategory = pRef;
		Invalidate( FALSE );
		return;
	}

	stTileInfo * pTileInfo;
	pTileInfo = GetTileFromPoint( point );
	if( pTileInfo )
	{
		m_pSelectedTile = pTileInfo;

		// Ÿ�� ������ �ٲ������ ���������쿡�� �˷��ش�.
		
		g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll()				;
		g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( pTileInfo->index );
		g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= pTileInfo->applyalpha		;

		Invalidate( FALSE );

		switch( GET_TEXTURE_TYPE( pTileInfo->index ) )
		{
		case TT_FLOORTILE	:
		case TT_ONE			:
		case TT_ALPHATILE	:
			{
				// FLOOR ���� �����Ѵ�.
				ChangeLayer( TILELAYER_BACKGROUND );
			}
			break;

		case TT_UPPERTILE	:
			{
				// Float ���� �����Ѵ�.
				ChangeLayer( TILELAYER_UPPER );
			}
			break;

		}


		return;
	}
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CUITileList_TileTabWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);
}


void	CUITileList_TileTabWnd::LoadScript()
{
	// ��ũ��Ʈ ���� �ε�..
	
	FILE	*pFile = fopen( TILECATEGORY_FILE , "rt" );
	if( pFile == NULL )
	{
		// ���Ͽ��� ����.
		return ;
	}

	CGetArg2		arg;
	char			strbuffer[	1024	];
	//unsigned int	nRead = 0;

	int count = 0;
	char	categoryname[ 256 ];
	stTileCategory	*pCategory;
	while( fgets( strbuffer , 1024 , pFile ) )
	{
		// �о�����..

		arg.SetParam( strbuffer , "|\n" );

		if( arg.GetArgCount() < 2 )
		{
			// ���� �̻�
			continue;
		}

		pCategory			= new stTileCategory			;
		pCategory->category		= atoi( arg.GetParam( 0 ) )		;
		strncpy( categoryname , arg.GetParam( 1 ) , 256 )	;
		pCategory->name		= categoryname					;

		// �����ϸ鼭 ������..
		AddCategory( pCategory );
		
		count ++;
	}

	fclose( pFile );

}

BOOL CUITileList_TileTabWnd::AddCategory( stTileCategory * pCategory )
{
	// �����ؼ� ����ִ´�..( Ÿ�Ժ���.. )
	AuNode< stTileCategory * > *	pNode = m_listCategory.GetHeadNode();
	stTileCategory *				pRef;

	while( pNode )
	{
		pRef = pNode->GetData();

		if( pRef->category > pCategory->category )
		{
			m_listCategory.InsertBefore( pNode , pCategory );
			return TRUE;
		}

		m_listCategory.GetNext( pNode );
	}

	m_listCategory.AddTail( pCategory );

	return TRUE;
}

void CUITileList_TileTabWnd::OnDestroy() 
{
	CWnd::OnDestroy();

	// ī�װ� ����Ʈ..
	AuNode< stTileCategory * > * pNode;
	stTileCategory *	pRef;

	while( pNode = m_listCategory.GetHeadNode() )
	{
		pRef = pNode->GetData();

		AuNode< stTileInfo > * pNode = pRef->list.GetHeadNode();
		stTileInfo	* pInsertedTileInfo;
		while( pNode )
		{
			pInsertedTileInfo = & pNode->GetData();

			// ��Ʈ�� ����..
			if( pInsertedTileInfo->pBmp )
			{
				pInsertedTileInfo->pBmp->Release();
				delete pInsertedTileInfo->pBmp;
				pInsertedTileInfo->pBmp = NULL;
			}

			pRef->list.GetNext( pNode );
		}

		delete pRef;

		m_listCategory.RemoveHead();
	}

	KillTimer( IDT_CURSOR );
}

BOOL CUITileList_TileTabWnd::SaveScript()
{
	AuNode< stTileCategory * > * pNode = m_listCategory.GetHeadNode();
	stTileCategory *	pRef;

	char str[ 1024 ];
	GetCurrentDirectory( 1024 , str );
	FILE	*pFile = fopen( TILECATEGORY_FILE , "wt" );
	if( pFile == NULL )
	{
		// ���Ͽ��� ����.
		return FALSE;
	}
	

	while( pNode )
	{
		pRef = pNode->GetData();
		wsprintf( str , "%d|%s\n" , pRef->category , (LPCTSTR) pRef->name );
		fputs( str , pFile );
		m_listCategory.GetNext( pNode );
	}

	fclose( pFile );
	return TRUE;
}

BOOL CUITileList_TileTabWnd::CreateTextureList(AcTextureList *pList)
{
	AuNode< AcTextureList::TextureInfo > * pNode = pList->TextureList.GetHeadNode();
	AcTextureList::TextureInfo *	pTi			;

	while( pNode )
	{
		pTi = & pNode->GetData();

		switch( GET_TEXTURE_TYPE( pTi->nIndex ) )
		{
		case TT_FLOORTILE	:
		case TT_ONE			:
		case TT_ALPHATILE	:
			{
				// �Ѱ��� ���.
				AddTile( pTi->nIndex , pTi->strComment );
			}
			break;
		case TT_UPPERTILE	:
			{
				// �������� ���.

				// ������ ��� - -;

				// ������ �߰��ϰԵɰŽ�.
				CString	str;
				for( int y = 0 ; y < 4 ; y ++ )
				{
					for( int x = 0 ; x < 4 ; x ++ )
					{
						str.Format( "%s %d" , pTi->strComment , y * 4 + x );
						AddTile( pTi->nIndex + y * 4 + x , (LPSTR) (LPCTSTR) str );
					}
				}
			}
			break;
		}

		pList->TextureList.GetNext( pNode );
	}

	return TRUE;
}

BOOL CUITileList_TileTabWnd::LoadBitmap			( stTileInfo  * pTileInfo , BOOL bForce )
{
	if( !bForce && pTileInfo->pBmp )
	{
		return TRUE;
	}
	else
	{
		if( pTileInfo->pBmp ) delete pTileInfo->pBmp;

		pTileInfo->pBmp = new CBmp;

		// �ε�����.
		CString	filename;
		filename.Format( "%s\\Map\\Temp\\%d,%d.bmp" , ALEF_CURRENT_DIRECTORY ,
			GET_TEXTURE_CATEGORY( pTileInfo->index ) , GET_TEXTURE_INDEX( pTileInfo->index ) );

		AcTextureList::TextureInfo *	pTextureInfo = AGCMMAP_THIS->m_TextureList.GetTextureInfo( pTileInfo->index	);
		if( pTextureInfo )
		{
			AGCMMAP_THIS->m_TextureList.MakeTempBitmap( pTextureInfo );
		}

		if( !pTileInfo->pBmp->Load( (LPSTR) (LPCTSTR) filename , NULL ) )
		{
			// ������� ���� ����..;
			pTileInfo->pBmp->Load( TILECATEGORY_ERROR_BITMAP , NULL );
		}
	}

	return TRUE;
}

BOOL CUITileList_TileTabWnd::AddTile	( int index , char * comment	)
{
	stTileCategory *				pCategory	;
	pCategory = FindCategory( GET_TEXTURE_CATEGORY( index ) );

	// ī�װ��� �������� �ʴ°��̶��
	// Ÿ���� ���������Ͽ� �׳� �����ع�����.
	if( pCategory == NULL )
	{
		pCategory = new stTileCategory;

		CString	str;
		str.Format( "%d��Ÿ��" , GET_TEXTURE_CATEGORY( index ) );
		pCategory->category	= GET_TEXTURE_CATEGORY( index );
		pCategory->name		= str;

		AddCategory( pCategory );
	}

	// �����ؼ� ����.
	// �����ؼ� ����ִ´�..( Ÿ�Ժ���.. )
	AuNode< stTileInfo > * pNode = pCategory->list.GetHeadNode();
	stTileInfo	* pInsertedTileInfo;
	stTileInfo	insertitem;

	insertitem.index		= index				;
	insertitem.name			= comment			;
	insertitem.pBmp			= NULL				;
	
	insertitem.applyalpha	= ALEF_DEFAULT_ALPHA;
	
	// ��Ʈ ���Ѽ� ���̿� ��¦ ��¦ �� �ִ´�.
	while( pNode )
	{
		pInsertedTileInfo = & pNode->GetData();

		if( GET_TEXTURE_INDEX ( pInsertedTileInfo->index	) >
			GET_TEXTURE_INDEX ( index						)	)
		{
			
			pCategory->list.InsertBefore( pNode , insertitem );
			return TRUE;
		}

		pCategory->list.GetNext( pNode );
	}

	pCategory->list.AddTail( insertitem );

	return TRUE;
}

CUITileList_TileTabWnd::stTileCategory * CUITileList_TileTabWnd::FindCategory( int category )
{
	AuNode< stTileCategory * > * pNode = m_listCategory.GetHeadNode();
	stTileCategory * pCategory	;

	while( pNode )
	{
		pCategory = pNode->GetData();

		if( pCategory->category == category ) return pCategory;

		m_listCategory.GetNext( pNode );
	}

	return NULL;
}

void CUITileList_TileTabWnd::OnTilewindowAddcategory() 
{
	// ī�װ� �߰���.
	// ī�װ� �߰� Dialog ���.
	// ī�װ� �ѹ� ( ���������� ������ , ���Ƿ� �������� )
	// ī�װ� �̸� ����.
	// ī�װ� �ѹ��� ��ġ���� ���� , ��ģ���� ������ ǥ�����ش�.

	stTileCategory * pCategory;
	
	pCategory			= new stTileCategory			;

	if( m_listCategory.GetTailNode() != NULL )
		pCategory->category		= 		m_listCategory.GetTail()->category + 1;	// �������� ��ϵ� Ÿ�� �ε��� ���� 1�� ���Ѱ����� �����.
	else
		pCategory->category		=		1						;
	pCategory->name.Format( "%d��Ÿ��" , pCategory->category );
	
	ASSERT( pCategory->category < 16 );

	// �����ϸ鼭 ������..
	AddCategory( pCategory );
	m_pSelectedCategory	= pCategory;

	Invalidate( FALSE );

	OnTilewindowEditCategory();
}

void CUITileList_TileTabWnd::OnTilewindowDeleteCategory() 
{
	if( IDNO == MessageBox( "ī�װ� ������ ����ǲ�����? -_-" , "����" , MB_YESNO ) )
		return;

	// ���� ���õž��ִ� ī�װ��� ������

	// 1, ������ ��� ������ ����ִ� Ÿ�ϵ��� ��� ����Ʈ ī�װ��� �ű��
	//     - ����Ʈ ī�װ��� ���ڸ��� ���� Ÿ�� �������� ���ڶ�� ����ó����.
	// 2, ���� ī�װ��� �����Ѵ�.	

	DisplayMessage( "���� ī�װ��� �����մϴ�. ����ִ� Ÿ�ϵ� ���� ��� ���� ���� �ϴϴ�." );

	// 1,�ش� ī�װ� ����.
	// 2,����ִ� Ÿ�� ���� ����..
	// 3,�ؽ��� ����Ʈ���� ����..
	// 4,���Ű� �ִ� �ʵ���Ÿ�� �˻��ؼ� , ����Ʈ Ÿ�Ϸ� ��ȯ��.

	AuNode< stTileInfo > * pNode;
	stTileInfo	* pTileInfo;

	while( pNode = m_pSelectedCategory->list.GetHeadNode() )
	{
		pTileInfo = & pNode->GetData();

		// ��Ʈ�� ����..
		if( pTileInfo->pBmp )
		{
			pTileInfo->pBmp->Release();
			delete pTileInfo->pBmp;
			pTileInfo->pBmp	= NULL;
		}

		// �ʿ��� Ÿ�� ���� ����.

			// ���� ���� ���� -_-;

		// �ؽ��� ����Ʈ���� ����.
		AGCMMAP_THIS->m_TextureList.DeleteTexture( pTileInfo->index , TRUE );

		m_pSelectedCategory->list.RemoveHead();
	}

	// ī�װ� ����..
	AuNode< stTileCategory * > *	pNodeCategory = m_listCategory.GetHeadNode();

	while( pNodeCategory )
	{
		if( pNodeCategory->GetData() == m_pSelectedCategory )
		{
			m_listCategory.RemoveNode( pNodeCategory );
			break;
		}
		m_listCategory.GetNext( pNodeCategory );
	}

	Invalidate( FALSE );

	SaveScript();
}

void CUITileList_TileTabWnd::OnTilewindowEditCategory() 
{
	// ī�װ� ������ ��� �����ش�.
	// 

	if( m_pSelectedCategory == NULL )
	{
		DisplayMessage( AEM_ERROR , "EditCategory : ���õž��ִ� ī�װ��� ��� ��ҵž����ϴ�." );
		return;
	}

	CTileEditCategoryDlg dlg;
	dlg.m_nType		= m_pSelectedCategory->category;
	dlg.m_strName	= m_pSelectedCategory->name;

	if( dlg.DoModal() == IDOK )
	{
		m_pSelectedCategory->name = dlg.m_strName;
		Invalidate( FALSE );
	}

	SaveScript();
}

CUITileList_TileTabWnd::stTileCategory *	CUITileList_TileTabWnd::GetCategoryFromPoint	( CPoint	point )
{
	// ȭ�� ��ǥ�� ī�װ��� ã�´�.

	if( point.x < TILECATEGORY_WIDTH )
	{
		// ��� ī�װ��� ���� �ž�����...
		AuNode< stTileCategory * > *	pNode = m_listCategory.GetHeadNode();
		stTileCategory *				pRef			;
		int								count = 0		;
		CRect							rectCategory	;

		while( pNode )
		{
			pRef = pNode->GetData();

			// Rect ����.
			rectCategory.left	= 0										;
			rectCategory.top	= TILELAYER_HEIGHT + TILECATEGORY_HEIGHT * count			;
			rectCategory.right	= TILECATEGORY_WIDTH					;
			rectCategory.bottom	= TILELAYER_HEIGHT + TILECATEGORY_HEIGHT * ( count + 1 )	;

			if( rectCategory.PtInRect( point ) )
			{
				return pRef;
			}

			m_listCategory.GetNext( pNode );
			count++;
		}
	}

	return NULL;
}

CUITileList_TileTabWnd::stTileInfo *		CUITileList_TileTabWnd::GetTileFromPoint		( CPoint	point )
{
	// ȭ�� ��ǥ�� Ÿ�� ������ ã�´�.
	if( point.x > TILECATEGORY_WIDTH && m_pSelectedCategory )
	{
		AuNode< stTileInfo > * pNode = m_pSelectedCategory->list.GetHeadNode();
		stTileInfo	* pTileInfo;

		int		count		;
		int		line		;
		int		offset		;
		CRect	rectTileBox	;
		CRect	rectOutLine	;
		CRect	rectTmp		;

		count	= 0;
		line	= 0;
		offset	= 0;

		while( pNode )
		{
			pTileInfo = & pNode->GetData();

			// ���� ����.
			line	= count / TILECATEGORY_COUNTPERLINE;
			offset	= count % TILECATEGORY_COUNTPERLINE;

			// Rect ����.
			rectTileBox.SetRect(
				TILECATEGORY_WIDTH +	TILECATEGORY_TILEWMARGIN * ( 1 + offset	* 2	) + TILECATEGORY_TILEWIDTH	* ( offset		),
				TILELAYER_HEIGHT +		TILECATEGORY_TILEHMARGIN * ( 1 + line	* 2	) + TILECATEGORY_TILEHEIGHT	* ( line		),
				TILECATEGORY_WIDTH +	TILECATEGORY_TILEWMARGIN * ( 1 + offset	* 2	) + TILECATEGORY_TILEWIDTH	* ( offset	+ 1	),
				TILELAYER_HEIGHT +		TILECATEGORY_TILEHMARGIN * ( 1 + line	* 2	) + TILECATEGORY_TILEHEIGHT	* ( line	+ 1	)
				);

			// ���� ���.
			// �ƿ����� �ڽ��� ����..
			rectOutLine.left	= rectTileBox.left		- TILECATEGORY_TILEWMARGIN;
			rectOutLine.top		= rectTileBox.top		- TILECATEGORY_TILEHMARGIN;
			rectOutLine.right	= rectTileBox.right		+ TILECATEGORY_TILEWMARGIN;
			rectOutLine.bottom	= rectTileBox.bottom	+ TILECATEGORY_TILEHMARGIN;

			if( rectOutLine.PtInRect( point ) )
			{
				return pTileInfo;
			}


			m_pSelectedCategory->list.GetNext( pNode );
			count ++;
		}
	}

	return NULL;
}

void CUITileList_TileTabWnd::OnDropFiles(HDROP hDropInfo) 
{
	char	lpDroppedFilename[ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	char	copyfilename[ 1024 ];
	int		category , index;
	int		count = 0;
	stTileInfo	*pTi;
	AcTextureList::TextureInfo * pTLInfo;
	
	
	// �켱 ��� ������ �巡�� �Ǵ� �� Ȯ���Ѵ�.
	UINT dragcount = ::DragQueryFile( hDropInfo , 0xffffffff , lpDroppedFilename , 1024 );

	if( m_pSelectedCategory == NULL )
	{
		DisplayMessage( AEM_ERROR , "ī�װ��� ���õž����� �����Ƿ� �۾��� ��� �� �� �����ϴ�.");
		return;
	}

	// ���� ���É� ī�װ� Ÿ�� �̿�.
	category	= m_pSelectedCategory->category;

	if( dragcount > 0 )
	{
		// ��δ� �˻� �Ϸ���
		for ( int i = 0 ; i < ( int ) dragcount ; i ++ )
		{
			::DragQueryFile( hDropInfo , i , lpDroppedFilename , 1024 );

			// ���⼭ ��ӵ� ���Ϸ� ������� Ŭ���̾�Ʈ ����Ÿ�� �����Ұ����� ���Ѵ�.
			// Dialog�� ���� 
			// �ؽ��� ������ �����Ͽ� �̹����� �����Ѵ�.

			TRACE( "��ӵ� %d ��° ���� -'%s'\n" , i + 1 , lpDroppedFilename );

			// Tile ���丮�� ���� ������ �ִ��� �˻��Ѵ�.
			// Tile ���丮�� ī���� �ִ´�.
			_splitpath( lpDroppedFilename , drive, dir, fname, ext );

			wsprintf( copyfilename , "%s\\Map\\Tile\\%s%s" , ALEF_CURRENT_DIRECTORY , fname , ext );

			CConvertTextureDlg	dlg;
			dlg.m_strFileNameOrigin		= copyfilename		;
			dlg.m_strFileOriginalPath	= lpDroppedFilename	;
			dlg.m_strName				= fname				;
			if( dlg.DoModal() == IDOK )
			{
				// �ٽ� ��..
				_splitpath( ( LPCTSTR ) dlg.m_strFileOriginalPath , drive, dir, fname, ext );

				if( CopyFile( ( LPCTSTR ) dlg.m_strFileOriginalPath , ( LPCTSTR )dlg.m_strFileNameOrigin , FALSE ) == FALSE )
				{
					// ���� ī�ǿ���!..
					DisplayMessage( AEM_ERROR , "\"%s%s\" ������ �̹� �����ϹǷ� ī���� ���� �� �����ϴ�." , fname , ext );
					DisplayMessage( AEM_WARNNING , "���� ������ ó���� ��� �մϴ�." );
					continue;
				}

				// ī�װ�, �ڸ�Ʈ, Ÿ���� ������ ����Ʈ�� �߰��Ѵ�.
				// Ÿ��, �ε����� �����Ѵ�.
				// �� �ε��� ���..
				{
					AuNode< stTileInfo > * pNode = m_pSelectedCategory->list.GetHeadNode();
					stTileInfo	*pTiNext;

					index = 1;
					while( pNode )
					{
						pTi		= & pNode->GetData();
						if( pNode->GetNextNode() )
						{
							pTiNext = &pNode->GetNextNode()->GetData();
							if( GET_TEXTURE_INDEX( pTi->index ) + 1 != GET_TEXTURE_INDEX( pTiNext->index ) )
							{
								// ��ĭ�̴�!;;;
								index = GET_TEXTURE_INDEX( pTi->index ) + 1;
								break;
							}
						}
						else
						{
							index = GET_TEXTURE_INDEX( pTi->index ) + 1;
						}
						m_pSelectedCategory->list.GetNext( pNode );
					}
					// ����!
				}
				// �ؽ��� ����Ʈ�� �߰���.
				
				wsprintf( copyfilename , "Map\\Tile\\%s%s" , fname , ext );
				pTLInfo = AGCMMAP_THIS->m_TextureList.AddTexture( dlg.m_nDimension , category , index , dlg.m_nType , copyfilename , ( LPSTR ) ( LPCTSTR ) dlg.m_strName );
				
				// ���� ���É� ī�װ��� Ÿ���� �߰��Ѵ�.
				// �ؽ��� ����Ʈ�� ������Ʈ ��Ű�� ������ �̹����� �����Ѵ�.
				switch( GET_TEXTURE_TYPE( pTLInfo->nIndex ) )
				{
				case TT_FLOORTILE	:
				case TT_ONE			:
				case TT_ALPHATILE	:
					{
						// �Ѱ��� ���.
						AddTile( pTLInfo->nIndex , pTLInfo->strComment );
						pTLInfo->nProperty	= dlg.m_nProperty;
					}
					break;
				case TT_UPPERTILE	:
					{
						// �������� ���.

						// ������ ��� - -;

						// ������ �߰��ϰԵɰŽ�.
						CString	str;
						for( int y = 0 ; y < 4 ; y ++ )
						{
							for( int x = 0 ; x < 4 ; x ++ )
							{
								str.Format( "%s %d" , pTLInfo->strComment , y * 4 + x );
								AddTile( pTLInfo->nIndex + y * 4 + x , (LPSTR) (LPCTSTR) str );
							}
						}					
					}
					break;
				}
				
				count ++;

				SaveSetChangeTileList();
			}
			else
			{
				// ī���� ���� ����..
				// ��������.
				DeleteFile( copyfilename );
				break;
			}
		}
	}

	// �ٵŵ�
	DisplayMessage( AEM_NOTIFY , "%d���� �̹����� Ÿ�Ϸ� �߰��Ͽ����ϴ�." , count );
	Invalidate( FALSE );

	CWnd::OnDropFiles(hDropInfo);
}

void CUITileList_TileTabWnd::OnTilewindowEditTile() 
{
	if( m_pSelectedTile )
	{
		// Ÿ�� ������ �ٲ������ ���������쿡�� �˷��ش�.
		g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll()						;
		g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( m_pSelectedTile->index )	;
		g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= m_pSelectedTile->applyalpha		;
		Invalidate( FALSE );
	}

	AcTextureList::TextureInfo * pInfo = AGCMMAP_THIS->m_TextureList.GetTextureInfo( m_pSelectedTile->index );
	if( m_pSelectedTile && pInfo )
	{
		char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];

		_splitpath( pInfo->strFilename , drive, dir, fname, ext );

		CConvertTextureDlg	dlg;
		dlg.m_strFileNameOrigin		= pInfo->strFilename;
		dlg.m_strFileOriginalPath	= pInfo->strFilename;
		dlg.m_strName				= pInfo->strComment	;

		dlg.m_nType					= GET_TEXTURE_TYPE		( pInfo->nIndex );
		dlg.m_nDimension			= GET_TEXTURE_DIMENSION	( pInfo->nIndex );
		dlg.m_bDisableType			= TRUE;
		dlg.m_nProperty				= pInfo->nProperty	;

		if( dlg.DoModal() == IDOK )
		{
			pInfo->nIndex = SET_TEXTURE_DIMENSION( pInfo->nIndex , dlg.m_nDimension );
			strncpy( pInfo->strComment , ( LPCTSTR ) dlg.m_strName , 256 );

			pInfo->nProperty		= dlg.m_nProperty;

			m_pSelectedTile->name	= dlg.m_strName;
			m_pSelectedTile->index	= pInfo->nIndex;

			g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll()						;
			g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( m_pSelectedTile->index )	;
			g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= m_pSelectedTile->applyalpha		;

			if( dlg.m_bTextureUpdate )
			{
				char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
				_splitpath( ( LPCTSTR) dlg.m_strFileNameOrigin , drive , dir , fname , ext );
				wsprintf( pInfo->strFilename , "Map\\Tile\\%s%s" , fname , ext );
				// ������ �ٽ� ����..
				AGCMMAP_THIS->m_TextureList.MakeTempBitmap	( pInfo );
				AGCMMAP_THIS->m_TextureList.LoadTexture		( pInfo , TRUE );

				LoadBitmap( m_pSelectedTile , TRUE );
			}
			Invalidate( FALSE );

			SaveSetChangeTileList();
		}
	}
	
}

void CUITileList_TileTabWnd::OnTilewindowDeleteTile() 
{
	if( m_pSelectedTile )
	{
		// Ÿ�� ������ �ٲ������ ���������쿡�� �˷��ش�.
		g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll()						;
		g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( m_pSelectedTile->index )	;
		g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= m_pSelectedTile->applyalpha		;
		Invalidate( FALSE );
	}

	// Ÿ�� ������.
	if( m_pSelectedTile && IDYES == MessageBox( "Ÿ�� ������ ����ǲ�����? -_-" , "����" , MB_YESNO ) )
	{
		AcTextureList::TextureInfo * pInfo;

		pInfo = AGCMMAP_THIS->m_TextureList.GetTextureInfo( m_pSelectedTile->index );

		// ���� ����.
		// ������ (2004-05-31 ���� 1:05:16) : 
		// ������ ��û���� ���� �������� ����.
//		char filename[ 256 ];
//		wsprintf( filename , "%s\\%s" , ALEF_CURRENT_DIRECTORY , pInfo->strFilename );
//		DeleteFile( filename );

		// �ؽ��� ����Ʈ���� ����
		AGCMMAP_THIS->m_TextureList.DeleteTexture( m_pSelectedTile->index , TRUE );

		// ���� ����.

		AuNode< stTileInfo > * pNode = m_pSelectedCategory->list.GetHeadNode();
		stTileInfo	* pTi;
		while( pNode )
		{
			pTi = &pNode->GetData();

			if( pTi == m_pSelectedTile )
			{
				m_pSelectedCategory->list.RemoveNode( pNode );
				break;
			}

			m_pSelectedCategory->list.GetNext( pNode );
		}

		m_pSelectedTile = NULL;

		Invalidate( FALSE );

		DisplayMessage( AEM_NOTIFY , "Ÿ���� �����Ͽ����ϴ�." );

		SaveSetChangeTileList();
	}
}

void CUITileList_TileTabWnd::OnTilewindowChangeTile() 
{
	if( g_pMainFrame->m_Document.m_listSelectedTileIndex.GetCount() == 2 &&
		IDYES == MessageBox( "Ÿ�� ��ü�� Ÿ�� 2���� �����ϼž��մϴ� ���� �ҷ���?" , "����" , MB_YESNO ) )
	{
		UINT32	uTiled , uReplace;

		uTiled		= g_pMainFrame->m_Document.m_listSelectedTileIndex[ 0 ];
		uReplace	= g_pMainFrame->m_Document.m_listSelectedTileIndex[ 1 ];
		
		AcTextureList::TextureInfo * pInfoTiled		;
		AcTextureList::TextureInfo * pInfoReplace	;

		pInfoTiled		= AGCMMAP_THIS->m_TextureList.GetTextureInfo( uTiled	);
		pInfoReplace	= AGCMMAP_THIS->m_TextureList.GetTextureInfo( uReplace	);

		CString	str;
		str.Format( "'%s(%s)' Ÿ���� '%s(%s)'Ÿ�Ϸ� ���������ϴ�. �������ðھ��?" ,
			pInfoTiled->strComment		, pInfoTiled->strFilename	,
			pInfoReplace->strComment	, pInfoReplace->strFilename	);

		if( IDYES == MessageBox( str , "����" , MB_YESNO ) )
		{
			AGCMMAP_THIS->SetChangeTile( uTiled , uReplace );
			AGCMMAP_THIS->UpdateAll();
			AGCMMAP_THIS->SetChangeTile( 0 , 0 );
		}

		Invalidate( FALSE );
	}
}

void CUITileList_TileTabWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown( nFlags , point );
	
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CUITileList_TileTabWnd::SelectTile(int tileindex)
{
	int	nonmaskindex;
	nonmaskindex = MAKE_TEXTURE_INDEX( GET_TEXTURE_DIMENSION( tileindex ) , GET_TEXTURE_CATEGORY( tileindex ) , GET_TEXTURE_INDEX( tileindex ) , GET_TEXTURE_TYPE( tileindex ) , 0 );

	AuNode< stTileCategory * >	*	pNode		= m_listCategory.GetHeadNode();
	AuNode< stTileInfo >		*	pNodeTile	;
	stTileCategory				*	pRef		;
	stTileInfo					*	pTileInfo	= NULL;

	while( pNode )
	{
		pRef = pNode->GetData();

		if( pRef->category == GET_TEXTURE_CATEGORY( nonmaskindex ) )
		{
			m_pSelectedCategory = pRef;

			pNodeTile = m_pSelectedCategory->list.GetHeadNode();
			while( pNodeTile )
			{
				pTileInfo = & pNodeTile->GetData();

				if( pTileInfo->index == nonmaskindex )
				{
					m_pSelectedTile	= pTileInfo;
					break;
				}
				m_pSelectedCategory->list.GetNext( pNodeTile );
			}
			break;
		}
		m_listCategory.GetNext( pNode );
	}

	if( pTileInfo )
	{
		g_pMainFrame->m_Document.m_listSelectedTileIndex.RemoveAll();
		g_pMainFrame->m_Document.m_listSelectedTileIndex.AddTail( nonmaskindex );
		g_pMainFrame->m_Document.m_nCurrentAlphaTexture	= pTileInfo->applyalpha	;
	}
}

void CUITileList_TileTabWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	InvalidateRect( m_rectCursor , FALSE );
	
	CWnd::OnTimer(nIDEvent);
}

void CUITileList_TileTabWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if( cx * cy > 0 )
	{
		CDC	*pDC = GetDC();
		m_MemBitmap.DeleteObject();
		m_MemBitmap.CreateCompatibleBitmap( pDC , cx , cy );
		m_MemDC.SelectObject( m_MemBitmap );

		ReleaseDC( pDC );
	}
}

void CUITileList_TileTabWnd::ChangeLayer( int layer )
{
	if( m_nSelectedLayer == layer ) return;

	g_MainWindow.OnTileLayerChange( layer , m_nSelectedLayer );
	
	m_nSelectedLayer = layer;
	g_pMainFrame->m_Document.m_nCurrentTileLayer	= m_nSelectedLayer	;
	Invalidate();
}

void CUITileList_TileTabWnd::UpdateDimensino	()
{
	// ������ (2004-05-29 ���� 10:16:34) : ����� ǥ�úκи� ������Ʈ

	CRect	rect;
	GetClientRect( rect );

	rect.top = rect.bottom - 20;

	InvalidateRect( rect , FALSE );
}

BOOL CUITileList_TileTabWnd::IsUsedTileName	( stTileInfo * pTInfo )
{
	ASSERT( NULL != pTInfo );
	if( NULL == pTInfo ) return FALSE;

	RwTexDictionary *pTexDic = g_pcsAgcmResourceLoader->GetDefaultTexDict();
	ASSERT( NULL != pTexDic );

	if( pTexDic )
	{
		char	strExportName[ 1024 ];
		VERIFY( AGCMMAP_THIS->m_TextureList.GetExportFileName( strExportName , pTInfo->index ) );

		RwTexture * pTexture = RwTexDictionaryFindNamedTexture( pTexDic , strExportName );
	
		if( pTexture )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}